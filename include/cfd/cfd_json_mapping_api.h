// Copyright 2020 CryptoGarage
/**
 * @file cfd_json_mapping_api.h
 *
 * @brief JsonApi class definition used in cfd-api
 *
 *Provides a JSON-formatted API.
 */
#ifndef CFD_INCLUDE_CFD_CFD_JSON_MAPPING_API_H_
#define CFD_INCLUDE_CFD_CFD_JSON_MAPPING_API_H_

#include <string>

#include "cfd/cfd_common.h"

/**
 * @brief cfd::api::json namespace
 */
namespace cfd {
namespace api {
namespace json {

/**
 * @brief Json Information mapping API class.
 */
class CFD_EXPORT JsonMappingApi {
 public:
  /**
   * @brief DecodeRawTransaction.
   * @param[in] request_message     json message
   * @return json message
   */
  static std::string DecodeRawTransaction(const std::string &request_message);

  /**
   * @brief DecodePsbt.
   * @param[in] request_message     json message
   * @return json message
   */
  static std::string DecodePsbt(const std::string &request_message);

#ifndef CFD_DISABLE_ELEMENTS

  /**
   * @brief ElementsDecodeRawTransaction.
   * @param[in] request_message     json message
   * @return json message
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
