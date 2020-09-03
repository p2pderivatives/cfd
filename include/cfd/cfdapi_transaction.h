// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_transaction.h
 *
 * @brief cfd-apiで利用するTransaction作成のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_TRANSACTION_H_
#define CFD_INCLUDE_CFD_CFDAPI_TRANSACTION_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_coin.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {
namespace api {

using cfd::SignParameter;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::HashType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::TxIn;
using cfd::core::TxInReference;
using cfd::core::TxOut;

/**
 * @brief Transaction関連のAPIクラス
 */
class CFD_EXPORT TransactionApi {
 public:
  /**
   * @brief constructor
   */
  TransactionApi() {}

  /**
   * @brief Raw Transactionを作成する.
   * @param[in] version     tx version
   * @param[in] locktime    lock time
   * @param[in] txins       tx input list
   * @param[in] txouts      tx output list
   * @return transaction controller
   */
  TransactionController CreateRawTransaction(
      uint32_t version, uint32_t locktime, const std::vector<TxIn>& txins,
      const std::vector<TxOut>& txouts) const;

  /**
   * @brief Raw Transactionに追加する.
   * @param[in] tx_hex      tx hex string
   * @param[in] txins       tx input list
   * @param[in] txouts      tx output list
   * @return transaction controller
   */
  TransactionController AddRawTransaction(
      const std::string& tx_hex, const std::vector<TxIn>& txins,
      const std::vector<TxOut>& txouts) const;

  /**
   * @brief WitnessStack数を出力する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @return WitnessStack数
   */
  uint32_t GetWitnessStackNum(
      const std::string& tx_hex, const Txid& txid, uint32_t vout) const;

  /**
   * @brief hexで与えられたtxに、SignDataを付与したTransctionControllerを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @param[in] sign_params     sign data list
   * @param[in] is_witness      use witness
   * @param[in] clear_stack     clear stack data before add.
   * @return SignDataが付与されたTransactionController
   */
  TransactionController AddSign(
      const std::string& tx_hex, const Txid& txid, const uint32_t vout,
      const std::vector<SignParameter>& sign_params, bool is_witness = true,
      bool clear_stack = false) const;

  /**
   * @brief WitnessStackの情報を更新する.
   * @param[in] tx_hex              tx hex string
   * @param[in] txid                target tx input txid
   * @param[in] vout                target tx input vout
   * @param[in] update_sign_param   sign data
   * @param[in] stack_index         witness stack index
   * @return TransactionController
   */
  TransactionController UpdateWitnessStack(
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const SignParameter& update_sign_param, uint32_t stack_index) const;

  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] pubkey          public key
   * @param[in] amount          amount
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const TxInReference& txin,
      const Pubkey& pubkey, const Amount& amount, HashType hash_type,
      const SigHashType& sighash_type) const;

  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] redeem_script   redeem script
   * @param[in] amount          amount
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const TxInReference& txin,
      const Script& redeem_script, const Amount& amount, HashType hash_type,
      const SigHashType& sighash_type) const;

  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] key_data        key data (pubkey or redeem script)
   * @param[in] amount          amount
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const TxInReference& txin,
      const ByteData& key_data, const Amount& amount, HashType hash_type,
      const SigHashType& sighash_type) const;

  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @param[in] key_data        key data (pubkey or redeem script)
   * @param[in] amount          amount
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const ByteData& key_data, const Amount& amount, HashType hash_type,
      const SigHashType& sighash_type) const;

  /**
   * @brief Multisig署名情報を追加する.
   * @details 追加するsignatureの順序は、redeem
   * scriptのpubkeyとsign_list内のrelatedPubkeyで
   *   対応をとって自動的に整列される.
   * (relatedPubkeyが設定されていない場合は、relatedPubkeyが
   *   設定されているsignatureを追加した後にsignParamの順序でsignatureを追加)
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] sign_list       value (amount or commitment)
   * @param[in] address_type    address type. (support is P2sh-P2wsh or P2wsh)
   * @param[in] witness_script  witness script
   * @param[in] redeem_script   redeem script
   * @param[in] clear_stack     clear stack data before add.
   * @return Transaction
   */
  TransactionController AddMultisigSign(
      const std::string& tx_hex, const TxInReference& txin,
      const std::vector<SignParameter>& sign_list, AddressType address_type,
      const Script& witness_script, const Script redeem_script = Script(),
      bool clear_stack = true);

  /**
   * @brief Multisig署名情報を追加する.
   * @details 追加するsignatureの順序は、redeem
   * scriptのpubkeyとsign_list内のrelatedPubkeyで
   *   対応をとって自動的に整列される.
   * (relatedPubkeyが設定されていない場合は、relatedPubkeyが
   *   設定されているsignatureを追加した後にsignParamの順序でsignatureを追加)
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @param[in] sign_list       value (amount or commitment)
   * @param[in] address_type    address type. (support is P2sh-P2wsh or P2wsh)
   * @param[in] witness_script  witness script
   * @param[in] redeem_script   redeem script
   * @param[in] clear_stack     clear stack data before add.
   * @return Transaction
   */
  TransactionController AddMultisigSign(
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const std::vector<SignParameter>& sign_list, AddressType address_type,
      const Script& witness_script, const Script redeem_script = Script(),
      bool clear_stack = true);

  /**
   * @brief estimate a fee amount from transaction.
   * @param[in] tx_hex              tx hex string
   * @param[in] utxos               using utxo data
   * @param[in] txout_fee           tx fee amount (ignore utxo)
   * @param[in] utxo_fee            utxo fee amount
   * @param[in] effective_fee_rate  effective fee rate (minimum)
   * @return tx fee (contains utxo)
   */
  Amount EstimateFee(
      const std::string& tx_hex, const std::vector<UtxoData>& utxos,
      Amount* txout_fee = nullptr, Amount* utxo_fee = nullptr,
      double effective_fee_rate = 1) const;

  /**
   * @brief calculate fund transaction.
   * @param[in] tx_hex                   tx hex string
   * @param[in] utxos                    using utxo data
   * @param[in] target_value             target value.
   *    Amount more than the specified amount is set in txout.
   *    default is 0 (disable).
   * @param[in] selected_txin_utxos      selected txin utxo
   * @param[in] reserve_txout_address    address for adding txout.
   *    Also serves as a change address.
   * @param[in] effective_fee_rate       effective fee rate (minimum)
   * @param[out] estimate_fee            estimate fee
   * @param[in] filter                   utxo search filter
   * @param[in] option_params            utxo search option
   * @param[out] append_txout_addresses  used txout additional address
   * @param[in] net_type                 network type
   * @param[in] prefix_list              address prefix list
   * @return tx controller
   */
  TransactionController FundRawTransaction(
      const std::string& tx_hex, const std::vector<UtxoData>& utxos,
      const Amount& target_value,
      const std::vector<UtxoData>& selected_txin_utxos,
      const std::string& reserve_txout_address,
      double effective_fee_rate = 20.0, Amount* estimate_fee = nullptr,
      const UtxoFilter* filter = nullptr,
      const CoinSelectionOption* option_params = nullptr,
      std::vector<std::string>* append_txout_addresses = nullptr,
      NetType net_type = NetType::kMainnet,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

  /**
   * @brief Create multisig scriptsig (unlocking script).
   * @details the order of signatures to be added is automatically aligned
   * with the corresponding pubkey in redeemscript and relatedPubkey in
   * signParam. (If relatedPubkey is not set, signatures are added in order of
   * signParam after adding signature with relatedPubkey).
   * @param[in] sign_list       sign parameters to add the input
   * @param[in] redeem_script   multisig script
   * @return scriptsig
   */
  std::string CreateMultisigScriptSig(
      const std::vector<SignParameter>& sign_list,
      const Script& redeem_script);
};

}  // namespace api
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_TRANSACTION_H_
