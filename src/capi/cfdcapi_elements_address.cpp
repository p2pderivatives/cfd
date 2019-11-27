// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_address.cpp
 *
 * @brief cfd-capiで利用するAddress操作の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#ifndef CFD_DISABLE_ELEMENTS
#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_elements_address.h"
#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::ElementsAddressFactory;
using cfd::api::AddressApi;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
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
      warn(CFD_LOG_SOURCE, "address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. address is null.");
    }
    if (IsEmptyString(confidential_key)) {
      warn(CFD_LOG_SOURCE, "confidential key is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. confidential key is null.");
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
  try {
    cfd::Initialize();
    if (IsEmptyString(confidential_address)) {
      warn(CFD_LOG_SOURCE, "confidential address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. confidential address is null.");
    }

    ElementsAddressFactory factory;
    ElementsConfidentialAddress confidential_addr;
    confidential_addr = factory.GetConfidentialAddress(confidential_address);
    Address addr = confidential_addr.GetUnblindedAddress();
    if (network_type != nullptr) {
      // convert to c_network_type
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
    if (address != nullptr) {
      *address = CreateString(addr.GetAddress());
    }
    if (confidential_key != nullptr) {
      *confidential_key =
          CreateString(confidential_addr.GetConfidentialKey().GetHex());
    }

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(address, confidential_key);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(address, confidential_key);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(address, confidential_key);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

};  // extern "C"

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_DISABLE_CAPI
