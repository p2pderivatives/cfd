// Copyright 2021 CryptoGarage
/**
 * @file cfd_psbt.cpp
 *
 * @brief This file is implements Partially Signed Bitcoin Transaction.
 */

#include "cfd/cfd_psbt.h"

#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_transaction.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_transaction.h"
#include "cfd_transaction_internal.h"  // NOLINT
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_psbt.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_taproot.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "wally_psbt.h"  // NOLINT

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Amount;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::TaprootScriptTree;
using cfd::core::TxIn;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// File internal function
// -----------------------------------------------------------------------------
/**
 * @brief Collect sighash by transaction.
 * @param[in] transaction       transaction
 * @param[in] outpoint          outpoint
 * @param[in] utxo              utxo
 * @param[in] sighash_type      sighash_type
 * @param[in] pubkey            pubkey
 * @param[in] redeem_script     redeem_script
 * @param[in] witness_version   witness_version
 * @return sighash
 */
static ByteData256 CollectSighashByTx(
    const Transaction* transaction, const OutPoint& outpoint,
    const UtxoData& utxo, const SigHashType& sighash_type,
    const Pubkey& pubkey, const Script& redeem_script,
    WitnessVersion witness_version, const ByteData*,
    const TaprootScriptTree*) {
  uint32_t index =
      transaction->GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
  Script script_data = redeem_script;
  if (script_data.IsEmpty()) {
    script_data = ScriptUtil::CreateP2pkhLockingScript(pubkey);
  }
  auto sighash = transaction->GetSignatureHash(
      index, script_data.GetData(), sighash_type, utxo.amount,
      witness_version);
  return sighash;
}

/**
 * @brief parse descriptor
 * @param[in] descriptor      descriptor string.
 * @param[out] key_list       key list.
 * @param[in] net_type        network type
 * @return descriptor script data
 */
cfd::DescriptorScriptData ParseDescriptor(
    const std::string& descriptor, std::vector<KeyData>* key_list,
    NetType* net_type = nullptr) {
  std::vector<KeyData> temp_key_list;
  cfd::DescriptorScriptData script_data;
  if (net_type == nullptr) {
    NetType target_list[] = {
        NetType::kMainnet, NetType::kTestnet, NetType::kRegtest};
    size_t max = (sizeof(target_list) / sizeof(NetType)) - 1;
    for (size_t index = 0; index <= max; ++index) {
      try {
        cfd::AddressFactory addr_factory(target_list[index]);
        script_data = addr_factory.ParseOutputDescriptor(
            descriptor, "", nullptr, nullptr, &temp_key_list);
        break;
      } catch (const CfdException& except) {
        if (index == max) {
          throw except;
        }
      }
    }
  } else {
    cfd::AddressFactory addr_factory(*net_type);
    script_data = addr_factory.ParseOutputDescriptor(
        descriptor, "", nullptr, nullptr, &temp_key_list);
  }
  if (key_list != nullptr) {
    for (const auto& key : temp_key_list) {
      if (key.IsValid() && (!key.GetFingerprint().IsEmpty())) {
        key_list->push_back(key);
      }
    }
  }
  return script_data;
}

/**
 * @brief Convert from utxo data.
 * @param[in] utxo            utxo
 * @param[out] outpoint       outpoint
 * @param[out] txout          txout
 * @param[out] key_list       key data list
 * @param[out] redeem_script  redeem script
 * @param[in] ignore_error    ignore error flag
 * @param[in] net_type        network type
 * @retval true   witness
 * @retval flse   other
 */
bool ConvertFromUtxoData(
    const UtxoData& utxo, OutPoint* outpoint, TxOut* txout,
    std::vector<KeyData>* key_list, Script* redeem_script,
    bool ignore_error = false, NetType* net_type = nullptr) {
  if (utxo.descriptor.empty()) {
    warn(CFD_LOG_SOURCE, "psbt not supported utxo data. need descriptor.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "psbt not supported utxo data. need descriptor.");
  }
  if (outpoint != nullptr) *outpoint = OutPoint(utxo.txid, utxo.vout);

  std::vector<KeyData> work_key_list;
  cfd::DescriptorScriptData script_data =
      ParseDescriptor(utxo.descriptor, &work_key_list, net_type);

  if (key_list != nullptr) {
    for (const auto& key : work_key_list) {
      key_list->push_back(key);
    }
  }
  if (redeem_script != nullptr) *redeem_script = script_data.redeem_script;

  if (script_data.redeem_script.IsEmpty() && work_key_list.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "psbt not supported pubkey format. need fingerprint and bip32 path.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "psbt not supported pubkey format. need fingerprint and bip32 path.");
  }

  if (txout != nullptr) {
    TxOut tmp_txout(utxo.amount, script_data.locking_script);
    *txout = tmp_txout;
  }

  bool is_witness = false;
  if (script_data.locking_script.IsWitnessProgram() ||
      (script_data.address_type == AddressType::kP2shP2wpkhAddress) ||
      (script_data.address_type == AddressType::kP2shP2wshAddress)) {
    is_witness = true;
  } else if (!ignore_error) {
    warn(CFD_LOG_SOURCE, "psbt not supported address type. need segwit.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "psbt not supported address type. need segwit.");
  }
  return is_witness;
}

// -----------------------------------------------------------------------------
// Psbt
// -----------------------------------------------------------------------------
Psbt::Psbt() : cfd::core::Psbt() {}

Psbt::Psbt(uint32_t version, uint32_t lock_time)
    : cfd::core::Psbt(version, lock_time) {}

Psbt::Psbt(uint32_t psbt_version, uint32_t version, uint32_t lock_time)
    : cfd::core::Psbt(psbt_version, version, lock_time) {}

Psbt::Psbt(const std::string& base64) : cfd::core::Psbt(base64) {}
Psbt::Psbt(const ByteData& byte_data) : cfd::core::Psbt(byte_data) {}
Psbt::Psbt(const Transaction& transaction) : cfd::core::Psbt(transaction) {}
Psbt::Psbt(uint32_t psbt_version, const Transaction& transaction)
    : cfd::core::Psbt(psbt_version, transaction) {}

Psbt::Psbt(const TransactionContext& context)
    : Psbt(GetDefaultVersion(), context) {}

Psbt::Psbt(uint32_t psbt_version, const TransactionContext& context)
    : cfd::core::Psbt(psbt_version, static_cast<const Transaction>(context)) {
  std::vector<UtxoData> utxo_list;
  for (const auto& txin : context.GetTxInList()) {
    utxo_list.push_back(context.GetTxInUtxoData(txin.GetOutPoint()));
  }
  CollectInputUtxo(utxo_list);
}

Psbt::Psbt(const Psbt& psbt) : cfd::core::Psbt(psbt.GetData()) {
  verify_ignore_map_ = psbt.verify_ignore_map_;
}

Psbt& Psbt::operator=(const Psbt& psbt) & {
  if (this != &psbt) {
    struct wally_psbt* psbt_pointer = nullptr;
    struct wally_psbt* psbt_src_pointer = nullptr;
    psbt_src_pointer =
        static_cast<struct wally_psbt*>(psbt.wally_psbt_pointer_);
    int ret = wally_psbt_clone_alloc(psbt_src_pointer, 0, &psbt_pointer);
    if (ret != WALLY_OK) {
      warn(CFD_LOG_SOURCE, "wally_psbt_clone_alloc NG[{}]", ret);
      throw CfdException(CfdError::kCfdInternalError, "psbt clone error.");
    }
    cfd::core::Psbt::FreeWallyPsbtAddress(wally_psbt_pointer_);  // free
    wally_psbt_pointer_ = psbt_pointer;
    base_tx_ = cfd::core::Psbt::RebuildTransaction(wally_psbt_pointer_);
    verify_ignore_map_ = psbt.verify_ignore_map_;
  }
  return *this;
}

TransactionContext Psbt::GetTransactionContext() const {
  TransactionContext tx(GetTransaction().GetHex());
  std::vector<UtxoData> utxo_list = GetUtxoDataAll();
  tx.CollectInputUtxo(utxo_list);
  return tx;
}

bool Psbt::HasAllUtxos() const {
  struct wally_psbt* psbt_pointer;
  psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);
  if (psbt_pointer == nullptr) {
    warn(CFD_LOG_SOURCE, "psbt pointer is null");
    throw CfdException(
        CfdError::kCfdIllegalStateError, "psbt pointer is null.");
  } else if (psbt_pointer->tx == nullptr) {
    warn(CFD_LOG_SOURCE, "psbt base tx is null");
    throw CfdException(
        CfdError::kCfdIllegalStateError, "psbt base tx is null.");
  }

  for (size_t index = 0; index < psbt_pointer->num_inputs; ++index) {
    if ((psbt_pointer->inputs[index].witness_utxo == nullptr) &&
        (psbt_pointer->inputs[index].utxo == nullptr)) {
      return false;
    }
  }
  return (psbt_pointer->num_inputs != 0);
}

Amount Psbt::GetFeeAmount() const {
  if (!HasAllUtxos()) return Amount();
  struct wally_psbt* psbt_pointer;
  psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);

  Amount total_input;
  for (size_t index = 0; index < psbt_pointer->num_inputs; ++index) {
    if (psbt_pointer->inputs[index].witness_utxo != nullptr) {
      total_input += psbt_pointer->inputs[index].witness_utxo->satoshi;
    } else if (psbt_pointer->inputs[index].utxo != nullptr) {
      uint32_t vout = psbt_pointer->tx->inputs[index].index;
      if (psbt_pointer->inputs[index].utxo->num_outputs <= vout) {
        warn(CFD_LOG_SOURCE, "psbt txin vout is invalid.");
        throw CfdException(
            CfdError::kCfdIllegalStateError, "psbt txin vout is invalid.");
      }
      total_input += psbt_pointer->inputs[index].utxo->outputs[vout].satoshi;
    } else {
      warn(CFD_LOG_SOURCE, "psbt txin utxo is empty.");
      throw CfdException(
          CfdError::kCfdIllegalStateError, "psbt txin utxo is empty.");
    }
  }

  Amount total_output;
  for (size_t index = 0; index < psbt_pointer->tx->num_outputs; ++index) {
    total_output += psbt_pointer->tx->outputs[index].satoshi;
  }

  if (total_input <= total_output) return Amount();
  return total_input - total_output;
}

bool Psbt::IsFinalizedInput(const OutPoint& outpoint) const {
  return cfd::core::Psbt::IsFinalizedInput(GetTxInIndex(outpoint));
}

uint32_t Psbt::AddTxIn(const OutPoint& outpoint) {
  return AddTxIn(outpoint, GetDefaultSequence());
}

uint32_t Psbt::AddTxIn(const OutPoint& outpoint, uint32_t sequence) {
  return cfd::core::Psbt::AddTxIn(
      outpoint.GetTxid(), outpoint.GetVout(), sequence);
}

uint32_t Psbt::AddTxInData(const UtxoData& utxo) {
  return AddTxInData(utxo, GetDefaultSequence());
}

uint32_t Psbt::AddTxInData(const UtxoData& utxo, uint32_t sequence) {
  OutPoint outpoint(utxo.txid, utxo.vout);
  TxOut txout;
  std::vector<KeyData> key_list;
  Script redeem_script;
  ConvertFromUtxoData(utxo, nullptr, &txout, &key_list, &redeem_script);

  uint32_t index = AddTxIn(outpoint, sequence);
  try {
    cfd::core::Psbt::SetTxInUtxo(
        index, TxOutReference(txout), redeem_script, key_list);
  } catch (const CfdException& except) {
    struct wally_psbt* psbt_pointer;
    psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);
    wally_psbt_remove_input(psbt_pointer, index);
    base_tx_ = cfd::core::Psbt::RebuildTransaction(wally_psbt_pointer_);
    throw except;
  }
  return index;
}

bool Psbt::IsFindTxIn(const OutPoint& outpoint, uint32_t* index) const {
  static constexpr const char* const kErrorMessage = "Txid is not found.";
  try {
    uint32_t temp_index =
        base_tx_.GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
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

uint32_t Psbt::GetTxInIndex(const OutPoint& outpoint) const {
  return base_tx_.GetTxInIndex(outpoint.GetTxid(), outpoint.GetVout());
}

void Psbt::SetTxInUtxo(
    const OutPoint& outpoint, const Transaction& tx, const KeyData& key) {
  cfd::core::Psbt::SetTxInUtxo(GetTxInIndex(outpoint), tx, key);
}

void Psbt::SetTxInUtxo(
    const OutPoint& outpoint, const Transaction& tx,
    const Script& redeem_script, const KeyData& key) {
  cfd::core::Psbt::SetTxInUtxo(GetTxInIndex(outpoint), tx, redeem_script, key);
}

void Psbt::SetTxInUtxo(
    const OutPoint& outpoint, const Transaction& tx,
    const Script& redeem_script, const std::vector<KeyData>& key_list) {
  cfd::core::Psbt::SetTxInUtxo(
      GetTxInIndex(outpoint), tx, redeem_script, key_list);
}

void Psbt::SetTxInUtxo(
    const OutPoint& outpoint, const TxOutReference& txout,
    const KeyData& key) {
  cfd::core::Psbt::SetTxInUtxo(GetTxInIndex(outpoint), txout, key);
}

void Psbt::SetTxInUtxo(
    const OutPoint& outpoint, const TxOutReference& txout,
    const Script& redeem_script, const KeyData& key) {
  cfd::core::Psbt::SetTxInUtxo(
      GetTxInIndex(outpoint), txout, redeem_script, key);
}

void Psbt::SetTxInUtxo(
    const OutPoint& outpoint, const TxOutReference& txout,
    const Script& redeem_script, const std::vector<KeyData>& key_list) {
  cfd::core::Psbt::SetTxInUtxo(
      GetTxInIndex(outpoint), txout, redeem_script, key_list);
}

void Psbt::SetTxInWitnessUtxoDirect(
    const OutPoint& outpoint, const TxOutReference& txout) {
  cfd::core::Psbt::SetTxInWitnessUtxoDirect(GetTxInIndex(outpoint), txout);
}

void Psbt::SetTxInBip32KeyDirect(
    const OutPoint& outpoint, const KeyData& key_data) {
  cfd::core::Psbt::SetTxInBip32KeyDirect(GetTxInIndex(outpoint), key_data);
}

void Psbt::SetTxInSignature(
    const OutPoint& outpoint, const KeyData& key, const ByteData& signature) {
  cfd::core::Psbt::SetTxInSignature(GetTxInIndex(outpoint), key, signature);
}

void Psbt::SetTxInSighashType(
    const OutPoint& outpoint, const SigHashType& sighash_type) {
  cfd::core::Psbt::SetTxInSighashType(GetTxInIndex(outpoint), sighash_type);
}

void Psbt::SetTxInFinalScript(
    const OutPoint& outpoint, const std::vector<ByteData>& unlocking_script) {
  cfd::core::Psbt::SetTxInFinalScript(
      GetTxInIndex(outpoint), unlocking_script);
}

void Psbt::SetTxInRecord(
    const OutPoint& outpoint, const ByteData& key, const ByteData& value) {
  cfd::core::Psbt::SetTxInRecord(GetTxInIndex(outpoint), key, value);
}

Transaction Psbt::GetTxInUtxoFull(
    const OutPoint& outpoint, bool ignore_error, bool* is_witness) const {
  return cfd::core::Psbt::GetTxInUtxoFull(
      GetTxInIndex(outpoint), ignore_error, is_witness);
}

TxOut Psbt::GetTxInUtxo(
    const OutPoint& outpoint, bool ignore_error, bool* is_witness) const {
  return cfd::core::Psbt::GetTxInUtxo(
      GetTxInIndex(outpoint), ignore_error, is_witness);
}
Script Psbt::GetTxInRedeemScript(
    const OutPoint& outpoint, bool ignore_error, bool* is_witness) const {
  return cfd::core::Psbt::GetTxInRedeemScript(
      GetTxInIndex(outpoint), ignore_error, is_witness);
}

Script Psbt::GetTxInRedeemScriptDirect(
    const OutPoint& outpoint, bool ignore_error, bool is_witness) const {
  return cfd::core::Psbt::GetTxInRedeemScriptDirect(
      GetTxInIndex(outpoint), ignore_error, is_witness);
}
std::vector<KeyData> Psbt::GetTxInKeyDataList(const OutPoint& outpoint) const {
  return cfd::core::Psbt::GetTxInKeyDataList(GetTxInIndex(outpoint));
}
KeyData Psbt::GetTxInKeyData(
    const OutPoint& outpoint, bool ignore_error) const {
  return cfd::core::Psbt::GetTxInKeyData(GetTxInIndex(outpoint), ignore_error);
}
std::vector<Pubkey> Psbt::GetTxInSignaturePubkeyList(
    const OutPoint& outpoint) const {
  return cfd::core::Psbt::GetTxInSignaturePubkeyList(GetTxInIndex(outpoint));
}
ByteData Psbt::GetTxInSignature(
    const OutPoint& outpoint, const Pubkey& pubkey) const {
  return cfd::core::Psbt::GetTxInSignature(GetTxInIndex(outpoint), pubkey);
}
bool Psbt::IsFindTxInSignature(
    const OutPoint& outpoint, const Pubkey& pubkey) const {
  return cfd::core::Psbt::IsFindTxInSignature(GetTxInIndex(outpoint), pubkey);
}

SigHashType Psbt::GetTxInSighashType(const OutPoint& outpoint) const {
  return cfd::core::Psbt::GetTxInSighashType(GetTxInIndex(outpoint));
}

bool Psbt::IsFindTxInSighashType(const OutPoint& outpoint) const {
  return cfd::core::Psbt::IsFindTxInSighashType(GetTxInIndex(outpoint));
}

std::vector<ByteData> Psbt::GetTxInFinalScript(
    const OutPoint& outpoint, bool is_witness_stack) const {
  return cfd::core::Psbt::GetTxInFinalScript(
      GetTxInIndex(outpoint), is_witness_stack);
}

ByteData Psbt::GetTxInRecord(
    const OutPoint& outpoint, const ByteData& key) const {
  return cfd::core::Psbt::GetTxInRecord(GetTxInIndex(outpoint), key);
}

bool Psbt::IsFindTxInRecord(
    const OutPoint& outpoint, const ByteData& key) const {
  return cfd::core::Psbt::IsFindTxInRecord(GetTxInIndex(outpoint), key);
}

std::vector<ByteData> Psbt::GetTxInRecordKeyList(
    const OutPoint& outpoint) const {
  return cfd::core::Psbt::GetTxInRecordKeyList(GetTxInIndex(outpoint));
}

void Psbt::ClearTxInSignData(const OutPoint& outpoint) {
  cfd::core::Psbt::ClearTxInSignData(GetTxInIndex(outpoint));
}

void Psbt::SetUtxoData(const UtxoData& utxo, const Transaction& transaction) {
  OutPoint outpoint;
  std::vector<KeyData> key_list;
  Script redeem_script;
  ConvertFromUtxoData(
      utxo, &outpoint, nullptr, &key_list, &redeem_script, true);

  SetTxInUtxo(outpoint, transaction, redeem_script, key_list);
}

void Psbt::SetWitnessUtxoData(const UtxoData& utxo) {
  OutPoint outpoint;
  std::vector<KeyData> key_list;
  Script redeem_script;
  TxOut txout;
  ConvertFromUtxoData(utxo, &outpoint, &txout, &key_list, &redeem_script);
  SetTxInUtxo(outpoint, TxOutReference(txout), redeem_script, key_list);
}

void Psbt::CollectInputUtxo(const std::vector<UtxoData>& utxos) {
  OutPoint outpoint;
  TxOut txout;
  std::vector<KeyData> key_list;
  Script redeem_script;
  for (const auto& utxo : utxos) {
    if (utxo.descriptor.empty()) continue;
    key_list.clear();
    bool is_witness = ConvertFromUtxoData(
        utxo, &outpoint, &txout, &key_list, &redeem_script, true);
    if (is_witness) {
      SetTxInUtxo(outpoint, TxOutReference(txout), redeem_script, key_list);
    }
  }
}

uint32_t Psbt::AddTxOut(const Amount& amount, const Address& address) {
  return cfd::core::Psbt::AddTxOut(address.GetLockingScript(), amount);
}

uint32_t Psbt::AddTxOutData(
    const Amount& amount, const Address& address, const KeyData& key_data) {
  uint32_t index =
      cfd::core::Psbt::AddTxOut(address.GetLockingScript(), amount);
  try {
    SetTxOutData(index, key_data);
  } catch (const CfdException& except) {
    struct wally_psbt* psbt_pointer;
    psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);
    wally_psbt_remove_output(psbt_pointer, index);
    base_tx_ = cfd::core::Psbt::RebuildTransaction(wally_psbt_pointer_);
    throw except;
  }
  return index;
}

uint32_t Psbt::AddTxOutData(
    const Amount& amount, const Address& address, const Script& redeem_script,
    const std::vector<KeyData>& key_list) {
  uint32_t index =
      cfd::core::Psbt::AddTxOut(address.GetLockingScript(), amount);
  try {
    SetTxOutData(index, redeem_script, key_list);
  } catch (const CfdException& except) {
    struct wally_psbt* psbt_pointer;
    psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);
    wally_psbt_remove_output(psbt_pointer, index);
    base_tx_ = cfd::core::Psbt::RebuildTransaction(wally_psbt_pointer_);
    throw except;
  }
  return 0;
}

void Psbt::IgnoreVerify(const OutPoint& outpoint) {
  verify_ignore_map_.emplace(outpoint);
}

void Psbt::Verify() const {
  if (!HasAllUtxos()) {
    warn(CFD_LOG_SOURCE, "psbt utxo not set.");
    throw CfdException(CfdError::kCfdIllegalStateError, "psbt utxo not set.");
  }

  struct wally_psbt* psbt_pointer;
  psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);
  uint32_t max = static_cast<uint32_t>(psbt_pointer->tx->num_inputs);
  for (uint32_t index = 0; index < max; ++index) {
    OutPoint outpoint(
        Txid(ByteData256(ByteData(
            psbt_pointer->tx->inputs[index].txhash,
            sizeof(psbt_pointer->tx->inputs[index].txhash)))),
        psbt_pointer->tx->inputs[index].index);
    Verify(outpoint);
  }
}

void Psbt::Verify(const OutPoint& outpoint) const {
  if (verify_ignore_map_.find(outpoint) == verify_ignore_map_.end()) {
    auto tx = GetTransaction();
    uint32_t index = GetTxInIndex(outpoint);
    if (!IsFinalizedInput(index)) {
      warn(CFD_LOG_SOURCE, "psbt txin not finalized yet.");
      throw CfdException(
          CfdError::kCfdIllegalStateError, "psbt txin not finalized yet.");
    }

    UtxoData utxo = GetUtxoData(index);
    TxIn txin(utxo.txid, utxo.vout, tx.GetTxIn(index).GetSequence());
    ByteData scriptsig = cfd::core::Psbt::GetTxInFinalScript(index, false)[0];
    if (!scriptsig.IsEmpty()) txin.SetUnlockingScript(Script(scriptsig));
    auto witness_stack = cfd::core::Psbt::GetTxInFinalScript(index, true);
    for (const auto& stack : witness_stack) {
      txin.AddScriptWitnessStack(stack);
    }

    TransactionContextUtil::Verify<Transaction>(
        &tx, outpoint, utxo, &txin, CollectSighashByTx);
  }
}

std::vector<UtxoData> Psbt::FundTransaction(
    const std::vector<UtxoData>& witness_utxos, double effective_fee_rate,
    const Descriptor* change_address, Amount* estimate_fee,
    const CoinSelectionOption* option_params, const UtxoFilter* filter,
    NetType net_type) {
  std::vector<UtxoData> input_utxos = GetUtxoDataAll();
  for (const auto& utxo : witness_utxos) {
    ConvertFromUtxoData(
        utxo, nullptr, nullptr, nullptr, nullptr, false, &net_type);
  }

  std::string reserve_txout_address;
  if (change_address != nullptr) {
    auto key_list = change_address->GetKeyDataAll();
    if (key_list.empty()) {
      warn(CFD_LOG_SOURCE, "psbt change address's KeyData is empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "psbt change address's KeyData is empty.");
    }
    auto ref = change_address->GetReference();
    switch (ref.GetAddressType()) {
      case AddressType::kP2wpkhAddress:
      case AddressType::kP2wshAddress:
      case AddressType::kP2shP2wpkhAddress:
      case AddressType::kP2shP2wshAddress:
        // do nothing
        break;
      case AddressType::kP2pkhAddress:
      case AddressType::kP2shAddress:
      default:
        warn(CFD_LOG_SOURCE, "psbt change address's address not segwit.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "psbt change address's address not segwit.");
        break;
    }
    reserve_txout_address = ref.GenerateAddress(net_type).GetAddress();
  }
  cfd::api::TransactionApi api;
  std::vector<std::string> append_txout_addresses;
  // TODO(k-matsuzawa) move to TransactionContext
  auto fund_tx = api.FundRawTransaction(
      base_tx_.GetHex(), witness_utxos, Amount(), input_utxos,
      reserve_txout_address, effective_fee_rate, estimate_fee, filter,
      option_params, &append_txout_addresses, net_type);
  auto fund_basetx = fund_tx.GetTransaction();

  std::vector<UtxoData> append_utxos;
  if (base_tx_.GetTxInCount() < fund_basetx.GetTxInCount()) {
    auto txin_list = fund_basetx.GetTxInList();
    for (const auto& txin : txin_list) {
      if (!IsFindTxIn(txin.GetOutPoint())) {
        auto outpoint = txin.GetOutPoint();
        for (const auto& utxo : witness_utxos) {
          if ((utxo.vout == outpoint.GetVout()) &&
              (utxo.txid.Equals(outpoint.GetTxid()))) {
            append_utxos.push_back(utxo);
            break;
          }
        }
      }
    }
  }
  if (base_tx_.GetTxOutCount() < fund_basetx.GetTxOutCount()) {
    if ((fund_basetx.GetTxOutCount() - base_tx_.GetTxOutCount()) > 1) {
      warn(
          CFD_LOG_SOURCE, "psbt txout count invalid. [{},{}]",
          fund_basetx.GetTxOutCount(), base_tx_.GetTxOutCount());
      throw CfdException(
          CfdError::kCfdInternalError, "psbt txout count invalid.");
    }
    if (append_txout_addresses.size() != 1) {
      warn(
          CFD_LOG_SOURCE, "psbt txout append address invalid. [{}]",
          append_txout_addresses.size());
      throw CfdException(
          CfdError::kCfdInternalError, "psbt txout append address invalid.");
    }
    if (change_address != nullptr) {
      std::vector<KeyData> key_list;
      cfd::DescriptorScriptData script_data =
          ParseDescriptor(change_address->ToString(), &key_list, &net_type);
      auto txout = fund_basetx.GetTxOut(base_tx_.GetTxOutCount());
      if (!script_data.locking_script.Equals(txout.GetLockingScript())) {
        warn(
            CFD_LOG_SOURCE, "psbt txout append locking script invalid. [{}]",
            txout.GetLockingScript().GetHex());
        throw CfdException(
            CfdError::kCfdInternalError,
            "psbt txout append locking script invalid.");
      }
      AddTxOutData(
          txout.GetValue(), script_data.address, script_data.redeem_script,
          key_list);
    }
  }
  for (const auto& utxo : append_utxos) {
    AddTxInData(utxo);
  }
  base_tx_ = cfd::core::Psbt::RebuildTransaction(wally_psbt_pointer_);
  return append_utxos;
}

uint32_t Psbt::GetDefaultSequence() const {
  return (base_tx_.GetLockTime() == 0) ? kSequenceDisableLockTime
                                       : kSequenceEnableLockTimeMax;
}

UtxoData Psbt::GetUtxoData(uint32_t index, NetType net_type) const {
  CheckTxInIndex(index, __LINE__, __FUNCTION__);
  struct wally_psbt* psbt_pointer;
  psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);
  UtxoData utxo;
  utxo.block_height = 0;
  utxo.address_type = AddressType::kP2shAddress;

  utxo.txid = Txid(ByteData256(ByteData(
      psbt_pointer->tx->inputs[index].txhash,
      sizeof(psbt_pointer->tx->inputs[index].txhash))));
  utxo.vout = psbt_pointer->tx->inputs[index].index;

  bool witness_only = false;
  if (psbt_pointer->inputs[index].witness_utxo != nullptr) {
    utxo.amount = Amount(static_cast<int64_t>(
        psbt_pointer->inputs[index].witness_utxo->satoshi));
    utxo.locking_script = Script(ByteData(
        psbt_pointer->inputs[index].witness_utxo->script,
        static_cast<uint32_t>(
            psbt_pointer->inputs[index].witness_utxo->script_len)));
    witness_only = true;
  } else if (psbt_pointer->inputs[index].utxo != nullptr) {
    if (psbt_pointer->inputs[index].utxo->num_outputs > utxo.vout) {
      auto output = &psbt_pointer->inputs[index].utxo->outputs[utxo.vout];
      utxo.amount = Amount(static_cast<int64_t>(output->satoshi));
      utxo.locking_script = Script(
          ByteData(output->script, static_cast<uint32_t>(output->script_len)));
    }
  }
  if (utxo.locking_script.IsP2shScript()) {
    utxo.address_type = AddressType::kP2shAddress;
  } else if (utxo.locking_script.IsP2pkhScript()) {
    utxo.address_type = AddressType::kP2pkhAddress;
  } else if (utxo.locking_script.IsP2wpkhScript()) {
    utxo.address_type = AddressType::kP2wpkhAddress;
  } else if (utxo.locking_script.IsP2wshScript()) {
    utxo.address_type = AddressType::kP2wshAddress;
  }

  std::vector<KeyData> key_list;
  bool is_finalized = IsFinalizedInput(index);
  if (is_finalized) {
    if (witness_only &&
        (psbt_pointer->inputs[index].final_witness != nullptr)) {
      auto witness_stack = psbt_pointer->inputs[index].final_witness;
      uint32_t last_index = static_cast<uint32_t>(witness_stack->num_items);
      if (last_index > 0) {
        ByteData last_data(
            witness_stack->items[last_index - 1].witness,
            static_cast<uint32_t>(
                witness_stack->items[last_index - 1].witness_len));
        if (Pubkey::IsValid(last_data)) {
          key_list.emplace_back(KeyData(Pubkey(last_data), "", ByteData()));
          if (utxo.address_type == AddressType::kP2shAddress) {
            utxo.address_type = AddressType::kP2shP2wpkhAddress;
          }
        } else {
          utxo.redeem_script = Script(last_data);
          if (utxo.address_type == AddressType::kP2shAddress) {
            utxo.address_type = AddressType::kP2shP2wshAddress;
          }
          if (utxo.redeem_script.IsP2pkScript()) {
            key_list.emplace_back(KeyData(
                Pubkey(utxo.redeem_script.GetElementList()[0].GetData()), "",
                ByteData()));
          }
        }
      }
    } else if (psbt_pointer->inputs[index].final_scriptsig != nullptr) {
      Script scriptsig(ByteData(
          psbt_pointer->inputs[index].final_scriptsig,
          static_cast<uint32_t>(
              psbt_pointer->inputs[index].final_scriptsig_len)));
      auto items = scriptsig.GetElementList();
      uint32_t last_index = static_cast<uint32_t>(items.size());
      if (last_index > 0) {
        ByteData last_data = items[last_index - 1].GetBinaryData();
        if (Pubkey::IsValid(last_data)) {
          key_list.emplace_back(KeyData(Pubkey(last_data), "", ByteData()));
        } else {
          utxo.redeem_script = Script(last_data);
          if (utxo.redeem_script.IsP2pkScript()) {
            key_list.emplace_back(KeyData(
                Pubkey(utxo.redeem_script.GetElementList()[0].GetData()), "",
                ByteData()));
          }
        }
      }
    }
  } else if (witness_only) {
    if (psbt_pointer->inputs[index].witness_script != nullptr) {
      utxo.redeem_script = Script(ByteData(
          psbt_pointer->inputs[index].witness_script,
          static_cast<uint32_t>(
              psbt_pointer->inputs[index].witness_script_len)));
      if (utxo.address_type == AddressType::kP2shAddress) {
        utxo.address_type = AddressType::kP2shP2wshAddress;
      }
    } else if (utxo.address_type == AddressType::kP2shAddress) {
      utxo.address_type = AddressType::kP2shP2wpkhAddress;
    }

    key_list = cfd::core::Psbt::GetTxInKeyDataList(index);
  } else {
    if (psbt_pointer->inputs[index].redeem_script != nullptr) {
      utxo.redeem_script = Script(ByteData(
          psbt_pointer->inputs[index].redeem_script,
          static_cast<uint32_t>(
              psbt_pointer->inputs[index].redeem_script_len)));
    }
    key_list = cfd::core::Psbt::GetTxInKeyDataList(index);
  }

  if ((!is_finalized) &&
      (psbt_pointer->inputs[index].keypaths.num_items == 0)) {
    // unset descriptor
  } else if (
      utxo.locking_script.IsP2wpkhScript() ||
      (utxo.locking_script.IsP2shScript() && utxo.redeem_script.IsEmpty())) {
    if (key_list.size() != 1) {
      warn(CFD_LOG_SOURCE, "psbt not supported. many pubkey for p2wpkh.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "psbt not supported. many pubkey for p2wpkh.");
    }
    utxo.descriptor = "wpkh(" + key_list[0].ToString() + ")";
    if (utxo.locking_script.IsP2shScript()) {
      utxo.descriptor = "sh(" + utxo.descriptor + ")";
    }
  } else if (
      utxo.locking_script.IsP2wshScript() ||
      utxo.locking_script.IsP2shScript()) {
    if (utxo.redeem_script.IsMultisigScript()) {
      uint32_t req_num = 0;
      auto multisig_key_list = ScriptUtil::ExtractPubkeysFromMultisigScript(
          utxo.redeem_script, &req_num);
      utxo.descriptor = "multi(" + std::to_string(req_num);
      for (const auto& pubkey : multisig_key_list) {
        bool is_find = false;
        for (const auto& key : key_list) {
          if (pubkey.Equals(key.GetPubkey())) {
            utxo.descriptor += "," + key.ToString();
            is_find = true;
          }
        }
        if (!is_find) {
          utxo.descriptor += "," + pubkey.GetHex();
        }
      }
      utxo.descriptor += ")";
    } else if (utxo.redeem_script.IsP2pkScript()) {
      if (key_list.size() != 1) {
        warn(CFD_LOG_SOURCE, "psbt not supported. many pubkey for p2pkh.");
        throw CfdException(
            CfdError::kCfdIllegalStateError,
            "psbt not supported. many pubkey for p2pkh.");
      }
      utxo.descriptor = "pkh(" + key_list[0].ToString() + ")";
    } else {
      // not key support
      utxo.descriptor = utxo.redeem_script.GetHex();
    }
    if (witness_only) {
      utxo.descriptor = "wsh(" + utxo.descriptor + ")";
    }
    if (utxo.locking_script.IsP2shScript()) {
      utxo.descriptor = "sh(" + utxo.descriptor + ")";
    }
  } else if (utxo.locking_script.IsP2pkhScript()) {
    if (key_list.size() != 1) {
      warn(CFD_LOG_SOURCE, "psbt not supported. many pubkey for p2pkh.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "psbt not supported. many pubkey for p2pkh.");
    }
    utxo.descriptor = "pkh(" + key_list[0].ToString() + ")";
  } else {
    // unsupported format
  }

  if (!utxo.descriptor.empty()) {
    Descriptor desc = Descriptor::Parse(utxo.descriptor);
    auto ref = desc.GetReference();
    if (!ref.GetLockingScript().Equals(utxo.locking_script)) {
      warn(
          CFD_LOG_SOURCE,
          "psbt invalid state. unmatch descriptor & lockingScript.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "psbt invalid state. unmatch descriptor & lockingScript.");
    }
    utxo.address = ref.GenerateAddress(net_type);
    utxo.address_type = ref.GetAddressType();
  }

  return utxo;
}

std::vector<UtxoData> Psbt::GetUtxoDataAll(NetType net_type) const {
  std::vector<UtxoData> list;
  if (!HasAllUtxos()) return list;

  struct wally_psbt* psbt_pointer;
  psbt_pointer = static_cast<struct wally_psbt*>(wally_psbt_pointer_);
  uint32_t max = static_cast<uint32_t>(psbt_pointer->num_inputs);
  for (uint32_t index = 0; index < max; ++index) {
    list.emplace_back(GetUtxoData(index, net_type));
  }
  return list;
}

}  // namespace cfd
