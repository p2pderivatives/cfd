// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_elements_transaction.cpp
 *
 * @brief \~english implementation of Confidential Transaction that uses cfd-api
 *   \~japanese cfd-apiで利用するConfidential Transaction操作の実装ファイル
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
  std::vector<TxInPeginParameters> pegins;
  std::vector<TxOutPegoutParameters> pegouts;
  return CreateRawTransaction(
      version, locktime, txins, pegins, txouts, pegouts, txout_fee);
}

ConfidentialTransactionController ElementsTransactionApi::CreateRawTransaction(
    uint32_t version, uint32_t locktime,
    const std::vector<ConfidentialTxIn>& txins,
    const std::vector<TxInPeginParameters>& pegins,
    const std::vector<ConfidentialTxOut>& txouts,
    const std::vector<TxOutPegoutParameters>& pegouts,
    const ConfidentialTxOut& txout_fee,
    std::map<std::string, Amount>* pegout_addresses) const {
  ConfidentialTransactionController ctxc(version, locktime);
  return AddRawTransaction(
      ctxc.GetHex(), txins, pegins, txouts, pegouts, txout_fee,
      pegout_addresses);
}

ConfidentialTransactionController ElementsTransactionApi::AddRawTransaction(
    const std::string& tx_hex, const std::vector<ConfidentialTxIn>& txins,
    const std::vector<TxInPeginParameters>& pegins,
    const std::vector<ConfidentialTxOut>& txouts,
    const std::vector<TxOutPegoutParameters>& pegouts,
    const ConfidentialTxOut& txout_fee,
    std::map<std::string, Amount>* pegout_addresses) const {
  ConfidentialTransactionController ctxc(tx_hex);

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

  for (const auto& pegin_data : pegins) {
    ctxc.AddPeginWitness(
        pegin_data.txid, pegin_data.vout, pegin_data.amount, pegin_data.asset,
        pegin_data.mainchain_blockhash, pegin_data.claim_script,
        pegin_data.mainchain_raw_tx, pegin_data.mainchain_txoutproof);
  }

  // TxOutの追加
  for (const auto& txout : txouts) {
    ctxc.AddTxOut(
        txout.GetLockingScript(), txout.GetConfidentialValue().GetAmount(),
        txout.GetAsset(), txout.GetNonce());
  }

  for (const auto& pegout_data : pegouts) {
    Address pegout_address;
    if (pegout_data.online_pubkey.IsValid() &&
        !pegout_data.master_online_key.IsInvalid()) {
      Address dummy_addr;
      ctxc.AddPegoutTxOut(
          pegout_data.amount, pegout_data.asset, pegout_data.genesisblock_hash,
          dummy_addr, pegout_data.net_type, pegout_data.online_pubkey,
          pegout_data.master_online_key, pegout_data.bitcoin_descriptor,
          pegout_data.bip32_counter, pegout_data.whitelist,
          pegout_data.elements_net_type, &pegout_address);
    } else {
      ctxc.AddPegoutTxOut(
          pegout_data.amount, pegout_data.asset, pegout_data.genesisblock_hash,
          pegout_data.btc_address);
    }
    if (pegout_addresses != nullptr) {
      pegout_addresses->emplace(
          pegout_address.GetAddress(), pegout_data.amount);
    }
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

std::string ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const ConfidentialTxInReference& txin,
    const Pubkey& pubkey, const ConfidentialValue& value, HashType hash_type,
    const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin, pubkey.GetData(), value, hash_type, sighash_type);
}

std::string ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const ConfidentialTxInReference& txin,
    const Script& redeem_script, const ConfidentialValue& value,
    HashType hash_type, const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin, redeem_script.GetData(), value, hash_type, sighash_type);
}

std::string ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const ConfidentialTxInReference& txin,
    const ByteData& key_data, const ConfidentialValue& value,
    HashType hash_type, const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin.GetTxid(), txin.GetVout(), key_data, value, hash_type,
      sighash_type);
}

std::string ElementsTransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const ByteData& key_data, const ConfidentialValue& value,
    HashType hash_type, const SigHashType& sighash_type) const {
  ByteData sig_hash;
  ConfidentialTransactionController txc(tx_hex);
  WitnessVersion version = WitnessVersion::kVersionNone;

  if ((hash_type == HashType::kP2wpkh) || (hash_type == HashType::kP2wsh)) {
    version = WitnessVersion::kVersion0;
  }

  switch (hash_type) {
    case HashType::kP2pkh:
      // fall-through
    case HashType::kP2wpkh:
      sig_hash = txc.CreateSignatureHash(
          txid, vout, Pubkey(key_data), sighash_type, value, version);
      break;
    case HashType::kP2sh:
      // fall-through
    case HashType::kP2wsh:
      sig_hash = txc.CreateSignatureHash(
          txid, vout, Script(key_data), sighash_type, value, version);
      break;
    default:
      warn(
          CFD_LOG_SOURCE,
          "Failed to CreateSignatureHash. Invalid hash_type: "
          "hash_type={}",  // NOLINT
          hash_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hash_type. hash_type must be \"p2pkh\"(0) "
          "or \"p2sh\"(1) or \"p2wpkh\"(2) or \"p2wsh\"(3).");  // NOLINT
  }

  return sig_hash.GetHex();
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
      if (txout_key.confidential_key.IsValid()) {
        txout_confidential_keys[txout_key.index] = txout_key.confidential_key;
      } else if (txout_key.blinding_key.IsValid()) {
        txout_confidential_keys[txout_key.index] = txout_key.blinding_key;
      }
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

  if (pegout_data.online_pubkey.IsValid() &&
      !pegout_data.master_online_key.IsInvalid()) {
    Address dummy_addr;
    ctxc.AddPegoutTxOut(
        pegout_data.amount, pegout_data.asset, pegout_data.genesisblock_hash,
        dummy_addr, pegout_data.net_type, pegout_data.online_pubkey,
        pegout_data.master_online_key, pegout_data.bitcoin_descriptor,
        pegout_data.bip32_counter, pegout_data.whitelist,
        pegout_data.elements_net_type, pegout_address);
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
    bool is_blind, double effective_fee_rate, int exponent,
    int minimum_bits) const {
  uint64_t fee_rate = static_cast<uint64_t>(floor(effective_fee_rate * 1000));
  return EstimateFee(
      tx_hex, utxos, fee_asset, tx_fee, utxo_fee, is_blind, fee_rate, exponent,
      minimum_bits);
}

Amount ElementsTransactionApi::EstimateFee(
    const std::string& tx_hex, const std::vector<ElementsUtxoAndOption>& utxos,
    const ConfidentialAssetId& fee_asset, Amount* tx_fee, Amount* utxo_fee,
    bool is_blind, uint64_t effective_fee_rate, int exponent,
    int minimum_bits) const {
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

  uint32_t tx_vsize = txc.GetVsizeIgnoreTxIn(is_blind, exponent, minimum_bits);

  uint32_t size = 0;
  uint32_t witness_size = 0;
  uint32_t wit_size = 0;
  uint32_t txin_size = 0;
  uint32_t rangeproof_size_cache = 0;
  ElementsAddressApi address_api;
  for (const auto& utxo : utxos) {
    uint32_t pegin_btc_tx_size = 0;
    Script fedpeg_script;
    if (utxo.is_pegin) {
      pegin_btc_tx_size = utxo.pegin_btc_tx_size;
      fedpeg_script = utxo.fedpeg_script;
    }
    // check descriptor
    std::string descriptor = utxo.utxo.descriptor;
    // set dummy NetType for getting AddressType.
    auto data =
        address_api.ParseOutputDescriptor(descriptor, NetType::kLiquidV1, "");

    AddressType addr_type;
    if (utxo.utxo.address.GetAddress().empty() ||
        data.address_type == AddressType::kP2shP2wpkhAddress ||
        data.address_type == AddressType::kP2shP2wshAddress) {
      addr_type = data.address_type;
    } else {
      addr_type = utxo.utxo.address.GetAddressType();
    }

    Script redeem_script;
    if (utxo.utxo.redeem_script.IsEmpty() && !data.redeem_script.IsEmpty()) {
      redeem_script = data.redeem_script;
    } else {
      redeem_script = utxo.utxo.redeem_script;
    }
    const Script* scriptsig_template = nullptr;
    if ((!redeem_script.IsEmpty()) &&
        (!utxo.utxo.scriptsig_template.IsEmpty())) {
      scriptsig_template = &utxo.utxo.scriptsig_template;
    }
    bool is_issuance = utxo.is_issuance;
    bool is_reissuance = false;
    bool is_blind_issuance = utxo.is_blind_issuance;
    try {
      auto ref = txc.GetTxIn(utxo.utxo.txid, utxo.utxo.vout);
      if (utxo.is_issuance && utxo.is_blind_issuance) {
        if ((!ref.GetAssetEntropy().IsEmpty()) &&
            (!ref.GetBlindingNonce().IsEmpty())) {
          is_reissuance = true;
        }
      } else if ((!utxo.is_issuance) && (!utxo.is_blind_issuance)) {
        if (!ref.GetAssetEntropy().IsEmpty()) {
          is_issuance = true;
          is_blind_issuance = is_blind;
          if (!ref.GetBlindingNonce().IsEmpty()) {
            is_reissuance = true;
            is_blind_issuance = true;
          }
        }
      }
      if ((!utxo.is_pegin) && (ref.GetPeginWitnessStackNum() >= 6)) {
        std::vector<ByteData> pegin_stack = ref.GetPeginWitness().GetWitness();
        pegin_btc_tx_size = pegin_stack[4].GetSerializeSize();
        fedpeg_script = Script(pegin_stack[3]);
      }
    } catch (const CfdException& except) {
      // do nothing
    }

    ConfidentialTxIn::EstimateTxInSize(
        addr_type, redeem_script, pegin_btc_tx_size, fedpeg_script,
        is_issuance, is_blind_issuance, &wit_size, &txin_size, is_reissuance,
        scriptsig_template, exponent, minimum_bits, &rangeproof_size_cache);
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
  int exponent = 0;
  int minimum_bits = 0;
  option.GetBlindInfo(&exponent, &minimum_bits);

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
  std::vector<std::string> asset_list;
  int32_t fee_index = -1;
  std::vector<ConfidentialTxOutReference> txout_list = ctx.GetTxOutList();
  for (size_t index = 0; index < txout_list.size(); ++index) {
    auto& txout = txout_list[index];
    if (txout.GetLockingScript().IsEmpty()) {
      // feeは収集対象から除外
      fee_index = static_cast<int32_t>(index);
    } else {
      std::string asset = txout.GetAsset().GetHex();
      if (std::find(asset_list.begin(), asset_list.end(), asset) ==
          asset_list.end()) {
        asset_list.push_back(asset);
      }

      if (tx_amount_map.find(asset) == tx_amount_map.end()) {
        Amount amount;
        tx_amount_map.emplace(asset, amount);
      }
      tx_amount_map[asset] += txout.GetConfidentialValue().GetAmount();
    }
  }
  std::map<std::string, std::vector<OutPoint>> asset_utxo_map;
  const auto& txin_list = ctx.GetTxInList();  // txin_utxo_list
  for (const auto& elements_utxo : selected_txin_utxos) {
    for (const auto& txin : txin_list) {
      if ((txin.GetTxid().Equals(elements_utxo.utxo.txid)) &&
          (elements_utxo.utxo.vout == txin.GetVout())) {
        std::string asset = elements_utxo.utxo.asset.GetHex();
        if (std::find(asset_list.begin(), asset_list.end(), asset) ==
            asset_list.end()) {
          asset_list.push_back(asset);
        }
        if (asset_utxo_map.find(asset) == asset_utxo_map.end()) {
          std::vector<OutPoint> outpoint_list = {
              OutPoint(elements_utxo.utxo.txid, elements_utxo.utxo.vout)};
          asset_utxo_map[asset] = outpoint_list;
        } else {
          OutPoint outpoint(elements_utxo.utxo.txid, elements_utxo.utxo.vout);
          asset_utxo_map[asset].push_back(outpoint);
        }

        if (txin_amount_map.find(asset) == txin_amount_map.end()) {
          Amount amount;
          txin_amount_map.emplace(asset, amount);
        }
        txin_amount_map[asset] += elements_utxo.utxo.amount;
        break;
      }
    }
  }

  // append issuance txin data.
  for (const auto& txin : txin_list) {
    if (!txin.GetBlindingNonce().IsEmpty() ||
        !txin.GetAssetEntropy().IsEmpty()) {
      BlindFactor asset_entropy;
      std::string token;
      if (txin.GetBlindingNonce().IsEmpty()) {
        asset_entropy = ConfidentialTransaction::CalculateAssetEntropy(
            txin.GetTxid(), txin.GetVout(), txin.GetAssetEntropy());
        ConfidentialAssetId token1 =
            ConfidentialTransaction::CalculateReissuanceToken(
                asset_entropy, true);
        ConfidentialAssetId token2 =
            ConfidentialTransaction::CalculateReissuanceToken(
                asset_entropy, false);
        std::string token_blind = token1.GetHex();
        std::string token_unblind = token2.GetHex();
        if (tx_amount_map.find(token_blind) != tx_amount_map.end()) {
          token = token_blind;
        } else if (tx_amount_map.find(token_unblind) != tx_amount_map.end()) {
          token = token_unblind;
        }
      } else {
        asset_entropy = BlindFactor(txin.GetAssetEntropy());
      }
      ConfidentialAssetId asset_id =
          ConfidentialTransaction::CalculateAsset(asset_entropy);
      std::string asset = asset_id.GetHex();

      if (txin.GetBlindingNonce().IsEmpty()) {
        // At the time of issuance, add to map if it is not registered.
        if (txin_amount_map.find(asset) == txin_amount_map.end()) {
          txin_amount_map.emplace(asset, txin.GetIssuanceAmount().GetAmount());
        }
        if ((!token.empty()) &&
            (txin_amount_map.find(token) == txin_amount_map.end())) {
          txin_amount_map.emplace(token, txin.GetInflationKeys().GetAmount());
        }
      } else if (txin_amount_map.find(asset) == txin_amount_map.end()) {
        // At the time of reissuance, add to map if it is not registered asset.
        txin_amount_map.emplace(asset, txin.GetIssuanceAmount().GetAmount());
      } else {
        // At the time of reissuance,
        // add to map if it is not registered asset of utxo.
        OutPoint outpoint(txin.GetTxid(), txin.GetVout());
        std::vector<OutPoint>& outpoint_list = asset_utxo_map[asset];
        if (std::find(outpoint_list.begin(), outpoint_list.end(), outpoint) ==
            outpoint_list.end()) {
          txin_amount_map[asset] += txin.GetIssuanceAmount().GetAmount();
        }
      }
    }
  }

  // targetsで設定されたassetをasset_listへ追加
  for (const auto& target : map_target_value) {
    std::string asset = target.first;
    if (std::find(asset_list.begin(), asset_list.end(), asset) ==
        asset_list.end()) {
      asset_list.push_back(asset);
    }
  }
  std::vector<UtxoData> utxodata_list;
  utxodata_list.reserve(utxos.size());
  for (const auto& utxo : utxos) {
    bool isFind = false;
    for (const auto& txin : txin_list) {
      if ((txin.GetTxid().Equals(utxo.txid)) &&
          (utxo.vout == txin.GetVout())) {
        isFind = true;
        break;
      }
    }
    if (!isFind) {
      utxodata_list.push_back(utxo);
    }
  }

  // txへfee領域の設定と初期のfee計算
  Amount fee;
  bool use_fee = false;
  if (option.GetEffectiveFeeBaserate() > 0) {
    if (fee_asset.IsEmpty()) {
      warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. Empty fee asset.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Empty fee asset.");
    }
    use_fee = true;
    std::string asset = fee_asset.GetHex();
    if (std::find(asset_list.begin(), asset_list.end(), asset) ==
        asset_list.end()) {
      asset_list.push_back(asset);  // insert fee asset
    }
    // feeの存在確認と、fee領域の確保
    if (fee_index == -1) {
      // txoutにfee追加
      ctxc.AddTxOutFee(Amount::CreateBySatoshiAmount(0), fee_asset);
      fee_index = static_cast<int32_t>(txout_list.size());
    }
    fee = EstimateFee(
        ctxc.GetHex(), selected_txin_utxos, fee_asset, nullptr, nullptr,
        is_blind_estimate_fee, option.GetEffectiveFeeBaserate(), exponent,
        minimum_bits);
    if (estimate_fee) *estimate_fee = fee;
  }

  // 探索対象assetを設定。未設定時はTxOutの合計額を設定。
  // asset毎にcoin selection額を設定
  // ** fee assetは誤差を少なくする為fee以外のtxoutを追加してから計算する **
  std::map<std::string, Amount> target_values = map_target_value;
  std::map<std::string, Amount> select_require_values;
  for (auto& asset : asset_list) {
    if (target_values.find(asset) == target_values.end()) {
      continue;
    }
    bool is_fee_asset = (asset == fee_asset.GetHex());
    if (use_fee && is_fee_asset) {
      continue;
    }
    Amount txin_amount = txin_amount_map[asset];
    Amount tx_amount = tx_amount_map[asset];
    Amount target_value = target_values[asset];
    Amount diff_amount = Amount();
    bool isForceCheck = false;

    if (txin_amount > tx_amount) {
      diff_amount = txin_amount - tx_amount;
      if (diff_amount < target_value) {
        // txinの余剰分でtarget分が満たされない場合、満たされない分をコインセレクト
        target_value -= diff_amount;
      } else {
        target_value = Amount();  // 不足分がない場合はtxinから全額補う
        isForceCheck = true;  // CoinSelect対象として処理する必要あり
      }
    } else if (txin_amount < tx_amount) {
      // txoutの不足分を計算
      diff_amount = tx_amount - txin_amount;
      // txoutの不足分を合わせてコインセレクト
      target_value += diff_amount;
    }
    if ((target_value != 0) || isForceCheck) {
      select_require_values[asset] = target_value;
    }
  }

  // execute coinselection
  CoinSelection coin_select;
  std::vector<Utxo> utxo_list = UtxoUtil::ConvertToUtxo(utxodata_list);
  std::map<std::string, Amount> amount_map;
  std::vector<Utxo> selected_coins;
  Amount utxo_fee;
  selected_coins = coin_select.SelectCoins(
      select_require_values, utxo_list, utxo_filter, option, fee, &amount_map,
      &utxo_fee, nullptr);
  // Amount total_fee = fee + utxo_fee;  // fee amount without fee asset

  // fee_asset_byteを用意
  std::string fee_asset_str;
  uint8_t fee_asset_bytes[33];
  if (use_fee) {
    fee_asset_str = fee_asset.GetHex();
    memcpy(
        fee_asset_bytes, fee_asset.GetData().GetBytes().data(),
        sizeof(fee_asset_bytes));
  }

  // txoutへ追加するamountのmapを用意
  std::map<std::string, Amount> append_txout_amount_map = amount_map;
  {  // for warning
    auto itr = amount_map.begin();
    while (itr != amount_map.end()) {
      std::string asset = itr->first;
      Amount txin_amount = txin_amount_map[asset];
      Amount txout_amount = tx_amount_map[asset];
      append_txout_amount_map[itr->first] =
          itr->second + txin_amount - txout_amount;

      if (use_fee && (itr->first == fee_asset_str)) {
        /* fee assetは別で計算するため、txout追加対象から除外 */
        append_txout_amount_map.erase(itr->first);
      }
      ++itr;
    }
  }

  // TxOut追加
  for (auto itr = append_txout_amount_map.begin();
       itr != append_txout_amount_map.end(); ++itr) {
    if (itr->second > 0) {
      std::string address_str;
      if (reserve_txout_address.find(itr->first) !=
          reserve_txout_address.end()) {
        address_str = reserve_txout_address.at(itr->first);
      }

      if (address_str.empty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to FundRawTransaction. Append asset address not set.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to FundRawTransaction. Append asset address not set.");
      }

      // address種別チェック
      if (ElementsConfidentialAddress::IsConfidentialAddress(address_str)) {
        ElementsConfidentialAddress ct_addr =
            addr_factory.GetConfidentialAddress(address_str);
        if (!addr_factory.CheckConfidentialAddressNetType(ct_addr, net_type)) {
          warn(
              CFD_LOG_SOURCE,
              "Failed to FundRawTransaction. "
              "Input address and network is unmatch."
              ": address=[{}], input_net_type=[{}], parsed_net_type=[{}]",
              address_str, net_type, ct_addr.GetNetType());
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "Failed to FundRawTransaction. "
              "Input address and network is unmatch.");
        }

        // txout追加処理
        Amount dust_amount =
            option.GetConfidentialDustFeeAmount(ct_addr.GetUnblindedAddress());
        if (itr->second > dust_amount) {
          ctxc.AddTxOut(ct_addr, itr->second, ConfidentialAssetId(itr->first));
        } else {
          warn(
              CFD_LOG_SOURCE,
              "Failed to FundRawTransaction. amount less than dust amount.");
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "amount less than dust amount.");
        }
      } else {
        Address address = addr_factory.GetAddress(address_str);
        if (!addr_factory.CheckAddressNetType(address, net_type)) {
          warn(
              CFD_LOG_SOURCE,
              "Failed to FundRawTransaction. "
              "Input address and network is unmatch."
              ": address=[{}], input_net_type=[{}], parsed_net_type=[{}]",
              address_str, net_type, address.GetNetType());
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "Failed to FundRawTransaction. "
              "Input address and network is unmatch.");
        }
        Amount dust_amount = option.GetConfidentialDustFeeAmount(address);
        if (itr->second > dust_amount) {
          ctxc.AddTxOut(address, itr->second, ConfidentialAssetId(itr->first));
        } else {
          warn(
              CFD_LOG_SOURCE,
              "Failed to FundRawTransaction. amount less than dust amount.");
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "amount less than dust amount.");
        }
      }
      if (append_txout_addresses)
        append_txout_addresses->push_back(address_str);
    }
  }

  // fee asset計算処理
  std::vector<uint8_t> txid_bytes(cfd::core::kByteData256Length);
  if (use_fee) {
    std::string address_str;
    if (reserve_txout_address.find(fee_asset_str) !=
        reserve_txout_address.end()) {
      address_str = reserve_txout_address.at(fee_asset_str);
    }
    if (address_str.empty()) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to FundRawTransaction. fee reserve address not set.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to FundRawTransaction. fee reserve address not set.");
    }
    Address address;
    if (ElementsConfidentialAddress::IsConfidentialAddress(address_str)) {
      address = addr_factory.GetConfidentialAddress(address_str)
                    .GetUnblindedAddress();
    } else {
      address = addr_factory.GetAddress(address_str);
    }
    if (!addr_factory.CheckAddressNetType(address, net_type)) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to FundRawTransaction. "
          "Input address and network is unmatch."
          ": address=[{}], input_net_type=[{}], parsed_net_type=[{}]",
          address_str, net_type, address.GetNetType());
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to FundRawTransaction. "
          "Input address and network is unmatch.");
    }

    Amount min_fee;
    {
      // dummyのtx作成
      ConfidentialTransactionContext txc_dummy(ctxc.GetHex());
      // fee再計算用に選択済みUTXO情報をElements用Utxoに再設定
      std::vector<ElementsUtxoAndOption> new_selected_utxos =
          selected_txin_utxos;
      Txid txid;
      for (const auto& coin : selected_coins) {
        memcpy(txid_bytes.data(), coin.txid, txid_bytes.size());
        txid = Txid(ByteData256(txid_bytes));
        for (const UtxoData& utxo : utxodata_list) {
          if ((txid.Equals(utxo.txid)) && (coin.vout == utxo.vout)) {
            ElementsUtxoAndOption utxo_data = {};
            utxo_data.utxo = utxo;
            new_selected_utxos.push_back(utxo_data);
            break;
          }
        }
      }
      min_fee = ElementsTransactionApi::EstimateFee(
          txc_dummy.GetHex(), new_selected_utxos, fee_asset, nullptr, nullptr,
          is_blind_estimate_fee, option.GetEffectiveFeeBaserate(), exponent,
          minimum_bits);
      // add dummy txout（余剰額のTxOut追加考慮）
      txc_dummy.AddTxOut(
          address, Amount(1), ConfidentialAssetId(fee_asset_str));
      fee = ElementsTransactionApi::EstimateFee(
          txc_dummy.GetHex(), new_selected_utxos, fee_asset, nullptr, nullptr,
          is_blind_estimate_fee, option.GetEffectiveFeeBaserate(), exponent,
          minimum_bits);
    }

    Amount dust_amount = option.GetConfidentialDustFeeAmount(address);

    Amount fee_asset_target_value;
    Amount txin_amount;
    Amount tx_amount;
    Amount target_value;
    {
      if (txin_amount_map.find(fee_asset_str) != txin_amount_map.end()) {
        txin_amount = txin_amount_map[fee_asset_str];
      }
      if (tx_amount_map.find(fee_asset_str) != tx_amount_map.end()) {
        tx_amount = tx_amount_map[fee_asset_str];
      }
      if (target_values.find(fee_asset_str) != target_values.end()) {
        target_value = target_values[fee_asset_str];
      }
      fee_asset_target_value = target_value + fee;
      Amount diff_amount;
      if (txin_amount > tx_amount) {
        Amount check_min_fee = dust_amount + min_fee;
        diff_amount = txin_amount - tx_amount;
        if ((target_value == 0) && (diff_amount > min_fee) &&
            (diff_amount < check_min_fee)) {
          // 既存UTXOで満たされる場合、coinselection不要かつTxOut追加も不要
          fee_asset_target_value = Amount();
          fee = min_fee;
          info(CFD_LOG_SOURCE, "use minimum fee[{}]", fee.GetSatoshiValue());
        } else if (diff_amount >= fee_asset_target_value) {
          fee_asset_target_value = Amount();
        } else {
          // txinの余剰分でtarget分が満たされない場合、不足分をコインセレクト
          fee_asset_target_value -= diff_amount;
        }
      } else if (txin_amount < tx_amount) {
        // txoutの不足分を計算
        diff_amount = tx_amount - txin_amount;
        // txoutの不足分を合わせてコインセレクト
        fee_asset_target_value += diff_amount;
      }
    }

    std::vector<Utxo> fee_selected_coins;
    // re-select coin (fee asset only)
    // collect amount is using large-fee.
    std::map<std::string, Amount> new_amount_map;
    Amount fee_selected_value;
    Amount append_fee_asset_txout_value;
    if (fee_asset_target_value != 0) {
      std::map<std::string, Amount> new_target_values;
      new_target_values.emplace(fee_asset_str, fee_asset_target_value);
      fee_selected_coins = coin_select.SelectCoins(
          new_target_values, utxo_list, utxo_filter, option, fee,
          &new_amount_map, &utxo_fee, nullptr);
      fee += utxo_fee;
      // append txout for fee asset
      for (auto itr = new_amount_map.begin(); itr != new_amount_map.end();
           ++itr) {
        if (itr->first == fee_asset_str) {
          fee_selected_value = itr->second;
          break;
        }
      }
    }
    if ((fee_selected_value + txin_amount) < (tx_amount + fee)) {
      warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. low fee asset.");
      throw CfdException(CfdError::kCfdIllegalArgumentError, "low fee asset.");
    }
    append_fee_asset_txout_value =
        fee_selected_value + txin_amount - tx_amount - fee;

    // fee assetのoutput amountが、dust amount以下であればfeeに設定。
    if (dust_amount > append_fee_asset_txout_value) {
      // feeに残高をすべて設定
      fee += append_fee_asset_txout_value;
    } else if (append_fee_asset_txout_value > 0) {
      // TxOut追加
      if (ElementsConfidentialAddress::IsConfidentialAddress(address_str)) {
        ctxc.AddTxOut(
            addr_factory.GetConfidentialAddress(address_str),
            append_fee_asset_txout_value, ConfidentialAssetId(fee_asset_str));
      } else {
        ctxc.AddTxOut(
            address, append_fee_asset_txout_value,
            ConfidentialAssetId(fee_asset_str));
      }
      if (append_txout_addresses)
        append_txout_addresses->push_back(address_str);
    }

    // fee更新
    ctxc.UpdateTxOutFeeAmount(fee_index, fee, fee_asset);

    // Selectしたfee UTXOをTxInに設定
    for (auto& utxo : fee_selected_coins) {
      if (memcmp(utxo.asset, fee_asset_bytes, sizeof(utxo.asset)) == 0) {
        memcpy(txid_bytes.data(), utxo.txid, txid_bytes.size());
        ctxc.AddTxIn(Txid(ByteData256(txid_bytes)), utxo.vout);
      }
    }
    if (estimate_fee) *estimate_fee = fee;
  }

  // SelectしたUTXOをTxInに設定
  for (auto& utxo : selected_coins) {
    if ((!use_fee) ||
        (memcmp(utxo.asset, fee_asset_bytes, sizeof(utxo.asset)) != 0)) {
      memcpy(txid_bytes.data(), utxo.txid, txid_bytes.size());
      ctxc.AddTxIn(Txid(ByteData256(txid_bytes)), utxo.vout);
    }
  }
  return ctxc;
}

}  // namespace api
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
