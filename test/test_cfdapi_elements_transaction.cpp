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
#include "cfdcore/cfdcore_elements_address.h"
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

  if ((txc.GetVsize() != 2768) && (txc.GetVsize() != 2767)) {
    EXPECT_EQ(txc.GetVsize(), 2768);
  }

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate);
  if ((minimum_fee != 2768) && (minimum_fee != 2767)) {
    EXPECT_EQ(minimum_fee, 2768);
  }
  // EXPECT_STREQ(tx.GetHex().c_str(), "");
}


TEST(ElementsTransactionApi, EstimateFee_MinimumValue)
{
  ElementsAddressFactory factory(NetType::kElementsRegtest);
  std::string tx_hex = "02000000000125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a0000000000ffffffff020125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000000005f5e100036a2e218bb512a3e65c80b59fec57aee428b7512276bcfa366c154dd7262994c817a914363273e2f851bda01e24cda41ba748b8d1f54cfe870125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a0100000000000000c8000000000000";

  // Address1
  // pubkey: '0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b',
  // privkey: 'cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5'
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
  utxo1.vout = 0;
  utxo1.locking_script = Script("0014eb3c0d55b7098a4aef4a18ee1eebcb1ed924a82b");
  // utxo1.redeem_script = Script("");
  utxo1.address = factory.GetAddress("ert1qav7q64dhpx9y4m62rrhpa67trmvjf2ptxfddld");
  utxo1.descriptor = "wpkh(03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6)";
  utxo1.amount = Amount(int64_t{2000000000});
  utxo1.address_type = AddressType::kP2wpkhAddress;
  utxo1.asset = ConfidentialAssetId("5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
  utxo1.asset_blind_factor = BlindFactor("ebfecaae1665f32a3843ce65c42fb6e3f51136fa9d37274b810887923ae89339");
  utxo1.amount_blind_factor = BlindFactor("80af7bd339db43ad22c1fa9109eea6d617c8b87b91c4bde2b5fafcbb1902211a");
  utxo1.value_commitment = ConfidentialValue("085e6338f9da8a7f754b8e2726894e04bee997c8ada526f3215de8bc151aa063d3");
  ElementsUtxoAndOption eutxo1;
  eutxo1.utxo = utxo1;
  std::vector<cfd::api::ElementsUtxoAndOption> utxos_and_options{
      eutxo1
  };

  ConfidentialTransactionContext txc(tx_hex);

  std::vector<cfd::UtxoData> utxos{utxo1};
  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

  // check estimateFee
  double effective_fee_rate = 0.1;
  uint64_t effective_fee_rate2 = 100;
  Amount calc_fee;
  Amount utxo_fee;
  Amount tx_fee;
  ElementsTransactionApi api;
  calc_fee = api.EstimateFee(txc.GetHex(), utxos_and_options, utxo1.asset,
      &tx_fee, &utxo_fee, true, effective_fee_rate, 0, 36);
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 99);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 92);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 7);

  std::vector<cfd::core::ElementsConfidentialAddress> ct_addrs;
  EXPECT_NO_THROW(txc.Blind(&ct_addrs, 1, 0, 36));
  std::vector<ConfidentialTxOutReference> txouts = txc.GetTxOutList();
  EXPECT_EQ(txouts.size(), 2);
  if (txouts.size() == 2) {
    EXPECT_TRUE(txouts[0].GetConfidentialValue().HasBlinding());
    EXPECT_FALSE(txouts[1].GetConfidentialValue().HasBlinding());
  }

  ByteData tx;

  EXPECT_NO_THROW(txc.SignWithKey(OutPoint(utxo1.txid, utxo1.vout),
      Pubkey("03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6"),
      Privkey::FromWif(
          "cU4KjNUT7GjHm7CkjRjG46SzLrXHXoH3ekXmqa2jTCFPMkQ64sw1", NetType::kTestnet)));
  EXPECT_NO_THROW(txc.Verify(OutPoint(utxo1.txid, utxo1.vout)));

  EXPECT_NO_THROW(tx = txc.Finalize());

  if ((tx.GetDataSize() != 3295) && (tx.GetDataSize() != 3294) && (tx.GetDataSize() != 3296)) {
    EXPECT_EQ(tx.GetDataSize(), 0);
  }

  if ((txc.GetVsize() != 988) && (txc.GetVsize() != 987)) {
    EXPECT_EQ(txc.GetVsize(), 988);
  }

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate2) / 1000;
  if ((minimum_fee != 98) && (minimum_fee != 97)) {
    EXPECT_EQ(minimum_fee, 98);
  }
  // EXPECT_STREQ(tx.GetHex().c_str(), "");
}


TEST(ElementsTransactionApi, EstimateFee_LargeAmount_MinBits36)
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
  utxo1.amount = Amount(int64_t{100000000000000});
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
  utxo2.amount = Amount(int64_t{18000000000000});
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
  utxo3.amount = Amount(int64_t{100000000000000});
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
  double effective_fee_rate = 0.1;
  uint64_t effective_fee_rate2 = 100;
  Amount calc_fee;
  Amount utxo_fee;
  Amount tx_fee;
  ElementsTransactionApi api;
  calc_fee = api.EstimateFee(txc.GetHex(), utxos_and_options, utxo1.asset,
      &tx_fee, &utxo_fee, true, effective_fee_rate, 0, 36);
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 256);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 225);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 31);

  std::vector<cfd::core::ElementsConfidentialAddress> ct_addrs;
  ct_addrs.push_back(asset_address);
  EXPECT_NO_THROW(txc.Blind(&ct_addrs, 1, 0, 36));
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

  if ((tx.GetDataSize() != 8533) && (tx.GetDataSize() != 8534) &&
      (tx.GetDataSize() != 8535) && (tx.GetDataSize() != 8536)) {
    EXPECT_EQ(tx.GetDataSize(), 0);
  }

  if ((txc.GetVsize() != 2551) && (txc.GetVsize() != 2552) && (txc.GetVsize() != 2553)) {
    EXPECT_EQ(txc.GetVsize(), 2552);
  }

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate2) / 1000;
  EXPECT_EQ(minimum_fee, 255);
  if ((calc_fee.GetSatoshiValue() != minimum_fee) &&
      (calc_fee.GetSatoshiValue() != (minimum_fee + 1))) {
    EXPECT_EQ(calc_fee.GetSatoshiValue(), minimum_fee);
  }
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
  static const char* const kExpTxData = "0200000000030f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c34600000bfa8774c5f753ce2f801a8106413b470af94edbff5b4242ed4c5a26d20e72b90000000000ffffffff040b0000000000000000000000000000000000000000000000000000000000000000000000ffffffff07010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b92700001976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac01cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c34600001976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0100000000000000000000000000000000000000000000000000000000000000aa010000000000989680001600144352a1a6e86311f22274f7ebb2746de21b09b15d0100000000000000000000000000000000000000000000000000000000000000bb01000000000007a120001600148beaaac4654cf4ebd8e46ca5062b0e7fb3e7470c0100000000000000000000000000000000000000000000000000000000000000aa01000000000000037300000100000000000000000000000000000000000000000000000000000000000000bb010000000001124c1e00160014a53be40113bb50f2b8b2d0bfea1e823e75632b5f0100000000000000000000000000000000000000000000000000000000000000aa0100000000004b57eb0016001478eb9fc2c9e1cdf633ecb646858ba862b21384ab00000000";

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
    // option.SetBlindInfo(0, 36);

    ElementsTransactionApi api;
    ConfidentialTransactionController ctx = api.FundRawTransaction(
        tx.GetHex(), utxos, map_target_value, selected_txin_utxos,
        reserve_txout_address, fee_asset, true, fee_rate, &estimate_fee,
        &filter, &option, &append_txout_addresses, NetType::kElementsRegtest);

    EXPECT_STREQ(kExpTxData, ctx.GetHex().c_str());
    EXPECT_EQ(883, estimate_fee.GetSatoshiValue());
    // EXPECT_EQ(657, estimate_fee.GetSatoshiValue());
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

static const std::vector<TestFundElementsUtxoVector> kFundCoinSelectElementsTestVector2 = {
  {
    "7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a",
    0,
    155062500000000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a",
    0,
    85062500000000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a",
    0,
    39062500000000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a",
    0,
    61062500000000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a",
    0,
    15675000000000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b",
    0,
    14938590000000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b01",
    0,
    26918400000000,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b02",
    0,
    750000000000,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b03",
    0,
    34643005000000,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000b04",
    0,
    18476350000000,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000c01",
    0,
    37654200000000,
    exp_dummy_asset_c.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  },
  {
    "0000000000000000000000000000000000000000000000000000000000000c02",
    0,
    127030000000000,
    exp_dummy_asset_c.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))"
  }
};

TEST(ElementsTransactionApi, FundRawTransaction_MinBits36) {
  static const char* const kExpTxData = "0200000000040a503dbd4f8f2b064c70e048b21f93fe4584174478abf5f44747932cd21da87c0000000000ffffffff0a9a33750a810cd384ca5d93b09513f1eb5d93c669091b29eef710d2391ff7300000000000ffffffff0ad4a335556c64c3e2599c3a4c3ddff5b28f616fa55cf2323d2ae642eef74a8f0000000000ffffffff020b0000000000000000000000000000000000000000000000000000000000000000000000ffffffff070100000000000000000000000000000000000000000000000000000000000000aa01000110d9316ec00003b86ca86f23bc47e6ae1eebf2dea191f0ab98b89e2d5b6e5e8de20631441caeaa1600144352a1a6e86311f22274f7ebb2746de21b09b15d0100000000000000000000000000000000000000000000000000000000000000bb010000000ba43b740003a9dda6bdef90309c4fb5fe9131e396e9f9b32dc1b8defea451f71e6af1127b1d1600148beaaac4654cf4ebd8e46ca5062b0e7fb3e7470c0100000000000000000000000000000000000000000000000000000000000000aa010000000ba43b740003fef01583b95b37ee4f23d3673dfcc01faa4654272c289e8e3caa6aeb31f634741600144852255c0ff013e835689f30fb52508f89b6fbe20100000000000000000000000000000000000000000000000000000000000000aa010000000ba43b7400037703a4ef463f044f462f35c4dece9d1f02d32681079c2c01cea4d6267dd782ef1600143159e9a1f6cdd588b6d5df65c3c7a190ecf0fba70100000000000000000000000000000000000000000000000000000000000000aa01000000000000026b00000100000000000000000000000000000000000000000000000000000000000000bb01000000a2fb40580000160014a53be40113bb50f2b8b2d0bfea1e823e75632b5f0100000000000000000000000000000000000000000000000000000000000000aa01000000fd340d18950016001478eb9fc2c9e1cdf633ecb646858ba862b21384ab00000000";

  ConfidentialTransactionContext tx(uint32_t{2}, uint32_t{0});
  std::vector<UtxoData> utxos;
  ElementsAddressFactory factory(NetType::kElementsRegtest);
  std::string descMulti = "wsh(multi(2,03a0434d9e47f3c86235477c7b1ae6ae5d3442d49b1943c2b752a68e2a47e247c7,03774ae7f858a9411e5ef4246b70c65aac5649980be5c17891bbec17895da008cb,03d01115d548e7561b15c38f004d734633687cf4419620095bc5b0f47070afe85a))";

  try {
    utxos.resize(kFundCoinSelectElementsTestVector2.size());
    uint32_t i = 0;
    for (const auto& test_data : kFundCoinSelectElementsTestVector2) {
      Txid txid;
      if (!test_data.txid.empty()) {
        txid = Txid(test_data.txid);
      }
      utxos[i].txid = Txid(test_data.txid);
      utxos[i].vout = test_data.vout;
      utxos[i].amount = Amount(test_data.amount);
      utxos[i].asset = ConfidentialAssetId(test_data.asset);
      utxos[i].descriptor = test_data.descriptor;
      utxos[i].address_type = AddressType::kP2shP2wpkhAddress;
      ++i;
    }

    // add txout
    ExtPubkey key = ExtPubkey("xpub661MyMwAqRbcGB88KaFbLGiYAat55APKhtWg4uYMkXAmfuSTbq2QYsn9sKJCj1YqZPafsboef4h4YbXXhNhPwMbkHTpkf3zLhx7HvFw1NDy");
    Address set_addr1 = factory.CreateP2wpkhAddress(key.DerivePubkey(11).GetPubkey());
    Address set_addr2 = factory.CreateP2wpkhAddress(key.DerivePubkey(12).GetPubkey());
    Address set_addr3 = factory.CreateP2wpkhAddress(key.DerivePubkey(13).GetPubkey());
    Address set_addr4 = factory.CreateP2wpkhAddress(key.DerivePubkey(14).GetPubkey());
    ElementsConfidentialAddress ct_addr1 = ElementsConfidentialAddress(
        set_addr1, key.DerivePubkey(111).GetPubkey());
    ElementsConfidentialAddress ct_addr2 = ElementsConfidentialAddress(
        set_addr2, key.DerivePubkey(112).GetPubkey());
    ElementsConfidentialAddress ct_addr3 = ElementsConfidentialAddress(
        set_addr3, key.DerivePubkey(113).GetPubkey());
    ElementsConfidentialAddress ct_addr4 = ElementsConfidentialAddress(
        set_addr4, key.DerivePubkey(114).GetPubkey());
    tx.AddTxOut(ct_addr1, Amount(int64_t{300000000000000}), exp_dummy_asset_a);
    tx.AddTxOut(ct_addr2, Amount(int64_t{50000000000}), exp_dummy_asset_b);
    tx.AddTxOut(ct_addr3, Amount(int64_t{50000000000}), exp_dummy_asset_a);
    tx.AddTxOut(ct_addr4, Amount(int64_t{50000000000}), exp_dummy_asset_a);

    ConfidentialAssetId fee_asset = exp_dummy_asset_a;
    std::map<std::string, Amount> map_target_value;
    map_target_value.emplace(exp_dummy_asset_a.GetHex(), Amount(int64_t{0}));
    map_target_value.emplace(exp_dummy_asset_b.GetHex(), Amount(int64_t{0}));
    Address addr1 = factory.CreateP2wpkhAddress(key.DerivePubkey(1).GetPubkey());
    Address addr2 = factory.CreateP2wpkhAddress(key.DerivePubkey(2).GetPubkey());
    std::map<std::string, std::string> reserve_txout_address;
    reserve_txout_address.emplace(exp_dummy_asset_a.GetHex(), addr1.GetAddress());
    reserve_txout_address.emplace(exp_dummy_asset_b.GetHex(), addr2.GetAddress());
    ElementsConfidentialAddress reserve_ct_addr1 = ElementsConfidentialAddress(
        addr1, key.DerivePubkey(91).GetPubkey());
    ElementsConfidentialAddress reserve_ct_addr2 = ElementsConfidentialAddress(
        addr2, key.DerivePubkey(92).GetPubkey());
    std::vector<ElementsUtxoAndOption> selected_txin_utxos;
    double fee_rate = 0.1;
    Amount estimate_fee;
    UtxoFilter filter;
    std::vector<std::string> append_txout_addresses;
    CoinSelectionOption option;
    option.InitializeConfidentialTxSizeInfo();
    option.SetEffectiveFeeBaserate(fee_rate);
    option.SetLongTermFeeBaserate(fee_rate);
    option.SetFeeAsset(fee_asset);
    option.SetBlindInfo(0, 36);

    ElementsTransactionApi api;
    ConfidentialTransactionController ctx = api.FundRawTransaction(
        tx.GetHex(), utxos, map_target_value, selected_txin_utxos,
        reserve_txout_address, fee_asset, true, fee_rate, &estimate_fee,
        &filter, &option, &append_txout_addresses, NetType::kElementsRegtest);

    EXPECT_STREQ(kExpTxData, ctx.GetHex().c_str());
    EXPECT_EQ(619, estimate_fee.GetSatoshiValue());
    EXPECT_EQ(size_t{2}, append_txout_addresses.size());
    if (append_txout_addresses.size() == size_t{2})
    {
      EXPECT_STREQ(addr1.GetAddress().c_str(), append_txout_addresses[1].c_str());
      EXPECT_STREQ(addr2.GetAddress().c_str(), append_txout_addresses[0].c_str());
    }

    ConfidentialTransactionContext context(ctx.GetHex());
    std::vector<cfd::core::ElementsConfidentialAddress> ct_addrs;
    ct_addrs.push_back(reserve_ct_addr1);
    ct_addrs.push_back(reserve_ct_addr2);
    context.CollectInputUtxo(utxos);
    context.Blind(&ct_addrs, 1, 0, 36);
    for (const auto& txin : context.GetTxInList()) {
      context.SignWithKey(OutPoint(txin.GetTxid(), txin.GetVout()),
          Pubkey("0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b"),
          Privkey::FromWif(
              "cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5", NetType::kTestnet));
    }
    EXPECT_EQ(6120, context.GetVsize());
    // EXPECT_STREQ("", context.GetHex().c_str());
  } catch (const CfdException& except) {
    EXPECT_STREQ("", except.what());
    return;
  }
}


TEST(ElementsTransactionApi, FundRawTransaction_MillionAmountValue) {
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
  utxo1.amount = Amount(int64_t{300000000000000});
  utxo1.address_type = AddressType::kP2pkhAddress;
  utxo1.asset = exp_dummy_asset_a;

  // Address2 (pattern-x1)
  // pubkey: '0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27',
  // privkey: 'cVtoSAzA814NCpEjz1Gumv2c5jCQ1f8Axcd58NDeds8Wxrn9dMVP'
  UtxoData utxo2;
  utxo2.block_height = 0;
  utxo2.binary_data = nullptr;
  utxo2.txid = Txid("31559192b619fd52b2cc0ca54d33778acae393ed31c453e29301a3919763b9e3");
  utxo2.vout = 0;
  utxo2.locking_script = Script("a9145d54db96a28f844a744e393fcd699d6f825b284187");
  // utxo1.redeem_script = Script("");
  utxo2.address = factory.GetAddress("XKrjM1JtrjasbbrdJ9Ci51dmkZ1DMxzPJE");
  utxo2.descriptor = "sh(wpkh(0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27))";
  utxo2.amount = Amount(int64_t{400000000000000});
  utxo2.address_type = AddressType::kP2shP2wpkhAddress;
  utxo2.asset = exp_dummy_asset_a;

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
  utxo3.amount = Amount(int64_t{300000000000000});
  utxo3.address_type = AddressType::kP2wpkhAddress;
  utxo3.asset = exp_dummy_asset_a;

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  Address address1 = factory.GetAddress("2dwbdChKUXiSWECFEzLdmegbRtGAAHTC2ph");
  // "0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"
  Address address2 = factory.GetAddress("ert1qg4nrukf07cf4slc0m4h8gq6v2guhzrw2ayudpu");
  Address address3 = factory.GetAddress("ert1q7vcwmqurlzhae9mamzrqp6u07yst590yvg8j0w");

  ExtPubkey key = ExtPubkey("xpub661MyMwAqRbcGB88KaFbLGiYAat55APKhtWg4uYMkXAmfuSTbq2QYsn9sKJCj1YqZPafsboef4h4YbXXhNhPwMbkHTpkf3zLhx7HvFw1NDy");

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};

  ElementsConfidentialAddress reserve_ct_addr1 = ElementsConfidentialAddress(
      address1, key.DerivePubkey(91).GetPubkey());
  ElementsConfidentialAddress reserve_ct_addr2 = ElementsConfidentialAddress(
      address2, key.DerivePubkey(92).GetPubkey());
  ElementsConfidentialAddress reserve_ct_addr3 = ElementsConfidentialAddress(
      address3, key.DerivePubkey(93).GetPubkey());

  double fee_rate = 0.11;
  ConfidentialAssetId fee_asset = exp_dummy_asset_a;
  std::map<std::string, Amount> map_target_value;
  map_target_value.emplace(exp_dummy_asset_a.GetHex(), Amount(int64_t{0}));
  std::map<std::string, std::string> reserve_txout_address;
  reserve_txout_address.emplace(exp_dummy_asset_a.GetHex(), reserve_ct_addr3.GetAddress());
  std::vector<ElementsUtxoAndOption> selected_txin_utxos;
  Amount estimate_fee;
  UtxoFilter filter;
  std::vector<std::string> append_txout_addresses;
  CoinSelectionOption option;
  option.InitializeConfidentialTxSizeInfo();
  option.SetEffectiveFeeBaserate(fee_rate);
  option.SetLongTermFeeBaserate(fee_rate);
  option.SetFeeAsset(fee_asset);
  option.SetBlindInfo(0, 52);

  Amount fee(int64_t{10000});
  ConfidentialTransactionContext txc(2, 0);
  txc.AddTxOut(reserve_ct_addr1, utxo1.amount + utxo3.amount - fee, exp_dummy_asset_a);
  txc.AddTxOut(reserve_ct_addr2, utxo2.amount, exp_dummy_asset_a);

  ElementsTransactionApi api;
  ConfidentialTransactionController ctx = api.FundRawTransaction(
      txc.GetHex(), utxos, map_target_value, selected_txin_utxos,
      reserve_txout_address, fee_asset, true, fee_rate, &estimate_fee,
      &filter, &option, &append_txout_addresses, NetType::kElementsRegtest);

  txc = ConfidentialTransactionContext(ctx.GetHex());
  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

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
  EXPECT_EQ(txc.GetFeeAmount().GetSatoshiValue(), estimate_fee.GetSatoshiValue());
  EXPECT_STREQ(tx.GetHex().c_str(), "020000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a4730440220767c6f9a83ea898b84385875615c7383f43ab2b7634123b870e63f399708647b022041d0331db0c5feef5780350358e3d0086a8d871972bd7f8fae3be3cc339d5cd601210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff040100000000000000000000000000000000000000000000000000000000000000aa01000221b262dd58f00308f34c0c7de1a49ec53acb73aeb94306a90987759c2c012492e877a0dde821d91976a914f330ed8383f8afdc977dd88600eb8ff120ba15e488ac0100000000000000000000000000000000000000000000000000000000000000aa0100016bcc41e9000002761f92316d668b7ad54ea7da8f4bfd1b266d677e9786b98274f647cc74ae306e16001445663e592ff613587f0fdd6e74034c5239710dca0100000000000000000000000000000000000000000000000000000000000000aa0100000000000001c200000100000000000000000000000000000000000000000000000000000000000000aa01000000000000254e029e1f4cd73bb6d29257ca37a3882cd09661783c90cdd92b57264a13bdab822905160014f330ed8383f8afdc977dd88600eb8ff120ba15e40000000000000247304402201ce591d9f01461db910530e15defa047c99915f12b14012ff5faf27f5fa3ab8202204823faa9f4567135057274a9ec6a25b549aa4eda504c73f770bf83fa19d155fd01210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a270000000247304402201973ebe5dae51bf6e2e642f7375cec057b2ed18b427ebf49bc9754f0369b6ce302206e07c9a91706b5dfa05778ed3b4f7be5f7c655d861f28b4d54a375704dba155a01210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b00000000000000000000000000");
  EXPECT_EQ(txc.GetVsize(), 662);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(fee_rate * 100) / 100;
  EXPECT_EQ(minimum_fee, 72);
}


TEST(ElementsTransactionApi, FundRawTransaction_LimitAmountValue) {
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
  utxo1.amount = Amount(int64_t{50000000000000});
  utxo1.address_type = AddressType::kP2pkhAddress;
  utxo1.asset = exp_dummy_asset_a;

  // Address2 (pattern-x1)
  // pubkey: '0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27',
  // privkey: 'cVtoSAzA814NCpEjz1Gumv2c5jCQ1f8Axcd58NDeds8Wxrn9dMVP'
  UtxoData utxo2;
  utxo2.block_height = 0;
  utxo2.binary_data = nullptr;
  utxo2.txid = Txid("31559192b619fd52b2cc0ca54d33778acae393ed31c453e29301a3919763b9e3");
  utxo2.vout = 0;
  utxo2.locking_script = Script("a9145d54db96a28f844a744e393fcd699d6f825b284187");
  // utxo1.redeem_script = Script("");
  utxo2.address = factory.GetAddress("XKrjM1JtrjasbbrdJ9Ci51dmkZ1DMxzPJE");
  utxo2.descriptor = "sh(wpkh(0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27))";
  utxo2.amount = Amount(int64_t{2000000000000000});
  utxo2.address_type = AddressType::kP2shP2wpkhAddress;
  utxo2.asset = exp_dummy_asset_a;

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
  utxo3.amount = Amount(int64_t{50000000000000});
  utxo3.address_type = AddressType::kP2wpkhAddress;
  utxo3.asset = exp_dummy_asset_a;

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  Address address1 = factory.GetAddress("2dwbdChKUXiSWECFEzLdmegbRtGAAHTC2ph");
  // "0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"
  Address address2 = factory.GetAddress("ert1qg4nrukf07cf4slc0m4h8gq6v2guhzrw2ayudpu");
  Address address3 = factory.GetAddress("ert1q7vcwmqurlzhae9mamzrqp6u07yst590yvg8j0w");

  ExtPubkey key = ExtPubkey("xpub661MyMwAqRbcGB88KaFbLGiYAat55APKhtWg4uYMkXAmfuSTbq2QYsn9sKJCj1YqZPafsboef4h4YbXXhNhPwMbkHTpkf3zLhx7HvFw1NDy");

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};

  ElementsConfidentialAddress reserve_ct_addr1 = ElementsConfidentialAddress(
      address1, key.DerivePubkey(91).GetPubkey());
  ElementsConfidentialAddress reserve_ct_addr2 = ElementsConfidentialAddress(
      address2, key.DerivePubkey(92).GetPubkey());
  ElementsConfidentialAddress reserve_ct_addr3 = ElementsConfidentialAddress(
      address3, key.DerivePubkey(93).GetPubkey());

  double fee_rate = 0.11;
  ConfidentialAssetId fee_asset = exp_dummy_asset_a;
  std::map<std::string, Amount> map_target_value;
  map_target_value.emplace(exp_dummy_asset_a.GetHex(), Amount(int64_t{0}));
  std::map<std::string, std::string> reserve_txout_address;
  reserve_txout_address.emplace(exp_dummy_asset_a.GetHex(), reserve_ct_addr3.GetAddress());
  std::vector<ElementsUtxoAndOption> selected_txin_utxos;
  Amount estimate_fee;
  UtxoFilter filter;
  std::vector<std::string> append_txout_addresses;
  CoinSelectionOption option;
  option.InitializeConfidentialTxSizeInfo();
  option.SetEffectiveFeeBaserate(fee_rate);
  option.SetLongTermFeeBaserate(fee_rate);
  option.SetFeeAsset(fee_asset);
  option.SetBlindInfo(0, 52);

  Amount fee(int64_t{10000});
  ConfidentialTransactionContext txc(2, 0);
  txc.AddTxOut(reserve_ct_addr1, utxo1.amount + utxo3.amount - fee, exp_dummy_asset_a);
  txc.AddTxOut(reserve_ct_addr2, utxo2.amount, exp_dummy_asset_a);

  try {
    ElementsTransactionApi api;
    ConfidentialTransactionController ctx = api.FundRawTransaction(
        txc.GetHex(), utxos, map_target_value, selected_txin_utxos,
        reserve_txout_address, fee_asset, true, fee_rate, &estimate_fee,
        &filter, &option, &append_txout_addresses, NetType::kElementsRegtest);
    txc = ConfidentialTransactionContext(ctx.GetHex());
  } catch (const CfdException& except) {
    EXPECT_STREQ("", except.what());
    throw except;
  }

  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

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
  EXPECT_EQ(txc.GetFeeAmount().GetSatoshiValue(), estimate_fee.GetSatoshiValue());
  EXPECT_STREQ(tx.GetHex().c_str(), "020000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a4730440220658ec298d039a988b5d5a8a450821324c58264d28ec6c25c37750db21bca28220220599f5c2c780b76940a23ba46d1ae029c58785e43fb20929920cce175532876f801210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff040100000000000000000000000000000000000000000000000000000000000000aa0100005af3107a18f00308f34c0c7de1a49ec53acb73aeb94306a90987759c2c012492e877a0dde821d91976a914f330ed8383f8afdc977dd88600eb8ff120ba15e488ac0100000000000000000000000000000000000000000000000000000000000000aa0100071afd498d000002761f92316d668b7ad54ea7da8f4bfd1b266d677e9786b98274f647cc74ae306e16001445663e592ff613587f0fdd6e74034c5239710dca0100000000000000000000000000000000000000000000000000000000000000aa0100000000000001c200000100000000000000000000000000000000000000000000000000000000000000aa01000000000000254e029e1f4cd73bb6d29257ca37a3882cd09661783c90cdd92b57264a13bdab822905160014f330ed8383f8afdc977dd88600eb8ff120ba15e40000000000000247304402204b18c6012c0e3026df2d5d1b30e5ba284871567593c348e51ada20842e25822902207af1a537be4a506e1f3fbb0cb4f6125302af6da0a29d1ab8f1c11a5b68bd72a101210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a270000000247304402202fda15abdd560632a517d122cc9414b31c1b67e05c05c0d510630731750a1350022075b85c7faa46f6ea0cef8f9ded53d427e8292d8d6200da06da18789cd684e1c101210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b00000000000000000000000000");
  EXPECT_EQ(txc.GetVsize(), 662);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(fee_rate * 100) / 100;
  EXPECT_EQ(minimum_fee, 72);
}

#endif  // CFD_DISABLE_ELEMENTS
