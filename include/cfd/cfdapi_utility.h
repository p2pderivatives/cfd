// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_utility.h
 *
 * @brief cfd-apiで利用する共通系のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_UTILITY_H_
#define CFD_INCLUDE_CFD_CFDAPI_UTILITY_H_

#include <string>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_struct.h"

/**
 * @brief cfdapi名前空間
 */
namespace cfd {
namespace js {
namespace api {

/**
 * @brief 共通系の関数群クラス
 */
class CFD_EXPORT UtilStructApi {
 public:
  /**
   * @brief サポート機能情報を取得する。
   * @return サポート機能の情報を格納したJSON文字列
   */
  static GetSupportedFunctionResponseStruct GetSupportedFunction();

 private:
  UtilStructApi();
};

}  // namespace api
}  // namespace js
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_UTILITY_H_
