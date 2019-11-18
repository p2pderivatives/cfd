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
#include "cfdcore/cfdcore_amount.h"

namespace cfd {

using cfd::core::Amount;

/**
 * @brief Fee計算を行うクラス
 */
class CFD_EXPORT FeeCalculator {
 public:
  /** @brief ベースレート(大き目) */
  static constexpr uint32_t kBaseRate = 11000;

  /**
   * @brief Feeを計算する.
   * @param[in] size    Transaction size
   * @param[in] vsize   Transaction virtual size
   * @param[in] rate    rate (base = 1000)
   * @return Fee Amount
   */
  static Amount CalculateFee(  // Fee計算
      uint32_t size, uint32_t vsize, uint32_t rate = kBaseRate);

 private:
  FeeCalculator();
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_FEE_H_
