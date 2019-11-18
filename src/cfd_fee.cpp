// Copyright 2019 CryptoGarage
/**
 * @file cfd_fee.cpp
 *
 * @brief Fee計算の関連クラスの実装ファイル
 */
#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_fee.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_transaction_common.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Amount;

// -----------------------------------------------------------------------------
// ファイル内定数
// -----------------------------------------------------------------------------
//! KB size
static constexpr const uint64_t kKiloByteSize = 1000;

// -----------------------------------------------------------------------------
// FeeCalculator
// -----------------------------------------------------------------------------
Amount FeeCalculator::CalculateFee(  // Fee計算
    uint32_t size, uint32_t vsize, uint64_t rate) {
  int64_t satoshi;
  uint32_t use_size = ((vsize != 0) && (size != vsize)) ? vsize : size;
  satoshi = use_size * rate / kKiloByteSize;
  if (satoshi < kRelayMinimumFee) {
    satoshi = kRelayMinimumFee;
  }
  return Amount::CreateBySatoshiAmount(satoshi);
}

FeeCalculator::FeeCalculator()
    : FeeCalculator(FeeCalculator::kRelayMinimumFee) {}

FeeCalculator::FeeCalculator(uint64_t baserate) : baserate_(baserate) {}

Amount FeeCalculator::GetFee(uint32_t size) const {
  return GetFee(static_cast<size_t>(size));
}

Amount FeeCalculator::GetFee(size_t size) const {
  int64_t byte_size = static_cast<int64_t>(size);
  int64_t fee = baserate_ * byte_size / 1000;
  if ((fee == 0) && (byte_size != 0) && (baserate_ != 0)) {
    fee = 1;
  }
  return Amount::CreateBySatoshiAmount(fee);
}

Amount FeeCalculator::GetFee(const Utxo& utxo) const {
  uint32_t minimum_txin = static_cast<uint32_t>(TxIn::kMinimumTxInSize);
  uint32_t nowit_size = minimum_txin + utxo.uscript_size_max;
  uint32_t vsize =
      AbstractTransaction::GetVsizeFromSize(nowit_size, utxo.witness_size_max);
  return GetFee(vsize);
}

}  // namespace cfd
