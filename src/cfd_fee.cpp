// Copyright 2019 CryptoGarage
/**
 * @file cfd_fee.cpp
 *
 * @brief Fee計算の関連クラスの実装ファイル
 */

#include <string>
#include <vector>

#include "cfd/cfd_fee.h"
#include "cfdcore/cfdcore_amount.h"

namespace cfd {

using cfd::core::Amount;

Amount FeeCalculator::CalculateFee(  // Fee計算
    uint32_t size, uint32_t vsize, uint32_t rate) {
  // 簡易計算
  static constexpr int64_t kMinimumFee = 1000;
  int64_t satoshi;
  if (size == vsize) {
    satoshi = size * rate / 1000;
  } else {
    satoshi = vsize * rate / 1000;
  }
  if (satoshi < kMinimumFee) {
    satoshi = kMinimumFee;
  }
  return Amount::CreateBySatoshiAmount(satoshi);
}

}  // namespace cfd
