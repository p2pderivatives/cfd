// Copyright 2019 CryptoGarage
/**
 * @file dlcapi_transaction.cpp
 *
 * @brief dlc-apiで利用するTransaction操作の実装ファイル
 */

#include <limits>
#include <string>
#include <vector>

#include "cfd/cfd_transaction.h"
#include "cfd/dlc_script.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_util.h"

#include "cfd/cfdapi_struct.h"
#include "cfd/cfdapi_transaction_base.h"
#include "cfd/dlcapi_transaction.h"
#include "dlcapi_internal.h"  // NOLINT

namespace dlc {
namespace js {
namespace api {

using cfd::TransactionController;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::CryptoUtil;
using cfd::core::Script;
using cfd::core::SigHashType;
using cfd::core::Txid;
using cfd::core::logger::warn;
using cfd::js::api::TransactionApiBase;
using dlc::DlcScriptUtil;

AddCETxSignResponseStruct DlcTransactionStructApi::AddCETxSign(
    const AddCETxSignRequestStruct& request) {
  auto call_func = [](const AddCETxSignRequestStruct& request)
      -> AddCETxSignResponseStruct {
    AddCETxSignResponseStruct response;
    const std::string& hex_string = request.tx;
    if (hex_string.empty()) {
      warn(CFD_LOG_SOURCE, "Failed to AddCETxSignRequest. hex empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty data.");
    }

    if (request.txin.sign.hex.empty()) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddCETxSignRequest. sign hex empty.");  // NOLINT
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty sign hex.");
    }

    if (request.txin.redeem_script.empty()) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to AddCETxSignRequest. redeem script empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Invalid hex string. empty redeem script.");
    }

    // TransactionController作成
    TransactionController txc(hex_string);

    ByteData sign_data;
    if (request.txin.sign.der_encode) {
      SigHashType sighashtype = TransactionApiBase::ConvertSigHashType(
          request.txin.sign.sighash_type,
          request.txin.sign.sighash_anyone_can_pay);
      sign_data = CryptoUtil::ConvertSignatureToDer(
          request.txin.sign.hex, sighashtype);
    } else {
      sign_data = ByteData(request.txin.sign.hex);
    }
    std::vector<uint8_t> delayed_unlocking;
    if (!request.txin.delayed_unlocking) {
      delayed_unlocking.push_back(1);
    }
    ByteData check_op_if_data(delayed_unlocking);
    Script redeem_script(request.txin.redeem_script);

    std::vector<ByteData> witness_datas(3);
    witness_datas[0] = sign_data;
    witness_datas[1] = check_op_if_data;
    witness_datas[2] = redeem_script.GetData();
    txc.AddWitnessStack(
        Txid(request.txin.txid), request.txin.vout, witness_datas);

    response.hex = txc.GetHex();
    return response;
  };

  AddCETxSignResponseStruct result;
  result =
      ExecuteStructApi<AddCETxSignRequestStruct, AddCETxSignResponseStruct>(
          request, call_func, std::string(__FUNCTION__));
  return result;
}

}  // namespace api
}  // namespace js
}  // namespace dlc
