/* Copyright 2019 CryptoGarage */
/**
 * @file cfdcapi_common.h
 * @brief Common definition file of cfd-capi.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_COMMON_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_COMMON_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief dll export definition on windows
 */
#ifndef CFDC_API
#if defined(_WIN32)
#ifdef CFD_BUILD
#define CFDC_API __declspec(dllexport)
#elif defined(CFD_SHARED)
#define CFDC_API __declspec(dllimport)
#else
#define CFDC_API
#endif
#elif defined(__GNUC__) && defined(CFD_BUILD)
#define CFDC_API __attribute__((visibility("default")))
#else
#define CFDC_API
#endif
#endif

/**
 * @brief error code for cfd
 */
enum CfdErrorCode {
  /** success */
  kCfdSuccess = 0,
  /** unknown error */
  kCfdUnknownError = -1,
  /** internal error */
  kCfdInternalError = -2,
  /** memory full error */
  kCfdMemoryFullError = -3,
  /** illegal argument */
  kCfdIllegalArgumentError = 1,
  /** illegal statement */
  kCfdIllegalStateError = 2,
  /** out of range */
  kCfdOutOfRangeError = 3,
  /** invalid setting */
  kCfdInvalidSettingError = 4,
  /** connection error */
  kCfdConnectionError = 5,
  /** disk access error */
  kCfdDiskAccessError = 6,
  /** Signature Verification Fail */
  kCfdSignVerificationError = 7,
  /** NotFound error */
  kCfdNotFoundError = 8
};

/**
 * @brief Definition value of the function supported by the cfd library
 */
enum CfdLibraryFunction {
  /** enable bitcoin function */
  kCfdEnableBitcoin = 0x0001,
  /** enable elements function */
  kCfdEnableElements = 0x0002,
};

/* API */
/**
 * @brief Get the value of the function supported by the library.
 * @param[out] support_flag   bit flag on the library function.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetSupportedFunction(uint64_t* support_flag);
/**
 * @brief Initialize for cfd.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitialize(void);
/**
 * @brief Finalize for cfd.
 * @param[in] is_finish_process   stopped the process.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalize(bool is_finish_process);

/**
 * @brief Create cfd handle.
 * @param[out] handle   handle pointer.
 *   When you are finished using it, release it with 'CfdFreeHandle'.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateHandle(void** handle);

/**
 * @brief Create simple cfd handle.
 * @param[out] handle   handle pointer.
 *   When you are finished using it, release it with 'CfdFreeHandle'.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateSimpleHandle(void** handle);

/**
 * @brief cloning cfd handle.
 * @param[in] source    source handle. (can use null)
 * @param[out] handle   handle pointer.
 *   When you are finished using it, release it with 'CfdFreeHandle'.
 * @return CfdErrorCode
 */
CFDC_API int CfdCloneHandle(void* source, void** handle);

/**
 * @brief copy error state on cfd handle.
 * @param[in] source        source handle. (can use null)
 * @param[out] destination  destination handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdCopyErrorState(void* source, void* destination);

/**
 * @brief Free cfd handle.
 * @param[in] handle    handle pointer
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeHandle(void* handle);

/**
 * @brief Free cfd native buffer.
 * @param[in] address   buffer pointer
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeBuffer(void* address);

#ifndef CFD_DISABLE_FREESTRING
/**
 * @brief Free cfd native string buffer.
 * @details CfdFreeBufferのalias.
 * @param[in] address   buffer pointer
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeStringBuffer(char* address);
#endif /* CFD_DISABLE_FREESTRING NOLINT */

/**
 * @brief Get last error code on cfd handle.
 * @param[in] handle    handle pointer.
 * @return last error code. (see CfdErrorCode)
 */
CFDC_API int CfdGetLastErrorCode(void* handle);

/**
 * @brief Get last error message on cfd handle.
 * @param[in] handle    handle pointer.
 * @param[out] message  message string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @retval kCfdSuccess  success
 * @retval kCfdIllegalArgumentError   message is NULL.
 */
CFDC_API int CfdGetLastErrorMessage(void* handle, char** message);

/**
 * @brief Request execute function by json string.
 * @param[in] handle                 handle pointer.
 * @param[in] name                   request name.
 * @param[in] json_string            request json data.
 * @param[out] response_json_string  response json data.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @retval kCfdSuccess               success
 * @retval kCfdIllegalArgumentError  value is NULL.
 * @retval kCfdIllegalStateError     not supported function.(disable definition)
 */
CFDC_API int CfdRequestExecuteJson(
    void* handle, const char* name, const char* json_string,
    char** response_json_string);

/**
 * @brief Serialize byte data. Append varint data.
 * @param[in] handle    handle pointer.
 * @param[in] buffer    byte array hex.
 * @param[out] output   output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdSerializeByteData(
    void* handle, const char* buffer, char** output);

/**
 * @brief Encrypto by AES.
 * @param[in] handle    handle pointer.
 * @param[in] key       key byte array hex.
 * @param[in] cbc_iv    initial vector (with cbc) hex.
 * @param[in] buffer    byte array hex.
 * @param[out] output   output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdEncryptAES(
    void* handle, const char* key, const char* cbc_iv, const char* buffer,
    char** output);

/**
 * @brief Decrypto by AES.
 * @param[in] handle    handle pointer.
 * @param[in] key       key byte array hex.
 * @param[in] cbc_iv    initial vector (with cbc) hex.
 * @param[in] buffer    AES byte array hex.
 * @param[out] output   output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdDecryptAES(
    void* handle, const char* key, const char* cbc_iv, const char* buffer,
    char** output);

/**
 * @brief Encode base64.
 * @param[in] handle    handle pointer.
 * @param[in] buffer    byte array hex.
 * @param[out] output   output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdEncodeBase64(void* handle, const char* buffer, char** output);

/**
 * @brief Decode base64.
 * @param[in] handle    handle pointer.
 * @param[in] base64    base64 string.
 * @param[out] output   output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdDecodeBase64(void* handle, const char* base64, char** output);

/**
 * @brief Encode base58.
 * @param[in] handle          handle pointer.
 * @param[in] buffer          byte array hex.
 * @param[in] use_checksum    using checksum.
 * @param[out] output         output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdEncodeBase58(
    void* handle, const char* buffer, bool use_checksum, char** output);

/**
 * @brief Decode base58.
 * @param[in] handle          handle pointer.
 * @param[in] base58          base58 string.
 * @param[in] use_checksum    using checksum.
 * @param[out] output         output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdDecodeBase58(
    void* handle, const char* base58, bool use_checksum, char** output);

/**
 * @brief Hash with ripemd160.
 * @param[in] handle            handle pointer.
 * @param[in] message           message string. (text or hex)
 * @param[in] has_text          message has text.
 * @param[out] output           output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdRipemd160(
    void* handle, const char* message, bool has_text, char** output);

/**
 * @brief Hash with sha256.
 * @param[in] handle            handle pointer.
 * @param[in] message           message string. (text or hex)
 * @param[in] has_text          message has text.
 * @param[out] output           output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdSha256(
    void* handle, const char* message, bool has_text, char** output);

/**
 * @brief Hash with hash160.
 * @param[in] handle            handle pointer.
 * @param[in] message           message string. (text or hex)
 * @param[in] has_text          message has text.
 * @param[out] output           output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdHash160(
    void* handle, const char* message, bool has_text, char** output);

/**
 * @brief Hash with hash256.
 * @param[in] handle            handle pointer.
 * @param[in] message           message string. (text or hex)
 * @param[in] has_text          message has text.
 * @param[out] output           output byte array hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdHash256(
    void* handle, const char* message, bool has_text, char** output);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_COMMON_H_ NOLINT */
