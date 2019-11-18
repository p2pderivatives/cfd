// Copyright 2019 CryptoGarage
/**
 * @file dlcapi_internal.h
 *
 * @brief dlc_apiの内部定義ヘッダ.
 */
#ifndef CFD_SRC_DLCAPI_INTERNAL_H_
#define CFD_SRC_DLCAPI_INTERNAL_H_

#include <string>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_struct.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_logger.h"

namespace dlc {
namespace js {
namespace api {

using cfd::core::CfdException;
using cfd::core::logger::warn;
using cfd::js::api::InnerErrorResponseStruct;

/**
 * @brief CfdExceptionをInnerErrorResponseStructに変換する.
 * @param cfde CfdException object
 * @return ErrorResponse object
 */
inline InnerErrorResponseStruct ConvertCfdExceptionToStruct(
    const CfdException& cfde) {
  InnerErrorResponseStruct result;
  result.code = cfde.GetErrorCode();
  result.type = cfde.GetErrorType();
  result.message = cfde.what();
  return result;
}

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
    response.error = dlc::js::api::ConvertCfdExceptionToStruct(cfde);
  } catch (const std::exception& except) {
    warn(
        CFD_LOG_SOURCE, "Failed to {}. Exception occurred: message={}",
        fuction_name, except.what());
    response.error = dlc::js::api::ConvertCfdExceptionToStruct(CfdException());
  } catch (...) {
    warn(
        CFD_LOG_SOURCE, "Failed to {}. Unknown exception occurred.",
        fuction_name);
    response.error = dlc::js::api::ConvertCfdExceptionToStruct(CfdException());
  }
  return response;
}

}  // namespace api
}  // namespace js
}  // namespace dlc

#endif  // CFD_SRC_DLCAPI_INTERNAL_H_
