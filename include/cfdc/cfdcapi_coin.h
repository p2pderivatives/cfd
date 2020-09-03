/* Copyright 2019 CryptoGarage */
/**
 * @file cfdcapi_coin.h
 *
 * @brief API definition file of Coin functions for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_COIN_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_COIN_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include <stddef.h>
#include <stdint.h>

#include "cfdc/cfdcapi_common.h"

/** coin selection option */
enum CfdCoinSelectionOption {
  /** blind option: exponent */
  kCfdCoinSelectionExponent = 1,
  /** blind option: minBits */
  kCfdCoinSelectionMinimumBits = 2,
};

/** estimate fee option */
enum CfdEstimateFeeOption {
  /** blind option: exponent */
  kCfdEstimateFeeExponent = 1,
  /** blind option: minBits */
  kCfdEstimateFeeMinimumBits = 2,
};

/**
 * @brief initialize coin selection handle.
 * @param[in] handle                cfd handle.
 * @param[in] utxo_count            utxo count.
 * @param[in] target_asset_count    target asset count.
 * @param[in] fee_asset             fee asset.
 * @param[in] tx_fee_amount         tx-fee amount.
 * @param[in] effective_fee_rate    effective fee rate. (default: -1)
 * @param[in] long_term_fee_rate    long-term fee rate. (default: -1)
 * @param[in] dust_fee_rate         dust fee rate. (default: -1)
 * @param[in] knapsack_min_change   knapsack min change. (default: -1)
 * @param[out] coin_select_handle   coin selection handle.
 *   Call 'CfdFreeCoinSelectionHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeCoinSelection(
    void* handle, uint32_t utxo_count, uint32_t target_asset_count,
    const char* fee_asset, int64_t tx_fee_amount, double effective_fee_rate,
    double long_term_fee_rate, double dust_fee_rate,
    int64_t knapsack_min_change, void** coin_select_handle);
/* other value is optional function */

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
 * @brief add coin selection's utxo.
 * @param[in] handle              cfd handle.
 * @param[in] coin_select_handle  coin selection handle.
 * @param[in] utxo_index          utxo index.
 * @param[in] txid                utxo txid.
 * @param[in] vout                utxo vout.
 * @param[in] amount              utxo amount.
 * @param[in] asset               utxo asset.
 * @param[in] descriptor          utxo output descriptor.
 * @param[in] scriptsig_template  utxo scriptsig template.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddCoinSelectionUtxoTemplate(
    void* handle, void* coin_select_handle, int32_t utxo_index,
    const char* txid, uint32_t vout, int64_t amount, const char* asset,
    const char* descriptor, const char* scriptsig_template);

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
 * @brief set coin selection's option.
 * @param[in] handle              cfd handle.
 * @param[in] coin_select_handle  coin selection handle.
 * @param[in] key                 option key.
 * @param[in] int64_value         int64 value.
 * @param[in] double_value        double value.
 * @param[in] bool_value          bool value.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetOptionCoinSelection(
    void* handle, void* coin_select_handle, int key, int64_t int64_value,
    double double_value, bool bool_value);

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

/**
 * @brief Initialize handle for fee estimation apis.
 * @param[in] handle        cfd handle.
 * @param[out] fee_handle   handle for fee estimation apis.
 * @param[in] is_elements   estimate on elements chain.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeEstimateFee(
    void* handle, void** fee_handle, bool is_elements);

/**
 * @brief Add Transaction Input for estimate fee handle.
 * @param[in] handle            cfd handle.
 * @param[in] fee_handle        handle for fee estimation apis.
 * @param[in] txid              input utxo's transaction id.
 * @param[in] vout              input utxo's vout.
 * @param[in] descriptor        the descriptor for creating locking_script.
 * @param[in] asset             utxo's unblind asset id.
 * @param[in] is_issuance       is utxo issuance input.
 * @param[in] is_blind_issuance is utxo blind issuance input.
 * @param[in] is_pegin          is utxo pegin input.
 * @param[in] pegin_btc_tx_size size of pegin transaction.
 * @param[in] fedpeg_script     utxo's unblind asset id.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTxInForEstimateFee(
    void* handle, void* fee_handle, const char* txid, uint32_t vout,
    const char* descriptor, const char* asset, bool is_issuance,
    bool is_blind_issuance, bool is_pegin, uint32_t pegin_btc_tx_size,
    const char* fedpeg_script);

/**
 * @brief Add Transaction Input for estimate fee handle.
 * @param[in] handle            cfd handle.
 * @param[in] fee_handle        handle for fee estimation apis.
 * @param[in] txid              input utxo's transaction id.
 * @param[in] vout              input utxo's vout.
 * @param[in] descriptor        the descriptor for creating locking_script.
 * @param[in] asset             utxo's unblind asset id.
 * @param[in] is_issuance       is utxo issuance input.
 * @param[in] is_blind_issuance is utxo blind issuance input.
 * @param[in] is_pegin          is utxo pegin input.
 * @param[in] pegin_btc_tx_size size of pegin transaction.
 * @param[in] fedpeg_script     utxo's unblind asset id.
 * @param[in] scriptsig_template  utxo's scriptsig template.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTxInTemplateForEstimateFee(
    void* handle, void* fee_handle, const char* txid, uint32_t vout,
    const char* descriptor, const char* asset, bool is_issuance,
    bool is_blind_issuance, bool is_pegin, uint32_t pegin_btc_tx_size,
    const char* fedpeg_script, const char* scriptsig_template);

/**
 * @brief set fee estimation's option.
 * @param[in] handle              cfd handle.
 * @param[in] fee_handle          handle for fee estimation apis.
 * @param[in] key                 option key.
 * @param[in] int64_value         int64 value.
 * @param[in] double_value        double value.
 * @param[in] bool_value          bool value.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetOptionEstimateFee(
    void* handle, void* fee_handle, int key, int64_t int64_value,
    double double_value, bool bool_value);

/**
 * @brief Finalize fee estimation api call.
 * @param[in] handle                cfd handle.
 * @param[in] fee_handle            handle for fee estimation apis.
 * @param[in] tx_hex                transaction hex.
 * @param[in] fee_asset             asset id used as fee.
 * @param[out] txout_fee            estimated fee by transaction base & output.
 *     (not contain utxo_fee.)
 * @param[out] utxo_fee             estimated fee by input utxos.
 *     (not contain txout_fee.)
 * @param[in] is_blind              need blind later.
 * @param[in] effective_fee_rate    effective fee rate for estimation.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeEstimateFee(
    void* handle, void* fee_handle, const char* tx_hex, const char* fee_asset,
    int64_t* txout_fee, int64_t* utxo_fee, bool is_blind,
    double effective_fee_rate);

/**
 * @brief Free handle for fee estimation
 * @param[in] handle        cfd handle.
 * @param[in] fee_handle    fee handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeEstimateFeeHandle(void* handle, void* fee_handle);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_COIN_H_ NOLINT */
