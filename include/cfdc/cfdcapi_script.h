// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_script.h
 *
 * @brief cfd-capiで利用するScript関連のAPI定義
 *
 * C言語のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

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

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_
