#include "gtest/gtest.h"

#include <map>
#include <string>
#include <vector>

#include "cfd/cfd_elements_transaction.h"
#include "cfd/cfd_elements_address.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfd/cfdapi_ledger.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_hdwallet.h"

#ifndef CFD_DISABLE_ELEMENTS
using cfd::ConfidentialTransactionContext;
using cfd::ConfidentialTransactionController;
using cfd::CoinSelectionOption;
using cfd::CoinSelection;
using cfd::IssuanceOutputParameter;
using cfd::UtxoData;
using cfd::UtxoFilter;
using cfd::ElementsAddressFactory;
using cfd::api::ElementsUtxoAndOption;
using cfd::api::ElementsTransactionApi;
using cfd::core::Amount;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::BlindFactor;
using cfd::core::ConfidentialAssetId;
using cfd::core::ConfidentialValue;
using cfd::core::ElementsConfidentialAddress;
using cfd::core::NetType;
using cfd::core::ExtPubkey;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::IssuanceParameter;
using cfd::core::OutPoint;
using cfd::core::Script;
using cfd::core::Txid;
using cfd::core::ConfidentialTxOutReference;
using cfd::core::CfdException;
using cfd::core::GetElementsAddressFormatList;



TEST(ElementsTransactionApi, EstimateFee_CheckRealValue)
{
  ElementsAddressFactory factory(NetType::kElementsRegtest);
  // Address1
  // pubkey: '0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b',
  // privkey: 'cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5'
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("4aa201f333e80b8f62ba5b593edb47b4730212e2917b21279f389ba1c14588a3");
  utxo1.vout = 0;
  utxo1.locking_script = Script("76a914f330ed8383f8afdc977dd88600eb8ff120ba15e488ac");
  // utxo1.redeem_script = Script("");
  utxo1.address = factory.GetAddress("2dwbdChKUXiSWECFEzLdmegbRtGAAHTC2ph");
  utxo1.descriptor = "pkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo1.amount = Amount(int64_t{10000000});
  utxo1.address_type = AddressType::kP2pkhAddress;
  utxo1.asset = ConfidentialAssetId("5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
  // utxo1.asset_blind_factor = BlindFactor("");
  // utxo1.amount_blind_factor = BlindFactor("");
  // utxo1.value_commitment = ConfidentialValue("");

  // Address2 (pattern-x1)
  // pubkey: '0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27',
  // privkey: 'cVtoSAzA814NCpEjz1Gumv2c5jCQ1f8Axcd58NDeds8Wxrn9dMVP'
  // blinding key: '66e4df5035a64acef16b4aa52ddc8bebd22b22c9eca150774e355abc72909d83'
  UtxoData utxo2;
  utxo2.block_height = 0;
  utxo2.binary_data = nullptr;
  utxo2.txid = Txid("31559192b619fd52b2cc0ca54d33778acae393ed31c453e29301a3919763b9e3");
  utxo2.vout = 0;
  utxo2.locking_script = Script("a9145d54db96a28f844a744e393fcd699d6f825b284187");
  // utxo1.redeem_script = Script("");
  utxo2.address = factory.GetAddress("XKrjM1JtrjasbbrdJ9Ci51dmkZ1DMxzPJE");
  utxo2.descriptor = "sh(wpkh(0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27))";
  utxo2.amount = Amount(int64_t{180000});
  utxo2.address_type = AddressType::kP2shP2wpkhAddress;
  utxo2.asset = ConfidentialAssetId("849cabdb3b0df8a05b97c5df0f2e2f891d5a94fccf6dbe9907ee34b477a1e735");
  // utxo2.confidential_address;
  utxo2.asset_blind_factor = BlindFactor("23b44363de1069842dff58ac6c965f75af71949e58d823d7bfcc963134db0b3e");
  utxo2.amount_blind_factor = BlindFactor("6dfe8c49f10c40cac178da4056dbf55eb11fa26e3169f41019e88252d86c42bc");
  utxo2.value_commitment = ConfidentialValue("09da970d871982c2405b2d4bff4f455e4907bedaf95c205f94015809f6bd9c59fa");

  // Address3
  // pubkey: '0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b',
  // privkey: 'cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5'
  UtxoData utxo3;
  utxo3.block_height = 0;
  utxo3.binary_data = nullptr;
  utxo3.txid = Txid("4aa201f333e80b8f62ba5b593edb47b4730212e2917b21279f389ba1c14588a3");
  utxo3.vout = 1;
  utxo3.locking_script = Script("0014f330ed8383f8afdc977dd88600eb8ff120ba15e4");
  // utxo1.redeem_script = Script("");
  utxo3.address = factory.GetAddress("ert1q7vcwmqurlzhae9mamzrqp6u07yst590yvg8j0w");
  utxo3.descriptor = "wpkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo3.amount = Amount(int64_t{10000000});
  utxo3.address_type = AddressType::kP2wpkhAddress;
  utxo3.asset = ConfidentialAssetId("5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  ElementsConfidentialAddress address("CTEkmnc7qLfRvwS5Z39KrrARxy8ugtXtgWrSMpqdbfk6am3ggeFikyDxDW5gU8WmTXtb2HWWjc23YXZz");
  // "2dbH8YS7rqRDM1F7EXrGBXvZywXxuEQtZ2z"
  ElementsConfidentialAddress asset_address("CTEkmnc7qLfRvwS5Z39KrrARxy8ugtXtgWrSMpqdbfk6am3VHXTF8dZAhKb1iBAnzYXEj527CSdfj2PS");

  Amount fee(int64_t{10000});
  ConfidentialTransactionContext txc(2, 0);
  EXPECT_NO_THROW(txc.AddInput(utxo1));
  txc.AddTxIn(OutPoint(utxo2.txid, utxo2.vout));
  txc.AddTxIn(OutPoint(utxo3.txid, utxo3.vout));
  txc.AddTxOut(address, utxo1.amount + utxo3.amount - fee, utxo1.asset);
  txc.AddTxOut(asset_address.GetUnblindedAddress(), utxo2.amount, utxo2.asset);
  txc.AddTxOutFee(fee, utxo1.asset);

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};
  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

  ElementsUtxoAndOption eutxo1;
  ElementsUtxoAndOption eutxo2;
  ElementsUtxoAndOption eutxo3;
  eutxo1.utxo = utxo1;
  eutxo2.utxo = utxo2;
  eutxo3.utxo = utxo3;
  std::vector<cfd::api::ElementsUtxoAndOption> utxos_and_options{
      eutxo1, eutxo2, eutxo3
  };

  // check estimateFee
  double effective_fee_rate = 1.0;
  Amount calc_fee;
  Amount utxo_fee;
  Amount tx_fee;
  ElementsTransactionApi api;
  calc_fee = api.EstimateFee(txc.GetHex(), utxos_and_options, utxo1.asset,
      &tx_fee, &utxo_fee, true, effective_fee_rate);
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 2768);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 2458);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 310);

  std::vector<cfd::core::ElementsConfidentialAddress> ct_addrs;
  ct_addrs.push_back(asset_address);
  EXPECT_NO_THROW(txc.Blind(&ct_addrs));
  std::vector<ConfidentialTxOutReference> txouts = txc.GetTxOutList();
  EXPECT_EQ(txouts.size(), 3);
  if (txouts.size() == 3) {
    EXPECT_TRUE(txouts[0].GetConfidentialValue().HasBlinding());
    EXPECT_TRUE(txouts[1].GetConfidentialValue().HasBlinding());
    EXPECT_FALSE(txouts[2].GetConfidentialValue().HasBlinding());
  }

  ByteData tx;

  EXPECT_NO_THROW(txc.SignWithKey(OutPoint(utxo1.txid, utxo1.vout),
      Pubkey("0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b"),
      Privkey::FromWif(
          "cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5", NetType::kTestnet)));
  EXPECT_NO_THROW(txc.Verify(OutPoint(utxo1.txid, utxo1.vout)));

  EXPECT_THROW((tx = txc.Finalize()), CfdException);

  EXPECT_NO_THROW(txc.SignWithKey(OutPoint(utxo2.txid, utxo2.vout),
      Pubkey("0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"),
      Privkey::FromWif(
          "cVtoSAzA814NCpEjz1Gumv2c5jCQ1f8Axcd58NDeds8Wxrn9dMVP", NetType::kTestnet)));
  EXPECT_NO_THROW(txc.Verify(OutPoint(utxo2.txid, utxo2.vout)));

  EXPECT_THROW((tx = txc.Finalize()), CfdException);

  EXPECT_NO_THROW(txc.SignWithKey(OutPoint(utxo3.txid, utxo3.vout),
      Pubkey("0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b"),
      Privkey::FromWif(
          "cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5", NetType::kTestnet)));
  EXPECT_NO_THROW(txc.Verify(OutPoint(utxo3.txid, utxo3.vout)));

  EXPECT_NO_THROW(tx = txc.Finalize());

  if ((tx.GetDataSize() != 9400) && (tx.GetDataSize() != 9401) && (tx.GetDataSize() != 9402)) {
    EXPECT_EQ(tx.GetDataSize(), 0);
  }

  EXPECT_EQ(txc.GetVsize(), 2768);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate);
  EXPECT_EQ(minimum_fee, 2768);
  // EXPECT_STREQ(tx.GetHex().c_str(), "");
}


struct TestFundElementsUtxoVector {
  std::string txid;
  uint32_t vout;
  int64_t amount;
  std::string asset;
  std::string descriptor;
};
static ConfidentialAssetId exp_dummy_asset_a("aa00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_b("bb00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_c("cc00000000000000000000000000000000000000000000000000000000000000");

static const std::vector<TestFundElementsUtxoVector> kFundCoinSelectElementsTestVector = {
  {
    "7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a",
    0,
    155062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a",
    0,
    85062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a",
    0,
    39062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a",
    0,
    61062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a",
    0,
    15675000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b",
    0,
    14938590,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b01",
    0,
    26918400,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b02",
    0,
    750000,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b03",
    0,
    346430050,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b04",
    0,
    18476350,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000c01",
    0,
    37654200,
    exp_dummy_asset_c.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000c02",
    0,
    127030000,
    exp_dummy_asset_c.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x"
  }
};




TEST(ElementsTransactionApi, FundRawTransaction_Reissueasset) {
  static const char* const kExpTxData = "0200000000030f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c34600000bfa8774c5f753ce2f801a8106413b470af94edbff5b4242ed4c5a26d20e72b90000000000ffffffff040b0000000000000000000000000000000000000000000000000000000000000000000000ffffffff07010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b92700001976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac01cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c34600001976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0100000000000000000000000000000000000000000000000000000000000000aa010000000000989680001600144352a1a6e86311f22274f7ebb2746de21b09b15d0100000000000000000000000000000000000000000000000000000000000000bb01000000000007a120001600148beaaac4654cf4ebd8e46ca5062b0e7fb3e7470c0100000000000000000000000000000000000000000000000000000000000000aa01000000000000037000000100000000000000000000000000000000000000000000000000000000000000bb010000000001124c1e00160014a53be40113bb50f2b8b2d0bfea1e823e75632b5f0100000000000000000000000000000000000000000000000000000000000000aa0100000000004b57ee0016001478eb9fc2c9e1cdf633ecb646858ba862b21384ab00000000";

  ElementsUtxoAndOption input_utxo;
  ConfidentialTransactionContext tx(uint32_t{2}, uint32_t{0});
  std::vector<UtxoData> utxos;
  ElementsAddressFactory factory(NetType::kElementsRegtest);
  std::string tokenAsset = "ed6927df918c89b5e3d8b5062acab2c749a3291bb7451d4267c7daaf1b52ad0b";
  std::string descMulti = "wsh(multi(2,03a0434d9e47f3c86235477c7b1ae6ae5d3442d49b1943c2b752a68e2a47e247c7,03774ae7f858a9411e5ef4246b70c65aac5649980be5c17891bbec17895da008cb,03d01115d548e7561b15c38f004d734633687cf4419620095bc5b0f47070afe85a))";

  try {
    utxos.resize(kFundCoinSelectElementsTestVector.size());
    uint32_t i = 0;
    for (const auto& test_data : kFundCoinSelectElementsTestVector) {
      Txid txid;
      if (!test_data.txid.empty()) {
        txid = Txid(test_data.txid);
      }
      utxos[i].txid = Txid(test_data.txid);
      utxos[i].vout = test_data.vout;
      utxos[i].amount = Amount(test_data.amount);
      utxos[i].asset = ConfidentialAssetId(test_data.asset);
      utxos[i].descriptor = test_data.descriptor;
      ++i;
    }

    // create reissue tx
    input_utxo.is_issuance = true;
    input_utxo.is_blind_issuance = true;
    input_utxo.utxo.txid = Txid("57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f");
    input_utxo.utxo.vout = 1;
    input_utxo.utxo.descriptor = descMulti;
    input_utxo.utxo.binary_data = nullptr;
    input_utxo.utxo.asset = ConfidentialAssetId(tokenAsset);
    input_utxo.utxo.asset_blind_factor =
      BlindFactor("0b8954757234fd3ec9cf0dd6ef0a89d825ec56a9532e7da4b6cb90c51be3bbd8");
    input_utxo.utxo.amount_blind_factor =
      BlindFactor("62e36e1f0fa4916b031648a6b6903083069fa587572a88b729250cde528cfd3b");
    input_utxo.utxo.amount = Amount(int64_t{700000000});
    input_utxo.utxo.address_type = AddressType::kP2wshAddress;
    tx.AddInput(input_utxo.utxo);

    Address addr = factory.GetAddress("2djHX9wtrtdyGw9cer1u6zB6Yq4SRD8V5zw");
    tx.AddTxOut(addr, Amount(int64_t{700000000}), ConfidentialAssetId(tokenAsset));
    IssuanceOutputParameter reissue_data;
    reissue_data.address = factory.GetAddress("2dodsWJgP3pTWWidK5hDxuYHqC1U4CEnT3n");
    reissue_data.amount =  Amount(int64_t{600000000});
    // IssuanceParameter issue;
    tx.SetAssetReissuance(
        OutPoint(input_utxo.utxo.txid, input_utxo.utxo.vout), reissue_data.amount,
        reissue_data, input_utxo.utxo.asset_blind_factor,
        BlindFactor("6f9ccf5949eba5d6a08bff7a015e825c97824e82d57c8a0c77f9a41908fe8306"));
    // output.WriteLine("issue asset: " + issueAsset.ToHexString());

    // add txout
    ExtPubkey key = ExtPubkey("xpub661MyMwAqRbcGB88KaFbLGiYAat55APKhtWg4uYMkXAmfuSTbq2QYsn9sKJCj1YqZPafsboef4h4YbXXhNhPwMbkHTpkf3zLhx7HvFw1NDy");
    Address set_addr1 = factory.CreateP2wpkhAddress(key.DerivePubkey(11).GetPubkey());
    Address set_addr2 = factory.CreateP2wpkhAddress(key.DerivePubkey(12).GetPubkey());
    tx.AddTxOut(set_addr1, Amount(int64_t{10000000}), exp_dummy_asset_a);
    tx.AddTxOut(set_addr2, Amount(int64_t{500000}), exp_dummy_asset_b);

    ConfidentialAssetId fee_asset = exp_dummy_asset_a;
    std::map<std::string, Amount> map_target_value;
    map_target_value.emplace(exp_dummy_asset_a.GetHex(), Amount(int64_t{0}));
    map_target_value.emplace(exp_dummy_asset_b.GetHex(), Amount(int64_t{0}));
    Address addr1 = factory.CreateP2wpkhAddress(key.DerivePubkey(1).GetPubkey());
    Address addr2 = factory.CreateP2wpkhAddress(key.DerivePubkey(2).GetPubkey());
    std::map<std::string, std::string> reserve_txout_address;
    reserve_txout_address.emplace(exp_dummy_asset_a.GetHex(), addr1.GetAddress());
    reserve_txout_address.emplace(exp_dummy_asset_b.GetHex(), addr2.GetAddress());
    std::vector<ElementsUtxoAndOption> selected_txin_utxos = {input_utxo};
    double fee_rate = 0.1;
    Amount estimate_fee;
    UtxoFilter filter;
    std::vector<std::string> append_txout_addresses;
    CoinSelectionOption option;
    option.InitializeConfidentialTxSizeInfo();
    option.SetEffectiveFeeBaserate(fee_rate);
    option.SetLongTermFeeBaserate(fee_rate);
    option.SetFeeAsset(fee_asset);

    ElementsTransactionApi api;
    ConfidentialTransactionController ctx = api.FundRawTransaction(
        tx.GetHex(), utxos, map_target_value, selected_txin_utxos,
        reserve_txout_address, fee_asset, true, fee_rate, &estimate_fee,
        &filter, &option, &append_txout_addresses, NetType::kElementsRegtest);

    EXPECT_STREQ(kExpTxData, ctx.GetHex().c_str());
    EXPECT_EQ(880, estimate_fee.GetSatoshiValue());
    EXPECT_EQ(size_t{2}, append_txout_addresses.size());
    if (append_txout_addresses.size() == size_t{2})
    {
      EXPECT_STREQ(addr1.GetAddress().c_str(), append_txout_addresses[1].c_str());
      EXPECT_STREQ(addr2.GetAddress().c_str(), append_txout_addresses[0].c_str());
    }
  } catch (const CfdException& except) {
    EXPECT_STREQ("", except.what());
    return;
  }
}

#endif  // CFD_DISABLE_ELEMENTS
