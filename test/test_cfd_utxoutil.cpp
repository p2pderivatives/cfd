#include "gtest/gtest.h"
#include <vector>

#include "cfdcore/cfdcore_amount.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfd_utxo.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_coin.h"

using cfd::Utxo;
using cfd::UtxoData;
using cfd::UtxoUtil;
using cfd::core::Txid;
using cfd::core::AddressType;

TEST(UtxoUtil, ConvertToUtxo_list)
{
  std::vector<UtxoData> utxos;
  UtxoData utxo1;
  utxo1.txid = Txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
  utxo1.vout = 1;
  utxo1.address_type = AddressType::kP2wpkhAddress;
  UtxoData utxo2;
  utxo2.txid = Txid("8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a");
  utxo2.vout = 2;
  utxo2.address_type = AddressType::kP2wpkhAddress;
  utxos.push_back(utxo1);
  utxos.push_back(utxo2);

  std::vector<Utxo> results;
  EXPECT_NO_THROW((results = UtxoUtil::ConvertToUtxo(utxos)));
  EXPECT_EQ(results.size(), 2);
  if (results.size() == 2) {
    EXPECT_TRUE((memcmp(results[0].txid,
        utxo1.txid.GetData().GetBytes().data(), sizeof(results[0].txid)) == 0));
    EXPECT_EQ(results[0].vout, utxo1.vout);
    EXPECT_TRUE((memcmp(results[1].txid,
        utxo2.txid.GetData().GetBytes().data(), sizeof(results[1].txid)) == 0));
    EXPECT_EQ(results[1].vout, utxo2.vout);
  }
}

TEST(UtxoUtil, ConvertToUtxo)
{
  UtxoData utxo1;
  utxo1.txid = Txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
  utxo1.vout = 1;
  utxo1.address_type = AddressType::kP2wpkhAddress;

  Utxo utxo;
  memset(&utxo, 0, sizeof(utxo));
  UtxoData dest;

  EXPECT_NO_THROW((UtxoUtil::ConvertToUtxo(utxo1, &utxo, &dest)));
  EXPECT_TRUE((memcmp(utxo.txid, utxo1.txid.GetData().GetBytes().data(), sizeof(utxo.txid)) == 0));
  EXPECT_EQ(utxo.vout, utxo1.vout);
  EXPECT_STREQ(dest.txid.GetData().GetHex().c_str(),
               utxo1.txid.GetData().GetHex().c_str());
  EXPECT_EQ(dest.vout, utxo1.vout);
}
