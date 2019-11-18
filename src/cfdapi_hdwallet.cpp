// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_hdwallet.cpp
 *
 * @brief cfd-apiで利用するHDWallet APIクラスの実装
 */
#include <string>
#include <vector>

#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_logger.h"

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_hdwallet.h"

//////////////////////////////////
/// HDWalletApi
//////////////////////////////////
namespace cfd {
namespace api {

using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::ExtPrivkey;
using cfd::core::ExtPubkey;
using cfd::core::HDWallet;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::logger::warn;

/// base58 error message
static constexpr const char* kBase58Error = " base58 decode error.";
/// key type error message
static constexpr const char* kKeyTypeError = " keytype error.";

std::vector<std::string> HDWalletApi::GetMnemonicWordlist(
    const std::string& language) const {
  try {
    return HDWallet::GetMnemonicWordlist(language);
  } catch (CfdException& e) {
    warn(
        CFD_LOG_SOURCE, "Failed to GetMnemonicWordlist. error: [{}]",
        e.what());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to ConvertMnemonicToSeed. " + std::string(e.what()));
  }
}

ByteData HDWalletApi::ConvertMnemonicToSeed(
    const std::vector<std::string>& mnemonic, const std::string& passphrase,
    bool strict_check, const std::string& language, bool use_ideographic_space,
    ByteData* entropy) const {
  if (strict_check) {
    if (language.empty()) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to ConvertMnemonicToSeed. If check mnemonic strictly, need "
          "to set language.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to ConvertMnemonicToSeed. If check mnemonic strictly, need "
          "to set language.");
    }

    if (!HDWallet::CheckValidMnemonic(mnemonic, language)) {
      warn(
          CFD_LOG_SOURCE,
          "Failed to ConvertMnemonicToSeed. Mnemonic strict check error.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Failed to ConvertMnemonicToSeed. Mnemonic strict check error.");
    }
  }

  try {
    // calculate entropy
    if (!language.empty() && entropy != nullptr) {
      *entropy = HDWallet::ConvertMnemonicToEntropy(mnemonic, language);
    }

    // calculate seed
    HDWallet wallet(mnemonic, passphrase, use_ideographic_space);
    return wallet.GetSeed();
  } catch (CfdException& e) {
    warn(
        CFD_LOG_SOURCE, "Failed to ConvertMnemonicToSeed. error: [{}]",
        e.what());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to ConvertMnemonicToSeed. " + std::string(e.what()));
  }
}

std::vector<std::string> HDWalletApi::ConvertEntropyToMnemonic(
    const ByteData& entropy, const std::string& language) const {
  try {
    // calculate seed
    std::vector<std::string> mnemonic =
        HDWallet::ConvertEntropyToMnemonic(entropy, language);
    return mnemonic;
  } catch (CfdException& e) {
    warn(
        CFD_LOG_SOURCE, "Failed to GetMnemonicWordlist. error: [{}]",
        e.what());

    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to ConvertEntropyToMnemonic. " + std::string(e.what()));
  }
}

std::string HDWalletApi::CreateExtkeyFromSeed(
    const ByteData& seed, NetType net_type, ExtKeyType output_key_type) const {
  ExtPrivkey privkey(seed, net_type);
  if (output_key_type == ExtKeyType::kExtPrivkey) {
    return privkey.ToString();
  } else {
    return privkey.GetExtPubkey().ToString();
  }
}

std::string HDWalletApi::CreateExtkeyFromParent(
    const std::string& extkey, NetType net_type, ExtKeyType output_key_type,
    uint32_t child_number, bool hardened) const {
  uint32_t child_num = child_number;
  if (hardened) {
    child_num |= ExtPrivkey::kHardenedKey;
  }
  std::vector<uint32_t> path = {child_num};
  return CreateExtkeyFromParentPath(extkey, net_type, output_key_type, path);
}

std::string HDWalletApi::CreateExtkeyFromParentPath(
    const std::string& extkey, NetType net_type, ExtKeyType output_key_type,
    const std::vector<uint32_t>& child_number_list) const {
  std::string result;
  uint32_t check_version;
  uint32_t version;

  if (child_number_list.empty()) {
    warn(CFD_LOG_SOURCE, "child_number_list empty.");
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "child_number_list empty.");
  }

  ExtPrivkey privkey;
  ExtPubkey pubkey;
  try {
    privkey = ExtPrivkey(extkey);
  } catch (const CfdException& except) {
    std::string errmsg(except.what());
    if ((errmsg.find(kBase58Error) == std::string::npos) &&
        (errmsg.find(kKeyTypeError) == std::string::npos)) {
      throw except;
    }
  }

  if (privkey.IsValid()) {
    if (output_key_type == ExtKeyType::kExtPrivkey) {
      result = privkey.DerivePrivkey(child_number_list).ToString();
    } else {
      result = privkey.DerivePubkey(child_number_list).ToString();
    }
    version = privkey.GetVersion();
    check_version = GetExtkeyVersion(ExtKeyType::kExtPrivkey, net_type);

  } else {
    try {
      pubkey = ExtPubkey(extkey);
    } catch (const CfdException& pub_except) {
      std::string errmsg(pub_except.what());
      if (errmsg.find(kBase58Error) != std::string::npos) {
        warn(CFD_LOG_SOURCE, "Illegal extkey. base58 decode error.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Illegal extkey. base58 decode error.");
      }
      throw pub_except;
    }

    if (output_key_type == ExtKeyType::kExtPrivkey) {
      warn(
          CFD_LOG_SOURCE,
          "Illegal output_key_type. Cannot create privkey from pubkey.");
      throw CfdException(
          CfdError::kCfdIllegalArgumentError,
          "Illegal output_key_type. Cannot create privkey from pubkey.");
    }
    for (const uint32_t child_num : child_number_list) {
      // libwallyでもエラー検知されるが、エラーを把握しやすくするため確認
      if ((child_num & ExtPrivkey::kHardenedKey) != 0) {
        warn(
            CFD_LOG_SOURCE, "Illegal child_number. Hardened is privkey only.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Illegal child_number. Hardened is privkey only.");
      }
    }
    result = pubkey.DerivePubkey(child_number_list).ToString();
    version = pubkey.GetVersion();
    check_version = GetExtkeyVersion(ExtKeyType::kExtPubkey, net_type);
  }

  if (version != check_version) {
    warn(CFD_LOG_SOURCE, "Version unmatch. key version: {}", version);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "extkey networkType unmatch.");
  }
  return result;
}

std::string HDWalletApi::CreateExtPubkey(
    const std::string& extkey, NetType net_type) const {
  ExtPrivkey privkey(extkey);
  uint32_t check_version = GetExtkeyVersion(ExtKeyType::kExtPrivkey, net_type);
  if (privkey.GetVersion() != check_version) {
    warn(
        CFD_LOG_SOURCE, "Version unmatch. key version: {}",
        privkey.GetVersion());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "extkey networkType unmatch.");
  }
  return privkey.GetExtPubkey().ToString();
}

std::string HDWalletApi::GetPrivkeyFromExtkey(
    const std::string& extkey, NetType net_type, bool wif,
    bool is_compressed) const {
  ExtPrivkey ext_privkey(extkey);
  uint32_t check_version = GetExtkeyVersion(ExtKeyType::kExtPrivkey, net_type);
  if (ext_privkey.GetVersion() != check_version) {
    warn(
        CFD_LOG_SOURCE, "Version unmatch. key version: {}",
        ext_privkey.GetVersion());
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "extkey networkType unmatch.");
  }

  Privkey privkey = ext_privkey.GetPrivkey();
  if (wif) {
    return privkey.ConvertWif(net_type, is_compressed);
  } else {
    return privkey.GetHex();
  }
}

std::string HDWalletApi::GetPubkeyFromExtkey(
    const std::string& extkey, NetType net_type) const {
  std::string result;
  ExtPrivkey ext_privkey;
  ExtPubkey ext_pubkey;

  try {
    ext_privkey = ExtPrivkey(extkey);
  } catch (const CfdException& except) {
    std::string errmsg(except.what());
    if ((errmsg.find(kBase58Error) == std::string::npos) &&
        (errmsg.find(kKeyTypeError) == std::string::npos)) {
      throw except;
    }
  }

  if (ext_privkey.IsValid()) {
    ext_pubkey = ext_privkey.GetExtPubkey();
  } else {
    try {
      ext_pubkey = ExtPubkey(extkey);
    } catch (const CfdException& pub_except) {
      std::string errmsg(pub_except.what());
      if (errmsg.find(kBase58Error) != std::string::npos) {
        warn(CFD_LOG_SOURCE, "Illegal extkey. base58 decode error.");
        throw CfdException(
            CfdError::kCfdIllegalArgumentError,
            "Illegal extkey. base58 decode error.");
      }
      throw pub_except;
    }
  }

  uint32_t version = ext_pubkey.GetVersion();
  uint32_t check_version = GetExtkeyVersion(ExtKeyType::kExtPubkey, net_type);
  if (version != check_version) {
    warn(CFD_LOG_SOURCE, "Version unmatch. key version: {}", version);
    throw CfdException(
        CfdError::kCfdIllegalArgumentError, "extkey networkType unmatch.");
  }
  return ext_pubkey.GetPubkey().GetHex();
}

uint32_t HDWalletApi::GetExtkeyVersion(ExtKeyType key_type, NetType net_type) {
  uint32_t version;
  if (key_type == ExtKeyType::kExtPrivkey) {
    version = ExtPrivkey::kVersionTestnetPrivkey;
    if ((net_type == NetType::kMainnet) || (net_type == NetType::kLiquidV1)) {
      version = ExtPrivkey::kVersionMainnetPrivkey;
    }
  } else {
    version = ExtPubkey::kVersionTestnetPubkey;
    if ((net_type == NetType::kMainnet) || (net_type == NetType::kLiquidV1)) {
      version = ExtPubkey::kVersionMainnetPubkey;
    }
  }
  return version;
}

}  // namespace api
}  // namespace cfd
