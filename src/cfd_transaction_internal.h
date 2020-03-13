// Copyright 2020 CryptoGarage
/**
 * @file cfd_transaction_internal.h
 *
 * @brief \~english declaration of transaction utility class.
 *   \~japanese Transaction Utilityクラスの定義ファイル
 */
#ifndef CFD_SRC_CFD_TRANSACTION_INTERNAL_H_
#define CFD_SRC_CFD_TRANSACTION_INTERNAL_H_

#include "cfd/cfd_transaction.h"

#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_util.h"
// #include "cfdcore_util.h"
// #include "cfd_node_util.h"

namespace cfd {

using cfd::core::AbstractTxIn;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::OutPoint;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::WitnessVersion;

/// シーケンス値(locktime有効)
constexpr uint32_t kSequenceEnableLockTimeMax = 0xfffffffeU;
/// シーケンス値(locktime無効)
constexpr uint32_t kSequenceDisableLockTime = 0xffffffffU;
/// multisig key数上限
constexpr uint32_t kMaximumMultisigKeyNum = 15;

/**
 * @brief Utility class for transaction context.
 */
class TransactionContextUtil {
 public:
  // sign-api
  /**
   * @brief 指定されたPubkeyHash形式のTxInに署名する.
   * @param[in] outpoint      TxIn
   * @param[in] pubkey        SignatureHashの公開鍵
   * @param[in] privkey       SignatureHashの秘密鍵
   * @param[in] address_type  address-type.(P2WPKH, P2SH-P2WPKH, P2PKH)
   * @return Witness version.
   */
  static WitnessVersion CheckSignWithPrivkeySimple(
      const OutPoint& outpoint, const Pubkey& pubkey, const Privkey& privkey,
      AddressType address_type);

  /**
   * @brief add pubkey-hash sign data to target outpoint.
   * @param[in,out] transaction   Transaction
   * @param[in] outpoint          TxIn
   * @param[in] signature         signature
   * @param[in] pubkey            pubkey
   * @param[in] address_type      address-type.(P2WPKH, P2SH-P2WPKH, P2PKH)
   */
  template <class Tx>
  static void AddPubkeyHashSign(
      Tx* transaction, const OutPoint& outpoint,
      const SignParameter& signature, const Pubkey& pubkey,
      AddressType address_type);

  /**
   * @brief add script-hash sign data to target outpoint.
   * @param[in,out] transaction     Transaction
   * @param[in] outpoint            TxIn
   * @param[in] signatures          signature list
   * @param[in] redeem_script       redeem script
   * @param[in] address_type        address-type.(P2WSH, P2SH-P2WSH, P2SH)
   * @param[in] is_multisig_script  use multisig script
   */
  template <class Tx>
  static void AddScriptHashSign(
      Tx* transaction, const OutPoint& outpoint,
      const std::vector<SignParameter>& signatures,
      const Script& redeem_script, AddressType address_type,
      bool is_multisig_script);

  /**
   * @brief add sign data to target outpoint.
   * @param[in,out] transaction   Transaction
   * @param[in] outpoint          TxIn
   * @param[in] sign_params       sign data list
   * @param[in] insert_witness    use witness
   * @param[in] clear_stack       clear stack data before add.
   */
  template <class Tx>
  static void AddSign(
      Tx* transaction, const OutPoint& outpoint,
      const std::vector<SignParameter>& sign_params, bool insert_witness,
      bool clear_stack);

  /**
   * @brief verify tx sign (signature) on outpoint.
   * @param[in,out] transaction       Transaction
   * @param[in] outpoint              utxo target txin & vout.
   * @param[in] utxo                  utxo.
   * @param[in] txin                  tx input.
   * @param[in] create_sighash_func   craete sighash function.
   */
  template <class Tx>
  static void Verify(
      const Tx* transaction, const OutPoint& outpoint, const UtxoData& utxo,
      const AbstractTxIn* txin,
      std::function<ByteData256(
          const Tx*, const OutPoint&, const UtxoData&, const SigHashType&,
          const Pubkey&, const Script&, WitnessVersion)>
          create_sighash_func);

  /**
   * @brief Has OP_TRUE locking script.
   * @param[in] utxo  utxo.
   * @param[in] txin  tx input.
   * @retval true   locking script is OP_TRUE.
   * @retval false  other.
   */
  static bool IsOpTrueLockingScript(
      const UtxoData& utxo, const AbstractTxIn* txin);

  /**
   * @brief Get verify signature stack.
   * @param[in] utxo                utxo.
   * @param[in] txin                tx input.
   * @param[out] witness_version    witness version.
   * @param[out] is_pubkey_stack    stack type is pubkey.
   * @return signature stack.
   */
  static std::vector<ByteData> GetVerifySignatureStack(
      const UtxoData& utxo, const AbstractTxIn* txin,
      WitnessVersion* witness_version, bool* is_pubkey_stack);

 private:
  /**
   * @brief constructor.
   */
  TransactionContextUtil();
};

}  // namespace cfd

#endif  // CFD_SRC_CFD_TRANSACTION_INTERNAL_H_
