// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_elements_transaction.cpp
 *
 * @brief-eng implementation of Confidential Transaction that uses cfd-api
 * @brief-jp cfd-apiで利用するConfidential Transaction操作の実装ファイル
 */
#ifndef CFD_DISABLE_ELEMENTS
#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_elements_transaction.h"
#include "cfd_manager.h"  // NOLINT
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_util.h"

#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_elements_address.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfd/cfdapi_struct.h"
#include "cfd/cfdapi_transaction.h"
#include "cfd/cfdapi_transaction_base.h"
#include "cfdapi_internal.h"  // NOLINT

namespace cfd {
namespace js {
namespace api {

using cfd::ConfidentialTransactionController;
using cfd::ElementsAddressFactory;
using cfd::api::AddressApi;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlindFactor;
using cfd::core::BlindParameter;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOut;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsAddressType;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::ElementsNetType;
using cfd::core::ExtKey;
using cfd::core::HashUtil;
using cfd::core::IssuanceBlindingKeyPair;
using cfd::core::IssuanceParameter;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::RangeProofInfo;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptElement;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::SigHashType;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::UnblindParameter;
using cfd::core::WitnessVersion;
using cfd::core::logger::info;
using cfd::core::logger::warn;
using cfd::js::api::AddressStructApi;
using cfd::js::api::TransactionStructApi;

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
// ファイル内関数
// -----------------------------------------------------------------------------

/**
 * @brief Create a ConfidentialTransactionController object.
 *
 * @param[in] hex of the transaction for which to create the controller object
 * @return a ConfidentialTransactionController instance
 */
static ConfidentialTransactionController CreateController(
    const std::string& hex) {
  return ConfidentialTransactionController(hex);
}

// -----------------------------------------------------------------------------
// ElementsTransactionStructApiクラス
// -----------------------------------------------------------------------------
ElementsCreateRawTransactionResponseStruct
ElementsTransactionStructApi::CreateRawTransaction(  // NOLINT
    const ElementsCreateRawTransactionRequestStruct& request) {
  auto call_func = [](const ElementsCreateRawTransactionRequestStruct& request)
      -> ElementsCreateRawTransactionResponseStruct {  // NOLINT
    ElementsCreateRawTransactionResponseStruct response;
    // Transaction作成
    ConfidentialTransactionController ctxc(request.version, request.locktime);

    // TxInの追加
    const uint32_t kLockTimeDisabledSequence =
        ctxc.GetLockTimeDisabledSequence();
    for (ElementsTxInRequestStruct txin_req : request.txins) {
      // TxInのunlocking_scriptは空で作成
      if (kLockTimeDisabledSequence == txin_req.sequence) {
        ctxc.AddTxIn(
            Txid(txin_req.txid), txin_req.vout, ctxc.GetDefaultSequence());
      } else {
        ctxc.AddTxIn(Txid(txin_req.txid), txin_req.vout, txin_req.sequence);
      }
    }

    // TxOutの追加
    for (ElementsTxOutRequestStruct txout_req : request.txouts) {
      const std::string addr = txout_req.address;
      if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
        ctxc.AddTxOut(
            ElementsConfidentialAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset), txout_req.is_remove_nonce);
      } else {
        ctxc.AddTxOut(
            ElementsAddressFactory().GetAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset));
      }
    }

    // feeの追加
    ElementsTxOutFeeRequestStruct fee_req = request.fee;
    // amountが0のfeeは無効と判定
    if (fee_req.amount != 0) {
      ctxc.AddTxOutFee(
          Amount::CreateBySatoshiAmount(fee_req.amount),
          ConfidentialAssetId(fee_req.asset));
    }

    response.hex = ctxc.GetHex();
    return response;
  };

  ElementsCreateRawTransactionResponseStruct result;
  result = ExecuteStructApi<
      ElementsCreateRawTransactionRequestStruct,
      ElementsCreateRawTransactionResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

ElementsDecodeRawTransactionResponseStruct
ElementsTransactionStructApi::DecodeRawTransaction(  // NOLINT
    const ElementsDecodeRawTransactionRequestStruct& request) {
  auto call_func = [](const ElementsDecodeRawTransactionRequestStruct& request)
      -> ElementsDecodeRawTransactionResponseStruct {  // NOLINT
    ElementsDecodeRawTransactionResponseStruct response;

    // validate input hex
    const std::string& hex_string = request.hex;
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
    ConfidentialTransactionController ctxc(hex_string);
    const ConfidentialTransaction& ctx = ctxc.GetTransaction();

    response.txid = ctx.GetTxid().GetHex();
    response.hash = Txid(ctx.GetWitnessHash()).GetHex();
    response.wtxid = Txid(ctx.GetWitnessHash()).GetHex();
    response.withash = Txid(ctx.GetWitnessOnlyHash()).GetHex();
    response.version = ctx.GetVersion();
    response.size = ctx.GetTotalSize();
    response.vsize = ctx.GetVsize();
    response.weight = ctx.GetWeight();
    response.locktime = ctx.GetLockTime();

    // TxInの追加
    for (const ConfidentialTxInReference& tx_in_ref : ctx.GetTxInList()) {
      ElementsDecodeRawTransactionTxInStruct tx_in_res;
      if (ctx.IsCoinBase()) {
        tx_in_res.ignore_items.insert("txid");
        tx_in_res.ignore_items.insert("vout");
        tx_in_res.ignore_items.insert("scriptSig");
        tx_in_res.ignore_items.insert("is_pegin");

        if (!tx_in_ref.GetUnlockingScript().IsEmpty()) {
          tx_in_res.coinbase = tx_in_ref.GetUnlockingScript().GetHex();
        }
      } else {
        tx_in_res.ignore_items.insert("coinbase");

        // FIXME(fujita-cg): Elemnets Specific Valueまでは共通化ができるはず
        tx_in_res.txid = tx_in_ref.GetTxid().GetHex();
        tx_in_res.vout = tx_in_ref.GetVout();
        if (!tx_in_ref.GetUnlockingScript().IsEmpty()) {
          tx_in_res.script_sig.asm_ =
              tx_in_ref.GetUnlockingScript().ToString();
          tx_in_res.script_sig.hex = tx_in_ref.GetUnlockingScript().GetHex();
        }
        tx_in_res.is_pegin = (tx_in_ref.GetPeginWitnessStackNum() > 0);
      }

      tx_in_res.sequence = tx_in_ref.GetSequence();

      for (const ByteData& witness :
           tx_in_ref.GetScriptWitness().GetWitness()) {  // NOLINT
        tx_in_res.txinwitness.push_back(witness.GetHex());
      }
      if (tx_in_res.txinwitness.empty()) {
        // txinwitnessを除外
        tx_in_res.ignore_items.insert("txinwitness");
      }

      // Elememts specific values
      // peg-in witness
      for (const ByteData& pegin_witness_item :
           tx_in_ref.GetPeginWitness().GetWitness()) {
        tx_in_res.pegin_witness.push_back(pegin_witness_item.GetHex());
      }
      if (tx_in_res.pegin_witness.empty()) {
        // pegin_witnessを除外
        tx_in_res.ignore_items.insert("pegin_witness");
      }

      // issuance
      Issuance issuance(
          tx_in_ref.GetBlindingNonce(), tx_in_ref.GetAssetEntropy(),
          tx_in_ref.GetIssuanceAmount(), tx_in_ref.GetInflationKeys());
      if (!issuance.isNull()) {
        tx_in_res.issuance.asset_blinding_nonce =
            BlindFactor(issuance.asset_blinding_nonce_).GetHex();

        BlindFactor asset_entropy;
        bool is_blind = issuance.amount_.HasBlinding();
        IssuanceParameter param;
        if (issuance.asset_blinding_nonce_.Equals(ByteData256())) {
          // asset entropy
          asset_entropy = ConfidentialTransaction::CalculateAssetEntropy(
              tx_in_ref.GetTxid(), tx_in_ref.GetVout(),
              issuance.asset_entropy_);
          tx_in_res.issuance.asset_entropy = asset_entropy.GetHex();
          tx_in_res.issuance.isreissuance = false;
          // token
          ConfidentialAssetId token =
              ConfidentialTransaction::CalculateReissuanceToken(
                  asset_entropy, is_blind);
          tx_in_res.issuance.token = token.GetHex();
        } else {
          asset_entropy = BlindFactor(issuance.asset_entropy_);
          tx_in_res.issuance.asset_entropy = asset_entropy.GetHex();
          tx_in_res.issuance.isreissuance = true;
          tx_in_res.issuance.ignore_items.insert("token");
        }
        // asset
        ConfidentialAssetId asset =
            ConfidentialTransaction::CalculateAsset(asset_entropy);
        tx_in_res.issuance.asset = asset.GetHex();

        const ConfidentialValue asset_amount = issuance.amount_;
        if (!asset_amount.IsEmpty()) {
          if (asset_amount.HasBlinding()) {
            tx_in_res.issuance.assetamountcommitment = asset_amount.GetHex();
            tx_in_res.issuance.ignore_items.insert("assetamount");
          } else {
            tx_in_res.issuance.assetamount =
                asset_amount.GetAmount().GetSatoshiValue();
            tx_in_res.issuance.ignore_items.insert("assetamountcommitment");
          }
        } else {
          tx_in_res.issuance.ignore_items.insert("assetamount");
          tx_in_res.issuance.ignore_items.insert("assetamountcommitment");
        }

        const ConfidentialValue inflation_keys = issuance.inflation_keys_;
        if (!inflation_keys.IsEmpty()) {
          if (inflation_keys.HasBlinding()) {
            tx_in_res.issuance.tokenamountcommitment = inflation_keys.GetHex();
            tx_in_res.issuance.ignore_items.insert("tokenamount");
          } else {
            tx_in_res.issuance.tokenamount =
                inflation_keys.GetAmount().GetSatoshiValue();
            tx_in_res.issuance.ignore_items.insert("tokenamountcommitment");
          }
        } else {
          tx_in_res.issuance.ignore_items.insert("tokenamount");
          tx_in_res.issuance.ignore_items.insert("tokenamountcommitment");
        }
      } else {
        // issuanceを除外
        tx_in_res.ignore_items.insert("issuance");
      }
      // End Elements specific values

      response.vin.push_back(tx_in_res);
    }

    // TxOut
    int32_t txout_count = 0;
    for (const ConfidentialTxOutReference& tx_out_ref : ctx.GetTxOutList()) {
      ElementsDecodeRawTransactionTxOutStruct tx_out_res;
      const ConfidentialValue tx_out_value = tx_out_ref.GetConfidentialValue();
      if (!tx_out_value.HasBlinding()) {
        tx_out_res.value = tx_out_value.GetAmount().GetSatoshiValue();
        tx_out_res.ignore_items.insert("value-minimum");
        tx_out_res.ignore_items.insert("value-maximum");
        tx_out_res.ignore_items.insert("ct-exponent");
        tx_out_res.ignore_items.insert("ct-bits");
        tx_out_res.ignore_items.insert("surjectionproof");
        tx_out_res.ignore_items.insert("valuecommitment");
      } else {
        const ByteData& range_proof = tx_out_ref.GetRangeProof();
        if (range_proof.GetDataSize()) {
          const RangeProofInfo& range_proof_info =
              ConfidentialTxOut::DecodeRangeProofInfo(range_proof);
          tx_out_res.value_minimum =
              Amount::CreateBySatoshiAmount(range_proof_info.min_value)
                  .GetSatoshiValue();
          tx_out_res.value_maximum =
              Amount::CreateBySatoshiAmount(range_proof_info.max_value)
                  .GetSatoshiValue();
          tx_out_res.ct_exponent = range_proof_info.exponent;
          tx_out_res.ct_bits = range_proof_info.mantissa;
        } else {
          tx_out_res.ignore_items.insert("value-minimum");
          tx_out_res.ignore_items.insert("value-maximum");
          tx_out_res.ignore_items.insert("ct-exponent");
          tx_out_res.ignore_items.insert("ct-bits");
        }

        const ByteData& surjection_proof = tx_out_ref.GetSurjectionProof();
        if (surjection_proof.GetDataSize()) {
          tx_out_res.surjectionproof = surjection_proof.GetHex();
        } else {
          tx_out_res.ignore_items.insert("surjectionproof");
        }

        tx_out_res.valuecommitment = tx_out_value.GetHex();
        tx_out_res.ignore_items.insert("value");
      }

      const ConfidentialAssetId asset = tx_out_ref.GetAsset();
      if (!asset.HasBlinding()) {
        tx_out_res.asset = asset.GetHex();
        tx_out_res.ignore_items.insert("assetcommitment");
      } else {
        tx_out_res.assetcommitment = asset.GetHex();
        tx_out_res.ignore_items.insert("asset");
      }
      ConfidentialNonce nonce = tx_out_ref.GetNonce();
      tx_out_res.commitmentnonce = nonce.GetHex();
      tx_out_res.commitmentnonce_fully_valid =
          Pubkey::IsValid(nonce.GetData());
      tx_out_res.n = txout_count;

      // Parse unlocking script
      ElementsDecodeLockingScriptStruct script_pub_key_res;
      Script locking_script = tx_out_ref.GetLockingScript();
      script_pub_key_res.asm_ = locking_script.ToString();
      script_pub_key_res.hex = locking_script.GetHex();

      ExtractScriptData extract_data =
          TransactionApiBase::ExtractLockingScript(locking_script);
      LockingScriptType type = extract_data.script_type;
      script_pub_key_res.type =
          TransactionApiBase::ConvertLockingScriptTypeString(type);
      script_pub_key_res.req_sigs = extract_data.pushed_datas.size();

      ElementsNetType elements_net_type =
          ElementsAddressStructApi::ConvertElementsNetType(request.network);
      ElementsAddressFactory addr_factory(elements_net_type);
      Address address;
      if (type == LockingScriptType::kMultisig) {
        script_pub_key_res.req_sigs = extract_data.req_sigs;
        for (ByteData pubkey_bytes : extract_data.pushed_datas) {
          Pubkey pubkey = Pubkey(pubkey_bytes);
          address = addr_factory.CreateP2pkhAddress(pubkey);
          script_pub_key_res.addresses.push_back(address.GetAddress());
        }
      } else if (type == LockingScriptType::kPayToPubkey) {
        Pubkey pubkey = Pubkey(extract_data.pushed_datas[0]);
        address = addr_factory.CreateP2pkhAddress(pubkey);
        script_pub_key_res.addresses.push_back(address.GetAddress());
      } else if (type == LockingScriptType::kPayToPubkeyHash) {
        ByteData160 hash =
            ByteData160(extract_data.pushed_datas[0].GetBytes());
        address = addr_factory.GetAddressByHash(
            ElementsAddressType::kP2pkhAddress, hash);
        script_pub_key_res.addresses.push_back(address.GetAddress());
      } else if (type == LockingScriptType::kPayToScriptHash) {
        ByteData160 hash =
            ByteData160(extract_data.pushed_datas[0].GetBytes());
        address = addr_factory.GetAddressByHash(
            ElementsAddressType::kP2shAddress, hash);
        script_pub_key_res.addresses.push_back(address.GetAddress());
      } else if (type == LockingScriptType::kWitnessV0KeyHash) {
        address =
            addr_factory.GetSegwitAddressByHash(extract_data.pushed_datas[0]);
        script_pub_key_res.addresses.push_back(address.GetAddress());
      } else if (type == LockingScriptType::kWitnessV0ScriptHash) {
        address =
            addr_factory.GetSegwitAddressByHash(extract_data.pushed_datas[0]);
        script_pub_key_res.addresses.push_back(address.GetAddress());
      } else {
        script_pub_key_res.ignore_items.insert("reqSigs");
        script_pub_key_res.ignore_items.insert("addresses");
      }

      // parse pegout locking script
      if (locking_script.IsPegoutScript()) {
        std::vector<ScriptElement> elems = locking_script.GetElementList();
        // pegout chain はリバースバイト表示
        std::vector<uint8_t> pegout_chain_bytes =
            elems[1].GetBinaryData().GetBytes();
        std::reverse(pegout_chain_bytes.begin(), pegout_chain_bytes.end());
        script_pub_key_res.pegout_chain =
            ByteData256(pegout_chain_bytes).GetHex();
        Script pegout_locking_script = Script(elems[2].GetBinaryData());
        script_pub_key_res.pegout_asm = pegout_locking_script.ToString();
        script_pub_key_res.pegout_hex = pegout_locking_script.GetHex();

        ExtractScriptData pegout_extract_data =
            TransactionApiBase::ExtractLockingScript(pegout_locking_script);
        LockingScriptType pegout_type = pegout_extract_data.script_type;
        script_pub_key_res.pegout_type =
            TransactionApiBase::ConvertLockingScriptTypeString(pegout_type);
        script_pub_key_res.pegout_req_sigs =
            pegout_extract_data.pushed_datas.size();

        const NetType net_type =
            AddressStructApi::ConvertNetType(request.mainchain_network);
        AddressFactory btcaddr_factory(net_type);
        if (pegout_type == LockingScriptType::kMultisig) {
          script_pub_key_res.pegout_req_sigs = pegout_extract_data.req_sigs;
          for (ByteData pubkey_bytes : pegout_extract_data.pushed_datas) {
            Pubkey pubkey = Pubkey(pubkey_bytes);
            address = btcaddr_factory.CreateP2pkhAddress(pubkey);
            script_pub_key_res.addresses.push_back(address.GetAddress());
          }
        } else if (pegout_type == LockingScriptType::kPayToPubkey) {
          Pubkey pubkey = Pubkey(pegout_extract_data.pushed_datas[0]);
          address = btcaddr_factory.CreateP2pkhAddress(pubkey);
          script_pub_key_res.pegout_addresses.push_back(address.GetAddress());
        } else if (pegout_type == LockingScriptType::kPayToPubkeyHash) {
          ByteData160 hash =
              ByteData160(pegout_extract_data.pushed_datas[0].GetBytes());
          address = btcaddr_factory.GetAddressByHash(
              AddressType::kP2pkhAddress, hash);
          script_pub_key_res.pegout_addresses.push_back(address.GetAddress());
        } else if (pegout_type == LockingScriptType::kPayToScriptHash) {
          ByteData160 hash =
              ByteData160(pegout_extract_data.pushed_datas[0].GetBytes());
          address = btcaddr_factory.GetAddressByHash(
              AddressType::kP2shAddress, hash);
          script_pub_key_res.pegout_addresses.push_back(address.GetAddress());
        } else if (pegout_type == LockingScriptType::kWitnessV0KeyHash) {
          address = btcaddr_factory.GetSegwitAddressByHash(
              pegout_extract_data.pushed_datas[0]);
          script_pub_key_res.pegout_addresses.push_back(address.GetAddress());
        } else if (pegout_type == LockingScriptType::kWitnessV0ScriptHash) {
          address = btcaddr_factory.GetSegwitAddressByHash(
              pegout_extract_data.pushed_datas[0]);
          script_pub_key_res.pegout_addresses.push_back(address.GetAddress());
        } else {
          script_pub_key_res.ignore_items.insert("pegout_reqSigs");
          script_pub_key_res.ignore_items.insert("pegout_addresses");
        }
      } else {
        script_pub_key_res.ignore_items.insert("pegout_chain");
        script_pub_key_res.ignore_items.insert("pegout_asm");
        script_pub_key_res.ignore_items.insert("pegout_hex");
        script_pub_key_res.ignore_items.insert("pegout_reqSigs");
        script_pub_key_res.ignore_items.insert("pegout_type");
        script_pub_key_res.ignore_items.insert("pegout_addresses");
      }

      tx_out_res.script_pub_key = script_pub_key_res;
      response.vout.push_back(tx_out_res);
      ++txout_count;
    }
    return response;
  };

  ElementsDecodeRawTransactionResponseStruct result;
  result = ExecuteStructApi<
      ElementsDecodeRawTransactionRequestStruct,
      ElementsDecodeRawTransactionResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

GetWitnessStackNumResponseStruct
ElementsTransactionStructApi::GetWitnessStackNum(
    const GetWitnessStackNumRequestStruct& request) {
  auto call_func = [](const GetWitnessStackNumRequestStruct& request)
      -> GetWitnessStackNumResponseStruct {  // NOLINT
    return TransactionApiBase::GetWitnessStackNum<
        ConfidentialTransactionController>(request, CreateController);
  };

  GetWitnessStackNumResponseStruct result;
  result = ExecuteStructApi<
      GetWitnessStackNumRequestStruct, GetWitnessStackNumResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

AddSignResponseStruct ElementsTransactionStructApi::AddSign(
    const AddSignRequestStruct& request) {
  auto call_func =
      [](const AddSignRequestStruct& request) -> AddSignResponseStruct {
    return TransactionApiBase::AddSign<ConfidentialTransactionController>(
        request, CreateController);
  };

  AddSignResponseStruct result;
  result = ExecuteStructApi<AddSignRequestStruct, AddSignResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

AddMultisigSignResponseStruct ElementsTransactionStructApi::AddMultisigSign(
    const AddMultisigSignRequestStruct& request) {
  auto call_func = [](const AddMultisigSignRequestStruct& request)
      -> AddMultisigSignResponseStruct {  // NOLINT
    AddMultisigSignResponseStruct response;
    // レスポンスとなるモデルへ変換
    // validate request
    if (request.txin.hash_type == "p2wsh") {
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to AddMultisigSign. p2wsh is excluded.");
    }

    return TransactionApiBase::AddMultisigSign<
        ConfidentialTransactionController>(request, CreateController);
  };

  AddMultisigSignResponseStruct result;
  result = ExecuteStructApi<
      AddMultisigSignRequestStruct, AddMultisigSignResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

UpdateWitnessStackResponseStruct
ElementsTransactionStructApi::UpdateWitnessStack(
    const UpdateWitnessStackRequestStruct& request) {
  auto call_func = [](const UpdateWitnessStackRequestStruct& request)
      -> UpdateWitnessStackResponseStruct {  // NOLINT
    return TransactionApiBase::UpdateWitnessStack<
        ConfidentialTransactionController>(request, CreateController);
  };

  UpdateWitnessStackResponseStruct result;
  result = ExecuteStructApi<
      UpdateWitnessStackRequestStruct, UpdateWitnessStackResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

CreateElementsSignatureHashResponseStruct
ElementsTransactionStructApi::CreateSignatureHash(  // NOLINT
    const CreateElementsSignatureHashRequestStruct& request) {
  auto call_func = [](const CreateElementsSignatureHashRequestStruct& request)
      -> CreateElementsSignatureHashResponseStruct {  // NOLINT
    CreateElementsSignatureHashResponseStruct response;
    std::string sig_hash;
    int64_t amount = request.txin.amount;
    const std::string& hashtype_str = request.txin.hash_type;
    const std::string& value_hex = request.txin.confidential_value_commitment;
    const Txid& txid = Txid(request.txin.txid);
    uint32_t vout = request.txin.vout;
    ConfidentialTransactionController txc(request.tx);
    SigHashType sighashtype = TransactionApiBase::ConvertSigHashType(
        request.txin.sighash_type, request.txin.sighash_anyone_can_pay);

    Pubkey pubkey;
    Script script;
    if (request.txin.key_data.type == "pubkey") {
      pubkey = Pubkey(request.txin.key_data.hex);
    } else if (request.txin.key_data.type == "redeem_script") {
      script = Script(request.txin.key_data.hex);
    }

    if ((hashtype_str == "p2pkh") || (hashtype_str == "p2wpkh")) {
      if (value_hex.empty()) {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, pubkey, sighashtype,
            Amount::CreateBySatoshiAmount(amount), (hashtype_str == "p2wpkh"));
      } else {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, pubkey, sighashtype, ByteData(value_hex),
            (hashtype_str == "p2wpkh"));
      }
    } else if ((hashtype_str == "p2sh") || (hashtype_str == "p2wsh")) {
      if (value_hex.empty()) {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, script, sighashtype,
            Amount::CreateBySatoshiAmount(amount), (hashtype_str == "p2wsh"));
      } else {
        sig_hash = txc.CreateSignatureHash(
            txid, vout, script, sighashtype, ByteData(value_hex),
            (hashtype_str == "p2wsh"));
      }
    } else {
      warn(
          CFD_LOG_SOURCE,
          "Failed to CreateSignatureHash. Invalid hashtype_str:  "
          "hashtype_str={}",  // NOLINT
          hashtype_str);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hashtype_str. hashtype_str must be \"p2pkh\" "
          "or \"p2sh\" or \"p2wpkh\" or \"p2wsh\".");  // NOLINT
    }

    response.sighash = sig_hash;
    return response;
  };

  CreateElementsSignatureHashResponseStruct result;
  result = ExecuteStructApi<
      CreateElementsSignatureHashRequestStruct,
      CreateElementsSignatureHashResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

BlindRawTransactionResponseStruct
ElementsTransactionStructApi::BlindTransaction(
    const BlindRawTransactionRequestStruct& request) {
  auto call_func = [](const BlindRawTransactionRequestStruct& request)
      -> BlindRawTransactionResponseStruct {  // NOLINT
    BlindRawTransactionResponseStruct response;
    ConfidentialTransactionController txc(request.tx);
    const ConfidentialTransaction& tx = txc.GetTransaction();

    uint32_t txin_count = tx.GetTxInCount();
    uint32_t txout_count = tx.GetTxOutCount();
    const std::vector<BlindTxInRequestStruct>& txins = request.txins;
    const std::vector<BlindTxOutRequestStruct>& txouts = request.txouts;
    if (txins.size() == 0) {
      warn(CFD_LOG_SOURCE, "Failed to txins empty.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "JSON value error. Empty txins.");
    }
    if (txins.size() != txin_count) {
      warn(
          CFD_LOG_SOURCE, "Failed to txins count. {} != {}", txins.size(),
          txin_count);
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "JSON value error. Unmatch txins count.");
    }
    if (txout_count == 0) {
      warn(CFD_LOG_SOURCE, "Failed to txouts empty.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "JSON value error. txouts empty.");
    }

    // fee count
    uint32_t fee_count = 0;
    uint32_t fee_index = 0;
    std::set<uint32_t> fee_indexes;
    for (const auto& txout : tx.GetTxOutList()) {
      if (txout.GetLockingScript().IsEmpty()) {
        ++fee_count;
        fee_indexes.insert(fee_index);
      }
      ++fee_index;
    }
    if (fee_count == txout_count) {
      warn(CFD_LOG_SOURCE, "Failed to txouts fee only.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "JSON value error. txouts fee only.");
    }
    uint32_t txout_value_count = txout_count - fee_count;
    if (txouts.size() < txout_value_count) {
      warn(
          CFD_LOG_SOURCE, "Failed to pubkey count. {} < {}", txouts.size(),
          txout_value_count);
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "JSON value error. Pubkey count not enough.");
    }
    bool is_insert_fee_key = false;
    if ((!fee_indexes.empty()) && (txouts.size() == txout_value_count)) {
      // insert empty key (for fee)
      is_insert_fee_key = true;
    }

    std::vector<BlindParameter> params(txin_count);
    std::vector<bool> exist_list(txin_count, false);
    std::vector<Pubkey> blind_pubkeys(txout_count);
    std::vector<IssuanceBlindingKeyPair> key_pairs;
    uint32_t index;

    // BlindingPubkey
    uint32_t pubkey_index = 0;
    for (index = 0; index < txout_count; ++index) {
      if (is_insert_fee_key && (fee_indexes.count(index) > 0)) {
        // throuth
      } else if (txouts.size() > pubkey_index) {
        // TODO(k-matsuzawa): 後で直す
        if (!txouts[pubkey_index].blind_pubkey.empty()) {
          blind_pubkeys[index] = Pubkey(txouts[pubkey_index].blind_pubkey);
        }
        ++pubkey_index;
      }
    }

    // BlindParameter
    uint32_t offset = 0;
    uint32_t count = 0;
    for (const auto& txin : txins) {
      if (!txin.txid.empty()) {
        // index指定
        offset = tx.GetTxInIndex(Txid(txin.txid), txin.vout);
        exist_list[offset] = true;
        params[offset].asset = ConfidentialAssetId(txin.asset);
        params[offset].vbf = BlindFactor(txin.blind_factor);
        params[offset].abf = BlindFactor(txin.asset_blind_factor);
        params[offset].value =
            ConfidentialValue(Amount::CreateBySatoshiAmount(txin.amount));
        ++count;
      }
    }

    if (count != txin_count) {
      for (const auto& txin : txins) {
        if (txin.txid.empty()) {
          // 指定なしなら前方から挿入
          for (index = 0; index < txin_count; ++index) {
            if (!exist_list[index]) {
              exist_list[index] = true;
              params[index].asset = ConfidentialAssetId(txin.asset);
              params[index].vbf = BlindFactor(txin.blind_factor);
              params[index].abf = BlindFactor(txin.asset_blind_factor);
              params[index].value = ConfidentialValue(
                  Amount::CreateBySatoshiAmount(txin.amount));
              break;
            }
          }
        }
      }
    }

    // Issuance
    if (request.issuances.size() > 0) {
      // txinの個数分確保
      key_pairs.resize(txin_count);

      for (BlindIssuanceRequestStruct issuance : request.issuances) {
        index = txc.GetTransaction().GetTxInIndex(
            Txid(issuance.txid), issuance.vout);
        IssuanceBlindingKeyPair key;
        if (!issuance.asset_blinding_key.empty()) {
          key.asset_key = Privkey(issuance.asset_blinding_key);
        }
        if (!issuance.token_blinding_key.empty()) {
          key.token_key = Privkey(issuance.token_blinding_key);
        }
        key_pairs[index] = key;
      }
    }

    txc.BlindTransaction(params, key_pairs, blind_pubkeys);
    response.hex = txc.GetHex();
    return response;
  };

  BlindRawTransactionResponseStruct result;
  result = ExecuteStructApi<
      BlindRawTransactionRequestStruct, BlindRawTransactionResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

UnblindRawTransactionResponseStruct
ElementsTransactionStructApi::UnblindTransaction(
    const UnblindRawTransactionRequestStruct& request) {
  auto call_func = [](const UnblindRawTransactionRequestStruct& request)
      -> UnblindRawTransactionResponseStruct {
    UnblindRawTransactionResponseStruct response;
    ConfidentialTransactionController ctxc(request.tx);
    const ConfidentialTransaction& tx = ctxc.GetTransaction();

    if (!request.txouts.empty()) {
      UnblindParameter unblind_param;
      for (const auto& txout : request.txouts) {
        // TxOutをUnblind
        const Privkey blinding_key(txout.blinding_key);
        unblind_param = ctxc.UnblindTxOut(txout.index, blinding_key);

        if (!unblind_param.asset.GetHex().empty()) {
          UnblindOutputStruct output;
          output.index = txout.index;
          output.asset = unblind_param.asset.GetHex();
          output.blind_factor = unblind_param.vbf.GetHex();
          output.asset_blind_factor = unblind_param.abf.GetHex();
          output.amount = unblind_param.value.GetAmount().GetSatoshiValue();
          response.outputs.push_back(output);
        }
      }
    }

    // TxIn Unblind(Issuanceの場合)
    if (request.issuances.size() != 0) {
      for (UnblindIssuanceStruct issuance : request.issuances) {
        uint32_t txin_index =
            tx.GetTxInIndex(Txid(issuance.txid), issuance.vout);
        bool is_find = false;
        UnblindIssuanceOutputStruct output;
        Privkey asset_blinding_key;
        Privkey token_blinding_key;
        if (!issuance.asset_blinding_key.empty()) {
          asset_blinding_key = Privkey(issuance.asset_blinding_key);
          is_find = true;
        }
        if (!issuance.token_blinding_key.empty()) {
          token_blinding_key = Privkey(issuance.token_blinding_key);
          is_find = true;
        }
        if (is_find) {
          std::vector<UnblindParameter> issuance_outputs =
              ctxc.UnblindIssuance(
                  txin_index, asset_blinding_key, token_blinding_key);

          output.txid = issuance.txid;
          output.vout = issuance.vout;
          output.asset = issuance_outputs[0].asset.GetHex();
          output.assetamount =
              issuance_outputs[0].value.GetAmount().GetSatoshiValue();
          if (issuance_outputs.size() > 1) {
            output.token = issuance_outputs[1].asset.GetHex();
            output.tokenamount =
                issuance_outputs[1].value.GetAmount().GetSatoshiValue();
          }
        }
        response.issuance_outputs.push_back(output);
      }
    }
    response.hex = ctxc.GetHex();
    return response;
  };

  UnblindRawTransactionResponseStruct result;
  result = ExecuteStructApi<
      UnblindRawTransactionRequestStruct, UnblindRawTransactionResponseStruct>(
      request, call_func, std::string(__FUNCTION__));

  return result;
}

SetRawIssueAssetResponseStruct ElementsTransactionStructApi::SetRawIssueAsset(
    const SetRawIssueAssetRequestStruct& request) {
  auto call_func = [](const SetRawIssueAssetRequestStruct& request)
      -> SetRawIssueAssetResponseStruct {  // NOLINT
    SetRawIssueAssetResponseStruct response;
    ConfidentialTransactionController ctxc(request.tx);
    ElementsAddressFactory address_factory;

    for (IssuanceDataRequestStruct req_issuance : request.issuances) {
      Script asset_locking_script;
      ByteData asset_nonce;

      if (ElementsConfidentialAddress::IsConfidentialAddress(
              req_issuance.asset_address)) {
        ElementsConfidentialAddress confidential_addr(
            req_issuance.asset_address);
        asset_locking_script = confidential_addr.GetLockingScript();
        if (!req_issuance.is_remove_nonce) {
          asset_nonce = confidential_addr.GetConfidentialKey().GetData();
        }
      } else {
        Address unblind_addr =
            address_factory.GetAddress(req_issuance.asset_address);
        asset_locking_script = unblind_addr.GetLockingScript();
      }

      Script token_locking_script;
      ByteData token_nonce;

      if (ElementsConfidentialAddress::IsConfidentialAddress(
              req_issuance.token_address)) {
        ElementsConfidentialAddress confidential_addr(
            req_issuance.token_address);
        token_locking_script = confidential_addr.GetLockingScript();
        if (!req_issuance.is_remove_nonce) {
          token_nonce = confidential_addr.GetConfidentialKey().GetData();
        }
      } else {
        Address unblind_addr =
            address_factory.GetAddress(req_issuance.token_address);
        token_locking_script = unblind_addr.GetLockingScript();
      }

      // Txin1つずつissuanceの設定を行う
      IssuanceParameter issuance_param = ctxc.SetAssetIssuance(
          Txid(req_issuance.txid), req_issuance.vout,
          Amount::CreateBySatoshiAmount(req_issuance.asset_amount),
          asset_locking_script, asset_nonce,
          Amount::CreateBySatoshiAmount(req_issuance.token_amount),
          token_locking_script, token_nonce, req_issuance.is_blind,
          ByteData256(req_issuance.contract_hash), false,
          req_issuance.is_remove_nonce);

      IssuanceDataResponseStruct res_issuance;
      res_issuance.txid = req_issuance.txid;
      res_issuance.vout = req_issuance.vout;
      res_issuance.asset = issuance_param.asset.GetHex();
      res_issuance.entropy = issuance_param.entropy.GetHex();
      res_issuance.token = issuance_param.token.GetHex();

      response.issuances.push_back(res_issuance);
    }

    // すべて設定後にTxoutのRandomSort
    if (request.is_random_sort_tx_out) {
      ctxc.RandomSortTxOut();
    }
    response.hex = ctxc.GetHex();
    return response;
  };

  SetRawIssueAssetResponseStruct result;
  result = ExecuteStructApi<
      SetRawIssueAssetRequestStruct, SetRawIssueAssetResponseStruct>(
      request, call_func, std::string(__FUNCTION__));

  return result;
}

SetRawReissueAssetResponseStruct
ElementsTransactionStructApi::SetRawReissueAsset(
    const SetRawReissueAssetRequestStruct& request) {
  auto call_func = [](const SetRawReissueAssetRequestStruct& request)
      -> SetRawReissueAssetResponseStruct {  // NOLINT
    SetRawReissueAssetResponseStruct response;
    ConfidentialTransactionController ctxc(request.tx);
    ElementsAddressFactory address_factory;

    for (ReissuanceDataRequestStruct req_issuance : request.issuances) {
      // Txin1つずつissuanceの設定を行う
      Script locking_script;
      ByteData nonce;

      if (ElementsConfidentialAddress::IsConfidentialAddress(
              req_issuance.address)) {
        ElementsConfidentialAddress confidential_addr(req_issuance.address);
        locking_script = confidential_addr.GetLockingScript();
        if (!req_issuance.is_remove_nonce) {
          nonce = confidential_addr.GetConfidentialKey().GetData();
        }
      } else {
        Address unblind_addr =
            address_factory.GetAddress(req_issuance.address);
        locking_script = unblind_addr.GetLockingScript();
      }

      IssuanceParameter issuance_param = ctxc.SetAssetReissuance(
          Txid(req_issuance.txid), req_issuance.vout,
          Amount::CreateBySatoshiAmount(req_issuance.amount), locking_script,
          nonce, BlindFactor(req_issuance.asset_blinding_nonce),
          BlindFactor(req_issuance.asset_entropy), false,
          req_issuance.is_remove_nonce);

      ReissuanceDataResponseStruct res_issuance;
      res_issuance.txid = req_issuance.txid;
      res_issuance.vout = req_issuance.vout;
      res_issuance.asset = issuance_param.asset.GetHex();
      res_issuance.entropy = issuance_param.entropy.GetHex();
      response.issuances.push_back(res_issuance);
    }

    // すべて設定後にTxoutのRandomSort
    if (request.is_random_sort_tx_out) {
      ctxc.RandomSortTxOut();
    }
    response.hex = ctxc.GetHex();
    return response;
  };

  SetRawReissueAssetResponseStruct result;
  result = ExecuteStructApi<
      SetRawReissueAssetRequestStruct, SetRawReissueAssetResponseStruct>(
      request, call_func, std::string(__FUNCTION__));

  return result;
}

ElementsCreateRawPeginResponseStruct
ElementsTransactionStructApi::CreateRawPeginTransaction(  // NOLINT
    const ElementsCreateRawPeginRequestStruct& request) {
  auto call_func = [](const ElementsCreateRawPeginRequestStruct& request)
      -> ElementsCreateRawPeginResponseStruct {  // NOLINT
    ElementsCreateRawPeginResponseStruct response;
    // Transaction作成
    ConfidentialTransactionController ctxc(request.version, request.locktime);
    ElementsAddressFactory address_factory;

    // TxInの追加
    const uint32_t kLockTimeDisabledSequence =
        ctxc.GetLockTimeDisabledSequence();
    for (ElementsPeginTxInStruct txin_req : request.txins) {
      Txid txid(txin_req.txid);
      uint32_t vout = txin_req.vout;

      // TxInのunlocking_scriptは空で作成
      if (kLockTimeDisabledSequence == txin_req.sequence) {
        ctxc.AddTxIn(txid, vout, ctxc.GetDefaultSequence());
      } else {
        ctxc.AddTxIn(txid, vout, txin_req.sequence);
      }

      // PeginWitnessの追加
      if (txin_req.is_pegin) {
        info(
            CFD_LOG_SOURCE, "rm btcWitness[{}]",
            txin_req.is_remove_mainchain_tx_witness);
        ByteData mainchain_tx =
            ByteData(txin_req.peginwitness.mainchain_raw_transaction);
        mainchain_tx = ConfidentialTransaction::GetBitcoinTransaction(
            mainchain_tx, txin_req.is_remove_mainchain_tx_witness);

        ctxc.AddPeginWitness(
            txid, vout,
            Amount::CreateBySatoshiAmount(txin_req.peginwitness.amount),
            ConfidentialAssetId(txin_req.peginwitness.asset),
            BlockHash(txin_req.peginwitness.mainchain_genesis_block_hash),
            Script(txin_req.peginwitness.claim_script), mainchain_tx,
            ByteData(txin_req.peginwitness.mainchain_txoutproof));
      }
    }

    // TxOutの追加
    for (ElementsPeginTxOutStruct txout_req : request.txouts) {
      const std::string addr = txout_req.address;
      if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
        ctxc.AddTxOut(
            ElementsConfidentialAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset), txout_req.is_remove_nonce);
      } else {
        ctxc.AddTxOut(
            address_factory.GetAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset));
      }
    }

    // feeの追加
    ElementsPeginTxOutFeeStruct fee_req = request.fee;
    ctxc.AddTxOutFee(
        Amount::CreateBySatoshiAmount(fee_req.amount),
        ConfidentialAssetId(fee_req.asset));

    // すべて設定後にTxoutのRandomSort
    if (request.is_random_sort_tx_out) {
      ctxc.RandomSortTxOut();
    }

    response.hex = ctxc.GetHex();
    return response;
  };

  ElementsCreateRawPeginResponseStruct result;
  result = ExecuteStructApi<
      ElementsCreateRawPeginRequestStruct,
      ElementsCreateRawPeginResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

ElementsCreateRawPegoutResponseStruct
ElementsTransactionStructApi::CreateRawPegoutTransaction(  // NOLINT
    const ElementsCreateRawPegoutRequestStruct& request) {
  auto call_func = [](const ElementsCreateRawPegoutRequestStruct& request)
      -> ElementsCreateRawPegoutResponseStruct {  // NOLINT
    ElementsCreateRawPegoutResponseStruct response;
    // Transaction作成
    ConfidentialTransactionController ctxc(request.version, request.locktime);
    ElementsAddressFactory address_factory;

    // TxInの追加
    const uint32_t kLockTimeDisabledSequence =
        ctxc.GetLockTimeDisabledSequence();
    for (ElementsPegoutTxInStruct txin_req : request.txins) {
      Txid txid(txin_req.txid);
      uint32_t vout = txin_req.vout;

      // TxInのunlocking_scriptは空で作成
      if (kLockTimeDisabledSequence == txin_req.sequence) {
        ctxc.AddTxIn(txid, vout, ctxc.GetDefaultSequence());
      } else {
        ctxc.AddTxIn(txid, vout, txin_req.sequence);
      }
    }

    // TxOutの追加
    for (ElementsPegoutTxOutStruct txout_req : request.txouts) {
      const std::string addr = txout_req.address;
      if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
        ctxc.AddTxOut(
            ElementsConfidentialAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset), txout_req.is_remove_nonce);
      } else {
        ctxc.AddTxOut(
            address_factory.GetAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset));
      }
    }

    // PegoutのTxOut追加
    const std::string pegout_address = request.pegout.btc_address;
    NetType net_type =
        AddressStructApi::ConvertNetType(request.pegout.network);

    if (!request.pegout.online_pubkey.empty() &&
        !request.pegout.master_online_key.empty()) {
      Privkey master_online_key;
      if (request.pegout.master_online_key.size() ==
          Privkey::kPrivkeySize * 2) {
        // hex
        master_online_key = Privkey(request.pegout.master_online_key);
      } else {
        // Wif
        master_online_key =
            Privkey::FromWif(request.pegout.master_online_key, net_type);
      }
      Address pegout_addr;
      if (pegout_address.empty()) {
        // TODO(k-matsuzawa): ExtKeyの正式対応が入るまでの暫定対応
        // pegoutのtemplateに従い、xpub/counterから生成する
        // descriptor parse
        std::string desc = request.pegout.bitcoin_descriptor;
        std::string::size_type start_point = desc.rfind('(');
        std::string arg_type;
        std::string xpub;
        if (start_point == std::string::npos) {
          xpub = desc;
        } else {
          arg_type = desc.substr(0, start_point);
          xpub = desc.substr(start_point + 1);
        }
        std::string::size_type end_point = xpub.find('/');
        if (end_point == std::string::npos) {
          end_point = xpub.find(')');
          if (end_point != std::string::npos) {
            xpub = xpub.substr(0, end_point);
          }
        } else {
          xpub = xpub.substr(0, end_point);
        }
        // info(CFD_LOG_SOURCE, "arg_type={}, xpub={}", arg_type, xpub);
        // key生成
        ExtKey ext_key = ExtKey(xpub).DerivePubkey(0).DerivePubkey(
            request.pegout.bip32_counter);
        Pubkey pubkey = ext_key.GetPubkey();

        // Addressクラス生成
        if (arg_type == "sh(wpkh") {
          Script wpkh_script = ScriptUtil::CreateP2wpkhLockingScript(pubkey);
          ByteData160 wpkh_hash = HashUtil::Hash160(wpkh_script);
          pegout_addr =
              Address(net_type, AddressType::kP2shAddress, wpkh_hash);
        } else if (arg_type == "wpkh") {
          pegout_addr = Address(net_type, WitnessVersion::kVersion0, pubkey);
        } else {  // if (arg_type == "pkh(")
          // pkh
          pegout_addr = Address(net_type, pubkey);
        }
      } else {
        pegout_addr = Address(pegout_address);
      }

      ctxc.AddPegoutTxOut(
          Amount::CreateBySatoshiAmount(request.pegout.amount),
          ConfidentialAssetId(request.pegout.asset),
          BlockHash(request.pegout.mainchain_genesis_block_hash), pegout_addr,
          net_type, Pubkey(request.pegout.online_pubkey), master_online_key,
          request.pegout.bitcoin_descriptor, request.pegout.bip32_counter,
          ByteData(request.pegout.whitelist));
      response.btc_address = pegout_addr.GetAddress();

    } else {
      ctxc.AddPegoutTxOut(
          Amount::CreateBySatoshiAmount(request.pegout.amount),
          ConfidentialAssetId(request.pegout.asset),
          BlockHash(request.pegout.mainchain_genesis_block_hash),
          Address(pegout_address));
      response.ignore_items.insert("btcAddress");
    }

    // feeの追加
    ElementsPegoutTxOutFeeStruct fee_req = request.fee;
    ctxc.AddTxOutFee(
        Amount::CreateBySatoshiAmount(fee_req.amount),
        ConfidentialAssetId(fee_req.asset));

    response.hex = ctxc.GetHex();
    return response;
  };

  ElementsCreateRawPegoutResponseStruct result;
  result = ExecuteStructApi<
      ElementsCreateRawPegoutRequestStruct,
      ElementsCreateRawPegoutResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

GetIssuanceBlindingKeyResponseStruct
ElementsTransactionStructApi::GetIssuanceBlindingKey(
    const GetIssuanceBlindingKeyRequestStruct& request) {
  auto call_func = [](const GetIssuanceBlindingKeyRequestStruct& request)
      -> GetIssuanceBlindingKeyResponseStruct {  // NOLINT
    GetIssuanceBlindingKeyResponseStruct response;
    Privkey blinding_key = ConfidentialTransaction::GetIssuanceBlindingKey(
        Privkey(request.master_blinding_key), Txid(request.txid),
        request.vout);
    response.blinding_key = blinding_key.GetHex();
    return response;
  };

  GetIssuanceBlindingKeyResponseStruct result;
  result = ExecuteStructApi<
      GetIssuanceBlindingKeyRequestStruct,
      GetIssuanceBlindingKeyResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

ElementsCreateDestroyAmountResponseStruct
ElementsTransactionStructApi::CreateDestroyAmountTransaction(
    const ElementsCreateDestroyAmountRequestStruct& request) {
  auto call_func = [](const ElementsCreateDestroyAmountRequestStruct& request)
      -> ElementsCreateDestroyAmountResponseStruct {  // NOLINT
    ElementsCreateDestroyAmountResponseStruct response;
    // Transaction作成
    ConfidentialTransactionController ctxc(request.version, request.locktime);
    ElementsAddressFactory address_factory;

    // TxInの追加
    const uint32_t kLockTimeDisabledSequence =
        ctxc.GetLockTimeDisabledSequence();
    for (ElementsDestroyAmountTxInStruct txin_req : request.txins) {
      Txid txid(txin_req.txid);
      uint32_t vout = txin_req.vout;

      // TxInのunlocking_scriptは空で作成
      if (kLockTimeDisabledSequence == txin_req.sequence) {
        ctxc.AddTxIn(txid, vout, ctxc.GetDefaultSequence());
      } else {
        ctxc.AddTxIn(txid, vout, txin_req.sequence);
      }
    }

    // TxOutの追加
    for (ElementsDestroyAmountTxOutStruct txout_req : request.txouts) {
      const std::string addr = txout_req.address;
      if (ElementsConfidentialAddress::IsConfidentialAddress(addr)) {
        ctxc.AddTxOut(
            ElementsConfidentialAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset), txout_req.is_remove_nonce);
      } else {
        ctxc.AddTxOut(
            address_factory.GetAddress(addr),
            Amount::CreateBySatoshiAmount(txout_req.amount),
            ConfidentialAssetId(txout_req.asset));
      }
    }

    // DestroyのTxOut追加
    // script作成
    ScriptBuilder builder;
    builder.AppendOperator(ScriptOperator::OP_RETURN);
    Script locking_script = builder.Build();

    ctxc.AddTxOut(
        locking_script, Amount::CreateBySatoshiAmount(request.destroy.amount),
        ConfidentialAssetId(request.destroy.asset));

    // feeの追加
    ElementsDestroyAmountFeeStruct fee_req = request.fee;
    ctxc.AddTxOutFee(
        Amount::CreateBySatoshiAmount(fee_req.amount),
        ConfidentialAssetId(fee_req.asset));

    response.hex = ctxc.GetHex();
    return response;
  };

  ElementsCreateDestroyAmountResponseStruct result;
  result = ExecuteStructApi<
      ElementsCreateDestroyAmountRequestStruct,
      ElementsCreateDestroyAmountResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

}  // namespace api
}  // namespace js
}  // namespace cfd
#endif  // CFD_DISABLE_ELEMENTS
