// Copyright 2021 CryptoGarage
/**
 * @file cfdcapi_block.cpp
 *
 * @brief implements block API on cfd-capi.
 */
#ifndef CFD_DISABLE_CAPI
#include "cfdc/cfdcapi_block.h"

#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_block.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::core::Block;
using cfd::core::BlockHash;
using cfd::core::BlockHeader;
using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Txid;

using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: data for block
constexpr const char* const kPrefixBlock = "BlockOpe";

/**
 * @brief cfd-capi CfdCapiBlockData struct.
 */
struct CfdCapiBlockData {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! network type
  NetType net_type;
  //! block
  Block* block;
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
// API
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiBlockData;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertNetType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kPrefixBlock;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

extern "C" {

int CfdInitializeBlockHandle(
    void* handle, int network_type, const char* block_hex,
    void** block_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiBlockData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (block_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "block_handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. block_handle is null.");
    }
    if (IsEmptyString(block_hex)) {
      warn(CFD_LOG_SOURCE, "block_hex is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. block_hex is null or empty.");
    }

    buffer = static_cast<CfdCapiBlockData*>(
        AllocBuffer(kPrefixBlock, sizeof(CfdCapiBlockData)));

    bool is_bitcoin = false;
    buffer->net_type = ConvertNetType(network_type, &is_bitcoin);
    if (is_bitcoin) {
      buffer->block = new Block(std::string(block_hex));
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    *block_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  if (buffer != nullptr) CfdFreeBlockHandle(handle, buffer);
  return result;
}

int CfdFreeBlockHandle(void* handle, void* block_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (block_handle != nullptr) {
      CheckBuffer(block_handle, kPrefixBlock);
      CfdCapiBlockData* data = static_cast<CfdCapiBlockData*>(block_handle);
      if (data->block != nullptr) {
        delete data->block;
        data->block = nullptr;
      }
    }
    FreeBuffer(block_handle, kPrefixBlock, sizeof(CfdCapiBlockData));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdGetBlockHash(void* handle, void* block_handle, char** block_hash) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(block_handle, kPrefixBlock);
    CfdCapiBlockData* block_data =
        static_cast<CfdCapiBlockData*>(block_handle);
    if (block_hash == nullptr) {
      warn(CFD_LOG_SOURCE, "block_hash is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. block_hash is null.");
    }

    if (block_data->block == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid handle state. block is null");
    } else if (block_data->net_type <= NetType::kRegtest) {
      *block_hash = CreateString(block_data->block->GetBlockHash().GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdGetBlockHeaderData(
    void* handle, void* block_handle, uint32_t* version,
    char** prev_block_hash, char** merkle_root_hash, uint32_t* time,
    uint32_t* bits, uint32_t* nonce) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_prev_block_hash = nullptr;
  char* work_merkle_root_hash = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(block_handle, kPrefixBlock);
    CfdCapiBlockData* block_data =
        static_cast<CfdCapiBlockData*>(block_handle);

    BlockHeader header;
    if (block_data->block == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid handle state. block is null");
    } else if (block_data->net_type <= NetType::kRegtest) {
      header = block_data->block->GetBlockHeader();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (prev_block_hash != nullptr) {
      work_prev_block_hash = CreateString(header.prev_block_hash.GetHex());
    }
    if (merkle_root_hash != nullptr) {
      work_merkle_root_hash = CreateString(header.merkle_root_hash.GetHex());
    }

    if (version != nullptr) *version = header.version;
    if (prev_block_hash != nullptr) {
      *prev_block_hash = work_prev_block_hash;
    }
    if (merkle_root_hash != nullptr) {
      *merkle_root_hash = work_merkle_root_hash;
    }
    if (time != nullptr) *time = header.time;
    if (bits != nullptr) *bits = header.bits;
    if (nonce != nullptr) *nonce = header.nonce;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_prev_block_hash, &work_merkle_root_hash);
  return result;
}

int CfdGetTransactionFromBlock(
    void* handle, void* block_handle, const char* txid, char** tx_hex) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(block_handle, kPrefixBlock);
    CfdCapiBlockData* block_data =
        static_cast<CfdCapiBlockData*>(block_handle);
    if (tx_hex == nullptr) {
      warn(CFD_LOG_SOURCE, "tx_hex is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx_hex is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }

    if (block_data->block == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid handle state. block is null");
    } else if (block_data->net_type <= NetType::kRegtest) {
      auto tx = block_data->block->GetTransaction(Txid(std::string(txid)));
      *tx_hex = CreateString(tx.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdGetTxOutProof(
    void* handle, void* block_handle, const char* txid, char** txout_proof) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(block_handle, kPrefixBlock);
    CfdCapiBlockData* block_data =
        static_cast<CfdCapiBlockData*>(block_handle);
    if (txout_proof == nullptr) {
      warn(CFD_LOG_SOURCE, "txout_proof is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txout_proof is null.");
    }
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }

    if (block_data->block == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid handle state. block is null");
    } else if (block_data->net_type <= NetType::kRegtest) {
      auto proof = block_data->block->GetTxOutProof(Txid(txid));
      *txout_proof = CreateString(proof.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdExistTxidInBlock(void* handle, void* block_handle, const char* txid) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(block_handle, kPrefixBlock);
    CfdCapiBlockData* block_data =
        static_cast<CfdCapiBlockData*>(block_handle);
    if (IsEmptyString(txid)) {
      warn(CFD_LOG_SOURCE, "txid is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null or empty.");
    }

    bool has_txid = false;
    if (block_data->block == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid handle state. block is null");
    } else if (block_data->net_type <= NetType::kRegtest) {
      has_txid = block_data->block->ExistTxid(Txid(std::string(txid)));
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    if (!has_txid) return CfdErrorCode::kCfdNotFoundError;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdGetTxCountInBlock(
    void* handle, void* block_handle, uint32_t* tx_count) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(block_handle, kPrefixBlock);
    CfdCapiBlockData* block_data =
        static_cast<CfdCapiBlockData*>(block_handle);
    if (tx_count == nullptr) {
      warn(CFD_LOG_SOURCE, "tx_count is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx_count is null.");
    }

    if (block_data->block == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid handle state. block is null");
    } else if (block_data->net_type <= NetType::kRegtest) {
      *tx_count = block_data->block->GetTransactionCount();
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdGetTxidFromBlock(
    void* handle, void* block_handle, uint32_t index, char** txid) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(block_handle, kPrefixBlock);
    CfdCapiBlockData* block_data =
        static_cast<CfdCapiBlockData*>(block_handle);
    if (txid == nullptr) {
      warn(CFD_LOG_SOURCE, "txid is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. txid is null.");
    }

    if (block_data->block == nullptr) {
      throw CfdException(
          CfdError::kCfdIllegalStateError,
          "Invalid handle state. block is null");
    } else if (block_data->net_type <= NetType::kRegtest) {
      auto txid_obj = block_data->block->GetTxid(index);
      *txid = CreateString(txid_obj.GetHex());
    } else {
#ifndef CFD_DISABLE_ELEMENTS
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#else
      throw CfdException(
          CfdError::kCfdIllegalArgumentError, "Elements is not supported.");
#endif  // CFD_DISABLE_ELEMENTS
    }

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

};  // extern "C"

#endif  // CFD_DISABLE_CAPI
