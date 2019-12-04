// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_transaction.h
 *
 * @brief cfd-capiで利用するTransaction作成のAPI定義
 *
 * C言語のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_TRANSACTION_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_TRANSACTION_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"

//! txin sequence locktime
enum CfdSequenceLockTime {
  /// disable locktime
  kCfdSequenceLockTimeDisable = 0xffffffffU,
  /// enable locktime (maximum time)
  kCfdSequenceLockTimeEnableMax = 0xfffffffeU,
};

// FIXME elementsを先に作る

/**
 * @brief initialized for multisig sign.
 * @param[in] handle          cfd handle.
 * @param[out] multisign_handle  multisig sign handle.
 *   Call 'CfdFreeMultisigSignHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeMultisigSign(void* handle, void** multisign_handle);

/**
 * @brief append for multisig signature data.
 * @param[in] handle            cfd handle.
 * @param[in] multisign_handle  multisig sign handle.
 * @param[in] signature         signature hex.
 * @param[in] related_pubkey    pubkey hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddMultisigSignData(
    void* handle, void* multisign_handle, const char* signature,
    const char* related_pubkey);

/**
 * @brief append for multisig signature data.
 * @param[in] handle                  cfd handle.
 * @param[in] multisign_handle        multisig sign handle.
 * @param[in] signature               signature hex.
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pey flag.
 * @param[in] related_pubkey          pubkey hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddMultisigSignDataToDer(
    void* handle, void* multisign_handle, const char* signature,
    int sighash_type, bool sighash_anyone_can_pay, const char* related_pubkey);

#if 0
/**
 * @brief append multisig sign to transaction.
 * @param[in] handle                  cfd handle.
 * @param[in] multisign_handle        multisig sign handle.
 * @param[in] tx_hex_string           tx hex.
 * @param[in] txid                    txin txid.
 * @param[in] vout                    txin vout.
 * @param[in] hash_type       hash type.
 * @param[in] witness_script  witness script for segwit.
 * @param[in] redeem_script   redeem script for p2sh.
 * @param[out] tx_string              signed tx hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeMultisigSign(
    void* handle, void* multisign_handle,
    const char* tx_hex_string, const char* txid, uint32_t vout,
    int hash_type, const char* witness_script, const char* redeem_script,
    char** tx_string);
#endif

/**
 * @brief free multisig sign handle.
 * @param[in] handle            handle pointer.
 * @param[in] multisign_handle  multisig sign handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeMultisigSignHandle(void* handle, void* multisign_handle);

#if 0
/*
CFDC_API int CfdAddTxSign(void* handle, const char* tx_hex_string,
    const char* txid, uint32_t vout, bool is_witness,
    const uint8_t* data, uint32_t data_size,
    char** tx_string);

CFDC_API int CfdAddTxDerSign(void* handle, const char* tx_hex_string,
    const char* txid, uint32_t vout, bool is_witness,
    const uint8_t* data, uint32_t data_size,
    uint8_t sighashType, bool sighash_anyone_can_pay,
    char** tx_string);


CFDC_API int CfdCreateSighash(void* handle, const char* tx_hex_string,
    int address_type, const char* pubkey, const char* redeem_script,
    int64_t value_satoshi,
    uint8_t** sighash, uint32_t* sighash_size);

- AddTxIn
- AddWitnessStack
- AddTxOut
- GetTxHex
- FreeTx
- GetTxInfo
  - version, locktime, 他
- GetTxIn
- GetTxInCount
- GetTxOut
- GetTxOutCount
- GetWitnessStack
- GetWitnessStackCount

      - Tx
        * CreateRawTransaction
          - InitTx
          - AddTxIn
          - AddWitnessStack
          - AddTxOut
          - GetTxHex
          - FreeTx
          - GetTxInfo
            - version, locktime, 他
          - GetTxIn
          - GetTxInCount
          - GetTxOut
          - GetTxOutCount
          - GetWitnessStack
          - GetWitnessStackCount
        * CreateSignatureHash
          - CreateSignatureHash
        * AddSign
          - AddSign
        * AddMultisigSign
          - InitializeMultisigSign
          - AddMultisigSign
          - FinalizeMultisigSign
        * get系
*/
#endif

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_TRANSACTION_H_
