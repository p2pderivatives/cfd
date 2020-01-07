// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction.h
 *
 * @brief Transaction操作の関連クラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_TRANSACTION_H_
#define CFD_INCLUDE_CFD_CFD_TRANSACTION_H_

#include <string>
#include <vector>

#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_util.h"

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::OutPoint;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::WitnessVersion;

/**
 * @brief Context class for generating bitcoin transaction.
 */
class CFD_EXPORT TransactionContext : public Transaction {
 public:
  /**
   * @brief コンストラクタ.
   */
  TransactionContext();
  /**
   * @brief コンストラクタ.
   * @param[in] version   Transaction version
   * @param[in] locktime  Timestamp or block height
   */
  explicit TransactionContext(uint32_t version, uint32_t locktime);
  /**
   * @brief コンストラクタ.
   * @param[in] tx_hex  Raw Transaction HEX string
   */
  explicit TransactionContext(const std::string& tx_hex);
  /**
   * @brief コンストラクタ
   * @param[in] context   Transaction Context
   */
  explicit TransactionContext(const TransactionContext& context);
  /**
   * @brief デストラクタ.
   */
  virtual ~TransactionContext() {
    // do nothing
  }

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
   * @brief TxOutを追加する.
   * @param[in] address  送金先アドレス
   * @param[in] value  送金額
   * @return 追加したTxOutのIndex番号
   */
  uint32_t AddTxOut(const Address& address, const Amount& value);
  /**
   * @brief TxOutを追加する. (set locking script direct)
   * @param[in] locking_script  送金先locking_script
   * @param[in] value  送金額
   * @return 追加したTxOutのIndex番号
   */
  uint32_t AddTxOut(const Script& locking_script, const Amount& value);

  /**
   * @brief TxInを除外したサイズを取得する。
   * @return TxInを除外したTxサイズ(Serialize)
   */
  uint32_t GetSizeIgnoreTxIn() const;

  // state-sequence-api
  /**
   * @brief collect utxo and cache into utxo_map_.
   * @param[in] utxos   utxo list.
   */
  void CollectInputUtxo(const std::vector<UtxoData>& utxos);

  /**
   * @brief sign with privkey.
   * @param[in] outpoint      utxo target.
   * @param[in] pubkey        public key.
   * @param[in] privkey       private key.
   * @param[in] sighash_type  sighash type.
   * @param[in] has_grind_r   calcurate signature glind-r flag. (default:true)
   */
  void SignWithKey(
      const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
      SigHashType sighash_type = SigHashType(), bool has_grind_r = true);
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
  /**
   * @brief clear sign area all.
   */
  void ClearSign();
  /**
   * @brief clear sign area on outpoint.
   * @param[in] outpoint    utxo target.
   */
  void ClearSign(const OutPoint& outpoint);

  // sign-api
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
   * @brief 指定されたPubkeyHash形式のTxInに署名する.
   * @param[in] outpoint    TxIn
   * @param[in] pubkey      SignatureHashの公開鍵
   * @param[in] private_key SignatureHashの秘密鍵
   * @param[in] sighash_type SigHashType値
   * @param[in] value TxInで指定したUTXOのamount
   * @param[in] version TxInで指定したUTXOのWitnessVersion
   * @param[in] has_grind_r signature計算時のオプション
   */
  void SignPrivkeySimple(
      const OutPoint& outpoint, const Pubkey& pubkey,
      const Privkey& private_key, SigHashType sighash_type = SigHashType(),
      const Amount& value = Amount(),
      WitnessVersion version = WitnessVersion::kVersionNone,
      bool has_grind_r = true);

  // FIXME API層から、AddSign, AddMultisigSignを移植したい。
  // FIXME AddSignはP2sh-P2wXXXにも対応させる。
  // TODO Signatureだけ生成するAPI作る？（Multisig用）

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

  /**
   * @brief ロックタイムからデフォルトのシーケンス番号を取得する。
   * @retval 0xffffffff     locktime値無効
   * @retval 0xfffffffe     locktime値有効
   */
  uint32_t GetDefaultSequence() const;

  /**
   * @brief ロックタイムからlocktime値無効のシーケンス番号を取得する。
   * @retval 0xffffffff     locktime値無効
   */
  static uint32_t GetLockTimeDisabledSequence();

 protected:
  /**
   * @brief This function is called by the state change.
   * @param[in] type    change type
   */
  virtual void CallbackStateChange(uint32_t type);

 private:
  /**
   * @brief utxo map.
   */
  std::map<OutPoint, UtxoData> utxo_map_;
  /**
   * @brief utxo signed map. (outpoint, SigHashType)
   */
  std::map<OutPoint, SigHashType> signed_map_;
  /**
   * @brief utxo verify map. (outpoint, SignVerifyType)
   */
  std::map<OutPoint, uint32_t> verify_state_map_;
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
  TransactionController(const TransactionController& transaction);
  /**
   * @brief デストラクタ.
   */
  virtual ~TransactionController() {
    // do nothing
  }

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
