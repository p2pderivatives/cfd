// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_common.cpp
 *
 * @brief cfd-capiで利用する共通処理の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include <string>
#include <vector>

#include "capi/cfdcapi_internal.h"
#include "cfd/cfd_common.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::core::CfdError;
using cfd::core::CfdException;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

/// cfd-capiインスタンス
static CfdCapiManager capi_instance;

// -----------------------------------------------------------------------------
// CfdCapiManager
// -----------------------------------------------------------------------------
CfdCapiManager::CfdCapiManager() : handle_list_(), mutex_() {
  // do nothing
}

CfdCapiManager::~CfdCapiManager() {
  if (!handle_list_.empty()) {
    for (CfdCapiHandleData* handle : handle_list_) {
      ::free(handle);
    }
  }
}

void* CfdCapiManager::CreateHandle(void) {
  // 排他制御開始
  std::lock_guard<std::mutex> lock(mutex_);
  CfdCapiHandleData* handle =
      static_cast<CfdCapiHandleData*>(::malloc(sizeof(CfdCapiHandleData)));
  if (handle == nullptr) {
    throw CfdException(
        CfdError::kCfdIllegalStateError, "CfdCreateHandle fail.");
  }
  ::memset(handle, 0, sizeof(CfdCapiHandleData));
  handle_list_.push_back(handle);
  return handle;
}

void CfdCapiManager::FreeHandle(void* handle) {
  if (handle != nullptr) {
    // 排他制御開始
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<CfdCapiHandleData*>::iterator ite;
    for (ite = handle_list_.begin(); ite != handle_list_.end(); ++ite) {
      if (handle == *ite) {
        info(CFD_LOG_SOURCE, "capi FreeHandle. addr={:p}.", handle);
        ::free(handle);
        handle_list_.erase(ite);
        break;
      }
    }
  }
}

void CfdCapiManager::SetLastError(
    void* handle, int error_code, const char* message) {
  // TODO(k-matsuzawa): handle存在チェックすべきかどうか
  if (handle != nullptr) {
    CfdCapiHandleData* data = static_cast<CfdCapiHandleData*>(handle);
    if (message != nullptr) {
      std::string err_str(message, sizeof(data->error_message));
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

extern "C" int CfdFreeHandle(void* handle) {
  try {
    cfd::Initialize();
    if (handle != nullptr) free(handle);
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

#endif  // CFD_DISABLE_CAPI
