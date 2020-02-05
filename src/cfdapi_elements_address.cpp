// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_address.cpp
 *
 * @brief \~english implementation of address operation that uses cfd-api
 *   \~japanese cfd-apiで利用するAddress操作の実装ファイル
 */
#ifndef CFD_DISABLE_ELEMENTS
#include <string>
#include <vector>

#include "cfd/cfd_elements_address.h"
#include "cfd_manager.h"  // NOLINT
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_script.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"

#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_elements_address.h"

namespace cfd {
namespace api {

using cfd::api::AddressApi;
using cfd::core::ContractHashUtil;
using cfd::core::ScriptUtil;

Address ElementsAddressApi::CreateAddress(
    NetType net_type, AddressType address_type, const Pubkey* pubkey,
    const Script* script, Script* locking_script, Script* redeem_script,
    const std::vector<AddressFormatData>* prefix_list) const {
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetElementsAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }

  AddressApi address_api;
  return address_api.CreateAddress(
      net_type, address_type, pubkey, script, locking_script, redeem_script,
      &addr_prefixes);
}

Address ElementsAddressApi::CreateMultisig(
    NetType net_type, AddressType address_type, uint32_t req_sig_num,
    const std::vector<Pubkey>& pubkeys, Script* redeem_script,
    Script* witness_script,
    const std::vector<AddressFormatData>* prefix_list) const {
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetElementsAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }

  AddressApi address_api;
  return address_api.CreateMultisig(
      net_type, address_type, req_sig_num, pubkeys, redeem_script,
      witness_script, &addr_prefixes);
}

std::vector<Address> ElementsAddressApi::GetAddressesFromMultisig(
    NetType net_type, AddressType address_type, const Script& redeem_script,
    std::vector<Pubkey>* pubkey_list,
    const std::vector<AddressFormatData>* prefix_list) const {
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetElementsAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }

  AddressApi address_api;
  return address_api.GetAddressesFromMultisig(
      net_type, address_type, redeem_script, pubkey_list, &addr_prefixes);
}

ElementsConfidentialAddress ElementsAddressApi::GetConfidentialAddress(
    const Address& address, const ConfidentialKey confidential_key) const {
  ConfidentialKey conf_key(confidential_key);
  ElementsConfidentialAddress conf_addr(address, conf_key);

  return conf_addr;
}

Address ElementsAddressApi::CreatePegInAddress(
    NetType net_type, AddressType address_type, const Script& fedpegscript,
    const Pubkey& pubkey, Script* claim_script, Script* tweak_fedpegscript,
    const std::vector<AddressFormatData>* prefix_list) const {
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetElementsAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }

  // create claim_script from pubkey
  Script claim_script_inner = ScriptUtil::CreateP2wpkhLockingScript(pubkey);

  // tweak add claim_script with fedpegscript
  Script tweak_fedpegscript_inner =
      ContractHashUtil::GetContractScript(claim_script_inner, fedpegscript);

  // create peg-in address
  Address p2ch =
      ElementsAddressFactory(net_type, addr_prefixes)
          .CreatePegInAddress(address_type, tweak_fedpegscript_inner);

  // convert parameters to response struct
  if (claim_script != nullptr) {
    *claim_script = claim_script_inner;
  }
  if (tweak_fedpegscript != nullptr) {
    *tweak_fedpegscript = tweak_fedpegscript_inner;
  }

  return p2ch;
}

DescriptorScriptData ElementsAddressApi::ParseOutputDescriptor(
    const std::string& descriptor, NetType net_type,
    const std::string& bip32_derivation_path,
    std::vector<DescriptorScriptData>* script_list,
    std::vector<DescriptorKeyData>* multisig_key_list,
    const std::vector<AddressFormatData>* prefix_list) const {
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetElementsAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }

  AddressApi address_api;
  return address_api.ParseOutputDescriptor(
      descriptor, net_type, bip32_derivation_path, script_list,
      multisig_key_list, &addr_prefixes);
}

}  // namespace api
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
