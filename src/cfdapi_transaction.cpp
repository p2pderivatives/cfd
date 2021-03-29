// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_transaction.cpp
 *
 * @brief implementation file for transaction operation that uses cfd-api
 */

#include "cfd/cfdapi_transaction.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#include "cfd/cfd_address.h"
#include "cfd/cfd_fee.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_coin.h"
#include "cfdapi_transaction_base.h"  // NOLINT
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_iterator.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfdcore/cfdcore_util.h"

namespace cfd {
namespace api {

using cfd::FeeCalculator;
using cfd::TransactionController;
using cfd::api::TransactionApiBase;
using cfd::core::AbstractTransaction;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Txid;
using cfd::core::logger::info;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// In-file function
// -----------------------------------------------------------------------------
/**
   * @brief Create a TransactionController object.
   *
   * @param[in] hex of the transaction for which to create the controller object
   * @return a TransactionController instance
   */
static TransactionController CreateController(const std::string& hex) {
  return TransactionController(hex);
}

// -----------------------------------------------------------------------------
// TransactionApi
// -----------------------------------------------------------------------------

TransactionController TransactionApi::CreateRawTransaction(
    uint32_t version, uint32_t locktime, const std::vector<TxIn>& txins,
    const std::vector<TxOut>& txouts) const {
  if (4 < version) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to CreateRawTransaction. invalid version number: version={}",  // NOLINT
        version);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Invalid version number. We supports only 1, 2, 3, or 4:");
  }

  TransactionController txc(version, locktime);
  return AddRawTransaction(txc.GetHex(), txins, txouts);
}

TransactionController TransactionApi::AddRawTransaction(
    const std::string& tx_hex, const std::vector<TxIn>& txins,
    const std::vector<TxOut>& txouts) const {
  TransactionController txc(tx_hex);

  const uint32_t kDisableLockTimeSequence =
      TransactionController::GetLockTimeDisabledSequence();
  for (TxIn txin : txins) {
    // TxIn unlocking_script make be empty.
    if (kDisableLockTimeSequence == txin.GetSequence()) {
      txc.AddTxIn(txin.GetTxid(), txin.GetVout(), txc.GetDefaultSequence());
    } else {
      txc.AddTxIn(txin.GetTxid(), txin.GetVout(), txin.GetSequence());
    }
  }

  for (TxOut txout : txouts) {
    txc.AddTxOut(txout.GetLockingScript(), txout.GetValue());
  }

  return txc;
}

uint32_t TransactionApi::GetWitnessStackNum(
    const std::string& tx_hex, const Txid& txid, const uint32_t vout) const {
  return TransactionApiBase::GetWitnessStackNum<TransactionController>(
      cfd::api::CreateController, tx_hex, txid, vout);
}

TransactionController TransactionApi::AddSign(
    const std::string& hex, const Txid& txid, const uint32_t vout,
    const std::vector<SignParameter>& sign_params, bool is_witness,
    bool clear_stack) const {
  return TransactionApiBase::AddSign<TransactionController>(
      cfd::api::CreateController, hex, txid, vout, sign_params, is_witness,
      clear_stack);
}

TransactionController TransactionApi::UpdateWitnessStack(
    const std::string& tx_hex, const Txid& txid, const uint32_t vout,
    const SignParameter& update_sign_param, uint32_t stack_index) const {
  return TransactionApiBase::UpdateWitnessStack<TransactionController>(
      cfd::api::CreateController, tx_hex, txid, vout, update_sign_param,
      stack_index);
}

std::string TransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const TxInReference& txin, const Pubkey& pubkey,
    const Amount& amount, HashType hash_type,
    const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin, pubkey.GetData(), amount, hash_type, sighash_type);
}

std::string TransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const TxInReference& txin,
    const Script& redeem_script, const Amount& amount, HashType hash_type,
    const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin, redeem_script.GetData(), amount, hash_type, sighash_type);
}

std::string TransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const TxInReference& txin,
    const ByteData& key_data, const Amount& amount, HashType hash_type,
    const SigHashType& sighash_type) const {
  return CreateSignatureHash(
      tx_hex, txin.GetTxid(), txin.GetVout(), key_data, amount, hash_type,
      sighash_type);
}

std::string TransactionApi::CreateSignatureHash(
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const ByteData& key_data, const Amount& amount, HashType hash_type,
    const SigHashType& sighash_type) const {
  ByteData sig_hash;
  TransactionController txc(tx_hex);
  WitnessVersion version = WitnessVersion::kVersionNone;
  if ((hash_type == HashType::kP2wpkh) || (hash_type == HashType::kP2wsh)) {
    version = WitnessVersion::kVersion0;
  }

  switch (hash_type) {
    case HashType::kP2pkh:
      // fall-through
    case HashType::kP2wpkh:
      sig_hash = txc.CreateSignatureHash(
          txid, vout, Pubkey(key_data), sighash_type, amount, version);
      break;
    case HashType::kP2sh:
      // fall-through
    case HashType::kP2wsh:
      sig_hash = txc.CreateSignatureHash(
          txid, vout, Script(key_data), sighash_type, amount, version);
      break;
    default:
      warn(
          CFD_LOG_SOURCE,
          "Failed to CreateSignatureHash. Invalid hash_type: "
          "hash_type={}",  // NOLINT
          hash_type);
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hash_type. hash_type must be \"p2pkh\"(0) "
          "or \"p2sh\"(1) or \"p2wpkh\"(2) or \"p2wsh\"(3).");  // NOLINT
  }

  return sig_hash.GetHex();
}

TransactionController TransactionApi::AddMultisigSign(
    const std::string& tx_hex, const TxInReference& txin,
    const std::vector<SignParameter>& sign_list, AddressType address_type,
    const Script& witness_script, const Script redeem_script,
    bool clear_stack) {
  return AddMultisigSign(
      tx_hex, txin.GetTxid(), txin.GetVout(), sign_list, address_type,
      witness_script, redeem_script, clear_stack);
}

TransactionController TransactionApi::AddMultisigSign(
    const std::string& tx_hex, const Txid& txid, uint32_t vout,
    const std::vector<SignParameter>& sign_list, AddressType address_type,
    const Script& witness_script, const Script redeem_script,
    bool clear_stack) {
  std::string result =
      TransactionApiBase::AddMultisigSign<TransactionController>(
          CreateController, tx_hex, txid, vout, sign_list, address_type,
          witness_script, redeem_script, clear_stack);
  return TransactionController(result);
}

Amount TransactionApi::EstimateFee(
    const std::string& tx_hex, const std::vector<UtxoData>& utxos,
    Amount* txout_fee, Amount* utxo_fee, double effective_fee_rate) const {
  TransactionContext txc(tx_hex);

  uint32_t size = 0;
  uint32_t witness_size = 0;
  uint32_t wit_size = 0;
  uint32_t nowit_size = 0;
  uint32_t not_witness_count = 0;
  for (const auto& utxo : utxos) {
    NetType net_type = NetType::kMainnet;
    if (!utxo.address.GetAddress().empty()) {
      net_type = utxo.address.GetNetType();
    }
    AddressFactory address_factory(net_type);
    // check descriptor
    std::string descriptor = utxo.descriptor;
    // set dummy NetType for getting AddressType.
    auto data = address_factory.ParseOutputDescriptor(descriptor);

    AddressType addr_type;
    if (utxo.address.GetAddress().empty() ||
        data.address_type == AddressType::kP2shP2wpkhAddress ||
        data.address_type == AddressType::kP2shP2wshAddress) {
      addr_type = data.address_type;
    } else {
      addr_type = utxo.address.GetAddressType();
    }

    Script redeem_script;
    if (utxo.redeem_script.IsEmpty() && !data.redeem_script.IsEmpty()) {
      redeem_script = data.redeem_script;
    } else {
      redeem_script = utxo.redeem_script;
    }
    const Script* scriptsig_template = nullptr;
    if (((!redeem_script.IsEmpty()) ||
         (addr_type == AddressType::kTaprootAddress)) &&
        (!utxo.scriptsig_template.IsEmpty())) {
      scriptsig_template = &utxo.scriptsig_template;
    }

    TxIn::EstimateTxInSize(
        addr_type, redeem_script, &wit_size, &nowit_size, scriptsig_template);
    size += nowit_size;
    witness_size += wit_size;
    if (wit_size == 0) ++not_witness_count;
  }

  uint32_t tx_size = txc.GetSizeIgnoreTxIn((witness_size != 0));
  uint32_t tx_vsize = AbstractTransaction::GetVsizeFromSize(tx_size, 0);

  if ((witness_size != 0) && (not_witness_count != 0) &&
      (not_witness_count < static_cast<uint32_t>(utxos.size()))) {
    // append witness size for p2pkh or p2sh
    witness_size += not_witness_count;
  }

  uint32_t utxo_vsize =
      AbstractTransaction::GetVsizeFromSize(size, witness_size);

  uint64_t fee_rate = static_cast<uint64_t>(floor(effective_fee_rate * 1000));
  FeeCalculator fee_calc(fee_rate);
  Amount tx_fee_amount = fee_calc.GetFee(tx_vsize);
  Amount utxo_fee_amount = fee_calc.GetFee(utxo_vsize);
  uint32_t total_size = tx_size + size;
  uint32_t total_vsize =
      AbstractTransaction::GetVsizeFromSize(total_size, witness_size);
  Amount fee = fee_calc.GetFee(total_vsize);

  if (txout_fee) *txout_fee = tx_fee_amount;
  if (utxo_fee) *utxo_fee = utxo_fee_amount;

  info(
      CFD_LOG_SOURCE, "EstimateFee rate={} fee={} tx={} utxo={}",
      effective_fee_rate, fee.GetSatoshiValue(),
      tx_fee_amount.GetSatoshiValue(), utxo_fee_amount.GetSatoshiValue());
  return fee;
}

TransactionController TransactionApi::FundRawTransaction(
    const std::string& tx_hex, const std::vector<UtxoData>& utxos,
    const Amount& target_value,
    const std::vector<UtxoData>& selected_txin_utxos,
    const std::string& reserve_txout_address, double effective_fee_rate,
    Amount* estimate_fee, const UtxoFilter* filter,
    const CoinSelectionOption* option_params,
    std::vector<std::string>* append_txout_addresses, NetType net_type,
    const std::vector<AddressFormatData>* prefix_list) const {
  // set option
  CoinSelectionOption option;
  UtxoFilter utxo_filter;
  if (filter) utxo_filter = *filter;
  if (option_params) {
    option = *option_params;
  } else {
    option.InitializeTxSizeInfo();
    option.SetEffectiveFeeBaserate(effective_fee_rate);
    option.SetLongTermFeeBaserate(effective_fee_rate);
  }

  AddressFactory addr_factory(net_type);
  if (prefix_list) {
    addr_factory = AddressFactory(net_type, *prefix_list);
  }
  Address reserve_address = addr_factory.GetAddress(reserve_txout_address);
  // FIXME(fujita-cg): AddressFactory::GetAddress()でチェックをしたいが、
  //   影響範囲が大きいためAPIでチェックを実行
  if (!addr_factory.CheckAddressNetType(reserve_address, net_type)) {
    warn(
        CFD_LOG_SOURCE,
        "Failed to FundRawTransaction. "
        "Input address and network is unmatch."
        ": address=[{}], input_net_type=[{}], parsed_net_type=[{}]",
        reserve_txout_address, net_type, reserve_address.GetNetType());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to FundRawTransaction. "
        "Input address and network is unmatch.");
  }

  // txから設定済みTxIn/TxOutの額を収集
  // (selected_txin_utxos指定分はtxid一致なら設定済みUTXO扱い)
  TransactionController txc(tx_hex);
  const Transaction& tx = txc.GetTransaction();
  Amount txin_amount;
  Amount txout_amount;
  for (const auto& txout : tx.GetTxOutList()) {
    txout_amount += txout.GetValue();
  }
  std::vector<UtxoData> matched_txin_utxos;
  matched_txin_utxos.reserve(selected_txin_utxos.size());
  const auto& txin_list = tx.GetTxInList();
  for (const auto& utxo : selected_txin_utxos) {
    for (const auto& txin : txin_list) {
      if ((txin.GetTxid().Equals(utxo.txid)) &&
          (utxo.vout == txin.GetVout())) {
        txin_amount += utxo.amount;
        matched_txin_utxos.push_back(utxo);
        break;
      }
    }
  }

  std::vector<UtxoData> utxodata_list;
  utxodata_list.reserve(utxos.size());
  for (const auto& utxo : utxos) {
    bool isFind = false;
    for (const auto& txin : txin_list) {
      if ((txin.GetTxid().Equals(utxo.txid)) &&
          (utxo.vout == txin.GetVout())) {
        isFind = true;
        break;
      }
    }
    if (!isFind) {
      utxodata_list.push_back(utxo);
    }
  }

  Amount fee;
  if (option.GetEffectiveFeeBaserate() != 0) {
    fee = EstimateFee(
        tx_hex, matched_txin_utxos, nullptr, nullptr, effective_fee_rate);
    info(
        CFD_LOG_SOURCE, "FundRawTransaction: pre estimation fee=[{}]",
        fee.GetSatoshiValue());
  }

  Amount diff_amount = Amount();        // difference between input and output
  Amount target_amount = target_value;  // selection require amount
  if (txin_amount > txout_amount) {
    // txinの余剰分を計算
    diff_amount = txin_amount - txout_amount;
    if (diff_amount < target_amount) {
      // txinの余剰分でtarget分が満たされない場合、満たされない分をコインセレクト
      target_amount -= diff_amount;
    }
  } else if (txin_amount < txout_amount) {
    // txoutの不足分を計算
    diff_amount = txout_amount - txin_amount;
    // txoutの不足分を合わせてコインセレクト
    target_amount += diff_amount;
  }

  // execute coinselection
  std::vector<Utxo> utxo_list = UtxoUtil::ConvertToUtxo(utxodata_list);
  Amount txin_total_amount = txin_amount;
  std::vector<Utxo> selected_coins;
  if (target_amount > 0 || fee > 0) {
    info(
        CFD_LOG_SOURCE, "FundRawTransaction:CoinSelection: target_amount=[{}]",
        target_amount.GetSatoshiValue());
    CoinSelection coin_select;
    selected_coins = coin_select.SelectCoins(
        target_amount, utxo_list, utxo_filter, option, fee, &txin_total_amount,
        nullptr, nullptr);
    txin_total_amount += txin_amount;
    info(
        CFD_LOG_SOURCE, "FundRawTransaction: txin_total_amount=[{}]",
        txin_total_amount.GetSatoshiValue());
    if (txin_total_amount < target_amount) {
      warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. low BTC.");
      throw CfdException(CfdError::kCfdIllegalArgumentError, "low BTC.");
    }
  }

  // dust_amountを計算
  Amount dust_amount = option.GetDustFeeAmount(reserve_address);
  info(
      CFD_LOG_SOURCE, "FundRawTransaction: dust_amount=[{}]",
      dust_amount.GetSatoshiValue());

  // txoutへの追加
  std::vector<uint8_t> txid_bytes(cfd::core::kByteData256Length);
  Amount new_txout_amount =
      txin_total_amount -
      txout_amount;  // diff_amount: selected + txin - txout = new txout amount
  int64_t new_txout_satoshi = new_txout_amount.GetSatoshiValue();
  // feeを考慮する場合、追加後のTxInが既存のTxOut額以上ある場合、TxOutが追加されるのでfeeを再計算
  if (option.GetEffectiveFeeBaserate() > 0) {
    Amount need_amount = txout_amount + fee;
    Amount check_amount = txin_total_amount - dust_amount;
    if (check_amount > need_amount) {
      // dummyのtx作成
      TransactionController txc_dummy(tx_hex);
      std::vector<UtxoData> new_selected_utxos = matched_txin_utxos;
      Txid txid;
      for (const Utxo& coin : selected_coins) {
        memcpy(txid_bytes.data(), coin.txid, txid_bytes.size());
        txid = Txid(ByteData256(txid_bytes));
        for (const UtxoData& utxo : utxodata_list) {
          if (txid.Equals(utxo.txid) && (coin.vout == utxo.vout)) {
            new_selected_utxos.push_back(utxo);
            break;
          }
        }
      }
      // ダミーへの追加のため額はfeeで代替
      txc_dummy.AddTxOut(reserve_address, fee);
      fee = EstimateFee(
          txc_dummy.GetHex(), new_selected_utxos, nullptr, nullptr,
          effective_fee_rate);
      info(
          CFD_LOG_SOURCE, "FundRawTransaction: new_fee={}",
          fee.GetSatoshiValue());
    }

    new_txout_amount -= fee;  // txoutの追加額からfee分を除外
    new_txout_satoshi = new_txout_amount.GetSatoshiValue();

    if (txin_total_amount < new_txout_amount) {
      warn(CFD_LOG_SOURCE, "Failed to FundRawTransaction. low fee.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to FundRawTransaction. low fee.");
    }

    // optionで、超過額の設定がある場合、余剰分をfeeに設定。
    if (dust_amount > new_txout_amount) {
      // feeに残高をすべて設定
      new_txout_satoshi = 0;
      fee = txin_total_amount - new_txout_amount;
    }
    info(
        CFD_LOG_SOURCE, "FundRawTransaction: new_txout_amount={}",
        new_txout_satoshi);
  }

  // dustより小さい場合はTxOutには追加しない
  // (fee計算ありの場合はチェック済だが、fee計算なしの場合は未チェックのため)
  if ((new_txout_satoshi != 0) && (dust_amount < new_txout_amount)) {
    txc.AddTxOut(reserve_address, new_txout_amount);
    info(
        CFD_LOG_SOURCE, "FundRawTransaction:AddTxOut: value=[{}]",
        diff_amount.GetSatoshiValue());
    if (append_txout_addresses) {
      append_txout_addresses->push_back(reserve_txout_address);
    }
  }
  if (estimate_fee) *estimate_fee = fee;

  // SelectしたUTXOをTxInに設定
  for (auto& utxo : selected_coins) {
    memcpy(txid_bytes.data(), utxo.txid, txid_bytes.size());
    txc.AddTxIn(Txid(ByteData256(txid_bytes)), utxo.vout);
  }

  return txc;
}

std::string TransactionApi::CreateMultisigScriptSig(
    const std::vector<SignParameter>& sign_list, const Script& redeem_script) {
  return TransactionApiBase::CreateMultisigScriptSig(sign_list, redeem_script);
}

}  // namespace api
}  // namespace cfd
