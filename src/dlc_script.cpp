// Copyright 2019 CryptoGarage
/**
 * @file dlc_script.h
 *
 * @brief-eng implementation of the script generation util class for dlc
 * @brief-jp dlc向けScript生成Utilクラス実装
 */
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"

#include "cfd/dlc_script.h"

namespace dlc {

using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptOperator;
using cfd::core::ScriptUtil;
using cfd::core::logger::warn;

Script DlcScriptUtil::CreateCETxRedeemScript(
    const Pubkey& pubkey_a, const Pubkey& message_key, int64_t delay,
    const Pubkey& pubkey_b) {
  // 合成Pubkey
  Pubkey combine_pubkey = Pubkey::CombinePubkey(pubkey_a, message_key);

  // script作成
  ScriptBuilder builder;
  //  builder.AppendOperator(ScriptOperator::OP_1);
  //  builder.AppendOperator(ScriptOperator::OP_EQUAL);
  builder.AppendOperator(ScriptOperator::OP_IF);
  builder.AppendData(combine_pubkey);
  builder.AppendOperator(ScriptOperator::OP_ELSE);
  builder.AppendData(delay);
  builder.AppendOperator(ScriptOperator::OP_CHECKSEQUENCEVERIFY);
  builder.AppendOperator(ScriptOperator::OP_DROP);
  builder.AppendData(pubkey_b);
  builder.AppendOperator(ScriptOperator::OP_ENDIF);
  builder.AppendOperator(ScriptOperator::OP_CHECKSIG);
  Script script = builder.Build();

  if (!ScriptUtil::IsValidRedeemScript(script)) {
    warn(CFD_LOG_SOURCE, "CETxLockingScript script size is over.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "CETxLockingScript size is over.");
  }
  return script;
}

}  // namespace dlc
