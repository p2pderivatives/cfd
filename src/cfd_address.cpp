// Copyright 2019 CryptoGarage
/**
 * @file cfd_address.cpp
 *
 * @brief implementation of classes related to address operation
 */

#include "cfd/cfd_address.h"

#include <string>
#include <vector>

#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Descriptor;
using cfd::core::DescriptorKeyReference;
using cfd::core::DescriptorNode;
using cfd::core::DescriptorScriptReference;
using cfd::core::GetBitcoinAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptElement;
using cfd::core::ScriptOperator;
using cfd::core::ScriptType;
using cfd::core::ScriptUtil;
using cfd::core::TaprootScriptTree;
using cfd::core::WitnessVersion;
using cfd::core::logger::warn;

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
  } else if (locking_script.IsTaprootScript()) {
    return GetSegwitAddressByHash(
        items[1].GetBinaryData(), WitnessVersion::kVersion1);
  } else if (
      locking_script.IsWitnessProgram() &&
      (items[0].GetOpCode() != ScriptOperator::OP_0)) {
    uint8_t var = items[0].GetOpCode().GetDataType() - ScriptType::kOp_1;
    return GetSegwitAddressByHash(
        items[1].GetBinaryData(), static_cast<WitnessVersion>(var + 1));
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

Address AddressFactory::GetSegwitAddressByHash(
    const ByteData& hash, WitnessVersion version) const {
  return Address(type_, version, hash, prefix_list_);
}

Address AddressFactory::CreateP2pkhAddress(const Pubkey& pubkey) const {
  return Address(type_, pubkey, prefix_list_);
}

Address AddressFactory::CreateP2shAddress(const Script& script) const {
  return Address(type_, script, prefix_list_);
}

Address AddressFactory::CreateP2wpkhAddress(const Pubkey& pubkey) const {
  return Address(type_, WitnessVersion::kVersion0, pubkey, prefix_list_);
}

Address AddressFactory::CreateP2wshAddress(const Script& script) const {
  return Address(type_, WitnessVersion::kVersion0, script, prefix_list_);
}

Address AddressFactory::CreateP2wshMultisigAddress(
    uint32_t require_num, const std::vector<Pubkey>& pubkeys) const {
  Script script = ScriptUtil::CreateMultisigRedeemScript(require_num, pubkeys);
  return Address(type_, WitnessVersion::kVersion0, script, prefix_list_);
}

Address AddressFactory::CreateTaprootAddress(
    const SchnorrPubkey& pubkey) const {
  return Address(type_, WitnessVersion::kVersion1, pubkey, prefix_list_);
}

Address AddressFactory::CreateTaprootAddress(
    const TaprootScriptTree& tree,
    const SchnorrPubkey& internal_pubkey) const {
  auto pk = tree.GetTweakedPubkey(internal_pubkey);
  return CreateTaprootAddress(pk);
}

Address AddressFactory::CreateTaprootAddress(const ByteData256& hash) const {
  return CreateTaprootAddress(SchnorrPubkey(hash));
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
          // fall-through
        case AddressType::kP2wshAddress:
          // fall-through
        case AddressType::kTaprootAddress:
          // fall-through
        case AddressType::kWitnessUnknown:
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

Address AddressFactory::CreateAddress(
    AddressType address_type, const Pubkey* pubkey, const Script* script,
    Script* locking_script, Script* redeem_script) const {
  if ((pubkey == nullptr) || (!pubkey->IsValid())) {
    if (address_type == AddressType::kP2pkhAddress ||
        address_type == AddressType::kP2wpkhAddress ||
        address_type == AddressType::kP2shP2wpkhAddress ||
        address_type == AddressType::kTaprootAddress) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to CreateAddress. Invalid pubkey hex: pubkey is empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "pubkey hex is empty.");
    }
  }
  if ((script == nullptr) || (script->IsEmpty())) {
    if (address_type == AddressType::kP2shAddress ||
        address_type == AddressType::kP2wshAddress ||
        address_type == AddressType::kP2shP2wshAddress) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to CreateAddress. Invalid script hex: script is empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "script hex is empty.");
    }
  }
  Address addr;

  Script temp_script;
  switch (address_type) {
    case AddressType::kP2pkhAddress:
      addr = CreateP2pkhAddress(*pubkey);
      break;
    case AddressType::kP2shAddress:
      addr = CreateP2shAddress(*script);
      break;
    case AddressType::kP2wpkhAddress:
      addr = CreateP2wpkhAddress(*pubkey);
      break;
    case AddressType::kP2wshAddress:
      addr = CreateP2wshAddress(*script);
      break;
    case AddressType::kP2shP2wpkhAddress:
      temp_script = ScriptUtil::CreateP2wpkhLockingScript(*pubkey);
      addr = CreateP2shAddress(temp_script);
      if (redeem_script != nullptr) {
        *redeem_script = temp_script;
      }
      break;
    case AddressType::kP2shP2wshAddress:
      temp_script = ScriptUtil::CreateP2wshLockingScript(*script);
      addr = CreateP2shAddress(temp_script);
      if (redeem_script != nullptr) {
        *redeem_script = temp_script;
      }
      break;
    case AddressType::kTaprootAddress:
      addr = CreateTaprootAddress(SchnorrPubkey::FromPubkey(*pubkey));
      break;
    default:
      warn(
          CFD_LOG_SOURCE,
          "Failed to CreateAddress. Invalid address type:  address_type={}",
          address_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid address_type. address_type must be \"p2pkh\" or "
          "\"p2sh\" or \"p2wpkh\" or \"p2wsh\" or \"p2sh-p2wpkh\" or "
          "\"p2sh-p2wsh\".");  // NOLINT
      break;
  }
  if (locking_script != nullptr) {
    *locking_script = addr.GetLockingScript();
  }

  return addr;
}

std::vector<Address> AddressFactory::GetAddressesFromMultisig(
    AddressType address_type, const Script& redeem_script,
    std::vector<Pubkey>* pubkey_list) const {
  if ((address_type != AddressType::kP2pkhAddress) &&
      (address_type != AddressType::kP2wpkhAddress) &&
      (address_type != AddressType::kP2shP2wpkhAddress)) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to GetAddressesFromMultisig. Invalid address_type passed:  "
        "addressType={}",  // NOLINT
        address_type);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid address_type. address_type must be \"p2pkh\" "
        "\"p2wpkh\" or \"p2sh-p2wpkh\".");  // NOLINT
  }

  std::vector<Pubkey> pubkeys =
      ScriptUtil::ExtractPubkeysFromMultisigScript(redeem_script);

  std::vector<Address> addr_list;
  Address addr;
  Script script;
  for (const auto& pubkey : pubkeys) {
    if (address_type == AddressType::kP2pkhAddress) {
      addr = CreateP2pkhAddress(pubkey);
    } else if (address_type == AddressType::kP2shP2wpkhAddress) {
      script = ScriptUtil::CreateP2wpkhLockingScript(pubkey);
      addr = CreateP2shAddress(script);
    } else if (address_type == AddressType::kP2wpkhAddress) {
      // Currently we support only witness version 0.
      addr = CreateP2wpkhAddress(pubkey);
    }
    addr_list.push_back(addr);
  }

  if (pubkey_list) *pubkey_list = pubkeys;
  return addr_list;
}

DescriptorScriptData AddressFactory::ParseOutputDescriptor(
    const std::string& descriptor, const std::string& bip32_derivation_path,
    std::vector<DescriptorScriptData>* script_list,
    std::vector<DescriptorKeyData>* multisig_key_list,
    std::vector<KeyData>* key_list) const {
  if (script_list != nullptr) script_list->clear();
  if (multisig_key_list != nullptr) multisig_key_list->clear();

  static auto get_keystr_function =
      [](const DescriptorKeyReference& key_ref) -> std::string {
    if (key_ref.HasExtPrivkey()) {
      return key_ref.GetExtPrivkey().ToString();
    } else if (key_ref.HasExtPubkey()) {
      return key_ref.GetExtPubkey().ToString();
    } else {
      return key_ref.GetPubkey().GetHex();
    }
  };

  DescriptorScriptData result;
  result.key_type = DescriptorKeyType::kDescriptorKeyNull;  // dummy init
  result.address_type = AddressType::kP2shAddress;          // dummy init
  result.multisig_req_sig_num = 0;
  Descriptor desc = Descriptor::Parse(descriptor, &prefix_list_);
  std::vector<std::string> args;
  for (uint32_t index = 0; index < desc.GetNeedArgumentNum(); ++index) {
    args.push_back(bip32_derivation_path);
  }
  if (key_list != nullptr) {
    auto temp_list = desc.GetKeyDataAll(&args);
    for (const auto& key : temp_list) {
      key_list->push_back(key);
    }
  }
  std::vector<DescriptorScriptReference> script_refs =
      desc.GetReferenceAll(&args);
  DescriptorNode node = desc.GetNode();

  result.type = node.GetScriptType();
  result.key_type = DescriptorKeyType::kDescriptorKeyNull;
  result.depth = 0;
  result.locking_script = script_refs[0].GetLockingScript();
  if (script_refs[0].HasAddress()) {
    result.address = script_refs[0].GenerateAddress(type_);
    result.address_type = script_refs[0].GetAddressType();
  }
  if (script_refs[0].HasRedeemScript()) {
    result.redeem_script = script_refs[0].GetRedeemScript();
  }

  std::vector<DescriptorKeyReference> multisig_keys;
  uint32_t multisig_req_num = 0;
  bool use_script_list = false;
  DescriptorKeyReference key_ref;
  switch (result.type) {
    case DescriptorScriptType::kDescriptorScriptCombo:
      for (const auto& ref : script_refs) {
        DescriptorScriptData ref_data;
        ref_data.type = ref.GetScriptType();
        ref_data.depth = 0;
        ref_data.locking_script = ref.GetLockingScript();
        if (ref.HasAddress()) {
          ref_data.address = ref.GenerateAddress(type_);
          ref_data.address_type = ref.GetAddressType();
        } else {
          ref_data.address_type = AddressType::kP2shAddress;  // dummy init
        }
        if (ref.HasRedeemScript()) {
          ref_data.redeem_script = ref.GetRedeemScript();
          ref_data.key_type = DescriptorKeyType::kDescriptorKeyNull;
        } else {
          key_ref = ref.GetKeyList()[0];
          ref_data.key_type = key_ref.GetKeyType();
          ref_data.key = get_keystr_function(key_ref);
          if (result.key_type == DescriptorKeyType::kDescriptorKeyNull) {
            result.key_type = ref_data.key_type;
            result.key = ref_data.key;
          }
        }
        if (script_list != nullptr) script_list->push_back(ref_data);
      }
      break;
    case DescriptorScriptType::kDescriptorScriptSh:
    case DescriptorScriptType::kDescriptorScriptWsh:
      use_script_list = true;
      break;
    case DescriptorScriptType::kDescriptorScriptPk:
    case DescriptorScriptType::kDescriptorScriptPkh:
    case DescriptorScriptType::kDescriptorScriptWpkh:
      key_ref = script_refs[0].GetKeyList()[0];
      result.key_type = key_ref.GetKeyType();
      result.key = get_keystr_function(key_ref);
      break;
    case DescriptorScriptType::kDescriptorScriptMulti:
    case DescriptorScriptType::kDescriptorScriptSortedMulti:
      multisig_keys = script_refs[0].GetKeyList();
      result.multisig_req_sig_num = script_refs[0].GetReqNum();
      break;
    case DescriptorScriptType::kDescriptorScriptTaproot:
      key_ref = script_refs[0].GetKeyList()[0];
      result.tree = script_refs[0].GetScriptTree();
      result.key_type = key_ref.GetKeyType();
      if (key_ref.HasExtPrivkey()) {
        result.key = key_ref.GetExtPrivkey().ToString();
      } else if (key_ref.HasExtPubkey()) {
        result.key = key_ref.GetExtPubkey().ToString();
      } else {
        result.key_type = DescriptorKeyType::kDescriptorKeySchnorr;
        result.key = key_ref.GetSchnorrPubkey().GetHex();
      }
      break;
    case DescriptorScriptType::kDescriptorScriptRaw:
    case DescriptorScriptType::kDescriptorScriptAddr:
    default:
      break;
  }

  if (use_script_list) {
    DescriptorScriptReference script_ref = script_refs[0];
    bool is_loop = script_ref.HasChild();
    uint32_t depth = 0;
    result.redeem_script = (is_loop) ? Script() : script_ref.GetRedeemScript();
    while (is_loop) {
      DescriptorScriptReference child;
      DescriptorScriptData data;
      switch (script_ref.GetScriptType()) {
        case DescriptorScriptType::kDescriptorScriptSh:
        case DescriptorScriptType::kDescriptorScriptWsh:
          if (!script_ref.HasChild()) {
            result.redeem_script = script_ref.GetRedeemScript();
          } else {
            child = script_ref.GetChild();
            if ((child.GetScriptType() ==
                 DescriptorScriptType::kDescriptorScriptMulti) ||
                (child.GetScriptType() ==
                 DescriptorScriptType::kDescriptorScriptSortedMulti)) {
              multisig_keys = child.GetKeyList();
              multisig_req_num = child.GetReqNum();
              result.multisig_req_sig_num = multisig_req_num;
              is_loop = false;
            } else if (
                child.GetScriptType() ==
                DescriptorScriptType::kDescriptorScriptMiniscript) {
              is_loop = false;
            }
            if (child.GetScriptType() !=
                DescriptorScriptType::kDescriptorScriptWpkh) {
              result.redeem_script = script_ref.GetRedeemScript();
            }
          }
          break;
        // case DescriptorScriptType::kDescriptorScriptPk:
        // case DescriptorScriptType::kDescriptorScriptPkh:
        // case DescriptorScriptType::kDescriptorScriptWpkh:
        default:
          is_loop = false;
          break;
      }
      data.type = script_ref.GetScriptType();
      data.depth = depth;
      data.locking_script = script_ref.GetLockingScript();
      data.address = script_ref.GenerateAddress(type_);
      data.address_type = script_ref.GetAddressType();
      if (script_ref.HasRedeemScript()) {
        data.redeem_script = script_ref.GetRedeemScript();
      }
      if (script_ref.HasKey()) {
        key_ref = script_ref.GetKeyList()[0];
        data.key_type = key_ref.GetKeyType();
        data.key = get_keystr_function(key_ref);
        result.key_type = key_ref.GetKeyType();
        result.key = get_keystr_function(key_ref);
      } else {
        data.key_type = DescriptorKeyType::kDescriptorKeyNull;
      }
      data.multisig_req_sig_num = multisig_req_num;

      if (script_list != nullptr) script_list->push_back(data);
      if (is_loop && script_ref.HasChild()) {
        child = script_ref.GetChild();
        script_ref = child;
        ++depth;
      } else {
        is_loop = false;
      }
    }
  }

  if ((!multisig_keys.empty()) && multisig_key_list) {
    for (const auto& ref : multisig_keys) {
      DescriptorKeyData key_data{ref.GetKeyType(), get_keystr_function(ref)};
      multisig_key_list->push_back(key_data);
    }
  }
  return result;
}

std::vector<AddressFormatData> AddressFactory::GetPrefixList() const {
  return prefix_list_;
}

}  // namespace cfd
