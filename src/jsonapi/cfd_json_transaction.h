// Copyright 2020 CryptoGarage
/**
 * @file cfd_json_transaction.h
 *
 * @brief cfd-apiで利用するTransaction作成のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_SRC_JSONAPI_CFD_JSON_TRANSACTION_H_
#define CFD_SRC_JSONAPI_CFD_JSON_TRANSACTION_H_

#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_util.h"
#include "jsonapi/autogen/cfd_api_json_autogen.h"  // NOLINT

namespace cfd {
namespace api {
namespace json {

using cfd::core::ByteData;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::NetType;

/**
 * @enum LockingScriptType
 * @brief LockingScript の Script Type 定義
 */
enum LockingScriptType {
  kNonStandard = 0,      //!< non standard locking script
  kPayToPubkey,          //!< p2pk locking script
  kPayToPubkeyHash,      //!< p2pkh locking script
  kPayToScriptHash,      //!< p2sh locking script
  kMultisig,             //!< multisig locking script
  kNullData,             //!< null data of locking script
  kWitnessV0ScriptHash,  //!< p2wsh locking script
  kWitnessV0KeyHash,     //!< p2wpkh locking script
  kWitnessUnknown,       //!< invalid witness ver locking script
  kTrue,                 //!< can spend anyone script
  kFee,                  //!< type fee (elements only)
};

/**
 * @brief LockingScriptの解析情報
 */
struct ExtractScriptData {
  LockingScriptType script_type;  //!< LockingScript種別
  std::vector<ByteData> pushed_datas;  //!< LockingScriptに含まれるhashデータ
  int64_t req_sigs;                    //!< Unlockingに必要なSignature数
};

/**
 * @brief Transaction関連のJSON APIクラス
 */
class TransactionJsonApi {
 public:
  /**
   * @brief JSONパラメータの情報を元に、Transactionをデコードして出力する.
   * @param[in] request Transactionとデコード用の情報を格納したオブジェクト
   * @param[out] response Transactionの表示用JSONデータを格納したオブジェクト
   */
  static void DecodeRawTransaction(
      DecodeRawTransactionRequest* request,
      DecodeRawTransactionResponse* response);

  /**
   * @brief Convert signature information to a signature.
   * @param[in] hex_string              Signature information
   * @param[in] is_sign                 Whether signature data is provided
   * @param[in] is_der_encode           Whether the signature is DER encoded
   * @param[in] sighash_type            SigHash type
   * @param[in] sighash_anyone_can_pay  Flag determining if SigHash is
   * anyone_can_pay
   * @return Converted signature information.
   */
  static ByteData ConvertSignDataToSignature(
      const std::string& hex_string, bool is_sign, bool is_der_encode,
      const std::string& sighash_type, bool sighash_anyone_can_pay);

  /**
   * @brief Convert a string to a SigHashType object.
   * @param[in] sighash_type_string   SigHashType as a string
   * @param[in] is_anyone_can_pay     whether or not anyone_can_pay is used
   * @return SigHashType object
   */
  static SigHashType ConvertSigHashType(
      const std::string& sighash_type_string, bool is_anyone_can_pay);

  /**
   * @brief LockingScriptの解析を行う.
   * @param[in] locking_script LockingScriptデータ
   * @return 解析された ExtractScriptData インスタンス
   */
  static ExtractScriptData ExtractLockingScript(Script locking_script);

  /**
   * @brief LockingScriptType を文字列情報に変換する.
   * @param[in] script_type LockingScriptType インスタンス
   * @return LockingScriptType に応じたscript_type文字列
   */
  static std::string ConvertLockingScriptTypeString(
      LockingScriptType script_type);

  /**
   * @brief bitcoinネットワーク文字列を、NetType情報へ変換する.
   * @param[in] network_type ネットワーク文字列
   * @return 引数に対応するNetType情報
   * @throw CfdException 指定文字列以外が渡された場合
   */
  static NetType ConvertNetType(const std::string& network_type);

 private:
  TransactionJsonApi();
};

}  // namespace json
}  // namespace api
}  // namespace cfd

#endif  // CFD_JS_SRC_CFDJS_JSON_TRANSACTION_H_
