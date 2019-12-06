// Copyright 2019 CryptoGarage
/**
 * @file cfd_fee.h
 *
 * @brief Fee操作の関連クラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_FEE_H_
#define CFD_INCLUDE_CFD_CFD_FEE_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfdcore/cfdcore_amount.h"

namespace cfd {

using cfd::core::Amount;

/**
 * @brief Fee計算を行うクラス
 */
class CFD_EXPORT FeeCalculator {
 public:
  /**
   * @brief 最小のfee
   * @details bitcoin relay minimum fee rate.
   * @see bitcoin: DEFAULT_INCREMENTAL_RELAY_FEE
   */
  static constexpr const int64_t kRelayMinimumFee = 1000;
  /**
   * @brief 最小のfee
   * @details elements relay minimum fee rate.
   */
  static constexpr const int64_t kElementsRelayMinimumFee = 100;

  /**
   * @brief Feeを計算する.
   * @param[in] size    Transaction size
   * @param[in] vsize   Transaction virtual size
   * @param[in] rate    rate (default = relay minimum)
   * @return Fee Amount
   */
  static Amount CalculateFee(  // Fee計算
      uint32_t size, uint32_t vsize, uint64_t rate = kRelayMinimumFee);

  /**
   * @brief constructor.
   */
  FeeCalculator();

  /**
   * @brief constructor.
   * @param[in] baserate  baserate (bitcoin: 1.0)
   */
  explicit FeeCalculator(uint64_t baserate);

  /**
   * @brief Feeを計算する.
   * @param[in] size    Transaction size
   * @return Fee Amount
   */
  Amount GetFee(uint32_t size) const;

  /**
   * @brief Feeを計算する.
   * @param[in] utxo    unused transaction output
   * @return Fee Amount
   */
  Amount GetFee(const Utxo& utxo) const;

 private:
  uint32_t baserate_;  //!< ベースレート
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_FEE_H_
