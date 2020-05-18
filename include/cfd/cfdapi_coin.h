// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_coin.h
 *
 * @brief cfd-apiで利用するCoin関連のクラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_COIN_H_
#define CFD_INCLUDE_CFD_CFDAPI_COIN_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfd_utxo.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {
namespace api {

using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlockHash;
using cfd::core::Script;
using cfd::core::Txid;
#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::ConfidentialAssetId;
#endif  // CFD_DISABLE_ELEMENTS

/**
 * @brief Coin関連のAPIクラス
 * @deprecated replace to UtxoUtil.
 */
class CFD_EXPORT CoinApi {
 public:
  /**
   * @brief constructor
   */
  CoinApi() {}

  /**
   * @brief convert to simple utxo list.
   * @param[in] utxos   utxo data list
   * @return UTXO list
   */
  std::vector<Utxo> ConvertToUtxo(const std::vector<UtxoData>& utxos) const;
  /**
   * @brief convert to simple utxo.
   * @param[in] utxo_data   utxo data
   * @param[out] utxo       utxo
   */
  void ConvertToUtxo(const UtxoData& utxo_data, Utxo* utxo) const;
};

}  // namespace api
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_COIN_H_
