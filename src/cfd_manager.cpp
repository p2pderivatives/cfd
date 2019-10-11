// Copyright 2019 CryptoGarage
/**
 * @file cfd_manager.cpp
 *
 * @brief cfd管理クラス実装ファイルです。
 *
 */
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_logger_interface.h"

#include "cfd_manager.h"  // NOLINT

using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::logger::info;

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------
namespace cfd {

/// cfdインスタンス
static CfdManager cfd_instance;

void Initialize(void) { cfd_instance.Initialize(); }

void Finalize(bool is_finish_process) {
  cfd_instance.Finalize(is_finish_process);
}

uint64_t GetSupportedFunction() { return cfd_instance.GetSupportedFunction(); }

// -----------------------------------------------------------------------------
// Management
// -----------------------------------------------------------------------------
void CfdManager::Initialize() {
  if (finalized_) {
    throw CfdException(
        CfdError::kCfdIllegalStateError, "Initialize already finalized.");
  }

  if (!initialized_) {
    // 初期化処理実施
    cfd::core::Initialize(&handle_);
    initialized_ = true;
    info(CFD_LOG_SOURCE, "cfd initialize.");
  }
}

void CfdManager::Finalize(bool is_finish_process) {
  if (initialized_ && (!finalized_)) {
    if (!is_finish_process) {
      info(CFD_LOG_SOURCE, "cfd finalize.");
    }
    cfd::core::Finalize(handle_, is_finish_process);
    finalized_ = true;
    handle_ = nullptr;
  }
}

uint64_t CfdManager::GetSupportedFunction() {
  uint64_t support_function = 0;
  // cfdcoreとcfdの両方でサポートしている必要がある。
  uint64_t core_functions = cfd::core::GetSupportedFunction();

#ifndef CFD_DISABLE_BITCOIN
  if (core_functions & cfd::core::LibraryFunction::kEnableBitcoin) {
    support_function |= LibraryFunction::kEnableBitcoin;
  }
#endif  // CFD_DISABLE_BITCOIN

#ifndef CFD_DISABLE_ELEMENTS
  if (core_functions & cfd::core::LibraryFunction::kEnableElements) {
    support_function |= LibraryFunction::kEnableElements;
  }
#endif  // CFD_DISABLE_ELEMENTS

  return support_function;
}

CfdManager::CfdManager()
    : handle_(nullptr), initialized_(false), finalized_(false) {
  // do nothing
}

CfdManager::~CfdManager() {
  if (initialized_ && (!finalized_)) {
    cfd::core::Finalize(handle_, true);
  }
}

}  // namespace cfd
