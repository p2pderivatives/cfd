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
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfdapi_key.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_transaction.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

#ifndef CFD_DISABLE_ELEMENTS
using cfd::ConfidentialTransactionContext;
using cfd::ElementsAddressFactory;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ElementsConfidentialAddress;
#endif  // CFD_DISABLE_ELEMENTS

using cfd::SignParameter;
using cfd::TransactionContext;
using cfd::api::KeyApi;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::HashType;
using cfd::core::NetType;
using cfd::core::OutPoint;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::TxInReference;
using cfd::core::TxOutReference;
using cfd::core::WitnessVersion;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

/**
 * @brief get transaction information.
 * @param[in] tx                transaction.
 * @param[out] txid             transaction id.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] wtxid            witness transaction id.
 *   If no-witness transaction, return is txid.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] size             transaction size.
 * @param[out] vsize            virtual transaction size.
 * @param[out] weight           weight.
 * @param[out] version          transaction version.
 * @param[out] locktime         transaction locktime.
 */
void GetTxInfo(
    AbstractTransaction* tx, char** txid, char** wtxid, uint32_t* size,
    uint32_t* vsize, uint32_t* weight, uint32_t* version, uint32_t* locktime) {
  if (txid != nullptr) {
    *txid = CreateString(tx->GetTxid().GetHex());
  }
  if (wtxid != nullptr) {
    *wtxid = CreateString(Txid(tx->GetWitnessHash()).GetHex());
  }
  if (size != nullptr) {
    *size = tx->GetTotalSize();
  }
  if (vsize != nullptr) {
    *vsize = tx->GetVsize();
  }
  if (weight != nullptr) {
    *weight = tx->GetWeight();
  }
  if (version != nullptr) {
    *version = tx->GetVersion();
  }
  if (locktime != nullptr) {
    *locktime = tx->GetLockTime();
  }
}

/**
 * @brief Add data to transaction.
 * @param[in,out] tx      transaction context
 * @param[in] txin_list   tx input list
 * @param[in] txout_list  tx output list
 */
void AddTxData(
    TransactionContext* tx, const std::vector<CfdCapiTxInputData>& txin_list,
    const std::vector<CfdCapiTxOutputData>& txout_list) {
  for (const auto& txin : txin_list) {
    tx->AddTxIn(txin.txid, txin.vout, txin.sequence);
  }
  for (const auto& txout : txout_list) {
    tx->AddTxOut(Amount(txout.amount), Script(txout.locking_script));
  }
}

#ifndef CFD_DISABLE_ELEMENTS
/**
 * @brief Add data to confidential transaction.
 * @param[in,out] tx      transaction context
 * @param[in] txin_list   tx input list
 * @param[in] txout_list  tx output list
 */
void AddConfidentialTxData(
    ConfidentialTransactionContext* tx,
    const std::vector<CfdCapiTxInputData>& txin_list,
    const std::vector<CfdCapiTxOutputData>& txout_list) {
  for (const auto& txin : txin_list) {
    tx->AddTxIn(txin.txid, txin.vout, txin.sequence);
  }
  for (const auto& txout : txout_list) {
    tx->AddTxOut(
        Amount(txout.amount), txout.asset, Script(txout.locking_script),
        txout.nonce);
  }
}
#endif  // CFD_DISABLE_ELEMENTS

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
using cfd::capi::AddTxData;
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiCreateTransactionData;
using cfd::capi::CfdCapiMultisigSignData;
using cfd::capi::CfdCapiTxInputData;
using cfd::capi::CfdCapiTxOutputData;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::GetTxInfo;
using cfd::capi::IsElementsNetType;
using cfd::capi::IsEmptyString;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPrefixCreateTxData;
using cfd::capi::kPrefixMultisigSignData;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::kSignatureHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::capi::AddConfidentialTxData;
#endif  // CFD_DISABLE_ELEMENTS

// API
extern "C" {

int CfdInitializeTransaction(
    void* handle, int net_type, uint32_t version, uint32_t locktime,
    const char* tx_hex_string, void** create_handle) {
  CfdCapiCreateTransactionData* buffer = nullptr;
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (create_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "create handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. create handle is null.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    std::string base_tx;
    if (is_bitcoin) {
      if (IsEmptyString(tx_hex_string)) {
        TransactionContext tx(version, locktime);
        base_tx = tx.GetHex();
      } else {
        TransactionContext tx(tx_hex_string);
        base_tx = tx.GetHex();
      }
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      if (IsEmptyString(tx_hex_string)) {
        ConfidentialTransactionContext tx(version, locktime);
        base_tx = tx.GetHex();
      } else {
        ConfidentialTransactionContext tx(tx_hex_string);
        base_tx = tx.GetHex();
      }
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    buffer = static_cast<CfdCapiCreateTransactionData*>(AllocBuffer(
        kPrefixCreateTxData, sizeof(CfdCapiCreateTransactionData)));
    buffer->net_type = net_type;
    buffer->base_tx_hex = base_tx;
    buffer->txin_list = new std::vector<CfdCapiTxInputData>();
    buffer->txout_list = new std::vector<CfdCapiTxOutputData>();
    *create_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  CfdFreeTransactionHandle(handle, buffer);
  return result;
}

int CfdAddTransactionInput(
    void* handle, void* create_handle, const char* txid, uint32_t vout,
    uint32_t sequence) {
  try {
    cfd::Initialize();
    CheckBuffer(create_handle, kPrefixCreateTxData);

    CfdCapiCreateTransactionData* buffer =
        static_cast<CfdCapiCreateTransactionData*>(create_handle);
    if (buffer->txin_list == nullptr) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }

    CfdCapiTxInputData data = {Txid(txid), vout, sequence};
    buffer->txin_list->push_back(data);
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

int CfdAddTransactionOutput(
    void* handle, void* create_handle, int64_t value_satoshi,
    const char* address, const char* direct_locking_script,
    const char* asset_string) {
  try {
    cfd::Initialize();
    CheckBuffer(create_handle, kPrefixCreateTxData);

    CfdCapiCreateTransactionData* buffer =
        static_cast<CfdCapiCreateTransactionData*>(create_handle);
    if (buffer->txout_list == nullptr) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }

    CfdCapiTxOutputData data;
    bool is_elements = false;
#ifndef CFD_DISABLE_ELEMENTS
    is_elements = IsElementsNetType(buffer->net_type);
    if (is_elements) {
      if (IsEmptyString(asset_string)) {
        warn(CFD_LOG_SOURCE, "asset is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. asset is null or empty.");
      }
      data.asset = ConfidentialAssetId(asset_string);
    }
#endif  // CFD_DISABLE_ELEMENTS

    data.amount = value_satoshi;
    if (!IsEmptyString(address)) {
      Address addr;
      if (!is_elements) {
        addr = Address(address);
      } else {
#ifndef CFD_DISABLE_ELEMENTS
        ElementsAddressFactory address_factory;
        // const std::string addr(address);
        if (ElementsConfidentialAddress::IsConfidentialAddress(address)) {
          ElementsConfidentialAddress confidential_addr(address);
          addr = confidential_addr.GetUnblindedAddress();
          data.nonce =
              ConfidentialNonce(confidential_addr.GetConfidentialKey());
        } else {
          addr = address_factory.GetAddress(address);
        }
#endif  // CFD_DISABLE_ELEMENTS
      }
      data.locking_script = addr.GetLockingScript().GetData();
    } else if (!IsEmptyString(direct_locking_script)) {
      Script script(direct_locking_script);
      data.locking_script = script.GetData();
    }

    buffer->txout_list->push_back(data);
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

int CfdFinalizeTransaction(
    void* handle, void* create_handle, char** tx_hex_string) {
  try {
    cfd::Initialize();
    CheckBuffer(create_handle, kPrefixCreateTxData);

    CfdCapiCreateTransactionData* buffer =
        static_cast<CfdCapiCreateTransactionData*>(create_handle);
    if ((buffer->txin_list == nullptr) || (buffer->txout_list == nullptr)) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }
    if (tx_hex_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx_hex_string is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx_hex_string is null.");
    }

    bool is_bitcoin = false;
    ConvertNetType(buffer->net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(buffer->base_tx_hex);
      AddTxData(&tx, *buffer->txin_list, *buffer->txout_list);
      *tx_hex_string = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(buffer->base_tx_hex);
      AddConfidentialTxData(&tx, *buffer->txin_list, *buffer->txout_list);
      *tx_hex_string = CreateString(tx.GetHex());
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

int CfdFreeTransactionHandle(void* handle, void* create_handle) {
  try {
    cfd::Initialize();
    if (create_handle != nullptr) {
      CfdCapiCreateTransactionData* create_tx_struct =
          static_cast<CfdCapiCreateTransactionData*>(create_handle);
      if (create_tx_struct->txin_list != nullptr) {
        delete create_tx_struct->txin_list;
        create_tx_struct->txin_list = nullptr;
      }
      if (create_tx_struct->txout_list != nullptr) {
        delete create_tx_struct->txout_list;
        create_tx_struct->txout_list = nullptr;
      }
    }
    FreeBuffer(
        create_handle, kPrefixCreateTxData,
        sizeof(CfdCapiCreateTransactionData));
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
        (IsEmptyString(sign_data_hex) && (use_der_encode || !is_witness))) {
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

int CfdAddSignWithPrivkeySimple(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* pubkey, const char* privkey,
    int64_t value_satoshi, int sighash_type, bool sighash_anyone_can_pay,
    bool has_grind_r, char** tx_string) {
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
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    AddressType addr_type = ConvertHashToAddressType(hash_type);
    Amount value = Amount(value_satoshi);

    Privkey privkey_obj;
    std::string privkey_str(privkey);
    if (privkey_str.length() == (Privkey::kPrivkeySize * 2)) {
      privkey_obj = Privkey(privkey_str);
    } else {
      KeyApi keyapi;
      privkey_obj = keyapi.GetPrivkeyFromWif(privkey_str);
    }

    OutPoint outpoint(Txid(txid), vout);
    SigHashType sighashtype(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      tx.SignWithPrivkeySimple(
          outpoint, Pubkey(pubkey), privkey_obj, sighashtype, value, addr_type,
          has_grind_r);
      *tx_string = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      tx.SignWithPrivkeySimple(
          outpoint, Pubkey(pubkey), privkey_obj, sighashtype, value, addr_type,
          has_grind_r);
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

int CfdCreateSighash(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* pubkey,
    const char* redeem_script, int64_t value_satoshi, int sighash_type,
    bool sighash_anyone_can_pay, char** sighash) {
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
    if (sighash == nullptr) {
      warn(CFD_LOG_SOURCE, "sighash is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sighash is null.");
    }

    AddressType addr_type = ConvertHashToAddressType(hash_type);
    HashType core_hash_type;
    WitnessVersion version = WitnessVersion::kVersionNone;
    switch (addr_type) {
      case AddressType::kP2shAddress:
        core_hash_type = HashType::kP2sh;
        break;
      case AddressType::kP2wshAddress:
        // fall-through
      case AddressType::kP2shP2wshAddress:
        core_hash_type = HashType::kP2wsh;
        version = WitnessVersion::kVersion0;
        break;
      case AddressType::kP2pkhAddress:
        core_hash_type = HashType::kP2pkh;
        break;
      case AddressType::kP2wpkhAddress:
        // fall-through
      case AddressType::kP2shP2wpkhAddress:
        core_hash_type = HashType::kP2wpkh;
        version = WitnessVersion::kVersion0;
        break;
      default:
        warn(CFD_LOG_SOURCE, "hashtype is illegal.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. hashtype is illegal.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    Amount value(value_satoshi);
    ByteData sighash_bytes;
    SigHashType sighashtype(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
    if ((core_hash_type == HashType::kP2sh) ||
        (core_hash_type == HashType::kP2wsh)) {
      if (IsEmptyString(redeem_script)) {
        warn(CFD_LOG_SOURCE, "redeem script is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. redeem script is null or empty.");
      }
      if (is_bitcoin) {
        TransactionContext tx(tx_hex_string);
        sighash_bytes = tx.CreateSignatureHash(
            OutPoint(Txid(std::string(txid)), vout), Script(redeem_script),
            sighashtype, value, version);
      } else {
#ifndef CFD_DISABLE_ELEMENTS
        ConfidentialTransactionContext tx(tx_hex_string);
        sighash_bytes = tx.CreateSignatureHash(
            OutPoint(Txid(std::string(txid)), vout), Script(redeem_script),
            sighashtype, value, version);
#else
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
      }
    } else {
      if (IsEmptyString(pubkey)) {
        warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. pubkey is null or empty.");
      }
      if (is_bitcoin) {
        TransactionContext tx(tx_hex_string);
        sighash_bytes = tx.CreateSignatureHash(
            OutPoint(Txid(std::string(txid)), vout), Pubkey(pubkey),
            sighashtype, value, version);
      } else {
#ifndef CFD_DISABLE_ELEMENTS
        ConfidentialTransactionContext tx(tx_hex_string);
        sighash_bytes = tx.CreateSignatureHash(
            OutPoint(Txid(std::string(txid)), vout), Pubkey(pubkey),
            sighashtype, value, version);
#else
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
      }
    }

    *sighash = CreateString(sighash_bytes.GetHex());
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

int CfdGetTxInfo(
    void* handle, int net_type, const char* tx_hex_string, char** txid,
    char** wtxid, uint32_t* size, uint32_t* vsize, uint32_t* weight,
    uint32_t* version, uint32_t* locktime) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_txid = nullptr;
  char* work_wtxid = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      GetTxInfo(
          &tx, &work_txid, &work_wtxid, size, vsize, weight, version,
          locktime);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      GetTxInfo(
          &tx, &work_txid, &work_wtxid, size, vsize, weight, version,
          locktime);
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (work_txid != nullptr) *txid = work_txid;
    if (work_wtxid != nullptr) *wtxid = work_wtxid;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_txid, &work_wtxid);
  return result;
}

int CfdGetTxIn(
    void* handle, int net_type, const char* tx_hex_string, uint32_t index,
    char** txid, uint32_t* vout, uint32_t* sequence, char** script_sig) {
  char* work_txid = nullptr;
  char* work_script_sig = nullptr;
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    Txid temp_txid;
    Script temp_unlocking_script;
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      const TxInReference ref = tx.GetTxIn(index);
      temp_txid = ref.GetTxid();
      temp_unlocking_script = ref.GetUnlockingScript();
      if (vout != nullptr) *vout = ref.GetVout();
      if (sequence != nullptr) *sequence = ref.GetSequence();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      const ConfidentialTxInReference ref = tx.GetTxIn(index);
      temp_txid = ref.GetTxid();
      temp_unlocking_script = ref.GetUnlockingScript();
      if (vout != nullptr) *vout = ref.GetVout();
      if (sequence != nullptr) *sequence = ref.GetSequence();
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (txid != nullptr) {
      work_txid = CreateString(temp_txid.GetHex());
    }
    if (script_sig != nullptr) {
      work_script_sig = CreateString(temp_unlocking_script.GetHex());
    }

    if (work_txid != nullptr) *txid = work_txid;
    if (work_script_sig != nullptr) *script_sig = work_script_sig;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_txid, &work_script_sig);
  return result;
}

int CfdGetTxInWitness(
    void* handle, int net_type, const char* tx_hex_string, uint32_t txin_index,
    uint32_t stack_index, char** stack_data) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (stack_data == nullptr) {
      warn(CFD_LOG_SOURCE, "stack data is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. stack data is null.");
    }

    std::vector<ByteData> witness_stack;
    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      const TxInReference ref = tx.GetTxIn(txin_index);
      witness_stack = ref.GetScriptWitness().GetWitness();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      const ConfidentialTxInReference ref = tx.GetTxIn(txin_index);
      witness_stack = ref.GetScriptWitness().GetWitness();
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (witness_stack.size() <= stack_index) {
      warn(CFD_LOG_SOURCE, "stackIndex is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. stackIndex out of witness stack.");
    }
    *stack_data = CreateString(witness_stack[stack_index].GetHex());

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

int CfdGetTxOut(
    void* handle, int net_type, const char* tx_hex_string, uint32_t index,
    int64_t* value_satoshi, char** locking_script) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    Amount temp_value;
    Script temp_locking_script;
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      const TxOutReference ref = tx.GetTxOut(index);
      temp_value = ref.GetValue();
      temp_locking_script = ref.GetLockingScript();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      const ConfidentialTxOutReference ref = tx.GetTxOut(index);
      temp_value = ref.GetValue();
      temp_locking_script = ref.GetLockingScript();
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (value_satoshi != nullptr) {
      *value_satoshi = temp_value.GetSatoshiValue();
    }
    if (locking_script != nullptr) {
      *locking_script = CreateString(temp_locking_script.GetHex());
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

int CfdGetTxInCount(
    void* handle, int net_type, const char* tx_hex_string, uint32_t* count) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      if (count != nullptr) *count = tx.GetTxInCount();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      if (count != nullptr) *count = tx.GetTxInCount();
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

int CfdGetTxInWitnessCount(
    void* handle, int net_type, const char* tx_hex_string, uint32_t txin_index,
    uint32_t* count) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      const TxInReference ref = tx.GetTxIn(txin_index);
      if (count != nullptr) *count = ref.GetScriptWitnessStackNum();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      const ConfidentialTxInReference ref = tx.GetTxIn(txin_index);
      if (count != nullptr) *count = ref.GetScriptWitnessStackNum();
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

int CfdGetTxOutCount(
    void* handle, int net_type, const char* tx_hex_string, uint32_t* count) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      if (count != nullptr) *count = tx.GetTxOutCount();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      if (count != nullptr) *count = tx.GetTxOutCount();
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

int CfdGetTxInIndex(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, uint32_t* index) {
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

    OutPoint outpoint(Txid(txid), vout);
    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      if (index != nullptr) *index = tx.GetTxInIndex(outpoint);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      if (index != nullptr) *index = tx.GetTxInIndex(outpoint);
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

int CfdGetTxOutIndex(
    void* handle, int net_type, const char* tx_hex_string, const char* address,
    const char* direct_locking_script, uint32_t* index) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }

    bool is_find = false;
    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      if (!IsEmptyString(direct_locking_script)) {
        is_find = tx.IsFindTxOut(Script(direct_locking_script), index);
      } else if (!IsEmptyString(address)) {
        Address addr(address);
        is_find = tx.IsFindTxOut(addr, index);
      } else {
        // do nothing
      }
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      if (!IsEmptyString(direct_locking_script)) {
        is_find = tx.IsFindTxOut(Script(direct_locking_script), index);
      } else if (!IsEmptyString(address)) {
        ElementsAddressFactory address_factory;
        std::string addr_str(address);
        Address addr;
        if (ElementsConfidentialAddress::IsConfidentialAddress(addr_str)) {
          addr = address_factory.GetConfidentialAddress(addr_str)
                     .GetUnblindedAddress();
        } else {
          addr = address_factory.GetAddress(addr_str);
        }
        is_find = tx.IsFindTxOut(addr, index);
      } else {
        // fee
        is_find = tx.IsFindFeeTxOut(index);
      }
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (!is_find) {
      warn(CFD_LOG_SOURCE, "target not found.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. search target is not found.");
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

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
