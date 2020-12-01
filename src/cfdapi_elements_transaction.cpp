// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_elements_transaction.cpp
 *
 * @brief \~english implementation of Confidential Transaction that uses cfd-api
 *   \~japanese cfd-apiで利用するConfidential Transaction操作の実装ファイル
 */
#ifndef CFD_DISABLE_ELEMENTS
#include "cfd/cfdapi_elements_transaction.h"

#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_fee.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_elements_address.h"
#include "cfd/cfdapi_transaction.h"
#include "cfd_manager.h"              // NOLINT
#include "cfdapi_transaction_base.h"  // NOLINT
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

namespace cfd {
namespace api {

using cfd::ConfidentialTransactionContext;
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
    const ConfidentialAssetId& fee_asset, Amount* txout_fee, Amount* utxo_fee,
    bool is_blind, double effective_fee_rate, int exponent,
    int minimum_bits) const {
  uint64_t fee_rate = static_cast<uint64_t>(floor(effective_fee_rate * 1000));
  return EstimateFee(
      tx_hex, utxos, fee_asset, txout_fee, utxo_fee, is_blind, fee_rate,
      exponent, minimum_bits);
}

Amount ElementsTransactionApi::EstimateFee(
    const std::string& tx_hex, const std::vector<ElementsUtxoAndOption>& utxos,
    const ConfidentialAssetId& fee_asset, Amount* txout_fee, Amount* utxo_fee,
    bool is_blind, uint64_t effective_fee_rate, int exponent,
    int minimum_bits) const {
  ConfidentialTransactionContext txc(tx_hex);

  if (fee_asset.IsEmpty()) {
    warn(CFD_LOG_SOURCE, "Failed to EstimateFee. Empty fee asset.");
    throw CfdException(CfdError::kCfdIllegalArgumentError, "Empty fee asset.");
  }

  // check fee in txout
  bool exist_fee = false;
  for (const auto& txout : txc.GetTxOutList()) {
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

  uint32_t size = 0;
  uint32_t witness_size = 0;
  uint32_t wit_size = 0;
  uint32_t txin_size = 0;
  uint32_t rangeproof_size_cache = 0;
  uint32_t asset_count = 0;
  uint32_t not_witness_count = 0;
  ElementsAddressApi address_api;
  for (const auto& utxo : utxos) {
    uint32_t pegin_btc_tx_size = 0;
    txin_size = 0;
    wit_size = 0;
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
      auto ref = txc.GetTxIn(OutPoint(utxo.utxo.txid, utxo.utxo.vout));
      if (utxo.is_issuance) {
        if ((!ref.GetAssetEntropy().IsEmpty()) &&
            (!ref.GetBlindingNonce().IsEmpty())) {
          is_reissuance = true;
        }
      } else if ((!utxo.is_issuance) && (!utxo.is_blind_issuance)) {  // init
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
        pegin_btc_tx_size =
            static_cast<uint32_t>(pegin_stack[4].GetSerializeSize());
        fedpeg_script = Script(pegin_stack[3]);
      }

      if (utxo.is_issuance && ref.GetAssetEntropy().IsEmpty()) {
        // unmatch pattern. (using utxo data)
      } else {
        ref.EstimateTxInSize(
            addr_type, redeem_script, is_blind_issuance, exponent,
            minimum_bits, fedpeg_script, scriptsig_template, &wit_size,
            &txin_size);
      }
    } catch (const CfdException& except) {
      info(CFD_LOG_SOURCE, "Error:{}", std::string(except.what()));
    }

    if (is_reissuance)
      ++asset_count;
    else if (is_issuance)
      asset_count += 2;

    ++asset_count;
    if (txin_size == 0) {
      ConfidentialTxIn::EstimateTxInSize(
          addr_type, redeem_script, pegin_btc_tx_size, fedpeg_script,
          is_issuance, is_blind_issuance, &wit_size, &txin_size, is_reissuance,
          scriptsig_template, exponent, minimum_bits, &rangeproof_size_cache);
    }
    size += txin_size;
    witness_size += wit_size;
    if (wit_size == 0) ++not_witness_count;
  }
  if ((not_witness_count != 0) &&
      (not_witness_count < static_cast<uint32_t>(utxos.size()))) {
    // append witness size for p2pkh or p2sh
    witness_size += not_witness_count * 4;
  }

  uint32_t utxo_vsize =
      AbstractTransaction::GetVsizeFromSize(size, witness_size);

  uint32_t tx_vsize =
      txc.GetVsizeIgnoreTxIn(is_blind, exponent, minimum_bits, asset_count);

  FeeCalculator fee_calc(effective_fee_rate);
  Amount tx_fee_amount = fee_calc.GetFee(tx_vsize);
  Amount utxo_fee_amount = fee_calc.GetFee(utxo_vsize);
  Amount fee = tx_fee_amount + utxo_fee_amount;

  if (txout_fee) *txout_fee = tx_fee_amount;
  if (utxo_fee) *utxo_fee = utxo_fee_amount;

  info(
      CFD_LOG_SOURCE, "EstimateFee rate={} fee={} tx={} utxo={}",
      effective_fee_rate, fee.GetSatoshiValue(),
      tx_fee_amount.GetSatoshiValue(), utxo_fee_amount.GetSatoshiValue());
  return fee;
}

/**
 * @brief collect utxo data by fundrawtransaction.
 * @param[in] ctx                  confidential transaction context
 * @param[in] selected_txin_utxos  txin utxo list
 * @param[out] txin_amount_map     txin amount map
 * @param[out] tx_amount_map       tx amount map
 * @param[out] asset_list          target asset list
 * @param[out] fee_index           fee index
 */
void CollectUtxoDataByFundRawTransaction(
    const ConfidentialTransactionContext& ctx,
    const std::vector<ElementsUtxoAndOption>& selected_txin_utxos,
    std::map<std::string, int64_t>* txin_amount_map,
    std::map<std::string, int64_t>* tx_amount_map,
    std::vector<std::string>* asset_list, int32_t* fee_index) {
  const auto txout_list = ctx.GetTxOutList();
  for (size_t index = 0; index < txout_list.size(); ++index) {
    auto& txout = txout_list[index];
    if (txout.GetLockingScript().IsEmpty()) {
      // Excludes fees from collection
      *fee_index = static_cast<int32_t>(index);
    } else {
      std::string asset = txout.GetAsset().GetHex();
      if (std::find(asset_list->begin(), asset_list->end(), asset) ==
          asset_list->end()) {
        asset_list->push_back(asset);
      }

      if (tx_amount_map->find(asset) == tx_amount_map->end()) {
        int64_t amount = 0;
        tx_amount_map->emplace(asset, amount);
      }
      (*tx_amount_map)[asset] +=
          txout.GetConfidentialValue().GetAmount().GetSatoshiValue();
    }
  }
  std::map<std::string, std::vector<OutPoint>> asset_utxo_map;
  const auto txin_list = ctx.GetTxInList();  // txin_utxo_list
  for (const auto& elements_utxo : selected_txin_utxos) {
    for (const auto& txin : txin_list) {
      if ((txin.GetTxid().Equals(elements_utxo.utxo.txid)) &&
          (elements_utxo.utxo.vout == txin.GetVout())) {
        std::string asset = elements_utxo.utxo.asset.GetHex();
        if (std::find(asset_list->begin(), asset_list->end(), asset) ==
            asset_list->end()) {
          asset_list->push_back(asset);
        }
        if (asset_utxo_map.find(asset) == asset_utxo_map.end()) {
          std::vector<OutPoint> outpoint_list = {
              OutPoint(elements_utxo.utxo.txid, elements_utxo.utxo.vout)};
          asset_utxo_map[asset] = outpoint_list;
        } else {
          OutPoint outpoint(elements_utxo.utxo.txid, elements_utxo.utxo.vout);
          asset_utxo_map[asset].push_back(outpoint);
        }

        if (txin_amount_map->find(asset) == txin_amount_map->end()) {
          int64_t amount = 0;
          txin_amount_map->emplace(asset, amount);
        }
        (*txin_amount_map)[asset] +=
            elements_utxo.utxo.amount.GetSatoshiValue();
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
        if (tx_amount_map->find(token_blind) != tx_amount_map->end()) {
          token = token_blind;
        } else if (
            tx_amount_map->find(token_unblind) != tx_amount_map->end()) {
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
        if (txin_amount_map->find(asset) == txin_amount_map->end()) {
          txin_amount_map->emplace(
              asset, txin.GetIssuanceAmount().GetAmount().GetSatoshiValue());
        }
        if ((!token.empty()) &&
            (txin_amount_map->find(token) == txin_amount_map->end())) {
          txin_amount_map->emplace(
              token, txin.GetInflationKeys().GetAmount().GetSatoshiValue());
        }
      } else if (txin_amount_map->find(asset) == txin_amount_map->end()) {
        // At the time of reissuance, add to map if it is not registered asset.
        txin_amount_map->emplace(
            asset, txin.GetIssuanceAmount().GetAmount().GetSatoshiValue());
      } else {
        // At the time of reissuance,
        // add to map if it is not registered asset of utxo.
        OutPoint outpoint(txin.GetTxid(), txin.GetVout());
        std::vector<OutPoint>& outpoint_list = asset_utxo_map[asset];
        if (std::find(outpoint_list.begin(), outpoint_list.end(), outpoint) ==
            outpoint_list.end()) {
          (*txin_amount_map)[asset] +=
              txin.GetIssuanceAmount().GetAmount().GetSatoshiValue();
        }
      }
    }
  }
}

/**
 * @brief calculate fee and fund transaction.
 * @param[in] api                       API object
 * @param[in] addr_factory              address factory object
 * @param[in] txin_amount_map           txin amount map
 * @param[in] tx_amount_map             tx amount map
 * @param[in] target_values             target amounts
 * @param[in] input_amount_map          input amount map
 * @param[in] input_max_map             input max amount map
 * @param[in] selected_coins            select utxo list
 * @param[in] utxodata_list             utxo list
 * @param[in] fee_asset                 fee asset
 * @param[in] selected_txin_utxos       txin utxo list
 * @param[in] reserve_txout_address     reserved address with asset
 * @param[in] net_type                  network type
 * @param[in] is_blind_estimate_fee     blind estimete fee flag
 * @param[in] utxo_filter               utxo filter
 * @param[in] option                    coin selection option
 * @param[in] utxo_list                 utxo list
 * @param[in,out] ctxc                  confidential transaction context
 * @param[out] append_txout_addresses   append txout address list
 * @param[out] estimate_fee             estimate fee
 */
void CalculateFeeAndFundTransaction(
    const ElementsTransactionApi& api,
    const ElementsAddressFactory& addr_factory,
    const std::map<std::string, int64_t>& txin_amount_map,
    const std::map<std::string, int64_t>& tx_amount_map,
    const std::map<std::string, int64_t>& target_values,
    const std::map<std::string, int64_t>& input_amount_map,
    const std::map<std::string, int64_t>& input_max_map,
    const std::vector<Utxo>& selected_coins,
    const std::vector<UtxoData>& utxodata_list,
    const ConfidentialAssetId& fee_asset,
    const std::vector<ElementsUtxoAndOption>& selected_txin_utxos,
    const std::map<std::string, std::string>& reserve_txout_address,
    NetType net_type, bool is_blind_estimate_fee,
    const UtxoFilter& utxo_filter, const CoinSelectionOption& option,
    const std::vector<Utxo>& utxo_list, ConfidentialTransactionContext* ctxc,
    std::vector<std::string>* append_txout_addresses, Amount* estimate_fee) {
  std::string fee_asset_str = fee_asset.GetHex();
  uint8_t fee_asset_bytes[33];
  memcpy(
      fee_asset_bytes, fee_asset.GetData().GetBytes().data(),
      sizeof(fee_asset_bytes));
  int exponent = 0;
  int minimum_bits = 0;
  option.GetBlindInfo(&exponent, &minimum_bits);
  std::vector<uint8_t> txid_bytes(cfd::core::kByteData256Length);
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
    address =
        addr_factory.GetConfidentialAddress(address_str).GetUnblindedAddress();
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

  int64_t txin_amount = 0;
  int64_t tx_amount = 0;
  int64_t target_value = 0;
  int64_t utxo_value = 0;
  int64_t max_utxo_value = 0;
  if (txin_amount_map.find(fee_asset_str) != txin_amount_map.end())
    txin_amount = txin_amount_map.at(fee_asset_str);
  if (tx_amount_map.find(fee_asset_str) != tx_amount_map.end())
    tx_amount = tx_amount_map.at(fee_asset_str);
  if (target_values.find(fee_asset_str) != target_values.end())
    target_value = target_values.at(fee_asset_str);
  if (input_amount_map.find(fee_asset_str) != input_amount_map.end())
    utxo_value = input_amount_map.at(fee_asset_str);
  if (input_max_map.find(fee_asset_str) != input_max_map.end())
    max_utxo_value = input_max_map.at(fee_asset_str);

  int64_t min_fee;
  ConfidentialTransactionContext txc_dummy(ctxc->GetHex());
  uint32_t dummy_txout_index = 0;
  std::vector<ElementsUtxoAndOption> new_selected_utxos;
  // Reset the selected UTXO information to the Utxo class for Elements to recalculate the fee. // NOLINT
  new_selected_utxos = selected_txin_utxos;
  for (const auto& coin : selected_coins) {
    memcpy(txid_bytes.data(), coin.txid, txid_bytes.size());
    Txid txid = Txid(ByteData256(txid_bytes));
    for (const UtxoData& utxo : utxodata_list) {
      if ((txid.Equals(utxo.txid)) && (coin.vout == utxo.vout)) {
        ElementsUtxoAndOption utxo_data = {};
        utxo_data.utxo = utxo;
        new_selected_utxos.push_back(utxo_data);
        break;
      }
    }
  }

  Amount min_fee_amount = api.EstimateFee(
      txc_dummy.GetHex(), new_selected_utxos, fee_asset, nullptr, nullptr,
      is_blind_estimate_fee, option.GetEffectiveFeeBaserate(), exponent,
      minimum_bits);
  min_fee = min_fee_amount.GetSatoshiValue();
  int64_t dummy_sat = (txin_amount < tx_amount) ? tx_amount - txin_amount : 0;
  dummy_sat += target_value + (min_fee * 2);
  dummy_sat = 2 * ((max_utxo_value > dummy_sat) ? max_utxo_value : dummy_sat);
  if ((dummy_sat <= 0) || (dummy_sat > cfd::core::kMaxAmount))
    dummy_sat = cfd::core::kMaxAmount;
  warn(CFD_LOG_SOURCE, "Set dummy_amount={}", dummy_sat);
  dummy_txout_index = txc_dummy.GetTxOutCount();
  txc_dummy.AddTxOut(
      address, Amount(dummy_sat), ConfidentialAssetId(fee_asset_str));
  Amount fee = api.EstimateFee(
      txc_dummy.GetHex(), new_selected_utxos, fee_asset, nullptr, nullptr,
      is_blind_estimate_fee, option.GetEffectiveFeeBaserate(), exponent,
      minimum_bits);

  int64_t dust_amount =
      option.GetConfidentialDustFeeAmount(address).GetSatoshiValue();
  int64_t fee_asset_target_value = target_value + fee.GetSatoshiValue();
  if (txin_amount > tx_amount) {
    int64_t check_min_fee = dust_amount + min_fee;
    int64_t diff_amount = txin_amount - tx_amount;
    if ((target_value == 0) && (diff_amount > min_fee) &&
        (diff_amount < check_min_fee)) {
      // If the existing UTXO is filled, there is no need to select coin and add TxOut. // NOLINT
      fee_asset_target_value = 0;
      fee = Amount(min_fee);
      info(CFD_LOG_SOURCE, "use minimum fee[{}]", fee.GetSatoshiValue());
    } else if (diff_amount >= fee_asset_target_value) {
      fee_asset_target_value = 0;
    } else {
      // If the surplus of txin does not meet the target, coin select the shortfall. // NOLINT
      fee_asset_target_value -= diff_amount;
    }
  } else if (txin_amount < tx_amount) {
    // Select coins according to the shortage of txout.
    fee_asset_target_value += tx_amount - txin_amount;
  }

  std::vector<Utxo> fee_selected_coins;
  // re-select coin (fee asset only). collect amount is using large-fee.
  std::map<std::string, int64_t> new_amount_map;
  int64_t fee_selected_value = 0;
  int64_t append_fee_asset_txout_value;
  if (fee_asset_target_value != 0) {
    CoinSelection coin_select;
    Amount utxo_fee;
    std::map<std::string, int64_t> new_target_values;
    new_target_values.emplace(fee_asset_str, fee_asset_target_value);
    fee_selected_coins = coin_select.SelectCoins(
        new_target_values, utxo_list, utxo_filter, option, fee,
        &new_amount_map, &utxo_fee, nullptr);
    // append txout for fee asset
    for (auto itr = new_amount_map.begin(); itr != new_amount_map.end();
         ++itr) {
      if (itr->first == fee_asset_str) {
        fee_selected_value = itr->second;
        break;
      }
    }
    // estimate fee after coinselection (new fee < old fee)
    int64_t dummy_amount =
        fee_selected_value + txin_amount - tx_amount - fee.GetSatoshiValue();
    txc_dummy.SetTxOutValue(dummy_txout_index, Amount(dummy_amount));
    Amount new_fee = api.EstimateFee(
        txc_dummy.GetHex(), new_selected_utxos, fee_asset, nullptr, nullptr,
        is_blind_estimate_fee, option.GetEffectiveFeeBaserate(), exponent,
        minimum_bits);
    if (new_fee < fee) fee = new_fee;
    fee += utxo_fee;
  }
  if ((fee_selected_value + txin_amount) < (tx_amount + fee)) {
    warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. low fee asset.");
    throw CfdException(CfdError::kCfdIllegalArgumentError, "low fee asset.");
  }
  append_fee_asset_txout_value =
      fee_selected_value + txin_amount - tx_amount - fee.GetSatoshiValue();

  // If the output amount of the fee asset is less than the dust amount, set it to fee.  // NOLINT
  if (dust_amount > append_fee_asset_txout_value) {
    // Set all the remaining amount to Fee.
    fee += append_fee_asset_txout_value;
  } else if (append_fee_asset_txout_value > 0) {
    if (ElementsConfidentialAddress::IsConfidentialAddress(address_str)) {
      ctxc->AddTxOut(
          addr_factory.GetConfidentialAddress(address_str),
          Amount(append_fee_asset_txout_value),
          ConfidentialAssetId(fee_asset_str));
    } else {
      ctxc->AddTxOut(
          address, Amount(append_fee_asset_txout_value),
          ConfidentialAssetId(fee_asset_str));
    }
    if (append_txout_addresses) append_txout_addresses->push_back(address_str);
  }

  ctxc->UpdateFeeAmount(fee, fee_asset);

  for (auto& utxo : fee_selected_coins) {
    if (memcmp(utxo.asset, fee_asset_bytes, sizeof(utxo.asset)) == 0) {
      memcpy(txid_bytes.data(), utxo.txid, txid_bytes.size());
      ctxc->AddTxIn(OutPoint(Txid(ByteData256(txid_bytes)), utxo.vout));
    }
  }
  if (estimate_fee) *estimate_fee = fee;
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
  if (prefix_list)
    addr_factory = ElementsAddressFactory(net_type, *prefix_list);

  // Collect TxIn and TxOut amounts from Transaction.
  // (If it exists in selected_txin_utxos and the txid matches,
  //  it is treated as a existed UTXO.)
  ConfidentialTransactionContext ctxc(tx_hex);
  std::map<std::string, int64_t> txin_amount_map;
  std::map<std::string, int64_t> tx_amount_map;
  std::vector<std::string> asset_list;
  int32_t fee_index = -1;

  // collect utxo data
  CollectUtxoDataByFundRawTransaction(
      ctxc, selected_txin_utxos, &txin_amount_map, &tx_amount_map, &asset_list,
      &fee_index);

  std::map<std::string, int64_t> target_values;
  for (const auto& target : map_target_value) {
    std::string asset = target.first;
    if (std::find(asset_list.begin(), asset_list.end(), asset) ==
        asset_list.end()) {
      asset_list.push_back(asset);
    }
    target_values.emplace(asset, target.second.GetSatoshiValue());  // copy
  }
  std::map<std::string, int64_t> input_amount_map;
  std::map<std::string, int64_t> input_max_map;
  std::vector<UtxoData> utxodata_list;
  utxodata_list.reserve(utxos.size());
  const auto txin_list = ctxc.GetTxInList();  // txin_utxo_list
  const auto txout_list = ctxc.GetTxOutList();
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
    std::string asset = utxo.asset.GetHex();
    int64_t utxo_amount = utxo.amount.GetSatoshiValue();
    if (input_amount_map.find(asset) == input_amount_map.end()) {
      int64_t amount;
      input_amount_map.emplace(asset, amount);
      input_max_map.emplace(asset, amount);
    }
    input_amount_map[asset] += utxo_amount;
    if (input_max_map[asset] < utxo_amount) input_max_map[asset] = utxo_amount;
  }

  // calculate initial fee.
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
    if (fee_index == -1) {
      // If fee is not registered, add fee.
      ctxc.AddTxOutFee(Amount::CreateBySatoshiAmount(0), fee_asset);
    }
    fee = EstimateFee(
        ctxc.GetHex(), selected_txin_utxos, fee_asset, nullptr, nullptr,
        is_blind_estimate_fee, option.GetEffectiveFeeBaserate(), exponent,
        minimum_bits);
    if (estimate_fee) *estimate_fee = fee;
  }

  // Set the coin selection amount for each asset.
  // The fee will be calculated later.
  std::map<std::string, int64_t> select_require_values;
  for (auto& asset : asset_list) {
    if (target_values.find(asset) == target_values.end()) {
      continue;
    }
    bool is_fee_asset = (asset == fee_asset.GetHex());
    if (use_fee && is_fee_asset) {
      continue;
    }
    int64_t txin_amount = txin_amount_map[asset];
    int64_t tx_amount = tx_amount_map[asset];
    int64_t target_value = target_values[asset];
    int64_t diff_amount = 0;
    bool isForceCheck = false;

    if (txin_amount > tx_amount) {
      diff_amount = txin_amount - tx_amount;
      if (diff_amount < target_value) {
        // If the amount of txin is insufficient, perform Coin Selection.
        target_value -= diff_amount;
      } else {
        target_value = 0;  // If the amount is sufficient, use only txin UTXO.
        isForceCheck = true;
        // CoinSelection needs to be done, so enable the force flag.
      }
    } else if (txin_amount < tx_amount) {
      // Set the amount to be added to txout.
      target_value += tx_amount - txin_amount;
    }
    if ((target_value != 0) || isForceCheck)
      select_require_values[asset] = target_value;
  }
  // execute coinselection
  CoinSelection coin_select;
  std::vector<Utxo> utxo_list = UtxoUtil::ConvertToUtxo(utxodata_list);
  std::map<std::string, int64_t> amount_map;
  std::vector<Utxo> selected_coins;
  Amount utxo_fee;
  selected_coins = coin_select.SelectCoins(
      select_require_values, utxo_list, utxo_filter, option, fee, &amount_map,
      &utxo_fee, nullptr);

  // defined fee_asset_bytes
  std::string fee_asset_str;
  uint8_t fee_asset_bytes[33];
  if (use_fee) {
    fee_asset_str = fee_asset.GetHex();
    memcpy(
        fee_asset_bytes, fee_asset.GetData().GetBytes().data(),
        sizeof(fee_asset_bytes));
  }

  std::map<std::string, int64_t> append_txout_amount_map = amount_map;
  {  // for warning
    auto itr = amount_map.begin();
    while (itr != amount_map.end()) {
      std::string asset = itr->first;
      int64_t txin_amount = txin_amount_map[asset];
      int64_t txout_amount = tx_amount_map[asset];

      if (use_fee && (itr->first == fee_asset_str)) {
        // The fee asset will be calculated later, so it will be excluded from the txout addition target.  // NOLINT
        append_txout_amount_map.erase(itr->first);
      } else {
        append_txout_amount_map[itr->first] =
            itr->second + txin_amount - txout_amount;
      }
      ++itr;
    }
  }

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

      // address check
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

        Amount dust_amount =
            option.GetConfidentialDustFeeAmount(ct_addr.GetUnblindedAddress());
        if ((use_fee) || (itr->second > dust_amount.GetSatoshiValue())) {
          ctxc.AddTxOut(
              ct_addr, Amount(itr->second), ConfidentialAssetId(itr->first));
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
        if ((use_fee) || (itr->second > dust_amount.GetSatoshiValue())) {
          ctxc.AddTxOut(
              address, Amount(itr->second), ConfidentialAssetId(itr->first));
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

  // calculate fee asset
  std::vector<uint8_t> txid_bytes(cfd::core::kByteData256Length);
  if (use_fee) {
    CalculateFeeAndFundTransaction(
        *this, addr_factory, txin_amount_map, tx_amount_map, target_values,
        input_amount_map, input_max_map, selected_coins, utxodata_list,
        fee_asset, selected_txin_utxos, reserve_txout_address, net_type,
        is_blind_estimate_fee, utxo_filter, option, utxo_list, &ctxc,
        append_txout_addresses, estimate_fee);
  }

  for (auto& utxo : selected_coins) {
    if ((!use_fee) ||
        (memcmp(utxo.asset, fee_asset_bytes, sizeof(utxo.asset)) != 0)) {
      memcpy(txid_bytes.data(), utxo.txid, txid_bytes.size());
      ctxc.AddTxIn(OutPoint(Txid(ByteData256(txid_bytes)), utxo.vout));
    }
  }
  return ConfidentialTransactionController(ctxc.GetHex());
}

}  // namespace api
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
