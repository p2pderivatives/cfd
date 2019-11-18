#include "gtest/gtest.h"
#include <vector>

#include "cfdcore/cfdcore_amount.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_fee.h"

using cfd::core::Amount;
using cfd::FeeCalculator;

TEST(FeeCalculator, CalculateFeeTest)
{
  Amount amt;
  EXPECT_NO_THROW((amt = FeeCalculator::CalculateFee(3000, 1000, 11000)));
  EXPECT_EQ(amt.GetSatoshiValue(), static_cast<int64_t>(11000));

  EXPECT_NO_THROW((amt = FeeCalculator::CalculateFee(3000, 3000, 10000)));
  EXPECT_EQ(amt.GetSatoshiValue(), static_cast<int64_t>(30000));
}
