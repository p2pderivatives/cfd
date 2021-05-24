// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction.h
 *
 * @brief Related class definition for Transaction operation
 */
#ifndef CFD_INCLUDE_CFD_CFD_TRANSACTION_H_
#define CFD_INCLUDE_CFD_CFD_TRANSACTION_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_schnorrsig.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_taproot.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::OutPoint;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::SchnorrPubkey;
using cfd::core::SchnorrSignature;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::TaprootScriptTree;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::TxInReference;
using cfd::core::WitnessVersion;

/**
 * @brief Context class for generating bitcoin transaction.
 */
class CFD_EXPORT TransactionContext : public Transaction {
 public:
  /**
   * @brief constructor.
   */
  TransactionContext();
  /**
   * @brief constructor.
   * @param[in] version   Transaction version
   * @param[in] locktime  Timestamp or block height
   */
  explicit TransactionContext(uint32_t version, uint32_t locktime);
  /**
   * @brief constructor.
   * @param[in] tx_hex  Raw Transaction HEX string
   */
  explicit TransactionContext(const std::string& tx_hex);
  /**
   * @brief constructor.
   * @param[in] byte_data  Raw Transaction Data
   */
  explicit TransactionContext(const ByteData& byte_data);
  /**
   * @brief constructor
   * @param[in] context   Transaction Context
   */
  TransactionContext(const TransactionContext& context);
  /**
   * @brief constructor
   * @param[in] transaction   Transaction
   */
  explicit TransactionContext(const Transaction& transaction);
  /**
   * @brief destructor.
   */
  virtual ~TransactionContext() {
    // do nothing
  }
  /**
   * @brief copy constructor.
   * @param[in] context   Transaction context
   * @return Transaction context
   */
  TransactionContext& operator=(const TransactionContext& context) &;

  /**
   * @brief Transaction's GetTxInIndex.
   */
  using Transaction::GetTxInIndex;
  /**
   * @brief Get the index of TxIn.
   * @param[in] outpoint  TxIn txid and vout
   * @return TxIn index
   */
  virtual uint32_t GetTxInIndex(const OutPoint& outpoint) const;
  /**
   * @brief Transaction's GetTxOutIndex.
   */
  using Transaction::GetTxOutIndex;
  /**
   * @brief Get the index of TxOut.
   * @param[in] address  address
   * @return TxOut index
   */
  virtual uint32_t GetTxOutIndex(const Address& address) const;
  /**
   * @brief Exist for TxIn.
   * @param[in] outpoint  TxIn txid and vout
   * @param[out] index  txout index.
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindTxIn(const OutPoint& outpoint, uint32_t* index = nullptr) const;
  /**
   * @brief Check if TxOut exists.
   * @param[in] locking_script  locking script
   * @param[out] index  txout index.
   * @param[out] indexes  txout index list.
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindTxOut(
      const Script& locking_script, uint32_t* index = nullptr,
      std::vector<uint32_t>* indexes = nullptr) const;
  /**
   * @brief Check if TxOut exists.
   * @param[in] address  address
   * @param[out] index  txout index.
   * @param[out] indexes  txout index list.
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindTxOut(
      const Address& address, uint32_t* index = nullptr,
      std::vector<uint32_t>* indexes = nullptr) const;
  /**
   * @brief Transaction's GetTxIn.
   */
  using Transaction::GetTxIn;
  /**
   * @brief Get txin by outpoint.
   * @param[in] outpoint  target outpoint.
   * @return TxInReference
   */
  const TxInReference GetTxIn(const OutPoint& outpoint) const;
  /**
   * @brief Get txout address by index.
   * @param[in] index     txout index.
   * @param[in] net_type  network type.
   * @return address
   */
  Address GetTxOutAddress(
      uint32_t index, NetType net_type = NetType::kMainnet) const;

  /**
   * @brief Transaction's AddTxIn.
   */
  using Transaction::AddTxIn;
  /**
   * @brief Add TxIn.
   * @param[in] outpoint  TxIn txid and vout
   * @return Index position of added TxIn
   */
  virtual uint32_t AddTxIn(const OutPoint& outpoint);
  /**
   * @brief Transaction's AddTxOut.
   */
  using Transaction::AddTxOut;
  /**
   * @brief Add TxOut.
   * @param[in] address  Remittance destination address
   * @param[in] value  Amount
   * @return TxOut Index
   */
  virtual uint32_t AddTxOut(const Address& address, const Amount& value);

  /**
   * @brief Get the size excluding TxIn.
   * @param[in] use_witness     witness use flag.
   * @return Tx size excluding TxIn
   */
  uint32_t GetSizeIgnoreTxIn(bool use_witness = true) const;

  /**
   * @brief Get the virtual size excluding TxIn.
   * @param[in] use_witness     witness use flag.
   * @return Tx virtual size excluding TxIn
   */
  uint32_t GetVsizeIgnoreTxIn(bool use_witness = true) const;

  // state-sequence-api
  /**
   * @brief add txin with utxo.
   * @param[in] utxo  UTXO
   */
  void AddInput(const UtxoData& utxo);
  /**
   * @brief add txin with utxo.
   * @param[in] utxo      UTXO
   * @param[in] sequence  unlock sequence value.
   */
  void AddInput(const UtxoData& utxo, uint32_t sequence);
  /**
   * @brief add txins with utxo.
   * @param[in] utxos   UTXO's
   */
  void AddInputs(const std::vector<UtxoData>& utxos);
  /**
   * @brief collect utxo and cache into utxo_map_.
   * @param[in] utxos   utxo list.
   */
  void CollectInputUtxo(const std::vector<UtxoData>& utxos);
  /**
   * @brief Get UTXO by outpoint
   * @param[in] outpoint  TxIn txid and vout
   * @return UTXO
   */
  UtxoData GetTxInUtxoData(const OutPoint& outpoint) const;
  /**
   * @brief Get the fee amount of TxOut.
   * @return the fee amount
   */
  Amount GetFeeAmount() const;

  /**
   * @brief Split output amount.
   * @param[in] index           txout index
   * @param[in] amount_list     amount list
   * @param[in] address_list    address list
   */
  void SplitTxOut(
      uint32_t index, const std::vector<Amount>& amount_list,
      const std::vector<Address>& address_list);

  /**
   * @brief Split output amount.
   * @param[in] index                   txout index
   * @param[in] amount_list             amount list
   * @param[in] locking_script_list     locking script list
   */
  void SplitTxOut(
      uint32_t index, const std::vector<Amount>& amount_list,
      const std::vector<Script>& locking_script_list);

  /**
   * @brief sign with privkey.
   * @param[in] outpoint      utxo target.
   * @param[in] pubkey        public key.
   * @param[in] privkey       private key.
   * @param[in] sighash_type  sighash type.
   * @param[in] has_grind_r   calcurate signature glind-r flag. (default:true)
   * @param[in] aux_rand      auxiliary random data used to create the nonce.
   * @param[in] annex         annex byte data.
   */
  void SignWithKey(
      const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
      SigHashType sighash_type = SigHashType(), bool has_grind_r = true,
      const ByteData256* aux_rand = nullptr, const ByteData* annex = nullptr);
  /**
   * @brief set ignore verify target.
   * @param[in] outpoint    utxo target.
   */
  void IgnoreVerify(const OutPoint& outpoint);
  /**
   * @brief verify tx sign (signature).
   */
  void Verify();
  /**
   * @brief verify tx sign (signature) on outpoint.
   * @param[in] outpoint    utxo target.
   */
  void Verify(const OutPoint& outpoint);
  /**
   * @brief verify tx and generate transaction raw data.
   * @return transaction raw data.
   */
  ByteData Finalize();

  // sign-api
  /**
   * @brief Calculates the TxIn Signature Hash in the specified PubkeyHash format.
   * @param[in] outpoint TxIn txid and vout
   * @param[in] pubkey  Public key
   * @param[in] sighash_type SigHash type
   * @param[in] value UTXO amount specified by TxIn
   * @param[in] version Witness Version of UTXO specified by TxIn
   * @return Signature hash
   */
  virtual ByteData CreateSignatureHash(
      const OutPoint& outpoint, const Pubkey& pubkey, SigHashType sighash_type,
      const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;
  /**
   * @brief Calculates the TxIn Signature Hash in the specified ScriptHash format.
   * @details Not supported for Scripts with OP_CODESEPARATOR
   * @param[in] outpoint TxIn txid and vout
   * @param[in] redeem_script   Script Hash Redeem Script
   * @param[in] sighash_type    SigHash type
   * @param[in] value UTXO amount specified by TxIn
   * @param[in] version Witness Version of UTXO specified by TxIn
   * @return Signature hash
   */
  virtual ByteData CreateSignatureHash(
      const OutPoint& outpoint, const Script& redeem_script,
      SigHashType sighash_type, const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Calculate Taproot Signature Hash.
   * @details It is necessary to set UTXO of all Inputs in advance.
   * @param[in] outpoint        target outpoint
   * @param[in] sighash_type    sighash type
   * @param[in] tap_leaf_hash   tapleaf hash.
   * @param[in] code_separator_position     OP_CODESEPARATOR position.
   * @param[in] annex           annex byte data.
   * @return Signature hash
   */
  ByteData256 CreateSignatureHashByTaproot(
      const OutPoint& outpoint, const SigHashType& sighash_type,
      const ByteData256* tap_leaf_hash = nullptr,
      const uint32_t* code_separator_position = nullptr,
      const ByteData* annex = nullptr) const;

  /**
   * @brief Sign the specified TxIn with pubkey hash.
   * @param[in] outpoint        outpoint
   * @param[in] pubkey          Public key
   * @param[in] privkey         Private key
   * @param[in] sighash_type    SigHash Type
   * @param[in] value           amount of UTXO
   * @param[in] address_type    address-type.(P2WPKH, P2SH-P2WPKH, P2PKH)
   * @param[in] has_grind_r     Grind-R option for sign.
   */
  void SignWithPrivkeySimple(
      const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
      SigHashType sighash_type = SigHashType(), const Amount& value = Amount(),
      AddressType address_type = AddressType::kP2wpkhAddress,
      bool has_grind_r = true);

  /**
   * @brief Sign the specified TxIn with Taproot.
   * @details It is necessary to set UTXO of all Inputs in advance.
   * @param[in] outpoint        outpoint
   * @param[in] privkey         private key for schnorr pubkey
   * @param[in] sighash_type    SigHashType
   * @param[in] aux_rand        auxiliary random data used to create the nonce.
   * @param[in] annex           annex byte data.
   */
  void SignWithSchnorrPrivkeySimple(
      const OutPoint& outpoint, const Privkey& privkey,
      const SigHashType& sighash_type = SigHashType(),
      const ByteData256* aux_rand = nullptr, const ByteData* annex = nullptr);

  /**
   * @brief add pubkey-hash sign data to target outpoint.
   * @param[in] outpoint        outpoint
   * @param[in] signature       signature
   * @param[in] pubkey          pubkey
   * @param[in] address_type    address-type.(P2WPKH, P2SH-P2WPKH, P2PKH)
   */
  void AddPubkeyHashSign(
      const OutPoint& outpoint, const SignParameter& signature,
      const Pubkey& pubkey, AddressType address_type);

  /**
   * @brief add script-hash sign data to target outpoint.
   * @param[in] outpoint            TxIn
   * @param[in] signatures          signature list
   * @param[in] redeem_script       redeem script
   * @param[in] address_type        address-type.(P2WSH, P2SH-P2WSH, P2SH)
   * @param[in] is_multisig_script  use multisig script
   */
  void AddScriptHashSign(
      const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
      const Script& redeem_script, AddressType address_type,
      bool is_multisig_script = false);

  /**
   * @brief add multisig sign data to target outpoint.
   * @details The order of the signatures to be added is \
   *    automatically arranged by correspondence between \
   *    the pubkey of the redeem script and the relatedPubkey \
   *    in the signatures.
   *    (If relatedPubkey is not set, add signatures in the order of \
   *    signParam after adding signatures with relatedPubkey set)
   * @param[in] outpoint          TxIn
   * @param[in] signatures        signature list
   * @param[in] redeem_script     redeem script
   * @param[in] hash_type         hash-type.(P2WSH, P2SH-P2WSH, P2SH)
   */
  void AddMultisigSign(
      const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
      const Script& redeem_script, AddressType hash_type);

  /**
   * @brief add schnorr-taproot sign data to target outpoint.
   * @param[in] outpoint        TxIn outpoint
   * @param[in] signature       signature
   * @param[in] annex           annex
   */
  void AddSchnorrSign(
      const OutPoint& outpoint, const SchnorrSignature& signature,
      const ByteData* annex = nullptr);

  /**
   * @brief add schnorr-taproot sign data to target outpoint.
   * @param[in] outpoint            xIn outpoint
   * @param[in] tree                script tree
   * @param[in] internal_pubkey     internal schnorr pubkey
   * @param[in] sign_data_list      sign data list
   * @param[in] annex               annex
   */
  void AddTapScriptSign(
      const OutPoint& outpoint, const TaprootScriptTree& tree,
      const SchnorrPubkey& internal_pubkey,
      const std::vector<SignParameter>& sign_data_list,
      const ByteData* annex = nullptr);

  /**
   * @brief add sign data to target outpoint.
   * @param[in] outpoint        TxIn
   * @param[in] sign_params     sign data list
   * @param[in] insert_witness  use witness
   * @param[in] clear_stack     clear stack data before add.
   */
  void AddSign(
      const OutPoint& outpoint, const std::vector<SignParameter>& sign_params,
      bool insert_witness = true, bool clear_stack = false);

  /**
   * @brief Verify signature which is specified (pubkey hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] outpoint            TxIn
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output amount.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey,
      const OutPoint& outpoint, SigHashType sighash_type,
      const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;
  /**
   * @brief Verify signature which is specified (script hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] outpoint            TxIn
   * @param[in] script              redeem script related previous output.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output amount.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey,
      const OutPoint& outpoint, const Script& script, SigHashType sighash_type,
      const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;
  /**
   * @brief Verify signature which is specified (taproot) input data.
   * @param[in] signature           signature to be verified.
   * @param[in] outpoint            TxIn
   * @param[in] utxo_list           utxo list for calculate sighash.
   * @param[in] pubkey              internal schnorr public key.
   * @param[in] annex               annex
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSchnorrSignature(
      const SchnorrSignature& signature, const OutPoint& outpoint,
      const std::vector<UtxoData>& utxo_list, const SchnorrPubkey& pubkey,
      const ByteData* annex = nullptr) const;

  /**
   * @brief Get the default sequence number from the lock time.
   * @retval 0xffffffff     locktime disable
   * @retval 0xfffffffe     locktime enable
   */
  uint32_t GetDefaultSequence() const;

  /**
   * @brief Get the default sequence number from the lock time.
   * @retval 0xffffffff     locktime disable
   */
  static uint32_t GetLockTimeDisabledSequence();

  /**
   * @brief check multisig script and parameter.
   * @param[in] sign_list       multisig sign list.
   * @param[in] redeem_script   multisig script.
   * @return sorted list.
   */
  static std::vector<SignParameter> CheckMultisig(
      const std::vector<SignParameter>& sign_list,
      const Script& redeem_script);

 protected:
  /**
   * @brief This function is called by the state change.
   * @param[in] type    change type
   */
  virtual void CallbackStateChange(uint32_t type);

  /**
   * @brief find target outpoint utxo list.
   * @param[in] outpoint    outpoint
   * @param[out] utxo       utxo
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindUtxoMap(const OutPoint& outpoint, UtxoData* utxo = nullptr) const;

  /**
   * @brief find target outpoint from list.
   * @param[in] list        outpoint list
   * @param[in] outpoint    outpoint
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindOutPoint(
      const std::vector<OutPoint>& list, const OutPoint& outpoint) const;

 private:
  /**
   * @brief utxo map.
   */
  std::vector<UtxoData> utxo_map_;
  /**
   * @brief utxo signed map. (outpoint, SigHashType)
   */
  std::map<OutPoint, SigHashType> signed_map_;
  /**
   * @brief utxo verify map. (outpoint)
   */
  std::vector<OutPoint> verify_map_;
  /**
   * @brief utxo verify ignore map. (outpoint)
   */
  std::vector<OutPoint> verify_ignore_map_;
};

// ----------------------------------------------------------------------------
// deprecated
// ----------------------------------------------------------------------------

using cfd::core::NetType;
using cfd::core::TxIn;
using cfd::core::TxInReference;
using cfd::core::TxOut;
using cfd::core::TxOutReference;

/**
 * @brief Transaction生成のためのControllerクラス
 * @deprecated replace to TransactionContext .
 */
class CFD_EXPORT TransactionController
    : public AbstractTransactionController {  // NOLINT
 public:
  /**
   * @brief コンストラクタ
   */
  TransactionController();  // NOLINT
  /**
   * @brief コンストラクタ.
   * @param[in] version   Transactionのバージョン
   * @param[in] locktime  Timestamp or ブロック高
   */
  explicit TransactionController(uint32_t version, uint32_t locktime);
  /**
   * @brief コンストラクタ.
   * @param[in] tx_hex  RawトランザクションHEX文字列
   */
  explicit TransactionController(const std::string& tx_hex);
  /**
   * @brief コンストラクタ
   * @param[in] transaction   トランザクション情報
   */
  TransactionController(const TransactionController& transaction);  // NOLINT
  /**
   * @brief デストラクタ.
   */
  virtual ~TransactionController() {
    // do nothing
  }
  /**
   * @brief copy constructor.
   * @param[in] transaction   Transaction
   * @return Transaction
   */
  TransactionController& operator=(const TransactionController& transaction) &;

  /**
   * @brief locking_scriptが空のTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const TxInReference AddTxIn(const Txid& txid, uint32_t vout);
  /**
   * @brief locking_scriptが空のTxInを追加する.
   * @param[in] txid      UTXOのTxid
   * @param[in] vout      UTXOのvout
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const TxInReference AddTxIn(
      const Txid& txid, uint32_t vout, uint32_t sequence);
  /**
   * @brief locking_scriptにredeem_scriptを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] redeem_script  unlockに必要なredeem_script
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const TxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Script& redeem_script);
  /**
   * @brief locking_scriptにredeem_scriptを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] redeem_script  unlockに必要なredeem_script
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const TxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Script& redeem_script,
      uint32_t sequence);
  /**
   * @brief locking_scriptにpubkeyを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] pubkey  unlockに必要なpubkey
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const TxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey);
  /**
   * @brief locking_scriptにpubkeyを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] pubkey  unlockに必要なpubkey
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const TxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey,
      uint32_t sequence);

  /**
   * TxInを取得する.
   * @param[in] txid 取得するTxInのTxId
   * @param[in] vout 取得するTxInのvout
   * @return TxInReferenceインスタンス
   */
  const TxInReference GetTxIn(const Txid& txid, uint32_t vout) const;

  /**
   * @brief TxInを削除する.
   * @param[in] txid 取得するTxInのTxId
   * @param[in] vout 取得するTxInのvout
   * @return 削除したTxInのTxInReferenceインスタンス
   */
  const TxInReference RemoveTxIn(const Txid& txid, uint32_t vout);

  /**
   * @brief TxOutを追加する.
   * @param[in] address  送金先アドレス
   * @param[in] value  送金額
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const TxOutReference AddTxOut(const Address& address, const Amount& value);
  /**
   * @brief TxOutを追加する.
   * @param[in] locking_script  送金先locking_script
   * @param[in] value  送金額
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const TxOutReference AddTxOut(
      const Script& locking_script, const Amount& value);
  /**
   * @brief TxOutを削除する.
   * @param[in] index 削除対象のindex
   * @return 削除したTxOutのTxOutReferenceインスタンス
   */
  const TxOutReference RemoveTxOut(uint32_t index);

  /**
   * @brief Unlocking Scriptに挿入する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 設定対象のTxInのtxid
   * @param[in] vout 設定対象のTxInのvout
   * @param[in] unlocking_scripts  署名を含むscriptリスト
   */
  void InsertUnlockingScript(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& unlocking_scripts);
  /**
   * @brief TxInにUnlocking Scriptを設定する.
   * @param[in] txid 設定対象のTxInのtxid
   * @param[in] vout 設定対象のTxInのvout
   * @param[in] unlocking_script  署名を含むunlockingScript全体
   */
  void SetUnlockingScript(
      const Txid& txid, uint32_t vout, const Script& unlocking_script);
  /**
   * @brief Unlocking Scriptを設定する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 設定対象のTxInのtxid
   * @param[in] vout 設定対象のTxInのvout
   * @param[in] unlocking_scripts  署名を含むscriptリスト
   */
  void SetUnlockingScript(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& unlocking_scripts);

  /**
   * @brief WitnessStackを追加する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] witness_datas   WitnessStack追加情報リスト
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& witness_datas);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hash  署名のHash文字列
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::string& signed_signature_hash);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hashes  署名のHash文字列リスト
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::vector<std::string>& signed_signature_hashes);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hash  署名のHash文字列
   * @param[in] pubkey  署名のpubkey
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::string& signed_signature_hash, const Pubkey& pubkey);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hash  署名のHash文字列
   * @param[in] redeem_script  署名のredeem_script
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::string& signed_signature_hash, const Script& redeem_script);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hashes  署名のHash文字列リスト
   * @param[in] redeem_script  署名のredeem_script
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::vector<std::string>& signed_signature_hashes,
      const Script& redeem_script);
  /**
   * @brief WitnessStackの指定Indexのデータを更新する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   * @param[in] witness_index   WitnessStackのindex
   * @param[in] witness_stack   WitnessStack更新情報
   */
  void SetWitnessStack(
      const Txid& txid, uint32_t vout, uint32_t witness_index,
      const ByteData& witness_stack);
  /**
   * @brief WitnessStackの指定Indexのデータを更新する.
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   * @param[in] witness_index   WitnessStackのindex
   * @param[in] hex_string      WitnessStack更新情報(HEX文字列)
   */
  void SetWitnessStack(
      const Txid& txid, uint32_t vout, uint32_t witness_index,
      const std::string& hex_string);
  /**
   * @brief WitnessStackを削除する.
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   */
  void RemoveWitnessStackAll(const Txid& txid, uint32_t vout);
  /**
   * @brief WitnessStack数を取得する.
   * @param[in] txid 取得対象のTxInのtxid
   * @param[in] vout 取得対象のTxInのvout
   * @return WitnessStack数
   */
  uint32_t GetWitnessStackNum(const Txid& txid, uint32_t vout) const;

  /**
   * @brief Transaction取得.
   * @return Transactionインスタンス
   */
  const Transaction& GetTransaction() const;

  /**
   * @brief TxInを除外したサイズを取得する。
   * @return TxInを除外したTxサイズ(Serialize)
   */
  uint32_t GetSizeIgnoreTxIn() const;

  /**
   * @brief TxInを除外した仮想サイズを取得する。
   * @return TxInを除外したTx仮想サイズ(Serialize)
   */
  uint32_t GetVsizeIgnoreTxIn() const;

  /**
   * @brief 指定されたPubkeyHash形式のTxInのSignatureHashを計算する.
   * @param[in] txid SignatureHash算出対象のTxInのtxid
   * @param[in] vout SignatureHash算出対象のTxInのvout
   * @param[in] pubkey SignatureHashの公開鍵
   * @param[in] sighash_type SigHashType値
   * @param[in] value TxInで指定したUTXOのamount
   * @param[in] version TxInで指定したUTXOのWitnessVersion
   * @return 算出されたSignatureHashのHex文字列
   */
  ByteData CreateSignatureHash(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey,
      SigHashType sighash_type, const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;
  /**
   * @brief 指定されたScriptHash形式のTxInのSignatureHashを計算する.
   * @details OP_CODESEPARATORが存在するScriptについては未対応
   * @param[in] txid SignatureHash算出対象のTxInのtxid
   * @param[in] vout SignatureHash算出対象のTxInのvout
   * @param[in] witness_script WSHのWitness Script
   * @param[in] sighash_type SigHashType値
   * @param[in] value TxInで指定したUTXOのamount
   * @param[in] version TxInで指定したUTXOのWitnessVersion
   * @return 算出されたSignatureHashのHex文字列
   */
  ByteData CreateSignatureHash(
      const Txid& txid, uint32_t vout, const Script& witness_script,
      SigHashType sighash_type, const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Verify signature which is specified (pubkey hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] txid                target input's transaction id.
   * @param[in] vout                target input's previous output index.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output amount.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
      uint32_t vout, SigHashType sighash_type, const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Verify signature which is specified (script hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] txid                target input's transaction id.
   * @param[in] vout                target input's previous output index.
   * @param[in] script              redeem script related previous output.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output amount.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
      uint32_t vout, const Script& script, SigHashType sighash_type,
      const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

 private:
  /**
   * @brief Transactionインスタンス.
   */
  Transaction transaction_;
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_TRANSACTION_H_
