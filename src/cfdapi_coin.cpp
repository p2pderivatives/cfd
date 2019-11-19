// Copyright 2019 CryptoGarage
/**
 * @file cfdapi_coin.cpp
 *
 * @brief cfd-apiで利用するCoin関連の実装ファイル
 */
#include <cstring>
#include <string>
#include <cstring>
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_coin.h"

namespace cfd {
namespace api {

std::vector<Utxo> CoinApi::ConvertToUtxo(
    const std::vector<UtxoData>& utxos) const {
  std::vector<Utxo> result;
  result.resize(utxos.size());
  std::vector<Utxo>::iterator ite = result.begin();
  for (const auto& utxo_data : utxos) {
    ConvertToUtxo(utxo_data, &(*ite));
    ++ite;
  }
  return result;
}

void CoinApi::ConvertToUtxo(const UtxoData& utxo_data, Utxo* utxo) const {
  if (utxo != nullptr) {
    memset(utxo, 0, sizeof(Utxo));
    utxo->block_height = utxo_data.block_height;
    utxo->vout = utxo_data.vout;
    utxo->binary_data = utxo_data.binary_data;
    utxo->amount = utxo_data.amount.GetSatoshiValue();

    ByteData block_hash = utxo_data.block_hash.GetData();
    if (!block_hash.Empty()) {
      memcpy(
          utxo->block_hash, block_hash.GetBytes().data(),
          sizeof(utxo->block_hash));
    }
    ByteData txid = utxo_data.txid.GetData();
    if (!txid.Empty()) {
      memcpy(utxo->txid, txid.GetBytes().data(), sizeof(utxo->txid));
    }

    // descriptorがあれば、そこから変換
    std::vector<uint8_t> locking_script_bytes;
    if (!utxo_data.descriptor.empty()) {
      // FIXME descriptor差し替え
      // utxo->locking_script = utxo_data.descriptor;
      // utxo->address_type = utxo_data.descriptor;
    } else if (!utxo_data.address.GetAddress().empty()) {
      locking_script_bytes =
          utxo_data.address.GetLockingScript().GetData().GetBytes();
      utxo->address_type =
          static_cast<uint16_t>(utxo_data.address.GetAddressType());
    } else if (!utxo_data.locking_script.IsEmpty()) {
      locking_script_bytes = utxo_data.locking_script.GetData().GetBytes();
      if (utxo_data.locking_script.IsP2wpkhScript()) {
        utxo->address_type = AddressType::kP2wpkhAddress;
      } else if (utxo_data.locking_script.IsP2wshScript()) {
        utxo->address_type = AddressType::kP2wshAddress;
      } else if (utxo_data.locking_script.IsP2pkhScript()) {
        utxo->address_type = AddressType::kP2pkhAddress;
      } else {  // TODO(k-matsuzawa): p2shに丸めておく
        utxo->address_type = AddressType::kP2shAddress;
      }
    }

    if (!locking_script_bytes.empty()) {
      utxo->script_length = static_cast<uint16_t>(locking_script_bytes.size());
      if (utxo->script_length < sizeof(utxo->locking_script)) {
        memcpy(
            utxo->locking_script, locking_script_bytes.data(),
            utxo->script_length);
      }
    }

    switch (utxo->address_type) {
      case AddressType::kP2wpkhAddress:
        utxo->witness_size_max = 71 + 33 + 2;
        // fall-through
      case AddressType::kP2shP2wpkhAddress:
        utxo->uscript_size_max = 20 + 2;
        break;
      case AddressType::kP2wshAddress:
        utxo->witness_size_max = 71 + utxo->script_length + 2;
        // fall-through
      case AddressType::kP2shP2wshAddress:
        utxo->uscript_size_max = 32 + 2;
        break;
      case AddressType::kP2pkhAddress:
        utxo->uscript_size_max = 71 + 33 + 3;
        break;
      case AddressType::kP2shAddress:
      default:
        utxo->uscript_size_max = 71 + utxo->script_length + 3;
        break;
    }

#ifndef CFD_DISABLE_ELEMENTS
    if (!utxo_data.asset.IsEmpty()) {
      std::vector<uint8_t> asset = utxo_data.asset.GetData().GetBytes();
      memcpy(utxo->asset, asset.data(), sizeof(utxo->asset));
      // utxo->blinded = false;
    }
#endif  // CFD_DISABLE_ELEMENTS
  }
}

}  // namespace api
}  // namespace cfd
