// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_common.cpp
 *
 * @brief cfd-capiで利用する共通処理の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include <exception>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#ifndef CFD_DISABLE_JSONAPI
#include "cfd/cfd_json_mapping_api.h"
#endif  // CFD_DISABLE_JSONAPI
#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::core::AddressType;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::NetType;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

// -----------------------------------------------------------------------------
// buffer API
// -----------------------------------------------------------------------------
void* AllocBuffer(const std::string& prefix, uint32_t size) {
  if (prefix.empty() || (size <= sizeof(CfdCapiPrefixTemplate))) {
    warn(CFD_LOG_SOURCE, "parameter error.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "parameter illegal.");
  }
  void* result = ::malloc(size);
  if (result == nullptr) {
    warn(CFD_LOG_SOURCE, "malloc NG. prefix={}", prefix);
    throw CfdException(CfdError::kCfdMemoryFullError, "allocate buffer fail.");
  }
  ::memset(result, 0, size);
  CfdCapiPrefixTemplate* buffer = static_cast<CfdCapiPrefixTemplate*>(result);
  prefix.copy(buffer->prefix, sizeof(buffer->prefix) - 1);
  return result;
}

void FreeBuffer(void* address, const std::string& prefix, uint32_t size) {
  CheckBuffer(address, prefix);

  if (size <= sizeof(CfdCapiPrefixTemplate)) {
    warn(CFD_LOG_SOURCE, "parameter error.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "parameter illegal.");
  }
  ::memset(address, 0, size);
  ::free(address);
}

void CheckBuffer(void* address, const std::string& prefix) {
  if (address == nullptr) {
    warn(CFD_LOG_SOURCE, "pointer is null.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to parameter. pointer is null.");
  }
  if (prefix.empty()) {
    warn(CFD_LOG_SOURCE, "parameter error.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "parameter illegal.");
  }

  CfdCapiPrefixTemplate* buffer = static_cast<CfdCapiPrefixTemplate*>(address);
  size_t len = prefix.length() + 1;
  if (memcmp(buffer->prefix, prefix.c_str(), len) != 0) {
    warn(CFD_LOG_SOURCE, "Illegal buffer data. prefix unmatch.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Illegal buffer data.");
  }
}

NetType ConvertNetType(int network_type, bool* is_bitcoin) {
  NetType net_type = NetType::kCustomChain;
  switch (network_type) {
    case kCfdNetworkMainnet:
    case kCfdNetworkTestnet:
    case kCfdNetworkRegtest:
      net_type = static_cast<NetType>(network_type);
      if (is_bitcoin != nullptr) {
        *is_bitcoin = true;
      }
      break;
    default:
#ifndef CFD_DISABLE_ELEMENTS
      if (network_type == kCfdNetworkLiquidv1) {
        net_type = NetType::kLiquidV1;
      } else if (network_type == kCfdNetworkElementsRegtest) {
        net_type = NetType::kElementsRegtest;
      } else if (network_type == kCfdNetworkCustomChain) {
        // do nothing
      } else {
        warn(CFD_LOG_SOURCE, "Illegal network type.({})", network_type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Illegal network type.");
      }
      if (is_bitcoin != nullptr) {
        *is_bitcoin = false;
      }
      break;
#else
      warn(CFD_LOG_SOURCE, "Illegal network type.({})", network_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Illegal network type.");
#endif  // CFD_DISABLE_ELEMENTS
  }
  return net_type;
}

bool IsElementsNetType(int network_type) {
  bool is_elements = false;
#ifndef CFD_DISABLE_ELEMENTS
  if (network_type == kCfdNetworkLiquidv1) {
    is_elements = true;
  } else if (network_type == kCfdNetworkElementsRegtest) {
    is_elements = true;
  } else if (network_type == kCfdNetworkCustomChain) {
    is_elements = true;
  } else {
    // do nothing
  }
#else
  warn(CFD_LOG_SOURCE, "Illegal network type.({})", network_type);
  throw CfdException(
      CfdError::kCfdIllegalArgumentError, "Illegal network type.");
#endif  // CFD_DISABLE_ELEMENTS
  return is_elements;
}

cfd::core::AddressType ConvertHashToAddressType(int hash_type) {
  AddressType addr_type;
  switch (hash_type) {
    case kCfdP2sh:
      addr_type = AddressType::kP2shAddress;
      break;
    case kCfdP2pkh:
      addr_type = AddressType::kP2pkhAddress;
      break;
    case kCfdP2wsh:
      addr_type = AddressType::kP2wshAddress;
      break;
    case kCfdP2wpkh:
      addr_type = AddressType::kP2wpkhAddress;
      break;
    case kCfdP2shP2wsh:
      addr_type = AddressType::kP2shP2wshAddress;
      break;
    case kCfdP2shP2wpkh:
      addr_type = AddressType::kP2shP2wpkhAddress;
      break;
    default:
      warn(CFD_LOG_SOURCE, "Illegal hash type.({})", hash_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Illegal hash type.");
  }
  return addr_type;
}

cfd::core::AddressType ConvertAddressType(int address_type) {
  AddressType addr_type;
  switch (address_type) {
    case kCfdP2shAddress:
      addr_type = AddressType::kP2shAddress;
      break;
    case kCfdP2pkhAddress:
      addr_type = AddressType::kP2pkhAddress;
      break;
    case kCfdP2wshAddress:
      addr_type = AddressType::kP2wshAddress;
      break;
    case kCfdP2wpkhAddress:
      addr_type = AddressType::kP2wpkhAddress;
      break;
    case kCfdP2shP2wshAddress:
      addr_type = AddressType::kP2shP2wshAddress;
      break;
    case kCfdP2shP2wpkhAddress:
      addr_type = AddressType::kP2shP2wpkhAddress;
      break;
    default:
      warn(CFD_LOG_SOURCE, "Illegal address type.({})", address_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Illegal address type.");
  }
  return addr_type;
}

cfd::core::WitnessVersion GetWitnessVersion(int hash_type) {
  cfd::core::WitnessVersion version;
  switch (hash_type) {
    case kCfdP2sh:
    case kCfdP2pkh:
      version = cfd::core::WitnessVersion::kVersionNone;
      break;
    case kCfdP2wsh:
    case kCfdP2wpkh:
    case kCfdP2shP2wsh:
    case kCfdP2shP2wpkh:
      version = cfd::core::WitnessVersion::kVersion0;
      break;
    default:
      warn(CFD_LOG_SOURCE, "Illegal hash type.({})", hash_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Illegal hash type.");
  }
  return version;
}

bool IsEmptyString(const char* message) {
  if ((message == nullptr) || (*message == '\0')) {
    return true;
  } else {
    return false;
  }
}

char* CreateString(const std::string& message) {
  size_t len = message.length();
  char* addr = static_cast<char*>(::malloc(len + 1));
  if (addr == nullptr) {
    warn(CFD_LOG_SOURCE, "malloc NG.");
    throw CfdException(CfdError::kCfdMemoryFullError, "allocate buffer fail.");
  }
  ::memset(addr, 0, len + 1);
  message.copy(addr, len);
  return addr;
}

void FreeBufferOnError(
    char** pointer1, char** pointer2, char** pointer3, char** pointer4,
    char** pointer5, char** pointer6) {
  char** pointer_list[] = {pointer1, pointer2, pointer3,
                           pointer4, pointer5, pointer6};
  for (size_t idx = 0; idx < 6; ++idx) {
    if (pointer_list[idx] != nullptr) {
      if (*(pointer_list[idx]) != nullptr) {
        ::free(*(pointer_list[idx]));
        *(pointer_list[idx]) = nullptr;
      }
    }
  }
}

// -----------------------------------------------------------------------------
// CfdCapiManager
// -----------------------------------------------------------------------------
/// cfd-capiインスタンス
static CfdCapiManager capi_instance;

CfdCapiManager::CfdCapiManager() : handle_list_(), mutex_() {
  // do nothing
}

void CfdCapiManager::FreeAllList(std::vector<CfdCapiHandleData*>* list) {
  if ((list != nullptr) && (!list->empty())) {
    for (CfdCapiHandleData* handle : *list) {
      ::free(handle);
    }
    list->clear();
  }
}

void* CfdCapiManager::CreateHandle(bool is_outside) {
  CfdCapiHandleData* handle;
  if (is_outside) {
    handle = static_cast<CfdCapiHandleData*>(
        AllocBuffer(kPrefixHandleData, sizeof(CfdCapiHandleData)));
    handle->is_outside = true;
    return handle;
  } else {
    // 排他制御開始
    std::lock_guard<std::mutex> lock(mutex_);
    handle = static_cast<CfdCapiHandleData*>(
        AllocBuffer(kPrefixHandleData, sizeof(CfdCapiHandleData)));
    handle_list_.push_back(handle);
    return handle;
  }
}

void CfdCapiManager::FreeHandle(void* handle) {
  if (handle != nullptr) {
    CfdCapiHandleData* handle_data = static_cast<CfdCapiHandleData*>(handle);
    size_t len = strlen(kPrefixHandleData) + 1;
    if (memcmp(handle_data->prefix, kPrefixHandleData, len) != 0) {
      warn(CFD_LOG_SOURCE, "Illegal handle data. prefix unmatch.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Illegal handle data.");
    }

    if (handle_data->is_outside) {  // outside handle
      memset(handle, 0, sizeof(CfdCapiHandleData));
      ::free(handle);
    } else {
      // 排他制御開始
      std::lock_guard<std::mutex> lock(mutex_);

      std::vector<CfdCapiHandleData*>::iterator ite;
      for (ite = handle_list_.begin(); ite != handle_list_.end(); ++ite) {
        if (handle == *ite) {
          info(CFD_LOG_SOURCE, "capi FreeHandle. addr={:p}.", handle);
          memset(handle, 0, sizeof(CfdCapiHandleData));
          ::free(handle);
          handle_list_.erase(ite);
          break;
        }
      }
    }
  }
}

void CfdCapiManager::CopyHandle(
    void* source, void* destination, bool error_state_only) const {
  if ((source != nullptr) && (destination != nullptr)) {
    CfdCapiHandleData* source_data = static_cast<CfdCapiHandleData*>(source);
    CfdCapiHandleData* dest_data =
        static_cast<CfdCapiHandleData*>(destination);
    size_t len = strlen(kPrefixHandleData) + 1;
    if ((memcmp(source_data->prefix, kPrefixHandleData, len) != 0) ||
        (memcmp(dest_data->prefix, kPrefixHandleData, len) != 0)) {
      warn(CFD_LOG_SOURCE, "Illegal handle data. prefix unmatch.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Illegal handle data.");
    }

    if (error_state_only) {
      dest_data->error_code = source_data->error_code;
      memcpy(
          dest_data->error_message, source_data->error_message,
          sizeof(dest_data->error_message));
    } else {
      // TODO(k-matsuzawa): 現在はコピーする情報が無い
      // bool is_outside = dest_data->is_outside;
      // memcpy(dest_data, source_data, sizeof(CfdCapiHandleData));
      // dest_data->is_outside = is_outside;
    }
  }
}

void CfdCapiManager::SetLastError(
    void* handle, int error_code, const char* message) {
  // TODO(k-matsuzawa): handle存在チェックすべきかどうか
  if (handle != nullptr) {
    CfdCapiHandleData* data = static_cast<CfdCapiHandleData*>(handle);
    memset(data->error_message, 0, sizeof(data->error_message));
    if (message != nullptr) {
      std::string err_str(
          message, strnlen(message, sizeof(data->error_message)));
      err_str.copy(data->error_message, sizeof(data->error_message) - 1);
      data->error_message[sizeof(data->error_message) - 1] = '\0';
    }
    data->error_code = error_code;
  }
}

void CfdCapiManager::SetLastFatalError(void* handle, const char* message) {
  SetLastError(handle, kCfdUnknownError, message);
}

CfdException CfdCapiManager::GetLastError(void* handle) {
  // TODO(k-matsuzawa): handle存在チェックすべきかどうか
  if (handle != nullptr) {
    CfdCapiHandleData* data = static_cast<CfdCapiHandleData*>(handle);
    char str_buffer[256];
    std::string err_str(data->error_message, sizeof(data->error_message));
    err_str.copy(str_buffer, sizeof(str_buffer) - 1);
    str_buffer[sizeof(str_buffer) - 1] = '\0';
    return CfdException(
        static_cast<CfdError>(data->error_code), std::string(str_buffer));
  }
  return CfdException(CfdError::kCfdSuccess);
}

// -----------------------------------------------------------------------------
// namespace api
// -----------------------------------------------------------------------------
int SetLastError(void* handle, int error_code, const char* message) {
  try {
    cfd::capi::capi_instance.SetLastError(handle, error_code, message);
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

int SetLastFatalError(void* handle, const char* message) {
  try {
    cfd::capi::capi_instance.SetLastFatalError(handle, message);
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

int SetLastError(void* handle, const CfdException& exception) {
  SetLastError(handle, exception.GetErrorCode(), exception.what());
  return exception.GetErrorCode();
}

void SetLastFatalError(void* handle, const std::exception& exception) {
  SetLastFatalError(handle, exception.what());
}

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
extern "C" int CfdGetSupportedFunction(uint64_t* support_flag) {
  try {
    cfd::Initialize();
    if (support_flag == nullptr) return kCfdIllegalArgumentError;
    *support_flag = cfd::GetSupportedFunction();
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdInitialize() {
  try {
    cfd::Initialize();
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdFinalize(bool is_finish_process) {
  try {
    cfd::Finalize(is_finish_process);
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdCreateHandle(void** handle) {
  try {
    if (handle == nullptr) return kCfdIllegalArgumentError;
    cfd::Initialize();
    *handle = cfd::capi::capi_instance.CreateHandle();
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdCreateSimpleHandle(void** handle) {
  try {
    if (handle == nullptr) return kCfdIllegalArgumentError;
    cfd::Initialize();
    *handle = cfd::capi::capi_instance.CreateHandle(true);
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdCloneHandle(void* source, void** handle) {
  int result = kCfdUnknownError;
  void* temp_handle = nullptr;
  try {
    if (handle == nullptr) return kCfdIllegalArgumentError;
    cfd::Initialize();
    temp_handle = cfd::capi::capi_instance.CreateHandle(true);

    if (source != nullptr) {
      cfd::capi::capi_instance.CopyHandle(source, temp_handle);
    }
    *handle = temp_handle;
    return kCfdSuccess;
  } catch (const CfdException& except) {
    result = except.GetErrorCode();
  } catch (...) {
    // do nothing
  }
  CfdFreeHandle(temp_handle);
  return result;
}

extern "C" int CfdCopyErrorState(void* source, void* destination) {
  try {
    cfd::Initialize();
    cfd::capi::capi_instance.CopyHandle(source, destination, true);
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdFreeHandle(void* handle) {
  try {
    cfd::Initialize();
    cfd::capi::capi_instance.FreeHandle(handle);
    return kCfdSuccess;
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdFreeBuffer(void* address) {
  try {
    cfd::Initialize();
    if (address != nullptr) ::free(address);
    return kCfdSuccess;
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdFreeStringBuffer(char* address) {
  try {
    cfd::Initialize();
    if (address != nullptr) ::free(address);
    return kCfdSuccess;
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdGetLastErrorCode(void* handle) {
  try {
    CfdException last_error = cfd::capi::capi_instance.GetLastError(handle);
    return last_error.GetErrorCode();
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdGetLastErrorMessage(void* handle, char** message) {
  try {
    if (message == nullptr) return kCfdIllegalArgumentError;
    CfdException last_error = cfd::capi::capi_instance.GetLastError(handle);
    std::string err_str(last_error.what());
    size_t len = err_str.length() + 1;
    char* buffer = static_cast<char*>(malloc(len));
    if (buffer == nullptr) return kCfdIllegalStateError;
    err_str.copy(buffer, len);
    buffer[len - 1] = '\0';
    *message = buffer;
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return except.GetErrorCode();
  } catch (...) {
    return kCfdUnknownError;
  }
}

extern "C" int CfdRequestExecuteJson(
    void* handle, const char* name, const char* json_string,
    char** response_json_string) {
#ifndef CFD_DISABLE_JSONAPI
  using cfd::api::json::JsonMappingApi;
  try {
    if ((name == nullptr) || (json_string == nullptr) ||
        (response_json_string == nullptr)) {
      return kCfdIllegalArgumentError;
    }

    std::string result;
    std::string command(name);
    if (command == "ElementsDecodeRawTransaction") {
#ifndef CFD_DISABLE_ELEMENTS
      result = JsonMappingApi::ElementsDecodeRawTransaction(
          std::string(json_string));
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "elements not support.");
#endif  // CFD_DISABLE_ELEMENTS
    } else if (command == "DecodeRawTransaction") {
      result = JsonMappingApi::DecodeRawTransaction(std::string(json_string));
    } else {
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "unknown request name.");
    }

    if (!result.empty() && (response_json_string != nullptr)) {
      *response_json_string = cfd::capi::CreateString(result);
    }
    return kCfdSuccess;
  } catch (const CfdException& except) {
    return cfd::capi::SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    cfd::capi::SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    cfd::capi::SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
#else
  return kCfdIllegalStateError;
#endif  // CFD_DISABLE_JSONAPI
}

int CfdSerializeByteData(void* handle, const char* buffer, char** output) {
  try {
    cfd::Initialize();
    if (buffer == nullptr) {
      warn(CFD_LOG_SOURCE, "buffer is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. buffer is null.");
    }
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }

    ByteData data(buffer);
    *output = cfd::capi::CreateString(data.Serialize().GetHex());

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return cfd::capi::SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    cfd::capi::SetLastFatalError(handle, std_except.what());
  } catch (...) {
    cfd::capi::SetLastFatalError(handle, "unknown error.");
  }
  return CfdErrorCode::kCfdUnknownError;
}

#endif  // CFD_DISABLE_CAPI
