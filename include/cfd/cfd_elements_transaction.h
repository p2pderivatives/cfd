// Copyright 2019 CryptoGarage
/**
 * @file cfd_elements_transaction.h
 *
 * @brief Related class definitions for Elements Transaction operations
 */
#ifndef CFD_INCLUDE_CFD_CFD_ELEMENTS_TRANSACTION_H_
#define CFD_INCLUDE_CFD_CFD_ELEMENTS_TRANSACTION_H_
#ifndef CFD_DISABLE_ELEMENTS

#include <map>
#include <set>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_util.h"

/**
 * @brief cfd namespace
 */
namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::BlindData;
using cfd::core::BlindFactor;
using cfd::core::BlindParameter;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialTransaction;
using cfd::core::ConfidentialTxInReference;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::IssuanceBlindingKeyPair;
using cfd::core::IssuanceParameter;
using cfd::core::kDefaultBlindMinimumBits;
using cfd::core::NetType;
using cfd::core::OutPoint;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::UnblindParameter;
using cfd::core::WitnessVersion;

/**
 * @brief Issuance tx output parameter.
 */
struct IssuanceOutputParameter {
  Address address;  //!< unconfidential address
  ElementsConfidentialAddress confidential_address;  //!< confidential address
  Script direct_locking_script;  //!< direct locking script (unuse address)
  Amount amount;                 //!< txout amount
  bool is_remove_nonce = false;  //!< remove nonce
};

/**
 * @brief blind parameter's confidential key.
 */
struct ConfidentialKeyBlindParameter {
  uint32_t index;           //!< txout index
  Pubkey confidential_key;  //!< confidential key
};

/**
 * @brief Context class for generating bitcoin transaction.
 */
class CFD_EXPORT ConfidentialTransactionContext
    : public ConfidentialTransaction {
 public:
  /**
   * @brief constructor.
   */
  ConfidentialTransactionContext();
  /**
   * @brief constructor.
   * @param[in] version   Transaction version
   * @param[in] locktime  Timestamp or block height
   */
  explicit ConfidentialTransactionContext(uint32_t version, uint32_t locktime);
  /**
   * @brief constructor.
   * @param[in] tx_hex  Raw Transaction HEX string
   */
  explicit ConfidentialTransactionContext(const std::string& tx_hex);
  /**
   * @brief constructor.
   * @param[in] byte_data  Raw Transaction Data
   */
  explicit ConfidentialTransactionContext(const ByteData& byte_data);
  /**
   * @brief constructor
   * @param[in] context   Transaction Context
   */
  explicit ConfidentialTransactionContext(
      const ConfidentialTransactionContext& context);
  /**
   * @brief constructor
   * @param[in] transaction   Transaction
   */
  explicit ConfidentialTransactionContext(
      const ConfidentialTransaction& transaction);
  /**
   * @brief destructor.
   */
  virtual ~ConfidentialTransactionContext() {
    // do nothing
  }
  /**
   * @brief copy constructor.
   * @param[in] context   Transaction context
   * @return Transaction context
   */
  ConfidentialTransactionContext& operator=(
      const ConfidentialTransactionContext& context) &;

  /**
   * @brief ConfidentialTransaction's GetTxInIndex.
   */
  using ConfidentialTransaction::GetTxInIndex;
  /**
   * @brief Get the index of TxIn.
   * @param[in] outpoint  TxIn txid and vout
   * @return TxIn index
   */
  virtual uint32_t GetTxInIndex(const OutPoint& outpoint) const;
  /**
   * @brief ConfidentialTransaction's GetTxOutIndex.
   */
  using ConfidentialTransaction::GetTxOutIndex;
  /**
   * @brief Get the index of TxOut.
   * @param[in] address  address
   * @return TxOut index
   */
  virtual uint32_t GetTxOutIndex(const Address& address) const;
  /**
   * @brief Check if TxIn exists.
   * @param[in] outpoint  TxIn txid and vout
   * @param[out] index  txout index.
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindTxIn(const OutPoint& outpoint, uint32_t* index = nullptr) const;
  /**
   * @brief Check if TxOut exists.
   * @param[in] locking_script  locking script
   * @param[out] index  txout index.
   * @param[out] indexes  txout index list.
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindTxOut(
      const Script& locking_script, uint32_t* index = nullptr,
      std::vector<uint32_t>* indexes = nullptr) const;
  /**
   * @brief Check if TxOut exists.
   * @param[in] address  address
   * @param[out] index  txout index.
   * @param[out] indexes  txout index list.
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindTxOut(
      const Address& address, uint32_t* index = nullptr,
      std::vector<uint32_t>* indexes = nullptr) const;
  /**
   * @brief ConfidentialTransaction's GetTxIn.
   */
  using ConfidentialTransaction::GetTxIn;
  /**
   * @brief Get txin by outpoint.
   * @param[in] outpoint  target outpoint.
   * @return ConfidentialTxInReference
   */
  const ConfidentialTxInReference GetTxIn(const OutPoint& outpoint) const;
  /**
   * @brief Get txout address by index.
   * @param[in] index         txout index.
   * @param[in] net_type      network type.
   * @param[in] ignore_error  ignore error flag.
   * @return address
   */
  Address GetTxOutAddress(
      uint32_t index, NetType net_type = NetType::kLiquidV1,
      bool ignore_error = false) const;

  /**
   * @brief Get if it is blind.
   * @retval true  blind
   * @retval false unblind
   */
  bool HasBlinding() const;

  /**
   * @brief ConfidentialTransaction's AddTxIn.
   */
  using ConfidentialTransaction::AddTxIn;
  /**
   * @brief Add TxIn.
   * @param[in] outpoint  TxIn txid and vout
   * @return Index position of added TxIn
   */
  virtual uint32_t AddTxIn(const OutPoint& outpoint);
  /**
   * @brief ConfidentialTransaction's AddTxOut.
   */
  using ConfidentialTransaction::AddTxOut;
  /**
   * @brief Add TxOut.
   * @param[in] address  Remittance destination unblinded address
   * @param[in] value  Amount
   * @param[in] asset AssetID
   * @return TxOut Index
   */
  virtual uint32_t AddTxOut(
      const Address& address, const Amount& value,
      const ConfidentialAssetId& asset);
  /**
   * @brief Add TxOut.
   * @param[in] address  Remittance destination confidential address
   * @param[in] value  Amount
   * @param[in] asset AssetID
   * @param[in] remove_nonce  Nonce forced delete flag
   * @return TxOut Index
   */
  virtual uint32_t AddTxOut(
      const ElementsConfidentialAddress& address,
      const Amount& value,  // amount
      const ConfidentialAssetId& asset, bool remove_nonce = false);

  /**
   * @brief Get the size excluding TxIn.
   * @param[in] is_blinded              Flag to get the expected size when blind
   * @param[out] witness_area_size      witness area size
   * @param[out] no_witness_area_size   no witness area size
   * @param[in] exponent                rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits            rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @param[in] asset_count             input asset count.
   * @return Tx size excluding TxIn (Serialize)
   */
  uint32_t GetSizeIgnoreTxIn(
      bool is_blinded = false, uint32_t* witness_area_size = nullptr,
      uint32_t* no_witness_area_size = nullptr, int exponent = 0,
      int minimum_bits = cfd::core::kDefaultBlindMinimumBits,
      uint32_t asset_count = 0) const;

  /**
   * @brief Get the virtual size excluding TxIn.
   * @param[in] is_blinded      Flag to get the expected size when blind
   * @param[in] exponent        rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits    rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @param[in] asset_count     input asset count.
   * @return Tx virtual size excluding TxIn (Serialize)
   */
  uint32_t GetVsizeIgnoreTxIn(
      bool is_blinded = false, int exponent = 0,
      int minimum_bits = cfd::core::kDefaultBlindMinimumBits,
      uint32_t asset_count = 0) const;

  /**
   * @brief Get the txout index of fee.
   * @param[out] index  txout index.
   * @retval true   fee exist
   * @retval false  fee not found
   */
  bool IsFindFeeTxOut(uint32_t* index = nullptr) const;

  /**
   * @brief Get the Fee amount for TxOut.
   * @param[out] asset    fee asset.
   * @return Fee amount
   */
  Amount GetFeeAmount(ConfidentialAssetId* asset = nullptr) const;

  /**
   * @brief Update Fee information of TxOut. If Fee does not exist, it will be added automatically.
   * @param[in] value Fee amount
   * @param[in] asset AssetID
   * @return Updated TxOut index
   */
  uint32_t UpdateFeeAmount(
      const Amount& value, const ConfidentialAssetId& asset);

  /**
   * @brief Split output amount.
   * @param[in] index           txout index
   * @param[in] amount_list     amount list
   * @param[in] address_list    address list
   */
  void SplitTxOut(
      uint32_t index, const std::vector<Amount>& amount_list,
      const std::vector<Address>& address_list);
  /**
   * @brief Split output amount.
   * @param[in] index           txout index
   * @param[in] amount_list     amount list
   * @param[in] address_list    address list
   */
  void SplitTxOut(
      uint32_t index, const std::vector<Amount>& amount_list,
      const std::vector<ElementsConfidentialAddress>& address_list);
  /**
   * @brief Split output amount.
   * @param[in] index                   txout index
   * @param[in] amount_list             amount list
   * @param[in] locking_script_list     locking script list
   * @param[in] nonce_list              nonce list
   */
  void SplitTxOut(
      uint32_t index, const std::vector<Amount>& amount_list,
      const std::vector<Script>& locking_script_list,
      const std::vector<ConfidentialNonce>& nonce_list);

  /**
   * @brief Perform a simple Fee calculation.
   * @param[in] append_feature_signed_size  size addition flag for sign
   * @param[in] append_signed_witness       Whether to treat the addition of sign as witness
   * @return Fee Amount
   */
  Amount CalculateSimpleFee(
      bool append_feature_signed_size = true,
      bool append_signed_witness = true) const;

  /**
   * @brief Add txout for pegged-out.
   * @param[in] value Pegout value
   * @param[in] asset AssetID
   * @param[in] genesisblock_hash mainchainのgenesisblock hash
   * @param[in] btc_address mainchain address
   * @param[in] bitcoin_net_type mainchain network type
   * @param[in] online_pubkey online pubkey for whitelist proof
   * @param[in] master_online_key  online privkey for whitelist proof
   * @param[in] btc_descriptor descriptor by initpegoutwallet
   * @param[in] bip32_counter descriptor counter by initpegoutwallet
   * @param[in] whitelist whitelist
   * @param[in] elements_net_type elements network type
   * @param[out] btc_derive_address btc address by derived btc descriptor
   * @return additional TxOut index number.
   */
  uint32_t AddPegoutTxOut(
      const Amount& value, const ConfidentialAssetId& asset,
      const BlockHash& genesisblock_hash, const Address& btc_address,
      NetType bitcoin_net_type = NetType::kMainnet,
      const Pubkey& online_pubkey = Pubkey(),
      const Privkey& master_online_key = Privkey(),
      const std::string& btc_descriptor = "", uint32_t bip32_counter = 0,
      const ByteData& whitelist = ByteData(),
      NetType elements_net_type = NetType::kLiquidV1,
      Address* btc_derive_address = nullptr);

  /**
   * @brief Add byte data obtained from the string to PeginWitness.
   * @param[in] outpoint    TxIn txid and vout
   * @param[in] amount      amount of pegin input
   * @param[in] asset_id    Remittance destination asset ID
   * @param[in] mainchain_genesis_block_hash hash of genesis block in mainchain
   * @param[in] claim_script pegin claim script
   * @param[in] mainchain_pegin_transaction pegin mainchain transaction
   * @param[in] tx_out_proof Proof value when pegin transaction is captured
   */
  void AddPeginTxIn(  // AddPeginWitness
      const OutPoint& outpoint, const Amount& amount,
      const ConfidentialAssetId& asset_id,
      const BlockHash& mainchain_genesis_block_hash,
      const Script& claim_script, const ByteData& mainchain_pegin_transaction,
      const ByteData& tx_out_proof);

  /**
   * @brief ConfidentialTransaction's SetAssetIssuance.
   */
  using ConfidentialTransaction::SetAssetIssuance;
  /**
   * @brief set issuance asset and token.
   * @param[in] outpoint             TxIn txid and vout
   * @param[in] issue_amount         issuance amount
   * @param[in] issue_output         issue amount output data.
   * @param[in] token_amount         token amount
   * @param[in] token_output         token amount output data.
   * @param[in] is_blind             blinding issuance
   * @param[in] contract_hash        asset entropy
   * @return issuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetIssuance(
      const OutPoint& outpoint, const Amount& issue_amount,
      const IssuanceOutputParameter& issue_output, const Amount& token_amount,
      const IssuanceOutputParameter& token_output, bool is_blind,
      const ByteData256& contract_hash);
  /**
   * @brief set issuance asset and token.
   * @param[in] outpoint             TxIn txid and vout
   * @param[in] issue_amount         issuance amount
   * @param[in] issue_output_list    issue amount output list.
   * @param[in] token_amount         token amount
   * @param[in] token_output_list    token amount output list.
   * @param[in] is_blind             blinding issuance
   * @param[in] contract_hash        asset entropy
   * @return issuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetIssuance(
      const OutPoint& outpoint, const Amount& issue_amount,
      const std::vector<IssuanceOutputParameter>& issue_output_list,
      const Amount& token_amount,
      const std::vector<IssuanceOutputParameter>& token_output_list,
      bool is_blind, const ByteData256& contract_hash);

  /**
   * @brief ConfidentialTransaction's SetAssetReissuance.
   */
  using ConfidentialTransaction::SetAssetReissuance;
  /**
   * @brief set reissue asset.
   * @param[in] outpoint           token txid and vout
   * @param[in] amount             reissuance amount
   * @param[in] issue_output       issue amount output data.
   * @param[in] blind_factor       token asset blinder
   * @param[in] entropy            asset entropy
   * @return reissuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetReissuance(
      const OutPoint& outpoint, const Amount& amount,
      const IssuanceOutputParameter& issue_output,
      const BlindFactor& blind_factor, const BlindFactor& entropy);
  /**
   * @brief set reissue asset.
   * @param[in] outpoint           token txid and vout
   * @param[in] amount             reissuance amount
   * @param[in] issue_output_list  issue amount output list.
   * @param[in] blind_factor       token asset blinder
   * @param[in] entropy            asset entropy
   * @return reissuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetReissuance(
      const OutPoint& outpoint, const Amount& amount,
      const std::vector<IssuanceOutputParameter>& issue_output_list,
      const BlindFactor& blind_factor, const BlindFactor& entropy);

  /**
   * @brief ConfidentialTransaction's BlindTransaction.
   */
  using ConfidentialTransaction::BlindTransaction;
  /**
   * @brief Blind Trasnsaction.
   * @param[in] utxo_info_map              txin utxo information map.
   * @param[in] issuance_key_map           issue blinding key map.
   * @param[in] confidential_address_list  txout confidential address list.
   * @param[in] minimum_range_value        rangeproof minimum value.
   *   0 to max(int64_t)
   * @param[in] exponent                   rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits               rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @param[out] blinder_list             blinder list. (default is null)
   */
  void BlindTransaction(
      const std::map<OutPoint, BlindParameter>& utxo_info_map,
      const std::map<OutPoint, IssuanceBlindingKeyPair>& issuance_key_map,
      const std::vector<ElementsConfidentialAddress>&
          confidential_address_list,
      int64_t minimum_range_value = 1, int exponent = 0,
      int minimum_bits = kDefaultBlindMinimumBits,
      std::vector<BlindData>* blinder_list = nullptr);
  /**
   * @brief Blind Trasnsaction.
   * @param[in] utxo_info_map              txin utxo information map.
   * @param[in] issuance_key_map           issue blinding key map.
   * @param[in] confidential_address_list  txout confidential address list.
   * @param[in] confidential_key_list      txout confidential key list.
   * @param[in] minimum_range_value        rangeproof minimum value.
   *   0 to max(int64_t)
   * @param[in] exponent                   rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits               rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @param[out] blinder_list             blinder list. (default is null)
   */
  void BlindTransactionWithDirectKey(
      const std::map<OutPoint, BlindParameter>& utxo_info_map,
      const std::map<OutPoint, IssuanceBlindingKeyPair>& issuance_key_map,
      const std::vector<ElementsConfidentialAddress>&
          confidential_address_list,
      const std::vector<ConfidentialKeyBlindParameter>& confidential_key_list,
      int64_t minimum_range_value = 1, int exponent = 0,
      int minimum_bits = kDefaultBlindMinimumBits,
      std::vector<BlindData>* blinder_list = nullptr);

  /**
   * @brief Unblind issuance data.
   * @param[in] outpoint TxIn txid and vout
   * @param[in] asset_blinding_key asset & token blinding privkey
   * @return list of structs contain blind factors and amount.
   */
  std::vector<UnblindParameter> UnblindIssuance(
      const OutPoint& outpoint,
      const IssuanceBlindingKeyPair& asset_blinding_key);

  // state-sequence-api
  /**
   * @brief add txin with utxo.
   * @param[in] utxo  UTXO
   */
  void AddInput(const UtxoData& utxo);
  /**
   * @brief add txin with utxo.
   * @param[in] utxo      UTXO
   * @param[in] sequence  unlock sequence value.
   */
  void AddInput(const UtxoData& utxo, uint32_t sequence);
  /**
   * @brief add txins with utxo.
   * @param[in] utxos   UTXO's
   */
  void AddInputs(const std::vector<UtxoData>& utxos);

  /**
   * @brief collect utxo and cache into utxo_map_.
   * @param[in] utxos   utxo list.
   */
  void CollectInputUtxo(const std::vector<UtxoData>& utxos);

  /**
   * @brief Get UTXO by outpoint
   * @param[in] outpoint  TxIn txid and vout
   * @return UTXO
   */
  UtxoData GetTxInUtxoData(const OutPoint& outpoint) const;

  /**
   * @brief Execute blinding txout using utxo data.
   * @param[in] confidential_addresses     txout confidential address list.
   *   (if set nullptr, confidential key collect from nonce.)
   * @param[in] minimum_range_value        rangeproof minimum value.
   *   0 to max(int64_t)
   * @param[in] exponent                   rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits               rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @param[out] blinder_list             blinder list. (default is null)
   */
  void Blind(
      const std::vector<ElementsConfidentialAddress>* confidential_addresses =
          nullptr,
      int64_t minimum_range_value = 1, int exponent = 0,
      int minimum_bits = kDefaultBlindMinimumBits,
      std::vector<BlindData>* blinder_list = nullptr);
  /**
   * @brief Execute blinding issuance & txout using utxo data.
   * @param[in] issuance_key_map           issue blinding key map.
   * @param[in] confidential_addresses     txout confidential address list.
   *   (if set nullptr, confidential key collect from nonce.)
   * @param[in] minimum_range_value        rangeproof minimum value.
   *   0 to max(int64_t)
   * @param[in] exponent                   rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits               rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @param[out] blinder_list             blinder list. (default is null)
   */
  void BlindIssuance(
      const std::map<OutPoint, IssuanceBlindingKeyPair>& issuance_key_map,
      const std::vector<ElementsConfidentialAddress>* confidential_addresses =
          nullptr,
      int64_t minimum_range_value = 1, int exponent = 0,
      int minimum_bits = kDefaultBlindMinimumBits,
      std::vector<BlindData>* blinder_list = nullptr);

  /**
   * @brief sign with privkey.
   * @param[in] outpoint      utxo target.
   * @param[in] pubkey        public key.
   * @param[in] privkey       private key.
   * @param[in] sighash_type  sighash type.
   * @param[in] has_grind_r   calcurate signature glind-r flag. (default:true)
   */
  void SignWithKey(
      const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
      SigHashType sighash_type = SigHashType(), bool has_grind_r = true);

  /**
   * @brief set ignore verify target.
   * @param[in] outpoint    utxo target.
   */
  void IgnoreVerify(const OutPoint& outpoint);
  /**
   * @brief verify tx sign (signature).
   */
  void Verify();
  /**
   * @brief verify tx sign (signature) on outpoint.
   * @param[in] outpoint    utxo target.
   */
  void Verify(const OutPoint& outpoint);
  /**
   * @brief verify tx and generate transaction raw data.
   * @return transaction raw data.
   */
  ByteData Finalize();

  // sign-api
  /**
   * @brief Calculates the TxIn Signature Hash in the specified PubkeyHash format.
   * @param[in] outpoint TxIn txid and vout
   * @param[in] pubkey  Public key
   * @param[in] sighash_type SigHash type
   * @param[in] value UTXO amount specified by TxIn
   * @param[in] version Witness Version of UTXO specified by TxIn
   * @return Signature hash
   */
  virtual ByteData CreateSignatureHash(
      const OutPoint& outpoint, const Pubkey& pubkey, SigHashType sighash_type,
      const Amount& value, WitnessVersion version) const;
  /**
   * @brief Calculates the TxIn Signature Hash in the specified ScriptHash format.
   * @details Not supported for Scripts with OP_CODESEPARATOR
   * @param[in] outpoint TxIn txid and vout
   * @param[in] redeem_script   Script Hash Redeem Script
   * @param[in] sighash_type    SigHash type
   * @param[in] value UTXO amount specified by TxIn
   * @param[in] version Witness Version of UTXO specified by TxIn
   * @return Signature hash
   */
  virtual ByteData CreateSignatureHash(
      const OutPoint& outpoint, const Script& redeem_script,
      SigHashType sighash_type, const Amount& value,
      WitnessVersion version) const;
  /**
   * @brief Calculates the TxIn Signature Hash in the specified PubkeyHash format.
   * @param[in] outpoint TxIn txid and vout
   * @param[in] pubkey  Public key
   * @param[in] sighash_type SigHash type
   * @param[in] value UTXO Value Commitment specified by TxIn
   * @param[in] version Witness Version of UTXO specified by TxIn
   * @return Signature hash
   */
  virtual ByteData CreateSignatureHash(
      const OutPoint& outpoint, const Pubkey& pubkey, SigHashType sighash_type,
      const ConfidentialValue& value, WitnessVersion version) const;
  /**
   * @brief Calculates the TxIn Signature Hash in the specified ScriptHash format.
   * @details Not supported for Scripts with OP_CODESEPARATOR
   * @param[in] outpoint TxIn txid and vout
   * @param[in] redeem_script   Script Hash Redeem Script
   * @param[in] sighash_type    SigHash type
   * @param[in] value UTXO Value Commitment specified by TxIn
   * @param[in] version Witness Version of UTXO specified by TxIn
   * @return Signature hash
   */
  virtual ByteData CreateSignatureHash(
      const OutPoint& outpoint, const Script& redeem_script,
      SigHashType sighash_type, const ConfidentialValue& value,
      WitnessVersion version) const;

  /**
   * @brief Sign the specified TxIn with pubkey hash.
   * @param[in] outpoint        outpoint
   * @param[in] pubkey          Public key
   * @param[in] privkey         Private key
   * @param[in] sighash_type    SigHash Type
   * @param[in] value           amount of UTXO
   * @param[in] address_type    address-type.(P2WPKH, P2SH-P2WPKH, P2PKH)
   * @param[in] has_grind_r     Grind-R option for sign.
   */
  void SignWithPrivkeySimple(
      const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
      SigHashType sighash_type, const Amount& value,
      AddressType address_type = AddressType::kP2wpkhAddress,
      bool has_grind_r = true);

  /**
   * @brief Sign the specified TxIn with pubkey hash.
   * @param[in] outpoint        outpoint
   * @param[in] pubkey          Public key
   * @param[in] privkey         Private key
   * @param[in] sighash_type    SigHash Type
   * @param[in] value           value commitment of UTXO
   * @param[in] address_type    address-type.(P2WPKH, P2SH-P2WPKH, P2PKH)
   * @param[in] has_grind_r     Grind-R option for sign.
   */
  void SignWithPrivkeySimple(
      const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
      SigHashType sighash_type, const ConfidentialValue& value,
      AddressType address_type = AddressType::kP2wpkhAddress,
      bool has_grind_r = true);

  /**
   * @brief add pubkey-hash sign data to target outpoint.
   * @param[in] outpoint        TxIn txid and vout
   * @param[in] signature       signature
   * @param[in] pubkey          pubkey
   * @param[in] address_type    address-type.(P2WPKH, P2SH-P2WPKH, P2PKH)
   */
  void AddPubkeyHashSign(
      const OutPoint& outpoint, const SignParameter& signature,
      const Pubkey& pubkey, AddressType address_type);

  /**
   * @brief add script-hash sign data to target outpoint.
   * @param[in] outpoint            TxIn txid and vout
   * @param[in] signatures          signature list
   * @param[in] redeem_script       redeem script
   * @param[in] address_type        address-type.(P2WSH, P2SH-P2WSH, P2SH)
   * @param[in] is_multisig_script  use multisig script
   */
  void AddScriptHashSign(
      const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
      const Script& redeem_script, AddressType address_type,
      bool is_multisig_script = false);

  /**
   * @brief add multisig sign data to target outpoint.
   * @details The order of the signatures to be added is \
   *    automatically arranged by correspondence between \
   *    the pubkey of the redeem script and the relatedPubkey \
   *    in the signatures.
   *    (If relatedPubkey is not set, add signatures in the order of \
   *    signParam after adding signatures with relatedPubkey set)
   * @param[in] outpoint          TxIn txid and vout
   * @param[in] signatures        signature list
   * @param[in] redeem_script     redeem script
   * @param[in] hash_type         hash-type.(P2WSH, P2SH-P2WSH, P2SH)
   */
  void AddMultisigSign(
      const OutPoint& outpoint, const std::vector<SignParameter>& signatures,
      const Script& redeem_script, AddressType hash_type);

  /**
   * @brief add sign data to target outpoint.
   * @param[in] outpoint        TxIn txid and vout
   * @param[in] sign_params     sign data list
   * @param[in] insert_witness  use witness
   * @param[in] clear_stack     clear stack data before add.
   */
  void AddSign(
      const OutPoint& outpoint, const std::vector<SignParameter>& sign_params,
      bool insert_witness = true, bool clear_stack = false);

  /**
   * @brief Verify signature which is specified (pubkey hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] outpoint            TxIn txid and vout
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output amount commitment.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey,
      const OutPoint& outpoint, SigHashType sighash_type,
      const ConfidentialValue& value,
      WitnessVersion version = WitnessVersion::kVersionNone) const;
  /**
   * @brief Verify signature which is specified (script hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] outpoint            TxIn txid and vout
   * @param[in] script              redeem script related previous output.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output commitment.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey,
      const OutPoint& outpoint, const Script& script, SigHashType sighash_type,
      const ConfidentialValue& value,
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Get the default sequence number from the lock time.
   * @retval 0xffffffff     locktime disable
   * @retval 0xfffffffe     locktime enable
   */
  uint32_t GetDefaultSequence() const;

  /**
   * @brief Get the default sequence number from the lock time.
   * @retval 0xffffffff     locktime disable
   */
  static uint32_t GetLockTimeDisabledSequence();

  /**
   * @brief check multisig script and parameter.
   * @param[in] sign_list       multisig sign list.
   * @param[in] redeem_script   multisig script.
   * @return sorted list.
   */
  static std::vector<SignParameter> CheckMultisig(
      const std::vector<SignParameter>& sign_list,
      const Script& redeem_script);

 protected:
  /**
   * @brief This function is called by the state change.
   * @param[in] type    change type
   */
  virtual void CallbackStateChange(uint32_t type);

  /**
   * @brief find target outpoint utxo list.
   * @param[in] outpoint    outpoint
   * @param[out] utxo       utxo
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindUtxoMap(const OutPoint& outpoint, UtxoData* utxo = nullptr) const;

  /**
   * @brief find target outpoint from list.
   * @param[in] list        outpoint list
   * @param[in] outpoint    outpoint
   * @retval true   exist
   * @retval false  not exist
   */
  bool IsFindOutPoint(
      const std::vector<OutPoint>& list, const OutPoint& outpoint) const;

 private:
  /**
   * @brief utxo map.
   */
  std::vector<UtxoData> utxo_map_;
  /**
   * @brief utxo signed map. (outpoint, SigHashType)
   */
  std::map<OutPoint, SigHashType> signed_map_;
  /**
   * @brief utxo verify map. (outpoint)
   */
  std::vector<OutPoint> verify_map_;
  /**
   * @brief utxo verify ignore map. (outpoint)
   */
  std::vector<OutPoint> verify_ignore_map_;
};

// ----------------------------------------------------------------------------
// deprecated
// ----------------------------------------------------------------------------

/**
 * @brief ConfidentialTransaction生成のためのControllerクラス
 * @deprecated replace to ConfidentialTransactionContext .
 */
class CFD_EXPORT ConfidentialTransactionController
    : public AbstractTransactionController {
 public:
  /**
   * @brief コンストラクタ.
   * @param[in] version   Transactionのバージョン
   * @param[in] locktime  Timestamp or ブロック高
   */
  explicit ConfidentialTransactionController(
      uint32_t version, uint32_t locktime);
  /**
   * @brief コンストラクタ.
   * @param[in] tx_hex  RawトランザクションHEX文字列
   */
  explicit ConfidentialTransactionController(const std::string& tx_hex);
  /**
   * @brief コンストラクタ
   * @param[in] transaction   トランザクション情報
   */
  ConfidentialTransactionController(
      const ConfidentialTransactionController& transaction);
  /**
   * @brief デストラクタ.
   */
  virtual ~ConfidentialTransactionController() {
    // do nothing
  }
  /**
   * @brief copy constructor.
   * @param[in] transaction   Transaction
   * @return Transaction
   */
  ConfidentialTransactionController& operator=(
      const ConfidentialTransactionController& transaction) &;

  /**
   * @brief locking_scriptが空のTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(const Txid& txid, uint32_t vout);
  /**
   * @brief locking_scriptが空のTxInを追加する.
   * @param[in] txid      UTXOのTxid
   * @param[in] vout      UTXOのvout
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, uint32_t sequence);
  /**
   * @brief locking_scriptにredeem_scriptを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] redeem_script  unlockに必要なredeem_script
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Script& redeem_script);
  /**
   * @brief locking_scriptにredeem_scriptを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] redeem_script  unlockに必要なredeem_script
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Script& redeem_script,
      uint32_t sequence);
  /**
   * @brief locking_scriptにpubkeyを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] pubkey  unlockに必要なpubkey
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey);
  /**
   * @brief locking_scriptにpubkeyを格納したTxInを追加する.
   * @param[in] txid  UTXOのTxid
   * @param[in] vout  UTXOのvout
   * @param[in] pubkey  unlockに必要なpubkey
   * @param[in] sequence  unlockで使用するsequence値
   * @return 追加したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference AddTxIn(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey,
      uint32_t sequence);

  /**
   * TxInを取得する.
   * @param[in] txid 取得するTxInのTxId
   * @param[in] vout 取得するTxInのvout
   * @return TxInReferenceインスタンス
   */
  const ConfidentialTxInReference GetTxIn(
      const Txid& txid, uint32_t vout) const;
  /**
   * @brief TxInを削除する.
   * @param[in] txid 取得するTxInのTxId
   * @param[in] vout 取得するTxInのvout
   * @return 削除したTxInのTxInReferenceインスタンス
   */
  const ConfidentialTxInReference RemoveTxIn(const Txid& txid, uint32_t vout);

  /**
   * @brief TxOutを追加する.
   * @param[in] address  送金先unblindedアドレス
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const Address& address, const Amount& value,
      const ConfidentialAssetId& asset);
  /**
   * @brief TxOutを追加する.
   * @param[in] address  送金先confidentialアドレス
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @param[in] remove_nonce      nonceの強制削除フラグ
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const ElementsConfidentialAddress& address,
      const Amount& value,  // amount
      const ConfidentialAssetId& asset, bool remove_nonce = false);
  /**
   * @brief TxOutを追加する.
   * @param[in] locking_script  送金先locking_script
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const Script& locking_script, const Amount& value,
      const ConfidentialAssetId& asset);
  /**
   * @brief TxOutを追加する.
   * @param[in] locking_script  送金先locking_script
   * @param[in] value  送金額
   * @param[in] asset AssetID
   * @param[in] nonce nonce
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOut(
      const Script& locking_script, const Amount& value,
      const ConfidentialAssetId& asset, const ConfidentialNonce& nonce);

  /**
   * @brief Add txout for pegged-out.
   * @param[in] value Pegout value
   * @param[in] asset AssetID
   * @param[in] genesisblock_hash mainchainのgenesisblock hash
   * @param[in] btc_address mainchain address
   * @param[in] net_type mainchain network type
   * @param[in] online_pubkey online pubkey for whitelist proof
   * @param[in] master_online_key  online privkey for whitelist proof
   * @param[in] btc_descriptor descriptor by initpegoutwallet
   * @param[in] bip32_counter descriptor counter by initpegoutwallet
   * @param[in] whitelist whitelist
   * @param[in] elements_net_type elements network type
   * @param[out] btc_derive_address btc address by derived btc descriptor
   * @return TxOutReference object with added txout.
   */
  const ConfidentialTxOutReference AddPegoutTxOut(
      const Amount& value, const ConfidentialAssetId& asset,
      const BlockHash& genesisblock_hash, const Address& btc_address,
      NetType net_type = NetType::kMainnet,
      const Pubkey& online_pubkey = Pubkey(),
      const Privkey& master_online_key = Privkey(),
      const std::string& btc_descriptor = "", uint32_t bip32_counter = 0,
      const ByteData& whitelist = ByteData(),
      NetType elements_net_type = NetType::kLiquidV1,
      Address* btc_derive_address = nullptr);

  /**
   * @brief TxOut(Fee)を追加する.
   * @param[in] value Fee額
   * @param[in] asset AssetID
   * @return 追加したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference AddTxOutFee(
      const Amount& value, const ConfidentialAssetId& asset);
  /**
   * @brief TxOutのFee情報を更新する.
   * @param[in] index 設定対象のindex
   * @param[in] value Fee額
   * @param[in] asset AssetID
   * @return 更新したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference UpdateTxOutFeeAmount(
      uint32_t index, const Amount& value, const ConfidentialAssetId& asset);
  /**
   * @brief TxOutを削除する.
   * @param[in] index 削除対象のindex
   * @return 削除したTxOutのTxOutReferenceインスタンス
   */
  const ConfidentialTxOutReference RemoveTxOut(uint32_t index);

  /**
   * @brief Unlocking Scriptに挿入する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 設定対象のTxInのtxid
   * @param[in] vout 設定対象のTxInのvout
   * @param[in] unlocking_scripts  署名を含むscriptリスト
   */
  void InsertUnlockingScript(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& unlocking_scripts);
  /**
   * @brief TxInにUnlocking Scriptを設定する.
   * @param[in] txid 設定対象のTxInのtxid
   * @param[in] vout 設定対象のTxInのvout
   * @param[in] unlocking_script  署名を含むunlockingScript全体
   */
  void SetUnlockingScript(
      const Txid& txid, uint32_t vout, const Script& unlocking_script);
  /**
   * @brief Unlocking Scriptを設定する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 設定対象のTxInのtxid
   * @param[in] vout 設定対象のTxInのvout
   * @param[in] unlocking_scripts  署名を含むscriptリスト
   */
  void SetUnlockingScript(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& unlocking_scripts);

  /**
   * @brief WitnessStackを追加する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] witness_datas   WitnessStack追加情報リスト
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& witness_datas);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hash  署名のHash文字列
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::string& signed_signature_hash);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hashes  署名のHash文字列リスト
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::vector<std::string>& signed_signature_hashes);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hash  署名のHash文字列
   * @param[in] pubkey  署名のpubkey
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::string& signed_signature_hash, const Pubkey& pubkey);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hash  署名のHash文字列
   * @param[in] redeem_script  署名のredeem_script
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::string& signed_signature_hash, const Script& redeem_script);
  /**
   * @brief WitnessStackを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] signed_signature_hashes  署名のHash文字列リスト
   * @param[in] redeem_script  署名のredeem_script
   */
  void AddWitnessStack(
      const Txid& txid, uint32_t vout,
      const std::vector<std::string>& signed_signature_hashes,
      const Script& redeem_script);
  /**
   * @brief WitnessStackの指定Indexのデータを更新する.
   * @details OP_CODEの追加は未対応
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   * @param[in] witness_index   WitnessStackのindex
   * @param[in] witness_stack   WitnessStack更新情報
   */
  void SetWitnessStack(
      const Txid& txid, uint32_t vout, uint32_t witness_index,
      const ByteData& witness_stack);
  /**
   * @brief WitnessStackの指定Indexのデータを更新する.
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   * @param[in] witness_index   WitnessStackのindex
   * @param[in] hex_string      WitnessStack更新情報(HEX文字列)
   */
  void SetWitnessStack(
      const Txid& txid, uint32_t vout, uint32_t witness_index,
      const std::string& hex_string);
  /**
   * @brief WitnessStackを削除する.
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   */
  void RemoveWitnessStackAll(const Txid& txid, uint32_t vout);
  /**
   * @brief WitnessStack数を取得する.
   * @param[in] txid 取得対象のTxInのtxid
   * @param[in] vout 取得対象のTxInのvout
   * @return WitnessStack数
   */
  uint32_t GetWitnessStackNum(const Txid& txid, uint32_t vout) const;

  /**
   * @brief PeginWitnessに文字列から得られるバイトデータを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] amount peginしたinputのamount
   * @param[in] asset_id 送金先アセットID
   * @param[in] mainchain_genesis_block_hash mainchainのgenesis_blockのhash値
   * @param[in] claim_script pegin請求用スクリプト
   * @param[in] mainchain_pegin_transaction peginしたmainchainのtransaction
   * @param[in] tx_out_proof peginしたtransactionが取り込まれた際のproof値
   */
  void AddPeginWitness(
      const Txid& txid, uint32_t vout, const Amount& amount,
      const ConfidentialAssetId& asset_id,
      const BlockHash& mainchain_genesis_block_hash,
      const Script& claim_script, const ByteData& mainchain_pegin_transaction,
      const ByteData& tx_out_proof);
  /**
   * @brief PeginWitnessにバイトデータを追加する.
   * @param[in] txid 追加対象のTxInのtxid
   * @param[in] vout 追加対象のTxInのvout
   * @param[in] witness_datas   WitnessStack追加情報リスト
   */
  void AddPeginWitness(
      const Txid& txid, uint32_t vout,
      const std::vector<ByteData>& witness_datas);
  /**
   * @brief WitnessStackを削除する.
   * @param[in] txid 更新対象のTxInのtxid
   * @param[in] vout 更新対象のTxInのvout
   */
  void RemovePeginWitnessAll(const Txid& txid, uint32_t vout);

  /**
   * @brief Transaction取得.
   * @return Transactionインスタンス
   */
  const ConfidentialTransaction& GetTransaction() const;

  /**
   * @brief TxInを除外したサイズを取得する。
   * @param[in] is_blinded    blind時の想定サイズを取得するフラグ
   * @param[out] witness_area_size     witness area size
   * @param[out] no_witness_area_size  no witness area size
   * @param[in] exponent                  rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits              rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @return TxInを除外したTxサイズ(Serialize)
   */
  uint32_t GetSizeIgnoreTxIn(
      bool is_blinded = false, uint32_t* witness_area_size = nullptr,
      uint32_t* no_witness_area_size = nullptr, int exponent = 0,
      int minimum_bits = cfd::core::kDefaultBlindMinimumBits) const;

  /**
   * @brief TxInを除外した仮想サイズを取得する。
   * @param[in] is_blinded    blind時の想定サイズを取得するフラグ
   * @param[in] exponent                  rangeproof exponent value.
   *   -1 to 18. -1 is public value. 0 is most private.
   * @param[in] minimum_bits              rangeproof blinding bits.
   *   0 to 64. Number of bits of the value to keep private. 0 is auto.
   * @return TxInを除外したTx仮想サイズ(Serialize)
   */
  uint32_t GetVsizeIgnoreTxIn(
      bool is_blinded = false, int exponent = 0,
      int minimum_bits = cfd::core::kDefaultBlindMinimumBits) const;

  /**
   * @brief IssueAssetの情報を設定する.
   * @param[in] txid                 対象のTxInのtxid
   * @param[in] vout                 対象のTxInのvout
   * @param[in] asset_amount         issuance amount
   * @param[in] asset_locking_script asset locking script
   * @param[in] asset_nonce          asset nonce
   * @param[in] token_amount         token amount
   * @param[in] token_locking_script token locking script
   * @param[in] token_nonce          token nonce
   * @param[in] is_blind             blinding issuance
   * @param[in] contract_hash        asset entropy
   * @param[in] is_random_sort       set txout randomize
   * @param[in] is_remove_nonce      nonceの強制削除フラグ
   * @return issuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetIssuance(
      const Txid& txid, uint32_t vout, const Amount& asset_amount,
      const Script& asset_locking_script, const ByteData& asset_nonce,
      const Amount& token_amount, const Script& token_locking_script,
      const ByteData& token_nonce, bool is_blind,
      const ByteData256& contract_hash, bool is_random_sort = false,
      bool is_remove_nonce = false);

  /**
   * @brief ReissueAssetの情報を設定する.
   * @param[in] txid              対象のTxInのtxid
   * @param[in] vout              対象のTxInのvout
   * @param[in] amount            reissuance amount
   * @param[in] locking_script    asset locking script
   * @param[in] asset_nonce             confidential key
   * @param[in] blind_factor      blind factor
   * @param[in] entropy           asset entropy
   * @param[in] is_random_sort    set txout randomize
   * @param[in] is_remove_nonce   nonceの強制削除フラグ
   * @return reissuance entropy and asset parameter.
   */
  IssuanceParameter SetAssetReissuance(
      const Txid& txid, uint32_t vout, const Amount& amount,
      const Script& locking_script, const ByteData& asset_nonce,
      const BlindFactor& blind_factor, const BlindFactor& entropy,
      bool is_random_sort = false, bool is_remove_nonce = false);

  /**
   * @brief TxOutの順序をランダム化する.
   * @details ブラインド前のみ実施可能.
   */
  void RandomSortTxOut();

  /**
   * @brief TrasnsactionをBlindする.
   * @param[in] txin_info_list            txin blind info list.
   * @param[in] issuance_blinding_keys    issue blinding key list.
   * @param[in] txout_confidential_keys   blinding pubkey list.
   */
  void BlindTransaction(
      const std::vector<BlindParameter>& txin_info_list,
      const std::vector<IssuanceBlindingKeyPair>& issuance_blinding_keys,
      const std::vector<Pubkey>& txout_confidential_keys);

  /**
   * @brief 指定indexのTxOutをUnblindする.
   * @param[in] tx_out_index unblind target txout index
   * @param[in] blinding_key blinding privkey
   * @return struct contain blind factors and amount.
   */
  UnblindParameter UnblindTxOut(
      uint32_t tx_out_index, const Privkey& blinding_key);

  /**
   * @brief TransactionをUnblindする.
   * @param[in] blinding_keys blinding privkey list
   * @return list of structs contain blind factors and amount.
   */
  std::vector<UnblindParameter> UnblindTransaction(
      const std::vector<Privkey>& blinding_keys);

  /**
   * @brief IssuanceをUnblindする.
   * @param[in] tx_in_index unblind target index
   * @param[in] asset_blinding_key asset blinding privkey
   * @param[in] token_blinding_key token blinding privkey
   * @return list of structs contain blind factors and amount.
   */
  std::vector<UnblindParameter> UnblindIssuance(
      uint32_t tx_in_index, const Privkey& asset_blinding_key,
      const Privkey& token_blinding_key);

  /**
   * @brief 指定されたPubkeyHash形式のTxInのSignatureHashを計算する.
   * @param[in] txid SignatureHash算出対象のTxInのtxid
   * @param[in] vout SignatureHash算出対象のTxInのvout
   * @param[in] pubkey SignatureHashの公開鍵
   * @param[in] sighash_type SigHashType値
   * @param[in] value TxInで指定したUTXOのamount
   * @param[in] version TxInで指定したUTXOのWitnessVersion
   * @return 算出されたSignatureHashのHex文字列
   */
  ByteData CreateSignatureHash(
      const Txid& txid, uint32_t vout, const Pubkey& pubkey,
      SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;
  /**
   * @brief 指定されたScriptHash形式のTxInのSignatureHashを計算する.
   * @details OP_CODESEPARATORが存在するScriptについては未対応
   * @param[in] txid SignatureHash算出対象のTxInのtxid
   * @param[in] vout SignatureHash算出対象のTxInのvout
   * @param[in] witness_script WSHのWitness Script
   * @param[in] sighash_type SigHashType値
   * @param[in] value TxInで指定したUTXOのamount/amountcommitment
   * @param[in] version TxInで指定したUTXOのWitnessVersion
   * @return 算出されたSignatureHashのHex文字列
   */
  ByteData CreateSignatureHash(
      const Txid& txid, uint32_t vout, const Script& witness_script,
      SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Verify signature which is specified (pubkey hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] txid                target input's transaction id.
   * @param[in] vout                target input's previous output index.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output confidential value.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
      uint32_t vout, SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief Verify signature which is specified (pubkey hash) input data.
   * @details Not supported yet the script which includes OP_CODESEPARATOR.
   * @param[in] signature           signature to be verified.
   * @param[in] pubkey              public key to verify signature.
   * @param[in] txid                target input's transaction id.
   * @param[in] vout                target input's previous output index.
   * @param[in] script              redeem script related previous output.
   * @param[in] sighash_type        sighash type class. (SigHashType)
   * @param[in] value               previous output confidential value.
   *     (require if the previous output send to witness.)
   * @param[in] version             previous output witness version.
   *     (specify witness version if the previous output send to witness.)
   * @retval true       correct signature.
   * @retval false      incorrect signature.
   */
  bool VerifyInputSignature(
      const ByteData& signature, const Pubkey& pubkey, const Txid& txid,
      uint32_t vout, const Script& script, SigHashType sighash_type,
      const ConfidentialValue& value = ConfidentialValue(),
      WitnessVersion version = WitnessVersion::kVersionNone) const;

  /**
   * @brief 簡易のFee計算を行う.
   * @param[in] append_feature_signed_size  sign分のサイズ加算フラグ
   * @param[in] append_signed_witness       sign分の加算をwitness扱いとするか
   * @return Fee Amount
   */
  Amount CalculateSimpleFee(
      bool append_feature_signed_size = true,
      bool append_signed_witness = true) const;

 private:
  /**
   * @brief Transactionインスタンス.
   */
  ConfidentialTransaction transaction_;
};

}  // namespace cfd

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFD_CFD_ELEMENTS_TRANSACTION_H_
