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
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_address.h"
#include "cfdc/cfdcapi_coin.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::AmountMap;
using cfd::CoinSelection;
using cfd::CoinSelectionOption;
using cfd::Utxo;
using cfd::UtxoFilter;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Txid;

using cfd::core::logger::info;
using cfd::core::logger::warn;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::ConfidentialAssetId;
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

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiCoinSelection;
using cfd::capi::CfdCapiTargetAsset;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kAssetSize;
using cfd::capi::kPrefixCoinSelection;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

extern "C" {

int CfdInitializeCoinSelection(
    void* handle, uint32_t utxo_count, uint32_t target_asset_count,
    const char* fee_asset, int64_t tx_fee_amount, double fee_rate,
    double effective_fee_rate, double long_term_fee_rate, double dust_fee_rate,
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
    obj.fee_rate = fee_rate;
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

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
