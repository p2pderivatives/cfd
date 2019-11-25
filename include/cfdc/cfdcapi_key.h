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

/**
 * @brief calcurate ec signature.
 * @param[in] handle        cfd handle.
 * @param[in] sighash       sighash.
 * @param[in] privkey       privkey hex.
 * @param[in] wif           privkey wif.
 * @param[in] network_type  privkey wif network type.
 * @param[in] has_grind_r   grind_r flag.
 * @param[out] signature    signature.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCalcurateEcSignature(
    void* handle, const char* sighash, const char* privkey, const char* wif,
    int network_type, bool has_grind_r, char** signature);

/**
 * @brief create key pair.
 * @param[in] handle          cfd handle.
 * @param[in] is_compressed   pubkey compressed.
 * @param[in] network_type    privkey wif network type.
 * @param[out] pubkey         pubkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] privkey        privkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] wif            privkey wif.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateKeyPair(
    void* handle, bool is_compressed, int network_type, char** pubkey,
    char** privkey, char** wif);

/**
 * @brief get privkey from WIF.
 * @param[in] handle          cfd handle.
 * @param[in] wif             privkey wif.
 * @param[in] network_type    privkey wif network type.
 * @param[out] privkey        privkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPrivkeyFromWif(
    void* handle, const char* wif, int network_type, char** privkey);

/**
 * @brief get pubkey from privkey.
 * @param[in] handle          cfd handle.
 * @param[in] privkey         privkey hex. (or wif)
 * @param[in] wif             privkey wif. (or privkey)
 * @param[in] is_compressed   pubkey compressed.
 * @param[out] pubkey         pubkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPubkeyFromPrivkey(
    void* handle, const char* privkey, const char* wif, bool is_compressed,
    char** pubkey);

/**
 * @brief create extkey from seed.
 * @param[in] handle          cfd handle.
 * @param[in] seed_hex        seed data(hex).
 * @param[in] network_type    network type.
 * @param[in] key_type        extkey type.
 * @param[out] extkey         extkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateExtkeyFromSeed(
    void* handle, const char* seed_hex, int network_type, int key_type,
    char** extkey);

/**
 * @brief create extkey from parent path.
 * @param[in] handle          cfd handle.
 * @param[in] extkey          parent extkey.
 * @param[in] path            create key path.(ex: 0/0h/0'/0)
 * @param[in] network_type    network type.
 * @param[in] key_type        extkey type.
 * @param[out] child_extkey   child extkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateExtkeyFromParentPath(
    void* handle, const char* extkey, const char* path, int network_type,
    int key_type, char** child_extkey);

/**
 * @brief create extpubkey from extprivkey.
 * @param[in] handle          cfd handle.
 * @param[in] extkey          ext privkey.
 * @param[in] network_type    network type.
 * @param[out] ext_pubkey     ext pubkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateExtPubkey(
    void* handle, const char* extkey, int network_type, char** ext_pubkey);
/**
 * @brief get privkey from extprivkey.
 * @param[in] handle          cfd handle.
 * @param[in] extkey          ext privkey.
 * @param[in] network_type    network type.
 * @param[out] privkey        privkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] wif            privkey wif.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPrivkeyFromExtkey(
    void* handle, const char* extkey, int network_type, char** privkey,
    char** wif);

/**
 * @brief get pubkey from extkey.
 * @param[in] handle          cfd handle.
 * @param[in] extkey          extkey.
 * @param[in] network_type    network type.
 * @param[out] pubkey         pubkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPubkeyFromExtkey(
    void* handle, const char* extkey, int network_type, char** pubkey);

#if 0
/*
      - key
        * CreateExtkeyFromParent


class CFD_EXPORT KeyApi {
 public:

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

  std::string CreateExtkeyFromParent(
      const std::string& extkey, NetType net_type, ExtKeyType output_key_type,
      uint32_t child_number, bool hardened) const;
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
