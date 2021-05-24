#include "gtest/gtest.h"

#include <stdexcept>
#include <string>
#include "capi/cfdc_internal.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_transaction_common.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_address.h"
#include "cfd/cfdapi_coin.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfdc/cfdcapi_coin.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_common.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_logger.h"

using cfd::AddressFactory;
using cfd::AmountMap;
using cfd::CoinSelection;
using cfd::CoinSelectionOption;
using cfd::Utxo;
using cfd::UtxoData;
using cfd::UtxoFilter;
using cfd::core::Address;
using cfd::core::Amount;
using cfd::core::AddressType;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdError;
using cfd::core::CfdException;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::Txid;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::BlindFactor;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialValue;
using cfd::ElementsAddressFactory;
using cfd::core::ElementsConfidentialAddress;
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

  AddressFactory factory;
  std::vector<UtxoData> utxos = {
    UtxoData{
      static_cast<uint32_t>(12),
      BlockHash("590ba2283fbe5fb9363c74417bc1cb3f76d4df9d31890f9124b9e1706136b4f4"),
      Txid("ffe2f9aa4e2c2adb1676008aa07aec5f64149470abb5d89d91b26dfba14e9318"),
      static_cast<uint32_t>(0),
      Script("a914155801788fcd51f57e097a0b6c30f1b69057290d87"),
      Script("001477567d12cf82a3ef6e9bb0ab3a1aba7f65410cbd"),
      factory.GetAddress("33dsYKS5E5Vp3LhAw3krvhzNaWtqmrH29k"),
      "sh(wpkh([ef735203/0\'/0\'/5\']03948c01f159b4204b682668d6e850440564b6610c0e5bf30da684b2131f77c449))#2u75feqc",
      Amount::CreateBySatoshiAmount(600000000),
      static_cast<AddressType>(0),
      nullptr,
#ifndef CFD_DISABLE_ELEMENTS
      ConfidentialAssetId(),
      ElementsConfidentialAddress(),
      BlindFactor(),
      BlindFactor(),
      ConfidentialValue(),
#endif  // CFD_DISABLE_ELEMENTS
      Script()
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
      static_cast<AddressType>(0),
#ifndef CFD_DISABLE_ELEMENTS
      nullptr,
      ConfidentialAssetId(),
      ElementsConfidentialAddress(),
      BlindFactor(),
      BlindFactor(),
      ConfidentialValue(),
#else
      nullptr,
#endif  // CFD_DISABLE_ELEMENTS
      Script()
    }
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
          handle, fee_handle, utxo.txid.GetHex().c_str(), utxo.vout,
          utxo.descriptor.c_str(), nullptr,
          false, false, false, 0, nullptr);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    int64_t tx_fee, utxo_fee;
    ret = CfdFinalizeEstimateFee(
        handle, fee_handle, kTxData, nullptr, &tx_fee, &utxo_fee, true, 20.0);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(static_cast<int64_t>(2820), tx_fee);
    EXPECT_EQ(static_cast<int64_t>(3660), utxo_fee);

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

TEST(cfdcapi_coin, EstimateFeeBySchnorr) {
  // FIXME 実装する
  AddressFactory factory;
  std::vector<UtxoData> utxos = {
    UtxoData{
      12,
      BlockHash("590ba2283fbe5fb9363c74417bc1cb3f76d4df9d31890f9124b9e1706136b4f4"),
      Txid("2fea883042440d030ca5929814ead927075a8f52fef5f4720fa3cec2e475d916"),
      0,
      Script("51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb"),
      Script(""),
      factory.GetAddress("bc1pzamhq9jglfxaj0r5ahvatr8uc77u973s5tm04yytdltsey5r8naspp3kr4"),
      "raw(51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb)",
      Amount::CreateBySatoshiAmount(2499999000),
      AddressType::kTaprootAddress,
#ifndef CFD_DISABLE_ELEMENTS
      nullptr,
      ConfidentialAssetId(),
      ElementsConfidentialAddress(),
      BlindFactor(),
      BlindFactor(),
      ConfidentialValue(),
#else
      nullptr,
#endif  // CFD_DISABLE_ELEMENTS
      Script()
    }
  };

  // 1 output data
  static const char* const kTxData = "0200000000010116d975e4c2cea30f72f4f5fe528f5a0727d9ea149892a50c030d44423088ea2f0000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d50000000000";

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
          handle, fee_handle, utxo.txid.GetHex().c_str(), utxo.vout,
          utxo.descriptor.c_str(), nullptr,
          false, false, false, 0, nullptr);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    int64_t tx_fee, utxo_fee;
    ret = CfdFinalizeEstimateFee(
        handle, fee_handle, kTxData, nullptr, &tx_fee, &utxo_fee, true, 2.0);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(static_cast<int64_t>(86), tx_fee);
    EXPECT_EQ(static_cast<int64_t>(116), utxo_fee);

    ret = CfdFreeEstimateFeeHandle(handle, fee_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  // exp: 0200000000010116d975e4c2cea30f72f4f5fe528f5a0727d9ea149892a50c030d44423088ea2f0000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d5014161f75636003a870b7a1685abae84eedf8c9527227ac70183c376f7b3a35b07ebcbea14749e58ce1a87565b035b2f3963baa5ae3ede95e89fd607ab7849f208720100000000
  // vsize: 100

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

TEST(cfdcapi_coin, EstimateFeeByTapscript) {
  ScriptBuilder builder;
  builder.AppendData("f5aa6b260f9df687786cd3813ba83b476e195041bccea800f2571212f4aae9848a538b6175a4f8ea291d38e351ea7f612a3d700dca63cd3aff05d315c5698ee901");
  builder.AppendData("201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac");
  builder.AppendData("c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54");
  Script template_script = builder.Build();
  EXPECT_EQ("41f5aa6b260f9df687786cd3813ba83b476e195041bccea800f2571212f4aae9848a538b6175a4f8ea291d38e351ea7f612a3d700dca63cd3aff05d315c5698ee90122201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac4c61c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54", template_script.GetHex());

  AddressFactory factory(NetType::kRegtest);
  std::vector<UtxoData> utxos = {
    UtxoData{
      12,
      BlockHash("590ba2283fbe5fb9363c74417bc1cb3f76d4df9d31890f9124b9e1706136b4f4"),
      Txid("195d6c18afaa6c33dcc2da86c6c0cd3f93ec2b44e4c8e9be00c7000eafa1805b"),
      0,
      Script("51203dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d2"),
      Script(""),
      factory.GetAddress("bcrt1p8hh955u8526hjqhn5m5a5pmhymgecmxgerrmqj70tgvhk25mq8fq50z666"),
      "raw(51203dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d2)",
      Amount::CreateBySatoshiAmount(2499999000),
      AddressType::kTaprootAddress,
#ifndef CFD_DISABLE_ELEMENTS
      nullptr,
      ConfidentialAssetId(),
      ElementsConfidentialAddress(),
      BlindFactor(),
      BlindFactor(),
      ConfidentialValue(),
#else
      nullptr,
#endif  // CFD_DISABLE_ELEMENTS
      template_script
    }
  };

  // 1 output data
  static const char* const kTxData = "02000000015b80a1af0e00c700bee9c8e4442bec933fcdc0c686dac2dc336caaaf186c5d190000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d500000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* fee_handle = nullptr;
  ret = CfdInitializeEstimateFee(handle, &fee_handle, false);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    for (auto& utxo : utxos) {
      ret = CfdAddTxInTemplateForEstimateFee(
          handle, fee_handle, utxo.txid.GetHex().c_str(), utxo.vout,
          utxo.descriptor.c_str(), nullptr,
          false, false, false, 0, nullptr,
          utxo.scriptsig_template.GetHex().c_str());
      EXPECT_EQ(kCfdSuccess, ret);
    }

    int64_t tx_fee, utxo_fee;
    ret = CfdFinalizeEstimateFee(
        handle, fee_handle, kTxData, nullptr, &tx_fee, &utxo_fee, true, 2.0);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(static_cast<int64_t>(86), tx_fee);
    EXPECT_EQ(static_cast<int64_t>(184), utxo_fee);

    ret = CfdFreeEstimateFeeHandle(handle, fee_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  // exp: 020000000001015b80a1af0e00c700bee9c8e4442bec933fcdc0c686dac2dc336caaaf186c5d190000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d50341f5aa6b260f9df687786cd3813ba83b476e195041bccea800f2571212f4aae9848a538b6175a4f8ea291d38e351ea7f612a3d700dca63cd3aff05d315c5698ee90122201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac61c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d5400000000
  // vsize: 133

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

#ifndef CFD_DISABLE_ELEMENTS
static ConfidentialAssetId exp_dummy_asset_ca("aa00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_cb("bb00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_cc("cc00000000000000000000000000000000000000000000000000000000000000");
#endif  // CFD_DISABLE_ELEMENTS

std::vector<Utxo> CfdGetElementsUtxoListByC(bool use_asset) {
  std::vector<Utxo> utxos;
  {
    Txid txid("7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 312500000;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 78125000;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 1250000000;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 39062500;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 156250000;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 2500000000;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_ca.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b01");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 26918400;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b02");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 750000;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b03");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 346430050;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b04");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 18476350;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_cb.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c01");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 37654200;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_cc.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c02");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 127030000;
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      memcpy(utxo.asset, exp_dummy_asset_cc.GetData().GetBytes().data(), sizeof(utxo.asset));
#endif  // CFD_DISABLE_ELEMENTS
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

std::vector<UtxoData> CfdGetElementsUtxoDataListByC(bool use_asset) {
  // mnemonic: token hair neglect leader furnace obtain sadness tool you father paddle skate remain carry impact dinosaur correct essay rent illness predict mercy exist ring
  // xpriv: xprv9s21ZrQH143K4QXrTfC9L43GKCuLcDiBCWjyVqfZUTzoPJWUstD4HTJKGz1U5jAGZzKshcX6cCyZ1ZdxSUQLz92pEZWEGwxa39ks2vhTsfA
  // derive: 44h/0h/0h/0/*
  std::vector<UtxoData> utxos;
  {
    Txid txid("7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(312500000);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_ca;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(0329165ca5832de80305c92d4b1415f10340d267ba05cbffcfe02d386dc5020e4d))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(78125000);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_ca;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(022db3cb17d98db6cd8d513f88b095dbe80ef9e57acd5b1d9e8bd7f24618079451))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(1250000000);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_ca;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(032d04e0b1474a82ad68c0ef37e1a7cf6c75ef01b22c00882e8e4e127a942823a1))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(39062500);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_ca;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(03a7fb569db921abf70f1b6b9ad9ac863196deecd99d606b139bba7d740d1cc5bf))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(156250000);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_ca;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(02df74fc8124ff6f3982e90afb318f3e955b10f58c4c6014b3a767e16160f811d9))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(int64_t{2500000000});
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_ca;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(029222484db385d268a2a4604ea40fd2228401061f741ad9da8c907ba9df29c2d3))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b01");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(26918400);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_cb;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(038f9011753b74fa0134d4b64a1491f99e0c4c0e16da616627c1f6a93c5e7555c0))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b02");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(750000);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_cb;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(0302f567f9671b570dbcf179f3ba5f2fb381ea7e8db6ab9e0968c07d40325c3fcd))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b03");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(346430050);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_cb;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(034ff60d8fb18ae88019f6f905cfaa0e1841f75edfa1f3c0a5bfaf77b796243901))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b04");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(18476350);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_cb;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(029f2126cd8b55af7cc3cee8154c44de7cb7cb214809f81144d6b323d9c7a3993e))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c01");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(37654200);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_cc;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(02f1d2c28388e3fd609ff383f022b615f1cd8a1931632706f63bfb6e253875ca03))";
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c02");
    UtxoData utxo;
    utxo.txid = txid;
    utxo.vout = 0;
    utxo.amount = Amount(127030000);
#ifndef CFD_DISABLE_ELEMENTS
    if (use_asset)
      utxo.asset = exp_dummy_asset_cc;
#endif  // CFD_DISABLE_ELEMENTS
    utxo.descriptor = "sh(wpkh(02cca4482dc1e7d54c879c0d9069e3d66c3bf91b2bf46eddc74f18d76c659dfd10))";
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

// SelectCoins(BTC) =====================================================

TEST(cfdcapi_coin, CfCoinSelection_BTC1) {
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
  std::vector<Utxo> utxos = CfdGetElementsUtxoListByC(false);
  int64_t tx_fee_amount = 2000;
  double effective_fee_rate = 20;
  double long_term_fee_rate = 20;
  double dust_fee_rate = -1;
  int64_t knapsack_min_change = -1;

  ret = CfdInitializeCoinSelection(
    handle, static_cast<uint32_t>(utxos.size()), 3,
    "", tx_fee_amount,
    effective_fee_rate, long_term_fee_rate, dust_fee_rate,
    knapsack_min_change, &coin_select_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_NE(nullptr, coin_select_handle);

  if (ret == kCfdSuccess) {
    for (int32_t index = 0; index < static_cast<int32_t>(utxos.size()); ++index) {
      const Utxo& utxo = utxos[index];
      ByteData txid = convert_to_byte(utxo.txid, sizeof(utxo.txid));
      ret = CfdAddCoinSelectionUtxo(
          handle, coin_select_handle, index,
          Txid(ByteData256(txid.GetBytes())).GetHex().c_str(), utxo.vout,
          utxo.amount,
          "",
          kDescriptor);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    ret = CfdAddCoinSelectionAmount(handle, coin_select_handle, 0, 180000000, "");
    EXPECT_EQ(kCfdSuccess, ret);

    int64_t utxo_fee_amount = 0;
    ret = CfdFinalizeCoinSelection(handle, coin_select_handle, &utxo_fee_amount);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(7360, utxo_fee_amount);

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
    EXPECT_EQ(4, indexes.size());

    if (indexes.size() == 4) {
      EXPECT_EQ(1, indexes[0]);
      EXPECT_EQ(3, indexes[1]);
      EXPECT_EQ(10, indexes[2]);
      EXPECT_EQ(6, indexes[3]);
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
    EXPECT_EQ(181760100, amount);
  }
  ret = CfdFreeCoinSelectionHandle(handle, coin_select_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

// SelectCoins(multi Asset) =====================================================

#ifndef CFD_DISABLE_ELEMENTS
TEST(cfdcapi_coin, CfCoinSelection_Asset1) {
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
  std::vector<Utxo> utxos = CfdGetElementsUtxoListByC(true);
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
    EXPECT_EQ(9200, utxo_fee_amount);

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
      static_cast<AddressType>(0),
      nullptr,
      ConfidentialAssetId("6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3"),
      ElementsConfidentialAddress(),
      BlindFactor(),
      BlindFactor(),
      ConfidentialValue(),
      Script()
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
      static_cast<AddressType>(0),
      nullptr,
      ConfidentialAssetId("6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3"),
      ElementsConfidentialAddress(),
      BlindFactor(),
      BlindFactor(),
      ConfidentialValue(),
      Script()
    }
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
          handle, fee_handle, utxo.txid.GetHex().c_str(), utxo.vout,
          utxo.descriptor.c_str(), utxo.asset.GetHex().c_str(),
          false, false, false, 0, nullptr);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    int64_t tx_fee, utxo_fee;
    ret = CfdFinalizeEstimateFee(
        handle, fee_handle, kTxData, kFeeAsset, &tx_fee, &utxo_fee, true, 1.0);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(static_cast<int64_t>(1802), tx_fee);
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

TEST(cfdcapi_coin, CfCoinSelection_Asset2) {
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
  std::vector<UtxoData> utxos = CfdGetElementsUtxoDataListByC(true);
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
      const auto& utxo = utxos[index];
      ret = CfdAddCoinSelectionUtxoTemplate(
          handle, coin_select_handle, index,
          utxo.txid.GetHex().c_str(), utxo.vout,
          utxo.amount.GetSatoshiValue(), utxo.asset.GetHex().c_str(),
          utxo.descriptor.c_str(), utxo.scriptsig_template.GetHex().c_str());
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
    EXPECT_EQ(9200, utxo_fee_amount);

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
