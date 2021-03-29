// Copyright 2019 CryptoGarage
/**
 * @file cfd_common.h
 * @brief Common definition file for cfd.
 */
#ifndef CFD_INCLUDE_CFD_CFD_COMMON_H_
#define CFD_INCLUDE_CFD_CFD_COMMON_H_
#include <cstddef>
#include <cstdint>

/**
 * @brief API DLL export definition
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
 * @brief DLL export definition for class
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
 * @brief cfd namespace
 */
namespace cfd {

/**
 * @brief Definition of the function supported by the library
 */
enum LibraryFunction {
  kEnableBitcoin = 0x0001,   //!< enable bitcoin function
  kEnableElements = 0x0002,  //!< enable elements function
};

// API
/**
 * @brief Get the supported function by the library.
 * @return Function bit flag
 */
CFD_API uint64_t GetSupportedFunction();
/**
 * @brief Initialize cfd.
 */
CFD_API void Initialize(void);
/**
 * @brief Performs cfd termination processing.
 * @param[in] is_finish_process   Whether at the end of the process
 */
CFD_API void Finalize(bool is_finish_process = false);

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_COMMON_H_
