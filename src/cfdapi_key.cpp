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
  static const std::string kWifError = "Error WIF to Private key.";
  Privkey key;

  try {
    key = Privkey::FromWif(privkey, NetType::kMainnet, is_compressed);
  } catch (const CfdException& except1) {
    std::string errmsg(except1.what());
    if (errmsg.find(kWifError) == std::string::npos) {
      throw except1;
    }
  }
  if (key.IsInvalid()) {
    try {
      key = Privkey::FromWif(privkey, NetType::kTestnet, is_compressed);
    } catch (const CfdException& except2) {
      std::string errmsg(except2.what());
      if (errmsg.find(kWifError) == std::string::npos) {
        throw except2;
      }
    }
  }
  if (key.IsInvalid()) {
    key = Privkey(ByteData(privkey));
  }
  return key.GeneratePubkey(is_compressed).GetHex();
}

}  // namespace api
}  // namespace cfd
