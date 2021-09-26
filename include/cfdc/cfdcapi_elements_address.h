/* Copyright 2019 CryptoGarage */
/**
 * @file cfdcapi_elements_address.h
 *
 * @brief API definition file of Elements Address function for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_
#ifndef CFD_DISABLE_ELEMENTS

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"

/**
 * @brief create confidential address.
 * @param[in] handle                 cfd handle
 * @param[in] address                address string
 * @param[in] confidential_key       confidential key(hex)
 * @param[out] confidential_address  confidential address
 * @return error code
 */
CFDC_API int CfdCreateConfidentialAddress(
    void* handle, const char* address, const char* confidential_key,
    char** confidential_address);

/**
 * @brief parse confidential address.
 * @param[in] handle                cfd handle
 * @param[in] confidential_address  confidential address
 * @param[out] address              address string
 * @param[out] confidential_key     confidential key(hex)
 * @param[out] network_type         network type
 * @return error code
 */
CFDC_API int CfdParseConfidentialAddress(
    void* handle, const char* confidential_address, char** address,
    char** confidential_key, int* network_type);

/**
 * @brief Get pegin address.
 * @param[in] handle                    cfd handle.
 * @param[in] mainchain_network_type    cfd handle.
 * @param[in] fedpeg_script             cfd handle.
 * @param[in] hash_type                 pegin hash type.
 *    (current is p2sh-segwit only. after dynafed, can use bech32.)
 * @param[in] pubkey                    pegin utxo pubkey.
 *   (if set redeem_script, pubkey is unused.)
 * @param[in] redeem_script             pegin utxo redeem script.
 * @param[out] pegin_address            pegin mainchain address.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] claim_script             claim script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] tweaked_fedpeg_script    tweaked redpeg script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return error code
 */
CFDC_API int CfdGetPeginAddress(
    void* handle, int mainchain_network_type, const char* fedpeg_script,
    int hash_type, const char* pubkey, const char* redeem_script,
    char** pegin_address, char** claim_script, char** tweaked_fedpeg_script);

/**
 * @brief get pegout address.
 * @param[in] handle                cfd handle.
 * @param[in] mainchain_network     mainchain network.
 * @param[in] elements_network      elements network.
 * @param[in] descriptor            descriptor.
 * @param[in] bip32_counter         bip32 counter.
 * @param[in] address_type          address type. (use if descriptor is xpub.)
 * @param[out] mainchain_address    mainchain address.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] base_descriptor      pegout base descriptor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPegoutAddress(
    void* handle, int mainchain_network, int elements_network,
    const char* descriptor, uint32_t bip32_counter, int address_type,
    char** mainchain_address, char** base_descriptor);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_DISABLE_ELEMENTS */
#endif /* CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_ NOLINT */
