// Copyright 2020 CryptoGarage
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
   * @brief Get ignore item lists of JSON mapping.
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
  int req_sigs_ = 0;
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
   * @brief Get ignore item lists of JSON mapping.
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
// DecodePsbtLockingScript
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodePsbtLockingScript) class
 */
class DecodePsbtLockingScript
  : public cfd::core::JsonClassBase<DecodePsbtLockingScript> {
 public:
  DecodePsbtLockingScript() {
    CollectFieldName();
  }
  virtual ~DecodePsbtLockingScript() {
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
      const DecodePsbtLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asm__);
  }
  /**
   * @brief Set json object to asm field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAsmString(  // line separate
      DecodePsbtLockingScript& obj,  // NOLINT
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
      const DecodePsbtLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      DecodePsbtLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
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
      const DecodePsbtLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.type_);
  }
  /**
   * @brief Set json object to type field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTypeString(  // line separate
      DecodePsbtLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.type_, json_value);
  }

  /**
   * @brief Get of address
   * @return address
   */
  std::string GetAddress() const {
    return address_;
  }
  /**
   * @brief Set to address
   * @param[in] address    setting value.
   */
  void SetAddress(  // line separate
    const std::string& address) {  // NOLINT
    this->address_ = address;
  }
  /**
   * @brief Get data type of address
   * @return Data type of address
   */
  static std::string GetAddressFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of address field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAddressString(  // line separate
      const DecodePsbtLockingScript& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.address_);
  }
  /**
   * @brief Set json object to address field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAddressString(  // line separate
      DecodePsbtLockingScript& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.address_, json_value);
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
      const DecodePsbtLockingScriptStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodePsbtLockingScriptStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodePsbtLockingScriptMapTable =
    cfd::core::JsonTableMap<DecodePsbtLockingScript>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodePsbtLockingScriptMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static DecodePsbtLockingScriptMapTable json_mapper;
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
   * @brief JsonAPI(type) value
   */
  std::string type_ = "";
  /**
   * @brief JsonAPI(address) value
   */
  std::string address_ = "";
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
  uint32_t GetSequence() const {
    return sequence_;
  }
  /**
   * @brief Set to sequence
   * @param[in] sequence    setting value.
   */
  void SetSequence(  // line separate
    const uint32_t& sequence) {  // NOLINT
    this->sequence_ = sequence;
  }
  /**
   * @brief Get data type of sequence
   * @return Data type of sequence
   */
  static std::string GetSequenceFieldType() {
    return "uint32_t";
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
   * @brief Get ignore item lists of JSON mapping.
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
  uint32_t vout_ = 0;
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
  uint32_t sequence_ = 0;
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
  int64_t GetValue() const {
    return value_;
  }
  /**
   * @brief Set to value
   * @param[in] value    setting value.
   */
  void SetValue(  // line separate
    const int64_t& value) {  // NOLINT
    this->value_ = value;
  }
  /**
   * @brief Get data type of value
   * @return Data type of value
   */
  static std::string GetValueFieldType() {
    return "int64_t";
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
   * @brief Get ignore item lists of JSON mapping.
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
  int64_t value_ = 0;
  /**
   * @brief JsonAPI(n) value
   */
  uint32_t n_ = 0;
  /**
   * @brief JsonAPI(scriptPubKey) value
   */
  DecodeLockingScript script_pub_key_;  // NOLINT
};

// ------------------------------------------------------------------------
// DecodePsbtUtxo
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodePsbtUtxo) class
 */
class DecodePsbtUtxo
  : public cfd::core::JsonClassBase<DecodePsbtUtxo> {
 public:
  DecodePsbtUtxo() {
    CollectFieldName();
  }
  virtual ~DecodePsbtUtxo() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of amount
   * @return amount
   */
  int64_t GetAmount() const {
    return amount_;
  }
  /**
   * @brief Set to amount
   * @param[in] amount    setting value.
   */
  void SetAmount(  // line separate
    const int64_t& amount) {  // NOLINT
    this->amount_ = amount;
  }
  /**
   * @brief Get data type of amount
   * @return Data type of amount
   */
  static std::string GetAmountFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of amount field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAmountString(  // line separate
      const DecodePsbtUtxo& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.amount_);
  }
  /**
   * @brief Set json object to amount field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAmountString(  // line separate
      DecodePsbtUtxo& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.amount_, json_value);
  }

  /**
   * @brief Get of scriptPubKey.
   * @return scriptPubKey
   */
  DecodePsbtLockingScript& GetScriptPubKey() {  // NOLINT
    return script_pub_key_;
  }
  /**
   * @brief Set to scriptPubKey.
   * @param[in] script_pub_key    setting value.
   */
  void SetScriptPubKey(  // line separate
      const DecodePsbtLockingScript& script_pub_key) {  // NOLINT
    this->script_pub_key_ = script_pub_key;
  }
  /**
   * @brief Get data type of scriptPubKey.
   * @return Data type of scriptPubKey.
   */
  static std::string GetScriptPubKeyFieldType() {
    return "DecodePsbtLockingScript";  // NOLINT
  }
  /**
   * @brief Get json string of scriptPubKey field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetScriptPubKeyString(  // line separate
      const DecodePsbtUtxo& obj) {  // NOLINT
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
      DecodePsbtUtxo& obj,  // NOLINT
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
      const DecodePsbtUtxoStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodePsbtUtxoStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodePsbtUtxoMapTable =
    cfd::core::JsonTableMap<DecodePsbtUtxo>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodePsbtUtxoMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static DecodePsbtUtxoMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(amount) value
   */
  int64_t amount_ = 0;
  /**
   * @brief JsonAPI(scriptPubKey) value
   */
  DecodePsbtLockingScript script_pub_key_;  // NOLINT
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
  uint32_t GetSize() const {
    return size_;
  }
  /**
   * @brief Set to size
   * @param[in] size    setting value.
   */
  void SetSize(  // line separate
    const uint32_t& size) {  // NOLINT
    this->size_ = size;
  }
  /**
   * @brief Get data type of size
   * @return Data type of size
   */
  static std::string GetSizeFieldType() {
    return "uint32_t";
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
  uint32_t GetVsize() const {
    return vsize_;
  }
  /**
   * @brief Set to vsize
   * @param[in] vsize    setting value.
   */
  void SetVsize(  // line separate
    const uint32_t& vsize) {  // NOLINT
    this->vsize_ = vsize;
  }
  /**
   * @brief Get data type of vsize
   * @return Data type of vsize
   */
  static std::string GetVsizeFieldType() {
    return "uint32_t";
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
  uint32_t GetWeight() const {
    return weight_;
  }
  /**
   * @brief Set to weight
   * @param[in] weight    setting value.
   */
  void SetWeight(  // line separate
    const uint32_t& weight) {  // NOLINT
    this->weight_ = weight;
  }
  /**
   * @brief Get data type of weight
   * @return Data type of weight
   */
  static std::string GetWeightFieldType() {
    return "uint32_t";
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
   * @brief Get ignore item lists of JSON mapping.
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
  uint32_t size_ = 0;
  /**
   * @brief JsonAPI(vsize) value
   */
  uint32_t vsize_ = 0;
  /**
   * @brief JsonAPI(weight) value
   */
  uint32_t weight_ = 0;
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
   * @brief Get of contractHash
   * @return contractHash
   */
  std::string GetContractHash() const {
    return contract_hash_;
  }
  /**
   * @brief Set to contractHash
   * @param[in] contract_hash    setting value.
   */
  void SetContractHash(  // line separate
    const std::string& contract_hash) {  // NOLINT
    this->contract_hash_ = contract_hash;
  }
  /**
   * @brief Get data type of contractHash
   * @return Data type of contractHash
   */
  static std::string GetContractHashFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of contractHash field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetContractHashString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.contract_hash_);
  }
  /**
   * @brief Set json object to contractHash field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetContractHashString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.contract_hash_, json_value);
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
  int64_t GetAssetamount() const {
    return assetamount_;
  }
  /**
   * @brief Set to assetamount
   * @param[in] assetamount    setting value.
   */
  void SetAssetamount(  // line separate
    const int64_t& assetamount) {  // NOLINT
    this->assetamount_ = assetamount;
  }
  /**
   * @brief Get data type of assetamount
   * @return Data type of assetamount
   */
  static std::string GetAssetamountFieldType() {
    return "int64_t";
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
  int64_t GetTokenamount() const {
    return tokenamount_;
  }
  /**
   * @brief Set to tokenamount
   * @param[in] tokenamount    setting value.
   */
  void SetTokenamount(  // line separate
    const int64_t& tokenamount) {  // NOLINT
    this->tokenamount_ = tokenamount;
  }
  /**
   * @brief Get data type of tokenamount
   * @return Data type of tokenamount
   */
  static std::string GetTokenamountFieldType() {
    return "int64_t";
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
   * @brief Get of assetRangeproof
   * @return assetRangeproof
   */
  std::string GetAssetRangeproof() const {
    return asset_rangeproof_;
  }
  /**
   * @brief Set to assetRangeproof
   * @param[in] asset_rangeproof    setting value.
   */
  void SetAssetRangeproof(  // line separate
    const std::string& asset_rangeproof) {  // NOLINT
    this->asset_rangeproof_ = asset_rangeproof;
  }
  /**
   * @brief Get data type of assetRangeproof
   * @return Data type of assetRangeproof
   */
  static std::string GetAssetRangeproofFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of assetRangeproof field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetAssetRangeproofString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asset_rangeproof_);
  }
  /**
   * @brief Set json object to assetRangeproof field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAssetRangeproofString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.asset_rangeproof_, json_value);
  }

  /**
   * @brief Get of tokenRangeproof
   * @return tokenRangeproof
   */
  std::string GetTokenRangeproof() const {
    return token_rangeproof_;
  }
  /**
   * @brief Set to tokenRangeproof
   * @param[in] token_rangeproof    setting value.
   */
  void SetTokenRangeproof(  // line separate
    const std::string& token_rangeproof) {  // NOLINT
    this->token_rangeproof_ = token_rangeproof;
  }
  /**
   * @brief Get data type of tokenRangeproof
   * @return Data type of tokenRangeproof
   */
  static std::string GetTokenRangeproofFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of tokenRangeproof field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTokenRangeproofString(  // line separate
      const ElementsDecodeIssuance& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.token_rangeproof_);
  }
  /**
   * @brief Set json object to tokenRangeproof field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTokenRangeproofString(  // line separate
      ElementsDecodeIssuance& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.token_rangeproof_, json_value);
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
   * @brief Get ignore item lists of JSON mapping.
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
   * @brief JsonAPI(contractHash) value
   */
  std::string contract_hash_ = "";
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
  int64_t assetamount_ = 0;
  /**
   * @brief JsonAPI(assetamountcommitment) value
   */
  std::string assetamountcommitment_ = "";
  /**
   * @brief JsonAPI(tokenamount) value
   */
  int64_t tokenamount_ = 0;
  /**
   * @brief JsonAPI(tokenamountcommitment) value
   */
  std::string tokenamountcommitment_ = "";
  /**
   * @brief JsonAPI(assetRangeproof) value
   */
  std::string asset_rangeproof_ = "";
  /**
   * @brief JsonAPI(tokenRangeproof) value
   */
  std::string token_rangeproof_ = "";
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
   * @brief Get ignore item lists of JSON mapping.
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
   * @brief Get ignore item lists of JSON mapping.
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
// PsbtBip32Data
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (PsbtBip32Data) class
 */
class PsbtBip32Data
  : public cfd::core::JsonClassBase<PsbtBip32Data> {
 public:
  PsbtBip32Data() {
    CollectFieldName();
  }
  virtual ~PsbtBip32Data() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of pubkey
   * @return pubkey
   */
  std::string GetPubkey() const {
    return pubkey_;
  }
  /**
   * @brief Set to pubkey
   * @param[in] pubkey    setting value.
   */
  void SetPubkey(  // line separate
    const std::string& pubkey) {  // NOLINT
    this->pubkey_ = pubkey;
  }
  /**
   * @brief Get data type of pubkey
   * @return Data type of pubkey
   */
  static std::string GetPubkeyFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of pubkey field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPubkeyString(  // line separate
      const PsbtBip32Data& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.pubkey_);
  }
  /**
   * @brief Set json object to pubkey field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPubkeyString(  // line separate
      PsbtBip32Data& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.pubkey_, json_value);
  }

  /**
   * @brief Get of master_fingerprint
   * @return master_fingerprint
   */
  std::string GetMaster_fingerprint() const {
    return master_fingerprint_;
  }
  /**
   * @brief Set to master_fingerprint
   * @param[in] master_fingerprint    setting value.
   */
  void SetMaster_fingerprint(  // line separate
    const std::string& master_fingerprint) {  // NOLINT
    this->master_fingerprint_ = master_fingerprint;
  }
  /**
   * @brief Get data type of master_fingerprint
   * @return Data type of master_fingerprint
   */
  static std::string GetMaster_fingerprintFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of master_fingerprint field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetMaster_fingerprintString(  // line separate
      const PsbtBip32Data& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.master_fingerprint_);
  }
  /**
   * @brief Set json object to master_fingerprint field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetMaster_fingerprintString(  // line separate
      PsbtBip32Data& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.master_fingerprint_, json_value);
  }

  /**
   * @brief Get of path
   * @return path
   */
  std::string GetPath() const {
    return path_;
  }
  /**
   * @brief Set to path
   * @param[in] path    setting value.
   */
  void SetPath(  // line separate
    const std::string& path) {  // NOLINT
    this->path_ = path;
  }
  /**
   * @brief Get data type of path
   * @return Data type of path
   */
  static std::string GetPathFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of path field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPathString(  // line separate
      const PsbtBip32Data& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.path_);
  }
  /**
   * @brief Set json object to path field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPathString(  // line separate
      PsbtBip32Data& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.path_, json_value);
  }

  /**
   * @brief Get of descriptor
   * @return descriptor
   */
  std::string GetDescriptor() const {
    return descriptor_;
  }
  /**
   * @brief Set to descriptor
   * @param[in] descriptor    setting value.
   */
  void SetDescriptor(  // line separate
    const std::string& descriptor) {  // NOLINT
    this->descriptor_ = descriptor;
  }
  /**
   * @brief Get data type of descriptor
   * @return Data type of descriptor
   */
  static std::string GetDescriptorFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of descriptor field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetDescriptorString(  // line separate
      const PsbtBip32Data& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.descriptor_);
  }
  /**
   * @brief Set json object to descriptor field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetDescriptorString(  // line separate
      PsbtBip32Data& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.descriptor_, json_value);
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
      const PsbtBip32DataStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  PsbtBip32DataStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using PsbtBip32DataMapTable =
    cfd::core::JsonTableMap<PsbtBip32Data>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const PsbtBip32DataMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static PsbtBip32DataMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(pubkey) value
   */
  std::string pubkey_ = "";
  /**
   * @brief JsonAPI(master_fingerprint) value
   */
  std::string master_fingerprint_ = "";
  /**
   * @brief JsonAPI(path) value
   */
  std::string path_ = "";
  /**
   * @brief JsonAPI(descriptor) value
   */
  std::string descriptor_ = "";
};

// ------------------------------------------------------------------------
// PsbtMapData
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (PsbtMapData) class
 */
class PsbtMapData
  : public cfd::core::JsonClassBase<PsbtMapData> {
 public:
  PsbtMapData() {
    CollectFieldName();
  }
  virtual ~PsbtMapData() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of key
   * @return key
   */
  std::string GetKey() const {
    return key_;
  }
  /**
   * @brief Set to key
   * @param[in] key    setting value.
   */
  void SetKey(  // line separate
    const std::string& key) {  // NOLINT
    this->key_ = key;
  }
  /**
   * @brief Get data type of key
   * @return Data type of key
   */
  static std::string GetKeyFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of key field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetKeyString(  // line separate
      const PsbtMapData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.key_);
  }
  /**
   * @brief Set json object to key field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetKeyString(  // line separate
      PsbtMapData& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.key_, json_value);
  }

  /**
   * @brief Get of value
   * @return value
   */
  std::string GetValue() const {
    return value_;
  }
  /**
   * @brief Set to value
   * @param[in] value    setting value.
   */
  void SetValue(  // line separate
    const std::string& value) {  // NOLINT
    this->value_ = value;
  }
  /**
   * @brief Get data type of value
   * @return Data type of value
   */
  static std::string GetValueFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of value field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetValueString(  // line separate
      const PsbtMapData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.value_);
  }
  /**
   * @brief Set json object to value field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetValueString(  // line separate
      PsbtMapData& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.value_, json_value);
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
      const PsbtMapDataStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  PsbtMapDataStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using PsbtMapDataMapTable =
    cfd::core::JsonTableMap<PsbtMapData>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const PsbtMapDataMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static PsbtMapDataMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(key) value
   */
  std::string key_ = "";
  /**
   * @brief JsonAPI(value) value
   */
  std::string value_ = "";
};

// ------------------------------------------------------------------------
// PsbtScriptData
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (PsbtScriptData) class
 */
class PsbtScriptData
  : public cfd::core::JsonClassBase<PsbtScriptData> {
 public:
  PsbtScriptData() {
    CollectFieldName();
  }
  virtual ~PsbtScriptData() {
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
      const PsbtScriptData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.asm__);
  }
  /**
   * @brief Set json object to asm field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetAsmString(  // line separate
      PsbtScriptData& obj,  // NOLINT
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
      const PsbtScriptData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      PsbtScriptData& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.hex_, json_value);
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
      const PsbtScriptData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.type_);
  }
  /**
   * @brief Set json object to type field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTypeString(  // line separate
      PsbtScriptData& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.type_, json_value);
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
      const PsbtScriptDataStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  PsbtScriptDataStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using PsbtScriptDataMapTable =
    cfd::core::JsonTableMap<PsbtScriptData>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const PsbtScriptDataMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static PsbtScriptDataMapTable json_mapper;
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
   * @brief JsonAPI(type) value
   */
  std::string type_ = "";
};

// ------------------------------------------------------------------------
// PsbtSignatureData
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (PsbtSignatureData) class
 */
class PsbtSignatureData
  : public cfd::core::JsonClassBase<PsbtSignatureData> {
 public:
  PsbtSignatureData() {
    CollectFieldName();
  }
  virtual ~PsbtSignatureData() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of pubkey
   * @return pubkey
   */
  std::string GetPubkey() const {
    return pubkey_;
  }
  /**
   * @brief Set to pubkey
   * @param[in] pubkey    setting value.
   */
  void SetPubkey(  // line separate
    const std::string& pubkey) {  // NOLINT
    this->pubkey_ = pubkey;
  }
  /**
   * @brief Get data type of pubkey
   * @return Data type of pubkey
   */
  static std::string GetPubkeyFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of pubkey field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPubkeyString(  // line separate
      const PsbtSignatureData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.pubkey_);
  }
  /**
   * @brief Set json object to pubkey field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPubkeyString(  // line separate
      PsbtSignatureData& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.pubkey_, json_value);
  }

  /**
   * @brief Get of signature
   * @return signature
   */
  std::string GetSignature() const {
    return signature_;
  }
  /**
   * @brief Set to signature
   * @param[in] signature    setting value.
   */
  void SetSignature(  // line separate
    const std::string& signature) {  // NOLINT
    this->signature_ = signature;
  }
  /**
   * @brief Get data type of signature
   * @return Data type of signature
   */
  static std::string GetSignatureFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of signature field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetSignatureString(  // line separate
      const PsbtSignatureData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.signature_);
  }
  /**
   * @brief Set json object to signature field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetSignatureString(  // line separate
      PsbtSignatureData& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.signature_, json_value);
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
      const PsbtSignatureDataStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  PsbtSignatureDataStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using PsbtSignatureDataMapTable =
    cfd::core::JsonTableMap<PsbtSignatureData>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const PsbtSignatureDataMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static PsbtSignatureDataMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(pubkey) value
   */
  std::string pubkey_ = "";
  /**
   * @brief JsonAPI(signature) value
   */
  std::string signature_ = "";
};

// ------------------------------------------------------------------------
// XpubData
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (XpubData) class
 */
class XpubData
  : public cfd::core::JsonClassBase<XpubData> {
 public:
  XpubData() {
    CollectFieldName();
  }
  virtual ~XpubData() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of base58
   * @return base58
   */
  std::string GetBase58() const {
    return base58_;
  }
  /**
   * @brief Set to base58
   * @param[in] base58    setting value.
   */
  void SetBase58(  // line separate
    const std::string& base58) {  // NOLINT
    this->base58_ = base58;
  }
  /**
   * @brief Get data type of base58
   * @return Data type of base58
   */
  static std::string GetBase58FieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of base58 field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetBase58String(  // line separate
      const XpubData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.base58_);
  }
  /**
   * @brief Set json object to base58 field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetBase58String(  // line separate
      XpubData& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.base58_, json_value);
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
      const XpubData& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.hex_);
  }
  /**
   * @brief Set json object to hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHexString(  // line separate
      XpubData& obj,  // NOLINT
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
      const XpubDataStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  XpubDataStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using XpubDataMapTable =
    cfd::core::JsonTableMap<XpubData>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const XpubDataMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static XpubDataMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(base58) value
   */
  std::string base58_ = "";
  /**
   * @brief JsonAPI(hex) value
   */
  std::string hex_ = "";
};

// ------------------------------------------------------------------------
// DecodePsbtInput
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodePsbtInput) class
 */
class DecodePsbtInput
  : public cfd::core::JsonClassBase<DecodePsbtInput> {
 public:
  DecodePsbtInput() {
    CollectFieldName();
  }
  virtual ~DecodePsbtInput() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of non_witness_utxo_hex
   * @return non_witness_utxo_hex
   */
  std::string GetNon_witness_utxo_hex() const {
    return non_witness_utxo_hex_;
  }
  /**
   * @brief Set to non_witness_utxo_hex
   * @param[in] non_witness_utxo_hex    setting value.
   */
  void SetNon_witness_utxo_hex(  // line separate
    const std::string& non_witness_utxo_hex) {  // NOLINT
    this->non_witness_utxo_hex_ = non_witness_utxo_hex;
  }
  /**
   * @brief Get data type of non_witness_utxo_hex
   * @return Data type of non_witness_utxo_hex
   */
  static std::string GetNon_witness_utxo_hexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of non_witness_utxo_hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetNon_witness_utxo_hexString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.non_witness_utxo_hex_);
  }
  /**
   * @brief Set json object to non_witness_utxo_hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetNon_witness_utxo_hexString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.non_witness_utxo_hex_, json_value);
  }

  /**
   * @brief Get of non_witness_utxo.
   * @return non_witness_utxo
   */
  DecodeRawTransactionResponse& GetNon_witness_utxo() {  // NOLINT
    return non_witness_utxo_;
  }
  /**
   * @brief Set to non_witness_utxo.
   * @param[in] non_witness_utxo    setting value.
   */
  void SetNon_witness_utxo(  // line separate
      const DecodeRawTransactionResponse& non_witness_utxo) {  // NOLINT
    this->non_witness_utxo_ = non_witness_utxo;
  }
  /**
   * @brief Get data type of non_witness_utxo.
   * @return Data type of non_witness_utxo.
   */
  static std::string GetNon_witness_utxoFieldType() {
    return "DecodeRawTransactionResponse";  // NOLINT
  }
  /**
   * @brief Get json string of non_witness_utxo field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetNon_witness_utxoString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.non_witness_utxo_.Serialize();
  }
  /**
   * @brief Set json object to non_witness_utxo field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetNon_witness_utxoString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.non_witness_utxo_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of witness_utxo.
   * @return witness_utxo
   */
  DecodePsbtUtxo& GetWitness_utxo() {  // NOLINT
    return witness_utxo_;
  }
  /**
   * @brief Set to witness_utxo.
   * @param[in] witness_utxo    setting value.
   */
  void SetWitness_utxo(  // line separate
      const DecodePsbtUtxo& witness_utxo) {  // NOLINT
    this->witness_utxo_ = witness_utxo;
  }
  /**
   * @brief Get data type of witness_utxo.
   * @return Data type of witness_utxo.
   */
  static std::string GetWitness_utxoFieldType() {
    return "DecodePsbtUtxo";  // NOLINT
  }
  /**
   * @brief Get json string of witness_utxo field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetWitness_utxoString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.witness_utxo_.Serialize();
  }
  /**
   * @brief Set json object to witness_utxo field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetWitness_utxoString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.witness_utxo_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of partial_signatures.
   * @return partial_signatures
   */
  JsonObjectVector<PsbtSignatureData, PsbtSignatureDataStruct>& GetPartial_signatures() {  // NOLINT
    return partial_signatures_;
  }
  /**
   * @brief Set to partial_signatures.
   * @param[in] partial_signatures    setting value.
   */
  void SetPartial_signatures(  // line separate
      const JsonObjectVector<PsbtSignatureData, PsbtSignatureDataStruct>& partial_signatures) {  // NOLINT
    this->partial_signatures_ = partial_signatures;
  }
  /**
   * @brief Get data type of partial_signatures.
   * @return Data type of partial_signatures.
   */
  static std::string GetPartial_signaturesFieldType() {
    return "JsonObjectVector<PsbtSignatureData, PsbtSignatureDataStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of partial_signatures field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetPartial_signaturesString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.partial_signatures_.Serialize();
  }
  /**
   * @brief Set json object to partial_signatures field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetPartial_signaturesString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.partial_signatures_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of sighash
   * @return sighash
   */
  std::string GetSighash() const {
    return sighash_;
  }
  /**
   * @brief Set to sighash
   * @param[in] sighash    setting value.
   */
  void SetSighash(  // line separate
    const std::string& sighash) {  // NOLINT
    this->sighash_ = sighash;
  }
  /**
   * @brief Get data type of sighash
   * @return Data type of sighash
   */
  static std::string GetSighashFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of sighash field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetSighashString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.sighash_);
  }
  /**
   * @brief Set json object to sighash field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetSighashString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.sighash_, json_value);
  }

  /**
   * @brief Get of redeem_script.
   * @return redeem_script
   */
  PsbtScriptData& GetRedeem_script() {  // NOLINT
    return redeem_script_;
  }
  /**
   * @brief Set to redeem_script.
   * @param[in] redeem_script    setting value.
   */
  void SetRedeem_script(  // line separate
      const PsbtScriptData& redeem_script) {  // NOLINT
    this->redeem_script_ = redeem_script;
  }
  /**
   * @brief Get data type of redeem_script.
   * @return Data type of redeem_script.
   */
  static std::string GetRedeem_scriptFieldType() {
    return "PsbtScriptData";  // NOLINT
  }
  /**
   * @brief Get json string of redeem_script field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetRedeem_scriptString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.redeem_script_.Serialize();
  }
  /**
   * @brief Set json object to redeem_script field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetRedeem_scriptString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.redeem_script_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of witness_script.
   * @return witness_script
   */
  PsbtScriptData& GetWitness_script() {  // NOLINT
    return witness_script_;
  }
  /**
   * @brief Set to witness_script.
   * @param[in] witness_script    setting value.
   */
  void SetWitness_script(  // line separate
      const PsbtScriptData& witness_script) {  // NOLINT
    this->witness_script_ = witness_script;
  }
  /**
   * @brief Get data type of witness_script.
   * @return Data type of witness_script.
   */
  static std::string GetWitness_scriptFieldType() {
    return "PsbtScriptData";  // NOLINT
  }
  /**
   * @brief Get json string of witness_script field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetWitness_scriptString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.witness_script_.Serialize();
  }
  /**
   * @brief Set json object to witness_script field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetWitness_scriptString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.witness_script_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of bip32_derivs.
   * @return bip32_derivs
   */
  JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct>& GetBip32_derivs() {  // NOLINT
    return bip32_derivs_;
  }
  /**
   * @brief Set to bip32_derivs.
   * @param[in] bip32_derivs    setting value.
   */
  void SetBip32_derivs(  // line separate
      const JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct>& bip32_derivs) {  // NOLINT
    this->bip32_derivs_ = bip32_derivs;
  }
  /**
   * @brief Get data type of bip32_derivs.
   * @return Data type of bip32_derivs.
   */
  static std::string GetBip32_derivsFieldType() {
    return "JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of bip32_derivs field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetBip32_derivsString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.bip32_derivs_.Serialize();
  }
  /**
   * @brief Set json object to bip32_derivs field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetBip32_derivsString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.bip32_derivs_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of final_scriptsig.
   * @return final_scriptsig
   */
  DecodeUnlockingScript& GetFinal_scriptsig() {  // NOLINT
    return final_scriptsig_;
  }
  /**
   * @brief Set to final_scriptsig.
   * @param[in] final_scriptsig    setting value.
   */
  void SetFinal_scriptsig(  // line separate
      const DecodeUnlockingScript& final_scriptsig) {  // NOLINT
    this->final_scriptsig_ = final_scriptsig;
  }
  /**
   * @brief Get data type of final_scriptsig.
   * @return Data type of final_scriptsig.
   */
  static std::string GetFinal_scriptsigFieldType() {
    return "DecodeUnlockingScript";  // NOLINT
  }
  /**
   * @brief Get json string of final_scriptsig field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetFinal_scriptsigString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.final_scriptsig_.Serialize();
  }
  /**
   * @brief Set json object to final_scriptsig field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetFinal_scriptsigString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.final_scriptsig_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of final_scriptwitness.
   * @return final_scriptwitness
   */
  JsonValueVector<std::string>& GetFinal_scriptwitness() {  // NOLINT
    return final_scriptwitness_;
  }
  /**
   * @brief Set to final_scriptwitness.
   * @param[in] final_scriptwitness    setting value.
   */
  void SetFinal_scriptwitness(  // line separate
      const JsonValueVector<std::string>& final_scriptwitness) {  // NOLINT
    this->final_scriptwitness_ = final_scriptwitness;
  }
  /**
   * @brief Get data type of final_scriptwitness.
   * @return Data type of final_scriptwitness.
   */
  static std::string GetFinal_scriptwitnessFieldType() {
    return "JsonValueVector<std::string>";  // NOLINT
  }
  /**
   * @brief Get json string of final_scriptwitness field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetFinal_scriptwitnessString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.final_scriptwitness_.Serialize();
  }
  /**
   * @brief Set json object to final_scriptwitness field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetFinal_scriptwitnessString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.final_scriptwitness_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of unknown.
   * @return unknown
   */
  JsonObjectVector<PsbtMapData, PsbtMapDataStruct>& GetUnknown() {  // NOLINT
    return unknown_;
  }
  /**
   * @brief Set to unknown.
   * @param[in] unknown    setting value.
   */
  void SetUnknown(  // line separate
      const JsonObjectVector<PsbtMapData, PsbtMapDataStruct>& unknown) {  // NOLINT
    this->unknown_ = unknown;
  }
  /**
   * @brief Get data type of unknown.
   * @return Data type of unknown.
   */
  static std::string GetUnknownFieldType() {
    return "JsonObjectVector<PsbtMapData, PsbtMapDataStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of unknown field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetUnknownString(  // line separate
      const DecodePsbtInput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.unknown_.Serialize();
  }
  /**
   * @brief Set json object to unknown field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetUnknownString(  // line separate
      DecodePsbtInput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.unknown_.DeserializeUniValue(json_value);
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
      const DecodePsbtInputStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodePsbtInputStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodePsbtInputMapTable =
    cfd::core::JsonTableMap<DecodePsbtInput>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodePsbtInputMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static DecodePsbtInputMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(non_witness_utxo_hex) value
   */
  std::string non_witness_utxo_hex_ = "";
  /**
   * @brief JsonAPI(non_witness_utxo) value
   */
  DecodeRawTransactionResponse non_witness_utxo_;  // NOLINT
  /**
   * @brief JsonAPI(witness_utxo) value
   */
  DecodePsbtUtxo witness_utxo_;  // NOLINT
  /**
   * @brief JsonAPI(partial_signatures) value
   */
  JsonObjectVector<PsbtSignatureData, PsbtSignatureDataStruct> partial_signatures_;  // NOLINT
  /**
   * @brief JsonAPI(sighash) value
   */
  std::string sighash_ = "";
  /**
   * @brief JsonAPI(redeem_script) value
   */
  PsbtScriptData redeem_script_;  // NOLINT
  /**
   * @brief JsonAPI(witness_script) value
   */
  PsbtScriptData witness_script_;  // NOLINT
  /**
   * @brief JsonAPI(bip32_derivs) value
   */
  JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct> bip32_derivs_;  // NOLINT
  /**
   * @brief JsonAPI(final_scriptsig) value
   */
  DecodeUnlockingScript final_scriptsig_;  // NOLINT
  /**
   * @brief JsonAPI(final_scriptwitness) value
   */
  JsonValueVector<std::string> final_scriptwitness_;  // NOLINT
  /**
   * @brief JsonAPI(unknown) value
   */
  JsonObjectVector<PsbtMapData, PsbtMapDataStruct> unknown_;  // NOLINT
};

// ------------------------------------------------------------------------
// DecodePsbtOutput
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodePsbtOutput) class
 */
class DecodePsbtOutput
  : public cfd::core::JsonClassBase<DecodePsbtOutput> {
 public:
  DecodePsbtOutput() {
    CollectFieldName();
  }
  virtual ~DecodePsbtOutput() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of redeem_script.
   * @return redeem_script
   */
  PsbtScriptData& GetRedeem_script() {  // NOLINT
    return redeem_script_;
  }
  /**
   * @brief Set to redeem_script.
   * @param[in] redeem_script    setting value.
   */
  void SetRedeem_script(  // line separate
      const PsbtScriptData& redeem_script) {  // NOLINT
    this->redeem_script_ = redeem_script;
  }
  /**
   * @brief Get data type of redeem_script.
   * @return Data type of redeem_script.
   */
  static std::string GetRedeem_scriptFieldType() {
    return "PsbtScriptData";  // NOLINT
  }
  /**
   * @brief Get json string of redeem_script field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetRedeem_scriptString(  // line separate
      const DecodePsbtOutput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.redeem_script_.Serialize();
  }
  /**
   * @brief Set json object to redeem_script field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetRedeem_scriptString(  // line separate
      DecodePsbtOutput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.redeem_script_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of witness_script.
   * @return witness_script
   */
  PsbtScriptData& GetWitness_script() {  // NOLINT
    return witness_script_;
  }
  /**
   * @brief Set to witness_script.
   * @param[in] witness_script    setting value.
   */
  void SetWitness_script(  // line separate
      const PsbtScriptData& witness_script) {  // NOLINT
    this->witness_script_ = witness_script;
  }
  /**
   * @brief Get data type of witness_script.
   * @return Data type of witness_script.
   */
  static std::string GetWitness_scriptFieldType() {
    return "PsbtScriptData";  // NOLINT
  }
  /**
   * @brief Get json string of witness_script field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetWitness_scriptString(  // line separate
      const DecodePsbtOutput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.witness_script_.Serialize();
  }
  /**
   * @brief Set json object to witness_script field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetWitness_scriptString(  // line separate
      DecodePsbtOutput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.witness_script_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of bip32_derivs.
   * @return bip32_derivs
   */
  JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct>& GetBip32_derivs() {  // NOLINT
    return bip32_derivs_;
  }
  /**
   * @brief Set to bip32_derivs.
   * @param[in] bip32_derivs    setting value.
   */
  void SetBip32_derivs(  // line separate
      const JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct>& bip32_derivs) {  // NOLINT
    this->bip32_derivs_ = bip32_derivs;
  }
  /**
   * @brief Get data type of bip32_derivs.
   * @return Data type of bip32_derivs.
   */
  static std::string GetBip32_derivsFieldType() {
    return "JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of bip32_derivs field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetBip32_derivsString(  // line separate
      const DecodePsbtOutput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.bip32_derivs_.Serialize();
  }
  /**
   * @brief Set json object to bip32_derivs field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetBip32_derivsString(  // line separate
      DecodePsbtOutput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.bip32_derivs_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of unknown.
   * @return unknown
   */
  JsonObjectVector<PsbtMapData, PsbtMapDataStruct>& GetUnknown() {  // NOLINT
    return unknown_;
  }
  /**
   * @brief Set to unknown.
   * @param[in] unknown    setting value.
   */
  void SetUnknown(  // line separate
      const JsonObjectVector<PsbtMapData, PsbtMapDataStruct>& unknown) {  // NOLINT
    this->unknown_ = unknown;
  }
  /**
   * @brief Get data type of unknown.
   * @return Data type of unknown.
   */
  static std::string GetUnknownFieldType() {
    return "JsonObjectVector<PsbtMapData, PsbtMapDataStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of unknown field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetUnknownString(  // line separate
      const DecodePsbtOutput& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.unknown_.Serialize();
  }
  /**
   * @brief Set json object to unknown field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetUnknownString(  // line separate
      DecodePsbtOutput& obj,  // NOLINT
      const UniValue& json_value) {
    obj.unknown_.DeserializeUniValue(json_value);
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
      const DecodePsbtOutputStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodePsbtOutputStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodePsbtOutputMapTable =
    cfd::core::JsonTableMap<DecodePsbtOutput>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodePsbtOutputMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static DecodePsbtOutputMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(redeem_script) value
   */
  PsbtScriptData redeem_script_;  // NOLINT
  /**
   * @brief JsonAPI(witness_script) value
   */
  PsbtScriptData witness_script_;  // NOLINT
  /**
   * @brief JsonAPI(bip32_derivs) value
   */
  JsonObjectVector<PsbtBip32Data, PsbtBip32DataStruct> bip32_derivs_;  // NOLINT
  /**
   * @brief JsonAPI(unknown) value
   */
  JsonObjectVector<PsbtMapData, PsbtMapDataStruct> unknown_;  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  int64_t GetValue() const {
    return value_;
  }
  /**
   * @brief Set to value
   * @param[in] value    setting value.
   */
  void SetValue(  // line separate
    const int64_t& value) {  // NOLINT
    this->value_ = value;
  }
  /**
   * @brief Get data type of value
   * @return Data type of value
   */
  static std::string GetValueFieldType() {
    return "int64_t";
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
  int64_t GetValue_minimum() const {
    return value_minimum_;
  }
  /**
   * @brief Set to value-minimum
   * @param[in] value_minimum    setting value.
   */
  void SetValue_minimum(  // line separate
    const int64_t& value_minimum) {  // NOLINT
    this->value_minimum_ = value_minimum;
  }
  /**
   * @brief Get data type of value-minimum
   * @return Data type of value-minimum
   */
  static std::string GetValue_minimumFieldType() {
    return "int64_t";
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
  int64_t GetValue_maximum() const {
    return value_maximum_;
  }
  /**
   * @brief Set to value-maximum
   * @param[in] value_maximum    setting value.
   */
  void SetValue_maximum(  // line separate
    const int64_t& value_maximum) {  // NOLINT
    this->value_maximum_ = value_maximum;
  }
  /**
   * @brief Get data type of value-maximum
   * @return Data type of value-maximum
   */
  static std::string GetValue_maximumFieldType() {
    return "int64_t";
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
   * @brief Get of rangeproof
   * @return rangeproof
   */
  std::string GetRangeproof() const {
    return rangeproof_;
  }
  /**
   * @brief Set to rangeproof
   * @param[in] rangeproof    setting value.
   */
  void SetRangeproof(  // line separate
    const std::string& rangeproof) {  // NOLINT
    this->rangeproof_ = rangeproof;
  }
  /**
   * @brief Get data type of rangeproof
   * @return Data type of rangeproof
   */
  static std::string GetRangeproofFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of rangeproof field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetRangeproofString(  // line separate
      const ElementsDecodeRawTransactionTxOut& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.rangeproof_);
  }
  /**
   * @brief Set json object to rangeproof field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetRangeproofString(  // line separate
      ElementsDecodeRawTransactionTxOut& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.rangeproof_, json_value);
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
   * @brief Get ignore item lists of JSON mapping.
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
  int64_t value_ = 0;
  /**
   * @brief JsonAPI(value-minimum) value
   */
  int64_t value_minimum_ = 0;
  /**
   * @brief JsonAPI(value-maximum) value
   */
  int64_t value_maximum_ = 0;
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
  /**
   * @brief JsonAPI(rangeproof) value
   */
  std::string rangeproof_ = "";
};

// ------------------------------------------------------------------------
// PsbtGlobalXpub
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (PsbtGlobalXpub) class
 */
class PsbtGlobalXpub
  : public cfd::core::JsonClassBase<PsbtGlobalXpub> {
 public:
  PsbtGlobalXpub() {
    CollectFieldName();
  }
  virtual ~PsbtGlobalXpub() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of xpub.
   * @return xpub
   */
  XpubData& GetXpub() {  // NOLINT
    return xpub_;
  }
  /**
   * @brief Set to xpub.
   * @param[in] xpub    setting value.
   */
  void SetXpub(  // line separate
      const XpubData& xpub) {  // NOLINT
    this->xpub_ = xpub;
  }
  /**
   * @brief Get data type of xpub.
   * @return Data type of xpub.
   */
  static std::string GetXpubFieldType() {
    return "XpubData";  // NOLINT
  }
  /**
   * @brief Get json string of xpub field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetXpubString(  // line separate
      const PsbtGlobalXpub& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.xpub_.Serialize();
  }
  /**
   * @brief Set json object to xpub field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetXpubString(  // line separate
      PsbtGlobalXpub& obj,  // NOLINT
      const UniValue& json_value) {
    obj.xpub_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of master_fingerprint
   * @return master_fingerprint
   */
  std::string GetMaster_fingerprint() const {
    return master_fingerprint_;
  }
  /**
   * @brief Set to master_fingerprint
   * @param[in] master_fingerprint    setting value.
   */
  void SetMaster_fingerprint(  // line separate
    const std::string& master_fingerprint) {  // NOLINT
    this->master_fingerprint_ = master_fingerprint;
  }
  /**
   * @brief Get data type of master_fingerprint
   * @return Data type of master_fingerprint
   */
  static std::string GetMaster_fingerprintFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of master_fingerprint field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetMaster_fingerprintString(  // line separate
      const PsbtGlobalXpub& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.master_fingerprint_);
  }
  /**
   * @brief Set json object to master_fingerprint field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetMaster_fingerprintString(  // line separate
      PsbtGlobalXpub& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.master_fingerprint_, json_value);
  }

  /**
   * @brief Get of path
   * @return path
   */
  std::string GetPath() const {
    return path_;
  }
  /**
   * @brief Set to path
   * @param[in] path    setting value.
   */
  void SetPath(  // line separate
    const std::string& path) {  // NOLINT
    this->path_ = path;
  }
  /**
   * @brief Get data type of path
   * @return Data type of path
   */
  static std::string GetPathFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of path field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPathString(  // line separate
      const PsbtGlobalXpub& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.path_);
  }
  /**
   * @brief Set json object to path field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPathString(  // line separate
      PsbtGlobalXpub& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.path_, json_value);
  }

  /**
   * @brief Get of descriptorXpub
   * @return descriptorXpub
   */
  std::string GetDescriptorXpub() const {
    return descriptor_xpub_;
  }
  /**
   * @brief Set to descriptorXpub
   * @param[in] descriptor_xpub    setting value.
   */
  void SetDescriptorXpub(  // line separate
    const std::string& descriptor_xpub) {  // NOLINT
    this->descriptor_xpub_ = descriptor_xpub;
  }
  /**
   * @brief Get data type of descriptorXpub
   * @return Data type of descriptorXpub
   */
  static std::string GetDescriptorXpubFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of descriptorXpub field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetDescriptorXpubString(  // line separate
      const PsbtGlobalXpub& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.descriptor_xpub_);
  }
  /**
   * @brief Set json object to descriptorXpub field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetDescriptorXpubString(  // line separate
      PsbtGlobalXpub& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.descriptor_xpub_, json_value);
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
      const PsbtGlobalXpubStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  PsbtGlobalXpubStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using PsbtGlobalXpubMapTable =
    cfd::core::JsonTableMap<PsbtGlobalXpub>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const PsbtGlobalXpubMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static PsbtGlobalXpubMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(xpub) value
   */
  XpubData xpub_;  // NOLINT
  /**
   * @brief JsonAPI(master_fingerprint) value
   */
  std::string master_fingerprint_ = "";
  /**
   * @brief JsonAPI(path) value
   */
  std::string path_ = "";
  /**
   * @brief JsonAPI(descriptorXpub) value
   */
  std::string descriptor_xpub_ = "";
};

// ------------------------------------------------------------------------
// DecodePsbtRequest
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodePsbtRequest) class
 */
class DecodePsbtRequest
  : public cfd::core::JsonClassBase<DecodePsbtRequest> {
 public:
  DecodePsbtRequest() {
    CollectFieldName();
  }
  virtual ~DecodePsbtRequest() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of psbt
   * @return psbt
   */
  std::string GetPsbt() const {
    return psbt_;
  }
  /**
   * @brief Set to psbt
   * @param[in] psbt    setting value.
   */
  void SetPsbt(  // line separate
    const std::string& psbt) {  // NOLINT
    this->psbt_ = psbt;
  }
  /**
   * @brief Get data type of psbt
   * @return Data type of psbt
   */
  static std::string GetPsbtFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of psbt field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetPsbtString(  // line separate
      const DecodePsbtRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.psbt_);
  }
  /**
   * @brief Set json object to psbt field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetPsbtString(  // line separate
      DecodePsbtRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.psbt_, json_value);
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
      const DecodePsbtRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.network_);
  }
  /**
   * @brief Set json object to network field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetNetworkString(  // line separate
      DecodePsbtRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.network_, json_value);
  }

  /**
   * @brief Get of hasDetail
   * @return hasDetail
   */
  bool GetHasDetail() const {
    return has_detail_;
  }
  /**
   * @brief Set to hasDetail
   * @param[in] has_detail    setting value.
   */
  void SetHasDetail(  // line separate
    const bool& has_detail) {  // NOLINT
    this->has_detail_ = has_detail;
  }
  /**
   * @brief Get data type of hasDetail
   * @return Data type of hasDetail
   */
  static std::string GetHasDetailFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of hasDetail field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHasDetailString(  // line separate
      const DecodePsbtRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.has_detail_);
  }
  /**
   * @brief Set json object to hasDetail field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHasDetailString(  // line separate
      DecodePsbtRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.has_detail_, json_value);
  }

  /**
   * @brief Get of hasSimple
   * @return hasSimple
   */
  bool GetHasSimple() const {
    return has_simple_;
  }
  /**
   * @brief Set to hasSimple
   * @param[in] has_simple    setting value.
   */
  void SetHasSimple(  // line separate
    const bool& has_simple) {  // NOLINT
    this->has_simple_ = has_simple;
  }
  /**
   * @brief Get data type of hasSimple
   * @return Data type of hasSimple
   */
  static std::string GetHasSimpleFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of hasSimple field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetHasSimpleString(  // line separate
      const DecodePsbtRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.has_simple_);
  }
  /**
   * @brief Set json object to hasSimple field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetHasSimpleString(  // line separate
      DecodePsbtRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.has_simple_, json_value);
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
      const DecodePsbtRequestStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodePsbtRequestStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodePsbtRequestMapTable =
    cfd::core::JsonTableMap<DecodePsbtRequest>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodePsbtRequestMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static DecodePsbtRequestMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(psbt) value
   */
  std::string psbt_ = "";
  /**
   * @brief JsonAPI(network) value
   */
  std::string network_ = "mainnet";
  /**
   * @brief JsonAPI(hasDetail) value
   */
  bool has_detail_ = false;
  /**
   * @brief JsonAPI(hasSimple) value
   */
  bool has_simple_ = false;
};

// ------------------------------------------------------------------------
// DecodePsbtResponse
// ------------------------------------------------------------------------
/**
 * @brief JSON-API (DecodePsbtResponse) class
 */
class DecodePsbtResponse
  : public cfd::core::JsonClassBase<DecodePsbtResponse> {
 public:
  DecodePsbtResponse() {
    CollectFieldName();
  }
  virtual ~DecodePsbtResponse() {
    // do nothing
  }
  /**
   * @brief collect field name.
   */
  static void CollectFieldName();

  /**
   * @brief Get of tx.
   * @return tx
   */
  DecodeRawTransactionResponse& GetTx() {  // NOLINT
    return tx_;
  }
  /**
   * @brief Set to tx.
   * @param[in] tx    setting value.
   */
  void SetTx(  // line separate
      const DecodeRawTransactionResponse& tx) {  // NOLINT
    this->tx_ = tx;
  }
  /**
   * @brief Get data type of tx.
   * @return Data type of tx.
   */
  static std::string GetTxFieldType() {
    return "DecodeRawTransactionResponse";  // NOLINT
  }
  /**
   * @brief Get json string of tx field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetTxString(  // line separate
      const DecodePsbtResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.tx_.Serialize();
  }
  /**
   * @brief Set json object to tx field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetTxString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.tx_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of tx_hex
   * @return tx_hex
   */
  std::string GetTx_hex() const {
    return tx_hex_;
  }
  /**
   * @brief Set to tx_hex
   * @param[in] tx_hex    setting value.
   */
  void SetTx_hex(  // line separate
    const std::string& tx_hex) {  // NOLINT
    this->tx_hex_ = tx_hex;
  }
  /**
   * @brief Get data type of tx_hex
   * @return Data type of tx_hex
   */
  static std::string GetTx_hexFieldType() {
    return "std::string";
  }
  /**
   * @brief Get json string of tx_hex field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetTx_hexString(  // line separate
      const DecodePsbtResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.tx_hex_);
  }
  /**
   * @brief Set json object to tx_hex field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetTx_hexString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.tx_hex_, json_value);
  }

  /**
   * @brief Get of xpubs.
   * @return xpubs
   */
  JsonObjectVector<PsbtGlobalXpub, PsbtGlobalXpubStruct>& GetXpubs() {  // NOLINT
    return xpubs_;
  }
  /**
   * @brief Set to xpubs.
   * @param[in] xpubs    setting value.
   */
  void SetXpubs(  // line separate
      const JsonObjectVector<PsbtGlobalXpub, PsbtGlobalXpubStruct>& xpubs) {  // NOLINT
    this->xpubs_ = xpubs;
  }
  /**
   * @brief Get data type of xpubs.
   * @return Data type of xpubs.
   */
  static std::string GetXpubsFieldType() {
    return "JsonObjectVector<PsbtGlobalXpub, PsbtGlobalXpubStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of xpubs field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetXpubsString(  // line separate
      const DecodePsbtResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.xpubs_.Serialize();
  }
  /**
   * @brief Set json object to xpubs field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetXpubsString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.xpubs_.DeserializeUniValue(json_value);
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
      const DecodePsbtResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.version_);
  }
  /**
   * @brief Set json object to version field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetVersionString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.version_, json_value);
  }

  /**
   * @brief Get of unknown.
   * @return unknown
   */
  JsonObjectVector<PsbtMapData, PsbtMapDataStruct>& GetUnknown() {  // NOLINT
    return unknown_;
  }
  /**
   * @brief Set to unknown.
   * @param[in] unknown    setting value.
   */
  void SetUnknown(  // line separate
      const JsonObjectVector<PsbtMapData, PsbtMapDataStruct>& unknown) {  // NOLINT
    this->unknown_ = unknown;
  }
  /**
   * @brief Get data type of unknown.
   * @return Data type of unknown.
   */
  static std::string GetUnknownFieldType() {
    return "JsonObjectVector<PsbtMapData, PsbtMapDataStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of unknown field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetUnknownString(  // line separate
      const DecodePsbtResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.unknown_.Serialize();
  }
  /**
   * @brief Set json object to unknown field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetUnknownString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.unknown_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of inputs.
   * @return inputs
   */
  JsonObjectVector<DecodePsbtInput, DecodePsbtInputStruct>& GetInputs() {  // NOLINT
    return inputs_;
  }
  /**
   * @brief Set to inputs.
   * @param[in] inputs    setting value.
   */
  void SetInputs(  // line separate
      const JsonObjectVector<DecodePsbtInput, DecodePsbtInputStruct>& inputs) {  // NOLINT
    this->inputs_ = inputs;
  }
  /**
   * @brief Get data type of inputs.
   * @return Data type of inputs.
   */
  static std::string GetInputsFieldType() {
    return "JsonObjectVector<DecodePsbtInput, DecodePsbtInputStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of inputs field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetInputsString(  // line separate
      const DecodePsbtResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.inputs_.Serialize();
  }
  /**
   * @brief Set json object to inputs field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetInputsString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.inputs_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of outputs.
   * @return outputs
   */
  JsonObjectVector<DecodePsbtOutput, DecodePsbtOutputStruct>& GetOutputs() {  // NOLINT
    return outputs_;
  }
  /**
   * @brief Set to outputs.
   * @param[in] outputs    setting value.
   */
  void SetOutputs(  // line separate
      const JsonObjectVector<DecodePsbtOutput, DecodePsbtOutputStruct>& outputs) {  // NOLINT
    this->outputs_ = outputs;
  }
  /**
   * @brief Get data type of outputs.
   * @return Data type of outputs.
   */
  static std::string GetOutputsFieldType() {
    return "JsonObjectVector<DecodePsbtOutput, DecodePsbtOutputStruct>";  // NOLINT
  }
  /**
   * @brief Get json string of outputs field.
   * @param[in,out] obj     class object
   * @return JSON string.
   */
  static std::string GetOutputsString(  // line separate
      const DecodePsbtResponse& obj) {  // NOLINT
    // Do not set to const, because substitution of member variables
    // may occur in pre / post processing inside Serialize
    return obj.outputs_.Serialize();
  }
  /**
   * @brief Set json object to outputs field.
   * @param[in,out] obj     class object
   * @param[in] json_value  JSON object
   */
  static void SetOutputsString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    obj.outputs_.DeserializeUniValue(json_value);
  }

  /**
   * @brief Get of fee
   * @return fee
   */
  int64_t GetFee() const {
    return fee_;
  }
  /**
   * @brief Set to fee
   * @param[in] fee    setting value.
   */
  void SetFee(  // line separate
    const int64_t& fee) {  // NOLINT
    this->fee_ = fee;
  }
  /**
   * @brief Get data type of fee
   * @return Data type of fee
   */
  static std::string GetFeeFieldType() {
    return "int64_t";
  }
  /**
   * @brief Get json string of fee field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetFeeString(  // line separate
      const DecodePsbtResponse& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.fee_);
  }
  /**
   * @brief Set json object to fee field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetFeeString(  // line separate
      DecodePsbtResponse& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.fee_, json_value);
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
      const DecodePsbtResponseStruct& data);

  /**
   * @brief Convert class to struct.
   * @return  struct data.
   */
  DecodePsbtResponseStruct ConvertToStruct()  const;

 protected:
  /**
   * @brief definition type of Map table.
   */
  using DecodePsbtResponseMapTable =
    cfd::core::JsonTableMap<DecodePsbtResponse>;

  /**
   * @brief Get JSON mapping object.
   * @return JSON mapping object.
   * @see cfd::core::JsonClassBase::GetJsonMapper()
   */
  virtual const DecodePsbtResponseMapTable& GetJsonMapper() const {  // NOLINT
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
   * @brief Get ignore item lists of JSON mapping.
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
  static DecodePsbtResponseMapTable json_mapper;
  /**
   * @brief field name list.
   */
  static std::vector<std::string> item_list;
  /**
   * @brief ignore item list.
   */
  std::set<std::string> ignore_items;

  /**
   * @brief JsonAPI(tx) value
   */
  DecodeRawTransactionResponse tx_;  // NOLINT
  /**
   * @brief JsonAPI(tx_hex) value
   */
  std::string tx_hex_ = "";
  /**
   * @brief JsonAPI(xpubs) value
   */
  JsonObjectVector<PsbtGlobalXpub, PsbtGlobalXpubStruct> xpubs_;  // NOLINT
  /**
   * @brief JsonAPI(version) value
   */
  uint32_t version_ = 0;
  /**
   * @brief JsonAPI(unknown) value
   */
  JsonObjectVector<PsbtMapData, PsbtMapDataStruct> unknown_;  // NOLINT
  /**
   * @brief JsonAPI(inputs) value
   */
  JsonObjectVector<DecodePsbtInput, DecodePsbtInputStruct> inputs_;  // NOLINT
  /**
   * @brief JsonAPI(outputs) value
   */
  JsonObjectVector<DecodePsbtOutput, DecodePsbtOutputStruct> outputs_;  // NOLINT
  /**
   * @brief JsonAPI(fee) value
   */
  int64_t fee_ = 0;
};

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
   * @brief Get ignore item lists of JSON mapping.
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
   * @brief Get of fullDump
   * @return fullDump
   */
  bool GetFullDump() const {
    return full_dump_;
  }
  /**
   * @brief Set to fullDump
   * @param[in] full_dump    setting value.
   */
  void SetFullDump(  // line separate
    const bool& full_dump) {  // NOLINT
    this->full_dump_ = full_dump;
  }
  /**
   * @brief Get data type of fullDump
   * @return Data type of fullDump
   */
  static std::string GetFullDumpFieldType() {
    return "bool";
  }
  /**
   * @brief Get json string of fullDump field.
   * @param[in,out] obj     class object.
   * @return JSON string
   */
  static std::string GetFullDumpString(  // line separate
      const ElementsDecodeRawTransactionRequest& obj) {  // NOLINT
    return cfd::core::ConvertToString(obj.full_dump_);
  }
  /**
   * @brief Set json object to fullDump field.
   * @param[in,out] obj     class object.
   * @param[in] json_value  JSON object.
   */
  static void SetFullDumpString(  // line separate
      ElementsDecodeRawTransactionRequest& obj,  // NOLINT
      const UniValue& json_value) {
    cfd::core::ConvertFromUniValue(  // line separate
      obj.full_dump_, json_value);
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
   * @brief Get ignore item lists of JSON mapping.
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
  /**
   * @brief JsonAPI(fullDump) value
   */
  bool full_dump_ = false;
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
   * @brief Get ignore item lists of JSON mapping.
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
