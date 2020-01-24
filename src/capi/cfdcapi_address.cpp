// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_address.cpp
 *
 * @brief cfd-capiで利用するAddress操作の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_elements_address.h"
#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::AddressFactory;
using cfd::api::AddressApi;
using cfd::api::DescriptorKeyData;
using cfd::api::DescriptorScriptData;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorScriptType;
using cfd::core::ExtPrivkey;
using cfd::core::ExtPubkey;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;

using cfd::core::logger::info;
using cfd::core::logger::warn;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::ElementsAddressFactory;
using cfd::api::ElementsAddressApi;
#endif  // CFD_DISABLE_ELEMENTS

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: MultisigScript
constexpr const char* const kPrefixMultisigScript = "MultisigScript";
//! prefix: OutputDescriptor
constexpr const char* const kPrefixOutputDescriptor = "Descriptor";
//! prefix: MultisigAddresses
constexpr const char* const kPrefixMultisigAddresses = "MultisigAddr";

/**
 * @brief cfd-capi MultisigScript構造体.
 */
struct CfdCapiMultisigScript {
  char prefix[kPrefixLength];  //!< buffer prefix
  int network_type;            //!< network type
  int hash_type;               //!< hash type
  //! pubkeys
  char pubkeys[kMultisigMaxKeyNum][kPubkeyHexSize + 1];
  uint32_t current_index;  //!< current index
};

/**
 * @brief cfd-capi OutputDescriptor構造体.
 * @details 情報量が多すぎるため、flat structにはしない。
 */
struct CfdCapiOutputDescriptor {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! script list
  std::vector<DescriptorScriptData>* script_list;
  //! multisig key list
  std::vector<DescriptorKeyData>* multisig_key_list;
};

/**
 * @brief cfd-capi MultisigAddresses構造体.
 */
struct CfdCapiMultisigAddresses {
  char prefix[kPrefixLength];               //!< buffer prefix
  char addresses[kMultisigMaxKeyNum][256];  //!< address list
  //! pubkeys
  char pubkeys[kMultisigMaxKeyNum][kPubkeyHexSize + 1];
  uint32_t list_size;  //!< list size
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiMultisigAddresses;
using cfd::capi::CfdCapiMultisigScript;
using cfd::capi::CfdCapiOutputDescriptor;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPrefixMultisigAddresses;
using cfd::capi::kPrefixMultisigScript;
using cfd::capi::kPrefixOutputDescriptor;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

extern "C" {

int CfdCreateAddress(
    void* handle, int hash_type, const char* pubkey, const char* redeem_script,
    int network_type, char** address, char** locking_script,
    char** p2sh_segwit_locking_script) {
  char* work_address = nullptr;
  char* work_locking_script = nullptr;
  char* work_p2sh_segwit_locking_script = nullptr;
  try {
    cfd::Initialize();
    if (address == nullptr) {
      warn(CFD_LOG_SOURCE, "address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. address is null.");
    }

    bool is_bitcoin = false;
    NetType net_type = cfd::capi::ConvertNetType(network_type, &is_bitcoin);
    Address addr;
    AddressType addr_type = ConvertHashToAddressType(hash_type);
    Pubkey pubkey_obj;
    Script script;
    Script lock_script;
    Script unlocking_script;

    if ((pubkey != nullptr) && (*pubkey != '\0')) {
      pubkey_obj = Pubkey(pubkey);
    }
    if ((redeem_script != nullptr) && (*redeem_script != '\0')) {
      script = Script(redeem_script);
    }

    if (is_bitcoin) {
      AddressApi api;
      addr = api.CreateAddress(
          net_type, addr_type, &pubkey_obj, &script, &lock_script,
          &unlocking_script);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ElementsAddressApi e_api;
      addr = e_api.CreateAddress(
          net_type, addr_type, &pubkey_obj, &script, &lock_script,
          &unlocking_script);
#else
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Elements not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    work_address = CreateString(addr.GetAddress());
    if (locking_script != nullptr) {
      if (!lock_script.IsEmpty()) {
        work_locking_script = CreateString(lock_script.GetHex());
      }
    }
    if (p2sh_segwit_locking_script != nullptr) {
      if (!unlocking_script.IsEmpty()) {
        work_p2sh_segwit_locking_script =
            CreateString(unlocking_script.GetHex());
      }
    }

    *address = work_address;
    if (work_locking_script != nullptr) *locking_script = work_locking_script;
    if (work_p2sh_segwit_locking_script != nullptr)
      *p2sh_segwit_locking_script = work_p2sh_segwit_locking_script;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(
        &work_address, &work_locking_script, &work_p2sh_segwit_locking_script);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(
        &work_address, &work_locking_script, &work_p2sh_segwit_locking_script);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(
        &work_address, &work_locking_script, &work_p2sh_segwit_locking_script);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdInitializeMultisigScript(
    void* handle, int network_type, int hash_type, void** multisig_handle) {
  try {
    cfd::Initialize();
    if (multisig_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "multisig handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. multisig handle is null.");
    }
    cfd::capi::ConvertNetType(network_type, nullptr);
    AddressType addr_type = ConvertHashToAddressType(hash_type);
    switch (addr_type) {
      case AddressType::kP2shAddress:
      case AddressType::kP2wshAddress:
      case AddressType::kP2shP2wshAddress:
        break;
      default:
        warn(CFD_LOG_SOURCE, "pkh is not target.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. hash type cannot used pkh target.");
    }

    *multisig_handle =
        AllocBuffer(kPrefixMultisigScript, sizeof(CfdCapiMultisigScript));
    CfdCapiMultisigScript* data =
        static_cast<CfdCapiMultisigScript*>(*multisig_handle);
    data->network_type = network_type;
    data->hash_type = hash_type;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdAddMultisigScriptData(
    void* handle, void* multisig_handle, const char* pubkey) {
  try {
    cfd::Initialize();
    CheckBuffer(multisig_handle, kPrefixMultisigScript);
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }

    CfdCapiMultisigScript* data =
        static_cast<CfdCapiMultisigScript*>(multisig_handle);
    if (data->current_index >= kMultisigMaxKeyNum) {
      warn(
          CFD_LOG_SOURCE, "The number of pubkey has reached the upper limit.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "The number of pubkey has reached the upper limit.");
    }

    Pubkey key = Pubkey(std::string(pubkey));
    std::string pubkey_str = key.GetHex();
    pubkey_str.copy(data->pubkeys[data->current_index], kPubkeyHexSize);
    ++(data->current_index);
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdFinalizeMultisigScript(
    void* handle, void* multisig_handle, uint32_t require_num, char** address,
    char** redeem_script, char** witness_script) {
  char* work_address = nullptr;
  char* work_redeem_script = nullptr;
  char* work_witness_script = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(multisig_handle, kPrefixMultisigScript);

    CfdCapiMultisigScript* data =
        static_cast<CfdCapiMultisigScript*>(multisig_handle);
    if (data->current_index == 0) {
      warn(CFD_LOG_SOURCE, "pubkey not found.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to parameter. pubkey not found.");
    }
    if (data->current_index > kMultisigMaxKeyNum) {
      warn(
          CFD_LOG_SOURCE, "The number of pubkey has reached the upper limit.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "The number of pubkey has reached the upper limit.");
    }

    bool is_bitcoin = false;
    NetType net_type =
        cfd::capi::ConvertNetType(data->network_type, &is_bitcoin);
    Address addr;
    AddressType addr_type = ConvertHashToAddressType(data->hash_type);
    std::vector<Pubkey> pubkeys;
    Script script;
    Script redeem_script_obj;
    Script witness_script_obj;

    for (uint32_t index = 0; index < data->current_index; ++index) {
      pubkeys.emplace_back(std::string(data->pubkeys[index]));
    }

    if (is_bitcoin) {
      AddressApi api;
      addr = api.CreateMultisig(
          net_type, addr_type, require_num, pubkeys, &redeem_script_obj,
          &witness_script_obj);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ElementsAddressApi e_api;
      addr = e_api.CreateMultisig(
          net_type, addr_type, require_num, pubkeys, &redeem_script_obj,
          &witness_script_obj);
#else
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Elements not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    work_address = CreateString(addr.GetAddress());
    if (redeem_script != nullptr) {
      if (!redeem_script_obj.IsEmpty()) {
        work_redeem_script = CreateString(redeem_script_obj.GetHex());
      }
    }
    if (witness_script != nullptr) {
      if (!witness_script_obj.IsEmpty()) {
        work_witness_script = CreateString(witness_script_obj.GetHex());
      }
    }

    *address = work_address;
    if (work_redeem_script != nullptr) *redeem_script = work_redeem_script;
    if (work_witness_script != nullptr) *witness_script = work_witness_script;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(
        &work_address, &work_redeem_script, &work_witness_script);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(
        &work_address, &work_redeem_script, &work_witness_script);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(
        &work_address, &work_redeem_script, &work_witness_script);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdFreeMultisigScriptHandle(void* handle, void* multisig_handle) {
  try {
    cfd::Initialize();
    FreeBuffer(
        multisig_handle, kPrefixMultisigScript, sizeof(CfdCapiMultisigScript));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdParseDescriptor(
    void* handle, const char* descriptor, int network_type,
    const char* bip32_derivation_path, void** descriptor_handle,
    uint32_t* max_index) {
  CfdCapiOutputDescriptor* buffer = nullptr;
  try {
    cfd::Initialize();
    if (descriptor_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "descriptor handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. descriptor handle is null.");
    }
    if (descriptor == nullptr) {
      warn(CFD_LOG_SOURCE, "descriptor is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. descriptor is null.");
    }

    bool is_bitcoin = false;
    NetType net_type = cfd::capi::ConvertNetType(network_type, &is_bitcoin);
    std::string derive_path;
    if ((bip32_derivation_path != nullptr) &&
        (*bip32_derivation_path != '\0')) {
      derive_path = std::string(bip32_derivation_path);
    }

    std::vector<DescriptorScriptData> script_list;
    std::vector<DescriptorKeyData> multisig_key_list;
    DescriptorScriptData desc_data;
    if (is_bitcoin) {
      AddressApi api;
      desc_data = api.ParseOutputDescriptor(
          descriptor, net_type, derive_path, &script_list, &multisig_key_list);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ElementsAddressApi e_api;
      desc_data = e_api.ParseOutputDescriptor(
          descriptor, net_type, derive_path, &script_list, &multisig_key_list);
#else
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Elements not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }
    if (script_list.empty()) {
      script_list.push_back(desc_data);
    }

    buffer = static_cast<CfdCapiOutputDescriptor*>(
        AllocBuffer(kPrefixOutputDescriptor, sizeof(CfdCapiOutputDescriptor)));
    buffer->script_list = new std::vector<DescriptorScriptData>();
    buffer->multisig_key_list = new std::vector<DescriptorKeyData>();
    buffer->script_list->assign(script_list.begin(), script_list.end());
    // *(buffer->script_list) = script_list;
    buffer->multisig_key_list->assign(
        multisig_key_list.begin(), multisig_key_list.end());
    // *(buffer->multisig_key_list) = multisig_key_list;
    if (max_index != nullptr) {
      *max_index = static_cast<uint32_t>(script_list.size() - 1);
    }
    *descriptor_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    if (buffer != nullptr) CfdFreeDescriptorHandle(handle, buffer);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    if (buffer != nullptr) CfdFreeDescriptorHandle(handle, buffer);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    if (buffer != nullptr) CfdFreeDescriptorHandle(handle, buffer);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetDescriptorData(
    void* handle, void* descriptor_handle, uint32_t index, uint32_t* max_index,
    uint32_t* depth, int* script_type, char** locking_script, char** address,
    int* hash_type, char** redeem_script, int* key_type, char** pubkey,
    char** ext_pubkey, char** ext_privkey, bool* is_multisig,
    uint32_t* max_key_num, uint32_t* req_sig_num) {
  char* work_locking_script = nullptr;
  char* work_address = nullptr;
  char* work_redeem_script = nullptr;
  char* work_pubkey = nullptr;
  char* work_ext_pubkey = nullptr;
  char* work_ext_privkey = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(descriptor_handle, kPrefixOutputDescriptor);

    CfdCapiOutputDescriptor* buffer =
        static_cast<CfdCapiOutputDescriptor*>(descriptor_handle);
    if ((buffer->script_list == nullptr) ||
        (index >= buffer->script_list->size())) {
      warn(CFD_LOG_SOURCE, "index is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. index is maximum over.");
    }

    const DescriptorScriptData& desc_data = buffer->script_list->at(index);
    uint32_t last_index =
        static_cast<uint32_t>(buffer->script_list->size()) - 1;
    if (max_index != nullptr) *max_index = last_index;
    if (depth != nullptr) *depth = desc_data.depth;
    if (script_type != nullptr) *script_type = desc_data.type;
    if ((locking_script != nullptr) && (!desc_data.locking_script.IsEmpty())) {
      work_locking_script = CreateString(desc_data.locking_script.GetHex());
    }
    if ((address != nullptr) &&
        (desc_data.type != DescriptorScriptType::kDescriptorScriptRaw) &&
        (desc_data.type != DescriptorScriptType::kDescriptorScriptRaw)) {
      std::string addr = desc_data.address.GetAddress();
      if (!addr.empty()) work_address = CreateString(addr);
    }
    if (hash_type != nullptr) {
      *hash_type = desc_data.address_type;
    }
    if ((redeem_script != nullptr) && (!desc_data.redeem_script.IsEmpty())) {
      work_redeem_script = CreateString(desc_data.redeem_script.GetHex());
    }
    if (key_type != nullptr) {
      *key_type = desc_data.key_type;
    }

    Pubkey pubkey_obj;
    ExtPubkey ext_pubkey_obj;
    ExtPrivkey ext_privkey_obj;
    switch (desc_data.key_type) {
      case DescriptorKeyType::kDescriptorKeyBip32Priv:
        ext_privkey_obj = ExtPrivkey(desc_data.key);
        ext_pubkey_obj = ext_privkey_obj.GetExtPubkey();
        pubkey_obj = ext_pubkey_obj.GetPubkey();
        break;
      case DescriptorKeyType::kDescriptorKeyBip32:
        ext_pubkey_obj = ExtPubkey(desc_data.key);
        pubkey_obj = ext_pubkey_obj.GetPubkey();
        break;
      case DescriptorKeyType::kDescriptorKeyPublic:
      default:
        if (!desc_data.key.empty()) pubkey_obj = Pubkey(desc_data.key);
        break;
    }
    if ((pubkey != nullptr) && (pubkey_obj.IsValid())) {
      work_pubkey = CreateString(pubkey_obj.GetHex());
    }
    if ((ext_pubkey != nullptr) && (ext_pubkey_obj.IsValid())) {
      work_ext_pubkey = CreateString(ext_pubkey_obj.ToString());
    }
    if ((ext_privkey != nullptr) && (ext_privkey_obj.IsValid())) {
      work_ext_privkey = CreateString(ext_privkey_obj.ToString());
    }
    if (is_multisig != nullptr) {
      if ((!buffer->multisig_key_list->empty()) && (index == last_index)) {
        *is_multisig = true;
        if (max_key_num != nullptr) {
          *max_key_num =
              static_cast<uint32_t>(buffer->multisig_key_list->size());
        }
        if (req_sig_num != nullptr) {
          *req_sig_num = static_cast<uint32_t>(desc_data.multisig_req_sig_num);
        }
      } else {
        *is_multisig = false;
      }
    }

    if (work_locking_script != nullptr) *locking_script = work_locking_script;
    if (work_address != nullptr) *address = work_address;
    if (work_redeem_script != nullptr) *redeem_script = work_redeem_script;
    if (work_pubkey != nullptr) *pubkey = work_pubkey;
    if (work_ext_pubkey != nullptr) *ext_pubkey = work_ext_pubkey;
    if (work_ext_privkey != nullptr) *ext_privkey = work_ext_privkey;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(
        &work_locking_script, &work_address, &work_redeem_script, &work_pubkey,
        &work_ext_pubkey, &work_ext_privkey);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(
        &work_locking_script, &work_address, &work_redeem_script, &work_pubkey,
        &work_ext_pubkey, &work_ext_privkey);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(
        &work_locking_script, &work_address, &work_redeem_script, &work_pubkey,
        &work_ext_pubkey, &work_ext_privkey);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetDescriptorMultisigKey(
    void* handle, void* descriptor_handle, uint32_t key_index, int* key_type,
    char** pubkey, char** ext_pubkey, char** ext_privkey) {
  char* work_pubkey = nullptr;
  char* work_ext_pubkey = nullptr;
  char* work_ext_privkey = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(descriptor_handle, kPrefixOutputDescriptor);

    CfdCapiOutputDescriptor* buffer =
        static_cast<CfdCapiOutputDescriptor*>(descriptor_handle);
    if ((buffer->multisig_key_list == nullptr) ||
        (key_index >= buffer->multisig_key_list->size())) {
      warn(CFD_LOG_SOURCE, "key index is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. key index is maximum over.");
    }

    DescriptorKeyData key_data = (*buffer->multisig_key_list)[key_index];
    if (key_type != nullptr) {
      *key_type = key_data.type;
    }

    Pubkey pubkey_obj;
    ExtPubkey ext_pubkey_obj;
    ExtPrivkey ext_privkey_obj;
    switch (key_data.type) {
      case DescriptorKeyType::kDescriptorKeyBip32Priv:
        ext_privkey_obj = ExtPrivkey(key_data.key);
        ext_pubkey_obj = ext_privkey_obj.GetExtPubkey();
        pubkey_obj = ext_pubkey_obj.GetPubkey();
        break;
      case DescriptorKeyType::kDescriptorKeyBip32:
        ext_pubkey_obj = ExtPubkey(key_data.key);
        pubkey_obj = ext_pubkey_obj.GetPubkey();
        break;
      case DescriptorKeyType::kDescriptorKeyPublic:
      default:
        if (!key_data.key.empty()) pubkey_obj = Pubkey(key_data.key);
        break;
    }
    if ((pubkey != nullptr) && (pubkey_obj.IsValid())) {
      work_pubkey = CreateString(pubkey_obj.GetHex());
    }
    if ((ext_pubkey != nullptr) && (ext_pubkey_obj.IsValid())) {
      work_ext_pubkey = CreateString(ext_pubkey_obj.ToString());
    }
    if ((ext_privkey != nullptr) && (ext_privkey_obj.IsValid())) {
      work_ext_privkey = CreateString(ext_privkey_obj.ToString());
    }

    if (work_pubkey != nullptr) *pubkey = work_pubkey;
    if (work_ext_pubkey != nullptr) *ext_pubkey = work_ext_pubkey;
    if (work_ext_privkey != nullptr) *ext_privkey = work_ext_privkey;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(&work_pubkey, &work_ext_pubkey, &work_ext_privkey);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(&work_pubkey, &work_ext_pubkey, &work_ext_privkey);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(&work_pubkey, &work_ext_pubkey, &work_ext_privkey);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdFreeDescriptorHandle(void* handle, void* descriptor_handle) {
  try {
    cfd::Initialize();
    if (descriptor_handle != nullptr) {
      CfdCapiOutputDescriptor* descriptor_struct =
          static_cast<CfdCapiOutputDescriptor*>(descriptor_handle);
      if (descriptor_struct->script_list != nullptr) {
        delete descriptor_struct->script_list;
        descriptor_struct->script_list = nullptr;
      }
      if (descriptor_struct->multisig_key_list != nullptr) {
        delete descriptor_struct->multisig_key_list;
        descriptor_struct->multisig_key_list = nullptr;
      }
    }
    FreeBuffer(
        descriptor_handle, kPrefixOutputDescriptor,
        sizeof(CfdCapiOutputDescriptor));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetAddressesFromMultisig(
    void* handle, const char* redeem_script, int network_type, int hash_type,
    void** addr_multisig_keys_handle, uint32_t* max_key_num) {
  try {
    cfd::Initialize();
    if (addr_multisig_keys_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "multisig key handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. multisig key handle is null.");
    }

    bool is_bitcoin = false;
    NetType net_type = cfd::capi::ConvertNetType(network_type, &is_bitcoin);
    AddressType addr_type = ConvertHashToAddressType(hash_type);
    Script redeem_script_obj;
    if ((redeem_script != nullptr) && (*redeem_script != '\0')) {
      redeem_script_obj = Script(redeem_script);
    }

    std::vector<Pubkey> pubkey_list;
    std::vector<Address> addr_list;
    if (is_bitcoin) {
      AddressApi api;
      addr_list = api.GetAddressesFromMultisig(
          net_type, addr_type, redeem_script_obj, &pubkey_list);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ElementsAddressApi e_api;
      addr_list = e_api.GetAddressesFromMultisig(
          net_type, addr_type, redeem_script_obj, &pubkey_list);
#else
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Elements not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    *addr_multisig_keys_handle = AllocBuffer(
        kPrefixMultisigAddresses, sizeof(CfdCapiMultisigAddresses));
    CfdCapiMultisigAddresses* data =
        static_cast<CfdCapiMultisigAddresses*>(*addr_multisig_keys_handle);

    uint32_t index = 0;
    for (const auto& addr : addr_list) {
      addr.GetAddress().copy(
          data->addresses[index], sizeof(data->addresses[index]) - 1);
      ++index;
    }
    index = 0;
    for (const auto& key : pubkey_list) {
      key.GetHex().copy(data->pubkeys[index], kPubkeyHexSize);
      ++index;
    }
    data->list_size = index;
    if (max_key_num != nullptr) *max_key_num = index;

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetAddressFromMultisigKey(
    void* handle, void* addr_multisig_keys_handle, uint32_t index,
    char** address, char** pubkey) {
  char* work_address = nullptr;
  char* work_pubkey = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(addr_multisig_keys_handle, kPrefixMultisigAddresses);

    CfdCapiMultisigAddresses* buffer =
        static_cast<CfdCapiMultisigAddresses*>(addr_multisig_keys_handle);
    if (index >= buffer->list_size) {
      warn(CFD_LOG_SOURCE, "index is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. index is maximum over.");
    }
    if (address != nullptr) {
      work_address = CreateString(std::string(buffer->addresses[index]));
    }
    if (pubkey != nullptr) {
      work_pubkey = CreateString(std::string(buffer->pubkeys[index]));
    }

    if (work_address != nullptr) *address = work_address;
    if (work_pubkey != nullptr) *pubkey = work_pubkey;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(&work_pubkey, &work_address);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(&work_pubkey, &work_address);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(&work_pubkey, &work_address);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdFreeAddressesMultisigHandle(
    void* handle, void* addr_multisig_keys_handle) {
  try {
    cfd::Initialize();
    FreeBuffer(
        addr_multisig_keys_handle, kPrefixMultisigAddresses,
        sizeof(CfdCapiMultisigAddresses));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetAddressFromLockingScript(
    void* handle, const char* locking_script, int network_type,
    char** address) {
  int error_code = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(locking_script)) {
      warn(CFD_LOG_SOURCE, "lockingScript is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. lockingScript is null.");
    }
    if (address == nullptr) {
      warn(CFD_LOG_SOURCE, "address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. address is null.");
    }

    bool is_bitcoin = false;
    NetType net_type = cfd::capi::ConvertNetType(network_type, &is_bitcoin);
    Script script = Script(locking_script);
    Address addr;

    if (is_bitcoin) {
      AddressFactory address_factory(net_type);
      addr = address_factory.GetAddressByLockingScript(script);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ElementsAddressFactory elements_factory(net_type);
      addr = elements_factory.GetAddressByLockingScript(script);
#else
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Elements not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }
    *address = CreateString(addr.GetAddress());
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    error_code = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return error_code;
}

int CfdGetAddressInfo(
    void* handle, const char* address, int* network_type, int* hash_type,
    int* witness_version, char** locking_script, char** hash) {
  int error_code = CfdErrorCode::kCfdUnknownError;
  char* work_locking_script = nullptr;
  char* work_hash = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(address)) {
      warn(CFD_LOG_SOURCE, "address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. address is null.");
    }

    Address addr;
#ifndef CFD_DISABLE_ELEMENTS
    try {
      ElementsAddressFactory factory;
      addr = factory.GetAddress(address);
    } catch (const CfdException& cfd_except) {
      std::string err_msg = cfd_except.what();
      if ((err_msg != "Base58 decode error.") &&
          (err_msg != "Unknown address prefix.")) {
        throw cfd_except;
      }
    }
#endif  // CFD_DISABLE_ELEMENTS

    if (addr.GetAddress().empty()) {
      AddressFactory factory;
      addr = factory.GetAddress(address);
    }

    if (network_type != nullptr) {
      switch (addr.GetNetType()) {
        case NetType::kMainnet:
          *network_type = kCfdNetworkMainnet;
          break;
        case NetType::kTestnet:
          *network_type = kCfdNetworkTestnet;
          break;
        case NetType::kRegtest:
          *network_type = kCfdNetworkRegtest;
          break;
        case NetType::kLiquidV1:
          *network_type = kCfdNetworkLiquidv1;
          break;
        case NetType::kElementsRegtest:
          *network_type = kCfdNetworkElementsRegtest;
          break;
        default:
          *network_type = kCfdNetworkCustomChain;
          break;
      }
    }

    if (hash_type != nullptr) {
      switch (addr.GetAddressType()) {
        case AddressType::kP2shAddress:
          *hash_type = kCfdP2sh;
          break;
        case AddressType::kP2pkhAddress:
          *hash_type = kCfdP2pkh;
          break;
        case AddressType::kP2wshAddress:
          *hash_type = kCfdP2wsh;
          break;
        case AddressType::kP2wpkhAddress:
          *hash_type = kCfdP2wpkh;
          break;
        case AddressType::kP2shP2wshAddress:
          *hash_type = kCfdP2shP2wsh;
          break;
        case AddressType::kP2shP2wpkhAddress:
          *hash_type = kCfdP2shP2wpkh;
          break;
        default:
          warn(
              CFD_LOG_SOURCE, "Illegal hash type.({})", addr.GetAddressType());
          throw CfdException(
              CfdError::kCfdIllegalArgumentError, "Illegal hash type.");
      }
    }

    if (witness_version != nullptr) {
      *witness_version = static_cast<int>(addr.GetWitnessVersion());
    }
    work_locking_script = CreateString(addr.GetLockingScript().GetHex());
    work_hash = CreateString(addr.GetHash().GetHex());

    if (locking_script != nullptr) {
      *locking_script = work_locking_script;
    }
    if (hash != nullptr) {
      *hash = work_hash;
    }
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    error_code = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_locking_script, &work_hash);
  return error_code;
}

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
