/* Copyright 2021 CryptoGarage */
/**
 * @file cfdcapi_block.h
 *
 * @brief API definition file of Block functions for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_BLOCK_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_BLOCK_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include <stddef.h>
#include <stdint.h>

#include "cfdc/cfdcapi_common.h"

/**
 * @brief initialize block handle.
 * @param[in] handle                cfd handle.
 * @param[in] network_type          network type.
 * @param[in] block_hex             block hex.
 * @param[out] block_handle         block handle.
 *   Call 'CfdFreeBlockHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeBlockHandle(
    void* handle, int network_type, const char* block_hex,
    void** block_handle);

/**
 * @brief free block handle.
 * @param[in] handle            cfd handle.
 * @param[in] block_handle      block handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeBlockHandle(void* handle, void* block_handle);

/**
 * @brief Get the block hash.
 * @param[in] handle                cfd handle.
 * @param[in] block_handle          block handle.
 * @param[out] block_hash           block hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetBlockHash(
    void* handle, void* block_handle, char** block_hash);

/**
 * @brief Get the block header data.
 * @param[in] handle                cfd handle.
 * @param[in] block_handle          block handle.
 * @param[out] version              version
 * @param[out] prev_block_hash      previous block hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] merkle_root_hash     merkle root hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] time                 create time
 * @param[out] bits                 bits
 * @param[out] nonce                nonce
 * @return CfdErrorCode
 */
CFDC_API int CfdGetBlockHeaderData(
    void* handle, void* block_handle, uint32_t* version,
    char** prev_block_hash, char** merkle_root_hash, uint32_t* time,
    uint32_t* bits, uint32_t* nonce);

/**
 * @brief Get transaction data from block.
 * @param[in] handle                cfd handle.
 * @param[in] block_handle          block handle.
 * @param[in] txid                  txid.
 * @param[out] tx_hex               transaction hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTransactionFromBlock(
    void* handle, void* block_handle, const char* txid, char** tx_hex);

/**
 * @brief Get txout proof.
 * @param[in] handle                cfd handle.
 * @param[in] block_handle          block handle.
 * @param[in] txid                  txid.
 * @param[out] txout_proof          txout proof.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTxOutProof(
    void* handle, void* block_handle, const char* txid, char** txout_proof);

/**
 * @brief Exist txid in the block.
 * @param[in] handle                cfd handle.
 * @param[in] block_handle          block handle.
 * @param[in] txid                  txid.
 * @return CfdErrorCode
 */
CFDC_API int CfdExistTxidInBlock(
    void* handle, void* block_handle, const char* txid);

/**
 * @brief Get transaction count in block.
 * @param[in] handle                cfd handle.
 * @param[in] block_handle          block handle.
 * @param[out] tx_count             tx count in block.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTxCountInBlock(
    void* handle, void* block_handle, uint32_t* tx_count);

/**
 * @brief Get txid from block.
 * @param[in] handle                cfd handle.
 * @param[in] block_handle          block handle.
 * @param[in] index                 index. (max: tx count)
 * @param[out] txid                 txid.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTxidFromBlock(
    void* handle, void* block_handle, uint32_t index, char** txid);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_BLOCK_H_ NOLINT */
