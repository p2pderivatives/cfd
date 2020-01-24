#include "gtest/gtest.h"
#include <vector>

#include "cfdcore/cfdcore_amount.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_fee.h"

using cfd::core::Amount;
using cfd::FeeCalculator;

TEST(FeeCalculator, Constructor)
{
  FeeCalculator fee;
  EXPECT_EQ(fee.GetFee(3000).GetSatoshiValue(), static_cast<int64_t>(3000));
}

TEST(FeeCalculator, CalculateFeeTest)
{
  Amount amt;
  EXPECT_NO_THROW((amt = FeeCalculator::CalculateFee(3000, 1000, 11000)));
  EXPECT_EQ(amt.GetSatoshiValue(), static_cast<int64_t>(11000));

  EXPECT_NO_THROW((amt = FeeCalculator::CalculateFee(3000, 3000, 10000)));
  EXPECT_EQ(amt.GetSatoshiValue(), static_cast<int64_t>(30000));
}

TEST(FeeCalculator, GetFeeTest)
{
  FeeCalculator fee(2000);
  size_t size = 3000;
  Amount amt;
  EXPECT_NO_THROW((amt = fee.GetFee(size)));
  EXPECT_EQ(amt.GetSatoshiValue(), static_cast<int64_t>(6000));

  FeeCalculator fee2(20000);
  EXPECT_NO_THROW((amt = fee2.GetFee(size)));
  EXPECT_EQ(amt.GetSatoshiValue(), static_cast<int64_t>(60000));
}
