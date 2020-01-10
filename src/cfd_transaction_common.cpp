// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction.cpp
 *
 * @brief \~english implementation of common classes related to transaction operation
 *   \~japanese Transaction操作共通の関連クラスの実装ファイル
 */
#include <algorithm>
#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfd_utxo.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_descriptor.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressFormatData;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
using cfd::core::Descriptor;
using cfd::core::DescriptorScriptReference;
using cfd::core::HashType;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;
using cfd::core::Txid;
using cfd::core::logger::warn;

// -----------------------------------------------------------------------------
// Define
// -----------------------------------------------------------------------------
/// シーケンス値(locktime有効)
constexpr uint32_t kSequenceEnableLockTimeMax = 0xfffffffeU;
/// シーケンス値(locktime無効)
constexpr uint32_t kSequenceDisableLockTime = 0xffffffffU;

// -----------------------------------------------------------------------------
// UtxoUtil
// -----------------------------------------------------------------------------
std::vector<Utxo> UtxoUtil::ConvertToUtxo(const std::vector<UtxoData>& utxos) {
  std::vector<Utxo> result;
  result.resize(utxos.size());
  std::vector<Utxo>::iterator ite = result.begin();
  for (const auto& utxo_data : utxos) {
    ConvertToUtxo(utxo_data, &(*ite));
    ++ite;
  }
  return result;
}

void UtxoUtil::ConvertToUtxo(
    const UtxoData& utxo_data, Utxo* utxo, UtxoData* dest) {
  if (utxo != nullptr) {
    UtxoData output = utxo_data;
    memset(utxo, 0, sizeof(Utxo));
    utxo->block_height = utxo_data.block_height;
    utxo->vout = utxo_data.vout;
    utxo->binary_data = utxo_data.binary_data;
    utxo->amount = utxo_data.amount.GetSatoshiValue();

    ByteData block_hash = utxo_data.block_hash.GetData();
    if (!block_hash.Empty()) {
      memcpy(
          utxo->block_hash, block_hash.GetBytes().data(),
          sizeof(utxo->block_hash));
    }
    ByteData txid = utxo_data.txid.GetData();
    if (!txid.Empty()) {
      memcpy(utxo->txid, txid.GetBytes().data(), sizeof(utxo->txid));
    }

    // convert from descriptor
    std::vector<uint8_t> locking_script_bytes;
    if (!utxo_data.descriptor.empty()) {
      NetType net_type = NetType::kMainnet;
      std::vector<AddressFormatData> addr_prefixes =
          cfd::core::GetBitcoinAddressFormatList();
#ifndef CFD_DISABLE_ELEMENTS
      if (!utxo_data.asset.IsEmpty()) {
        std::vector<AddressFormatData> elements_prefixes =
            cfd::core::GetElementsAddressFormatList();
        addr_prefixes = elements_prefixes;
      }
#endif  // CFD_DISABLE_ELEMENTS
      if (!utxo_data.address.GetAddress().empty()) {
        addr_prefixes.clear();
        addr_prefixes.push_back(utxo_data.address.GetAddressFormatData());
        net_type = utxo_data.address.GetNetType();
      }

      Descriptor desc =
          Descriptor::Parse(utxo_data.descriptor, &addr_prefixes);
      if (desc.GetNeedArgumentNum() == 0) {
        DescriptorScriptReference script_ref = desc.GetReference();
        output.locking_script = script_ref.GetLockingScript();
        locking_script_bytes = output.locking_script.GetData().GetBytes();
        output.address_type = script_ref.GetAddressType();
        utxo->address_type = static_cast<uint16_t>(output.address_type);
        output.address = script_ref.GenerateAddress(net_type);
      }
    }

    if (!locking_script_bytes.empty()) {
      // do nothing
    } else if (!utxo_data.address.GetAddress().empty()) {
      output.locking_script = utxo_data.address.GetLockingScript();
      locking_script_bytes = output.locking_script.GetData().GetBytes();
      AddressType addr_type = utxo_data.address.GetAddressType();
      if ((addr_type == AddressType::kP2shAddress) &&
          ((utxo_data.address_type == AddressType::kP2shP2wshAddress) ||
           (utxo_data.address_type == AddressType::kP2shP2wpkhAddress))) {
        // direct set. output.address_type;
      } else {
        output.address_type = addr_type;
      }
      utxo->address_type = static_cast<uint16_t>(output.address_type);
    } else if (!utxo_data.locking_script.IsEmpty()) {
      locking_script_bytes = utxo_data.locking_script.GetData().GetBytes();
      if (utxo_data.locking_script.IsP2wpkhScript()) {
        utxo->address_type = AddressType::kP2wpkhAddress;
      } else if (utxo_data.locking_script.IsP2wshScript()) {
        utxo->address_type = AddressType::kP2wshAddress;
      } else if (utxo_data.locking_script.IsP2pkhScript()) {
        utxo->address_type = AddressType::kP2pkhAddress;
      } else {  // TODO(k-matsuzawa): unbknown type is convert to p2sh
        utxo->address_type = AddressType::kP2shAddress;
      }
      output.address_type = static_cast<AddressType>(utxo->address_type);
    }

    if (!locking_script_bytes.empty()) {
      utxo->script_length = static_cast<uint16_t>(locking_script_bytes.size());
      if (utxo->script_length < sizeof(utxo->locking_script)) {
        memcpy(
            utxo->locking_script, locking_script_bytes.data(),
            utxo->script_length);
      }
    }

    switch (utxo->address_type) {
      case AddressType::kP2wpkhAddress:
        utxo->witness_size_max = 71 + 33 + 2;
        // fall-through
      case AddressType::kP2shP2wpkhAddress:
        utxo->uscript_size_max = 20 + 2;
        break;
      case AddressType::kP2wshAddress:
        utxo->witness_size_max = 71 + utxo->script_length + 2;
        // fall-through
      case AddressType::kP2shP2wshAddress:
        utxo->uscript_size_max = 32 + 2;
        break;
      case AddressType::kP2pkhAddress:
        utxo->uscript_size_max = 71 + 33 + 3;
        break;
      case AddressType::kP2shAddress:
      default:
        utxo->uscript_size_max = 71 + utxo->script_length + 3;
        break;
    }

#ifndef CFD_DISABLE_ELEMENTS
    if (!utxo_data.asset.IsEmpty()) {
      std::vector<uint8_t> asset = utxo_data.asset.GetData().GetBytes();
      memcpy(utxo->asset, asset.data(), sizeof(utxo->asset));
      // utxo->blinded = false;
    }
#endif  // CFD_DISABLE_ELEMENTS

    if (dest != nullptr) {
      *dest = output;
    }
  }
}

// -----------------------------------------------------------------------------
// SignParameter
// -----------------------------------------------------------------------------

SignParameter::SignParameter()
    : data_(),
      data_type_(SignDataType::kBinary),
      related_pubkey_(),
      der_encode_(false),
      sighash_type_(),
      op_code_(ScriptOperator::OP_INVALIDOPCODE) {
  // do nothing
}

SignParameter::SignParameter(
    const std::string& text_message, bool der_encode,
    const SigHashType sighash_type)
    : data_(),
      data_type_(SignDataType::kBinary),
      related_pubkey_(),
      der_encode_(false),
      sighash_type_(),
      op_code_(ScriptOperator::OP_INVALIDOPCODE) {
  if (ScriptOperator::IsValid(text_message)) {
    data_type_ = SignDataType::kOpCode;
    op_code_ = ScriptOperator::Get(text_message);
    std::vector<uint8_t> list(1);
    list[0] = static_cast<uint8_t>(op_code_.GetDataType());
    data_ = ByteData(list);
  } else {
    data_ = ByteData(text_message);
    der_encode_ = der_encode;
    sighash_type_ = sighash_type;
    if (der_encode) data_type_ = SignDataType::kSign;
  }
}

SignParameter::SignParameter(
    const ByteData& data, bool der_encode, const SigHashType sighash_type)
    : data_(data),
      data_type_(SignDataType::kSign),
      related_pubkey_(),
      der_encode_(der_encode),
      sighash_type_(sighash_type),
      op_code_(ScriptOperator::OP_INVALIDOPCODE) {
  // do nothing
}

SignParameter::SignParameter(const ByteData& data)
    : data_(data),
      data_type_(SignDataType::kBinary),
      related_pubkey_(),
      der_encode_(false),
      sighash_type_(),
      op_code_(ScriptOperator::OP_INVALIDOPCODE) {
  // do nothing
}

SignParameter::SignParameter(const Pubkey& pubkey)
    : data_(pubkey.GetData()),
      data_type_(SignDataType::kPubkey),
      related_pubkey_(),
      der_encode_(false),
      sighash_type_(),
      op_code_(ScriptOperator::OP_INVALIDOPCODE) {
  // do nothing
}

SignParameter::SignParameter(const Script& redeem_script)
    : data_(redeem_script.GetData()),
      data_type_(SignDataType::kRedeemScript),
      related_pubkey_(),
      der_encode_(false),
      sighash_type_(),
      op_code_(ScriptOperator::OP_INVALIDOPCODE) {
  // do nothing
}

SignParameter::SignParameter(const ScriptOperator& op_code)
    : data_(),
      data_type_(SignDataType::kOpCode),
      related_pubkey_(),
      der_encode_(false),
      sighash_type_(),
      op_code_(op_code) {
  std::vector<uint8_t> list(1);
  list[0] = static_cast<uint8_t>(op_code_.GetDataType());
  data_ = ByteData(list);
}

SignParameter& SignParameter::operator=(const SignParameter& sign_parameter) {
  data_ = sign_parameter.GetData();
  data_type_ = sign_parameter.GetDataType();
  related_pubkey_ = sign_parameter.GetRelatedPubkey();
  der_encode_ = sign_parameter.IsDerEncode();
  sighash_type_ = sign_parameter.GetSigHashType();
  return *this;
}

void SignParameter::SetRelatedPubkey(const Pubkey& pubkey) {
  if (pubkey.IsValid()) {
    related_pubkey_ = pubkey;
  }
}

ScriptOperator SignParameter::GetOpCode() const { return op_code_; }

bool SignParameter::IsOpCode() const {
  return data_type_ == SignDataType::kOpCode;
}

ByteData SignParameter::GetData() const { return data_; }

SignDataType SignParameter::GetDataType() const { return data_type_; }

Pubkey SignParameter::GetRelatedPubkey() const { return related_pubkey_; }

bool SignParameter::IsDerEncode() const { return der_encode_; }

SigHashType SignParameter::GetSigHashType() const { return sighash_type_; }

ByteData SignParameter::ConvertToSignature() const {
  ByteData byte_data;
  if (der_encode_) {
    if (data_.Empty()) {
      warn(CFD_LOG_SOURCE, "Failed to ConvertToSignature. sign hex empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty sign data.");
    }
    byte_data =
        CryptoUtil::ConvertSignatureToDer(data_.GetHex(), sighash_type_);
  } else {
    byte_data = data_;
  }
  return byte_data;
}

// -----------------------------------------------------------------------------
// TransactionController
// -----------------------------------------------------------------------------
AbstractTransactionController::AbstractTransactionController()
    : tx_address_(nullptr) {
  // do nothing
}

std::string AbstractTransactionController::GetHex() const {
  return tx_address_->GetHex();
}

uint32_t AbstractTransactionController::GetLockTimeDisabledSequence() {
  return kSequenceDisableLockTime;
}

uint32_t AbstractTransactionController::GetDefaultSequence() const {
  if (tx_address_->GetLockTime() == 0) {
    return kSequenceDisableLockTime;
  } else {
    return kSequenceEnableLockTimeMax;
  }
}
}  // namespace cfd
