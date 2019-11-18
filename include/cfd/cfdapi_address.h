// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_address.h
 *
 * @brief cfd-apiで利用するAddress操作のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_ADDRESS_H_
#define CFD_INCLUDE_CFD_CFDAPI_ADDRESS_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_struct.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_key.h"

/**
 * @brief cfdapi名前空間
 */
namespace cfd {
namespace js {
namespace api {

using cfd::core::AddressType;

/**
 * @brief Address関連の関数群クラス
 */
class CFD_EXPORT AddressStructApi {
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
   * @brief bitcoinネットワーク文字列を、NetType構造体へ変換する.
   * @param[in] network_type ネットワーク文字列
   * @return 引数に対応するNetType構造体
   * @throws CfdException 指定文字列以外が渡された場合
   */
  static cfd::core::NetType ConvertNetType(const std::string& network_type);

  /**
   * @brief Convert address type from string to AddressType.
   * @param[in] address_type the address type as a string.
   * @return the converted AddressType.
   * @throws CfdException if address_type does not match any known AddressType.
   */
  static AddressType ConvertAddressType(const std::string& address_type);

 private:
  AddressStructApi();
};

}  // namespace api
}  // namespace js
}  // namespace cfd

namespace cfd {
namespace api {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;

/**
 * @brief Address関連の関数群クラス
 * @details 現状は内部クラス扱い。あとで名称変更予定.
 */
class CFD_EXPORT AddressApi {
 public:
  /**
   * @brief Addressを作成する
   * @param[in] net_type        network type
   * @param[in] address_type    address type
   * @param[in] pubkey          public key (default: nullptr)
   * @param[in] script          script (default: nullptr)
   * @param[out] locking_script locking script
   * @param[out] redeem_script  redeem script
   * @param[in] prefix_list     address prefix list
   * @return Address
   */
  static Address CreateAddress(
      NetType net_type, AddressType address_type, const Pubkey* pubkey,
      const Script* script, Script* locking_script = nullptr,
      Script* redeem_script = nullptr,
      std::vector<AddressFormatData>* prefix_list = nullptr);

  /**
   * @brief Multisig Addressを作成する
   * @param[in] net_type        network type
   * @param[in] address_type    address type
   * @param[in] req_sig_num     multisig require sign num
   * @param[in] pubkeys         public key list
   * @param[out] redeem_script  redeem script (p2sh, p2sh-p2wsh)
   * @param[out] witness_script witness script (p2wsh, p2sh-p2wsh)
   * @param[in] prefix_list     address prefix list
   * @return Address
   */
  static Address CreateMultisig(
      NetType net_type, AddressType address_type, uint32_t req_sig_num,
      const std::vector<Pubkey>& pubkeys, Script* redeem_script = nullptr,
      Script* witness_script = nullptr,
      std::vector<AddressFormatData>* prefix_list = nullptr);

 private:
  AddressApi();
};

}  // namespace api
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_ADDRESS_H_
