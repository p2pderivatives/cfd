// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_address.h
 *
 * @brief cfd-capiで利用するAddress操作のクラス定義
 *
 * C言語形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_ADDRESS_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_ADDRESS_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

#include "cfdc/cfdcapi_common.h"

/**
 * @brief network type
 */
enum CfdNetworkType {
  kCfdNetworkMainnet = 0,      //!< btc mainnet
  kCfdNetworkTestnet,          //!< btc testnet
  kCfdNetworkRegtest,          //!< btc regtest
  kCfdNetworkLiquidv1 = 10,    //!< liquidv1
  kCfdNetworkElementsRegtest,  //!< elements regtest
  kCfdNetworkCustomChain,      //!< elements custom chain
};

/**
 * @brief address type
 */
enum CfdAddressType {
  kCfdP2shAddress = 1,   //!< Legacy address (Script Hash)
  kCfdP2pkhAddress,      //!< Legacy address (PublicKey Hash)
  kCfdP2wshAddress,      //!< Native segwit address (Script Hash)
  kCfdP2wpkhAddress,     //!< Native segwit address (PublicKey Hash)
  kCfdP2shP2wshAddress,  //!< P2sh wrapped address (Script Hash)
  kCfdP2shP2wpkhAddress  //!< P2sh wrapped address (Pubkey Hash)
};

/**
 * @brief hash type
 */
enum CfdHashType {
  kCfdP2sh = 1,   //!< Script Hash
  kCfdP2pkh,      //!< PublicKey Hash
  kCfdP2wsh,      //!< Native segwit Script Hash
  kCfdP2wpkh,     //!< Native segwit PublicKey Hash
  kCfdP2shP2wsh,  //!< P2sh wrapped segwit Script Hash
  kCfdP2shP2wpkh  //!< P2sh wrapped segwit Pubkey Hash
};

/**
 * @brief sighash type
 */
enum CfdSighashType {
  kCfdSigHashAll = 0x01,    //!< SIGHASH_ALL
  kCfdSigHashNone = 0x02,   //!< SIGHASH_NONE
  kCfdSigHashSingle = 0x03  //!< SIGHASH_SINGLE
};

/**
 * @brief descriptor script type.
 */
enum CfdDescriptorScriptType {
  kCfdDescriptorScriptNull,         //!< null
  kCfdDescriptorScriptSh,           //!< script hash
  kCfdDescriptorScriptWsh,          //!< segwit script hash
  kCfdDescriptorScriptPk,           //!< pubkey
  kCfdDescriptorScriptPkh,          //!< pubkey hash
  kCfdDescriptorScriptWpkh,         //!< segwit pubkey hash
  kCfdDescriptorScriptCombo,        //!< combo
  kCfdDescriptorScriptMulti,        //!< multisig
  kCfdDescriptorScriptSortedMulti,  //!< sorted multisig
  kCfdDescriptorScriptAddr,         //!< address
  kCfdDescriptorScriptRaw           //!< raw script
};

/**
 * @brief descriptor key type.
 */
enum CfdDescriptorKeyType {
  kCfdDescriptorKeyPublic,    //!< pubkey
  kCfdDescriptorKeyBip32,     //!< bip32 extpubkey
  kCfdDescriptorKeyBip32Priv  //!< bip32 extprivkey
};

/**
 * @brief create address by pubkey or script.
 * @param[in] handle           handle pointer.
 * @param[in] hash_type        hash type. (see CfdHashType)
 * @param[in] pubkey           pubkey. (p2pkh, p2wpkh, p2sh-p2wpkh)
 * @param[in] redeem_script    redeemScript. (p2sh, p2wsh, p2sh-p2wsh)
 * @param[in] network_type     network type. (see CfdNetworkType)
 * @param[out] address         address string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] locking_script  lockingScript.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] p2sh_segwit_locking_script  lockingScript for p2sh-segwit.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateAddress(
    void* handle, int hash_type, const char* pubkey, const char* redeem_script,
    int network_type, char** address, char** locking_script,
    char** p2sh_segwit_locking_script);

/** @addtogroup CreateMultisigAddress */
/* @{ */
/**
 * @brief start of create multisig script and address.
 * @param[in] handle           handle pointer.
 * @param[in] network_type     network type. (see CfdNetworkType)
 * @param[in] hash_type        hash type. (p2sh, p2wsh, p2sh-p2wsh)
 * @param[out] multisig_handle  multisig creating handle.
 *   Call 'CfdFreeMultisigScriptHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeMultisigScript(
    void* handle, int network_type, int hash_type, void** multisig_handle);

/**
 * @brief add multisig script's pubkey.
 * @param[in] handle           handle pointer.
 * @param[in] multisig_handle  multisig creating handle.
 * @param[in] pubkey           pubkey.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddMultisigScriptData(
    void* handle, void* multisig_handle, const char* pubkey);

/**
 * @brief create multisig script and address.
 * @param[in] handle           handle pointer.
 * @param[in] multisig_handle  multisig creating handle.
 * @param[in] require_num      multisig require num.
 * @param[out] address         address string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] redeem_script   redeemScript for scriptsig.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] witness_script  segwit redeemScript for witness stack.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeMultisigScript(
    void* handle, void* multisig_handle, uint32_t require_num, char** address,
    char** redeem_script, char** witness_script);

/**
 * @brief free multisig handle.
 * @param[in] handle           handle pointer.
 * @param[in] multisig_handle  multisig creating handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeMultisigScriptHandle(void* handle, void* multisig_handle);
/* @} */

/** @addtogroup ParseOutputDescriptor */
/* @{ */
/**
 * @brief parse output descriptor.
 * @param[in] handle           handle pointer.
 * @param[in] descriptor       output descriptor.
 * @param[in] network_type     network type. (see CfdNetworkType)
 * @param[in] bip32_derivation_path  bip32 derive path.
 * @param[out] descriptor_handle     handle of getting descriptor data.
 *   Call 'CfdFreeDescriptorHandle' after you are finished using it.
 * @param[out] max_index             this descriptor maximum index.
 * @return CfdErrorCode
 */
CFDC_API int CfdParseDescriptor(
    void* handle, const char* descriptor, int network_type,
    const char* bip32_derivation_path, void** descriptor_handle,
    uint32_t* max_index);

/**
 * @brief get parsed output descriptor data.
 * @param[in] handle             handle pointer.
 * @param[in] descriptor_handle  handle of getting descriptor data.
 * @param[in] index              descriptor data index. (0 - )
 * @param[out] max_index         this descriptor maximum index.
 * @param[out] depth             this descriptor depth.
 * @param[out] script_type       script type.
 * @param[out] locking_script    locking script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] address           address string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] hash_type         hash type. (see CfdHashType)
 * @param[out] redeem_script     redeem script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] key_type          key type. (see CfdDescriptorKeyType)
 * @param[out] pubkey            pubkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] ext_pubkey        ext pubkey string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] ext_privkey       ext privkey string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] is_multisig       has multisig.
 * @param[out] max_key_num       key list count.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetDescriptorData(
    void* handle, void* descriptor_handle, uint32_t index, uint32_t* max_index,
    uint32_t* depth, int* script_type, char** locking_script, char** address,
    int* hash_type, char** redeem_script, int* key_type, char** pubkey,
    char** ext_pubkey, char** ext_privkey, bool* is_multisig,
    uint32_t* max_key_num);

/**
 * @brief get outputDescriptor's multisig keys.
 * @details If `is_multisig` argument of `CfdGetDescriptorData` is true,
 *   call this API.
 * @param[in] handle             handle pointer.
 * @param[in] descriptor_handle  handle of getting descriptor data.
 * @param[in] key_index          descriptor key index. (0 - )
 * @param[out] key_type          key type. (see CfdDescriptorKeyType)
 * @param[out] pubkey            pubkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] ext_pubkey        ext pubkey string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] ext_privkey       ext privkey string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetDescriptorMultisigKey(
    void* handle, void* descriptor_handle, uint32_t key_index, int* key_type,
    char** pubkey, char** ext_pubkey, char** ext_privkey);

/**
 * @brief free descriptor handle.
 * @param[in] handle             handle pointer.
 * @param[in] descriptor_handle  handle of getting descriptor data.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeDescriptorHandle(void* handle, void* descriptor_handle);
/* @} */

/** @addtogroup GetAddressesFromMultisig */
/* @{ */
/**
 * @brief get multisig's pubkey handle.
 * @param[in] handle           handle pointer.
 * @param[in] redeem_script    redeem script.
 * @param[in] network_type     network type. (see CfdNetworkType)
 * @param[out] hash_type       hash type. (see CfdHashType)
 * @param[out] addr_multisig_keys_handle  handle of getting multisig keys addr.
 *   Call 'CfdFreeAddressesMultisigHandle' after you are finished using it.
 * @param[out] max_key_num     multisig pubkey num.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetAddressesFromMultisig(
    void* handle, const char* redeem_script, int network_type, int hash_type,
    void** addr_multisig_keys_handle, uint32_t* max_key_num);

/**
 * @brief get multisig's pubkey address.
 * @param[in] handle           handle pointer.
 * @param[in] addr_multisig_keys_handle  handle of getting multisig keys addr.
 * @param[in] index            pubkey index.
 * @param[out] address         pubkey address.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] pubkey          pubkey hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetAddressFromMultisigKey(
    void* handle, void* addr_multisig_keys_handle, uint32_t index,
    char** address, char** pubkey);

/**
 * @brief free multisig keys address handle.
 * @param[in] handle             handle pointer.
 * @param[in] addr_multisig_keys_handle  handle of getting multisig keys addr.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeAddressesMultisigHandle(
    void* handle, void* addr_multisig_keys_handle);
/* @} */

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_ADDRESS_H_
