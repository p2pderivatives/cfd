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

// FIXME elements first.

/**
 * @brief add sign to transaction.
 * @param[in] handle          cfd handle.
 * @param[in] net_type        network type.
 * @param[in] tx_hex_string   transaction hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[in] hash_type       hash type.
 * @param[in] sign_data_hex   add sign data hex.
 * @param[in] use_der_encode  der encode flag.
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pay flag.
 * @param[in] clear_stack     cleanup scriptSig stack data.
 * @param[out] tx_string      signed transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTxSign(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* sign_data_hex,
    bool use_der_encode, int sighash_type, bool sighash_anyone_can_pay,
    bool clear_stack, char** tx_string);

/**
 * @brief add sign signature and pubkey to transaction.
 * @param[in] handle          cfd handle.
 * @param[in] net_type        network type.
 * @param[in] tx_hex_string   transaction hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[in] hash_type       hash type.
 * @param[in] pubkey          pubkey hex.
 * @param[in] signature       add sign data signature.
 * @param[in] use_der_encode  der encode flag.
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pay flag.
 * @param[out] tx_string      signed transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddPubkeyHashSign(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* pubkey, const char* signature,
    bool use_der_encode, int sighash_type, bool sighash_anyone_can_pay,
    char** tx_string);

/**
 * @brief add redeem script to transaction.
 * @details using after CfdAddTxSign.
 * @param[in] handle          cfd handle.
 * @param[in] net_type        network type.
 * @param[in] tx_hex_string   transaction hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[in] hash_type       hash type.
 * @param[in] redeem_script   redeem script.
 * @param[in] clear_stack     cleanup scriptSig stack data.
 * @param[out] tx_string      signed transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddScriptHashSign(
    void* handle, int net_type, const char* tx_hex_string, const char* txid,
    uint32_t vout, int hash_type, const char* redeem_script, bool clear_stack,
    char** tx_string);

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

/**
 * @brief append multisig sign to transaction.
 * @param[in] handle            cfd handle.
 * @param[in] multisign_handle  multisig sign handle.
 * @param[in] net_type        network type.
 * @param[in] tx_hex_string     tx hex.
 * @param[in] txid              txin txid.
 * @param[in] vout              txin vout.
 * @param[in] hash_type         hash type.
 * @param[in] redeem_script     multisig redeem script.
 * @param[out] tx_string        signed tx hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeMultisigSign(
    void* handle, void* multisign_handle, int net_type,
    const char* tx_hex_string, const char* txid, uint32_t vout, int hash_type,
    const char* redeem_script, char** tx_string);

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
*/

/**
 * @brief get tx-input index.
 * @param[in] handle          cfd handle.
 * @param[in] tx_hex_string   tx hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[out] index          txin index.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTxInIndex(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    uint32_t* index);

/**
 * @brief get tx-input index.
 * @param[in] handle                 cfd handle.
 * @param[in] tx_hex_string          tx hex.
 * @param[in] address                txout address.
 * @param[in] direct_locking_script  txout locking script. (not use address)
 * @param[out] index                 txout index.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTxOutIndex(
    void* handle, const char* tx_hex_string, const char* address,
    const char* direct_locking_script, uint32_t* index);

/*
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
