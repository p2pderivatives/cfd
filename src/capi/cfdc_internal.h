// Copyright 2019 CryptoGarage
/**
 * @file cfdc_internal.h
 *
 * @brief This file is cfd-capi internal definition.
 *
 */
#ifndef CFD_SRC_CAPI_CFDC_INTERNAL_H_
#define CFD_SRC_CAPI_CFDC_INTERNAL_H_

#include <exception>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <vector>

#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_script.h"

using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::NetType;
using cfd::core::Txid;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialNonce;
using cfd::core::ConfidentialValue;
#endif  // CFD_DISABLE_ELEMENTS

/**
 * @brief cfd namespace
 */
namespace cfd {
/**
 * @brief capi namespace
 */
namespace capi {

using cfd::core::CfdException;
using cfd::core::Script;

//! empty 32byte data
constexpr const char* const kEmpty32Bytes =
    "0000000000000000000000000000000000000000000000000000000000000000";

//! blinding minimum bits (current is old value)
constexpr uint32_t kMinimumBits = 36;

//! prefix size
constexpr const uint32_t kPrefixLength = 16;
//! prefix: handle data
constexpr const char* const kPrefixHandleData = "CapiHandleData";

/**
 * @brief template structure of cfd-capi structure prefix definition.
 */
struct CfdCapiPrefixTemplate {
  char prefix[kPrefixLength];  //!< buffer prefix
};

//! asset max size
constexpr uint32_t kAssetSize = 33;

//! prefix: MultisigSign
constexpr const char* const kPrefixMultisigSignData = "MultisigSign";
//! multisig max key num
constexpr const uint32_t kMultisigMaxKeyNum = Script::kMaxMultisigPubkeyNum;
//! signature hex size (73 * 2)
constexpr const uint32_t kSignatureHexSize = 146;
//! pubkey hex size (cfd::core::Pubkey::kPubkeySize * 2)
constexpr const uint32_t kPubkeyHexSize = 130;
/**
 * @brief cfd-capi MultisigScript structure.
 */
struct CfdCapiMultisigSignData {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! signatures
  char signatures[kMultisigMaxKeyNum][kSignatureHexSize + 1];
  //! pubkeys
  char pubkeys[kMultisigMaxKeyNum][kPubkeyHexSize + 1];
  uint32_t current_index;  //!< current index
};

//! prefix: TxData
constexpr const char* const kPrefixTransactionData = "TransactionData";

/**
 * @brief cfd-capi TransactionData structure.
 */
struct CfdCapiTransactionData {
  char prefix[kPrefixLength];  //!< buffer prefix
  int net_type;                //!< network type
  void* tx_obj;                //!< transaction object
};

/**
 * @brief allocate buffer.
 * @param[in] prefix  prefix string (max: 15 char)
 * @param[in] size    allocate size
 * @return buffer pointer
 */
CFDC_API void* AllocBuffer(const std::string& prefix, uint32_t size);

/**
 * @brief free buffer.
 * @param[in] address  buffer address.
 * @param[in] prefix   prefix string (max: 15 char)
 * @param[in] size     struct size
 */
CFDC_API void FreeBuffer(
    void* address, const std::string& prefix, uint32_t size);

/**
 * @brief check buffer.
 * @param[in] address  buffer address.
 * @param[in] prefix   prefix string (max: 15 char)
 */
CFDC_API void CheckBuffer(void* address, const std::string& prefix);

/**
 * @brief convert from net type.
 * @param[in] network_type  network type.
 * @return CfdNetworkType
 */
CFDC_API int ConvertFromCfdNetType(cfd::core::NetType network_type);

/**
 * @brief convert to net type.
 * @param[in] network_type  network type.
 * @param[out] is_bitcoin   bitcoin or other.
 * @return NetType
 */
CFDC_API cfd::core::NetType ConvertNetType(int network_type, bool* is_bitcoin);

/**
 * @brief check elements net type.
 * @param[in] network_type  network type.
 * @retval true   elements NetType
 * @retval false  other NetType
 */
CFDC_API bool IsElementsNetType(int network_type);

/**
 * @brief convert to address type.
 * @param[in] hash_type  hash type.
 * @return AddressType
 */
CFDC_API cfd::core::AddressType ConvertHashToAddressType(int hash_type);

/**
 * @brief convert to address type.
 * @param[in] address_type  address type.
 * @return AddressType
 */
CFDC_API cfd::core::AddressType ConvertAddressType(int address_type);

/**
 * @brief get witness version.
 * @param[in] hash_type  hash type.
 * @return WitnessVersion
 */
CFDC_API cfd::core::WitnessVersion GetWitnessVersion(int hash_type);

/**
 * @brief is empty string.
 * @param[in] message  string text.
 * @retval true   empty string
 * @retval false  exist string
 */
CFDC_API bool IsEmptyString(const char* message);

/**
 * @brief Check empty string and throw exception.
 * @param[in] parameter   string text.
 * @param[in] name        parameter name.
 * @param[in] location    caller location.
 */
CFDC_API void CheckEmptyString(
    const char* parameter, const char* name,
    const cfd::core::logger::CfdSourceLocation& location);

/**
 * @brief Check null and throw exception.
 * @param[in] parameter   string text.
 * @param[in] name        parameter name.
 * @param[in] location    caller location.
 */
CFDC_API void CheckNull(
    const char* parameter, const char* name,
    const cfd::core::logger::CfdSourceLocation& location);

/**
 * @brief create string.
 * @param[in] message  message string.
 * @return string buffer
 */
CFDC_API char* CreateString(const std::string& message);

/**
 * @brief free heap buffer on error.
 * @param[in] pointer1  free address.
 * @param[in] pointer2  free address.
 * @param[in] pointer3  free address.
 * @param[in] pointer4  free address.
 * @param[in] pointer5  free address.
 * @param[in] pointer6  free address.
 */
CFDC_API void FreeBufferOnError(
    char** pointer1, char** pointer2 = nullptr, char** pointer3 = nullptr,
    char** pointer4 = nullptr, char** pointer5 = nullptr,
    char** pointer6 = nullptr);

/**
 * @brief cfd-capiハンドル情報構造体.
 */
struct CfdCapiHandleData {
  char prefix[kPrefixLength];  //!< buffer prefix
  int32_t error_code;          //!< error code
  bool is_outside;             //!< outside handle
  char error_message[256];     //!< error message
};

/**
 * @brief エラー情報を設定する。
 * @param[in] handle      ハンドル情報
 * @param[in] error_code  エラーコード
 * @param[in] message     エラーメッセージ
 * @return error code
 */
CFDC_API int SetLastError(void* handle, int error_code, const char* message);

/**
 * @brief 例外エラー情報を設定する。
 * @param[in] handle    ハンドル情報
 * @param[in] message   エラーメッセージ
 * @return error code
 */
CFDC_API int SetLastFatalError(void* handle, const char* message);

/**
 * @brief エラー情報を設定する。
 * @param[in] handle     ハンドル情報
 * @param[in] exception  CFD例外オブジェクト
 * @return error code
 */
CFDC_API int SetLastError(void* handle, const CfdException& exception);

/**
 * @brief 例外エラー情報を設定する。
 * @param[in] handle     ハンドル情報
 * @param[in] exception  CFD例外オブジェクト
 */
CFDC_API void SetLastFatalError(void* handle, const std::exception& exception);

/**
 * @brief cfd-capi管理クラス。
 */
class CfdCapiManager {
 public:
  /**
   * @brief コンストラクタ。
   */
  CfdCapiManager();
  /**
   * @brief デストラクタ。
   */
  virtual ~CfdCapiManager() { FreeAllList(&handle_list_); }

  /**
   * @brief ハンドルを作成する。
   * @param[in] is_outside  create outside handle
   * @return handle
   */
  void* CreateHandle(bool is_outside = false);
  /**
   * @brief ハンドルを解放する。
   * @param[in] handle      ハンドル情報
   */
  void FreeHandle(void* handle);
  /**
   * @brief copy handle.
   * @param[in] source            source handle
   * @param[out] destination      destinate handle
   * @param[in] error_state_only  copy is error state only.
   */
  void CopyHandle(
      void* source, void* destination, bool error_state_only = true) const;
  /**
   * @brief エラー情報を設定する。
   * @param[in] handle      ハンドル情報
   * @param[in] error_code  エラーコード
   * @param[in] message     エラーメッセージ
   */
  void SetLastError(void* handle, int error_code, const char* message);
  /**
   * @brief 例外エラー情報を設定する。
   * @param[in] handle    ハンドル情報
   * @param[in] message   エラーメッセージ
   */
  void SetLastFatalError(void* handle, const char* message);
  /**
   * @brief エラー情報を取得する。
   * @param[in] handle      ハンドル情報
   * @return 例外情報
   */
  CfdException GetLastError(void* handle);

 protected:
  std::vector<CfdCapiHandleData*> handle_list_;  ///< ハンドル一覧
  std::mutex mutex_;  ///< 排他制御用オブジェクト

  /**
   * @brief ハンドル一覧の解放を行う。
   * @param[in,out] list  handle list
   */
  static void FreeAllList(std::vector<CfdCapiHandleData*>* list);
};

}  // namespace capi
}  // namespace cfd

#endif  // CFD_SRC_CAPI_CFDC_INTERNAL_H_
