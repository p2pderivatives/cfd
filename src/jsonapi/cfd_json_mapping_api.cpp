// Copyright 2019 CryptoGarage
/**
 * @file cfd_json_mapping_api.cpp
 *
 * @brief cfdのnode.js向けAPI実装ファイル
 */
#include <string>

#include "cfd/cfd_common.h"
#include "cfd/cfd_json_mapping_api.h"
#include "jsonapi/autogen/cfd_api_json_autogen.h"           // NOLINT
#include "jsonapi/cfd_json_transaction.h"           // NOLINT
#include "jsonapi/cfd_json_psbt.h"                  // NOLINT
#include "jsonapi/cfd_json_elements_transaction.h"  // NOLINT

// using
using cfd::core::CfdError;
using cfd::core::CfdException;

// -----------------------------------------------------------------------------
// API wrapper for node addon
// -----------------------------------------------------------------------------

namespace cfd {
namespace api {
namespace json {

/**
 * @brief NodeAddonのJSON APIテンプレート関数(request, response).
 * @param[in] request_message     リクエストされたjson文字列
 * @param[in] bitcoin_function   bitcoin有効時に呼び出されるcfdの呼び出し関数
 * @param[in] elements_function   elements有効時に呼び出されるcfdの呼び出し関数
 * @return 戻り値(JSON文字列)
 */
template <typename RequestType, typename ResponseType>
std::string ExecuteDirectApi(
    const std::string &request_message,
    std::function<void(RequestType *, ResponseType *)>
        function) {  // NOLINT
  // リクエストjson_strから、モデルへ変換
  RequestType req;
  try {
    req.Deserialize(request_message);
  } catch (const CfdException &cfd_except) {
    throw cfd_except;
  } catch (...) {
    CfdException ex(
        CfdError::kCfdOutOfRangeError,
        "JSON value convert error. Value out of range.");
    throw ex;
  }

  ResponseType response;
  function(&req, &response);
  return response.Serialize();
}

#if 0
/**
 * @brief NodeAddonのJSON APIテンプレート関数(request, response).
 * @param[in] request_message     リクエストされたjson文字列
 * @param[in] bitcoin_function   bitcoin有効時に呼び出されるcfdの呼び出し関数
 * @param[in] elements_function   elements有効時に呼び出されるcfdの呼び出し関数
 * @return 戻り値(JSON文字列)
 */
template <typename RequestType, typename ResponseType>
std::string ExecuteElementsCheckDirectApi(
    const std::string &request_message,
    std::function<void(RequestType *, ResponseType *)>
        bitcoin_function,  // NOLINT
    std::function<void(RequestType *, ResponseType *)>
        elements_function) {  // NOLINT
  // リクエストjson_strから、モデルへ変換
  RequestType req;
  try {
    req.Deserialize(request_message);
  } catch (const CfdException &cfd_except) {
    throw cfd_except;
  } catch (...) {
    CfdException ex(
        CfdError::kCfdOutOfRangeError,
        "JSON value convert error. Value out of range.");
    throw ex;
  }

  ResponseType response;

  if (req.GetIsElements()) {
#ifndef CFD_DISABLE_ELEMENTS
    elements_function(&req, &response);
#else
    CfdException ex(
        CfdError::kCfdIllegalArgumentError, "functionType not supported.");
    throw ex;
#endif  // CFD_DISABLE_ELEMENTS
  } else {
    bitcoin_function(&req, &response);
  }
  return response.Serialize();
}
#endif

std::string JsonMappingApi::DecodeRawTransaction(
    const std::string &request_message) {
  return ExecuteDirectApi<
      DecodeRawTransactionRequest,
      DecodeRawTransactionResponse>(
      request_message, TransactionJsonApi::DecodeRawTransaction);
}

std::string JsonMappingApi::DecodePsbt(
    const std::string &request_message) {
  return ExecuteDirectApi<DecodePsbtRequest, DecodePsbtResponse>(
      request_message, PsbtJsonApi::DecodePsbt);
}

#ifndef CFD_DISABLE_ELEMENTS

std::string JsonMappingApi::ElementsDecodeRawTransaction(
    const std::string &request_message) {
  return ExecuteDirectApi<
      ElementsDecodeRawTransactionRequest,
      ElementsDecodeRawTransactionResponse>(
      request_message, ElementsTransactionJsonApi::DecodeRawTransaction);
}

#endif  // CFD_DISABLE_ELEMENTS

}  // namespace json
}  // namespace api
}  // namespace cfd
