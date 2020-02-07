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
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_address.h"
#include "cfdcore/cfdcore_elements_address.h"

namespace cfd {
namespace api {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::ConfidentialKey;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorScriptType;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;

/**
 * @brief ElementsAddress関連のAPI群クラス
 */
class CFD_EXPORT ElementsAddressApi {
 public:
  /**
   * @brief constructor
   */
  ElementsAddressApi() {}

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
   * @brief AddressからConfidentialAddressを取得する.
   * @param[in] address Address
   * @param[in] confidential_key confidential key
   * @return ElementsConfidentialAddress
   */
  ElementsConfidentialAddress GetConfidentialAddress(
      const Address& address, const ConfidentialKey confidential_key) const;

  /**
   * @brief bitcoin blockchainからのpeginに利用できるAddressを生成する
   * @param[in] net_type              network type of mainchain
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] fedpegscript          fed peg script
   * @param[in] pubkey                pubkey related to mainchain address
   * @param[out] claim_script         claim script used when claiming peg-in bitcoin
   * @param[out] tweak_fedpegscript   fedpeg_script with pubkey added as tweak
   * @param[in] prefix_list           address prefix list
   * @return peg-inに利用できるAddressインスタンス
   */
  Address CreatePegInAddress(
      NetType net_type, AddressType address_type, const Script& fedpegscript,
      const Pubkey& pubkey, Script* claim_script = nullptr,
      Script* tweak_fedpegscript = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

  /**
   * @brief bitcoin blockchainからのpeginに利用できるAddressを生成する
   * @param[in] net_type              network type of mainchain
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] fedpegscript          fed peg script
   * @param[in] redeem_script         redeem script
   * @param[out] claim_script         claim script used when claiming peg-in bitcoin
   * @param[out] tweak_fedpegscript   fedpeg_script with pubkey added as tweak
   * @param[in] prefix_list           address prefix list
   * @return peg-inに利用できるAddressインスタンス
   */
  Address CreatePegInAddress(
      NetType net_type, AddressType address_type, const Script& fedpegscript,
      const Script& redeem_script, Script* claim_script = nullptr,
      Script* tweak_fedpegscript = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

  /**
   * @brief bitcoin blockchainからのpeginに利用できるAddressを生成する
   * @param[in] net_type              network type of mainchain
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] fedpegscript          fed peg script
   * @param[in] pubkey                pubkey related to mainchain address
   * @param[in] redeem_script         redeem script
   * @param[out] claim_script         claim script used when claiming peg-in bitcoin
   * @param[out] tweak_fedpegscript   fedpeg_script with pubkey added as tweak
   * @param[in] prefix_list           address prefix list
   * @return peg-inに利用できるAddressインスタンス
   */
  Address CreatePegInAddress(
      NetType net_type, AddressType address_type, const Script& fedpegscript,
      const Pubkey& pubkey, const Script& redeem_script,
      Script* claim_script = nullptr, Script* tweak_fedpegscript = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

  /**
   * @brief Output descriptorから情報を抽出する
   * @param[in] descriptor              output descriptor
   * @param[in] net_type                network type
   * @param[in] bip32_derivation_path   bip32 derivation path
   * @param[out] script_list            descriptor script list
   * @param[out] multisig_key_list      descriptor multisig key list
   * @param[in] prefix_list             address prefix list
   * @return descriptor script data (top level or high security)
   */
  DescriptorScriptData ParseOutputDescriptor(
      const std::string& descriptor, NetType net_type,
      const std::string& bip32_derivation_path = "",
      std::vector<DescriptorScriptData>* script_list = nullptr,
      std::vector<DescriptorKeyData>* multisig_key_list = nullptr,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;
};

}  // namespace api
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFD_CFDAPI_ELEMENTS_ADDRESS_H_
