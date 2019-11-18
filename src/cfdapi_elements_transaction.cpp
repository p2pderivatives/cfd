// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_elements_transaction.cpp
 *
 * @brief-eng implementation of Confidential Transaction that uses cfd-api
 * @brief-jp cfd-apiで利用するConfidential Transaction操作の実装ファイル
 */
#ifndef CFD_DISABLE_ELEMENTS
#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_fee.h"
#include "cfd_manager.h"  // NOLINT
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_util.h"

#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_elements_address.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfd/cfdapi_transaction.h"
#include "cfdapi_transaction_base.h"  // NOLINT

namespace cfd {
namespace api {

using cfd::ConfidentialTransactionController;
using cfd::FeeCalculator;
using cfd::SignParameter;
using cfd::api::TransactionApiBase;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlindParameter;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxIn;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOut;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::ExtPubkey;
using cfd::core::HashType;
using cfd::core::HashUtil;
using cfd::core::IssuanceBlindingKeyPair;
using cfd::core::IssuanceParameter;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::UnblindParameter;
using cfd::core::WitnessVersion;
using cfd::core::logger::info;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// ファイル内関数
// -----------------------------------------------------------------------------

/**
 * @brief Create a ConfidentialTransactionController object.
 *
 * @param[in] hex of the transaction for which to create the controller object
 * @return a ConfidentialTransactionController instance
 */
static ConfidentialTransactionController CreateController(
    const std::string& hex) {
  return ConfidentialTransactionController(hex);
}

ConfidentialTransactionController ElementsTransactionApi::CreateRawTransaction(
    uint32_t version, uint32_t locktime,
    const std::vector<ConfidentialTxIn>& txins,
    const std::vector<ConfidentialTxOut>& txouts,
    const ConfidentialTxOut& txout_fee) const {
  // Transaction作成
  ConfidentialTransactionController ctxc(version, locktime);

  // TxInの追加
  const uint32_t kLockTimeDisabledSequence =
      ctxc.GetLockTimeDisabledSequence();
  for (const auto& txin : txins) {
    // TxInのunlocking_scriptは空で作成
    if (kLockTimeDisabledSequence == txin.GetSequence()) {
      ctxc.AddTxIn(txin.GetTxid(), txin.GetVout(), ctxc.GetDefaultSequence());
    } else {
      ctxc.AddTxIn(txin.GetTxid(), txin.GetVout(), txin.GetSequence());
    }
  }

  // TxOutの追加
  for (const auto& txout : txouts) {
    ctxc.AddTxOut(
        txout.GetLockingScript(), txout.GetConfidentialValue().GetAmount(),
        txout.GetAsset(), txout.GetNonce());
  }

  // amountが0のfeeは無効と判定
  if (txout_fee.GetConfidentialValue().GetAmount() != 0) {
    ctxc.AddTxOutFee(
        txout_fee.GetConfidentialValue().GetAmount(), txout_fee.GetAsset());
  }

  return ctxc;
}

uint32_t ElementsTransactionApi::GetWitnessStackNum(
    const std::string& tx_hex, const Txid& txid, uint32_t vout) const {
  return TransactionApiBase::GetWitnessStackNum<
      ConfidentialTransactionController>(
      cfd::api::CreateController, tx_hex, txid, vout);
}

ConfidentialTransactionController ElementsTransactionApi::AddSign(
    const std::string& hex, const Txid& txid, uint32_t vout,
    const std::vector<SignParameter>& sign_params, bool is_witness,
    bool clear_stack) const {
  return TransactionApiBase::AddSign<ConfidentialTransactionController>(
      cfd::api::CreateController, hex, txid, vout, sign_params, is_witness,
      clear_stack);
}

ConfidentialTransactionController ElementsTransactionApi::UpdateWitnessStack(
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const SignParameter& update_sign_param, uint32_t stack_index) const {
  return TransactionApiBase::UpdateWitnessStack<
      ConfidentialTransactionController>(
      cfd::api::CreateController, tx_hex, txid, vout, update_sign_param,
      stack_index);
}

ByteData ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const ConfidentialTxInReference& txin,
    const Pubkey& pubkey, const ConfidentialValue& value, HashType hash_type,
    const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin, pubkey.GetData(), value, hash_type, sighash_type);
}

ByteData ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const ConfidentialTxInReference& txin,
    const Script& redeem_script, const ConfidentialValue& value,
    HashType hash_type, const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin, redeem_script.GetData(), value, hash_type, sighash_type);
}

ByteData ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const ConfidentialTxInReference& txin,
    const ByteData& key_data, const ConfidentialValue& value,
    HashType hash_type, const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin.GetTxid(), txin.GetVout(), key_data, value, hash_type,
      sighash_type);
}

ByteData ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const ByteData& key_data, const ConfidentialValue& value,
    HashType hash_type, const SigHashType& sighash_type) const {
  std::string sig_hash;
  ConfidentialTransactionController txc(tx_hex);
  bool is_witness = false;

  switch (hash_type) {
    case HashType::kP2pkh:
      // fall-through
    case HashType::kP2wpkh:
      if (hash_type == HashType::kP2wpkh) {
        is_witness = true;
      }
      if (value.HasBlinding()) {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, Pubkey(key_data), sighash_type, value.GetData(),
            is_witness);
      } else {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, Pubkey(key_data), sighash_type, value.GetAmount(),
            is_witness);
      }
      break;
    case HashType::kP2sh:
      // fall-through
    case HashType::kP2wsh:
      if (hash_type == HashType::kP2wsh) {
        is_witness = true;
      }
      if (value.HasBlinding()) {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, Script(key_data), sighash_type, value.GetData(),
            is_witness);
      } else {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, Script(key_data), sighash_type, value.GetAmount(),
            is_witness);
      }
      break;
    default:
      warn(
          CFD_LOG_SOURCE,
          "Failed to CreateSignatureHash. Invalid hash_type: {}", hash_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Invalid hash_type.");
      break;
  }

  return ByteData(sig_hash);
}

ConfidentialTransactionController ElementsTransactionApi::AddMultisigSign(
    const std::string& tx_hex, const ConfidentialTxInReference& txin,
    const std::vector<SignParameter>& sign_list, AddressType address_type,
    const Script& witness_script, const Script redeem_script,
    bool clear_stack) {
  return AddMultisigSign(
      tx_hex, txin.GetTxid(), txin.GetVout(), sign_list, address_type,
      witness_script, redeem_script, clear_stack);
}

ConfidentialTransactionController ElementsTransactionApi::AddMultisigSign(
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const std::vector<SignParameter>& sign_list, AddressType address_type,
    const Script& witness_script, const Script redeem_script,
    bool clear_stack) {
  std::string result =
      TransactionApiBase::AddMultisigSign<ConfidentialTransactionController>(
          CreateController, tx_hex, txid, vout, sign_list, address_type,
          witness_script, redeem_script, clear_stack);
  return ConfidentialTransactionController(result);
}

ConfidentialTransactionController ElementsTransactionApi::BlindTransaction(
    const std::string& tx_hex,
    const std::vector<TxInBlindParameters>& txin_blind_keys,
    const std::vector<TxOutBlindKeys>& txout_blind_keys,
    bool is_issuance_blinding) {
  ConfidentialTransactionController txc(tx_hex);

  uint32_t txin_count = txc.GetTransaction().GetTxInCount();
  uint32_t txout_count = txc.GetTransaction().GetTxOutCount();

  if (txin_blind_keys.size() == 0) {
    warn(CFD_LOG_SOURCE, "Failed to txins empty.");
    throw CfdException(
        CfdError::kCfdOutOfRangeError, "JSON value error. Empty txins.");
  }
  if (txout_blind_keys.size() == 0) {
    warn(CFD_LOG_SOURCE, "Failed to txouts empty.");
    throw CfdException(
        CfdError::kCfdOutOfRangeError, "JSON value error. Empty txouts.");
  }

  std::vector<BlindParameter> txin_info_list(txin_count);
  std::vector<Pubkey> txout_confidential_keys(txout_count);
  std::vector<IssuanceBlindingKeyPair> issuance_blinding_keys;
  if (is_issuance_blinding) {
    issuance_blinding_keys.resize(txin_count);
  }

  // TxInのBlind情報設定
  for (TxInBlindParameters txin_key : txin_blind_keys) {
    uint32_t index =
        txc.GetTransaction().GetTxInIndex(txin_key.txid, txin_key.vout);
    txin_info_list[index].asset = txin_key.blind_param.asset;
    txin_info_list[index].vbf = txin_key.blind_param.vbf;
    txin_info_list[index].abf = txin_key.blind_param.abf;
    txin_info_list[index].value = txin_key.blind_param.value;
    if (txin_key.is_issuance) {
      issuance_blinding_keys[index].asset_key =
          txin_key.issuance_key.asset_key;
      issuance_blinding_keys[index].token_key =
          txin_key.issuance_key.token_key;
    }
  }

  // TxOutのBlind情報設定
  for (TxOutBlindKeys txout_key : txout_blind_keys) {
    if (txout_key.index < txout_count) {
      txout_confidential_keys[txout_key.index] = txout_key.blinding_key;
    } else {
      warn(
          CFD_LOG_SOURCE,
          "Failed to BlindTransaction. Invalid txout index: {}",
          txout_key.index);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Invalid txout index.");
    }
  }

  txc.BlindTransaction(
      txin_info_list, issuance_blinding_keys, txout_confidential_keys);
  return txc;
}

ConfidentialTransactionController ElementsTransactionApi::UnblindTransaction(
    const std::string& tx_hex,
    const std::vector<TxOutUnblindKeys>& txout_unblind_keys,
    const std::vector<IssuanceBlindKeys>& issuance_blind_keys,
    std::vector<UnblindOutputs>* blind_outputs,
    std::vector<UnblindIssuanceOutputs>* issuance_outputs) {
  ConfidentialTransactionController ctxc(tx_hex);

  if (!txout_unblind_keys.empty() && blind_outputs != nullptr) {
    UnblindParameter unblind_param;
    for (const auto& txout : txout_unblind_keys) {
      // TxOutをUnblind
      const Privkey blinding_key(txout.blinding_key);
      unblind_param = ctxc.UnblindTxOut(txout.index, blinding_key);

      if (!unblind_param.asset.GetHex().empty()) {
        UnblindOutputs output;
        output.index = txout.index;
        output.blind_param.asset = unblind_param.asset;
        output.blind_param.vbf = unblind_param.vbf;
        output.blind_param.abf = unblind_param.abf;
        output.blind_param.value = unblind_param.value;
        blind_outputs->push_back(output);
      }
    }
  }

  if (!issuance_blind_keys.empty() && issuance_outputs != nullptr) {
    for (const auto& issuance : issuance_blind_keys) {
      uint32_t txin_index = ctxc.GetTransaction().GetTxInIndex(
          Txid(issuance.txid), issuance.vout);

      std::vector<UnblindParameter> issuance_param = ctxc.UnblindIssuance(
          txin_index, issuance.issuance_key.asset_key,
          issuance.issuance_key.token_key);

      UnblindIssuanceOutputs output;
      output.txid = issuance.txid;
      output.vout = issuance.vout;
      output.asset = issuance_param[0].asset;
      output.asset_amount = issuance_param[0].value;
      if (issuance_param.size() > 1) {
        output.token = issuance_param[1].asset;
        output.token_amount = issuance_param[1].value;
      }
      issuance_outputs->push_back(output);
    }
  }
  return ctxc;
}

ConfidentialTransactionController ElementsTransactionApi::SetRawIssueAsset(
    const std::string& tx_hex,
    const std::vector<TxInIssuanceParameters>& issuances,
    std::vector<IssuanceOutput>* issuance_output) {
  ConfidentialTransactionController ctxc(tx_hex);

  for (const auto& issuance : issuances) {
    Script asset_locking_script = issuance.asset_txout.GetLockingScript();
    ByteData asset_nonce = issuance.asset_txout.GetNonce().GetData();
    Script token_locking_script = issuance.token_txout.GetLockingScript();
    ByteData token_nonce = issuance.token_txout.GetNonce().GetData();

    IssuanceParameter issuance_param = ctxc.SetAssetIssuance(
        issuance.txid, issuance.vout, issuance.asset_amount,
        asset_locking_script, asset_nonce, issuance.token_amount,
        token_locking_script, token_nonce, issuance.is_blind,
        issuance.contract_hash, false);

    if (issuance_output != nullptr) {
      IssuanceOutput output;
      output.txid = issuance.txid;
      output.vout = issuance.vout;
      output.output.asset = issuance_param.asset;
      output.output.entropy = issuance_param.entropy;
      output.output.token = issuance_param.token;
      issuance_output->push_back(output);
    }
  }
  return ctxc;
}

ConfidentialTransactionController ElementsTransactionApi::SetRawReissueAsset(
    const std::string& tx_hex,
    const std::vector<TxInReissuanceParameters>& issuances,
    std::vector<IssuanceOutput>* issuance_output) {
  ConfidentialTransactionController ctxc(tx_hex);

  for (const auto& issuance : issuances) {
    Script locking_script = issuance.asset_txout.GetLockingScript();
    ByteData nonce = issuance.asset_txout.GetNonce().GetData();

    IssuanceParameter issuance_param = ctxc.SetAssetReissuance(
        issuance.txid, issuance.vout, issuance.amount, locking_script, nonce,
        issuance.blind_factor, issuance.entropy, false);

    if (issuance_output != nullptr) {
      IssuanceOutput output;
      output.txid = issuance.txid;
      output.vout = issuance.vout;
      output.output.asset = issuance_param.asset;
      output.output.entropy = issuance_param.entropy;
      issuance_output->push_back(output);
    }
  }
  return ctxc;
}

ConfidentialTransactionController
ElementsTransactionApi::CreateRawPeginTransaction(
    uint32_t version, uint32_t locktime,
    const std::vector<ConfidentialTxIn>& txins,
    const std::vector<TxInPeginParameters>& pegins,
    const std::vector<ConfidentialTxOut>& txouts,
    const ConfidentialTxOut& txout_fee) const {
  ConfidentialTransactionController ctxc =
      CreateRawTransaction(version, locktime, txins, txouts, txout_fee);

  for (const auto& pegin_data : pegins) {
    ctxc.AddPeginWitness(
        pegin_data.txid, pegin_data.vout, pegin_data.amount, pegin_data.asset,
        pegin_data.mainchain_blockhash, pegin_data.claim_script,
        pegin_data.mainchain_raw_tx, pegin_data.mainchain_txoutproof);
  }

  return ctxc;
}

ConfidentialTransactionController
ElementsTransactionApi::CreateRawPegoutTransaction(
    uint32_t version, uint32_t locktime,
    const std::vector<ConfidentialTxIn>& txins,
    const std::vector<ConfidentialTxOut>& txouts,
    const TxOutPegoutParameters& pegout_data,
    const ConfidentialTxOut& txout_fee, Address* pegout_address) const {
  ConfidentialTxOut empty_fee;
  ConfidentialTransactionController ctxc =
      CreateRawTransaction(version, locktime, txins, txouts, empty_fee);

  // PegoutのTxOut追加
  const std::string pegout_addr_string = pegout_data.btc_address.GetAddress();

  if (pegout_data.online_pubkey.IsValid() &&
      !pegout_data.master_online_key.IsInvalid()) {
    Address pegout_addr;
    if (pegout_addr_string.empty()) {
      // TODO(k-matsuzawa): ExtKeyの正式対応が入るまでの暫定対応
      // pegoutのtemplateに従い、xpub/counterから生成する
      // descriptor parse
      std::string desc = pegout_data.bitcoin_descriptor;
      std::string::size_type start_point = desc.rfind('(');
      std::string arg_type;
      std::string xpub;
      if (start_point == std::string::npos) {
        xpub = desc;
      } else {
        arg_type = desc.substr(0, start_point);
        xpub = desc.substr(start_point + 1);
      }
      std::string::size_type end_point = xpub.find('/');
      if (end_point == std::string::npos) {
        end_point = xpub.find(')');
        if (end_point != std::string::npos) {
          xpub = xpub.substr(0, end_point);
        }
      } else {
        xpub = xpub.substr(0, end_point);
      }
      // info(CFD_LOG_SOURCE, "arg_type={}, xpub={}", arg_type, xpub);
      // key生成
      std::vector<uint32_t> path = {0, pegout_data.bip32_counter};
      ExtPubkey ext_key = ExtPubkey(xpub).DerivePubkey(path);
      Pubkey pubkey = ext_key.GetPubkey();

      // Addressクラス生成
      if (arg_type == "sh(wpkh") {
        Script wpkh_script = ScriptUtil::CreateP2wpkhLockingScript(pubkey);
        ByteData160 wpkh_hash = HashUtil::Hash160(wpkh_script);
        pegout_addr = Address(
            pegout_data.net_type, AddressType::kP2shAddress, wpkh_hash);
      } else if (arg_type == "wpkh") {
        pegout_addr =
            Address(pegout_data.net_type, WitnessVersion::kVersion0, pubkey);
      } else {  // if (arg_type == "pkh(")
        // pkh
        pegout_addr = Address(pegout_data.net_type, pubkey);
      }
    } else {
      pegout_addr = pegout_data.btc_address;
    }

    ctxc.AddPegoutTxOut(
        pegout_data.amount, pegout_data.asset, pegout_data.genesisblock_hash,
        pegout_addr, pegout_data.net_type, pegout_data.online_pubkey,
        pegout_data.master_online_key, pegout_data.bitcoin_descriptor,
        pegout_data.bip32_counter, pegout_data.whitelist);
    if (pegout_address != nullptr) {
      *pegout_address = pegout_addr;
    }

  } else {
    ctxc.AddPegoutTxOut(
        pegout_data.amount, pegout_data.asset, pegout_data.genesisblock_hash,
        pegout_data.btc_address);
  }

  // amountが0のfeeは無効と判定
  if (txout_fee.GetConfidentialValue().GetAmount() != 0) {
    ctxc.AddTxOutFee(
        txout_fee.GetConfidentialValue().GetAmount(), txout_fee.GetAsset());
  }

  return ctxc;
}

Privkey ElementsTransactionApi::GetIssuanceBlindingKey(
    const Privkey& master_blinding_key, const Txid& txid, int32_t vout) {
  Privkey blinding_key = ConfidentialTransaction::GetIssuanceBlindingKey(
      master_blinding_key, txid, vout);

  return blinding_key;
}

Amount ElementsTransactionApi::EstimateFee(
    const std::string& tx_hex, const std::vector<ElementsUtxoAndOption>& utxos,
    const ConfidentialAssetId& fee_asset, Amount* tx_fee, Amount* utxo_fee,
    bool is_blind, double effective_fee_rate) const {
  uint64_t fee_rate = static_cast<uint64_t>(floor(effective_fee_rate * 1000));
  return EstimateFee(
      tx_hex, utxos, fee_asset, tx_fee, utxo_fee, is_blind, fee_rate);
}

Amount ElementsTransactionApi::EstimateFee(
    const std::string& tx_hex, const std::vector<ElementsUtxoAndOption>& utxos,
    const ConfidentialAssetId& fee_asset, Amount* tx_fee, Amount* utxo_fee,
    bool is_blind, uint64_t effective_fee_rate) const {
  ConfidentialTransactionController txc(tx_hex);

  if (fee_asset.IsEmpty()) {
    warn(CFD_LOG_SOURCE, "Failed to EstimateFee. Empty fee asset.");
    throw CfdException(CfdError::kCfdIllegalArgumentError, "Empty fee asset.");
  }

  // check fee in txout
  bool exist_fee = false;
  const ConfidentialTransaction& ctx = txc.GetTransaction();
  for (const auto& txout : ctx.GetTxOutList()) {
    if (txout.GetLockingScript().IsEmpty()) {
      if (txout.GetAsset().GetHex() != fee_asset.GetHex()) {
        warn(CFD_LOG_SOURCE, "Failed to EstimateFee. Unmatch fee asset.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Unmatch fee asset.");
      }
      exist_fee = true;
      break;
    }
  }
  if (!exist_fee) {
    txc.AddTxOutFee(Amount::CreateBySatoshiAmount(1), fee_asset);  // dummy fee
  }

  uint32_t size;
  uint32_t witness_size = 0;
  size = txc.GetSizeIgnoreTxIn(is_blind, &witness_size);
  size -= witness_size;
  uint32_t tx_vsize =
      AbstractTransaction::GetVsizeFromSize(size, witness_size);

  size = 0;
  witness_size = 0;
  uint32_t wit_size = 0;
  for (const auto& utxo : utxos) {
    uint32_t pegin_btc_tx_size = 0;
    Script fedpeg_script;
    if (utxo.is_pegin) {
      pegin_btc_tx_size = utxo.pegin_btc_tx_size;
      fedpeg_script = utxo.fedpeg_script;
    }
    // check descriptor
    AddressType addr_type = utxo.utxo.address.GetAddressType();
    // TODO(k-matsuzawa): output descriptorの正式対応後に差し替え
    if (utxo.utxo.address.GetAddress().empty()) {
      if (utxo.utxo.descriptor.find("wpkh(") == 0) {
        addr_type = AddressType::kP2wpkhAddress;
      } else if (utxo.utxo.descriptor.find("wsh(") == 0) {
        addr_type = AddressType::kP2wshAddress;
      } else if (utxo.utxo.descriptor.find("pkh(") == 0) {
        addr_type = AddressType::kP2pkhAddress;
      } else if (utxo.utxo.descriptor.find("sh(") == 0) {
        addr_type = AddressType::kP2shAddress;
      }
    }
    if (utxo.utxo.descriptor.find("sh(wpkh(") == 0) {
      addr_type = AddressType::kP2shP2wpkhAddress;
    } else if (utxo.utxo.descriptor.find("sh(wsh(") == 0) {
      addr_type = AddressType::kP2shP2wshAddress;
    }

    uint32_t txin_size = ConfidentialTxIn::EstimateTxInSize(
        addr_type, utxo.utxo.redeem_script, pegin_btc_tx_size, fedpeg_script,
        utxo.is_issuance, utxo.is_blind_issuance, &wit_size);
    txin_size -= wit_size;
    size += txin_size;
    witness_size += wit_size;
  }
  uint32_t utxo_vsize =
      AbstractTransaction::GetVsizeFromSize(size, witness_size);

  FeeCalculator fee_calc(effective_fee_rate);
  Amount tx_fee_amount = fee_calc.GetFee(tx_vsize);
  Amount utxo_fee_amount = fee_calc.GetFee(utxo_vsize);
  Amount fee = tx_fee_amount + utxo_fee_amount;

  if (tx_fee) *tx_fee = tx_fee_amount;
  if (utxo_fee) *utxo_fee = utxo_fee_amount;

  info(
      CFD_LOG_SOURCE, "EstimateFee rate={} fee={} tx={} utxo={}",
      effective_fee_rate, fee.GetSatoshiValue(),
      tx_fee_amount.GetSatoshiValue(), utxo_fee_amount.GetSatoshiValue());
  return fee;
}

ConfidentialTransactionController ElementsTransactionApi::FundRawTransaction(
    const std::string& tx_hex, const std::vector<UtxoData>& utxos,
    const std::map<std::string, Amount>& map_target_value,
    const std::vector<ElementsUtxoAndOption>& selected_txin_utxos,
    const std::map<std::string, std::string>& reserve_txout_address,
    const ConfidentialAssetId& fee_asset, bool is_blind_estimate_fee,
    double effective_fee_rate, Amount* estimate_fee, const UtxoFilter* filter,
    const CoinSelectionOption* option_params,
    std::vector<std::string>* append_txout_addresses, NetType net_type,
    const std::vector<AddressFormatData>* prefix_list) const {
  // set option
  CoinSelectionOption option;
  UtxoFilter utxo_filter;
  if (filter) utxo_filter = *filter;
  if (option_params) {
    option = *option_params;
  } else {
    option.InitializeConfidentialTxSizeInfo();
    option.SetEffectiveFeeBaserate(effective_fee_rate);
    option.SetLongTermFeeBaserate(effective_fee_rate);
  }
  option.SetFeeAsset(fee_asset);

  ElementsAddressFactory addr_factory(net_type);
  if (prefix_list) {
    addr_factory = ElementsAddressFactory(net_type, *prefix_list);
  }

  // txから設定済みTxIn/TxOutの額を収集
  // (selected_txin_utxos指定分はtxid一致なら設定済みUTXO扱い)
  ConfidentialTransactionController ctxc(tx_hex);
  const ConfidentialTransaction& ctx = ctxc.GetTransaction();
  std::map<std::string, Amount> txin_amount_map;
  std::map<std::string, Amount> tx_amount_map;
  int32_t fee_index = -1;
  std::vector<ConfidentialTxOutReference> txout_list = ctx.GetTxOutList();
  for (size_t index = 0; index < txout_list.size(); ++index) {
    auto& txout = txout_list[index];
    if (txout.GetLockingScript().IsEmpty()) {
      // feeは収集対象から除外
      fee_index = static_cast<int32_t>(index);
    } else {
      std::string asset = txout.GetAsset().GetHex();
      if (tx_amount_map.find(asset) == tx_amount_map.end()) {
        Amount amount;
        tx_amount_map.emplace(asset, amount);
      }
      tx_amount_map[asset] += txout.GetConfidentialValue().GetAmount();
    }
  }
  const auto& txin_list = ctx.GetTxInList();
  for (const auto& utxo : selected_txin_utxos) {
    for (const auto& txin : txin_list) {
      if ((txin.GetTxid().Equals(utxo.utxo.txid)) &&
          (utxo.utxo.vout == txin.GetVout())) {
        std::string asset = utxo.utxo.asset.GetHex();
        if (tx_amount_map.find(asset) == tx_amount_map.end()) {
          Amount amount;
          txin_amount_map.emplace(asset, amount);
        }
        txin_amount_map[asset] += utxo.utxo.amount;
        break;
      }
    }
  }

  Amount fee;
  if (option.GetEffectiveFeeBaserate() != 0) {
    if (fee_asset.IsEmpty()) {
      warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. Empty fee asset.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Empty fee asset.");
    }
    // feeの存在確認と、fee領域の確保
    if (fee_index == -1) {
      // txoutにfee追加
      ctxc.AddTxOutFee(Amount::CreateBySatoshiAmount(0), fee_asset);
      fee_index = static_cast<int32_t>(txout_list.size());
    }
    fee = EstimateFee(
        ctxc.GetHex(), selected_txin_utxos, fee_asset, nullptr, nullptr,
        is_blind_estimate_fee, option.GetEffectiveFeeBaserate());
    if (estimate_fee) *estimate_fee = fee;
  }

  // 探索対象assetを設定。未設定時はTxOutの合計額を設定。
  std::map<std::string, Amount> target_values = map_target_value;
  if (target_values.empty()) {
    target_values = tx_amount_map;
  }

  // execute coinselection
  CoinApi coin_api;
  CoinSelection coin_select;
  std::vector<Utxo> utxo_list = coin_api.ConvertToUtxo(utxos);
  std::map<std::string, Amount> amount_map;
  std::vector<Utxo> selected_coins;
  selected_coins = coin_select.SelectCoins(
      target_values, utxo_list, utxo_filter, option, fee, &amount_map, nullptr,
      nullptr);

  // 収集したcoinとtxoutの額が一致するかどうか確認
  std::map<std::string, Amount> diff_amount_map = amount_map;
  uint32_t append_txout_count = 0;
  bool use_fee = false;
  std::string fee_asset_str;
  uint8_t lbtc_asset[33];
  if ((option.GetEffectiveFeeBaserate() > 0) && (!fee_asset.IsEmpty())) {
    use_fee = true;
    fee_asset_str = fee_asset.GetHex();
    memcpy(
        lbtc_asset, fee_asset.GetData().GetBytes().data(), sizeof(lbtc_asset));
  }
  for (auto itr = diff_amount_map.begin(); itr != diff_amount_map.end();
       ++itr) {
    Amount dest_amount = tx_amount_map[itr->first];
    if ((target_values.find(itr->first) != target_values.end()) &&
        (target_values[itr->first] > dest_amount)) {
      // txout設定額よりも大きな額を収集要求した
      dest_amount = target_values[itr->first];
    }
    if (use_fee && (itr->first == fee_asset_str)) {
      Amount need_amount = dest_amount + fee;
      if (itr->second >= need_amount) {
        itr->second -= dest_amount;
      } else {
        warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. low fee asset.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "low fee asset.");
      }
    } else {
      if (itr->second == dest_amount) {
        // match
      } else if (itr->second >= dest_amount) {
        itr->second -= dest_amount;
        ++append_txout_count;
      } else {
        warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. low asset.");
        throw CfdException(CfdError::kCfdIllegalArgumentError, "low asset.");
      }
    }
  }

  // 追加が必要なTxOutを追加
  if (append_txout_count != 0) {
    for (auto itr = diff_amount_map.begin(); itr != diff_amount_map.end();
         ++itr) {
      if (use_fee && (itr->first == fee_asset_str)) {
        // fall-through
      } else if (itr->second > 0) {
        if (reserve_txout_address.find(itr->first) ==
            reserve_txout_address.end()) {
          warn(
              CFD_LOG_SOURCE,
              "Failed to FundRawTransaction. append asset address not set.");
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "append asset address not set.");
        }
        const std::string& addr = reserve_txout_address.at(itr->first);
        if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
          ElementsConfidentialAddress ct_addr =
              addr_factory.GetConfidentialAddress(addr);
          Amount dust_amount = option.GetConfidentialDustFeeAmount(
              ct_addr.GetUnblindedAddress());
          if (itr->second > dust_amount) {
            ctxc.AddTxOut(
                addr_factory.GetConfidentialAddress(addr), itr->second,
                ConfidentialAssetId(itr->first));
          } else {
            warn(
                CFD_LOG_SOURCE,
                "Failed to FundRawTransaction. amount less than dust amount.");
            throw CfdException(
                CfdError::kCfdIllegalArgumentError,
                "amount less than dust amount.");
          }
        } else {
          Address address = addr_factory.GetAddress(addr);
          Amount dust_amount = option.GetConfidentialDustFeeAmount(address);
          if (itr->second > dust_amount) {
            ctxc.AddTxOut(
                address, itr->second, ConfidentialAssetId(itr->first));
          } else {
            warn(
                CFD_LOG_SOURCE,
                "Failed to FundRawTransaction. amount less than dust amount.");
            throw CfdException(
                CfdError::kCfdIllegalArgumentError,
                "amount less than dust amount.");
          }
        }
        info(
            CFD_LOG_SOURCE, "addTxOut. asset={} value={}", itr->first,
            itr->second.GetSatoshiValue());
        if (append_txout_addresses) append_txout_addresses->push_back(addr);
      }
    }
  }

  std::vector<uint8_t> txid_bytes(cfd::core::kByteData256Length);
  if (use_fee) {
    Amount new_fee = fee;
    std::vector<Utxo> lbtc_selected_coins = selected_coins;
    Amount dest_amount = tx_amount_map[fee_asset_str];
    if ((target_values.find(fee_asset_str) != target_values.end()) &&
        (target_values.at(fee_asset_str) > dest_amount)) {
      // txout設定額よりも大きな額を収集要求した
      dest_amount = target_values[fee_asset_str];
    }
    Amount need_amount = dest_amount + fee;
    Amount diff_amount = diff_amount_map[fee_asset_str];

    // Tx更新があった場合、fee再計算
    if (append_txout_count != 0) {
      // fee再計算用に選択済みUTXO情報を再設定
      std::vector<ElementsUtxoAndOption> new_selected_utxos =
          selected_txin_utxos;
      Txid txid;
      for (const auto& coin : selected_coins) {
        memcpy(txid_bytes.data(), coin.txid, txid_bytes.size());
        txid = Txid(ByteData256(txid_bytes));
        for (const UtxoData& utxo : utxos) {
          if ((txid.Equals(utxo.txid)) && (coin.vout == utxo.vout)) {
            ElementsUtxoAndOption utxo_data = {};
            utxo_data.utxo = utxo;
            new_selected_utxos.push_back(utxo_data);
            break;
          }
        }
      }
      new_fee = ElementsTransactionApi::EstimateFee(
          ctxc.GetHex(), new_selected_utxos, fee_asset, nullptr, nullptr,
          is_blind_estimate_fee, option.GetEffectiveFeeBaserate());
    }

    Amount fee_amount = fee;
    fee_amount += diff_amount;
    // fee＋余剰額よりも、再計算時のfeeが大きい場合、再度CoinSelectionを行う
    if (new_fee > fee_amount) {
      // re-select coin (fee asset only)
      std::map<std::string, Amount> new_amount_map;
      std::map<std::string, Amount> new_target_values;
      new_target_values.emplace(fee_asset_str, target_values[fee_asset_str]);
      lbtc_selected_coins = coin_select.SelectCoins(
          new_target_values, utxo_list, utxo_filter, option, new_fee,
          &new_amount_map, nullptr, nullptr);
      for (auto itr = new_amount_map.begin(); itr != new_amount_map.end();
           ++itr) {
        if (itr->first == fee_asset_str) {
          need_amount = dest_amount;
          need_amount += new_fee;
          if (itr->second >= need_amount) {
            diff_amount = itr->second;
            diff_amount -= dest_amount;
          } else {
            warn(
                CFD_LOG_SOURCE,
                "Failed to FundRawTransaction. low fee asset.");
            throw CfdException(
                CfdError::kCfdIllegalArgumentError, "low fee asset.");
          }
          fee = new_fee;
          break;
        }
      }
    }
    const std::string& addr = reserve_txout_address.at(fee_asset_str);
    Address address;
    if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
      address =
          addr_factory.GetConfidentialAddress(addr).GetUnblindedAddress();
    } else {
      address = addr_factory.GetAddress(addr);
    }
    Amount dust_amount = option.GetConfidentialDustFeeAmount(address);

    // optionで、fee対象assetかつ指定額以内の設定がある場合、余剰分をfeeに設定。
    int64_t diff_satoshi = diff_amount.GetSatoshiValue();
    int64_t fee_satoshi = fee.GetSatoshiValue();
    if (dust_amount > diff_amount) {
      // feeに残高をすべて設定
      fee_satoshi = diff_satoshi;
    } else if (diff_satoshi > 0) {
      // TxOut追加
      if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
        ctxc.AddTxOut(
            addr_factory.GetConfidentialAddress(addr),
            Amount::CreateBySatoshiAmount(diff_satoshi),
            ConfidentialAssetId(fee_asset_str));
      } else {
        ctxc.AddTxOut(
            address, Amount::CreateBySatoshiAmount(diff_satoshi),
            ConfidentialAssetId(fee_asset_str));
      }
      info(
          CFD_LOG_SOURCE, "addTxOut. asset={} value={}", fee_asset_str,
          diff_satoshi);
      if (append_txout_addresses) append_txout_addresses->push_back(addr);
    }

    // fee更新
    ctxc.UpdateTxOutFeeAmount(
        fee_index, Amount::CreateBySatoshiAmount(fee_satoshi), fee_asset);

    // Selectしたfee UTXOをTxInに設定
    for (auto& utxo : lbtc_selected_coins) {
      if (memcmp(utxo.asset, lbtc_asset, sizeof(utxo.asset)) == 0) {
        memcpy(txid_bytes.data(), utxo.txid, txid_bytes.size());
        ctxc.AddTxIn(Txid(ByteData256(txid_bytes)), utxo.vout);
      }
    }
  }
  if (estimate_fee) *estimate_fee = fee;

  // SelectしたUTXOをTxInに設定
  for (auto& utxo : selected_coins) {
    if ((!use_fee) ||
        (memcmp(utxo.asset, lbtc_asset, sizeof(utxo.asset)) != 0)) {
      memcpy(txid_bytes.data(), utxo.txid, txid_bytes.size());
      ctxc.AddTxIn(Txid(ByteData256(txid_bytes)), utxo.vout);
    }
  }
  return ctxc;
}

}  // namespace api
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
