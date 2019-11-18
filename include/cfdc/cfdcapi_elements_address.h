// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_elements_address.h
 *
 * @brief cfd-capiで利用するElementsのAddress操作API定義
 *
 * C言語形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_


CFDC_API int CfdCreateConfidentialAddress(void* handle,
    const char* address, 
    const char* confidential_key, 
    char** confidential_address);

CFDC_API int CfdParseConfidentialAddress(void* handle,
    const char* confidential_address, 
    char** address, 
    char** confidential_key);


#if 0

/**
 * @brief ElementsAddress関連のAPI群クラス
 */
class CFD_EXPORT ElementsAddressApi {
 public:

  /**
   * @brief bitcoin blockchainからのpeginに利用できるAddressを生成する
   * @param[in] net_type              network type of mainchain
   * @param[in] address_type          for future use
   *     (currently fixed with p2sh-p2wpkh)
   * @param[in] fedpegscript          fed peg script
   * @param[in] pubkey                pubkey related to mainchain address
   * @param[out] claim_script         claim script used when claiming peg-in bitcoin
   * @param[out] tweak_fedpegscript   fedpeg_script with pubkey added as tweak
   * @param[in] prefix_list           address prefix list
   * @return peg-inに利用できるAddressインスタンス
   */
  Address CreatePegInAddress(
      NetType net_type, AddressType address_type, const Script& fedpegscript,
      const Pubkey& pubkey, Script* claim_script = nullptr,
      Script* tweak_fedpegscript = nullptr,
      std::vector<AddressFormatData>* prefix_list = nullptr);
};
#endif

#endif  // CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_ADDRESS_H_
