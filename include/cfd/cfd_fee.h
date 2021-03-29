// Copyright 2019 CryptoGarage
/**
 * @file cfd_fee.h
 *
 * @brief Related class definition for Fee operations
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
 * @brief Class that performs Fee calculation
 */
class CFD_EXPORT FeeCalculator {
 public:
  /**
   * @brief Smallest fee
   * @details bitcoin relay minimum fee rate.
   * @see bitcoin: DEFAULT_INCREMENTAL_RELAY_FEE
   */
  static constexpr const int64_t kRelayMinimumFee = 1000;
  /**
   * @brief Smallest fee
   * @details elements relay minimum fee rate.
   */
  static constexpr const int64_t kElementsRelayMinimumFee = 100;

  /**
   * @brief Calculate Fee.
   * @param[in] size    Transaction size
   * @param[in] vsize   Transaction virtual size
   * @param[in] rate    rate (default = relay minimum)
   * @return Fee Amount
   */
  static Amount CalculateFee(  // Calculate Fee.
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
   * @brief Calculate Fee.
   * @param[in] size    Transaction size
   * @return Fee Amount
   */
  Amount GetFee(uint32_t size) const;

  /**
   * @brief Calculate Fee.
   * @param[in] utxo    unused transaction output
   * @return Fee Amount
   */
  Amount GetFee(const Utxo& utxo) const;

 private:
  uint32_t baserate_;  //!< base rate
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_FEE_H_
