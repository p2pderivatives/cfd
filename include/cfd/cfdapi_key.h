// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_key.h
 *
 * @brief cfd-apiで利用する鍵関連のクラス定義
 *
 * 構造体形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_KEY_H_
#define CFD_INCLUDE_CFD_CFDAPI_KEY_H_

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_struct.h"

/**
 * @brief cfdapi名前空間
 */
namespace cfd {
namespace js {
namespace api {

/**
 * @brief 鍵情報関連の関数群クラス
 */
class CFD_EXPORT KeyStructApi {
 public:
  /**
   *  @brief
   * @param request
   * @return
   */
  static CreateKeyPairResponseStruct CreateKeyPair(
      const CreateKeyPairRequestStruct& request);

 private:
  KeyStructApi();
};

}  // namespace api
}  // namespace js
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_KEY_H_
