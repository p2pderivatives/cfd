// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_transaction.cpp
 *
 * @brief \~english implementation of Elements Transaction operation related class
 *   \~japanese Elements Transaction操作の関連クラスの実装ファイル
 */
#ifndef CFD_DISABLE_ELEMENTS
#include "cfd/cfd_elements_transaction.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_fee.h"
#include "cfd_transaction_internal.h"  // NOLINT
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlindFactor;
using cfd::core::BlindParameter;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
using cfd::core::CryptoUtil;
using cfd::core::ElementsAddressType;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::IssuanceBlindingKeyPair;
using cfd::core::IssuanceParameter;
using cfd::core::NetType;
using cfd::core::PegoutKeyData;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptElement;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::ScriptWitness;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;
using cfd::core::Txid;
using cfd::core::UnblindParameter;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// Define
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// internal function
// -----------------------------------------------------------------------------
/**
 * @brief Create signature hash with confidential transaction.
 * @param[in] transaction     confidential transaction.
 * @param[in] outpoint        utxo txin & vout.
 * @param[in] utxo            utxo.
 * @param[in] sighash_type    sighash type.
 * @param[in] pubkey          pubkey on pubkey hash.
 * @param[in] redeem_script   redeem script on script hash.
 * @param[in] version         witness version.
 * @return signature hash.
 */
static ByteData256 CreateConfidentialTxSighash(
    const ConfidentialTransactionContext* transaction,
    const OutPoint& outpoint, const UtxoData& utxo,
    const SigHashType& sighash_type, const Pubkey& pubkey,
    const Script& redeem_script, WitnessVersion version) {
  ConfidentialValue value;
  if (utxo.value_commitment.IsEmpty()) {
    value = ConfidentialValue(utxo.amount);
  } else {
    value = utxo.value_commitment;
  }
  ByteData sig;
  if (redeem_script.IsEmpty()) {
    sig = transaction->CreateSignatureHash(
        outpoint, pubkey, sighash_type, value, version);
  } else {
    sig = transaction->CreateSignatureHash(
        outpoint, redeem_script, sighash_type, value, version);
  }
  return ByteData256(sig);
}
/*
  std::function<ByteData256(const Tx*, const OutPoint&,
    const UtxoData&, const SigHashType&, const Pubkey&y,
    const Script&, WitnessVersion)> create_sighash_func;
*/

// -----------------------------------------------------------------------------
// ConfidentialTransactionContext
// -----------------------------------------------------------------------------
ConfidentialTransactionContext::ConfidentialTransactionContext() {}

ConfidentialTransactionContext::ConfidentialTransactionContext(
    uint32_t version, uint32_t locktime)
    : ConfidentialTransaction(version, locktime) {}

ConfidentialTransactionContext::ConfidentialTransactionContext(
    const std::string& tx_hex)
    : ConfidentialTransaction(tx_hex) {}

ConfidentialTransactionContext::ConfidentialTransactionContext(
    const ByteData& byte_data)
    : ConfidentialTransaction(byte_data.GetHex()) {}

ConfidentialTransactionContext::ConfidentialTransactionContext(
    const ConfidentialTransactionContext& context)
    : ConfidentialTransaction(context.GetHex()) {
  utxo_map_ = context.utxo_map_;
  signed_map_ = context.signed_map_;
  verify_map_ = context.verify_map_;
  verify_ignore_map_ = context.verify_ignore_map_;
}

ConfidentialTransactionContext::ConfidentialTransactionContext(
    const ConfidentialTransaction& transaction)
    : ConfidentialTransaction(transaction.GetHex()) {}

ConfidentialTransactionContext& ConfidentialTransactionContext::operator=(
    const ConfidentialTransactionContext& context) & {
  SetFromHex(context.GetHex());
  utxo_map_ = context.utxo_map_;
  signed_map_ = context.signed_map_;
  verify_map_ = context.verify_map_;
  verify_ignore_map_ = context.verify_ignore_map_;
  return *this;
}

uint32_t ConfidentialTransactionContext::GetTxInIndex(
    const OutPoint& outpoint) const {
  return GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
}

uint32_t ConfidentialTransactionContext::AddTxIn(const OutPoint& outpoint) {
  return AddTxIn(
      outpoint.GetTxid(), outpoint.GetVout(), GetDefaultSequence(),
      Script::Empty);
}

uint32_t ConfidentialTransactionContext::GetTxOutIndex(
    const Address& address) const {
  return GetTxOutIndex(address.GetLockingScript());
}

bool ConfidentialTransactionContext::IsFindTxIn(
    const OutPoint& outpoint, uint32_t* index) const {
  static constexpr const char* const kErrorMessage = "Txid is not found.";
  try {
    uint32_t temp_index = GetTxInIndex(outpoint);
    if (index != nullptr) *index = temp_index;
    return true;
  } catch (const CfdException& except) {
    std::string errmsg(except.what());
    if (errmsg == kErrorMessage) {
      return false;
    } else {
      throw except;
    }
  }
}

bool ConfidentialTransactionContext::IsFindTxOut(
    const Script& locking_script, uint32_t* index) const {
  try {
    uint32_t temp_index = GetTxOutIndex(locking_script);
    if (index != nullptr) *index = temp_index;
    return true;
  } catch (const CfdException& except) {
    if (except.GetErrorCode() == CfdError::kCfdIllegalArgumentError) {
      return false;
    } else {
      throw except;
    }
  }
}

bool ConfidentialTransactionContext::IsFindTxOut(
    const Address& address, uint32_t* index) const {
  try {
    uint32_t temp_index = GetTxOutIndex(address);
    if (index != nullptr) *index = temp_index;
    return true;
  } catch (const CfdException& except) {
    if (except.GetErrorCode() == CfdError::kCfdIllegalArgumentError) {
      return false;
    } else {
      throw except;
    }
  }
}

const ConfidentialTxInReference ConfidentialTransactionContext::GetTxIn(
    const OutPoint& outpoint) const {
  uint32_t index = GetTxInIndex(outpoint);
  return GetTxIn(index);
}

Address ConfidentialTransactionContext::GetTxOutAddress(
    uint32_t index, NetType net_type, bool ignore_error) const {
  if (vout_.size() <= index) {
    throw CfdException(
        CfdError::kCfdOutOfRangeError, "vout out_of_range error.");
  }
  try {
    ElementsAddressFactory address_factory(net_type);
    return address_factory.GetAddressByLockingScript(
        vout_[index].GetLockingScript());
  } catch (const CfdException& except) {
    if (!ignore_error) {
      throw except;
    }
    return Address();
  }
}

uint32_t ConfidentialTransactionContext::AddTxOut(
    const Address& address, const Amount& value,
    const ConfidentialAssetId& asset) {
  return AddTxOut(value, asset, address.GetLockingScript());
}

uint32_t ConfidentialTransactionContext::AddTxOut(
    const ElementsConfidentialAddress& address, const Amount& value,
    const ConfidentialAssetId& asset, bool remove_nonce) {
  ByteData nonce;
  if (!remove_nonce) {
    nonce = address.GetConfidentialKey().GetData();
  }
  return AddTxOut(
      value, asset, address.GetLockingScript(), ConfidentialNonce(nonce));
}

uint32_t ConfidentialTransactionContext::GetSizeIgnoreTxIn(
    bool is_blinded, uint32_t* witness_area_size,
    uint32_t* no_witness_area_size, int exponent, int minimum_bits,
    uint32_t asset_count) const {
  uint32_t size = ConfidentialTransaction::kElementsTransactionMinimumSize;
  std::vector<ConfidentialTxOutReference> txouts = GetTxOutList();
  uint32_t temp_asset_count = asset_count;

  // search vin from issue/reissue
  if (temp_asset_count == 0) {
    for (const auto& txin : vin_) {
      if (!txin.GetAssetEntropy().IsEmpty()) {
        ++temp_asset_count;
        if (!txin.GetBlindingNonce().IsEmpty()) {
          // reissuance
        } else {
          ++temp_asset_count;  // issuance
        }
      }
      ++temp_asset_count;
    }
  }

  uint32_t witness_size = 0;
  uint32_t no_witness_size = 0;
  uint32_t temp_witness_size = 0;
  uint32_t temp_no_witness_size = 0;
  for (const auto& txout : txouts) {
    txout.GetSerializeSize(
        is_blinded, &temp_witness_size, &temp_no_witness_size, exponent,
        minimum_bits, nullptr, temp_asset_count);
    witness_size += temp_witness_size;
    no_witness_size += temp_no_witness_size;
  }
  size += no_witness_size;

  if (witness_area_size != nullptr) {
    *witness_area_size += witness_size;
  }
  if (no_witness_area_size != nullptr) {
    *no_witness_area_size += size;
  }
  return size + witness_size;
}

uint32_t ConfidentialTransactionContext::GetVsizeIgnoreTxIn(
    bool is_blinded, int exponent, int minimum_bits,
    uint32_t asset_count) const {
  uint32_t witness_size = 0;
  uint32_t no_witness_size = 0;
  GetSizeIgnoreTxIn(
      is_blinded, &witness_size, &no_witness_size, exponent, minimum_bits,
      asset_count);
  return AbstractTransaction::GetVsizeFromSize(no_witness_size, witness_size);
}

bool ConfidentialTransactionContext::IsFindFeeTxOut(uint32_t* index) const {
  bool is_find = false;
  uint32_t count = 0;
  for (const auto& txout : vout_) {
    if (txout.GetLockingScript().IsEmpty()) {
      if (is_find) {
        throw CfdException(
            CfdError::kCfdIllegalStateError,
            "Invalid fee state. multiple fee exist on txout.");
      }
      is_find = true;
      if (index != nullptr) *index = count;
    }
    ++count;
  }
  return is_find;
}

Amount ConfidentialTransactionContext::GetFeeAmount(
    ConfidentialAssetId* asset) const {
  Amount result;
  uint32_t index = 0;
  if (IsFindFeeTxOut(&index)) {
    const auto& txout = vout_[index];
    result = txout.GetConfidentialValue().GetAmount();
    if (asset != nullptr) *asset = txout.GetAsset();
  }
  return result;
}

uint32_t ConfidentialTransactionContext::UpdateFeeAmount(
    const Amount& value, const ConfidentialAssetId& asset) {
  Amount result;
  uint32_t index = 0;
  if (IsFindFeeTxOut(&index)) {
    SetTxOutCommitment(
        index, asset, ConfidentialValue(value), ConfidentialNonce(),
        ByteData(), ByteData());
  } else {
    index = AddTxOutFee(value, asset);
  }
  return index;
}

Amount ConfidentialTransactionContext::CalculateSimpleFee(
    bool append_feature_signed_size, bool append_signed_witness) const {
  static constexpr uint32_t kP2wpkhWitnessSize = 72 + 33 + 3;
  // 簡易計算
  uint32_t size = GetTotalSize();
  uint32_t vsize = GetVsize();
  uint32_t rate = FeeCalculator::kRelayMinimumFee;
  if (append_feature_signed_size) {
    uint32_t weight = GetWeight();
    uint32_t count = GetTxInCount();
    uint32_t add_size;
    add_size = kP2wpkhWitnessSize * count;
    if (!append_signed_witness) {
      // no segwit (x4)
      add_size *= 4;
    }
    size += add_size;
    weight += add_size;
    vsize = (weight + 3) / 4;  // wally_tx_vsize_from_weight
  }
  return FeeCalculator::CalculateFee(size, vsize, rate);
}

uint32_t ConfidentialTransactionContext::AddPegoutTxOut(
    const Amount& value, const ConfidentialAssetId& asset,
    const BlockHash& genesisblock_hash, const Address& btc_address,
    NetType bitcoin_net_type, const Pubkey& online_pubkey,
    const Privkey& master_online_key, const std::string& btc_descriptor,
    uint32_t bip32_counter, const ByteData& whitelist,
    NetType elements_net_type, Address* btc_derive_address) {
  Script script;
  PegoutKeyData key_data;
  if (online_pubkey.IsValid() && master_online_key.IsValid()) {
    // generate pubkey and whitelistproof
    Address derive_addr;
    key_data = ConfidentialTransaction::GetPegoutPubkeyData(
        online_pubkey, master_online_key, btc_descriptor, bip32_counter,
        whitelist, bitcoin_net_type, ByteData(), elements_net_type,
        &derive_addr);
    if (!derive_addr.GetAddress().empty()) {
      script = derive_addr.GetLockingScript();
      if (btc_derive_address != nullptr) {
        *btc_derive_address = derive_addr;
      }
    }
  } else {
    script = btc_address.GetLockingScript();
  }

  Script locking_script = ScriptUtil::CreatePegoutLogkingScript(
      genesisblock_hash, script, key_data.btc_pubkey_bytes,
      key_data.whitelist_proof);
  return AddTxOut(value, asset, locking_script);
}

void ConfidentialTransactionContext::AddPeginTxIn(  // AddPeginWitness
    const OutPoint& outpoint, const Amount& amount,
    const ConfidentialAssetId& asset_id,
    const BlockHash& mainchain_genesis_block_hash, const Script& claim_script,
    const ByteData& mainchain_pegin_transaction,
    const ByteData& tx_out_proof) {
  uint32_t index = 0;
  if (!IsFindTxIn(outpoint, &index)) {
    index = AddTxIn(outpoint);
  }
  AddPeginWitnessStack(index, amount.GetByteData());
  AddPeginWitnessStack(index, asset_id.GetUnblindedData());
  AddPeginWitnessStack(index, mainchain_genesis_block_hash.GetData());
  AddPeginWitnessStack(index, claim_script.GetData());
  AddPeginWitnessStack(index, mainchain_pegin_transaction);
  AddPeginWitnessStack(index, tx_out_proof);
}

IssuanceParameter ConfidentialTransactionContext::SetAssetIssuance(
    const OutPoint& outpoint, const Amount& issue_amount,
    const IssuanceOutputParameter& issue_output, const Amount& token_amount,
    const IssuanceOutputParameter& token_output, bool is_blind,
    const ByteData256& contract_hash) {
  std::vector<IssuanceOutputParameter> issue_output_list;
  std::vector<IssuanceOutputParameter> token_output_list;
  issue_output_list.push_back(issue_output);
  token_output_list.push_back(token_output);
  return SetAssetIssuance(
      outpoint, issue_amount, issue_output_list, token_amount,
      token_output_list, is_blind, contract_hash);
}

IssuanceParameter ConfidentialTransactionContext::SetAssetIssuance(
    const OutPoint& outpoint, const Amount& issue_amount,
    const std::vector<IssuanceOutputParameter>& issue_output_list,
    const Amount& token_amount,
    const std::vector<IssuanceOutputParameter>& token_output_list,
    bool is_blind, const ByteData256& contract_hash) {
  std::vector<Amount> asset_output_amount_list;
  std::vector<Script> asset_locking_script_list;
  std::vector<ConfidentialNonce> asset_nonce_list;
  std::vector<Amount> token_output_amount_list;
  std::vector<Script> token_locking_script_list;
  std::vector<ConfidentialNonce> token_nonce_list;

  static auto set_func = [](const std::vector<IssuanceOutputParameter>& list,
                            std::vector<Amount>* amount_list,
                            std::vector<Script>* locking_script_list,
                            std::vector<ConfidentialNonce>* nonce_list) {
    for (const auto& data : list) {
      ConfidentialNonce nonce;
      Script script;
      if (!data.direct_locking_script.IsEmpty()) {
        script = data.direct_locking_script;
      } else {
        Address addr;
        if (data.confidential_address.GetAddress().empty()) {
          addr = data.address;
        } else {
          addr = data.confidential_address.GetUnblindedAddress();
          if (!data.is_remove_nonce) {
            nonce = ConfidentialNonce(
                data.confidential_address.GetConfidentialKey());
          }
        }
        script = addr.GetLockingScript();
      }
      amount_list->push_back(data.amount);
      locking_script_list->push_back(script);
      nonce_list->push_back(nonce);
    }
    return;
  };

  uint32_t txin_index = GetTxInIndex(outpoint);
  set_func(
      issue_output_list, &asset_output_amount_list, &asset_locking_script_list,
      &asset_nonce_list);
  set_func(
      token_output_list, &token_output_amount_list, &token_locking_script_list,
      &token_nonce_list);

  return SetAssetIssuance(
      txin_index, issue_amount, asset_output_amount_list,
      asset_locking_script_list, asset_nonce_list, token_amount,
      token_output_amount_list, token_locking_script_list, token_nonce_list,
      is_blind, contract_hash);
}

IssuanceParameter ConfidentialTransactionContext::SetAssetReissuance(
    const OutPoint& outpoint, const Amount& amount,
    const IssuanceOutputParameter& issue_output,
    const BlindFactor& blind_factor, const BlindFactor& entropy) {
  std::vector<IssuanceOutputParameter> issue_output_list;
  issue_output_list.push_back(issue_output);
  return SetAssetReissuance(
      outpoint, amount, issue_output_list, blind_factor, entropy);
}

IssuanceParameter ConfidentialTransactionContext::SetAssetReissuance(
    const OutPoint& outpoint, const Amount& amount,
    const std::vector<IssuanceOutputParameter>& issue_output_list,
    const BlindFactor& blind_factor, const BlindFactor& entropy) {
  std::vector<Amount> asset_output_amount_list;
  std::vector<Script> asset_locking_script_list;
  std::vector<ConfidentialNonce> asset_nonce_list;

  static auto set_func = [](const std::vector<IssuanceOutputParameter>& list,
                            std::vector<Amount>* amount_list,
                            std::vector<Script>* locking_script_list,
                            std::vector<ConfidentialNonce>* nonce_list) {
    for (const auto& data : list) {
      ConfidentialNonce nonce;
      Script script;
      if (!data.direct_locking_script.IsEmpty()) {
        script = data.direct_locking_script;
      } else {
        Address addr;
        if (data.confidential_address.GetAddress().empty()) {
          addr = data.address;
        } else {
          addr = data.confidential_address.GetUnblindedAddress();
          if (!data.is_remove_nonce) {
            nonce = ConfidentialNonce(
                data.confidential_address.GetConfidentialKey());
          }
        }
        script = addr.GetLockingScript();
      }
      amount_list->push_back(data.amount);
      locking_script_list->push_back(script);
      nonce_list->push_back(nonce);
    }
    return;
  };

  uint32_t txin_index = GetTxInIndex(outpoint);
  set_func(
      issue_output_list, &asset_output_amount_list, &asset_locking_script_list,
      &asset_nonce_list);

  return SetAssetReissuance(
      txin_index, amount, asset_output_amount_list, asset_locking_script_list,
      asset_nonce_list, blind_factor, entropy);
}

void ConfidentialTransactionContext::BlindTransaction(
    const std::map<OutPoint, BlindParameter>& utxo_info_map,
    const std::map<OutPoint, IssuanceBlindingKeyPair>& issuance_key_map,
    const std::vector<ElementsConfidentialAddress>& confidential_address_list,
    int64_t minimum_range_value, int exponent, int minimum_bits) {
  std::vector<ConfidentialKeyBlindParameter> confidential_key_list;
  BlindTransactionWithDirectKey(
      utxo_info_map, issuance_key_map, confidential_address_list,
      confidential_key_list, minimum_range_value, exponent, minimum_bits);
}

void ConfidentialTransactionContext::BlindTransactionWithDirectKey(
    const std::map<OutPoint, BlindParameter>& utxo_info_map,
    const std::map<OutPoint, IssuanceBlindingKeyPair>& issuance_key_map,
    const std::vector<ElementsConfidentialAddress>& confidential_address_list,
    const std::vector<ConfidentialKeyBlindParameter>& confidential_key_list,
    int64_t minimum_range_value, int exponent, int minimum_bits) {
  uint32_t index = 0;
  std::vector<BlindParameter> txin_info_list;
  if (!utxo_info_map.empty()) {
    txin_info_list.resize(vin_.size());
    for (const auto& utxo_info : utxo_info_map) {
      index = GetTxInIndex(utxo_info.first);
      txin_info_list[index] = utxo_info.second;
    }
  }

  std::vector<IssuanceBlindingKeyPair> issuance_blinding_keys;
  if (!issuance_key_map.empty()) {
    issuance_blinding_keys.resize(vin_.size());
    for (const auto& key_info : issuance_key_map) {
      index = GetTxInIndex(key_info.first);
      issuance_blinding_keys[index] = key_info.second;
    }
  }

  std::vector<Pubkey> confidential_keys(vout_.size());
  std::map<std::string, Pubkey> ct_map;
  if (!confidential_address_list.empty()) {
    for (const auto& ct_addr : confidential_address_list) {
      ct_map.emplace(
          ct_addr.GetUnblindedAddress().GetLockingScript().GetHex(),
          ct_addr.GetConfidentialKey());
    }
  }
  if (!confidential_key_list.empty()) {
    for (const auto& data : confidential_key_list) {
      if (data.index < static_cast<uint32_t>(vout_.size())) {
        confidential_keys[data.index] = data.confidential_key;
      }
    }
  }
  for (index = 0; index < static_cast<uint32_t>(vout_.size()); ++index) {
    const auto& txout = vout_[index];
    std::string script = txout.GetLockingScript().GetHex();
    if (confidential_keys[index].IsValid()) {
      // do nothing
    } else if (ct_map.find(script) != std::end(ct_map)) {
      confidential_keys[index] = ct_map.find(script)->second;
    } else {
      ByteData nonce = txout.GetNonce().GetData();
      if (Pubkey::IsValid(nonce)) {
        confidential_keys[index] = Pubkey(nonce);
      }
    }
  }

  BlindTransaction(
      txin_info_list, issuance_blinding_keys, confidential_keys,
      minimum_range_value, exponent, minimum_bits);
}

std::vector<UnblindParameter> ConfidentialTransactionContext::UnblindIssuance(
    const OutPoint& outpoint,
    const IssuanceBlindingKeyPair& asset_blinding_key) {
  return UnblindTxIn(
      GetTxInIndex(outpoint), asset_blinding_key.asset_key,
      asset_blinding_key.token_key);
}

void ConfidentialTransactionContext::AddInput(const UtxoData& utxo) {
  AddInput(utxo, GetDefaultSequence());
}

void ConfidentialTransactionContext::AddInput(
    const UtxoData& utxo, uint32_t sequence) {
  if (!utxo.txid.IsValid()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to AddInput. invalid utxo txid.");
  }

  UtxoData dest;
  Utxo temp;
  memset(&temp, 0, sizeof(temp));
  UtxoUtil::ConvertToUtxo(utxo, &temp, &dest);

  AddTxIn(utxo.txid, utxo.vout, sequence, Script::Empty);
  utxo_map_.emplace(OutPoint(utxo.txid, utxo.vout), dest);
}

void ConfidentialTransactionContext::AddInputs(
    const std::vector<UtxoData>& utxos) {
  for (const auto& utxo : utxos) {
    AddInput(utxo, GetDefaultSequence());
  }
}

void ConfidentialTransactionContext::CollectInputUtxo(
    const std::vector<UtxoData>& utxos) {
  if ((!utxos.empty()) && (utxo_map_.size() != GetTxInCount())) {
    for (const auto& txin_ref : vin_) {
      const Txid& txid = txin_ref.GetTxid();
      uint32_t vout = txin_ref.GetVout();

      OutPoint outpoint(txid, vout);
      if (utxo_map_.find(outpoint) == std::end(utxo_map_)) {
        for (const auto& utxo : utxos) {
          if ((utxo.vout == vout) && utxo.txid.Equals(txid)) {
            utxo_map_.emplace(outpoint, utxo);
            break;
          }
        }
      }
    }
  }
}

void ConfidentialTransactionContext::Blind(
    const std::vector<ElementsConfidentialAddress>* confidential_addresses,
    int64_t minimum_range_value, int exponent, int minimum_bits) {
  BlindIssuance(
      std::map<OutPoint, IssuanceBlindingKeyPair>(), confidential_addresses,
      minimum_range_value, exponent, minimum_bits);
}

void ConfidentialTransactionContext::BlindIssuance(
    const std::map<OutPoint, IssuanceBlindingKeyPair>& issuance_key_map,
    const std::vector<ElementsConfidentialAddress>* confidential_addresses,
    int64_t minimum_range_value, int exponent, int minimum_bits) {
  std::map<OutPoint, BlindParameter> utxo_info_map;
  for (const auto& txin_ref : vin_) {
    OutPoint outpoint = txin_ref.GetOutPoint();
    if (utxo_map_.find(outpoint) == std::end(utxo_map_)) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Utxo is not found. blind fail.");
    }
    UtxoData utxo = utxo_map_.find(outpoint)->second;
    BlindParameter param;
    param.asset = utxo.asset;
    param.abf = utxo.asset_blind_factor;
    param.vbf = utxo.amount_blind_factor;
    param.value = ConfidentialValue(utxo.amount);
    utxo_info_map.emplace(outpoint, param);
  }
  // confidentialkey is already set to nonce.

  std::vector<ElementsConfidentialAddress> confidential_addrs;
  if (confidential_addresses != nullptr) {
    // set updating keys.
    confidential_addrs = *confidential_addresses;
  }
  BlindTransaction(
      utxo_info_map, issuance_key_map, confidential_addrs, minimum_range_value,
      exponent, minimum_bits);
}

void ConfidentialTransactionContext::SignWithKey(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
    SigHashType sighash_type, bool has_grind_r) {
  if (utxo_map_.find(outpoint) == std::end(utxo_map_)) {
    throw CfdException(
        CfdError::kCfdIllegalStateError, "Utxo is not found. sign fail.");
  }
  UtxoData utxo = utxo_map_.find(outpoint)->second;

  if (utxo.amount_blind_factor.IsEmpty() &&
      (!utxo.value_commitment.HasBlinding())) {
    SignWithPrivkeySimple(
        outpoint, pubkey, privkey, sighash_type, utxo.amount,
        utxo.address_type, has_grind_r);
  } else {
    SignWithPrivkeySimple(
        outpoint, pubkey, privkey, sighash_type, utxo.value_commitment,
        utxo.address_type, has_grind_r);
  }
}

void ConfidentialTransactionContext::IgnoreVerify(const OutPoint& outpoint) {
  GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
  verify_ignore_map_.emplace(outpoint);
}

void ConfidentialTransactionContext::Verify() {
  for (const auto& vin : vin_) {
    OutPoint outpoint = vin.GetOutPoint();
    if (verify_ignore_map_.find(outpoint) == std::end(verify_ignore_map_)) {
      Verify(outpoint);
    }
  }
}

void ConfidentialTransactionContext::Verify(const OutPoint& outpoint) {
  if (utxo_map_.find(outpoint) == std::end(utxo_map_)) {
    throw CfdException(
        CfdError::kCfdIllegalStateError, "Utxo is not found. verify fail.");
  }
  const auto& utxo = utxo_map_.find(outpoint)->second;
  const auto& txin = vin_[GetTxInIndex(outpoint)];

  TransactionContextUtil::Verify<ConfidentialTransactionContext>(
      this, outpoint, utxo, &txin, CreateConfidentialTxSighash);
  verify_map_.emplace(outpoint);
}

ByteData ConfidentialTransactionContext::Finalize() {
  Verify();
  return AbstractTransaction::GetData();
}

#if 0
// priority: low
void ConfidentialTransactionContext::ClearSign() { return; }

// priority: low
void ConfidentialTransactionContext::ClearSign(const OutPoint& outpoint) {
  return;
}
#endif

ByteData ConfidentialTransactionContext::CreateSignatureHash(
    const OutPoint& outpoint, const Pubkey& pubkey, SigHashType sighash_type,
    const Amount& value, WitnessVersion version) const {
  return CreateSignatureHash(
      outpoint, pubkey, sighash_type, ConfidentialValue(value), version);
}

ByteData ConfidentialTransactionContext::CreateSignatureHash(
    const OutPoint& outpoint, const Script& redeem_script,
    SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  return CreateSignatureHash(
      outpoint, redeem_script, sighash_type, ConfidentialValue(value),
      version);
}

ByteData ConfidentialTransactionContext::CreateSignatureHash(
    const OutPoint& outpoint, const Pubkey& pubkey, SigHashType sighash_type,
    const ConfidentialValue& value, WitnessVersion version) const {
  Script script = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  ByteData256 sighash = GetElementsSignatureHash(
      GetTxInIndex(outpoint), script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

ByteData ConfidentialTransactionContext::CreateSignatureHash(
    const OutPoint& outpoint, const Script& redeem_script,
    SigHashType sighash_type, const ConfidentialValue& value,
    WitnessVersion version) const {
  // TODO(soejima): OP_CODESEPARATOR存在時、Scriptの分割が必要。
  // TODO(k-matsuzawa): 現状は利用側で分割し、適用する箇所だけ指定してもらう。
  ByteData256 sighash = GetElementsSignatureHash(
      GetTxInIndex(outpoint), redeem_script.GetData(), sighash_type, value,
      version);
  return ByteData(sighash.GetBytes());
}

void ConfidentialTransactionContext::SignWithPrivkeySimple(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
    SigHashType sighash_type, const Amount& value, AddressType address_type,
    bool has_grind_r) {
  SignWithPrivkeySimple(
      outpoint, pubkey, privkey, sighash_type, ConfidentialValue(value),
      address_type, has_grind_r);
}

void ConfidentialTransactionContext::SignWithPrivkeySimple(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
    SigHashType sighash_type, const ConfidentialValue& value,
    AddressType address_type, bool has_grind_r) {
  WitnessVersion version = TransactionContextUtil::CheckSignWithPrivkeySimple(
      outpoint, pubkey, privkey, address_type);

  ByteData sighash =
      CreateSignatureHash(outpoint, pubkey, sighash_type, value, version);
  ByteData signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash), privkey, has_grind_r);
  SignParameter sign(signature, true, sighash_type);

  AddPubkeyHashSign(outpoint, sign, pubkey, address_type);
}

void ConfidentialTransactionContext::AddPubkeyHashSign(
    const OutPoint& outpoint, const SignParameter& signature,
    const Pubkey& pubkey, AddressType address_type) {
  TransactionContextUtil::AddPubkeyHashSign<ConfidentialTransactionContext>(
      this, outpoint, signature, pubkey, address_type);
  signed_map_.emplace(outpoint, signature.GetSigHashType());
}

void ConfidentialTransactionContext::AddScriptHashSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
    const Script& redeem_script, AddressType address_type,
    bool is_multisig_script) {
  TransactionContextUtil::AddScriptHashSign<ConfidentialTransactionContext>(
      this, outpoint, signatures, redeem_script, address_type,
      is_multisig_script);

  // TODO(k-matsuzawa): consider to multi-signature.
  // signed_map_.emplace(outpoint, signature.GetSigHashType());
}

void ConfidentialTransactionContext::AddMultisigSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
    const Script& redeem_script, AddressType address_type) {
  std::vector<SignParameter> sign_list =
      TransactionContext::CheckMultisig(signatures, redeem_script);
  AddScriptHashSign(outpoint, sign_list, redeem_script, address_type, true);
}

void ConfidentialTransactionContext::AddSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& sign_params,
    bool insert_witness, bool clear_stack) {
  return TransactionContextUtil::AddSign<ConfidentialTransactionContext>(
      this, outpoint, sign_params, insert_witness, clear_stack);
}

bool ConfidentialTransactionContext::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const OutPoint& outpoint,
    SigHashType sighash_type, const ConfidentialValue& value,
    WitnessVersion version) const {
  auto sighash =
      CreateSignatureHash(outpoint, pubkey, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

bool ConfidentialTransactionContext::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const OutPoint& outpoint,
    const Script& script, SigHashType sighash_type,
    const ConfidentialValue& value, WitnessVersion version) const {
  auto sighash =
      CreateSignatureHash(outpoint, script, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

uint32_t ConfidentialTransactionContext::GetDefaultSequence() const {
  if (GetLockTime() == 0) {
    return kSequenceDisableLockTime;
  } else {
    return kSequenceEnableLockTimeMax;
  }
}

uint32_t ConfidentialTransactionContext::GetLockTimeDisabledSequence() {
  return kSequenceDisableLockTime;
}

void ConfidentialTransactionContext::CallbackStateChange(uint32_t type) {
  cfd::core::logger::trace(
      CFD_LOG_SOURCE, "CallbackStateChange type::{}", type);
  verify_map_.clear();
}

// -----------------------------------------------------------------------------
// ConfidentialTransactionController
// -----------------------------------------------------------------------------
ConfidentialTransactionController::ConfidentialTransactionController(
    uint32_t version, uint32_t locktime)
    : transaction_(version, locktime) {
  tx_address_ = &transaction_;
}

ConfidentialTransactionController::ConfidentialTransactionController(
    const std::string& tx_hex)
    : transaction_(tx_hex) {
  tx_address_ = &transaction_;
}

ConfidentialTransactionController::ConfidentialTransactionController(
    const ConfidentialTransactionController& transaction)
    : ConfidentialTransactionController(transaction.GetHex()) {
  // do nothing
}

ConfidentialTransactionController&
ConfidentialTransactionController::operator=(
    const ConfidentialTransactionController& transaction) & {
  transaction_ = transaction.transaction_;
  return *this;
}

const ConfidentialTxInReference ConfidentialTransactionController::AddTxIn(
    const Txid& txid, uint32_t vout) {
  return AddTxIn(txid, vout, GetDefaultSequence());
}

const ConfidentialTxInReference ConfidentialTransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, uint32_t sequence) {
  uint32_t index = transaction_.AddTxIn(txid, vout, sequence);
  return transaction_.GetTxIn(index);
}

const ConfidentialTxInReference ConfidentialTransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Script& redeem_script) {
  return AddTxIn(txid, vout, redeem_script, GetDefaultSequence());
}

const ConfidentialTxInReference ConfidentialTransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Script& redeem_script,
    uint32_t sequence) {
  uint32_t index = transaction_.AddTxIn(txid, vout, sequence, redeem_script);
  return transaction_.GetTxIn(index);
}

const ConfidentialTxInReference ConfidentialTransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey) {
  return AddTxIn(txid, vout, pubkey, GetDefaultSequence());
}

const ConfidentialTxInReference ConfidentialTransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey, uint32_t sequence) {
  uint32_t index = transaction_.AddTxIn(
      txid, vout, sequence, ScriptBuilder().AppendData(pubkey).Build());
  return transaction_.GetTxIn(index);
}

const ConfidentialTxInReference ConfidentialTransactionController::GetTxIn(
    const Txid& txid, uint32_t vout) const {
  uint32_t index = transaction_.GetTxInIndex(txid, vout);
  return transaction_.GetTxIn(index);
}

const ConfidentialTxInReference ConfidentialTransactionController::RemoveTxIn(
    const Txid& txid, uint32_t vout) {
  uint32_t index = transaction_.GetTxInIndex(txid, vout);
  ConfidentialTxInReference ref = transaction_.GetTxIn(index);
  transaction_.RemoveTxIn(index);
  return ref;
}

const ConfidentialTxOutReference ConfidentialTransactionController::AddTxOut(
    const Address& address, const Amount& value,
    const ConfidentialAssetId& asset) {
  const ByteData hash_data = address.GetHash();
  Script locking_script = address.GetLockingScript();

  return AddTxOut(locking_script, value, asset, ConfidentialNonce());
}

const ConfidentialTxOutReference ConfidentialTransactionController::AddTxOut(
    const ElementsConfidentialAddress& address, const Amount& value,
    const ConfidentialAssetId& asset, bool remove_nonce) {
  const ByteData hash_data = address.GetHash();
  Script locking_script = address.GetLockingScript();

  ByteData nonce;
  if (!remove_nonce) {
    ElementsConfidentialAddress confidential_addr(address.GetAddress());
    nonce = confidential_addr.GetConfidentialKey().GetData();
  }
  return AddTxOut(locking_script, value, asset, ConfidentialNonce(nonce));
}

const ConfidentialTxOutReference ConfidentialTransactionController::AddTxOut(
    const Script& locking_script, const Amount& value,
    const ConfidentialAssetId& asset) {
  return AddTxOut(locking_script, value, asset, ConfidentialNonce());
}

const ConfidentialTxOutReference ConfidentialTransactionController::AddTxOut(
    const Script& locking_script, const Amount& value,
    const ConfidentialAssetId& asset, const ConfidentialNonce& nonce) {
  uint32_t index = transaction_.AddTxOut(value, asset, locking_script, nonce);
  return transaction_.GetTxOut(index);
}

const ConfidentialTxOutReference
ConfidentialTransactionController::AddPegoutTxOut(
    const Amount& value, const ConfidentialAssetId& asset,
    const BlockHash& genesisblock_hash, const Address& btc_address,
    NetType net_type, const Pubkey& online_pubkey,
    const Privkey& master_online_key, const std::string& btc_descriptor,
    uint32_t bip32_counter, const ByteData& whitelist,
    NetType elements_net_type, Address* btc_derive_address) {
  Script script;
  PegoutKeyData key_data;
  if (online_pubkey.IsValid() && !master_online_key.IsInvalid()) {
    // generate pubkey and whitelistproof
    Address derive_addr;
    key_data = ConfidentialTransaction::GetPegoutPubkeyData(
        online_pubkey, master_online_key, btc_descriptor, bip32_counter,
        whitelist, net_type, ByteData(), elements_net_type, &derive_addr);
    if (!derive_addr.GetAddress().empty()) {
      script = derive_addr.GetLockingScript();
      if (btc_derive_address != nullptr) {
        *btc_derive_address = derive_addr;
      }
    }
  } else {
    script = btc_address.GetLockingScript();
  }

  Script locking_script = ScriptUtil::CreatePegoutLogkingScript(
      genesisblock_hash, script, key_data.btc_pubkey_bytes,
      key_data.whitelist_proof);

  return AddTxOut(locking_script, value, asset);
}

const ConfidentialTxOutReference
ConfidentialTransactionController::AddTxOutFee(
    const Amount& value, const ConfidentialAssetId& asset) {
  uint32_t index = transaction_.AddTxOutFee(value, asset);
  return transaction_.GetTxOut(index);
}

const ConfidentialTxOutReference
ConfidentialTransactionController::UpdateTxOutFeeAmount(
    uint32_t index, const Amount& value, const ConfidentialAssetId& asset) {
  ConfidentialTxOutReference ref = transaction_.GetTxOut(index);
  if (!ref.GetLockingScript().IsEmpty()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "target is not fee txout.");
  }
  transaction_.SetTxOutCommitment(
      index, asset, ConfidentialValue(value), ConfidentialNonce(), ByteData(),
      ByteData());
  return transaction_.GetTxOut(index);
}

const ConfidentialTxOutReference
ConfidentialTransactionController::RemoveTxOut(uint32_t index) {
  ConfidentialTxOutReference ref = transaction_.GetTxOut(index);
  transaction_.RemoveTxOut(index);
  return ref;
}

void ConfidentialTransactionController::InsertUnlockingScript(
    const Txid& txid, uint32_t vout,
    const std::vector<ByteData>& unlocking_scripts) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  Script script = transaction_.GetTxIn(txin_index).GetUnlockingScript();
  if (script.IsEmpty()) {
    transaction_.SetUnlockingScript(txin_index, unlocking_scripts);
  } else {
    ScriptBuilder builder;
    for (const auto& element : script.GetElementList()) {
      builder.AppendElement(element);
    }
    for (const ByteData& data : unlocking_scripts) {
      builder.AppendData(data);
    }
    transaction_.SetUnlockingScript(txin_index, builder.Build());
  }
}

void ConfidentialTransactionController::SetUnlockingScript(
    const Txid& txid, uint32_t vout, const Script& unlocking_script) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.SetUnlockingScript(txin_index, unlocking_script);
}

void ConfidentialTransactionController::SetUnlockingScript(
    const Txid& txid, uint32_t vout,
    const std::vector<ByteData>& unlocking_scripts) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.SetUnlockingScript(txin_index, unlocking_scripts);
}

void ConfidentialTransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout,
    const std::vector<ByteData>& witness_datas) {
  if (witness_datas.empty()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "witness_datas empty.");
  }
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);

  for (const ByteData& witness_data : witness_datas) {
    transaction_.AddScriptWitnessStack(txin_index, witness_data);
  }
}

void ConfidentialTransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout,
    const std::string& signed_signature_hash) {
  std::vector<std::string> signed_signature_hashes(1);
  signed_signature_hashes[0] = signed_signature_hash;
  AddWitnessStack(txid, vout, signed_signature_hashes);
}

void ConfidentialTransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout,
    const std::vector<std::string>& signed_signature_hashes) {
  if (signed_signature_hashes.empty()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "signed signature empty.");
  }
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);

  // 格納
  // append to witness stack
  for (const std::string& sig_hash : signed_signature_hashes) {
    const ByteData& byte_data = ByteData(sig_hash);
    transaction_.AddScriptWitnessStack(txin_index, byte_data);
  }
}

void ConfidentialTransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout, const std::string& signed_signature_hash,
    const Pubkey& pubkey) {
  std::vector<std::string> signed_signature_hashes(2);
  signed_signature_hashes[0] = signed_signature_hash;
  signed_signature_hashes[1] = pubkey.GetHex();
  AddWitnessStack(txid, vout, signed_signature_hashes);
}

void ConfidentialTransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout, const std::string& signed_signature_hash,
    const Script& redeem_script) {
  std::vector<std::string> signed_signature_hashes(1);
  signed_signature_hashes[0] = signed_signature_hash;
  AddWitnessStack(txid, vout, signed_signature_hashes, redeem_script);
}

void ConfidentialTransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout,
    const std::vector<std::string>& signed_signature_hashes,
    const Script& redeem_script) {
  std::vector<std::string> list;
  std::copy(
      signed_signature_hashes.begin(), signed_signature_hashes.end(),
      std::back_inserter(list));
  list.push_back(redeem_script.GetData().GetHex());
  AddWitnessStack(txid, vout, list);
}

void ConfidentialTransactionController::SetWitnessStack(
    const Txid& txid, uint32_t vout, uint32_t witness_index,
    const ByteData& witness_stack) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.SetScriptWitnessStack(txin_index, witness_index, witness_stack);
}

void ConfidentialTransactionController::SetWitnessStack(
    const Txid& txid, uint32_t vout, uint32_t witness_index,
    const std::string& hex_string) {
  SetWitnessStack(txid, vout, witness_index, ByteData(hex_string));
}

void ConfidentialTransactionController::RemoveWitnessStackAll(
    const Txid& txid, uint32_t vout) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.RemoveScriptWitnessStackAll(txin_index);
}

uint32_t ConfidentialTransactionController::GetWitnessStackNum(
    const Txid& txid, uint32_t vout) const {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  return transaction_.GetScriptWitnessStackNum(txin_index);
}

void ConfidentialTransactionController::AddPeginWitness(
    const Txid& txid, uint32_t vout, const Amount& amount,
    const ConfidentialAssetId& asset_id,
    const BlockHash& mainchain_genesis_block_hash, const Script& claim_script,
    const ByteData& mainchain_pegin_transaction,
    const ByteData& tx_out_proof) {
  std::vector<ByteData> witness_datas;
  witness_datas.push_back(amount.GetByteData());
  witness_datas.push_back(asset_id.GetUnblindedData());
  witness_datas.push_back(mainchain_genesis_block_hash.GetData());
  witness_datas.push_back(claim_script.GetData());
  witness_datas.push_back(mainchain_pegin_transaction);
  witness_datas.push_back(tx_out_proof);

  AddPeginWitness(txid, vout, witness_datas);
}

void ConfidentialTransactionController::AddPeginWitness(
    const Txid& txid, uint32_t vout,
    const std::vector<ByteData>& witness_datas) {
  if (witness_datas.empty()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Add empty datas to peg-in Witness");
  }
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);

  for (const ByteData& witness_data : witness_datas) {
    transaction_.AddPeginWitnessStack(txin_index, witness_data);
  }
}

void ConfidentialTransactionController::RemovePeginWitnessAll(
    const Txid& txid, uint32_t vout) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.RemovePeginWitnessStackAll(txin_index);
}

const ConfidentialTransaction&
ConfidentialTransactionController::GetTransaction() const {
  return transaction_;
}

uint32_t ConfidentialTransactionController::GetSizeIgnoreTxIn(
    bool is_blinded, uint32_t* witness_area_size,
    uint32_t* no_witness_area_size, int exponent, int minimum_bits) const {
  uint32_t size = ConfidentialTransaction::kElementsTransactionMinimumSize;
  std::vector<ConfidentialTxOutReference> txouts = transaction_.GetTxOutList();
  std::vector<ConfidentialTxInReference> txins = transaction_.GetTxInList();

  uint32_t temp_asset_count = 0;

  // search vin from issue/reissue
  for (const auto& txin : txins) {
    if (!txin.GetAssetEntropy().IsEmpty()) {
      ++temp_asset_count;
      if (!txin.GetBlindingNonce().IsEmpty()) {
        // reissuance
      } else {
        ++temp_asset_count;  // issuance
      }
    }
    ++temp_asset_count;
  }

  uint32_t witness_size = 0;
  uint32_t no_witness_size = 0;
  uint32_t temp_witness_size = 0;
  uint32_t temp_no_witness_size = 0;
  for (const auto& txout : txouts) {
    txout.GetSerializeSize(
        is_blinded, &temp_witness_size, &temp_no_witness_size, exponent,
        minimum_bits, nullptr, temp_asset_count);
    witness_size += temp_witness_size;
    no_witness_size += temp_no_witness_size;
  }
  size += no_witness_size;

  if (witness_area_size != nullptr) {
    *witness_area_size += witness_size;
  }
  if (no_witness_area_size != nullptr) {
    *no_witness_area_size += size;
  }
  return size + witness_size;
}

uint32_t ConfidentialTransactionController::GetVsizeIgnoreTxIn(
    bool is_blinded, int exponent, int minimum_bits) const {
  uint32_t witness_size = 0;
  uint32_t no_witness_size = 0;
  GetSizeIgnoreTxIn(
      is_blinded, &witness_size, &no_witness_size, exponent, minimum_bits);
  return AbstractTransaction::GetVsizeFromSize(no_witness_size, witness_size);
}

IssuanceParameter ConfidentialTransactionController::SetAssetIssuance(
    const Txid& txid, uint32_t vout, const Amount& asset_amount,
    const Script& asset_locking_script, const ByteData& asset_nonce,
    const Amount& token_amount, const Script& token_locking_script,
    const ByteData& token_nonce, bool is_blind,
    const ByteData256& contract_hash, bool is_random_sort,
    bool is_remove_nonce) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);

  ConfidentialNonce confidential_asset_nonce;
  ConfidentialNonce confidential_token_nonce;
  if (!is_remove_nonce) {
    confidential_asset_nonce = ConfidentialNonce(asset_nonce);
    confidential_token_nonce = ConfidentialNonce(token_nonce);
  }

  IssuanceParameter param;
  param = transaction_.SetAssetIssuance(
      txin_index, asset_amount, asset_locking_script, confidential_asset_nonce,
      token_amount, token_locking_script, confidential_token_nonce, is_blind,
      contract_hash);
  if (is_random_sort) {
    RandomSortTxOut();
  }
  return param;
}

IssuanceParameter ConfidentialTransactionController::SetAssetReissuance(
    const Txid& txid, uint32_t vout, const Amount& amount,
    const Script& locking_script, const ByteData& asset_nonce,
    const BlindFactor& blind_factor, const BlindFactor& entropy,
    bool is_random_sort, bool is_remove_nonce) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);

  ConfidentialNonce confidential_asset_nonce;
  if (!is_remove_nonce) {
    confidential_asset_nonce = ConfidentialNonce(asset_nonce);
  }

  IssuanceParameter param;
  param = transaction_.SetAssetReissuance(
      txin_index, amount, locking_script, confidential_asset_nonce,
      blind_factor, entropy);

  if (is_random_sort) {
    RandomSortTxOut();
  }
  return param;
}

void ConfidentialTransactionController::RandomSortTxOut() {
  transaction_.RandomSortTxOut();
}

void ConfidentialTransactionController::BlindTransaction(
    const std::vector<BlindParameter>& txin_info_list,
    const std::vector<IssuanceBlindingKeyPair>& issuance_blinding_keys,
    const std::vector<Pubkey>& txout_confidential_keys) {
  transaction_.BlindTransaction(
      txin_info_list, issuance_blinding_keys, txout_confidential_keys);
}

UnblindParameter ConfidentialTransactionController::UnblindTxOut(
    uint32_t tx_out_index, const Privkey& blinding_key) {
  return transaction_.UnblindTxOut(tx_out_index, blinding_key);
}

std::vector<UnblindParameter>
ConfidentialTransactionController::UnblindTransaction(  // NOLINT
    const std::vector<Privkey>& blinding_keys) {
  return transaction_.UnblindTxOut(blinding_keys);
}

std::vector<UnblindParameter>
ConfidentialTransactionController::UnblindIssuance(
    uint32_t tx_in_index, const Privkey& asset_blinding_key,
    const Privkey& token_blinding_key) {
  return transaction_.UnblindTxIn(
      tx_in_index, asset_blinding_key, token_blinding_key);
}

ByteData ConfidentialTransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey,
    SigHashType sighash_type, const ConfidentialValue& value,
    WitnessVersion version) const {
  Script script = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  ByteData256 sighash = transaction_.GetElementsSignatureHash(
      txin_index, script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

ByteData ConfidentialTransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Script& redeem_script,
    SigHashType sighash_type, const ConfidentialValue& value,
    WitnessVersion version) const {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  ByteData256 sighash = transaction_.GetElementsSignatureHash(
      txin_index, redeem_script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

bool ConfidentialTransactionController::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
    uint32_t vout, SigHashType sighash_type, const ConfidentialValue& value,
    WitnessVersion version) const {
  auto sighash = this->CreateSignatureHash(
      txid, vout, pubkey, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

bool ConfidentialTransactionController::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
    uint32_t vout, const Script& script, SigHashType sighash_type,
    const ConfidentialValue& value, WitnessVersion version) const {
  auto sighash = this->CreateSignatureHash(
      txid, vout, script, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

Amount ConfidentialTransactionController::CalculateSimpleFee(
    bool append_feature_signed_size, bool append_signed_witness) const {
  static constexpr uint32_t kP2wpkhWitnessSize = 72 + 33 + 3;
  // 簡易計算
  uint32_t size = transaction_.GetTotalSize();
  uint32_t vsize = transaction_.GetVsize();
  uint32_t rate = FeeCalculator::kRelayMinimumFee;
  if (append_feature_signed_size) {
    uint32_t weight = transaction_.GetWeight();
    uint32_t count = transaction_.GetTxInCount();
    uint32_t add_size;
    add_size = kP2wpkhWitnessSize * count;
    if (!append_signed_witness) {
      // no segwit (x4)
      add_size *= 4;
    }
    size += add_size;
    weight += add_size;
    vsize = (weight + 3) / 4;  // wally_tx_vsize_from_weight
  }
  return FeeCalculator::CalculateFee(size, vsize, rate);
}

}  // namespace cfd
#endif  // CFD_DISABLE_ELEMENTS
