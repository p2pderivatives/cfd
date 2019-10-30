// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_hdwallet.cpp
 *
 * @brief-eng implementation of HDWallet API class that uses cfd-api
 * @brief-jp cfd-apiで利用するHDWallet APIクラスの実装
 */
#include <string>
#include <vector>

#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_hdwallet.h"
#include "cfdcore/cfdcore_logger.h"

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_hdwallet.h"
#include "cfdapi_internal.h"  // NOLINT

//////////////////////////////////
/// HDWalletStructApi
//////////////////////////////////
namespace cfd {
namespace js {
namespace api {

using cfd::api::HDWalletApi;
using cfd::core::ByteData;

GetMnemonicWordlistResponseStruct HDWalletStructApi::GetMnemonicWordlist(
    const GetMnemonicWordlistRequestStruct& request) {
  auto call_func = [](const GetMnemonicWordlistRequestStruct& request)
      -> GetMnemonicWordlistResponseStruct {
    GetMnemonicWordlistResponseStruct response;
    // check language is support
    std::string language = request.language;

    // get bip39 wordlist
    std::vector<std::string> wordlist =
        HDWalletApi::GetMnemonicWordlist(language);

    response.wordlist = wordlist;
    return response;
  };

  GetMnemonicWordlistResponseStruct result;
  result = ExecuteStructApi<
      GetMnemonicWordlistRequestStruct, GetMnemonicWordlistResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

ConvertMnemonicToSeedResponseStruct HDWalletStructApi::ConvertMnemonicToSeed(
    const ConvertMnemonicToSeedRequestStruct& request) {
  auto call_func = [](const ConvertMnemonicToSeedRequestStruct& request)
      -> ConvertMnemonicToSeedResponseStruct {
    ConvertMnemonicToSeedResponseStruct response;
    // check language is support
    std::vector<std::string> mnemonic = request.mnemonic;
    std::string passphrase = request.passphrase;
    bool strict_check = request.strict_check;
    std::string language = request.language;
    bool use_ideographic_space =
        ((language == "jp") && request.use_ideographic_space);
    ByteData entropy;

    // get bip39 wordlist
    ByteData seed = HDWalletApi::ConvertMnemonicToSeed(
        mnemonic, passphrase, strict_check, language, use_ideographic_space,
        &entropy);
    if (entropy.Empty()) {
      response.ignore_items.insert("entropy");
    } else {
      response.entropy = entropy.GetHex();
    }

    response.seed = seed.GetHex();
    return response;
  };

  ConvertMnemonicToSeedResponseStruct result;
  result = ExecuteStructApi<
      ConvertMnemonicToSeedRequestStruct, ConvertMnemonicToSeedResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

ConvertEntropyToMnemonicResponseStruct
HDWalletStructApi::ConvertEntropyToMnemonic(
    const ConvertEntropyToMnemonicRequestStruct& request) {
  auto call_func = [](const ConvertEntropyToMnemonicRequestStruct& request)
      -> ConvertEntropyToMnemonicResponseStruct {
    ConvertEntropyToMnemonicResponseStruct response;
    ByteData entropy(request.entropy);
    std::string language = request.language;

    // entropy to mnemonic
    std::vector<std::string> mnemonic =
        HDWalletApi::ConvertEntropyToMnemonic(entropy, language);

    response.mnemonic = mnemonic;
    return response;
  };

  ConvertEntropyToMnemonicResponseStruct result;
  result = ExecuteStructApi<
      ConvertEntropyToMnemonicRequestStruct,
      ConvertEntropyToMnemonicResponseStruct>(
      request, call_func, std::string(__FUNCTION__));
  return result;
}

}  // namespace api
}  // namespace js
}  // namespace cfd

//////////////////////////////////
/// HDWalletApi
//////////////////////////////////
namespace cfd {
namespace api {

using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::HDWallet;
using cfd::core::logger::warn;

std::vector<std::string> HDWalletApi::GetMnemonicWordlist(
    const std::string& language) {
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
    ByteData* entropy) {
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
    const ByteData& entropy, const std::string& language) {
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

}  // namespace api
}  // namespace cfd
