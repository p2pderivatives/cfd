// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_key.h
 *
 * @brief cfd-capiで利用する鍵関連のクラス定義
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

/*
CFDC_API int CfdCalcurateEcSignature(
    void* handle, const char* sighash, const char* privkey, bool has_grind_r,
    char** signature);
*/

#if 0
/*
      - key
        * CreateKeyPair
        * GetPubkeyFromPrivkey
        * CreateExtkeyFromSeed
        * CreateExtkeyFromParent
        * CreateExtkeyFromParentPath
        * CreateExtPubkey
        * GetPrivkeyFromExtkey
        * GetPubkeyFromExtkey


class CFD_EXPORT KeyApi {
 public:
  Privkey CreateKeyPair(
      bool is_compressed, Pubkey* pubkey, std::string* wif = nullptr,
      NetType net_type = NetType::kMainnet);

  std::string GetPubkeyFromPrivkey(
      const std::string& privkey, bool is_compressed = true) const;
};
class CFD_EXPORT HDWalletApi {
 public:
  std::vector<std::string> GetMnemonicWordlist(
      const std::string& language) const;

  ByteData ConvertMnemonicToSeed(
      const std::vector<std::string>& mnemonic, const std::string& passphrase,
      bool strict_check = true, const std::string& language = "en",
      bool use_ideographic_space = false, ByteData* entropy = nullptr) const;

  std::vector<std::string> ConvertEntropyToMnemonic(
      const ByteData& entropy, const std::string& language) const;

  std::string CreateExtkeyFromSeed(
      const ByteData& seed, NetType net_type,
      ExtKeyType output_key_type) const;

  std::string CreateExtkeyFromParent(
      const std::string& extkey, NetType net_type, ExtKeyType output_key_type,
      uint32_t child_number, bool hardened) const;

  std::string CreateExtkeyFromParentPath(
      const std::string& extkey, NetType net_type, ExtKeyType output_key_type,
      const std::vector<uint32_t>& child_number_list) const;

  std::string CreateExtPubkey(
      const std::string& extkey, NetType net_type) const;

  std::string GetPrivkeyFromExtkey(
      const std::string& extkey, NetType net_type, bool wif = true,
      bool is_compressed = true) const;

  std::string GetPubkeyFromExtkey(
      const std::string& extkey, NetType net_type) const;
};
*/
#endif

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_KEY_H_
