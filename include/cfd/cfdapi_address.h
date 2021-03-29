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

#include "cfd/cfd_address.h"
#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_key.h"

namespace cfd {
namespace api {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorScriptType;
using cfd::core::KeyData;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;

/**
 * @brief Address関連の関数群クラス
 * @details 現状は内部クラス扱い。あとで名称変更予定.
 * @deprecated The function has moved to AddressFactory.
 */
class CFD_EXPORT AddressApi {
 public:
  /**
   * @brief constructor
   */
  AddressApi() {}

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
  Address CreateAddress(
      NetType net_type, AddressType address_type, const Pubkey* pubkey,
      const Script* script, Script* locking_script = nullptr,
      Script* redeem_script = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

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
  Address CreateMultisig(
      NetType net_type, AddressType address_type, uint32_t req_sig_num,
      const std::vector<Pubkey>& pubkeys, Script* redeem_script = nullptr,
      Script* witness_script = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

  /**
   * @brief Multisig ScriptからPubkey Address一覧を作成する
   * @param[in] net_type        network type
   * @param[in] address_type    address type
   * @param[in] redeem_script   multisig script
   * @param[out] pubkey_list    pubkey list
   * @param[in] prefix_list     address prefix list
   * @return pubkey address list
   */
  std::vector<Address> GetAddressesFromMultisig(
      NetType net_type, AddressType address_type, const Script& redeem_script,
      std::vector<Pubkey>* pubkey_list = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

  /**
   * @brief Output descriptorから情報を抽出する
   * @param[in] descriptor              output descriptor
   * @param[in] net_type                network type
   * @param[in] bip32_derivation_path   bip32 derivation path
   * @param[out] script_list            descriptor script list
   * @param[out] multisig_key_list      descriptor multisig key list
   * @param[in] prefix_list             address prefix list
   * @param[out] key_list               key data list
   * @return descriptor script data (top level or high security)
   */
  DescriptorScriptData ParseOutputDescriptor(
      const std::string& descriptor, NetType net_type,
      const std::string& bip32_derivation_path = "",
      std::vector<DescriptorScriptData>* script_list = nullptr,
      std::vector<DescriptorKeyData>* multisig_key_list = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr,
      std::vector<KeyData>* key_list = nullptr) const;
};

}  // namespace api
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_ADDRESS_H_
