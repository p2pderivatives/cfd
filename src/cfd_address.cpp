// Copyright 2019 CryptoGarage
/**
 * @file cfd_address.cpp
 *
 * @brief-eng Address操作の関連クラスの実装ファイル
 * @brief-jp implementation of classes related to address operation
 */

#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::GetBitcoinAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
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

}  // namespace cfd
