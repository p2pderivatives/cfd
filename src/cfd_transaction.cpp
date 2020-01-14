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
using cfd::core::Privkey;
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
// multisig key数上限
constexpr uint32_t kMaximumMultisigKeyNum = 15;

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

TransactionContext::TransactionContext(const Transaction& transaction)
    : Transaction(transaction.GetHex()) {}

TransactionContext& TransactionContext::operator=(
    const TransactionContext& context) & {
  SetFromHex(context.GetHex());
  utxo_map_ = context.utxo_map_;
  signed_map_ = context.signed_map_;
  verify_map_ = context.verify_map_;
  verify_ignore_map_ = context.verify_ignore_map_;
  return *this;
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

#if 0
void TransactionContext::SignWithKey(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
    SigHashType sighash_type, bool has_grind_r) {
  if (utxo_map_.count(outpoint) == 0) {
    warn(CFD_LOG_SOURCE, "Failed to SignWithKey. utxo not found.");
    throw CfdException(CfdError::kCfdIllegalStateError, "utxo not found.");
  }
  UtxoData utxo = utxo_map_[outpoint];
  Amount value = utxo.amount;

  SignWithPrivkeySimple(
      outpoint, pubkey, privkey, sighash_type, value, utxo.address_type,
      has_grind_r);
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
#endif

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
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
    SigHashType sighash_type, const Amount& value, AddressType address_type,
    bool has_grind_r) {
  if (!outpoint.IsValid()) {
    warn(CFD_LOG_SOURCE, "Failed to SignWithPrivkeySimple. Invalid outpoint.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Invalid outpoint.");
  }
  if (!pubkey.IsValid()) {
    warn(CFD_LOG_SOURCE, "Failed to SignWithPrivkeySimple. Invalid pubkey.");
    throw CfdException(CfdError::kCfdIllegalArgumentError, "Invalid pubkey.");
  }
  if (privkey.IsInvalid()) {
    warn(CFD_LOG_SOURCE, "Failed to SignWithPrivkeySimple. Invalid privkey.");
    throw CfdException(CfdError::kCfdIllegalArgumentError, "Invalid privkey.");
  }

  Txid txid = outpoint.GetTxid();
  uint32_t vout = outpoint.GetVout();
  WitnessVersion version = WitnessVersion::kVersionNone;
  switch (address_type) {
    case AddressType::kP2wshAddress:
    case AddressType::kP2shP2wshAddress:
    case AddressType::kP2wpkhAddress:
    case AddressType::kP2shP2wpkhAddress:
      version = WitnessVersion::kVersion0;
    default:
      break;
  }

  ByteData sighash =
      CreateSignatureHash(txid, vout, pubkey, sighash_type, value, version);
  ByteData signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash), privkey, has_grind_r);
  SignParameter sign(signature, true, sighash_type);

  AddPubkeyHashSign(outpoint, sign, pubkey, address_type);
}

void TransactionContext::AddPubkeyHashSign(
    const OutPoint& outpoint, const SignParameter& signature,
    const Pubkey& pubkey, AddressType address_type) {
  if ((signature.GetDataType() != SignDataType::kSign) &&
      (signature.GetDataType() != SignDataType::kBinary)) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddPubkeyHashSign. Invalid signature type: {}",
        signature.GetDataType());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid signature type. signature type must be \"kSign\" "
        "or \"kBinary\".");  // NOLINT
  }
  if (!pubkey.IsValid()) {
    warn(CFD_LOG_SOURCE, "Failed to AddPubkeyHashSign. Invalid pubkey.");
    throw CfdException(CfdError::kCfdIllegalArgumentError, "Invalid pubkey.");
  }
  if (!outpoint.IsValid()) {
    warn(CFD_LOG_SOURCE, "Failed to AddPubkeyHashSign. Invalid outpoint.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Invalid outpoint.");
  }

  bool has_witness = false;
  bool has_scriptsig = false;
  Script locking_script;
  std::vector<SignParameter> sign_params;
  switch (address_type) {
    case AddressType::kP2wpkhAddress:
      has_witness = true;
      break;
    case AddressType::kP2shP2wpkhAddress:
      has_witness = true;
      has_scriptsig = true;
      locking_script = ScriptUtil::CreateP2wpkhLockingScript(pubkey);
      break;
    case AddressType::kP2pkhAddress:
      has_scriptsig = true;
      break;
    default:
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddPubkeyHashSign. Invalid address_type: {}",
          address_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid address_type. address_type must be \"p2wpkh\" "
          "or \"p2sh-p2wpkh\" or \"p2pkh\".");  // NOLINT
  }
  sign_params.push_back(signature);
  sign_params.push_back(SignParameter(pubkey));

  if (has_witness) {
    AddSign(outpoint, sign_params, true, true);
  }
  if (has_scriptsig) {
    if (!locking_script.IsEmpty()) {  // p2sh-p2wpkh
      std::vector<SignParameter> scriptsig;
      scriptsig.push_back(SignParameter(locking_script));
      AddSign(outpoint, scriptsig, false, true);
    } else {
      AddSign(outpoint, sign_params, false, true);
    }
  }

  signed_map_.emplace(outpoint, signature.GetSigHashType());
}

void TransactionContext::AddScriptHashSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
    const Script& redeem_script, AddressType address_type,
    bool is_multisig_script) {
  if (redeem_script.IsEmpty()) {
    warn(CFD_LOG_SOURCE, "Failed to AddScriptHashSign. Empty script.");
    throw CfdException(CfdError::kCfdIllegalArgumentError, "Empty script.");
  }
  if (!outpoint.IsValid()) {
    warn(CFD_LOG_SOURCE, "Failed to AddPubkeyHashSign. Invalid outpoint.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Invalid outpoint.");
  }
  for (const auto& signature : signatures) {
    if ((signature.GetDataType() != SignDataType::kSign) &&
        (signature.GetDataType() != SignDataType::kBinary)) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddScriptHashSign. Invalid signature type: {}",
          signature.GetDataType());
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid signature type. signature type must be \"kSign\" "
          "or \"kBinary\".");  // NOLINT
    }
  }

  bool has_witness = false;
  bool has_scriptsig = false;
  Script locking_script;
  std::vector<SignParameter> sign_params;
  switch (address_type) {
    case AddressType::kP2wshAddress:
      has_witness = true;
      break;
    case AddressType::kP2shP2wshAddress:
      has_witness = true;
      has_scriptsig = true;
      locking_script = ScriptUtil::CreateP2wshLockingScript(redeem_script);
      break;
    case AddressType::kP2shAddress:
      has_scriptsig = true;
      break;
    default:
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddScriptHashSign. Invalid address_type: {}",
          address_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid address_type. address_type must be \"p2wsh\" "
          "or \"p2sh-p2wsh\" or \"p2sh\".");  // NOLINT
  }
  if (is_multisig_script) {
    if (has_witness) {
      sign_params.push_back(SignParameter(ByteData()));
    } else {
      sign_params.push_back(SignParameter(ScriptOperator::OP_0));
    }
  }
  for (const auto& signature : signatures) {
    sign_params.push_back(signature);
  }
  sign_params.push_back(SignParameter(redeem_script));

  if (has_witness) {
    AddSign(outpoint, sign_params, true, true);
  }
  if (has_scriptsig) {
    if (!locking_script.IsEmpty()) {  // p2sh-p2wpkh
      std::vector<SignParameter> scriptsig;
      scriptsig.push_back(SignParameter(locking_script));
      AddSign(outpoint, scriptsig, false, true);
    } else {
      AddSign(outpoint, sign_params, false, true);
    }
  }

  // TODO(k-matsuzawa): consider to multi-signature.
  // signed_map_.emplace(outpoint, signature.GetSigHashType());
}

void TransactionContext::AddMultisigSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
    const Script& redeem_script, AddressType address_type) {
  std::vector<SignParameter> sign_list =
      CheckMultisig(signatures, redeem_script);
  AddScriptHashSign(outpoint, sign_list, redeem_script, address_type, true);
}

void TransactionContext::AddSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& sign_params,
    bool insert_witness, bool clear_stack) {
  if (sign_params.empty()) {
    warn(CFD_LOG_SOURCE, "Failed to AddScriptHashSign. Empty sign_params.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Empty sign_params.");
  }
  if (!outpoint.IsValid()) {
    warn(CFD_LOG_SOURCE, "Failed to AddPubkeyHashSign. Invalid outpoint.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Invalid outpoint.");
  }

  bool has_op_code = false;
  for (const SignParameter& sign_param : sign_params) {
    if (sign_param.IsOpCode()) {
      ScriptOperator op_code = sign_param.GetOpCode();
      if (op_code.IsPushOperator()) {
        has_op_code = true;
      }
    }
  }
  Txid txid = outpoint.GetTxid();
  uint32_t vout = outpoint.GetVout();
  uint32_t txin_index = GetTxInIndex(txid, vout);

  if (insert_witness) {
    if (clear_stack) {
      RemoveScriptWitnessStackAll(txin_index);
    }
    for (const SignParameter& sign_param : sign_params) {
      AddScriptWitnessStack(txin_index, sign_param.ConvertToSignature());
    }
  } else {
    Script script;
    if (!clear_stack) {
      script = GetTxIn(txin_index).GetUnlockingScript();
    }
    ScriptBuilder builder;
    for (const auto& element : script.GetElementList()) {
      builder.AppendElement(element);
    }
    for (const SignParameter& sign_param : sign_params) {
      if (has_op_code && sign_param.IsOpCode()) {
        // Checking push-operator is performed at the time of registration.
        builder.AppendOperator(sign_param.GetOpCode());
      } else {
        builder.AppendData(sign_param.ConvertToSignature());
      }
    }
    SetUnlockingScript(txin_index, builder.Build());
  }
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

void TransactionContext::CallbackStateChange(uint32_t type) {
  cfd::core::logger::trace(
      CFD_LOG_SOURCE, "CallbackStateChange type::{}", type);
}

std::vector<SignParameter> TransactionContext::CheckMultisig(
    const std::vector<SignParameter>& sign_list, const Script& redeem_script) {
  // check signData (too much data)
  if (sign_list.empty() || (sign_list.size() > kMaximumMultisigKeyNum)) {
    warn(
        CFD_LOG_SOURCE, "Failed to CheckMultisig. sign array is '1 - {}'.",
        kMaximumMultisigKeyNum);
    throw CfdException(
        CfdError::kCfdOutOfRangeError,
        "Value out of range. sign array length fail.");
  }

  // check multisig format and extract pubkey.
  std::vector<Pubkey> pubkeys =
      ScriptUtil::ExtractPubkeysFromMultisigScript(redeem_script);

  std::vector<SignParameter> signature_list;
  std::vector<SignParameter> sign_params = sign_list;
  for (const Pubkey& pubkey : pubkeys) {
    std::string pubkey_hex = pubkey.GetHex();
    for (auto itr = sign_params.begin(); itr != sign_params.end();) {
      SignParameter sign_param = *itr;
      Pubkey related_pubkey = sign_param.GetRelatedPubkey();
      if ((!related_pubkey.IsValid()) ||
          (related_pubkey.GetHex() != pubkey_hex)) {
        ++itr;
        continue;
      }

      itr = sign_params.erase(itr);
      signature_list.push_back(sign_param);
    }
  }

  // set the others to signature data
  for (const auto& sign_param : sign_params) {
    // related pubkey not found in script
    if (sign_param.GetRelatedPubkey().IsValid()) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to CheckMultisig. Missing related pubkey"
          " in script.: relatedPubkey={}, script={}",
          sign_param.GetRelatedPubkey().GetHex(), redeem_script.GetHex());
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Missing related pubkey in script."
          " Check your signature and pubkey pair.");
    }
    signature_list.push_back(sign_param);
  }

  return signature_list;
}

// -----------------------------------------------------------------------------
// TransactionController
// -----------------------------------------------------------------------------
TransactionController::TransactionController() : transaction_(2, 0) {
  tx_address_ = &transaction_;
}

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

TransactionController& TransactionController::operator=(
    const TransactionController& transaction) & {
  transaction_ = transaction.transaction_;
  return *this;
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
