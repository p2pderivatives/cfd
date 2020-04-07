// Copyright 2020 CryptoGarage
/**
 * @file cfdcapi_ledger.cpp
 *
 * @brief implements ledger support function on cfd-capi.
 */
#ifndef CFD_DISABLE_CAPI
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfdapi_ledger.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_ledger.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

using cfd::api::LedgerApi;
using cfd::api::LedgerMetaDataStackItem;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::HashUtil;
using cfd::core::NetType;
using cfd::core::StringUtil;

using cfd::core::logger::info;
using cfd::core::logger::warn;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::ConfidentialTransactionContext;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOutReference;
#endif  // CFD_DISABLE_ELEMENTS

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: Ledger MetaData
constexpr const char* const kPrefixLedgerMetaData = "LedgerMetaData";

/**
 * @brief cfd-capi Ledger MetaDataハンドル情報構造体.
 * @details stack dataは数や内容が可変のため、配列にはしない
 */
struct CfdCapiLedgerMetaData {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! MetaData list
  std::vector<LedgerMetaDataStackItem>* metadata_stack;
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiLedgerMetaData;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kPrefixLedgerMetaData;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

extern "C" {

int CfdInitializeTxSerializeForLedger(void* handle, void** serialize_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiLedgerMetaData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (serialize_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "serialize handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. serialize handle is null.");
    }

    buffer = static_cast<CfdCapiLedgerMetaData*>(
        AllocBuffer(kPrefixLedgerMetaData, sizeof(CfdCapiLedgerMetaData)));
    buffer->metadata_stack = new std::vector<LedgerMetaDataStackItem>();

    *serialize_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    if (buffer != nullptr) CfdFreeTxSerializeForLedger(handle, buffer);
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    if (buffer != nullptr) CfdFreeTxSerializeForLedger(handle, buffer);
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    if (buffer != nullptr) CfdFreeTxSerializeForLedger(handle, buffer);
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdAddTxOutMetaDataForLedger(
    void* handle, void* serialize_handle, uint32_t index,
    const char* metadata1, const char* metadata2, const char* metadata3) {
  static constexpr uint32_t kAssetHexSize = 33 * 2;
  static constexpr uint32_t kValueHexSize = 33 * 2;
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(serialize_handle, kPrefixLedgerMetaData);

    CfdCapiLedgerMetaData* buffer =
        static_cast<CfdCapiLedgerMetaData*>(serialize_handle);
    if (buffer->metadata_stack == nullptr) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }

    LedgerMetaDataStackItem item;

    static auto check_metadata_function =
        [](const char* metadata, uint32_t max_length,
           const std::string error_field_name) -> std::string {
      char str_buf[80];
      memset(str_buf, 0, sizeof(str_buf));
      size_t len;
      if (IsEmptyString(metadata)) {
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to metadata. " + error_field_name + " is empty.");
      }
      len = strnlen(metadata, sizeof(str_buf));
      if (len > max_length) {
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to metadata. " + error_field_name + " is length over.");
      }
      memcpy(str_buf, metadata, len);
      return std::string(str_buf);
    };

    item.metadata1 =
        check_metadata_function(metadata1, kAssetHexSize, "asset metadata");
    item.metadata2 =
        check_metadata_function(metadata2, kValueHexSize, "value metadata");
    if (!IsEmptyString(metadata3)) {
      // unused value
      // item.metadata3 = check_metadata_function(metadata3, 0);
    }

    if (index == buffer->metadata_stack->size()) {
      buffer->metadata_stack->push_back(item);
    } else {
      if (index >= buffer->metadata_stack->size()) {
        buffer->metadata_stack->resize(index + 1);
      }
      LedgerMetaDataStackItem& buf_item = buffer->metadata_stack->at(index);
      buf_item = item;
    }
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdFinalizeTxSerializeForLedger(
    void* handle, void* serialize_handle, int net_type,
    const char* tx_hex_string, bool skip_witness, bool is_authorization,
    char** serialize_hex) {
  return CfdFinalizeTxSerializeHashForLedger(
      handle, serialize_handle, net_type, tx_hex_string, skip_witness,
      is_authorization, true, serialize_hex);
}

int CfdFinalizeTxSerializeHashForLedger(
    void* handle, void* serialize_handle, int net_type,
    const char* tx_hex_string, bool skip_witness, bool is_authorization,
    bool is_sha256, char** serialize_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(serialize_handle, kPrefixLedgerMetaData);

    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (serialize_hex == nullptr) {
      warn(CFD_LOG_SOURCE, "serialize output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. serialize output is null.");
    }

    CfdCapiLedgerMetaData* buffer =
        static_cast<CfdCapiLedgerMetaData*>(serialize_handle);
    if (buffer->metadata_stack == nullptr) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }

    ByteData serialize_data;
    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Bitcoin serialize not implement.");
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      LedgerApi api;
      serialize_data = api.Serialize(
          tx, *(buffer->metadata_stack), skip_witness, is_authorization);
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }
    if (is_sha256) {
      ByteData256 hashed_data = HashUtil::Sha256(serialize_data);
      serialize_data = hashed_data.GetData();
    }
    *serialize_hex = CreateString(serialize_data.GetHex());

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdFreeTxSerializeForLedger(void* handle, void* serialize_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (serialize_handle != nullptr) {
      CfdCapiLedgerMetaData* metadata_struct =
          static_cast<CfdCapiLedgerMetaData*>(serialize_handle);
      if (metadata_struct->metadata_stack != nullptr) {
        delete metadata_struct->metadata_stack;
        metadata_struct->metadata_stack = nullptr;
      }
    }
    FreeBuffer(
        serialize_handle, kPrefixLedgerMetaData,
        sizeof(CfdCapiLedgerMetaData));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
