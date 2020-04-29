// Copyright 2019 CryptoGarage
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
namespace cfd {
namespace api {

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
// DecodeRawTransactionTxInStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeRawTransactionTxInStruct struct
 */
struct DecodeRawTransactionTxInStruct {
  std::string coinbase = "";               //!< coinbase  // NOLINT
  std::string txid = "";                   //!< txid  // NOLINT
  int64_t vout = 0;                        //!< vout  // NOLINT
  DecodeUnlockingScriptStruct script_sig;  //!< script_sig  // NOLINT
  std::vector<std::string> txinwitness;    //!< txinwitness  // NOLINT
  int64_t sequence = 0;                    //!< sequence  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodeLockingScriptStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeLockingScriptStruct struct
 */
struct DecodeLockingScriptStruct {
  std::string asm_ = "";               //!< asm_  // NOLINT
  std::string hex = "";                //!< hex  // NOLINT
  int64_t req_sigs = 0;                //!< req_sigs  // NOLINT
  std::string type = "";               //!< type  // NOLINT
  std::vector<std::string> addresses;  //!< addresses  // NOLINT
  std::set<std::string> ignore_items;   //!< using on JSON mapping convert.
};

// ------------------------------------------------------------------------
// DecodeRawTransactionTxOutStruct
// ------------------------------------------------------------------------
/**
 * @brief DecodeRawTransactionTxOutStruct struct
 */
struct DecodeRawTransactionTxOutStruct {
  double value = 0;                          //!< value  // NOLINT
  int64_t n = 0;                             //!< n  // NOLINT
  DecodeLockingScriptStruct script_pub_key;  //!< script_pub_key  // NOLINT
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
  int64_t size = 0;                                   //!< size  // NOLINT
  int64_t vsize = 0;                                  //!< vsize  // NOLINT
  int64_t weight = 0;                                 //!< weight  // NOLINT
  uint32_t locktime = 0;                              //!< locktime  // NOLINT
  std::vector<DecodeRawTransactionTxInStruct> vin;    //!< vin  // NOLINT
  std::vector<DecodeRawTransactionTxOutStruct> vout;  //!< vout  // NOLINT
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
