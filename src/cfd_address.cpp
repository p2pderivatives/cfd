// Copyright 2019 CryptoGarage
/**
 * @file cfd_address.cpp
 *
 * @brief \~english implementation of classes related to address operation
 *   \~japanese Address操作の関連クラスの実装ファイル
 */

#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::GetBitcoinAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptElement;
using cfd::core::ScriptUtil;
using cfd::core::WitnessVersion;

AddressFactory::AddressFactory()
    : type_(NetType::kMainnet),
      wit_ver_(WitnessVersion::kVersion0),
      prefix_list_(GetBitcoinAddressFormatList()) {
  // do nothing
}

AddressFactory::AddressFactory(NetType type)
    : type_(type),
      wit_ver_(WitnessVersion::kVersion0),
      prefix_list_(GetBitcoinAddressFormatList()) {
  // do nothing
}

AddressFactory::AddressFactory(NetType type, WitnessVersion wit_ver)
    : type_(type),
      wit_ver_(wit_ver),
      prefix_list_(GetBitcoinAddressFormatList()) {
  // do nothing
}

AddressFactory::AddressFactory(
    NetType type, WitnessVersion wit_ver,
    const std::vector<AddressFormatData>& prefix_list)
    : type_(type), wit_ver_(wit_ver), prefix_list_(prefix_list) {
  // do nothing
}

AddressFactory::AddressFactory(
    NetType type, const std::vector<AddressFormatData>& prefix_list)
    : type_(type),
      wit_ver_(WitnessVersion::kVersion0),
      prefix_list_(prefix_list) {
  // do nothing
}

Address AddressFactory::GetAddress(const std::string& address_string) const {
  return Address(address_string, prefix_list_);
}

Address AddressFactory::GetAddressByLockingScript(
    const Script& locking_script) const {
  std::vector<ScriptElement> items = locking_script.GetElementList();
  if (locking_script.IsP2pkScript()) {
    Pubkey pubkey = Pubkey(items[0].GetBinaryData());
    return CreateP2pkhAddress(pubkey);
  } else if (locking_script.IsP2pkhScript()) {
    ByteData160 hash = ByteData160(items[2].GetBinaryData().GetBytes());
    return GetAddressByHash(AddressType::kP2pkhAddress, hash);
  } else if (locking_script.IsP2shScript()) {
    ByteData160 hash = ByteData160(items[1].GetBinaryData().GetBytes());
    return GetAddressByHash(AddressType::kP2shAddress, hash);
  } else if (
      locking_script.IsP2wpkhScript() || locking_script.IsP2wshScript()) {
    return GetSegwitAddressByHash(items[1].GetBinaryData());
  } else if (locking_script.IsMultisigScript()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "script type is multisig script.");
  } else {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "unknown type locking script.");
  }
}

Address AddressFactory::GetAddressByHash(
    AddressType address_type, const ByteData& hash) const {
  return GetAddressByHash(address_type, ByteData160(hash.GetBytes()));
}

Address AddressFactory::GetAddressByHash(
    AddressType address_type, const ByteData160& hash) const {
  return Address(type_, address_type, hash, prefix_list_);
}

Address AddressFactory::GetSegwitAddressByHash(const ByteData& hash) const {
  return Address(type_, wit_ver_, hash, prefix_list_);
}

Address AddressFactory::CreateP2pkhAddress(const Pubkey& pubkey) const {
  return Address(type_, pubkey, prefix_list_);
}

Address AddressFactory::CreateP2shAddress(const Script& script) const {
  return Address(type_, script, prefix_list_);
}

Address AddressFactory::CreateP2wpkhAddress(const Pubkey& pubkey) const {
  return Address(type_, wit_ver_, pubkey, prefix_list_);
}

Address AddressFactory::CreateP2wshAddress(const Script& script) const {
  return Address(type_, wit_ver_, script, prefix_list_);
}

Address AddressFactory::CreateP2wshMultisigAddress(
    uint32_t require_num, const std::vector<Pubkey>& pubkeys) const {
  Script script = ScriptUtil::CreateMultisigRedeemScript(require_num, pubkeys);
  return Address(type_, wit_ver_, script, prefix_list_);
}

bool AddressFactory::CheckAddressNetType(
    const Address& address, NetType net_type) const {
  if (address.GetNetType() == net_type) {
    return true;
  }

  // check prefix
  bool result = false;
  AddressType addr_type = address.GetAddressType();
  AddressFormatData addr_format = address.GetAddressFormatData();
  for (auto& prefix : prefix_list_) {
    if (prefix.GetNetType() == net_type) {
      switch (addr_type) {
        case AddressType::kP2pkhAddress:
          result = (prefix.GetP2pkhPrefix() == addr_format.GetP2pkhPrefix());
          break;
        case AddressType::kP2shP2wpkhAddress:
          // fall-through
        case AddressType::kP2shP2wshAddress:
          // fall-through
        case AddressType::kP2shAddress:
          result = (prefix.GetP2shPrefix() == addr_format.GetP2shPrefix());
          break;
        case AddressType::kP2wpkhAddress:
        case AddressType::kP2wshAddress:
          result = (prefix.GetBech32Hrp() == addr_format.GetBech32Hrp());
          break;
        default:
          result = false;
          break;
      }
      break;
    }
  }

  return result;
}

}  // namespace cfd
