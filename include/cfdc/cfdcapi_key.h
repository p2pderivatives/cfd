// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_key.h
 *
 * @brief cfd-apiで利用する鍵関連のクラス定義
 *
 * 構造体形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_KEY_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_KEY_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

#include <stddef.h>
#include <stdint.h>

#include "cfdc/cfdcapi_common.h"

CFDC_API int CfdCalcurateEcSignature(void* handle,
    const uint8_t* sighash, uint32_t sighash_size,
    const uint8_t* privkey, uint32_t privkey_size,
    bool has_grind_r, 
    uint8_t** signature, uint32_t* signature_size);

CFDC_API int CfdVerifyEcSignature(void* handle,
    const uint8_t* sighash, uint32_t sighash_size,
    const char* pubkey,
    bool has_grind_r, 
    uint8_t** signature, uint32_t* signature_size);

#if 0
      - key
        * CreateKeyPair
        * GetPubkeyFromPrivkey
        * CreateExtkeyFromSeed
        * CreateExtkeyFromParent
        * CreateExtkeyFromParentPath
        * CreateExtPubkey
        * GetPrivkeyFromExtkey
        * GetPubkeyFromExtkey

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
  std::vector<std::string> GetMnemonicWordlist(
      const std::string& language) const;

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
  ByteData ConvertMnemonicToSeed(
      const std::vector<std::string>& mnemonic, const std::string& passphrase,
      bool strict_check = true, const std::string& language = "en",
      bool use_ideographic_space = false, ByteData* entropy = nullptr) const;

  /**
   * @brief entropyからmnemonicを生成する.
   * @param[in] entropy     entropy ByteData
   * @param[in] language    language to use mnemonic
   * @return mnemonic配列
   */
  std::vector<std::string> ConvertEntropyToMnemonic(
      const ByteData& entropy, const std::string& language) const;

  /**
   * @brief seedから拡張鍵を生成する.
   * @param[in] seed              seed
   * @param[in] net_type          network type
   * @param[in] output_key_type   output extkey type
   * @return extkey
   */
  std::string CreateExtkeyFromSeed(
      const ByteData& seed, NetType net_type,
      ExtKeyType output_key_type) const;

  /**
   * @brief 拡張鍵から派生拡張鍵を生成する.
   * @param[in] extkey            extended key
   * @param[in] net_type          network type
   * @param[in] output_key_type   output extkey type
   * @param[in] child_number      child number
   * @param[in] hardened          強化鍵生成フラグ
   * @return extkey
   */
  std::string CreateExtkeyFromParent(
      const std::string& extkey, NetType net_type, ExtKeyType output_key_type,
      uint32_t child_number, bool hardened) const;

  /**
   * @brief 拡張鍵から派生拡張鍵を生成する.
   * @param[in] extkey            extended key
   * @param[in] net_type          network type
   * @param[in] output_key_type   output extkey type
   * @param[in] child_number_list child number list
   * @return extkey
   */
  std::string CreateExtkeyFromParentPath(
      const std::string& extkey, NetType net_type, ExtKeyType output_key_type,
      const std::vector<uint32_t>& child_number_list) const;

  /**
   * @brief 拡張秘密鍵から同階層の拡張公開鍵を生成する.
   * @param[in] extkey            extended key
   * @param[in] net_type          network type
   * @return extkey
   */
  std::string CreateExtPubkey(
      const std::string& extkey, NetType net_type) const;

  /**
   * @brief 拡張秘密鍵から秘密鍵を取得する.
   * @param[in] extkey            extended key
   * @param[in] net_type          network type
   * @param[in] wif               WIF format flag
   * @param[in] is_compressed     key compressed flag
   * @return privkey
   */
  std::string GetPrivkeyFromExtkey(
      const std::string& extkey, NetType net_type, bool wif = true,
      bool is_compressed = true) const;

  /**
   * @brief 拡張鍵から公開鍵を取得する.
   * @param[in] extkey            extended key
   * @param[in] net_type          network type
   * @return pubkey
   */
  std::string GetPubkeyFromExtkey(
      const std::string& extkey, NetType net_type) const;

 private:
  /**
   * @brief 指定networkにおける拡張鍵のversion値を取得する.
   * @param[in] key_type          extended key type
   * @param[in] net_type          network type
   * @return version information
   */
  static uint32_t GetExtkeyVersion(ExtKeyType key_type, NetType net_type);
};
#endif

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_KEY_H_
