// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_transaction_base.cpp
 *
 * @brief \~english implementation file for transaction operation that uses cfd-api
 *   \~japanese cfd-apiで利用するTransaction作成の実装ファイル
 */
#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfdapi_address.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_iterator.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_util.h"

#include "cfdapi_transaction_base.h"  // NOLINT

namespace cfd {
namespace api {

using cfd::SignParameter;
using cfd::TransactionController;
using cfd::core::AbstractTxInReference;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
using cfd::core::IteratorWrapper;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptElement;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::logger::warn;

/**
 * @brief Sets a P2sh unlocking script for a transaction input.
 * @param[in] signature_data the signatures to include in the script.
 * @param[in] redeem_script the redeem script for the input.
 * @param[in] txid the transaction id of the UTXO being spent by the input.
 * @param[in] vout the vout of the UTXO being spent by the input.
 * @param[in] txc the transaction controller containing the input.
 */
template <class T>
static void SetP2shMultisigUnlockingScript(
    const std::vector<ByteData>& signature_data, const Script& redeem_script,
    const Txid& txid, const uint32_t vout, T* txc) {
  ScriptBuilder sb;
  sb.AppendOperator(ScriptOperator::OP_0);
  for (const ByteData& signature : signature_data) {
    sb.AppendData(signature);
  }
  sb.AppendData(redeem_script);
  txc->SetUnlockingScript(txid, vout, sb.Build());
}

/**
 * @brief Sets the witness stack of a transaction to unlock a multisig output.
 * @param[in] signature_data the signatures to include in the script.
 * @param[in] redeem_script the redeem script for the input.
 * @param[in] txid the transaction id of the UTXO being spent by the input.
 * @param[in] vout the vout of the UTXO being spent by the input.
 * @param[in] clear_stack wether the witness stack should be cleared before
 * being set.
 * @param[in] txc the transaction controller containing the input.
 */
template <class T>
static void SetP2wshMultisigWitnessStack(
    const std::vector<ByteData>& signature_data, const Script& redeem_script,
    const Txid& txid, const uint32_t vout, const bool clear_stack, T* txc) {
  if (clear_stack) {
    txc->RemoveWitnessStackAll(txid, vout);
  }
  // set empty data
  std::vector<ByteData> witness_stack;
  // push empty byte
  witness_stack.push_back(ByteData());
  // create stack
  std::copy(
      signature_data.begin(), signature_data.end(),
      std::back_inserter(witness_stack));
  witness_stack.push_back(redeem_script.GetData());

  // set witness stack
  txc->AddWitnessStack(txid, vout, witness_stack);
}

/**
 * @brief Validate the request for AddMultisigSign.
 * @param[in] tx_hex          tx hex string
 * @param[in] sign_list       value (amount or commitment)
 * @param[in] address_type    address type. (support is P2sh-P2wsh or P2wsh)
 * @param[in] witness_script  witness script
 * @param[in] redeem_script   redeem script
 */
static void ValidateAddMultisigSign(  // linefeed
    const std::string& tx_hex, const std::vector<SignParameter>& sign_list,
    AddressType address_type, const Script& witness_script,
    const Script redeem_script) {
  // check txHex
  if (tx_hex.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddSegwitMultisigSign. Transaction hex empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid hex string. empty txHex.");
  }

  // check require script
  switch (address_type) {
    case AddressType::kP2shAddress: {
      if (redeem_script.IsEmpty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSign. redeem script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty redeemScript.");
      }
      break;
    }
    case AddressType::kP2wshAddress: {
      if (witness_script.IsEmpty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSign. witness script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty witnessScript.");
      }
      break;
    }
    case AddressType::kP2shP2wshAddress: {
      if (redeem_script.IsEmpty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSign. redeem script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty redeemScript.");
      }
      if (witness_script.IsEmpty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSign. witness script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty witnessScript.");
      }
      break;
    }
    default: {
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddSegwitMultisigSign. address type must be one "
          "of p2sh address.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Invalid address type.");
    }
  }

  // check signData (not empty)
  if (sign_list.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddSegwitMultisigSign. sign parameters empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid array length. empty signParams.");
  }

  // check signData (too much data)
  if (sign_list.size() > 15) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddSegwitMultisigSign. sign array length over.");
    throw CfdException(
        CfdError::kCfdOutOfRangeError,
        "Value out of range. sign array length over.");
  }
}

template <class T>
uint32_t TransactionApiBase::GetWitnessStackNum(
    std::function<T(const std::string&)> create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout) {
  if (tx_hex.empty()) {
    warn(CFD_LOG_SOURCE, "Failed to GetWitnessStackNum. hex empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Invalid hex string. empty data.");
  }

  // TransactionController作成
  T txc = create_controller(tx_hex);

  return txc.GetWitnessStackNum(txid, vout);
}

template <class T>
T TransactionApiBase::UpdateWitnessStack(
    std::function<T(const std::string&)> create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const SignParameter& update_sign_param, uint32_t stack_index) {
  if (tx_hex.empty()) {
    warn(CFD_LOG_SOURCE, "Failed to UpdateWitnessStack. hex empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Invalid hex string. empty data.");
  }

  // TransactionController作成
  T txc = create_controller(tx_hex);

  // Witnessの更新
  txc.SetWitnessStack(
      txid, vout, stack_index, update_sign_param.ConvertToSignature());
  return txc;
}

template <class T>
T TransactionApiBase::AddSign(
    std::function<T(const std::string&)> create_controller,
    const std::string& hex, const Txid& txid, uint32_t vout,
    const std::vector<SignParameter>& sign_params, bool is_witness,
    bool clear_stack) {
  if (hex.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddSign. Invalid hex string. empty data. tx=[{}]", hex);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "Invalid hex string. empty data.");
  }

  // TransactionController作成
  T txc = create_controller(hex);

  std::vector<ByteData> sign_stack;
  bool has_op_code = false;
  for (const SignParameter& sign_param : sign_params) {
    if (sign_param.IsOpCode()) {
      ScriptOperator op_code = sign_param.GetOpCode();
      if (op_code.IsPushOperator()) {
        has_op_code = true;
      }
    }
    sign_stack.push_back(sign_param.ConvertToSignature());
  }

  if (is_witness) {
    // Witnessの追加
    if (clear_stack) {
      txc.RemoveWitnessStackAll(txid, vout);
    }
    txc.AddWitnessStack(txid, vout, sign_stack);
  } else if (!has_op_code) {
    if (clear_stack) {
      txc.SetUnlockingScript(txid, vout, sign_stack);
    } else {
      txc.InsertUnlockingScript(txid, vout, sign_stack);
    }
  } else {
    Script script;
    if (!clear_stack) {
      script = txc.GetTxIn(txid, vout).GetUnlockingScript();
    }
    ScriptBuilder builder;
    for (const auto& element : script.GetElementList()) {
      builder.AppendElement(element);
    }
    for (const SignParameter& sign_param : sign_params) {
      if (sign_param.IsOpCode()) {
        // Checking push-operator is performed at the time of registration.
        builder.AppendOperator(sign_param.GetOpCode());
      } else {
        builder.AppendData(sign_param.ConvertToSignature());
      }
    }
    txc.SetUnlockingScript(txid, vout, builder.Build());
  }

  return txc;
}

template <class T>
std::string TransactionApiBase::AddMultisigSign(
    std::function<T(const std::string&)> create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const std::vector<SignParameter>& sign_list, AddressType address_type,
    const Script& witness_script, const Script redeem_script,
    bool clear_stack) {
  ValidateAddMultisigSign(
      tx_hex, sign_list, address_type, witness_script, redeem_script);
  T txc = create_controller(tx_hex);

  // extract pubkeys from redeem script
  // ValidateAddMultiSignRequest ensures that we have one of three correct
  // types.
  Script script = (address_type == AddressType::kP2shAddress) ? redeem_script
                                                              : witness_script;

  std::vector<Pubkey> pubkeys =
      ScriptUtil::ExtractPubkeysFromMultisigScript(script);
  // get signParams from json request
  std::vector<SignParameter> sign_params = sign_list;

  // set signParam to signature_data (only contains relatedPubkey);
  std::vector<ByteData> signature_data;
  for (const Pubkey& pubkey : pubkeys) {
    std::string pubkey_hex = pubkey.GetHex();
    for (auto itr = sign_params.begin(); itr != sign_params.end();) {
      SignParameter sign_param = *itr;
      Pubkey related_pubkey = sign_param.GetRelatedPubkey();
      if ((!related_pubkey.IsValid()) ||
          (related_pubkey.GetHex() != pubkey_hex)) {
        ++itr;
        continue;
      }

      itr = sign_params.erase(itr);
      signature_data.push_back(sign_param.ConvertToSignature());
    }
  }

  // set the others to signature data
  for (const auto& sign_param : sign_params) {
    // related pubkey not found in script
    if (sign_param.GetRelatedPubkey().IsValid()) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddMultisigSign. Missing related pubkey"
          " in script.: relatedPubkey={}, script={}",
          sign_param.GetRelatedPubkey().GetHex(), script.GetHex());
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Missing related pubkey in script."
          " Check your signature and pubkey pair.");
    }
    signature_data.push_back(sign_param.ConvertToSignature());
  }

  // set signatures to target input
  if (address_type == AddressType::kP2shAddress) {
    SetP2shMultisigUnlockingScript(signature_data, script, txid, vout, &txc);
  } else {
    SetP2wshMultisigWitnessStack(
        signature_data, script, txid, vout, clear_stack, &txc);
  }

  if (address_type == AddressType::kP2shP2wshAddress) {
    // set p2sh redeem script to unlockking script
    ScriptBuilder sb;
    sb.AppendData(redeem_script);
    txc.SetUnlockingScript(txid, vout, sb.Build());
  }

  return txc.GetHex();
}

template uint32_t
TransactionApiBase::GetWitnessStackNum<TransactionController>(
    std::function<TransactionController(const std::string&)> create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout);

template TransactionController
TransactionApiBase::UpdateWitnessStack<TransactionController>(
    std::function<TransactionController(const std::string&)> create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const SignParameter& update_sign_param, uint32_t stack_index);

template TransactionController
TransactionApiBase::AddSign<TransactionController>(
    std::function<TransactionController(const std::string&)> create_controller,
    const std::string& hex, const Txid& txid, const uint32_t vout,
    const std::vector<SignParameter>& sign_params, bool is_witness,
    bool clear_stack);

template std::string
TransactionApiBase::AddMultisigSign<TransactionController>(
    std::function<TransactionController(const std::string&)> create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const std::vector<SignParameter>& sign_list, AddressType address_type,
    const Script& witness_script, const Script redeem_script,
    bool clear_stack);

#ifndef CFD_DISABLE_ELEMENTS

using cfd::ConfidentialTransactionController;

template uint32_t
TransactionApiBase::GetWitnessStackNum<ConfidentialTransactionController>(
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout);

template ConfidentialTransactionController
TransactionApiBase::UpdateWitnessStack<ConfidentialTransactionController>(
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const SignParameter& update_sign_param, uint32_t stack_index);

template ConfidentialTransactionController
TransactionApiBase::AddSign<ConfidentialTransactionController>(
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller,
    const std::string& hex, const Txid& txid, const uint32_t vout,
    const std::vector<SignParameter>& sign_params, bool is_witness,
    bool clear_stack);

template std::string
TransactionApiBase::AddMultisigSign<ConfidentialTransactionController>(
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller,
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const std::vector<SignParameter>& sign_list, AddressType address_type,
    const Script& witness_script, const Script redeem_script,
    bool clear_stack);
#endif

}  // namespace api
}  // namespace cfd
