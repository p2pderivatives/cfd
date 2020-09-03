// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_elements_transaction.h
 *
 * @brief cfd-apiで利用するElements用Transaction操作のクラス定義
 *
 * JSON形式のAPIを提供する.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_ELEMENTS_TRANSACTION_H_
#define CFD_INCLUDE_CFD_CFDAPI_ELEMENTS_TRANSACTION_H_
#ifndef CFD_DISABLE_ELEMENTS

#include <map>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_fee.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfdapi_coin.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

/**
 * @brief cfdapi名前空間
 */
namespace cfd {
namespace api {

using cfd::FeeCalculator;
using cfd::core::AddressFormatData;
using cfd::core::Amount;
using cfd::core::BlindFactor;
using cfd::core::BlindParameter;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialTxIn;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOut;
using cfd::core::ConfidentialValue;
using cfd::core::HashType;
using cfd::core::IssuanceBlindingKeyPair;
using cfd::core::IssuanceParameter;
using cfd::core::NetType;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::Txid;

/**
 * @brief TxIn Blinding parameters
 */
struct TxInBlindParameters {
  Txid txid;                             //!< txid
  uint32_t vout = 0;                     //!< vout
  BlindParameter blind_param;            //!< blinding parameter
  bool is_issuance = false;              //!< issuance flag
  IssuanceBlindingKeyPair issuance_key;  //!< issuance blinding keys
};

/**
 * @brief TxOut Blinding keys
 */
struct TxOutBlindKeys {
  uint32_t index = 0;       //!< txout index
  Pubkey confidential_key;  //!< confidential key.
  /**
   * @brief confidential key.
   * @deprecated
   */
  Pubkey blinding_key;
  std::string confidential_address;  //!< confidential address
};

/**
 * @brief TxIn pegin parameters
 */
struct TxInPeginParameters {
  Txid txid;                      //!< txid
  uint32_t vout = 0;              //!< vout
  Amount amount;                  //!< amount
  ConfidentialAssetId asset;      //!< asset
  BlockHash mainchain_blockhash;  //!< mainchain genesis block hash
  Script claim_script;            //!< claim script
  /**
   * @brief mainchain raw transaction.
   * @see ConfidentialTransaction::GetBitcoinTransaction
   */
  ByteData mainchain_raw_tx;
  ByteData mainchain_txoutproof;  //!< mainchain txoutproof
};

/**
 * @brief TxOut pegout parameters
 */
struct TxOutPegoutParameters {
  Amount amount;                         //!< amount
  ConfidentialAssetId asset;             //!< asset
  BlockHash genesisblock_hash;           //!< mainchain genesis block hash
  Address btc_address;                   //!< mainchain bitcoin address
  NetType net_type = NetType::kMainnet;  //!< mainchain network type
  Pubkey online_pubkey;                  //!< online pubkey
  Privkey master_online_key;             //!< master online key
  std::string bitcoin_descriptor;        //!< bitcoin descriptor
  uint32_t bip32_counter = 0;            //!< bip32 counter
  ByteData whitelist;                    //!< claim script
  NetType elements_net_type = NetType::kLiquidV1;  //!< elements network type
};

/**
 * @brief TxOut Unblinding keys
 */
struct TxOutUnblindKeys {
  uint32_t index;        //!< txout index
  Privkey blinding_key;  //!< blinding key
};

/**
 * @brief Issuance Blinding keys
 */
struct IssuanceBlindKeys {
  Txid txid;                             //!< txid
  uint32_t vout;                         //!< vout
  IssuanceBlindingKeyPair issuance_key;  //!< issuance blinding keys
};

/**
 * @brief Unblind output
 */
struct UnblindOutputs {
  uint32_t index;              //!< txout index
  BlindParameter blind_param;  //!< blinding parameter
};

/**
 * @brief Issuance Unblind output
 */
struct UnblindIssuanceOutputs {
  Txid txid;                       //!< txid
  uint32_t vout;                   //!< vout
  ConfidentialAssetId asset;       //!< asset id
  ConfidentialValue asset_amount;  //!< asset amount
  ConfidentialAssetId token;       //!< token asset id
  ConfidentialValue token_amount;  //!< token amount
};

/**
 * @brief Issuance input
 */
struct TxInIssuanceParameters {
  Txid txid;                      //!< txid
  uint32_t vout;                  //!< vout
  Amount asset_amount;            //!< asset amount
  ConfidentialTxOut asset_txout;  //!< asset output
  Amount token_amount;            //!< token amount
  ConfidentialTxOut token_txout;  //!< token output
  ByteData256 contract_hash;      //!< contract hash
  bool is_blind;                  //!< blind flag
};

/**
 * @brief Reissuance input
 */
struct TxInReissuanceParameters {
  Txid txid;                      //!< txid
  uint32_t vout;                  //!< vout
  Amount amount;                  //!< amount
  ConfidentialTxOut asset_txout;  //!< asset output
  BlindFactor blind_factor;       //!< blind factor
  BlindFactor entropy;            //!< entropy
};

/**
 * @brief Issueance / Reissuance output
 */
struct IssuanceOutput {
  Txid txid;                 //!< txid
  uint32_t vout;             //!< vout
  IssuanceParameter output;  //!< issuance output
};

/**
 * @brief Using utxo and option data in elements
 */
struct ElementsUtxoAndOption {
  UtxoData utxo;                   //!< utxo
  bool is_issuance = false;        //!< use issuance/reissuance
  bool is_blind_issuance = false;  //!< use blind issuance/reissuance
  bool is_pegin = false;           //!< use pegin
  uint32_t pegin_btc_tx_size = 0;  //!< btc pegin tx size
  Script fedpeg_script;            //!< fedpeg script for pegin
};

/**
 * @brief Elements用Transaction関連の関数群クラス
 */
class CFD_EXPORT ElementsTransactionApi {
 public:
  /**
   * @brief constructor.
   */
  ElementsTransactionApi() {}

  /**
   * @brief Elements用のRaw Transactionを作成する.
   * @param[in] version     tx version
   * @param[in] locktime    lock time
   * @param[in] txins       tx input list
   * @param[in] txouts      tx output list
   * @param[in] txout_fee   tx output fee
   * @return Transaction
   */
  ConfidentialTransactionController CreateRawTransaction(
      uint32_t version, uint32_t locktime,
      const std::vector<ConfidentialTxIn>& txins,
      const std::vector<ConfidentialTxOut>& txouts,
      const ConfidentialTxOut& txout_fee) const;

  /**
   * @brief Create elements raw transaction.
   * @param[in] version     tx version
   * @param[in] locktime    lock time
   * @param[in] txins       tx input list
   * @param[in] pegins      pegin tx input list
   * @param[in] txouts      tx output list
   * @param[in] pegouts     pegout tx output list
   * @param[in] txout_fee   tx output fee
   * @param[out] pegout_addresses   pegout address list
   * @return Transaction
   */
  ConfidentialTransactionController CreateRawTransaction(
      uint32_t version, uint32_t locktime,
      const std::vector<ConfidentialTxIn>& txins,
      const std::vector<TxInPeginParameters>& pegins,
      const std::vector<ConfidentialTxOut>& txouts,
      const std::vector<TxOutPegoutParameters>& pegouts,
      const ConfidentialTxOut& txout_fee,
      std::map<std::string, Amount>* pegout_addresses = nullptr) const;

  /**
   * @brief Add elements raw transaction.
   * @param[in] tx_hex      tx hex string
   * @param[in] txins       tx input list
   * @param[in] pegins      pegin tx input list
   * @param[in] txouts      tx output list
   * @param[in] pegouts     pegout tx output list
   * @param[in] txout_fee   tx output fee
   * @param[out] pegout_addresses   pegout address list
   * @return Transaction
   */
  ConfidentialTransactionController AddRawTransaction(
      const std::string& tx_hex, const std::vector<ConfidentialTxIn>& txins,
      const std::vector<TxInPeginParameters>& pegins,
      const std::vector<ConfidentialTxOut>& txouts,
      const std::vector<TxOutPegoutParameters>& pegouts,
      const ConfidentialTxOut& txout_fee,
      std::map<std::string, Amount>* pegout_addresses = nullptr) const;

  /**
   * @brief WitnessStack数を出力する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @return WitnessStack数
   */
  uint32_t GetWitnessStackNum(
      const std::string& tx_hex, const Txid& txid, uint32_t vout) const;

  /**
   * @brief hexで与えられたtxに、SignDataを付与した
   *     ConfidentialTransctionControllerを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @param[in] sign_params     sign data list
   * @param[in] is_witness      use witness
   * @param[in] clear_stack     clear stack data before add.
   * @return SignDataが付与されたTransactionController
   */
  ConfidentialTransactionController AddSign(
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const std::vector<SignParameter>& sign_params, bool is_witness = true,
      bool clear_stack = false) const;

  /**
   * @brief WitnessStackの情報を更新する.
   * @param[in] tx_hex              tx hex string
   * @param[in] txid                target tx input txid
   * @param[in] vout                target tx input vout
   * @param[in] update_sign_param   sign data
   * @param[in] stack_index         witness stack index
   * @return TransactionController
   */
  ConfidentialTransactionController UpdateWitnessStack(
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const SignParameter& update_sign_param, uint32_t stack_index) const;

  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] pubkey          public key
   * @param[in] value           value (amount or commitment)
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const ConfidentialTxInReference& txin,
      const Pubkey& pubkey, const ConfidentialValue& value, HashType hash_type,
      const SigHashType& sighash_type) const;
  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] redeem_script   redeem script
   * @param[in] value           value (amount or commitment)
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const ConfidentialTxInReference& txin,
      const Script& redeem_script, const ConfidentialValue& value,
      HashType hash_type, const SigHashType& sighash_type) const;
  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] key_data        key data (pubkey or redeem script)
   * @param[in] value           value (amount or commitment)
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const ConfidentialTxInReference& txin,
      const ByteData& key_data, const ConfidentialValue& value,
      HashType hash_type, const SigHashType& sighash_type) const;
  /**
   * @brief tx情報およびパラメータから、SigHashを作成する.
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @param[in] key_data        key data (pubkey or redeem script)
   * @param[in] value           value (amount or commitment)
   * @param[in] hash_type       hash type
   * @param[in] sighash_type    sighash type
   * @return sighash
   */
  std::string CreateSignatureHash(
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const ByteData& key_data, const ConfidentialValue& value,
      HashType hash_type, const SigHashType& sighash_type) const;

  /**
   * @brief Multisig署名情報を追加する.
   * @details 追加するsignatureの順序は、redeem
   * scriptのpubkeyとsign_list内のrelatedPubkeyで
   * 対応をとって自動的に整列される.
   * (relatedPubkeyが設定されていない場合は、relatedPubkeyが
   *  設定されているsignatureを追加した後にsignParamの順序でsignatureを追加)
   * @param[in] tx_hex          tx hex string
   * @param[in] txin            target tx input
   * @param[in] sign_list       sign data list
   * @param[in] address_type    address type. (support is P2sh-P2wsh or P2wsh)
   * @param[in] witness_script  witness script
   * @param[in] redeem_script   redeem script
   * @param[in] clear_stack     clear stack data before add.
   * @return Transaction
   */
  ConfidentialTransactionController AddMultisigSign(
      const std::string& tx_hex, const ConfidentialTxInReference& txin,
      const std::vector<SignParameter>& sign_list, AddressType address_type,
      const Script& witness_script, const Script redeem_script = Script(),
      bool clear_stack = true);

  /**
   * @brief Multisig署名情報を追加する.
   * @details 追加するsignatureの順序は、redeem
   * scriptのpubkeyとsign_list内のrelatedPubkeyで
   *   対応をとって自動的に整列される.
   * (relatedPubkeyが設定されていない場合は、relatedPubkeyが
   *   設定されているsignatureを追加した後にsignParamの順序でsignatureを追加)
   * @param[in] tx_hex          tx hex string
   * @param[in] txid            target tx input txid
   * @param[in] vout            target tx input vout
   * @param[in] sign_list       sign data list
   * @param[in] address_type    address type. (support is P2sh-P2wsh or P2wsh)
   * @param[in] witness_script  witness script
   * @param[in] redeem_script   redeem script
   * @param[in] clear_stack     clear stack data before add.
   * @return Transaction
   */
  ConfidentialTransactionController AddMultisigSign(
      const std::string& tx_hex, const Txid& txid, uint32_t vout,
      const std::vector<SignParameter>& sign_list, AddressType address_type,
      const Script& witness_script, const Script redeem_script = Script(),
      bool clear_stack = true);

  /**
   * @brief Elements用RawTransactionをBlindする.
   * @param[in] tx_hex                 transaction hex string
   * @param[in] txin_blind_keys        txin blinding data
   * @param[in] txout_blind_keys       txout blinding data
   * @param[in] is_issuance_blinding   issuance有無
   * @return Transaction
   */
  ConfidentialTransactionController BlindTransaction(
      const std::string& tx_hex,
      const std::vector<TxInBlindParameters>& txin_blind_keys,
      const std::vector<TxOutBlindKeys>& txout_blind_keys,
      bool is_issuance_blinding = false);

  /**
   * @brief Elements用RawTransactionをUnblindする.
   * @param[in]  tx_hex                 transaction hex string
   * @param[in]  txout_unblind_keys     txout blinding data
   * @param[in]  issuance_blind_keys    issuance blinding data
   * @param[out] blind_outputs          blind parameter
   * @param[out] issuance_outputs       issuance parameter
   * @return Transaction
   */
  ConfidentialTransactionController UnblindTransaction(
      const std::string& tx_hex,
      const std::vector<TxOutUnblindKeys>& txout_unblind_keys,
      const std::vector<IssuanceBlindKeys>& issuance_blind_keys,
      std::vector<UnblindOutputs>* blind_outputs,
      std::vector<UnblindIssuanceOutputs>* issuance_outputs);

  /**
   * @brief Elements用RawTransactionにIssuance情報を設定する.
   * @param[in]  tx_hex                 transaction hex string
   * @param[in]  issuances              issuance parameter
   * @param[out] issuance_output        issuance output data
   * @return
   */
  ConfidentialTransactionController SetRawIssueAsset(
      const std::string& tx_hex,
      const std::vector<TxInIssuanceParameters>& issuances,
      std::vector<IssuanceOutput>* issuance_output);

  /**
   * @brief Elements用RawTransactionにReissuance情報を設定する.
   * @param[in]  tx_hex                 transaction hex string
   * @param[in]  issuances              issuance parameter
   * @param[out] issuance_output        issuance output data
   * @return
   */
  ConfidentialTransactionController SetRawReissueAsset(
      const std::string& tx_hex,
      const std::vector<TxInReissuanceParameters>& issuances,
      std::vector<IssuanceOutput>* issuance_output);

  /**
   * @brief Elements用のRaw Pegin Transactionを作成する.
   * @param[in] version     tx version
   * @param[in] locktime    lock time
   * @param[in] txins       tx input list
   * @param[in] pegins      tx pegin input list
   * @param[in] txouts      tx output list
   * @param[in] txout_fee   tx output fee
   * @return Pegin Transaction
   */
  ConfidentialTransactionController CreateRawPeginTransaction(
      uint32_t version, uint32_t locktime,
      const std::vector<ConfidentialTxIn>& txins,
      const std::vector<TxInPeginParameters>& pegins,
      const std::vector<ConfidentialTxOut>& txouts,
      const ConfidentialTxOut& txout_fee) const;

  /**
   * @brief Elements用のRaw Pegout Transactionを作成する.
   * @param[in] version     tx version
   * @param[in] locktime    lock time
   * @param[in] txins       tx input list
   * @param[in] txouts      tx output list
   * @param[in] pegout_data tx pegout data
   * @param[in] txout_fee   tx output fee
   * @param[out] pegout_address   pegout address
   * @return Pegout Transaction
   */
  ConfidentialTransactionController CreateRawPegoutTransaction(
      uint32_t version, uint32_t locktime,
      const std::vector<ConfidentialTxIn>& txins,
      const std::vector<ConfidentialTxOut>& txouts,
      const TxOutPegoutParameters& pegout_data,
      const ConfidentialTxOut& txout_fee,
      Address* pegout_address = nullptr) const;

  /**
   * @brief Issue用BlindingKeyを作成する.
   * @param[in] master_blinding_key master blindingKey
   * @param[in] txid                issuance utxo txid
   * @param[in] vout                issuance utxo vout
   * @return blinding key
   */
  Privkey GetIssuanceBlindingKey(
      const Privkey& master_blinding_key, const Txid& txid, int32_t vout);

  /**
   * @brief estimate a fee amount from transaction.
   * @param[in] tx_hex              tx hex string
   * @param[in] utxos               using utxo data
   * @param[in] fee_asset           using fee asset
   * @param[in] txout_fee           tx fee amount (ignore utxo)
   * @param[in] utxo_fee            utxo fee amount
   * @param[in] is_blind            using tx blinding
   * @param[in] effective_fee_rate  effective fee rate (minimum)
   * @param[in] exponent                  rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits              rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @return tx fee (contains utxo)
   */
  Amount EstimateFee(
      const std::string& tx_hex,
      const std::vector<ElementsUtxoAndOption>& utxos,
      const ConfidentialAssetId& fee_asset, Amount* txout_fee = nullptr,
      Amount* utxo_fee = nullptr, bool is_blind = true,
      double effective_fee_rate = 1, int exponent = 0,
      int minimum_bits = cfd::core::kDefaultBlindMinimumBits) const;

  /**
   * @brief estimate a fee amount from transaction.
   * @param[in] tx_hex              tx hex string
   * @param[in] utxos               using utxo data
   * @param[in] fee_asset           using fee asset
   * @param[in] txout_fee           tx fee amount (ignore utxo)
   * @param[in] utxo_fee            utxo fee amount
   * @param[in] is_blind            using tx blinding
   * @param[in] effective_fee_rate  effective fee rate (minimum)
   * @param[in] exponent                  rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits              rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @return tx fee (contains utxo)
   */
  Amount EstimateFee(
      const std::string& tx_hex,
      const std::vector<ElementsUtxoAndOption>& utxos,
      const ConfidentialAssetId& fee_asset, Amount* txout_fee = nullptr,
      Amount* utxo_fee = nullptr, bool is_blind = true,
      uint64_t effective_fee_rate = 1000, int exponent = 0,
      int minimum_bits = cfd::core::kDefaultBlindMinimumBits) const;

  /**
   * @brief calculate fund transaction.
   * @param[in] tx_hex                   tx hex string
   * @param[in] utxos                    using utxo data
   * @param[in] map_target_value         asset target value map.
   *    Amount more than the specified amount is set in txout.
   *    default is 0 (disable).
   * @param[in] selected_txin_utxos      selected txin utxo
   * @param[in] reserve_txout_address    address for adding txout.
   *    Also serves as a change address.
   * @param[in] fee_asset                using fee asset
   * @param[in] is_blind_estimate_fee    using tx blinding
   * @param[in] effective_fee_rate       effective fee rate (minimum)
   * @param[out] estimate_fee            estimate fee
   * @param[in] filter                   utxo search filter
   * @param[in] option_params            utxo search option
   * @param[out] append_txout_addresses  used txout additional address
   * @param[in] net_type                 network type
   * @param[in] prefix_list              address prefix list
   * @return tx controller
   */
  ConfidentialTransactionController FundRawTransaction(
      const std::string& tx_hex, const std::vector<UtxoData>& utxos,
      const std::map<std::string, Amount>& map_target_value,
      const std::vector<ElementsUtxoAndOption>& selected_txin_utxos,
      const std::map<std::string, std::string>& reserve_txout_address,
      const ConfidentialAssetId& fee_asset, bool is_blind_estimate_fee = true,
      double effective_fee_rate = 1, Amount* estimate_fee = nullptr,
      const UtxoFilter* filter = nullptr,
      const CoinSelectionOption* option_params = nullptr,
      std::vector<std::string>* append_txout_addresses = nullptr,
      NetType net_type = NetType::kLiquidV1,
      const std::vector<AddressFormatData>* prefix_list = nullptr) const;

  // CreateDestroyAmountTransaction
  // see CreateRawTransaction and ConfidentialTxOut::CreateDestroyAmountTxOut
};

}  // namespace api
}  // namespace cfd
#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFD_CFDAPI_ELEMENTS_TRANSACTION_H_
