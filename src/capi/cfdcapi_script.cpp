// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_script.h
 *
 * @brief cfd-capiで利用するTransaction作成のAPI定義
 *
 * C言語のAPIを提供する.
 */
#ifndef CFD_DISABLE_CAPI
#include <exception>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_script.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Script;
using cfd::core::ScriptElement;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: ScriptItem
constexpr const char* const kPrefixScriptItem = "ScriptItem";

/**
 * @brief cfd-capi Script Itemハンドル情報構造体.
 * @details script itemは数や内容が可変のため、配列にはしない
 */
struct CfdCapiScriptItemHandleData {
  char prefix[kPrefixLength];  //!< buffer prefix
  std::vector<std::string>* script_items;  //! elements
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
using cfd::capi::AllocBuffer;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;
using cfd::capi::CfdCapiScriptItemHandleData;
using cfd::capi::kPrefixScriptItem;

// API
extern "C" {

/**
 * @brief parse script to script item array.
 * @param[in] handle            cfd handle.
 * @param[in] script            script string.
 * @param[out] script_handle    script item hex string array.
 * @param[out] script_item_num  num of items contains script.
 * @return CfdErrorCode
 */
CFDC_API int CfdParseScript(
    void* handle, const char* script, void** script_item_handle,
    int* script_item_num) {
  CfdCapiScriptItemHandleData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (script == nullptr) {
      warn(CFD_LOG_SOURCE, "script is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parse script. script is null.");
    }
    if (script_item_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "script item handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parse script. script item handle is null.");
    }

    Script script_obj;
    if (*script != '\0') {
      script_obj = Script(script);
    }
    std::vector<ScriptElement> script_elems = script_obj.GetElementList();

    buffer = static_cast<CfdCapiScriptItemHandleData*>(
        AllocBuffer(kPrefixScriptItem, sizeof(CfdCapiScriptItemHandleData)));
    buffer->script_items = new std::vector<std::string>;
    buffer->script_items->reserve(script_elems.size());
    for(const auto& elem : script_elems) {
      buffer->script_items->push_back(elem.GetData().GetHex());
    }
    *script_item_handle = buffer;
    *script_item_num = static_cast<int>(buffer->script_items->size());

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

int CfdGetScriptItem(
    void* handle, void* script_item_handle, int index, char** script_item) {
  try {
    cfd::Initialize();
    CheckBuffer(script_item_handle, kPrefixScriptItem);

    CfdCapiScriptItemHandleData* buffer =
        static_cast<CfdCapiScriptItemHandleData*>(script_item_handle);
    int item_num = static_cast<int>(buffer->script_items->size());
    if ((buffer->script_items == nullptr) || (index >= item_num)) {
      warn(CFD_LOG_SOURCE, "index is out of range.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parse script item. index is out of range.");
    }

    const std::string& item_str = buffer->script_items->at(index);
    if (script_item != nullptr) {
      *script_item = CreateString(item_str);
    }
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

int CfdFreeScriptItemHandle(void* handle, void* script_item_handle) {
  try {
    cfd::Initialize();
    if (script_item_handle != nullptr) {
      CfdCapiScriptItemHandleData* item_struct =
          static_cast<CfdCapiScriptItemHandleData*>(script_item_handle);
      if (item_struct->script_items != nullptr) {
        delete item_struct->script_items;
        item_struct->script_items = nullptr;
      }
    }
    FreeBuffer(
        script_item_handle, kPrefixScriptItem,
        sizeof(CfdCapiScriptItemHandleData));
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

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
