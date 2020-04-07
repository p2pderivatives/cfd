// Copyright 2020 CryptoGarage
/**
 * @file cfdapi_ledger.h
 *
 * @brief convert for ledger format api.
 */
#ifndef CFD_INCLUDE_CFD_CFDAPI_LEDGER_H_
#define CFD_INCLUDE_CFD_CFDAPI_LEDGER_H_

#include <string>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_transaction.h"
#include "cfdcore/cfdcore_bytedata.h"

namespace cfd {
namespace api {

using cfd::core::ByteData;

/**
 * @brief Ledger MetaData Stack struct information.
 */
struct LedgerMetaDataStackItem {
  std::string metadata1;  //!< metadata
  std::string metadata2;  //!< metadata
  std::string metadata3;  //!< metadata
};

/**
 * @brief convert api class for ledger.
 */
class CFD_EXPORT LedgerApi {
 public:
  /**
   * @brief constructor
   */
  LedgerApi() {}

#ifndef CFD_DISABLE_ELEMENTS
  /**
   * @brief serialize ledger format (confidential transaction).
   * @param[in] tx                confidential transaction.
   * @param[in] metadata_stack    metadata stack.
   * @param[in] skip_witness      skip output witness area.
   * @param[in] is_authorization  authorization mode.
   * @return serialize data.
   */
  ByteData Serialize(
      const ConfidentialTransactionContext& tx,
      const std::vector<LedgerMetaDataStackItem>& metadata_stack,
      bool skip_witness, bool is_authorization) const;
#endif  // CFD_DISABLE_ELEMENTS
};

}  // namespace api
}  // namespace cfd

#endif  // CFD_INCLUDE_CFD_CFDAPI_LEDGER_H_
