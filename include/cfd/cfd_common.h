// Copyright 2019 CryptoGarage
/**
 * @file cfd_common.h
 * @brief cfdの共通定義ファイル。
 */
#ifndef CFD_INCLUDE_CFD_CFD_COMMON_H_
#define CFD_INCLUDE_CFD_CFD_COMMON_H_
#include <cstddef>
#include <cstdint>

/**
 * @brief APIのDLLエクスポート定義
 */
#ifndef CFD_API
#if defined(_WIN32)
#ifdef CFD_BUILD
#define CFD_API __declspec(dllexport)
#elif defined(CFD_SHARED)
#define CFD_API __declspec(dllimport)
#else
#define CFD_API
#endif
#elif defined(__GNUC__) && defined(CFD_BUILD)
#define CFD_API __attribute__((visibility("default")))
#else
#define CFD_API
#endif
#endif

/**
 * @brief クラスのDLLエクスポート定義
 */
#ifndef CFD_EXPORT
#if defined(_WIN32)
#ifdef CFD_BUILD
#define CFD_EXPORT __declspec(dllexport)
#elif defined(CFD_SHARED)
#define CFD_EXPORT __declspec(dllimport)
#else
#define CFD_EXPORT
#endif
#elif defined(__GNUC__) && defined(CFD_BUILD)
#define CFD_EXPORT __attribute__((visibility("default")))
#else
#define CFD_EXPORT
#endif
#endif

/**
 * @brief cfd名前空間
 */
namespace cfd {

/**
 * @brief ライブラリがサポートしている機能の定義値
 */
enum LibraryFunction {
  kEnableBitcoin = 0x0001,   //!< enable bitcoin function
  kEnableElements = 0x0002,  //!< enable elements function
};

// API
/**
 * @brief ライブラリがサポートしている機能の値を取得する。
 * @return LibraryFunctionのビットフラグ
 */
CFD_API uint64_t GetSupportedFunction();
/**
 * @brief cfdの初期化を行う。
 */
CFD_API void Initialize(void);
/**
 * @brief cfdの終了処理を行う。
 * @param[in] is_finish_process   プロセス終了時かどうか
 */
CFD_API void Finalize(bool is_finish_process = false);

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_COMMON_H_
