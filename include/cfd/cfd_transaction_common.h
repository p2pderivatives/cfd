// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction_common.h
 *
 * @brief Transaction操作共通の関連クラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_
#define CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::Txid;

/**
 * @brief Transaction生成のためのController基底クラス
 */
class CFD_EXPORT AbstractTransactionController {
 public:
  /**
   * @brief コンストラクタ.
   */
  AbstractTransactionController();
  /**
   * @brief デストラクタ.
   */
  virtual ~AbstractTransactionController() {
    // do nothing
  }

  /**
   * @brief TransactionHex文字列取得.
   * @return TransactionHEX文字列
   */
  std::string GetHex() const;

  /**
   * @brief ロックタイムからデフォルトのシーケンス番号を取得する。
   * @retval 0xffffffff     locktime値無効
   * @retval 0xfffffffe     locktime値有効
   */
  uint32_t GetDefaultSequence();

  /**
   * @brief ロックタイムからlocktime値無効のシーケンス番号を取得する。
   * @retval 0xffffffff     locktime値無効
   */
  static uint32_t GetLockTimeDisabledSequence();

 protected:
  AbstractTransaction *tx_address_;  //!< Transaction基底オブジェクト
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_
