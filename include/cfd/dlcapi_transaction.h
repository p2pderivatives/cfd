// Copyright 2019 CryptoGarage
/**
 * @file dlcapi_transaction.h
 *
 * @brief dlc-apiで利用するTransaction操作関連のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_DLCAPI_TRANSACTION_H_
#define CFD_INCLUDE_CFD_DLCAPI_TRANSACTION_H_

#include <string>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_struct.h"

/**
 * @brief dlc_api名前空間
 */
namespace dlc {
namespace js {
namespace api {

/**
 * @brief DLCの取引に用いるTransaction関連のJOSN-APIクラス
 */
class CFD_EXPORT DlcTransactionStructApi {
 public:
  /**
   * @brief
   * JSONパラメータの情報を元に、UTXOにCETxを用いた際の署名情報を追加する.
   * @param[in] request CETx用署名追加を行うパラメータ
   * @return CETx用署名に関連するhexデータを格納した構造体
   * @details ClosingTxのSign時に用いられることを想定している.
   */
  static AddCETxSignResponseStruct AddCETxSign(
      const AddCETxSignRequestStruct& request);

 private:
  DlcTransactionStructApi();
};

}  // namespace api
}  // namespace js
}  // namespace dlc

#endif  // CFD_INCLUDE_CFD_DLCAPI_TRANSACTION_H_
