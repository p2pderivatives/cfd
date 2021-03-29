// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction.cpp
 *
 * @brief implementation of classes related to transaction operation
 */
#include "cfd/cfd_transaction.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd_transaction_internal.h"  // NOLINT
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_schnorrsig.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_taproot.h"
#include "cfdcore/cfdcore_transaction.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Descriptor;
using cfd::core::HashType;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::SchnorrUtil;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;
using cfd::core::TaprootScriptTree;
using cfd::core::TaprootUtil;
using cfd::core::TapScriptData;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::TxIn;
using cfd::core::TxInReference;
using cfd::core::TxOut;
using cfd::core::TxOutReference;
using cfd::core::logger::warn;

using cfd::TransactionController;

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
 * @param[in] annex           annex
 * @param[in] script_tree     taproot script tree.
 * @return signature hash.
 */
static ByteData256 CreateTxSighash(
    const TransactionContext* transaction, const OutPoint& outpoint,
    const UtxoData& utxo, const SigHashType& sighash_type,
    const Pubkey& pubkey, const Script& redeem_script, WitnessVersion version,
    const ByteData* annex, const TaprootScriptTree* script_tree) {
  ByteData sig;
  if (version == WitnessVersion::kVersion1) {
    ByteData256 tapleaf_hash;
    if (script_tree != nullptr) tapleaf_hash = script_tree->GetTapLeafHash();
    return transaction->CreateSignatureHashByTaproot(
        outpoint, sighash_type,
        (script_tree != nullptr) ? &tapleaf_hash : nullptr, 0, annex);
  } else if (redeem_script.IsEmpty()) {
    sig = transaction->CreateSignatureHash(
        outpoint, pubkey, sighash_type, utxo.amount, version);
  } else {
    sig = transaction->CreateSignatureHash(
        outpoint, redeem_script, sighash_type, utxo.amount, version);
  }
  return ByteData256(sig);
}
/*
  std::function<ByteData256(const Tx*, const OutPoint&,
    const UtxoData&, const SigHashType&, const Pubkey&y,
    const Script&, WitnessVersion)> create_sighash_func;
*/

/**
 * @brief Get locking script from utxo.
 * @param[in] utxo    utxo data.
 * @return locking script
 */
static Script GetLockingScriptFromUtxoData(const UtxoData& utxo) {
  Script locking_script = utxo.locking_script;
  if (!utxo.address.GetAddress().empty()) {
    locking_script = utxo.address.GetLockingScript();
  } else if (!utxo.descriptor.empty()) {
    auto desc = Descriptor::Parse(utxo.descriptor);
    locking_script = desc.GetLockingScript();
  }
  return locking_script;
}

// -----------------------------------------------------------------------------
// TransactionController
// -----------------------------------------------------------------------------
TransactionContext::TransactionContext() {
  utxo_map_.clear();
  signed_map_.clear();
  verify_map_.clear();
  verify_ignore_map_.clear();
}

TransactionContext::TransactionContext(uint32_t version, uint32_t locktime)
    : Transaction(version, locktime) {
  utxo_map_.clear();
  signed_map_.clear();
  verify_map_.clear();
  verify_ignore_map_.clear();
}

TransactionContext::TransactionContext(const std::string& tx_hex)
    : Transaction(tx_hex) {
  utxo_map_.clear();
  signed_map_.clear();
  verify_map_.clear();
  verify_ignore_map_.clear();
}

TransactionContext::TransactionContext(const ByteData& byte_data)
    : Transaction(byte_data.GetHex()) {
  utxo_map_.clear();
  signed_map_.clear();
  verify_map_.clear();
  verify_ignore_map_.clear();
}

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
  if (this != &context) {
    SetFromHex(context.GetHex());
    utxo_map_ = context.utxo_map_;
    signed_map_ = context.signed_map_;
    verify_map_ = context.verify_map_;
    verify_ignore_map_ = context.verify_ignore_map_;
  }
  return *this;
}

uint32_t TransactionContext::GetTxInIndex(const OutPoint& outpoint) const {
  return GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
}

uint32_t TransactionContext::AddTxIn(const OutPoint& outpoint) {
  return AddTxIn(
      outpoint.GetTxid(), outpoint.GetVout(), GetDefaultSequence(),
      Script::Empty);
}

uint32_t TransactionContext::GetTxOutIndex(const Address& address) const {
  return GetTxOutIndex(address.GetLockingScript());
}

bool TransactionContext::IsFindTxIn(
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

bool TransactionContext::IsFindTxOut(
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

bool TransactionContext::IsFindTxOut(
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

const TxInReference TransactionContext::GetTxIn(
    const OutPoint& outpoint) const {
  uint32_t index = GetTxInIndex(outpoint);
  return GetTxIn(index);
}

Address TransactionContext::GetTxOutAddress(
    uint32_t index, NetType net_type) const {
  if (vout_.size() <= index) {
    throw CfdException(
        CfdError::kCfdOutOfRangeError, "vout out_of_range error.");
  }
  AddressFactory address_factory(net_type);
  return address_factory.GetAddressByLockingScript(
      vout_[index].GetLockingScript());
}

uint32_t TransactionContext::AddTxOut(
    const Address& address, const Amount& value) {
  const ByteData hash_data = address.GetHash();
  return AddTxOut(value, address.GetLockingScript());
}

uint32_t TransactionContext::GetSizeIgnoreTxIn(bool use_witness) const {
  uint32_t result = AbstractTransaction::kTransactionMinimumSize;
  if (use_witness) result += 2;
  std::vector<TxOutReference> txouts = GetTxOutList();
  for (const auto& txout : txouts) {
    result += txout.GetSerializeSize();
  }
  return result;
}

uint32_t TransactionContext::GetVsizeIgnoreTxIn(bool use_witness) const {
  return AbstractTransaction::GetVsizeFromSize(
      GetSizeIgnoreTxIn(use_witness), 0);
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
  utxo_map_.emplace_back(dest);
}

void TransactionContext::AddInputs(const std::vector<UtxoData>& utxos) {
  for (const auto& utxo : utxos) {
    AddInput(utxo, GetDefaultSequence());
  }
}

void TransactionContext::CollectInputUtxo(const std::vector<UtxoData>& utxos) {
  if ((!utxos.empty()) && (utxo_map_.size() != GetTxInCount())) {
    for (const auto& txin_ref : vin_) {
      const Txid& txid = txin_ref.GetTxid();
      uint32_t vout = txin_ref.GetVout();

      OutPoint outpoint(txid, vout);
      if (!IsFindUtxoMap(outpoint)) {
        for (const auto& utxo : utxos) {
          if ((utxo.vout == vout) && utxo.txid.Equals(txid)) {
            UtxoData dest;
            Utxo temp;
            memset(&temp, 0, sizeof(temp));
            UtxoUtil::ConvertToUtxo(utxo, &temp, &dest);
            utxo_map_.emplace_back(dest);
            break;
          }
        }
      }
    }
  }
}

UtxoData TransactionContext::GetTxInUtxoData(const OutPoint& outpoint) const {
  UtxoData utxo;
  utxo.address_type = AddressType::kP2shAddress;
  utxo.vout = 0;
  utxo.block_height = 0;
  utxo.binary_data = nullptr;
  IsFindUtxoMap(outpoint, &utxo);
  return utxo;
}

Amount TransactionContext::GetFeeAmount() const {
  Amount input;
  UtxoData utxo;
  for (const auto& txin_ref : vin_) {
    OutPoint outpoint(txin_ref.GetTxid(), txin_ref.GetVout());
    if (!IsFindUtxoMap(outpoint, &utxo)) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Utxo is not found. GetFeeAmount fail.");
    }
    input += utxo.amount;
  }

  Amount output;
  for (const auto& txout_ref : vout_) {
    output += txout_ref.GetValue();
  }
  Amount result = input - output;
  if (result.GetSatoshiValue() < 0) {
    return Amount();
  }
  return result;
}

void TransactionContext::SignWithKey(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
    SigHashType sighash_type, bool has_grind_r, const ByteData256* aux_rand,
    const ByteData* annex) {
  UtxoData utxo;
  if (!IsFindUtxoMap(outpoint, &utxo)) {
    throw CfdException(
        CfdError::kCfdIllegalStateError, "Utxo is not found. sign fail.");
  }
  utxo.locking_script = GetLockingScriptFromUtxoData(utxo);
  if (utxo.locking_script.IsTaprootScript()) {
    SignWithSchnorrPrivkeySimple(
        outpoint, privkey, sighash_type, aux_rand, annex);
  } else {
    SignWithPrivkeySimple(
        outpoint, pubkey, privkey, sighash_type, utxo.amount,
        utxo.address_type, has_grind_r);
  }
}

void TransactionContext::IgnoreVerify(const OutPoint& outpoint) {
  GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
  if (!IsFindOutPoint(verify_ignore_map_, outpoint)) {
    verify_ignore_map_.emplace_back(outpoint);
  }
}

void TransactionContext::Verify() {
  for (const auto& vin : vin_) {
    OutPoint outpoint = vin.GetOutPoint();
    if (!IsFindOutPoint(verify_ignore_map_, outpoint)) {
      Verify(outpoint);
    }
  }
}

void TransactionContext::Verify(const OutPoint& outpoint) {
  UtxoData utxo;
  if (!IsFindUtxoMap(outpoint, &utxo)) {
    throw CfdException(
        CfdError::kCfdIllegalStateError, "Utxo is not found. verify fail.");
  }
  const auto& txin = vin_[GetTxInIndex(outpoint)];

  std::vector<UtxoData> utxo_list;
  utxo.locking_script = GetLockingScriptFromUtxoData(utxo);
  if (utxo.locking_script.IsTaprootScript()) {
    UtxoData work_utxo;
    for (const auto& txin_ref : vin_) {
      OutPoint target_outpoint(txin_ref.GetTxid(), txin_ref.GetVout());
      if (!IsFindUtxoMap(target_outpoint, &work_utxo)) {
        throw CfdException(
            CfdError::kCfdIllegalStateError,
            "Utxo is not found. Verify fail.");
      }
      work_utxo.locking_script = GetLockingScriptFromUtxoData(work_utxo);
      utxo_list.emplace_back(work_utxo);
    }
  }
  TransactionContextUtil::Verify<TransactionContext>(
      this, outpoint, utxo, &txin, CreateTxSighash);
  if (!IsFindOutPoint(verify_map_, outpoint)) {
    verify_map_.emplace_back(outpoint);
  }
}

ByteData TransactionContext::Finalize() {
  Verify();
  return AbstractTransaction::GetData();
}

ByteData TransactionContext::CreateSignatureHash(
    const OutPoint& outpoint, const Pubkey& pubkey, SigHashType sighash_type,
    const Amount& value, WitnessVersion version) const {
  Script script = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  ByteData256 sighash = GetSignatureHash(
      GetTxInIndex(outpoint), script.GetData(), sighash_type, value, version);
  return ByteData(sighash.GetBytes());
}

ByteData TransactionContext::CreateSignatureHash(
    const OutPoint& outpoint, const Script& redeem_script,
    SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  // TODO(k-matsuzawa): For now, when using OP_CODESEPARATOR, divide it on the user side and ask them to specify only the applicable part.  // NOLINT
  ByteData256 sighash = GetSignatureHash(
      GetTxInIndex(outpoint), redeem_script.GetData(), sighash_type, value,
      version);
  return ByteData(sighash.GetBytes());
}

ByteData256 TransactionContext::CreateSignatureHashByTaproot(
    const OutPoint& outpoint, const SigHashType& sighash_type,
    const ByteData256* tap_leaf_hash, const uint32_t* code_separator_position,
    const ByteData* annex) const {
  UtxoData utxo;
  std::vector<TxOut> utxos;
  // utxos.reserve(vin_.size());
  // for (const auto& txin_ref : vin_) {
  for (size_t index = 0; index < vin_.size(); ++index) {
    const auto& txin_ref = vin_[index];
    OutPoint target_outpoint(txin_ref.GetTxid(), txin_ref.GetVout());
    if (!IsFindUtxoMap(target_outpoint, &utxo)) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Utxo is not found. CreateSignatureHashByTaproot fail.");
    }
    utxo.locking_script = GetLockingScriptFromUtxoData(utxo);
    utxos.emplace_back(TxOut(utxo.amount, utxo.locking_script));
  }

  uint32_t txin_index = GetTxInIndex(outpoint);
  ByteData annex_data;
  if (annex != nullptr) annex_data = *annex;
  TapScriptData script_data;
  if (tap_leaf_hash != nullptr) {
    script_data.tap_leaf_hash = *tap_leaf_hash;
    if (code_separator_position != nullptr) {
      script_data.code_separator_position = *code_separator_position;
    }
  }

  return GetSchnorrSignatureHash(
      txin_index, sighash_type, utxos,
      (tap_leaf_hash != nullptr) ? &script_data : nullptr, annex_data);
}

void TransactionContext::SignWithPrivkeySimple(
    const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
    SigHashType sighash_type, const Amount& value, AddressType address_type,
    bool has_grind_r) {
  WitnessVersion version = TransactionContextUtil::CheckSignWithPrivkeySimple(
      outpoint, pubkey, privkey, address_type);

  ByteData sighash =
      CreateSignatureHash(outpoint, pubkey, sighash_type, value, version);
  ByteData signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash), privkey, has_grind_r);
  SignParameter sign(signature, true, sighash_type);

  AddPubkeyHashSign(outpoint, sign, pubkey, address_type);
}

void TransactionContext::SignWithSchnorrPrivkeySimple(
    const OutPoint& outpoint, const Privkey& privkey,
    const SigHashType& sighash_type, const ByteData256* aux_rand,
    const ByteData* annex) {
  auto sighash =
      CreateSignatureHashByTaproot(outpoint, sighash_type, nullptr, 0, annex);
  SchnorrSignature signature;
  if (aux_rand != nullptr) {
    signature = SchnorrUtil::Sign(sighash, privkey, *aux_rand);
  } else {
    signature = SchnorrUtil::Sign(sighash, privkey);
  }
  signature.SetSigHashType(sighash_type);
  AddSchnorrSign(outpoint, signature, annex);
}

void TransactionContext::AddPubkeyHashSign(
    const OutPoint& outpoint, const SignParameter& signature,
    const Pubkey& pubkey, AddressType address_type) {
  TransactionContextUtil::AddPubkeyHashSign(
      this, outpoint, signature, pubkey, address_type);

  signed_map_.emplace(outpoint, signature.GetSigHashType());
}

void TransactionContext::AddScriptHashSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
    const Script& redeem_script, AddressType address_type,
    bool is_multisig_script) {
  TransactionContextUtil::AddScriptHashSign(
      this, outpoint, signatures, redeem_script, address_type,
      is_multisig_script);

  // TODO(k-matsuzawa): consider to multi-signature.
  // signed_map_.emplace(outpoint, signature.GetSigHashType());
}

void TransactionContext::AddMultisigSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
    const Script& redeem_script, AddressType address_type) {
  std::vector<SignParameter> sign_list =
      TransactionContext::CheckMultisig(signatures, redeem_script);
  AddScriptHashSign(outpoint, sign_list, redeem_script, address_type, true);
}

void TransactionContext::AddSchnorrSign(
    const OutPoint& outpoint, const SchnorrSignature& signature,
    const ByteData* annex) {
  std::vector<SignParameter> sign_params = {
      SignParameter(signature.GetData(true))};
  if (annex != nullptr) sign_params.emplace_back(*annex);
  AddSign(outpoint, sign_params, true, true);
  signed_map_.emplace(outpoint, signature.GetSigHashType());
}

void TransactionContext::AddTapScriptSign(
    const OutPoint& outpoint, const TaprootScriptTree& tree,
    const SchnorrPubkey& internal_pubkey,
    const std::vector<SignParameter>& sign_data_list, const ByteData* annex) {
  std::vector<SignParameter> sign_params = sign_data_list;
  auto control = TaprootUtil::CreateTapScriptControl(internal_pubkey, tree);
  auto script = tree.GetScript();
  sign_params.emplace_back(script);
  sign_params.emplace_back(control);
  if (annex != nullptr) sign_params.emplace_back(*annex);
  AddSign(outpoint, sign_params, true, true);
}

void TransactionContext::AddSign(
    const OutPoint& outpoint, const std::vector<SignParameter>& sign_params,
    bool insert_witness, bool clear_stack) {
  TransactionContextUtil::AddSign(
      this, outpoint, sign_params, insert_witness, clear_stack);
}

bool TransactionContext::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const OutPoint& outpoint,
    SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  auto sighash =
      CreateSignatureHash(outpoint, pubkey, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

bool TransactionContext::VerifyInputSignature(
    const ByteData& signature, const Pubkey& pubkey, const OutPoint& outpoint,
    const Script& script, SigHashType sighash_type, const Amount& value,
    WitnessVersion version) const {
  auto sighash =
      CreateSignatureHash(outpoint, script, sighash_type, value, version);
  return SignatureUtil::VerifyEcSignature(
      ByteData256(sighash.GetBytes()), pubkey, signature);
}

bool TransactionContext::VerifyInputSchnorrSignature(
    const SchnorrSignature& signature, const OutPoint& outpoint,
    const std::vector<UtxoData>& utxo_list, const SchnorrPubkey& pubkey,
    const ByteData* annex) const {
  UtxoData target_utxo;
  std::vector<TxOut> utxos;
  utxos.reserve(vin_.size());
  for (const auto& txin_ref : vin_) {
    OutPoint target_outpoint(txin_ref.GetTxid(), txin_ref.GetVout());
    bool is_find = false;
    for (const auto& utxo : utxo_list) {
      if (txin_ref.GetTxid().Equals(utxo.txid) &&
          (txin_ref.GetVout() == utxo.vout)) {
        is_find = true;
        Script locking_script = GetLockingScriptFromUtxoData(utxo);
        utxos.emplace_back(utxo.amount, locking_script);

        if (outpoint == target_outpoint) {
          target_utxo = utxo;
          target_utxo.locking_script = locking_script;
        }
      }
    }
    if (!is_find) {
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Utxo is not found. VerifyInputSchnorrSignature fail.");
    }
  }
  if (target_utxo.amount == 0) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "OutPoint is not found into utxo_list.");
  } else if (!target_utxo.locking_script.IsTaprootScript()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Target OutPoint is not taproot.");
  }
  auto hash = target_utxo.locking_script.GetElementList()[1].GetBinaryData();
  if (!hash.Equals(pubkey.GetData())) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Unmatch locking script.");
  }

  auto sighash = GetSchnorrSignatureHash(
      GetTxInIndex(outpoint), signature.GetSigHashType(), utxos, nullptr,
      (annex != nullptr) ? *annex : ByteData());
  return pubkey.Verify(signature, sighash);
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

bool TransactionContext::IsFindUtxoMap(
    const OutPoint& outpoint, UtxoData* utxo) const {
  for (const auto& utxo_data : utxo_map_) {
    if ((outpoint.GetVout() == utxo_data.vout) &&
        utxo_data.txid.Equals(outpoint.GetTxid())) {
      if (utxo != nullptr) *utxo = utxo_data;
      return true;
    }
  }
  return false;
}

bool TransactionContext::IsFindOutPoint(
    const std::vector<OutPoint>& list, const OutPoint& outpoint) const {
  for (const auto& target : list) {
    if (outpoint == target) return true;
  }
  return false;
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
  if (this != &transaction) {
    transaction_ = transaction.transaction_;
  }
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

uint32_t TransactionController::GetVsizeIgnoreTxIn() const {
  return AbstractTransaction::GetVsizeFromSize(GetSizeIgnoreTxIn(), 0);
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
