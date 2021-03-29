// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_key.cpp
 *
 * @brief cfd-capiで利用するKey操作の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include "cfdc/cfdcapi_key.h"

#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfd/cfdapi_hdwallet.h"
#include "cfd/cfdapi_key.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_ecdsa_adaptor.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_schnorrsig.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

using cfd::api::ExtKeyType;
using cfd::api::HDWalletApi;
using cfd::api::KeyApi;
using cfd::core::AdaptorPair;
using cfd::core::AdaptorProof;
using cfd::core::AdaptorSignature;
using cfd::core::AdaptorUtil;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
using cfd::core::DescriptorKeyInfo;
using cfd::core::ExtPrivkey;
using cfd::core::ExtPubkey;
using cfd::core::HDWallet;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::SchnorrPubkey;
using cfd::core::SchnorrSignature;
using cfd::core::SchnorrUtil;
using cfd::core::Script;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;
using cfd::core::StringUtil;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! privkey uncompress size
static constexpr size_t kPrivkeyWifUncompressSize = 51;
//! prefix: GetMnemonicWordList
constexpr const char* const kPrefixGetMnemonicWordList = "GetMnemonicList";
//! prefix: CombinePubkey
constexpr const char* const kPrefixCombinePubkey = "CombinePubkey";
/**
 * @brief cfd-capi GetMnemonicWordList構造体.
 */
struct CfdCapiGetMnemonicWordList {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! mnemonic word list
  std::vector<std::string>* wordlist;
};

/**
 * @brief cfd-capi CfdCapiCombinePubkey構造体.
 */
struct CfdCapiCombinePubkey {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! pubkey list
  std::vector<std::string>* pubkey_list;
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiCombinePubkey;
using cfd::capi::CfdCapiGetMnemonicWordList;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kPrefixCombinePubkey;
using cfd::capi::kPrefixGetMnemonicWordList;
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

int CfdVerifyEcSignature(
    void* handle, const char* sighash, const char* pubkey,
    const char* signature) {
  try {
    cfd::Initialize();
    if (IsEmptyString(sighash)) {
      warn(CFD_LOG_SOURCE, "sighash is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sighash is null or empty.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }

    Pubkey pubkey_obj(pubkey);
    bool is_verify = pubkey_obj.VerifyEcSignature(
        ByteData256(sighash), ByteData(signature));
    if (!is_verify) {
      return CfdErrorCode::kCfdSignVerificationError;
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

int CfdSignEcdsaAdaptor(
    void* handle, const char* msg, const char* sk, const char* adaptor,
    char** adaptor_signature, char** adaptor_proof) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_signature = nullptr;
  char* work_proof = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(msg)) {
      warn(CFD_LOG_SOURCE, "msg is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. msg is null or empty.");
    }
    if (IsEmptyString(sk)) {
      warn(CFD_LOG_SOURCE, "sk is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sk is null or empty.");
    }
    if (IsEmptyString(adaptor)) {
      warn(CFD_LOG_SOURCE, "adaptor is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor is null or empty.");
    }
    if (adaptor_signature == nullptr) {
      warn(CFD_LOG_SOURCE, "adaptor signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor signature is null.");
    }
    if (adaptor_proof == nullptr) {
      warn(CFD_LOG_SOURCE, "adaptor proof is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor proof is null.");
    }

    AdaptorPair pair =
        AdaptorUtil::Sign(ByteData256(msg), Privkey(sk), Pubkey(adaptor));

    work_signature = CreateString(pair.signature.GetData().GetHex());
    work_proof = CreateString(pair.proof.GetData().GetHex());

    *adaptor_signature = work_signature;
    *adaptor_proof = work_proof;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_signature, &work_proof);
  return result;
}

int CfdAdaptEcdsaAdaptor(
    void* handle, const char* adaptor_signature, const char* adaptor_secret,
    char** signature) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(adaptor_signature)) {
      warn(CFD_LOG_SOURCE, "adaptor_signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor_signature is null or empty.");
    }
    if (IsEmptyString(adaptor_secret)) {
      warn(CFD_LOG_SOURCE, "adaptor_secret is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor_secret is null or empty.");
    }
    if (signature == nullptr) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }

    ByteData sig = AdaptorUtil::Adapt(
        AdaptorSignature(adaptor_signature), Privkey(adaptor_secret));

    *signature = CreateString(sig.GetHex());
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

int CfdExtractEcdsaAdaptorSecret(
    void* handle, const char* adaptor_signature, const char* signature,
    const char* adaptor, char** adaptor_secret) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(adaptor_signature)) {
      warn(CFD_LOG_SOURCE, "adaptor_signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor_signature is null or empty.");
    }
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }
    if (IsEmptyString(adaptor)) {
      warn(CFD_LOG_SOURCE, "adaptor is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor is null or empty.");
    }
    if (adaptor_secret == nullptr) {
      warn(CFD_LOG_SOURCE, "adaptor_secret is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor_secret is null.");
    }

    Privkey secret = AdaptorUtil::ExtractSecret(
        AdaptorSignature(adaptor_signature), ByteData(signature),
        Pubkey(adaptor));

    *adaptor_secret = CreateString(secret.GetHex());
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

int CfdVerifyEcdsaAdaptor(
    void* handle, const char* adaptor_signature, const char* proof,
    const char* adaptor, const char* msg, const char* pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(adaptor_signature)) {
      warn(CFD_LOG_SOURCE, "adaptor_signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor_signature is null or empty.");
    }
    if (IsEmptyString(proof)) {
      warn(CFD_LOG_SOURCE, "proof is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. proof is null or empty.");
    }
    if (IsEmptyString(adaptor)) {
      warn(CFD_LOG_SOURCE, "adaptor is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. adaptor is null or empty.");
    }
    if (IsEmptyString(msg)) {
      warn(CFD_LOG_SOURCE, "msg is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. msg is null or empty.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    bool is_verify = AdaptorUtil::Verify(
        AdaptorSignature(adaptor_signature), AdaptorProof(proof),
        Pubkey(adaptor), ByteData256(msg), Pubkey(pubkey));
    if (!is_verify) {
      return CfdErrorCode::kCfdSignVerificationError;
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

int CfdGetSchnorrPubkeyFromPrivkey(
    void* handle, const char* privkey, char** pubkey, bool* parity) {
  try {
    cfd::Initialize();
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }

    Privkey privkey_obj = Privkey(std::string(privkey));
    SchnorrPubkey schnorr_pubkey =
        SchnorrPubkey::FromPrivkey(privkey_obj, parity);
    *pubkey = CreateString(schnorr_pubkey.GetHex());

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

int CfdGetSchnorrPubkeyFromPubkey(
    void* handle, const char* pubkey, char** schnorr_pubkey, bool* parity) {
  try {
    cfd::Initialize();
    if (schnorr_pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "schnorr pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. schnorr pubkey is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    Pubkey pubkey_obj = Pubkey(std::string(pubkey));
    SchnorrPubkey schnorr_pubkey_obj =
        SchnorrPubkey::FromPubkey(pubkey_obj, parity);
    *schnorr_pubkey = CreateString(schnorr_pubkey_obj.GetHex());

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

int CfdSchnorrPubkeyTweakAdd(
    void* handle, const char* pubkey, const char* tweak, char** output,
    bool* parity) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "tweak pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak pubkey is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }
    if (IsEmptyString(tweak)) {
      warn(CFD_LOG_SOURCE, "tweak is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak is null or empty.");
    }

    ByteData256 tweak_obj = ByteData256(std::string(tweak));
    SchnorrPubkey pubkey_obj = SchnorrPubkey(std::string(pubkey));
    SchnorrPubkey schnorr_pubkey =
        pubkey_obj.CreateTweakAdd(tweak_obj, parity);
    *output = CreateString(schnorr_pubkey.GetHex());

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

int CfdSchnorrKeyPairTweakAdd(
    void* handle, const char* privkey, const char* tweak,
    char** tweaked_pubkey, bool* tweaked_parity, char** tweaked_privkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_pubkey = nullptr;
  char* work_privkey = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }
    if (IsEmptyString(tweak)) {
      warn(CFD_LOG_SOURCE, "tweak is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak is null or empty.");
    }

    ByteData256 tweak_obj = ByteData256(std::string(tweak));
    Privkey privkey_obj = Privkey(std::string(privkey));
    Privkey tweaked_privkey_obj;
    SchnorrPubkey schnorr_pubkey = SchnorrPubkey::CreateTweakAddFromPrivkey(
        privkey_obj, tweak_obj, &tweaked_privkey_obj, tweaked_parity);
    if (tweaked_pubkey != nullptr) {
      work_pubkey = CreateString(schnorr_pubkey.GetHex());
    }
    if (tweaked_privkey != nullptr) {
      work_privkey = CreateString(tweaked_privkey_obj.GetHex());
    }

    if (tweaked_pubkey != nullptr) *tweaked_pubkey = work_pubkey;
    if (tweaked_privkey != nullptr) *tweaked_privkey = work_privkey;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_pubkey, &work_privkey);
  return result;
}

int CfdCheckTweakAddFromSchnorrPubkey(
    void* handle, const char* tweaked_pubkey, bool tweaked_parity,
    const char* base_pubkey, const char* tweak) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(tweaked_pubkey)) {
      warn(CFD_LOG_SOURCE, "tweak pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak pubkey is null or empty.");
    }
    if (IsEmptyString(base_pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }
    if (IsEmptyString(tweak)) {
      warn(CFD_LOG_SOURCE, "tweak is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak is null or empty.");
    }

    ByteData256 tweak_obj = ByteData256(std::string(tweak));
    SchnorrPubkey pubkey_obj = SchnorrPubkey(std::string(tweaked_pubkey));
    SchnorrPubkey base_pubkey_obj = SchnorrPubkey(std::string(base_pubkey));
    bool is_tweaked =
        pubkey_obj.IsTweaked(base_pubkey_obj, tweak_obj, tweaked_parity);
    if (!is_tweaked) {
      return CfdErrorCode::kCfdSignVerificationError;
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

int CfdSignSchnorr(
    void* handle, const char* msg, const char* sk, const char* aux_rand,
    char** signature) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(msg)) {
      warn(CFD_LOG_SOURCE, "msg is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. msg is null or empty.");
    }
    if (IsEmptyString(sk)) {
      warn(CFD_LOG_SOURCE, "sk is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sk is null or empty.");
    }
    if (signature == nullptr) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }

    SchnorrSignature schnorr_sig;
    if (IsEmptyString(aux_rand)) {
      schnorr_sig = SchnorrUtil::Sign(ByteData256(msg), Privkey(sk));
    } else {
      schnorr_sig = SchnorrUtil::Sign(
          ByteData256(msg), Privkey(sk), ByteData256(aux_rand));
    }

    *signature = CreateString(schnorr_sig.GetHex());
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

int CfdSignSchnorrWithNonce(
    void* handle, const char* msg, const char* sk, const char* nonce,
    char** signature) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(msg)) {
      warn(CFD_LOG_SOURCE, "msg is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. msg is null or empty.");
    }
    if (IsEmptyString(nonce)) {
      warn(CFD_LOG_SOURCE, "nonce is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. nonce is null or empty.");
    }
    if (IsEmptyString(sk)) {
      warn(CFD_LOG_SOURCE, "sk is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sk is null or empty.");
    }
    if (signature == nullptr) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }

    SchnorrSignature schnorr_sig = SchnorrUtil::SignWithNonce(
        ByteData256(msg), Privkey(sk), Privkey(nonce));

    *signature = CreateString(schnorr_sig.GetHex());
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

int CfdAddSighashTypeInSchnorrSignature(
    void* handle, const char* signature, int sighash_type, bool anyone_can_pay,
    char** added_signature) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }
    if (added_signature == nullptr) {
      warn(CFD_LOG_SOURCE, "added_signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. added_signature is null or empty.");
    }
    SchnorrSignature sig(signature);
    SigHashType sighash_type_obj = SigHashType::Create(
        static_cast<uint8_t>(sighash_type), anyone_can_pay);
    sig.SetSigHashType(sighash_type_obj);
    *added_signature = CreateString(sig.GetData(true).GetHex());
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

int CfdGetSighashTypeFromSchnorrSignature(
    void* handle, const char* signature, int* sighash_type,
    bool* anyone_can_pay) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }
    SchnorrSignature sig(signature);
    auto sighash_type_obj = sig.GetSigHashType();
    if (sighash_type != nullptr) {
      *sighash_type = static_cast<int>(sighash_type_obj.GetSigHashAlgorithm());
    }
    if (anyone_can_pay != nullptr) {
      *anyone_can_pay = sighash_type_obj.IsAnyoneCanPay();
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

int CfdComputeSchnorrSigPoint(
    void* handle, const char* msg, const char* nonce, const char* pubkey,
    char** sigpoint) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(msg)) {
      warn(CFD_LOG_SOURCE, "msg is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. msg is null or empty.");
    }
    if (IsEmptyString(nonce)) {
      warn(CFD_LOG_SOURCE, "nonce is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. nonce is null or empty.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }
    if (sigpoint == nullptr) {
      warn(CFD_LOG_SOURCE, "sigpoint is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sigpoint is null.");
    }

    Pubkey sig_point = SchnorrUtil::ComputeSigPoint(
        ByteData256(msg), SchnorrPubkey(nonce), SchnorrPubkey(pubkey));

    *sigpoint = CreateString(sig_point.GetHex());
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

int CfdVerifySchnorr(
    void* handle, const char* signature, const char* msg, const char* pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }
    if (IsEmptyString(msg)) {
      warn(CFD_LOG_SOURCE, "msg is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. msg is null or empty.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    bool is_verify = SchnorrUtil::Verify(
        SchnorrSignature(signature), ByteData256(msg), SchnorrPubkey(pubkey));
    if (!is_verify) {
      return CfdErrorCode::kCfdSignVerificationError;
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

int CfdSplitSchnorrSignature(
    void* handle, const char* signature, char** nonce, char** key) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_nonce = nullptr;
  char* work_key = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }
    if (nonce == nullptr) {
      warn(CFD_LOG_SOURCE, "nonce is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. nonce is null.");
    }
    if (key == nullptr) {
      warn(CFD_LOG_SOURCE, "key is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. key is null.");
    }

    SchnorrSignature sig = SchnorrSignature(std::string(signature));
    work_nonce = CreateString(sig.GetNonce().GetHex());
    work_key = CreateString(sig.GetPrivkey().GetHex());

    *nonce = work_nonce;
    *key = work_key;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_nonce, &work_key);
  return result;
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

    SigHashType type = SigHashType::Create(
        static_cast<uint8_t>(sighash_type), sighash_anyone_can_pay);
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

    size_t wif_len = strnlen(wif, kPrivkeyWifUncompressSize + 1);
    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    Privkey key = Privkey::FromWif(
        wif, net_type, (wif_len != kPrivkeyWifUncompressSize));
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

int CfdGetPubkeyFingerprint(
    void* handle, const char* pubkey, char** fingerprint) {
  try {
    cfd::Initialize();
    if (fingerprint == nullptr) {
      warn(CFD_LOG_SOURCE, "fingerprint is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. fingerprint is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    Pubkey key(pubkey);
    *fingerprint = CreateString(key.GetFingerprint().GetHex());

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

int CfdCompressPubkey(void* handle, const char* pubkey, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    Pubkey key(pubkey);
    *output = CreateString(key.Compress().GetHex());

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

int CfdUncompressPubkey(void* handle, const char* pubkey, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    Pubkey key(pubkey);
    *output = CreateString(key.Uncompress().GetHex());

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

int CfdInitializeCombinePubkey(void* handle, void** combine_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiCombinePubkey* buffer = nullptr;
  try {
    cfd::Initialize();
    if (combine_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "combine handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. combine handle is null.");
    }

    buffer = static_cast<CfdCapiCombinePubkey*>(
        AllocBuffer(kPrefixCombinePubkey, sizeof(CfdCapiCombinePubkey)));
    buffer->pubkey_list = new std::vector<std::string>();
    *combine_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  if (buffer != nullptr) CfdFreeCombinePubkeyHandle(handle, buffer);
  return result;
}

int CfdAddCombinePubkey(
    void* handle, void* combine_handle, const char* pubkey) {
  try {
    cfd::Initialize();
    CheckBuffer(combine_handle, kPrefixCombinePubkey);
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    CfdCapiCombinePubkey* buffer =
        static_cast<CfdCapiCombinePubkey*>(combine_handle);
    if (buffer->pubkey_list == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey_list is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to parameter. pubkey_list is null.");
    }
    Pubkey key(pubkey);
    buffer->pubkey_list->push_back(key.GetHex());
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return CfdErrorCode::kCfdUnknownError;
}

int CfdFinalizeCombinePubkey(
    void* handle, void* combine_handle, char** output) {
  try {
    cfd::Initialize();
    CheckBuffer(combine_handle, kPrefixCombinePubkey);
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }

    CfdCapiCombinePubkey* buffer =
        static_cast<CfdCapiCombinePubkey*>(combine_handle);
    if ((buffer->pubkey_list == nullptr) || buffer->pubkey_list->empty()) {
      warn(CFD_LOG_SOURCE, "pubkey_list is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to parameter. pubkey_list is null or empty.");
    }
    std::vector<Pubkey> key_list;
    key_list.reserve(buffer->pubkey_list->size());
    for (const auto& pubkey : *buffer->pubkey_list) {
      key_list.emplace_back(pubkey);
    }
    Pubkey key = Pubkey::CombinePubkey(key_list);
    *output = CreateString(key.GetHex());

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return CfdErrorCode::kCfdUnknownError;
}

int CfdFreeCombinePubkeyHandle(void* handle, void* combine_handle) {
  try {
    cfd::Initialize();
    if (combine_handle != nullptr) {
      CfdCapiCombinePubkey* combine_struct =
          static_cast<CfdCapiCombinePubkey*>(combine_handle);
      if (combine_struct->pubkey_list != nullptr) {
        delete combine_struct->pubkey_list;
        combine_struct->pubkey_list = nullptr;
      }
    }
    FreeBuffer(
        combine_handle, kPrefixCombinePubkey, sizeof(CfdCapiCombinePubkey));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return CfdErrorCode::kCfdUnknownError;
}

int CfdPubkeyTweakAdd(
    void* handle, const char* pubkey, const char* tweak, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }
    if (IsEmptyString(tweak)) {
      warn(CFD_LOG_SOURCE, "tweak is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak is null or empty.");
    }

    Pubkey key(pubkey);
    ByteData256 tweak_data(tweak);
    *output = CreateString(key.CreateTweakAdd(tweak_data).GetHex());

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

int CfdPubkeyTweakMul(
    void* handle, const char* pubkey, const char* tweak, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }
    if (IsEmptyString(tweak)) {
      warn(CFD_LOG_SOURCE, "tweak is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak is null or empty.");
    }

    Pubkey key(pubkey);
    ByteData256 tweak_data(tweak);
    *output = CreateString(key.CreateTweakMul(tweak_data).GetHex());

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

int CfdNegatePubkey(void* handle, const char* pubkey, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    Pubkey key(pubkey);
    *output = CreateString(key.CreateNegate().GetHex());

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

int CfdPrivkeyTweakAdd(
    void* handle, const char* privkey, const char* tweak, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }
    if (IsEmptyString(tweak)) {
      warn(CFD_LOG_SOURCE, "tweak is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak is null or empty.");
    }

    KeyApi api;
    Privkey key = api.GetPrivkey(std::string(privkey), nullptr, nullptr);
    ByteData256 tweak_data(tweak);
    *output = CreateString(key.CreateTweakAdd(tweak_data).GetHex());

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

int CfdPrivkeyTweakMul(
    void* handle, const char* privkey, const char* tweak, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }
    if (IsEmptyString(tweak)) {
      warn(CFD_LOG_SOURCE, "tweak is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweak is null or empty.");
    }

    KeyApi api;
    Privkey key = api.GetPrivkey(std::string(privkey), nullptr, nullptr);
    ByteData256 tweak_data(tweak);
    *output = CreateString(key.CreateTweakMul(tweak_data).GetHex());

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

int CfdNegatePrivkey(void* handle, const char* privkey, char** output) {
  try {
    cfd::Initialize();
    if (output == nullptr) {
      warn(CFD_LOG_SOURCE, "output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output is null.");
    }
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }

    KeyApi api;
    Privkey key = api.GetPrivkey(std::string(privkey), nullptr, nullptr);
    *output = CreateString(key.CreateNegate().GetHex());

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

CFDC_API int CfdCreateExtkey(
    void* handle, int network_type, int key_type, const char* parent_key,
    const char* fingerprint, const char* key, const char* chain_code,
    unsigned char depth, uint32_t child_number, char** extkey) {
  try {
    cfd::Initialize();
    if (extkey == nullptr) {
      warn(CFD_LOG_SOURCE, "extkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. extkey is null.");
    }
    if (IsEmptyString(key)) {
      warn(CFD_LOG_SOURCE, "key is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. key is null or empty.");
    }

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    std::string key_str = key;
    std::string chain_code_str = chain_code;
    std::string parent_key_str;
    if (!IsEmptyString(parent_key)) {
      parent_key_str = parent_key;
    }
    std::string fingerprint_str;
    if (!IsEmptyString(fingerprint)) {
      fingerprint_str = fingerprint;
    }

    if (key_type == ExtKeyType::kExtPrivkey) {
      Privkey privkey;
      if (key_str.size() == (Privkey::kPrivkeySize * 2)) {
        privkey = Privkey(key_str);
      } else {
        privkey = Privkey::FromWif(key_str, net_type);
      }
      if (fingerprint_str.empty()) {
        Privkey parent_privkey;
        if (parent_key_str.size() == (Privkey::kPrivkeySize * 2)) {
          parent_privkey = Privkey(parent_key_str);
        } else {
          parent_privkey = Privkey::FromWif(parent_key_str, net_type);
        }
        ExtPrivkey extprivkey(
            net_type, parent_privkey, privkey, ByteData256(chain_code_str),
            depth, child_number);
        *extkey = CreateString(extprivkey.ToString());
      } else {
        ExtPrivkey extprivkey(
            net_type, ByteData(fingerprint_str), privkey,
            ByteData256(chain_code_str), depth, child_number);
        *extkey = CreateString(extprivkey.ToString());
      }
    } else {
      if (fingerprint_str.empty()) {
        ExtPubkey extpubkey(
            net_type, Pubkey(parent_key_str), Pubkey(key_str),
            ByteData256(chain_code_str), depth, child_number);
        *extkey = CreateString(extpubkey.ToString());
      } else {
        ExtPubkey extpubkey(
            net_type, ByteData(fingerprint_str), Pubkey(key_str),
            ByteData256(chain_code_str), depth, child_number);
        *extkey = CreateString(extpubkey.ToString());
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

int CfdCreateExtkeyFromParent(
    void* handle, const char* extkey, uint32_t child_number, bool hardened,
    int network_type, int key_type, char** child_extkey) {
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

    cfd::core::NetType net_type = ConvertNetType(network_type, nullptr);
    ExtKeyType output_key_type = static_cast<ExtKeyType>(key_type);
    HDWalletApi api;
    std::string key = api.CreateExtkeyFromParent(
        extkey, net_type, output_key_type, child_number, hardened);
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

int CfdInitializeMnemonicWordList(
    void* handle, const char* language, void** mnemonic_handle,
    uint32_t* max_index) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiGetMnemonicWordList* buffer = nullptr;
  try {
    cfd::Initialize();
    if (mnemonic_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "mnemonic handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. mnemonic handle is null.");
    }

    std::string lang;
    if (!IsEmptyString(language)) {
      lang = language;
    }
    HDWalletApi api;
    std::vector<std::string> wordlist = api.GetMnemonicWordlist(lang);
    if (max_index != nullptr) {
      *max_index = static_cast<uint32_t>(wordlist.size());
    }

    buffer = static_cast<CfdCapiGetMnemonicWordList*>(AllocBuffer(
        kPrefixGetMnemonicWordList, sizeof(CfdCapiGetMnemonicWordList)));
    buffer->wordlist = new std::vector<std::string>();
    buffer->wordlist->assign(wordlist.begin(), wordlist.end());
    *mnemonic_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  if (buffer != nullptr) CfdFreeDescriptorHandle(handle, buffer);
  return result;
}

int CfdGetMnemonicWord(
    void* handle, void* mnemonic_handle, uint32_t index,
    char** mnemonic_word) {
  try {
    cfd::Initialize();
    CheckBuffer(mnemonic_handle, kPrefixGetMnemonicWordList);
    if (mnemonic_word == nullptr) {
      warn(CFD_LOG_SOURCE, "mnemonic_word is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. mnemonic_word is null.");
    }

    CfdCapiGetMnemonicWordList* buffer =
        static_cast<CfdCapiGetMnemonicWordList*>(mnemonic_handle);
    if ((buffer->wordlist == nullptr) || (index >= buffer->wordlist->size())) {
      warn(CFD_LOG_SOURCE, "index is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. index is maximum over.");
    }

    *mnemonic_word = CreateString((*buffer->wordlist)[index]);
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return CfdErrorCode::kCfdUnknownError;
}

int CfdFreeMnemonicWordList(void* handle, void* mnemonic_handle) {
  try {
    cfd::Initialize();
    if (mnemonic_handle != nullptr) {
      CfdCapiGetMnemonicWordList* mnemonic_struct =
          static_cast<CfdCapiGetMnemonicWordList*>(mnemonic_handle);
      if (mnemonic_struct->wordlist != nullptr) {
        delete mnemonic_struct->wordlist;
        mnemonic_struct->wordlist = nullptr;
      }
    }
    FreeBuffer(
        mnemonic_handle, kPrefixGetMnemonicWordList,
        sizeof(CfdCapiGetMnemonicWordList));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return CfdErrorCode::kCfdUnknownError;
}

int CfdConvertMnemonicToSeed(
    void* handle, const char* mnemonic, const char* passphrase,
    bool strict_check, const char* language, bool use_ideographic_space,
    char** seed, char** entropy) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_entropy = nullptr;
  try {
    cfd::Initialize();
    if (seed == nullptr) {
      warn(CFD_LOG_SOURCE, "seed is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. seed is null.");
    }
    if (IsEmptyString(mnemonic)) {
      warn(CFD_LOG_SOURCE, "mnemonic is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. mnemonic is null or empty.");
    }

    std::string lang = "en";
    std::string passphrase_str;
    if (!IsEmptyString(language)) lang = language;
    if (!IsEmptyString(passphrase)) passphrase_str = passphrase;
    std::vector<std::string> mnemonic_words = StringUtil::Split(mnemonic, " ");

    ByteData entropy_data;
    HDWalletApi api;
    ByteData seed_data = api.ConvertMnemonicToSeed(
        mnemonic_words, passphrase_str, strict_check, lang,
        use_ideographic_space, &entropy_data);
    work_entropy = CreateString(entropy_data.GetHex());

    *seed = CreateString(seed_data.GetHex());
    if (entropy != nullptr) *entropy = work_entropy;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_entropy);
  return result;
}

int CfdConvertEntropyToMnemonic(
    void* handle, const char* entropy, const char* language, char** mnemonic) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (mnemonic == nullptr) {
      warn(CFD_LOG_SOURCE, "mnemonic is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. mnemonic is null.");
    }
    if (IsEmptyString(entropy)) {
      warn(CFD_LOG_SOURCE, "entropy is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. entropy is null or empty.");
    }

    std::string lang = "en";
    if (!IsEmptyString(language)) {
      lang = language;
    }

    HDWalletApi api;
    std::vector<std::string> word_list =
        api.ConvertEntropyToMnemonic(ByteData(std::string(entropy)), lang);
    std::string mnemonic_str;
    for (const auto& word : word_list) {
      if (!mnemonic_str.empty()) mnemonic_str += " ";
      mnemonic_str += word;
    }
    *mnemonic = CreateString(mnemonic_str);

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
