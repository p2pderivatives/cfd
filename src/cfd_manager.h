// Copyright 2019 CryptoGarage
/**
 * @file cfd_manager.h
 *
 * @brief-eng definition file for cfd manager class
 * @brief-jp cfd管理クラス定義ファイルです。
 *
 */
#ifndef CFD_SRC_CFD_MANAGER_H_
#define CFD_SRC_CFD_MANAGER_H_

#include <memory>

#include "cfdcore/cfdcore_common.h"

#include "cfd/cfd_common.h"

/**
 * @brief cfd名前空間
 */
namespace cfd {

/**
 * @brief cfd管理クラス。
 */
class CfdManager {
 public:
  /**
   * @brief コンストラクタ。
   */
  CfdManager();
  /**
   * @brief コンストラクタ。
   */
  virtual ~CfdManager();

  /**
   * @brief cfdを初期化する。
   */
  void Initialize();
  /**
   * @brief cfdを終了する。
   * @param[in] is_finish_process   プロセス終了時かどうか
   */
  void Finalize(bool is_finish_process);
  /**
   * @brief ライブラリがサポートしている機能の値を取得する。
   * @return LibraryFunctionのビットフラグ
   */
  uint64_t GetSupportedFunction();

 private:
  cfd::core::CfdCoreHandle handle_;  ///< ハンドル
  bool initialized_;                 ///< 初期化済みフラグ
  bool finalized_;                   ///< 終了済みフラグ
};

}  // namespace cfd

#endif  // CFD_SRC_CFD_MANAGER_H_
