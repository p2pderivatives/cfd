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
};

/*
CFDC_API int CfdCreateAddress(
    void* handle, int hash_type, const char* pubkey, const char* redeem_script,
    int network_type, char** address, char** locking_script,
    char** p2sh_segwit_locking_script);

CFDC_API int CfdInitializeMultisigScript(
    void* handle, int network_type, int hash_type, void** multisig_handle);
CFDC_API int CfdAddMultisigScriptData(
    void* handle, void* multisig_handle, const char* pubkey,
    void** multisig_handle);
CFDC_API int CfdFinalizeMultisigScript(
    void* handle, void* multisig_handle, uint32_t nrequired, char** address,
    char** redeem_script, char** witness_script);
CFDC_API int CfdFreeMultisigScript(void* handle, void* multisig_handle);

CFDC_API int CfdParseDescriptor(
    void* handle, const char* descriptor, int network_type,
    const char* bip32_derivation_path, void** descriptor_handle);
CFDC_API int CfdGetDescriptorData(
    void* handle, void* descriptor_handle, uint32_t depth,
    char** locking_script, char** address, int* hash_type,
    char** redeem_script, int* key_type, char** ext_pubkey, char** pubkey,
    bool* is_multisig);
CFDC_API int CfdGetDescriptorMultisigKey(
    void* handle, void* descriptor_handle, uint32_t depth, int* key_type,
    char** ext_pubkey, char** pubkey);
CFDC_API int CfdFreeDescriptor(void* handle, void* descriptor_handle);
*/

// CfdGetAddressesFromMultisig
// CfdGetAddressesMultisigKey
// CfdFreeAddressesMultisig(void* handle, void* addr_multisig_handle);

#if 0
/*
class CFD_EXPORT AddressApi {
 public:
  std::vector<Address> GetAddressesFromMultisig(
      NetType net_type, AddressType address_type, const Script& redeem_script,
      std::vector<Pubkey>* pubkey_list = nullptr,
      std::vector<AddressFormatData>* prefix_list = nullptr);
};
*/
#endif

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_ADDRESS_H_
