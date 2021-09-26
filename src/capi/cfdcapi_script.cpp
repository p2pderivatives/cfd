// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_script.h
 *
 * @brief API implements file of Script function for used in cfd-capi
 */
#ifndef CFD_DISABLE_CAPI
#include "cfdc/cfdcapi_script.h"

#include <exception>
#include <sstream>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfdapi_transaction.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"
#include "cfdcore/cfdcore_schnorrsig.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_taproot.h"
#include "cfdcore/cfdcore_util.h"

using cfd::SignParameter;
using cfd::api::TransactionApi;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::SchnorrPubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptElement;
using cfd::core::ScriptElementType;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::StringUtil;
using cfd::core::TapBranch;
using cfd::core::TaprootScriptTree;
using cfd::core::TaprootUtil;
using cfd::core::logger::info;
using cfd::core::logger::warn;

// =============================================================================
// internal c-api
// =============================================================================
namespace cfd {
namespace capi {

//! prefix: ScriptItem
constexpr const char* const kPrefixScriptItem = "ScriptItem";
//! prefix: MultisigScriptSig
constexpr const char* const kPrefixMultisigScriptSig = "MultiScriptSig";
//! prefix: TapscriptTree
constexpr const char* const kPrefixTapscriptTree = "TapscriptTree";
//! delimiter of script asm
constexpr const char kScriptAsmDelimiter = ' ';
//! MultisigScriptSig struct
using CfdCapiMultisigScriptSigData = cfd::capi::CfdCapiMultisigSignData;

/**
 * @brief cfd-capi Script Itemハンドル情報構造体.
 * @details script itemは数や内容が可変のため、配列にはしない
 */
struct CfdCapiScriptItemHandleData {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! script item hex list
  std::vector<std::string>* script_items;
};

/**
 * @brief cfd-capi TapscriptTree handle data.
 */
struct CfdCapiTapscriptTree {
  char prefix[kPrefixLength];  //!< buffer prefix
  //! tree container
  std::vector<TaprootScriptTree>* tree_buffer;
  //! branch container
  std::vector<TapBranch>* branch_buffer;
};

}  // namespace capi
}  // namespace cfd

// =============================================================================
// extern c-api
// =============================================================================
using cfd::capi::AllocBuffer;
using cfd::capi::CfdCapiMultisigScriptSigData;
using cfd::capi::CfdCapiScriptItemHandleData;
using cfd::capi::CfdCapiTapscriptTree;
using cfd::capi::CheckBuffer;
using cfd::capi::ConvertHashToAddressType;
using cfd::capi::CreateString;
using cfd::capi::FreeBuffer;
using cfd::capi::FreeBufferOnError;
using cfd::capi::IsEmptyString;
using cfd::capi::kMultisigMaxKeyNum;
using cfd::capi::kPrefixMultisigScriptSig;
using cfd::capi::kPrefixScriptItem;
using cfd::capi::kPrefixTapscriptTree;
using cfd::capi::kPubkeyHexSize;
using cfd::capi::kScriptAsmDelimiter;
using cfd::capi::kSignatureHexSize;
using cfd::capi::SetLastError;
using cfd::capi::SetLastFatalError;

// API
extern "C" {

CFDC_API int CfdParseScript(
    void* handle, const char* script, void** script_item_handle,
    uint32_t* script_item_num) {
  CfdCapiScriptItemHandleData* buffer = nullptr;
  try {
    cfd::Initialize();
    if (script == nullptr) {
      warn(CFD_LOG_SOURCE, "script is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parse script. script is null.");
    }
    if (script_item_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "script item handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parse script. script item handle is null.");
    }

    Script script_obj;
    if (*script != '\0') {
      script_obj = Script(script);
    }
    std::vector<ScriptElement> script_elems = script_obj.GetElementList();

    buffer = static_cast<CfdCapiScriptItemHandleData*>(
        AllocBuffer(kPrefixScriptItem, sizeof(CfdCapiScriptItemHandleData)));
    buffer->script_items = new std::vector<std::string>;
    buffer->script_items->reserve(script_elems.size());
    for (const auto& elem : script_elems) {
      std::string data;
      if (elem.IsOpCode()) {
        // Convert to OpCode string
        data = elem.GetOpCode().ToCodeString();
      } else {
        data = elem.ToString();
      }
      buffer->script_items->push_back(data);
    }
    *script_item_handle = buffer;
    *script_item_num = static_cast<uint32_t>(buffer->script_items->size());

    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    if (buffer != nullptr) CfdFreeScriptItemHandle(handle, buffer);
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    if (buffer != nullptr) CfdFreeScriptItemHandle(handle, buffer);
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    if (buffer != nullptr) CfdFreeScriptItemHandle(handle, buffer);
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdGetScriptItem(
    void* handle, void* script_item_handle, uint32_t index,
    char** script_item) {
  try {
    cfd::Initialize();
    CheckBuffer(script_item_handle, kPrefixScriptItem);
    if (script_item == nullptr) {
      warn(CFD_LOG_SOURCE, "script_item is null.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to get script item. script_item is null.");
    }

    CfdCapiScriptItemHandleData* buffer =
        static_cast<CfdCapiScriptItemHandleData*>(script_item_handle);
    uint32_t item_num = static_cast<uint32_t>(buffer->script_items->size());
    if ((buffer->script_items == nullptr) || (index >= item_num)) {
      warn(CFD_LOG_SOURCE, "index is out of range.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to get script item. index is out of range.");
    }

    const std::string& item_str = buffer->script_items->at(index);
    *script_item = CreateString(item_str);
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdFreeScriptItemHandle(void* handle, void* script_item_handle) {
  try {
    cfd::Initialize();
    if (script_item_handle != nullptr) {
      CfdCapiScriptItemHandleData* item_struct =
          static_cast<CfdCapiScriptItemHandleData*>(script_item_handle);
      if (item_struct->script_items != nullptr) {
        delete item_struct->script_items;
        item_struct->script_items = nullptr;
      }
    }
    FreeBuffer(
        script_item_handle, kPrefixScriptItem,
        sizeof(CfdCapiScriptItemHandleData));
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdConvertScriptAsmToHex(
    void* handle, const char* script_asm, char** script_hex) {
  try {
    cfd::Initialize();
    if (script_hex == nullptr) {
      warn(CFD_LOG_SOURCE, "script hex is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to convert script. script hex is null.");
    }
    ScriptBuilder sb = ScriptBuilder();

    std::istringstream istream(script_asm);
    std::string script_item;
    while (std::getline(istream, script_item, kScriptAsmDelimiter)) {
      sb.AppendString(script_item);
    }
    Script script = sb.Build();

    *script_hex = CreateString(script.GetHex());
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    return SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
    return CfdErrorCode::kCfdUnknownError;
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
    return CfdErrorCode::kCfdUnknownError;
  }
}

int CfdInitializeMultisigScriptSig(void* handle, void** multisig_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (multisig_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "multisig sign handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. multisig sign handle is null.");
    }

    *multisig_handle = AllocBuffer(
        kPrefixMultisigScriptSig, sizeof(CfdCapiMultisigScriptSigData));
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

int CfdAddMultisigScriptSigData(
    void* handle, void* multisig_handle, const char* signature,
    const char* related_pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(multisig_handle, kPrefixMultisigScriptSig);
    if (signature == nullptr) {
      warn(CFD_LOG_SOURCE, "signature is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null.");
    }

    CfdCapiMultisigScriptSigData* data =
        static_cast<CfdCapiMultisigScriptSigData*>(multisig_handle);
    if (data->current_index >= kMultisigMaxKeyNum) {
      warn(
          CFD_LOG_SOURCE,
          "The number of signature has reached the upper limit.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "The number of signature has reached the upper limit.");
    }

    if (!IsEmptyString(signature)) {
      ByteData signature_bytes = ByteData(std::string(signature));
      std::string sig_str = signature_bytes.GetHex();
      sig_str.copy(data->signatures[data->current_index], kSignatureHexSize);
    }

    if (!IsEmptyString(related_pubkey)) {
      Pubkey key = Pubkey(std::string(related_pubkey));
      std::string pubkey_str = key.GetHex();
      pubkey_str.copy(data->pubkeys[data->current_index], kPubkeyHexSize);
    }

    ++(data->current_index);
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

int CfdAddMultisigScriptSigDataToDer(
    void* handle, void* multisig_handle, const char* signature,
    int sighash_type, bool sighash_anyone_can_pay,
    const char* related_pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (IsEmptyString(signature)) {
      warn(CFD_LOG_SOURCE, "signature is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. signature is null or empty.");
    }

    // encode to der
    SigHashType sighashtype = SigHashType::Create(
        static_cast<uint8_t>(sighash_type), sighash_anyone_can_pay);
    ByteData signature_bytes = ByteData(std::string(signature));
    SignParameter param(signature_bytes, true, sighashtype);
    ByteData signature_der = param.ConvertToSignature();

    return CfdAddMultisigScriptSigData(
        handle, multisig_handle, signature_der.GetHex().c_str(),
        related_pubkey);
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  return result;
}

int CfdFinalizeMultisigScriptSig(
    void* handle, void* multisig_handle, const char* redeem_script,
    char** scriptsig) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(multisig_handle, kPrefixMultisigScriptSig);
    if (IsEmptyString(redeem_script)) {
      warn(CFD_LOG_SOURCE, "redeemScript is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. redeemScript is null or empty.");
    }
    if (scriptsig == nullptr) {
      warn(CFD_LOG_SOURCE, "tx output is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tx output is null.");
    }

    Script redeem_script_obj = Script(std::string(redeem_script));

    CfdCapiMultisigScriptSigData* data =
        static_cast<CfdCapiMultisigScriptSigData*>(multisig_handle);
    if (data->current_index == 0) {
      warn(CFD_LOG_SOURCE, "The signature has empty.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError, "The signature has empty.");
    }
    if (data->current_index > kMultisigMaxKeyNum) {
      warn(
          CFD_LOG_SOURCE,
          "The number of signature has reached the upper limit.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "The number of signature has reached the upper limit.");
    }

    std::vector<SignParameter> sign_list;
    for (uint32_t index = 0; index < data->current_index; ++index) {
      SignParameter param(ByteData(std::string(data->signatures[index])));
      std::string pubkey(data->pubkeys[index]);
      if (Pubkey::IsValid(ByteData(pubkey))) {
        param.SetRelatedPubkey(Pubkey(pubkey));
      }
      sign_list.push_back(param);
    }

    TransactionApi api;
    std::string multisig_scriptsig =
        api.CreateMultisigScriptSig(sign_list, redeem_script_obj);
    *scriptsig = CreateString(multisig_scriptsig);

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

int CfdFreeMultisigScriptSigHandle(void* handle, void* multisig_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    FreeBuffer(
        multisig_handle, kPrefixMultisigScriptSig,
        sizeof(CfdCapiMultisigScriptSigData));
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

int CfdInitializeTaprootScriptTree(void* handle, void** tree_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  CfdCapiTapscriptTree* buffer = nullptr;
  try {
    cfd::Initialize();
    if (tree_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "tree_handle is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tree_handle is null.");
    }

    buffer = static_cast<CfdCapiTapscriptTree*>(
        AllocBuffer(kPrefixTapscriptTree, sizeof(CfdCapiTapscriptTree)));
    buffer->tree_buffer = new std::vector<TaprootScriptTree>(1);
    buffer->branch_buffer = new std::vector<TapBranch>(1);
    *tree_handle = buffer;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  CfdFreeTaprootScriptTreeHandle(handle, buffer);
  return result;
}

int CfdSetInitialTapLeaf(
    void* handle, void* tree_handle, const char* tapscript,
    uint8_t leaf_version) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    if (IsEmptyString(tapscript)) {
      warn(CFD_LOG_SOURCE, "tapscript is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tapscript is null or empty.");
    }
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    auto& tree = buffer->tree_buffer->at(0);

    TaprootScriptTree leaf(leaf_version, Script(tapscript));
    tree = leaf;
    buffer->branch_buffer->clear();
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

int CfdSetInitialTapBranchByHash(
    void* handle, void* tree_handle, const char* hash) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    auto& tree = buffer->tree_buffer->at(0);
    buffer->branch_buffer->clear();

    TapBranch branch;
    if (!IsEmptyString(hash)) branch = TapBranch(ByteData256(hash));
    buffer->branch_buffer->emplace_back(branch);

    tree = TaprootScriptTree();  // clear
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

int CfdSetScriptTreeFromString(
    void* handle, void* tree_handle, const char* tree_string,
    const char* tapscript, uint8_t leaf_version, const char* control_nodes) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    auto& tree = buffer->tree_buffer->at(0);

    if (IsEmptyString(tapscript)) {
      TapBranch branch;
      if (!IsEmptyString(tree_string)) {
        branch = TapBranch::FromString(tree_string);
      }
      buffer->branch_buffer->clear();
      buffer->branch_buffer->emplace_back(branch);
      tree = TaprootScriptTree();
    } else if (IsEmptyString(tree_string)) {
      warn(CFD_LOG_SOURCE, "tree_string is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tree_string is null or empty.");
    } else {
      if (leaf_version != TaprootScriptTree::kTapScriptLeafVersion) {
        // TODO(k-matsuzawa): Support in the future.
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Failed to parameter. leaf_version is not support.");
      }
      Script tapscript_obj(tapscript);
      std::vector<ByteData256> target_nodes;
      if (!IsEmptyString(control_nodes)) {
        std::string control_str(control_nodes);
        if ((control_str.size() % (cfd::core::kByteData256Length * 2)) == 0) {
          size_t split_size = cfd::core::kByteData256Length * 2;
          size_t max = control_str.size() / split_size;
          for (size_t index = 0; index < max; ++index) {
            size_t offset = index * split_size;
            ByteData256 node(control_str.substr(offset, split_size));
            target_nodes.emplace_back(node);
          }
        } else {  // control block
          std::vector<ByteData> stack;
          stack.emplace_back(tapscript_obj.GetData());
          stack.emplace_back(ByteData(control_str));
          TaprootUtil::ParseTaprootSignData(
              stack, nullptr, nullptr, nullptr, nullptr, &target_nodes,
              nullptr);
        }
      }

      tree = TaprootScriptTree::FromString(
          tree_string, tapscript_obj, target_nodes);
      buffer->branch_buffer->clear();
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

int CfdSetTapScriptByWitnessStack(
    void* handle, void* tree_handle, const char* control_block,
    const char* tapscript, char** internal_pubkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_internal_pubkey = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    if (IsEmptyString(control_block)) {
      warn(CFD_LOG_SOURCE, "control_block is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. control_block is null or empty.");
    }
    if (IsEmptyString(tapscript)) {
      warn(CFD_LOG_SOURCE, "tapscript is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tapscript is null or empty.");
    }
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    auto& tree = buffer->tree_buffer->at(0);

    uint8_t leaf_version = 0;
    SchnorrPubkey internal_pubkey_obj;
    std::vector<ByteData256> nodes;
    Script tapscript_obj;
    std::vector<ByteData> witness_stack;
    witness_stack.emplace_back(std::string(tapscript));
    witness_stack.emplace_back(std::string(control_block));
    TaprootUtil::ParseTaprootSignData(
        witness_stack, nullptr, nullptr, &leaf_version, &internal_pubkey_obj,
        &nodes, &tapscript_obj);
    TaprootScriptTree new_tree(leaf_version, tapscript_obj);
    for (const auto& node : nodes) new_tree.AddBranch(node);

    if (internal_pubkey != nullptr) {
      work_internal_pubkey = CreateString(internal_pubkey_obj.GetHex());
    }

    tree = new_tree;
    buffer->branch_buffer->clear();
    if (internal_pubkey != nullptr) *internal_pubkey = work_internal_pubkey;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_internal_pubkey);
  return result;
}

int CfdAddTapBranchByHash(
    void* handle, void* tree_handle, const char* branch_hash) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    if (IsEmptyString(branch_hash)) {
      warn(CFD_LOG_SOURCE, "branch_hash is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. branch_hash is null or empty.");
    }
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    ByteData256 hash(branch_hash);
    if (!buffer->branch_buffer->empty()) {
      auto& branch = buffer->branch_buffer->at(0);
      if (branch.ToString().empty()) {
        branch = TapBranch(hash);  // override
      } else {
        branch.AddBranch(hash);
      }
    } else {
      auto& tree = buffer->tree_buffer->at(0);
      tree.AddBranch(hash);
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

int CfdAddTapBranchByScriptTree(
    void* handle, void* tree_handle, void* branch_tree) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CheckBuffer(branch_tree, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    CfdCapiTapscriptTree* branch_buffer =
        static_cast<CfdCapiTapscriptTree*>(branch_tree);

    TapBranch* src_tree;
    if (!branch_buffer->branch_buffer->empty()) {
      src_tree = &branch_buffer->branch_buffer->at(0);
    } else {
      src_tree = &branch_buffer->tree_buffer->at(0);
    }

    if (!buffer->branch_buffer->empty()) {
      auto& branch = buffer->branch_buffer->at(0);
      if (branch.ToString().empty()) {
        branch = TapBranch(*src_tree);  // override
      } else {
        branch.AddBranch(*src_tree);
      }
    } else {
      auto& dest_tree = buffer->tree_buffer->at(0);
      dest_tree.AddBranch(*src_tree);
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

int CfdAddTapBranchByScriptTreeString(
    void* handle, void* tree_handle, const char* tree_string) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    if (IsEmptyString(tree_string)) {
      warn(CFD_LOG_SOURCE, "tree_string is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tree_string is null or empty.");
    }
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);

    TapBranch add_branch = TapBranch::FromString(tree_string);
    if (!buffer->branch_buffer->empty()) {
      auto& branch = buffer->branch_buffer->at(0);
      if (branch.ToString().empty()) {
        branch = TapBranch(add_branch);  // override
      } else {
        branch.AddBranch(add_branch);
      }
    } else {
      auto& dest_tree = buffer->tree_buffer->at(0);
      dest_tree.AddBranch(add_branch);
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

int CfdAddTapBranchByTapLeaf(
    void* handle, void* tree_handle, const char* tapscript,
    uint8_t leaf_version) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    if (IsEmptyString(tapscript)) {
      warn(CFD_LOG_SOURCE, "tapscript is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tapscript is null or empty.");
    }
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    TaprootScriptTree leaf(leaf_version, Script(tapscript));
    if (!buffer->branch_buffer->empty()) {
      auto& branch = buffer->branch_buffer->at(0);
      if (branch.ToString().empty()) {
        branch = leaf;  // override
      } else {
        branch.AddBranch(leaf);
      }
    } else {
      auto& tree = buffer->tree_buffer->at(0);
      tree.AddBranch(leaf);
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

int CfdGetBaseTapLeaf(
    void* handle, void* tree_handle, uint8_t* leaf_version, char** tapscript,
    char** tap_leaf_hash) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_tapscript = nullptr;
  char* work_tap_leaf_hash = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    if (!buffer->branch_buffer->empty()) {
      auto& branch = buffer->branch_buffer->at(0);
      if (branch.HasTapLeaf()) {
        if (tapscript != nullptr) {
          work_tapscript = CreateString(branch.GetScript().GetHex());
        }
        if (leaf_version != nullptr) *leaf_version = branch.GetLeafVersion();
      } else {
        if (leaf_version != nullptr) *leaf_version = 0;
      }
      if (tap_leaf_hash != nullptr) {
        work_tap_leaf_hash = CreateString(branch.GetBaseHash().GetHex());
      }
    } else {
      auto& tree = buffer->tree_buffer->at(0);
      if (tapscript != nullptr) {
        work_tapscript = CreateString(tree.GetScript().GetHex());
      }
      if (tap_leaf_hash != nullptr) {
        work_tap_leaf_hash = CreateString(tree.GetTapLeafHash().GetHex());
      }
      if (leaf_version != nullptr) *leaf_version = tree.GetLeafVersion();
    }

    if (tapscript != nullptr) *tapscript = work_tapscript;
    if (tap_leaf_hash != nullptr) *tap_leaf_hash = work_tap_leaf_hash;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_tapscript, &work_tap_leaf_hash);
  return result;
}

int CfdGetTapBranchCount(
    void* handle, void* tree_handle, uint32_t* branch_count) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    uint32_t count;
    if (!buffer->branch_buffer->empty()) {
      count = static_cast<uint32_t>(
          buffer->branch_buffer->at(0).GetBranchList().size());
    } else {
      count = static_cast<uint32_t>(
          buffer->tree_buffer->at(0).GetBranchList().size());
    }

    if (branch_count != nullptr) *branch_count = count;
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

int CfdGetTapBranchData(
    void* handle, void* tree_handle, uint8_t index_from_leaf,
    bool is_root_data, char** branch_hash, uint8_t* leaf_version,
    char** tapscript, uint8_t* depth_by_leaf_or_end) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_tapscript = nullptr;
  char* work_branch_hash = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    TapBranch* branch;
    if (!buffer->branch_buffer->empty()) {
      branch = &buffer->branch_buffer->at(0);
    } else {
      branch = &buffer->tree_buffer->at(0);
    }

    ByteData256 hash_obj;
    TapBranch branch_data;
    bool has_leaf = false;
    uint8_t branch_count = 0;
    auto branches = branch->GetBranchList();
    uint32_t max = static_cast<uint32_t>(branches.size());
    if (max <= index_from_leaf) {
      warn(CFD_LOG_SOURCE, "index_from_leaf is out of range.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. index_from_leaf is out of range.");
    }
    if (is_root_data) {
      hash_obj = branch->GetBranchHash(index_from_leaf);
    } else {
      branch_data = branches.at(index_from_leaf);
      hash_obj = branch_data.GetCurrentBranchHash();
      has_leaf = branch_data.HasTapLeaf();
      branch_count = static_cast<uint8_t>(branch_data.GetBranchList().size());
    }

    if (has_leaf && (tapscript != nullptr)) {
      work_tapscript = CreateString(branch_data.GetScript().GetHex());
    }
    if (branch_hash != nullptr) {
      work_branch_hash = CreateString(hash_obj.GetHex());
    }

    if (leaf_version != nullptr) {
      *leaf_version = (has_leaf) ? branch_data.GetLeafVersion() : 0;
    }
    if (depth_by_leaf_or_end != nullptr) *depth_by_leaf_or_end = branch_count;
    if (tapscript != nullptr) *tapscript = work_tapscript;
    if (branch_hash != nullptr) *branch_hash = work_branch_hash;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_tapscript, &work_branch_hash);
  return result;
}

int CfdGetTapBranchHandle(
    void* handle, void* tree_handle, uint8_t index_from_leaf,
    char** branch_hash, void** branch_tree_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_branch_hash = nullptr;
  CfdCapiTapscriptTree* work_branch_buffer = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    if (branch_tree_handle == nullptr) {
      warn(CFD_LOG_SOURCE, "branch_tree_handle is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. branch_tree_handle is null or empty.");
    }
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    TapBranch* branch;
    if (!buffer->branch_buffer->empty()) {
      branch = &buffer->branch_buffer->at(0);
    } else {
      branch = &buffer->tree_buffer->at(0);
    }

    auto branches = branch->GetBranchList();
    uint32_t max = static_cast<uint32_t>(branches.size());
    if (max <= index_from_leaf) {
      warn(CFD_LOG_SOURCE, "index_from_leaf is out of range.");
      throw CfdException(
          CfdError::kCfdOutOfRangeError,
          "Failed to parameter. index_from_leaf is out of range.");
    }
    TapBranch& branch_data = branches.at(index_from_leaf);
    ByteData256 hash_obj = branch_data.GetCurrentBranchHash();
    if (branch_hash != nullptr) {
      work_branch_hash = CreateString(hash_obj.GetHex());
    }

    work_branch_buffer = static_cast<CfdCapiTapscriptTree*>(
        AllocBuffer(kPrefixTapscriptTree, sizeof(CfdCapiTapscriptTree)));
    work_branch_buffer->tree_buffer = new std::vector<TaprootScriptTree>(1);
    work_branch_buffer->branch_buffer = new std::vector<TapBranch>();
    if (branch_data.HasTapLeaf()) {
      TaprootScriptTree tree(branch_data);
      auto& dest_tree = work_branch_buffer->tree_buffer->at(0);
      dest_tree = tree;
    } else {
      work_branch_buffer->branch_buffer->emplace_back(branch_data);
    }

    *branch_tree_handle = work_branch_buffer;
    if (branch_hash != nullptr) *branch_hash = work_branch_hash;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_branch_hash);
  CfdFreeTaprootScriptTreeHandle(handle, work_branch_buffer);
  return result;
}

int CfdGetTaprootScriptTreeHash(
    void* handle, void* tree_handle, const char* internal_pubkey, char** hash,
    char** tap_leaf_hash, char** control_block) {
  int result = CfdErrorCode::kCfdUnknownError;
  char* work_hash = nullptr;
  char* work_tap_leaf_hash = nullptr;
  char* work_control_block = nullptr;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    if (IsEmptyString(internal_pubkey)) {
      warn(CFD_LOG_SOURCE, "internal_pubkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. internal_pubkey is null or empty.");
    }
    TapBranch branch;
    if (!buffer->branch_buffer->empty()) {
      branch = buffer->branch_buffer->at(0);
    } else {
      auto& tree = buffer->tree_buffer->at(0);
      if (tap_leaf_hash != nullptr) {
        work_tap_leaf_hash = CreateString(tree.GetTapLeafHash().GetHex());
      }
      branch = tree;
    }
    SchnorrPubkey tapscript_hash;
    auto control = TaprootUtil::CreateTapScriptControl(
        SchnorrPubkey(internal_pubkey), branch, &tapscript_hash);

    if (hash != nullptr) {
      work_hash = CreateString(tapscript_hash.GetHex());
    }
    if (control_block != nullptr) {
      work_control_block = CreateString(control.GetHex());
    }

    if (hash != nullptr) *hash = work_hash;
    if ((tap_leaf_hash != nullptr) && (work_tap_leaf_hash != nullptr)) {
      *tap_leaf_hash = work_tap_leaf_hash;
    }
    if (control_block != nullptr) *control_block = work_control_block;
    return CfdErrorCode::kCfdSuccess;
  } catch (const CfdException& except) {
    result = SetLastError(handle, except);
  } catch (const std::exception& std_except) {
    SetLastFatalError(handle, std_except.what());
  } catch (...) {
    SetLastFatalError(handle, "unknown error.");
  }
  FreeBufferOnError(&work_hash, &work_tap_leaf_hash, &work_control_block);
  return result;
}

int CfdGetTaprootTweakedPrivkey(
    void* handle, void* tree_handle, const char* internal_privkey,
    char** tweaked_privkey) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    if (IsEmptyString(internal_privkey)) {
      warn(CFD_LOG_SOURCE, "internal_privkey is null or empty.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. internal_privkey is null or empty.");
    }
    if (tweaked_privkey == nullptr) {
      warn(CFD_LOG_SOURCE, "tweaked_privkey is null.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to parameter. tweaked_privkey is null.");
    }
    TapBranch branch;
    if (!buffer->branch_buffer->empty()) {
      branch = buffer->branch_buffer->at(0);
    } else {
      branch = buffer->tree_buffer->at(0);
    }
    Privkey privkey;
    if (Privkey::HasWif(internal_privkey)) {
      privkey = Privkey::FromWif(internal_privkey);
    } else {
      privkey = Privkey(internal_privkey);
    }
    auto taproot_privkey = branch.GetTweakedPrivkey(privkey);
    *tweaked_privkey = CreateString(taproot_privkey.GetHex());
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

int CfdGetTaprootScriptTreeSrting(
    void* handle, void* tree_handle, char** tree_string) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    CheckBuffer(tree_handle, kPrefixTapscriptTree);
    CfdCapiTapscriptTree* buffer =
        static_cast<CfdCapiTapscriptTree*>(tree_handle);
    std::string tree_str;
    if (!buffer->branch_buffer->empty()) {
      tree_str = buffer->branch_buffer->at(0).ToString();
    } else {
      tree_str = buffer->tree_buffer->at(0).ToString();
    }

    if (tree_string != nullptr) {
      *tree_string = CreateString(tree_str);
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

int CfdFreeTaprootScriptTreeHandle(void* handle, void* tree_handle) {
  int result = CfdErrorCode::kCfdUnknownError;
  try {
    cfd::Initialize();
    if (tree_handle != nullptr) {
      CheckBuffer(tree_handle, kPrefixTapscriptTree);
      CfdCapiTapscriptTree* data =
          static_cast<CfdCapiTapscriptTree*>(tree_handle);
      if (data->tree_buffer != nullptr) {
        delete data->tree_buffer;
        data->tree_buffer = nullptr;
      }
      if (data->branch_buffer != nullptr) {
        delete data->branch_buffer;
        data->branch_buffer = nullptr;
      }
      FreeBuffer(
          tree_handle, kPrefixTapscriptTree, sizeof(CfdCapiTapscriptTree));
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
