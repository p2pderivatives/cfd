// Copyright 2019 CryptoGarage
/**
 * @file cfd_address.h
 *
 * @brief Address操作の関連クラス定義
 */
#ifndef CFD_INCLUDE_CFD_CFD_ADDRESS_H_
#define CFD_INCLUDE_CFD_CFD_ADDRESS_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

namespace cfd {

using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::WitnessVersion;

/**
 * @brief Addressを生成するFactoryクラス
 */
class CFD_EXPORT AddressFactory {
 public:
  /**
   * @brief コンストラクタ.
   */
  AddressFactory();

  /**
   * @brief コンストラクタ.
   * @param[in] type      network type
   */
  explicit AddressFactory(NetType type);

  /**
   * @brief コンストラクタ.
   * @param[in] type      network type
   * @param[in] wit_ver   witness version
   */
  explicit AddressFactory(NetType type, WitnessVersion wit_ver);

  /**
   * @brief コンストラクタ.
   * @param[in] type          network type
   * @param[in] wit_ver       witness version
   * @param[in] prefix_list   address prefix list
   */
  explicit AddressFactory(
      NetType type, WitnessVersion wit_ver,
      const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief コンストラクタ.
   * @param[in] type          network type
   * @param[in] prefix_list   address prefix list
   */
  explicit AddressFactory(
      NetType type, const std::vector<AddressFormatData>& prefix_list);

  /**
   * @brief デストラクタ.
   */
  virtual ~AddressFactory() {
    // do nothing
  }

  /**
   * @brief アドレスを作成する
   * @param[in] address_string address文字列
   * @return address
   */
  Address GetAddress(const std::string& address_string) const;

  /**
   * @brief LockingScriptからアドレスを作成する
   * @param[in] locking_script  locking script
   * @return address
   */
  Address GetAddressByLockingScript(const Script& locking_script) const;

  /**
   * @brief Hash情報からアドレスを作成する
   * @param[in] address_type  address type
   * @param[in] hash          hash data
   * @return address
   */
  Address GetAddressByHash(
      AddressType address_type, const ByteData& hash) const;
  /**
   * @brief Hash情報からアドレスを作成する
   * @param[in] address_type  address type
   * @param[in] hash          hash data
   * @return address
   */
  Address GetAddressByHash(
      AddressType address_type, const ByteData160& hash) const;

  /**
   * @brief Hash情報から segwit native アドレスを作成する
   * @param[in] hash  hash data
   * @return address
   */
  Address GetSegwitAddressByHash(const ByteData& hash) const;

  /**
   * @brief P2PKHアドレスを生成する.
   * @param[in] pubkey  Pubkeyインスタンス
   * @return P2PKHアドレスのAddressインスタンス
   */
  Address CreateP2pkhAddress(const Pubkey& pubkey) const;

  /**
   * @brief P2SHのアドレスを生成する.
   * @param[in] script  Scriptインスタンス
   * @return P2SHアドレスのAddressインスタンス
   */
  Address CreateP2shAddress(const Script& script) const;

  /**
   * @brief P2WPKHのアドレスを生成する.
   * @param[in] pubkey      Pubkeyインスタンス
   * @return P2WPKHのAddressインスタンス
   */
  Address CreateP2wpkhAddress(const Pubkey& pubkey) const;

  /**
   * @brief P2WSHのアドレスを生成する.
   * @param[in] script      Scriptインスタンス
   * @return P2WSHのAddressインスタンス
   */
  Address CreateP2wshAddress(const Script& script) const;

  /**
   * @brief P2WSHのMultisig(n of m)アドレスを生成する.
   * @param[in] require_num signature要求数(n)
   * @param[in] pubkeys     Pubkeyリスト(m)
   * @return P2WSH MultisigのAddressインスタンス
   */
  Address CreateP2wshMultisigAddress(
      uint32_t require_num, const std::vector<Pubkey>& pubkeys) const;

  /**
   * @brief check address's network type is valid.
   * @param[in] address address which is checked network type
   * @param[in] net_type check network type (ref: cfd::core::NetType)
   * @return true: much net_type, false: unmatch
   */
  bool CheckAddressNetType(const Address& address, NetType net_type) const;

 protected:
  NetType type_;                                //!< network type
  WitnessVersion wit_ver_;                      //!< witness version
  std::vector<AddressFormatData> prefix_list_;  //!< address prefix list
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_ADDRESS_H_
