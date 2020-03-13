// Copyright 2020 CryptoGarage
/**
 * @file cfd_json_elements_transaction.h
 *
 * @brief cfd-apiで利用するElements用Transaction操作のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_SRC_JSONAPI_CFD_JSON_ELEMENTS_TRANSACTION_H_
#define CFD_SRC_JSONAPI_CFD_JSON_ELEMENTS_TRANSACTION_H_
#ifndef CFD_DISABLE_ELEMENTS

#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_key.h"
#include "jsonapi/autogen/cfd_api_json_autogen.h"  // NOLINT

namespace cfd {
namespace api {
namespace json {

/**
 * @brief Elements用Transaction関連の関数群クラス
 */
class ElementsTransactionJsonApi {
 public:
  /**
   * @brief JSONパラメータの情報を元に、Transactionをデコードして出力する.
   * @param[in] request Transactionとデコード用の情報を格納したオブジェクト
   * @param[out] response Transactionの表示用JSONデータを格納したオブジェクト
   */
  static void DecodeRawTransaction(
      ElementsDecodeRawTransactionRequest* request,
      ElementsDecodeRawTransactionResponse* response);

  /**
   * @brief elementsネットワーク文字列を、ElementsNetType構造体へ変換する.
   * @param[in] elements_net_type ネットワーク文字列
   * @return 引数に対応するElementsNetType構造体
   * @throws CfdException 指定文字列以外が渡された場合
   */
  static cfd::core::ElementsNetType ConvertElementsNetType(
      const std::string& elements_net_type);

 private:
  ElementsTransactionJsonApi();
};

}  // namespace json
}  // namespace api
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_SRC_JSONAPI_CFD_JSON_ELEMENTS_TRANSACTION_H_
