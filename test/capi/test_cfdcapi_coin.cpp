#include "gtest/gtest.h"

#include <stdexcept>
#include <string>
#include "capi/cfdc_internal.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_address.h"
#include "cfdc/cfdcapi_coin.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::AmountMap;
using cfd::CoinSelection;
using cfd::CoinSelectionOption;
using cfd::Utxo;
using cfd::UtxoFilter;
using cfd::core::Amount;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Txid;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::ConfidentialAssetId;
#endif  // CFD_DISABLE_ELEMENTS

/**
 * @brief testing class.
 */
class cfdcapi_coin : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

// SelectCoins(Single Asset) =====================================================

#ifndef CFD_DISABLE_ELEMENTS
static ConfidentialAssetId exp_dummy_asset_ca("aa00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_cb("bb00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_cc("cc00000000000000000000000000000000000000000000000000000000000000");

static std::vector<Utxo> GetElementsUtxoListByC() {
  std::vector<Utxo> utxos;
  {
    Txid txid("7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 312500000;
    memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 78125000;
    memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 1250000000;
    memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 39062500;
    memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 156250000;
    memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 2500000000;
    memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b01");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 26918400;
    memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b02");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 750000;
    memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b03");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 346430050;
    memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b04");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 18476350;
    memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c01");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 37654200;
    memcpy(utxo.asset, exp_dummy_asset_cc.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c02");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 127030000;
    memcpy(utxo.asset, exp_dummy_asset_cc.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
/*
0:  A,  312500000
1:  A,   78125000
2:  A, 1250000000
3:  A,   39062500
4:  A,  156250000
5:  A, 2500000000
6:  B,   26918400
7:  B,     750000
8:  B,  346430050
9:  B,   18476350
10: C,   37654200
11: C,  127030000
*/
  return utxos;
}

TEST(cfdcapi_coin, CfCoinSelection_1) {
  constexpr const char* kDescriptor = "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x";
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  auto convert_to_byte = [](const uint8_t* byte_array, size_t size) -> ByteData {
    std::vector<uint8_t> bytes(size);
    memcpy(bytes.data(), byte_array, bytes.size());
    return ByteData(bytes);
  };

  void* coin_select_handle = nullptr;
  std::vector<Utxo> utxos = GetElementsUtxoListByC();
  int64_t tx_fee_amount = 2000;
  double fee_rate = -1;
  double long_term_fee_rate = -1;
  double dust_fee_rate = -1;
  int64_t knapsack_min_change = -1;

  ret = CfdInitializeCoinSelection(
    handle, static_cast<uint32_t>(utxos.size()), 3,
    exp_dummy_asset_ca.GetHex().c_str(), tx_fee_amount, fee_rate,
    long_term_fee_rate, dust_fee_rate, knapsack_min_change,
    &coin_select_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_NE(nullptr, coin_select_handle);

  if (ret == kCfdSuccess) {
    for (int32_t index = 0; index < static_cast<int32_t>(utxos.size()); ++index) {
      const Utxo& utxo = utxos[index];
      ByteData txid = convert_to_byte(utxo.txid, sizeof(utxo.txid));
      std::string asset;
      ret = CfdAddCoinSelectionUtxo(
          handle, coin_select_handle, index,
          Txid(ByteData256(txid.GetBytes())).GetHex().c_str(), utxo.vout,
          utxo.amount,
          convert_to_byte(utxo.asset, sizeof(utxo.asset)).GetHex().c_str(),
          kDescriptor);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    ret = CfdAddCoinSelectionAmount(handle, coin_select_handle, 0, 115800000,
         exp_dummy_asset_ca.GetHex().c_str());
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdAddCoinSelectionAmount(handle, coin_select_handle, 1, 347180040,
        exp_dummy_asset_cb.GetHex().c_str());
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdAddCoinSelectionAmount(handle, coin_select_handle, 2, 37654100,
        exp_dummy_asset_cc.GetHex().c_str());
    EXPECT_EQ(kCfdSuccess, ret);

    int64_t utxo_fee_amount = 0;
    ret = CfdFinalizeCoinSelection(handle, coin_select_handle, &utxo_fee_amount);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(9000, utxo_fee_amount);

    int32_t utxo_index = 0;
    std::vector<int32_t> indexes;
    for (uint32_t index = 0; index < utxos.size(); ++index) {
      ret = CfdGetSelectedCoinIndex(
          handle, coin_select_handle, index, &utxo_index);
      EXPECT_EQ(kCfdSuccess, ret);
      if (utxo_index == -1) {
        break;
      }
      indexes.push_back(utxo_index);
    }
    EXPECT_EQ(5, indexes.size());

    if (indexes.size() == 5) {
      EXPECT_EQ(8, indexes[0]);
      EXPECT_EQ(7, indexes[1]);
      EXPECT_EQ(10, indexes[2]);
      EXPECT_EQ(1, indexes[3]);
      EXPECT_EQ(3, indexes[4]);
      /*
      req:
            A,  115800000
            B,  347180050
            C,   37654200
      res:
        8:  B,  346430050
        7:  B,     750000
        10: C,   37654200
        1:  A,   78125000
        3:  A,   39062500
      */
    }

    int64_t amount = 0;
    ret = CfdGetSelectedCoinAssetAmount(handle, coin_select_handle, 0, &amount);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(117187500, amount);
    ret = CfdGetSelectedCoinAssetAmount(handle, coin_select_handle, 1, &amount);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(347180050, amount);
    ret = CfdGetSelectedCoinAssetAmount(handle, coin_select_handle, 2, &amount);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(37654200, amount);
  }
  ret = CfdFreeCoinSelectionHandle(handle, coin_select_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}
#endif  // CFD_DISABLE_ELEMENTS
