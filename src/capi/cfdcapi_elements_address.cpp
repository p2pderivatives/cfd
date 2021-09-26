// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_address.cpp
 *
 * @brief cfd-capiで利用するElements Address操作の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#ifndef CFD_DISABLE_ELEMENTS
#include "cfdc/cfdcapi_elements_address.h"

#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfdapi_elements_address.h"
#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_script.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::ElementsAddressFactory;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ContractHashUtil;
using cfd::core::Descriptor;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertAddressType;
using cfd::capi::ConvertFromCfdNetType;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

extern "C" {

int CfdCreateConfidentialAddress(
    void* handle, const char* address, const char* confidential_key,
    char** confidential_address) {
  try {
    cfd::Initialize();
    if (confidential_address == nullptr) {
      warn(CFD_LOG_SOURCE, "confidential address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. confidential address is null.");
    }
    if (IsEmptyString(address)) {
      warn(CFD_LOG_SOURCE, "address is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. address is null or empty.");
    }
    if (IsEmptyString(confidential_key)) {
      warn(CFD_LOG_SOURCE, "confidential key is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. confidential key is null or empty.");
    }

    ElementsAddressFactory factory;
    Address addr = factory.GetAddress(address);
    Pubkey key = Pubkey(confidential_key);

    ElementsConfidentialAddress confidential_addr;
    confidential_addr = factory.GetConfidentialAddress(addr, key);
    *confidential_address = CreateString(confidential_addr.GetAddress());

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

int CfdParseConfidentialAddress(
    void* handle, const char* confidential_address, char** address,
    char** confidential_key, int* network_type) {
  char* work_address = nullptr;
  char* work_confidential_key = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(confidential_address)) {
      warn(CFD_LOG_SOURCE, "confidential address is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. confidential address is null or empty.");
    }

    ElementsAddressFactory factory;
    ElementsConfidentialAddress confidential_addr;
    confidential_addr = factory.GetConfidentialAddress(confidential_address);
    Address addr = confidential_addr.GetUnblindedAddress();
    if (network_type != nullptr) {
      // convert to c_network_type
      *network_type = ConvertFromCfdNetType(addr.GetNetType());
    }
    if (address != nullptr) {
      work_address = CreateString(addr.GetAddress());
    }
    if (confidential_key != nullptr) {
      work_confidential_key =
          CreateString(confidential_addr.GetConfidentialKey().GetHex());
    }

    if (work_address != nullptr) *address = work_address;
    if (work_confidential_key != nullptr)
      *confidential_key = work_confidential_key;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(&work_address, &work_confidential_key);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(&work_address, &work_confidential_key);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(&work_address, &work_confidential_key);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetPeginAddress(
    void* handle, int mainchain_network_type, const char* fedpeg_script,
    int hash_type, const char* pubkey, const char* redeem_script,
    char** pegin_address, char** claim_script, char** tweaked_fedpeg_script) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_address = nullptr;
  char* work_script = nullptr;
  char* work_fedpeg_script = nullptr;
  try {
    cfd::Initialize();
    if (pegin_address == nullptr) {
      warn(CFD_LOG_SOURCE, "pegin_address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pegin_address is null.");
    }
    if ((pubkey == nullptr) && (redeem_script == nullptr)) {
      warn(CFD_LOG_SOURCE, "pubkey and script is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey and script is null.");
    }
    if (claim_script == nullptr) {
      warn(CFD_LOG_SOURCE, "claim_script is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. claim_script is null.");
    }
    if (IsEmptyString(fedpeg_script)) {
      warn(CFD_LOG_SOURCE, "fedpeg_script is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. fedpeg_script is null or empty.");
    }

    bool is_bitcoin = false;
    auto net_type = ConvertNetType(mainchain_network_type, &is_bitcoin);
    if (!is_bitcoin) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid network type. need mainchain network type.");
    }
    auto addr_type = ConvertHashToAddressType(hash_type);
    Script claim_script_obj;
    if ((pubkey != nullptr) && Pubkey::IsValid(ByteData(pubkey))) {
      claim_script_obj = ScriptUtil::CreateP2wpkhLockingScript(Pubkey(pubkey));
    } else {
      claim_script_obj =
          ScriptUtil::CreateP2wshLockingScript(Script(redeem_script));
    }

    Script tweak_fedpegscript = ContractHashUtil::GetContractScript(
        claim_script_obj, Script(fedpeg_script));
    auto addr = ElementsAddressFactory::CreatePegInAddress(
        net_type, addr_type, tweak_fedpegscript);

    work_address = CreateString(addr.GetAddress());
    work_script = CreateString(claim_script_obj.GetHex());
    if (tweaked_fedpeg_script != nullptr) {
      work_fedpeg_script = CreateString(tweak_fedpegscript.GetHex());
    }

    *pegin_address = work_address;
    *claim_script = work_script;
    if (tweaked_fedpeg_script != nullptr) {
      *tweaked_fedpeg_script = work_fedpeg_script;
    }
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_address, &work_script, &work_fedpeg_script);
  return result;
}

int CfdGetPegoutAddress(
    void* handle, int mainchain_network, int elements_network,
    const char* descriptor, uint32_t bip32_counter, int address_type,
    char** mainchain_address, char** base_descriptor) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_address = nullptr;
  char* work_descriptor = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(descriptor)) {
      warn(CFD_LOG_SOURCE, "descriptor is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. descriptor is null or empty.");
    }

    auto mainchain_net_type = ConvertNetType(mainchain_network, nullptr);
    auto elements_net_type = ConvertNetType(elements_network, nullptr);
    auto addr_type = ConvertAddressType(address_type);
    Descriptor desc;
    auto addr = ElementsAddressFactory::CreatePegOutAddress(
        mainchain_net_type, elements_net_type, descriptor, bip32_counter,
        addr_type, &desc);
    if (mainchain_address != nullptr) {
      work_address = CreateString(addr.GetAddress());
    }
    if (base_descriptor != nullptr) {
      work_descriptor = CreateString(desc.ToString(false));
    }

    if (base_descriptor != nullptr) *base_descriptor = work_descriptor;
    if (mainchain_address != nullptr) *mainchain_address = work_address;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_address, &work_descriptor);
  return result;
}

};  // extern "C"

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_DISABLE_CAPI
