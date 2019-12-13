// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_transaction_base.h
 *
 * @brief cfd-apiで利用するTransaction作成のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_SRC_CFDAPI_TRANSACTION_BASE_H_
#define CFD_SRC_CFDAPI_TRANSACTION_BASE_H_

#include <functional>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_transaction.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

/**
 * @brief cfdapi namespace
 */
namespace cfd {
namespace api {

using cfd::core::AbstractTxInReference;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::Txid;

/**
 * @brief Class providing common functionalities to TransactionStructApi and
 * ConfidentialTransactionApi.
 */
class TransactionApiBase {
 public:
  /**
   * @brief Outputs number of elements in witness stack.
   * @param[in] create_controller a callback to create a transaction controller.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @return number of the witness stack.
   */
  template <class T>
  static uint32_t GetWitnessStackNum(
      std::function<T(const std::string&)> create_controller,
      const std::string& tx_hex, const Txid& txid, uint32_t vout);

  /**
   * @brief Update witness stack.
   * @param[in] create_controller a callback to create a transaction controller.
   * @param[in] tx_hex              tx hex string
   * @param[in] txid                target tx input txid
   * @param[in] vout                target tx input vout
   * @param[in] update_sign_param   sign parameter to update the input
   * @param[in] stack_index         witness stack index
   * @return Transaction controller
   */
  template <class T>
  static T UpdateWitnessStack(
      std::function<T(const std::string&)> create_controller,
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const SignParameter& update_sign_param, uint32_t stack_index);

  /**
   * @brief Add signature information based on parameter information.
   * @param[in] create_controller   transaction controller create function
   * @param[in] hex                 transaction hex
   * @param[in] txid                txid of input to add sign parameters to
   * @param[in] vout                vout of input to add sign parameters to
   * @param[in] sign_params         sign parameters to add the input
   * @param[in] is_witness          flag to add sign parameters to
   *     witness or unlocking script
   * @param[in] clear_stack         flag of clear all stacks
   * @return Transaction controller
   */
  template <class T>
  static T AddSign(
      std::function<T(const std::string&)> create_controller,
      const std::string& hex, const Txid& txid, uint32_t vout,
      const std::vector<SignParameter>& sign_params, bool is_witness = true,
      bool clear_stack = true);

  /**
   * @brief Add Segwit multisig signature information.
   * @details the order of signatures to be added is automatically aligned
   * with the corresponding pubkey in redeemscript and relatedPubkey in
   * signParam. (If relatedPubkey is not set, signatures are added in order of
   * signParam after adding signature with relatedPubkey).
   * @param[in] create_controller a callback to create a transaction controller.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            txid of input to add sign parameters to
   * @param[in] vout            vout of input to add sign parameters to
   * @param[in] sign_list       sign parameters to add the input
   * @param[in] address_type    address type. (support is P2sh-P2wsh or P2wsh)
   * @param[in] witness_script  witness script
   * @param[in] redeem_script   redeem script
   * @param[in] clear_stack     clear stack data before add.
   * @return Transaction controller
   */
  template <class T>
  static std::string AddMultisigSign(
      std::function<T(const std::string&)> create_controller,
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const std::vector<SignParameter>& sign_list, AddressType address_type,
      const Script& witness_script, const Script& redeem_script,
      bool clear_stack);

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
  static std::string CreateMultisigScriptSig(
      const std::vector<SignParameter>& sign_list,
      const Script& redeem_script);

  /**
   * @brief Create multisig signature list.
   * @details the order of signatures to be added is automatically aligned
   * with the corresponding pubkey in redeemscript and relatedPubkey in
   * signParam. (If relatedPubkey is not set, signatures are added in order of
   * signParam after adding signature with relatedPubkey).
   * @param[in] sign_list       sign parameters to add the input
   * @param[in] redeem_script   multisig script
   * @return signature list
   */
  static std::vector<ByteData> CreateMultisigSignatureData(
      const std::vector<SignParameter>& sign_list,
      const Script& redeem_script);
};

}  // namespace api
}  // namespace cfd

#endif  // CFD_SRC_CFDAPI_TRANSACTION_BASE_H_
