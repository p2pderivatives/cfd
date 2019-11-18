// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_transaction_base.cpp
 *
 * @brief cfd-apiで利用するTransaction作成の実装ファイル
 */
#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_struct.h"
#include "cfd/cfdapi_transaction_base.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_iterator.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_util.h"

#include "cfdapi_internal.h"  // NOLINT

namespace cfd {
namespace js {
namespace api {

using cfd::TransactionController;
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
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::logger::warn;

/**
 * @brief Validate the request for AddMultisigSign.
 * @param[in] req       request information
 * @param[in] addr_type the address type for the request
 */
static void ValidateAddMultisigSignRequest(  // linefeed
    AddMultisigSignRequestStruct req, AddressType addr_type) {
  // check txHex
  if (req.tx.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddSegwitMultisigSignRequest. Transaction hex empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid hex string. empty txHex.");
  }

  // check require script
  switch (addr_type) {
    case AddressType::kP2shAddress: {
      if (req.txin.redeem_script.empty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSignRequest. redeem script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty redeemScript.");
      }
      break;
    }
    case AddressType::kP2wshAddress: {
      if (req.txin.witness_script.empty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSignRequest. witness script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty witnessScript.");
      }
      break;
    }
    case AddressType::kP2shP2wshAddress: {
      if (req.txin.redeem_script.empty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSignRequest. redeem script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty redeemScript.");
      }
      if (req.txin.witness_script.empty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddSegwitMultisigSignRequest. witness script empty.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Invalid hex string. empty witnessScript.");
      }
      break;
    }
    default: {
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddSegwitMultisigSignRequest. address type must be one "
          "of p2sh address.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Invalid address type.");
    }
  }

  // check signData (not empty)
  if (req.txin.sign_params.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddSegwitMultisigSignRequest. sign parameters empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid array length. empty signParams.");
  }

  // check signData (too much data)
  if (req.txin.sign_params.size() > 15) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to AddSegwitMultisigSignRequest. sign array length over.");
    throw CfdException(
        CfdError::kCfdOutOfRangeError,
        "Value out of range. sign array length over.");
  }
}

SigHashType TransactionApiBase::ConvertSigHashType(
    const std::string& sighash_type_string, bool is_anyone_can_pay) {
  std::string check_string = sighash_type_string;
  std::transform(
      check_string.begin(), check_string.end(), check_string.begin(),
      ::tolower);
  if (check_string == "all") {
    return SigHashType(SigHashAlgorithm::kSigHashAll, is_anyone_can_pay);
  } else if (check_string == "none") {
    return SigHashType(SigHashAlgorithm::kSigHashNone, is_anyone_can_pay);
  } else if (check_string == "single") {
    return SigHashType(SigHashAlgorithm::kSigHashSingle, is_anyone_can_pay);
  }
  warn(
      CFD_LOG_SOURCE,
      "Failed to CreateMultisig. Invalid sighash_type: sighashType={}",
      sighash_type_string);
  throw CfdException(
      CfdError::kCfdIllegalArgumentError,
      "Invalid sighashType. sighashType must be "
      "\"all, none, single\".");
}

/**
 * @brief Convert signature information to a signature.
 * @param[in] hex_string              Signature information
 * @param[in] is_sign                 Whether signature data is provided
 * @param[in] is_der_encode           Whether the signature is DER encoded
 * @param[in] sighash_type            SigHash type
 * @param[in] sighash_anyone_can_pay  Flag determining if SigHash is
 * anyone_can_pay
 * @return Converted signature information.
 */
static ByteData ConvertSignDataToSignature(
    const std::string& hex_string, bool is_sign, bool is_der_encode,
    const std::string& sighash_type, bool sighash_anyone_can_pay) {
  ByteData byte_data;
  if (is_sign && is_der_encode) {
    if (hex_string.empty()) {
      warn(CFD_LOG_SOURCE, "Failed to AddMultisigSign. sign hex empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty sign hex.");
    }
    SigHashType sighashtype = TransactionApiBase::ConvertSigHashType(
        sighash_type, sighash_anyone_can_pay);
    byte_data = CryptoUtil::ConvertSignatureToDer(hex_string, sighashtype);
  } else if (hex_string.empty()) {
    // do nothing
  } else {
    byte_data = ByteData(hex_string);
  }
  return byte_data;
}

template <class T>
AddSignResponseStruct TransactionApiBase::AddSign(
    const AddSignRequestStruct& request,
    std::function<T(const std::string&)> create_controller) {
  auto call_func =
      [create_controller](
          const AddSignRequestStruct& request) -> AddSignResponseStruct {
    AddSignResponseStruct response;
    const std::string& hex_string = request.tx;
    if (hex_string.empty()) {
      warn(CFD_LOG_SOURCE, "Failed to AddSignRequest. hex empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty data.");
    }

    // TransactionController作成
    T txc = create_controller(hex_string);

    if (request.txin.is_witness) {
      // Witnessの追加
      Txid txid = Txid(request.txin.txid);
      if (request.txin.clear_stack) {
        txc.RemoveWitnessStackAll(txid, request.txin.vout);
      }
      std::vector<ByteData> witness_datas;
      for (const SignDataStruct& stack_req : request.txin.sign_param) {
        ByteData byte_data = ConvertSignDataToSignature(
            stack_req.hex, (stack_req.type == "sign"), stack_req.der_encode,
            stack_req.sighash_type, stack_req.sighash_anyone_can_pay);
        witness_datas.push_back(byte_data);
      }
      txc.AddWitnessStack(txid, request.txin.vout, witness_datas);
    } else {
      std::vector<ByteData> unlock_script;
      for (const SignDataStruct& stack_req : request.txin.sign_param) {
        ByteData byte_data = ConvertSignDataToSignature(
            stack_req.hex, (stack_req.type == "sign"), stack_req.der_encode,
            stack_req.sighash_type, stack_req.sighash_anyone_can_pay);
        unlock_script.push_back(byte_data);
      }
      txc.SetUnlockingScript(
          Txid(request.txin.txid), request.txin.vout, unlock_script);
    }

    response.hex = txc.GetHex();
    return response;
  };

  AddSignResponseStruct result;
  result = ExecuteStructApi<AddSignRequestStruct, AddSignResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

template <class T>
GetWitnessStackNumResponseStruct TransactionApiBase::GetWitnessStackNum(
    const GetWitnessStackNumRequestStruct& request,
    std::function<T(const std::string&)> create_controller) {
  auto call_func =
      [create_controller](const GetWitnessStackNumRequestStruct& request)
      -> GetWitnessStackNumResponseStruct {  // NOLINT
    GetWitnessStackNumResponseStruct response;
    std::string hex_string = request.tx;
    if (hex_string.empty()) {
      warn(CFD_LOG_SOURCE, "Failed to GetWitnessStackNum. hex empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty data.");
    }

    // TransactionController作成
    T txc = create_controller(hex_string);

    uint32_t count =
        txc.GetWitnessStackNum(Txid(request.txin.txid), request.txin.vout);

    response.count = count;
    return response;
  };
  GetWitnessStackNumResponseStruct result;
  result = ExecuteStructApi<
      GetWitnessStackNumRequestStruct, GetWitnessStackNumResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

template <class T>
UpdateWitnessStackResponseStruct TransactionApiBase::UpdateWitnessStack(
    const UpdateWitnessStackRequestStruct& request,
    std::function<T(const std::string&)> create_controller) {
  auto call_func =
      [create_controller](const UpdateWitnessStackRequestStruct& request)
      -> UpdateWitnessStackResponseStruct {  // NOLINT
    UpdateWitnessStackResponseStruct response;
    const std::string& hex_string = request.tx;
    if (hex_string.empty()) {
      warn(CFD_LOG_SOURCE, "Failed to UpdateWitnessStack. hex empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty data.");
    }

    // TransactionController作成
    T txc = create_controller(hex_string);

    // Witnessの更新
    const WitnessStackDataStruct& stack_req = request.txin.witness_stack;
    ByteData byte_data = ConvertSignDataToSignature(
        stack_req.hex, (stack_req.type == "sign"), stack_req.der_encode,
        stack_req.sighash_type, stack_req.sighash_anyone_can_pay);
    txc.SetWitnessStack(
        Txid(request.txin.txid), request.txin.vout,
        static_cast<uint32_t>(stack_req.index), byte_data);

    response.hex = txc.GetHex();
    return response;
  };

  UpdateWitnessStackResponseStruct result;
  result = ExecuteStructApi<
      UpdateWitnessStackRequestStruct, UpdateWitnessStackResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

/**
 * @brief Get the set of public keys contained in a multisig script.
 * @details if the redeem script contains multiple OP_CHECKMULTISIG(VERIFY),
 * returns only the public keys required for the last one.
 * @param[in] multisig_script the multisig redeem script.
 * @return an array of public keys.
 */
const std::vector<Pubkey> ExtractPubkeysFromMultisigScript(
    const Script& multisig_script) {
  std::vector<Pubkey> pubkeys;

  const std::vector<ScriptElement> elements = multisig_script.GetElementList();

  // find OP_CHECKMULTISIG or OP_CHECKMULTISIGVERIFY
  IteratorWrapper<ScriptElement> itr = IteratorWrapper<ScriptElement>(
      elements, "Invalid script element access", true);
  // search OP_CHECKMULTISIG(or VERIFY)
  while (itr.hasNext()) {
    ScriptElement element = itr.next();
    if (!element.IsOpCode()) {
      continue;
    }
    if (element.GetOpCode() == ScriptOperator::OP_CHECKMULTISIG ||
        element.GetOpCode() == ScriptOperator::OP_CHECKMULTISIGVERIFY) {
      break;
    }
  }
  // target opcode not found
  if (!itr.hasNext()) {
    warn(
        CFD_LOG_SOURCE,
        "Multisig opcode (OP_CHECKMULTISIG|VERIFY) not found"
        " in redeem script: script={}",
        multisig_script.ToString());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "OP_CHCKMULTISIG(OP_CHECKMULTISIGVERIFY) not found"
        " in redeem script.");
  }

  // get contain pubkey num
  const ScriptElement& op_m = itr.next();
  if (!op_m.IsNumber()) {
    warn(
        CFD_LOG_SOURCE,
        "Invalid OP_CHECKMULTISIG(VERIFY) input in redeem script."
        " Missing contain pubkey number.: script={}",
        multisig_script.ToString());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid OP_CHCKMULTISIG(OP_CHECKMULTISIGVERIFY) input"
        " in redeem script. Missing contain pubkey number.");
  }

  // set pubkey to vector(reverse data)
  int64_t contain_pubkey_num = op_m.GetNumber();
  for (int64_t i = 0; i < contain_pubkey_num; ++i) {
    if (!itr.hasNext()) {
      warn(
          CFD_LOG_SOURCE,
          "Not found enough pubkeys in redeem script.: "
          "require_pubkey_num={}, script={}",
          contain_pubkey_num, multisig_script.ToString());
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Not found enough pubkeys in redeem script.");
    }

    const ScriptElement& pubkey_element = itr.next();
    // check script element type
    if (!pubkey_element.IsBinary()) {
      warn(
          CFD_LOG_SOURCE,
          "Invalid script element. Not binary element.: "
          "ScriptElementType={}, data={}",
          pubkey_element.GetType(), pubkey_element.ToString());
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid ScriptElementType.(not binary)");
    }

    // push pubkey data
    pubkeys.push_back(Pubkey(pubkey_element.GetBinaryData()));
  }

  // check opcode(require signature num)
  if (!itr.hasNext() || !itr.next().IsNumber()) {
    warn(
        CFD_LOG_SOURCE,
        "Invalid OP_CHECKMULTISIG(VERIFY) input in redeem script."
        " Missing require signature number.: script={}",
        multisig_script.ToString());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid OP_CHCKMULTISIG(OP_CHECKMULTISIGVERIFY) input"
        " in redeem script. Missing require signature number.");
  }

  // return reverse pubkey vector
  std::reverse(std::begin(pubkeys), std::end(pubkeys));
  return pubkeys;
}

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

template <class T>
AddMultisigSignResponseStruct TransactionApiBase::AddMultisigSign(
    const AddMultisigSignRequestStruct& request,
    std::function<T(const std::string&)> create_controller) {
  auto call_func =
      [create_controller](const AddMultisigSignRequestStruct& request)
      -> AddMultisigSignResponseStruct {  // NOLINT
    AddMultisigSignResponseStruct response;
    // validate request
    AddressType addr_type =
        AddressStructApi::ConvertAddressType(request.txin.hash_type);
    ValidateAddMultisigSignRequest(request, addr_type);

    const std::string& hex_string = request.tx;
    T txc = create_controller(hex_string);

    // extract pubkeys from redeem script
    // ValidateAddMultiSignRequest ensures that we have one of three correct
    // types.
    Script redeem_script = addr_type == AddressType::kP2shAddress
                               ? Script(request.txin.redeem_script)
                               : Script(request.txin.witness_script);

    std::vector<Pubkey> pubkeys =
        ExtractPubkeysFromMultisigScript(redeem_script);
    // get signParams from json request
    std::vector<MultisigSignDataStruct> sign_params = request.txin.sign_params;

    // set signParam to signature_data (only contains relatedPubkey);
    std::vector<ByteData> signature_data;
    for (const Pubkey& pubkey : pubkeys) {
      for (auto itr = sign_params.begin(); itr != sign_params.end();) {
        MultisigSignDataStruct sign_param = *itr;
        if (sign_param.related_pubkey != pubkey.GetHex()) {
          ++itr;
          continue;
        }

        itr = sign_params.erase(itr);
        ByteData byte_data = ConvertSignDataToSignature(
            sign_param.hex, true, sign_param.der_encode,
            sign_param.sighash_type, sign_param.sighash_anyone_can_pay);
        signature_data.push_back(byte_data);
      }
    }

    // set the others to signature data
    for (MultisigSignDataStruct sign_param : sign_params) {
      // related pubkey not found in script
      if (!sign_param.related_pubkey.empty()) {
        warn(
            CFD_LOG_SOURCE,
            "Failed to AddMultisigSign. Missing related pubkey"
            " in script.: relatedPubkey={}, script={}",
            sign_param.related_pubkey, redeem_script.GetHex());
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Missing related pubkey in script."
            " Check your signature and pubkey pair.");
      }
      ByteData byte_data = ConvertSignDataToSignature(
          sign_param.hex, true, sign_param.der_encode, sign_param.sighash_type,
          sign_param.sighash_anyone_can_pay);
      signature_data.push_back(byte_data);
    }

    // set signatures to target input
    Txid txid(request.txin.txid);
    if (addr_type == AddressType::kP2shAddress) {
      SetP2shMultisigUnlockingScript(
          signature_data, redeem_script, txid, request.txin.vout, &txc);
    } else {
      SetP2wshMultisigWitnessStack(
          signature_data, redeem_script, txid, request.txin.vout,
          request.txin.clear_stack, &txc);
    }

    if (addr_type == AddressType::kP2shP2wshAddress) {
      // set p2sh redeem script to unlockking script
      Script p2sh_redeem_script(request.txin.redeem_script);
      ScriptBuilder sb;
      sb.AppendData(p2sh_redeem_script);
      txc.SetUnlockingScript(
          Txid(request.txin.txid), request.txin.vout, sb.Build());
    }

    response.hex = txc.GetHex();
    return response;
  };

  AddMultisigSignResponseStruct result;
  result = ExecuteStructApi<
      AddMultisigSignRequestStruct, AddMultisigSignResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

ExtractScriptData TransactionApiBase::ExtractLockingScript(
    Script locking_script) {
  ExtractScriptData extract_data;

  std::string script_type;
  std::vector<ScriptElement> elems = locking_script.GetElementList();
  // FIXME(fujita-cg): anyonecanspent_aremineフラグの判定
  if (elems.size() == 1 && elems[0].GetOpCode() == ScriptOperator::OP_TRUE) {
    extract_data.script_type = LockingScriptType::kTrue;
    return extract_data;
  }
  if (locking_script.IsEmpty()) {
    // empty
    extract_data.script_type = LockingScriptType::kFee;
    return extract_data;
  }
  if (locking_script.IsP2shScript()) {
    // P2shScript
    extract_data.script_type = LockingScriptType::kPayToScriptHash;
    extract_data.pushed_datas.push_back(elems[1].GetBinaryData());
    return extract_data;
  }

  // WitnessData
  if (locking_script.IsWitnessProgram()) {
    uint8_t witness_version = static_cast<uint8_t>(elems[0].GetNumber());
    ByteData program = elems[1].GetBinaryData();
    if (locking_script.IsP2wpkhScript()) {
      // tx witness keyhash
      extract_data.script_type = LockingScriptType::kWitnessV0KeyHash;
      extract_data.pushed_datas.push_back(program);
    } else if (locking_script.IsP2wshScript()) {
      // tx witness scripthash
      extract_data.script_type = LockingScriptType::kWitnessV0ScriptHash;
      extract_data.pushed_datas.push_back(program);
    } else if (witness_version != 0) {
      // tx witness unknown
      extract_data.script_type = LockingScriptType::kWitnessUnknown;
      std::vector<uint8_t> data;
      data.reserve(program.GetDataSize() + 1);
      data.push_back(witness_version);
      std::copy(
          program.GetBytes().begin(), program.GetBytes().end(),
          std::back_inserter(data));
      extract_data.pushed_datas.push_back(ByteData(data));
    } else {
      // non standard
      extract_data.script_type = LockingScriptType::kNonStandard;
    }
    return extract_data;
  }

  if (elems.size() >= 1 && elems[0].GetOpCode() == ScriptOperator::OP_RETURN) {
    // tx null data
    extract_data.script_type = LockingScriptType::kNullData;
    return extract_data;
  }
  if (locking_script.IsP2pkScript()) {
    // tx pubkey
    extract_data.script_type = LockingScriptType::kPayToPubkey;
    extract_data.pushed_datas.push_back(elems[0].GetBinaryData());
    return extract_data;
  }
  if (locking_script.IsP2pkhScript()) {
    // tx pubkey hash
    extract_data.script_type = LockingScriptType::kPayToPubkeyHash;
    extract_data.pushed_datas.push_back(elems[2].GetBinaryData());
    return extract_data;
  }
  if (locking_script.IsMultisigScript()) {
    // tx multisig
    extract_data.script_type = LockingScriptType::kMultisig;
    decltype(elems)::const_iterator itr = elems.cbegin();
    extract_data.req_sigs = (*itr).GetNumber();
    ++itr;
    for (; itr != (elems.cend() - 2); ++itr) {
      ByteData pubkey_byte = (*itr).GetBinaryData();
      extract_data.pushed_datas.push_back(pubkey_byte);
    }
    return extract_data;
  }

  // tx nonstandard
  extract_data.script_type = LockingScriptType::kNonStandard;
  return extract_data;
}

std::string TransactionApiBase::ConvertLockingScriptTypeString(
    LockingScriptType script_type) {
  switch (script_type) {
    case LockingScriptType::kNonStandard:
      return "nonstandard";
    case LockingScriptType::kPayToPubkey:
      return "pubkey";
    case LockingScriptType::kPayToPubkeyHash:
      return "pubkeyhash";
    case LockingScriptType::kPayToScriptHash:
      return "scripthash";
    case LockingScriptType::kMultisig:
      return "multisig";
    case LockingScriptType::kNullData:
      return "nulldata";
    case LockingScriptType::kWitnessV0ScriptHash:
      return "witness_v0_scripthash";
    case LockingScriptType::kWitnessV0KeyHash:
      return "witness_v0_keyhash";
    case LockingScriptType::kWitnessUnknown:
      return "witness_unknown";
    case LockingScriptType::kTrue:
      return "true";
#ifndef CFD_DISABLE_ELEMENTS
    case LockingScriptType::kFee:
      return "fee";
#endif  // CFD_DISABLE_ELEMENTS
  }
  return "";
}

template AddSignResponseStruct
TransactionApiBase::AddSign<TransactionController>(
    const AddSignRequestStruct& request,
    std::function<TransactionController(const std::string&)>
        create_controller);

template GetWitnessStackNumResponseStruct
TransactionApiBase::GetWitnessStackNum<TransactionController>(
    const GetWitnessStackNumRequestStruct& request,
    std::function<TransactionController(const std::string&)>
        create_controller);

template UpdateWitnessStackResponseStruct
TransactionApiBase::UpdateWitnessStack<TransactionController>(
    const UpdateWitnessStackRequestStruct& request,
    std::function<TransactionController(const std::string&)>
        create_controller);

template AddMultisigSignResponseStruct
TransactionApiBase::AddMultisigSign<TransactionController>(
    const AddMultisigSignRequestStruct& request,
    std::function<TransactionController(const std::string&)>
        create_controller);

#ifndef CFD_DISABLE_ELEMENTS

using cfd::ConfidentialTransactionController;

template AddSignResponseStruct
TransactionApiBase::AddSign<ConfidentialTransactionController>(
    const AddSignRequestStruct& request,
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller);

template GetWitnessStackNumResponseStruct
TransactionApiBase::GetWitnessStackNum<ConfidentialTransactionController>(
    const GetWitnessStackNumRequestStruct& request,
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller);

template UpdateWitnessStackResponseStruct
TransactionApiBase::UpdateWitnessStack<ConfidentialTransactionController>(
    const UpdateWitnessStackRequestStruct& request,
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller);

template AddMultisigSignResponseStruct
TransactionApiBase::AddMultisigSign<ConfidentialTransactionController>(
    const AddMultisigSignRequestStruct& request,
    std::function<ConfidentialTransactionController(const std::string&)>
        create_controller);
#endif

}  // namespace api
}  // namespace js
}  // namespace cfd
