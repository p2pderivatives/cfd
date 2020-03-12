// Copyright 2020 CryptoGarage
/**
 * @file cfdjs_internal.h
 *
 * @brief cfd_apiの内部定義ヘッダ.
 */
#ifndef CFD_SRC_JSONAPI_CFD_JSON_INTERNAL_H_
#define CFD_SRC_JSONAPI_CFD_JSON_INTERNAL_H_

#include <string>

#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfd_struct.h"

namespace cfd {
namespace api {
namespace json {

using cfd::core::CfdException;
using cfd::core::logger::warn;

/**
 * @brief 構造体指定処理の共通部テンプレート関数.
 * @param[in] request         要求値
 * @param[in] call_function   実行関数定義
 * @param[in] fuction_name    実行元関数名
 * @return Response object
 */
template <typename RequestStructType, typename ResponseStructType>
ResponseStructType ExecuteStructApi(
    const RequestStructType& request,
    std::function<ResponseStructType(const RequestStructType&)> call_function,
    std::string fuction_name) {
  ResponseStructType response;
  try {
    cfd::Initialize();

    response = call_function(request);
  } catch (const CfdException& cfde) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to {}. CfdException occurred:  code={}, message={}",
        fuction_name, cfde.GetErrorCode(), cfde.what());
    response.error = cfd::js::api::ConvertCfdExceptionToStruct(cfde);
  } catch (const std::exception& except) {
    warn(
        CFD_LOG_SOURCE, "Failed to {}. Exception occurred: message={}",
        fuction_name, except.what());
    response.error = cfd::js::api::ConvertCfdExceptionToStruct(CfdException());
  } catch (...) {
    warn(
        CFD_LOG_SOURCE, "Failed to {}. Unknown exception occurred.",
        fuction_name);
    response.error = cfd::js::api::ConvertCfdExceptionToStruct(CfdException());
  }
  return response;
}

/**
 * @brief 構造体指定処理の共通部テンプレート関数.
 * @param[in] call_function   実行関数定義
 * @param[in] fuction_name    実行元関数名
 * @return Response object
 */
template <typename ResponseStructType>
ResponseStructType ExecuteResponseStructApi(
    std::function<ResponseStructType()> call_function,
    std::string fuction_name) {
  ResponseStructType response;
  try {
    cfd::Initialize();

    response = call_function();
  } catch (const CfdException& cfde) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to {}. CfdException occurred:  code={}, message={}",
        fuction_name, cfde.GetErrorCode(), cfde.what());
    response.error = cfd::js::api::ConvertCfdExceptionToStruct(cfde);
  } catch (const std::exception& except) {
    warn(
        CFD_LOG_SOURCE, "Failed to {}. Exception occurred: message={}",
        fuction_name, except.what());
    response.error = cfd::js::api::ConvertCfdExceptionToStruct(CfdException());
  } catch (...) {
    warn(
        CFD_LOG_SOURCE, "Failed to {}. Unknown exception occurred.",
        fuction_name);
    response.error = cfd::js::api::ConvertCfdExceptionToStruct(CfdException());
  }
  return response;
}

}  // namespace json
}  // namespace api
}  // namespace cfd

#endif  // CFD_SRC_JSONAPI_CFD_JSON_INTERNAL_H_
