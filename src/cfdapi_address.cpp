// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_address.cpp
 *
 * @brief cfd-apiで利用するAddress操作の実装ファイル
 */
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd_manager.h"  // NOLINT
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"

#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_struct.h"
#include "cfd/cfdapi_utility.h"
#include "cfdapi_internal.h"  // NOLINT

namespace cfd {
namespace js {
namespace api {

using cfd::api::AddressApi;
using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;

CreateAddressResponseStruct AddressStructApi::CreateAddress(
    const CreateAddressRequestStruct& request) {
  auto call_func = [](const CreateAddressRequestStruct& request)
      -> CreateAddressResponseStruct {  // NOLINT
    CreateAddressResponseStruct response;
    // Address作成
    Address addr;
    Pubkey pubkey;
    Script script;
    Script locking_script;
    Script redeem_script;
    NetType net_type = ConvertNetType(request.network);
    AddressType addr_type = ConvertAddressType(request.hash_type);

    if (request.key_data.type == "pubkey") {
      pubkey = Pubkey(request.key_data.hex);
    } else if (request.key_data.type == "redeem_script") {
      script = Script(request.key_data.hex);
    }
    addr = AddressApi::CreateAddress(
        net_type, addr_type, &pubkey, &script, &locking_script,
        &redeem_script);

    // レスポンスとなるモデルへ変換
    response.error.code = 0;
    response.address = addr.GetAddress();
    response.locking_script = locking_script.GetHex();
    if (redeem_script.IsEmpty()) {
      response.ignore_items.insert("redeemScript");
    } else {
      response.redeem_script = redeem_script.GetHex();
    }
    return response;
  };

  CreateAddressResponseStruct result;
  result = ExecuteStructApi<
      CreateAddressRequestStruct, CreateAddressResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

CreateMultisigResponseStruct AddressStructApi::CreateMultisig(
    const CreateMultisigRequestStruct& request) {
  auto call_func = [](const CreateMultisigRequestStruct& request)
      -> CreateMultisigResponseStruct {  // NOLINT
    CreateMultisigResponseStruct response;
    // pubkeyモデルへの変換
    std::vector<Pubkey> pubkeys;
    for (std::string key : request.keys) {
      pubkeys.push_back(Pubkey(key));
    }

    uint32_t req_sig_num = static_cast<uint32_t>(request.nrequired);
    NetType net_type = ConvertNetType(request.network);
    AddressType addr_type = ConvertAddressType(request.hash_type);
    Script witness_script;
    Script redeem_script;

    Address addr = AddressApi::CreateMultisig(
        net_type, addr_type, req_sig_num, pubkeys, &redeem_script,
        &witness_script);

    // レスポンスとなるモデルへ変換
    response.address = addr.GetAddress();
    if (redeem_script.IsEmpty()) {
      response.ignore_items.insert("redeemScript");
    } else {
      response.redeem_script = redeem_script.GetHex();
    }
    if (witness_script.IsEmpty()) {
      response.ignore_items.insert("witnessScript");
    } else {
      response.witness_script = witness_script.GetHex();
    }
    return response;
  };

  CreateMultisigResponseStruct result;
  result = ExecuteStructApi<
      CreateMultisigRequestStruct, CreateMultisigResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

NetType AddressStructApi::ConvertNetType(const std::string& network_type) {
  NetType net_type;
  if (network_type == "mainnet") {
    net_type = NetType::kMainnet;
  } else if (network_type == "testnet") {
    net_type = NetType::kTestnet;
  } else if (network_type == "regtest") {
    net_type = NetType::kRegtest;
  } else {
    warn(
        CFD_LOG_SOURCE,
        "Failed to ConvertNetType. Invalid network_type passed:  "
        "network_type={}",  // NOLINT
        network_type);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid network_type passed. network_type must be \"mainnet\""
        " or \"testnet\" or \"regtest\".");
  }
  return net_type;
}

AddressType AddressStructApi::ConvertAddressType(
    const std::string& address_type) {
  AddressType addr_type;
  if (address_type == "p2pkh") {
    addr_type = AddressType::kP2pkhAddress;
  } else if (address_type == "p2sh") {
    addr_type = AddressType::kP2shAddress;
  } else if (address_type == "p2wpkh") {
    addr_type = AddressType::kP2wpkhAddress;
  } else if (address_type == "p2wsh") {
    addr_type = AddressType::kP2wshAddress;
  } else if (address_type == "p2sh-p2wpkh") {
    addr_type = AddressType::kP2shP2wpkhAddress;
  } else if (address_type == "p2sh-p2wsh") {
    addr_type = AddressType::kP2shP2wshAddress;
  } else {
    warn(
        CFD_LOG_SOURCE,
        "Failed to ConvertAddress Type. "
        "Invalid address_type passed:  address_type={}",
        address_type);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid address_type passed. address_type must be"
        " \"p2pkh\", \"p2sh\", \"p2wpkh\", \"p2wsh\", \"p2sh-p2wpkh\", or "
        "\"p2sh-p2wsh\".");
  }
  return addr_type;
}

}  // namespace api
}  // namespace js
}  // namespace cfd

namespace cfd {
namespace api {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::WitnessVersion;
using cfd::core::logger::warn;

Address AddressApi::CreateAddress(
    NetType net_type, AddressType address_type, const Pubkey* pubkey,
    const Script* script, Script* locking_script, Script* redeem_script,
    std::vector<AddressFormatData>* prefix_list) {
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
    Script* witness_script, std::vector<AddressFormatData>* prefix_list) {
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

}  // namespace api
}  // namespace cfd
