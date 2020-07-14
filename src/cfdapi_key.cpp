// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_key.cpp
 *
 * @brief \~english implementation file for key management that uses cfd-api
 * \~japanese cfd-apiで利用する鍵関連の実装ファイル
 */
#include "cfd/cfdapi_key.h"

#include <string>

#include "cfd/cfd_common.h"
#include "cfd/cfdapi_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_transaction_common.h"

namespace cfd {
namespace api {

using cfd::core::ByteData;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::NetType;
using cfd::core::Privkey;

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
  NetType net_type = NetType::kMainnet;

  bool has_wif = Privkey::HasWif(privkey, &net_type, nullptr);
  if (has_wif) {
    key = Privkey::FromWif(privkey, net_type, is_compressed);
  } else {
    key = Privkey(privkey);
  }
  return key.GeneratePubkey(is_compressed).GetHex();
}

Privkey KeyApi::GetPrivkeyFromWif(
    const std::string& wif, NetType* net_type, bool* is_compressed) const {
  Privkey key;
  bool is_compress = true;
  NetType temp_net_type = NetType::kMainnet;

  bool has_wif = Privkey::HasWif(wif, &temp_net_type, &is_compress);
  if (has_wif) {
    key = Privkey::FromWif(wif, temp_net_type, is_compress);
    if (net_type != nullptr) *net_type = temp_net_type;
    if (is_compressed != nullptr) *is_compressed = is_compress;
  }

  if (!key.IsValid()) {
    throw CfdException(
        CfdError::kCfdIllegalArgumentError,
        "Failed to GetPrivkeyFromWif. Wif format error.");
  }
  return key;
}

Privkey KeyApi::GetPrivkey(
    const std::string& privkey, NetType* net_type, bool* is_compressed) const {
  Privkey key;
  bool is_compress = true;
  NetType temp_net_type = NetType::kMainnet;

  bool has_wif = Privkey::HasWif(privkey, &temp_net_type, &is_compress);
  if (has_wif) {
    key = Privkey::FromWif(privkey, temp_net_type, is_compress);
    if (net_type != nullptr) *net_type = temp_net_type;
    if (is_compressed != nullptr) *is_compressed = is_compress;
  } else {
    key = Privkey(privkey);
  }
  return key;
}

}  // namespace api
}  // namespace cfd
