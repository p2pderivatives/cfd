// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_coin.h
 *
 * @brief cfd-capiで利用するCoin関連のAPI定義
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_COIN_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_COIN_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

#include <stddef.h>
#include <stdint.h>

#include "cfdc/cfdcapi_common.h"

/**
 * @brief initialize coin selection handle.
 * @param[in] handle                cfd handle.
 * @param[in] utxo_count            utxo count.
 * @param[in] target_asset_count    target asset count.
 * @param[in] fee_asset             fee asset.
 * @param[in] tx_fee_amount         tx-fee amount.
 * @param[in] fee_rate              fee rate. (default: -1)
 * @param[in] long_term_fee_rate    long-term fee rate. (default: -1)
 * @param[in] dust_fee_rate         dust fee rate. (default: -1)
 * @param[in] knapsack_min_change   knapsack min change. (default: -1)
 * @param[out] coin_select_handle   coin selection handle.
 *   Call 'CfdFreeCoinSelectionHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeCoinSelection(
    void* handle, uint32_t utxo_count, uint32_t target_asset_count,
    const char* fee_asset, int64_t tx_fee_amount, double fee_rate,
    double long_term_fee_rate, double dust_fee_rate,
    int64_t knapsack_min_change, void** coin_select_handle);
// other value is optional function

/**
 * @brief add coin selection's utxo.
 * @param[in] handle              cfd handle.
 * @param[in] coin_select_handle  coin selection handle.
 * @param[in] utxo_index          utxo index.
 * @param[in] txid                utxo txid.
 * @param[in] vout                utxo vout.
 * @param[in] amount              utxo amount.
 * @param[in] asset               utxo asset.
 * @param[in] descriptor          utxo output descriptor.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddCoinSelectionUtxo(
    void* handle, void* coin_select_handle, int32_t utxo_index,
    const char* txid, uint32_t vout, int64_t amount, const char* asset,
    const char* descriptor);

/**
 * @brief add coin selection's target asset.
 * @param[in] handle              cfd handle.
 * @param[in] coin_select_handle  coin selection handle.
 * @param[in] asset_index         target asset index.
 * @param[in] amount              target amount.
 * @param[in] asset               target asset.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddCoinSelectionAmount(
    void* handle, void* coin_select_handle, uint32_t asset_index,
    int64_t amount, const char* asset);

/**
 * @brief Get selected utxo coin.
 * @param[in] handle              handle pointer.
 * @param[in] coin_select_handle  coin selection handle.
 * @param[out] utxo_fee_amount    utxo's fee amount.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeCoinSelection(
    void* handle, void* coin_select_handle, int64_t* utxo_fee_amount);

/**
 * @brief Get selected utxo coin.
 * @param[in] handle              handle pointer.
 * @param[in] coin_select_handle  coin selection handle.
 * @param[in] index               select index.
 * @param[out] utxo_index         utxo index. nothing is set -1.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetSelectedCoinIndex(
    void* handle, void* coin_select_handle, uint32_t index,
    int32_t* utxo_index);

/**
 * @brief Get selected asset total amount.
 * @param[in] handle              handle pointer.
 * @param[in] coin_select_handle  coin selection handle.
 * @param[in] asset_index         target asset index.
 * @param[out] amount             selected asset total amount.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetSelectedCoinAssetAmount(
    void* handle, void* coin_select_handle, uint32_t asset_index,
    int64_t* amount);

/**
 * @brief free coin selection handle.
 * @param[in] handle              handle pointer.
 * @param[in] coin_select_handle  coin selection handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeCoinSelectionHandle(
    void* handle, void* coin_select_handle);

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_COIN_H_
