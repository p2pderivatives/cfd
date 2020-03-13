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
#include "cfdcore/cfdcore_descriptor.h"
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
using cfd::core::DescriptorKeyInfo;
using cfd::core::ExtPrivkey;
using cfd::core::ExtPubkey;
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
      warn(CFD_LOG_SOURCE, "sighash is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sighash is null or empty.");
    }
    if (IsEmptyString(privkey) && IsEmptyString(wif)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
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
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
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

int CfdDecodeSignatureFromDer(
    void* handle, const char* der_signature, char** signature,
    int* sighash_type, bool* sighash_anyone_can_pay) {
  try {
    cfd::Initialize();
    if (IsEmptyString(der_signature)) {
      warn(CFD_LOG_SOURCE, "der_signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. der_signature is null or empty.");
    }
    if (signature == nullptr) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }

    SigHashType type;
    ByteData sig =
        CryptoUtil::ConvertSignatureFromDer(ByteData(der_signature), &type);
    *signature = CreateString(sig.GetHex());
    if (sighash_type) *sighash_type = int{type.GetSigHashAlgorithm()};
    if (sighash_anyone_can_pay)
      *sighash_anyone_can_pay = type.IsAnyoneCanPay();

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
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
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
      warn(CFD_LOG_SOURCE, "wif is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. wif is null or empty.");
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

int CfdGetPrivkeyWif(
    void* handle, const char* privkey, int network_type, bool is_compressed,
    char** wif) {
  try {
    cfd::Initialize();
    if (wif == nullptr) {
      warn(CFD_LOG_SOURCE, "wif is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. wif is null.");
    }
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    Privkey key(privkey);
    std::string privkey_wif = key.ConvertWif(net_type, is_compressed);
    *wif = CreateString(privkey_wif);

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

int CfdParsePrivkeyWif(
    void* handle, const char* wif, char** privkey, int* network_type,
    bool* is_compressed) {
  try {
    cfd::Initialize();
    if (IsEmptyString(wif)) {
      warn(CFD_LOG_SOURCE, "wif is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. wif is null or empty.");
    }

    NetType temp_nettype = NetType::kMainnet;
    bool temp_is_compressed = false;
    KeyApi api;
    Privkey key =
        api.GetPrivkeyFromWif(wif, &temp_nettype, &temp_is_compressed);
    if (privkey != nullptr) *privkey = CreateString(key.GetHex());
    if (is_compressed != nullptr) {
      *is_compressed = temp_is_compressed;
    }
    if (network_type != nullptr) {
      if (temp_nettype == NetType::kMainnet) {
        *network_type = kCfdNetworkMainnet;
      } else {
        *network_type = kCfdNetworkTestnet;
      }
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
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
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
      warn(CFD_LOG_SOURCE, "seed is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. seed is null or empty.");
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
      warn(CFD_LOG_SOURCE, "extkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null or empty.");
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
      warn(CFD_LOG_SOURCE, "extkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null or empty.");
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
      warn(CFD_LOG_SOURCE, "extkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null or empty.");
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
      warn(CFD_LOG_SOURCE, "extkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null or empty.");
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

int CfdGetParentExtkeyPathData(
    void* handle, const char* parent_extkey, const char* path,
    int child_key_type, char** key_path_data, char** child_key) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_key_path_data = nullptr;
  char* work_child_key = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(parent_extkey)) {
      warn(CFD_LOG_SOURCE, "parent extkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to get parameter. parent extkey is null or empty.");
    }

    std::string path_from_parent;
    if (!IsEmptyString(path)) {
      path_from_parent = path;
    }

    std::string parent_info;
    std::string child_key_string;
    std::string parent_key = parent_extkey;
    std::string hdkey_top;
    if (parent_key.size() > 4) {
      hdkey_top = parent_key.substr(1, 3);
    }
    if (hdkey_top == "prv") {
      ExtPrivkey ext_privkey(parent_key);
      parent_info = DescriptorKeyInfo::GetExtPrivkeyInformation(
          ext_privkey, path_from_parent);
      if (!path_from_parent.empty()) {
        ext_privkey = ext_privkey.DerivePrivkey(path_from_parent);
        if (child_key_type == kCfdExtPrivkey) {
          child_key_string = ext_privkey.ToString();
        } else {
          child_key_string = ext_privkey.GetExtPubkey().ToString();
        }
      }
    } else {
      ExtPubkey ext_pubkey(parent_key);
      parent_info = DescriptorKeyInfo::GetExtPubkeyInformation(
          ext_pubkey, path_from_parent);
      if (!path_from_parent.empty()) {
        ext_pubkey = ext_pubkey.DerivePubkey(path_from_parent);
        child_key_string = ext_pubkey.ToString();
      }
    }

    work_key_path_data = CreateString(parent_info);
    work_child_key = CreateString(child_key_string);

    if (key_path_data != nullptr) *key_path_data = work_key_path_data;
    if (child_key != nullptr) *child_key = work_child_key;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_key_path_data, &work_child_key);
  return result;
}

int CfdGetExtkeyInformation(
    void* handle, const char* extkey, char** version, char** fingerprint,
    char** chain_code, uint32_t* depth, uint32_t* child_number) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_version = nullptr;
  char* work_fingerprint = nullptr;
  char* work_chain_code = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(extkey)) {
      warn(CFD_LOG_SOURCE, "parent extkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to get parameter. parent extkey is null or empty.");
    }

    std::string extkey_string = extkey;
    std::string hdkey_top;
    uint32_t work_depth = 0;
    uint32_t work_child_number = 0;
    if (extkey_string.size() > 4) {
      hdkey_top = extkey_string.substr(1, 3);
    }
    if (hdkey_top == "prv") {
      ExtPrivkey ext_privkey(extkey_string);
      work_version = CreateString(ext_privkey.GetVersionData().GetHex());
      work_fingerprint =
          CreateString(ext_privkey.GetFingerprintData().GetHex());
      work_chain_code = CreateString(ext_privkey.GetChainCode().GetHex());
      work_depth = ext_privkey.GetDepth();
      work_child_number = ext_privkey.GetChildNum();
    } else {
      ExtPubkey ext_pubkey(extkey_string);
      work_version = CreateString(ext_pubkey.GetVersionData().GetHex());
      work_fingerprint =
          CreateString(ext_pubkey.GetFingerprintData().GetHex());
      work_chain_code = CreateString(ext_pubkey.GetChainCode().GetHex());
      work_depth = ext_pubkey.GetDepth();
      work_child_number = ext_pubkey.GetChildNum();
    }

    if (version != nullptr) *version = work_version;
    if (fingerprint != nullptr) *fingerprint = work_fingerprint;
    if (chain_code != nullptr) *chain_code = work_chain_code;
    if (depth != nullptr) *depth = work_depth;
    if (child_number != nullptr) *child_number = work_child_number;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_version, &work_fingerprint, &work_chain_code);
  return result;
}

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
