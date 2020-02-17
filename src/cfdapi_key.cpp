// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_key.cpp
 *
 * @brief \~english implementation file for key management that uses cfd-api
 * \~japanese cfd-apiで利用する鍵関連の実装ファイル
 */
#include <string>

#include "cfd/cfdapi_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_transaction_common.h"

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_key.h"

namespace cfd {
namespace api {

using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::NetType;
using cfd::core::Privkey;

/// wif format error message.
static constexpr const char* kWifError = "Error WIF to Private key.";
/// wif format net types.
static const NetType kWifNetTypes[] = {NetType::kMainnet, NetType::kTestnet};
/// wif format net types num.
static constexpr const uint8_t kNetTypesNum = 2;

Privkey KeyApi::CreateKeyPair(
    bool is_compressed, Pubkey* pubkey, std::string* wif, NetType net_type) {
  // generate random private key
  Privkey privkey = Privkey::GenerageRandomKey();

  // derive pubkey from private key
  const Pubkey out_pubkey = privkey.GeneratePubkey(is_compressed);

  if (pubkey != nullptr) {
    *pubkey = out_pubkey;
  }

  if (wif != nullptr) {
    *wif = privkey.ConvertWif(net_type, is_compressed);
  }
  return privkey;
}

std::string KeyApi::GetPubkeyFromPrivkey(
    const std::string& privkey, bool is_compressed) const {
  Privkey key;

  for (uint8_t net_index = 0; net_index < kNetTypesNum; ++net_index) {
    try {
      key = Privkey::FromWif(privkey, kWifNetTypes[net_index], is_compressed);
      break;
    } catch (const CfdException& except) {
      std::string errmsg(except.what());
      if (errmsg.find(kWifError) == std::string::npos) {
        throw except;
      }
    }
  }

  if (key.IsInvalid()) {
    key = Privkey(ByteData(privkey));
  }
  return key.GeneratePubkey(is_compressed).GetHex();
}

Privkey KeyApi::GetPrivkeyFromWif(
    const std::string& wif, NetType* net_type, bool* is_compressed) const {
  static const bool kCompress[2] = {true, false};
  static const uint8_t kListNum = 2;

  Privkey key;
  for (uint8_t compress_index = 0; compress_index < kListNum;
       ++compress_index) {
    for (uint8_t net_index = 0; net_index < kNetTypesNum; ++net_index) {
      try {
        key = Privkey::FromWif(
            wif, kWifNetTypes[net_index], kCompress[compress_index]);
        if (net_type != nullptr) *net_type = kWifNetTypes[net_index];
        if (is_compressed != nullptr) {
          *is_compressed = kCompress[compress_index];
        }
        break;
      } catch (const CfdException& except) {
        std::string errmsg(except.what());
        if (errmsg.find(kWifError) == std::string::npos) {
          throw except;
        }
      }
    }
  }

  if (key.IsInvalid()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to GetPrivkeyFromWif. Wif format error.");
  }
  return key;
}

}  // namespace api
}  // namespace cfd
