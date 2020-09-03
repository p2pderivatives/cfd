// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_transaction.cpp
 *
 * @brief cfd-capiで利用するTransaction処理の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include "cfdc/cfdcapi_transaction.h"

#include <exception>
#include <map>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfd/cfdapi_key.h"
#include "cfd/cfdapi_transaction.h"
#include "cfdc/cfdcapi_common.h"
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
using cfd::ConfidentialTransactionController;
using cfd::ElementsAddressFactory;
using cfd::api::ElementsTransactionApi;
using cfd::api::ElementsUtxoAndOption;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ElementsConfidentialAddress;
#endif  // CFD_DISABLE_ELEMENTS

using cfd::AddressFactory;
using cfd::AmountMap;
using cfd::CoinSelectionOption;
using cfd::SignParameter;
using cfd::TransactionContext;
using cfd::TransactionController;
using cfd::Utxo;
using cfd::UtxoData;
using cfd::UtxoFilter;
using cfd::api::KeyApi;
using cfd::api::TransactionApi;
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

//! prefix: data for fee estimation
constexpr const char* const kPrefixFundRawTxData = "FundRawTxData";

/**
 * @brief cfd-capi FundTargetAmount構造体.
 */
struct CfdCapiFundTargetAmount {
  // coinselection option
  std::string asset;             //!< target asset
  int64_t amount;                //!< target amount
  std::string reserved_address;  //!< reserved address
};

/**
 * @brief cfd-capi CfdCapiFundRawTxData構造体.
 * @details 最大情報量が多すぎるため、flat structにはしない。
 */
struct CfdCapiFundRawTxData {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! network type
  NetType net_type;
  //! fee asset
  uint8_t fee_asset[kAssetSize];
  //! elements flag
  bool is_elements;
  //! utxo list
  std::vector<UtxoData>* utxos;
  //! input utxo list
  std::vector<UtxoData>* input_utxos;
#ifndef CFD_DISABLE_ELEMENTS
  //! input utxo list for elements
  std::vector<ElementsUtxoAndOption>* input_elements_utxos;
#endif  // CFD_DISABLE_ELEMENTS
  //! txout's append address list.
  std::vector<std::string>* append_txout_addresses;
  //! target list
  std::vector<CfdCapiFundTargetAmount>* targets;  //!< target list
  /// use blind fee (bool)
  bool is_blind;
  /// dust fee rate (double)
  double dust_fee_rate;
  /// longterm fee rate (double)
  double long_term_fee_rate;
  /// knapsack min change (int64)
  int64_t knapsack_min_change;
  /// blind exponent (int64)
  int exponent;
  /// blind minimum bits (int64)
  int minimum_bits;
};

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

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiFundRawTxData;
using cfd::capi::CfdCapiFundTargetAmount;
using cfd::capi::CfdCapiMultisigSignData;
using cfd::capi::CfdCapiTransactionData;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertAddressType;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::GetTxInfo;
using cfd::capi::GetWitnessVersion;
using cfd::capi::IsElementsNetType;
using cfd::capi::IsEmptyString;
using cfd::capi::kAssetSize;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPrefixFundRawTxData;
using cfd::capi::kPrefixMultisigSignData;
using cfd::capi::kPrefixTransactionData;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::kSignatureHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

// API
extern "C" {

int CfdInitializeTransaction(
    void* handle, int net_type, uint32_t version, uint32_t locktime,
    const char* tx_hex_string, void** create_handle) {
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

    return CfdInitializeTxDataHandle(
        handle, net_type, base_tx.c_str(), create_handle);
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdAddTransactionInput(
    void* handle, void* create_handle, const char* txid, uint32_t vout,
    uint32_t sequence) {
  try {
    cfd::Initialize();
    CheckBuffer(create_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(create_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      tx->AddTxIn(Txid(std::string(txid)), vout, sequence);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      tx->AddTxIn(Txid(std::string(txid)), vout, sequence);
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

int CfdAddTransactionOutput(
    void* handle, void* create_handle, int64_t value_satoshi,
    const char* address, const char* direct_locking_script,
    const char* asset_string) {
  try {
    cfd::Initialize();
    CheckBuffer(create_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(create_handle);

    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    Amount amount = Amount(value_satoshi);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      if (!IsEmptyString(address)) {
        tx->AddTxOut(Address(address), amount);
      } else if (!IsEmptyString(direct_locking_script)) {
        tx->AddTxOut(amount, Script(direct_locking_script));
      }
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      if (IsEmptyString(asset_string)) {
        warn(CFD_LOG_SOURCE, "asset is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. asset is null or empty.");
      }
      ConfidentialAssetId asset = ConfidentialAssetId(asset_string);

      if (!IsEmptyString(address)) {
        ElementsAddressFactory address_factory;
        if (ElementsConfidentialAddress::IsConfidentialAddress(address)) {
          ElementsConfidentialAddress confidential_addr(address);
          tx->AddTxOut(confidential_addr, amount, asset, false);
        } else {
          tx->AddTxOut(address_factory.GetAddress(address), amount, asset);
        }
      } else if (!IsEmptyString(direct_locking_script)) {
        ConfidentialNonce nonce;
        tx->AddTxOut(amount, asset, Script(direct_locking_script), nonce);
      } else {
        tx->UpdateFeeAmount(amount, asset);
      }
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

int CfdFinalizeTransaction(
    void* handle, void* create_handle, char** tx_hex_string) {
  return CfdGetModifiedTxByHandle(handle, create_handle, tx_hex_string);
}

int CfdFreeTransactionHandle(void* handle, void* create_handle) {
  return CfdFreeTxDataHandle(handle, create_handle);
}

int CfdUpdateTxOutAmount(
    void* handle, int net_type, const char* tx_hex_string, uint32_t index,
    int64_t amount, char** tx_string) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex_string);
      tx.SetTxOutValue(index, Amount(amount));
      *tx_string = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex_string);
      tx.SetTxOutValue(index, Amount(amount));
      *tx_string = CreateString(tx.GetHex());
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
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

int CfdVerifySignature(
    void* handle, int net_type, const char* tx_hex, const char* signature,
    int hash_type, const char* pubkey, const char* script, const char* txid,
    uint32_t vout, int sighash_type, bool sighash_anyone_can_pay,
    int64_t value_satoshi, const char* value_bytedata) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex)) {
      warn(CFD_LOG_SOURCE, "tx is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null or empty.");
    }
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }

    WitnessVersion version = GetWitnessVersion(hash_type);
    Amount amount = Amount(value_satoshi);
    OutPoint outpoint(Txid(txid), vout);
    SigHashType sighashtype(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
    ByteData signature_obj(signature);

    bool is_verify = false;
    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (is_bitcoin) {
      TransactionContext tx(tx_hex);
      if (!IsEmptyString(script)) {
        is_verify = tx.VerifyInputSignature(
            signature_obj, Pubkey(pubkey), outpoint, Script(script),
            sighashtype, amount, version);
      } else if (!IsEmptyString(pubkey)) {
        is_verify = tx.VerifyInputSignature(
            signature_obj, Pubkey(pubkey), outpoint, sighashtype, amount,
            version);
      }
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialValue value;
      if (!IsEmptyString(value_bytedata)) {
        value = ConfidentialValue(value_bytedata);
      } else {
        value = ConfidentialValue(amount);
      }
      ConfidentialTransactionContext tx(tx_hex);
      if (!IsEmptyString(script)) {
        is_verify = tx.VerifyInputSignature(
            signature_obj, Pubkey(pubkey), outpoint, Script(script),
            sighashtype, value, version);
      } else if (!IsEmptyString(pubkey)) {
        is_verify = tx.VerifyInputSignature(
            signature_obj, Pubkey(pubkey), outpoint, sighashtype, value,
            version);
      }
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

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

int CfdVerifyTxSign(
    void* handle, int net_type, const char* tx_hex, const char* txid,
    uint32_t vout, const char* address, int address_type,
    const char* direct_locking_script, int64_t value_satoshi,
    const char* value_bytedata) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex)) {
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

    AddressType addr_type = ConvertAddressType(address_type);
    Amount amount = Amount(value_satoshi);
    OutPoint outpoint(Txid(txid), vout);

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);

    UtxoData utxo;
    utxo.block_height = 0;
    utxo.binary_data = nullptr;
    utxo.descriptor = "";
    utxo.txid = outpoint.GetTxid();
    utxo.vout = outpoint.GetVout();
    utxo.address_type = addr_type;
    utxo.amount = amount;
    if (!IsEmptyString(address)) {
      std::string addr(address);
      if (is_bitcoin) {
        AddressFactory address_factory;
        utxo.address = address_factory.GetAddress(addr);
      } else {
#ifndef CFD_DISABLE_ELEMENTS
        ElementsAddressFactory address_factory;
        if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
          ElementsConfidentialAddress confidential_addr(addr);
          utxo.address = confidential_addr.GetUnblindedAddress();
        } else {
          utxo.address = address_factory.GetAddress(addr);
        }
#else
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
      }
      utxo.locking_script = utxo.address.GetLockingScript();
    } else if (!IsEmptyString(direct_locking_script)) {
      utxo.locking_script = Script(direct_locking_script);
    }
    std::vector<UtxoData> utxos = {utxo};

    if (is_bitcoin) {
      TransactionContext tx(tx_hex);
      tx.GetTxInIndex(outpoint);
      tx.CollectInputUtxo(utxos);
      result = CfdErrorCode::kCfdSignVerificationError;
      tx.Verify(outpoint);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext tx(tx_hex);
      if (!IsEmptyString(value_bytedata)) {
        utxo.value_commitment = ConfidentialValue(value_bytedata);
        utxos[0] = utxo;
      }
      tx.GetTxInIndex(outpoint);
      tx.CollectInputUtxo(utxos);
      result = CfdErrorCode::kCfdSignVerificationError;
      tx.Verify(outpoint);
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    if (result != CfdErrorCode::kCfdSignVerificationError) {
      result = SetLastError(handle, except);
    } else {
      SetLastError(handle, except);  // collect error message
    }
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
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

int CfdInitializeFundRawTx(
    void* handle, int network_type, uint32_t target_asset_count,
    const char* fee_asset, void** fund_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiFundRawTxData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (fund_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "fund handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. fund handle is null.");
    }
    if (target_asset_count == 0) {
      warn(CFD_LOG_SOURCE, "target_asset_count is zero.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. target_asset_count is null.");
    }
    CfdCapiFundRawTxData obj;
    memset(&obj, 0, sizeof(obj));
    bool is_bitcoin = false;
    obj.net_type = ConvertNetType(network_type, &is_bitcoin);
    if (!IsEmptyString(fee_asset)) {
#ifndef CFD_DISABLE_ELEMENTS
      std::string asset_hex(fee_asset);
      if (!asset_hex.empty()) {
        ConfidentialAssetId asset_data(asset_hex);
        memcpy(
            obj.fee_asset, asset_data.GetData().GetBytes().data(),
            sizeof(obj.fee_asset));
        uint8_t empty_asset[kAssetSize];
        memset(empty_asset, 0, sizeof(empty_asset));
        if (memcmp(obj.fee_asset, empty_asset, sizeof(empty_asset)) != 0) {
          obj.is_elements = true;
          if (is_bitcoin) {
            warn(CFD_LOG_SOURCE, "network type is not elements.");
            throw CfdException(
                CfdError::kCfdIllegalArgumentError,
                "Failed to parameter. network type is not elements.");
          }
        }
      }
#else
      info(CFD_LOG_SOURCE, "unuse asset[{}]", fee_asset);
#endif  // CFD_DISABLE_ELEMENTS
    }
    if (obj.is_elements == is_bitcoin) {
      warn(CFD_LOG_SOURCE, "network type not is bitcoin.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. network type is not bitcoin.");
    }
    obj.is_blind = false;
    obj.long_term_fee_rate = (obj.is_elements) ? 0.15 : 20.0;
    obj.dust_fee_rate = 3.0;
    obj.knapsack_min_change = -1;
    obj.exponent = 0;
    obj.minimum_bits = cfd::capi::kMinimumBits;  // = 36(old)

    buffer = static_cast<CfdCapiFundRawTxData*>(
        AllocBuffer(kPrefixFundRawTxData, sizeof(CfdCapiFundRawTxData)));
    memcpy(obj.prefix, buffer->prefix, sizeof(obj.prefix));
    *buffer = obj;
    buffer->utxos = new std::vector<UtxoData>();
    buffer->input_utxos = new std::vector<UtxoData>();
#ifndef CFD_DISABLE_ELEMENTS
    buffer->input_elements_utxos = new std::vector<ElementsUtxoAndOption>();
#endif  // CFD_DISABLE_ELEMENTS
    buffer->targets =
        new std::vector<CfdCapiFundTargetAmount>(target_asset_count);
    buffer->append_txout_addresses = new std::vector<std::string>();
    *fund_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  if (buffer != nullptr) CfdFreeFundRawTxHandle(handle, buffer);
  return result;
}

int CfdAddTxInForFundRawTx(
    void* handle, void* fund_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* descriptor, const char* asset,
    bool is_issuance, bool is_blind_issuance, bool is_pegin,
    uint32_t pegin_btc_tx_size, const char* fedpeg_script) {
  return CfdAddTxInTemplateForFundRawTx(
      handle, fund_handle, txid, vout, amount, descriptor, asset, is_issuance,
      is_blind_issuance, is_pegin, pegin_btc_tx_size, fedpeg_script, nullptr);
}

int CfdAddTxInTemplateForFundRawTx(
    void* handle, void* fund_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* descriptor, const char* asset,
    bool is_issuance, bool is_blind_issuance, bool is_pegin,
    uint32_t pegin_btc_tx_size, const char* fedpeg_script,
    const char* scriptsig_template) {
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixFundRawTxData);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (IsEmptyString(descriptor)) {
      warn(CFD_LOG_SOURCE, "descriptor is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. descriptor is null or empty.");
    }
    CfdCapiFundRawTxData* buffer =
        static_cast<CfdCapiFundRawTxData*>(fund_handle);

    UtxoData utxo;
    utxo.txid = Txid(txid);
    utxo.vout = vout;
    utxo.amount = Amount(amount);
    utxo.descriptor = std::string(descriptor);
    if (!IsEmptyString(scriptsig_template)) {
      utxo.scriptsig_template = Script(std::string(scriptsig_template));
    }
    if (buffer->is_elements) {
#ifndef CFD_DISABLE_ELEMENTS
      if (IsEmptyString(asset)) {
        warn(CFD_LOG_SOURCE, "utxo asset is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. utxo asset is null or empty.");
      }
      utxo.asset = ConfidentialAssetId(asset);

      ElementsUtxoAndOption param;
      param.utxo = utxo;
      param.is_issuance = is_issuance;
      param.is_blind_issuance = is_blind_issuance;
      param.is_pegin = is_pegin;
      param.pegin_btc_tx_size = pegin_btc_tx_size;
      if (!IsEmptyString(fedpeg_script)) {
        param.fedpeg_script = Script(fedpeg_script);
      }
      buffer->input_elements_utxos->push_back(param);
#else
      info(
          CFD_LOG_SOURCE,
          "unuse parameters: [is_issuance={}, is_blind_issuance={}, "
          "is_pegin={}, pegin_btc_tx_size={}, fedpeg_script={}]",
          is_issuance, is_blind_issuance, is_pegin, pegin_btc_tx_size,
          fedpeg_script);
#endif  // CFD_DISABLE_ELEMENTS
    } else {
      buffer->input_utxos->push_back(utxo);
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

int CfdAddUtxoForFundRawTx(
    void* handle, void* fund_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* descriptor, const char* asset) {
  return CfdAddUtxoTemplateForFundRawTx(
      handle, fund_handle, txid, vout, amount, descriptor, asset, nullptr);
}

int CfdAddUtxoTemplateForFundRawTx(
    void* handle, void* fund_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* descriptor, const char* asset,
    const char* scriptsig_template) {
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixFundRawTxData);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (IsEmptyString(descriptor)) {
      warn(CFD_LOG_SOURCE, "descriptor is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. descriptor is null or empty.");
    }
    CfdCapiFundRawTxData* buffer =
        static_cast<CfdCapiFundRawTxData*>(fund_handle);

    UtxoData utxo;
    utxo.txid = Txid(txid);
    utxo.vout = vout;
    utxo.amount = Amount(amount);
    utxo.descriptor = std::string(descriptor);
    utxo.address_type = AddressType::kP2shAddress;  // force init
    if (!IsEmptyString(scriptsig_template)) {
      utxo.scriptsig_template = Script(std::string(scriptsig_template));
    }
    if (buffer->is_elements) {
#ifndef CFD_DISABLE_ELEMENTS
      if (IsEmptyString(asset)) {
        warn(CFD_LOG_SOURCE, "utxo asset is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. utxo asset is null or empty.");
      }
      utxo.asset = ConfidentialAssetId(asset);
#endif  // CFD_DISABLE_ELEMENTS
    }
    buffer->utxos->push_back(utxo);
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

int CfdAddTargetAmountForFundRawTx(
    void* handle, void* fund_handle, uint32_t asset_index, int64_t amount,
    const char* asset, const char* reserved_address) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixFundRawTxData);
    if (IsEmptyString(reserved_address)) {
      warn(CFD_LOG_SOURCE, "reserved_address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. reserved_address is null.");
    }

    CfdCapiFundRawTxData* buffer =
        static_cast<CfdCapiFundRawTxData*>(fund_handle);
    if ((buffer->targets == nullptr) ||
        (asset_index >= buffer->targets->size())) {
      warn(CFD_LOG_SOURCE, "target amounts is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. target amounts is maximum over.");
    }

    CfdCapiFundTargetAmount object;
    object.amount = amount;
    object.reserved_address = reserved_address;
    if (!IsEmptyString(asset)) {
#ifndef CFD_DISABLE_ELEMENTS
      object.asset = std::string(asset);
#endif  // CFD_DISABLE_ELEMENTS
    }

    (*buffer->targets)[asset_index] = object;
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

int CfdSetOptionFundRawTx(
    void* handle, void* fund_handle, int key, int64_t int64_value,
    double double_value, bool bool_value) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixFundRawTxData);
    CfdCapiFundRawTxData* buffer =
        static_cast<CfdCapiFundRawTxData*>(fund_handle);

    switch (key) {
      case kCfdFundTxIsBlind:
        buffer->is_blind = bool_value;
        break;
      case kCfdFundTxDustFeeRate:
        buffer->dust_fee_rate = double_value;
        break;
      case kCfdFundTxLongTermFeeRate:
        buffer->long_term_fee_rate = double_value;
        break;
      case kCfdFundTxKnapsackMinChange:
        buffer->knapsack_min_change = int64_value;
        break;
      case kCfdFundTxBlindExponent:
        buffer->exponent = static_cast<int>(int64_value);
        break;
      case kCfdFundTxBlindMinimumBits:
        if (int64_value >= 0) {
          buffer->minimum_bits = static_cast<int>(int64_value);
        }
        break;
      default:
        warn(CFD_LOG_SOURCE, "illegal key {}.", key);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. key is illegal.");
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

int CfdFinalizeFundRawTx(
    void* handle, void* fund_handle, const char* tx_hex,
    double effective_fee_rate, int64_t* tx_fee, uint32_t* append_txout_count,
    char** output_tx_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
#ifndef CFD_DISABLE_ELEMENTS
  auto convert_to_asset = [](const uint8_t* asset) -> ConfidentialAssetId {
    std::vector<uint8_t> bytes(33);
    memcpy(bytes.data(), asset, bytes.size());
    return ConfidentialAssetId(ByteData(bytes));
  };
#endif  // CFD_DISABLE_ELEMENTS

  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixFundRawTxData);
    if (IsEmptyString(tx_hex)) {
      warn(CFD_LOG_SOURCE, "tx_hex is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx_hex is null or empty.");
    }
    if (output_tx_hex == nullptr) {
      warn(CFD_LOG_SOURCE, "output_tx_hex is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output_tx_hex is null.");
    }
    CfdCapiFundRawTxData* buffer =
        static_cast<CfdCapiFundRawTxData*>(fund_handle);

    CoinSelectionOption option_params;
    option_params.SetEffectiveFeeBaserate(effective_fee_rate);
    option_params.SetLongTermFeeBaserate(buffer->long_term_fee_rate);
    option_params.SetDustFeeRate(buffer->dust_fee_rate);
    option_params.SetKnapsackMinimumChange(buffer->knapsack_min_change);
#ifndef CFD_DISABLE_ELEMENTS
    option_params.SetBlindInfo(buffer->exponent, buffer->minimum_bits);
#endif  // CFD_DISABLE_ELEMENTS

    Amount utxo_fee_value;
    UtxoFilter filter;
    Amount tx_fee_value;
    std::vector<Utxo> utxo_list;
    if (buffer->is_elements) {
#ifndef CFD_DISABLE_ELEMENTS
      // elements
      option_params.InitializeConfidentialTxSizeInfo();
      ConfidentialAssetId fee_asset = convert_to_asset(buffer->fee_asset);
      option_params.SetFeeAsset(fee_asset);

      AmountMap map_target_value;
      std::map<std::string, std::string> reserve_txout_address;
      for (const auto& target : *(buffer->targets)) {
        map_target_value.emplace(
            target.asset, Amount::CreateBySatoshiAmount(target.amount));
        reserve_txout_address.emplace(target.asset, target.reserved_address);
      }

      ElementsTransactionApi api;
      ConfidentialTransactionController ctxc = api.FundRawTransaction(
          tx_hex, *buffer->utxos, map_target_value,
          *buffer->input_elements_utxos, reserve_txout_address, fee_asset,
          buffer->is_blind, effective_fee_rate, &tx_fee_value, &filter,
          &option_params, buffer->append_txout_addresses, buffer->net_type);
      if (output_tx_hex != nullptr) {
        *output_tx_hex = CreateString(ctxc.GetHex());
      }
#endif  // CFD_DISABLE_ELEMENTS
    } else {
      // bitcoin
      option_params.InitializeTxSizeInfo();
      auto& target = buffer->targets->at(0);
      Amount target_value(target.amount);

      TransactionApi api;
      TransactionController txc = api.FundRawTransaction(
          tx_hex, *buffer->utxos, target_value, *buffer->input_utxos,
          target.reserved_address, effective_fee_rate, &tx_fee_value, &filter,
          &option_params, buffer->append_txout_addresses, buffer->net_type);
      if (output_tx_hex != nullptr) {
        *output_tx_hex = CreateString(txc.GetHex());
      }
    }

    if (tx_fee != nullptr) {
      *tx_fee = tx_fee_value.GetSatoshiValue();
    }
    if (append_txout_count != nullptr) {
      *append_txout_count =
          static_cast<uint32_t>(buffer->append_txout_addresses->size());
    }

    result = CfdErrorCode::kCfdSuccess;
    return result;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdGetAppendTxOutFundRawTx(
    void* handle, void* fund_handle, uint32_t index, char** append_address) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixFundRawTxData);
    if (append_address == nullptr) {
      warn(CFD_LOG_SOURCE, "append_address is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. append_address is null.");
    }

    CfdCapiFundRawTxData* buffer =
        static_cast<CfdCapiFundRawTxData*>(fund_handle);
    if ((buffer->append_txout_addresses == nullptr) ||
        (index >= buffer->append_txout_addresses->size())) {
      warn(CFD_LOG_SOURCE, "target addresses is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. target addresses is maximum over.");
    }

    *append_address = CreateString(buffer->append_txout_addresses->at(index));
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

int CfdFreeFundRawTxHandle(void* handle, void* fund_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (fund_handle != nullptr) {
      CheckBuffer(fund_handle, kPrefixFundRawTxData);
      CfdCapiFundRawTxData* fund_struct =
          static_cast<CfdCapiFundRawTxData*>(fund_handle);
      if (fund_struct->utxos != nullptr) {
        delete fund_struct->utxos;
        fund_struct->utxos = nullptr;
      }
      if (fund_struct->input_utxos != nullptr) {
        delete fund_struct->input_utxos;
        fund_struct->input_utxos = nullptr;
      }
#ifndef CFD_DISABLE_ELEMENTS
      if (fund_struct->input_elements_utxos != nullptr) {
        delete fund_struct->input_elements_utxos;
        fund_struct->input_elements_utxos = nullptr;
      }
#endif  // CFD_DISABLE_ELEMENTS
      if (fund_struct->append_txout_addresses != nullptr) {
        delete fund_struct->append_txout_addresses;
        fund_struct->append_txout_addresses = nullptr;
      }
      if (fund_struct->targets != nullptr) {
        delete fund_struct->targets;
        fund_struct->targets = nullptr;
      }
    }
    FreeBuffer(
        fund_handle, kPrefixFundRawTxData, sizeof(CfdCapiFundRawTxData));
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

int CfdInitializeTxDataHandle(
    void* handle, int net_type, const char* tx_hex_string,
    void** tx_data_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiTransactionData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (tx_data_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "tx data handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx data handle is null.");
    }
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx_hex is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx_hex is null or empty.");
    }

    bool is_bitcoin = false;
    ConvertNetType(net_type, &is_bitcoin);
    if (!is_bitcoin) {
#ifndef CFD_DISABLE_ELEMENTS
      info(CFD_LOG_SOURCE, "net_type[{}]", net_type);
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    buffer = static_cast<CfdCapiTransactionData*>(
        AllocBuffer(kPrefixTransactionData, sizeof(CfdCapiTransactionData)));
    buffer->net_type = net_type;
    if (is_bitcoin) {
      TransactionContext* tx =
          new TransactionContext(std::string(tx_hex_string));
      buffer->tx_obj = tx;
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          new ConfidentialTransactionContext(std::string(tx_hex_string));
      buffer->tx_obj = tx;
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }
    *tx_data_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  CfdFreeTxDataHandle(handle, buffer);
  return result;
}

int CfdFreeTxDataHandle(void* handle, void* tx_data_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (tx_data_handle != nullptr) {
      CheckBuffer(tx_data_handle, kPrefixTransactionData);
      CfdCapiTransactionData* tx_data =
          static_cast<CfdCapiTransactionData*>(tx_data_handle);
      bool is_bitcoin = false;
      ConvertNetType(tx_data->net_type, &is_bitcoin);
      if (tx_data->tx_obj == nullptr) {
        // do nothing
      } else if (is_bitcoin) {
        TransactionContext* tx =
            static_cast<TransactionContext*>(tx_data->tx_obj);
        delete tx;
      } else {
#ifndef CFD_DISABLE_ELEMENTS
        ConfidentialTransactionContext* tx =
            static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
        delete tx;
#else
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
      }
      tx_data->tx_obj = nullptr;
    }
    FreeBuffer(
        tx_data_handle, kPrefixTransactionData,
        sizeof(CfdCapiTransactionData));
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

int CfdGetModifiedTxByHandle(
    void* handle, void* tx_data_handle, char** tx_hex_string) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);
    if (tx_hex_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx hex string is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx hex string is null.");
    }

    std::string base_tx;
    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      *tx_hex_string = CreateString(tx->GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      *tx_hex_string = CreateString(tx->GetHex());
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
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

int CfdGetTxInfoByHandle(
    void* handle, void* tx_data_handle, char** txid, char** wtxid,
    uint32_t* size, uint32_t* vsize, uint32_t* weight, uint32_t* version,
    uint32_t* locktime) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_txid = nullptr;
  char* work_wtxid = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    uint32_t temp_size = 0;
    uint32_t temp_vsize = 0;
    uint32_t temp_weight = 0;
    uint32_t temp_version = 0;
    uint32_t temp_locktime = 0;
    std::string temp_txid;
    std::string temp_wtxid;
    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      temp_txid = tx->GetTxid().GetHex();
      temp_wtxid = Txid(tx->GetWitnessHash()).GetHex();
      temp_size = tx->GetTotalSize();
      temp_vsize = tx->GetVsize();
      temp_weight = tx->GetWeight();
      temp_version = tx->GetVersion();
      temp_locktime = tx->GetLockTime();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      temp_txid = tx->GetTxid().GetHex();
      temp_wtxid = Txid(tx->GetWitnessHash()).GetHex();
      temp_size = tx->GetTotalSize();
      temp_vsize = tx->GetVsize();
      temp_weight = tx->GetWeight();
      temp_version = tx->GetVersion();
      temp_locktime = tx->GetLockTime();
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }
    if (txid != nullptr) work_txid = CreateString(temp_txid);
    if (wtxid != nullptr) work_wtxid = CreateString(temp_wtxid);
    if (size != nullptr) *size = temp_size;
    if (vsize != nullptr) *vsize = temp_vsize;
    if (weight != nullptr) *weight = temp_weight;
    if (version != nullptr) *version = temp_version;
    if (locktime != nullptr) *locktime = temp_locktime;

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

int CfdGetTxInByHandle(
    void* handle, void* tx_data_handle, uint32_t index, char** txid,
    uint32_t* vout, uint32_t* sequence, char** script_sig) {
  char* work_txid = nullptr;
  char* work_script_sig = nullptr;
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    Txid temp_txid;
    Script temp_unlocking_script;
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      const TxInReference ref = tx->GetTxIn(index);
      temp_txid = ref.GetTxid();
      temp_unlocking_script = ref.GetUnlockingScript();
      if (vout != nullptr) *vout = ref.GetVout();
      if (sequence != nullptr) *sequence = ref.GetSequence();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      const ConfidentialTxInReference ref = tx->GetTxIn(index);
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

int CfdGetTxInWitnessByHandle(
    void* handle, void* tx_data_handle, int stack_type, uint32_t txin_index,
    uint32_t stack_index, char** stack_data) {
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    if (stack_data == nullptr) {
      warn(CFD_LOG_SOURCE, "stack data is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. stack data is null.");
    }
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    std::vector<ByteData> witness_stack;
    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      const TxInReference ref = tx->GetTxIn(txin_index);
      if (stack_type != kCfdTxWitnessStackNormal) {
        warn(CFD_LOG_SOURCE, "Invalid stack type[{}]", stack_type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Invalid stack type.");
      }
      witness_stack = ref.GetScriptWitness().GetWitness();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      const ConfidentialTxInReference ref = tx->GetTxIn(txin_index);
      if (stack_type == kCfdTxWitnessStackNormal) {
        witness_stack = ref.GetScriptWitness().GetWitness();
      } else if (stack_type == kCfdTxWitnessStackPegin) {
        witness_stack = ref.GetPeginWitness().GetWitness();
      } else {
        warn(CFD_LOG_SOURCE, "Invalid stack type[{}]", stack_type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Invalid stack type.");
      }
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

int CfdGetTxOutByHandle(
    void* handle, void* tx_data_handle, uint32_t index, int64_t* value_satoshi,
    char** locking_script, char** asset) {
  int error_code = CfdErrorCode::kCfdUnknownError;
  char* work_script = nullptr;
  char* work_asset = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    Amount temp_value;
    Script temp_locking_script;
    std::string temp_asset;
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      const TxOutReference ref = tx->GetTxOut(index);
      temp_value = ref.GetValue();
      temp_locking_script = ref.GetLockingScript();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      const ConfidentialTxOutReference ref = tx->GetTxOut(index);
      temp_value = ref.GetValue();
      temp_asset = ref.GetAsset().GetHex();
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (locking_script != nullptr) {
      work_script = CreateString(temp_locking_script.GetHex());
    }
    if (asset != nullptr) work_asset = CreateString(temp_asset);

    if (value_satoshi != nullptr) {
      *value_satoshi = temp_value.GetSatoshiValue();
    }
    if (locking_script != nullptr) *locking_script = work_script;
    if (asset != nullptr) *asset = work_asset;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    error_code = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_script, &work_asset);
  return error_code;
}

int CfdGetTxInCountByHandle(
    void* handle, void* tx_data_handle, uint32_t* count) {
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      if (count != nullptr) *count = tx->GetTxInCount();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      if (count != nullptr) *count = tx->GetTxInCount();
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

int CfdGetTxInWitnessCountByHandle(
    void* handle, void* tx_data_handle, int stack_type, uint32_t txin_index,
    uint32_t* count) {
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      const TxInReference ref = tx->GetTxIn(txin_index);
      if (stack_type != kCfdTxWitnessStackNormal) {
        warn(CFD_LOG_SOURCE, "Invalid stack type[{}]", stack_type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Invalid stack type.");
      }
      if (count != nullptr) *count = ref.GetScriptWitnessStackNum();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      const ConfidentialTxInReference ref = tx->GetTxIn(txin_index);
      if (stack_type == kCfdTxWitnessStackNormal) {
        if (count != nullptr) *count = ref.GetScriptWitnessStackNum();
      } else if (stack_type == kCfdTxWitnessStackPegin) {
        if (count != nullptr) *count = ref.GetPeginWitnessStackNum();
      } else {
        warn(CFD_LOG_SOURCE, "Invalid stack type[{}]", stack_type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError, "Invalid stack type.");
      }
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

int CfdGetTxOutCountByHandle(
    void* handle, void* tx_data_handle, uint32_t* count) {
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      if (count != nullptr) *count = tx->GetTxOutCount();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      if (count != nullptr) *count = tx->GetTxOutCount();
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

int CfdGetTxInIndexByHandle(
    void* handle, void* tx_data_handle, const char* txid, uint32_t vout,
    uint32_t* index) {
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    OutPoint outpoint(Txid(txid), vout);
    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      if (index != nullptr) *index = tx->GetTxInIndex(outpoint);
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      if (index != nullptr) *index = tx->GetTxInIndex(outpoint);
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

int CfdGetTxOutIndexByHandle(
    void* handle, void* tx_data_handle, const char* address,
    const char* direct_locking_script, uint32_t* index) {
  try {
    cfd::Initialize();
    CheckBuffer(tx_data_handle, kPrefixTransactionData);
    CfdCapiTransactionData* tx_data =
        static_cast<CfdCapiTransactionData*>(tx_data_handle);

    bool is_find = false;
    bool is_bitcoin = false;
    ConvertNetType(tx_data->net_type, &is_bitcoin);
    if (tx_data->tx_obj == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError, "Invalid handle state. tx is null");
    } else if (is_bitcoin) {
      TransactionContext* tx =
          static_cast<TransactionContext*>(tx_data->tx_obj);
      if (!IsEmptyString(direct_locking_script)) {
        is_find = tx->IsFindTxOut(Script(direct_locking_script), index);
      } else if (!IsEmptyString(address)) {
        Address addr(address);
        is_find = tx->IsFindTxOut(addr, index);
      } else {
        // do nothing
      }
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialTransactionContext* tx =
          static_cast<ConfidentialTransactionContext*>(tx_data->tx_obj);
      if (!IsEmptyString(direct_locking_script)) {
        is_find = tx->IsFindTxOut(Script(direct_locking_script), index);
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
        is_find = tx->IsFindTxOut(addr, index);
      } else {
        // fee
        is_find = tx->IsFindFeeTxOut(index);
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
