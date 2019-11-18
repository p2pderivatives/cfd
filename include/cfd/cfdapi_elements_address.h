// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_elements_address.h
 *
 * @brief cfd-apiで利用するElementsAddress操作のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_ELEMENTS_ADDRESS_H_
#define CFD_INCLUDE_CFD_CFDAPI_ELEMENTS_ADDRESS_H_
#ifndef CFD_DISABLE_ELEMENTS

#include <string>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_struct.h"
#include "cfdcore/cfdcore_elements_address.h"

/**
 * @brief cfdapi名前空間
 */
namespace cfd {
namespace js {
namespace api {

/**
 * @brief ElementsAddress関連の関数群クラス
 */
class CFD_EXPORT ElementsAddressStructApi {
 public:
  /**
   * @brief JSONパラメータの情報を元に、Addressを作成する
   * @param[in] request Addressを構築するパラメータ
   * @return Addressのhexデータを格納した構造体
   */
  static CreateAddressResponseStruct CreateAddress(
      const CreateAddressRequestStruct& request);

  /**
   * @brief JSONパラメータの情報を元に、Multisigを作成する
   * @param[in] request Multisigを構築するパラメータ
   * @return MultisigAddressとredeem scriptのhexデータを格納した構造体
   */
  static CreateMultisigResponseStruct CreateMultisig(
      const CreateMultisigRequestStruct& request);

  /**
   * @brief
   * 構造体の情報を元に、UnblinedAddressからElements用ConfidentialAddressを取得する
   * @param[in] request ConfidentialAddressを構築するパラメータ
   * @return Addressのhexデータを格納した構造体
   */
  static GetConfidentialAddressResponseStruct GetConfidentialAddress(
      const GetConfidentialAddressRequestStruct& request);

  /**
   * @brief
   * 構造体の情報を元に、ConfidentialAddressからElements用UnblinedAddressを取得する
   * @param[in] request ConfidentialAddressを構築するパラメータ
   * @return Addressのhexデータを格納した構造体
   */
  static GetUnblindedAddressResponseStruct GetUnblindedAddress(
      const GetUnblindedAddressRequestStruct& request);

  /**
   * @brief 構造体の情報を元に、bitcoin
   * blockchainからのpeginに利用できるAddressを生成する
   * @param[in] request peg-inに利用できるAddressを構成するパラメータ
   * @return peg-inに利用できるAddress hexを格納した構造体
   */
  static ElementsCreatePegInAddressResponseStruct CreatePegInAddress(
      const ElementsCreatePegInAddressRequestStruct& request);

  /**
   * @brief elementsネットワーク文字列を、ElementsNetType構造体へ変換する.
   * @param[in] elements_net_type ネットワーク文字列
   * @return 引数に対応するElementsNetType構造体
   * @throws CfdException 指定文字列以外が渡された場合
   */
  static cfd::core::ElementsNetType ConvertElementsNetType(
      const std::string& elements_net_type);

 private:
  ElementsAddressStructApi();
};

}  // namespace api
}  // namespace js
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFD_CFDAPI_ELEMENTS_ADDRESS_H_
