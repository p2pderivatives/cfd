// Copyright 2021 CryptoGarage
/**
 * @file cfd_json_psbt.h
 *
 * @brief This file is defines Partially Signed Bitcoin Transaction by json.
 */
#ifndef CFD_SRC_JSONAPI_CFD_JSON_PSBT_H_
#define CFD_SRC_JSONAPI_CFD_JSON_PSBT_H_

#include "jsonapi/autogen/cfd_api_json_autogen.h"  // NOLINT

namespace cfd {
namespace api {
namespace json {

/**
 * @brief The class of PSBT Json API.
 */
class PsbtJsonApi {
 public:
  /**
   * @brief Decode PSBT using JSON parameter.
   * @param[in] request  request data
   * @param[out] response  response data
   */
  static void DecodePsbt(DecodePsbtRequest* request,
      DecodePsbtResponse* response);

 private:
  PsbtJsonApi();
};

}  // namespace json
}  // namespace api
}  // namespace cfd

#endif  // CFD_SRC_JSONAPI_CFD_JSON_PSBT_H_
