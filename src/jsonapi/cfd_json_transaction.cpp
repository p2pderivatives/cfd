// Copyright 2020 CryptoGarage
/**
 * @file cfd_json_transaction.cpp
 *
 * @brief cfd-apiで利用するTransaction作成の実装ファイル
 */

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfdapi_coin.h"
#include "cfd/cfdapi_transaction.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_script.h"
#include "jsonapi/autogen/cfd_api_json_autogen.h"  // NOLINT
#include "jsonapi/cfd_json_transaction.h"  // NOLINT

namespace cfd {
namespace api {
namespace json {

using cfd::UtxoData;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
using cfd::core::HashType;
using cfd::core::kByteData160Length;
using cfd::core::kByteData256Length;
using cfd::core::kScriptHashP2pkhLength;
using cfd::core::kScriptHashP2shLength;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptElement;
using cfd::core::ScriptOperator;
using cfd::core::SigHashType;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::TxIn;
using cfd::core::TxInReference;
using cfd::core::TxOut;
using cfd::core::TxOutReference;
using cfd::core::WitnessVersion;
using cfd::core::logger::warn;


// -----------------------------------------------------------------------------
// TransactionJsonApiクラス
// -----------------------------------------------------------------------------
void TransactionJsonApi::DecodeRawTransaction(
    DecodeRawTransactionRequest* request,
    DecodeRawTransactionResponse* response) {
  if (request == nullptr) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "request is null.");
  }
  if (response == nullptr) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "response is null.");
  }
  // validate version number
  const std::string& hex_string = request->GetHex();
  if (hex_string.empty()) {
    warn(
        CFD_LOG_SOURCE, "Failed to DecodeRawTransactionRequest. hex empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid hex string. empty data.");
  }
  // TODO(k-matsuzawa): iswitness未使用。bitcoincoreの指定方法が不明瞭

  NetType net_type = ConvertNetType(request->GetNetwork());

  // TransactionController作成
  Transaction tx(hex_string);

  response->SetTxid(tx.GetTxid().GetHex());
  response->SetHash(Txid(tx.GetWitnessHash()).GetHex());
  response->SetSize(tx.GetTotalSize());
  response->SetVsize(tx.GetVsize());
  response->SetWeight(tx.GetWeight());
  response->SetVersion(tx.GetVersion());
  response->SetLocktime(tx.GetLockTime());

  // TxInの追加
  for (const TxInReference& tx_in_ref : tx.GetTxInList()) {
    DecodeRawTransactionTxIn res_txin;
    if (tx.IsCoinBase()) {
      res_txin.SetIgnoreItem("txid");
      res_txin.SetIgnoreItem("vout");
      res_txin.SetIgnoreItem("scriptSig");
      res_txin.SetIgnoreItem("txinwitness");

      if (!tx_in_ref.GetUnlockingScript().IsEmpty()) {
        res_txin.SetCoinbase(tx_in_ref.GetUnlockingScript().GetHex());
      }
    } else {
      res_txin.SetIgnoreItem("coinbase");

      res_txin.SetTxid(tx_in_ref.GetTxid().GetHex());
      res_txin.SetVout(tx_in_ref.GetVout());
      if (!tx_in_ref.GetUnlockingScript().IsEmpty()) {
        res_txin.GetScriptSig().SetAsm(tx_in_ref.GetUnlockingScript().ToString());
        res_txin.GetScriptSig().SetHex(tx_in_ref.GetUnlockingScript().GetHex());
      }
      for (const ByteData& witness :
           tx_in_ref.GetScriptWitness().GetWitness()) {  // NOLINT
        res_txin.GetTxinwitness().push_back(witness.GetHex());
      }
      if (res_txin.GetTxinwitness().empty()) {
        // txinwitnessを除外
        res_txin.SetIgnoreItem("txinwitness");
      }
    }
    res_txin.SetSequence(tx_in_ref.GetSequence());
    response->GetVin().push_back(res_txin);
  }

  // TxOutの追加
  int32_t txout_count = 0;
  for (const TxOutReference& txout_ref : tx.GetTxOutList()) {
    DecodeRawTransactionTxOut res_txout;
    res_txout.SetValue(txout_ref.GetValue().GetSatoshiValue());
    res_txout.SetN(txout_count);

    DecodeLockingScript script_pub_key_res;
    std::vector<std::string> addresses;
    Script locking_script = txout_ref.GetLockingScript();
    std::vector<ScriptElement> script_element =
        locking_script.GetElementList();
    res_txout.GetScriptPubKey().SetHex(locking_script.GetHex());
    res_txout.GetScriptPubKey().SetAsm(locking_script.ToString());

    ExtractScriptData extract_data =
        TransactionJsonApi::ExtractLockingScript(locking_script);
    LockingScriptType type = extract_data.script_type;
    script_pub_key_res.SetType("nonstandard");
    if (type != LockingScriptType::kFee) {
      script_pub_key_res.SetType(
          TransactionJsonApi::ConvertLockingScriptTypeString(type));
    }
    script_pub_key_res.SetReqSigs(extract_data.pushed_datas.size());

    AddressFactory addr_factory(net_type);
    Address address;
    if (type == LockingScriptType::kMultisig) {
      script_pub_key_res.SetReqSigs(extract_data.req_sigs);
      for (ByteData pubkey_bytes : extract_data.pushed_datas) {
        Pubkey pubkey = Pubkey(pubkey_bytes);
        address = addr_factory.CreateP2pkhAddress(pubkey);
        script_pub_key_res.GetAddresses().push_back(address.GetAddress());
      }
    } else if (type == LockingScriptType::kPayToPubkey) {
      Pubkey pubkey = Pubkey(extract_data.pushed_datas[0]);
      address = addr_factory.CreateP2pkhAddress(pubkey);
      script_pub_key_res.GetAddresses().push_back(address.GetAddress());
    } else if (type == LockingScriptType::kPayToPubkeyHash) {
      ByteData160 hash =
          ByteData160(extract_data.pushed_datas[0].GetBytes());
      address = addr_factory.GetAddressByHash(
          AddressType::kP2pkhAddress, hash);
      script_pub_key_res.GetAddresses().push_back(address.GetAddress());
    } else if (type == LockingScriptType::kPayToScriptHash) {
      ByteData160 hash =
          ByteData160(extract_data.pushed_datas[0].GetBytes());
      address = addr_factory.GetAddressByHash(
          AddressType::kP2shAddress, hash);
      script_pub_key_res.GetAddresses().push_back(address.GetAddress());
    } else if (type == LockingScriptType::kWitnessV0KeyHash) {
      address =
          addr_factory.GetSegwitAddressByHash(extract_data.pushed_datas[0]);
      script_pub_key_res.GetAddresses().push_back(address.GetAddress());
    } else if (type == LockingScriptType::kWitnessV0ScriptHash) {
      address =
          addr_factory.GetSegwitAddressByHash(extract_data.pushed_datas[0]);
      script_pub_key_res.GetAddresses().push_back(address.GetAddress());
    } else {
      script_pub_key_res.SetIgnoreItem("reqSigs");
      script_pub_key_res.SetIgnoreItem("addresses");
    }
    res_txout.SetScriptPubKey(script_pub_key_res);

    response->GetVout().push_back(res_txout);
    ++txout_count;
  }
}

ByteData TransactionJsonApi::ConvertSignDataToSignature(
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
    SigHashType sighashtype = TransactionJsonApi::ConvertSigHashType(
        sighash_type, sighash_anyone_can_pay);
    byte_data = CryptoUtil::ConvertSignatureToDer(hex_string, sighashtype);
  } else if (hex_string.empty()) {
    // do nothing
  } else {
    byte_data = ByteData(hex_string);
  }
  return byte_data;
}

SigHashType TransactionJsonApi::ConvertSigHashType(
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

ExtractScriptData TransactionJsonApi::ExtractLockingScript(
    Script locking_script) {
  ExtractScriptData extract_data;

  std::string script_type;
  std::vector<ScriptElement> elems = locking_script.GetElementList();
  // FIXME(fujita-cg): anyonecanspent_aremineフラグの判定
  if (elems.size() == 1 && elems[0].GetOpCode() == ScriptOperator::OP_TRUE) {
    extract_data.script_type = LockingScriptType::kTrue;
    return extract_data;
  }
#ifndef CFD_DISABLE_ELEMENTS
  if (locking_script.IsEmpty()) {
    // empty
    extract_data.script_type = LockingScriptType::kFee;
    return extract_data;
  }
#endif  // CFD_DISABLE_ELEMENTS
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

std::string TransactionJsonApi::ConvertLockingScriptTypeString(
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
    default:
      break;
  }
  return "";
}

NetType TransactionJsonApi::ConvertNetType(const std::string& network_type) {
  NetType net_type;
  if (network_type == "mainnet") {
    net_type = NetType::kMainnet;
  } else if (network_type == "testnet") {
    net_type = NetType::kTestnet;
  } else if (network_type == "regtest") {
    net_type = NetType::kRegtest;
  } else {
    warn(
        CFD_LOG_SOURCE,
        "Failed to ConvertNetType. Invalid network_type passed:  "
        "network_type={}",  // NOLINT
        network_type);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid network_type passed. network_type must be \"mainnet\""
        " or \"testnet\" or \"regtest\".");
  }
  return net_type;
}

}  // namespace json
}  // namespace api
}  // namespace cfd
