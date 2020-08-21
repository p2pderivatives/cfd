/* Copyright 2020 CryptoGarage */
/**
 * @file cfdcapi_ledger.h
 *
 * @brief API definition file of ledger function for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_LEDGER_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_LEDGER_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include "cfdc/cfdcapi_common.h"

/**
 * @brief initialized transaction serialize for ledger.
 * @param[in] handle              cfd handle.
 * @param[out] serialize_handle   serialize handle.
 *   Call 'CfdFreeTxSerializeForLedger' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeTxSerializeForLedger(
    void* handle, void** serialize_handle);

/**
 * @brief append txout metadata for ledger.
 * @param[in] handle            cfd handle.
 * @param[in] serialize_handle  serialize handle.
 * @param[in] index             txout index.
 * @param[in] metadata1         metadata1.
 * @param[in] metadata2         metadata2.
 * @param[in] metadata3         metadata3.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTxOutMetaDataForLedger(
    void* handle, void* serialize_handle, uint32_t index,
    const char* metadata1, const char* metadata2, const char* metadata3);

/**
 * @brief output transaction serialize for ledger.
 * @param[in] handle            cfd handle.
 * @param[in] serialize_handle  serialize handle.
 * @param[in] net_type          network type.
 * @param[in] tx_hex_string     tx hex.
 * @param[in] skip_witness      skip witness output.
 * @param[in] is_authorization  authorization flag.
 * @param[out] serialize_hex    serialized tx hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeTxSerializeForLedger(
    void* handle, void* serialize_handle, int net_type,
    const char* tx_hex_string, bool skip_witness, bool is_authorization,
    char** serialize_hex);

/**
 * @brief output transaction serialize for ledger.
 * @param[in] handle            cfd handle.
 * @param[in] serialize_handle  serialize handle.
 * @param[in] net_type          network type.
 * @param[in] tx_hex_string     tx hex.
 * @param[in] skip_witness      skip witness output.
 * @param[in] is_authorization  authorization flag.
 * @param[in] is_sha256         sha256 hashed flag.
 * @param[out] serialize_hex    serialized tx hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeTxSerializeHashForLedger(
    void* handle, void* serialize_handle, int net_type,
    const char* tx_hex_string, bool skip_witness, bool is_authorization,
    bool is_sha256, char** serialize_hex);

/**
 * @brief free serialize handle.
 * @param[in] handle            handle pointer.
 * @param[in] serialize_handle  serialize handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeTxSerializeForLedger(void* handle, void* serialize_handle);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_LEDGER_H_ NOLINT */
