// Copyright 2019 CryptoGarage
/**
 * @file dlc_script.h
 *
 * @brief dlc向けScript生成Utilクラス定義
 */
#ifndef CFD_INCLUDE_CFD_DLC_SCRIPT_H_
#define CFD_INCLUDE_CFD_DLC_SCRIPT_H_

#include "cfd/cfd_common.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_script.h"

/**
 * @brief dlc名前空間
 */
namespace dlc {

using cfd::core::Pubkey;
using cfd::core::Script;

/**
 * @brief DLCの取引に用いるScriptを生成するUtilクラス
 */
class CFD_EXPORT DlcScriptUtil {
 public:
  /**
   * @brief CETxのlocking scriptを作成する.
   * @param[in] pubkey_a     送金先Pubkey
   * @param[in] message_key  Oracle Message public key
   * @param[in] delay        delay値
   * @param[in] pubkey_b     送金元Pubkey
   * @return Scriptインスタンス
   * @details 下記の内容のScriptを作成する.
   * @code{.unparse}
   * OP_IF
   *     <pubkey_a + message_key>
   * OP_ELSE
   *     delay(fix <delay>)
   *     OP_CHECKSEQUENCEVERIFY
   *     OP_DROP
   *     <pubkey_b>
   * OP_ENDIF
   * OP_CHECKSIG
   * @endcode
   */
  static Script CreateCETxRedeemScript(
      const Pubkey& pubkey_a, const Pubkey& message_key, int64_t delay,
      const Pubkey& pubkey_b);

 private:
  DlcScriptUtil();
};

}  // namespace dlc

#endif  // CFD_INCLUDE_CFD_DLC_SCRIPT_H_
