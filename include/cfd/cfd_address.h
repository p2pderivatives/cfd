// Copyright 2019 CryptoGarage
/**
 * @file cfd_address.h
 *
 * @brief Related class definition for Address operation
 */
#ifndef CFD_INCLUDE_CFD_CFD_ADDRESS_H_
#define CFD_INCLUDE_CFD_CFD_ADDRESS_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_schnorrsig.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_taproot.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::ByteData256;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorScriptType;
using cfd::core::KeyData;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::SchnorrPubkey;
using cfd::core::Script;
using cfd::core::TaprootScriptTree;
using cfd::core::WitnessVersion;

/**
 * @brief Descriptor Script information structure
 */
struct DescriptorScriptData {
  DescriptorScriptType type;      //!< script type
  Script locking_script;          //!< locking script
  uint32_t depth;                 //!< depth
  Address address;                //!< address
  AddressType address_type;       //!< address type
  Script redeem_script;           //!< redeem script
  DescriptorKeyType key_type;     //!< key type
  std::string key;                //!< key string
  uint32_t multisig_req_sig_num;  //!< multisig num of require signatures
  TaprootScriptTree tree;         //!< taproot script tree
};

/**
 * @brief Descriptor Key information structure
 */
struct DescriptorKeyData {
  DescriptorKeyType type;  //!< key type
  std::string key;         //!< key string
};

/**
 * @brief Factory class that generates Address
 */
class CFD_EXPORT AddressFactory {
 public:
  /**
   * @brief Constructor.
   */
  AddressFactory();

  /**
   * @brief Constructor.
   * @param[in] type      network type
   */
  explicit AddressFactory(NetType type);

  /**
   * @brief Constructor.
   * @param[in] type      network type
   * @param[in] wit_ver   witness version
   */
  explicit AddressFactory(NetType type, WitnessVersion wit_ver);

  /**
   * @brief Constructor.
   * @param[in] type          network type
   * @param[in] wit_ver       witness version
   * @param[in] prefix_list   address prefix list
   */
  explicit AddressFactory(
      NetType type, WitnessVersion wit_ver,
      const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief Constructor.
   * @param[in] type          network type
   * @param[in] prefix_list   address prefix list
   */
  explicit AddressFactory(
      NetType type, const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief Destructor.
   */
  virtual ~AddressFactory() {
    // do nothing
  }

  /**
   * @brief Create an address
   * @param[in] address_string  address string
   * @return address
   */
  Address GetAddress(const std::string& address_string) const;

  /**
   * @brief Create an address from locking script
   * @param[in] locking_script  locking script
   * @return address
   */
  Address GetAddressByLockingScript(const Script& locking_script) const;

  /**
   * @brief Create an address from hash data
   * @param[in] address_type  address type
   * @param[in] hash          hash data
   * @return address
   */
  Address GetAddressByHash(
      AddressType address_type, const ByteData& hash) const;
  /**
   * @brief Create an address from hash data
   * @param[in] address_type  address type
   * @param[in] hash          hash data
   * @return address
   */
  Address GetAddressByHash(
      AddressType address_type, const ByteData160& hash) const;

  /**
   * @brief Create a segwit native address from hash data.
   * @param[in] hash  hash data
   * @return address
   */
  Address GetSegwitAddressByHash(const ByteData& hash) const;

  /**
   * @brief Create a segwit native address from hash data.
   * @param[in] hash        hash data
   * @param[in] version     witness version
   * @return address
   */
  Address GetSegwitAddressByHash(
      const ByteData& hash, WitnessVersion version) const;

  /**
   * @brief Create a P2PKH address.
   * @param[in] pubkey  Pubkey
   * @return address
   */
  Address CreateP2pkhAddress(const Pubkey& pubkey) const;

  /**
   * @brief Create a P2SH address.
   * @param[in] script  Redeem script
   * @return address
   */
  Address CreateP2shAddress(const Script& script) const;

  /**
   * @brief Create a P2WPKH address.
   * @param[in] pubkey      Pubkey
   * @return address
   */
  Address CreateP2wpkhAddress(const Pubkey& pubkey) const;

  /**
   * @brief Create a P2WSH address.
   * @param[in] script      Redeem script
   * @return address
   */
  Address CreateP2wshAddress(const Script& script) const;

  /**
   * @brief Create a P2WSH Multisig (n of m) address.
   * @param[in] require_num     signature require num(n)
   * @param[in] pubkeys         Pubkey list(m)
   * @return address
   */
  Address CreateP2wshMultisigAddress(
      uint32_t require_num, const std::vector<Pubkey>& pubkeys) const;

  /**
   * @brief Create taproot address by schnorr pubkey.
   * @param[in] pubkey      schnorr pubkey
   * @return Address by taproot
   */
  Address CreateTaprootAddress(const SchnorrPubkey& pubkey) const;

  /**
   * @brief Create taproot address by tapscript.
   * @param[in] tree                merkle tree
   * @param[in] internal_pubkey     internal schnorr pubkey
   * @return Address by taproot
   */
  Address CreateTaprootAddress(
      const TaprootScriptTree& tree,
      const SchnorrPubkey& internal_pubkey) const;

  /**
   * @brief Create taproot address by hash.
   * @param[in] hash      hash
   * @return Address by taproot
   */
  Address CreateTaprootAddress(const ByteData256& hash) const;

  /**
   * @brief check address's network type is valid.
   * @param[in] address address which is checked network type
   * @param[in] net_type check network type (ref: cfd::core::NetType)
   * @return true: much net_type, false: unmatch
   */
  bool CheckAddressNetType(const Address& address, NetType net_type) const;

  /**
   * @brief Create address.
   * @param[in] address_type    address type
   * @param[in] pubkey          public key (default: nullptr)
   * @param[in] script          script (default: nullptr)
   * @param[out] locking_script locking script
   * @param[out] redeem_script  redeem script
   * @return Address
   */
  Address CreateAddress(
      AddressType address_type, const Pubkey* pubkey, const Script* script,
      Script* locking_script = nullptr, Script* redeem_script = nullptr) const;

  /**
   * @brief Create a Pubkey Address list from Multisig Script.
   * @param[in] address_type    address type
   * @param[in] redeem_script   multisig script
   * @param[out] pubkey_list    pubkey list
   * @return pubkey address list
   */
  std::vector<Address> GetAddressesFromMultisig(
      AddressType address_type, const Script& redeem_script,
      std::vector<Pubkey>* pubkey_list = nullptr) const;

  /**
   * @brief Extract information from Output descriptor.
   * @param[in] descriptor              output descriptor
   * @param[in] bip32_derivation_path   bip32 derivation path
   * @param[out] script_list            descriptor script list
   * @param[out] multisig_key_list      descriptor multisig key list
   * @param[out] key_list               key data list
   * @return descriptor script data (top level or high security)
   */
  DescriptorScriptData ParseOutputDescriptor(
      const std::string& descriptor,
      const std::string& bip32_derivation_path = "",
      std::vector<DescriptorScriptData>* script_list = nullptr,
      std::vector<DescriptorKeyData>* multisig_key_list = nullptr,
      std::vector<KeyData>* key_list = nullptr) const;

  /**
   * @brief Get current address prefix list.
   * @return address prefix list.
   */
  std::vector<AddressFormatData> GetPrefixList() const;

 protected:
  NetType type_;                                //!< network type
  WitnessVersion wit_ver_;                      //!< witness version
  std::vector<AddressFormatData> prefix_list_;  //!< address prefix list
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_ADDRESS_H_
