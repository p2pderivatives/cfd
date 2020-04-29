// Copyright 2020 CryptoGarage
/**
 * @file cfdjs_elements_transaction.cpp
 *
 * @brief cfd-apiで利用するConfidential Transaction操作の実装ファイル
 */
#ifndef CFD_DISABLE_ELEMENTS
#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "jsonapi/autogen/cfd_api_json_autogen.h"  // NOLINT
#include "jsonapi/cfd_json_elements_transaction.h"
#include "jsonapi/cfd_json_transaction.h"

namespace cfd {
namespace api {
namespace json {

using cfd::ElementsAddressFactory;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlindFactor;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxIn;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOut;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsAddressType;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::ElementsNetType;
using cfd::core::HashType;
using cfd::core::IssuanceParameter;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::RangeProofInfo;
using cfd::core::Script;
using cfd::core::ScriptElement;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::logger::info;
using cfd::core::logger::warn;

/**
 * @brief Issuance領域を表現する構造体
 */
struct Issuance {
  ByteData256 asset_blinding_nonce_;  //!< nonce value for blinding asset
  ByteData256 asset_entropy_;         //!< entropy for calculate asset/token
  ConfidentialValue amount_;          //!< asset value
  ConfidentialValue inflation_keys_;  //!< reissuance token value

  /**
   * @brief constructor
   * @param[in] asset_blinding_nonce nonce for blinding
   * @param[in] asset_entropy entropy for calculate asset
   * @param[in] amount asset value (or commitment)
   * @param[in] inflation_keys reissuance token value (or commitment)
   */
  Issuance(
      const ByteData256& asset_blinding_nonce,
      const ByteData256& asset_entropy, const ConfidentialValue& amount,
      const ConfidentialValue& inflation_keys)
      : asset_blinding_nonce_(asset_blinding_nonce),
        asset_entropy_(asset_entropy),
        amount_(amount),
        inflation_keys_(inflation_keys) {
    // do nothing
  }

  /**
   * @brief IssuanceがNullであるかどうかの判定を行う.
   * @retval true issuance is null
   * @retval false issuance is not null
   * @details assetのamountとtokenのamountが設定されているかを判定している.
   */
  bool isNull() const {
    return (amount_.IsEmpty() && inflation_keys_.IsEmpty());
  }
};

// -----------------------------------------------------------------------------
// ElementsTransactionJsonApiクラス
// -----------------------------------------------------------------------------
void ElementsTransactionJsonApi::DecodeRawTransaction(  // NOLINT
    ElementsDecodeRawTransactionRequest* request,
    ElementsDecodeRawTransactionResponse* response) {
  if (request == nullptr) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "request is null.");
  }
  if (response == nullptr) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "response is null.");
  }
  // validate input hex
  const std::string& hex_string = request->GetHex();
  if (hex_string.empty()) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to ElementsDecodeRawTransactionRequest. empty hex.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid hex string. empty data.");
  }
  // FIXME(fujita-cg): 引数のiswitness未使用。elementsでの利用シーンが不明瞭

  // Decode transaction hex
  ConfidentialTransaction ctx(hex_string);

  response->SetTxid(ctx.GetTxid().GetHex());
  response->SetHash(Txid(ctx.GetWitnessHash()).GetHex());
  response->SetWtxid(Txid(ctx.GetWitnessHash()).GetHex());
  response->SetWithash(Txid(ctx.GetWitnessOnlyHash()).GetHex());
  response->SetSize(ctx.GetTotalSize());
  response->SetVsize(ctx.GetVsize());
  response->SetWeight(ctx.GetWeight());
  response->SetVersion(ctx.GetVersion());
  response->SetLocktime(ctx.GetLockTime());

  // TxInの追加
  for (const ConfidentialTxInReference& tx_in_ref : ctx.GetTxInList()) {
    ElementsDecodeRawTransactionTxIn tx_in_res;
    if (ctx.IsCoinBase()) {
      tx_in_res.SetIgnoreItem("txid");
      tx_in_res.SetIgnoreItem("vout");
      tx_in_res.SetIgnoreItem("scriptSig");
      tx_in_res.SetIgnoreItem("is_pegin");

      if (!tx_in_ref.GetUnlockingScript().IsEmpty()) {
        tx_in_res.SetCoinbase(tx_in_ref.GetUnlockingScript().GetHex());
      }
    } else {
      tx_in_res.SetIgnoreItem("coinbase");

      // FIXME(fujita-cg): Elemnets Specific Valueまでは共通化ができるはず
      tx_in_res.SetTxid(tx_in_ref.GetTxid().GetHex());
      tx_in_res.SetVout(tx_in_ref.GetVout());
      if (!tx_in_ref.GetUnlockingScript().IsEmpty()) {
        tx_in_res.GetScriptSig().SetAsm(
            tx_in_ref.GetUnlockingScript().ToString());
        tx_in_res.GetScriptSig().SetHex(tx_in_ref.GetUnlockingScript().GetHex());
      }
      tx_in_res.SetIs_pegin((tx_in_ref.GetPeginWitnessStackNum() > 0));
    }

    tx_in_res.SetSequence(tx_in_ref.GetSequence());

    for (const ByteData& witness :
         tx_in_ref.GetScriptWitness().GetWitness()) {  // NOLINT
      tx_in_res.GetTxinwitness().push_back(witness.GetHex());
    }
    if (tx_in_res.GetTxinwitness().empty()) {
      // txinwitnessを除外
      tx_in_res.SetIgnoreItem("txinwitness");
    }

    // Elememts specific values
    // peg-in witness
    for (const ByteData& pegin_witness_item :
         tx_in_ref.GetPeginWitness().GetWitness()) {
      tx_in_res.GetPegin_witness().push_back(pegin_witness_item.GetHex());
    }
    if (tx_in_res.GetPegin_witness().empty()) {
      // pegin_witnessを除外
      tx_in_res.SetIgnoreItem("pegin_witness");
    }

    // issuance
    Issuance issuance(
        tx_in_ref.GetBlindingNonce(), tx_in_ref.GetAssetEntropy(),
        tx_in_ref.GetIssuanceAmount(), tx_in_ref.GetInflationKeys());
    if (!issuance.isNull()) {
      tx_in_res.GetIssuance().SetAssetBlindingNonce(
          BlindFactor(issuance.asset_blinding_nonce_).GetHex());

      BlindFactor asset_entropy;
      bool is_blind = issuance.amount_.HasBlinding();
      IssuanceParameter param;
      if (issuance.asset_blinding_nonce_.Equals(ByteData256())) {
        // asset entropy
        asset_entropy = ConfidentialTransaction::CalculateAssetEntropy(
            tx_in_ref.GetTxid(), tx_in_ref.GetVout(),
            issuance.asset_entropy_);
        tx_in_res.GetIssuance().SetAssetEntropy(asset_entropy.GetHex());
        tx_in_res.GetIssuance().SetIsreissuance(false);
        tx_in_res.GetIssuance().SetContractHash(
            BlindFactor(tx_in_ref.GetAssetEntropy()).GetHex());
        // token
        ConfidentialAssetId token =
            ConfidentialTransaction::CalculateReissuanceToken(
                asset_entropy, is_blind);
        tx_in_res.GetIssuance().SetToken(token.GetHex());
      } else {
        asset_entropy = BlindFactor(issuance.asset_entropy_);
        tx_in_res.GetIssuance().SetAssetEntropy(asset_entropy.GetHex());
        tx_in_res.GetIssuance().SetIsreissuance(true);
        tx_in_res.GetIssuance().SetIgnoreItem("token");
        tx_in_res.GetIssuance().SetIgnoreItem("contractHash");
      }
      // asset
      ConfidentialAssetId asset =
          ConfidentialTransaction::CalculateAsset(asset_entropy);
      tx_in_res.GetIssuance().SetAsset(asset.GetHex());

      const ConfidentialValue asset_amount = issuance.amount_;
      if (!asset_amount.IsEmpty()) {
        if (asset_amount.HasBlinding()) {
          tx_in_res.GetIssuance().SetAssetamountcommitment(asset_amount.GetHex());
          tx_in_res.GetIssuance().SetIgnoreItem("assetamount");
          if (request->GetFullDump()) {
            tx_in_res.GetIssuance().SetAssetRangeproof(
                tx_in_ref.GetIssuanceAmountRangeproof().GetHex());
          } else {
            tx_in_res.GetIssuance().SetIgnoreItem("assetRangeproof");
          }
        } else {
          tx_in_res.GetIssuance().SetAssetamount(
              asset_amount.GetAmount().GetSatoshiValue());
          tx_in_res.GetIssuance().SetIgnoreItem("assetamountcommitment");
          tx_in_res.GetIssuance().SetIgnoreItem("assetRangeproof");
        }
      } else {
        tx_in_res.GetIssuance().SetIgnoreItem("assetamount");
        tx_in_res.GetIssuance().SetIgnoreItem("assetamountcommitment");
        tx_in_res.GetIssuance().SetIgnoreItem("assetRangeproof");
      }

      const ConfidentialValue inflation_keys = issuance.inflation_keys_;
      if (!inflation_keys.IsEmpty()) {
        if (inflation_keys.HasBlinding()) {
          tx_in_res.GetIssuance().SetTokenamountcommitment(inflation_keys.GetHex());
          tx_in_res.GetIssuance().SetIgnoreItem("tokenamount");
          if (request->GetFullDump()) {
            tx_in_res.GetIssuance().SetTokenRangeproof(
                tx_in_ref.GetInflationKeysRangeproof().GetHex());
          } else {
            tx_in_res.GetIssuance().SetIgnoreItem("tokenRangeproof");
          }
        } else {
          tx_in_res.GetIssuance().SetTokenamount(
              inflation_keys.GetAmount().GetSatoshiValue());
          tx_in_res.GetIssuance().SetIgnoreItem("tokenamountcommitment");
          tx_in_res.GetIssuance().SetIgnoreItem("tokenRangeproof");
        }
      } else {
        tx_in_res.GetIssuance().SetIgnoreItem("tokenamount");
        tx_in_res.GetIssuance().SetIgnoreItem("tokenamountcommitment");
        tx_in_res.GetIssuance().SetIgnoreItem("tokenRangeproof");
      }
    } else {
      // issuanceを除外
      tx_in_res.SetIgnoreItem("issuance");
    }
    // End Elements specific values

    response->GetVin().push_back(tx_in_res);
  }

  // TxOut
  int32_t txout_count = 0;
  for (const ConfidentialTxOutReference& tx_out_ref : ctx.GetTxOutList()) {
    ElementsDecodeRawTransactionTxOut tx_out_res;
    const ConfidentialValue tx_out_value = tx_out_ref.GetConfidentialValue();
    if (!tx_out_value.HasBlinding()) {
      tx_out_res.SetValue(tx_out_value.GetAmount().GetSatoshiValue());
      tx_out_res.SetIgnoreItem("value-minimum");
      tx_out_res.SetIgnoreItem("value-maximum");
      tx_out_res.SetIgnoreItem("ct-exponent");
      tx_out_res.SetIgnoreItem("ct-bits");
      tx_out_res.SetIgnoreItem("surjectionproof");
      tx_out_res.SetIgnoreItem("rangeproof");
      tx_out_res.SetIgnoreItem("valuecommitment");
    } else {
      const ByteData& range_proof = tx_out_ref.GetRangeProof();
      if (range_proof.GetDataSize()) {
        const RangeProofInfo& range_proof_info =
            ConfidentialTxOut::DecodeRangeProofInfo(range_proof);
        tx_out_res.SetValue_minimum(
            Amount(static_cast<int64_t>(range_proof_info.min_value))
                .GetSatoshiValue());
        tx_out_res.SetValue_maximum(
            Amount(static_cast<int64_t>(range_proof_info.max_value), true)
                .GetSatoshiValue());
        tx_out_res.SetCt_exponent(range_proof_info.exponent);
        tx_out_res.SetCt_bits(range_proof_info.mantissa);
      } else {
        tx_out_res.SetIgnoreItem("value-minimum");
        tx_out_res.SetIgnoreItem("value-maximum");
        tx_out_res.SetIgnoreItem("ct-exponent");
        tx_out_res.SetIgnoreItem("ct-bits");
      }

      const ByteData& surjection_proof = tx_out_ref.GetSurjectionProof();
      if (surjection_proof.GetDataSize()) {
        tx_out_res.SetSurjectionproof(surjection_proof.GetHex());
      } else {
        tx_out_res.SetIgnoreItem("surjectionproof");
      }
      if (request->GetFullDump()) {
        tx_out_res.SetRangeproof(range_proof.GetHex());
      } else {
        tx_out_res.SetIgnoreItem("rangeproof");
      }

      tx_out_res.SetValuecommitment(tx_out_value.GetHex());
      tx_out_res.SetIgnoreItem("value");
    }

    const ConfidentialAssetId asset = tx_out_ref.GetAsset();
    if (!asset.HasBlinding()) {
      tx_out_res.SetAsset(asset.GetHex());
      tx_out_res.SetIgnoreItem("assetcommitment");
    } else {
      tx_out_res.SetAssetcommitment(asset.GetHex());
      tx_out_res.SetIgnoreItem("asset");
    }
    ConfidentialNonce nonce = tx_out_ref.GetNonce();
    tx_out_res.SetCommitmentnonce(nonce.GetHex());
    tx_out_res.SetCommitmentnonce_fully_valid(
        Pubkey::IsValid(nonce.GetData()));
    tx_out_res.SetN(txout_count);

    // Parse unlocking script
    ElementsDecodeLockingScript script_pub_key_res;
    Script locking_script = tx_out_ref.GetLockingScript();
    script_pub_key_res.SetAsm(locking_script.ToString());
    script_pub_key_res.SetHex(locking_script.GetHex());

    ExtractScriptData extract_data =
        TransactionJsonApi::ExtractLockingScript(locking_script);
    LockingScriptType type = extract_data.script_type;
    script_pub_key_res.SetType(
        TransactionJsonApi::ConvertLockingScriptTypeString(type));
    script_pub_key_res.SetReqSigs(
        static_cast<int>(extract_data.pushed_datas.size()));

    ElementsNetType elements_net_type = ConvertElementsNetType(request->GetNetwork());
    ElementsAddressFactory addr_factory(elements_net_type);
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
          ElementsAddressType::kP2pkhAddress, hash);
      script_pub_key_res.GetAddresses().push_back(address.GetAddress());
    } else if (type == LockingScriptType::kPayToScriptHash) {
      ByteData160 hash =
          ByteData160(extract_data.pushed_datas[0].GetBytes());
      address = addr_factory.GetAddressByHash(
          ElementsAddressType::kP2shAddress, hash);
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

    // parse pegout locking script
    if (locking_script.IsPegoutScript()) {
      std::vector<ScriptElement> elems = locking_script.GetElementList();
      // pegout chain はリバースバイト表示
      std::vector<uint8_t> pegout_chain_bytes =
          elems[1].GetBinaryData().GetBytes();
      std::reverse(pegout_chain_bytes.begin(), pegout_chain_bytes.end());
      script_pub_key_res.SetPegout_chain(
          ByteData256(pegout_chain_bytes).GetHex());
      Script pegout_locking_script = Script(elems[2].GetBinaryData());
      script_pub_key_res.SetPegout_asm(pegout_locking_script.ToString());
      script_pub_key_res.SetPegout_hex(pegout_locking_script.GetHex());

      ExtractScriptData pegout_extract_data =
          TransactionJsonApi::ExtractLockingScript(
              pegout_locking_script);
      LockingScriptType pegout_type = pegout_extract_data.script_type;
      script_pub_key_res.SetPegout_type(
          TransactionJsonApi::ConvertLockingScriptTypeString(
              pegout_type));
      script_pub_key_res.SetPegout_reqSigs(
          static_cast<int>(pegout_extract_data.pushed_datas.size()));

      const NetType net_type = TransactionJsonApi::ConvertNetType(request->GetMainchainNetwork());
      AddressFactory btcaddr_factory(net_type);
      if (pegout_type == LockingScriptType::kMultisig) {
        script_pub_key_res.SetPegout_reqSigs(pegout_extract_data.req_sigs);
        for (ByteData pubkey_bytes : pegout_extract_data.pushed_datas) {
          Pubkey pubkey = Pubkey(pubkey_bytes);
          address = btcaddr_factory.CreateP2pkhAddress(pubkey);
          script_pub_key_res.GetPegout_addresses().push_back(address.GetAddress());
        }
      } else if (pegout_type == LockingScriptType::kPayToPubkey) {
        Pubkey pubkey = Pubkey(pegout_extract_data.pushed_datas[0]);
        address = btcaddr_factory.CreateP2pkhAddress(pubkey);
        script_pub_key_res.GetPegout_addresses().push_back(address.GetAddress());
      } else if (pegout_type == LockingScriptType::kPayToPubkeyHash) {
        ByteData160 hash =
            ByteData160(pegout_extract_data.pushed_datas[0].GetBytes());
        address = btcaddr_factory.GetAddressByHash(
            AddressType::kP2pkhAddress, hash);
        script_pub_key_res.GetPegout_addresses().push_back(address.GetAddress());
      } else if (pegout_type == LockingScriptType::kPayToScriptHash) {
        ByteData160 hash =
            ByteData160(pegout_extract_data.pushed_datas[0].GetBytes());
        address = btcaddr_factory.GetAddressByHash(
            AddressType::kP2shAddress, hash);
        script_pub_key_res.GetPegout_addresses().push_back(address.GetAddress());
      } else if (pegout_type == LockingScriptType::kWitnessV0KeyHash) {
        address = btcaddr_factory.GetSegwitAddressByHash(
            pegout_extract_data.pushed_datas[0]);
        script_pub_key_res.GetPegout_addresses().push_back(address.GetAddress());
      } else if (pegout_type == LockingScriptType::kWitnessV0ScriptHash) {
        address = btcaddr_factory.GetSegwitAddressByHash(
            pegout_extract_data.pushed_datas[0]);
        script_pub_key_res.GetPegout_addresses().push_back(address.GetAddress());
      } else {
        script_pub_key_res.SetIgnoreItem("pegout_reqSigs");
        script_pub_key_res.SetIgnoreItem("pegout_addresses");
      }
    } else {
      script_pub_key_res.SetIgnoreItem("pegout_chain");
      script_pub_key_res.SetIgnoreItem("pegout_asm");
      script_pub_key_res.SetIgnoreItem("pegout_hex");
      script_pub_key_res.SetIgnoreItem("pegout_reqSigs");
      script_pub_key_res.SetIgnoreItem("pegout_type");
      script_pub_key_res.SetIgnoreItem("pegout_addresses");
    }

    tx_out_res.SetScriptPubKey(script_pub_key_res);
    response->GetVout().push_back(tx_out_res);
    ++txout_count;
  }
}

ElementsNetType ElementsTransactionJsonApi::ConvertElementsNetType(
    const std::string& elements_net_type) {
  ElementsNetType net_type;
  if (elements_net_type == "liquidv1") {
    net_type = ElementsNetType::kLiquidV1;
  } else if ((elements_net_type == "elementsregtest") ||
      (elements_net_type == "liquidregtest") ||
      (elements_net_type == "regtest")) {
    net_type = ElementsNetType::kElementsRegtest;
  } else {
    warn(
        CFD_LOG_SOURCE,
        "Failed to ConvertElementsNetType. Invalid elements_network_type "
        "passed:  elements_network_type={}",  // NOLINT
        elements_net_type);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid elements_network_type passed. elements_network_type must be "
        "\"liquidv1\" or \"regtest\".");  // NOLINT
  }
  return net_type;
}

}  // namespace json
}  // namespace api
}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
