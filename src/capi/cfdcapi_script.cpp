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
#include <sstream>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfdapi_transaction.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_script.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_util.h"

using cfd::SignParameter;
using cfd::api::TransactionApi;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptElement;
using cfd::core::ScriptElementType;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: ScriptItem
constexpr const char* const kPrefixScriptItem = "ScriptItem";
//! prefix: MultisigScriptSig
constexpr const char* const kPrefiMultisigScriptSig = "MultiScriptSig";
//! delimiter of script asm
constexpr const char kScriptAsmDelimiter = ' ';
//! MultisigScriptSig struct
using CfdCapiMultisigScriptSigData = cfd::capi::CfdCapiMultisigSignData;

/**
 * @brief cfd-capi Script Itemハンドル情報構造体.
 * @details script itemは数や内容が可変のため、配列にはしない
 */
struct CfdCapiScriptItemHandleData {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! script item hex list
  std::vector<std::string>* script_items;
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiMultisigScriptSigData;
using cfd::capi::CfdCapiScriptItemHandleData;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPrefiMultisigScriptSig;
using cfd::capi::kPrefixScriptItem;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::kScriptAsmDelimiter;
using cfd::capi::kSignatureHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

// API
extern "C" {

CFDC_API int CfdParseScript(
    void* handle, const char* script, void** script_item_handle,
    uint32_t* script_item_num) {
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
    for (const auto& elem : script_elems) {
      std::string data;
      if (elem.IsOpCode()) {
        // Convert to OpCode string
        data = elem.GetOpCode().ToCodeString();
      } else {
        data = elem.ToString();
      }
      buffer->script_items->push_back(data);
    }
    *script_item_handle = buffer;
    *script_item_num = static_cast<uint32_t>(buffer->script_items->size());

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    if (buffer != nullptr) CfdFreeScriptItemHandle(handle, buffer);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    if (buffer != nullptr) CfdFreeScriptItemHandle(handle, buffer);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    if (buffer != nullptr) CfdFreeScriptItemHandle(handle, buffer);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetScriptItem(
    void* handle, void* script_item_handle, uint32_t index,
    char** script_item) {
  try {
    cfd::Initialize();
    CheckBuffer(script_item_handle, kPrefixScriptItem);
    if (script_item == nullptr) {
      warn(CFD_LOG_SOURCE, "script_item is null.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to get script item. script_item is null.");
    }

    CfdCapiScriptItemHandleData* buffer =
        static_cast<CfdCapiScriptItemHandleData*>(script_item_handle);
    uint32_t item_num = static_cast<uint32_t>(buffer->script_items->size());
    if ((buffer->script_items == nullptr) || (index >= item_num)) {
      warn(CFD_LOG_SOURCE, "index is out of range.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to get script item. index is out of range.");
    }

    const std::string& item_str = buffer->script_items->at(index);
    *script_item = CreateString(item_str);
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

int CfdConvertScriptAsmToHex(
    void* handle, const char* script_asm, char** script_hex) {
  try {
    cfd::Initialize();
    if (script_hex == nullptr) {
      warn(CFD_LOG_SOURCE, "script hex is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to convert script. script hex is null.");
    }
    ScriptBuilder sb = ScriptBuilder();

    std::istringstream istream(script_asm);
    std::string script_item;
    while (std::getline(istream, script_item, kScriptAsmDelimiter)) {
      sb.AppendString(script_item);
    }
    Script script = sb.Build();

    *script_hex = CreateString(script.GetHex());
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

int CfdInitializeMultisigScriptSig(void* handle, void** multisig_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (multisig_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "multisig sign handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. multisig sign handle is null.");
    }

    *multisig_handle = AllocBuffer(
        kPrefiMultisigScriptSig, sizeof(CfdCapiMultisigScriptSigData));
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

int CfdAddMultisigScriptSigData(
    void* handle, void* multisig_handle, const char* signature,
    const char* related_pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(multisig_handle, kPrefiMultisigScriptSig);
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }

    CfdCapiMultisigScriptSigData* data =
        static_cast<CfdCapiMultisigScriptSigData*>(multisig_handle);
    if (data->current_index >= kMultisigMaxKeyNum) {
      warn(
          CFD_LOG_SOURCE,
          "The number of signature has reached the upper limit.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "The number of signature has reached the upper limit.");
    }

    ByteData signature_bytes = ByteData(std::string(signature));
    std::string sig_str = signature_bytes.GetHex();
    sig_str.copy(data->signatures[data->current_index], kSignatureHexSize);

    if (!IsEmptyString(related_pubkey)) {
      Pubkey key = Pubkey(std::string(related_pubkey));
      std::string pubkey_str = key.GetHex();
      pubkey_str.copy(data->pubkeys[data->current_index], kPubkeyHexSize);
    }

    ++(data->current_index);
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

int CfdAddMultisigScriptSigDataToDer(
    void* handle, void* multisig_handle, const char* signature,
    int sighash_type, bool sighash_anyone_can_pay,
    const char* related_pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }

    // encode to der
    SigHashType sighashtype(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
    ByteData signature_bytes = ByteData(std::string(signature));
    SignParameter param(signature_bytes, true, sighashtype);
    ByteData signature_der = param.ConvertToSignature();

    return CfdAddMultisigScriptSigData(
        handle, multisig_handle, signature_der.GetHex().c_str(),
        related_pubkey);
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdFinalizeMultisigScriptSig(
    void* handle, void* multisig_handle, const char* redeem_script,
    char** scriptsig) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(multisig_handle, kPrefiMultisigScriptSig);
    if (IsEmptyString(redeem_script)) {
      warn(CFD_LOG_SOURCE, "redeemScript is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (scriptsig == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    Script redeem_script_obj = Script(std::string(redeem_script));

    CfdCapiMultisigScriptSigData* data =
        static_cast<CfdCapiMultisigScriptSigData*>(multisig_handle);
    if (data->current_index == 0) {
      warn(CFD_LOG_SOURCE, "The signature has empty.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "The signature has empty.");
    }
    if (data->current_index > kMultisigMaxKeyNum) {
      warn(
          CFD_LOG_SOURCE,
          "The number of signature has reached the upper limit.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "The number of signature has reached the upper limit.");
    }

    std::vector<SignParameter> sign_list;
    for (uint32_t index = 0; index < data->current_index; ++index) {
      SignParameter param(ByteData(std::string(data->signatures[index])));
      std::string pubkey(data->pubkeys[index]);
      if (Pubkey::IsValid(ByteData(pubkey))) {
        param.SetRelatedPubkey(Pubkey(pubkey));
      }
      sign_list.push_back(param);
    }

    TransactionApi api;
    std::string multisig_scriptsig =
        api.CreateMultisigScriptSig(sign_list, redeem_script_obj);
    *scriptsig = CreateString(multisig_scriptsig);

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

int CfdFreeMultisigScriptSigHandle(void* handle, void* multisig_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    FreeBuffer(
        multisig_handle, kPrefiMultisigScriptSig,
        sizeof(CfdCapiMultisigScriptSigData));
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
