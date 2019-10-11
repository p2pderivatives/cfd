// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_hdwallet.h
 *
 * @brief cfd-apiで利用するHDWallet APIのクラス定義
 *
 * 構造体形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_HDWALLET_H_
#define CFD_INCLUDE_CFD_CFDAPI_HDWALLET_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_struct.h"
#include "cfdcore/cfdcore_bytedata.h"

/**
 * @brief cfdapi名前空間
 */
namespace cfd {
namespace js {
namespace api {

/**
 * @brief HDWallet構造体関数群クラス
 */
class CFD_EXPORT HDWalletStructApi {
 public:
  /**
   * @brief JSONパラメータの情報を元に、BIP39 で利用できる wordlist を取得する.
   * @param[in] request wordlistの言語を含むリクエスト構造体
   * @return wordlist一覧を含むレスポンス構造体
   */
  static GetMnemonicWordlistResponseStruct GetMnemonicWordlist(
      const GetMnemonicWordlistRequestStruct& request);

  /**
   * @brief JSONパラメータの情報を元に、mnemonic から hdwallet seedを生成する.
   * @param[in] request mnemonicを含むリクエスト構造体
   * @return seedを含むレスポンス構造体
   */
  static ConvertMnemonicToSeedResponseStruct ConvertMnemonicToSeed(
      const ConvertMnemonicToSeedRequestStruct& request);

  /**
   * @brief JSONパラメータの情報を元に、entropyからmnemonicを生成する.
   * @param[in] request Entropyを含むリクエスト構造体
   * @return mnemonicを含むレスポンス構造体
   */
  static ConvertEntropyToMnemonicResponseStruct ConvertEntropyToMnemonic(
      const ConvertEntropyToMnemonicRequestStruct& request);

 private:
  HDWalletStructApi();
};

}  // namespace api
}  // namespace js
}  // namespace cfd

namespace cfd {
namespace api {

using cfd::core::ByteData;
/**
 * @brief HDWallet関数群クラス
 */
class CFD_EXPORT HDWalletApi {
 public:
  /**
   * @brief BIP39 で利用できる wordlist を取得するAPI.
   * @param[in] language  取得するwordlistの言語
   * @return wordlist
   */
  static std::vector<std::string> GetMnemonicWordlist(
      const std::string& language);

  /**
   * @brief mnemonic から hdwallet Seedを生成する.
   * @param[in] mnemonic                mnemonicを含むリクエスト構造体
   * @param[in] passphrase              seedのsoltとなるpassphrase文字列
   * @param[in] strict_check            mnemonic の厳密チェックフラグ(default: true)
   * @param[in] language                mnemonic の言語
   * @param[in] use_ideographic_space   mnemonic のセパレータとして
   *     全角スペースを使うかのフラグ(default: false)
   * @param[out] entropy                mnemonicのentropy ByteData
   *     languageが指定されている場合のみ返却
   * @return seedのByteData
   */
  static ByteData ConvertMnemonicToSeed(
      const std::vector<std::string>& mnemonic, const std::string& passphrase,
      bool strict_check = true, const std::string& language = "en",
      bool use_ideographic_space = false, ByteData* entropy = nullptr);

  /**
   * @brief entropyからmnemonicを生成する.
   * @param[in] entropy     entropy ByteData
   * @param[in] language    language to use mnemonic
   * @return mnemonic配列
   */
  static std::vector<std::string> ConvertEntropyToMnemonic(
      const ByteData& entropy, const std::string& language);

 private:
  HDWalletApi();
};
}  // namespace api
}  // namespace cfd
#endif  // CFD_INCLUDE_CFD_CFDAPI_HDWALLET_H_
