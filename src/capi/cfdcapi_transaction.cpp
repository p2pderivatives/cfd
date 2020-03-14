// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_transaction.cpp
 *
 * @brief cfd-capiで利用するTransaction処理の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include <exception>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_transaction.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

#ifndef CFD_DISABLE_ELEMENTS
using cfd::ConfidentialTransactionContext;
#endif  // CFD_DISABLE_ELEMENTS
using cfd::SignParameter;
using cfd::TransactionContext;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::NetType;
using cfd::core::OutPoint;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::Txid;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiMultisigSignData;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPrefixMultisigSignData;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::kSignatureHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

// API
extern "C" {

int CfdAddTxSign(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* sign_data_hex,
    bool use_der_encode, int sighash_type, bool sighash_anyone_can_pay,
    bool clear_stack, char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    AddressType addr_type = ConvertHashToAddressType(hash_type);
    bool is_witness = true;
    if ((addr_type == AddressType::kP2shAddress) ||
        (addr_type == AddressType::kP2pkhAddress)) {
      is_witness = false;
    }

    if ((sign_data_hex == nullptr) ||
        (IsEmptyString(sign_data_hex) && (use_der_encode || is_witness))) {
      warn(CFD_LOG_SOURCE, "sign data is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sign data is null or empty.");
    }

    SignParameter param;
    if (use_der_encode) {
      // encode to der
      SigHashType sighashtype(
          static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
      ByteData signature_bytes = ByteData(std::string(sign_data_hex));
      param = SignParameter(signature_bytes, true, sighashtype);
    } else {
      param = SignParameter(std::string(sign_data_hex));
    }
    std::vector<SignParameter> sign_list;
    sign_list.push_back(param);

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    OutPoint outpoint(Txid(txid), vout);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      tx.AddSign(outpoint, sign_list, is_witness, clear_stack);
      *tx_string = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      tx.AddSign(outpoint, sign_list, is_witness, clear_stack);
      *tx_string = CreateString(tx.GetHex());
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
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

int CfdAddPubkeyHashSign(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* pubkey, const char* signature,
    bool use_der_encode, int sighash_type, bool sighash_anyone_can_pay,
    char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
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
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    AddressType addr_type = ConvertHashToAddressType(hash_type);
    SignParameter param;
    if (use_der_encode) {
      // encode to der
      SigHashType sighashtype(
          static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
      ByteData signature_bytes = ByteData(std::string(signature));
      param = SignParameter(signature_bytes, true, sighashtype);
    } else {
      param = SignParameter(std::string(signature));
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    OutPoint outpoint(Txid(txid), vout);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      tx.AddPubkeyHashSign(outpoint, param, Pubkey(pubkey), addr_type);
      *tx_string = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      tx.AddPubkeyHashSign(outpoint, param, Pubkey(pubkey), addr_type);
      *tx_string = CreateString(tx.GetHex());
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
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

int CfdAddScriptHashSign(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* redeem_script, bool clear_stack,
    char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (IsEmptyString(redeem_script)) {
      warn(CFD_LOG_SOURCE, "redeem_script is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. redeem_script is null or empty.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    AddressType addr_type = ConvertHashToAddressType(hash_type);
    bool is_witness = true;
    if ((addr_type == AddressType::kP2shAddress) ||
        (addr_type == AddressType::kP2pkhAddress)) {
      is_witness = false;
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    OutPoint outpoint(Txid(txid), vout);
    std::vector<SignParameter> list;
    Script script = Script(std::string(redeem_script));
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      uint32_t index = tx.GetTxInIndex(outpoint);
      if (clear_stack) {
        if (is_witness) {
          tx.RemoveScriptWitnessStackAll(index);
        } else {
          tx.SetUnlockingScript(index, Script());
        }
      }
      tx.AddScriptHashSign(outpoint, list, script, addr_type);
      *tx_string = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      uint32_t index = tx.GetTxInIndex(outpoint);
      if (clear_stack) {
        if (is_witness) {
          tx.RemoveScriptWitnessStackAll(index);
        } else {
          tx.SetUnlockingScript(index, Script());
        }
      }
      tx.AddScriptHashSign(outpoint, list, script, addr_type);
      *tx_string = CreateString(tx.GetHex());
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
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

#if 0
/**
 * @brief add sign with privkey.
 * @param[in] handle          cfd handle.
 * @param[in] tx_hex_string   transaction hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[in] is_witness      insert to witness stack.
 * @param[in] signature       add sign data signature.
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pay flag.
 * @param[in] clear_stack     cleanup scriptSig stack data.
 * @param[out] tx_string      signed transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddSignWithPrivkeySimple(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* pubkey, const char* privkey,
    int64_t value_satoshi, int sighash_type, bool sighash_anyone_can_pay,
    char** tx_string);
#endif

int CfdInitializeMultisigSign(void* handle, void** multisign_handle) {
  try {
    cfd::Initialize();
    if (multisign_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "multisig sign handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. multisig sign handle is null.");
    }

    *multisign_handle =
        AllocBuffer(kPrefixMultisigSignData, sizeof(CfdCapiMultisigSignData));
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

int CfdAddMultisigSignData(
    void* handle, void* multisign_handle, const char* signature,
    const char* related_pubkey) {
  try {
    cfd::Initialize();
    CheckBuffer(multisign_handle, kPrefixMultisigSignData);
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }

    CfdCapiMultisigSignData* data =
        static_cast<CfdCapiMultisigSignData*>(multisign_handle);
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
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdAddMultisigSignDataToDer(
    void* handle, void* multisign_handle, const char* signature,
    int sighash_type, bool sighash_anyone_can_pay,
    const char* related_pubkey) {
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

    return CfdAddMultisigSignData(
        handle, multisign_handle, signature_der.GetHex().c_str(),
        related_pubkey);
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

int CfdFinalizeMultisigSign(
    void* handle, void* multisign_handle, int net_type,
    const char* tx_hex_string, const char* txid, uint32_t vout, int hash_type,
    const char* redeem_script, char** tx_string) {
  try {
    cfd::Initialize();
    CheckBuffer(multisign_handle, kPrefixMultisigSignData);
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (IsEmptyString(redeem_script)) {
      warn(CFD_LOG_SOURCE, "redeemScript is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. redeemScript is null or empty.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    AddressType addr_type = ConvertHashToAddressType(hash_type);
    Script witness_script_obj;
    Script redeem_script_obj;
    switch (addr_type) {
      case AddressType::kP2shAddress:
        redeem_script_obj = Script(std::string(redeem_script));
        break;
      case AddressType::kP2wshAddress:
        witness_script_obj = Script(std::string(redeem_script));
        break;
      case AddressType::kP2shP2wshAddress:
        witness_script_obj = Script(std::string(redeem_script));
        redeem_script_obj =
            ScriptUtil::CreateP2shLockingScript(witness_script_obj);
        break;
      default:
        warn(CFD_LOG_SOURCE, "pkh is not target.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. hash type cannot used pkh target.");
    }

    CfdCapiMultisigSignData* data =
        static_cast<CfdCapiMultisigSignData*>(multisign_handle);
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

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    OutPoint outpoint(Txid(txid), vout);
    Script script = Script(std::string(redeem_script));
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      tx.AddMultisigSign(outpoint, sign_list, script, addr_type);
      *tx_string = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      tx.AddMultisigSign(outpoint, sign_list, script, addr_type);
      *tx_string = CreateString(tx.GetHex());
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
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

int CfdFreeMultisigSignHandle(void* handle, void* multisign_handle) {
  try {
    cfd::Initialize();
    FreeBuffer(
        multisign_handle, kPrefixMultisigSignData,
        sizeof(CfdCapiMultisigSignData));
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
