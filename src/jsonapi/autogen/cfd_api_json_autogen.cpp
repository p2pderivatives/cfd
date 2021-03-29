// Copyright 2020 CryptoGarage
/**
 * @file cfd_api_json_autogen.cpp
 *
 * @brief JSON mapping file (auto generate)
 */
#include <set>
#include <string>
#include <vector>

#include "cfd_api_json_autogen.h"  // NOLINT

namespace cfd {
namespace api {
namespace json {

using cfd::core::JsonClassBase;
using cfd::core::JsonObjectVector;
using cfd::core::JsonValueVector;
using cfd::core::JsonVector;
// clang-format off
// @formatter:off

// ------------------------------------------------------------------------
// DecodeLockingScript
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodeLockingScript>
  DecodeLockingScript::json_mapper;
std::vector<std::string> DecodeLockingScript::item_list;

void DecodeLockingScript::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodeLockingScript> func_table;  // NOLINT

  func_table = {
    DecodeLockingScript::GetAsmString,
    DecodeLockingScript::SetAsmString,
    DecodeLockingScript::GetAsmFieldType,
  };
  json_mapper.emplace("asm", func_table);
  item_list.push_back("asm");
  func_table = {
    DecodeLockingScript::GetHexString,
    DecodeLockingScript::SetHexString,
    DecodeLockingScript::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
  func_table = {
    DecodeLockingScript::GetReqSigsString,
    DecodeLockingScript::SetReqSigsString,
    DecodeLockingScript::GetReqSigsFieldType,
  };
  json_mapper.emplace("reqSigs", func_table);
  item_list.push_back("reqSigs");
  func_table = {
    DecodeLockingScript::GetTypeString,
    DecodeLockingScript::SetTypeString,
    DecodeLockingScript::GetTypeFieldType,
  };
  json_mapper.emplace("type", func_table);
  item_list.push_back("type");
  func_table = {
    DecodeLockingScript::GetAddressesString,
    DecodeLockingScript::SetAddressesString,
    DecodeLockingScript::GetAddressesFieldType,
  };
  json_mapper.emplace("addresses", func_table);
  item_list.push_back("addresses");
}

void DecodeLockingScript::ConvertFromStruct(
    const DecodeLockingScriptStruct& data) {
  asm__ = data.asm_;
  hex_ = data.hex;
  req_sigs_ = data.req_sigs;
  type_ = data.type;
  addresses_.ConvertFromStruct(data.addresses);
  ignore_items = data.ignore_items;
}

DecodeLockingScriptStruct DecodeLockingScript::ConvertToStruct() const {  // NOLINT
  DecodeLockingScriptStruct result;
  result.asm_ = asm__;
  result.hex = hex_;
  result.req_sigs = req_sigs_;
  result.type = type_;
  result.addresses = addresses_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodeUnlockingScript
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodeUnlockingScript>
  DecodeUnlockingScript::json_mapper;
std::vector<std::string> DecodeUnlockingScript::item_list;

void DecodeUnlockingScript::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodeUnlockingScript> func_table;  // NOLINT

  func_table = {
    DecodeUnlockingScript::GetAsmString,
    DecodeUnlockingScript::SetAsmString,
    DecodeUnlockingScript::GetAsmFieldType,
  };
  json_mapper.emplace("asm", func_table);
  item_list.push_back("asm");
  func_table = {
    DecodeUnlockingScript::GetHexString,
    DecodeUnlockingScript::SetHexString,
    DecodeUnlockingScript::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
}

void DecodeUnlockingScript::ConvertFromStruct(
    const DecodeUnlockingScriptStruct& data) {
  asm__ = data.asm_;
  hex_ = data.hex;
  ignore_items = data.ignore_items;
}

DecodeUnlockingScriptStruct DecodeUnlockingScript::ConvertToStruct() const {  // NOLINT
  DecodeUnlockingScriptStruct result;
  result.asm_ = asm__;
  result.hex = hex_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodePsbtLockingScript
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodePsbtLockingScript>
  DecodePsbtLockingScript::json_mapper;
std::vector<std::string> DecodePsbtLockingScript::item_list;

void DecodePsbtLockingScript::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodePsbtLockingScript> func_table;  // NOLINT

  func_table = {
    DecodePsbtLockingScript::GetAsmString,
    DecodePsbtLockingScript::SetAsmString,
    DecodePsbtLockingScript::GetAsmFieldType,
  };
  json_mapper.emplace("asm", func_table);
  item_list.push_back("asm");
  func_table = {
    DecodePsbtLockingScript::GetHexString,
    DecodePsbtLockingScript::SetHexString,
    DecodePsbtLockingScript::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
  func_table = {
    DecodePsbtLockingScript::GetTypeString,
    DecodePsbtLockingScript::SetTypeString,
    DecodePsbtLockingScript::GetTypeFieldType,
  };
  json_mapper.emplace("type", func_table);
  item_list.push_back("type");
  func_table = {
    DecodePsbtLockingScript::GetAddressString,
    DecodePsbtLockingScript::SetAddressString,
    DecodePsbtLockingScript::GetAddressFieldType,
  };
  json_mapper.emplace("address", func_table);
  item_list.push_back("address");
}

void DecodePsbtLockingScript::ConvertFromStruct(
    const DecodePsbtLockingScriptStruct& data) {
  asm__ = data.asm_;
  hex_ = data.hex;
  type_ = data.type;
  address_ = data.address;
  ignore_items = data.ignore_items;
}

DecodePsbtLockingScriptStruct DecodePsbtLockingScript::ConvertToStruct() const {  // NOLINT
  DecodePsbtLockingScriptStruct result;
  result.asm_ = asm__;
  result.hex = hex_;
  result.type = type_;
  result.address = address_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodeRawTransactionTxIn
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodeRawTransactionTxIn>
  DecodeRawTransactionTxIn::json_mapper;
std::vector<std::string> DecodeRawTransactionTxIn::item_list;

void DecodeRawTransactionTxIn::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodeRawTransactionTxIn> func_table;  // NOLINT

  func_table = {
    DecodeRawTransactionTxIn::GetCoinbaseString,
    DecodeRawTransactionTxIn::SetCoinbaseString,
    DecodeRawTransactionTxIn::GetCoinbaseFieldType,
  };
  json_mapper.emplace("coinbase", func_table);
  item_list.push_back("coinbase");
  func_table = {
    DecodeRawTransactionTxIn::GetTxidString,
    DecodeRawTransactionTxIn::SetTxidString,
    DecodeRawTransactionTxIn::GetTxidFieldType,
  };
  json_mapper.emplace("txid", func_table);
  item_list.push_back("txid");
  func_table = {
    DecodeRawTransactionTxIn::GetVoutString,
    DecodeRawTransactionTxIn::SetVoutString,
    DecodeRawTransactionTxIn::GetVoutFieldType,
  };
  json_mapper.emplace("vout", func_table);
  item_list.push_back("vout");
  func_table = {
    DecodeRawTransactionTxIn::GetScriptSigString,
    DecodeRawTransactionTxIn::SetScriptSigString,
    DecodeRawTransactionTxIn::GetScriptSigFieldType,
  };
  json_mapper.emplace("scriptSig", func_table);
  item_list.push_back("scriptSig");
  func_table = {
    DecodeRawTransactionTxIn::GetTxinwitnessString,
    DecodeRawTransactionTxIn::SetTxinwitnessString,
    DecodeRawTransactionTxIn::GetTxinwitnessFieldType,
  };
  json_mapper.emplace("txinwitness", func_table);
  item_list.push_back("txinwitness");
  func_table = {
    DecodeRawTransactionTxIn::GetSequenceString,
    DecodeRawTransactionTxIn::SetSequenceString,
    DecodeRawTransactionTxIn::GetSequenceFieldType,
  };
  json_mapper.emplace("sequence", func_table);
  item_list.push_back("sequence");
}

void DecodeRawTransactionTxIn::ConvertFromStruct(
    const DecodeRawTransactionTxInStruct& data) {
  coinbase_ = data.coinbase;
  txid_ = data.txid;
  vout_ = data.vout;
  script_sig_.ConvertFromStruct(data.script_sig);
  txinwitness_.ConvertFromStruct(data.txinwitness);
  sequence_ = data.sequence;
  ignore_items = data.ignore_items;
}

DecodeRawTransactionTxInStruct DecodeRawTransactionTxIn::ConvertToStruct() const {  // NOLINT
  DecodeRawTransactionTxInStruct result;
  result.coinbase = coinbase_;
  result.txid = txid_;
  result.vout = vout_;
  result.script_sig = script_sig_.ConvertToStruct();
  result.txinwitness = txinwitness_.ConvertToStruct();
  result.sequence = sequence_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodeRawTransactionTxOut
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodeRawTransactionTxOut>
  DecodeRawTransactionTxOut::json_mapper;
std::vector<std::string> DecodeRawTransactionTxOut::item_list;

void DecodeRawTransactionTxOut::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodeRawTransactionTxOut> func_table;  // NOLINT

  func_table = {
    DecodeRawTransactionTxOut::GetValueString,
    DecodeRawTransactionTxOut::SetValueString,
    DecodeRawTransactionTxOut::GetValueFieldType,
  };
  json_mapper.emplace("value", func_table);
  item_list.push_back("value");
  func_table = {
    DecodeRawTransactionTxOut::GetNString,
    DecodeRawTransactionTxOut::SetNString,
    DecodeRawTransactionTxOut::GetNFieldType,
  };
  json_mapper.emplace("n", func_table);
  item_list.push_back("n");
  func_table = {
    DecodeRawTransactionTxOut::GetScriptPubKeyString,
    DecodeRawTransactionTxOut::SetScriptPubKeyString,
    DecodeRawTransactionTxOut::GetScriptPubKeyFieldType,
  };
  json_mapper.emplace("scriptPubKey", func_table);
  item_list.push_back("scriptPubKey");
}

void DecodeRawTransactionTxOut::ConvertFromStruct(
    const DecodeRawTransactionTxOutStruct& data) {
  value_ = data.value;
  n_ = data.n;
  script_pub_key_.ConvertFromStruct(data.script_pub_key);
  ignore_items = data.ignore_items;
}

DecodeRawTransactionTxOutStruct DecodeRawTransactionTxOut::ConvertToStruct() const {  // NOLINT
  DecodeRawTransactionTxOutStruct result;
  result.value = value_;
  result.n = n_;
  result.script_pub_key = script_pub_key_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodePsbtUtxo
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodePsbtUtxo>
  DecodePsbtUtxo::json_mapper;
std::vector<std::string> DecodePsbtUtxo::item_list;

void DecodePsbtUtxo::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodePsbtUtxo> func_table;  // NOLINT

  func_table = {
    DecodePsbtUtxo::GetAmountString,
    DecodePsbtUtxo::SetAmountString,
    DecodePsbtUtxo::GetAmountFieldType,
  };
  json_mapper.emplace("amount", func_table);
  item_list.push_back("amount");
  func_table = {
    DecodePsbtUtxo::GetScriptPubKeyString,
    DecodePsbtUtxo::SetScriptPubKeyString,
    DecodePsbtUtxo::GetScriptPubKeyFieldType,
  };
  json_mapper.emplace("scriptPubKey", func_table);
  item_list.push_back("scriptPubKey");
}

void DecodePsbtUtxo::ConvertFromStruct(
    const DecodePsbtUtxoStruct& data) {
  amount_ = data.amount;
  script_pub_key_.ConvertFromStruct(data.script_pub_key);
  ignore_items = data.ignore_items;
}

DecodePsbtUtxoStruct DecodePsbtUtxo::ConvertToStruct() const {  // NOLINT
  DecodePsbtUtxoStruct result;
  result.amount = amount_;
  result.script_pub_key = script_pub_key_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodeRawTransactionResponse
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodeRawTransactionResponse>
  DecodeRawTransactionResponse::json_mapper;
std::vector<std::string> DecodeRawTransactionResponse::item_list;

void DecodeRawTransactionResponse::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodeRawTransactionResponse> func_table;  // NOLINT

  func_table = {
    DecodeRawTransactionResponse::GetTxidString,
    DecodeRawTransactionResponse::SetTxidString,
    DecodeRawTransactionResponse::GetTxidFieldType,
  };
  json_mapper.emplace("txid", func_table);
  item_list.push_back("txid");
  func_table = {
    DecodeRawTransactionResponse::GetHashString,
    DecodeRawTransactionResponse::SetHashString,
    DecodeRawTransactionResponse::GetHashFieldType,
  };
  json_mapper.emplace("hash", func_table);
  item_list.push_back("hash");
  func_table = {
    DecodeRawTransactionResponse::GetVersionString,
    DecodeRawTransactionResponse::SetVersionString,
    DecodeRawTransactionResponse::GetVersionFieldType,
  };
  json_mapper.emplace("version", func_table);
  item_list.push_back("version");
  func_table = {
    DecodeRawTransactionResponse::GetSizeString,
    DecodeRawTransactionResponse::SetSizeString,
    DecodeRawTransactionResponse::GetSizeFieldType,
  };
  json_mapper.emplace("size", func_table);
  item_list.push_back("size");
  func_table = {
    DecodeRawTransactionResponse::GetVsizeString,
    DecodeRawTransactionResponse::SetVsizeString,
    DecodeRawTransactionResponse::GetVsizeFieldType,
  };
  json_mapper.emplace("vsize", func_table);
  item_list.push_back("vsize");
  func_table = {
    DecodeRawTransactionResponse::GetWeightString,
    DecodeRawTransactionResponse::SetWeightString,
    DecodeRawTransactionResponse::GetWeightFieldType,
  };
  json_mapper.emplace("weight", func_table);
  item_list.push_back("weight");
  func_table = {
    DecodeRawTransactionResponse::GetLocktimeString,
    DecodeRawTransactionResponse::SetLocktimeString,
    DecodeRawTransactionResponse::GetLocktimeFieldType,
  };
  json_mapper.emplace("locktime", func_table);
  item_list.push_back("locktime");
  func_table = {
    DecodeRawTransactionResponse::GetVinString,
    DecodeRawTransactionResponse::SetVinString,
    DecodeRawTransactionResponse::GetVinFieldType,
  };
  json_mapper.emplace("vin", func_table);
  item_list.push_back("vin");
  func_table = {
    DecodeRawTransactionResponse::GetVoutString,
    DecodeRawTransactionResponse::SetVoutString,
    DecodeRawTransactionResponse::GetVoutFieldType,
  };
  json_mapper.emplace("vout", func_table);
  item_list.push_back("vout");
}

void DecodeRawTransactionResponse::ConvertFromStruct(
    const DecodeRawTransactionResponseStruct& data) {
  txid_ = data.txid;
  hash_ = data.hash;
  version_ = data.version;
  size_ = data.size;
  vsize_ = data.vsize;
  weight_ = data.weight;
  locktime_ = data.locktime;
  vin_.ConvertFromStruct(data.vin);
  vout_.ConvertFromStruct(data.vout);
  ignore_items = data.ignore_items;
}

DecodeRawTransactionResponseStruct DecodeRawTransactionResponse::ConvertToStruct() const {  // NOLINT
  DecodeRawTransactionResponseStruct result;
  result.txid = txid_;
  result.hash = hash_;
  result.version = version_;
  result.size = size_;
  result.vsize = vsize_;
  result.weight = weight_;
  result.locktime = locktime_;
  result.vin = vin_.ConvertToStruct();
  result.vout = vout_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// ElementsDecodeIssuance
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<ElementsDecodeIssuance>
  ElementsDecodeIssuance::json_mapper;
std::vector<std::string> ElementsDecodeIssuance::item_list;

void ElementsDecodeIssuance::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<ElementsDecodeIssuance> func_table;  // NOLINT

  func_table = {
    ElementsDecodeIssuance::GetAssetBlindingNonceString,
    ElementsDecodeIssuance::SetAssetBlindingNonceString,
    ElementsDecodeIssuance::GetAssetBlindingNonceFieldType,
  };
  json_mapper.emplace("assetBlindingNonce", func_table);
  item_list.push_back("assetBlindingNonce");
  func_table = {
    ElementsDecodeIssuance::GetAssetEntropyString,
    ElementsDecodeIssuance::SetAssetEntropyString,
    ElementsDecodeIssuance::GetAssetEntropyFieldType,
  };
  json_mapper.emplace("assetEntropy", func_table);
  item_list.push_back("assetEntropy");
  func_table = {
    ElementsDecodeIssuance::GetContractHashString,
    ElementsDecodeIssuance::SetContractHashString,
    ElementsDecodeIssuance::GetContractHashFieldType,
  };
  json_mapper.emplace("contractHash", func_table);
  item_list.push_back("contractHash");
  func_table = {
    ElementsDecodeIssuance::GetIsreissuanceString,
    ElementsDecodeIssuance::SetIsreissuanceString,
    ElementsDecodeIssuance::GetIsreissuanceFieldType,
  };
  json_mapper.emplace("isreissuance", func_table);
  item_list.push_back("isreissuance");
  func_table = {
    ElementsDecodeIssuance::GetTokenString,
    ElementsDecodeIssuance::SetTokenString,
    ElementsDecodeIssuance::GetTokenFieldType,
  };
  json_mapper.emplace("token", func_table);
  item_list.push_back("token");
  func_table = {
    ElementsDecodeIssuance::GetAssetString,
    ElementsDecodeIssuance::SetAssetString,
    ElementsDecodeIssuance::GetAssetFieldType,
  };
  json_mapper.emplace("asset", func_table);
  item_list.push_back("asset");
  func_table = {
    ElementsDecodeIssuance::GetAssetamountString,
    ElementsDecodeIssuance::SetAssetamountString,
    ElementsDecodeIssuance::GetAssetamountFieldType,
  };
  json_mapper.emplace("assetamount", func_table);
  item_list.push_back("assetamount");
  func_table = {
    ElementsDecodeIssuance::GetAssetamountcommitmentString,
    ElementsDecodeIssuance::SetAssetamountcommitmentString,
    ElementsDecodeIssuance::GetAssetamountcommitmentFieldType,
  };
  json_mapper.emplace("assetamountcommitment", func_table);
  item_list.push_back("assetamountcommitment");
  func_table = {
    ElementsDecodeIssuance::GetTokenamountString,
    ElementsDecodeIssuance::SetTokenamountString,
    ElementsDecodeIssuance::GetTokenamountFieldType,
  };
  json_mapper.emplace("tokenamount", func_table);
  item_list.push_back("tokenamount");
  func_table = {
    ElementsDecodeIssuance::GetTokenamountcommitmentString,
    ElementsDecodeIssuance::SetTokenamountcommitmentString,
    ElementsDecodeIssuance::GetTokenamountcommitmentFieldType,
  };
  json_mapper.emplace("tokenamountcommitment", func_table);
  item_list.push_back("tokenamountcommitment");
  func_table = {
    ElementsDecodeIssuance::GetAssetRangeproofString,
    ElementsDecodeIssuance::SetAssetRangeproofString,
    ElementsDecodeIssuance::GetAssetRangeproofFieldType,
  };
  json_mapper.emplace("assetRangeproof", func_table);
  item_list.push_back("assetRangeproof");
  func_table = {
    ElementsDecodeIssuance::GetTokenRangeproofString,
    ElementsDecodeIssuance::SetTokenRangeproofString,
    ElementsDecodeIssuance::GetTokenRangeproofFieldType,
  };
  json_mapper.emplace("tokenRangeproof", func_table);
  item_list.push_back("tokenRangeproof");
}

void ElementsDecodeIssuance::ConvertFromStruct(
    const ElementsDecodeIssuanceStruct& data) {
  asset_blinding_nonce_ = data.asset_blinding_nonce;
  asset_entropy_ = data.asset_entropy;
  contract_hash_ = data.contract_hash;
  isreissuance_ = data.isreissuance;
  token_ = data.token;
  asset_ = data.asset;
  assetamount_ = data.assetamount;
  assetamountcommitment_ = data.assetamountcommitment;
  tokenamount_ = data.tokenamount;
  tokenamountcommitment_ = data.tokenamountcommitment;
  asset_rangeproof_ = data.asset_rangeproof;
  token_rangeproof_ = data.token_rangeproof;
  ignore_items = data.ignore_items;
}

ElementsDecodeIssuanceStruct ElementsDecodeIssuance::ConvertToStruct() const {  // NOLINT
  ElementsDecodeIssuanceStruct result;
  result.asset_blinding_nonce = asset_blinding_nonce_;
  result.asset_entropy = asset_entropy_;
  result.contract_hash = contract_hash_;
  result.isreissuance = isreissuance_;
  result.token = token_;
  result.asset = asset_;
  result.assetamount = assetamount_;
  result.assetamountcommitment = assetamountcommitment_;
  result.tokenamount = tokenamount_;
  result.tokenamountcommitment = tokenamountcommitment_;
  result.asset_rangeproof = asset_rangeproof_;
  result.token_rangeproof = token_rangeproof_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// ElementsDecodeLockingScript
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<ElementsDecodeLockingScript>
  ElementsDecodeLockingScript::json_mapper;
std::vector<std::string> ElementsDecodeLockingScript::item_list;

void ElementsDecodeLockingScript::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<ElementsDecodeLockingScript> func_table;  // NOLINT

  func_table = {
    ElementsDecodeLockingScript::GetAsmString,
    ElementsDecodeLockingScript::SetAsmString,
    ElementsDecodeLockingScript::GetAsmFieldType,
  };
  json_mapper.emplace("asm", func_table);
  item_list.push_back("asm");
  func_table = {
    ElementsDecodeLockingScript::GetHexString,
    ElementsDecodeLockingScript::SetHexString,
    ElementsDecodeLockingScript::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
  func_table = {
    ElementsDecodeLockingScript::GetReqSigsString,
    ElementsDecodeLockingScript::SetReqSigsString,
    ElementsDecodeLockingScript::GetReqSigsFieldType,
  };
  json_mapper.emplace("reqSigs", func_table);
  item_list.push_back("reqSigs");
  func_table = {
    ElementsDecodeLockingScript::GetTypeString,
    ElementsDecodeLockingScript::SetTypeString,
    ElementsDecodeLockingScript::GetTypeFieldType,
  };
  json_mapper.emplace("type", func_table);
  item_list.push_back("type");
  func_table = {
    ElementsDecodeLockingScript::GetAddressesString,
    ElementsDecodeLockingScript::SetAddressesString,
    ElementsDecodeLockingScript::GetAddressesFieldType,
  };
  json_mapper.emplace("addresses", func_table);
  item_list.push_back("addresses");
  func_table = {
    ElementsDecodeLockingScript::GetPegout_chainString,
    ElementsDecodeLockingScript::SetPegout_chainString,
    ElementsDecodeLockingScript::GetPegout_chainFieldType,
  };
  json_mapper.emplace("pegout_chain", func_table);
  item_list.push_back("pegout_chain");
  func_table = {
    ElementsDecodeLockingScript::GetPegout_asmString,
    ElementsDecodeLockingScript::SetPegout_asmString,
    ElementsDecodeLockingScript::GetPegout_asmFieldType,
  };
  json_mapper.emplace("pegout_asm", func_table);
  item_list.push_back("pegout_asm");
  func_table = {
    ElementsDecodeLockingScript::GetPegout_hexString,
    ElementsDecodeLockingScript::SetPegout_hexString,
    ElementsDecodeLockingScript::GetPegout_hexFieldType,
  };
  json_mapper.emplace("pegout_hex", func_table);
  item_list.push_back("pegout_hex");
  func_table = {
    ElementsDecodeLockingScript::GetPegout_reqSigsString,
    ElementsDecodeLockingScript::SetPegout_reqSigsString,
    ElementsDecodeLockingScript::GetPegout_reqSigsFieldType,
  };
  json_mapper.emplace("pegout_reqSigs", func_table);
  item_list.push_back("pegout_reqSigs");
  func_table = {
    ElementsDecodeLockingScript::GetPegout_typeString,
    ElementsDecodeLockingScript::SetPegout_typeString,
    ElementsDecodeLockingScript::GetPegout_typeFieldType,
  };
  json_mapper.emplace("pegout_type", func_table);
  item_list.push_back("pegout_type");
  func_table = {
    ElementsDecodeLockingScript::GetPegout_addressesString,
    ElementsDecodeLockingScript::SetPegout_addressesString,
    ElementsDecodeLockingScript::GetPegout_addressesFieldType,
  };
  json_mapper.emplace("pegout_addresses", func_table);
  item_list.push_back("pegout_addresses");
}

void ElementsDecodeLockingScript::ConvertFromStruct(
    const ElementsDecodeLockingScriptStruct& data) {
  asm__ = data.asm_;
  hex_ = data.hex;
  req_sigs_ = data.req_sigs;
  type_ = data.type;
  addresses_.ConvertFromStruct(data.addresses);
  pegout_chain_ = data.pegout_chain;
  pegout_asm_ = data.pegout_asm;
  pegout_hex_ = data.pegout_hex;
  pegout_req_sigs_ = data.pegout_req_sigs;
  pegout_type_ = data.pegout_type;
  pegout_addresses_.ConvertFromStruct(data.pegout_addresses);
  ignore_items = data.ignore_items;
}

ElementsDecodeLockingScriptStruct ElementsDecodeLockingScript::ConvertToStruct() const {  // NOLINT
  ElementsDecodeLockingScriptStruct result;
  result.asm_ = asm__;
  result.hex = hex_;
  result.req_sigs = req_sigs_;
  result.type = type_;
  result.addresses = addresses_.ConvertToStruct();
  result.pegout_chain = pegout_chain_;
  result.pegout_asm = pegout_asm_;
  result.pegout_hex = pegout_hex_;
  result.pegout_req_sigs = pegout_req_sigs_;
  result.pegout_type = pegout_type_;
  result.pegout_addresses = pegout_addresses_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// ElementsDecodeUnlockingScript
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<ElementsDecodeUnlockingScript>
  ElementsDecodeUnlockingScript::json_mapper;
std::vector<std::string> ElementsDecodeUnlockingScript::item_list;

void ElementsDecodeUnlockingScript::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<ElementsDecodeUnlockingScript> func_table;  // NOLINT

  func_table = {
    ElementsDecodeUnlockingScript::GetAsmString,
    ElementsDecodeUnlockingScript::SetAsmString,
    ElementsDecodeUnlockingScript::GetAsmFieldType,
  };
  json_mapper.emplace("asm", func_table);
  item_list.push_back("asm");
  func_table = {
    ElementsDecodeUnlockingScript::GetHexString,
    ElementsDecodeUnlockingScript::SetHexString,
    ElementsDecodeUnlockingScript::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
}

void ElementsDecodeUnlockingScript::ConvertFromStruct(
    const ElementsDecodeUnlockingScriptStruct& data) {
  asm__ = data.asm_;
  hex_ = data.hex;
  ignore_items = data.ignore_items;
}

ElementsDecodeUnlockingScriptStruct ElementsDecodeUnlockingScript::ConvertToStruct() const {  // NOLINT
  ElementsDecodeUnlockingScriptStruct result;
  result.asm_ = asm__;
  result.hex = hex_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// PsbtBip32Data
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<PsbtBip32Data>
  PsbtBip32Data::json_mapper;
std::vector<std::string> PsbtBip32Data::item_list;

void PsbtBip32Data::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<PsbtBip32Data> func_table;  // NOLINT

  func_table = {
    PsbtBip32Data::GetPubkeyString,
    PsbtBip32Data::SetPubkeyString,
    PsbtBip32Data::GetPubkeyFieldType,
  };
  json_mapper.emplace("pubkey", func_table);
  item_list.push_back("pubkey");
  func_table = {
    PsbtBip32Data::GetMaster_fingerprintString,
    PsbtBip32Data::SetMaster_fingerprintString,
    PsbtBip32Data::GetMaster_fingerprintFieldType,
  };
  json_mapper.emplace("master_fingerprint", func_table);
  item_list.push_back("master_fingerprint");
  func_table = {
    PsbtBip32Data::GetPathString,
    PsbtBip32Data::SetPathString,
    PsbtBip32Data::GetPathFieldType,
  };
  json_mapper.emplace("path", func_table);
  item_list.push_back("path");
  func_table = {
    PsbtBip32Data::GetDescriptorString,
    PsbtBip32Data::SetDescriptorString,
    PsbtBip32Data::GetDescriptorFieldType,
  };
  json_mapper.emplace("descriptor", func_table);
  item_list.push_back("descriptor");
}

void PsbtBip32Data::ConvertFromStruct(
    const PsbtBip32DataStruct& data) {
  pubkey_ = data.pubkey;
  master_fingerprint_ = data.master_fingerprint;
  path_ = data.path;
  descriptor_ = data.descriptor;
  ignore_items = data.ignore_items;
}

PsbtBip32DataStruct PsbtBip32Data::ConvertToStruct() const {  // NOLINT
  PsbtBip32DataStruct result;
  result.pubkey = pubkey_;
  result.master_fingerprint = master_fingerprint_;
  result.path = path_;
  result.descriptor = descriptor_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// PsbtMapData
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<PsbtMapData>
  PsbtMapData::json_mapper;
std::vector<std::string> PsbtMapData::item_list;

void PsbtMapData::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<PsbtMapData> func_table;  // NOLINT

  func_table = {
    PsbtMapData::GetKeyString,
    PsbtMapData::SetKeyString,
    PsbtMapData::GetKeyFieldType,
  };
  json_mapper.emplace("key", func_table);
  item_list.push_back("key");
  func_table = {
    PsbtMapData::GetValueString,
    PsbtMapData::SetValueString,
    PsbtMapData::GetValueFieldType,
  };
  json_mapper.emplace("value", func_table);
  item_list.push_back("value");
}

void PsbtMapData::ConvertFromStruct(
    const PsbtMapDataStruct& data) {
  key_ = data.key;
  value_ = data.value;
  ignore_items = data.ignore_items;
}

PsbtMapDataStruct PsbtMapData::ConvertToStruct() const {  // NOLINT
  PsbtMapDataStruct result;
  result.key = key_;
  result.value = value_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// PsbtScriptData
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<PsbtScriptData>
  PsbtScriptData::json_mapper;
std::vector<std::string> PsbtScriptData::item_list;

void PsbtScriptData::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<PsbtScriptData> func_table;  // NOLINT

  func_table = {
    PsbtScriptData::GetAsmString,
    PsbtScriptData::SetAsmString,
    PsbtScriptData::GetAsmFieldType,
  };
  json_mapper.emplace("asm", func_table);
  item_list.push_back("asm");
  func_table = {
    PsbtScriptData::GetHexString,
    PsbtScriptData::SetHexString,
    PsbtScriptData::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
  func_table = {
    PsbtScriptData::GetTypeString,
    PsbtScriptData::SetTypeString,
    PsbtScriptData::GetTypeFieldType,
  };
  json_mapper.emplace("type", func_table);
  item_list.push_back("type");
}

void PsbtScriptData::ConvertFromStruct(
    const PsbtScriptDataStruct& data) {
  asm__ = data.asm_;
  hex_ = data.hex;
  type_ = data.type;
  ignore_items = data.ignore_items;
}

PsbtScriptDataStruct PsbtScriptData::ConvertToStruct() const {  // NOLINT
  PsbtScriptDataStruct result;
  result.asm_ = asm__;
  result.hex = hex_;
  result.type = type_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// PsbtSignatureData
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<PsbtSignatureData>
  PsbtSignatureData::json_mapper;
std::vector<std::string> PsbtSignatureData::item_list;

void PsbtSignatureData::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<PsbtSignatureData> func_table;  // NOLINT

  func_table = {
    PsbtSignatureData::GetPubkeyString,
    PsbtSignatureData::SetPubkeyString,
    PsbtSignatureData::GetPubkeyFieldType,
  };
  json_mapper.emplace("pubkey", func_table);
  item_list.push_back("pubkey");
  func_table = {
    PsbtSignatureData::GetSignatureString,
    PsbtSignatureData::SetSignatureString,
    PsbtSignatureData::GetSignatureFieldType,
  };
  json_mapper.emplace("signature", func_table);
  item_list.push_back("signature");
}

void PsbtSignatureData::ConvertFromStruct(
    const PsbtSignatureDataStruct& data) {
  pubkey_ = data.pubkey;
  signature_ = data.signature;
  ignore_items = data.ignore_items;
}

PsbtSignatureDataStruct PsbtSignatureData::ConvertToStruct() const {  // NOLINT
  PsbtSignatureDataStruct result;
  result.pubkey = pubkey_;
  result.signature = signature_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// XpubData
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<XpubData>
  XpubData::json_mapper;
std::vector<std::string> XpubData::item_list;

void XpubData::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<XpubData> func_table;  // NOLINT

  func_table = {
    XpubData::GetBase58String,
    XpubData::SetBase58String,
    XpubData::GetBase58FieldType,
  };
  json_mapper.emplace("base58", func_table);
  item_list.push_back("base58");
  func_table = {
    XpubData::GetHexString,
    XpubData::SetHexString,
    XpubData::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
}

void XpubData::ConvertFromStruct(
    const XpubDataStruct& data) {
  base58_ = data.base58;
  hex_ = data.hex;
  ignore_items = data.ignore_items;
}

XpubDataStruct XpubData::ConvertToStruct() const {  // NOLINT
  XpubDataStruct result;
  result.base58 = base58_;
  result.hex = hex_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodePsbtInput
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodePsbtInput>
  DecodePsbtInput::json_mapper;
std::vector<std::string> DecodePsbtInput::item_list;

void DecodePsbtInput::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodePsbtInput> func_table;  // NOLINT

  func_table = {
    DecodePsbtInput::GetNon_witness_utxo_hexString,
    DecodePsbtInput::SetNon_witness_utxo_hexString,
    DecodePsbtInput::GetNon_witness_utxo_hexFieldType,
  };
  json_mapper.emplace("non_witness_utxo_hex", func_table);
  item_list.push_back("non_witness_utxo_hex");
  func_table = {
    DecodePsbtInput::GetNon_witness_utxoString,
    DecodePsbtInput::SetNon_witness_utxoString,
    DecodePsbtInput::GetNon_witness_utxoFieldType,
  };
  json_mapper.emplace("non_witness_utxo", func_table);
  item_list.push_back("non_witness_utxo");
  func_table = {
    DecodePsbtInput::GetWitness_utxoString,
    DecodePsbtInput::SetWitness_utxoString,
    DecodePsbtInput::GetWitness_utxoFieldType,
  };
  json_mapper.emplace("witness_utxo", func_table);
  item_list.push_back("witness_utxo");
  func_table = {
    DecodePsbtInput::GetPartial_signaturesString,
    DecodePsbtInput::SetPartial_signaturesString,
    DecodePsbtInput::GetPartial_signaturesFieldType,
  };
  json_mapper.emplace("partial_signatures", func_table);
  item_list.push_back("partial_signatures");
  func_table = {
    DecodePsbtInput::GetSighashString,
    DecodePsbtInput::SetSighashString,
    DecodePsbtInput::GetSighashFieldType,
  };
  json_mapper.emplace("sighash", func_table);
  item_list.push_back("sighash");
  func_table = {
    DecodePsbtInput::GetRedeem_scriptString,
    DecodePsbtInput::SetRedeem_scriptString,
    DecodePsbtInput::GetRedeem_scriptFieldType,
  };
  json_mapper.emplace("redeem_script", func_table);
  item_list.push_back("redeem_script");
  func_table = {
    DecodePsbtInput::GetWitness_scriptString,
    DecodePsbtInput::SetWitness_scriptString,
    DecodePsbtInput::GetWitness_scriptFieldType,
  };
  json_mapper.emplace("witness_script", func_table);
  item_list.push_back("witness_script");
  func_table = {
    DecodePsbtInput::GetBip32_derivsString,
    DecodePsbtInput::SetBip32_derivsString,
    DecodePsbtInput::GetBip32_derivsFieldType,
  };
  json_mapper.emplace("bip32_derivs", func_table);
  item_list.push_back("bip32_derivs");
  func_table = {
    DecodePsbtInput::GetFinal_scriptsigString,
    DecodePsbtInput::SetFinal_scriptsigString,
    DecodePsbtInput::GetFinal_scriptsigFieldType,
  };
  json_mapper.emplace("final_scriptsig", func_table);
  item_list.push_back("final_scriptsig");
  func_table = {
    DecodePsbtInput::GetFinal_scriptwitnessString,
    DecodePsbtInput::SetFinal_scriptwitnessString,
    DecodePsbtInput::GetFinal_scriptwitnessFieldType,
  };
  json_mapper.emplace("final_scriptwitness", func_table);
  item_list.push_back("final_scriptwitness");
  func_table = {
    DecodePsbtInput::GetUnknownString,
    DecodePsbtInput::SetUnknownString,
    DecodePsbtInput::GetUnknownFieldType,
  };
  json_mapper.emplace("unknown", func_table);
  item_list.push_back("unknown");
}

void DecodePsbtInput::ConvertFromStruct(
    const DecodePsbtInputStruct& data) {
  non_witness_utxo_hex_ = data.non_witness_utxo_hex;
  non_witness_utxo_.ConvertFromStruct(data.non_witness_utxo);
  witness_utxo_.ConvertFromStruct(data.witness_utxo);
  partial_signatures_.ConvertFromStruct(data.partial_signatures);
  sighash_ = data.sighash;
  redeem_script_.ConvertFromStruct(data.redeem_script);
  witness_script_.ConvertFromStruct(data.witness_script);
  bip32_derivs_.ConvertFromStruct(data.bip32_derivs);
  final_scriptsig_.ConvertFromStruct(data.final_scriptsig);
  final_scriptwitness_.ConvertFromStruct(data.final_scriptwitness);
  unknown_.ConvertFromStruct(data.unknown);
  ignore_items = data.ignore_items;
}

DecodePsbtInputStruct DecodePsbtInput::ConvertToStruct() const {  // NOLINT
  DecodePsbtInputStruct result;
  result.non_witness_utxo_hex = non_witness_utxo_hex_;
  result.non_witness_utxo = non_witness_utxo_.ConvertToStruct();
  result.witness_utxo = witness_utxo_.ConvertToStruct();
  result.partial_signatures = partial_signatures_.ConvertToStruct();
  result.sighash = sighash_;
  result.redeem_script = redeem_script_.ConvertToStruct();
  result.witness_script = witness_script_.ConvertToStruct();
  result.bip32_derivs = bip32_derivs_.ConvertToStruct();
  result.final_scriptsig = final_scriptsig_.ConvertToStruct();
  result.final_scriptwitness = final_scriptwitness_.ConvertToStruct();
  result.unknown = unknown_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodePsbtOutput
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodePsbtOutput>
  DecodePsbtOutput::json_mapper;
std::vector<std::string> DecodePsbtOutput::item_list;

void DecodePsbtOutput::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodePsbtOutput> func_table;  // NOLINT

  func_table = {
    DecodePsbtOutput::GetRedeem_scriptString,
    DecodePsbtOutput::SetRedeem_scriptString,
    DecodePsbtOutput::GetRedeem_scriptFieldType,
  };
  json_mapper.emplace("redeem_script", func_table);
  item_list.push_back("redeem_script");
  func_table = {
    DecodePsbtOutput::GetWitness_scriptString,
    DecodePsbtOutput::SetWitness_scriptString,
    DecodePsbtOutput::GetWitness_scriptFieldType,
  };
  json_mapper.emplace("witness_script", func_table);
  item_list.push_back("witness_script");
  func_table = {
    DecodePsbtOutput::GetBip32_derivsString,
    DecodePsbtOutput::SetBip32_derivsString,
    DecodePsbtOutput::GetBip32_derivsFieldType,
  };
  json_mapper.emplace("bip32_derivs", func_table);
  item_list.push_back("bip32_derivs");
  func_table = {
    DecodePsbtOutput::GetUnknownString,
    DecodePsbtOutput::SetUnknownString,
    DecodePsbtOutput::GetUnknownFieldType,
  };
  json_mapper.emplace("unknown", func_table);
  item_list.push_back("unknown");
}

void DecodePsbtOutput::ConvertFromStruct(
    const DecodePsbtOutputStruct& data) {
  redeem_script_.ConvertFromStruct(data.redeem_script);
  witness_script_.ConvertFromStruct(data.witness_script);
  bip32_derivs_.ConvertFromStruct(data.bip32_derivs);
  unknown_.ConvertFromStruct(data.unknown);
  ignore_items = data.ignore_items;
}

DecodePsbtOutputStruct DecodePsbtOutput::ConvertToStruct() const {  // NOLINT
  DecodePsbtOutputStruct result;
  result.redeem_script = redeem_script_.ConvertToStruct();
  result.witness_script = witness_script_.ConvertToStruct();
  result.bip32_derivs = bip32_derivs_.ConvertToStruct();
  result.unknown = unknown_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionTxIn
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<ElementsDecodeRawTransactionTxIn>
  ElementsDecodeRawTransactionTxIn::json_mapper;
std::vector<std::string> ElementsDecodeRawTransactionTxIn::item_list;

void ElementsDecodeRawTransactionTxIn::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<ElementsDecodeRawTransactionTxIn> func_table;  // NOLINT

  func_table = {
    ElementsDecodeRawTransactionTxIn::GetCoinbaseString,
    ElementsDecodeRawTransactionTxIn::SetCoinbaseString,
    ElementsDecodeRawTransactionTxIn::GetCoinbaseFieldType,
  };
  json_mapper.emplace("coinbase", func_table);
  item_list.push_back("coinbase");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetTxidString,
    ElementsDecodeRawTransactionTxIn::SetTxidString,
    ElementsDecodeRawTransactionTxIn::GetTxidFieldType,
  };
  json_mapper.emplace("txid", func_table);
  item_list.push_back("txid");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetVoutString,
    ElementsDecodeRawTransactionTxIn::SetVoutString,
    ElementsDecodeRawTransactionTxIn::GetVoutFieldType,
  };
  json_mapper.emplace("vout", func_table);
  item_list.push_back("vout");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetScriptSigString,
    ElementsDecodeRawTransactionTxIn::SetScriptSigString,
    ElementsDecodeRawTransactionTxIn::GetScriptSigFieldType,
  };
  json_mapper.emplace("scriptSig", func_table);
  item_list.push_back("scriptSig");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetIs_peginString,
    ElementsDecodeRawTransactionTxIn::SetIs_peginString,
    ElementsDecodeRawTransactionTxIn::GetIs_peginFieldType,
  };
  json_mapper.emplace("is_pegin", func_table);
  item_list.push_back("is_pegin");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetSequenceString,
    ElementsDecodeRawTransactionTxIn::SetSequenceString,
    ElementsDecodeRawTransactionTxIn::GetSequenceFieldType,
  };
  json_mapper.emplace("sequence", func_table);
  item_list.push_back("sequence");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetTxinwitnessString,
    ElementsDecodeRawTransactionTxIn::SetTxinwitnessString,
    ElementsDecodeRawTransactionTxIn::GetTxinwitnessFieldType,
  };
  json_mapper.emplace("txinwitness", func_table);
  item_list.push_back("txinwitness");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetPegin_witnessString,
    ElementsDecodeRawTransactionTxIn::SetPegin_witnessString,
    ElementsDecodeRawTransactionTxIn::GetPegin_witnessFieldType,
  };
  json_mapper.emplace("pegin_witness", func_table);
  item_list.push_back("pegin_witness");
  func_table = {
    ElementsDecodeRawTransactionTxIn::GetIssuanceString,
    ElementsDecodeRawTransactionTxIn::SetIssuanceString,
    ElementsDecodeRawTransactionTxIn::GetIssuanceFieldType,
  };
  json_mapper.emplace("issuance", func_table);
  item_list.push_back("issuance");
}

void ElementsDecodeRawTransactionTxIn::ConvertFromStruct(
    const ElementsDecodeRawTransactionTxInStruct& data) {
  coinbase_ = data.coinbase;
  txid_ = data.txid;
  vout_ = data.vout;
  script_sig_.ConvertFromStruct(data.script_sig);
  is_pegin_ = data.is_pegin;
  sequence_ = data.sequence;
  txinwitness_.ConvertFromStruct(data.txinwitness);
  pegin_witness_.ConvertFromStruct(data.pegin_witness);
  issuance_.ConvertFromStruct(data.issuance);
  ignore_items = data.ignore_items;
}

ElementsDecodeRawTransactionTxInStruct ElementsDecodeRawTransactionTxIn::ConvertToStruct() const {  // NOLINT
  ElementsDecodeRawTransactionTxInStruct result;
  result.coinbase = coinbase_;
  result.txid = txid_;
  result.vout = vout_;
  result.script_sig = script_sig_.ConvertToStruct();
  result.is_pegin = is_pegin_;
  result.sequence = sequence_;
  result.txinwitness = txinwitness_.ConvertToStruct();
  result.pegin_witness = pegin_witness_.ConvertToStruct();
  result.issuance = issuance_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionTxOut
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<ElementsDecodeRawTransactionTxOut>
  ElementsDecodeRawTransactionTxOut::json_mapper;
std::vector<std::string> ElementsDecodeRawTransactionTxOut::item_list;

void ElementsDecodeRawTransactionTxOut::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<ElementsDecodeRawTransactionTxOut> func_table;  // NOLINT

  func_table = {
    ElementsDecodeRawTransactionTxOut::GetValueString,
    ElementsDecodeRawTransactionTxOut::SetValueString,
    ElementsDecodeRawTransactionTxOut::GetValueFieldType,
  };
  json_mapper.emplace("value", func_table);
  item_list.push_back("value");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetValue_minimumString,
    ElementsDecodeRawTransactionTxOut::SetValue_minimumString,
    ElementsDecodeRawTransactionTxOut::GetValue_minimumFieldType,
  };
  json_mapper.emplace("value-minimum", func_table);
  item_list.push_back("value-minimum");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetValue_maximumString,
    ElementsDecodeRawTransactionTxOut::SetValue_maximumString,
    ElementsDecodeRawTransactionTxOut::GetValue_maximumFieldType,
  };
  json_mapper.emplace("value-maximum", func_table);
  item_list.push_back("value-maximum");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetCt_exponentString,
    ElementsDecodeRawTransactionTxOut::SetCt_exponentString,
    ElementsDecodeRawTransactionTxOut::GetCt_exponentFieldType,
  };
  json_mapper.emplace("ct-exponent", func_table);
  item_list.push_back("ct-exponent");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetCt_bitsString,
    ElementsDecodeRawTransactionTxOut::SetCt_bitsString,
    ElementsDecodeRawTransactionTxOut::GetCt_bitsFieldType,
  };
  json_mapper.emplace("ct-bits", func_table);
  item_list.push_back("ct-bits");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetSurjectionproofString,
    ElementsDecodeRawTransactionTxOut::SetSurjectionproofString,
    ElementsDecodeRawTransactionTxOut::GetSurjectionproofFieldType,
  };
  json_mapper.emplace("surjectionproof", func_table);
  item_list.push_back("surjectionproof");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetValuecommitmentString,
    ElementsDecodeRawTransactionTxOut::SetValuecommitmentString,
    ElementsDecodeRawTransactionTxOut::GetValuecommitmentFieldType,
  };
  json_mapper.emplace("valuecommitment", func_table);
  item_list.push_back("valuecommitment");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetAssetString,
    ElementsDecodeRawTransactionTxOut::SetAssetString,
    ElementsDecodeRawTransactionTxOut::GetAssetFieldType,
  };
  json_mapper.emplace("asset", func_table);
  item_list.push_back("asset");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetAssetcommitmentString,
    ElementsDecodeRawTransactionTxOut::SetAssetcommitmentString,
    ElementsDecodeRawTransactionTxOut::GetAssetcommitmentFieldType,
  };
  json_mapper.emplace("assetcommitment", func_table);
  item_list.push_back("assetcommitment");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetCommitmentnonceString,
    ElementsDecodeRawTransactionTxOut::SetCommitmentnonceString,
    ElementsDecodeRawTransactionTxOut::GetCommitmentnonceFieldType,
  };
  json_mapper.emplace("commitmentnonce", func_table);
  item_list.push_back("commitmentnonce");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetCommitmentnonce_fully_validString,
    ElementsDecodeRawTransactionTxOut::SetCommitmentnonce_fully_validString,
    ElementsDecodeRawTransactionTxOut::GetCommitmentnonce_fully_validFieldType,
  };
  json_mapper.emplace("commitmentnonce_fully_valid", func_table);
  item_list.push_back("commitmentnonce_fully_valid");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetNString,
    ElementsDecodeRawTransactionTxOut::SetNString,
    ElementsDecodeRawTransactionTxOut::GetNFieldType,
  };
  json_mapper.emplace("n", func_table);
  item_list.push_back("n");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetScriptPubKeyString,
    ElementsDecodeRawTransactionTxOut::SetScriptPubKeyString,
    ElementsDecodeRawTransactionTxOut::GetScriptPubKeyFieldType,
  };
  json_mapper.emplace("scriptPubKey", func_table);
  item_list.push_back("scriptPubKey");
  func_table = {
    ElementsDecodeRawTransactionTxOut::GetRangeproofString,
    ElementsDecodeRawTransactionTxOut::SetRangeproofString,
    ElementsDecodeRawTransactionTxOut::GetRangeproofFieldType,
  };
  json_mapper.emplace("rangeproof", func_table);
  item_list.push_back("rangeproof");
}

void ElementsDecodeRawTransactionTxOut::ConvertFromStruct(
    const ElementsDecodeRawTransactionTxOutStruct& data) {
  value_ = data.value;
  value_minimum_ = data.value_minimum;
  value_maximum_ = data.value_maximum;
  ct_exponent_ = data.ct_exponent;
  ct_bits_ = data.ct_bits;
  surjectionproof_ = data.surjectionproof;
  valuecommitment_ = data.valuecommitment;
  asset_ = data.asset;
  assetcommitment_ = data.assetcommitment;
  commitmentnonce_ = data.commitmentnonce;
  commitmentnonce_fully_valid_ = data.commitmentnonce_fully_valid;
  n_ = data.n;
  script_pub_key_.ConvertFromStruct(data.script_pub_key);
  rangeproof_ = data.rangeproof;
  ignore_items = data.ignore_items;
}

ElementsDecodeRawTransactionTxOutStruct ElementsDecodeRawTransactionTxOut::ConvertToStruct() const {  // NOLINT
  ElementsDecodeRawTransactionTxOutStruct result;
  result.value = value_;
  result.value_minimum = value_minimum_;
  result.value_maximum = value_maximum_;
  result.ct_exponent = ct_exponent_;
  result.ct_bits = ct_bits_;
  result.surjectionproof = surjectionproof_;
  result.valuecommitment = valuecommitment_;
  result.asset = asset_;
  result.assetcommitment = assetcommitment_;
  result.commitmentnonce = commitmentnonce_;
  result.commitmentnonce_fully_valid = commitmentnonce_fully_valid_;
  result.n = n_;
  result.script_pub_key = script_pub_key_.ConvertToStruct();
  result.rangeproof = rangeproof_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// PsbtGlobalXpub
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<PsbtGlobalXpub>
  PsbtGlobalXpub::json_mapper;
std::vector<std::string> PsbtGlobalXpub::item_list;

void PsbtGlobalXpub::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<PsbtGlobalXpub> func_table;  // NOLINT

  func_table = {
    PsbtGlobalXpub::GetXpubString,
    PsbtGlobalXpub::SetXpubString,
    PsbtGlobalXpub::GetXpubFieldType,
  };
  json_mapper.emplace("xpub", func_table);
  item_list.push_back("xpub");
  func_table = {
    PsbtGlobalXpub::GetMaster_fingerprintString,
    PsbtGlobalXpub::SetMaster_fingerprintString,
    PsbtGlobalXpub::GetMaster_fingerprintFieldType,
  };
  json_mapper.emplace("master_fingerprint", func_table);
  item_list.push_back("master_fingerprint");
  func_table = {
    PsbtGlobalXpub::GetPathString,
    PsbtGlobalXpub::SetPathString,
    PsbtGlobalXpub::GetPathFieldType,
  };
  json_mapper.emplace("path", func_table);
  item_list.push_back("path");
  func_table = {
    PsbtGlobalXpub::GetDescriptorXpubString,
    PsbtGlobalXpub::SetDescriptorXpubString,
    PsbtGlobalXpub::GetDescriptorXpubFieldType,
  };
  json_mapper.emplace("descriptorXpub", func_table);
  item_list.push_back("descriptorXpub");
}

void PsbtGlobalXpub::ConvertFromStruct(
    const PsbtGlobalXpubStruct& data) {
  xpub_.ConvertFromStruct(data.xpub);
  master_fingerprint_ = data.master_fingerprint;
  path_ = data.path;
  descriptor_xpub_ = data.descriptor_xpub;
  ignore_items = data.ignore_items;
}

PsbtGlobalXpubStruct PsbtGlobalXpub::ConvertToStruct() const {  // NOLINT
  PsbtGlobalXpubStruct result;
  result.xpub = xpub_.ConvertToStruct();
  result.master_fingerprint = master_fingerprint_;
  result.path = path_;
  result.descriptor_xpub = descriptor_xpub_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodePsbtRequest
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodePsbtRequest>
  DecodePsbtRequest::json_mapper;
std::vector<std::string> DecodePsbtRequest::item_list;

void DecodePsbtRequest::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodePsbtRequest> func_table;  // NOLINT

  func_table = {
    DecodePsbtRequest::GetPsbtString,
    DecodePsbtRequest::SetPsbtString,
    DecodePsbtRequest::GetPsbtFieldType,
  };
  json_mapper.emplace("psbt", func_table);
  item_list.push_back("psbt");
  func_table = {
    DecodePsbtRequest::GetNetworkString,
    DecodePsbtRequest::SetNetworkString,
    DecodePsbtRequest::GetNetworkFieldType,
  };
  json_mapper.emplace("network", func_table);
  item_list.push_back("network");
  func_table = {
    DecodePsbtRequest::GetHasDetailString,
    DecodePsbtRequest::SetHasDetailString,
    DecodePsbtRequest::GetHasDetailFieldType,
  };
  json_mapper.emplace("hasDetail", func_table);
  item_list.push_back("hasDetail");
  func_table = {
    DecodePsbtRequest::GetHasSimpleString,
    DecodePsbtRequest::SetHasSimpleString,
    DecodePsbtRequest::GetHasSimpleFieldType,
  };
  json_mapper.emplace("hasSimple", func_table);
  item_list.push_back("hasSimple");
}

void DecodePsbtRequest::ConvertFromStruct(
    const DecodePsbtRequestStruct& data) {
  psbt_ = data.psbt;
  network_ = data.network;
  has_detail_ = data.has_detail;
  has_simple_ = data.has_simple;
  ignore_items = data.ignore_items;
}

DecodePsbtRequestStruct DecodePsbtRequest::ConvertToStruct() const {  // NOLINT
  DecodePsbtRequestStruct result;
  result.psbt = psbt_;
  result.network = network_;
  result.has_detail = has_detail_;
  result.has_simple = has_simple_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodePsbtResponse
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodePsbtResponse>
  DecodePsbtResponse::json_mapper;
std::vector<std::string> DecodePsbtResponse::item_list;

void DecodePsbtResponse::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodePsbtResponse> func_table;  // NOLINT

  func_table = {
    DecodePsbtResponse::GetTxString,
    DecodePsbtResponse::SetTxString,
    DecodePsbtResponse::GetTxFieldType,
  };
  json_mapper.emplace("tx", func_table);
  item_list.push_back("tx");
  func_table = {
    DecodePsbtResponse::GetTx_hexString,
    DecodePsbtResponse::SetTx_hexString,
    DecodePsbtResponse::GetTx_hexFieldType,
  };
  json_mapper.emplace("tx_hex", func_table);
  item_list.push_back("tx_hex");
  func_table = {
    DecodePsbtResponse::GetXpubsString,
    DecodePsbtResponse::SetXpubsString,
    DecodePsbtResponse::GetXpubsFieldType,
  };
  json_mapper.emplace("xpubs", func_table);
  item_list.push_back("xpubs");
  func_table = {
    DecodePsbtResponse::GetVersionString,
    DecodePsbtResponse::SetVersionString,
    DecodePsbtResponse::GetVersionFieldType,
  };
  json_mapper.emplace("version", func_table);
  item_list.push_back("version");
  func_table = {
    DecodePsbtResponse::GetUnknownString,
    DecodePsbtResponse::SetUnknownString,
    DecodePsbtResponse::GetUnknownFieldType,
  };
  json_mapper.emplace("unknown", func_table);
  item_list.push_back("unknown");
  func_table = {
    DecodePsbtResponse::GetInputsString,
    DecodePsbtResponse::SetInputsString,
    DecodePsbtResponse::GetInputsFieldType,
  };
  json_mapper.emplace("inputs", func_table);
  item_list.push_back("inputs");
  func_table = {
    DecodePsbtResponse::GetOutputsString,
    DecodePsbtResponse::SetOutputsString,
    DecodePsbtResponse::GetOutputsFieldType,
  };
  json_mapper.emplace("outputs", func_table);
  item_list.push_back("outputs");
  func_table = {
    DecodePsbtResponse::GetFeeString,
    DecodePsbtResponse::SetFeeString,
    DecodePsbtResponse::GetFeeFieldType,
  };
  json_mapper.emplace("fee", func_table);
  item_list.push_back("fee");
}

void DecodePsbtResponse::ConvertFromStruct(
    const DecodePsbtResponseStruct& data) {
  tx_.ConvertFromStruct(data.tx);
  tx_hex_ = data.tx_hex;
  xpubs_.ConvertFromStruct(data.xpubs);
  version_ = data.version;
  unknown_.ConvertFromStruct(data.unknown);
  inputs_.ConvertFromStruct(data.inputs);
  outputs_.ConvertFromStruct(data.outputs);
  fee_ = data.fee;
  ignore_items = data.ignore_items;
}

DecodePsbtResponseStruct DecodePsbtResponse::ConvertToStruct() const {  // NOLINT
  DecodePsbtResponseStruct result;
  result.tx = tx_.ConvertToStruct();
  result.tx_hex = tx_hex_;
  result.xpubs = xpubs_.ConvertToStruct();
  result.version = version_;
  result.unknown = unknown_.ConvertToStruct();
  result.inputs = inputs_.ConvertToStruct();
  result.outputs = outputs_.ConvertToStruct();
  result.fee = fee_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// DecodeRawTransactionRequest
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<DecodeRawTransactionRequest>
  DecodeRawTransactionRequest::json_mapper;
std::vector<std::string> DecodeRawTransactionRequest::item_list;

void DecodeRawTransactionRequest::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<DecodeRawTransactionRequest> func_table;  // NOLINT

  func_table = {
    DecodeRawTransactionRequest::GetHexString,
    DecodeRawTransactionRequest::SetHexString,
    DecodeRawTransactionRequest::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
  func_table = {
    DecodeRawTransactionRequest::GetNetworkString,
    DecodeRawTransactionRequest::SetNetworkString,
    DecodeRawTransactionRequest::GetNetworkFieldType,
  };
  json_mapper.emplace("network", func_table);
  item_list.push_back("network");
  func_table = {
    DecodeRawTransactionRequest::GetIswitnessString,
    DecodeRawTransactionRequest::SetIswitnessString,
    DecodeRawTransactionRequest::GetIswitnessFieldType,
  };
  json_mapper.emplace("iswitness", func_table);
  item_list.push_back("iswitness");
}

void DecodeRawTransactionRequest::ConvertFromStruct(
    const DecodeRawTransactionRequestStruct& data) {
  hex_ = data.hex;
  network_ = data.network;
  iswitness_ = data.iswitness;
  ignore_items = data.ignore_items;
}

DecodeRawTransactionRequestStruct DecodeRawTransactionRequest::ConvertToStruct() const {  // NOLINT
  DecodeRawTransactionRequestStruct result;
  result.hex = hex_;
  result.network = network_;
  result.iswitness = iswitness_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionRequest
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<ElementsDecodeRawTransactionRequest>
  ElementsDecodeRawTransactionRequest::json_mapper;
std::vector<std::string> ElementsDecodeRawTransactionRequest::item_list;

void ElementsDecodeRawTransactionRequest::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<ElementsDecodeRawTransactionRequest> func_table;  // NOLINT

  func_table = {
    ElementsDecodeRawTransactionRequest::GetHexString,
    ElementsDecodeRawTransactionRequest::SetHexString,
    ElementsDecodeRawTransactionRequest::GetHexFieldType,
  };
  json_mapper.emplace("hex", func_table);
  item_list.push_back("hex");
  func_table = {
    ElementsDecodeRawTransactionRequest::GetNetworkString,
    ElementsDecodeRawTransactionRequest::SetNetworkString,
    ElementsDecodeRawTransactionRequest::GetNetworkFieldType,
  };
  json_mapper.emplace("network", func_table);
  item_list.push_back("network");
  func_table = {
    ElementsDecodeRawTransactionRequest::GetMainchainNetworkString,
    ElementsDecodeRawTransactionRequest::SetMainchainNetworkString,
    ElementsDecodeRawTransactionRequest::GetMainchainNetworkFieldType,
  };
  json_mapper.emplace("mainchainNetwork", func_table);
  item_list.push_back("mainchainNetwork");
  func_table = {
    ElementsDecodeRawTransactionRequest::GetIswitnessString,
    ElementsDecodeRawTransactionRequest::SetIswitnessString,
    ElementsDecodeRawTransactionRequest::GetIswitnessFieldType,
  };
  json_mapper.emplace("iswitness", func_table);
  item_list.push_back("iswitness");
  func_table = {
    ElementsDecodeRawTransactionRequest::GetFullDumpString,
    ElementsDecodeRawTransactionRequest::SetFullDumpString,
    ElementsDecodeRawTransactionRequest::GetFullDumpFieldType,
  };
  json_mapper.emplace("fullDump", func_table);
  item_list.push_back("fullDump");
}

void ElementsDecodeRawTransactionRequest::ConvertFromStruct(
    const ElementsDecodeRawTransactionRequestStruct& data) {
  hex_ = data.hex;
  network_ = data.network;
  mainchain_network_ = data.mainchain_network;
  iswitness_ = data.iswitness;
  full_dump_ = data.full_dump;
  ignore_items = data.ignore_items;
}

ElementsDecodeRawTransactionRequestStruct ElementsDecodeRawTransactionRequest::ConvertToStruct() const {  // NOLINT
  ElementsDecodeRawTransactionRequestStruct result;
  result.hex = hex_;
  result.network = network_;
  result.mainchain_network = mainchain_network_;
  result.iswitness = iswitness_;
  result.full_dump = full_dump_;
  result.ignore_items = ignore_items;
  return result;
}

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionResponse
// ------------------------------------------------------------------------
cfd::core::JsonTableMap<ElementsDecodeRawTransactionResponse>
  ElementsDecodeRawTransactionResponse::json_mapper;
std::vector<std::string> ElementsDecodeRawTransactionResponse::item_list;

void ElementsDecodeRawTransactionResponse::CollectFieldName() {
  if (!json_mapper.empty()) {
    return;
  }
  cfd::core::CLASS_FUNCTION_TABLE<ElementsDecodeRawTransactionResponse> func_table;  // NOLINT

  func_table = {
    ElementsDecodeRawTransactionResponse::GetTxidString,
    ElementsDecodeRawTransactionResponse::SetTxidString,
    ElementsDecodeRawTransactionResponse::GetTxidFieldType,
  };
  json_mapper.emplace("txid", func_table);
  item_list.push_back("txid");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetHashString,
    ElementsDecodeRawTransactionResponse::SetHashString,
    ElementsDecodeRawTransactionResponse::GetHashFieldType,
  };
  json_mapper.emplace("hash", func_table);
  item_list.push_back("hash");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetWtxidString,
    ElementsDecodeRawTransactionResponse::SetWtxidString,
    ElementsDecodeRawTransactionResponse::GetWtxidFieldType,
  };
  json_mapper.emplace("wtxid", func_table);
  item_list.push_back("wtxid");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetWithashString,
    ElementsDecodeRawTransactionResponse::SetWithashString,
    ElementsDecodeRawTransactionResponse::GetWithashFieldType,
  };
  json_mapper.emplace("withash", func_table);
  item_list.push_back("withash");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetVersionString,
    ElementsDecodeRawTransactionResponse::SetVersionString,
    ElementsDecodeRawTransactionResponse::GetVersionFieldType,
  };
  json_mapper.emplace("version", func_table);
  item_list.push_back("version");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetSizeString,
    ElementsDecodeRawTransactionResponse::SetSizeString,
    ElementsDecodeRawTransactionResponse::GetSizeFieldType,
  };
  json_mapper.emplace("size", func_table);
  item_list.push_back("size");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetVsizeString,
    ElementsDecodeRawTransactionResponse::SetVsizeString,
    ElementsDecodeRawTransactionResponse::GetVsizeFieldType,
  };
  json_mapper.emplace("vsize", func_table);
  item_list.push_back("vsize");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetWeightString,
    ElementsDecodeRawTransactionResponse::SetWeightString,
    ElementsDecodeRawTransactionResponse::GetWeightFieldType,
  };
  json_mapper.emplace("weight", func_table);
  item_list.push_back("weight");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetLocktimeString,
    ElementsDecodeRawTransactionResponse::SetLocktimeString,
    ElementsDecodeRawTransactionResponse::GetLocktimeFieldType,
  };
  json_mapper.emplace("locktime", func_table);
  item_list.push_back("locktime");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetVinString,
    ElementsDecodeRawTransactionResponse::SetVinString,
    ElementsDecodeRawTransactionResponse::GetVinFieldType,
  };
  json_mapper.emplace("vin", func_table);
  item_list.push_back("vin");
  func_table = {
    ElementsDecodeRawTransactionResponse::GetVoutString,
    ElementsDecodeRawTransactionResponse::SetVoutString,
    ElementsDecodeRawTransactionResponse::GetVoutFieldType,
  };
  json_mapper.emplace("vout", func_table);
  item_list.push_back("vout");
}

void ElementsDecodeRawTransactionResponse::ConvertFromStruct(
    const ElementsDecodeRawTransactionResponseStruct& data) {
  txid_ = data.txid;
  hash_ = data.hash;
  wtxid_ = data.wtxid;
  withash_ = data.withash;
  version_ = data.version;
  size_ = data.size;
  vsize_ = data.vsize;
  weight_ = data.weight;
  locktime_ = data.locktime;
  vin_.ConvertFromStruct(data.vin);
  vout_.ConvertFromStruct(data.vout);
  ignore_items = data.ignore_items;
}

ElementsDecodeRawTransactionResponseStruct ElementsDecodeRawTransactionResponse::ConvertToStruct() const {  // NOLINT
  ElementsDecodeRawTransactionResponseStruct result;
  result.txid = txid_;
  result.hash = hash_;
  result.wtxid = wtxid_;
  result.withash = withash_;
  result.version = version_;
  result.size = size_;
  result.vsize = vsize_;
  result.weight = weight_;
  result.locktime = locktime_;
  result.vin = vin_.ConvertToStruct();
  result.vout = vout_.ConvertToStruct();
  result.ignore_items = ignore_items;
  return result;
}

// @formatter:on
// clang-format on

}  // namespace json
}  // namespace api
}  // namespace cfd
