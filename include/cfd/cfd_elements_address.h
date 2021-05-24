// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_address.h
 *
 * @brief Definition of Address operation related class for Elements
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
 * @brief Factory class that generates address for Elements
 */
class CFD_EXPORT ElementsAddressFactory : public AddressFactory {
 public:
  /**
   * @brief Constructor.
   */
  ElementsAddressFactory();

  /**
   * @brief Constructor.
   * @param[in] type      network type
   */
  explicit ElementsAddressFactory(NetType type);

  /**
   * @brief Constructor.
   * @param[in] type      network type
   * @param[in] prefix_list   address prefix list
   */
  explicit ElementsAddressFactory(
      NetType type, const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief Constructor.
   * @param[in] type      network type
   * @param[in] wit_ver   witness version
   */
  explicit ElementsAddressFactory(NetType type, WitnessVersion wit_ver);

  /**
   * @brief Constructor.
   * @param[in] type      network type
   * @param[in] wit_ver   witness version
   * @param[in] prefix_list   address prefix list
   */
  explicit ElementsAddressFactory(
      NetType type, WitnessVersion wit_ver,
      const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief Destructor.
   */
  virtual ~ElementsAddressFactory() {
    // do nothing
  }

  /**
   * @brief Get the Confidential Address that blinded \
   *   the Unblinded Address with the confidential key.
   * @param[in] unblinded_address Address
   * @param[in] confidential_key  ConfidentialKey (ec public key)
   * @return ElementsConfidentialAddress instance
   */
  static ElementsConfidentialAddress GetConfidentialAddress(
      const Address& unblinded_address,
      const ConfidentialKey& confidential_key);

  /**
   * @brief Get ConfidentialAddress.
   * @param[in] address Address string
   * @return ElementsConfidentialAddress instance
   */
  ElementsConfidentialAddress GetConfidentialAddress(
      const std::string& address) const;

  /**
   * @brief Create peg-in address for mainchain according \
   *   to net_type from fedpegscript and pubkey
   * @param[in] address_type    address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] pubkey  public key
   * @param[in] fedpegscript elements fedpegscript
   * @return peg-in address for mainchain
   */
  Address CreatePegInAddress(
      AddressType address_type, const Pubkey& pubkey,
      const Script& fedpegscript) const;

  /**
   * @brief Create peg-in address for mainchain according to \
   *    net_type from fedpegscript and redeemScript
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] redeem_script         redeem script
   * @param[in] fedpegscript          elements fedpegscript
   * @return peg-in address for mainchain
   */
  Address CreatePegInAddressWithScript(
      AddressType address_type, const Script& redeem_script,
      const Script& fedpegscript) const;

  /**
   * @brief Create peg-in address for mainchain according \
   *   to net_type from fedpegscript and claim_script
   * @param[in] address_type    address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] claim_script    Claim script required for asset collection on sidechain
   * @param[in] fedpegscript    elements fedpegscript
   * @return peg-in address for mainchain
   */
  Address CreatePegInAddress(
      AddressType address_type, const Script& claim_script,
      const Script& fedpegscript) const;

  /**
   * @brief Create peg-in address for mainchain according \
   *    to net_type from fedpegscript with tweak added
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] tweak_fedpegscript
   *     A script that combines pubkey inside fedpegscript with tweak.
   *     (ref: cfd::core::ContractHashUtil)
   * @return peg-in address for mainchain
   */
  Address CreatePegInAddress(
      AddressType address_type, const Script& tweak_fedpegscript) const;

  /**
   * @brief Create peg-in address for mainchain according \
   *    to net_type from fedpegscript with tweak added
   * @param[in] mainchain_net_type    mainchain network type
   * @param[in] address_type          address type (p2sh, p2wsh, p2sh-p2wsh)
   * @param[in] tweak_fedpegscript
   *     A script that combines pubkey inside fedpegscript with tweak.
   *     (ref: cfd::core::ContractHashUtil)
   * @return peg-in address for mainchain
   */
  static Address CreatePegInAddress(
      NetType mainchain_net_type, AddressType address_type,
      const Script& tweak_fedpegscript);

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
