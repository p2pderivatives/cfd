// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction.cpp
 *
 * @brief \~english implementation of classes related to transaction operation
 *   \~japanese Transaction操作の関連クラスの実装ファイル
 */
#include "cfd/cfd_transaction.h"

#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
// #include "cfdcore_util.h"
// #include "cfd_node_util.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::HashType;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::TxIn;
using cfd::core::TxInReference;
using cfd::core::TxOut;
using cfd::core::TxOutReference;
using cfd::core::logger::warn;

using cfd::TransactionController;

// -----------------------------------------------------------------------------
// Define
// -----------------------------------------------------------------------------
/// シーケンス値(locktime有効)
constexpr uint32_t kSequenceEnableLockTimeMax = 0xfffffffeU;
/// シーケンス値(locktime無効)
constexpr uint32_t kSequenceDisableLockTime = 0xffffffffU;

// -----------------------------------------------------------------------------
// TransactionController
// -----------------------------------------------------------------------------
TransactionContext::TransactionContext() {}

TransactionContext::TransactionContext(uint32_t version, uint32_t locktime)
    : Transaction(version, locktime) {}

TransactionContext::TransactionContext(const std::string& tx_hex)
    : Transaction(tx_hex) {}

TransactionContext::TransactionContext(const ByteData& byte_data)
    : Transaction(byte_data.GetHex()) {}

TransactionContext::TransactionContext(const TransactionContext& context)
    : Transaction(context.GetHex()) {
  utxo_map_ = context.utxo_map_;
  signed_map_ = context.signed_map_;
  verify_map_ = context.verify_map_;
  verify_ignore_map_ = context.verify_ignore_map_;
}

void TransactionContext::AddInput(const UtxoData& utxo) {
  AddInput(utxo, GetDefaultSequence());
}

void TransactionContext::AddInput(const UtxoData& utxo, uint32_t sequence) {
  if (!utxo.txid.IsValid()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to AddInput. invalid utxo txid.");
  }

  UtxoData dest;
  Utxo temp;
  memset(&temp, 0, sizeof(temp));
  UtxoUtil::ConvertToUtxo(utxo, &temp, &dest);

  AddTxIn(utxo.txid, utxo.vout, sequence);
  utxo_map_.emplace(OutPoint(utxo.txid, utxo.vout), dest);
}

void TransactionContext::AddInputs(const std::vector<UtxoData>& utxos) {
  for (const auto& utxo : utxos) {
    AddInput(utxo, GetDefaultSequence());
  }
}

uint32_t TransactionContext::AddTxOut(
    const Address& address, const Amount& value) {
  const ByteData hash_data = address.GetHash();
  Script locking_script = address.GetLockingScript();
  return Transaction::AddTxOut(value, locking_script);
}

uint32_t TransactionContext::GetSizeIgnoreTxIn() const {
  uint32_t result = AbstractTransaction::kTransactionMinimumSize;
  std::vector<TxOutReference> txouts = GetTxOutList();
  for (const auto& txout : txouts) {
    result += txout.GetSerializeSize();
  }
  return result;
}

void TransactionContext::CollectInputUtxo(const std::vector<UtxoData>& utxos) {
  if ((!utxos.empty()) && (utxo_map_.size() != GetTxInCount())) {
    for (const auto& txin_ref : GetTxInList()) {
      const Txid& txid = txin_ref.GetTxid();
      uint32_t vout = txin_ref.GetVout();

      OutPoint outpoint(txid, vout);
      if (utxo_map_.count(outpoint) == 0) {
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

void TransactionContext::SignWithKey(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& private_key,
    SigHashType sighash_type, bool has_grind_r) {
  // FIXME please implements.
  // 1. add sign
  // 2. add map(signed_map_, etc.)
  return;
}

void TransactionContext::IgnoreVerify(const OutPoint& outpoint) {
  GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
  verify_ignore_map_.emplace(outpoint);
  verify_map_.erase(outpoint);
}

void TransactionContext::Verify() {
  // FIXME please implements.
  return;
}

void TransactionContext::Verify(const OutPoint& outpoint) {
  GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());

  if (utxo_map_.count(outpoint) == 0) {
    // FIXME throw utxo not found.
  }

  // FIXME please implements.

  verify_map_.emplace(outpoint);
  verify_ignore_map_.erase(outpoint);
}

ByteData TransactionContext::Finalize() {
  if ((verify_map_.size() + verify_ignore_map_.size()) != GetTxInCount()) {
    Verify();
  }
  return AbstractTransaction::GetData();
}

// priority: low
void TransactionContext::ClearSign() { return; }

// priority: low
void TransactionContext::ClearSign(const OutPoint& outpoint) { return; }

ByteData TransactionContext::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey,
    SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  Script script = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  uint32_t txin_index = GetTxInIndex(txid, vout);
  ByteData256 sighash = GetSignatureHash(
      txin_index, script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

ByteData TransactionContext::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Script& redeem_script,
    SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  uint32_t txin_index = GetTxInIndex(txid, vout);
  // TODO(soejima): OP_CODESEPARATOR存在時、Scriptの分割が必要。
  ByteData256 sighash = GetSignatureHash(
      txin_index, redeem_script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

void TransactionContext::SignWithPrivkeySimple(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& private_key,
    SigHashType sighash_type, const Amount& value, WitnessVersion version,
    bool has_grind_r) {
  // FIXME please implements.
  return;
}

bool TransactionContext::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
    uint32_t vout, SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  auto sighash =
      CreateSignatureHash(txid, vout, pubkey, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

bool TransactionContext::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
    uint32_t vout, const Script& script, SigHashType sighash_type,
    const Amount& value, WitnessVersion version) const {
  auto sighash =
      CreateSignatureHash(txid, vout, script, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

uint32_t TransactionContext::GetDefaultSequence() const {
  if (GetLockTime() == 0) {
    return kSequenceDisableLockTime;
  } else {
    return kSequenceEnableLockTimeMax;
  }
}

uint32_t TransactionContext::GetLockTimeDisabledSequence() {
  return kSequenceDisableLockTime;
}

void TransactionContext::CallbackStateChange(uint32_t type) {}

// -----------------------------------------------------------------------------
// TransactionController
// -----------------------------------------------------------------------------
TransactionController::TransactionController(
    uint32_t version, uint32_t locktime)
    : transaction_(version, locktime) {
  tx_address_ = &transaction_;
}

TransactionController::TransactionController(const std::string& tx_hex)
    : transaction_(tx_hex) {
  tx_address_ = &transaction_;
}

TransactionController::TransactionController(
    const TransactionController& transaction)
    : TransactionController(transaction.GetHex()) {
  // do nothing
}

const TxInReference TransactionController::AddTxIn(
    const Txid& txid, uint32_t vout) {
  return AddTxIn(txid, vout, GetDefaultSequence());
}

const TxInReference TransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, uint32_t sequence) {
  uint32_t index = transaction_.AddTxIn(txid, vout, sequence);
  return transaction_.GetTxIn(index);
}

const TxInReference TransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Script& redeem_script) {
  return AddTxIn(txid, vout, redeem_script, GetDefaultSequence());
}

const TxInReference TransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Script& redeem_script,
    uint32_t sequence) {
  uint32_t index = transaction_.AddTxIn(txid, vout, sequence, redeem_script);
  return transaction_.GetTxIn(index);
}

const TxInReference TransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey) {
  return AddTxIn(txid, vout, pubkey, GetDefaultSequence());
}

const TxInReference TransactionController::AddTxIn(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey, uint32_t sequence) {
  uint32_t index = transaction_.AddTxIn(
      txid, vout, sequence, ScriptBuilder().AppendData(pubkey).Build());
  return transaction_.GetTxIn(index);
}

const TxInReference TransactionController::RemoveTxIn(
    const Txid& txid, uint32_t vout) {
  uint32_t index = transaction_.GetTxInIndex(txid, vout);
  TxInReference ref = transaction_.GetTxIn(index);
  transaction_.RemoveTxIn(index);
  return ref;
}

const TxOutReference TransactionController::AddTxOut(
    const Script& locking_script, const Amount& value) {
  uint32_t index = transaction_.AddTxOut(value, locking_script);
  return transaction_.GetTxOut(index);
}

const TxOutReference TransactionController::AddTxOut(
    const Address& address, const Amount& value) {
  const ByteData hash_data = address.GetHash();
  Script locking_script = address.GetLockingScript();
  uint32_t index = transaction_.AddTxOut(value, locking_script);
  return transaction_.GetTxOut(index);
}

const TxOutReference TransactionController::RemoveTxOut(uint32_t index) {
  TxOutReference ref = transaction_.GetTxOut(index);
  transaction_.RemoveTxOut(index);
  return ref;
}

void TransactionController::InsertUnlockingScript(
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

void TransactionController::SetUnlockingScript(
    const Txid& txid, uint32_t vout, const Script& unlocking_script) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.SetUnlockingScript(txin_index, unlocking_script);
}

void TransactionController::SetUnlockingScript(
    const Txid& txid, uint32_t vout,
    const std::vector<ByteData>& unlocking_scripts) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.SetUnlockingScript(txin_index, unlocking_scripts);
}

uint32_t TransactionController::GetWitnessStackNum(
    const Txid& txid, uint32_t vout) const {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  return transaction_.GetScriptWitnessStackNum(txin_index);
}

void TransactionController::AddWitnessStack(
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

void TransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout,
    const std::string& signed_signature_hash) {
  std::vector<std::string> signed_signature_hashes(1);
  signed_signature_hashes[0] = signed_signature_hash;
  AddWitnessStack(txid, vout, signed_signature_hashes);
}

void TransactionController::AddWitnessStack(
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

void TransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout, const std::string& signed_signature_hash,
    const Pubkey& pubkey) {
  std::vector<std::string> signed_signature_hashes(2);
  signed_signature_hashes[0] = signed_signature_hash;
  signed_signature_hashes[1] = pubkey.GetHex();
  AddWitnessStack(txid, vout, signed_signature_hashes);
}

void TransactionController::AddWitnessStack(
    const Txid& txid, uint32_t vout, const std::string& signed_signature_hash,
    const Script& redeem_script) {
  std::vector<std::string> signed_signature_hashes(1);
  signed_signature_hashes[0] = signed_signature_hash;
  AddWitnessStack(txid, vout, signed_signature_hashes, redeem_script);
}

void TransactionController::AddWitnessStack(
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

void TransactionController::SetWitnessStack(
    const Txid& txid, uint32_t vout, uint32_t witness_index,
    const ByteData& witness_stack) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.SetScriptWitnessStack(txin_index, witness_index, witness_stack);
}

void TransactionController::SetWitnessStack(
    const Txid& txid, uint32_t vout, uint32_t witness_index,
    const std::string& hex_string) {
  SetWitnessStack(txid, vout, witness_index, ByteData(hex_string));
}

void TransactionController::RemoveWitnessStackAll(  // linefeed
    const Txid& txid, uint32_t vout) {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  transaction_.RemoveScriptWitnessStackAll(txin_index);
}

const Transaction& TransactionController::GetTransaction() const {
  return transaction_;
}

uint32_t TransactionController::GetSizeIgnoreTxIn() const {
  uint32_t result = AbstractTransaction::kTransactionMinimumSize;
  std::vector<TxOutReference> txouts = transaction_.GetTxOutList();
  for (const auto& txout : txouts) {
    result += txout.GetSerializeSize();
  }
  return result;
}

const TxInReference TransactionController::GetTxIn(
    const Txid& txid, uint32_t vout) const {
  uint32_t index = transaction_.GetTxInIndex(txid, vout);
  return transaction_.GetTxIn(index);
}

ByteData TransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Pubkey& pubkey,
    SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  Script script = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  ByteData256 sighash = transaction_.GetSignatureHash(
      txin_index, script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

ByteData TransactionController::CreateSignatureHash(
    const Txid& txid, uint32_t vout, const Script& redeem_script,
    SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  uint32_t txin_index = transaction_.GetTxInIndex(txid, vout);
  // TODO(soejima): OP_CODESEPARATOR存在時、Scriptの分割が必要。
  ByteData256 sighash = transaction_.GetSignatureHash(
      txin_index, redeem_script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

bool TransactionController::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
    uint32_t vout, SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  auto sighash = this->CreateSignatureHash(
      txid, vout, pubkey, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

bool TransactionController::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
    uint32_t vout, const Script& script, SigHashType sighash_type,
    const Amount& value, WitnessVersion version) const {
  auto sighash = this->CreateSignatureHash(
      txid, vout, script, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

}  // namespace cfd
