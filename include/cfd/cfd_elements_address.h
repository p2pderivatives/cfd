// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_address.h
 *
 * @brief Elements用Address操作関連クラスの定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_ELEMENTS_ADDRESS_H_
#define CFD_INCLUDE_CFD_CFD_ELEMENTS_ADDRESS_H_
#ifndef CFD_DISABLE_ELEMENTS

#include <string>
#include "cfd/cfd_common.h"

#include "cfd/cfd_address.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::ConfidentialKey;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::ElementsNetType;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::WitnessVersion;

/**
 * @brief Elements用Addressを生成するFactoryクラス
 */
class CFD_EXPORT ElementsAddressFactory : public AddressFactory {
 public:
  /**
   * @brief コンストラクタ.
   */
  ElementsAddressFactory();

  /**
   * @brief コンストラクタ.
   * @param[in] type      network type
   */
  explicit ElementsAddressFactory(NetType type);

  /**
   * @brief コンストラクタ.
   * @param[in] type      network type
   * @param[in] wit_ver   witness version
   */
  explicit ElementsAddressFactory(NetType type, WitnessVersion wit_ver);

  /**
   * @brief デストラクタ.
   */
  virtual ~ElementsAddressFactory() {
    // do nothing
  }

  /**
   * @brief UnblindedAddressをconfidential
   * keyでブラインドしたConfidentialAddressを取得する.
   * @param[in] unblinded_address Addressインスタンス
   * @param[in] confidential_key  ConfidentialKeyインスタンス(ec public key)
   * @return BlindされたElementsConfidentialAddressインスタンス
   */
  static ElementsConfidentialAddress GetConfidentialAddress(
      const Address& unblinded_address,
      const ConfidentialKey& confidential_key);

  /**
   * @brief fedpegscriptとpubkeyから、net_typeに応じたmainchain用のpeg-in
   * addressを作成する
   * @param[in] pubkey 公開鍵
   * @param[in] fedpegscript elementsのfedpegscript
   * @return mainchain用peg-in address
   */
  Address CreatePegInAddress(
      const Pubkey& pubkey, const Script& fedpegscript) const;

  /**
   * @brief fedpegscriptとclaim_scriptから、net_typeに応じたmainchain用のpeg-in
   * addressを作成する
   * @param[in] claim_script sidechainでの資産引取りに必要なclaim script
   * @param[in] fedpegscript elementsのfedpegscript
   * @return mainchain用peg-in address
   */
  Address CreatePegInAddress(
      const Script& claim_script, const Script& fedpegscript) const;

  /**
   * @brief tweakが足されたfedpegscriptから、net_typeに応じたmainchain用のpeg-in
   * addressを作成する
   * @param[in] tweak_fedpegscript
   * fedpegscript内部のpubkeyをtweakと合成させたscript. (ref:
   * cfd::core::ContractHashUtil)
   * @return mainchain用peg-in address
   */
  Address CreatePegInAddress(const Script& tweak_fedpegscript) const;
};

}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFD_CFD_ELEMENTS_ADDRESS_H_
