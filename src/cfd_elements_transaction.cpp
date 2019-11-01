// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_transaction.cpp
 *
 * @brief-eng implementation of Elements Transaction operation related class
 * @brief-jp Elements Transaction操作の関連クラスの実装ファイル
 */
#ifndef CFD_DISABLE_ELEMENTS
#include "cfd/cfd_elements_transaction.h"

#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_fee.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"

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
using cfd::core::ScriptUtil;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::UnblindParameter;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// Define
// -----------------------------------------------------------------------------

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
    uint32_t bip32_counter, const ByteData& whitelist) {
  // AddressTypeの種別ごとに、locking_scriptを作成
  const ByteData hash_data = btc_address.GetHash();
  Script script = btc_address.GetLockingScript();

  PegoutKeyData key_data;
  if (online_pubkey.IsValid() && !master_online_key.IsInvalid()) {
    // pubkeys・whitelistproofを算出
    key_data = ConfidentialTransaction::GetPegoutPubkeyData(
        online_pubkey, master_online_key, btc_descriptor, bip32_counter,
        whitelist, net_type);
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

std::string ConfidentialTransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey,
    SigHashType sighash_type, Amount amount, bool is_witness) {
  Script script = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  ByteData256 sighash = transaction_.GetElementsSignatureHash(
      txin_index, script.GetData(), sighash_type, amount, is_witness);
  return sighash.GetHex();
}

std::string ConfidentialTransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey,
    SigHashType sighash_type, const ByteData& confidential_value,
    bool is_witness) {
  Script script = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  ByteData256 sighash = transaction_.GetElementsSignatureHash(
      txin_index, script.GetData(), sighash_type, confidential_value,
      is_witness);

  return sighash.GetHex();
}

std::string ConfidentialTransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Script& redeem_script,
    SigHashType sighash_type, Amount amount, bool is_witness) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  ByteData256 sighash = transaction_.GetElementsSignatureHash(
      txin_index, redeem_script.GetData(), sighash_type, amount, is_witness);
  return sighash.GetHex();
}

std::string ConfidentialTransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Script& redeem_script,
    SigHashType sighash_type, const ByteData& confidential_value,
    bool is_witness) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  ByteData256 sighash = transaction_.GetElementsSignatureHash(
      txin_index, redeem_script.GetData(), sighash_type, confidential_value,
      is_witness);
  return sighash.GetHex();
}

Amount ConfidentialTransactionController::CalculateSimpleFee(
    bool append_feature_signed_size, bool append_signed_witness) {
  static constexpr uint32_t kP2wpkhWitnessSize = 72 + 33 + 3;
  // 簡易計算
  uint32_t size = transaction_.GetTotalSize();
  uint32_t vsize = transaction_.GetVsize();
  uint32_t rate = FeeCalculator::kBaseRate;
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
