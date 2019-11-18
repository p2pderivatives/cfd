// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_key.h
 *
 * @brief cfd-apiで利用する鍵関連のクラス定義
 *
 * 構造体形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_KEY_H_
#define CFD_INCLUDE_CFD_CFDAPI_KEY_H_

#include <string>

#include "cfd/cfd_common.h"

namespace cfd {
namespace api {

using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;

/**
 * @brief 鍵情報関連のAPI群クラス
 */
class CFD_EXPORT KeyApi {
 public:
  /**
   * @brief constructor
   */
  KeyApi() {}

  /**
   * @brief 秘密鍵と公開鍵のペアを生成する.
   * @param[in] is_compressed 圧縮pubkeyかどうか
   * @param[out] pubkey pubkeyオブジェクト
   * @param[out] wif WIF文字列
   * @param[in] net_type mainnet/testnet/regtest/liquidv1
   * @return Privkeyオブジェクト
   */
  Privkey CreateKeyPair(
      bool is_compressed, Pubkey* pubkey, std::string* wif = nullptr,
      NetType net_type = NetType::kMainnet);

  /**
   * @brief 秘密鍵から公開鍵を取得する.
   * @param[in] privkey           private key (WIF or hex)
   * @param[in] is_compressed     key compressed flag (for WIF format)
   * @return pubkey
   */
  std::string GetPubkeyFromPrivkey(
      const std::string& privkey, bool is_compressed = true) const;
};
}  // namespace api
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_KEY_H_
