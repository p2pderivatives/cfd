// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_elements_transaction.cpp
 *
 * @brief cfd-capiで利用するConfidentialTransaction処理の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#ifndef CFD_DISABLE_ELEMENTS
#include <exception>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfdapi_coin.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

using cfd::ConfidentialTransactionController;
using cfd::ElementsAddressFactory;
using cfd::SignParameter;
using cfd::api::ElementsTransactionApi;
using cfd::api::ElementsUtxoAndOption;
using cfd::api::IssuanceOutput;
using cfd::api::TxInBlindParameters;
using cfd::api::TxInReissuanceParameters;
using cfd::api::TxOutBlindKeys;
using cfd::api::UtxoData;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlindFactor;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxIn;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOut;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::HashType;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptUtil;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::UnblindParameter;
using cfd::core::WitnessVersion;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: Blind Transaction Data
constexpr const char* const kPrefixBlindTxData = "BlindTxData";

/**
 * @brief cfd-capi Blind Transaction Data構造体.
 * @details 最大情報量が多すぎるため、flat structにはしない。
 */
struct CfdCapiBlindTxData {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! txin list
  std::vector<TxInBlindParameters>* txin_blind_keys;
  //! txout list
  std::vector<TxOutBlindKeys>* txout_blind_keys;
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiBlindTxData;
using cfd::capi::CfdCapiMultisigSignData;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kEmpty32Bytes;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPrefixBlindTxData;
using cfd::capi::kPrefixMultisigSignData;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

// API
extern "C" {

int CfdInitializeConfidentialTx(
    void* handle, uint32_t version, uint32_t locktime, char** tx_string) {
  try {
    cfd::Initialize();
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    ConfidentialTransactionController ctxc(version, locktime);
    *tx_string = CreateString(ctxc.GetHex());

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

int CfdAddConfidentialTxIn(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    uint32_t sequence, char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    ctxc.AddTxIn(Txid(txid), vout, sequence);
    *tx_string = CreateString(ctxc.GetHex());

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

int CfdAddConfidentialTxOut(
    void* handle, const char* tx_hex_string, const char* asset_string,
    int64_t value_satoshi, const char* value_commitment, const char* address,
    const char* direct_locking_script, const char* nonce, char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }
    if (IsEmptyString(asset_string)) {
      warn(CFD_LOG_SOURCE, "asset is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. asset is null.");
    }
    if ((value_satoshi <= 0) && (IsEmptyString(value_commitment))) {
      warn(CFD_LOG_SOURCE, "value is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. value is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);

    Amount amount;
    if (IsEmptyString(value_commitment)) {
      amount = Amount::CreateBySatoshiAmount(value_satoshi);
    } else {
      ConfidentialValue value = ConfidentialValue(value_commitment);
      // unconfidential only
      if (!value.HasBlinding()) amount = value.GetAmount();
    }

    // add TxOut
    ElementsAddressFactory address_factory;
    ConfidentialAssetId asset_obj(asset_string);
    ConfidentialNonce nonce_obj;
    if (!IsEmptyString(nonce)) {
      nonce_obj = ConfidentialNonce(nonce);
    }
    if (!IsEmptyString(address)) {
      // const std::string addr(address);
      if (ElementsConfidentialAddress::IsConfidentialAddress(address)) {
        ElementsConfidentialAddress confidential_addr(address);
        ctxc.AddTxOut(confidential_addr, amount, asset_obj, false);
      } else {
        ctxc.AddTxOut(address_factory.GetAddress(address), amount, asset_obj);
      }
    } else if (!IsEmptyString(direct_locking_script)) {
      Script script(direct_locking_script);
      ctxc.AddTxOut(script, amount, asset_obj, nonce_obj);
    } else if (amount > 0) {
      // fee
      ctxc.AddTxOutFee(amount, asset_obj);
    }

    *tx_string = CreateString(ctxc.GetHex());

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

int CfdUpdateConfidentialTxOut(
    void* handle, const char* tx_hex_string, uint32_t index,
    const char* asset_string, int64_t value_satoshi,
    const char* value_commitment, const char* address,
    const char* direct_locking_script, const char* nonce, char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }
    if (IsEmptyString(asset_string)) {
      warn(CFD_LOG_SOURCE, "asset is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. asset is null.");
    }
    if ((value_satoshi <= 0) && (IsEmptyString(value_commitment))) {
      warn(CFD_LOG_SOURCE, "value is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. value is null.");
    }

    ConfidentialTransaction ctx(tx_hex_string);

    ConfidentialValue value;
    if (IsEmptyString(value_commitment)) {
      value = ConfidentialValue(Amount::CreateBySatoshiAmount(value_satoshi));
    } else {
      ConfidentialValue temp_value = ConfidentialValue(value_commitment);
      // unconfidential only
      if (!temp_value.HasBlinding()) value = temp_value;
    }

    // add TxOut
    ElementsAddressFactory address_factory;
    ConfidentialAssetId asset_obj(asset_string);
    ConfidentialNonce nonce_obj;
    if (!IsEmptyString(nonce)) {
      nonce_obj = ConfidentialNonce(std::string(nonce));
    }
    Script locking_script;
    if (!IsEmptyString(address)) {
      const std::string addr(address);
      if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
        ElementsConfidentialAddress confidential_addr(addr);
        locking_script = confidential_addr.GetLockingScript();
      } else {
        Address unconfidential_addr = address_factory.GetAddress(addr);
        locking_script = unconfidential_addr.GetLockingScript();
      }
    } else if (!IsEmptyString(direct_locking_script)) {
      locking_script = Script(std::string(direct_locking_script));
    }

    ctx.SetTxOutCommitment(
        index, asset_obj, value, nonce_obj, ByteData(), ByteData());
    *tx_string = CreateString(ctx.GetHex());

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

int CfdGetConfidentialTxInfo(
    void* handle, const char* tx_hex_string, char** txid, char** wtxid,
    char** wit_hash, uint32_t* size, uint32_t* vsize, uint32_t* weight,
    uint32_t* version, uint32_t* locktime) {
  int error_code = CfdErrorCode::kCfdUnknownError;
  char* work_txid = nullptr;
  char* work_wtxid = nullptr;
  char* work_wit_hash = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();

    if (txid != nullptr) {
      work_txid = CreateString(tx.GetTxid().GetHex());
    }
    if (wtxid != nullptr) {
      work_wtxid = CreateString(Txid(tx.GetWitnessHash()).GetHex());
    }
    if (wit_hash != nullptr) {
      work_wit_hash = CreateString(Txid(tx.GetWitnessOnlyHash()).GetHex());
    }
    if (size != nullptr) {
      *size = tx.GetTotalSize();
    }
    if (vsize != nullptr) {
      *vsize = tx.GetVsize();
    }
    if (weight != nullptr) {
      *weight = tx.GetWeight();
    }
    if (version != nullptr) {
      *version = tx.GetVersion();
    }
    if (locktime != nullptr) {
      *locktime = tx.GetLockTime();
    }

    if (work_txid != nullptr) *txid = work_txid;
    if (work_wtxid != nullptr) *wtxid = work_wtxid;
    if (work_wit_hash != nullptr) *wit_hash = work_wit_hash;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    error_code = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_txid, &work_wtxid, &work_wit_hash);
  return error_code;
}

int CfdGetConfidentialTxIn(
    void* handle, const char* tx_hex_string, uint32_t index, char** txid,
    uint32_t* vout, uint32_t* sequence, char** script_sig) {
  char* work_txid = nullptr;
  char* work_script_sig = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();
    const ConfidentialTxInReference ref = tx.GetTxIn(index);

    if (txid != nullptr) {
      work_txid = CreateString(ref.GetTxid().GetHex());
    }
    if (vout != nullptr) {
      *vout = ref.GetVout();
    }
    if (sequence != nullptr) {
      *sequence = ref.GetSequence();
    }
    if (script_sig != nullptr) {
      work_script_sig = CreateString(ref.GetUnlockingScript().GetHex());
    }

    if (work_txid != nullptr) *txid = work_txid;
    if (work_script_sig != nullptr) *script_sig = work_script_sig;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(&work_txid, &work_script_sig);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(&work_txid, &work_script_sig);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(&work_txid, &work_script_sig);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetConfidentialTxInWitness(
    void* handle, const char* tx_hex_string, uint32_t txin_index,
    uint32_t stack_index, char** stack_data) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (stack_data == nullptr) {
      warn(CFD_LOG_SOURCE, "stack data is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. stack data is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();
    const ConfidentialTxInReference ref = tx.GetTxIn(txin_index);

    const std::vector<ByteData> witness_stack =
        ref.GetScriptWitness().GetWitness();
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

int CfdGetTxInIssuanceInfo(
    void* handle, const char* tx_hex_string, uint32_t index, char** entropy,
    char** nonce, int64_t* asset_amount, char** asset_value,
    int64_t* token_amount, char** token_value, char** asset_rangeproof,
    char** token_rangeproof) {
  char* work_entropy = nullptr;
  char* work_nonce = nullptr;
  char* work_asset_value = nullptr;
  char* work_token_value = nullptr;
  char* work_asset_rangeproof = nullptr;
  char* work_token_rangeproof = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();
    const ConfidentialTxInReference ref = tx.GetTxIn(index);

    if (entropy != nullptr) {
      work_entropy = CreateString(BlindFactor(ref.GetAssetEntropy()).GetHex());
    }
    if (nonce != nullptr) {
      work_nonce = CreateString(BlindFactor(ref.GetBlindingNonce()).GetHex());
    }
    const ConfidentialValue& asset_obj = ref.GetIssuanceAmount();
    const ConfidentialValue& token_obj = ref.GetInflationKeys();
    if ((asset_amount != nullptr) && (!asset_obj.HasBlinding())) {
      *asset_amount = (asset_obj.HasBlinding())
                          ? 0
                          : asset_obj.GetAmount().GetSatoshiValue();
    }
    if (asset_value != nullptr) {
      work_asset_value = CreateString(asset_obj.GetHex());
    }
    if ((token_amount != nullptr) && (!token_obj.HasBlinding())) {
      *token_amount = (token_obj.HasBlinding())
                          ? 0
                          : token_obj.GetAmount().GetSatoshiValue();
    }
    if (token_value != nullptr) {
      work_token_value = CreateString(token_obj.GetHex());
    }
    if (asset_rangeproof != nullptr) {
      work_asset_rangeproof =
          CreateString(ref.GetIssuanceAmountRangeproof().GetHex());
    }
    if (token_rangeproof != nullptr) {
      work_token_rangeproof =
          CreateString(ref.GetInflationKeysRangeproof().GetHex());
    }

    if (work_entropy != nullptr) *entropy = work_entropy;
    if (work_nonce != nullptr) *nonce = work_nonce;
    if (work_asset_value != nullptr) *asset_value = work_asset_value;
    if (work_token_value != nullptr) *token_value = work_token_value;
    if (work_asset_rangeproof != nullptr)
      *asset_rangeproof = work_asset_rangeproof;
    if (work_token_rangeproof != nullptr)
      *token_rangeproof = work_token_rangeproof;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(
        &work_entropy, &work_nonce, &work_asset_value, &work_token_value,
        &work_asset_rangeproof, &work_token_rangeproof);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(
        &work_entropy, &work_nonce, &work_asset_value, &work_token_value,
        &work_asset_rangeproof, &work_token_rangeproof);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(
        &work_entropy, &work_nonce, &work_asset_value, &work_token_value,
        &work_asset_rangeproof, &work_token_rangeproof);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetConfidentialTxOut(
    void* handle, const char* tx_hex_string, uint32_t index,
    char** asset_string, int64_t* value_satoshi, char** value_commitment,
    char** nonce, char** locking_script, char** surjection_proof,
    char** rangeproof) {
  char* work_asset_string = nullptr;
  char* work_value_commitment = nullptr;
  char* work_nonce = nullptr;
  char* work_locking_script = nullptr;
  char* work_surjection_proof = nullptr;
  char* work_rangeproof = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();
    const ConfidentialTxOutReference ref = tx.GetTxOut(index);

    if (asset_string != nullptr) {
      work_asset_string = CreateString(ref.GetAsset().GetHex());
    }
    ConfidentialValue value = ref.GetConfidentialValue();
    if ((value_satoshi != nullptr) && (!value.HasBlinding())) {
      *value_satoshi = value.GetAmount().GetSatoshiValue();
    }
    if (value_commitment != nullptr) {
      work_value_commitment = CreateString(value.GetHex());
    }
    if (nonce != nullptr) {
      work_nonce = CreateString(ref.GetNonce().GetHex());
    }
    if (locking_script != nullptr) {
      work_locking_script = CreateString(ref.GetLockingScript().GetHex());
    }
    if (surjection_proof != nullptr) {
      work_surjection_proof = CreateString(ref.GetSurjectionProof().GetHex());
    }
    if (rangeproof != nullptr) {
      work_rangeproof = CreateString(ref.GetRangeProof().GetHex());
    }

    if (work_asset_string != nullptr) *asset_string = work_asset_string;
    if (work_value_commitment != nullptr)
      *value_commitment = work_value_commitment;
    if (work_nonce != nullptr) *nonce = work_nonce;
    if (work_locking_script != nullptr) *locking_script = work_locking_script;
    if (work_surjection_proof != nullptr)
      *surjection_proof = work_surjection_proof;
    if (work_rangeproof != nullptr) *rangeproof = work_rangeproof;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(
        &work_asset_string, &work_value_commitment, &work_nonce,
        &work_locking_script, &work_surjection_proof, &work_rangeproof);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(
        &work_asset_string, &work_value_commitment, &work_nonce,
        &work_locking_script, &work_surjection_proof, &work_rangeproof);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(
        &work_asset_string, &work_value_commitment, &work_nonce,
        &work_locking_script, &work_surjection_proof, &work_rangeproof);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetConfidentialTxInCount(
    void* handle, const char* tx_hex_string, uint32_t* count) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();

    if (count != nullptr) {
      *count = tx.GetTxInCount();
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

int CfdGetConfidentialTxInWitnessCount(
    void* handle, const char* tx_hex_string, uint32_t txin_index,
    uint32_t* count) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();
    const ConfidentialTxInReference ref = tx.GetTxIn(txin_index);

    if (count != nullptr) {
      *count = ref.GetScriptWitnessStackNum();
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

int CfdGetConfidentialTxOutCount(
    void* handle, const char* tx_hex_string, uint32_t* count) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();

    if (count != nullptr) {
      *count = tx.GetTxOutCount();
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

int CfdSetRawReissueAsset(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    int64_t asset_amount, const char* blinding_nonce, const char* entropy,
    const char* address, const char* direct_locking_script,
    char** asset_string, char** tx_string) {
  char* work_asset_string = nullptr;
  char* work_tx_string = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output tx is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (IsEmptyString(blinding_nonce)) {
      warn(CFD_LOG_SOURCE, "blinding nonce is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. blinding nonce is null.");
    }
    if (IsEmptyString(entropy)) {
      warn(CFD_LOG_SOURCE, "entropy is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. entropy is null.");
    }

    ConfidentialTxOut asset_txout;
    Amount amount = Amount::CreateBySatoshiAmount(asset_amount);
    ElementsAddressFactory address_factory;
    if (!IsEmptyString(address)) {
      std::string addr(address);
      if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
        ElementsConfidentialAddress confidential_addr(addr);
        asset_txout = ConfidentialTxOut(
            confidential_addr, ConfidentialAssetId(), amount);
      } else {
        Address unblind_addr = address_factory.GetAddress(addr);
        asset_txout =
            ConfidentialTxOut(unblind_addr, ConfidentialAssetId(), amount);
      }
    } else if (!IsEmptyString(direct_locking_script)) {
      Script script(direct_locking_script);
      asset_txout = ConfidentialTxOut(
          script, ConfidentialAssetId(), ConfidentialValue(amount));
    }

    TxInReissuanceParameters params;
    params.txid = Txid(std::string(txid));
    params.vout = vout;
    params.amount = amount;
    params.asset_txout = asset_txout;
    params.blind_factor = BlindFactor(std::string(blinding_nonce));
    params.entropy = BlindFactor(std::string(entropy));

    ElementsTransactionApi api;
    std::vector<IssuanceOutput> outputs;
    std::vector<TxInReissuanceParameters> issuances{params};
    ConfidentialTransactionController ctxc =
        api.SetRawReissueAsset(tx_hex_string, issuances, &outputs);

    if (!outputs.empty() && (asset_string != nullptr)) {
      work_asset_string = CreateString(outputs[0].output.asset.GetHex());
    }
    work_tx_string = CreateString(ctxc.GetHex());

    if (work_asset_string != nullptr) *asset_string = work_asset_string;
    *tx_string = work_tx_string;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(&work_asset_string, &work_tx_string);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(&work_asset_string, &work_tx_string);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(&work_asset_string, &work_tx_string);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetIssuanceBlindingKey(
    void* handle, const char* master_blinding_key, const char* txid,
    uint32_t vout, char** blinding_key) {
  try {
    cfd::Initialize();
    if (IsEmptyString(master_blinding_key)) {
      warn(CFD_LOG_SOURCE, "master blinding key is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. master blinding key is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (blinding_key == nullptr) {
      warn(CFD_LOG_SOURCE, "blinding key is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. blinding key is null.");
    }

    ElementsTransactionApi api;
    Privkey privkey = api.GetIssuanceBlindingKey(
        Privkey(std::string(master_blinding_key)), Txid(txid),
        static_cast<int32_t>(vout));

    *blinding_key = CreateString(privkey.GetHex());

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

int CfdInitializeBlindTx(void* handle, void** blind_handle) {
  CfdCapiBlindTxData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (blind_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "blind handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. blind handle is null.");
    }

    buffer = static_cast<CfdCapiBlindTxData*>(
        AllocBuffer(kPrefixBlindTxData, sizeof(CfdCapiBlindTxData)));
    buffer->txin_blind_keys = new std::vector<TxInBlindParameters>();
    buffer->txout_blind_keys = new std::vector<TxOutBlindKeys>();

    *blind_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    if (buffer != nullptr) CfdFreeBlindHandle(handle, buffer);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    if (buffer != nullptr) CfdFreeBlindHandle(handle, buffer);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    if (buffer != nullptr) CfdFreeBlindHandle(handle, buffer);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdAddBlindTxInData(
    void* handle, void* blind_handle, const char* txid, uint32_t vout,
    const char* asset_string, const char* asset_blind_factor,
    const char* value_blind_vactor, int64_t value_satoshi,
    const char* asset_key, const char* token_key) {
  try {
    cfd::Initialize();
    CheckBuffer(blind_handle, kPrefixBlindTxData);

    CfdCapiBlindTxData* buffer =
        static_cast<CfdCapiBlindTxData*>(blind_handle);
    if (buffer->txin_blind_keys == nullptr) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (IsEmptyString(asset_string)) {
      warn(CFD_LOG_SOURCE, "asset is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. asset is null.");
    }

    TxInBlindParameters params;
    params.txid = Txid(txid);
    params.vout = vout;
    params.blind_param.asset = ConfidentialAssetId(asset_string);
    params.blind_param.value =
        ConfidentialValue(Amount::CreateBySatoshiAmount(value_satoshi));
    if (IsEmptyString(asset_blind_factor)) {
      params.blind_param.abf = BlindFactor(kEmpty32Bytes);
    } else {
      params.blind_param.abf = BlindFactor(asset_blind_factor);
    }
    if (IsEmptyString(value_blind_vactor)) {
      params.blind_param.vbf = BlindFactor(kEmpty32Bytes);
    } else {
      params.blind_param.vbf = BlindFactor(value_blind_vactor);
    }

    if (!IsEmptyString(asset_key)) {
      params.is_issuance = true;
      params.issuance_key.asset_key = Privkey(asset_key);
    }
    if (!IsEmptyString(token_key)) {
      params.is_issuance = true;
      params.issuance_key.token_key = Privkey(token_key);
    }

    buffer->txin_blind_keys->push_back(params);
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

int CfdAddBlindTxOutData(
    void* handle, void* blind_handle, uint32_t index,
    const char* confidential_key) {
  try {
    cfd::Initialize();
    CheckBuffer(blind_handle, kPrefixBlindTxData);

    CfdCapiBlindTxData* buffer =
        static_cast<CfdCapiBlindTxData*>(blind_handle);
    if (buffer->txout_blind_keys == nullptr) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }
    if (confidential_key == nullptr) {
      warn(CFD_LOG_SOURCE, "confidential key is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. confidential key is null.");
    }

    TxOutBlindKeys params;
    params.index = index;
    params.blinding_key = Pubkey(confidential_key);

    buffer->txout_blind_keys->push_back(params);
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

int CfdFinalizeBlindTx(
    void* handle, void* blind_handle, const char* tx_hex_string,
    char** tx_string) {
  try {
    cfd::Initialize();
    CheckBuffer(blind_handle, kPrefixBlindTxData);
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. output tx is null.");
    }

    CfdCapiBlindTxData* buffer =
        static_cast<CfdCapiBlindTxData*>(blind_handle);
    if ((buffer->txin_blind_keys == nullptr) ||
        (buffer->txout_blind_keys == nullptr)) {
      warn(CFD_LOG_SOURCE, "buffer state is illegal.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. buffer state is illegal.");
    }
    if (buffer->txin_blind_keys->empty()) {
      warn(CFD_LOG_SOURCE, "txin blind data is empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txin blind data is empty.");
    }
    if (buffer->txout_blind_keys->empty()) {
      warn(CFD_LOG_SOURCE, "txout blind data is empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txout blind data is empty.");
    }

    bool is_issuance_blinding = false;
    for (const auto& txin_blind_data : *buffer->txin_blind_keys) {
      if (txin_blind_data.is_issuance) {
        is_issuance_blinding = true;
        break;
      }
    }

    ElementsTransactionApi api;
    ConfidentialTransactionController ctxc = api.BlindTransaction(
        tx_hex_string, *buffer->txin_blind_keys, *buffer->txout_blind_keys,
        is_issuance_blinding);
    *tx_string = CreateString(ctxc.GetHex());

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

int CfdFreeBlindHandle(void* handle, void* blind_handle) {
  try {
    cfd::Initialize();
    if (blind_handle != nullptr) {
      CfdCapiBlindTxData* blind_tx_struct =
          static_cast<CfdCapiBlindTxData*>(blind_handle);
      if (blind_tx_struct->txin_blind_keys != nullptr) {
        delete blind_tx_struct->txin_blind_keys;
        blind_tx_struct->txin_blind_keys = nullptr;
      }
      if (blind_tx_struct->txout_blind_keys != nullptr) {
        delete blind_tx_struct->txout_blind_keys;
        blind_tx_struct->txout_blind_keys = nullptr;
      }
    }
    FreeBuffer(blind_handle, kPrefixBlindTxData, sizeof(CfdCapiBlindTxData));
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

int CfdAddConfidentialTxSign(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    bool is_witness, const char* sign_data_hex, bool clear_stack,
    char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (IsEmptyString(sign_data_hex)) {
      warn(CFD_LOG_SOURCE, "sign data is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sign data is null.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    std::vector<SignParameter> sign_list;
    // SignParameter param();
    sign_list.emplace_back(std::string(sign_data_hex));

    ElementsTransactionApi api;
    ConfidentialTransactionController ctxc = api.AddSign(
        std::string(tx_hex_string), Txid(std::string(txid)), vout, sign_list,
        is_witness, clear_stack);
    *tx_string = CreateString(ctxc.GetHex());

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

int CfdAddConfidentialTxDerSign(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    bool is_witness, const char* signature, int sighash_type,
    bool sighash_anyone_can_pay, bool clear_stack, char** tx_string) {
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }

    // encode to der
    SigHashType sighashtype(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
    ByteData signature_bytes = ByteData(std::string(signature));
    SignParameter param(signature_bytes, true, sighashtype);
    ByteData signature_der = param.ConvertToSignature();

    return CfdAddConfidentialTxSign(
        handle, tx_hex_string, txid, vout, is_witness,
        signature_der.GetHex().c_str(), clear_stack, tx_string);
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

int CfdFinalizeElementsMultisigSign(
    void* handle, void* multisign_handle, const char* tx_hex_string,
    const char* txid, uint32_t vout, int hash_type, const char* witness_script,
    const char* redeem_script, bool clear_stack, char** tx_string) {
  try {
    cfd::Initialize();
    CheckBuffer(multisign_handle, kPrefixMultisigSignData);
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (tx_string == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    Script witness_script_obj;
    Script redeem_script_obj;
    AddressType addr_type = ConvertHashToAddressType(hash_type);
    switch (addr_type) {
      case AddressType::kP2shAddress:
        if (IsEmptyString(redeem_script)) {
          warn(CFD_LOG_SOURCE, "redeemScript is null.");
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "Failed to parameter. redeemScript is null.");
        }
        redeem_script_obj = Script(std::string(redeem_script));
        break;
      case AddressType::kP2wshAddress:
        // fall-through
      case AddressType::kP2shP2wshAddress:
        if (IsEmptyString(witness_script)) {
          warn(CFD_LOG_SOURCE, "witnessScript is null.");
          throw CfdException(
              CfdError::kCfdIllegalArgumentError,
              "Failed to parameter. witnessScript is null.");
        }
        witness_script_obj = Script(std::string(witness_script));
        break;
      default:
        warn(CFD_LOG_SOURCE, "pkh is not target.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. hash type cannot used pkh target.");
    }

    if (addr_type == AddressType::kP2shP2wshAddress) {
      if (IsEmptyString(redeem_script)) {
        redeem_script_obj =
            ScriptUtil::CreateP2shLockingScript(witness_script_obj);
      } else {
        redeem_script_obj = Script(std::string(redeem_script));
      }
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

    ElementsTransactionApi api;
    ConfidentialTransactionController ctxc = api.AddMultisigSign(
        std::string(tx_hex_string), Txid(std::string(txid)), vout, sign_list,
        addr_type, witness_script_obj, redeem_script_obj, clear_stack);
    *tx_string = CreateString(ctxc.GetHex());

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

int CfdCreateConfidentialSighash(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    int hash_type, const char* pubkey, const char* redeem_script,
    int64_t value_satoshi, const char* value_commitment, int sighash_type,
    bool sighash_anyone_can_pay, char** sighash) {
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (sighash == nullptr) {
      warn(CFD_LOG_SOURCE, "sighash is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. sighash is null.");
    }

    AddressType addr_type = ConvertHashToAddressType(hash_type);
    HashType core_hash_type;
    switch (addr_type) {
      case AddressType::kP2shAddress:
        core_hash_type = HashType::kP2sh;
        break;
      case AddressType::kP2wshAddress:
        // fall-through
      case AddressType::kP2shP2wshAddress:
        core_hash_type = HashType::kP2wsh;
        break;
      case AddressType::kP2pkhAddress:
        core_hash_type = HashType::kP2pkh;
        break;
      case AddressType::kP2wpkhAddress:
        // fall-through
      case AddressType::kP2shP2wpkhAddress:
        core_hash_type = HashType::kP2wpkh;
        break;
      default:
        warn(CFD_LOG_SOURCE, "hashtype is illegal.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. hashtype is illegal.");
    }

    ByteData key_data;
    if ((core_hash_type == HashType::kP2sh) ||
        (core_hash_type == HashType::kP2wsh)) {
      if (IsEmptyString(redeem_script)) {
        warn(CFD_LOG_SOURCE, "redeem script is null.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. redeem script is null.");
      }
      Script redeem_script_obj = Script(redeem_script);
      key_data = redeem_script_obj.GetData();
    } else {
      if (IsEmptyString(pubkey)) {
        warn(CFD_LOG_SOURCE, "pubkey is null.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. pubkey is null.");
      }
      Pubkey pubkey_obj = Pubkey(pubkey);
      key_data = pubkey_obj.GetData();
    }

    ConfidentialValue value;
    if (IsEmptyString(value_commitment)) {
      value = ConfidentialValue(Amount::CreateBySatoshiAmount(value_satoshi));
    } else {
      value = ConfidentialValue(value_commitment);
    }

    ElementsTransactionApi api;
    SigHashType sighashtype(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
    std::string sighash_bytes = api.CreateSignatureHash(
        std::string(tx_hex_string), Txid(std::string(txid)), vout, key_data,
        value, core_hash_type, sighashtype);
    *sighash = CreateString(sighash_bytes);
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

int CfdUnblindTxOut(
    void* handle, const char* tx_hex_string, uint32_t tx_out_index,
    const char* blinding_key, char** asset, int64_t* value,
    char** asset_blind_factor, char** value_blind_factor) {
  char* work_asset = nullptr;
  char* work_asset_blind_factor = nullptr;
  char* work_value_blind_factor = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (IsEmptyString(blinding_key)) {
      warn(CFD_LOG_SOURCE, "blinding key is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. blinding key is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    UnblindParameter unblind_data =
        ctxc.UnblindTxOut(tx_out_index, Privkey(blinding_key));

    if (!unblind_data.asset.IsEmpty()) {
      if (asset != nullptr) {
        work_asset = CreateString(unblind_data.asset.GetHex());
      }
      if (value != nullptr) {
        *value = unblind_data.value.GetAmount().GetSatoshiValue();
      }
      if (asset_blind_factor != nullptr) {
        work_asset_blind_factor = CreateString(unblind_data.abf.GetHex());
      }
      if (value_blind_factor != nullptr) {
        work_value_blind_factor = CreateString(unblind_data.vbf.GetHex());
      }
    }

    if (work_asset != nullptr) *asset = work_asset;
    if (work_asset_blind_factor != nullptr)
      *asset_blind_factor = work_asset_blind_factor;
    if (work_value_blind_factor != nullptr)
      *value_blind_factor = work_value_blind_factor;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(
        &work_asset, &work_asset_blind_factor, &work_value_blind_factor);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(
        &work_asset, &work_asset_blind_factor, &work_value_blind_factor);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(
        &work_asset, &work_asset_blind_factor, &work_value_blind_factor);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdUnblindIssuance(
    void* handle, const char* tx_hex_string, uint32_t tx_in_index,
    const char* asset_blinding_key, const char* token_blinding_key,
    char** asset, int64_t* asset_value, char** asset_blind_factor,
    char** asset_value_blind_factor, char** token, int64_t* token_value,
    char** token_blind_factor, char** token_value_blind_factor) {
  char* work_asset = nullptr;
  char* work_asset_blind_factor = nullptr;
  char* work_asset_value_blind_factor = nullptr;
  char* work_token = nullptr;
  char* work_token_blind_factor = nullptr;
  char* work_token_value_blind_factor = nullptr;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex_string)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex_string);
    Privkey asset_key;
    Privkey token_key;
    if (!IsEmptyString(asset_blinding_key)) {
      asset_key = Privkey(asset_blinding_key);
    }
    if (!IsEmptyString(token_blinding_key)) {
      token_key = Privkey(token_blinding_key);
    }

    std::vector<UnblindParameter> unblind_datas =
        ctxc.UnblindIssuance(tx_in_index, asset_key, token_key);

    UnblindParameter unblind_asset;
    UnblindParameter unblind_token;
    if (unblind_datas.size() == 2) {  // 2固定
      unblind_asset = unblind_datas[0];
      unblind_token = unblind_datas[1];
    }

    if (!unblind_asset.asset.IsEmpty()) {
      if (asset != nullptr) {
        work_asset = CreateString(unblind_asset.asset.GetHex());
      }
      if (asset_value != nullptr) {
        *asset_value = unblind_asset.value.GetAmount().GetSatoshiValue();
      }
      if (asset_blind_factor != nullptr) {
        work_asset_blind_factor = CreateString(unblind_asset.abf.GetHex());
      }
      if (asset_value_blind_factor != nullptr) {
        work_asset_value_blind_factor =
            CreateString(unblind_asset.vbf.GetHex());
      }
    }
    if (!unblind_token.asset.IsEmpty()) {
      if (token != nullptr) {
        work_token = CreateString(unblind_token.asset.GetHex());
      }
      if (token_value != nullptr) {
        *token_value = unblind_token.value.GetAmount().GetSatoshiValue();
      }
      if (token_blind_factor != nullptr) {
        work_token_blind_factor = CreateString(unblind_token.abf.GetHex());
      }
      if (token_value_blind_factor != nullptr) {
        work_token_value_blind_factor =
            CreateString(unblind_token.vbf.GetHex());
      }
    }

    if (work_asset != nullptr) *asset = work_asset;
    if (work_asset_blind_factor != nullptr)
      *asset_blind_factor = work_asset_blind_factor;
    if (work_asset_value_blind_factor != nullptr)
      *asset_value_blind_factor = work_asset_value_blind_factor;
    if (work_token != nullptr) *token = work_token;
    if (work_token_blind_factor != nullptr)
      *token_blind_factor = work_token_blind_factor;
    if (work_token_value_blind_factor != nullptr)
      *token_value_blind_factor = work_token_value_blind_factor;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    FreeBufferOnError(
        &work_asset, &work_asset_blind_factor, &work_asset_value_blind_factor,
        &work_token, &work_token_blind_factor, &work_token_value_blind_factor);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    FreeBufferOnError(
        &work_asset, &work_asset_blind_factor, &work_asset_value_blind_factor,
        &work_token, &work_token_blind_factor, &work_token_value_blind_factor);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    FreeBufferOnError(
        &work_asset, &work_asset_blind_factor, &work_asset_value_blind_factor,
        &work_token, &work_token_blind_factor, &work_token_value_blind_factor);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

CFDC_API int CfdConfidentialTxVerifySignature(
    void* handle, const char* tx_hex, const char* signature,
    const char* pubkey, const char* script, const char* txid, uint32_t vout,
    int sighash_type, bool sighash_anyone_can_pay, int64_t value_satoshi,
    const char* value_commitment, int witness_version, bool* result) {
  bool work_result = false;
  try {
    cfd::Initialize();
    if (IsEmptyString(tx_hex)) {
      warn(CFD_LOG_SOURCE, "tx is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx is null.");
    }
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }
    if (IsEmptyString(pubkey)) {
      warn(CFD_LOG_SOURCE, "pubkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. pubkey is null.");
    }
    if (result == nullptr) {
      warn(CFD_LOG_SOURCE, "result is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. result is null.");
    }

    ConfidentialTransactionController ctxc(tx_hex);
    ByteData signature_obj(signature);
    Txid txid_obj(txid);
    SigHashType sighash_type_obj(
        static_cast<SigHashAlgorithm>(sighash_type), sighash_anyone_can_pay);
    ConfidentialValue value;
    if (!IsEmptyString(value_commitment)) {
      value = ConfidentialValue(value_commitment);
    } else {
      value = ConfidentialValue(Amount::CreateBySatoshiAmount(value_satoshi));
    }

    if (!IsEmptyString(script)) {
      work_result = ctxc.VerifyInputSignature(
          signature_obj, Pubkey(pubkey), txid_obj, vout, Script(script),
          sighash_type_obj, value,
          static_cast<WitnessVersion>(witness_version));
    } else if (!IsEmptyString(pubkey)) {
      work_result = ctxc.VerifyInputSignature(
          signature_obj, Pubkey(pubkey), txid_obj, vout, sighash_type_obj,
          value, static_cast<WitnessVersion>(witness_version));
    }

    *result = work_result;
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

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_DISABLE_CAPI
