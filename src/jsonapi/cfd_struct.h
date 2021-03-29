// Copyright 2020 CryptoGarage
/**
 * @file cfd_struct.h
 *
 * @brief Struct mapping file. (auto generate)
 */
#ifndef CFD_SRC_JSONAPI_CFD_STRUCT_H_
#define CFD_SRC_JSONAPI_CFD_STRUCT_H_

#include <cstdint>
#include <set>
#include <string>
#include <vector>

// clang-format off
// @formatter:off

// ------------------------------------------------------------------------
// DecodeLockingScriptStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeLockingScriptStruct struct
 */
struct DecodeLockingScriptStruct {
  std::string asm_ = "";               //!< asm_  // NOLINT
  std::string hex = "";                //!< hex  // NOLINT
  int req_sigs = 0;                    //!< req_sigs  // NOLINT
  std::string type = "";               //!< type  // NOLINT
  std::vector<std::string> addresses;  //!< addresses  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodeUnlockingScriptStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeUnlockingScriptStruct struct
 */
struct DecodeUnlockingScriptStruct {
  std::string asm_ = "";  //!< asm_  // NOLINT
  std::string hex = "";   //!< hex  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodePsbtLockingScriptStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodePsbtLockingScriptStruct struct
 */
struct DecodePsbtLockingScriptStruct {
  std::string asm_ = "";     //!< asm_  // NOLINT
  std::string hex = "";      //!< hex  // NOLINT
  std::string type = "";     //!< type  // NOLINT
  std::string address = "";  //!< address  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodeRawTransactionTxInStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeRawTransactionTxInStruct struct
 */
struct DecodeRawTransactionTxInStruct {
  std::string coinbase = "";               //!< coinbase  // NOLINT
  std::string txid = "";                   //!< txid  // NOLINT
  uint32_t vout = 0;                       //!< vout  // NOLINT
  DecodeUnlockingScriptStruct script_sig;  //!< script_sig  // NOLINT
  std::vector<std::string> txinwitness;    //!< txinwitness  // NOLINT
  uint32_t sequence = 0;                   //!< sequence  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodeRawTransactionTxOutStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeRawTransactionTxOutStruct struct
 */
struct DecodeRawTransactionTxOutStruct {
  int64_t value = 0;                         //!< value  // NOLINT
  uint32_t n = 0;                            //!< n  // NOLINT
  DecodeLockingScriptStruct script_pub_key;  //!< script_pub_key  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodePsbtUtxoStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodePsbtUtxoStruct struct
 */
struct DecodePsbtUtxoStruct {
  int64_t amount = 0;                            //!< amount  // NOLINT
  DecodePsbtLockingScriptStruct script_pub_key;  //!< script_pub_key  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodeRawTransactionResponseStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeRawTransactionResponseStruct struct
 */
struct DecodeRawTransactionResponseStruct {
  std::string txid = "";                              //!< txid  // NOLINT
  std::string hash = "";                              //!< hash  // NOLINT
  uint32_t version = 0;                               //!< version  // NOLINT
  uint32_t size = 0;                                  //!< size  // NOLINT
  uint32_t vsize = 0;                                 //!< vsize  // NOLINT
  uint32_t weight = 0;                                //!< weight  // NOLINT
  uint32_t locktime = 0;                              //!< locktime  // NOLINT
  std::vector<DecodeRawTransactionTxInStruct> vin;    //!< vin  // NOLINT
  std::vector<DecodeRawTransactionTxOutStruct> vout;  //!< vout  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// ElementsDecodeIssuanceStruct
// ------------------------------------------------------------------------
/**
 * @brief ElementsDecodeIssuanceStruct struct
 */
struct ElementsDecodeIssuanceStruct {
  std::string asset_blinding_nonce = "";   //!< asset_blinding_nonce  // NOLINT
  std::string asset_entropy = "";          //!< asset_entropy  // NOLINT
  std::string contract_hash = "";          //!< contract_hash  // NOLINT
  bool isreissuance = false;               //!< isreissuance  // NOLINT
  std::string token = "";                  //!< token  // NOLINT
  std::string asset = "";                  //!< asset  // NOLINT
  int64_t assetamount = 0;                 //!< assetamount  // NOLINT
  std::string assetamountcommitment = "";  //!< assetamountcommitment  // NOLINT
  int64_t tokenamount = 0;                 //!< tokenamount  // NOLINT
  std::string tokenamountcommitment = "";  //!< tokenamountcommitment  // NOLINT
  std::string asset_rangeproof = "";       //!< asset_rangeproof  // NOLINT
  std::string token_rangeproof = "";       //!< token_rangeproof  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// ElementsDecodeLockingScriptStruct
// ------------------------------------------------------------------------
/**
 * @brief ElementsDecodeLockingScriptStruct struct
 */
struct ElementsDecodeLockingScriptStruct {
  std::string asm_ = "";                      //!< asm_  // NOLINT
  std::string hex = "";                       //!< hex  // NOLINT
  int req_sigs = 0;                           //!< req_sigs  // NOLINT
  std::string type = "";                      //!< type  // NOLINT
  std::vector<std::string> addresses;         //!< addresses  // NOLINT
  std::string pegout_chain = "";              //!< pegout_chain  // NOLINT
  std::string pegout_asm = "";                //!< pegout_asm  // NOLINT
  std::string pegout_hex = "";                //!< pegout_hex  // NOLINT
  int pegout_req_sigs = 0;                    //!< pegout_req_sigs  // NOLINT
  std::string pegout_type = "";               //!< pegout_type  // NOLINT
  std::vector<std::string> pegout_addresses;  //!< pegout_addresses  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// ElementsDecodeUnlockingScriptStruct
// ------------------------------------------------------------------------
/**
 * @brief ElementsDecodeUnlockingScriptStruct struct
 */
struct ElementsDecodeUnlockingScriptStruct {
  std::string asm_ = "";  //!< asm_  // NOLINT
  std::string hex = "";   //!< hex  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// PsbtBip32DataStruct
// ------------------------------------------------------------------------
/**
 * @brief PsbtBip32DataStruct struct
 */
struct PsbtBip32DataStruct {
  std::string pubkey = "";              //!< pubkey  // NOLINT
  std::string master_fingerprint = "";  //!< master_fingerprint  // NOLINT
  std::string path = "";                //!< path  // NOLINT
  std::string descriptor = "";          //!< descriptor  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// PsbtMapDataStruct
// ------------------------------------------------------------------------
/**
 * @brief PsbtMapDataStruct struct
 */
struct PsbtMapDataStruct {
  std::string key = "";    //!< key  // NOLINT
  std::string value = "";  //!< value  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// PsbtScriptDataStruct
// ------------------------------------------------------------------------
/**
 * @brief PsbtScriptDataStruct struct
 */
struct PsbtScriptDataStruct {
  std::string asm_ = "";  //!< asm_  // NOLINT
  std::string hex = "";   //!< hex  // NOLINT
  std::string type = "";  //!< type  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// PsbtSignatureDataStruct
// ------------------------------------------------------------------------
/**
 * @brief PsbtSignatureDataStruct struct
 */
struct PsbtSignatureDataStruct {
  std::string pubkey = "";     //!< pubkey  // NOLINT
  std::string signature = "";  //!< signature  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// XpubDataStruct
// ------------------------------------------------------------------------
/**
 * @brief XpubDataStruct struct
 */
struct XpubDataStruct {
  std::string base58 = "";  //!< base58  // NOLINT
  std::string hex = "";     //!< hex  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodePsbtInputStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodePsbtInputStruct struct
 */
struct DecodePsbtInputStruct {
  std::string non_witness_utxo_hex = "";                    //!< non_witness_utxo_hex  // NOLINT
  DecodeRawTransactionResponseStruct non_witness_utxo;      //!< non_witness_utxo  // NOLINT
  DecodePsbtUtxoStruct witness_utxo;                        //!< witness_utxo  // NOLINT
  std::vector<PsbtSignatureDataStruct> partial_signatures;  //!< partial_signatures  // NOLINT
  std::string sighash = "";                                 //!< sighash  // NOLINT
  PsbtScriptDataStruct redeem_script;                       //!< redeem_script  // NOLINT
  PsbtScriptDataStruct witness_script;                      //!< witness_script  // NOLINT
  std::vector<PsbtBip32DataStruct> bip32_derivs;            //!< bip32_derivs  // NOLINT
  DecodeUnlockingScriptStruct final_scriptsig;              //!< final_scriptsig  // NOLINT
  std::vector<std::string> final_scriptwitness;             //!< final_scriptwitness  // NOLINT
  std::vector<PsbtMapDataStruct> unknown;                   //!< unknown  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodePsbtOutputStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodePsbtOutputStruct struct
 */
struct DecodePsbtOutputStruct {
  PsbtScriptDataStruct redeem_script;             //!< redeem_script  // NOLINT
  PsbtScriptDataStruct witness_script;            //!< witness_script  // NOLINT
  std::vector<PsbtBip32DataStruct> bip32_derivs;  //!< bip32_derivs  // NOLINT
  std::vector<PsbtMapDataStruct> unknown;         //!< unknown  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionTxInStruct
// ------------------------------------------------------------------------
/**
 * @brief ElementsDecodeRawTransactionTxInStruct struct
 */
struct ElementsDecodeRawTransactionTxInStruct {
  std::string coinbase = "";                       //!< coinbase  // NOLINT
  std::string txid = "";                           //!< txid  // NOLINT
  uint32_t vout = 0;                               //!< vout  // NOLINT
  ElementsDecodeUnlockingScriptStruct script_sig;  //!< script_sig  // NOLINT
  bool is_pegin = false;                           //!< is_pegin  // NOLINT
  int64_t sequence = 0;                            //!< sequence  // NOLINT
  std::vector<std::string> txinwitness;            //!< txinwitness  // NOLINT
  std::vector<std::string> pegin_witness;          //!< pegin_witness  // NOLINT
  ElementsDecodeIssuanceStruct issuance;           //!< issuance  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionTxOutStruct
// ------------------------------------------------------------------------
/**
 * @brief ElementsDecodeRawTransactionTxOutStruct struct
 */
struct ElementsDecodeRawTransactionTxOutStruct {
  int64_t value = 0;                                 //!< value  // NOLINT
  int64_t value_minimum = 0;                         //!< value_minimum  // NOLINT
  int64_t value_maximum = 0;                         //!< value_maximum  // NOLINT
  int ct_exponent = 0;                               //!< ct_exponent  // NOLINT
  int ct_bits = 0;                                   //!< ct_bits  // NOLINT
  std::string surjectionproof = "";                  //!< surjectionproof  // NOLINT
  std::string valuecommitment = "";                  //!< valuecommitment  // NOLINT
  std::string asset = "";                            //!< asset  // NOLINT
  std::string assetcommitment = "";                  //!< assetcommitment  // NOLINT
  std::string commitmentnonce = "";                  //!< commitmentnonce  // NOLINT
  bool commitmentnonce_fully_valid = false;          //!< commitmentnonce_fully_valid  // NOLINT
  uint32_t n = 0;                                    //!< n  // NOLINT
  ElementsDecodeLockingScriptStruct script_pub_key;  //!< script_pub_key  // NOLINT
  std::string rangeproof = "";                       //!< rangeproof  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// PsbtGlobalXpubStruct
// ------------------------------------------------------------------------
/**
 * @brief PsbtGlobalXpubStruct struct
 */
struct PsbtGlobalXpubStruct {
  XpubDataStruct xpub;                  //!< xpub  // NOLINT
  std::string master_fingerprint = "";  //!< master_fingerprint  // NOLINT
  std::string path = "";                //!< path  // NOLINT
  std::string descriptor_xpub = "";     //!< descriptor_xpub  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};
namespace cfd {
namespace api {

// ------------------------------------------------------------------------
// DecodePsbtRequestStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodePsbtRequestStruct struct
 */
struct DecodePsbtRequestStruct {
  std::string psbt = "";            //!< psbt  // NOLINT
  std::string network = "mainnet";  //!< network  // NOLINT
  bool has_detail = false;          //!< has_detail  // NOLINT
  bool has_simple = false;          //!< has_simple  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodePsbtResponseStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodePsbtResponseStruct struct
 */
struct DecodePsbtResponseStruct {
  DecodeRawTransactionResponseStruct tx;        //!< tx  // NOLINT
  std::string tx_hex = "";                      //!< tx_hex  // NOLINT
  std::vector<PsbtGlobalXpubStruct> xpubs;      //!< xpubs  // NOLINT
  uint32_t version = 0;                         //!< version  // NOLINT
  std::vector<PsbtMapDataStruct> unknown;       //!< unknown  // NOLINT
  std::vector<DecodePsbtInputStruct> inputs;    //!< inputs  // NOLINT
  std::vector<DecodePsbtOutputStruct> outputs;  //!< outputs  // NOLINT
  int64_t fee = 0;                              //!< fee  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodeRawTransactionRequestStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeRawTransactionRequestStruct struct
 */
struct DecodeRawTransactionRequestStruct {
  std::string hex = "";             //!< hex  // NOLINT
  std::string network = "mainnet";  //!< network  // NOLINT
  bool iswitness = true;            //!< iswitness  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionRequestStruct
// ------------------------------------------------------------------------
/**
 * @brief ElementsDecodeRawTransactionRequestStruct struct
 */
struct ElementsDecodeRawTransactionRequestStruct {
  std::string hex = "";                //!< hex  // NOLINT
  std::string network = "liquidv1";    //!< network  // NOLINT
  std::string mainchain_network = "";  //!< mainchain_network  // NOLINT
  bool iswitness = true;               //!< iswitness  // NOLINT
  bool full_dump = false;              //!< full_dump  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// ElementsDecodeRawTransactionResponseStruct
// ------------------------------------------------------------------------
/**
 * @brief ElementsDecodeRawTransactionResponseStruct struct
 */
struct ElementsDecodeRawTransactionResponseStruct {
  std::string txid = "";                                      //!< txid  // NOLINT
  std::string hash = "";                                      //!< hash  // NOLINT
  std::string wtxid = "";                                     //!< wtxid  // NOLINT
  std::string withash = "";                                   //!< withash  // NOLINT
  uint32_t version = 0;                                       //!< version  // NOLINT
  int64_t size = 0;                                           //!< size  // NOLINT
  int64_t vsize = 0;                                          //!< vsize  // NOLINT
  int64_t weight = 0;                                         //!< weight  // NOLINT
  uint32_t locktime = 0;                                      //!< locktime  // NOLINT
  std::vector<ElementsDecodeRawTransactionTxInStruct> vin;    //!< vin  // NOLINT
  std::vector<ElementsDecodeRawTransactionTxOutStruct> vout;  //!< vout  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

}  // namespace api
}  // namespace cfd

// @formatter:on
// clang-format on

#endif  // CFD_SRC_JSONAPI_CFD_STRUCT_H_
