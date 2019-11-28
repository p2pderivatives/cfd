// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_key.cpp
 *
 * @brief cfd-capiで利用するKey操作の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
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
#include "cfdcore/cfdcore_transaction_common.h"

using cfd::api::AddressApi;
using cfd::api::DescriptorKeyData;
using cfd::api::DescriptorScriptData;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorScriptType;
using cfd::core::ExtPrivkey;
using cfd::core::ExtPubkey;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SignatureUtil;

using cfd::core::logger::info;
using cfd::core::logger::warn;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::api::ElementsAddressApi;
#endif  // CFD_DISABLE_ELEMENTS

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CheckBuffer;
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

int CfdCalculateEcSignature(
    void* handle, const char* sighash, const char* privkey, const char* wif,
    int network_type, bool has_grind_r, char** signature) {
  try {
    cfd::Initialize();
    if (IsEmptyString(sighash)) {
      warn(CFD_LOG_SOURCE, "sighash is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sighash is null.");
    }
    if (IsEmptyString(privkey) && IsEmptyString(wif)) {
      warn(CFD_LOG_SOURCE, "privkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null.");
    }
    if (signature == nullptr) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    Privkey private_key;
    if (!IsEmptyString(wif)) {
      private_key = Privkey::FromWif(wif, net_type);
    } else {
      private_key = Privkey(privkey);
    }
    ByteData data = SignatureUtil::CalculateEcSignature(
        ByteData256(sighash), private_key, has_grind_r);
    *signature = CreateString(data.GetHex());

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

int CfdCreateKeyPair(
    void* handle, bool is_compressed, int network_type, char** pubkey,
    char** privkey, char** wif) {
  return -1;
}

int CfdGetPrivkeyFromWif(
    void* handle, const char* wif, int network_type, char** privkey) {
  return -1;
}

int CfdGetPubkeyFromPrivkey(
    void* handle, const char* privkey, const char* wif, bool is_compressed,
    char** pubkey) {
  return -1;
}

int CfdCreateExtkeyFromSeed(
    void* handle, const char* seed_hex, int network_type, int key_type,
    char** extkey) {
  return -1;
}

int CfdCreateExtkeyFromParentPath(
    void* handle, const char* extkey, const char* path, int network_type,
    int key_type, char** child_extkey) {
  return -1;
}

int CfdCreateExtPubkey(
    void* handle, const char* extkey, int network_type, char** ext_pubkey) {
  return -1;
}

int CfdGetPrivkeyFromExtkey(
    void* handle, const char* extkey, int network_type, char** privkey,
    char** wif) {
  return -1;
}

int CfdGetPubkeyFromExtkey(
    void* handle, const char* extkey, int network_type, char** pubkey) {
  return -1;
}

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
