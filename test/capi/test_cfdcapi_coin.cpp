#include "gtest/gtest.h"

#include <stdexcept>
#include <string>
#include "capi/cfdc_internal.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_coin.h"
#include "cfd/cfdapi_elements_transaction.h"
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

TEST(cfdcapi_coin, EstimateFeeTest) {
  using cfd::AddressFactory;
  using cfd::api::ElementsUtxoAndOption;
  using cfd::api::UtxoData;
  using cfd::core::Address;
  using cfd::core::Amount;
  using cfd::core::BlockHash;
  using cfd::core::ConfidentialAssetId;
  using cfd::core::Script;
  using cfd::core::Txid;

  AddressFactory factory;
  std::vector<UtxoData> utxos = {
    UtxoData{
      12,
      BlockHash("590ba2283fbe5fb9363c74417bc1cb3f76d4df9d31890f9124b9e1706136b4f4"),
      Txid("ffe2f9aa4e2c2adb1676008aa07aec5f64149470abb5d89d91b26dfba14e9318"),
      0,
      Script("a914155801788fcd51f57e097a0b6c30f1b69057290d87"),
      Script("001477567d12cf82a3ef6e9bb0ab3a1aba7f65410cbd"),
      factory.GetAddress("33dsYKS5E5Vp3LhAw3krvhzNaWtqmrH29k"),
      "sh(wpkh([ef735203/0\'/0\'/5\']03948c01f159b4204b682668d6e850440564b6610c0e5bf30da684b2131f77c449))#2u75feqc",
      Amount::CreateBySatoshiAmount(600000000),
      nullptr,
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialAssetId(),
#endif  // CFD_DISABLE_ELEMENTS
    },
    UtxoData{
      22,
      BlockHash("e05419389a3aa71017eb4dd1d9eec73500971b86aab2eb0db116707c7be67996"),
      Txid("d68154ae19408850308967b2cb1b84a92166829610fb2321548b655c4a747c79"),
      0,
      Script("a914155801788fcd51f57e097a0b6c30f1b69057290d87"),
      Script("001477567d12cf82a3ef6e9bb0ab3a1aba7f65410cbd"),
      factory.GetAddress("33dsYKS5E5Vp3LhAw3krvhzNaWtqmrH29k"),
      "sh(wpkh([ef735203/0\'/0\'/5\']03948c01f159b4204b682668d6e850440564b6610c0e5bf30da684b2131f77c449))#2u75feqc",
      Amount::CreateBySatoshiAmount(700000000),
      nullptr,
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialAssetId(),
#endif  // CFD_DISABLE_ELEMENTS
    },
  };

  // 1 output data
  static const char* const kTxData = "02000000034cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff81ddd34c6c0c32544e3b89f5e24c6cd7afca62f2b5069281ac9fced6251191d20000000000ffffffff81ddd34c6c0c32544e3b89f5e24c6cd7afca62f2b5069281ac9fced6251191d20100000000ffffffff030200000000000000220020c5ae4ff17cec055e964b573601328f3f879fa441e53ef88acdfd4d8e8df429ef406f400100000000220020ea5a7208cddfbc20dd93e12bf29deb00b68c056382a502446c9c5b55490954d215cd5b0700000000220020f39f6272ba6b57918eb047c5dc44fb475356b0f24c12fca39b19284e80008a4200000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* fee_handle = nullptr;
  ret = CfdInitializeEstimateFee(handle, &fee_handle, false);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    for (auto& utxo : utxos) {
      ret = CfdAddTxInForEstimateFee(
          handle, fee_handle, utxo.txid.GetHex().c_str(),
          utxo.vout,utxo.redeem_script.GetHex().c_str(),
          utxo.address.GetAddress().c_str(), utxo.descriptor.c_str(),
          nullptr, false, false, false, 0, nullptr);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    int64_t tx_fee, utxo_fee;
    ret = CfdFinalizeEstimateFee(
        handle, fee_handle, kTxData, nullptr, &tx_fee, &utxo_fee, true, 20.0);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(static_cast<int64_t>(2780), tx_fee);
    EXPECT_EQ(static_cast<int64_t>(3600), utxo_fee);

    ret = CfdFreeEstimateFeeHandle(handle, fee_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdGetLastErrorCode(handle);
  if (ret != kCfdSuccess) {
    char* str_buffer = NULL;
    ret = CfdGetLastErrorMessage(handle, &str_buffer);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("", str_buffer);
    CfdFreeStringBuffer(str_buffer);
    str_buffer = NULL;
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

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
  double effective_fee_rate = -1;
  double long_term_fee_rate = -1;
  double dust_fee_rate = -1;
  int64_t knapsack_min_change = -1;

  ret = CfdInitializeCoinSelection(
    handle, static_cast<uint32_t>(utxos.size()), 3,
    exp_dummy_asset_ca.GetHex().c_str(), tx_fee_amount,
    effective_fee_rate, long_term_fee_rate, dust_fee_rate,
    knapsack_min_change, &coin_select_handle);
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

TEST(cfdcapi_coin, EstimateFeeElementsTest) {
  using cfd::ElementsAddressFactory;
  using cfd::api::ElementsUtxoAndOption;
  using cfd::api::UtxoData;
  using cfd::core::Address;
  using cfd::core::Amount;
  using cfd::core::BlockHash;
  using cfd::core::ConfidentialAssetId;
  using cfd::core::Script;
  using cfd::core::Txid;

  ElementsAddressFactory factory;
  std::vector<UtxoData> utxos = {
    UtxoData{
      15,
      BlockHash("590ba2283fbe5fb9363c74417bc1cb3f76d4df9d31890f9124b9e1706136b4f4"),
      Txid("ffe2f9aa4e2c2adb1676008aa07aec5f64149470abb5d89d91b26dfba14e9318"),
      0,
      Script("0014b4c93f8091216fd8f1a2d6ad944447f4927425de"),
      Script(""),
      factory.GetAddress("ert1qknynlqy3y9ha3udz66keg3z87jf8gfw7zjg8xe"),
      "wpkh([e3c39d64/0'/0'/9']029f74462149b36b8ac582a96afa8d257bbc8835c25aeab61d6712d69fbfa8e539)#mx0szvef",
      Amount::CreateBySatoshiAmount(600000000),
      nullptr,
      ConfidentialAssetId("6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3")
    },
    UtxoData{
      5,
      BlockHash("e05419389a3aa71017eb4dd1d9eec73500971b86aab2eb0db116707c7be67996"),
      Txid("d68154ae19408850308967b2cb1b84a92166829610fb2321548b655c4a747c79"),
      0,
      Script("0014384546aabafe0e229bdd5ce547131c77d94df665"),
      Script(""),
      factory.GetAddress("ert1q8pz5d246lc8z9x7atnj5wycuwlv5man94nt8a7"),
      "wpkh([e3c39d64/0'/0'/10']03f78d89b000a8e747d696fb2d9f1420e24283eb49679c68bdc3a3f3a49baeb703)#wm6p842k",
      Amount::CreateBySatoshiAmount(700000000),
      nullptr,
      ConfidentialAssetId("6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3")
    },
  };

  // 1 output data
  static const char* const kTxData = "020000000002d4b91f8ea0be3d89d33f9588884a843e78688152f4dff8aca5abc6f5973a83ae0000000000feffffff140510708ffd1fc8bea09e204d36b0d5b9402a31767a4f6c36f23b40cd0cbaf70a00000000faffffff0301f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f01000000003b9328e0001976a9146d715ab3da8090fd8f9e7aada1588e531b16b7da88ac0151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000008f0d1800255fb6295bfd3bbf4760776a59d9476fac2e52f9c0c16549aea5c2ca5f1c15e6f1976a9147cafacbfc72f3682b1055b3a6b8711f3622eabfd88ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f01000000000007a120000090000000";
  static const char* const kFeeAsset = "6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* fee_handle = nullptr;
  ret = CfdInitializeEstimateFee(handle, &fee_handle, true);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    for (auto& utxo : utxos) {
      ret = CfdAddTxInForEstimateFee(
          handle, fee_handle, utxo.txid.GetHex().c_str(),
          utxo.vout,utxo.redeem_script.GetHex().c_str(),
          utxo.address.GetAddress().c_str(), utxo.descriptor.c_str(),
          utxo.asset.GetHex().c_str(), false, false, false, 0, nullptr);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    int64_t tx_fee, utxo_fee;
    ret = CfdFinalizeEstimateFee(
        handle, fee_handle, kTxData, kFeeAsset, &tx_fee, &utxo_fee, true, 1.0);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(static_cast<int64_t>(1820), tx_fee);
    EXPECT_EQ(static_cast<int64_t>(138), utxo_fee);

    ret = CfdFreeEstimateFeeHandle(handle, fee_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdGetLastErrorCode(handle);
  if (ret != kCfdSuccess) {
    char* str_buffer = NULL;
    ret = CfdGetLastErrorMessage(handle, &str_buffer);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("", str_buffer);
    CfdFreeStringBuffer(str_buffer);
    str_buffer = NULL;
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}
#endif  // CFD_DISABLE_ELEMENTS
