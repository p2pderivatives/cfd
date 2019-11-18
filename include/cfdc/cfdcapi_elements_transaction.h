// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_elements_transaction.h
 *
 * @brief cfd-capiで利用するElements用Transaction操作のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_TRANSACTION_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_TRANSACTION_H_
#ifndef CFD_DISABLE_ELEMENTS

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

#include <stddef.h>
#include <stdint.h>

#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_transaction.h"

// TODO(k-matsuzawa): バイト列とHEX文字列のどちらに統一すべきか？（引数観点でHEXにしたい）

CFDC_API int CfdInitializeConfidentialTx(
    void* handle, uint32_t version, uint32_t locktime, char** tx_string);
CFDC_API int CfdAddConfidentialTxIn(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    uint32_t sequence, char** tx_string);
CFDC_API int CfdUpdateConfidentialTxIn(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    const uint8_t* script_sig, uint32_t script_sig_size, uint32_t sequence,
    char** tx_string);
CFDC_API int CfdAddElementsWitnessStack(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    const uint8_t* buffer, uint32_t buffer_size, char** tx_string);
CFDC_API int CfdAddPeginWitnessStack(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    const uint8_t* buffer, uint32_t buffer_size, char** tx_string);
CFDC_API int CfdAddConfidentialTxOut(
    void* handle, const char* tx_hex_string, const char* asset_string,
    int64_t value_satoshi, const uint8_t* value_commitment,
    uint32_t value_commitment_size, const char* address,
    const char* direct_script_pubkey, const uint8_t* nonce,
    uint32_t nonce_size, char** tx_string);
CFDC_API int CfdUpdateConfidentialTxOut(
    void* handle, const char* tx_hex_string, uint32_t index,
    const char* asset_string, int64_t value_satoshi,
    const uint8_t* value_commitment, uint32_t value_commitment_size,
    const char* address, const char* direct_script_pubkey,
    const uint8_t* nonce, uint32_t nonce_size, char** tx_string);

CFDC_API int CfdGetConfidentialTxIn(
    void* handle, const char* tx_hex_string, uint32_t index, char** txid,
    uint32_t* vout, uint32_t* sequence, uint8_t** script_sig,
    uint32_t* script_sig_size);
CFDC_API int CfdGetTxInIssuanceInfo(
    void* handle, const char* tx_hex_string, uint32_t index, uint8_t** entropy,
    uint32_t* entropy_size, uint8_t** nonce, uint32_t* nonce_size,
    char** asset_string, char** token_string, uint8_t** asset_rangeproof,
    uint32_t* asset_rangeproof_size, uint8_t** token_rangeproof,
    uint32_t* token_rangeproof_size);
CFDC_API int CfdGetElementsWitnessStack(
    void* handle, const char* tx_hex_string, uint32_t index,
    uint32_t witness_index, uint8_t** buffer, uint32_t* buffer_size);
CFDC_API int CfdGetPeginWitnessStack(
    void* handle, const char* tx_hex_string, uint32_t index,
    uint32_t witness_index, uint8_t** buffer, uint32_t* buffer_size);
CFDC_API int CfdGetConfidentialTxOut(
    void* handle, const char* tx_hex_string, uint32_t index,
    char** asset_string, int64_t* value_satoshi, uint8_t** value_commitment,
    uint32_t* value_commitment_size, uint8_t** nonce, uint32_t* nonce_size,
    uint8_t** surjection_proof, uint32_t* surjection_proof_size,
    uint8_t** rangeproof, uint32_t* rangeproof_size);

CFDC_API int CfdGetElementsWitnessStackCount(
    void* handle, const char* tx_hex_string, uint32_t index,
    uint32_t witness_index, uint32_t* count);
CFDC_API int CfdGetPeginWitnessStackCount(
    void* handle, const char* tx_hex_string, uint32_t index,
    uint32_t witness_index, uint32_t* count);
CFDC_API int CfdGetConfidentialTxInCount(
    void* handle, const char* tx_hex_string, uint32_t index, uint32_t* count);
CFDC_API int CfdGetConfidentialTxOutCount(
    void* handle, const char* tx_hex_string, uint32_t index, uint32_t* count);

CFDC_API int CfdSetReissueAsset(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    int64_t asset_amount, const uint8_t* blinding_nonce,
    const uint8_t* entropy, const char* address,
    const char* direct_script_pubkey, uint8_t** reissue_entropy,
    uint32_t* reissue_entropy_size, char** asset_string, char** tx_string);

CFDC_API int CfdGetIssuanceBlindingKey(
    void* handle, const uint8_t* master_blinding_key, uint32_t key_size,
    const char* txid, uint32_t vout, uint8_t** blinding_key,
    uint32_t* blinding_key_size);

CFDC_API int CfdInitializeBlindTx(
    void* handle, const char* tx_hex_string, void** blind_handle);
CFDC_API int CfdAddBlindTxInData(
    void* handle, void* blind_handle, const char* txid, uint32_t vout,
    const char* asset_string, const uint8_t* abf, uint32_t abf_size,
    const uint8_t* vbf, uint32_t vbf_size, int64_t value_satoshi,
    const uint8_t* asset_key, uint32_t asset_key_size,
    const uint8_t* token_key, uint32_t token_key_size);
CFDC_API int CfdAddBlindTxOutData(
    void* handle, void* blind_handle, uint32_t index,
    const uint8_t* blinding_key, uint32_t blinding_key_size);
CFDC_API int CfdFinalizeBlindTx(
    void* handle, void* blind_handle, char** tx_string);
CFDC_API int CfdFreeBlindHandle(void* handle, void* blind_handle);

CFDC_API int CfdAddConfidentialTxSign(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    bool is_witness, const uint8_t* data, uint32_t data_size,
    char** tx_string);

CFDC_API int CfdAddConfidentialTxDerSign(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    bool is_witness, const uint8_t* data, uint32_t data_size,
    uint8_t sighashType, bool sighash_anyone_can_pay, char** tx_string);

CFDC_API int CfdFinalizeElementsMultisigSign(
    void* handle, void* multisign_handle, const char* tx_hex_string,
    const Txid& txid, uint32_t vout, bool is_witness, char** tx_string);

CFDC_API int CfdCreateConfidentialSighash(
    void* handle, const char* tx_hex_string, int hash_type, const char* pubkey,
    const char* redeem_script, int64_t value_satoshi,
    const uint8_t* value_commitment,
    uint32_t value_commitment_size uint8_t** sighash, uint32_t* sighash_size);

#if 0
        * SetRawIssueAsset
          - SetIssueAsset
            - Asset, Token, EntropyをOUTで。
            - TxOutIndexも。
        * UnblindRawTransaction
          - UnblindTxIn
          - UnblindTxOut
        * get系
    * handle内部buffer案
      - err_hdl
        - hdl_type:uchar[8]
        - funcname:char[60]
        - errcode:uint32
        - errmsg:char[256]
        - hdl_type2:uchar[8]
      - normal_hdl
        - hdl_type:uchar[8]
        - errcode:uint32
        - pointer1:void*
        - pointer2:void*
        - pointer3:void*
        - pointer4:void*
        - hdl_type2:uchar[8]

#endif

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_TRANSACTION_H_
