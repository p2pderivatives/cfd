// Copyright 2019 CryptoGarage
/**
 * @file cfd_api_json_autogen.h
 *
 * @brief JSON mapping file. (auto generate)
 */
#ifndef CFD_SRC_JSONAPI_AUTOGEN_CFD_API_JSON_AUTOGEN_H_
#define CFD_SRC_JSONAPI_AUTOGEN_CFD_API_JSON_AUTOGEN_H_

#include <set>
#include <string>
#include <vector>

#include "cfdcore/cfdcore_json_mapping_base.h"

#include "jsonapi/cfd_struct.h"

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
// DecodeRawTransactionRequest
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodeRawTransactionRequest) class
 */
class DecodeRawTransactionRequest
  : public cfd::core::JsonClassBase<DecodeRawTransactionRequest> {
 public:
  DecodeRawTransactionRequest() {
    CollectFieldName();
  }
  virtual ~DecodeRawTransactionRequest() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of hex
   * @return hex
   */
  std::string GetHex() const {
    return hex_;
  }
  /**
   * @brief Set to hex
   * @param[in] hex    setting value.
   */
  void SetHex(  // line separate
    const std::string& hex) {  // NOLINT
    this->hex_ = hex;
  }
  /**
   * @brief Get data type of hex
   * @return Data type of hex
   */
  static std::string GetHexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHexString(  // line separate
      const DecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      DecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
  }

  /**
   * @brief Get of network
   * @return network
   */
  std::string GetNetwork() const {
    return network_;
  }
  /**
   * @brief Set to network
   * @param[in] network    setting value.
   */
  void SetNetwork(  // line separate
    const std::string& network) {  // NOLINT
    this->network_ = network;
  }
  /**
   * @brief Get data type of network
   * @return Data type of network
   */
  static std::string GetNetworkFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of network field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetNetworkString(  // line separate
      const DecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.network_);
  }
  /**
   * @brief Set json object to network field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetNetworkString(  // line separate
      DecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.network_, json_value);
  }

  /**
   * @brief Get of iswitness
   * @return iswitness
   */
  bool GetIswitness() const {
    return iswitness_;
  }
  /**
   * @brief Set to iswitness
   * @param[in] iswitness    setting value.
   */
  void SetIswitness(  // line separate
    const bool& iswitness) {  // NOLINT
    this->iswitness_ = iswitness;
  }
  /**
   * @brief Get data type of iswitness
   * @return Data type of iswitness
   */
  static std::string GetIswitnessFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of iswitness field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetIswitnessString(  // line separate
      const DecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.iswitness_);
  }
  /**
   * @brief Set json object to iswitness field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetIswitnessString(  // line separate
      DecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.iswitness_, json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const DecodeRawTransactionRequestStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodeRawTransactionRequestStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodeRawTransactionRequestMapTable =
    cfd::core::JsonTableMap<DecodeRawTransactionRequest>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodeRawTransactionRequestMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static DecodeRawTransactionRequestMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(hex) value
   */
  std::string hex_ = "";
  /**
   * @brief JsonAPI(network) value
   */
  std::string network_ = "mainnet";
  /**
   * @brief JsonAPI(iswitness) value
   */
  bool iswitness_ = true;
};

// ------------------------------------------------------------------------
// DecodeUnlockingScript
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodeUnlockingScript) class
 */
class DecodeUnlockingScript
  : public cfd::core::JsonClassBase<DecodeUnlockingScript> {
 public:
  DecodeUnlockingScript() {
    CollectFieldName();
  }
  virtual ~DecodeUnlockingScript() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of asm
   * @return asm
   */
  std::string GetAsm() const {
    return asm__;
  }
  /**
   * @brief Set to asm
   * @param[in] asm_    setting value.
   */
  void SetAsm(  // line separate
    const std::string& asm_) {  // NOLINT
    this->asm__ = asm_;
  }
  /**
   * @brief Get data type of asm
   * @return Data type of asm
   */
  static std::string GetAsmFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of asm field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAsmString(  // line separate
      const DecodeUnlockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asm__);
  }
  /**
   * @brief Set json object to asm field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAsmString(  // line separate
      DecodeUnlockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asm__, json_value);
  }

  /**
   * @brief Get of hex
   * @return hex
   */
  std::string GetHex() const {
    return hex_;
  }
  /**
   * @brief Set to hex
   * @param[in] hex    setting value.
   */
  void SetHex(  // line separate
    const std::string& hex) {  // NOLINT
    this->hex_ = hex;
  }
  /**
   * @brief Get data type of hex
   * @return Data type of hex
   */
  static std::string GetHexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHexString(  // line separate
      const DecodeUnlockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      DecodeUnlockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const DecodeUnlockingScriptStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodeUnlockingScriptStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodeUnlockingScriptMapTable =
    cfd::core::JsonTableMap<DecodeUnlockingScript>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodeUnlockingScriptMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static DecodeUnlockingScriptMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(asm) value
   */
  std::string asm__ = "";
  /**
   * @brief JsonAPI(hex) value
   */
  std::string hex_ = "";
};

// ------------------------------------------------------------------------
// DecodeRawTransactionTxIn
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodeRawTransactionTxIn) class
 */
class DecodeRawTransactionTxIn
  : public cfd::core::JsonClassBase<DecodeRawTransactionTxIn> {
 public:
  DecodeRawTransactionTxIn() {
    CollectFieldName();
  }
  virtual ~DecodeRawTransactionTxIn() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of coinbase
   * @return coinbase
   */
  std::string GetCoinbase() const {
    return coinbase_;
  }
  /**
   * @brief Set to coinbase
   * @param[in] coinbase    setting value.
   */
  void SetCoinbase(  // line separate
    const std::string& coinbase) {  // NOLINT
    this->coinbase_ = coinbase;
  }
  /**
   * @brief Get data type of coinbase
   * @return Data type of coinbase
   */
  static std::string GetCoinbaseFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of coinbase field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetCoinbaseString(  // line separate
      const DecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.coinbase_);
  }
  /**
   * @brief Set json object to coinbase field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetCoinbaseString(  // line separate
      DecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.coinbase_, json_value);
  }

  /**
   * @brief Get of txid
   * @return txid
   */
  std::string GetTxid() const {
    return txid_;
  }
  /**
   * @brief Set to txid
   * @param[in] txid    setting value.
   */
  void SetTxid(  // line separate
    const std::string& txid) {  // NOLINT
    this->txid_ = txid;
  }
  /**
   * @brief Get data type of txid
   * @return Data type of txid
   */
  static std::string GetTxidFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of txid field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTxidString(  // line separate
      const DecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.txid_);
  }
  /**
   * @brief Set json object to txid field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTxidString(  // line separate
      DecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.txid_, json_value);
  }

  /**
   * @brief Get of vout
   * @return vout
   */
  int64_t GetVout() const {
    return vout_;
  }
  /**
   * @brief Set to vout
   * @param[in] vout    setting value.
   */
  void SetVout(  // line separate
    const int64_t& vout) {  // NOLINT
    this->vout_ = vout;
  }
  /**
   * @brief Get data type of vout
   * @return Data type of vout
   */
  static std::string GetVoutFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of vout field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetVoutString(  // line separate
      const DecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.vout_);
  }
  /**
   * @brief Set json object to vout field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetVoutString(  // line separate
      DecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.vout_, json_value);
  }

  /**
   * @brief Get of scriptSig.
   * @return scriptSig
   */
  DecodeUnlockingScript& GetScriptSig() {  // NOLINT
    return script_sig_;
  }
  /**
   * @brief Set to scriptSig.
   * @param[in] script_sig    setting value.
   */
  void SetScriptSig(  // line separate
      const DecodeUnlockingScript& script_sig) {  // NOLINT
    this->script_sig_ = script_sig;
  }
  /**
   * @brief Get data type of scriptSig.
   * @return Data type of scriptSig.
   */
  static std::string GetScriptSigFieldType() {
    return "DecodeUnlockingScript";  // NOLINT
  }
  /**
   * @brief Get json string of scriptSig field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetScriptSigString(  // line separate
      const DecodeRawTransactionTxIn& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.script_sig_.Serialize();
  }
  /**
   * @brief Set json object to scriptSig field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetScriptSigString(  // line separate
      DecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    obj.script_sig_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of txinwitness.
   * @return txinwitness
   */
  JsonValueVector<std::string>& GetTxinwitness() {  // NOLINT
    return txinwitness_;
  }
  /**
   * @brief Set to txinwitness.
   * @param[in] txinwitness    setting value.
   */
  void SetTxinwitness(  // line separate
      const JsonValueVector<std::string>& txinwitness) {  // NOLINT
    this->txinwitness_ = txinwitness;
  }
  /**
   * @brief Get data type of txinwitness.
   * @return Data type of txinwitness.
   */
  static std::string GetTxinwitnessFieldType() {
    return "JsonValueVector<std::string>";  // NOLINT
  }
  /**
   * @brief Get json string of txinwitness field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetTxinwitnessString(  // line separate
      const DecodeRawTransactionTxIn& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.txinwitness_.Serialize();
  }
  /**
   * @brief Set json object to txinwitness field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetTxinwitnessString(  // line separate
      DecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    obj.txinwitness_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of sequence
   * @return sequence
   */
  int64_t GetSequence() const {
    return sequence_;
  }
  /**
   * @brief Set to sequence
   * @param[in] sequence    setting value.
   */
  void SetSequence(  // line separate
    const int64_t& sequence) {  // NOLINT
    this->sequence_ = sequence;
  }
  /**
   * @brief Get data type of sequence
   * @return Data type of sequence
   */
  static std::string GetSequenceFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of sequence field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetSequenceString(  // line separate
      const DecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.sequence_);
  }
  /**
   * @brief Set json object to sequence field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetSequenceString(  // line separate
      DecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.sequence_, json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const DecodeRawTransactionTxInStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodeRawTransactionTxInStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodeRawTransactionTxInMapTable =
    cfd::core::JsonTableMap<DecodeRawTransactionTxIn>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodeRawTransactionTxInMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static DecodeRawTransactionTxInMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(coinbase) value
   */
  std::string coinbase_ = "";
  /**
   * @brief JsonAPI(txid) value
   */
  std::string txid_ = "";
  /**
   * @brief JsonAPI(vout) value
   */
  int64_t vout_ = 0;
  /**
   * @brief JsonAPI(scriptSig) value
   */
  DecodeUnlockingScript script_sig_;  // NOLINT
  /**
   * @brief JsonAPI(txinwitness) value
   */
  JsonValueVector<std::string> txinwitness_;  // NOLINT
  /**
   * @brief JsonAPI(sequence) value
   */
  int64_t sequence_ = 0;
};

// ------------------------------------------------------------------------
// DecodeLockingScript
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodeLockingScript) class
 */
class DecodeLockingScript
  : public cfd::core::JsonClassBase<DecodeLockingScript> {
 public:
  DecodeLockingScript() {
    CollectFieldName();
  }
  virtual ~DecodeLockingScript() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of asm
   * @return asm
   */
  std::string GetAsm() const {
    return asm__;
  }
  /**
   * @brief Set to asm
   * @param[in] asm_    setting value.
   */
  void SetAsm(  // line separate
    const std::string& asm_) {  // NOLINT
    this->asm__ = asm_;
  }
  /**
   * @brief Get data type of asm
   * @return Data type of asm
   */
  static std::string GetAsmFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of asm field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAsmString(  // line separate
      const DecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asm__);
  }
  /**
   * @brief Set json object to asm field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAsmString(  // line separate
      DecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asm__, json_value);
  }

  /**
   * @brief Get of hex
   * @return hex
   */
  std::string GetHex() const {
    return hex_;
  }
  /**
   * @brief Set to hex
   * @param[in] hex    setting value.
   */
  void SetHex(  // line separate
    const std::string& hex) {  // NOLINT
    this->hex_ = hex;
  }
  /**
   * @brief Get data type of hex
   * @return Data type of hex
   */
  static std::string GetHexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHexString(  // line separate
      const DecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      DecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
  }

  /**
   * @brief Get of reqSigs
   * @return reqSigs
   */
  int64_t GetReqSigs() const {
    return req_sigs_;
  }
  /**
   * @brief Set to reqSigs
   * @param[in] req_sigs    setting value.
   */
  void SetReqSigs(  // line separate
    const int64_t& req_sigs) {  // NOLINT
    this->req_sigs_ = req_sigs;
  }
  /**
   * @brief Get data type of reqSigs
   * @return Data type of reqSigs
   */
  static std::string GetReqSigsFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of reqSigs field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetReqSigsString(  // line separate
      const DecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.req_sigs_);
  }
  /**
   * @brief Set json object to reqSigs field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetReqSigsString(  // line separate
      DecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.req_sigs_, json_value);
  }

  /**
   * @brief Get of type
   * @return type
   */
  std::string GetType() const {
    return type_;
  }
  /**
   * @brief Set to type
   * @param[in] type    setting value.
   */
  void SetType(  // line separate
    const std::string& type) {  // NOLINT
    this->type_ = type;
  }
  /**
   * @brief Get data type of type
   * @return Data type of type
   */
  static std::string GetTypeFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of type field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTypeString(  // line separate
      const DecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.type_);
  }
  /**
   * @brief Set json object to type field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTypeString(  // line separate
      DecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.type_, json_value);
  }

  /**
   * @brief Get of addresses.
   * @return addresses
   */
  JsonValueVector<std::string>& GetAddresses() {  // NOLINT
    return addresses_;
  }
  /**
   * @brief Set to addresses.
   * @param[in] addresses    setting value.
   */
  void SetAddresses(  // line separate
      const JsonValueVector<std::string>& addresses) {  // NOLINT
    this->addresses_ = addresses;
  }
  /**
   * @brief Get data type of addresses.
   * @return Data type of addresses.
   */
  static std::string GetAddressesFieldType() {
    return "JsonValueVector<std::string>";  // NOLINT
  }
  /**
   * @brief Get json string of addresses field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetAddressesString(  // line separate
      const DecodeLockingScript& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.addresses_.Serialize();
  }
  /**
   * @brief Set json object to addresses field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetAddressesString(  // line separate
      DecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    obj.addresses_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const DecodeLockingScriptStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodeLockingScriptStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodeLockingScriptMapTable =
    cfd::core::JsonTableMap<DecodeLockingScript>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodeLockingScriptMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static DecodeLockingScriptMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(asm) value
   */
  std::string asm__ = "";
  /**
   * @brief JsonAPI(hex) value
   */
  std::string hex_ = "";
  /**
   * @brief JsonAPI(reqSigs) value
   */
  int64_t req_sigs_ = 0;
  /**
   * @brief JsonAPI(type) value
   */
  std::string type_ = "";
  /**
   * @brief JsonAPI(addresses) value
   */
  JsonValueVector<std::string> addresses_;  // NOLINT
};

// ------------------------------------------------------------------------
// DecodeRawTransactionTxOut
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodeRawTransactionTxOut) class
 */
class DecodeRawTransactionTxOut
  : public cfd::core::JsonClassBase<DecodeRawTransactionTxOut> {
 public:
  DecodeRawTransactionTxOut() {
    CollectFieldName();
  }
  virtual ~DecodeRawTransactionTxOut() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of value
   * @return value
   */
  double GetValue() const {
    return value_;
  }
  /**
   * @brief Set to value
   * @param[in] value    setting value.
   */
  void SetValue(  // line separate
    const double& value) {  // NOLINT
    this->value_ = value;
  }
  /**
   * @brief Get data type of value
   * @return Data type of value
   */
  static std::string GetValueFieldType() {
    return "double";
  }
  /**
   * @brief Get json string of value field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetValueString(  // line separate
      const DecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.value_);
  }
  /**
   * @brief Set json object to value field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetValueString(  // line separate
      DecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.value_, json_value);
  }

  /**
   * @brief Get of n
   * @return n
   */
  int64_t GetN() const {
    return n_;
  }
  /**
   * @brief Set to n
   * @param[in] n    setting value.
   */
  void SetN(  // line separate
    const int64_t& n) {  // NOLINT
    this->n_ = n;
  }
  /**
   * @brief Get data type of n
   * @return Data type of n
   */
  static std::string GetNFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of n field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetNString(  // line separate
      const DecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.n_);
  }
  /**
   * @brief Set json object to n field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetNString(  // line separate
      DecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.n_, json_value);
  }

  /**
   * @brief Get of scriptPubKey.
   * @return scriptPubKey
   */
  DecodeLockingScript& GetScriptPubKey() {  // NOLINT
    return script_pub_key_;
  }
  /**
   * @brief Set to scriptPubKey.
   * @param[in] script_pub_key    setting value.
   */
  void SetScriptPubKey(  // line separate
      const DecodeLockingScript& script_pub_key) {  // NOLINT
    this->script_pub_key_ = script_pub_key;
  }
  /**
   * @brief Get data type of scriptPubKey.
   * @return Data type of scriptPubKey.
   */
  static std::string GetScriptPubKeyFieldType() {
    return "DecodeLockingScript";  // NOLINT
  }
  /**
   * @brief Get json string of scriptPubKey field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetScriptPubKeyString(  // line separate
      const DecodeRawTransactionTxOut& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.script_pub_key_.Serialize();
  }
  /**
   * @brief Set json object to scriptPubKey field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetScriptPubKeyString(  // line separate
      DecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    obj.script_pub_key_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const DecodeRawTransactionTxOutStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodeRawTransactionTxOutStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodeRawTransactionTxOutMapTable =
    cfd::core::JsonTableMap<DecodeRawTransactionTxOut>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodeRawTransactionTxOutMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static DecodeRawTransactionTxOutMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(value) value
   */
  double value_ = 0;
  /**
   * @brief JsonAPI(n) value
   */
  int64_t n_ = 0;
  /**
   * @brief JsonAPI(scriptPubKey) value
   */
  DecodeLockingScript script_pub_key_;  // NOLINT
};

// ------------------------------------------------------------------------
// DecodeRawTransactionResponse
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodeRawTransactionResponse) class
 */
class DecodeRawTransactionResponse
  : public cfd::core::JsonClassBase<DecodeRawTransactionResponse> {
 public:
  DecodeRawTransactionResponse() {
    CollectFieldName();
  }
  virtual ~DecodeRawTransactionResponse() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of txid
   * @return txid
   */
  std::string GetTxid() const {
    return txid_;
  }
  /**
   * @brief Set to txid
   * @param[in] txid    setting value.
   */
  void SetTxid(  // line separate
    const std::string& txid) {  // NOLINT
    this->txid_ = txid;
  }
  /**
   * @brief Get data type of txid
   * @return Data type of txid
   */
  static std::string GetTxidFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of txid field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTxidString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.txid_);
  }
  /**
   * @brief Set json object to txid field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTxidString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.txid_, json_value);
  }

  /**
   * @brief Get of hash
   * @return hash
   */
  std::string GetHash() const {
    return hash_;
  }
  /**
   * @brief Set to hash
   * @param[in] hash    setting value.
   */
  void SetHash(  // line separate
    const std::string& hash) {  // NOLINT
    this->hash_ = hash;
  }
  /**
   * @brief Get data type of hash
   * @return Data type of hash
   */
  static std::string GetHashFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hash field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHashString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hash_);
  }
  /**
   * @brief Set json object to hash field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHashString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hash_, json_value);
  }

  /**
   * @brief Get of version
   * @return version
   */
  uint32_t GetVersion() const {
    return version_;
  }
  /**
   * @brief Set to version
   * @param[in] version    setting value.
   */
  void SetVersion(  // line separate
    const uint32_t& version) {  // NOLINT
    this->version_ = version;
  }
  /**
   * @brief Get data type of version
   * @return Data type of version
   */
  static std::string GetVersionFieldType() {
    return "uint32_t";
  }
  /**
   * @brief Get json string of version field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetVersionString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.version_);
  }
  /**
   * @brief Set json object to version field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetVersionString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.version_, json_value);
  }

  /**
   * @brief Get of size
   * @return size
   */
  int64_t GetSize() const {
    return size_;
  }
  /**
   * @brief Set to size
   * @param[in] size    setting value.
   */
  void SetSize(  // line separate
    const int64_t& size) {  // NOLINT
    this->size_ = size;
  }
  /**
   * @brief Get data type of size
   * @return Data type of size
   */
  static std::string GetSizeFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of size field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetSizeString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.size_);
  }
  /**
   * @brief Set json object to size field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetSizeString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.size_, json_value);
  }

  /**
   * @brief Get of vsize
   * @return vsize
   */
  int64_t GetVsize() const {
    return vsize_;
  }
  /**
   * @brief Set to vsize
   * @param[in] vsize    setting value.
   */
  void SetVsize(  // line separate
    const int64_t& vsize) {  // NOLINT
    this->vsize_ = vsize;
  }
  /**
   * @brief Get data type of vsize
   * @return Data type of vsize
   */
  static std::string GetVsizeFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of vsize field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetVsizeString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.vsize_);
  }
  /**
   * @brief Set json object to vsize field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetVsizeString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.vsize_, json_value);
  }

  /**
   * @brief Get of weight
   * @return weight
   */
  int64_t GetWeight() const {
    return weight_;
  }
  /**
   * @brief Set to weight
   * @param[in] weight    setting value.
   */
  void SetWeight(  // line separate
    const int64_t& weight) {  // NOLINT
    this->weight_ = weight;
  }
  /**
   * @brief Get data type of weight
   * @return Data type of weight
   */
  static std::string GetWeightFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of weight field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetWeightString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.weight_);
  }
  /**
   * @brief Set json object to weight field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetWeightString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.weight_, json_value);
  }

  /**
   * @brief Get of locktime
   * @return locktime
   */
  uint32_t GetLocktime() const {
    return locktime_;
  }
  /**
   * @brief Set to locktime
   * @param[in] locktime    setting value.
   */
  void SetLocktime(  // line separate
    const uint32_t& locktime) {  // NOLINT
    this->locktime_ = locktime;
  }
  /**
   * @brief Get data type of locktime
   * @return Data type of locktime
   */
  static std::string GetLocktimeFieldType() {
    return "uint32_t";
  }
  /**
   * @brief Get json string of locktime field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetLocktimeString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.locktime_);
  }
  /**
   * @brief Set json object to locktime field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetLocktimeString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.locktime_, json_value);
  }

  /**
   * @brief Get of vin.
   * @return vin
   */
  JsonObjectVector<DecodeRawTransactionTxIn, DecodeRawTransactionTxInStruct>& GetVin() {  // NOLINT
    return vin_;
  }
  /**
   * @brief Set to vin.
   * @param[in] vin    setting value.
   */
  void SetVin(  // line separate
      const JsonObjectVector<DecodeRawTransactionTxIn, DecodeRawTransactionTxInStruct>& vin) {  // NOLINT
    this->vin_ = vin;
  }
  /**
   * @brief Get data type of vin.
   * @return Data type of vin.
   */
  static std::string GetVinFieldType() {
    return "JsonObjectVector<DecodeRawTransactionTxIn, DecodeRawTransactionTxInStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of vin field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetVinString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.vin_.Serialize();
  }
  /**
   * @brief Set json object to vin field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetVinString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.vin_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of vout.
   * @return vout
   */
  JsonObjectVector<DecodeRawTransactionTxOut, DecodeRawTransactionTxOutStruct>& GetVout() {  // NOLINT
    return vout_;
  }
  /**
   * @brief Set to vout.
   * @param[in] vout    setting value.
   */
  void SetVout(  // line separate
      const JsonObjectVector<DecodeRawTransactionTxOut, DecodeRawTransactionTxOutStruct>& vout) {  // NOLINT
    this->vout_ = vout;
  }
  /**
   * @brief Get data type of vout.
   * @return Data type of vout.
   */
  static std::string GetVoutFieldType() {
    return "JsonObjectVector<DecodeRawTransactionTxOut, DecodeRawTransactionTxOutStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of vout field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetVoutString(  // line separate
      const DecodeRawTransactionResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.vout_.Serialize();
  }
  /**
   * @brief Set json object to vout field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetVoutString(  // line separate
      DecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.vout_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const DecodeRawTransactionResponseStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodeRawTransactionResponseStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodeRawTransactionResponseMapTable =
    cfd::core::JsonTableMap<DecodeRawTransactionResponse>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodeRawTransactionResponseMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static DecodeRawTransactionResponseMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(txid) value
   */
  std::string txid_ = "";
  /**
   * @brief JsonAPI(hash) value
   */
  std::string hash_ = "";
  /**
   * @brief JsonAPI(version) value
   */
  uint32_t version_ = 0;
  /**
   * @brief JsonAPI(size) value
   */
  int64_t size_ = 0;
  /**
   * @brief JsonAPI(vsize) value
   */
  int64_t vsize_ = 0;
  /**
   * @brief JsonAPI(weight) value
   */
  int64_t weight_ = 0;
  /**
   * @brief JsonAPI(locktime) value
   */
  uint32_t locktime_ = 0;
  /**
   * @brief JsonAPI(vin) value
   */
  JsonObjectVector<DecodeRawTransactionTxIn, DecodeRawTransactionTxInStruct> vin_;  // NOLINT
  /**
   * @brief JsonAPI(vout) value
   */
  JsonObjectVector<DecodeRawTransactionTxOut, DecodeRawTransactionTxOutStruct> vout_;  // NOLINT
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionRequest
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (ElementsDecodeRawTransactionRequest) class
 */
class ElementsDecodeRawTransactionRequest
  : public cfd::core::JsonClassBase<ElementsDecodeRawTransactionRequest> {
 public:
  ElementsDecodeRawTransactionRequest() {
    CollectFieldName();
  }
  virtual ~ElementsDecodeRawTransactionRequest() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of hex
   * @return hex
   */
  std::string GetHex() const {
    return hex_;
  }
  /**
   * @brief Set to hex
   * @param[in] hex    setting value.
   */
  void SetHex(  // line separate
    const std::string& hex) {  // NOLINT
    this->hex_ = hex;
  }
  /**
   * @brief Get data type of hex
   * @return Data type of hex
   */
  static std::string GetHexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHexString(  // line separate
      const ElementsDecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      ElementsDecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
  }

  /**
   * @brief Get of network
   * @return network
   */
  std::string GetNetwork() const {
    return network_;
  }
  /**
   * @brief Set to network
   * @param[in] network    setting value.
   */
  void SetNetwork(  // line separate
    const std::string& network) {  // NOLINT
    this->network_ = network;
  }
  /**
   * @brief Get data type of network
   * @return Data type of network
   */
  static std::string GetNetworkFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of network field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetNetworkString(  // line separate
      const ElementsDecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.network_);
  }
  /**
   * @brief Set json object to network field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetNetworkString(  // line separate
      ElementsDecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.network_, json_value);
  }

  /**
   * @brief Get of mainchainNetwork
   * @return mainchainNetwork
   */
  std::string GetMainchainNetwork() const {
    return mainchain_network_;
  }
  /**
   * @brief Set to mainchainNetwork
   * @param[in] mainchain_network    setting value.
   */
  void SetMainchainNetwork(  // line separate
    const std::string& mainchain_network) {  // NOLINT
    this->mainchain_network_ = mainchain_network;
  }
  /**
   * @brief Get data type of mainchainNetwork
   * @return Data type of mainchainNetwork
   */
  static std::string GetMainchainNetworkFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of mainchainNetwork field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetMainchainNetworkString(  // line separate
      const ElementsDecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.mainchain_network_);
  }
  /**
   * @brief Set json object to mainchainNetwork field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetMainchainNetworkString(  // line separate
      ElementsDecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.mainchain_network_, json_value);
  }

  /**
   * @brief Get of iswitness
   * @return iswitness
   */
  bool GetIswitness() const {
    return iswitness_;
  }
  /**
   * @brief Set to iswitness
   * @param[in] iswitness    setting value.
   */
  void SetIswitness(  // line separate
    const bool& iswitness) {  // NOLINT
    this->iswitness_ = iswitness;
  }
  /**
   * @brief Get data type of iswitness
   * @return Data type of iswitness
   */
  static std::string GetIswitnessFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of iswitness field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetIswitnessString(  // line separate
      const ElementsDecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.iswitness_);
  }
  /**
   * @brief Set json object to iswitness field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetIswitnessString(  // line separate
      ElementsDecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.iswitness_, json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ElementsDecodeRawTransactionRequestStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ElementsDecodeRawTransactionRequestStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ElementsDecodeRawTransactionRequestMapTable =
    cfd::core::JsonTableMap<ElementsDecodeRawTransactionRequest>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ElementsDecodeRawTransactionRequestMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ElementsDecodeRawTransactionRequestMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(hex) value
   */
  std::string hex_ = "";
  /**
   * @brief JsonAPI(network) value
   */
  std::string network_ = "liquidv1";
  /**
   * @brief JsonAPI(mainchainNetwork) value
   */
  std::string mainchain_network_ = "";
  /**
   * @brief JsonAPI(iswitness) value
   */
  bool iswitness_ = true;
};

// ------------------------------------------------------------------------
// ElementsDecodeUnlockingScript
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (ElementsDecodeUnlockingScript) class
 */
class ElementsDecodeUnlockingScript
  : public cfd::core::JsonClassBase<ElementsDecodeUnlockingScript> {
 public:
  ElementsDecodeUnlockingScript() {
    CollectFieldName();
  }
  virtual ~ElementsDecodeUnlockingScript() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of asm
   * @return asm
   */
  std::string GetAsm() const {
    return asm__;
  }
  /**
   * @brief Set to asm
   * @param[in] asm_    setting value.
   */
  void SetAsm(  // line separate
    const std::string& asm_) {  // NOLINT
    this->asm__ = asm_;
  }
  /**
   * @brief Get data type of asm
   * @return Data type of asm
   */
  static std::string GetAsmFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of asm field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAsmString(  // line separate
      const ElementsDecodeUnlockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asm__);
  }
  /**
   * @brief Set json object to asm field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAsmString(  // line separate
      ElementsDecodeUnlockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asm__, json_value);
  }

  /**
   * @brief Get of hex
   * @return hex
   */
  std::string GetHex() const {
    return hex_;
  }
  /**
   * @brief Set to hex
   * @param[in] hex    setting value.
   */
  void SetHex(  // line separate
    const std::string& hex) {  // NOLINT
    this->hex_ = hex;
  }
  /**
   * @brief Get data type of hex
   * @return Data type of hex
   */
  static std::string GetHexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHexString(  // line separate
      const ElementsDecodeUnlockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      ElementsDecodeUnlockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ElementsDecodeUnlockingScriptStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ElementsDecodeUnlockingScriptStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ElementsDecodeUnlockingScriptMapTable =
    cfd::core::JsonTableMap<ElementsDecodeUnlockingScript>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ElementsDecodeUnlockingScriptMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ElementsDecodeUnlockingScriptMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(asm) value
   */
  std::string asm__ = "";
  /**
   * @brief JsonAPI(hex) value
   */
  std::string hex_ = "";
};

// ------------------------------------------------------------------------
// ElementsDecodeIssuance
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (ElementsDecodeIssuance) class
 */
class ElementsDecodeIssuance
  : public cfd::core::JsonClassBase<ElementsDecodeIssuance> {
 public:
  ElementsDecodeIssuance() {
    CollectFieldName();
  }
  virtual ~ElementsDecodeIssuance() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of assetBlindingNonce
   * @return assetBlindingNonce
   */
  std::string GetAssetBlindingNonce() const {
    return asset_blinding_nonce_;
  }
  /**
   * @brief Set to assetBlindingNonce
   * @param[in] asset_blinding_nonce    setting value.
   */
  void SetAssetBlindingNonce(  // line separate
    const std::string& asset_blinding_nonce) {  // NOLINT
    this->asset_blinding_nonce_ = asset_blinding_nonce;
  }
  /**
   * @brief Get data type of assetBlindingNonce
   * @return Data type of assetBlindingNonce
   */
  static std::string GetAssetBlindingNonceFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of assetBlindingNonce field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetBlindingNonceString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asset_blinding_nonce_);
  }
  /**
   * @brief Set json object to assetBlindingNonce field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetBlindingNonceString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asset_blinding_nonce_, json_value);
  }

  /**
   * @brief Get of assetEntropy
   * @return assetEntropy
   */
  std::string GetAssetEntropy() const {
    return asset_entropy_;
  }
  /**
   * @brief Set to assetEntropy
   * @param[in] asset_entropy    setting value.
   */
  void SetAssetEntropy(  // line separate
    const std::string& asset_entropy) {  // NOLINT
    this->asset_entropy_ = asset_entropy;
  }
  /**
   * @brief Get data type of assetEntropy
   * @return Data type of assetEntropy
   */
  static std::string GetAssetEntropyFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of assetEntropy field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetEntropyString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asset_entropy_);
  }
  /**
   * @brief Set json object to assetEntropy field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetEntropyString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asset_entropy_, json_value);
  }

  /**
   * @brief Get of isreissuance
   * @return isreissuance
   */
  bool GetIsreissuance() const {
    return isreissuance_;
  }
  /**
   * @brief Set to isreissuance
   * @param[in] isreissuance    setting value.
   */
  void SetIsreissuance(  // line separate
    const bool& isreissuance) {  // NOLINT
    this->isreissuance_ = isreissuance;
  }
  /**
   * @brief Get data type of isreissuance
   * @return Data type of isreissuance
   */
  static std::string GetIsreissuanceFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of isreissuance field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetIsreissuanceString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.isreissuance_);
  }
  /**
   * @brief Set json object to isreissuance field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetIsreissuanceString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.isreissuance_, json_value);
  }

  /**
   * @brief Get of token
   * @return token
   */
  std::string GetToken() const {
    return token_;
  }
  /**
   * @brief Set to token
   * @param[in] token    setting value.
   */
  void SetToken(  // line separate
    const std::string& token) {  // NOLINT
    this->token_ = token;
  }
  /**
   * @brief Get data type of token
   * @return Data type of token
   */
  static std::string GetTokenFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of token field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTokenString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.token_);
  }
  /**
   * @brief Set json object to token field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTokenString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.token_, json_value);
  }

  /**
   * @brief Get of asset
   * @return asset
   */
  std::string GetAsset() const {
    return asset_;
  }
  /**
   * @brief Set to asset
   * @param[in] asset    setting value.
   */
  void SetAsset(  // line separate
    const std::string& asset) {  // NOLINT
    this->asset_ = asset;
  }
  /**
   * @brief Get data type of asset
   * @return Data type of asset
   */
  static std::string GetAssetFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of asset field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asset_);
  }
  /**
   * @brief Set json object to asset field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asset_, json_value);
  }

  /**
   * @brief Get of assetamount
   * @return assetamount
   */
  double GetAssetamount() const {
    return assetamount_;
  }
  /**
   * @brief Set to assetamount
   * @param[in] assetamount    setting value.
   */
  void SetAssetamount(  // line separate
    const double& assetamount) {  // NOLINT
    this->assetamount_ = assetamount;
  }
  /**
   * @brief Get data type of assetamount
   * @return Data type of assetamount
   */
  static std::string GetAssetamountFieldType() {
    return "double";
  }
  /**
   * @brief Get json string of assetamount field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetamountString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.assetamount_);
  }
  /**
   * @brief Set json object to assetamount field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetamountString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.assetamount_, json_value);
  }

  /**
   * @brief Get of assetamountcommitment
   * @return assetamountcommitment
   */
  std::string GetAssetamountcommitment() const {
    return assetamountcommitment_;
  }
  /**
   * @brief Set to assetamountcommitment
   * @param[in] assetamountcommitment    setting value.
   */
  void SetAssetamountcommitment(  // line separate
    const std::string& assetamountcommitment) {  // NOLINT
    this->assetamountcommitment_ = assetamountcommitment;
  }
  /**
   * @brief Get data type of assetamountcommitment
   * @return Data type of assetamountcommitment
   */
  static std::string GetAssetamountcommitmentFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of assetamountcommitment field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetamountcommitmentString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.assetamountcommitment_);
  }
  /**
   * @brief Set json object to assetamountcommitment field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetamountcommitmentString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.assetamountcommitment_, json_value);
  }

  /**
   * @brief Get of tokenamount
   * @return tokenamount
   */
  double GetTokenamount() const {
    return tokenamount_;
  }
  /**
   * @brief Set to tokenamount
   * @param[in] tokenamount    setting value.
   */
  void SetTokenamount(  // line separate
    const double& tokenamount) {  // NOLINT
    this->tokenamount_ = tokenamount;
  }
  /**
   * @brief Get data type of tokenamount
   * @return Data type of tokenamount
   */
  static std::string GetTokenamountFieldType() {
    return "double";
  }
  /**
   * @brief Get json string of tokenamount field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTokenamountString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.tokenamount_);
  }
  /**
   * @brief Set json object to tokenamount field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTokenamountString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.tokenamount_, json_value);
  }

  /**
   * @brief Get of tokenamountcommitment
   * @return tokenamountcommitment
   */
  std::string GetTokenamountcommitment() const {
    return tokenamountcommitment_;
  }
  /**
   * @brief Set to tokenamountcommitment
   * @param[in] tokenamountcommitment    setting value.
   */
  void SetTokenamountcommitment(  // line separate
    const std::string& tokenamountcommitment) {  // NOLINT
    this->tokenamountcommitment_ = tokenamountcommitment;
  }
  /**
   * @brief Get data type of tokenamountcommitment
   * @return Data type of tokenamountcommitment
   */
  static std::string GetTokenamountcommitmentFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of tokenamountcommitment field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTokenamountcommitmentString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.tokenamountcommitment_);
  }
  /**
   * @brief Set json object to tokenamountcommitment field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTokenamountcommitmentString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.tokenamountcommitment_, json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ElementsDecodeIssuanceStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ElementsDecodeIssuanceStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ElementsDecodeIssuanceMapTable =
    cfd::core::JsonTableMap<ElementsDecodeIssuance>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ElementsDecodeIssuanceMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ElementsDecodeIssuanceMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(assetBlindingNonce) value
   */
  std::string asset_blinding_nonce_ = "";
  /**
   * @brief JsonAPI(assetEntropy) value
   */
  std::string asset_entropy_ = "";
  /**
   * @brief JsonAPI(isreissuance) value
   */
  bool isreissuance_ = false;
  /**
   * @brief JsonAPI(token) value
   */
  std::string token_ = "";
  /**
   * @brief JsonAPI(asset) value
   */
  std::string asset_ = "";
  /**
   * @brief JsonAPI(assetamount) value
   */
  double assetamount_ = 0;
  /**
   * @brief JsonAPI(assetamountcommitment) value
   */
  std::string assetamountcommitment_ = "";
  /**
   * @brief JsonAPI(tokenamount) value
   */
  double tokenamount_ = 0;
  /**
   * @brief JsonAPI(tokenamountcommitment) value
   */
  std::string tokenamountcommitment_ = "";
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionTxIn
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (ElementsDecodeRawTransactionTxIn) class
 */
class ElementsDecodeRawTransactionTxIn
  : public cfd::core::JsonClassBase<ElementsDecodeRawTransactionTxIn> {
 public:
  ElementsDecodeRawTransactionTxIn() {
    CollectFieldName();
  }
  virtual ~ElementsDecodeRawTransactionTxIn() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of coinbase
   * @return coinbase
   */
  std::string GetCoinbase() const {
    return coinbase_;
  }
  /**
   * @brief Set to coinbase
   * @param[in] coinbase    setting value.
   */
  void SetCoinbase(  // line separate
    const std::string& coinbase) {  // NOLINT
    this->coinbase_ = coinbase;
  }
  /**
   * @brief Get data type of coinbase
   * @return Data type of coinbase
   */
  static std::string GetCoinbaseFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of coinbase field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetCoinbaseString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.coinbase_);
  }
  /**
   * @brief Set json object to coinbase field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetCoinbaseString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.coinbase_, json_value);
  }

  /**
   * @brief Get of txid
   * @return txid
   */
  std::string GetTxid() const {
    return txid_;
  }
  /**
   * @brief Set to txid
   * @param[in] txid    setting value.
   */
  void SetTxid(  // line separate
    const std::string& txid) {  // NOLINT
    this->txid_ = txid;
  }
  /**
   * @brief Get data type of txid
   * @return Data type of txid
   */
  static std::string GetTxidFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of txid field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTxidString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.txid_);
  }
  /**
   * @brief Set json object to txid field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTxidString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.txid_, json_value);
  }

  /**
   * @brief Get of vout
   * @return vout
   */
  uint32_t GetVout() const {
    return vout_;
  }
  /**
   * @brief Set to vout
   * @param[in] vout    setting value.
   */
  void SetVout(  // line separate
    const uint32_t& vout) {  // NOLINT
    this->vout_ = vout;
  }
  /**
   * @brief Get data type of vout
   * @return Data type of vout
   */
  static std::string GetVoutFieldType() {
    return "uint32_t";
  }
  /**
   * @brief Get json string of vout field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetVoutString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.vout_);
  }
  /**
   * @brief Set json object to vout field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetVoutString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.vout_, json_value);
  }

  /**
   * @brief Get of scriptSig.
   * @return scriptSig
   */
  ElementsDecodeUnlockingScript& GetScriptSig() {  // NOLINT
    return script_sig_;
  }
  /**
   * @brief Set to scriptSig.
   * @param[in] script_sig    setting value.
   */
  void SetScriptSig(  // line separate
      const ElementsDecodeUnlockingScript& script_sig) {  // NOLINT
    this->script_sig_ = script_sig;
  }
  /**
   * @brief Get data type of scriptSig.
   * @return Data type of scriptSig.
   */
  static std::string GetScriptSigFieldType() {
    return "ElementsDecodeUnlockingScript";  // NOLINT
  }
  /**
   * @brief Get json string of scriptSig field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetScriptSigString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.script_sig_.Serialize();
  }
  /**
   * @brief Set json object to scriptSig field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetScriptSigString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    obj.script_sig_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of is_pegin
   * @return is_pegin
   */
  bool GetIs_pegin() const {
    return is_pegin_;
  }
  /**
   * @brief Set to is_pegin
   * @param[in] is_pegin    setting value.
   */
  void SetIs_pegin(  // line separate
    const bool& is_pegin) {  // NOLINT
    this->is_pegin_ = is_pegin;
  }
  /**
   * @brief Get data type of is_pegin
   * @return Data type of is_pegin
   */
  static std::string GetIs_peginFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of is_pegin field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetIs_peginString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.is_pegin_);
  }
  /**
   * @brief Set json object to is_pegin field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetIs_peginString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.is_pegin_, json_value);
  }

  /**
   * @brief Get of sequence
   * @return sequence
   */
  int64_t GetSequence() const {
    return sequence_;
  }
  /**
   * @brief Set to sequence
   * @param[in] sequence    setting value.
   */
  void SetSequence(  // line separate
    const int64_t& sequence) {  // NOLINT
    this->sequence_ = sequence;
  }
  /**
   * @brief Get data type of sequence
   * @return Data type of sequence
   */
  static std::string GetSequenceFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of sequence field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetSequenceString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.sequence_);
  }
  /**
   * @brief Set json object to sequence field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetSequenceString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.sequence_, json_value);
  }

  /**
   * @brief Get of txinwitness.
   * @return txinwitness
   */
  JsonValueVector<std::string>& GetTxinwitness() {  // NOLINT
    return txinwitness_;
  }
  /**
   * @brief Set to txinwitness.
   * @param[in] txinwitness    setting value.
   */
  void SetTxinwitness(  // line separate
      const JsonValueVector<std::string>& txinwitness) {  // NOLINT
    this->txinwitness_ = txinwitness;
  }
  /**
   * @brief Get data type of txinwitness.
   * @return Data type of txinwitness.
   */
  static std::string GetTxinwitnessFieldType() {
    return "JsonValueVector<std::string>";  // NOLINT
  }
  /**
   * @brief Get json string of txinwitness field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetTxinwitnessString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.txinwitness_.Serialize();
  }
  /**
   * @brief Set json object to txinwitness field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetTxinwitnessString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    obj.txinwitness_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of pegin_witness.
   * @return pegin_witness
   */
  JsonValueVector<std::string>& GetPegin_witness() {  // NOLINT
    return pegin_witness_;
  }
  /**
   * @brief Set to pegin_witness.
   * @param[in] pegin_witness    setting value.
   */
  void SetPegin_witness(  // line separate
      const JsonValueVector<std::string>& pegin_witness) {  // NOLINT
    this->pegin_witness_ = pegin_witness;
  }
  /**
   * @brief Get data type of pegin_witness.
   * @return Data type of pegin_witness.
   */
  static std::string GetPegin_witnessFieldType() {
    return "JsonValueVector<std::string>";  // NOLINT
  }
  /**
   * @brief Get json string of pegin_witness field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetPegin_witnessString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.pegin_witness_.Serialize();
  }
  /**
   * @brief Set json object to pegin_witness field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetPegin_witnessString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    obj.pegin_witness_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of issuance.
   * @return issuance
   */
  ElementsDecodeIssuance& GetIssuance() {  // NOLINT
    return issuance_;
  }
  /**
   * @brief Set to issuance.
   * @param[in] issuance    setting value.
   */
  void SetIssuance(  // line separate
      const ElementsDecodeIssuance& issuance) {  // NOLINT
    this->issuance_ = issuance;
  }
  /**
   * @brief Get data type of issuance.
   * @return Data type of issuance.
   */
  static std::string GetIssuanceFieldType() {
    return "ElementsDecodeIssuance";  // NOLINT
  }
  /**
   * @brief Get json string of issuance field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetIssuanceString(  // line separate
      const ElementsDecodeRawTransactionTxIn& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.issuance_.Serialize();
  }
  /**
   * @brief Set json object to issuance field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetIssuanceString(  // line separate
      ElementsDecodeRawTransactionTxIn& obj,  // NOLINT
      const UniValue& json_value) {
    obj.issuance_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ElementsDecodeRawTransactionTxInStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ElementsDecodeRawTransactionTxInStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ElementsDecodeRawTransactionTxInMapTable =
    cfd::core::JsonTableMap<ElementsDecodeRawTransactionTxIn>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ElementsDecodeRawTransactionTxInMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ElementsDecodeRawTransactionTxInMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(coinbase) value
   */
  std::string coinbase_ = "";
  /**
   * @brief JsonAPI(txid) value
   */
  std::string txid_ = "";
  /**
   * @brief JsonAPI(vout) value
   */
  uint32_t vout_ = 0;
  /**
   * @brief JsonAPI(scriptSig) value
   */
  ElementsDecodeUnlockingScript script_sig_;  // NOLINT
  /**
   * @brief JsonAPI(is_pegin) value
   */
  bool is_pegin_ = false;
  /**
   * @brief JsonAPI(sequence) value
   */
  int64_t sequence_ = 0;
  /**
   * @brief JsonAPI(txinwitness) value
   */
  JsonValueVector<std::string> txinwitness_;  // NOLINT
  /**
   * @brief JsonAPI(pegin_witness) value
   */
  JsonValueVector<std::string> pegin_witness_;  // NOLINT
  /**
   * @brief JsonAPI(issuance) value
   */
  ElementsDecodeIssuance issuance_;  // NOLINT
};

// ------------------------------------------------------------------------
// ElementsDecodeLockingScript
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (ElementsDecodeLockingScript) class
 */
class ElementsDecodeLockingScript
  : public cfd::core::JsonClassBase<ElementsDecodeLockingScript> {
 public:
  ElementsDecodeLockingScript() {
    CollectFieldName();
  }
  virtual ~ElementsDecodeLockingScript() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of asm
   * @return asm
   */
  std::string GetAsm() const {
    return asm__;
  }
  /**
   * @brief Set to asm
   * @param[in] asm_    setting value.
   */
  void SetAsm(  // line separate
    const std::string& asm_) {  // NOLINT
    this->asm__ = asm_;
  }
  /**
   * @brief Get data type of asm
   * @return Data type of asm
   */
  static std::string GetAsmFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of asm field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAsmString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asm__);
  }
  /**
   * @brief Set json object to asm field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAsmString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asm__, json_value);
  }

  /**
   * @brief Get of hex
   * @return hex
   */
  std::string GetHex() const {
    return hex_;
  }
  /**
   * @brief Set to hex
   * @param[in] hex    setting value.
   */
  void SetHex(  // line separate
    const std::string& hex) {  // NOLINT
    this->hex_ = hex;
  }
  /**
   * @brief Get data type of hex
   * @return Data type of hex
   */
  static std::string GetHexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHexString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
  }

  /**
   * @brief Get of reqSigs
   * @return reqSigs
   */
  int GetReqSigs() const {
    return req_sigs_;
  }
  /**
   * @brief Set to reqSigs
   * @param[in] req_sigs    setting value.
   */
  void SetReqSigs(  // line separate
    const int& req_sigs) {  // NOLINT
    this->req_sigs_ = req_sigs;
  }
  /**
   * @brief Get data type of reqSigs
   * @return Data type of reqSigs
   */
  static std::string GetReqSigsFieldType() {
    return "int";
  }
  /**
   * @brief Get json string of reqSigs field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetReqSigsString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.req_sigs_);
  }
  /**
   * @brief Set json object to reqSigs field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetReqSigsString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.req_sigs_, json_value);
  }

  /**
   * @brief Get of type
   * @return type
   */
  std::string GetType() const {
    return type_;
  }
  /**
   * @brief Set to type
   * @param[in] type    setting value.
   */
  void SetType(  // line separate
    const std::string& type) {  // NOLINT
    this->type_ = type;
  }
  /**
   * @brief Get data type of type
   * @return Data type of type
   */
  static std::string GetTypeFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of type field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTypeString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.type_);
  }
  /**
   * @brief Set json object to type field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTypeString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.type_, json_value);
  }

  /**
   * @brief Get of addresses.
   * @return addresses
   */
  JsonValueVector<std::string>& GetAddresses() {  // NOLINT
    return addresses_;
  }
  /**
   * @brief Set to addresses.
   * @param[in] addresses    setting value.
   */
  void SetAddresses(  // line separate
      const JsonValueVector<std::string>& addresses) {  // NOLINT
    this->addresses_ = addresses;
  }
  /**
   * @brief Get data type of addresses.
   * @return Data type of addresses.
   */
  static std::string GetAddressesFieldType() {
    return "JsonValueVector<std::string>";  // NOLINT
  }
  /**
   * @brief Get json string of addresses field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetAddressesString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.addresses_.Serialize();
  }
  /**
   * @brief Set json object to addresses field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetAddressesString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    obj.addresses_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of pegout_chain
   * @return pegout_chain
   */
  std::string GetPegout_chain() const {
    return pegout_chain_;
  }
  /**
   * @brief Set to pegout_chain
   * @param[in] pegout_chain    setting value.
   */
  void SetPegout_chain(  // line separate
    const std::string& pegout_chain) {  // NOLINT
    this->pegout_chain_ = pegout_chain;
  }
  /**
   * @brief Get data type of pegout_chain
   * @return Data type of pegout_chain
   */
  static std::string GetPegout_chainFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of pegout_chain field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPegout_chainString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.pegout_chain_);
  }
  /**
   * @brief Set json object to pegout_chain field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPegout_chainString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.pegout_chain_, json_value);
  }

  /**
   * @brief Get of pegout_asm
   * @return pegout_asm
   */
  std::string GetPegout_asm() const {
    return pegout_asm_;
  }
  /**
   * @brief Set to pegout_asm
   * @param[in] pegout_asm    setting value.
   */
  void SetPegout_asm(  // line separate
    const std::string& pegout_asm) {  // NOLINT
    this->pegout_asm_ = pegout_asm;
  }
  /**
   * @brief Get data type of pegout_asm
   * @return Data type of pegout_asm
   */
  static std::string GetPegout_asmFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of pegout_asm field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPegout_asmString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.pegout_asm_);
  }
  /**
   * @brief Set json object to pegout_asm field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPegout_asmString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.pegout_asm_, json_value);
  }

  /**
   * @brief Get of pegout_hex
   * @return pegout_hex
   */
  std::string GetPegout_hex() const {
    return pegout_hex_;
  }
  /**
   * @brief Set to pegout_hex
   * @param[in] pegout_hex    setting value.
   */
  void SetPegout_hex(  // line separate
    const std::string& pegout_hex) {  // NOLINT
    this->pegout_hex_ = pegout_hex;
  }
  /**
   * @brief Get data type of pegout_hex
   * @return Data type of pegout_hex
   */
  static std::string GetPegout_hexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of pegout_hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPegout_hexString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.pegout_hex_);
  }
  /**
   * @brief Set json object to pegout_hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPegout_hexString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.pegout_hex_, json_value);
  }

  /**
   * @brief Get of pegout_reqSigs
   * @return pegout_reqSigs
   */
  int GetPegout_reqSigs() const {
    return pegout_req_sigs_;
  }
  /**
   * @brief Set to pegout_reqSigs
   * @param[in] pegout_req_sigs    setting value.
   */
  void SetPegout_reqSigs(  // line separate
    const int& pegout_req_sigs) {  // NOLINT
    this->pegout_req_sigs_ = pegout_req_sigs;
  }
  /**
   * @brief Get data type of pegout_reqSigs
   * @return Data type of pegout_reqSigs
   */
  static std::string GetPegout_reqSigsFieldType() {
    return "int";
  }
  /**
   * @brief Get json string of pegout_reqSigs field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPegout_reqSigsString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.pegout_req_sigs_);
  }
  /**
   * @brief Set json object to pegout_reqSigs field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPegout_reqSigsString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.pegout_req_sigs_, json_value);
  }

  /**
   * @brief Get of pegout_type
   * @return pegout_type
   */
  std::string GetPegout_type() const {
    return pegout_type_;
  }
  /**
   * @brief Set to pegout_type
   * @param[in] pegout_type    setting value.
   */
  void SetPegout_type(  // line separate
    const std::string& pegout_type) {  // NOLINT
    this->pegout_type_ = pegout_type;
  }
  /**
   * @brief Get data type of pegout_type
   * @return Data type of pegout_type
   */
  static std::string GetPegout_typeFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of pegout_type field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPegout_typeString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.pegout_type_);
  }
  /**
   * @brief Set json object to pegout_type field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPegout_typeString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.pegout_type_, json_value);
  }

  /**
   * @brief Get of pegout_addresses.
   * @return pegout_addresses
   */
  JsonValueVector<std::string>& GetPegout_addresses() {  // NOLINT
    return pegout_addresses_;
  }
  /**
   * @brief Set to pegout_addresses.
   * @param[in] pegout_addresses    setting value.
   */
  void SetPegout_addresses(  // line separate
      const JsonValueVector<std::string>& pegout_addresses) {  // NOLINT
    this->pegout_addresses_ = pegout_addresses;
  }
  /**
   * @brief Get data type of pegout_addresses.
   * @return Data type of pegout_addresses.
   */
  static std::string GetPegout_addressesFieldType() {
    return "JsonValueVector<std::string>";  // NOLINT
  }
  /**
   * @brief Get json string of pegout_addresses field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetPegout_addressesString(  // line separate
      const ElementsDecodeLockingScript& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.pegout_addresses_.Serialize();
  }
  /**
   * @brief Set json object to pegout_addresses field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetPegout_addressesString(  // line separate
      ElementsDecodeLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    obj.pegout_addresses_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ElementsDecodeLockingScriptStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ElementsDecodeLockingScriptStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ElementsDecodeLockingScriptMapTable =
    cfd::core::JsonTableMap<ElementsDecodeLockingScript>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ElementsDecodeLockingScriptMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ElementsDecodeLockingScriptMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(asm) value
   */
  std::string asm__ = "";
  /**
   * @brief JsonAPI(hex) value
   */
  std::string hex_ = "";
  /**
   * @brief JsonAPI(reqSigs) value
   */
  int req_sigs_ = 0;
  /**
   * @brief JsonAPI(type) value
   */
  std::string type_ = "";
  /**
   * @brief JsonAPI(addresses) value
   */
  JsonValueVector<std::string> addresses_;  // NOLINT
  /**
   * @brief JsonAPI(pegout_chain) value
   */
  std::string pegout_chain_ = "";
  /**
   * @brief JsonAPI(pegout_asm) value
   */
  std::string pegout_asm_ = "";
  /**
   * @brief JsonAPI(pegout_hex) value
   */
  std::string pegout_hex_ = "";
  /**
   * @brief JsonAPI(pegout_reqSigs) value
   */
  int pegout_req_sigs_ = 0;
  /**
   * @brief JsonAPI(pegout_type) value
   */
  std::string pegout_type_ = "";
  /**
   * @brief JsonAPI(pegout_addresses) value
   */
  JsonValueVector<std::string> pegout_addresses_;  // NOLINT
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionTxOut
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (ElementsDecodeRawTransactionTxOut) class
 */
class ElementsDecodeRawTransactionTxOut
  : public cfd::core::JsonClassBase<ElementsDecodeRawTransactionTxOut> {
 public:
  ElementsDecodeRawTransactionTxOut() {
    CollectFieldName();
  }
  virtual ~ElementsDecodeRawTransactionTxOut() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of value
   * @return value
   */
  double GetValue() const {
    return value_;
  }
  /**
   * @brief Set to value
   * @param[in] value    setting value.
   */
  void SetValue(  // line separate
    const double& value) {  // NOLINT
    this->value_ = value;
  }
  /**
   * @brief Get data type of value
   * @return Data type of value
   */
  static std::string GetValueFieldType() {
    return "double";
  }
  /**
   * @brief Get json string of value field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetValueString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.value_);
  }
  /**
   * @brief Set json object to value field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetValueString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.value_, json_value);
  }

  /**
   * @brief Get of value-minimum
   * @return value-minimum
   */
  double GetValue_minimum() const {
    return value_minimum_;
  }
  /**
   * @brief Set to value-minimum
   * @param[in] value_minimum    setting value.
   */
  void SetValue_minimum(  // line separate
    const double& value_minimum) {  // NOLINT
    this->value_minimum_ = value_minimum;
  }
  /**
   * @brief Get data type of value-minimum
   * @return Data type of value-minimum
   */
  static std::string GetValue_minimumFieldType() {
    return "double";
  }
  /**
   * @brief Get json string of value-minimum field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetValue_minimumString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.value_minimum_);
  }
  /**
   * @brief Set json object to value-minimum field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetValue_minimumString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.value_minimum_, json_value);
  }

  /**
   * @brief Get of value-maximum
   * @return value-maximum
   */
  double GetValue_maximum() const {
    return value_maximum_;
  }
  /**
   * @brief Set to value-maximum
   * @param[in] value_maximum    setting value.
   */
  void SetValue_maximum(  // line separate
    const double& value_maximum) {  // NOLINT
    this->value_maximum_ = value_maximum;
  }
  /**
   * @brief Get data type of value-maximum
   * @return Data type of value-maximum
   */
  static std::string GetValue_maximumFieldType() {
    return "double";
  }
  /**
   * @brief Get json string of value-maximum field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetValue_maximumString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.value_maximum_);
  }
  /**
   * @brief Set json object to value-maximum field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetValue_maximumString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.value_maximum_, json_value);
  }

  /**
   * @brief Get of ct-exponent
   * @return ct-exponent
   */
  int GetCt_exponent() const {
    return ct_exponent_;
  }
  /**
   * @brief Set to ct-exponent
   * @param[in] ct_exponent    setting value.
   */
  void SetCt_exponent(  // line separate
    const int& ct_exponent) {  // NOLINT
    this->ct_exponent_ = ct_exponent;
  }
  /**
   * @brief Get data type of ct-exponent
   * @return Data type of ct-exponent
   */
  static std::string GetCt_exponentFieldType() {
    return "int";
  }
  /**
   * @brief Get json string of ct-exponent field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetCt_exponentString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.ct_exponent_);
  }
  /**
   * @brief Set json object to ct-exponent field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetCt_exponentString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.ct_exponent_, json_value);
  }

  /**
   * @brief Get of ct-bits
   * @return ct-bits
   */
  int GetCt_bits() const {
    return ct_bits_;
  }
  /**
   * @brief Set to ct-bits
   * @param[in] ct_bits    setting value.
   */
  void SetCt_bits(  // line separate
    const int& ct_bits) {  // NOLINT
    this->ct_bits_ = ct_bits;
  }
  /**
   * @brief Get data type of ct-bits
   * @return Data type of ct-bits
   */
  static std::string GetCt_bitsFieldType() {
    return "int";
  }
  /**
   * @brief Get json string of ct-bits field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetCt_bitsString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.ct_bits_);
  }
  /**
   * @brief Set json object to ct-bits field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetCt_bitsString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.ct_bits_, json_value);
  }

  /**
   * @brief Get of surjectionproof
   * @return surjectionproof
   */
  std::string GetSurjectionproof() const {
    return surjectionproof_;
  }
  /**
   * @brief Set to surjectionproof
   * @param[in] surjectionproof    setting value.
   */
  void SetSurjectionproof(  // line separate
    const std::string& surjectionproof) {  // NOLINT
    this->surjectionproof_ = surjectionproof;
  }
  /**
   * @brief Get data type of surjectionproof
   * @return Data type of surjectionproof
   */
  static std::string GetSurjectionproofFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of surjectionproof field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetSurjectionproofString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.surjectionproof_);
  }
  /**
   * @brief Set json object to surjectionproof field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetSurjectionproofString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.surjectionproof_, json_value);
  }

  /**
   * @brief Get of valuecommitment
   * @return valuecommitment
   */
  std::string GetValuecommitment() const {
    return valuecommitment_;
  }
  /**
   * @brief Set to valuecommitment
   * @param[in] valuecommitment    setting value.
   */
  void SetValuecommitment(  // line separate
    const std::string& valuecommitment) {  // NOLINT
    this->valuecommitment_ = valuecommitment;
  }
  /**
   * @brief Get data type of valuecommitment
   * @return Data type of valuecommitment
   */
  static std::string GetValuecommitmentFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of valuecommitment field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetValuecommitmentString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.valuecommitment_);
  }
  /**
   * @brief Set json object to valuecommitment field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetValuecommitmentString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.valuecommitment_, json_value);
  }

  /**
   * @brief Get of asset
   * @return asset
   */
  std::string GetAsset() const {
    return asset_;
  }
  /**
   * @brief Set to asset
   * @param[in] asset    setting value.
   */
  void SetAsset(  // line separate
    const std::string& asset) {  // NOLINT
    this->asset_ = asset;
  }
  /**
   * @brief Get data type of asset
   * @return Data type of asset
   */
  static std::string GetAssetFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of asset field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asset_);
  }
  /**
   * @brief Set json object to asset field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asset_, json_value);
  }

  /**
   * @brief Get of assetcommitment
   * @return assetcommitment
   */
  std::string GetAssetcommitment() const {
    return assetcommitment_;
  }
  /**
   * @brief Set to assetcommitment
   * @param[in] assetcommitment    setting value.
   */
  void SetAssetcommitment(  // line separate
    const std::string& assetcommitment) {  // NOLINT
    this->assetcommitment_ = assetcommitment;
  }
  /**
   * @brief Get data type of assetcommitment
   * @return Data type of assetcommitment
   */
  static std::string GetAssetcommitmentFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of assetcommitment field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetcommitmentString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.assetcommitment_);
  }
  /**
   * @brief Set json object to assetcommitment field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetcommitmentString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.assetcommitment_, json_value);
  }

  /**
   * @brief Get of commitmentnonce
   * @return commitmentnonce
   */
  std::string GetCommitmentnonce() const {
    return commitmentnonce_;
  }
  /**
   * @brief Set to commitmentnonce
   * @param[in] commitmentnonce    setting value.
   */
  void SetCommitmentnonce(  // line separate
    const std::string& commitmentnonce) {  // NOLINT
    this->commitmentnonce_ = commitmentnonce;
  }
  /**
   * @brief Get data type of commitmentnonce
   * @return Data type of commitmentnonce
   */
  static std::string GetCommitmentnonceFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of commitmentnonce field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetCommitmentnonceString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.commitmentnonce_);
  }
  /**
   * @brief Set json object to commitmentnonce field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetCommitmentnonceString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.commitmentnonce_, json_value);
  }

  /**
   * @brief Get of commitmentnonce_fully_valid
   * @return commitmentnonce_fully_valid
   */
  bool GetCommitmentnonce_fully_valid() const {
    return commitmentnonce_fully_valid_;
  }
  /**
   * @brief Set to commitmentnonce_fully_valid
   * @param[in] commitmentnonce_fully_valid    setting value.
   */
  void SetCommitmentnonce_fully_valid(  // line separate
    const bool& commitmentnonce_fully_valid) {  // NOLINT
    this->commitmentnonce_fully_valid_ = commitmentnonce_fully_valid;
  }
  /**
   * @brief Get data type of commitmentnonce_fully_valid
   * @return Data type of commitmentnonce_fully_valid
   */
  static std::string GetCommitmentnonce_fully_validFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of commitmentnonce_fully_valid field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetCommitmentnonce_fully_validString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.commitmentnonce_fully_valid_);
  }
  /**
   * @brief Set json object to commitmentnonce_fully_valid field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetCommitmentnonce_fully_validString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.commitmentnonce_fully_valid_, json_value);
  }

  /**
   * @brief Get of n
   * @return n
   */
  uint32_t GetN() const {
    return n_;
  }
  /**
   * @brief Set to n
   * @param[in] n    setting value.
   */
  void SetN(  // line separate
    const uint32_t& n) {  // NOLINT
    this->n_ = n;
  }
  /**
   * @brief Get data type of n
   * @return Data type of n
   */
  static std::string GetNFieldType() {
    return "uint32_t";
  }
  /**
   * @brief Get json string of n field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetNString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.n_);
  }
  /**
   * @brief Set json object to n field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetNString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.n_, json_value);
  }

  /**
   * @brief Get of scriptPubKey.
   * @return scriptPubKey
   */
  ElementsDecodeLockingScript& GetScriptPubKey() {  // NOLINT
    return script_pub_key_;
  }
  /**
   * @brief Set to scriptPubKey.
   * @param[in] script_pub_key    setting value.
   */
  void SetScriptPubKey(  // line separate
      const ElementsDecodeLockingScript& script_pub_key) {  // NOLINT
    this->script_pub_key_ = script_pub_key;
  }
  /**
   * @brief Get data type of scriptPubKey.
   * @return Data type of scriptPubKey.
   */
  static std::string GetScriptPubKeyFieldType() {
    return "ElementsDecodeLockingScript";  // NOLINT
  }
  /**
   * @brief Get json string of scriptPubKey field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetScriptPubKeyString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.script_pub_key_.Serialize();
  }
  /**
   * @brief Set json object to scriptPubKey field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetScriptPubKeyString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    obj.script_pub_key_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ElementsDecodeRawTransactionTxOutStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ElementsDecodeRawTransactionTxOutStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ElementsDecodeRawTransactionTxOutMapTable =
    cfd::core::JsonTableMap<ElementsDecodeRawTransactionTxOut>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ElementsDecodeRawTransactionTxOutMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ElementsDecodeRawTransactionTxOutMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(value) value
   */
  double value_ = 0;
  /**
   * @brief JsonAPI(value-minimum) value
   */
  double value_minimum_ = 0;
  /**
   * @brief JsonAPI(value-maximum) value
   */
  double value_maximum_ = 0;
  /**
   * @brief JsonAPI(ct-exponent) value
   */
  int ct_exponent_ = 0;
  /**
   * @brief JsonAPI(ct-bits) value
   */
  int ct_bits_ = 0;
  /**
   * @brief JsonAPI(surjectionproof) value
   */
  std::string surjectionproof_ = "";
  /**
   * @brief JsonAPI(valuecommitment) value
   */
  std::string valuecommitment_ = "";
  /**
   * @brief JsonAPI(asset) value
   */
  std::string asset_ = "";
  /**
   * @brief JsonAPI(assetcommitment) value
   */
  std::string assetcommitment_ = "";
  /**
   * @brief JsonAPI(commitmentnonce) value
   */
  std::string commitmentnonce_ = "";
  /**
   * @brief JsonAPI(commitmentnonce_fully_valid) value
   */
  bool commitmentnonce_fully_valid_ = false;
  /**
   * @brief JsonAPI(n) value
   */
  uint32_t n_ = 0;
  /**
   * @brief JsonAPI(scriptPubKey) value
   */
  ElementsDecodeLockingScript script_pub_key_;  // NOLINT
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionResponse
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (ElementsDecodeRawTransactionResponse) class
 */
class ElementsDecodeRawTransactionResponse
  : public cfd::core::JsonClassBase<ElementsDecodeRawTransactionResponse> {
 public:
  ElementsDecodeRawTransactionResponse() {
    CollectFieldName();
  }
  virtual ~ElementsDecodeRawTransactionResponse() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of txid
   * @return txid
   */
  std::string GetTxid() const {
    return txid_;
  }
  /**
   * @brief Set to txid
   * @param[in] txid    setting value.
   */
  void SetTxid(  // line separate
    const std::string& txid) {  // NOLINT
    this->txid_ = txid;
  }
  /**
   * @brief Get data type of txid
   * @return Data type of txid
   */
  static std::string GetTxidFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of txid field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTxidString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.txid_);
  }
  /**
   * @brief Set json object to txid field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTxidString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.txid_, json_value);
  }

  /**
   * @brief Get of hash
   * @return hash
   */
  std::string GetHash() const {
    return hash_;
  }
  /**
   * @brief Set to hash
   * @param[in] hash    setting value.
   */
  void SetHash(  // line separate
    const std::string& hash) {  // NOLINT
    this->hash_ = hash;
  }
  /**
   * @brief Get data type of hash
   * @return Data type of hash
   */
  static std::string GetHashFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of hash field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHashString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hash_);
  }
  /**
   * @brief Set json object to hash field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHashString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hash_, json_value);
  }

  /**
   * @brief Get of wtxid
   * @return wtxid
   */
  std::string GetWtxid() const {
    return wtxid_;
  }
  /**
   * @brief Set to wtxid
   * @param[in] wtxid    setting value.
   */
  void SetWtxid(  // line separate
    const std::string& wtxid) {  // NOLINT
    this->wtxid_ = wtxid;
  }
  /**
   * @brief Get data type of wtxid
   * @return Data type of wtxid
   */
  static std::string GetWtxidFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of wtxid field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetWtxidString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.wtxid_);
  }
  /**
   * @brief Set json object to wtxid field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetWtxidString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.wtxid_, json_value);
  }

  /**
   * @brief Get of withash
   * @return withash
   */
  std::string GetWithash() const {
    return withash_;
  }
  /**
   * @brief Set to withash
   * @param[in] withash    setting value.
   */
  void SetWithash(  // line separate
    const std::string& withash) {  // NOLINT
    this->withash_ = withash;
  }
  /**
   * @brief Get data type of withash
   * @return Data type of withash
   */
  static std::string GetWithashFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of withash field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetWithashString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.withash_);
  }
  /**
   * @brief Set json object to withash field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetWithashString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.withash_, json_value);
  }

  /**
   * @brief Get of version
   * @return version
   */
  uint32_t GetVersion() const {
    return version_;
  }
  /**
   * @brief Set to version
   * @param[in] version    setting value.
   */
  void SetVersion(  // line separate
    const uint32_t& version) {  // NOLINT
    this->version_ = version;
  }
  /**
   * @brief Get data type of version
   * @return Data type of version
   */
  static std::string GetVersionFieldType() {
    return "uint32_t";
  }
  /**
   * @brief Get json string of version field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetVersionString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.version_);
  }
  /**
   * @brief Set json object to version field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetVersionString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.version_, json_value);
  }

  /**
   * @brief Get of size
   * @return size
   */
  int64_t GetSize() const {
    return size_;
  }
  /**
   * @brief Set to size
   * @param[in] size    setting value.
   */
  void SetSize(  // line separate
    const int64_t& size) {  // NOLINT
    this->size_ = size;
  }
  /**
   * @brief Get data type of size
   * @return Data type of size
   */
  static std::string GetSizeFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of size field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetSizeString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.size_);
  }
  /**
   * @brief Set json object to size field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetSizeString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.size_, json_value);
  }

  /**
   * @brief Get of vsize
   * @return vsize
   */
  int64_t GetVsize() const {
    return vsize_;
  }
  /**
   * @brief Set to vsize
   * @param[in] vsize    setting value.
   */
  void SetVsize(  // line separate
    const int64_t& vsize) {  // NOLINT
    this->vsize_ = vsize;
  }
  /**
   * @brief Get data type of vsize
   * @return Data type of vsize
   */
  static std::string GetVsizeFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of vsize field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetVsizeString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.vsize_);
  }
  /**
   * @brief Set json object to vsize field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetVsizeString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.vsize_, json_value);
  }

  /**
   * @brief Get of weight
   * @return weight
   */
  int64_t GetWeight() const {
    return weight_;
  }
  /**
   * @brief Set to weight
   * @param[in] weight    setting value.
   */
  void SetWeight(  // line separate
    const int64_t& weight) {  // NOLINT
    this->weight_ = weight;
  }
  /**
   * @brief Get data type of weight
   * @return Data type of weight
   */
  static std::string GetWeightFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of weight field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetWeightString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.weight_);
  }
  /**
   * @brief Set json object to weight field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetWeightString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.weight_, json_value);
  }

  /**
   * @brief Get of locktime
   * @return locktime
   */
  uint32_t GetLocktime() const {
    return locktime_;
  }
  /**
   * @brief Set to locktime
   * @param[in] locktime    setting value.
   */
  void SetLocktime(  // line separate
    const uint32_t& locktime) {  // NOLINT
    this->locktime_ = locktime;
  }
  /**
   * @brief Get data type of locktime
   * @return Data type of locktime
   */
  static std::string GetLocktimeFieldType() {
    return "uint32_t";
  }
  /**
   * @brief Get json string of locktime field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetLocktimeString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.locktime_);
  }
  /**
   * @brief Set json object to locktime field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetLocktimeString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.locktime_, json_value);
  }

  /**
   * @brief Get of vin.
   * @return vin
   */
  JsonObjectVector<ElementsDecodeRawTransactionTxIn, ElementsDecodeRawTransactionTxInStruct>& GetVin() {  // NOLINT
    return vin_;
  }
  /**
   * @brief Set to vin.
   * @param[in] vin    setting value.
   */
  void SetVin(  // line separate
      const JsonObjectVector<ElementsDecodeRawTransactionTxIn, ElementsDecodeRawTransactionTxInStruct>& vin) {  // NOLINT
    this->vin_ = vin;
  }
  /**
   * @brief Get data type of vin.
   * @return Data type of vin.
   */
  static std::string GetVinFieldType() {
    return "JsonObjectVector<ElementsDecodeRawTransactionTxIn, ElementsDecodeRawTransactionTxInStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of vin field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetVinString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.vin_.Serialize();
  }
  /**
   * @brief Set json object to vin field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetVinString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.vin_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of vout.
   * @return vout
   */
  JsonObjectVector<ElementsDecodeRawTransactionTxOut, ElementsDecodeRawTransactionTxOutStruct>& GetVout() {  // NOLINT
    return vout_;
  }
  /**
   * @brief Set to vout.
   * @param[in] vout    setting value.
   */
  void SetVout(  // line separate
      const JsonObjectVector<ElementsDecodeRawTransactionTxOut, ElementsDecodeRawTransactionTxOutStruct>& vout) {  // NOLINT
    this->vout_ = vout;
  }
  /**
   * @brief Get data type of vout.
   * @return Data type of vout.
   */
  static std::string GetVoutFieldType() {
    return "JsonObjectVector<ElementsDecodeRawTransactionTxOut, ElementsDecodeRawTransactionTxOutStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of vout field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetVoutString(  // line separate
      const ElementsDecodeRawTransactionResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.vout_.Serialize();
  }
  /**
   * @brief Set json object to vout field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetVoutString(  // line separate
      ElementsDecodeRawTransactionResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.vout_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Set ignore item.
   * @param[in] key   ignore target key name.
   */
  void SetIgnoreItem(const std::string& key) {
    ignore_items.insert(key);
  }

  /**
   * @brief Convert struct to class.
   * @param[in] data   struct data.
   */
  void ConvertFromStruct(
      const ElementsDecodeRawTransactionResponseStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  ElementsDecodeRawTransactionResponseStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using ElementsDecodeRawTransactionResponseMapTable =
    cfd::core::JsonTableMap<ElementsDecodeRawTransactionResponse>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const ElementsDecodeRawTransactionResponseMapTable& GetJsonMapper() const {  // NOLINT
    return json_mapper;
  }
  /**
   * @brief Get item lists of JSON mapping.
   * Fetch a list of target variable names in the order of definition.
   * @return Item lists of JSON mapping.
   * @see cfd::core::JsonClassBase::GetJsonItemList()
   */
  virtual const std::vector<std::string>& GetJsonItemList() const {
    return item_list;
  }
  /**
   * @brief Get ignore item lists of JSON mnapping.
   * Ignore the target variable at Serialize.
   * @return Item list of JSON mapping.
   * @see cfd::core::JsonClassBase::GetIgnoreItem()
   */
  virtual const std::set<std::string>& GetIgnoreItem() const {
    return ignore_items;
  }

 private:
 /**
  * @brief JsonFunctionMap table
  */
  static ElementsDecodeRawTransactionResponseMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(txid) value
   */
  std::string txid_ = "";
  /**
   * @brief JsonAPI(hash) value
   */
  std::string hash_ = "";
  /**
   * @brief JsonAPI(wtxid) value
   */
  std::string wtxid_ = "";
  /**
   * @brief JsonAPI(withash) value
   */
  std::string withash_ = "";
  /**
   * @brief JsonAPI(version) value
   */
  uint32_t version_ = 0;
  /**
   * @brief JsonAPI(size) value
   */
  int64_t size_ = 0;
  /**
   * @brief JsonAPI(vsize) value
   */
  int64_t vsize_ = 0;
  /**
   * @brief JsonAPI(weight) value
   */
  int64_t weight_ = 0;
  /**
   * @brief JsonAPI(locktime) value
   */
  uint32_t locktime_ = 0;
  /**
   * @brief JsonAPI(vin) value
   */
  JsonObjectVector<ElementsDecodeRawTransactionTxIn, ElementsDecodeRawTransactionTxInStruct> vin_;  // NOLINT
  /**
   * @brief JsonAPI(vout) value
   */
  JsonObjectVector<ElementsDecodeRawTransactionTxOut, ElementsDecodeRawTransactionTxOutStruct> vout_;  // NOLINT
};

// @formatter:on
// clang-format on

}  // namespace json
}  // namespace api
}  // namespace cfd

#endif  // CFD_SRC_JSONAPI_AUTOGEN_CFD_API_JSON_AUTOGEN_H_
