// Copyright 2019 CryptoGarage
/**
 * @file cfd_transaction.cpp
 *
 * @brief-eng implementation of common classes related to transaction operation
 * @brief-jp Transaction操作共通の関連クラスの実装ファイル
 */
#include "cfd/cfd_transaction_common.h"

#include <algorithm>
#include <string>
#include <vector>

#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"

namespace cfd {

using cfd::core::AbstractTransaction;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
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

SignParameter::SignParameter(const std::string& text_message)
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
