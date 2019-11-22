// Copyright 2019 CryptoGarage
/**
 * @file cfd_utxo.cpp
 *
 * @brief UTXO操作の関連クラスの実装ファイル
 */
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "cfd/cfd_utxo.h"

#include "cfd/cfd_common.h"
#include "cfd/cfd_fee.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction_common.h"
#ifndef CFD_DISABLE_ELEMENTS
#include "cfdcore/cfdcore_elements_transaction.h"
#endif  // CFD_DISABLE_ELEMENTS

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlockHash;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::kMaxAmount;
using cfd::core::RandomNumberUtil;
using cfd::core::Script;
using cfd::core::Txid;
using cfd::core::TxIn;
#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialTxIn;
using cfd::core::ConfidentialTxOut;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
#endif  // CFD_DISABLE_ELEMENTS
using cfd::core::logger::info;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// Inner definitions
// -----------------------------------------------------------------------------
//! SelectCoinsBnBの最大繰り返し回数
static constexpr const size_t kBnBMaxTotalTries = 100000;

//! KnapsackSolver ApproximateBestSubsetの繰り返し回数
static constexpr const int kApproximateBestSubsetIterations = 100000;

//! Change最小値
static constexpr const uint64_t kMinChange = 1000000;  // MIN_CHANGE

//! LongTerm fee rate default (20.0)
static constexpr const uint64_t kDefaultLongTermFeeRate = 20000;

//! dust relay tx fee rate
static constexpr const uint64_t kDustRelayTxFeeRate = 3000;

//! default discard fee
static constexpr const uint64_t kDefaultDiscardFee = 10000;

//! WITNESS_SCALE_FACTOR
static constexpr const uint32_t kWitnessScaleFactor = 4;

// -----------------------------------------------------------------------------
// CoinSelectionOption
// -----------------------------------------------------------------------------
CoinSelectionOption::CoinSelectionOption()
    : effective_fee_baserate_(kDefaultLongTermFeeRate),
      long_term_fee_baserate_(kDefaultLongTermFeeRate),
      knapsack_minimum_change_(-1),
      dust_fee_rate_(kDustRelayTxFeeRate) {
  // do nothing
}

bool CoinSelectionOption::IsUseBnB() const { return use_bnb_; }

size_t CoinSelectionOption::GetChangeOutputSize() const {
  return change_output_size_;
}

size_t CoinSelectionOption::GetChangeSpendSize() const {
  return change_spend_size_;
}

uint64_t CoinSelectionOption::GetEffectiveFeeBaserate() const {
  return effective_fee_baserate_;
}

uint64_t CoinSelectionOption::GetLongTermFeeBaserate() const {
  return long_term_fee_baserate_;
}

int64_t CoinSelectionOption::GetKnapsackMinimumChange() const {
  return knapsack_minimum_change_;
}

Amount CoinSelectionOption::GetDustFeeAmount(const Address& address) const {
  FeeCalculator dust_fee(
      (dust_fee_rate_ <= 0) ? kDustRelayTxFeeRate : dust_fee_rate_);
  Script locking_script = address.GetLockingScript();
  TxOut txout(Amount(), locking_script);
  TxOutReference txout_ref(txout);
  uint32_t size = txout_ref.GetSerializeSize();

  // Reference: bitcoin/src/policy/policy.cpp : GetDustThreshold()
  if (locking_script.IsWitnessProgram()) {
    // sum the sizes of the parts of a transaction input
    // with 75% segwit discount applied to the script size.
    size += (32 + 4 + 1 + (107 / kWitnessScaleFactor) + 4);
  } else {
    size += (32 + 4 + 1 + 107 + 4);  // the 148 mentioned above
  }
  return dust_fee.GetFee(size);
}

void CoinSelectionOption::SetUseBnB(bool use_bnb) { use_bnb_ = use_bnb; }

void CoinSelectionOption::SetChangeOutputSize(size_t size) {
  change_output_size_ = size;
}

void CoinSelectionOption::SetChangeSpendSize(size_t size) {
  change_spend_size_ = size;
}

void CoinSelectionOption::SetEffectiveFeeBaserate(double baserate) {
  effective_fee_baserate_ = static_cast<uint64_t>(floor(baserate * 1000));
}

void CoinSelectionOption::SetLongTermFeeBaserate(double baserate) {
  long_term_fee_baserate_ = static_cast<uint64_t>(floor(baserate * 1000));
}

void CoinSelectionOption::SetKnapsackMinimumChange(int64_t min_change) {
  knapsack_minimum_change_ = min_change;
}

void CoinSelectionOption::SetDustFeeRate(double baserate) {
  dust_fee_rate_ = static_cast<uint64_t>(floor(baserate * 1000));
}

void CoinSelectionOption::InitializeTxSizeInfo() {
  // wpkh想定
  Script wpkh_script("0014ffffffffffffffffffffffffffffffffffffffff");
  TxOut txout(Amount(), wpkh_script);
  TxOutReference txout_ref(txout);
  uint32_t size = txout_ref.GetSerializeSize();
  change_output_size_ = AbstractTransaction::GetVsizeFromSize(size, 0);
  uint32_t witness_size = 0;
  uint32_t total_size = TxIn::EstimateTxInSize(
      AddressType::kP2wpkhAddress, Script(), &witness_size);
  change_spend_size_ = AbstractTransaction::GetVsizeFromSize(
      (total_size - witness_size), witness_size);
}

#ifndef CFD_DISABLE_ELEMENTS
ConfidentialAssetId CoinSelectionOption::GetFeeAsset() const {
  return fee_asset_;
}

void CoinSelectionOption::SetFeeAsset(const ConfidentialAssetId& asset) {
  fee_asset_ = asset;
}

Amount CoinSelectionOption::GetConfidentialDustFeeAmount(
    const Address& address) const {
  FeeCalculator dust_fee(
      (dust_fee_rate_ <= 0) ? kDustRelayTxFeeRate : dust_fee_rate_);
  Script locking_script = address.GetLockingScript();
  ConfidentialTxOut ctxout(
      locking_script, ConfidentialAssetId(), ConfidentialValue());
  ConfidentialTxOutReference txout(ctxout);
  uint32_t witness_size = 0;
  uint32_t size = txout.GetSerializeSize(true, &witness_size);
  size = AbstractTransaction::GetVsizeFromSize(
      (size - witness_size), witness_size);

  // Reference: bitcoin/src/policy/policy.cpp : GetDustThreshold()
  if (locking_script.IsWitnessProgram()) {
    // sum the sizes of the parts of a transaction input
    // with 75% segwit discount applied to the script size.
    size += (32 + 4 + 1 + (107 / kWitnessScaleFactor) + 4);
  } else {
    size += (32 + 4 + 1 + 107 + 4);  // the 148 mentioned above
  }
  return dust_fee.GetFee(size);
}

void CoinSelectionOption::InitializeConfidentialTxSizeInfo() {
  uint32_t size;
  uint32_t witness_size = 0;

  // wpkh想定
  Script wpkh_script("0014ffffffffffffffffffffffffffffffffffffffff");
  ConfidentialTxOut ctxout(
      wpkh_script, ConfidentialAssetId(), ConfidentialValue());
  ConfidentialTxOutReference txout(ctxout);
  witness_size = 0;
  size = txout.GetSerializeSize(true, &witness_size);
  change_output_size_ = AbstractTransaction::GetVsizeFromSize(
      (size - witness_size), witness_size);

  witness_size = 0;
  size = ConfidentialTxIn::EstimateTxInSize(
      AddressType::kP2wpkhAddress, Script(), 0, Script(), false, false,
      &witness_size);
  change_spend_size_ = AbstractTransaction::GetVsizeFromSize(
      (size - witness_size), witness_size);
}
#endif  // CFD_DISABLE_ELEMENTS

// -----------------------------------------------------------------------------
// CoinSelection
// -----------------------------------------------------------------------------
CoinSelection::CoinSelection() : use_bnb_(true) {
  // do nothing
}

CoinSelection::CoinSelection(bool use_bnb) : use_bnb_(use_bnb) {
  // do nothing
}

std::vector<Utxo> CoinSelection::SelectCoins(
    const Amount& target_value, const std::vector<Utxo>& utxos,
    const UtxoFilter& filter, const CoinSelectionOption& option_params,
    const Amount& tx_fee_value, Amount* select_value, Amount* utxo_fee_value,
    bool* searched_bnb) {
#ifndef CFD_DISABLE_ELEMENTS
  bool first = true;
  uint8_t src[33];
  for (auto& utxo : utxos) {
    if (first) {
      memcpy(src, utxo.asset, sizeof(src));
      first = false;
    } else if (memcmp(utxo.asset, src, sizeof(src)) != 0) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to SelectCoins. Exists multiple assets in utxo list.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to SelectCoins. Exists multiple assets in utxo list.");
    }
  }
#endif
  if (select_value == nullptr) {
    warn(CFD_LOG_SOURCE, "Outparameter(select_value) is nullptr.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to select coin. Outparameter is nullptr.");
  }

  // convert utxo list
  std::vector<Utxo> work_utxos = utxos;
  std::vector<Utxo*> p_utxos;
  p_utxos.reserve(utxos.size());
  for (auto& utxo : work_utxos) {
    p_utxos.push_back(&utxo);
  }

  // initialize output parameter
  Amount utxo_fee_out = Amount();
  bool use_bnb_out = false;
  const bool consider_fee = true;
  std::vector<Utxo> result = SelectCoinsMinConf(
      target_value, p_utxos, filter, option_params, tx_fee_value, consider_fee,
      select_value, &utxo_fee_out, &use_bnb_out);
  if (utxo_fee_value != nullptr) {
    *utxo_fee_value = utxo_fee_out;
  }
  if (searched_bnb != nullptr) {
    *searched_bnb = use_bnb_out;
  }

  return result;
}

#ifndef CFD_DISABLE_ELEMENTS
std::vector<Utxo> CoinSelection::SelectCoins(
    const AmountMap& map_target_value, const std::vector<Utxo>& utxos,
    const UtxoFilter& filter, const CoinSelectionOption& option_params,
    const Amount& tx_fee_value, AmountMap* map_select_value,
    Amount* utxo_fee_value, std::map<std::string, bool>* map_searched_bnb) {
  bool calculate_fee = (option_params.GetEffectiveFeeBaserate() != 0);
  if (calculate_fee && option_params.GetFeeAsset().IsEmpty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to SelectCoins. Fee calculation option error."
        ": effective_fee_base_rate=[{}], fee_asset=[{}]",
        option_params.GetEffectiveFeeBaserate(),
        option_params.GetFeeAsset().GetHex());
    throw CfdException(
        CfdError::kCfdIllegalStateError,
        "Failed to SelectCoins. Fee calculation option error.");
  }
  if (map_select_value == nullptr) {
    warn(CFD_LOG_SOURCE, "Failed to SelectCoins. map_select_value is empty.");
    throw CfdException(
        CfdError::kCfdIllegalStateError,
        "Failed to SelectCoins. map_select_value is empty.");
  }

  // add fee asset to target asset list
  AmountMap work_target_values = map_target_value;
  ConfidentialAssetId fee_asset;
  if (calculate_fee) {
    fee_asset = option_params.GetFeeAsset();
    auto iter = work_target_values.find(fee_asset.GetHex());
    if (iter == std::end(work_target_values)) {
      work_target_values.insert(std::make_pair(fee_asset.GetHex(), Amount()));
    }
  }

  // asset exists check
  std::map<std::string, std::vector<Utxo*>> asset_utxos;
  std::vector<Utxo> work_utxos = utxos;
  for (auto& target : work_target_values) {
    // asset valid check...
    ConfidentialAssetId target_asset(target.first);
    if (target_asset.IsEmpty()) {
      warn(CFD_LOG_SOURCE, "Failed to SelectCoins. Target asset is empty.");
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to SelectCoins. Target asset is empty.");
    }

    // convert utxo list to ptr
    std::vector<Utxo*> p_utxos;
    p_utxos.reserve(utxos.size());
    for (auto& utxo : work_utxos) {
      std::vector<uint8_t> asset_byte(
          std::begin(utxo.asset), std::end(utxo.asset));
      if (target.first == ConfidentialAssetId(asset_byte).GetHex()) {
        p_utxos.push_back(&utxo);
      }
    }
    if (p_utxos.size() == 0) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to SelectCoins. Target asset is not found in utxo list."
          "target_asset=[{}]",
          target.first);
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to SelectCoins. Target asset is not found in utxo list.");
    }

    asset_utxos.insert(std::make_pair(target.first, p_utxos));
  }

  // coin selection function
  std::vector<Utxo> result;
  result.reserve(utxos.size());
  Amount tx_fee_out = tx_fee_value;
  AmountMap work_selected_values;
  Amount work_utxo_fee = Amount();
  std::map<std::string, bool> work_searched_bnb;
  auto coin_selection_function = [this, filter, option_params, &result,
                                  &tx_fee_out, &work_selected_values,
                                  &work_utxo_fee, &work_searched_bnb](
                                     const Amount& target_value,
                                     const std::vector<Utxo*> utxos,
                                     const Amount& tx_fee,
                                     const std::string asset_id,
                                     const bool consider_fee) {
    Amount select_value_out = Amount();
    Amount utxo_fee_out = Amount();
    bool use_bnb_out = false;
    std::vector<Utxo> ret_utxos = SelectCoinsMinConf(
        target_value, utxos, filter, option_params, tx_fee, consider_fee,
        &select_value_out, &utxo_fee_out, &use_bnb_out);
    std::copy(ret_utxos.begin(), ret_utxos.end(), std::back_inserter(result));
    tx_fee_out += utxo_fee_out;
    work_selected_values[asset_id] = select_value_out;
    work_utxo_fee += utxo_fee_out;
    work_searched_bnb[asset_id] = use_bnb_out;
  };

  // do coin selection exclude fee asset
  for (auto& target : work_target_values) {
    // skip fee asset
    if (target.first == fee_asset.GetHex()) {
      continue;
    }

    // fee以外の asset については、tx_fee=0, feeを考慮せずに計算
    const Amount& target_value = target.second;
    coin_selection_function(
        target_value, asset_utxos[target.first], Amount(), target.first,
        false);
  }

  // do coin selection with fee asset
  if (calculate_fee) {
    const Amount& target_value = work_target_values[fee_asset.GetHex()];
    coin_selection_function(
        target_value, asset_utxos[fee_asset.GetHex()], tx_fee_out,
        fee_asset.GetHex(), true);
  }

  if (map_select_value != nullptr) {
    *map_select_value = work_selected_values;
  }
  if (utxo_fee_value != nullptr) {
    *utxo_fee_value = work_utxo_fee;
  }
  if (map_searched_bnb != nullptr) {
    *map_searched_bnb = work_searched_bnb;
  }

  return result;
}
#endif  // CFD_DISABLE_ELEMENTS

std::vector<Utxo> CoinSelection::SelectCoinsMinConf(
    const Amount& target_value, const std::vector<Utxo*>& utxos,
    const UtxoFilter& filter, const CoinSelectionOption& option_params,
    const Amount& tx_fee_value, const bool consider_fee, Amount* select_value,
    Amount* utxo_fee_value, bool* searched_bnb) {
  // for btc default(DUST_RELAY_TX_FEE(3000)) -> DEFAULT_DISCARD_FEE(10000)
  if (select_value != nullptr) {
    *select_value = Amount::CreateBySatoshiAmount(0);
  } else {
    cfd::core::logger::info(
        CFD_LOG_SOURCE, "select_value=null, filter={}",
        static_cast<const void*>(&filter));
    // for unused parameter
  }
  if (searched_bnb != nullptr) *searched_bnb = false;

  // Copy the list to change the calculation area.
  std::vector<Utxo*> work_utxos = utxos;
  FeeCalculator effective_fee(option_params.GetEffectiveFeeBaserate());
  FeeCalculator discard_fee(kDefaultDiscardFee);
  Amount cost_of_change = Amount::CreateBySatoshiAmount(0);
  bool use_fee = false;
  if (option_params.GetEffectiveFeeBaserate() != 0) {
    cost_of_change = discard_fee.GetFee(option_params.GetChangeSpendSize()) +
                     effective_fee.GetFee(option_params.GetChangeOutputSize());
    use_fee = true;
  }

  std::vector<Utxo*> utxo_pool;
  if (use_bnb_ && option_params.IsUseBnB()) {
    // Get long term estimate
    FeeCalculator long_term_fee(option_params.GetLongTermFeeBaserate());

    // NOLINT Filter by the min conf specs and add to utxo_pool and calculate effective value
    for (auto& utxo : work_utxos) {
      if (utxo == nullptr) continue;
      // if (!group.EligibleForSpending(eligibility_filter)) continue;
      utxo->fee = 0;
      utxo->long_term_fee = 0;
      utxo->effective_value = 0;

      uint64_t fee = effective_fee.GetFee(*utxo).GetSatoshiValue();
      // Only include outputs that are positive effective value (i.e. not dust)
      if (utxo->amount > fee) {
        uint64_t effective_value = utxo->amount;
        if (use_fee) {
          if (consider_fee) {
            effective_value -= fee;
          }
          utxo->fee = fee;
          utxo->long_term_fee = long_term_fee.GetFee(*utxo).GetSatoshiValue();
        }
#if 0
        std::vector<uint8_t> txid_byte(sizeof(utxo->txid));
        memcpy(txid_byte.data(), utxo->txid, txid_byte.size());
        info(
            CFD_LOG_SOURCE, "utxo({},{}) size={}/{} amount={}/{}/{}",
            Txid(txid_byte).GetHex(), utxo->vout, utxo->uscript_size_max,
            utxo->witness_size_max, utxo->amount, utxo->fee,
            utxo->long_term_fee);
#endif
        if (utxo->long_term_fee > utxo->fee) {
          utxo->long_term_fee = utxo->fee;  // TODO(k-matsuzawa): 後で見直し
        }
        utxo->effective_value = effective_value;
        utxo_pool.push_back(utxo);
      }
    }
    // Calculate the fees for things that aren't inputs
    std::vector<Utxo> result = SelectCoinsBnB(
        target_value, utxo_pool, cost_of_change, tx_fee_value, select_value,
        utxo_fee_value);
    if (!result.empty()) {
      if (searched_bnb) *searched_bnb = true;
      return result;
    }
    // SelectCoinsBnB fail, go to KnapsackSolver.
  }

  // Filter by the min conf specs and add to utxo_pool
  // TODO(k-matsuzawa): 現状はフィルタリングしていないためUtxo一覧の再作成不要
  // for (const OutputGroup& group : groups) {
  //   if (!group.EligibleForSpending(eligibility_filter)) continue;
  //   utxo_pool.push_back(group);
  // }
  if (utxo_pool.empty()) {
    for (auto& utxo : work_utxos) {
      if (utxo == nullptr) continue;
      utxo->fee =
          (use_fee) ? effective_fee.GetFee(*utxo).GetSatoshiValue() : 0;
      if (utxo->amount > utxo->fee) {
        utxo->effective_value = utxo->amount - utxo->fee;
        utxo_pool.push_back(utxo);
      }
    }
  }
  Amount search_value = target_value;
  Amount utxo_fee = Amount::CreateBySatoshiAmount(0);
  if (tx_fee_value.GetSatoshiValue() > 0) {
    search_value += tx_fee_value;
  }
  // using minimum fee
  uint64_t min_change = kMinChange;
  int64_t opt_min_change = option_params.GetKnapsackMinimumChange();
  if (opt_min_change >= 0) {
    if ((!use_fee) || (opt_min_change > cost_of_change.GetSatoshiValue())) {
      min_change = static_cast<uint64_t>(opt_min_change);
    } else if (use_fee) {
      min_change = static_cast<uint64_t>(cost_of_change.GetSatoshiValue());
    }
  }
  std::vector<Utxo> result = KnapsackSolver(
      search_value, utxo_pool, min_change, select_value, &utxo_fee);
  if (use_fee) {
    // Check if the required amount was detected
    // (May be a non-passing route)
    int64_t find_value = select_value->GetSatoshiValue();
    int64_t need_value = search_value.GetSatoshiValue();
    need_value += utxo_fee.GetSatoshiValue();
    if (need_value > find_value) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to KnapsackSolver. Not enough utxos."
          ": find_value={}, need_value={}",
          find_value, need_value);
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to KnapsackSolver. Not enough utxos.");
    }
  }
  if (utxo_fee_value != nullptr) {
    *utxo_fee_value = utxo_fee;
  }
  return result;
}

std::vector<Utxo> CoinSelection::SelectCoinsBnB(
    const Amount& target_value, const std::vector<Utxo*>& utxos,
    const Amount& cost_of_change, const Amount& not_input_fees,
    Amount* select_value, Amount* utxo_fee_value) {
  info(
      CFD_LOG_SOURCE,
      "SelectCoinsBnB start. cost_of_change={}, not_input_fees={}",
      cost_of_change.GetSatoshiValue(), not_input_fees.GetSatoshiValue());

  std::vector<Utxo> results;
  Amount curr_value = Amount::CreateBySatoshiAmount(0);

  std::vector<bool> curr_selection;
  curr_selection.reserve(utxos.size());
  Amount actual_target = not_input_fees + target_value;

  // Calculate curr_available_value
  Amount curr_available_value = Amount::CreateBySatoshiAmount(0);
  for (const Utxo* utxo : utxos) {
    // Assert that this utxo is not negative. It should never be negative,
    //  effective value calculation should have removed it
    // assert(utxo->effective_value > 0);
    if (utxo->effective_value == 0) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to SelectCoinsBnB. effective_value is 0."
          ": effective_value={}",
          utxo->effective_value);
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Failed to select coin. effective amount is 0.");
    }
    curr_available_value += utxo->effective_value;
  }
  if (curr_available_value < actual_target) {
    // not enough amount
    warn(
        CFD_LOG_SOURCE,
        "Failed to SelectCoinsBnB. Not enough utxos."
        ": curr_available_value={},"
        "actual_target={}",
        curr_available_value.GetSatoshiValue(),
        actual_target.GetSatoshiValue());
    throw CfdException(
        CfdError::kCfdIllegalStateError,
        "Failed to select coin. Not enough utxos.");
  }

  // Sort the utxos
  std::vector<Utxo*> p_utxos = utxos;
  std::sort(p_utxos.begin(), p_utxos.end(), [](const Utxo* a, const Utxo* b) {
    return a->effective_value > b->effective_value;
  });

  Amount curr_waste = Amount::CreateBySatoshiAmount(0);
  std::vector<bool> best_selection;
  Amount best_waste = Amount::CreateBySatoshiAmount(kMaxAmount);

  // Depth First search loop for choosing the UTXOs
  for (size_t i = 0; i < kBnBMaxTotalTries; ++i) {
    // Conditions for starting a backtrack
    bool backtrack = false;
    if (curr_value + curr_available_value <
            actual_target ||  //NOLINT Cannot possibly reach target with the amount remaining in the curr_available_value.
        curr_value >
            actual_target +
                cost_of_change ||  //NOLINT Selected value is out of range, go back and try other branch
        (curr_waste > best_waste &&
         (p_utxos.at(0)->fee - p_utxos.at(0)->long_term_fee) >
             0)) {  //NOLINT Don't select things which we know will be more wasteful if the waste is increasing
      backtrack = true;
    } else if (
        curr_value >= actual_target) {  //NOLINT Selected value is within range
      curr_waste +=
          (curr_value -
           actual_target);  //NOLINT This is the excess value which is added to the waste for the below comparison
      //NOLINT Adding another UTXO after this check could bring the waste down if the long term fee is higher than the current fee.
      //NOLINT However we are not going to explore that because this optimization for the waste is only done when we have hit our target
      //NOLINT value. Adding any more UTXOs will be just burning the UTXO; it will go entirely to fees. Thus we aren't going to
      //NOLINT explore any more UTXOs to avoid burning money like that.
      if (curr_waste <= best_waste) {
        best_selection = curr_selection;
        best_selection.resize(p_utxos.size());
        best_waste = curr_waste;
      }
      curr_waste -= (curr_value - actual_target);
      //NOLINT Remove the excess value as we will be selecting different coins now
      backtrack = true;
    }

    // Backtracking, moving backwards
    if (backtrack) {
      //NOLINT Walk backwards to find the last included UTXO that still needs to have its omission branch traversed.
      while (!curr_selection.empty() && !curr_selection.back()) {
        curr_selection.pop_back();
        curr_available_value +=
            p_utxos.at(curr_selection.size())->effective_value;
      }

      if (curr_selection
              .empty()) {  //NOLINT We have walked back to the first utxo and no branch is untraversed. All solutions searched
        break;
      }

      // Output was included on previous iterations, try excluding now.
      curr_selection.back() = false;
      const Utxo* utxo = p_utxos.at(curr_selection.size() - 1);
      curr_value -= utxo->effective_value;
      curr_waste -= utxo->fee - utxo->long_term_fee;
    } else {  // Moving forwards, continuing down this branch
      const Utxo* utxo = p_utxos.at(curr_selection.size());

      // Remove this utxo from the curr_available_value utxo amount
      curr_available_value -= utxo->effective_value;

      // NOLINT Avoid searching a branch if the previous UTXO has the same value and same waste and was excluded. Since the ratio of fee to
      // NOLINT long term fee is the same, we only need to check if one of those values match in order to know that the waste is the same.
      if (!curr_selection.empty() && !curr_selection.back() &&
          utxo->effective_value ==
              p_utxos.at(curr_selection.size() - 1)->effective_value &&
          utxo->fee == p_utxos.at(curr_selection.size() - 1)->fee) {
        curr_selection.push_back(false);
      } else {
        // Inclusion branch first (Largest First Exploration)
        curr_selection.push_back(true);
        curr_value += utxo->effective_value;
        curr_waste += utxo->fee - utxo->long_term_fee;
      }
    }
  }

  // Check for solution
  Amount fee_value = Amount::CreateBySatoshiAmount(0);
  if (!best_selection.empty()) {
    // Set output set
    *select_value = Amount::CreateBySatoshiAmount(0);
    for (size_t i = 0; i < best_selection.size(); ++i) {
      if (best_selection.at(i)) {
        Utxo* p_utxo = p_utxos.at(i);
        results.push_back(*p_utxo);
        *select_value += static_cast<int64_t>(p_utxo->amount);
        fee_value += static_cast<int64_t>(p_utxo->fee);
      }
    }
  }
  if (utxo_fee_value != nullptr) {
    *utxo_fee_value = fee_value;
  }

  info(CFD_LOG_SOURCE, "SelectCoinsBnB end. results={}", results.size());
  return results;
}

std::vector<Utxo> CoinSelection::KnapsackSolver(
    const Amount& target_value, const std::vector<Utxo*>& utxos,
    uint64_t min_change, Amount* select_value, Amount* utxo_fee_value) {
  std::vector<Utxo> ret_utxos;
  uint64_t n_target = target_value.GetSatoshiValue();
  info(CFD_LOG_SOURCE, "KnapsackSolver start. target={}", n_target);

  // List of values less than target
  const Utxo* lowest_larger = nullptr;
  std::vector<const Utxo*> applicable_groups;
  uint64_t n_total = 0;
  uint64_t n_effective_total = 0;  // amount excluding fee
  uint64_t utxo_fee = 0;

  std::vector<uint32_t> indexes =
      RandomNumberUtil::GetRandomIndexes(static_cast<uint32_t>(utxos.size()));

  for (size_t index = 0; index < indexes.size(); ++index) {
    // if (utxos[index]->amount == n_target) {
    if (utxos[index]->effective_value == n_target) {
      // that meets the required value
      ret_utxos.push_back(*utxos[index]);
      *select_value = Amount::CreateBySatoshiAmount(utxos[index]->amount);
      *utxo_fee_value = Amount::CreateBySatoshiAmount(utxos[index]->fee);
      info(CFD_LOG_SOURCE, "KnapsackSolver end. results={}", ret_utxos.size());
      return ret_utxos;

    } else if (utxos[index]->effective_value < n_target + min_change) {
      // } else if ((utxos[index]->amount < n_target + min_change) {
      applicable_groups.push_back(utxos[index]);
      n_total += utxos[index]->amount;
      n_effective_total += utxos[index]->effective_value;

    } else if (
        lowest_larger == nullptr ||
        utxos[index]->amount < lowest_larger->amount) {
      // greater than `n_target + min_change`
      lowest_larger = utxos[index];
    }
  }

  // if (n_total == n_target) {
  if (n_effective_total == n_target) {
    uint64_t ret_value = 0;
    for (const auto& utxo : applicable_groups) {
      ret_utxos.push_back(*utxo);
      ret_value += utxo->amount;
      utxo_fee += utxo->fee;
    }
    *select_value = Amount::CreateBySatoshiAmount(ret_value);
    *utxo_fee_value = Amount::CreateBySatoshiAmount(utxo_fee);
    info(CFD_LOG_SOURCE, "KnapsackSolver end. results={}", ret_utxos.size());
    return ret_utxos;
  }

  // if (n_total < n_target) {
  if (n_effective_total < n_target) {
    if (lowest_larger == nullptr) {
      warn(
          CFD_LOG_SOURCE, "insufficient funds. effective_total:{} target:{}",
          n_effective_total, n_target);
      throw CfdException(
          CfdError::kCfdIllegalStateError, "insufficient funds.");
    }

    ret_utxos.push_back(*lowest_larger);
    *select_value = Amount::CreateBySatoshiAmount(lowest_larger->amount);
    *utxo_fee_value = Amount::CreateBySatoshiAmount(lowest_larger->fee);
    info(CFD_LOG_SOURCE, "KnapsackSolver end. results={}", ret_utxos.size());
    return ret_utxos;
  }

  std::sort(
      applicable_groups.begin(), applicable_groups.end(),
      [](const Utxo* a, const Utxo* b) {
        return a->effective_value > b->effective_value;
      });
  std::vector<char> vf_best;
  uint64_t n_best;

  randomize_cache_.clear();
  ApproximateBestSubset(
      applicable_groups, n_effective_total, n_target, &vf_best, &n_best,
      kApproximateBestSubsetIterations);
  if (n_best != n_target && n_effective_total >= n_target + min_change) {
    uint64_t n_best2 = n_best;
    std::vector<char> vf_best2;
    ApproximateBestSubset(
        applicable_groups, n_effective_total, (n_target + min_change),
        &vf_best2, &n_best2, kApproximateBestSubsetIterations);
    if ((n_best2 == n_target) || (n_best > n_best2)) {
      n_best = n_best2;
      vf_best = vf_best2;
    }
  }

  // NOLINT If we have a bigger coin and (either the stochastic approximation didn't find a good solution,
  // NOLINT                                or the next bigger coin is closer), return the bigger coin
  if (lowest_larger != nullptr &&
      ((n_best != n_target && n_best < n_target + min_change) ||
       lowest_larger->effective_value <= n_best)) {
    // lowest_larger->amount <= n_best)) {
    ret_utxos.push_back(*lowest_larger);
    *select_value = Amount::CreateBySatoshiAmount(lowest_larger->amount);
    *utxo_fee_value = Amount::CreateBySatoshiAmount(lowest_larger->fee);

  } else {
    uint64_t ret_value = 0;
    for (unsigned int i = 0; i < applicable_groups.size(); i++) {
      if (vf_best[i]) {
        ret_utxos.push_back(*applicable_groups[i]);
        ret_value += applicable_groups[i]->amount;
        utxo_fee += applicable_groups[i]->fee;
      }
    }
    *select_value = Amount::CreateBySatoshiAmount(ret_value);
    *utxo_fee_value = Amount::CreateBySatoshiAmount(utxo_fee);
  }
  info(CFD_LOG_SOURCE, "KnapsackSolver end. results={}", ret_utxos.size());
  return ret_utxos;
}

void CoinSelection::ApproximateBestSubset(
    const std::vector<const Utxo*>& utxos, uint64_t n_total_value,
    uint64_t n_target_value, std::vector<char>* vf_best, uint64_t* n_best,
    int iterations) {
  if (vf_best == nullptr || n_best == nullptr) {
    warn(CFD_LOG_SOURCE, "Outparameter(select_value) is nullptr.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to select coin. Outparameter is nullptr.");
  }

  std::vector<char> vf_includes;
  vf_best->assign(utxos.size(), true);
  *n_best = n_total_value;

  for (int n_rep = 0; n_rep < iterations && *n_best != n_target_value;
       n_rep++) {
    vf_includes.assign(utxos.size(), false);
    uint64_t n_total = 0;
    bool is_reached_target = false;
    for (int n_pass = 0; n_pass < 2 && !is_reached_target; n_pass++) {
      for (unsigned int i = 0; i < utxos.size(); i++) {
        // The solver here uses a randomized algorithm,
        // the randomness serves no real security purpose but is just
        // needed to prevent degenerate behavior and it is important
        // that the rng is fast. We do not use a constant random sequence,
        // because there may be some privacy improvement by making
        // the selection random.
        bool rand_bool = !vf_includes[i];
        if (n_pass == 0) {
          rand_bool = RandomNumberUtil::GetRandomBool(&randomize_cache_);
        }
        if (rand_bool) {
          // n_total += utxos[i]->amount;
          n_total += utxos[i]->effective_value;
          vf_includes[i] = true;
          if (n_total >= n_target_value) {
            is_reached_target = true;
            if (n_total < *n_best) {
              *n_best = n_total;
              *vf_best = vf_includes;
            }
            // n_total -= utxos[i]->amount;
            n_total -= utxos[i]->effective_value;
            vf_includes[i] = false;
          }
        }
      }
    }
  }
}

void CoinSelection::ConvertToUtxo(
    const Txid& txid, uint32_t vout, const std::string& output_descriptor,
    const Amount& amount, const std::string& asset, const void* binary_data,
    Utxo* utxo) {
  static constexpr const uint16_t kScriptSize = 50;
  if (utxo == nullptr) {
    warn(CFD_LOG_SOURCE, "utxo is nullptr.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to convert utxo. utxo is nullptr.");
  }
  memset(utxo, 0, sizeof(*utxo));
  ByteData txid_data = txid.GetData();
  if (!txid_data.Empty()) {
    memcpy(utxo->txid, txid_data.GetBytes().data(), sizeof(utxo->txid));
  }
  utxo->vout = vout;

  // TODO(k-matsuzawa): descriptor解析は暫定対処。本対応が必要。

  uint32_t minimum_txin = static_cast<uint32_t>(TxIn::kMinimumTxInSize);
  uint32_t witness_size = 0;
  if (output_descriptor.find("wpkh(") == 0) {
    utxo->address_type = AddressType::kP2wpkhAddress;
    TxIn::EstimateTxInSize(
        AddressType::kP2wpkhAddress, Script(), &witness_size);
    utxo->witness_size_max = static_cast<uint16_t>(witness_size);
  } else if (output_descriptor.find("wsh(") == 0) {
    utxo->address_type = AddressType::kP2wshAddress;
    TxIn::EstimateTxInSize(
        AddressType::kP2wshAddress, Script(), &witness_size);
    utxo->witness_size_max = static_cast<uint16_t>(witness_size);
    utxo->witness_size_max += kScriptSize;
  } else if (output_descriptor.find("sh(") == 0) {
    if (output_descriptor.find("sh(wpkh(") == 0) {
      utxo->address_type = AddressType::kP2shP2wpkhAddress;
      utxo->uscript_size_max = 22;
      TxIn::EstimateTxInSize(
          AddressType::kP2wpkhAddress, Script(), &witness_size);
      utxo->witness_size_max = static_cast<uint16_t>(witness_size);
    } else if (output_descriptor.find("sh(wsh(") == 0) {
      utxo->address_type = AddressType::kP2shP2wshAddress;
      utxo->uscript_size_max = 34;
      TxIn::EstimateTxInSize(
          AddressType::kP2wshAddress, Script(), &witness_size);
      utxo->witness_size_max = static_cast<uint16_t>(witness_size);
      utxo->witness_size_max += kScriptSize;
    } else {
      utxo->address_type = AddressType::kP2shAddress;
      utxo->uscript_size_max = kScriptSize;
      utxo->uscript_size_max +=
          TxIn::EstimateTxInSize(AddressType::kP2shAddress, Script()) -
          minimum_txin;
    }
  } else if (output_descriptor.find("pkh(") == 0) {
    utxo->address_type = AddressType::kP2pkhAddress;
    utxo->uscript_size_max =
        TxIn::EstimateTxInSize(AddressType::kP2pkhAddress, Script()) -
        minimum_txin;
  } else {
    // unknown type?
  }
#if 0
  const std::vector<uint8_t>& scrpt = locking_script.GetData().GetBytes();
  if (scrpt.size() < sizeof(utxo->locking_script)) {
    memcpy(utxo->locking_script, scrpt.data(), sizeof(utxo->locking_script));
    utxo->script_length = static_cast<uint16_t>(scrpt.size());
  }
#endif

  utxo->amount = amount.GetSatoshiValue();
  memcpy(&utxo->binary_data, &binary_data, sizeof(void*));
  utxo->effective_value = utxo->amount;

#ifndef CFD_DISABLE_ELEMENTS
  if (!asset.empty()) {
    ConfidentialAssetId asset_data(asset);
    utxo->blinded = asset_data.HasBlinding();
    memcpy(
        utxo->asset, asset_data.GetData().GetBytes().data(),
        sizeof(utxo->asset));
  }
#endif  // CFD_DISABLE_ELEMENTS
}

void CoinSelection::ConvertToUtxo(
    uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
    uint32_t vout, const Script& locking_script,
    const std::string& output_descriptor, const Amount& amount,
    const void* binary_data, Utxo* utxo) {
  static constexpr const uint16_t kScriptSize = 50;
  if (utxo == nullptr) {
    warn(CFD_LOG_SOURCE, "utxo is nullptr.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to convert utxo. utxo is nullptr.");
  }
  memset(utxo, 0, sizeof(*utxo));

  utxo->block_height = block_height;
  if (!block_hash.GetData().Empty()) {
    memcpy(
        utxo->block_hash, block_hash.GetData().GetBytes().data(),
        sizeof(utxo->block_hash));
  }
  ByteData txid_data = txid.GetData();
  if (!txid_data.Empty()) {
    memcpy(utxo->txid, txid_data.GetBytes().data(), sizeof(utxo->txid));
  }
  utxo->vout = vout;
  const std::vector<uint8_t>& script = locking_script.GetData().GetBytes();
  if (script.size() < sizeof(utxo->locking_script)) {
    memcpy(utxo->locking_script, script.data(), script.size());
    utxo->script_length = static_cast<uint16_t>(script.size());
  }

  uint32_t minimum_txin = static_cast<uint32_t>(TxIn::kMinimumTxInSize);
  uint32_t witness_size = 0;
  if (locking_script.IsP2pkhScript()) {
    utxo->address_type = AddressType::kP2pkhAddress;
    utxo->uscript_size_max =
        TxIn::EstimateTxInSize(AddressType::kP2pkhAddress, Script()) -
        minimum_txin;
  } else if (locking_script.IsP2shScript()) {
    utxo->address_type = AddressType::kP2shAddress;
    utxo->uscript_size_max = kScriptSize;
    utxo->uscript_size_max +=
        TxIn::EstimateTxInSize(AddressType::kP2shAddress, Script()) -
        minimum_txin;
    if (output_descriptor.find("sh(wpkh(") == 0) {
      utxo->address_type = AddressType::kP2shP2wpkhAddress;
      utxo->uscript_size_max = 22;
      TxIn::EstimateTxInSize(
          AddressType::kP2wpkhAddress, Script(), &witness_size);
      utxo->witness_size_max = static_cast<uint16_t>(witness_size);
    } else if (output_descriptor.find("sh(wsh(") == 0) {
      utxo->address_type = AddressType::kP2shP2wshAddress;
      utxo->uscript_size_max = 34;
      TxIn::EstimateTxInSize(
          AddressType::kP2wshAddress, Script(), &witness_size);
      utxo->witness_size_max = static_cast<uint16_t>(witness_size);
      utxo->witness_size_max += kScriptSize;
    }
  } else if (locking_script.IsP2wpkhScript()) {
    utxo->address_type = AddressType::kP2wpkhAddress;
    TxIn::EstimateTxInSize(
        AddressType::kP2wpkhAddress, Script(), &witness_size);
    utxo->witness_size_max = static_cast<uint16_t>(witness_size);
  } else if (locking_script.IsP2wshScript()) {
    utxo->address_type = AddressType::kP2wshAddress;
    TxIn::EstimateTxInSize(
        AddressType::kP2wshAddress, Script(), &witness_size);
    utxo->witness_size_max = static_cast<uint16_t>(witness_size);
    utxo->witness_size_max += kScriptSize;
  }
  // TODO(k-matsuzawa): 後でOutputDescriptor対応する
  utxo->amount = amount.GetSatoshiValue();
  memcpy(&utxo->binary_data, &binary_data, sizeof(void*));
  utxo->effective_value = utxo->amount;
}

#ifndef CFD_DISABLE_ELEMENTS
void CoinSelection::ConvertToUtxo(
    uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
    uint32_t vout, const Script& locking_script,
    const std::string& output_descriptor, const Amount& amount,
    const ConfidentialAssetId& asset, const void* binary_data, Utxo* utxo) {
  ConvertToUtxo(
      block_height, block_hash, txid, vout, locking_script, output_descriptor,
      amount, binary_data, utxo);
  utxo->blinded = asset.HasBlinding();
  memcpy(utxo->asset, asset.GetData().GetBytes().data(), sizeof(utxo->asset));
}
#endif  // CFD_DISABLE_ELEMENTS

}  // namespace cfd
