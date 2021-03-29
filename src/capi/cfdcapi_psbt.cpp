// Copyright 2021 CryptoGarage
/**
 * @file cfdcapi_psbt.cpp
 *
 * @brief This file is implements Partially Signed Bitcoin Transaction.
 */
#ifndef CFD_DISABLE_CAPI
#include "cfdc/cfdcapi_psbt.h"

#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_psbt.h"
#include "cfd/cfd_transaction_common.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

using cfd::AddressFactory;
using cfd::CoinSelectionOption;
using cfd::UtxoData;
using cfd::UtxoFilter;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
using cfd::core::Descriptor;
using cfd::core::DescriptorKeyInfo;
using cfd::core::Deserializer;
using cfd::core::ExtPrivkey;
using cfd::core::ExtPubkey;
using cfd::core::HashUtil;
using cfd::core::HDWallet;
using cfd::core::KeyData;
using cfd::core::NetType;
using cfd::core::OutPoint;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;
using cfd::core::StringUtil;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::TxOut;
using cfd::core::TxOutReference;

using cfd::core::logger::info;
using cfd::core::logger::warn;

using cfd::Psbt;

// =============================================================================
// internal file
// =============================================================================
//! ExtPubkey hex size.
static constexpr size_t kCfdExtPubkeyHexSize = ExtPrivkey::kSerializeSize * 2;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: PsbtHandle
constexpr const char* const kPrefixPsbtHandle = "PsbtHandle";
//! prefix: PsbtPubkeyList
constexpr const char* const kPrefixPsbtPubkeyList = "PsbtPubkeyList";
//! prefix: PsbtByteDataList
constexpr const char* const kPrefixPsbtByteDataList = "PsbtDataList";
//! prefix: PsbtFund
constexpr const char* const kPrefixPsbtFundHandle = "PsbtFund";

/**
 * @brief cfd-capi PsbtHandle struct.
 */
struct CfdCapiPsbtHandle {
  char prefix[kPrefixLength];  //!< buffer prefix
  NetType net_type;            //!< network type
  //! psbt object
  Psbt* psbt;
};

/**
 * @brief cfd-capi PsbtPubkeyListHandle struct.
 */
struct CfdCapiPsbtPubkeyListHandle {
  char prefix[kPrefixLength];      //!< buffer prefix
  bool has_bip32_list;             //!< bip32 list
  bool has_xpub_list;              //!< xpubkey list
  std::vector<KeyData>* key_list;  //!< key list
};

/**
 * @brief cfd-capi PsbtByteDataListHandle struct.
 */
struct CfdCapiPsbtByteDataListHandle {
  char prefix[kPrefixLength];        //!< buffer prefix
  std::vector<ByteData>* data_list;  //!< data list
};

/**
 * @brief cfd-capi PsbtFundHandle struct.
 */
struct CfdCapiPsbtFundHandle {
  char prefix[kPrefixLength];      //!< buffer prefix
  std::vector<UtxoData>* utxos;    //!< utxo list
  std::vector<uint32_t>* indexes;  //!< index list
  /// fee rate (double)
  double fee_rate;
  /// dust fee rate (double)
  double dust_fee_rate;
  /// longterm fee rate (double)
  double long_term_fee_rate;
  /// knapsack min change (int64)
  int64_t knapsack_min_change;
};

/**
 * @brief parse pubkey.
 * @param[in] kind      psbt record kind.
 * @param[in] pubkey    pubkey or xpubkey.
 * @param[in] net_type  network type.
 * @return key data.
 */
static KeyData ParsePubkey(int kind, const char* pubkey, NetType net_type) {
  constexpr size_t kCompressPubkeyHexSize = Pubkey::kCompressedPubkeySize * 2;
  std::string pubkey_str(pubkey);

  bool has_extpubkey = false;
  switch (kind) {
    case kCfdPsbtRecordGloalXpub:
      has_extpubkey = true;
      break;
    case kCfdPsbtRecordInputBip32:
    case kCfdPsbtRecordInputSignature:
    case kCfdPsbtRecordOutputBip32:
    default:
      break;
  }

  bool is_pubkey = false;
  if (pubkey_str.length() == kCompressPubkeyHexSize) {
    is_pubkey = true;
  }
  if (is_pubkey == has_extpubkey) {
    warn(CFD_LOG_SOURCE, "unmatch pubkey type.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to parameter. unmatch pubkey type.");
  }

  if (is_pubkey) {
    return KeyData(pubkey_str);
  }
  KeyData data;
  if (pubkey_str.length() == kCfdExtPubkeyHexSize) {
    data = KeyData(ExtPubkey(ByteData(pubkey_str)), "", ByteData());
  } else {
    data = KeyData(ExtPubkey(pubkey_str), "", ByteData());
  }
  bool has_mainnet =
      (data.GetExtPubkey().GetNetworkType() == NetType::kMainnet);

  if (((net_type == NetType::kMainnet) && (!has_mainnet)) ||
      ((net_type != NetType::kMainnet) && (has_mainnet))) {
    warn(CFD_LOG_SOURCE, "unmatch xpubkey network type.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to parameter. unmatch xpubkey network type.");
  }
  return data;
}

/**
 * @brief convert to keyData.
 * @param[in] pubkey          pubkey or descriptor.
 * @param[in] fingerprint     fingerprint.
 * @param[in] bip32_path      bip32 path.
 * @param[out] key_list       key data list.
 */
static void ConvertToKeyData(
    const std::string& pubkey, const char* fingerprint, const char* bip32_path,
    std::vector<KeyData>* key_list) {
  KeyData key_data;
  try {
    auto desc = Descriptor::Parse(pubkey);
    key_data = desc.GetKeyData();
  } catch (const CfdException&) {
    // fall through
  }
  if (!key_data.IsValid()) key_data = KeyData(pubkey);

  if (key_data.GetBip32Path().empty() ||
      (key_data.GetFingerprint().GetDataSize() != 4)) {
    if (IsEmptyString(fingerprint)) {
      warn(CFD_LOG_SOURCE, "fingerprint is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. fingerprint is null or empty.");
    }
    if (IsEmptyString(bip32_path)) {
      warn(CFD_LOG_SOURCE, "bip32_path is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. bip32_path is null or empty.");
    }
    key_list->emplace_back(
        Pubkey(pubkey), std::string(bip32_path), ByteData(fingerprint));
  } else {
    key_list->emplace_back(key_data.ToString());
  }
}

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiPsbtByteDataListHandle;
using cfd::capi::CfdCapiPsbtFundHandle;
using cfd::capi::CfdCapiPsbtHandle;
using cfd::capi::CfdCapiPsbtPubkeyListHandle;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertNetType;
using cfd::capi::ConvertToKeyData;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kPrefixPsbtByteDataList;
using cfd::capi::kPrefixPsbtFundHandle;
using cfd::capi::kPrefixPsbtHandle;
using cfd::capi::kPrefixPsbtPubkeyList;
using cfd::capi::ParsePubkey;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

extern "C" {

int CfdCreatePsbtHandle(
    void* handle, int net_type, const char* psbt_string,
    const char* tx_hex_string, uint32_t version, uint32_t locktime,
    void** psbt_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiPsbtHandle* buffer = nullptr;
  try {
    cfd::Initialize();

    if (psbt_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt_handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. psbt_handle is null.");
    }
    bool is_bitcoin = false;
    NetType network_type = ConvertNetType(net_type, &is_bitcoin);
    if (!is_bitcoin) {
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
    }

    Psbt psbt;
    if (!IsEmptyString(psbt_string)) {
      try {
        psbt = Psbt(psbt_string);
      } catch (const CfdException& fail_base64) {
        std::string err_msg(fail_base64.what());
        if (err_msg != "psbt unmatch magic error.") {
          throw fail_base64;
        }
        psbt = Psbt(ByteData(psbt_string));
      }
    } else if (!IsEmptyString(tx_hex_string)) {
      psbt = Psbt(Transaction(tx_hex_string));
    } else {
      psbt = Psbt(version, locktime);
    }

    buffer = static_cast<CfdCapiPsbtHandle*>(
        AllocBuffer(kPrefixPsbtHandle, sizeof(CfdCapiPsbtHandle)));
    buffer->psbt = new Psbt(psbt);
    buffer->net_type = network_type;
    *psbt_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  CfdFreePsbtHandle(handle, buffer);
  return result;
}

int CfdFreePsbtHandle(void* handle, void* psbt_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (psbt_handle != nullptr) {
      CheckBuffer(psbt_handle, kPrefixPsbtHandle);
      CfdCapiPsbtHandle* psbt_data =
          static_cast<CfdCapiPsbtHandle*>(psbt_handle);
      if (psbt_data->psbt != nullptr) {
        delete psbt_data->psbt;
        psbt_data->psbt = nullptr;
      }
      FreeBuffer(psbt_handle, kPrefixPsbtHandle, sizeof(CfdCapiPsbtHandle));
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

int CfdGetPsbtData(
    void* handle, void* psbt_handle, char** psbt_base64, char** psbt_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_base64 = nullptr;
  char* work_hex = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    if (psbt_base64 != nullptr) {
      work_base64 = CreateString(psbt_obj->psbt->GetBase64());
    }
    if (psbt_hex != nullptr) {
      work_hex = CreateString(psbt_obj->psbt->GetData().GetHex());
    }
    if (psbt_base64 != nullptr) *psbt_base64 = work_base64;
    if (psbt_hex != nullptr) *psbt_hex = work_hex;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_base64, &work_hex);
  return result;
}

int CfdGetPsbtGlobalData(
    void* handle, void* psbt_handle, uint32_t* psbt_version, char** base_tx,
    uint32_t* txin_count, uint32_t* txout_count) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    if (psbt_version != nullptr) {
      *psbt_version = psbt_obj->psbt->GetPsbtVersion();
    }
    auto tx = psbt_obj->psbt->GetTransaction();
    if (txin_count != nullptr) *txin_count = tx.GetTxInCount();
    if (txout_count != nullptr) *txout_count = tx.GetTxOutCount();
    if (base_tx != nullptr) *base_tx = CreateString(tx.GetHex());
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

int CfdJoinPsbt(
    void* handle, void* psbt_handle, const char* psbt_join_base64) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(psbt_join_base64)) {
      warn(CFD_LOG_SOURCE, "psbt_join_base64 is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. psbt_join_base64 is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    Psbt join_psbt(psbt_join_base64);
    psbt_obj->psbt->Join(join_psbt);
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

int CfdSignPsbt(
    void* handle, void* psbt_handle, const char* privkey, bool has_grind_r) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(privkey)) {
      warn(CFD_LOG_SOURCE, "privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. privkey is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    std::string privkey_str(privkey);
    Privkey privkey_obj;
    if (Privkey::HasWif(privkey_str)) {
      privkey_obj = Privkey::FromWif(privkey_str);
    } else {
      privkey_obj = Privkey(privkey_str);
    }
    psbt_obj->psbt->Sign(privkey_obj, has_grind_r);
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

int CfdCombinePsbt(
    void* handle, void* psbt_handle, const char* psbt_combine_base64) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(psbt_combine_base64)) {
      warn(CFD_LOG_SOURCE, "psbt_combine_base64 is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. psbt_combine_base64 is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    Psbt combine_psbt(psbt_combine_base64);
    psbt_obj->psbt->Combine(combine_psbt);
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

int CfdFinalizePsbt(void* handle, void* psbt_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    psbt_obj->psbt->Finalize();
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

int CfdExtractPsbtTransaction(
    void* handle, void* psbt_handle, char** transaction) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (transaction == nullptr) {
      warn(CFD_LOG_SOURCE, "transaction is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. transaction is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    *transaction = CreateString(psbt_obj->psbt->Extract().GetHex());
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

int CfdIsFinalizedPsbt(void* handle, void* psbt_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    if (!psbt_obj->psbt->IsFinalized()) return kCfdSignVerificationError;
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

int CfdIsFinalizedPsbtInput(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    Txid txid_obj(txid);
    if (!psbt_obj->psbt->IsFinalizedInput(OutPoint(txid_obj, vout))) {
      return kCfdSignVerificationError;
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

int CfdAddPsbtTxInWithPubkey(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    uint32_t sequence, int64_t amount, const char* locking_script,
    const char* descriptor, const char* full_tx_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  Psbt backup_psbt;
  bool can_restore_psbt = false;
  CfdCapiPsbtHandle* psbt_obj = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    backup_psbt = *(psbt_obj->psbt);

    OutPoint outpoint(Txid(txid), vout);

    Script locking_script_obj;
    std::vector<KeyData> key_list;
    if (!IsEmptyString(descriptor)) {
      Descriptor desc = Descriptor::Parse(descriptor);
      std::vector<std::string> path_args;
      auto key = desc.GetKeyData(path_args);
      auto ref = desc.GetReference(&path_args);
      if (IsEmptyString(locking_script)) {
        locking_script_obj = ref.GetLockingScript();
      } else {
        locking_script_obj = Script(locking_script);
      }
      key_list.push_back(key);
    } else if (!IsEmptyString(locking_script)) {
      locking_script_obj = Script(locking_script);
    }

    psbt_obj->psbt->AddTxIn(outpoint, sequence);
    can_restore_psbt = true;

    if (!IsEmptyString(full_tx_hex)) {
      Transaction tx(full_tx_hex);
      if (!tx.GetTxOut(vout).GetLockingScript().Equals(locking_script_obj)) {
        warn(CFD_LOG_SOURCE, "unmatch locking script.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. unmatch locking script.");
      }
      psbt_obj->psbt->SetTxInUtxo(outpoint, tx, Script(), key_list);
    } else if (!locking_script_obj.IsEmpty()) {
      TxOut txout(Amount(amount), locking_script_obj);
      psbt_obj->psbt->SetTxInUtxo(
          outpoint, TxOutReference(txout), Script(), key_list);
    }
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  if (can_restore_psbt) *(psbt_obj->psbt) = backup_psbt;
  return result;
}

int CfdAddPsbtTxInWithScript(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    uint32_t sequence, int64_t amount, const char* locking_script,
    const char* redeem_script, const char* descriptor,
    const char* full_tx_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  Psbt backup_psbt;
  bool can_restore_psbt = false;
  CfdCapiPsbtHandle* psbt_obj = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    backup_psbt = *(psbt_obj->psbt);

    OutPoint outpoint(Txid(txid), vout);
    std::vector<KeyData> key_list;

    Script script;
    Script locking_script_obj;
    if (!IsEmptyString(locking_script)) {
      locking_script_obj = Script(locking_script);
    }
    if (!IsEmptyString(redeem_script)) {
      script = Script(redeem_script);
    }

    if (!IsEmptyString(descriptor)) {
      Descriptor desc = Descriptor::Parse(descriptor);
      std::vector<std::string> path_args;
      key_list = desc.GetKeyDataAll(&path_args);
      auto ref_list = desc.GetReferenceAll(&path_args);
      if (!IsEmptyString(locking_script)) {
        locking_script_obj = ref_list.front().GetLockingScript();
      }
      if (IsEmptyString(redeem_script)) {
        auto ref = ref_list.front();
        while (ref.HasChild()) {
          if (ref.HasRedeemScript()) script = ref.GetRedeemScript();
          ref = ref.GetChild();
        }
        if (ref.HasRedeemScript()) script = ref.GetRedeemScript();
      }
    }

    psbt_obj->psbt->AddTxIn(outpoint, sequence);
    can_restore_psbt = true;

    if (!IsEmptyString(full_tx_hex)) {
      Transaction tx(full_tx_hex);
      if (!tx.GetTxOut(vout).GetLockingScript().Equals(locking_script_obj)) {
        warn(CFD_LOG_SOURCE, "unmatch locking script.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. unmatch locking script.");
      }
      psbt_obj->psbt->SetTxInUtxo(outpoint, tx, script, key_list);
    } else if (!locking_script_obj.IsEmpty()) {
      TxOut txout(Amount(amount), locking_script_obj);
      psbt_obj->psbt->SetTxInUtxo(
          outpoint, TxOutReference(txout), script, key_list);
    }
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  if (can_restore_psbt) *(psbt_obj->psbt) = backup_psbt;
  return result;
}

int CfdSetPsbtTxInUtxo(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* locking_script, const char* full_tx_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);
    auto redeem_script = psbt_obj->psbt->GetTxInRedeemScript(outpoint, true);
    std::vector<KeyData> key_list;
    if (!IsEmptyString(full_tx_hex)) {
      Transaction tx(full_tx_hex);
      psbt_obj->psbt->SetTxInUtxo(outpoint, tx, redeem_script, key_list);
    } else if (!IsEmptyString(locking_script)) {
      Script script(locking_script);
      TxOut txout(Amount(amount), script);
      auto pubkeys = psbt_obj->psbt->GetTxInKeyDataList(outpoint);
      if (script.IsWitnessProgram() || (!redeem_script.IsEmpty()) ||
          (!pubkeys.empty())) {
        psbt_obj->psbt->SetTxInUtxo(
            outpoint, TxOutReference(txout), redeem_script, key_list);
      } else {  // on P2SH-segwit
        psbt_obj->psbt->SetTxInWitnessUtxoDirect(
            outpoint, TxOutReference(txout));
      }
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

int CfdSetPsbtTxInBip32Pubkey(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    const char* pubkey, const char* fingerprint, const char* bip32_path) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
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
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);
    std::vector<KeyData> key_list;
    ConvertToKeyData(pubkey, fingerprint, bip32_path, &key_list);

    auto script = psbt_obj->psbt->GetTxInRedeemScript(outpoint, true);
    auto tx = psbt_obj->psbt->GetTxInUtxoFull(outpoint, true);
    if (tx.GetTxOutCount() > vout) {
      psbt_obj->psbt->SetTxInUtxo(outpoint, tx, script, key_list);
    } else {
      auto txout = psbt_obj->psbt->GetTxInUtxo(outpoint, true);
      if (!txout.GetLockingScript().IsEmpty()) {
        psbt_obj->psbt->SetTxInUtxo(
            outpoint, TxOutReference(txout), script, key_list);
      } else {
        psbt_obj->psbt->SetTxInBip32KeyDirect(outpoint, key_list[0]);
      }
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

int CfdSetPsbtSignature(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    const char* pubkey, const char* der_signature) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
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
    if (IsEmptyString(der_signature)) {
      warn(CFD_LOG_SOURCE, "der_signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. der_signature is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);

    KeyData key = KeyData(Pubkey(pubkey), std::string(), ByteData());
    psbt_obj->psbt->SetTxInSignature(outpoint, key, ByteData(der_signature));
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

int CfdSetPsbtSighashType(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int sighash_type) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);

    psbt_obj->psbt->SetTxInSighashType(
        outpoint, SigHashType(static_cast<SigHashAlgorithm>(sighash_type)));
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

int CfdSetPsbtFinalizeScript(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    const char* scriptsig) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (IsEmptyString(scriptsig)) {
      warn(CFD_LOG_SOURCE, "scriptsig is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. scriptsig is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);

    bool is_witness = false;
    auto txout = psbt_obj->psbt->GetTxInUtxo(outpoint, false, &is_witness);
    auto script = psbt_obj->psbt->GetTxInRedeemScript(outpoint, true);
    std::vector<ByteData> script_stack;
    if (is_witness) {
      Script scriptsig_obj(scriptsig);
      bool is_multi_first = script.IsMultisigScript();
      for (const auto item : scriptsig_obj.GetElementList()) {
        if (is_multi_first) {
          if (item.GetOpCode() == ScriptOperator::OP_0) {
            script_stack.emplace_back(ByteData());
          } else if (item.IsBinary()) {
            script_stack.emplace_back(item.GetBinaryData());
          } else {
            script_stack.emplace_back(item.GetData());
          }
          is_multi_first = false;
        } else {
          if (item.IsBinary()) {
            script_stack.emplace_back(item.GetBinaryData());
          } else {
            script_stack.emplace_back(item.GetData());
          }
        }
      }
    } else {
      script_stack.emplace_back(ByteData(scriptsig));
    }
    psbt_obj->psbt->SetTxInFinalScript(outpoint, script_stack);
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

int CfdClearPsbtSignData(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);

    psbt_obj->psbt->ClearTxInSignData(outpoint);
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

int CfdGetPsbtSighashType(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int* sighash_type) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (sighash_type == nullptr) {
      warn(CFD_LOG_SOURCE, "sighash_type is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sighash_type is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);

    if (!psbt_obj->psbt->IsFindTxInSighashType(outpoint)) {
      *sighash_type = 0;
    } else {
      auto sighash_type_obj = psbt_obj->psbt->GetTxInSighashType(outpoint);
      *sighash_type =
          static_cast<uint32_t>(sighash_type_obj.GetSigHashAlgorithm());
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

int CfdGetPsbtUtxoData(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int64_t* amount, char** locking_script, char** redeem_script,
    char** descriptor, char** full_tx_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);
    uint32_t index = psbt_obj->psbt->GetTxInIndex(outpoint);

    return CfdGetPsbtUtxoDataByIndex(
        handle, psbt_handle, index, nullptr, nullptr, amount, locking_script,
        redeem_script, descriptor, full_tx_hex);
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdGetPsbtUtxoDataByIndex(
    void* handle, void* psbt_handle, uint32_t index, char** txid,
    uint32_t* vout, int64_t* amount, char** locking_script,
    char** redeem_script, char** descriptor, char** full_tx_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_txid = nullptr;
  char* work_locking_script = nullptr;
  char* work_redeem_script = nullptr;
  char* work_descriptor = nullptr;
  char* work_tx = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    auto txin = psbt_obj->psbt->GetTransaction().GetTxIn(index);
    OutPoint outpoint = txin.GetOutPoint();
    if (txid != nullptr) {
      work_txid = CreateString(outpoint.GetTxid().GetHex());
    }
    if (vout != nullptr) *vout = outpoint.GetVout();

    auto utxo = psbt_obj->psbt->GetUtxoData(index);
    if (!utxo.locking_script.IsEmpty()) {
      if (locking_script != nullptr) {
        work_locking_script = CreateString(utxo.locking_script.GetHex());
      }
      if (amount != nullptr) *amount = utxo.amount.GetSatoshiValue();
    }

    Script script = psbt_obj->psbt->GetTxInRedeemScript(index, true);
    if ((!script.IsEmpty()) && (!script.IsP2wpkhScript()) &&
        (redeem_script != nullptr)) {
      work_redeem_script = CreateString(script.GetHex());
    }

    auto tx = psbt_obj->psbt->GetTxInUtxoFull(index, true);
    TxOutReference txout;
    if (tx.GetTxOutCount() > outpoint.GetVout()) {
      txout = tx.GetTxOut(outpoint.GetVout());
      if (full_tx_hex != nullptr) work_tx = CreateString(tx.GetHex());
    }

    auto key_list = psbt_obj->psbt->GetTxInKeyDataList(index);
    if (utxo.descriptor.empty()) {
      Script temp_script = txout.GetLockingScript();
      if (temp_script.IsP2shScript() && script.IsMultisigScript()) {
        uint32_t req_num = 0;
        auto multisig_key_list =
            ScriptUtil::ExtractPubkeysFromMultisigScript(script, &req_num);
        utxo.descriptor = "multi(" + std::to_string(req_num);
        for (const auto& pubkey : multisig_key_list) {
          bool is_find = false;
          for (const auto& key : key_list) {
            if (pubkey.Equals(key.GetPubkey())) {
              utxo.descriptor += "," + key.ToString();
              is_find = true;
            }
          }
          if (!is_find) {
            utxo.descriptor += "," + pubkey.GetHex();
          }
        }
        utxo.descriptor = "sh(" + utxo.descriptor + ")";
      } else if (temp_script.IsP2pkhScript() || temp_script.IsP2pkScript()) {
        if (key_list.size() == 1) {
          if (temp_script.IsP2pkhScript())
            utxo.descriptor = "pkh(";
          else if (temp_script.IsP2pkScript())
            utxo.descriptor = "pk(";
          utxo.descriptor += key_list[0].ToString();
          utxo.descriptor += ")";
        }
      }
    }
    if ((!utxo.descriptor.empty()) && (descriptor != nullptr)) {
      work_descriptor = CreateString(utxo.descriptor);
    }

    if (txid != nullptr) *txid = work_txid;
    if (locking_script != nullptr) *locking_script = work_locking_script;
    if (redeem_script != nullptr) *redeem_script = work_redeem_script;
    if (descriptor != nullptr) *descriptor = work_descriptor;
    if (full_tx_hex != nullptr) *full_tx_hex = work_tx;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(
      &work_txid, &work_locking_script, &work_redeem_script, &work_descriptor,
      &work_tx);
  return result;
}

int CfdAddPsbtTxOutWithPubkey(
    void* handle, void* psbt_handle, int64_t amount,
    const char* locking_script, const char* descriptor, uint32_t* index) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    Script script;
    uint32_t txout_index = 0;
    if (!IsEmptyString(locking_script)) {
      script = Script(locking_script);
    }

    if (!IsEmptyString(descriptor)) {
      Descriptor desc = Descriptor::Parse(descriptor);
      std::vector<std::string> path_args;
      auto key = desc.GetKeyData(path_args);
      auto ref = desc.GetReference(&path_args);
      if ((!script.IsEmpty()) && (!script.Equals(ref.GetLockingScript()))) {
        std::string err_msg = "locking_script doesn't match the descriptor.";
        warn(CFD_LOG_SOURCE, "{}", err_msg);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. " + err_msg);
      }

      txout_index = psbt_obj->psbt->AddTxOutData(
          Amount(amount), ref.GenerateAddress(psbt_obj->net_type), key);
    } else if (!script.IsEmpty()) {
      txout_index = psbt_obj->psbt->AddTxOut(script, Amount(amount));
    } else {
      warn(CFD_LOG_SOURCE, "locking_script is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. locking_script is null or empty.");
    }

    if (index != nullptr) *index = txout_index;
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

int CfdAddPsbtTxOutWithScript(
    void* handle, void* psbt_handle, int64_t amount,
    const char* locking_script, const char* redeem_script,
    const char* descriptor, uint32_t* index) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    Script script;
    Script redeem_script_obj;
    uint32_t txout_index = 0;
    if (!IsEmptyString(locking_script)) script = Script(locking_script);
    if (!IsEmptyString(redeem_script)) {
      redeem_script_obj = Script(redeem_script);
    }

    if (!IsEmptyString(descriptor)) {
      Descriptor desc = Descriptor::Parse(descriptor);
      std::vector<std::string> path_args;
      auto key_list = desc.GetKeyDataAll();
      auto ref_list = desc.GetReferenceAll();
      Script temp_redeem_script;
      Address addr;
      auto ref = ref_list.front();
      addr = ref.GenerateAddress(psbt_obj->net_type);
      while (ref.HasChild()) {
        if (ref.HasRedeemScript()) temp_redeem_script = ref.GetRedeemScript();
        ref = ref.GetChild();
      }
      if (ref.HasRedeemScript()) temp_redeem_script = ref.GetRedeemScript();

      if ((!script.IsEmpty()) && (!script.Equals(ref.GetLockingScript()))) {
        std::string err_msg = "locking_script doesn't match the descriptor.";
        warn(CFD_LOG_SOURCE, "{}", err_msg);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. " + err_msg);
      }
      if ((!redeem_script_obj.IsEmpty()) &&
          (!redeem_script_obj.Equals(temp_redeem_script))) {
        std::string err_msg = "redeem_script doesn't match the descriptor.";
        warn(CFD_LOG_SOURCE, "{}", err_msg);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. " + err_msg);
      }

      txout_index = psbt_obj->psbt->AddTxOutData(
          Amount(amount), addr, temp_redeem_script, key_list);
    } else if ((!redeem_script_obj.IsEmpty()) && (!script.IsEmpty())) {
      AddressFactory factory(psbt_obj->net_type);
      Address addr;
      if (script.IsP2wshScript()) {
        auto temp_script =
            ScriptUtil::CreateP2wshLockingScript(redeem_script_obj);
        if (!temp_script.Equals(script)) {
          std::string err_msg = "locking_script is not match redeem_script.";
          warn(CFD_LOG_SOURCE, "{}", err_msg);
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "Failed to parameter. " + err_msg);
        }
        addr = factory.CreateP2wshAddress(redeem_script_obj);
      } else if (script.IsP2shScript()) {
        auto temp_sh_script =
            ScriptUtil::CreateP2shLockingScript(redeem_script_obj);
        auto temp_wsh_script =
            ScriptUtil::CreateP2wshLockingScript(redeem_script_obj);
        auto temp_script =
            ScriptUtil::CreateP2shLockingScript(temp_wsh_script);
        if (temp_sh_script.Equals(script)) {
          addr = factory.CreateP2shAddress(redeem_script_obj);
        } else if (temp_script.Equals(script)) {
          addr = Address(
              psbt_obj->net_type, AddressType::kP2shP2wshAddress,
              HashUtil::Hash160(temp_wsh_script));
        } else {
          std::string err_msg = "locking_script is not match redeem_script.";
          warn(CFD_LOG_SOURCE, "{}", err_msg);
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "Failed to parameter. " + err_msg);
        }
      } else {
        std::string err_msg = "locking_script is not support format.";
        warn(CFD_LOG_SOURCE, "{}", err_msg);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. " + err_msg);
      }
      std::vector<KeyData> key_list;
      txout_index = psbt_obj->psbt->AddTxOutData(
          Amount(amount), addr, redeem_script_obj, key_list);
    } else if (!script.IsEmpty()) {
      txout_index = psbt_obj->psbt->AddTxOut(script, Amount(amount));
    } else {
      warn(CFD_LOG_SOURCE, "locking_script is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. locking_script is null or empty.");
    }

    if (index != nullptr) *index = txout_index;
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

int CfdSetPsbtTxOutBip32Pubkey(
    void* handle, void* psbt_handle, uint32_t index, const char* pubkey,
    const char* fingerprint, const char* bip32_path) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    std::vector<KeyData> key_list;
    ConvertToKeyData(pubkey, fingerprint, bip32_path, &key_list);

    auto script = psbt_obj->psbt->GetTxOutScript(index, true);
    psbt_obj->psbt->SetTxOutData(index, script, key_list);
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

int CfdGetPsbtTxInIndex(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    uint32_t* index) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (index == nullptr) {
      warn(CFD_LOG_SOURCE, "index is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. index is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    OutPoint outpoint(Txid(txid), vout);

    *index = psbt_obj->psbt->GetTxInIndex(outpoint);
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

int CfdGetPsbtPubkeyRecord(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    const char* pubkey, char** value) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (value == nullptr) {
      warn(CFD_LOG_SOURCE, "value is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. value is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    KeyData pk_obj = ParsePubkey(kind, pubkey, psbt_obj->net_type);
    ByteData key;
    ByteData data;
    switch (kind) {
      case kCfdPsbtRecordInputSignature:
        key = Psbt::CreatePubkeyRecordKey(
            Psbt::kPsbtInputPartialSig, pk_obj.GetPubkey());
        data = psbt_obj->psbt->GetTxInRecord(index, key);
        break;
      case kCfdPsbtRecordInputBip32:
        key = Psbt::CreatePubkeyRecordKey(
            Psbt::kPsbtInputBip32Derivation, pk_obj.GetPubkey());
        data = psbt_obj->psbt->GetTxInRecord(index, key);
        break;
      case kCfdPsbtRecordOutputBip32:
        key = Psbt::CreatePubkeyRecordKey(
            Psbt::kPsbtOutputBip32Derivation, pk_obj.GetPubkey());
        data = psbt_obj->psbt->GetTxOutRecord(index, key);
        break;
      case kCfdPsbtRecordGloalXpub:
        key = Psbt::CreateFixRecordKey(
            Psbt::kPsbtGlobalXpub, pk_obj.GetExtPubkey().GetData());
        data = psbt_obj->psbt->GetGlobalRecord(key);
        break;
      default:
        warn(CFD_LOG_SOURCE, "kind is invalid: {}", kind);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. kind is invalid.");
    }

    *value = CreateString(data.GetHex());
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

int CfdIsFindPsbtPubkeyRecord(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    const char* pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    KeyData pk_obj = ParsePubkey(kind, pubkey, psbt_obj->net_type);
    ByteData key;
    bool is_find = true;
    switch (kind) {
      case kCfdPsbtRecordInputSignature:
        key = Psbt::CreatePubkeyRecordKey(
            Psbt::kPsbtInputPartialSig, pk_obj.GetPubkey());
        is_find = psbt_obj->psbt->IsFindTxInRecord(index, key);
        break;
      case kCfdPsbtRecordInputBip32:
        key = Psbt::CreatePubkeyRecordKey(
            Psbt::kPsbtInputBip32Derivation, pk_obj.GetPubkey());
        is_find = psbt_obj->psbt->IsFindTxInRecord(index, key);
        break;
      case kCfdPsbtRecordOutputBip32:
        key = Psbt::CreatePubkeyRecordKey(
            Psbt::kPsbtOutputBip32Derivation, pk_obj.GetPubkey());
        is_find = psbt_obj->psbt->IsFindTxOutRecord(index, key);
        break;
      case kCfdPsbtRecordGloalXpub:
        key = Psbt::CreateFixRecordKey(
            Psbt::kPsbtGlobalXpub, pk_obj.GetExtPubkey().GetData());
        is_find = psbt_obj->psbt->IsFindGlobalRecord(key);
        break;
      default:
        warn(CFD_LOG_SOURCE, "kind is invalid: {}", kind);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. kind is invalid.");
    }

    if (!is_find) return CfdErrorCode::kCfdNotFoundError;
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

int CfdGetPsbtBip32Data(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    const char* pubkey, char** fingerprint, char** bip32_path) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_fingerprint = nullptr;
  char* work_path = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    KeyData pk_obj = ParsePubkey(kind, pubkey, psbt_obj->net_type);

    std::vector<KeyData> key_list;
    if (kind == kCfdPsbtRecordInputBip32) {
      key_list = psbt_obj->psbt->GetTxInKeyDataList(index);
    } else if (kind == kCfdPsbtRecordOutputBip32) {
      key_list = psbt_obj->psbt->GetTxOutKeyDataList(index);
    } else if (kind == kCfdPsbtRecordGloalXpub) {
      key_list = psbt_obj->psbt->GetGlobalXpubkeyDataList();
    } else {
      warn(CFD_LOG_SOURCE, "kind is invalid: {}", kind);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. kind is invalid.");
    }

    KeyData key;
    bool has_extpubkey = pk_obj.HasExtPubkey();
    Pubkey target_pubkey = pk_obj.GetPubkey();
    for (const auto temp_key : key_list) {
      if (temp_key.GetPubkey().Equals(target_pubkey)) {
        if (has_extpubkey) {
          if (temp_key.GetExtPubkey().GetData().Equals(
                  pk_obj.GetExtPubkey().GetData())) {
            key = temp_key;
            break;
          }
        } else {
          key = temp_key;
          break;
        }
      }
    }
    if (!key.IsValid()) {
      warn(CFD_LOG_SOURCE, "pubkey not found: {}", pk_obj.ToString());
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey not found");
    }

    if (fingerprint != nullptr) {
      work_fingerprint = CreateString(key.GetFingerprint().GetHex());
    }
    if (bip32_path != nullptr) work_path = CreateString(key.GetBip32Path());

    if (fingerprint != nullptr) *fingerprint = work_fingerprint;
    if (bip32_path != nullptr) *bip32_path = work_path;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_fingerprint, &work_path);
  return result;
}

int CfdGetPsbtPubkeyList(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    uint32_t* list_num, void** pubkey_list_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiPsbtPubkeyListHandle* buffer = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (pubkey_list_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey_list_handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey_list_handle is null.");
    }
    if (list_num == nullptr) {
      warn(CFD_LOG_SOURCE, "list_num is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. list_num is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    std::vector<Pubkey> pk_list;
    std::vector<KeyData> key_list;
    bool has_bip32_list = true;
    bool has_xpub_list = false;
    switch (kind) {
      case kCfdPsbtRecordInputSignature:
        pk_list = psbt_obj->psbt->GetTxInSignaturePubkeyList(index);
        break;
      case kCfdPsbtRecordInputBip32:
        key_list = psbt_obj->psbt->GetTxInKeyDataList(index);
        break;
      case kCfdPsbtRecordOutputBip32:
        key_list = psbt_obj->psbt->GetTxOutKeyDataList(index);
        break;
      case kCfdPsbtRecordGloalXpub:
        key_list = psbt_obj->psbt->GetGlobalXpubkeyDataList();
        has_xpub_list = true;
        break;
      default:
        warn(CFD_LOG_SOURCE, "kind is invalid: {}", kind);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. kind is invalid.");
    }
    if (key_list.empty()) {
      has_bip32_list = false;
      ByteData empty_fingerprint;
      std::string empty_path;
      key_list.reserve(pk_list.size());
      for (const auto pubkey : pk_list) {
        key_list.emplace_back(pubkey, empty_path, empty_fingerprint);
      }
    }

    buffer = static_cast<CfdCapiPsbtPubkeyListHandle*>(AllocBuffer(
        kPrefixPsbtPubkeyList, sizeof(CfdCapiPsbtPubkeyListHandle)));
    buffer->key_list = new std::vector<KeyData>();
    buffer->has_bip32_list = has_bip32_list;
    buffer->has_xpub_list = has_xpub_list;
    *(buffer->key_list) = key_list;
    *list_num = static_cast<uint32_t>(key_list.size());
    *pubkey_list_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  CfdFreePsbtPubkeyList(handle, buffer);
  return result;
}

int CfdGetPsbtPubkeyListData(
    void* handle, void* pubkey_list_handle, uint32_t index, char** pubkey,
    char** pubkey_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_pubkey = nullptr;
  char* work_pubkey_hex = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(pubkey_list_handle, kPrefixPsbtPubkeyList);
    CfdCapiPsbtPubkeyListHandle* list_handle =
        static_cast<CfdCapiPsbtPubkeyListHandle*>(pubkey_list_handle);
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (list_handle->key_list == nullptr) {
      warn(CFD_LOG_SOURCE, "key_list is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. key_list is null.");
    }
    if (index >= list_handle->key_list->size()) {
      warn(CFD_LOG_SOURCE, "out of range error.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "Failed to out of range.");
    }

    auto key = list_handle->key_list->at(index);
    if (list_handle->has_xpub_list) {
      auto extkey = key.GetExtPubkey();
      work_pubkey = CreateString(extkey.ToString());
      if (pubkey_hex != nullptr) {
        work_pubkey_hex = CreateString(extkey.GetData().GetHex());
      }
    } else {
      auto pubkey_obj = key.GetPubkey();
      work_pubkey = CreateString(pubkey_obj.GetHex());
      if (pubkey_hex != nullptr) {
        work_pubkey_hex = CreateString(pubkey_obj.GetHex());
      }
    }

    *pubkey = work_pubkey;
    if (pubkey_hex != nullptr) *pubkey_hex = work_pubkey_hex;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_pubkey, &work_pubkey_hex);
  return result;
}

int CfdGetPsbtPubkeyListBip32Data(
    void* handle, void* pubkey_list_handle, uint32_t index, char** pubkey,
    char** fingerprint, char** bip32_path) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_pubkey = nullptr;
  char* work_fingerprint = nullptr;
  char* work_path = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(pubkey_list_handle, kPrefixPsbtPubkeyList);
    CfdCapiPsbtPubkeyListHandle* list_handle =
        static_cast<CfdCapiPsbtPubkeyListHandle*>(pubkey_list_handle);
    if (pubkey == nullptr) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (list_handle->key_list == nullptr) {
      warn(CFD_LOG_SOURCE, "key_list is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. key_list is null.");
    }
    if (index >= list_handle->key_list->size()) {
      warn(CFD_LOG_SOURCE, "out of range error.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "Failed to out of range.");
    }
    if (!list_handle->has_bip32_list) {
      warn(CFD_LOG_SOURCE, "bip32 not support error.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Failed to bip32 not support.");
    }

    auto key = list_handle->key_list->at(index);
    if (list_handle->has_xpub_list) {
      work_pubkey = CreateString(key.GetExtPubkey().ToString());
    } else {
      work_pubkey = CreateString(key.GetPubkey().GetHex());
    }
    if (fingerprint != nullptr) {
      work_fingerprint = CreateString(key.GetFingerprint().GetHex());
    }
    if (bip32_path != nullptr) work_path = CreateString(key.GetBip32Path());

    *pubkey = work_pubkey;
    if (fingerprint != nullptr) *fingerprint = work_fingerprint;
    if (bip32_path != nullptr) *bip32_path = work_path;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_pubkey, &work_fingerprint, &work_path);
  return result;
}

int CfdFreePsbtPubkeyList(void* handle, void* pubkey_list_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (pubkey_list_handle != nullptr) {
      CheckBuffer(pubkey_list_handle, kPrefixPsbtPubkeyList);
      CfdCapiPsbtPubkeyListHandle* list_handle =
          static_cast<CfdCapiPsbtPubkeyListHandle*>(pubkey_list_handle);
      if (list_handle->key_list != nullptr) {
        delete list_handle->key_list;
        list_handle->key_list = nullptr;
      }
      FreeBuffer(
          pubkey_list_handle, kPrefixPsbtPubkeyList,
          sizeof(CfdCapiPsbtPubkeyListHandle));
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

int CfdGetPsbtByteDataList(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    uint32_t* list_num, void** data_list_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiPsbtByteDataListHandle* buffer = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (data_list_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "data_list_handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. data_list_handle is null.");
    }
    if (list_num == nullptr) {
      warn(CFD_LOG_SOURCE, "list_num is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. list_num is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    std::vector<ByteData> data_list;
    switch (kind) {
      case kCfdPsbtRecordFinalWitness:
        data_list = psbt_obj->psbt->GetTxInFinalScript(index, true);
        break;
      case kCfdPsbtRecordInputUnknownKeys:
        data_list = psbt_obj->psbt->GetTxInRecordKeyList(index);
        break;
      case kCfdPsbtRecordOutputUnknownKeys:
        data_list = psbt_obj->psbt->GetTxOutRecordKeyList(index);
        break;
      case kCfdPsbtRecordGlobalUnknownKeys:
        data_list = psbt_obj->psbt->GetGlobalRecordKeyList();
        break;
      default:
        warn(CFD_LOG_SOURCE, "kind is invalid: {}", kind);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. kind is invalid.");
    }

    buffer = static_cast<CfdCapiPsbtByteDataListHandle*>(AllocBuffer(
        kPrefixPsbtByteDataList, sizeof(CfdCapiPsbtByteDataListHandle)));
    buffer->data_list = new std::vector<ByteData>();
    *(buffer->data_list) = data_list;
    *list_num = static_cast<uint32_t>(data_list.size());
    *data_list_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  CfdFreePsbtByteDataList(handle, buffer);
  return result;
}

int CfdGetPsbtByteDataItem(
    void* handle, void* data_list_handle, uint32_t index, char** data) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(data_list_handle, kPrefixPsbtByteDataList);
    CfdCapiPsbtByteDataListHandle* list_handle =
        static_cast<CfdCapiPsbtByteDataListHandle*>(data_list_handle);
    if (data == nullptr) {
      warn(CFD_LOG_SOURCE, "data is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. data is null.");
    }
    if (list_handle->data_list == nullptr) {
      warn(CFD_LOG_SOURCE, "data_list is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. data_list is null.");
    }
    if (index >= list_handle->data_list->size()) {
      warn(CFD_LOG_SOURCE, "out of range error.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "Failed to out of range.");
    }

    auto byte_data = list_handle->data_list->at(index);
    *data = CreateString(byte_data.GetHex());
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

int CfdFreePsbtByteDataList(void* handle, void* data_list_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (data_list_handle != nullptr) {
      CheckBuffer(data_list_handle, kPrefixPsbtByteDataList);
      CfdCapiPsbtByteDataListHandle* list_handle =
          static_cast<CfdCapiPsbtByteDataListHandle*>(data_list_handle);
      if (list_handle->data_list != nullptr) {
        delete list_handle->data_list;
        list_handle->data_list = nullptr;
      }
      FreeBuffer(
          data_list_handle, kPrefixPsbtByteDataList,
          sizeof(CfdCapiPsbtByteDataListHandle));
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

int CfdAddPsbtGlobalXpubkey(
    void* handle, void* psbt_handle, const char* xpubkey,
    const char* fingerprint, const char* bip32_path) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(xpubkey)) {
      warn(CFD_LOG_SOURCE, "xpubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. xpubkey is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    std::string xpub_str(xpubkey);
    KeyData key_data;
    if (xpub_str.length() == kCfdExtPubkeyHexSize) {
      key_data = KeyData(ExtPubkey(ByteData(xpub_str)), "", ByteData());
    } else {
      key_data = KeyData(xpub_str);
      if ((!key_data.HasExtPubkey()) || key_data.HasExtPrivkey()) {
        warn(CFD_LOG_SOURCE, "psbt invalid xpubkey format.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. psbt invalid xpubkey format.");
      }
    }

    bool has_mainnet =
        (key_data.GetExtPubkey().GetNetworkType() == NetType::kMainnet);
    if (((psbt_obj->net_type == NetType::kMainnet) && (!has_mainnet)) ||
        ((psbt_obj->net_type != NetType::kMainnet) && (has_mainnet))) {
      warn(CFD_LOG_SOURCE, "unmatch xpubkey network type.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. unmatch xpubkey network type.");
    }

    if (key_data.GetBip32Path().empty() ||
        (key_data.GetFingerprint().GetDataSize() != 4)) {
      if (IsEmptyString(fingerprint)) {
        warn(CFD_LOG_SOURCE, "fingerprint is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. fingerprint is null or empty.");
      }
      if (IsEmptyString(bip32_path)) {
        warn(CFD_LOG_SOURCE, "bip32_path is null or empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. bip32_path is null or empty.");
      }
      key_data = KeyData(
          key_data.GetExtPubkey(), std::string(bip32_path),
          ByteData(fingerprint));
    }
    psbt_obj->psbt->SetGlobalXpubkey(key_data);
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

int CfdSetPsbtRedeemScript(
    void* handle, void* psbt_handle, int type, uint32_t index,
    const char* redeem_script) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(redeem_script)) {
      warn(CFD_LOG_SOURCE, "redeem_script is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. redeem_script is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    Script script(redeem_script);
    std::vector<KeyData> key_list;
    switch (type) {
      case kCfdPsbtRecordTypeInput: {
        auto txout = psbt_obj->psbt->GetTxInUtxo(index);
        psbt_obj->psbt->SetTxInUtxo(
            index, TxOutReference(txout), script, key_list);
      } break;
      case kCfdPsbtRecordTypeOutput:
        psbt_obj->psbt->SetTxOutData(index, script, key_list);
        break;
      default:
        warn(CFD_LOG_SOURCE, "type is invalid: {}", type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. type is invalid.");
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

int CfdAddPsbtRecord(
    void* handle, void* psbt_handle, int type, uint32_t index, const char* key,
    const char* value) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(key)) {
      warn(CFD_LOG_SOURCE, "key is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. key is null or empty.");
    }
    if (value == nullptr) {
      warn(CFD_LOG_SOURCE, "value is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. value is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    ByteData key_data(key);
    ByteData data(value);
    switch (type) {
      case kCfdPsbtRecordTypeGlobal:
        psbt_obj->psbt->SetGlobalRecord(key_data, data);
        break;
      case kCfdPsbtRecordTypeInput:
        psbt_obj->psbt->SetTxInRecord(index, key_data, data);
        break;
      case kCfdPsbtRecordTypeOutput:
        psbt_obj->psbt->SetTxOutRecord(index, key_data, data);
        break;
      default:
        warn(CFD_LOG_SOURCE, "type is invalid: {}", type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. type is invalid.");
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

int CfdGetPsbtRecord(
    void* handle, void* psbt_handle, int type, uint32_t index, const char* key,
    char** value) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(key)) {
      warn(CFD_LOG_SOURCE, "key is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. key is null or empty.");
    }
    if (value == nullptr) {
      warn(CFD_LOG_SOURCE, "value is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. value is null.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    ByteData key_data(key);
    ByteData data;
    switch (type) {
      case kCfdPsbtRecordTypeGlobal:
        data = psbt_obj->psbt->GetGlobalRecord(key_data);
        break;
      case kCfdPsbtRecordTypeInput:
        data = psbt_obj->psbt->GetTxInRecord(index, key_data);
        break;
      case kCfdPsbtRecordTypeOutput:
        data = psbt_obj->psbt->GetTxOutRecord(index, key_data);
        break;
      default:
        warn(CFD_LOG_SOURCE, "type is invalid: {}", type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. type is invalid.");
    }

    *value = CreateString(data.GetHex());
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

int CfdIsFindPsbtRecord(
    void* handle, void* psbt_handle, int type, uint32_t index,
    const char* key) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(key)) {
      warn(CFD_LOG_SOURCE, "key is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. key is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    ByteData key_data(key);
    bool is_find = true;
    switch (type) {
      case kCfdPsbtRecordTypeGlobal:
        is_find = psbt_obj->psbt->IsFindGlobalRecord(key_data);
        break;
      case kCfdPsbtRecordTypeInput:
        is_find = psbt_obj->psbt->IsFindTxInRecord(index, key_data);
        break;
      case kCfdPsbtRecordTypeOutput:
        is_find = psbt_obj->psbt->IsFindTxOutRecord(index, key_data);
        break;
      default:
        warn(CFD_LOG_SOURCE, "type is invalid: {}", type);
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. type is invalid.");
    }

    if (!is_find) return CfdErrorCode::kCfdNotFoundError;
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

int CfdVerifyPsbtTxIn(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }
    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }

    Txid txid_obj(txid);
    psbt_obj->psbt->Verify(OutPoint(txid_obj, vout));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
    if (std::string(except.what()) == "psbt txin not finalized yet.") {
      result = kCfdSignVerificationError;
    }
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdInitializeFundPsbt(void* handle, void** fund_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiPsbtFundHandle* buffer = nullptr;
  try {
    cfd::Initialize();
    if (fund_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "fund handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. fund handle is null.");
    }

    buffer = static_cast<CfdCapiPsbtFundHandle*>(
        AllocBuffer(kPrefixPsbtFundHandle, sizeof(CfdCapiPsbtFundHandle)));
    buffer->utxos = new std::vector<UtxoData>();
    buffer->indexes = new std::vector<uint32_t>();
    buffer->fee_rate = 20.0;
    buffer->long_term_fee_rate = 20.0;
    buffer->dust_fee_rate = 3.0;
    buffer->knapsack_min_change = -1;
    *fund_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  CfdFreeFundPsbt(handle, buffer);
  return result;
}

int CfdFundPsbtAddToUtxoList(
    void* handle, void* fund_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* asset, const char* descriptor,
    const char* scriptsig_template, const char* full_utxo_tx) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixPsbtFundHandle);
    CfdCapiPsbtFundHandle* buffer =
        static_cast<CfdCapiPsbtFundHandle*>(fund_handle);
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
    if ((buffer->indexes == nullptr) || (buffer->utxos == nullptr)) {
      warn(CFD_LOG_SOURCE, "invalid handle statement.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to statement. invalid handle statement.");
    }
    if (!buffer->indexes->empty()) {
      warn(CFD_LOG_SOURCE, "already funded.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to statement. already funded.");
    }

    UtxoData utxo;
    utxo.address_type = AddressType::kP2shAddress;
    utxo.block_height = 0;
    utxo.binary_data = nullptr;
    utxo.txid = Txid(txid);
    utxo.vout = vout;
    utxo.amount = Amount(amount);
    utxo.descriptor = std::string(descriptor);
    if (!IsEmptyString(scriptsig_template)) {
      utxo.scriptsig_template = Script(std::string(scriptsig_template));
    }
    if (!IsEmptyString(asset)) {
      // do nothing
    }
    if (!IsEmptyString(full_utxo_tx)) {
      // TODO(k-matsuzawa) add full utxo tx with list
      // (If necessary, fix it at the same time as FundPsbt.)
    }
    buffer->utxos->push_back(utxo);
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

int CfdSetOptionFundPsbt(
    void* handle, void* fund_handle, int key, int64_t int64_value,
    double double_value, bool bool_value) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixPsbtFundHandle);
    CfdCapiPsbtFundHandle* buffer =
        static_cast<CfdCapiPsbtFundHandle*>(fund_handle);

    switch (key) {
      case kCfdPsbtFundEstimateFeeRate:
        buffer->fee_rate = double_value;
        break;
      case kCfdPsbtFundDustFeeRate:
        buffer->dust_fee_rate = double_value;
        break;
      case kCfdPsbtFundLongTermFeeRate:
        buffer->long_term_fee_rate = double_value;
        break;
      case kCfdPsbtFundKnapsackMinChange:
        buffer->knapsack_min_change = int64_value;
        break;
      case kCfdPsbtFundIsBlind:
        if (bool_value) {
          // do nothing
        }
        break;
      case kCfdPsbtFundBlindExponent:
        break;
      case kCfdPsbtFundBlindMinimumBits:
        if (int64_value >= 0) {
          // do nothing
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

int CfdFinalizeFundPsbt(
    void* handle, void* psbt_handle, void* fund_handle,
    const char* change_address_descriptor, int64_t* tx_fee,
    uint32_t* used_utxo_count) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(psbt_handle, kPrefixPsbtHandle);
    CheckBuffer(fund_handle, kPrefixPsbtFundHandle);
    CfdCapiPsbtHandle* psbt_obj = static_cast<CfdCapiPsbtHandle*>(psbt_handle);
    CfdCapiPsbtFundHandle* buffer =
        static_cast<CfdCapiPsbtFundHandle*>(fund_handle);

    if (psbt_obj->psbt == nullptr) {
      warn(CFD_LOG_SOURCE, "psbt is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to handle statement. psbt is null.");
    }
    if ((buffer->indexes == nullptr) || (buffer->utxos == nullptr)) {
      warn(CFD_LOG_SOURCE, "invalid handle statement.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to statement. invalid handle statement.");
    }

    CoinSelectionOption option_params;
    option_params.SetEffectiveFeeBaserate(buffer->fee_rate);
    option_params.SetLongTermFeeBaserate(buffer->long_term_fee_rate);
    option_params.SetDustFeeRate(buffer->dust_fee_rate);
    option_params.SetKnapsackMinimumChange(buffer->knapsack_min_change);

    UtxoFilter filter;
    Amount tx_fee_value;
    option_params.InitializeTxSizeInfo();  // bitcoin
    Descriptor* change_address_ptr = nullptr;
    Descriptor change_address;
    if (!IsEmptyString(change_address_descriptor)) {
      change_address = Descriptor::Parse(change_address_descriptor);
      change_address_ptr = &change_address;
    }

    auto used_utxos = psbt_obj->psbt->FundTransaction(
        *(buffer->utxos), buffer->fee_rate, change_address_ptr, &tx_fee_value,
        &option_params, &filter, psbt_obj->net_type);
    {
      buffer->indexes->clear();
      buffer->indexes->reserve(used_utxos.size());

      bool is_find;
      uint32_t utxo_index = 0;
      for (const auto& used_utxo : used_utxos) {
        is_find = false;
        for (size_t index = 0; index < buffer->utxos->size(); ++index) {
          const auto& utxo = buffer->utxos->at(index);
          if ((used_utxo.vout == utxo.vout) &&
              (used_utxo.txid.Equals(utxo.txid))) {
            is_find = true;
            utxo_index = static_cast<uint32_t>(index);
            break;
          }
        }
        if (!is_find) {
          warn(CFD_LOG_SOURCE, "invalid utxo state.");
          throw CfdException(
              CfdError::kCfdIllegalStateError,
              "Failed to statement. invalid utxo state.");
        }
        buffer->indexes->push_back(utxo_index);
      }
    }

    if (tx_fee != nullptr) *tx_fee = tx_fee_value.GetSatoshiValue();
    if (used_utxo_count != nullptr) {
      *used_utxo_count = static_cast<uint32_t>(used_utxos.size());
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

int CfdGetFundPsbtUsedUtxo(
    void* handle, void* fund_handle, uint32_t index, uint32_t* utxo_index,
    char** txid, uint32_t* vout, int64_t* amount, char** asset,
    char** descriptor, char** scriptsig_template) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_txid = nullptr;
  char* work_descriptor = nullptr;
  char* work_asset = nullptr;
  char* work_scriptsig = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(fund_handle, kPrefixPsbtFundHandle);
    CfdCapiPsbtFundHandle* buffer =
        static_cast<CfdCapiPsbtFundHandle*>(fund_handle);

    if ((buffer->indexes == nullptr) || (buffer->indexes->size() <= index)) {
      warn(CFD_LOG_SOURCE, "index out of range.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. index is out of range.");
    }
    uint32_t target_index = buffer->indexes->at(index);
    if (utxo_index != nullptr) *utxo_index = target_index;

    UtxoData utxo = buffer->utxos->at(target_index);
    if (vout != nullptr) *vout = utxo.vout;
    if (amount != nullptr) *amount = utxo.amount.GetSatoshiValue();

    if (txid != nullptr) work_txid = CreateString(utxo.txid.GetHex());
#ifndef CFD_DISABLE_ELEMENTS
    if (asset != nullptr) work_asset = CreateString(utxo.asset.GetHex());
#endif  // CFD_DISABLE_ELEMENTS
    if (descriptor != nullptr) work_descriptor = CreateString(utxo.descriptor);
    if (scriptsig_template != nullptr) {
      work_scriptsig = CreateString(utxo.scriptsig_template.GetHex());
    }

    if (txid != nullptr) *txid = work_txid;
#ifndef CFD_DISABLE_ELEMENTS
    if (asset != nullptr) *asset = work_asset;
#endif  // CFD_DISABLE_ELEMENTS
    if (descriptor != nullptr) *descriptor = work_descriptor;
    if (scriptsig_template != nullptr) *scriptsig_template = work_scriptsig;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(
      &work_txid, &work_descriptor, &work_asset, &work_scriptsig);
  return result;
}

int CfdFreeFundPsbt(void* handle, void* fund_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (fund_handle != nullptr) {
      CheckBuffer(fund_handle, kPrefixPsbtFundHandle);
      CfdCapiPsbtFundHandle* buffer =
          static_cast<CfdCapiPsbtFundHandle*>(fund_handle);
      if (buffer->utxos != nullptr) {
        delete buffer->utxos;
        buffer->utxos = nullptr;
      }
      if (buffer->indexes != nullptr) {
        delete buffer->indexes;
        buffer->indexes = nullptr;
      }
      FreeBuffer(
          fund_handle, kPrefixPsbtFundHandle, sizeof(CfdCapiPsbtFundHandle));
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

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
