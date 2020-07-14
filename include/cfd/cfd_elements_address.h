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
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
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
   * @param[in] prefix_list   address prefix list
   */
  explicit ElementsAddressFactory(
      NetType type, const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief コンストラクタ.
   * @param[in] type      network type
   * @param[in] wit_ver   witness version
   */
  explicit ElementsAddressFactory(NetType type, WitnessVersion wit_ver);

  /**
   * @brief コンストラクタ.
   * @param[in] type      network type
   * @param[in] wit_ver   witness version
   * @param[in] prefix_list   address prefix list
   */
  explicit ElementsAddressFactory(
      NetType type, WitnessVersion wit_ver,
      const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief デストラクタ.
   */
  virtual ~ElementsAddressFactory() {
    // do nothing
  }

  /**
   * @brief UnblindedAddressをconfidential
   *     keyでブラインドしたConfidentialAddressを取得する.
   * @param[in] unblinded_address Addressインスタンス
   * @param[in] confidential_key  ConfidentialKeyインスタンス(ec public key)
   * @return BlindされたElementsConfidentialAddressインスタンス
   */
  static ElementsConfidentialAddress GetConfidentialAddress(
      const Address& unblinded_address,
      const ConfidentialKey& confidential_key);

  /**
   * @brief ConfidentialAddressを取得する.
   * @param[in] address Address文字列
   * @return BlindされたElementsConfidentialAddressインスタンス
   */
  ElementsConfidentialAddress GetConfidentialAddress(
      const std::string& address) const;

  /**
   * @brief fedpegscriptとpubkeyから、net_typeに応じたmainchain用のpeg-in
   *     addressを作成する
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] pubkey 公開鍵
   * @param[in] fedpegscript elementsのfedpegscript
   * @return mainchain用peg-in address
   */
  Address CreatePegInAddress(
      AddressType address_type, const Pubkey& pubkey,
      const Script& fedpegscript) const;

  /**
   * @brief fedpegscriptとredeemScriptから、net_typeに応じた
   *     mainchain用のpeg-in addressを作成する
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] redeem_script         redeem script
   * @param[in] fedpegscript          elements fedpegscript
   * @return mainchain用peg-in address
   */
  Address CreatePegInAddressWithScript(
      AddressType address_type, const Script& redeem_script,
      const Script& fedpegscript) const;

  /**
   * @brief fedpegscriptとclaim_scriptから、net_typeに応じたmainchain用のpeg-in
   *     addressを作成する
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] claim_script sidechainでの資産引取りに必要なclaim script
   * @param[in] fedpegscript elementsのfedpegscript
   * @return mainchain用peg-in address
   */
  Address CreatePegInAddress(
      AddressType address_type, const Script& claim_script,
      const Script& fedpegscript) const;

  /**
   * @brief tweakが足されたfedpegscriptから、net_typeに応じたmainchain用のpeg-in
   *     addressを作成する
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] tweak_fedpegscript
   *     fedpegscript内部のpubkeyをtweakと合成させたscript.
   *     (ref: cfd::core::ContractHashUtil)
   * @return mainchain用peg-in address
   */
  Address CreatePegInAddress(
      AddressType address_type, const Script& tweak_fedpegscript) const;

  /**
   * @brief check confidential address's network type is valid.
   * @param[in] conf_addr confidential address which is checked network type
   * @param[in] net_type check network type (ref: cfd::core::NetType)
   * @return true: much net_type, false: unmatch
   */
  bool CheckConfidentialAddressNetType(
      const ElementsConfidentialAddress& conf_addr, NetType net_type) const;
};

}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFD_CFD_ELEMENTS_ADDRESS_H_
