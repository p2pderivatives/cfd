// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction_common.h
 *
 * @brief Transaction操作共通の関連クラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_
#define CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptOperator;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::Txid;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::BlindFactor;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsConfidentialAddress;
#endif  // CFD_DISABLE_ELEMENTS

/**
 * @typedef SignDataType
 * @brief SignData種別
 */
enum SignDataType {
  kSign = 0,
  kBinary,
  kPubkey,
  kRedeemScript,
  kOpCode,
};

/**
 * @brief UTXO構造体
 */
struct UtxoData {
  uint64_t block_height;     //!< blick高
  BlockHash block_hash;      //!< block hash
  Txid txid;                 //!< txid
  uint32_t vout;             //!< vout
  Script locking_script;     //!< locking script
  Script redeem_script;      //!< script
  Address address;           //!< address
  std::string descriptor;    //!< output descriptor
  Amount amount;             //!< amount
  AddressType address_type;  //!< address type
  void* binary_data;         //!< binary data option
#ifndef CFD_DISABLE_ELEMENTS
  ConfidentialAssetId asset;  //!< asset
  // elements
  ElementsConfidentialAddress confidential_address;  //!< Confidential address
  BlindFactor asset_blind_factor;                    //!< asset blind factor
  BlindFactor amount_blind_factor;                   //!< blind vactor
  ConfidentialValue value_commitment;                //!< value commitment
#endif                                               // CFD_DISABLE_ELEMENTS
  Script scriptsig_template;                         //!< scriptsig template
  // int32_t status;          //!< utxo status (reserved)
};

/**
 * @brief Coin関連のAPIクラス
 */
class CFD_EXPORT UtxoUtil {
 public:
  /**
   * @brief convert to simple utxo list.
   * @param[in] utxos   utxo data list
   * @return UTXO list
   */
  static std::vector<Utxo> ConvertToUtxo(const std::vector<UtxoData>& utxos);
  /**
   * @brief convert to simple utxo.
   * @param[in] utxo_data   utxo data
   * @param[out] utxo       utxo
   * @param[out] dest       analyzed utxo data
   */
  static void ConvertToUtxo(
      const UtxoData& utxo_data, Utxo* utxo, UtxoData* dest = nullptr);

 private:
  /**
   * @brief constructor (disable)
   */
  UtxoUtil();
};

/**
 * @brief Sign生成のためのデータモデル
 */
class CFD_EXPORT SignParameter {
 public:
  /**
   * @brief コンストラクタ(for vector)
   */
  SignParameter();
  /**
   * @brief コンストラクタ(Type: auto)
   * @param[in] text_message            text data
   * @param[in] der_encode              flag of need der encode
   * @param[in] sighash_type            sighash type (SigHashType)
   */
  explicit SignParameter(
      const std::string& text_message, bool der_encode = false,
      const SigHashType sighash_type =
          SigHashType(SigHashAlgorithm::kSigHashAll));
  /**
   * @brief コンストラクタ(Type: Sign)
   * @param[in] data                    byte data
   * @param[in] der_encode              flag of need der encode
   * @param[in] sighash_type            sighash type (SigHashType)
   */
  explicit SignParameter(
      const ByteData& data, bool der_encode,
      const SigHashType sighash_type =
          SigHashType(SigHashAlgorithm::kSigHashAll));
  /**
   * @brief コンストラクタ(Type: Binary)
   * @param[in] data  data
   */
  explicit SignParameter(const ByteData& data);
  /**
   * @brief コンストラクタ(Type: Pubkey)
   * @param[in] pubkey  pubkey data
   */
  explicit SignParameter(const Pubkey& pubkey);
  /**
   * @brief コンストラクタ(Type: RedeemScript)
   * @param[in] redeem_script  redeem script data
   */
  explicit SignParameter(const Script& redeem_script);
  /**
   * @brief コンストラクタ(Type: ScriptOperator)
   * @param[in] op_code  op code
   */
  explicit SignParameter(const ScriptOperator& op_code);
  /**
   * @brief コピーコンストラクタ.
   * @param[in] sign_parameter     Sign生成情報オブジェクト
   */
  SignParameter(const SignParameter& sign_parameter);
  /**
   * @brief コピーコンストラクタ.
   * @param[in] sign_parameter     Sign生成情報オブジェクト
   * @return SignParameterオブジェクト
   */
  SignParameter& operator=(const SignParameter& sign_parameter);
  /**
   * @brief RelatedPubkeyのセット
   * @param[in] pubkey  realated pubkey
   */
  void SetRelatedPubkey(const Pubkey& pubkey);

  /**
   * @brief dataを取得する
   * @return data
   */
  ByteData GetData() const;
  /**
   * @brief get op_code.
   * @return op_code
   */
  ScriptOperator GetOpCode() const;
  /**
   * @brief check op_code.
   * @retval true op_code
   * @retval false other value
   */
  bool IsOpCode() const;
  /**
   * @brief data typeを取得する
   * @return data
   */
  SignDataType GetDataType() const;
  /**
   * @brief RelatedPubkeyを取得する
   * @return related pubkey data
   */
  Pubkey GetRelatedPubkey() const;
  /**
   * @brief DerEncodeフラグを取得する
   * @return der encode 実施フラグ
   */
  bool IsDerEncode() const;
  /**
   * @brief SigHashTypeを取得する
   * @return sighash type
   */
  SigHashType GetSigHashType() const;
  /**
   * @brief 格納された情報でdataをsignatureへ変換する
   * @return signature data
   */
  ByteData ConvertToSignature() const;

 private:
  ByteData data_;             //!< data hex
  SignDataType data_type_;    //!< data type
  Pubkey related_pubkey_;     //!< need der encode flag
  bool der_encode_;           //!< sighash type
  SigHashType sighash_type_;  //!< AnyoneCanPay flag
  ScriptOperator op_code_;    //!< op_code
};

// ----------------------------------------------------------------------------
// deprecated
// ----------------------------------------------------------------------------

/**
 * @brief Transaction生成のためのController基底クラス
 * @deprecated replace to TransactionContext .
 */
class CFD_EXPORT AbstractTransactionController {
 public:
  /**
   * @brief コンストラクタ.
   */
  AbstractTransactionController();
  /**
   * @brief デストラクタ.
   */
  virtual ~AbstractTransactionController() {
    // do nothing
  }

  /**
   * @brief TransactionHex文字列取得.
   * @return TransactionHEX文字列
   */
  std::string GetHex() const;

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
  AbstractTransaction* tx_address_;  //!< Transaction基底オブジェクト
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_
