// Copyright 2019 CryptoGarage
/**
 * @file cfd_utxo.h
 *
 * @brief UTXO操作の関連クラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_UTXO_H_
#define CFD_INCLUDE_CFD_CFD_UTXO_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_transaction.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::TransactionController;
using cfd::core::Address;
using cfd::core::Amount;
using cfd::core::BlockHash;
using cfd::core::Script;
using cfd::core::Txid;
#ifndef CFD_DISABLE_ELEMENTS
using cfd::ConfidentialTransactionController;
using cfd::core::ConfidentialAssetId;
#endif  // CFD_DISABLE_ELEMENTS
//! Asset Amountマップへのエイリアス (key: asset(str), value: amount)
using AmountMap = std::map<std::string, Amount>;

/**
 * @brief 最小のデータのみを保持するUTXO構造体。
 * @details witness_size_max, uscript_size_max, address_type
 *  は専用APIで算出する。
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
   * @brief 任意のバイナリデータアドレス
   * @details cfdでは領域だけ設けておき、アクセスはしない
   */
  void* binary_data;
#if 0
  int32_t status;           //!< utxo status (reserved)
  // elements
  uint8_t confidential_key[33];      //!< Confidential key
#endif  // if 0
  // calculate
  uint64_t effective_value;  //!< amountからfeeを除外した有効額
  uint64_t fee;              //!< fee
  uint64_t long_term_fee;    //!< 長期間後のfee
};

/**
 * @brief UTXOのフィルタリング条件を指定する。
 *   utxoのamount上限などの指定に利用することを想定
 */
struct UtxoFilter {
  uint32_t reserved;  //!< 予約領域
};

/**
 * @brief CoinSelectionのオプション情報を保持するクラス
 */
class CFD_EXPORT CoinSelectionOption {
 public:
  /**
   * @brief コンストラクタ
   */
  CoinSelectionOption();

  /**
   * @brief BnB使用フラグを取得します.
   * @retval true BnB使用
   * @retval false BnB未使用
   */
  bool IsUseBnB() const;
  /**
   * @brief 出力変更サイズを取得します.
   * @return 出力変更サイズ
   */
  uint32_t GetChangeOutputSize() const;
  /**
   * @brief 出力変更サイズを取得します.
   * @return 出力変更サイズ
   */
  uint32_t GetChangeSpendSize() const;
  /**
   * @brief 効果的なfeeのbaserateを取得します.
   * @return 効果的なfeeのbaserate
   */
  uint64_t GetEffectiveFeeBaserate() const;
  /**
   * @brief 長期的なfeeのbaserateを取得します.
   * @return 長期的なfeeのbaserate
   */
  uint64_t GetLongTermFeeBaserate() const;
  /**
   * @brief knapsack探索時の最小の加算値を取得します.
   * @return knapsack minimum change
   */
  int64_t GetKnapsackMinimumChange() const;
  /**
   * @brief DustとしてFeeに取り込まれる上限額を取得します.
   * @param[in] address     txout address
   * @return dust fee satoshi
   */
  Amount GetDustFeeAmount(const Address& address) const;

  /**
   * @brief BnB 使用フラグを設定します.
   * @param[in] use_bnb   BnB 使用フラグ
   */
  void SetUseBnB(bool use_bnb);
  /**
   * @brief 出力変更サイズを設定します.
   * @param[in] size    サイズ
   */
  void SetChangeOutputSize(size_t size);
  /**
   * @brief 受入変更サイズを設定します.
   * @param[in] size    サイズ
   */
  void SetChangeSpendSize(size_t size);
  /**
   * @brief 効果的なfeeのbaserateを設定します.
   * @param[in] baserate    fee baserate (for BTC/byte)
   */
  void SetEffectiveFeeBaserate(double baserate);
  /**
   * @brief 長期的なfeeのbaserateを設定します.
   * @param[in] baserate    fee baserate (for BTC/byte)
   */
  void SetLongTermFeeBaserate(double baserate);
  /**
   * @brief knapsack探索時の最小の加算値を設定します.
   * @param[in] min_change    knapsack minimum change
   */
  void SetKnapsackMinimumChange(int64_t min_change);
  /**
   * @brief DustとしてFeeに取り込まれる額のrateを設定します.
   * @param[in] baserate    fee baserate (for BTC/byte)
   */
  void SetDustFeeRate(double baserate);

  /**
   * @brief bitcoin相当でサイズ関連情報を初期化します。
   */
  void InitializeTxSizeInfo();

#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief feeに使用するassetを取得します.
   * @return feeに使用するasset
   */
  ConfidentialAssetId GetFeeAsset() const;
  /**
   * @brief feeに使用するassetを設定します.
   * @param[in] asset   asset
   */
  void SetFeeAsset(const ConfidentialAssetId& asset);
  /**
   * @brief DustとしてFeeに取り込まれる上限額を取得します.
   * @param[in] address     txout address
   * @return dust fee satoshi
   */
  Amount GetConfidentialDustFeeAmount(const Address& address) const;

  /**
   * @brief ConfidentialTxベースでサイズ関連情報を初期化します。
   */
  void InitializeConfidentialTxSizeInfo();
#endif  // CFD_DISABLE_ELEMENTS

 private:
  bool use_bnb_ = true;              //!< BnB 使用フラグ
  uint32_t change_output_size_ = 0;  //!< 出力変更サイズ
  uint32_t change_spend_size_ = 0;   //!< 受入変更サイズ
  uint64_t effective_fee_baserate_;  //!< fee baserate
  uint64_t long_term_fee_baserate_;  //!< longterm fee baserate
  int64_t knapsack_minimum_change_;  //!< knapsack min change
  int64_t dust_fee_rate_;            //!< dust fee rate
#ifndef CFD_DISABLE_ELEMENTS
  ConfidentialAssetId fee_asset_;  //!< feeとして利用するasset
#endif                             // CFD_DISABLE_ELEMENTS
};

/**
 * @brief CoinSelection計算を行うクラス
 */
class CFD_EXPORT CoinSelection {
 public:
  /**
   * @brief コンストラクタ
   */
  CoinSelection();

  /**
   * @brief コンストラクタ
   * @param[in] use_bnb
   */
  explicit CoinSelection(bool use_bnb);

  /**
   * @brief 最小のCoinを選択する。
   * @param[in] target_value    収集額
   * @param[in] utxos           検索対象UTXO一覧
   * @param[in] filter          UTXO収集フィルタ情報
   * @param[in] option_params   オプション情報
   * @param[in] tx_fee_value    transaction fee information
   * @param[out] select_value   UTXO収集成功時、合計収集額
   * @param[out] utxo_fee_value UTXO収集成功時、utxo分のfee金額
   * @param[out] searched_bnb   BnBで検索したかのフラグ
   * @return UTXO一覧。空の場合はエラー終了。
   */
  std::vector<Utxo> SelectCoins(
      const Amount& target_value, const std::vector<Utxo>& utxos,
      const UtxoFilter& filter, const CoinSelectionOption& option_params,
      const Amount& tx_fee_value, Amount* select_value,
      Amount* utxo_fee_value = nullptr, bool* searched_bnb = nullptr);

#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief 最小のCoinを選択する。(マルチアセット版)
   * @details utxosに対して、map_target_valueのkeyで指定された asset id で
   *   CoinSelectionを行う。また、option_paramで fee asset が指定されている場合
   *   対象のassetに対してのCoinSelectionも実施する。
   * @param[in] map_target_value  Asset毎の収集額map
   *  bitcoinの場合はkeyに空文字を設定
   * @param[in,out] utxos         検索対象UTXO一覧
   * @param[in] filter            UTXO収集フィルタ情報
   * @param[in] option_params     オプション情報
   * @param[in] tx_fee_value      transaction fee information
   * @param[out] map_select_value UTXO収集成功時、Asset毎の合計収集額map
   *   map_target_valueで指定されたAsset毎の収集額が格納される
   * @param[out] utxo_fee_value   UTXO収集成功時、utxo分のfee金額
   * @param[out] map_searched_bnb asset毎にBnBで検索したかのフラグ
   *   map_target_valueで指定されたAsset毎に結果が格納される
   * @return UTXO一覧。空の場合はエラー終了。
   */
  std::vector<Utxo> SelectCoins(
      const AmountMap& map_target_value, const std::vector<Utxo>& utxos,
      const UtxoFilter& filter, const CoinSelectionOption& option_params,
      const Amount& tx_fee_value, AmountMap* map_select_value,
      Amount* utxo_fee_value = nullptr,
      std::map<std::string, bool>* map_searched_bnb = nullptr);
#endif  // CFD_DISABLE_ELEMENTS

  /**
   * @brief UTXO構造体への変換を行う。
   * @param[in] txid                txid
   * @param[in] vout                vout
   * @param[in] output_descriptor   output descriptor
   * @param[in] amount              amount
   * @param[in] asset               asset
   * @param[in] binary_data         任意のデータアドレス
   * @param[out] utxo               変換後のUTXO
   */
  static void ConvertToUtxo(
      const Txid& txid, uint32_t vout, const std::string& output_descriptor,
      const Amount& amount, const std::string& asset, const void* binary_data,
      Utxo* utxo);

  /**
   * @brief UTXO構造体への変換を行う。
   * @param[in] block_height        block高
   * @param[in] block_hash          blockハッシュ
   * @param[in] txid                txid
   * @param[in] vout                vout
   * @param[in] locking_script      block高
   * @param[in] output_descriptor   output descriptor
   * @param[in] amount              amount
   * @param[in] binary_data         任意のデータアドレス
   * @param[out] utxo               変換後のUTXO
   */
  static void ConvertToUtxo(
      uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
      uint32_t vout, const Script& locking_script,
      const std::string& output_descriptor, const Amount& amount,
      const void* binary_data, Utxo* utxo);

#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief UTXO構造体への変換を行う。
   * @param[in] block_height        block高
   * @param[in] block_hash          blockハッシュ
   * @param[in] txid                txid
   * @param[in] vout                vout
   * @param[in] locking_script      block高
   * @param[in] output_descriptor   output descriptor
   * @param[in] amount              amount
   * @param[in] asset               asset
   * @param[in] binary_data         任意のデータアドレス
   * @param[out] utxo               変換後のUTXO
   */
  static void ConvertToUtxo(
      uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
      uint32_t vout, const Script& locking_script,
      const std::string& output_descriptor, const Amount& amount,
      const ConfidentialAssetId& asset, const void* binary_data, Utxo* utxo);
#endif  // CFD_DISABLE_ELEMENTS

 protected:
  /**
   * @brief 最小のCoinを選択する。
   * @details utxosで指定されるutxoについては、複数のassetが入力されることは
   *   想定していない。そのため、複数assetが混在したutxoが入力された場合
   *   返却されるutxoには複数のassetが混在する可能性がある。
   * @param[in] target_value     収集額
   * @param[in,out] utxos        検索対象UTXO一覧
   * @param[in] filter           UTXO収集フィルタ情報
   * @param[in] option_params    オプション情報
   * @param[in] tx_fee_value     transaction fee information
   * @param[in] consider_fee     feeを考慮したCoinSelectionの実施フラグ (default: true)
   * @param[out] select_value    UTXO収集成功時、合計収集額
   * @param[out] utxo_fee_value  UTXO収集成功時、utxo分のfee金額
   * @param[out] searched_bnb    BnBで検索できたかどうか
   * @return UTXO一覧。空の場合はエラー終了。
   */
  std::vector<Utxo> SelectCoinsMinConf(
      const Amount& target_value, const std::vector<Utxo*>& utxos,
      const UtxoFilter& filter, const CoinSelectionOption& option_params,
      const Amount& tx_fee_value, const bool consider_fee,
      Amount* select_value, Amount* utxo_fee_value = nullptr,
      bool* searched_bnb = nullptr);

  /**
   * @brief CoinSelection(BnB)を実施する。
   * @param[in] target_value     収集額
   * @param[in] utxos            検索対象UTXO一覧
   * @param[in] cost_of_change   コストの変更範囲。
   *              target_value+本値が収集上限値となる。
   * @param[in] not_input_fees   TxIn部を除いたfee額
   * @param[out] select_value    UTXO収集成功時、合計収集額
   * @param[out] utxo_fee_value  UTXO収集成功時、utxo分のfee金額
   * @return UTXO一覧。空の場合はエラー終了。
   */
  std::vector<Utxo> SelectCoinsBnB(
      const Amount& target_value, const std::vector<Utxo*>& utxos,
      const Amount& cost_of_change, const Amount& not_input_fees,
      Amount* select_value, Amount* utxo_fee_value);

  /**
   * @brief CoinSelection(KnapsackSolver)を実施する。
   * @param[in] target_value     収集額
   * @param[in] utxos            検索対象UTXO一覧
   * @param[in] min_change       最小の差額
   * @param[out] select_value    UTXO収集成功時、合計収集額
   * @param[out] utxo_fee_value  UTXO収集成功時、utxo分のfee金額
   * @return UTXO一覧。空の場合はエラー終了。
   */
  std::vector<Utxo> KnapsackSolver(
      const Amount& target_value, const std::vector<Utxo*>& utxos,
      uint64_t min_change, Amount* select_value, Amount* utxo_fee_value);

 private:
  bool use_bnb_;                       //!< BnB 利用フラグ
  std::vector<bool> randomize_cache_;  //!< randomize cache

  /**
   * 収集額に最も近い合計額となるUTXO一覧を決定する
   * @param[in]  utxos          収集額より小さいUTXO一覧
   * @param[in]  n_total_value  utxo一覧の合計額
   * @param[in]  n_target_value 収集額
   * @param[out] vf_best        収集対象フラグ一覧
   * @param[out] n_best         収集額に最も近い合計額
   * @param[in]  iterations     繰り返し数
   */
  void ApproximateBestSubset(
      const std::vector<const Utxo*>& utxos, uint64_t n_total_value,
      uint64_t n_target_value, std::vector<char>* vf_best, uint64_t* n_best,
      int iterations);
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_UTXO_H_
