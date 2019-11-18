// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_address.cpp
 *
 * @brief-eng implementation of address operation that uses cfd-api
 * @brief-jp cfd-apiで利用するAddress操作の実装ファイル
 */
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd_manager.h"  // NOLINT
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"

#include "cfd/cfdapi_address.h"

namespace cfd {
namespace api {

using cfd::AddressFactory;
using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Descriptor;
using cfd::core::DescriptorKeyReference;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorNode;
using cfd::core::DescriptorScriptReference;
using cfd::core::DescriptorScriptType;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::WitnessVersion;
using cfd::core::logger::warn;

Address AddressApi::CreateAddress(
    NetType net_type, AddressType address_type, const Pubkey* pubkey,
    const Script* script, Script* locking_script, Script* redeem_script,
    const std::vector<AddressFormatData>* prefix_list) const {
  if ((pubkey == nullptr) || (!pubkey->IsValid())) {
    if (address_type == AddressType::kP2pkhAddress ||
        address_type == AddressType::kP2wpkhAddress ||
        address_type == AddressType::kP2shP2wpkhAddress) {
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
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetBitcoinAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }
  Address addr;

  Script temp_script;
  switch (address_type) {
    case AddressType::kP2pkhAddress:
      addr = Address(net_type, *pubkey, addr_prefixes);
      if (locking_script != nullptr) {
        *locking_script = ScriptUtil::CreateP2pkhLockingScript(*pubkey);
      }
      break;
    case AddressType::kP2shAddress:
      addr = Address(net_type, *script, addr_prefixes);
      if (locking_script != nullptr) {
        *locking_script = ScriptUtil::CreateP2shLockingScript(*script);
      }
      break;
    case AddressType::kP2wpkhAddress:
      addr =
          Address(net_type, WitnessVersion::kVersion0, *pubkey, addr_prefixes);
      if (locking_script != nullptr) {
        *locking_script = ScriptUtil::CreateP2wpkhLockingScript(*pubkey);
      }
      break;
    case AddressType::kP2wshAddress:
      addr =
          Address(net_type, WitnessVersion::kVersion0, *script, addr_prefixes);
      if (locking_script != nullptr) {
        *locking_script = ScriptUtil::CreateP2wshLockingScript(*script);
      }
      break;
    case AddressType::kP2shP2wpkhAddress:
      temp_script = ScriptUtil::CreateP2wpkhLockingScript(*pubkey);
      addr = Address(net_type, temp_script, addr_prefixes);
      if (locking_script != nullptr) {
        *locking_script = ScriptUtil::CreateP2shLockingScript(temp_script);
      }
      if (redeem_script != nullptr) {
        *redeem_script = temp_script;
      }
      break;
    case AddressType::kP2shP2wshAddress:
      temp_script = ScriptUtil::CreateP2wshLockingScript(*script);
      addr = Address(net_type, temp_script, addr_prefixes);
      if (locking_script != nullptr) {
        *locking_script = ScriptUtil::CreateP2shLockingScript(temp_script);
      }
      if (redeem_script != nullptr) {
        *redeem_script = temp_script;
      }
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

  return addr;
}

Address AddressApi::CreateMultisig(
    NetType net_type, AddressType address_type, uint32_t req_sig_num,
    const std::vector<Pubkey>& pubkeys, Script* redeem_script,
    Script* witness_script,
    const std::vector<AddressFormatData>* prefix_list) const {
  // Multisig redeem scriptの作成
  Script multisig_script =
      ScriptUtil::CreateMultisigRedeemScript(req_sig_num, pubkeys);

  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetBitcoinAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }

  // Address作成
  Address addr;
  Script script;
  if (address_type == AddressType::kP2shAddress) {
    addr = Address(net_type, multisig_script, addr_prefixes);
    if (redeem_script != nullptr) {
      *redeem_script = multisig_script;
    }
  } else if (address_type == AddressType::kP2wshAddress) {
    // Currently we support only witness version 0.
    addr = Address(
        net_type, WitnessVersion::kVersion0, multisig_script, addr_prefixes);
    if (witness_script != nullptr) {
      *witness_script = multisig_script;
    }
  } else if (address_type == AddressType::kP2shP2wshAddress) {
    script = ScriptUtil::CreateP2wshLockingScript(multisig_script);
    addr = Address(net_type, script, addr_prefixes);
    if (redeem_script != nullptr) {
      *redeem_script = script;
    }
    if (witness_script != nullptr) {
      *witness_script = multisig_script;
    }
  } else {
    warn(
        CFD_LOG_SOURCE,
        "Failed to CreateMultisig. Invalid address_type passed:  "
        "addressType={}",  // NOLINT
        address_type);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid address_type. address_type must be \"p2sh\" "
        "\"p2wsh\" or \"p2sh-p2wsh\".");  // NOLINT
  }
  return addr;
}

std::vector<Address> AddressApi::GetAddressesFromMultisig(
    NetType net_type, AddressType address_type, const Script& redeem_script,
    std::vector<Pubkey>* pubkey_list,
    const std::vector<AddressFormatData>* prefix_list) const {
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetBitcoinAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }

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
  AddressFactory addr_factory(net_type, addr_prefixes);

  std::vector<Pubkey> pubkeys =
      ScriptUtil::ExtractPubkeysFromMultisigScript(redeem_script);

  std::vector<Address> addr_list;
  Address addr;
  Script script;
  for (const auto& pubkey : pubkeys) {
    if (address_type == AddressType::kP2pkhAddress) {
      addr = addr_factory.CreateP2pkhAddress(pubkey);
    } else if (address_type == AddressType::kP2shP2wpkhAddress) {
      script = ScriptUtil::CreateP2wpkhLockingScript(pubkey);
      addr = addr_factory.CreateP2shAddress(script);
    } else if (address_type == AddressType::kP2wpkhAddress) {
      // Currently we support only witness version 0.
      addr = addr_factory.CreateP2wpkhAddress(pubkey);
    }
    addr_list.push_back(addr);
  }

  if (pubkey_list) {
    *pubkey_list = pubkeys;
  }

  return addr_list;
}

DescriptorScriptData AddressApi::ParseOutputDescriptor(
    const std::string& descriptor, NetType net_type,
    const std::string& bip32_derivation_path,
    std::vector<DescriptorScriptData>* script_list,
    std::vector<DescriptorKeyData>* multisig_key_list,
    const std::vector<AddressFormatData>* prefix_list) const {
  std::vector<AddressFormatData> addr_prefixes;
  if (prefix_list == nullptr) {
    addr_prefixes = cfd::core::GetBitcoinAddressFormatList();
  } else {
    addr_prefixes = *prefix_list;
  }
  if (script_list) script_list->clear();
  if (multisig_key_list) multisig_key_list->clear();

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
  Descriptor desc = Descriptor::Parse(descriptor, &addr_prefixes);
  std::vector<std::string> args;
  for (uint32_t index = 0; index < desc.GetNeedArgumentNum(); ++index) {
    args.push_back(bip32_derivation_path);
  }
  std::vector<DescriptorScriptReference> script_refs =
      desc.GetReferenceAll(&args);
  DescriptorNode node = desc.GetNode();

  result.type = node.GetScriptType();
  result.key_type = DescriptorKeyType::kDescriptorKeyNull;
  result.depth = 0;
  result.locking_script = script_refs[0].GetLockingScript();
  if (script_refs[0].HasAddress()) {
    result.address = script_refs[0].GenerateAddress(net_type);
    result.address_type = script_refs[0].GetAddressType();
  }
  if (script_refs[0].HasRedeemScript()) {
    result.redeem_script = script_refs[0].GetRedeemScript();
  }

  std::vector<DescriptorKeyReference> multisig_keys;
  bool use_script_list = false;
  DescriptorKeyReference key_ref;
  switch (result.type) {
    case DescriptorScriptType::kDescriptorScriptCombo:
      if (script_list) {
        for (const auto& ref : script_refs) {
          DescriptorScriptData ref_data;
          ref_data.type = ref.GetScriptType();
          ref_data.depth = 0;
          ref_data.locking_script = ref.GetLockingScript();
          if (ref.HasAddress()) {
            ref_data.address = ref.GenerateAddress(net_type);
            ref_data.address_type = ref.GetAddressType();
          }
          if (ref.HasRedeemScript()) {
            ref_data.redeem_script = ref.GetRedeemScript();
          } else {
            key_ref = ref.GetKeyList()[0];
            ref_data.key_type = key_ref.GetKeyType();
            ref_data.key = get_keystr_function(key_ref);
          }
          script_list->push_back(ref_data);
        }
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
      break;
    case DescriptorScriptType::kDescriptorScriptRaw:
    case DescriptorScriptType::kDescriptorScriptAddr:
    default:
      break;
  }

  if (use_script_list && script_list) {
    DescriptorScriptReference script_ref = script_refs[0];
    bool is_loop = script_ref.HasChild();
    uint32_t depth = 0;
    while (is_loop) {
      DescriptorScriptReference child;
      if (script_ref.HasChild()) {
        child = script_ref.GetChild();
      }
      DescriptorScriptData data;
      switch (script_ref.GetScriptType()) {
        case DescriptorScriptType::kDescriptorScriptSh:
        case DescriptorScriptType::kDescriptorScriptWsh:
          child = script_ref.GetChild();
          if ((child.GetScriptType() ==
               DescriptorScriptType::kDescriptorScriptMulti) ||
              (child.GetScriptType() ==
               DescriptorScriptType::kDescriptorScriptSortedMulti)) {
            multisig_keys = child.GetKeyList();
            is_loop = false;
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
      data.address = script_ref.GenerateAddress(net_type);
      data.address_type = script_ref.GetAddressType();
      if (script_ref.HasRedeemScript()) {
        data.redeem_script = script_ref.GetRedeemScript();
      }
      if (script_ref.HasKey()) {
        key_ref = script_ref.GetKeyList()[0];
        data.key_type = key_ref.GetKeyType();
        data.key = get_keystr_function(key_ref);
      } else {
        data.key_type = DescriptorKeyType::kDescriptorKeyNull;
      }

      script_list->push_back(data);
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

}  // namespace api
}  // namespace cfd
