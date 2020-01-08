// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_key.cpp
 *
 * @brief cfd-capiで利用するKey操作の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include <string>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfd/cfdapi_hdwallet.h"
#include "cfd/cfdapi_key.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

using cfd::api::ExtKeyType;
using cfd::api::HDWalletApi;
using cfd::api::KeyApi;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! privkey uncompress size
static constexpr size_t kPrivkeyWifUncompressSize = 51;

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kPrivkeyWifUncompressSize;
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
      private_key = Privkey::FromWif(
          wif, net_type, (strlen(wif) != kPrivkeyWifUncompressSize));
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

int CfdEncodeSignatureByDer(
    void* handle, const char* signature, int sighash_type,
    bool sighash_anyone_can_pay, char** der_signature) {
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }
    if (signature == nullptr) {
      warn(CFD_LOG_SOURCE, "der_signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. der_signature is null.");
    }

    SigHashType type = SigHashType(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
    ByteData der_sig = CryptoUtil::ConvertSignatureToDer(signature, type);
    *der_signature = CreateString(der_sig.GetHex());

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

int CfdNormalizeSignature(
    void* handle, const char* signature, char** normalized_signature) {
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }
    if (normalized_signature == nullptr) {
      warn(CFD_LOG_SOURCE, "normalized_signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. normalized_signature is null.");
    }

    ByteData result = CryptoUtil::NormalizeSignature(ByteData(signature));
    *normalized_signature = CreateString(result.GetHex());

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
  char* work_pubkey = nullptr;
  char* work_privkey = nullptr;
  char* work_wif = nullptr;
  try {
    cfd::Initialize();

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);

    KeyApi api;
    Pubkey pubkey_obj;
    std::string privkey_wif;
    Privkey key =
        api.CreateKeyPair(is_compressed, &pubkey_obj, &privkey_wif, net_type);

    if (wif != nullptr) {
      work_wif = CreateString(privkey_wif);
    }
    if (privkey != nullptr) {
      work_privkey = CreateString(key.GetHex());
    }
    if (pubkey != nullptr) {
      work_pubkey = CreateString(pubkey_obj.GetHex());
    }

    if (work_privkey != nullptr) *privkey = work_privkey;
    if (work_wif != nullptr) *wif = work_wif;
    if (work_pubkey != nullptr) *pubkey = work_pubkey;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(&work_privkey, &work_wif, &work_pubkey);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(&work_privkey, &work_wif, &work_pubkey);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(&work_privkey, &work_wif, &work_pubkey);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetPrivkeyFromWif(
    void* handle, const char* wif, int network_type, char** privkey) {
  try {
    cfd::Initialize();
    if (privkey == nullptr) {
      warn(CFD_LOG_SOURCE, "privkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null.");
    }
    if (IsEmptyString(wif)) {
      warn(CFD_LOG_SOURCE, "wif is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. wif is null.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    Privkey key = Privkey::FromWif(
        wif, net_type, (strlen(wif) != kPrivkeyWifUncompressSize));
    *privkey = CreateString(key.GetHex());

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

int CfdGetPubkeyFromPrivkey(
    void* handle, const char* privkey, const char* wif, bool is_compressed,
    char** pubkey) {
  try {
    cfd::Initialize();
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (IsEmptyString(privkey) && IsEmptyString(wif)) {
      warn(CFD_LOG_SOURCE, "privkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null.");
    }

    KeyApi api;
    std::string key;
    if (!IsEmptyString(wif)) {
      key = api.GetPubkeyFromPrivkey(wif, is_compressed);
    } else {
      key = api.GetPubkeyFromPrivkey(privkey, is_compressed);
    }
    *pubkey = CreateString(key);

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

int CfdCreateExtkeyFromSeed(
    void* handle, const char* seed_hex, int network_type, int key_type,
    char** extkey) {
  try {
    cfd::Initialize();
    if (extkey == nullptr) {
      warn(CFD_LOG_SOURCE, "extkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null.");
    }
    if (IsEmptyString(seed_hex)) {
      warn(CFD_LOG_SOURCE, "seed is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. seed is null.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    ExtKeyType output_key_type = static_cast<ExtKeyType>(key_type);
    HDWalletApi api;
    std::string key = api.CreateExtkeyFromSeed(
        ByteData(seed_hex), net_type, output_key_type);
    *extkey = CreateString(key);

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

int CfdCreateExtkeyFromParentPath(
    void* handle, const char* extkey, const char* path, int network_type,
    int key_type, char** child_extkey) {
  try {
    cfd::Initialize();
    if (child_extkey == nullptr) {
      warn(CFD_LOG_SOURCE, "child extkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. child extkey is null.");
    }
    if (IsEmptyString(extkey)) {
      warn(CFD_LOG_SOURCE, "extkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null.");
    }

    std::string path_string;
    if (!IsEmptyString(path)) {
      path_string = path;
    }
    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    ExtKeyType output_key_type = static_cast<ExtKeyType>(key_type);
    HDWalletApi api;
    std::string key = api.CreateExtkeyFromPathString(
        extkey, net_type, output_key_type, path_string);
    *child_extkey = CreateString(key);

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

int CfdCreateExtPubkey(
    void* handle, const char* extkey, int network_type, char** ext_pubkey) {
  try {
    cfd::Initialize();
    if (ext_pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "extpubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extpubkey is null.");
    }
    if (IsEmptyString(extkey)) {
      warn(CFD_LOG_SOURCE, "extkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    HDWalletApi api;
    std::string key = api.CreateExtPubkey(extkey, net_type);
    *ext_pubkey = CreateString(key);

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

int CfdGetPrivkeyFromExtkey(
    void* handle, const char* extkey, int network_type, char** privkey,
    char** wif) {
  char* work_privkey = nullptr;
  char* work_wif = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(extkey)) {
      warn(CFD_LOG_SOURCE, "extkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    HDWalletApi api;
    std::string key;
    if (privkey != nullptr) {
      key = api.GetPrivkeyFromExtkey(extkey, net_type, false);
      work_privkey = CreateString(key);
    }
    if (wif != nullptr) {
      key = api.GetPrivkeyFromExtkey(extkey, net_type, true);
      work_wif = CreateString(key);
    }

    if (work_privkey != nullptr) *privkey = work_privkey;
    if (work_wif != nullptr) *wif = work_wif;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(&work_privkey, &work_wif);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(&work_privkey, &work_wif);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(&work_privkey, &work_wif);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetPubkeyFromExtkey(
    void* handle, const char* extkey, int network_type, char** pubkey) {
  try {
    cfd::Initialize();
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (IsEmptyString(extkey)) {
      warn(CFD_LOG_SOURCE, "extkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    HDWalletApi api;
    std::string key = api.GetPubkeyFromExtkey(extkey, net_type);
    *pubkey = CreateString(key);

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
