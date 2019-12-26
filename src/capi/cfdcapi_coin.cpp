// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_coin.cpp
 *
 * @brief cfd-capiで利用するCoin操作の実装ファイル
 */
#ifndef CFD_DISABLE_CAPI
#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_coin.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfd/cfdapi_transaction.h"
#include "cfdc/cfdcapi_coin.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::AddressFactory;
using cfd::AmountMap;
using cfd::CoinSelection;
using cfd::CoinSelectionOption;
using cfd::Utxo;
using cfd::UtxoFilter;
using cfd::api::TransactionApi;
using cfd::api::UtxoData;
using cfd::core::Address;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Script;
using cfd::core::Txid;

using cfd::core::logger::info;
using cfd::core::logger::warn;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::ConfidentialAssetId;
using cfd::ElementsAddressFactory;
using cfd::api::ElementsTransactionApi;
using cfd::api::ElementsUtxoAndOption;
using cfd::core::ElementsConfidentialAddress;
#endif  // CFD_DISABLE_ELEMENTS

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: CoinSelection
constexpr const char* const kPrefixCoinSelection = "CoinSelection";
//! asset max size
constexpr uint32_t kAssetSize = 33;

/**
 * @brief cfd-capi TargetAsset構造体.
 */
struct CfdCapiTargetAsset {
  // coinselection option
  uint8_t asset[kAssetSize];  //!< target asset
  int64_t amount;             //!< target amount
  int64_t selected_amount;    //!< selected amount
};

/**
 * @brief cfd-capi CoinSelection構造体.
 */
struct CfdCapiCoinSelection {
  char prefix[kPrefixLength];  //!< buffer prefix
  // coinselection option
  uint8_t fee_asset[kAssetSize];  //!< fee asset
  int64_t tx_fee_amount;          //!< tx fee amount
  double fee_rate;                //!< feeRate
  double effective_fee_rate;      //!< effective feeRate
  double long_term_fee_rate;      //!< longterm feeRate
  double dust_fee_rate;           //!< dust feeRate
  int64_t knapsack_min_change;    //!< knapsack minChange
  //! utxo list
  std::vector<Utxo>* utxos;  //!< utxo list
  //! target list
  std::vector<CfdCapiTargetAsset>* targets;  //!< target list
  //! target list
  std::vector<int32_t>* indexes;  //!< select index list
};

//! prefix: data for fee estimation
constexpr const char* const kPrefixEstimateFeeData = "EstimateFeeData";

/**
 * @brief cfd-capi EstimateFeeData構造体.
 * @details 最大情報量が多すぎるため、flat structにはしない。
 */
struct CfdCapiEstimateFeeData {
  char prefix[kPrefixLength];  //!< buffer prefix
#ifndef CFD_DISABLE_ELEMENTS
  //! handle for elements
  bool is_elements = false;
  //! input utxo list for elements
  std::vector<ElementsUtxoAndOption>* input_elements_utxos;
#endif  // CFD_DISABLE_ELEMENTS
  //! input utxo list
  std::vector<UtxoData>* input_utxos;
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiCoinSelection;
using cfd::capi::CfdCapiEstimateFeeData;
using cfd::capi::CfdCapiTargetAsset;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kAssetSize;
using cfd::capi::kPrefixCoinSelection;
using cfd::capi::kPrefixEstimateFeeData;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

extern "C" {

int CfdInitializeCoinSelection(
    void* handle, uint32_t utxo_count, uint32_t target_asset_count,
    const char* fee_asset, int64_t tx_fee_amount, double effective_fee_rate,
    double long_term_fee_rate, double dust_fee_rate,
    int64_t knapsack_min_change, void** coin_select_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiCoinSelection* buffer = nullptr;
  try {
    cfd::Initialize();
    if (coin_select_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "coinselect handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. coinselect handle is null.");
    }
    CfdCapiCoinSelection obj;
    memset(&obj, 0, sizeof(obj));
    if (fee_asset != nullptr) {
#ifndef CFD_DISABLE_ELEMENTS
      std::string asset_hex(fee_asset);
      if (!asset_hex.empty()) {
        ConfidentialAssetId asset_data(asset_hex);
        memcpy(
            obj.fee_asset, asset_data.GetData().GetBytes().data(),
            sizeof(obj.fee_asset));
      }
#else
      info(CFD_LOG_SOURCE, "unuse asset[{}]", fee_asset);
#endif  // CFD_DISABLE_ELEMENTS
    }
    obj.tx_fee_amount = tx_fee_amount;
    obj.effective_fee_rate = effective_fee_rate;
    obj.long_term_fee_rate = long_term_fee_rate;
    obj.dust_fee_rate = dust_fee_rate;
    obj.knapsack_min_change = knapsack_min_change;

    buffer = static_cast<CfdCapiCoinSelection*>(
        AllocBuffer(kPrefixCoinSelection, sizeof(CfdCapiCoinSelection)));
    memcpy(obj.prefix, buffer->prefix, sizeof(obj.prefix));
    *buffer = obj;
    buffer->utxos = new std::vector<Utxo>(utxo_count);
    buffer->targets = new std::vector<CfdCapiTargetAsset>(target_asset_count);
    buffer->indexes = new std::vector<int32_t>();
    *coin_select_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  if (buffer != nullptr) CfdFreeCoinSelectionHandle(handle, buffer);
  return result;
}

int CfdAddCoinSelectionUtxo(
    void* handle, void* coin_select_handle, int32_t utxo_index,
    const char* txid, uint32_t vout, int64_t amount, const char* asset,
    const char* descriptor) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(coin_select_handle, kPrefixCoinSelection);
    if (coin_select_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "coinselect handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. coinselect handle is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }
    if (utxo_index < 0) {
      warn(CFD_LOG_SOURCE, "utxoIndex is under 0.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. utxoIndex is under 0.");
    }

    CfdCapiCoinSelection* buffer =
        static_cast<CfdCapiCoinSelection*>(coin_select_handle);
    if ((buffer->utxos == nullptr) ||
        (utxo_index >= static_cast<int32_t>(buffer->utxos->size()))) {
      warn(CFD_LOG_SOURCE, "utxo index is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. utxo index is maximum over.");
    }

    std::string temp_asset;
    std::string temp_descriptor;
    if (asset != nullptr) {
      temp_asset = std::string(asset);
    }
    if (descriptor != nullptr) {
      temp_descriptor = std::string(descriptor);
    }
    CoinSelection::ConvertToUtxo(
        Txid(std::string(txid)), vout, temp_descriptor,
        Amount::CreateBySatoshiAmount(amount), temp_asset,
        reinterpret_cast<const void*>(utxo_index),
        &((*buffer->utxos)[utxo_index]));
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

int CfdAddCoinSelectionAmount(
    void* handle, void* coin_select_handle, uint32_t asset_index,
    int64_t amount, const char* asset) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(coin_select_handle, kPrefixCoinSelection);
    if (coin_select_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "coinselect handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. coinselect handle is null.");
    }

    CfdCapiCoinSelection* buffer =
        static_cast<CfdCapiCoinSelection*>(coin_select_handle);
    if ((buffer->targets == nullptr) ||
        (asset_index >= buffer->targets->size())) {
      warn(CFD_LOG_SOURCE, "target amounts is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. target amounts is maximum over.");
    }

    CfdCapiTargetAsset object;
    memset(&object, 0, sizeof(object));
    object.amount = amount;
    if (asset != nullptr) {
#ifndef CFD_DISABLE_ELEMENTS
      std::string asset_hex(asset);
      if (!asset_hex.empty()) {
        ConfidentialAssetId asset_data(asset_hex);
        memcpy(
            object.asset, asset_data.GetData().GetBytes().data(),
            sizeof(object.asset));
      }
#else
      info(CFD_LOG_SOURCE, "unuse asset[{}]", asset);
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

int CfdFinalizeCoinSelection(
    void* handle, void* coin_select_handle, int64_t* utxo_fee_amount) {
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
    CheckBuffer(coin_select_handle, kPrefixCoinSelection);
    if (coin_select_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "coinselect handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. coinselect handle is null.");
    }

    CfdCapiCoinSelection* buffer =
        static_cast<CfdCapiCoinSelection*>(coin_select_handle);
    if (buffer->utxos == nullptr) {
      warn(CFD_LOG_SOURCE, "utxos is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to parameter. utxos is null.");
    }
    if (buffer->targets == nullptr) {
      warn(CFD_LOG_SOURCE, "target amounts is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to parameter. target amounts is null.");
    }
    uint8_t empty_asset[kAssetSize];
    memset(empty_asset, 0, sizeof(empty_asset));

    AmountMap map_target_value;
    for (const auto& target : *(buffer->targets)) {
      map_target_value.emplace(
          convert_to_asset(target.asset).GetHex(),
          Amount::CreateBySatoshiAmount(target.amount));
    }

    CoinSelectionOption option_params;
    if (memcmp(buffer->fee_asset, empty_asset, sizeof(empty_asset)) == 0) {
      // bitcoin
      option_params.InitializeTxSizeInfo();
    } else {
      // elements
#ifndef CFD_DISABLE_ELEMENTS
      option_params.InitializeConfidentialTxSizeInfo();
      option_params.SetFeeAsset(convert_to_asset(buffer->fee_asset));
#endif  // CFD_DISABLE_ELEMENTS
    }
    option_params.SetEffectiveFeeBaserate(buffer->effective_fee_rate);
    option_params.SetLongTermFeeBaserate(buffer->long_term_fee_rate);
    option_params.SetDustFeeRate(buffer->dust_fee_rate);
    option_params.SetKnapsackMinimumChange(buffer->knapsack_min_change);

    Amount utxo_fee_value;
    AmountMap map_select_value;
    UtxoFilter filter;
    Amount tx_fee_value;
    CoinSelection select_object;
    std::vector<Utxo> utxo_list = select_object.SelectCoins(
        map_target_value, *(buffer->utxos), filter, option_params,
        tx_fee_value, &map_select_value, &utxo_fee_value);

    // save return value from AmountMap
#ifndef CFD_DISABLE_ELEMENTS
    for (auto& target : *(buffer->targets)) {
      std::string asset = convert_to_asset(target.asset).GetHex();
      if (map_select_value.find(asset) != map_select_value.end()) {
        target.selected_amount = map_select_value[asset].GetSatoshiValue();
      }
    }
#endif  // CFD_DISABLE_ELEMENTS

    // save return value from utxo_list
    buffer->indexes->reserve(utxo_list.size());
    for (const auto& utxo : utxo_list) {
      int32_t utxo_index;
      memcpy(&utxo_index, &utxo.binary_data, sizeof(utxo_index));
      buffer->indexes->push_back(utxo_index);
    }

    if (utxo_fee_amount != nullptr) {
      *utxo_fee_amount = utxo_fee_value.GetSatoshiValue();
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

int CfdGetSelectedCoinIndex(
    void* handle, void* coin_select_handle, uint32_t index,
    int32_t* utxo_index) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(coin_select_handle, kPrefixCoinSelection);
    if (utxo_index == nullptr) {
      warn(CFD_LOG_SOURCE, "utxo index is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to parameter. utxo index is null.");
    }

    CfdCapiCoinSelection* buffer =
        static_cast<CfdCapiCoinSelection*>(coin_select_handle);
    if ((buffer->indexes == nullptr) || buffer->indexes->empty()) {
      warn(CFD_LOG_SOURCE, "select coin list is empty.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. select coin list is empty.");
    } else if (index >= buffer->indexes->size()) {
      *utxo_index = -1;
    } else {
      *utxo_index = buffer->indexes->at(index);
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

int CfdGetSelectedCoinAssetAmount(
    void* handle, void* coin_select_handle, uint32_t asset_index,
    int64_t* amount) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(coin_select_handle, kPrefixCoinSelection);
    if (amount == nullptr) {
      warn(CFD_LOG_SOURCE, "amount is null.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to parameter. amount is null.");
    }

    CfdCapiCoinSelection* buffer =
        static_cast<CfdCapiCoinSelection*>(coin_select_handle);
    if ((buffer->targets == nullptr) ||
        (asset_index >= buffer->targets->size())) {
      warn(CFD_LOG_SOURCE, "target amounts is maximum over.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. target amounts is maximum over.");
    }

    *amount = buffer->targets->at(asset_index).selected_amount;
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

int CfdFreeCoinSelectionHandle(void* handle, void* coin_select_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (coin_select_handle != nullptr) {
      CfdCapiCoinSelection* coin_select_struct =
          static_cast<CfdCapiCoinSelection*>(coin_select_handle);
      if (coin_select_struct->utxos != nullptr) {
        delete coin_select_struct->utxos;
        coin_select_struct->utxos = nullptr;
      }
      if (coin_select_struct->targets != nullptr) {
        delete coin_select_struct->targets;
        coin_select_struct->targets = nullptr;
      }
      if (coin_select_struct->indexes != nullptr) {
        delete coin_select_struct->indexes;
        coin_select_struct->indexes = nullptr;
      }
    }
    FreeBuffer(
        coin_select_handle, kPrefixCoinSelection,
        sizeof(CfdCapiCoinSelection));
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

int CfdInitializeEstimateFee(
    void* handle, void** fee_handle, bool is_elements) {
  CfdCapiEstimateFeeData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (fee_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "Fee handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. Fee handle is null.");
    }

    buffer = static_cast<CfdCapiEstimateFeeData*>(
        AllocBuffer(kPrefixEstimateFeeData, sizeof(CfdCapiEstimateFeeData)));
    buffer->input_utxos = new std::vector<UtxoData>();
#ifndef CFD_DISABLE_ELEMENTS
    buffer->is_elements = is_elements;
    buffer->input_elements_utxos = new std::vector<ElementsUtxoAndOption>();
#endif  // CFD_DISABLE_ELEMENTS

    *fee_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    if (buffer != nullptr) CfdFreeEstimateFeeHandle(handle, buffer);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    if (buffer != nullptr) CfdFreeEstimateFeeHandle(handle, buffer);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    if (buffer != nullptr) CfdFreeEstimateFeeHandle(handle, buffer);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdAddTxInForEstimateFee(
    void* handle, void* fee_handle, const char* txid, uint32_t vout,
    const char* descriptor, const char* asset, bool is_issuance,
    bool is_blind_issuance, bool is_pegin, uint32_t pegin_btc_tx_size,
    const char* fedpeg_script) {
  try {
    cfd::Initialize();
    CheckBuffer(fee_handle, kPrefixEstimateFeeData);
    if (IsEmptyString(descriptor)) {
      warn(CFD_LOG_SOURCE, "descriptor is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. descriptor is null.");
    }
    CfdCapiEstimateFeeData* buffer =
        static_cast<CfdCapiEstimateFeeData*>(fee_handle);

    UtxoData utxo;
    utxo.txid = Txid(txid);
    utxo.vout = vout;
    utxo.descriptor = std::string(descriptor);
#ifndef CFD_DISABLE_ELEMENTS
    if (buffer->is_elements) {
      if (IsEmptyString(asset)) {
        warn(CFD_LOG_SOURCE, "utxo asset is null.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. utxo asset is null.");
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
    }
#else
    info(
        CFD_LOG_SOURCE,
        "unuse parameters: [is_issuance={}, is_blind_issuance={}, "
        "is_pegin={}, pegin_btc_tx_size={}, fedpeg_script={}]",
        is_issuance, is_blind_issuance, is_pegin, pegin_btc_tx_size,
        fedpeg_script);
#endif  // CFD_DISABLE_ELEMENTS
    buffer->input_utxos->push_back(utxo);
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

int CfdFinalizeEstimateFee(
    void* handle, void* fee_handle, const char* tx_hex, const char* fee_asset,
    int64_t* tx_fee, int64_t* utxo_fee, bool is_blind,
    double effective_fee_rate) {
  try {
    cfd::Initialize();
    CheckBuffer(fee_handle, kPrefixEstimateFeeData);
    CfdCapiEstimateFeeData* buffer =
        static_cast<CfdCapiEstimateFeeData*>(fee_handle);
    if (tx_fee == nullptr) {
      warn(CFD_LOG_SOURCE, "tx fee is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx fee is null.");
    }
    if (utxo_fee == nullptr) {
      warn(CFD_LOG_SOURCE, "utxo fee is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. utxo fee is null.");
    }
#ifndef CFD_DISABLE_ELEMENTS
    if (buffer->is_elements && IsEmptyString(fee_asset)) {
      warn(CFD_LOG_SOURCE, "fee asset is empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. fee asset is empty.");
    }
#else
    info(CFD_LOG_SOURCE, "unuse fee asset[{}]", fee_asset);
#endif  // CFD_DISABLE_ELEMENTS

    Amount tx_fee_amt, utxo_fee_amt;
#ifndef CFD_DISABLE_ELEMENTS
    if (buffer->is_elements) {
      ElementsTransactionApi api;
      api.EstimateFee(
          std::string(tx_hex), *(buffer->input_elements_utxos),
          ConfidentialAssetId(fee_asset), &tx_fee_amt, &utxo_fee_amt, is_blind,
          effective_fee_rate);
    } else {
      TransactionApi api;
      api.EstimateFee(
          std::string(tx_hex), *(buffer->input_utxos), &tx_fee_amt,
          &utxo_fee_amt, effective_fee_rate);
    }
#else
    info(CFD_LOG_SOURCE, "unuse fee asset[{}]", fee_asset);
    TransactionApi api;
    api.EstimateFee(
        std::string(tx_hex), *(buffer->input_utxos), &tx_fee_amt,
        &utxo_fee_amt, effective_fee_rate);
#endif  // CFD_DISABLE_ELEMENTS
    *tx_fee = tx_fee_amt.GetSatoshiValue();
    *utxo_fee = utxo_fee_amt.GetSatoshiValue();

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

int CfdFreeEstimateFeeHandle(void* handle, void* fee_handle) {
  try {
    cfd::Initialize();
    if (fee_handle != nullptr) {
      CfdCapiEstimateFeeData* buffer =
          static_cast<CfdCapiEstimateFeeData*>(fee_handle);
      if (buffer->input_utxos != nullptr) {
        delete buffer->input_utxos;
        buffer->input_utxos = nullptr;
      }
#ifndef CFD_DISABLE_ELEMENTS
      if (buffer->input_elements_utxos != nullptr) {
        delete buffer->input_elements_utxos;
        buffer->input_elements_utxos = nullptr;
      }
#endif  // CFD_DISABLE_ELEMENTS
    }
    FreeBuffer(
        fee_handle, kPrefixEstimateFeeData, sizeof(CfdCapiEstimateFeeData));
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
