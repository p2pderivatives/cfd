// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_elements_address.h
 *
 * @brief cfd-capiで利用するElementsのAddress操作API定義
 *
 * C言語形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

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

#if 0
/*
class CFD_EXPORT ElementsAddressApi {
 public:
  Address CreatePegInAddress(
      NetType net_type, AddressType address_type, const Script& fedpegscript,
      const Pubkey& pubkey, Script* claim_script = nullptr,
      Script* tweak_fedpegscript = nullptr,
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

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_
