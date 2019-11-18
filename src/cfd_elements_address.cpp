// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_address.cpp
 *
 * @brief Elements用Address操作関連クラスの実装
 */
#ifndef CFD_DISABLE_ELEMENTS
#include "cfd/cfd_elements_address.h"
#include <string>
#include <vector>
#include "cfd/cfd_common.h"

#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_script.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::ConfidentialKey;
using cfd::core::ContractHashUtil;
using cfd::core::ElementsAddressType;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::ElementsNetType;
using cfd::core::GetElementsAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::WitnessVersion;

ElementsAddressFactory::ElementsAddressFactory()
    : AddressFactory(NetType::kMainnet, GetElementsAddressFormatList()) {
  // do nothing
}

ElementsAddressFactory::ElementsAddressFactory(NetType type)
    : AddressFactory(type, GetElementsAddressFormatList()) {
  // do nothing
}

ElementsAddressFactory::ElementsAddressFactory(
    NetType type, const std::vector<AddressFormatData>& prefix_list)
    : AddressFactory(type, prefix_list) {
  // do nothing
}

ElementsAddressFactory::ElementsAddressFactory(
    NetType type, WitnessVersion wit_ver)
    : AddressFactory(type, wit_ver, GetElementsAddressFormatList()) {
  // do nothing
}

ElementsAddressFactory::ElementsAddressFactory(
    NetType type, WitnessVersion wit_ver,
    const std::vector<AddressFormatData>& prefix_list)
    : AddressFactory(type, wit_ver, prefix_list) {
  // do nothing
}

ElementsConfidentialAddress ElementsAddressFactory::GetConfidentialAddress(
    const Address& unblinded_address,
    const ConfidentialKey& confidential_key) {
  return ElementsConfidentialAddress(unblinded_address, confidential_key);
}

ElementsConfidentialAddress ElementsAddressFactory::GetConfidentialAddress(
    const std::string& address) const {
  return ElementsConfidentialAddress(address, prefix_list_);
}

Address ElementsAddressFactory::CreatePegInAddress(
    AddressType address_type, const Pubkey& pubkey,
    const Script& fedpegscript) const {
  // create claim_script from pubkey
  Script claim_script = ScriptUtil::CreateP2wpkhLockingScript(pubkey);
  return CreatePegInAddress(address_type, claim_script, fedpegscript);
}

Address ElementsAddressFactory::CreatePegInAddress(
    AddressType address_type, const Script& claim_script,
    const Script& fedpegscript) const {
  // tweak add claim_script with fedpegscript
  Script tweak_fedpegscript =
      ContractHashUtil::GetContractScript(claim_script, fedpegscript);
  return CreatePegInAddress(address_type, tweak_fedpegscript);
}

Address ElementsAddressFactory::CreatePegInAddress(
    AddressType address_type, const Script& tweak_fedpegscript) const {
  // FIXME(fujita-cg): implements of processing by address_type
  // create peg-in address(P2CH = P2SH-P2WSH)
  Script witness_program =
      ScriptUtil::CreateP2wshLockingScript(tweak_fedpegscript);
  return Address(type_, witness_program);
}

}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
