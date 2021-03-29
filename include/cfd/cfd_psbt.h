// Copyright 2021 CryptoGarage
/**
 * @file cfd_psbt.h
 *
 * @brief This file is defines Partially Signed Bitcoin Transaction.
 */
#ifndef CFD_INCLUDE_CFD_CFD_PSBT_H_
#define CFD_INCLUDE_CFD_CFD_PSBT_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_psbt.h"
#include "cfdcore/cfdcore_transaction_common.h"

namespace cfd {

using cfd::core::Descriptor;
using cfd::core::KeyData;
using cfd::core::OutPoint;
using cfd::core::Transaction;

/**
 * @brief The class of Partially Signed Bitcoin Transaction.
 */
class CFD_EXPORT Psbt : public cfd::core::Psbt {
 public:
  /**
   * @brief constructor.
   *
   * for List.
   */
  Psbt();
  /**
   * @brief constructor
   * @param[in] version       tx version
   * @param[in] lock_time     lock time
   */
  explicit Psbt(uint32_t version, uint32_t lock_time);
  /**
   * @brief constructor
   * @param[in] psbt_version  psbt version
   * @param[in] version       tx version
   * @param[in] lock_time     lock time
   */
  explicit Psbt(uint32_t psbt_version, uint32_t version, uint32_t lock_time);
  /**
   * @brief constructor
   * @param[in] base64    base64 string.
   */
  explicit Psbt(const std::string& base64);
  /**
   * @brief constructor
   * @param[in] byte_data   byte data
   */
  explicit Psbt(const ByteData& byte_data);
  /**
   * @brief constructor
   * @param[in] transaction   Transaction object.
   */
  explicit Psbt(const Transaction& transaction);
  /**
   * @brief constructor
   * @param[in] psbt_version  psbt version
   * @param[in] transaction   Transaction object.
   */
  explicit Psbt(uint32_t psbt_version, const Transaction& transaction);
  /**
   * @brief constructor
   * @param[in] context       Transaction object.
   */
  explicit Psbt(const TransactionContext& context);
  /**
   * @brief constructor
   * @param[in] psbt_version  psbt version
   * @param[in] context       Transaction object.
   */
  explicit Psbt(uint32_t psbt_version, const TransactionContext& context);
  /**
   * @brief constructor
   * @param[in] psbt   Psbt object.
   */
  Psbt(const Psbt& psbt);
  /**
   * @brief destructor
   */
  virtual ~Psbt() {}

  /**
   * @brief copy constructor.
   * @param[in] psbt   Psbt object.
   * @return Psbt object.
   */
  Psbt& operator=(const Psbt& psbt) &;

  /**
   * @brief Get transaction context base.
   * @return transaction context.
   */
  TransactionContext GetTransactionContext() const;

  /**
   * @brief Get transaction fee. (only if HasAllUtxos is true.)
   * @retval true   Already set all utxos.
   * @retval false  There is an input for which utxo is not set.
   */
  bool HasAllUtxos() const;

  /**
   * @brief Get transaction fee. (only if HasAllUtxos is true.)
   * @return fee amount
   */
  Amount GetFeeAmount() const;

  using cfd::core::Psbt::IsFinalizedInput;
  /**
   * @brief check finalized input.
   * @param[in] outpoint    outpoint
   * @retval true   already finalized input.
   * @retval false  not finalized input.
   */
  bool IsFinalizedInput(const OutPoint& outpoint) const;

  using cfd::core::Psbt::AddTxIn;
  /**
   * @brief add transaction input.
   * @param[in] outpoint    outpoint
   * @return add index.
   */
  uint32_t AddTxIn(const OutPoint& outpoint);
  /**
   * @brief add transaction input.
   * @param[in] outpoint    outpoint
   * @param[in] sequence    sequence
   * @return add index.
   */
  uint32_t AddTxIn(const OutPoint& outpoint, uint32_t sequence);

  /**
   * @brief add transaction input and utxo data.
   * @param[in] utxo        utxo data
   * @return add index.
   */
  uint32_t AddTxInData(const UtxoData& utxo);
  /**
   * @brief add transaction input and utxo data.
   * @param[in] utxo        utxo data
   * @param[in] sequence    sequence
   * @return add index.
   */
  uint32_t AddTxInData(const UtxoData& utxo, uint32_t sequence);

  /**
   * @brief find txin.
   * @param[in] outpoint    outpoint
   * @param[out] index  txout index.
   * @retval true   exist txin
   * @retval false  txin not found
   */
  bool IsFindTxIn(const OutPoint& outpoint, uint32_t* index = nullptr) const;
  /**
   * @brief Get a txin index.
   * @param[in] outpoint    outpoint
   * @return txin index
   */
  uint32_t GetTxInIndex(const OutPoint& outpoint) const;

  using cfd::core::Psbt::SetTxInUtxo;
  /**
   * @brief set input utxo data.
   * @param[in] outpoint  outpoint
   * @param[in] tx      utxo transaction
   * @param[in] key     utxo related pubkey
   */
  void SetTxInUtxo(
      const OutPoint& outpoint, const Transaction& tx, const KeyData& key);
  /**
   * @brief set input utxo data.
   * @param[in] outpoint  outpoint
   * @param[in] tx      utxo transaction
   * @param[in] redeem_script   utxo related script (only script hash)
   * @param[in] key     utxo related pubkey
   */
  void SetTxInUtxo(
      const OutPoint& outpoint, const Transaction& tx,
      const Script& redeem_script, const KeyData& key);
  /**
   * @brief set input utxo data.
   * @param[in] outpoint  outpoint
   * @param[in] tx      utxo transaction
   * @param[in] redeem_script   utxo related script (only script hash)
   * @param[in] key_list     utxo related pubkey list
   */
  void SetTxInUtxo(
      const OutPoint& outpoint, const Transaction& tx,
      const Script& redeem_script, const std::vector<KeyData>& key_list);
  /**
   * @brief set input utxo data.
   * @param[in] outpoint  outpoint
   * @param[in] txout   utxo witness transaction output
   * @param[in] key     utxo related pubkey
   */
  void SetTxInUtxo(
      const OutPoint& outpoint, const TxOutReference& txout,
      const KeyData& key);
  /**
   * @brief set input utxo data.
   * @param[in] outpoint  outpoint
   * @param[in] txout   utxo witness transaction output
   * @param[in] redeem_script   utxo related script (only script hash)
   * @param[in] key     utxo related pubkey
   */
  void SetTxInUtxo(
      const OutPoint& outpoint, const TxOutReference& txout,
      const Script& redeem_script, const KeyData& key);
  /**
   * @brief set input utxo data.
   * @param[in] outpoint  outpoint
   * @param[in] txout   utxo witness transaction output
   * @param[in] redeem_script   utxo related script (only script hash)
   * @param[in] key_list     utxo related pubkey list
   */
  void SetTxInUtxo(
      const OutPoint& outpoint, const TxOutReference& txout,
      const Script& redeem_script, const std::vector<KeyData>& key_list);

  using cfd::core::Psbt::SetTxInWitnessUtxoDirect;
  /**
   * @brief set input utxo data on direct.
   * @param[in] outpoint    outpoint
   * @param[in] txout       utxo witness transaction output
   */
  void SetTxInWitnessUtxoDirect(
      const OutPoint& outpoint, const TxOutReference& txout);

  using cfd::core::Psbt::SetTxInBip32KeyDirect;
  /**
   * @brief set input bip32 key on direct.
   * @param[in] outpoint    outpoint
   * @param[in] key_data    key data
   */
  void SetTxInBip32KeyDirect(
      const OutPoint& outpoint, const KeyData& key_data);

  using cfd::core::Psbt::SetTxInSignature;
  /**
   * @brief set input signature.
   * @param[in] outpoint  outpoint
   * @param[in] key     utxo related pubkey
   * @param[in] signature   signature data
   */
  void SetTxInSignature(
      const OutPoint& outpoint, const KeyData& key, const ByteData& signature);

  using cfd::core::Psbt::SetTxInSighashType;
  /**
   * @brief set input sighash type.
   * @param[in] outpoint  outpoint
   * @param[in] sighash_type   sighash type
   */
  void SetTxInSighashType(
      const OutPoint& outpoint, const SigHashType& sighash_type);

  using cfd::core::Psbt::SetTxInFinalScript;
  /**
   * @brief set input final script.
   * @param[in] outpoint  outpoint
   * @param[in] unlocking_script   unlocking script data list.
   */
  void SetTxInFinalScript(
      const OutPoint& outpoint, const std::vector<ByteData>& unlocking_script);

  using cfd::core::Psbt::SetTxInRecord;
  /**
   * @brief set input record.
   * @param[in] outpoint  outpoint
   * @param[in] key    key
   * @param[in] value  value
   */
  void SetTxInRecord(
      const OutPoint& outpoint, const ByteData& key, const ByteData& value);

  using cfd::core::Psbt::GetTxInUtxoFull;
  /**
   * @brief get input utxo full transaction.
   * @param[in] outpoint  outpoint
   * @param[in] ignore_error   ignore error with empty data.
   * @param[out] is_witness    has witness.
   * @return utxo transaction
   */
  Transaction GetTxInUtxoFull(
      const OutPoint& outpoint, bool ignore_error = false,
      bool* is_witness = nullptr) const;

  using cfd::core::Psbt::GetTxInUtxo;
  /**
   * @brief get input utxo output data.
   * @param[in] outpoint  outpoint
   * @param[in] ignore_error   ignore error with empty data.
   * @param[out] is_witness    has witness.
   * @return utxo transaction output
   */
  TxOut GetTxInUtxo(
      const OutPoint& outpoint, bool ignore_error = false,
      bool* is_witness = nullptr) const;

  using cfd::core::Psbt::GetTxInRedeemScript;
  /**
   * @brief get input redeem script.
   * @param[in] outpoint  outpoint
   * @param[in] ignore_error   ignore error with empty data.
   * @param[out] is_witness    has witness.
   * @return redeem script (or witness script)
   */
  Script GetTxInRedeemScript(
      const OutPoint& outpoint, bool ignore_error = false,
      bool* is_witness = nullptr) const;

  using cfd::core::Psbt::GetTxInRedeemScriptDirect;
  /**
   * @brief get input redeem script.
   * @param[in] outpoint  outpoint
   * @param[in] ignore_error   ignore error with empty data.
   * @param[in] is_witness     getting target witness.
   * @return redeem script (or witness script)
   */
  Script GetTxInRedeemScriptDirect(
      const OutPoint& outpoint, bool ignore_error = false,
      bool is_witness = true) const;

  using cfd::core::Psbt::GetTxInKeyDataList;
  /**
   * @brief get input key data list.
   * @param[in] outpoint  outpoint
   * @return key data list
   */
  std::vector<KeyData> GetTxInKeyDataList(const OutPoint& outpoint) const;

  using cfd::core::Psbt::GetTxInKeyData;
  /**
   * @brief get input key data (only list top data).
   * @param[in] outpoint  outpoint
   * @param[in] ignore_error   ignore error with empty data.
   * @return key data
   */
  KeyData GetTxInKeyData(
      const OutPoint& outpoint, bool ignore_error = false) const;

  using cfd::core::Psbt::GetTxInSignaturePubkeyList;
  /**
   * @brief get input key data list related to signature.
   * @param[in] outpoint  outpoint
   * @return key data list
   */
  std::vector<Pubkey> GetTxInSignaturePubkeyList(
      const OutPoint& outpoint) const;

  using cfd::core::Psbt::GetTxInSignature;
  /**
   * @brief get input signature related pubkey.
   * @param[in] outpoint  outpoint
   * @param[in] pubkey  pubkey
   * @return signature
   */
  ByteData GetTxInSignature(
      const OutPoint& outpoint, const Pubkey& pubkey) const;

  using cfd::core::Psbt::IsFindTxInSignature;
  /**
   * @brief exist input signature related pubkey.
   * @param[in] outpoint  outpoint
   * @param[in] pubkey  pubkey
   * @retval true  exist signature
   * @retval false  signature not found
   */
  bool IsFindTxInSignature(
      const OutPoint& outpoint, const Pubkey& pubkey) const;

  using cfd::core::Psbt::GetTxInSighashType;
  /**
   * @brief get input sighash type.
   * @param[in] outpoint  outpoint
   * @return sighash type
   */
  SigHashType GetTxInSighashType(const OutPoint& outpoint) const;

  using cfd::core::Psbt::IsFindTxInSighashType;
  /**
   * @brief exist input sighash type.
   * @param[in] outpoint  outpoint
   * @retval true  exist sighash type
   * @retval false  sighash type not found
   */
  bool IsFindTxInSighashType(const OutPoint& outpoint) const;

  using cfd::core::Psbt::GetTxInFinalScript;
  /**
   * @brief get input final script.
   * @param[in] outpoint  outpoint
   * @param[in] is_witness_stack  target witness flag
   * @return witness stack or scriptSig
   */
  std::vector<ByteData> GetTxInFinalScript(
      const OutPoint& outpoint, bool is_witness_stack = true) const;

  using cfd::core::Psbt::GetTxInRecord;
  /**
   * @brief get input record value.
   * @param[in] outpoint  outpoint
   * @param[in] key    record key
   * @return record value
   */
  ByteData GetTxInRecord(const OutPoint& outpoint, const ByteData& key) const;

  using cfd::core::Psbt::IsFindTxInRecord;
  /**
   * @brief exist input record.
   * @param[in] outpoint  outpoint
   * @param[in] key    record key
   * @retval true  exist record
   * @retval false  record not found
   */
  bool IsFindTxInRecord(const OutPoint& outpoint, const ByteData& key) const;

  using cfd::core::Psbt::GetTxInRecordKeyList;
  /**
   * @brief get record key list.
   * @param[in] outpoint  outpoint
   * @return record key list
   */
  std::vector<ByteData> GetTxInRecordKeyList(const OutPoint& outpoint) const;

  using cfd::core::Psbt::ClearTxInSignData;
  /**
   * @brief clear input sign data.
   * @details clear target is redeem script, signature, sighashtype.
   * @param[in] outpoint  outpoint
   */
  void ClearTxInSignData(const OutPoint& outpoint);

  /**
   * @brief set utxo data.
   * @param[in] utxo            utxo
   * @param[in] transaction     transaction
   */
  void SetUtxoData(const UtxoData& utxo, const Transaction& transaction);
  /**
   * @brief set witness utxo.
   * @param[in] utxo    witness utxo
   */
  void SetWitnessUtxoData(const UtxoData& utxo);
  /**
   * @brief collect utxo and cache into utxo_map_.
   * @param[in] utxos   utxo list.
   */
  void CollectInputUtxo(const std::vector<UtxoData>& utxos);

  using cfd::core::Psbt::AddTxOut;
  /**
   * @brief Add txout.
   * @param[in] amount          amount
   * @param[in] address         address
   * @return txout index
   */
  uint32_t AddTxOut(const Amount& amount, const Address& address);
  /**
   * @brief Add txout data.
   * @param[in] amount          amount
   * @param[in] address         address
   * @param[in] key_data        key
   * @return txout index
   */
  uint32_t AddTxOutData(
      const Amount& amount, const Address& address, const KeyData& key_data);
  /**
   * @brief Add txout data.
   * @param[in] amount          amount
   * @param[in] address         address
   * @param[in] redeem_script   redeem script
   * @param[in] key_list        key list
   * @return txout index
   */
  uint32_t AddTxOutData(
      const Amount& amount, const Address& address,
      const Script& redeem_script, const std::vector<KeyData>& key_list);

  // GetTxOutXXXX is see `cfd::core::Psbt`

  /**
   * @brief set ignore verify target.
   * @param[in] outpoint    utxo target.
   */
  void IgnoreVerify(const OutPoint& outpoint);
  /**
   * @brief verify tx sign (signature).
   */
  void Verify() const;
  /**
   * @brief verify tx sign (signature) on outpoint.
   * @param[in] outpoint    utxo target.
   */
  void Verify(const OutPoint& outpoint) const;

  /**
   * @brief fund transaction.
   * @param[in] witness_utxos       witness utxo list
   * @param[in] effective_fee_rate  fee rate
   * @param[in] change_address      change address descriptor.
   * @param[out] estimate_fee       estimate fee
   * @param[in] option_params       coin selection option data
   * @param[in] filter              utxo filter
   * @param[in] net_type            network type
   * @return selected utxo list.
   */
  std::vector<UtxoData> FundTransaction(
      const std::vector<UtxoData>& witness_utxos,
      double effective_fee_rate = 20.0,
      const Descriptor* change_address = nullptr,
      Amount* estimate_fee = nullptr,
      const CoinSelectionOption* option_params = nullptr,
      const UtxoFilter* filter = nullptr,
      NetType net_type = NetType::kMainnet);

  /**
   * @brief Get utxo.
   * @param[in] index       txin index
   * @param[in] net_type    network type
   * @return utxo data.
   */
  UtxoData GetUtxoData(
      uint32_t index, NetType net_type = NetType::kMainnet) const;

  /**
   * @brief Get all utxo.
   * @param[in] net_type    network type
   * @return utxo list.
   */
  std::vector<UtxoData> GetUtxoDataAll(
      NetType net_type = NetType::kMainnet) const;

 private:
  /**
   * @brief utxo verify ignore map. (outpoint)
   */
  std::set<OutPoint> verify_ignore_map_;

  /**
   * @brief Get default sequence from locktime.
   * @retval 0xfffffffe     enable locktime
   * @retval 0xffffffff     disable locktime
   */
  uint32_t GetDefaultSequence() const;
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_PSBT_H_
