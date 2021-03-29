// Copyright 2021 CryptoGarage
/**
 * @file cfd_json_psbt.cpp
 *
 * @brief This file is defines Partially Signed Bitcoin Transaction by json.
 */
#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfd_psbt.h"
#include "cfdcore/cfdcore_psbt.h"
#include "cfdcore/cfdcore_util.h"
#include "jsonapi/autogen/cfd_api_json_autogen.h"  // NOLINT
#include "jsonapi/cfd_json_psbt.h"
#include "jsonapi/cfd_json_transaction.h"

namespace cfd {
namespace api {
namespace json {

using cfd::AddressFactory;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::HashType;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Psbt;
using cfd::core::Script;
using cfd::core::ScriptElement;
using cfd::core::ScriptUtil;
using cfd::core::SigHashType;
using cfd::core::SigHashAlgorithm;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::logger::info;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// PsbtJsonApiクラス
// -----------------------------------------------------------------------------
void PsbtJsonApi::DecodePsbt(
    DecodePsbtRequest* request, DecodePsbtResponse* response) {
  if (request == nullptr) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "request is null.");
  }
  if (response == nullptr) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "response is null.");
  }
  // validate input hex
  const std::string& hex_string = request->GetPsbt();
  if (hex_string.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to DecodePsbt. empty psbt.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid hex string. empty data.");
  }

  auto net_type = TransactionJsonApi::ConvertNetType(request->GetNetwork());
  AddressFactory addr_factory(net_type);

  // Decode transaction hex
  Psbt psbt(hex_string);
  Transaction tx = psbt.GetTransaction();

  DecodeRawTransactionRequest tx_req;
  DecodeRawTransactionResponse tx_res;
  PsbtMapData item;

  // global
  auto tx_hex = tx.GetHex();
  tx_req.SetHex(tx_hex);
  tx_req.SetNetwork(request->GetNetwork());
  TransactionJsonApi::DecodeRawTransaction(&tx_req, &tx_res);
  response->SetTx(tx_res);
  if (request->GetHasDetail()) {
    response->SetTx_hex(tx_hex);
    if (request->GetHasSimple()) response->SetIgnoreItem("tx");
  } else {
    response->SetIgnoreItem("tx_hex");
  }

  auto key_list = psbt.GetGlobalRecordKeyList();
    if (request->GetHasDetail()) {
      response->SetVersion(psbt.GetPsbtVersion());
      auto xpub_list = psbt.GetGlobalXpubkeyDataList();
      auto& xpubs = response->GetXpubs();
      for (const auto& xpub : xpub_list) {
        const auto& extkey = xpub.GetExtPubkey();
        PsbtGlobalXpub xpub_obj;
        auto& xpub_data = xpub_obj.GetXpub();
        xpub_data.SetBase58(extkey.ToString());
        xpub_data.SetHex(extkey.GetData().GetHex());
        xpub_obj.SetPath(xpub.GetBip32Path());
        xpub_obj.SetMaster_fingerprint(xpub.GetFingerprint().GetHex());
        xpub_obj.SetDescriptorXpub(xpub.ToString());
        xpubs.emplace_back(xpub_obj);
      }
      if (xpubs.empty()) response->SetIgnoreItem("xpubs");
    } else {
      response->SetIgnoreItem("version");
      response->SetIgnoreItem("xpubs");
      if (psbt.GetPsbtVersion() > 0) key_list.push_back(ByteData("fb"));
    }

  if (psbt.GetPsbtVersion() > 0) {
    key_list.push_back(ByteData("fb"));
  }
  auto& unknown_list = response->GetUnknown();
  for (const auto& key : key_list) {
      if (request->GetHasDetail() && (key.GetHeadData() == Psbt::kPsbtGlobalXpub)) {
        continue;
      }
    auto data = psbt.GetGlobalRecord(key);
    item.SetKey(key.GetHex());
    item.SetValue(data.GetHex());
    unknown_list.emplace_back(item);
  }
  if (request->GetHasSimple() && unknown_list.empty()) {
    response->SetIgnoreItem("unknown");
  }

  Amount total_input;
  bool is_unset_utxo = false;
  uint32_t max = static_cast<uint32_t>(tx.GetTxInCount());
  for (uint32_t index=0; index<max; ++index) {
    DecodePsbtInput input;
    auto tx_input = tx.GetTxIn(index);
    bool has_amount = false;
      bool is_witness = false;

    auto utxo = psbt.GetTxInUtxo(index, true, &is_witness);
    if (utxo.GetLockingScript().IsEmpty() || (!is_witness)) {
      input.SetIgnoreItem("witness_utxo");
    } else {
      has_amount = true;
      total_input += utxo.GetValue();
      input.GetWitness_utxo().SetAmount(utxo.GetValue().GetSatoshiValue());
      auto& script = input.GetWitness_utxo().GetScriptPubKey();

      std::string script_type;
      auto addr_list = TransactionJsonApi::ConvertFromLockingScript(
          addr_factory, utxo.GetLockingScript(), &script_type, nullptr);
      if (!addr_list.empty()) {
        script.SetAddress(addr_list[0].GetAddress());
      }
      script.SetHex(utxo.GetLockingScript().GetHex());
      script.SetAsm(utxo.GetLockingScript().ToString());
      script.SetType(script_type);
    }

    auto full_utxo = psbt.GetTxInUtxoFull(index, true);
    if (!full_utxo.GetTxid().Equals(tx_input.GetTxid())) {
      input.SetIgnoreItem("non_witness_utxo");
      input.SetIgnoreItem("non_witness_utxo_hex");
      if (!has_amount) is_unset_utxo = true;
    } else {
      auto utxo_tx_hex = full_utxo.GetHex();
      tx_req.SetHex(utxo_tx_hex);
      tx_req.SetNetwork(request->GetNetwork());
      TransactionJsonApi::DecodeRawTransaction(&tx_req, &tx_res);
      input.SetNon_witness_utxo(tx_res);

      if (request->GetHasDetail()) {
        input.SetNon_witness_utxo_hex(utxo_tx_hex);
        if (utxo_tx_hex.empty()) {
          input.SetIgnoreItem("non_witness_utxo_hex");
        }
        if (request->GetHasSimple()) {
          input.SetIgnoreItem("non_witness_utxo");
        }
      } else {
        input.SetIgnoreItem("non_witness_utxo_hex");
      }
      if (has_amount) {
        // do nothing
      } else if (full_utxo.GetTxOutCount() > tx_input.GetVout()) {
        auto txout = full_utxo.GetTxOut(tx_input.GetVout());
        total_input += txout.GetValue();
        has_amount = true;
      } else {
        input.SetIgnoreItem("non_witness_utxo");
        is_unset_utxo = true;
      }
    }

    auto sig_pubkey_list = psbt.GetTxInSignaturePubkeyList(index);
    if (sig_pubkey_list.empty()) {
      input.SetIgnoreItem("partial_signatures");
    } else {
      auto& sig_list = input.GetPartial_signatures();
      for (auto pubkey : sig_pubkey_list) {
        auto sig = psbt.GetTxInSignature(index, pubkey);
        PsbtSignatureData sig_data;
        sig_data.SetPubkey(pubkey.GetHex());
        sig_data.SetSignature(sig.GetHex());
        sig_list.emplace_back(sig_data);
      }
    }

    if (!psbt.IsFindTxInSighashType(index)) {
      input.SetIgnoreItem("sighash");
    } else {
      auto sighashtype = psbt.GetTxInSighashType(index);
      input.SetSighash(sighashtype.ToString());
    }

    Script redeem_script = psbt.GetTxInRedeemScriptDirect(index, true, false);
    if (redeem_script.IsEmpty()) {
      input.SetIgnoreItem("redeem_script");
    } else {
      std::string script_type;
      auto addr_list = TransactionJsonApi::ConvertFromLockingScript(
          addr_factory, redeem_script, &script_type, nullptr);
      input.GetRedeem_script().SetHex(redeem_script.GetHex());
      input.GetRedeem_script().SetAsm(redeem_script.ToString());
      input.GetRedeem_script().SetType(script_type);
    }

    Script witness_script = psbt.GetTxInRedeemScriptDirect(index, true, true);
    if (witness_script.IsEmpty()) {
      input.SetIgnoreItem("witness_script");
    } else {
      std::string script_type;
      auto addr_list = TransactionJsonApi::ConvertFromLockingScript(
          addr_factory, witness_script, &script_type, nullptr);
      input.GetWitness_script().SetHex(witness_script.GetHex());
      input.GetWitness_script().SetAsm(witness_script.ToString());
      input.GetWitness_script().SetType(script_type);
    }

    auto bip32_pubkey_list = psbt.GetTxInKeyDataList(index);
    if (bip32_pubkey_list.empty()) {
      input.SetIgnoreItem("bip32_derivs");
    } else {
      auto& bip32_list = input.GetBip32_derivs();
      for (auto key_data : bip32_pubkey_list) {
        PsbtBip32Data bip32_data;
        bip32_data.SetPubkey(key_data.GetPubkey().GetHex());
        bip32_data.SetMaster_fingerprint(key_data.GetFingerprint().GetHex());
        bip32_data.SetPath(key_data.GetBip32Path());
        
          if (request->GetHasDetail()) {
            bip32_data.SetDescriptor(key_data.ToString());
          } else {
            bip32_data.SetIgnoreItem("descriptor");
          }
        bip32_list.emplace_back(bip32_data);
      }
    }

    auto scriptsig_arr = psbt.GetTxInFinalScript(index, false);
    if ((!scriptsig_arr.empty()) && (!scriptsig_arr[0].IsEmpty())) {
      input.GetFinal_scriptsig().SetHex(scriptsig_arr[0].GetHex());
      input.GetFinal_scriptsig().SetAsm(Script(scriptsig_arr[0]).ToString());
    } else {
      input.SetIgnoreItem("final_scriptsig");
    }

    auto witness_stack = psbt.GetTxInFinalScript(index, true);
    if (witness_stack.empty()) {
      input.SetIgnoreItem("final_scriptwitness");
    } else {
      auto& witness = input.GetFinal_scriptwitness();
      for (const auto& stack : witness_stack) {
        witness.emplace_back(stack.GetHex());
      }
    }

    key_list = psbt.GetTxInRecordKeyList(index);
    for (const auto& key : key_list) {
      auto data = psbt.GetTxInRecord(index, key);
      item.SetKey(key.GetHex());
      item.SetValue(data.GetHex());
      input.GetUnknown().emplace_back(item);
    }
    if (input.GetUnknown().empty()) input.SetIgnoreItem("unknown");

    response->GetInputs().push_back(input);
  }

  Amount total_output;
  max = static_cast<uint32_t>(tx.GetTxOutCount());
  for (uint32_t index=0; index<tx.GetTxOutCount(); ++index) {
    DecodePsbtOutput output;
    const auto& txout = tx.GetTxOut(index);
    total_output += txout.GetValue();
    bool is_witness = false;
    auto script = psbt.GetTxOutScript(index, true, &is_witness);
    if (script.IsEmpty()) {
      output.SetIgnoreItem("redeem_script");
      output.SetIgnoreItem("witness_script");
    } else if (is_witness) {
      output.GetWitness_script().SetHex(script.GetHex());
      output.GetWitness_script().SetAsm(script.ToString());
      if (txout.GetLockingScript().IsP2shScript()) {
        auto sh_script = ScriptUtil::CreateP2wshLockingScript(script);
        output.GetRedeem_script().SetHex(sh_script.GetHex());
        output.GetRedeem_script().SetAsm(sh_script.ToString());
      } else {
        output.SetIgnoreItem("redeem_script");
      }
    } else {
      output.SetIgnoreItem("witness_script");
      output.GetRedeem_script().SetHex(script.GetHex());
      output.GetRedeem_script().SetAsm(script.ToString());
    }

    auto bip32_pubkey_list = psbt.GetTxOutKeyDataList(index);
    if (bip32_pubkey_list.empty()) {
      output.SetIgnoreItem("bip32_derivs");
    } else {
      auto& bip32_list = output.GetBip32_derivs();
      for (auto key_data : bip32_pubkey_list) {
        PsbtBip32Data bip32_data;
        bip32_data.SetPubkey(key_data.GetPubkey().GetHex());
        bip32_data.SetMaster_fingerprint(key_data.GetFingerprint().GetHex());
        bip32_data.SetPath(key_data.GetBip32Path());
        
          if (request->GetHasDetail()) {
            bip32_data.SetDescriptor(key_data.ToString());
          } else {
            bip32_data.SetIgnoreItem("descriptor");
          }
        bip32_list.emplace_back(bip32_data);
      }
    }

    key_list = psbt.GetTxOutRecordKeyList(index);
    for (const auto& key : key_list) {
      auto data = psbt.GetTxOutRecord(index, key);
      item.SetKey(key.GetHex());
      item.SetValue(data.GetHex());
      output.GetUnknown().emplace_back(item);
    }
    if (output.GetUnknown().empty()) output.SetIgnoreItem("unknown");

    response->GetOutputs().push_back(output);
  }

  if (is_unset_utxo || (total_output > total_input)) {
    response->SetIgnoreItem("fee");
  } else {
    response->SetFee((total_input - total_output).GetSatoshiValue());
  }
}

}  // namespace json
}  // namespace api
}  // namespace cfd
