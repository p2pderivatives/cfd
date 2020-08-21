/* Copyright 2019 CryptoGarage */
/**
 * @file cfdcapi_script.h
 *
 * @brief API definition file of Script function for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include "cfdc/cfdcapi_common.h"

/**
 * @brief parse script and return script item handle.
 * @param[in] handle              cfd handle.
 * @param[in] script              script string.
 * @param[out] script_item_handle script item handle.
 * @param[out] script_item_num    num of items contains script.
 * @return CfdErrorCode
 */
CFDC_API int CfdParseScript(
    void* handle, const char* script, void** script_item_handle,
    uint32_t* script_item_num);

/**
 * @brief get script item from script item handle.
 * @param[in] handle              cfd handle.
 * @param[in] script_item_handle  script item handle.
 * @param[in] index               index of script item.
 * @param[out] script_item        string of script item.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetScriptItem(
    void* handle, void* script_item_handle, uint32_t index,
    char** script_item);

/**
 * @brief free script item handle.
 * @param[in] handle             handle pointer.
 * @param[in] script_item_handle handle of getting script item.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeScriptItemHandle(void* handle, void* script_item_handle);

/**
 * @brief Convert script asm format to hex data.
 * @param[in] handle        cfd handle.
 * @param[in] script_asm    assembly format script string.
 * @param[out] script_hex    converted hex string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdConvertScriptAsmToHex(
    void* handle, const char* script_asm, char** script_hex);

/**
 * @brief initialized for multisig scriptsig.
 * @param[in] handle          cfd handle.
 * @param[out] multisig_handle  multisig scriptsig handle.
 *   Call 'CfdFreeMultisigScriptSigHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeMultisigScriptSig(
    void* handle, void** multisig_handle);

/**
 * @brief append for multisig scriptsig data.
 * @param[in] handle            cfd handle.
 * @param[in] multisig_handle   multisig scriptsig handle.
 * @param[in] signature         signature hex.
 * @param[in] related_pubkey    pubkey hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddMultisigScriptSigData(
    void* handle, void* multisig_handle, const char* signature,
    const char* related_pubkey);

/**
 * @brief append for multisig scriptsig data.
 * @param[in] handle                  cfd handle.
 * @param[in] multisig_handle         multisig scriptsig handle.
 * @param[in] signature               signature hex.
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pey flag.
 * @param[in] related_pubkey          pubkey hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddMultisigScriptSigDataToDer(
    void* handle, void* multisig_handle, const char* signature,
    int sighash_type, bool sighash_anyone_can_pay, const char* related_pubkey);

/**
 * @brief create multisig scriptsig.
 * @param[in] handle           handle pointer.
 * @param[in] multisig_handle  multisig scriptsig handle.
 * @param[in] redeem_script    redeemScript for multisig.
 * @param[out] scriptsig       generate scriptsig.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeMultisigScriptSig(
    void* handle, void* multisig_handle, const char* redeem_script,
    char** scriptsig);

/**
 * @brief free multisig scriptsig handle.
 * @param[in] handle            handle pointer.
 * @param[in] multisig_handle   multisig scriptsig handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeMultisigScriptSigHandle(
    void* handle, void* multisig_handle);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_ NOLINT */
