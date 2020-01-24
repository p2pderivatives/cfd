// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_transaction.h
 *
 * @brief Elements Transaction操作の関連クラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_ELEMENTS_TRANSACTION_H_
#define CFD_INCLUDE_CFD_CFD_ELEMENTS_TRANSACTION_H_
#ifndef CFD_DISABLE_ELEMENTS

#include <string>
#include <vector>

#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_util.h"

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"

/**
 * @brief cfd名前空間
 */
namespace cfd {

using cfd::core::Address;
using cfd::core::Amount;
using cfd::core::BlindFactor;
using cfd::core::BlindParameter;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::IssuanceBlindingKeyPair;
using cfd::core::IssuanceParameter;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::UnblindParameter;
using cfd::core::WitnessVersion;

/**
 * @brief ConfidentialTransaction生成のためのControllerクラス
 */
class CFD_EXPORT ConfidentialTransactionController
    : public AbstractTransactionController {
 public:
  /**
   * @brief コンストラクタ.
   * @param[in] version   Transactionのバージョン
   * @param[in] locktime  Timestamp or ブロック高
   */
  explicit ConfidentialTransactionController(
      uint32_t version, uint32_t locktime);
  /**
   * @brief コンストラクタ.
   * @param[in] tx_hex  RawトランザクションHEX文字列
   */
  explicit ConfidentialTransactionController(const std::string& tx_hex);
  /**
   * @brief コンストラクタ
   * @param[in] transaction   トランザクション情報
   */
  ConfidentialTransactionController(
      const ConfidentialTransactionController& transaction);
  /**
   * @brief デストラクタ.
   */
  virtual ~ConfidentialTransactionController() {
    // do nothing
  }
  /**
   * @brief copy constructor.
   * @param[in] transaction   Transaction
   * @return Transaction
   */
  ConfidentialTransactionController& operator=(
      const ConfidentialTransactionController& transaction) &;

  /**
   * @brief locking_scriptが空のTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(const Txid& txid, uint32_t vout);
  /**
   * @brief locking_scriptが空のTxInを追加する.
   * @param[in] txid      UTXOのTxid
   * @param[in] vout      UTXOのvout
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, uint32_t sequence);
  /**
   * @brief locking_scriptにredeem_scriptを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] redeem_script  unlockに必要なredeem_script
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Script& redeem_script);
  /**
   * @brief locking_scriptにredeem_scriptを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] redeem_script  unlockに必要なredeem_script
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Script& redeem_script,
      uint32_t sequence);
  /**
   * @brief locking_scriptにpubkeyを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] pubkey  unlockに必要なpubkey
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey);
  /**
   * @brief locking_scriptにpubkeyを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] pubkey  unlockに必要なpubkey
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey,
      uint32_t sequence);

  /**
   * TxInを取得する.
   * @param[in] txid 取得するTxInのTxId
   * @param[in] vout 取得するTxInのvout
   * @return TxInReferenceインスタンス
   */
  const ConfidentialTxInReference GetTxIn(
      const Txid& txid, uint32_t vout) const;
  /**
   * @brief TxInを削除する.
   * @param[in] txid 取得するTxInのTxId
   * @param[in] vout 取得するTxInのvout
   * @return 削除したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference RemoveTxIn(const Txid& txid, uint32_t vout);

  /**
   * @brief TxOutを追加する.
   * @param[in] address  送金先unblindedアドレス
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const Address& address, const Amount& value,
      const ConfidentialAssetId& asset);
  /**
   * @brief TxOutを追加する.
   * @param[in] address  送金先confidentialアドレス
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @param[in] remove_nonce      nonceの強制削除フラグ
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const ElementsConfidentialAddress& address,
      const Amount& value,  // amount
      const ConfidentialAssetId& asset, bool remove_nonce = false);
  /**
   * @brief TxOutを追加する.
   * @param[in] locking_script  送金先locking_script
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const Script& locking_script, const Amount& value,
      const ConfidentialAssetId& asset);
  /**
   * @brief TxOutを追加する.
   * @param[in] locking_script  送金先locking_script
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @param[in] nonce nonce
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const Script& locking_script, const Amount& value,
      const ConfidentialAssetId& asset, const ConfidentialNonce& nonce);

  /**
   * @brief Add txout for pegged-out.
   * @param[in] value Pegout value
   * @param[in] asset AssetID
   * @param[in] genesisblock_hash mainchainのgenesisblock hash
   * @param[in] btc_address mainchain address
   * @param[in] net_type mainchain network type
   * @param[in] online_pubkey online pubkey for whitelist proof
   * @param[in] master_online_key  online privkey for whitelist proof
   * @param[in] btc_descriptor descriptor by initpegoutwallet
   * @param[in] bip32_counter descriptor counter by initpegoutwallet
   * @param[in] whitelist whitelist
   * @param[in] elements_net_type elements network type
   * @param[out] btc_derive_address btc address by derived btc descriptor
   * @return TxOutReference object with added txout.
   */
  const ConfidentialTxOutReference AddPegoutTxOut(
      const Amount& value, const ConfidentialAssetId& asset,
      const BlockHash& genesisblock_hash, const Address& btc_address,
      NetType net_type = NetType::kMainnet,
      const Pubkey& online_pubkey = Pubkey(),
      const Privkey& master_online_key = Privkey(),
      const std::string& btc_descriptor = "", uint32_t bip32_counter = 0,
      const ByteData& whitelist = ByteData(),
      NetType elements_net_type = NetType::kLiquidV1,
      Address* btc_derive_address = nullptr);

  /**
   * @brief TxOut(Fee)を追加する.
   * @param[in] value Fee額
   * @param[in] asset AssetID
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOutFee(
      const Amount& value, const ConfidentialAssetId& asset);
  /**
   * @brief TxOutのFee情報を更新する.
   * @param[in] index 設定対象のindex
   * @param[in] value Fee額
   * @param[in] asset AssetID
   * @return 更新したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference UpdateTxOutFeeAmount(
      uint32_t index, const Amount& value, const ConfidentialAssetId& asset);
  /**
   * @brief TxOutを削除する.
   * @param[in] index 削除対象のindex
   * @return 削除したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference RemoveTxOut(uint32_t index);

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
   * @brief PeginWitnessに文字列から得られるバイトデータを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] amount peginしたinputのamount
   * @param[in] asset_id 送金先アセットID
   * @param[in] mainchain_genesis_block_hash mainchainのgenesis_blockのhash値
   * @param[in] claim_script pegin請求用スクリプト
   * @param[in] mainchain_pegin_transaction peginしたmainchainのtransaction
   * @param[in] tx_out_proof peginしたtransactionが取り込まれた際のproof値
   */
  void AddPeginWitness(
      const Txid& txid, uint32_t vout, const Amount& amount,
      const ConfidentialAssetId& asset_id,
      const BlockHash& mainchain_genesis_block_hash,
      const Script& claim_script, const ByteData& mainchain_pegin_transaction,
      const ByteData& tx_out_proof);
  /**
   * @brief PeginWitnessにバイトデータを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] witness_datas   WitnessStack追加情報リスト
   */
  void AddPeginWitness(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& witness_datas);
  /**
   * @brief WitnessStackを削除する.
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   */
  void RemovePeginWitnessAll(const Txid& txid, uint32_t vout);

  /**
   * @brief Transaction取得.
   * @return Transactionインスタンス
   */
  const ConfidentialTransaction& GetTransaction() const;

  /**
   * @brief TxInを除外したサイズを取得する。
   * @param[in] is_blinded    blind時の想定サイズを取得するフラグ
   * @param[out] witness_stack_size   witness stack size
   * @return TxInを除外したTxサイズ(Serialize)
   */
  uint32_t GetSizeIgnoreTxIn(
      bool is_blinded = false, uint32_t* witness_stack_size = nullptr) const;

  /**
   * @brief IssueAssetの情報を設定する.
   * @param[in] txid                 対象のTxInのtxid
   * @param[in] vout                 対象のTxInのvout
   * @param[in] asset_amount         issuance amount
   * @param[in] asset_locking_script asset locking script
   * @param[in] asset_nonce          asset nonce
   * @param[in] token_amount         token amount
   * @param[in] token_locking_script token locking script
   * @param[in] token_nonce          token nonce
   * @param[in] is_blind             blinding issuance
   * @param[in] contract_hash        asset entropy
   * @param[in] is_random_sort       set txout randomize
   * @param[in] is_remove_nonce      nonceの強制削除フラグ
   * @return issuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetIssuance(
      const Txid& txid, uint32_t vout, const Amount& asset_amount,
      const Script& asset_locking_script, const ByteData& asset_nonce,
      const Amount& token_amount, const Script& token_locking_script,
      const ByteData& token_nonce, bool is_blind,
      const ByteData256& contract_hash, bool is_random_sort = false,
      bool is_remove_nonce = false);

  /**
   * @brief ReissueAssetの情報を設定する.
   * @param[in] txid              対象のTxInのtxid
   * @param[in] vout              対象のTxInのvout
   * @param[in] amount            reissuance amount
   * @param[in] locking_script    asset locking script
   * @param[in] asset_nonce             confidential key
   * @param[in] blind_factor      blind factor
   * @param[in] entropy           asset entropy
   * @param[in] is_random_sort    set txout randomize
   * @param[in] is_remove_nonce   nonceの強制削除フラグ
   * @return reissuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetReissuance(
      const Txid& txid, uint32_t vout, const Amount& amount,
      const Script& locking_script, const ByteData& asset_nonce,
      const BlindFactor& blind_factor, const BlindFactor& entropy,
      bool is_random_sort = false, bool is_remove_nonce = false);

  /**
   * @brief TxOutの順序をランダム化する.
   * @details ブラインド前のみ実施可能.
   */
  void RandomSortTxOut();

  /**
   * @brief TrasnsactionをBlindする.
   * @param[in] txin_info_list            txin blind info list.
   * @param[in] issuance_blinding_keys    issue blinding key list.
   * @param[in] txout_confidential_keys   blinding pubkey list.
   */
  void BlindTransaction(
      const std::vector<BlindParameter>& txin_info_list,
      const std::vector<IssuanceBlindingKeyPair>& issuance_blinding_keys,
      const std::vector<Pubkey>& txout_confidential_keys);

  /**
   * @brief 指定indexのTxOutをUnblindする.
   * @param[in] tx_out_index unblind target txout index
   * @param[in] blinding_key blinding privkey
   * @return struct contain blind factors and amount.
   */
  UnblindParameter UnblindTxOut(
      uint32_t tx_out_index, const Privkey& blinding_key);

  /**
   * @brief TransactionをUnblindする.
   * @param[in] blinding_keys blinding privkey list
   * @return list of structs contain blind factors and amount.
   */
  std::vector<UnblindParameter> UnblindTransaction(
      const std::vector<Privkey>& blinding_keys);

  /**
   * @brief IssuanceをUnblindする.
   * @param[in] tx_in_index unblind target index
   * @param[in] asset_blinding_key asset blinding privkey
   * @param[in] token_blinding_key token blinding privkey
   * @return list of structs contain blind factors and amount.
   */
  std::vector<UnblindParameter> UnblindIssuance(
      uint32_t tx_in_index, const Privkey& asset_blinding_key,
      const Privkey& token_blinding_key);

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
      SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;
  /**
   * @brief 指定されたScriptHash形式のTxInのSignatureHashを計算する.
   * @details OP_CODESEPARATORが存在するScriptについては未対応
   * @param[in] txid SignatureHash算出対象のTxInのtxid
   * @param[in] vout SignatureHash算出対象のTxInのvout
   * @param[in] witness_script WSHのWitness Script
   * @param[in] sighash_type SigHashType値
   * @param[in] value TxInで指定したUTXOのamount/amountcommitment
   * @param[in] version TxInで指定したUTXOのWitnessVersion
   * @return 算出されたSignatureHashのHex文字列
   */
  ByteData CreateSignatureHash(
      const Txid& txid, uint32_t vout, const Script& witness_script,
      SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Verify signature which is specified (pubkey hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] txid                target input's transaction id.
   * @param[in] vout                target input's previous output index.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output confidential value.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
      uint32_t vout, SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Verify signature which is specified (pubkey hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] txid                target input's transaction id.
   * @param[in] vout                target input's previous output index.
   * @param[in] script              redeem script related previous output.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output confidential value.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
      uint32_t vout, const Script& script, SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief 簡易のFee計算を行う.
   * @param[in] append_feature_signed_size  sign分のサイズ加算フラグ
   * @param[in] append_signed_witness       sign分の加算をwitness扱いとするか
   * @return Fee Amount
   */
  Amount CalculateSimpleFee(
      bool append_feature_signed_size = true,
      bool append_signed_witness = true) const;

 private:
  /**
   * @brief Transactionインスタンス.
   */
  ConfidentialTransaction transaction_;
};

}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFD_CFD_ELEMENTS_TRANSACTION_H_
