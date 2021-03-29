// Copyright 2019 CryptoGarage
/**
 * @file cfd_utxo.h
 *
 * @brief Related class definitions for UTXO operations
 */
#ifndef CFD_INCLUDE_CFD_CFD_UTXO_H_
#define CFD_INCLUDE_CFD_CFD_UTXO_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::Amount;
using cfd::core::BlockHash;
using cfd::core::Script;
using cfd::core::Txid;
#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::ConfidentialAssetId;
#endif  // CFD_DISABLE_ELEMENTS
//! Alias ​​to Asset Amount map (key: asset(str), value: amount)
using AmountMap = std::map<std::string, int64_t>;

/**
 * @brief A UTXO structure that holds only the smallest data.
 * @details witness_size_max, uscript_size_max, address_type
 *  is calculated by the dedicated API.
 * @see cfd::CoinSelection::ConvertToUtxo()
 */
struct Utxo {
  uint64_t block_height;       //!< blick高
  uint8_t block_hash[32];      //!< block hash
  uint8_t txid[32];            //!< txid
  uint32_t vout;               //!< vout
  uint8_t locking_script[40];  //!< locking script
  uint16_t script_length;      //!< locking script length
  uint16_t address_type;       //!< address type (cfd::core::AddressType)
  uint16_t witness_size_max;   //!< witness stack size maximum
  uint16_t uscript_size_max;   //!< unlocking script size maximum
  uint64_t amount;             //!< amount
#ifndef CFD_DISABLE_ELEMENTS
  bool blinded;       //!< has blind
  uint8_t asset[33];  //!< asset
#endif                // CFD_DISABLE_ELEMENTS
  /**
   * @brief Any binary data address
   * @details In cfd, only the area is provided and it is not accessed.
   */
  void* binary_data;
  // calculate
  uint64_t effective_value;   //!< Effective amount excluding fee from amount
  uint64_t fee;               //!< fee
  uint64_t long_term_fee;     //!< Fee after a long term
  int64_t effective_k_value;  //!< Effective amount for knapsack
};

/**
 * @brief Specify UTXO filtering conditions.
 */
struct UtxoFilter {
  uint32_t reserved;  //!< reserved
};

/**
 * @brief Class that holds option information of CoinSelection.
 */
class CFD_EXPORT CoinSelectionOption {
 public:
  /**
   * @brief constructor.
   */
  CoinSelectionOption();

  /**
   * @brief Get the BnB using flag.
   * @retval true   use BnB
   * @retval false  unuse BnB
   */
  bool IsUseBnB() const;
  /**
   * @brief Get the output change size.
   * @return output change size.
   */
  uint32_t GetChangeOutputSize() const;
  /**
   * @brief Get the spend change size.
   * @return spend change size.
   */
  uint32_t GetChangeSpendSize() const;
  /**
   * @brief Get an effective fee baserate.
   * @return effective fee baserate.
   */
  uint64_t GetEffectiveFeeBaserate() const;
  /**
   * @brief Get a long-term fee baserate.
   * @return long-term fee baserate.
   */
  uint64_t GetLongTermFeeBaserate() const;
  /**
   * @brief Get the minimum addition value when searching for knapsack.
   * @return knapsack minimum change
   */
  int64_t GetKnapsackMinimumChange() const;
  /**
   * @brief Get the maximum amount that will be included in Fee as dust.
   * @param[in] address     txout address
   * @return dust fee satoshi
   */
  Amount GetDustFeeAmount(const Address& address) const;

  /**
   * @brief Set the BnB using flag.
   * @param[in] use_bnb   BnB using flag.
   */
  void SetUseBnB(bool use_bnb);
  /**
   * @brief Set the output change size.
   * @param[in] size    output change size.
   */
  void SetChangeOutputSize(size_t size);
  /**
   * @brief Set the spend change size.
   * @param[in] size    spend change size.
   */
  void SetChangeSpendSize(size_t size);
  /**
   * @brief Set an effective fee baserate.
   * @param[in] baserate    fee baserate (for BTC/byte)
   */
  void SetEffectiveFeeBaserate(double baserate);
  /**
   * @brief Set a long-term fee baserate.
   * @param[in] baserate    fee baserate (for BTC/byte)
   */
  void SetLongTermFeeBaserate(double baserate);
  /**
   * @brief Set the minimum addition value when searching for knapsack.
   * @param[in] min_change    knapsack minimum change
   */
  void SetKnapsackMinimumChange(int64_t min_change);
  /**
   * @brief Set the maximum amount that will be included in Fee as dust.
   * @param[in] baserate    fee baserate (for BTC/byte)
   */
  void SetDustFeeRate(double baserate);

  /**
   * @brief Initializes size related information equivalent to bitcoin.
   */
  void InitializeTxSizeInfo();

#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief Get the asset to use for the fee.
   * @return asset to use for the fee.
   */
  ConfidentialAssetId GetFeeAsset() const;
  /**
   * @brief Set the asset to use for the fee.
   * @param[in] asset   asset
   */
  void SetFeeAsset(const ConfidentialAssetId& asset);
  /**
   * @brief Get the maximum amount that will be included in Fee as dust.
   * @param[in] address     txout address
   * @return dust fee satoshi
   */
  Amount GetConfidentialDustFeeAmount(const Address& address) const;

  /**
   * @brief set blind information.
   * @param[in] exponent                  rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits              rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   */
  void SetBlindInfo(int exponent, int minimum_bits);

  /**
   * @brief get blind information.
   * @param[out] exponent                  rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[out] minimum_bits              rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   */
  void GetBlindInfo(int* exponent, int* minimum_bits) const;

  /**
   * @brief Initializes size-related information based on Confidential Tx.
   */
  void InitializeConfidentialTxSizeInfo();
#endif  // CFD_DISABLE_ELEMENTS

 private:
  bool use_bnb_ = true;              //!< BnB using flag
  uint32_t change_output_size_ = 0;  //!< output change size
  uint32_t change_spend_size_ = 0;   //!< spend change size
  uint64_t effective_fee_baserate_;  //!< fee baserate
  uint64_t long_term_fee_baserate_;  //!< longterm fee baserate
  int64_t knapsack_minimum_change_;  //!< knapsack min change
  int64_t dust_fee_rate_;            //!< dust fee rate
#ifndef CFD_DISABLE_ELEMENTS
  ConfidentialAssetId fee_asset_;  //!< asset to be used as a fee
  int exponent_ = 0;               //!< rangeproof exponent value
  int minimum_bits_ = cfd::core::kDefaultBlindMinimumBits;  //!< minimum bits
#endif  // CFD_DISABLE_ELEMENTS
};

/**
 * @brief Class that performs CoinSelection calculation
 */
class CFD_EXPORT CoinSelection {
 public:
  /**
   * @brief Constructor
   */
  CoinSelection();

  /**
   * @brief Constructor
   * @param[in] use_bnb
   */
  explicit CoinSelection(bool use_bnb);

  /**
   * @brief Select the smallest Coin.
   * @param[in] target_value    Collection amount
   * @param[in] utxos           List of UTXOs to be searched
   * @param[in] filter          UTXO collection filter (reserved)
   * @param[in] option_params   collect Option
   * @param[in] tx_fee_value    transaction fee information
   * @param[out] select_value   Total collection amount
   * @param[out] utxo_fee_value the fee amount for utxo
   * @param[out] searched_bnb   Flag of whether you searched with BnB
   * @return UTXO list. If it is empty, the error ends.
   */
  std::vector<Utxo> SelectCoins(
      const Amount& target_value, const std::vector<Utxo>& utxos,
      const UtxoFilter& filter, const CoinSelectionOption& option_params,
      const Amount& tx_fee_value, Amount* select_value,
      Amount* utxo_fee_value = nullptr, bool* searched_bnb = nullptr);

#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief Select the smallest Coin. (Multi-asset version)
   * @details CoinSelection is performed on utxos with the asset id \
   *    specified by the key of map_target_value. \
   *    In addition, if fee asset is specified in option_param, \
   *    CoinSelection for the target asset is also executed.
   * @param[in] map_target_value  Collection amount map for each Asset.
   *    For bitcoin, set the key to an empty string
   * @param[in,out] utxos         List of UTXOs to be searched
   * @param[in] filter            UTXO collection filter (reserved)
   * @param[in] option_params     collect Option
   * @param[in] tx_fee_value      transaction fee information
   * @param[out] map_select_value Total collection amount map.
   *   The collection amount for each Asset specified by map_target_value is stored.
   * @param[out] utxo_fee_value   the fee amount for utxo
   * @param[out] map_searched_bnb Flag of whether you searched with BnB.
   *   The result is stored for each Asset specified by map_target_value.
   * @return UTXO list. If it is empty, the error ends.
   */
  std::vector<Utxo> SelectCoins(
      const AmountMap& map_target_value, const std::vector<Utxo>& utxos,
      const UtxoFilter& filter, const CoinSelectionOption& option_params,
      const Amount& tx_fee_value, AmountMap* map_select_value,
      Amount* utxo_fee_value = nullptr,
      std::map<std::string, bool>* map_searched_bnb = nullptr);
#endif  // CFD_DISABLE_ELEMENTS

  /**
   * @brief Conversion to UTXO structure.
   * @param[in] txid                txid
   * @param[in] vout                vout
   * @param[in] output_descriptor   output descriptor
   * @param[in] amount              amount
   * @param[in] asset               asset
   * @param[in] binary_data         Any data address
   * @param[out] utxo               Converted UTXO
   * @param[in] scriptsig_template  scriptsig template
   */
  static void ConvertToUtxo(
      const Txid& txid, uint32_t vout, const std::string& output_descriptor,
      const Amount& amount, const std::string& asset, const void* binary_data,
      Utxo* utxo, const Script* scriptsig_template = nullptr);

  /**
   * @brief Conversion to UTXO structure.
   * @param[in] block_height        block height
   * @param[in] block_hash          block hash
   * @param[in] txid                txid
   * @param[in] vout                vout
   * @param[in] locking_script      locking script
   * @param[in] output_descriptor   output descriptor
   * @param[in] amount              amount
   * @param[in] binary_data         Any data address
   * @param[out] utxo               Converted UTXO
   * @param[in] scriptsig_template  scriptsig template
   */
  static void ConvertToUtxo(
      uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
      uint32_t vout, const Script& locking_script,
      const std::string& output_descriptor, const Amount& amount,
      const void* binary_data, Utxo* utxo,
      const Script* scriptsig_template = nullptr);

#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief Conversion to UTXO structure.
   * @param[in] block_height        block height
   * @param[in] block_hash          block hash
   * @param[in] txid                txid
   * @param[in] vout                vout
   * @param[in] locking_script      locking script
   * @param[in] output_descriptor   output descriptor
   * @param[in] amount              amount
   * @param[in] asset               asset
   * @param[in] binary_data         Any data address
   * @param[out] utxo               Converted UTXO
   * @param[in] scriptsig_template  scriptsig template
   */
  static void ConvertToUtxo(
      uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
      uint32_t vout, const Script& locking_script,
      const std::string& output_descriptor, const Amount& amount,
      const ConfidentialAssetId& asset, const void* binary_data, Utxo* utxo,
      const Script* scriptsig_template = nullptr);
#endif  // CFD_DISABLE_ELEMENTS

 protected:
  /**
   * @brief Select the smallest Coin.
   * @details For utxo specified by utxos, it is not assumed that \
   *    multiple assets will be input. Therefore, if a utxo with \
   *    a mixture of multiple assets is input, there is a possibility \
   *    that multiple assets will be mixed in the returned utxo.
   * @param[in] target_value    Collection amount
   * @param[in] utxos           List of UTXOs to be searched
   * @param[in] filter          UTXO collection filter (reserved)
   * @param[in] option_params   collect Option
   * @param[in] tx_fee_value    transaction fee information
   * @param[in] consider_fee    \
   *    Coin Selection implementation flag considering fee (default: true)
   * @param[out] select_value   Total collection amount
   * @param[out] utxo_fee_value the fee amount for utxo
   * @param[out] searched_bnb   Flag of whether you searched with BnB
   * @return UTXO list. If it is empty, the error ends.
   */
  std::vector<Utxo> SelectCoinsMinConf(
      const int64_t& target_value, const std::vector<Utxo*>& utxos,
      const UtxoFilter& filter, const CoinSelectionOption& option_params,
      const Amount& tx_fee_value, const bool consider_fee,
      int64_t* select_value, Amount* utxo_fee_value = nullptr,
      bool* searched_bnb = nullptr);

  /**
   * @brief Perform Coin Selection (BnB).
   * @param[in] target_value      Collection amount
   * @param[in] utxos             List of UTXOs to be searched
   * @param[in] cost_of_change    Cost change range.
   *    target_value + This value is the upper limit of collection.
   * @param[in] not_input_fees    Fee amount excluding TxIn part
   * @param[in] ignore_error      ignore throw exception.
   * @param[out] select_value     Total collection amount
   * @param[out] utxo_fee_value   the fee amount for utxo
   * @return UTXO list. If it is empty, the error ends.
   */
  std::vector<Utxo> SelectCoinsBnB(
      const int64_t& target_value, const std::vector<Utxo*>& utxos,
      const int64_t& cost_of_change, const Amount& not_input_fees,
      bool ignore_error, int64_t* select_value, Amount* utxo_fee_value);

  /**
   * @brief Perform Coin Selection (Knapsack Solver).
   * @param[in] target_value      Collection amount
   * @param[in] utxos             List of UTXOs to be searched
   * @param[in] min_change        Minimum change amount
   * @param[out] select_value     Total collection amount
   * @param[out] utxo_fee_value   the fee amount for utxo
   * @return UTXO list. If it is empty, the error ends.
   */
  std::vector<Utxo> KnapsackSolver(
      const int64_t& target_value, const std::vector<Utxo*>& utxos,
      uint64_t min_change, int64_t* select_value, Amount* utxo_fee_value);

 private:
  bool use_bnb_;                       //!< BnB using flag
  std::vector<bool> randomize_cache_;  //!< randomize cache

  /**
   * Determine the UTXO list with the total amount closest to the collected amount
   * @param[in]  utxos          UTXO list smaller than the collected amount
   * @param[in]  n_total_value  Total amount of utxo list
   * @param[in]  n_target_value Collection amount
   * @param[out] vf_best        List of flags to be collected
   * @param[out] n_best         Total amount closest to the collected amount
   * @param[in]  iterations     Number of repetitions
   */
  void ApproximateBestSubset(
      const std::vector<const Utxo*>& utxos, int64_t n_total_value,
      int64_t n_target_value, std::vector<char>* vf_best, int64_t* n_best,
      int iterations);
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_UTXO_H_
