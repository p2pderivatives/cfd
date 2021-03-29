// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction_common.h
 *
 * @brief Related class definition common to Transaction operations
 */
#ifndef CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_
#define CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptOperator;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::Txid;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::BlindFactor;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsConfidentialAddress;
#endif  // CFD_DISABLE_ELEMENTS

/**
 * @typedef SignDataType
 * @brief Sign data type
 */
enum SignDataType {
  kSign = 0,
  kBinary,
  kPubkey,
  kRedeemScript,
  kOpCode,
};

/**
 * @brief UTXO structure
 */
struct CFD_EXPORT UtxoData {
 public:
  uint64_t block_height = 0;  //!< blick height
  BlockHash block_hash;       //!< block hash
  Txid txid;                  //!< txid
  uint32_t vout = 0;          //!< vout
  Script locking_script;      //!< locking script
  Script redeem_script;       //!< script
  Address address;            //!< address
  std::string descriptor;     //!< output descriptor
  Amount amount;              //!< amount
  //! address type
  AddressType address_type = AddressType::kP2shAddress;
  //! binary data option
  void* binary_data = nullptr;
#ifndef CFD_DISABLE_ELEMENTS
  ConfidentialAssetId asset;  //!< asset
  // elements
  ElementsConfidentialAddress confidential_address;  //!< Confidential address
  BlindFactor asset_blind_factor;                    //!< asset blind factor
  BlindFactor amount_blind_factor;                   //!< blind vactor
  ConfidentialValue value_commitment;                //!< value commitment
#endif                                               // CFD_DISABLE_ELEMENTS
  Script scriptsig_template;                         //!< scriptsig template

 public:
  /**
   * @brief constructor.
   */
  UtxoData();
#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief constructor.
   * @param[in] block_height    block_height
   * @param[in] block_hash    block_hash
   * @param[in] txid    txid
   * @param[in] vout    vout
   * @param[in] locking_script    locking_script
   * @param[in] redeem_script    redeem_script
   * @param[in] address    address
   * @param[in] descriptor    descriptor
   * @param[in] amount    amount
   * @param[in] address_type    address_type
   * @param[in] binary_data    binary_data
   * @param[in] asset    asset
   * @param[in] confidential_address    confidential_address
   * @param[in] asset_blind_factor    asset_blind_factor
   * @param[in] amount_blind_factor    amount_blind_factor
   * @param[in] value_commitment    value_commitment
   * @param[in] scriptsig_template    scriptsig_template
   */
  explicit UtxoData(
      uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
      uint32_t vout, const Script& locking_script, const Script& redeem_script,
      const Address& address, const std::string& descriptor,
      const Amount& amount, AddressType address_type, void* binary_data,
      const ConfidentialAssetId& asset,
      const ElementsConfidentialAddress& confidential_address,
      const BlindFactor& asset_blind_factor,
      const BlindFactor& amount_blind_factor,
      const ConfidentialValue& value_commitment,
      const Script& scriptsig_template);
#else
  /**
   * @brief constructor.
   * @param[in] block_height    block_height
   * @param[in] block_hash    block_hash
   * @param[in] txid    txid
   * @param[in] vout    vout
   * @param[in] locking_script    locking_script
   * @param[in] redeem_script    redeem_script
   * @param[in] address    address
   * @param[in] descriptor    descriptor
   * @param[in] amount    amount
   * @param[in] address_type    address_type
   * @param[in] binary_data    binary_data
   * @param[in] scriptsig_template    scriptsig_template
   */
  explicit UtxoData(
      uint64_t block_height, const BlockHash& block_hash, const Txid& txid,
      uint32_t vout, const Script& locking_script, const Script& redeem_script,
      const Address& address, const std::string& descriptor,
      const Amount& amount, AddressType address_type, void* binary_data,
      const Script& scriptsig_template);
#endif  // CFD_DISABLE_ELEMENTS
  /**
   * @brief copy constructor.
   * @param[in] object    object
   */
  UtxoData(const UtxoData& object);
  /**
   * @brief copy constructor.
   * @param[in] object    object
   * @return object
   */
  UtxoData& operator=(const UtxoData& object) &;
};

/**
 * @brief Utxo related API classes
 */
class CFD_EXPORT UtxoUtil {
 public:
  /**
   * @brief convert to simple utxo list.
   * @param[in] utxos   utxo data list
   * @return UTXO list
   */
  static std::vector<Utxo> ConvertToUtxo(const std::vector<UtxoData>& utxos);
  /**
   * @brief convert to simple utxo.
   * @param[in] utxo_data   utxo data
   * @param[out] utxo       utxo
   * @param[out] dest       analyzed utxo data
   */
  static void ConvertToUtxo(
      const UtxoData& utxo_data, Utxo* utxo, UtxoData* dest = nullptr);

 private:
  /**
   * @brief constructor (disable)
   */
  UtxoUtil();
};

/**
 * @brief Data model for sign generation
 */
class CFD_EXPORT SignParameter {
 public:
  /**
   * @brief constructor(for vector)
   */
  SignParameter();
  /**
   * @brief constructor(Type: auto)
   * @param[in] text_message            text data
   * @param[in] der_encode              flag of need der encode
   * @param[in] sighash_type            sighash type (SigHashType)
   */
  explicit SignParameter(
      const std::string& text_message, bool der_encode = false,
      const SigHashType sighash_type =
          SigHashType(SigHashAlgorithm::kSigHashAll));
  /**
   * @brief constructor(Type: Sign)
   * @param[in] data                    byte data
   * @param[in] der_encode              flag of need der encode
   * @param[in] sighash_type            sighash type (SigHashType)
   */
  explicit SignParameter(
      const ByteData& data, bool der_encode,
      const SigHashType sighash_type =
          SigHashType(SigHashAlgorithm::kSigHashAll));
  /**
   * @brief constructor(Type: Binary)
   * @param[in] data  data
   */
  explicit SignParameter(const ByteData& data);
  /**
   * @brief constructor(Type: Pubkey)
   * @param[in] pubkey  pubkey data
   */
  explicit SignParameter(const Pubkey& pubkey);
  /**
   * @brief constructor(Type: RedeemScript)
   * @param[in] redeem_script  redeem script data
   */
  explicit SignParameter(const Script& redeem_script);
  /**
   * @brief constructor(Type: ScriptOperator)
   * @param[in] op_code  op code
   */
  explicit SignParameter(const ScriptOperator& op_code);
  /**
   * @brief copy constructor.
   * @param[in] sign_parameter     object
   */
  SignParameter(const SignParameter& sign_parameter);
  /**
   * @brief copy constructor.
   * @param[in] sign_parameter     object
   * @return object
   */
  SignParameter& operator=(const SignParameter& sign_parameter);
  /**
   * @brief Set the related pubkey.
   * @param[in] pubkey  realated pubkey
   */
  void SetRelatedPubkey(const Pubkey& pubkey);

  /**
   * @brief Get data
   * @return data
   */
  ByteData GetData() const;
  /**
   * @brief get op_code.
   * @return op_code
   */
  ScriptOperator GetOpCode() const;
  /**
   * @brief check op_code.
   * @retval true op_code
   * @retval false other value
   */
  bool IsOpCode() const;
  /**
   * @brief Get data type
   * @return data
   */
  SignDataType GetDataType() const;
  /**
   * @brief Get RelatedPubkey
   * @return related pubkey data
   */
  Pubkey GetRelatedPubkey() const;
  /**
   * @brief Get DerEncode
   * @return der encode use flag
   */
  bool IsDerEncode() const;
  /**
   * @brief Get sighash type
   * @return sighash type
   */
  SigHashType GetSigHashType() const;
  /**
   * @brief Convert data to signature with stored information
   * @return signature data
   */
  ByteData ConvertToSignature() const;

 private:
  ByteData data_;             //!< data hex
  SignDataType data_type_;    //!< data type
  Pubkey related_pubkey_;     //!< need der encode flag
  bool der_encode_;           //!< sighash type
  SigHashType sighash_type_;  //!< AnyoneCanPay flag
  ScriptOperator op_code_;    //!< op_code
};

// ----------------------------------------------------------------------------
// deprecated
// ----------------------------------------------------------------------------

/**
 * @brief Transaction生成のためのController基底クラス
 * @deprecated replace to TransactionContext .
 */
class CFD_EXPORT AbstractTransactionController {
 public:
  /**
   * @brief コンストラクタ.
   */
  AbstractTransactionController();
  /**
   * @brief デストラクタ.
   */
  virtual ~AbstractTransactionController() {
    // do nothing
  }

  /**
   * @brief TransactionHex文字列取得.
   * @return TransactionHEX文字列
   */
  std::string GetHex() const;

  /**
   * @brief ロックタイムからデフォルトのシーケンス番号を取得する。
   * @retval 0xffffffff     locktime値無効
   * @retval 0xfffffffe     locktime値有効
   */
  uint32_t GetDefaultSequence() const;

  /**
   * @brief ロックタイムからlocktime値無効のシーケンス番号を取得する。
   * @retval 0xffffffff     locktime値無効
   */
  static uint32_t GetLockTimeDisabledSequence();

 protected:
  AbstractTransaction* tx_address_;  //!< Transaction基底オブジェクト
};

}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFD_TRANSACTION_COMMON_H_
