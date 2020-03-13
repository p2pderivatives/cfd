// Copyright 2020 CryptoGarage
/**
 * @file cfd_json_mapping_api.h
 *
 * @brief cfd-apiで利用するJsonApiクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFD_JSON_MAPPING_API_H_
#define CFD_INCLUDE_CFD_CFD_JSON_MAPPING_API_H_

#include <string>

#include "cfd/cfd_common.h"

/**
 * @brief cfdapi名前空間
 */
namespace cfd {
namespace api {
namespace json {

/**
 * @brief 共通系の関数群クラス
 */
class CFD_EXPORT JsonMappingApi {
 public:
  /**
   * @brief DecodeRawTransactionのJSON API関数(request, response).
   * @param[in] request_message     リクエストされたjson文字列
   * @return 戻り値(JSON文字列)
   */
  static std::string DecodeRawTransaction(const std::string &request_message);

#ifndef CFD_DISABLE_ELEMENTS

  /**
   * @brief ElementsDecodeRawTransactionのJSON API関数(request, response).
   * @param[in] request_message     リクエストされたjson文字列
   * @return 戻り値(JSON文字列)
   */
  static std::string ElementsDecodeRawTransaction(
      const std::string &request_message);
#endif  // CFD_DISABLE_ELEMENTS

 private:
  JsonMappingApi();
};

}  // namespace json
}  // namespace api
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_JSON_MAPPING_API_H_
