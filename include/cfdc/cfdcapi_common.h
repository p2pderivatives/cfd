// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_common.h
 * @brief cfd-capiの共通定義ファイル。
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_COMMON_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_COMMON_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief APIのDLLエクスポート定義
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
 * @brief エラーコード定義
 */
enum CfdErrorCode {
  kCfdSuccess = 0,               //!< 正常終了
  kCfdUnknownError = -1,         //!< 不明なエラー
  kCfdInternalError = -2,        //!< 内部エラー
  kCfdMemoryFullError = -3,      //!< メモリ確保エラー
  kCfdIllegalArgumentError = 1,  //!< 引数不正
  kCfdIllegalStateError = 2,     //!< 状態不正
  kCfdOutOfRangeError = 3,       //!< 範囲外の値
  kCfdInvalidSettingError = 4,   //!< 設定不正
  kCfdConnectionError = 5,       //!< 接続エラー
  kCfdDiskAccessError = 6,       //!< ディスクアクセスエラー
  kCfdSignVerificationError = 7  //!< Signature Verification 失敗時のエラー
};

/**
 * @brief ライブラリがサポートしている機能の定義値
 */
enum CfdLibraryFunction {
  kCfdEnableBitcoin = 0x0001,   //!< enable bitcoin function
  kCfdEnableElements = 0x0002,  //!< enable elements function
};

// API
/**
 * @brief ライブラリがサポートしている機能の値を取得する。
 * @param[out] support_flag   LibraryFunctionのビットフラグ
 * @return CfdErrorCode
 */
CFDC_API int CfdGetSupportedFunction(uint64_t* support_flag);
/**
 * @brief cfdの初期化を行う。
 * @return CfdErrorCode
 */
CFDC_API int CfdInitialize(void);
/**
 * @brief cfdの終了処理を行う。
 * @param[in] is_finish_process   プロセス終了時かどうか
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalize(bool is_finish_process);

/**
 * @brief cfdのハンドルを取得する。
 * @param[out] handle   handle pointer.
 *   When you are finished using it, release it with 'CfdFreeHandle'.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateHandle(void** handle);

/**
 * @brief cfdの簡易ハンドル(管理外ハンドル)を取得する。
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
 * @brief cfdのハンドルを解放する。
 * @param[in] handle    handle pointer
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeHandle(void* handle);

/**
 * @brief cfdが確保したバッファを解放する。
 * @param[in] address   buffer pointer
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeBuffer(void* address);

#ifndef CFD_DISABLE_FREESTRING
/**
 * @brief cfdが確保した文字列バッファを解放する。
 * @details CfdFreeBufferのalias.
 * @param[in] address   buffer pointer
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeStringBuffer(char* address);
#endif  // CFD_DISABLE_FREESTRING

/**
 * @brief cfdで最後に発生したエラーコードを取得する。
 * @param[in] handle    handle pointer.
 * @return last error code. (see CfdErrorCode)
 */
CFDC_API int CfdGetLastErrorCode(void* handle);

/**
 * @brief cfdで最後に発生したエラーのエラーメッセージを取得する。
 * @param[in] handle    handle pointer.
 * @param[out] message  message string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @retval kCfdSuccess  success
 * @retval kCfdIllegalArgumentError   message is NULL.
 */
CFDC_API int CfdGetLastErrorMessage(void* handle, char** message);

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_COMMON_H_
