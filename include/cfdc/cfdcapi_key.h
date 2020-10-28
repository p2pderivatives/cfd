/* Copyright 2019 CryptoGarage */
/**
 * @file cfdcapi_key.h
 *
 * @brief API definition file of Key function for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_KEY_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_KEY_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include <stddef.h>
#include <stdint.h>

#include "cfdc/cfdcapi_common.h"

/**
 * @brief extkey type.
 */
enum CfdExtKeyType {
  /** extended privkey */
  kCfdExtPrivkey = 0,
  /** extended pubkey */
  kCfdExtPubkey
};

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
CFDC_API int CfdCalculateEcSignature(
    void* handle, const char* sighash, const char* privkey, const char* wif,
    int network_type, bool has_grind_r, char** signature);

/**
 * @brief Verify if a signature with respect to a public key and a message.
 * @param[in] handle          cfd handle.
 * @param[in] sighash         The signature hash.
 * @param[in] pubkey          The public key to verify the signature against.
 * @param[in] signature       The 64byte signature to verify.
 * @return CfdErrorCode
 */
CFDC_API int CfdVerifyEcSignature(
    void* handle, const char* sighash, const char* pubkey,
    const char* signature);

/**
 * @brief Create an adaptor signature over the given message using the given
 * private key. Returns an AdaptorPair of the adaptor signature and its proof.
 *
 * @param[in] handle cfd handle.
 * @param[in] msg the message to create the signature for.
 * @param[in] sk the secret key to create the signature with.
 * @param[in] adaptor the adaptor to adapt the signature with.
 * @param[out] adaptor_signature the adaptor signature
 * @param[out] adaptor_proof the adaptor proof
 * @return CfdErrorCode
 */
CFDC_API int CfdSignEcdsaAdaptor(
    void* handle, const char* msg, const char* sk, const char* adaptor,
    char** adaptor_signature, char** adaptor_proof);

/**
 * @brief "Decrypt" an adaptor signature using the provided secret, returning
 * an ecdsa signature in compact format.
 *
 * @param[in] handle cfd handle.
 * @param[in] adaptor_signature the adaptor signature
 * @param[in] adaptor_secret the secret
 * @param[out] signature the ECDSA signature
 * @return CfdErrorCode
 */
CFDC_API int CfdAdaptEcdsaAdaptor(
    void* handle, const char* adaptor_signature, const char* adaptor_secret,
    char** signature);

/**
 * @brief Extract an adaptor secret from an ECDSA signature for a given
 * adaptor signature.
 *
 * @param[in] handle cfd handle.
 * @param[in] adaptor_signature the adaptor signature
 * @param[in] signature the ECDSA signature
 * @param[in] adaptor the adaptor for the signature
 * @param[out] adaptor_secret the secret
 * @return CfdErrorCode
 */
CFDC_API int CfdExtractEcdsaAdaptorSecret(
    void* handle, const char* adaptor_signature, const char* signature,
    const char* adaptor, char** adaptor_secret);

/**
 * @brief Verify that an adaptor proof is valid with respect to a given
 * adaptor signature, adaptor, message and public key.
 *
 * @param[in] handle cfd handle.
 * @param[in] adaptor_signature the adaptor signature
 * @param[in] proof the adaptor proof
 * @param[in] adaptor the adaptor for the signature
 * @param[in] msg the message to create the signature for.
 * @param[in] pubkey the public key
 * @return CfdErrorCode
 */
CFDC_API int CfdVerifyEcdsaAdaptor(
    void* handle, const char* adaptor_signature, const char* proof,
    const char* adaptor, const char* msg, const char* pubkey);

/**
 * @brief Get a schnorr public key from a private key.
 *
 * @param[in] handle cfd handle.
 * @param[in] privkey the private key.
 * @param[out] schnorr_pubkey the schnorr public key.
 * @param[out] parity the y-parity flag.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetSchnorrPubkeyFromPrivkey(
    void* handle, const char* privkey, char** schnorr_pubkey, bool* parity);

/**
 * @brief Get a schnorr public key from a public key.
 *
 * @param[in] handle cfd handle.
 * @param[in] pubkey the public key.
 * @param[out] schnorr_pubkey the schnorr public key.
 * @param[out] parity the y-parity flag.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetSchnorrPubkeyFromPubkey(
    void* handle, const char* pubkey, char** schnorr_pubkey, bool* parity);

/**
 * @brief Add tweak to pubkey.
 * @param[in] handle          cfd handle.
 * @param[in] pubkey          pubkey hex.
 * @param[in] tweak           tweak data hex. (32-byte (64-charactors))
 * @param[out] output         pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] parity the y-parity flag.
 * @return CfdErrorCode
 */
CFDC_API int CfdSchnorrPubkeyTweakAdd(
    void* handle, const char* pubkey, const char* tweak, char** output,
    bool* parity);

/**
 * @brief Add tweak to key pair.
 * @param[in] handle          cfd handle.
 * @param[in] privkey         privkey hex.
 * @param[in] tweak           tweak data hex. (32-byte (64-charactors))
 * @param[out] tweaked_pubkey   tweaked schnorr pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] tweaked_parity the y-parity flag.
 * @param[out] tweaked_privkey  tweaked privkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdSchnorrKeyPairTweakAdd(
    void* handle, const char* privkey, const char* tweak,
    char** tweaked_pubkey, bool* tweaked_parity, char** tweaked_privkey);

/**
 * @brief Check tweak-add from base pubkey.
 * @param[in] handle          cfd handle.
 * @param[in] tweaked_pubkey  tweaked pubkey hex.
 * @param[in] tweaked_parity  tweaked y-parity flag.
 * @param[in] base_pubkey     base pubkey hex.
 * @param[in] tweak           tweak data hex. (32-byte (64-charactors))
 * @retval kCfdSuccess                check success.
 * @retval kCfdSignVerificationError  check fail.
 * @retval kCfdIllegalArgumentError   illegal argument.
 */
CFDC_API int CfdCheckTweakAddFromSchnorrPubkey(
    void* handle, const char* tweaked_pubkey, bool tweaked_parity,
    const char* base_pubkey, const char* tweak);

/**
 * @brief Create a schnorr signature over the given message using the given
 * private key and auxiliary random data.
 *
 * @param[in] handle cfd handle.
 * @param[in] msg the message to create the signature for.
 * @param[in] sk the secret key to create the signature with.
 * @param[in] aux_rand the auxiliary random data used to create the nonce.
 * @param[out] signature the signature.
 * @return CfdErrorCode
 */
CFDC_API int CfdSignSchnorr(
    void* handle, const char* msg, const char* sk, const char* aux_rand,
    char** signature);

/**
 * @brief Create a schnorr signature over the given message using the given
 * private key.
 *
 * @param[in] handle cfd handle.
 * @param[in] msg the message to create the signature for.
 * @param[in] sk the secret key to create the signature with.
 * @param[in] nonce the nonce to use to create the signature.
 * @param[out] signature the signature.
 * @return CfdErrorCode
 */
CFDC_API int CfdSignSchnorrWithNonce(
    void* handle, const char* msg, const char* sk, const char* nonce,
    char** signature);

/**
 * @brief Compute a signature point for a Schnorr signature.
 *
 * @param[in] handle cfd handle.
 * @param[in] msg the message that will be signed.
 * @param[in] nonce the public component of the nonce that will be used.
 * @param[in] pubkey the public key for which the signature will be valid.
 * @param[out] sigpoint the signature point.
 * @return CfdErrorCode
 */
CFDC_API int CfdComputeSchnorrSigPoint(
    void* handle, const char* msg, const char* nonce, const char* pubkey,
    char** sigpoint);

/**
 * @brief Verify a Schnorr signature.
 *
 * @param[in] handle cfd handle.
 * @param[in] signature the signature to verify.
 * @param[in] msg the message to verify the signature against.
 * @param[in] pubkey the public key to verify the signature against.
 * @return CfdErrorCode
 */
CFDC_API int CfdVerifySchnorr(
    void* handle, const char* signature, const char* msg, const char* pubkey);

/**
 * @brief Split a Schnorr signature.
 *
 * @param[in] handle cfd handle.
 * @param[in] signature the signature.
 * @param[out] nonce the nonce part of the signature.
 * @param[out] key the second part of the signature as a Privkey instance.
 * @return CfdErrorCode
 */
CFDC_API int CfdSplitSchnorrSignature(
    void* handle, const char* signature, char** nonce, char** key);

/**
 * @brief encode ec signature by der encoding.
 * @param[in] handle                  cfd handle.
 * @param[in] signature               compact signature string.
 * @param[in] sighash_type            sign sighash type.
 * @param[in] sighash_anyone_can_pay  flag of signing only the current input.
 * @param[out] der_signature  signature encoded by der encoding.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdEncodeSignatureByDer(
    void* handle, const char* signature, int sighash_type,
    bool sighash_anyone_can_pay, char** der_signature);

/**
 * @brief decode ec signature from der encoding.
 * @param[in] handle                   cfd handle.
 * @param[in] der_signature            signature encoded by der encoding.
 * @param[out] signature               compact signature string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] sighash_type            sign sighash type.
 * @param[out] sighash_anyone_can_pay  flag of signing only the current input.
 * @return CfdErrorCode
 */
CFDC_API int CfdDecodeSignatureFromDer(
    void* handle, const char* der_signature, char** signature,
    int* sighash_type, bool* sighash_anyone_can_pay);

/**
 * @brief convert ec signature to low-s form
 * @param[in] handle                  cfd handle.
 * @param[in] signature               to convert ec signature
 * @param[out] normalized_signature   normalized signature
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdNormalizeSignature(
    void* handle, const char* signature, char** normalized_signature);

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
 * @brief get privkey WIF from hex.
 * @param[in] handle         cfd handle.
 * @param[in] privkey        privkey hex.
 * @param[in] network_type   privkey wif network type.
 * @param[in] is_compressed  compress flag.
 * @param[out] wif           privkey wif.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPrivkeyWif(
    void* handle, const char* privkey, int network_type, bool is_compressed,
    char** wif);

/**
 * @brief parse privkey WIF information.
 * @param[in] handle          cfd handle.
 * @param[in] wif             privkey wif.
 * @param[out] privkey        privkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] network_type   network type.
 * @param[out] is_compressed  compress flag.
 * @return CfdErrorCode
 */
CFDC_API int CfdParsePrivkeyWif(
    void* handle, const char* wif, char** privkey, int* network_type,
    bool* is_compressed);

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
 * @brief Compress pubkey.
 * @param[in] handle          cfd handle.
 * @param[in] pubkey          pubkey hex.
 * @param[out] output         pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCompressPubkey(
    void* handle, const char* pubkey, char** output);

/**
 * @brief Uncompress pubkey. (prefix is 0x04 only.)
 * @param[in] handle          cfd handle.
 * @param[in] pubkey          pubkey hex.
 * @param[out] output         pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdUncompressPubkey(
    void* handle, const char* pubkey, char** output);

/**
 * @brief Initialize combine pubkey handle.
 * @param[in] handle            cfd handle.
 * @param[out] combine_handle   combine handle.
 *   Call 'CfdFreeCombinePubkeyHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeCombinePubkey(void* handle, void** combine_handle);

/**
 * @brief Add combine target pubkey.
 * @param[in] handle            cfd handle.
 * @param[out] combine_handle   combine handle.
 * @param[in] pubkey            pubkey hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddCombinePubkey(
    void* handle, void* combine_handle, const char* pubkey);

/**
 * @brief Combine pubkey.
 * @param[in] handle            cfd handle.
 * @param[out] combine_handle   combine handle.
 * @param[out] output           pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeCombinePubkey(
    void* handle, void* combine_handle, char** output);

/**
 * @brief Free combine pubkey handle.
 * @param[in] handle            cfd handle.
 * @param[out] combine_handle   combine handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeCombinePubkeyHandle(void* handle, void* combine_handle);

/**
 * @brief Add tweak to pubkey.
 * @param[in] handle          cfd handle.
 * @param[in] pubkey          pubkey hex.
 * @param[in] tweak           tweak data hex. (32-byte (64-charactors))
 * @param[out] output         pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdPubkeyTweakAdd(
    void* handle, const char* pubkey, const char* tweak, char** output);

/**
 * @brief Multiplication tweak to pubkey.
 * @param[in] handle          cfd handle.
 * @param[in] pubkey          pubkey hex.
 * @param[in] tweak           tweak data hex. (32-byte (64-charactors))
 * @param[out] output         pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdPubkeyTweakMul(
    void* handle, const char* pubkey, const char* tweak, char** output);

/**
 * @brief Negate pubkey.
 * @param[in] handle          cfd handle.
 * @param[in] pubkey          pubkey hex.
 * @param[out] output         pubkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdNegatePubkey(void* handle, const char* pubkey, char** output);

/**
 * @brief Add tweak to privkey.
 * @param[in] handle          cfd handle.
 * @param[in] privkey         privkey hex.
 * @param[in] tweak           tweak data hex. (32-byte (64-charactors))
 * @param[out] output         privkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdPrivkeyTweakAdd(
    void* handle, const char* privkey, const char* tweak, char** output);

/**
 * @brief Multiplication tweak to privkey.
 * @param[in] handle          cfd handle.
 * @param[in] privkey         privkey hex.
 * @param[in] tweak           tweak data hex. (32-byte (64-charactors))
 * @param[out] output         privkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdPrivkeyTweakMul(
    void* handle, const char* privkey, const char* tweak, char** output);

/**
 * @brief Negate privkey.
 * @param[in] handle          cfd handle.
 * @param[in] privkey         privkey hex.
 * @param[out] output         privkey output hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdNegatePrivkey(
    void* handle, const char* privkey, char** output);

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
 * @brief create extkey from direct info.
 * @param[in] handle          cfd handle.
 * @param[in] network_type    network type.
 * @param[in] key_type        extkey type.
 * @param[in] parent_key      parent key. (If there is no fingerprint)
 * @param[in] fingerprint     fingerprint.
 * @param[in] key             public or private key.
 * @param[in] chain_code      chain code.
 * @param[in] depth           depth.
 * @param[in] child_number    child key number.
 * @param[out] extkey         extkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateExtkey(
    void* handle, int network_type, int key_type, const char* parent_key,
    const char* fingerprint, const char* key, const char* chain_code,
    unsigned char depth, uint32_t child_number, char** extkey);

/**
 * @brief derive extkey from number.
 * @param[in] handle          cfd handle.
 * @param[in] extkey          parent extkey.
 * @param[in] child_number    child key number.
 * @param[in] hardened        hardened flag.
 * @param[in] network_type    network type.
 * @param[in] key_type        extkey type.
 * @param[out] child_extkey   child extkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateExtkeyFromParent(
    void* handle, const char* extkey, uint32_t child_number, bool hardened,
    int network_type, int key_type, char** child_extkey);

/**
 * @brief derive extkey from path.
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

/**
 * @brief get parent key path data.
 * @param[in] handle             handle pointer.
 * @param[in] parent_extkey      parent ext key string.
 * @param[in] path               child path.
 * @param[in] child_key_type     child key type. (see CfdDescriptorKeyType)
 * @param[out] key_path_data     key path data.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] child_key         child ext key string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetParentExtkeyPathData(
    void* handle, const char* parent_extkey, const char* path,
    int child_key_type, char** key_path_data, char** child_key);

/**
 * @brief get extkey information.
 * @param[in] handle             handle pointer.
 * @param[in] extkey             ext key string.
 * @param[out] version           version.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] fingerprint       parent fingerprint.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] chain_code        chain code.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] depth             depth.
 * @param[out] child_number      child number.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetExtkeyInformation(
    void* handle, const char* extkey, char** version, char** fingerprint,
    char** chain_code, uint32_t* depth, uint32_t* child_number);

/**
 * @brief initialize getting mnemonic word list.
 * @param[in] handle            cfd handle.
 * @param[in] language          language. (default: en)
 * @param[out] mnemonic_handle  get mnemonic word handle.
 *   Call 'CfdFreeMnemonicWordList' after you are finished using it.
 * @param[out] max_index        mnemonic word list num.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeMnemonicWordList(
    void* handle, const char* language, void** mnemonic_handle,
    uint32_t* max_index);

/**
 * @brief get mnemonic word.
 * @param[in] handle            cfd handle.
 * @param[in] mnemonic_handle   get mnemonic word handle.
 * @param[in] index             index.
 * @param[out] mnemonic_word    mnemonic word.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetMnemonicWord(
    void* handle, void* mnemonic_handle, uint32_t index, char** mnemonic_word);

/**
 * @brief free getting mnemonic word list.
 * @param[in] handle            cfd handle.
 * @param[in] mnemonic_handle   get mnemonic word handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeMnemonicWordList(void* handle, void* mnemonic_handle);

/**
 * @brief convert mnemonic to seed.
 * @param[in] handle                 cfd handle.
 * @param[in] mnemonic               mnemonic.
 * @param[in] passphrase             passphrase.
 * @param[in] strict_check           strict check flag.
 * @param[in] language               language. (default: en)
 * @param[in] use_ideographic_space  use ideographic space.
 * @param[out] seed                  seed.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] entropy               mnemonic word entropy.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdConvertMnemonicToSeed(
    void* handle, const char* mnemonic, const char* passphrase,
    bool strict_check, const char* language, bool use_ideographic_space,
    char** seed, char** entropy);

/**
 * @brief convert entropy to mnemonic.
 * @param[in] handle          cfd handle.
 * @param[in] entropy         mnemonic word entropy.
 * @param[in] language        language. (default: en)
 * @param[out] mnemonic       mnemonic.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdConvertEntropyToMnemonic(
    void* handle, const char* entropy, const char* language, char** mnemonic);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_KEY_H_ NOLINT */
