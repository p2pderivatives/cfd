// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_address.cpp
 *
 * @brief implementation of classes related to address operation for Elements 
 */
#ifndef CFD_DISABLE_ELEMENTS
#include "cfd/cfd_elements_address.h"

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_script.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ConfidentialKey;
using cfd::core::ConfidentialTransaction;
using cfd::core::ContractHashUtil;
using cfd::core::ElementsAddressType;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::ElementsNetType;
using cfd::core::ExtPrivkey;
using cfd::core::ExtPubkey;
using cfd::core::GetElementsAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::WitnessVersion;

ElementsAddressFactory::ElementsAddressFactory()
    : AddressFactory(NetType::kLiquidV1, GetElementsAddressFormatList()) {
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
  ElementsConfidentialAddress obj(unblinded_address, confidential_key);
  return obj;
}

ElementsConfidentialAddress ElementsAddressFactory::GetConfidentialAddress(
    const std::string& address) const {
  ElementsConfidentialAddress obj(address, prefix_list_);
  return obj;
}

Address ElementsAddressFactory::CreatePegInAddress(
    AddressType address_type, const Pubkey& pubkey,
    const Script& fedpegscript) const {
  // create claim_script from pubkey
  Script claim_script = ScriptUtil::CreateP2wpkhLockingScript(pubkey);
  return CreatePegInAddress(address_type, claim_script, fedpegscript);
}

Address ElementsAddressFactory::CreatePegInAddressWithScript(
    AddressType address_type, const Script& redeem_script,
    const Script& fedpegscript) const {
  Script claim_script = ScriptUtil::CreateP2wshLockingScript(redeem_script);
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
  NetType type = type_;
  if (type == NetType::kLiquidV1)
    type = NetType::kMainnet;
  else if (type == NetType::kElementsRegtest)
    type = NetType::kRegtest;
  return CreatePegInAddress(type, address_type, tweak_fedpegscript);
}

Address ElementsAddressFactory::CreatePegInAddress(
    NetType mainchain_net_type, AddressType address_type,
    const Script& tweak_fedpegscript) {
  if (address_type == AddressType::kP2shAddress) {
    return Address(mainchain_net_type, tweak_fedpegscript);
  } else if (address_type == AddressType::kP2wshAddress) {
    return Address(
        mainchain_net_type, WitnessVersion::kVersion0, tweak_fedpegscript);
  } else if (address_type == AddressType::kP2shP2wshAddress) {
    Script witness_program =
        ScriptUtil::CreateP2wshLockingScript(tweak_fedpegscript);
    return Address(mainchain_net_type, witness_program);
  } else {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Pubkey-hash cannot be used for the peg-in address type.");
  }
}

Address ElementsAddressFactory::CreatePegOutAddress(
    NetType mainchain_network, NetType elements_network,
    const std::string& descriptor_or_xpub, uint32_t bip32_counter,
    AddressType address_type, Descriptor* base_descriptor) {
  if (mainchain_network > NetType::kRegtest) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to mainchain network type. invalid value.");
  }
  if ((elements_network < NetType::kLiquidV1) ||
      (elements_network > NetType::kCustomChain)) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to elements network type. invalid value.");
  }
  bool is_mainnet = (mainchain_network == NetType::kMainnet);
  if ((address_type == AddressType::kP2pkhAddress) ||
      ((!is_mainnet) && (address_type == AddressType::kP2shP2wpkhAddress)) ||
      ((!is_mainnet) && (address_type == AddressType::kP2wpkhAddress))) {
    // do nothing
  } else {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid address type. This address type is not support on pegout.");
  }

  std::string descriptor = descriptor_or_xpub;
  if (descriptor_or_xpub.find("(") == std::string::npos) {
    if (address_type == AddressType::kP2pkhAddress) {
      descriptor = "pkh(" + descriptor_or_xpub + ")";
    } else if (address_type == AddressType::kP2shP2wpkhAddress) {
      descriptor = "sh(wpkh(" + descriptor_or_xpub + "))";
    } else if (address_type == AddressType::kP2wpkhAddress) {
      descriptor = "wpkh(" + descriptor_or_xpub + ")";
    }
  }

  auto desc = Descriptor::Parse(descriptor);
  if (descriptor.find("/") != std::string::npos) {
    auto start_pos = descriptor.rfind("(") + 1;
    auto end_pos = descriptor.find("/");
    std::string extkey = descriptor.substr(start_pos, end_pos - start_pos);
    ExtPubkey pk(extkey);
    auto index_str = std::to_string(bip32_counter);
    std::vector<std::string> args = {index_str};
    auto derived_key = desc.GetKeyData(args);
    auto check_key = pk.DerivePubkey("0/" + index_str);
    if (!derived_key.GetPubkey().Equals(check_key.GetPubkey())) {
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid descriptor format. This path is not support on pegout.");
    }

    auto ref = desc.GetReference(&args);
    descriptor = pk.ToString();
    auto addr_type = ref.GetAddressType();
    if (addr_type == AddressType::kP2pkhAddress) {
      descriptor = "pkh(" + descriptor + ")";
    } else if (addr_type == AddressType::kP2shP2wpkhAddress) {
      descriptor = "sh(wpkh(" + descriptor + "))";
    } else if (addr_type == AddressType::kP2wpkhAddress) {
      descriptor = "wpkh(" + descriptor + ")";
    } else {
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid descriptor. This descriptor type isn't support on pegout.");
    }
    desc = Descriptor::Parse(descriptor);
  }

  auto addr = ConfidentialTransaction::GetPegoutAddressFromDescriptor(
      descriptor, bip32_counter, mainchain_network, elements_network);
  if (base_descriptor != nullptr) *base_descriptor = desc;
  return addr;
}

bool ElementsAddressFactory::CheckConfidentialAddressNetType(
    const ElementsConfidentialAddress& conf_addr, NetType net_type) const {
  return CheckAddressNetType(conf_addr.GetUnblindedAddress(), net_type);
}

}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
