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
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 2770);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 2458);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 312);

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

  if ((tx.GetDataSize() != 9399) && (tx.GetDataSize() != 9400) &&
      (tx.GetDataSize() != 9401) && (tx.GetDataSize() != 9402)) {
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
  utxo1.amount = Amount(int64_t{100000200});
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
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 32);

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
      (calc_fee.GetSatoshiValue() != (minimum_fee + 1)) &&
      (calc_fee.GetSatoshiValue() != (minimum_fee + 2))) {
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
  static const char* const kExpTxData = "0200000000030f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c34600000bfa8774c5f753ce2f801a8106413b470af94edbff5b4242ed4c5a26d20e72b90000000000ffffffff040b0000000000000000000000000000000000000000000000000000000000000000000000ffffffff07010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b92700001976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac01cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c34600001976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0100000000000000000000000000000000000000000000000000000000000000aa010000000000989680001600144352a1a6e86311f22274f7ebb2746de21b09b15d0100000000000000000000000000000000000000000000000000000000000000bb01000000000007a120001600148beaaac4654cf4ebd8e46ca5062b0e7fb3e7470c0100000000000000000000000000000000000000000000000000000000000000aa01000000000000036800000100000000000000000000000000000000000000000000000000000000000000bb010000000001124c1e00160014a53be40113bb50f2b8b2d0bfea1e823e75632b5f0100000000000000000000000000000000000000000000000000000000000000aa0100000000004b57f60016001478eb9fc2c9e1cdf633ecb646858ba862b21384ab00000000";

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
    EXPECT_EQ(872, estimate_fee.GetSatoshiValue());
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
  static const char* const kExpTxData = "0200000000040a503dbd4f8f2b064c70e048b21f93fe4584174478abf5f44747932cd21da87c0000000000ffffffff0a9a33750a810cd384ca5d93b09513f1eb5d93c669091b29eef710d2391ff7300000000000ffffffff0ad4a335556c64c3e2599c3a4c3ddff5b28f616fa55cf2323d2ae642eef74a8f0000000000ffffffff020b0000000000000000000000000000000000000000000000000000000000000000000000ffffffff070100000000000000000000000000000000000000000000000000000000000000aa01000110d9316ec00003b86ca86f23bc47e6ae1eebf2dea191f0ab98b89e2d5b6e5e8de20631441caeaa1600144352a1a6e86311f22274f7ebb2746de21b09b15d0100000000000000000000000000000000000000000000000000000000000000bb010000000ba43b740003a9dda6bdef90309c4fb5fe9131e396e9f9b32dc1b8defea451f71e6af1127b1d1600148beaaac4654cf4ebd8e46ca5062b0e7fb3e7470c0100000000000000000000000000000000000000000000000000000000000000aa010000000ba43b740003fef01583b95b37ee4f23d3673dfcc01faa4654272c289e8e3caa6aeb31f634741600144852255c0ff013e835689f30fb52508f89b6fbe20100000000000000000000000000000000000000000000000000000000000000aa010000000ba43b7400037703a4ef463f044f462f35c4dece9d1f02d32681079c2c01cea4d6267dd782ef1600143159e9a1f6cdd588b6d5df65c3c7a190ecf0fba70100000000000000000000000000000000000000000000000000000000000000aa01000000000000026400000100000000000000000000000000000000000000000000000000000000000000bb01000000a2fb40580000160014a53be40113bb50f2b8b2d0bfea1e823e75632b5f0100000000000000000000000000000000000000000000000000000000000000aa01000000fd340d189c0016001478eb9fc2c9e1cdf633ecb646858ba862b21384ab00000000";

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
    EXPECT_EQ(612, estimate_fee.GetSatoshiValue());
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
    if ((context.GetVsize() != 6118) && (context.GetVsize() != 6119) && (context.GetVsize() != 6120)) {
      EXPECT_EQ(100, context.GetVsize());
    }
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
  EXPECT_STREQ(tx.GetHex().c_str(), "020000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a47304402206454682354c6da513edad5380f2e83fffcae69d5004c61fee3452f371596b6d20220413f7978b23b25da56afd237db9f6532256a2962e400e4bbf01f70f318725cc401210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff040100000000000000000000000000000000000000000000000000000000000000aa01000221b262dd58f00308f34c0c7de1a49ec53acb73aeb94306a90987759c2c012492e877a0dde821d91976a914f330ed8383f8afdc977dd88600eb8ff120ba15e488ac0100000000000000000000000000000000000000000000000000000000000000aa0100016bcc41e9000002761f92316d668b7ad54ea7da8f4bfd1b266d677e9786b98274f647cc74ae306e16001445663e592ff613587f0fdd6e74034c5239710dca0100000000000000000000000000000000000000000000000000000000000000aa0100000000000001b500000100000000000000000000000000000000000000000000000000000000000000aa01000000000000255b029e1f4cd73bb6d29257ca37a3882cd09661783c90cdd92b57264a13bdab822905160014f330ed8383f8afdc977dd88600eb8ff120ba15e40000000000000247304402201f8ab340cc5c6c85ff01f8cb48649ae111f0eecefce996cee79835270942631d02207f2d32c047ed8b3b02e9bca2d9c237243bb3564ccb353fca9678844eb36c691701210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a2700000002473044022061d1a2c6903da2571ca14cfcdcec006ac3b95dc10a75b34d73b0a426003ba6c0022058e7453692dbe1d23c4f2676d567f04a737791edc025004c3c553bab6cec81d301210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b00000000000000000000000000");
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
  EXPECT_STREQ(tx.GetHex().c_str(), "020000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a473044022077acd09b88fbef8f95d5b2ca3fe588f2e2f406003e45ef0eeeab66f204c8e76402205ba0b727583ec92b0beb0567d211c7bd5aa3ffd3606caaa89b3f33923207d90601210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff040100000000000000000000000000000000000000000000000000000000000000aa0100005af3107a18f00308f34c0c7de1a49ec53acb73aeb94306a90987759c2c012492e877a0dde821d91976a914f330ed8383f8afdc977dd88600eb8ff120ba15e488ac0100000000000000000000000000000000000000000000000000000000000000aa0100071afd498d000002761f92316d668b7ad54ea7da8f4bfd1b266d677e9786b98274f647cc74ae306e16001445663e592ff613587f0fdd6e74034c5239710dca0100000000000000000000000000000000000000000000000000000000000000aa0100000000000001b500000100000000000000000000000000000000000000000000000000000000000000aa01000000000000255b029e1f4cd73bb6d29257ca37a3882cd09661783c90cdd92b57264a13bdab822905160014f330ed8383f8afdc977dd88600eb8ff120ba15e40000000000000247304402204c2c8c0428909569faf5a89c7a88a3a027bd5e43450205e4a5c760c50117a4df022070dd6ab0c704617f9deeda19e91a40404c7cae8b90350cc5d8b938ffc32bf30501210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a270000000247304402200e26640ecb98890dd7ad65e835082318e5531baf31f6b9ac36873b2fb6e0bd2802203be61a8173d6a3eb87970be301e7ff1a4fde765ef2991893afb9bc64775a6cf101210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b00000000000000000000000000");
  EXPECT_EQ(txc.GetVsize(), 662);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(fee_rate * 100) / 100;
  EXPECT_EQ(minimum_fee, 72);
}


TEST(ElementsTransactionApi, FundRawTransaction_AssetLimitAmountValue) {
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
  utxo1.amount = Amount(int64_t{2000000000000000});
  utxo1.address_type = AddressType::kP2pkhAddress;
  utxo1.asset = exp_dummy_asset_b;

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
  utxo2.asset = exp_dummy_asset_b;

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
  Address address4 = factory.GetAddress("ert1qg4nrukf07cf4slc0m4h8gq6v2guhzrw2ayudpu");

  ExtPubkey key = ExtPubkey("xpub661MyMwAqRbcGB88KaFbLGiYAat55APKhtWg4uYMkXAmfuSTbq2QYsn9sKJCj1YqZPafsboef4h4YbXXhNhPwMbkHTpkf3zLhx7HvFw1NDy");

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};

  ElementsConfidentialAddress reserve_ct_addr1 = ElementsConfidentialAddress(
      address1, key.DerivePubkey(91).GetPubkey());
  ElementsConfidentialAddress reserve_ct_addr2 = ElementsConfidentialAddress(
      address2, key.DerivePubkey(92).GetPubkey());
  ElementsConfidentialAddress reserve_ct_addr3 = ElementsConfidentialAddress(
      address3, key.DerivePubkey(93).GetPubkey());
  ElementsConfidentialAddress reserve_ct_addr4 = ElementsConfidentialAddress(
      address4, key.DerivePubkey(94).GetPubkey());

  double fee_rate = 0.11;
  ConfidentialAssetId fee_asset = exp_dummy_asset_a;
  std::map<std::string, Amount> map_target_value;
  map_target_value.emplace(exp_dummy_asset_a.GetHex(), Amount());
  map_target_value.emplace(exp_dummy_asset_b.GetHex(), Amount());
  std::map<std::string, std::string> reserve_txout_address;
  reserve_txout_address.emplace(exp_dummy_asset_a.GetHex(), reserve_ct_addr3.GetAddress());
  reserve_txout_address.emplace(exp_dummy_asset_b.GetHex(), reserve_ct_addr4.GetAddress());
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
  txc.AddTxOut(reserve_ct_addr1, utxo3.amount - fee, exp_dummy_asset_a);
  txc.AddTxOut(reserve_ct_addr2, Amount(int64_t{2100000000000000}), exp_dummy_asset_b);

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
  EXPECT_STREQ(tx.GetHex().c_str(), "020000000103a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a47304402200ab5855d6f2aacba3c02bf7e7d7feb5ac07f0dd1081f645b26d7c3761a6586dc02206ade8c8d928f3deefa3ca205b25fa6368a4cf90277196f24a2be3c21b867f78101210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffffe3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffff050100000000000000000000000000000000000000000000000000000000000000aa0100002d79883cf8f00308f34c0c7de1a49ec53acb73aeb94306a90987759c2c012492e877a0dde821d91976a914f330ed8383f8afdc977dd88600eb8ff120ba15e488ac0100000000000000000000000000000000000000000000000000000000000000bb01000775f05a07400002761f92316d668b7ad54ea7da8f4bfd1b266d677e9786b98274f647cc74ae306e16001445663e592ff613587f0fdd6e74034c5239710dca0100000000000000000000000000000000000000000000000000000000000000aa01000000000000023900000100000000000000000000000000000000000000000000000000000000000000bb010006c00a3912c000033b8298639bbce61dddf46d9d9d257e6ab497314075555f2029c5b9ab4a3ad9c716001445663e592ff613587f0fdd6e74034c5239710dca0100000000000000000000000000000000000000000000000000000000000000aa0100000000000024d7029e1f4cd73bb6d29257ca37a3882cd09661783c90cdd92b57264a13bdab822905160014f330ed8383f8afdc977dd88600eb8ff120ba15e400000000000002473044022003c51f2fb5cd560550759c1652ca962a6644e54bfef8adf675d1e3e3791b7a5d02204640a7d3abbc4e31668b59178b35b2cb0bad8abe5f1dfb474db3d40295e07db301210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b0000000000000002473044022033cf5cb157f8932d77683fd55ee70b21fc41b715f0f82d746dbc254e1c114ec60220355b6c0170c55da3b7d4c2c6b5fbd62da8a839c18798f06b7c29e39808b1ec5601210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a270000000000000000000000");
  EXPECT_EQ(txc.GetVsize(), 761);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(fee_rate * 100) / 100;
  EXPECT_EQ(minimum_fee, 83);
}


TEST(ElementsTransactionApi, FundRawTransaction_prod) {
  cfd::Initialize();
  ElementsAddressFactory factory(NetType::kLiquidV1);
  ConfidentialAssetId asset1("6f0279e9ed041c3d710a9f57d0c02928416460c4b722ae3457a11eec381c526d");
  ConfidentialAssetId asset2("f59c5f3e8141f322276daa63ed5f307085808aea6d4ef9ba61e28154533fdec7");
  // Address1
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("cd4433fd3d014187050aefe878f76ab76aa8b1eef3ba965cb8dc76b0d271d002");
  utxo1.vout = 2;
  utxo1.locking_script = Script("0014860d7bed1010c2ccb5247889daa5c58023fa9f8d");
  // utxo1.redeem_script = Script("");
  utxo1.address = factory.GetAddress("ex1qscxhhmgszrpvedfy0zya4fw9sq3l48udevqlh2");
  utxo1.descriptor = "wpkh(02db28ad892aa1e500d1e88ffa24200088bc82a8a87807cd13a1d1a1c7799c41e5)";
  utxo1.amount = Amount(int64_t{999799});
  utxo1.address_type = AddressType::kP2wpkhAddress;
  utxo1.asset = asset1;

  // Address2
  UtxoData utxo2;
  utxo2.block_height = 0;
  utxo2.binary_data = nullptr;
  utxo2.txid = Txid("62ac8a5272b67aab0ca60db859bed64729463de72ee1743fd7eb9c72f4437b06");
  utxo2.vout = 0;
  utxo2.locking_script = Script("0014891fd3ada0038759b124189ed64ab2343f7de0b1");
  // utxo1.redeem_script = Script("");
  utxo2.address = factory.GetAddress("ex1q3y0a8tdqqwr4nvfyrz0dvj4jxslhmc93hzzdt9");
  utxo2.descriptor = "wpkh(023004f49c61a63e339fa554e218774d6b4752bbfcfca61fe1c567b96af196b524)";
  utxo2.amount = Amount(int64_t{19999});
  utxo2.address_type = AddressType::kP2wpkhAddress;
  utxo2.asset = asset2;

  // Address3
  UtxoData utxo3;
  utxo3.block_height = 0;
  utxo3.binary_data = nullptr;
  utxo3.txid = Txid("376906dfec46e2abc2c98cc4a51d725cb8bbc5ece38ae1127c90802823202988");
  utxo3.vout = 2;
  utxo3.locking_script = Script("001495f728a019f3ecc3725479b32b230de51df74d4a");
  // utxo1.redeem_script = Script("");
  utxo3.address = factory.GetAddress("ex1qjhmj3gqe70kvxuj50xejkgcdu5wlwn225d0cu6");
  utxo3.descriptor = "wpkh(02272bcdbec1c0a2170e72f2d8cf42188d59ea7eae9296d60d435af3fe465d9bb5)";
  utxo3.amount = Amount(int64_t{1});
  utxo3.address_type = AddressType::kP2wpkhAddress;
  utxo3.asset = asset2;

  double fee_rate = 0.11;
  ConfidentialAssetId fee_asset = asset1;
  std::map<std::string, Amount> map_target_value;
  map_target_value.emplace(asset1.GetHex(), Amount());
  map_target_value.emplace(asset2.GetHex(), Amount());
  std::map<std::string, std::string> reserve_txout_address;
  reserve_txout_address.emplace(asset1.GetHex(), "lq1qqgv5wwfp4h0pfnyy2kkxl0kg3qnahcpfq7emrxu9xusz879axq0spg9cxu8wf72ktsft5r8vxnkfd8s5kmg32fvy8texp5p6s");
  reserve_txout_address.emplace(asset2.GetHex(), "lq1qqwqawne0jyc2swqv9qp8fstrgxuux2824zxkqew9gdak4yudxvwhha0kwdv2p3j0lyekhchrzmuekp94fpfp6fkeggjkerfr8");
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
  option.SetKnapsackMinimumChange(0);

  ConfidentialTransactionContext txc("0200000000000101c7de3f535481e261baf94e6dea8a808570305fed63aa6d2722f341813e5f9cf5010000000000004e20021f70a7514c145cfb01c8751b92ac5b3518d585afb256dae9bc4be464bf546ac616001429aa90ad4af8ef2859d0573e69cf567957ea330000000000");
  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};

  try {
    ElementsTransactionApi api;
    ConfidentialTransactionController ctx = api.FundRawTransaction(
        txc.GetHex(), utxos, map_target_value, selected_txin_utxos,
        reserve_txout_address, fee_asset, true, fee_rate, &estimate_fee,
        &filter, &option, &append_txout_addresses, NetType::kLiquidV1);
    txc = ConfidentialTransactionContext(ctx.GetHex());
  } catch (const CfdException& except) {
    EXPECT_STREQ("", except.what());
    throw except;
  }

  EXPECT_EQ(txc.GetFeeAmount().GetSatoshiValue(), estimate_fee.GetSatoshiValue());
  EXPECT_STREQ(txc.GetHex().c_str(), "02000000000302d071d2b076dcb85c96baf3eeb1a86ab76af778e8ef0a058741013dfd3344cd0200000000ffffffff067b43f4729cebd73f74e12ee73d462947d6be59b80da60cab7ab672528aac620000000000ffffffff882920232880907c12e18ae3ecc5bbb85c721da5c48cc9c2abe246ecdf0669370200000000ffffffff0301c7de3f535481e261baf94e6dea8a808570305fed63aa6d2722f341813e5f9cf5010000000000004e20021f70a7514c145cfb01c8751b92ac5b3518d585afb256dae9bc4be464bf546ac616001429aa90ad4af8ef2859d0573e69cf567957ea3300016d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f0100000000000001250000016d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f0100000000000f40520219473921adde14cc8455ac6fbec88827dbe02907b3b19b85372023f8bd301f00160014a0b8370ee4f9565c12ba0cec34ec969e14b6d11500000000");
  EXPECT_EQ(txc.GetVsize(), 374);
}

TEST(ElementsTransactionApi, FundRawTransaction_emptyUtxo) {
  cfd::Initialize();
  ElementsAddressFactory factory(NetType::kLiquidV1);
  ConfidentialAssetId asset("5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
  // Address1
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("5e5fd4e860d999b30b268ed583dfcfe805c395f8290d8307f6617fdc3f029de3");
  utxo1.vout = 0;
  utxo1.locking_script = Script("0014860d7bed1010c2ccb5247889daa5c58023fa9f8d");
  // utxo1.redeem_script = Script("");
  utxo1.address = factory.GetAddress("ert1qscxhhmgszrpvedfy0zya4fw9sq3l48udr728gs");
  utxo1.descriptor = "wpkh(02db28ad892aa1e500d1e88ffa24200088bc82a8a87807cd13a1d1a1c7799c41e5)";
  utxo1.amount = Amount(int64_t{99999500});
  utxo1.address_type = AddressType::kP2wpkhAddress;
  utxo1.asset = asset;

  double fee_rate = 0.15;
  ConfidentialAssetId fee_asset = asset;
  std::map<std::string, Amount> map_target_value;
  map_target_value.emplace(asset.GetHex(), Amount());
  std::map<std::string, std::string> reserve_txout_address;
  reserve_txout_address.emplace(asset.GetHex(), "el1qqtl9a3n6878ex25u0wv8u5qlzpfkycc0cftk65t52pkauk55jqka0fajk8d80lafn4t9kqxe77cu9ez2dyr6sq54lwy009uex");
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
  option.SetKnapsackMinimumChange(0);

  ConfidentialTransactionContext txc("020000000101e39d023fdc7f61f607830d29f895c305e8cfdf83d58e260bb399d960e8d45f5e0000004000ffffffff000000000000000006080cdff505000000002025b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a2006226e46111a0b59caaf126043eb5bbf28c34f3a5e332a1fc7b2b73cf188910f160014f3ea0aba73fdb23912ebd21f46e156cdd9e94280c0020000000001014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff010cdff5050000000017a9141500eb4946dee5979e708c8b2c6d090773f3b8d1870247304402204d9faa0b3b9c76b3ee875ae9205b50e05c2d0a8dff8e26d269f68eb72531af1402201f71d1e2bec6b7ea90d45dec158d3f85942e0fc09cfad29d917d3cbc6acd981d012103b64236b2c8f34a18e3a584fe0877fb944e2abb4544cb14bee5458bcc2480cefc000000009700000020fe3b574c1ce6d5cb68fc518e86f7976e599fafc0a2e5754aace7ca16d97a7c78ef9325b8d4f0a4921e060fc5e71435f46a18fa339688142cd4b028c8488c9f8dd1495b5dffff7f200200000002000000024a180a6822abffc3b1080c49016899c6dac25083936df14af12f58db11958ef27926299350fdc2f4d0da1d4f0fbbd3789d29f9dc016358ae42463c0cebf393f30105");
  cfd::core::Transaction tx(
    "020000000001014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff010cdff5050000000017a9141500eb4946dee5979e708c8b2c6d090773f3b8d1870247304402204d9faa0b3b9c76b3ee875ae9205b50e05c2d0a8dff8e26d269f68eb72531af1402201f71d1e2bec6b7ea90d45dec158d3f85942e0fc09cfad29d917d3cbc6acd981d012103b64236b2c8f34a18e3a584fe0877fb944e2abb4544cb14bee5458bcc2480cefc00000000");
  Script claim_script("0014f3ea0aba73fdb23912ebd21f46e156cdd9e94280");
  std::vector<cfd::UtxoData> utxos;
  ElementsUtxoAndOption utxo_data1;
  utxo_data1.utxo = utxo1;
  utxo_data1.is_pegin = true;
  utxo_data1.claim_script = claim_script;
  utxo_data1.pegin_btc_tx_size = tx.GetTotalSize();
  utxo_data1.pegin_txoutproof_size = 250;  // dummy
  selected_txin_utxos.push_back(utxo_data1);

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

  EXPECT_EQ(txc.GetFeeAmount().GetSatoshiValue(), estimate_fee.GetSatoshiValue());
  EXPECT_STREQ(txc.GetHex().c_str(), "020000000101e39d023fdc7f61f607830d29f895c305e8cfdf83d58e260bb399d960e8d45f5e0000004000ffffffff020125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a0100000000000000d500000125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000000005f5de3702fe5ec67a3f8f932a9c7b987e501f105362630fc2576d5174506dde5a94902dd7160014a7b2b1da77ffa99d565b00d9f7b1c2e44a6907a80000000000000006080cdff505000000002025b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a2006226e46111a0b59caaf126043eb5bbf28c34f3a5e332a1fc7b2b73cf188910f160014f3ea0aba73fdb23912ebd21f46e156cdd9e94280c0020000000001014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff010cdff5050000000017a9141500eb4946dee5979e708c8b2c6d090773f3b8d1870247304402204d9faa0b3b9c76b3ee875ae9205b50e05c2d0a8dff8e26d269f68eb72531af1402201f71d1e2bec6b7ea90d45dec158d3f85942e0fc09cfad29d917d3cbc6acd981d012103b64236b2c8f34a18e3a584fe0877fb944e2abb4544cb14bee5458bcc2480cefc000000009700000020fe3b574c1ce6d5cb68fc518e86f7976e599fafc0a2e5754aace7ca16d97a7c78ef9325b8d4f0a4921e060fc5e71435f46a18fa339688142cd4b028c8488c9f8dd1495b5dffff7f200200000002000000024a180a6822abffc3b1080c49016899c6dac25083936df14af12f58db11958ef27926299350fdc2f4d0da1d4f0fbbd3789d29f9dc016358ae42463c0cebf393f3010500000000");
  EXPECT_EQ(txc.GetVsize(), 307);
}

TEST(ElementsTransactionApi, FundRawTransaction_Check1) {
  // from cfd-go
  static const std::vector<TestFundElementsUtxoVector> kFundCoinSelectElementsTestVector3 = {
    // mnemonic: token hair neglect leader furnace obtain sadness tool you father paddle skate remain carry impact dinosaur correct essay rent illness predict mercy exist ring
    // xpriv: xprv9s21ZrQH143K4QXrTfC9L43GKCuLcDiBCWjyVqfZUTzoPJWUstD4HTJKGz1U5jAGZzKshcX6cCyZ1ZdxSUQLz92pEZWEGwxa39ks2vhTsfA
    // derive: 44h/0h/0h/0/*
    {
      "7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a",
      0,
      312500000,
      exp_dummy_asset_a.GetHex(),
      "sh(wpkh(0329165ca5832de80305c92d4b1415f10340d267ba05cbffcfe02d386dc5020e4d))",
    },
    {
      "30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a",
      0,
      78125000,
      exp_dummy_asset_a.GetHex(),
      "sh(wpkh(022db3cb17d98db6cd8d513f88b095dbe80ef9e57acd5b1d9e8bd7f24618079451))",
    },
    {
      "9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a",
      0,
      1250000000,
      exp_dummy_asset_a.GetHex(),
      "sh(wpkh(032d04e0b1474a82ad68c0ef37e1a7cf6c75ef01b22c00882e8e4e127a942823a1))",
    },
    {
      "8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a",
      0,
      39062500,
      exp_dummy_asset_a.GetHex(),
      "sh(wpkh(03a7fb569db921abf70f1b6b9ad9ac863196deecd99d606b139bba7d740d1cc5bf))",
    },
    {
      "4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a",
      0,
      156250000,
      exp_dummy_asset_a.GetHex(),
      "sh(wpkh(02df74fc8124ff6f3982e90afb318f3e955b10f58c4c6014b3a767e16160f811d9))",
    },
    {
      "b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b",
      0,
      2500000000,
      exp_dummy_asset_a.GetHex(),
      "sh(wpkh(029222484db385d268a2a4604ea40fd2228401061f741ad9da8c907ba9df29c2d3))",
    },
    {
      "0000000000000000000000000000000000000000000000000000000000000b01",
      0,
      26918400,
      exp_dummy_asset_b.GetHex(),
      "sh(wpkh(038f9011753b74fa0134d4b64a1491f99e0c4c0e16da616627c1f6a93c5e7555c0))",
    },
    {
      "0000000000000000000000000000000000000000000000000000000000000b02",
      0,
      750000,
      exp_dummy_asset_b.GetHex(),
      "sh(wpkh(0302f567f9671b570dbcf179f3ba5f2fb381ea7e8db6ab9e0968c07d40325c3fcd))",
    },
    {
      "0000000000000000000000000000000000000000000000000000000000000b03",
      0,
      346430050,
      exp_dummy_asset_b.GetHex(),
      "sh(wpkh(034ff60d8fb18ae88019f6f905cfaa0e1841f75edfa1f3c0a5bfaf77b796243901))",
    },
    {
      "0000000000000000000000000000000000000000000000000000000000000b04",
      0,
      18476350,
      exp_dummy_asset_b.GetHex(),
      "sh(wpkh(029f2126cd8b55af7cc3cee8154c44de7cb7cb214809f81144d6b323d9c7a3993e))",
    },
    {
      "0000000000000000000000000000000000000000000000000000000000000c01",
      0,
      37654200,
      exp_dummy_asset_c.GetHex(),
      "sh(wpkh(02f1d2c28388e3fd609ff383f022b615f1cd8a1931632706f63bfb6e253875ca03))",
    },
    {
      "0000000000000000000000000000000000000000000000000000000000000c02",
      0,
      127030000,
      exp_dummy_asset_c.GetHex(),
      "sh(wpkh(02cca4482dc1e7d54c879c0d9069e3d66c3bf91b2bf46eddc74f18d76c659dfd10))",
    },
  };
  static const std::string kExpTxData = "010000000006fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff0a9a33750a810cd384ca5d93b09513f1eb5d93c669091b29eef710d2391ff7300000000000feffffff0ad4a335556c64c3e2599c3a4c3ddff5b28f616fa55cf2323d2ae642eef74a8f0000000000feffffff030b0000000000000000000000000000000000000000000000000000000000000000000000feffffff020b0000000000000000000000000000000000000000000000000000000000000000000000feffffff010c0000000000000000000000000000000000000000000000000000000000000000000000feffffff050100000000000000000000000000000000000000000000000000000000000000aa010000000006b22c2000160014c6598809d09edaacb8f4f4d5b9b81e4413a572430100000000000000000000000000000000000000000000000000000000000000aa01000000000000026a00000100000000000000000000000000000000000000000000000000000000000000bb010000000014b18c12001600148aff8eea7bef9ec60d35d7034b2e48e180e93c5d0100000000000000000000000000000000000000000000000000000000000000cc0100000000023e8eb800160014799a8d3f11251b6a6df4ba156a28dd64ad969a910100000000000000000000000000000000000000000000000000000000000000aa010000000006fc2142001600146cd31ad8b8552934f4bd9c8cf84a93cbd7a49de111000000";

  cfd::Initialize();
  ElementsAddressFactory factory(NetType::kLiquidV1);

  try {
    ConfidentialTransactionContext tx("010000000001fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff010100000000000000000000000000000000000000000000000000000000000000aa010000000006b22c2000160014c6598809d09edaacb8f4f4d5b9b81e4413a5724311000000");
    std::vector<cfd::UtxoData> utxos;
    utxos.resize(kFundCoinSelectElementsTestVector3.size());
    uint32_t i = 0;
    for (const auto& test_data : kFundCoinSelectElementsTestVector3) {
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

    ExtPubkey key = ExtPubkey("xpub661MyMwAqRbcGB88KaFbLGiYAat55APKhtWg4uYMkXAmfuSTbq2QYsn9sKJCj1YqZPafsboef4h4YbXXhNhPwMbkHTpkf3zLhx7HvFw1NDy");
    ConfidentialAssetId fee_asset = exp_dummy_asset_a;
    std::map<std::string, Amount> map_target_value;
    map_target_value.emplace(exp_dummy_asset_a.GetHex(), Amount(int64_t{115800000}));
    map_target_value.emplace(exp_dummy_asset_b.GetHex(), Amount(int64_t{347180040}));
    map_target_value.emplace(exp_dummy_asset_c.GetHex(), Amount(int64_t{37654100}));
    Address addr = factory.GetAddress("ex1qcevcszwsnmd2ew857n2mnwq7gsf62ujrkz734h");
    Address addr1 = factory.GetAddress("ex1qdnf34k9c255nfa9anjx0sj5ne0t6f80p5rne4e");
    Address addr2 = factory.GetAddress("ex1q3tlca6nma70vvrf46up5ktjguxqwj0zamt7ktn");
    Address addr3 = factory.GetAddress("ex1q0xdg60c3y5dk5m05hg2k52xavjkedx53t3k40m");
    std::map<std::string, std::string> reserve_txout_address;
    reserve_txout_address.emplace(exp_dummy_asset_a.GetHex(), addr1.GetAddress());
    reserve_txout_address.emplace(exp_dummy_asset_b.GetHex(), addr2.GetAddress());
    reserve_txout_address.emplace(exp_dummy_asset_c.GetHex(), addr3.GetAddress());
    ElementsConfidentialAddress ct_addr1 = ElementsConfidentialAddress(
        addr, key.DerivePubkey(90).GetPubkey());
    ElementsConfidentialAddress reserve_ct_addr1 = ElementsConfidentialAddress(
        addr1, key.DerivePubkey(91).GetPubkey());
    ElementsConfidentialAddress reserve_ct_addr2 = ElementsConfidentialAddress(
        addr2, key.DerivePubkey(92).GetPubkey());
    ElementsConfidentialAddress reserve_ct_addr3 = ElementsConfidentialAddress(
        addr3, key.DerivePubkey(93).GetPubkey());
    std::vector<UtxoData> selected_utxos;
    std::vector<ElementsUtxoAndOption> selected_txin_utxos;
    ElementsUtxoAndOption txin_utxo;
    txin_utxo.utxo.txid = Txid("9f96ade4b41d5433f4eda31e1738ec2b36f6e7d1420d94a6af99801a88f7f7ff");
    txin_utxo.utxo.amount = Amount(int64_t{112340000});
    txin_utxo.utxo.asset = exp_dummy_asset_a;
    txin_utxo.utxo.descriptor = "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x";
    txin_utxo.utxo.address_type = AddressType::kP2shP2wpkhAddress;
    selected_txin_utxos.push_back(txin_utxo);
    selected_utxos.push_back(txin_utxo.utxo);
    double fee_rate = 0.15;
    Amount estimate_fee;
    UtxoFilter filter;
    std::vector<std::string> append_txout_addresses;
    CoinSelectionOption option;
    option.InitializeConfidentialTxSizeInfo();
    option.SetEffectiveFeeBaserate(fee_rate);
    option.SetLongTermFeeBaserate(0.15);
    option.SetFeeAsset(fee_asset);
    option.SetBlindInfo(0, 36);

    ElementsTransactionApi api;
    ConfidentialTransactionController ctx = api.FundRawTransaction(
        tx.GetHex(), utxos, map_target_value, selected_txin_utxos,
        reserve_txout_address, fee_asset, true, fee_rate, &estimate_fee,
        &filter, &option, &append_txout_addresses, NetType::kLiquidV1);

    EXPECT_EQ(kExpTxData, ctx.GetHex());
    EXPECT_EQ(618, estimate_fee.GetSatoshiValue());
    EXPECT_EQ(size_t{3}, append_txout_addresses.size());
    if (append_txout_addresses.size() == size_t{3})
    {
      EXPECT_STREQ(addr1.GetAddress().c_str(), append_txout_addresses[2].c_str());
      EXPECT_STREQ(addr2.GetAddress().c_str(), append_txout_addresses[0].c_str());
      EXPECT_STREQ(addr3.GetAddress().c_str(), append_txout_addresses[1].c_str());
    }

    ConfidentialTransactionContext context(ctx.GetHex());
    std::vector<cfd::core::ElementsConfidentialAddress> ct_addrs;
    ct_addrs.push_back(ct_addr1);
    ct_addrs.push_back(reserve_ct_addr1);
    ct_addrs.push_back(reserve_ct_addr2);
    ct_addrs.push_back(reserve_ct_addr3);
    context.CollectInputUtxo(selected_utxos);
    context.CollectInputUtxo(utxos);
    context.Blind(&ct_addrs, 1, 0, 36);
    for (const auto& txin : context.GetTxInList()) {
      context.SignWithKey(OutPoint(txin.GetTxid(), txin.GetVout()),
          Pubkey("0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b"),
          Privkey::FromWif(
              "cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5"));
    }
    if ((context.GetVsize() != 4120) && (context.GetVsize() != 4121) &&
        (context.GetVsize() != 4122)) {
      EXPECT_EQ(100, context.GetVsize());
    }
    // EXPECT_STREQ("", context.GetHex().c_str());
  } catch (const CfdException& except) {
    EXPECT_STREQ("", except.what());
    return;
  }
}

TEST(ElementsTransactionApi, EstimateFee_PeginTx) {
  // signed pegin tx
  std::string pegin_tx_hex = "02000000010141fb1b60887f17a4d3351880635ff5e76e7941203348955d2675005892ca343b0000004000ffffffff02016d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f010000000000005e7f001600149aa483fc0fd0b1806f25e57903e4cbefb19c20a1016d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f01000000000000002900000000000000000247304402204c761ad9b55f2fd502d09551656c34f5e118220c286072c7408fe1b2d9eb033202201cb5fbbd90038d82fd21b686a23fa26ac20574da7f28a2afff5e958547ab72ef012102026e4a89b56f6f89de4f0c00d084eacb9dc310e387433049b55d7737fa2e04a70608a85e000000000000206d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f206fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d6190000000000160014aab2ed7ac90f976a17ddb850c4a0c4e62db49b0753020000000123edd3be8309747693f42e5b6a0a0c57899fca7d6fb5a8d9c4f52fbf7398f03d00000000000000008001a85e00000000000017a914fdfbf873904e8276428aa892576acd028775fbda8700000000fdd9010400c0209ac0009d7289eb8e52ef1883501b35ec6ee8a5bfc83a09000000000000000000e157506b8e9b09bed1b9a9d6045fa26735547d696d9ce230c7f2003d09704594f558e160ce981317486c10c46e0600000c7eab9226e14b446023e58c32f677298a3a60448556b3b6dcf189ea680035de47fcd1a47ece1bda2de728f1ea7ca166338ddd0ab1ee2797c7cd2c9c5d5d07b055ed58f3b21f1324c98e1c09a8f4dd3a3f5b3a7ca8efc48fcbcb41cbf5476c40e9e3abc1c5d78e4b59dde0cfabf3c47d68f2f83893b19c0079b6902525c41e146ad09cb3859268063fd6e6b123036e6c65a975698553d701e04f0801752a4c454ff689b184b1c48c3fdfc33a7e25c2d96f8b9dfcbbb6b970f7da7ecbccd6f3d33141fb1b60887f17a4d3351880635ff5e76e7941203348955d2675005892ca343b04b0b4b9206faf789f1c0659f100a69eea88bfdc66a2b2382fe628fb159304ffd3f7ef70acc4e0438b1fb380da205c7ae0746dfbb451bb2110a9a25e224455719c6c22fa85161cb1df6fc4a870b337d2de35c7cc478c6658147ea8e1d64e7152d172e425e8e3ffd1cfae750d09193292f6e170a1fbfd65d8d43ecc3c5e0932fddf4b338fb84ae565a87fb78bc9d4d5d32ab8bf1fba7880c08c3a8a3a7e3aa22803bdd60200000000";

  // Address1
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("3b34ca92580075265d9548332041796ee7f55f63801835d3a4177f88601bfb41");
  utxo1.vout = 0;
  utxo1.locking_script = Script("0014eb3c0d55b7098a4aef4a18ee1eebcb1ed924a82b");
  utxo1.descriptor = "wpkh(03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6)";
  utxo1.amount = Amount(int64_t{24232});
  utxo1.address_type = AddressType::kP2wpkhAddress;
  utxo1.asset = ConfidentialAssetId("6f0279e9ed041c3d710a9f57d0c02928416460c4b722ae3457a11eec381c526d");
  ElementsUtxoAndOption eutxo1;
  eutxo1.utxo = utxo1;
  std::vector<cfd::api::ElementsUtxoAndOption> utxos_and_options{
      eutxo1
  };

  ConfidentialTransactionContext txc(pegin_tx_hex);

  std::vector<cfd::UtxoData> utxos{utxo1};
  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

  // check estimateFee
  double effective_fee_rate = 0.1;
  Amount calc_fee;
  Amount utxo_fee;
  Amount tx_fee;
  ElementsTransactionApi api;
  calc_fee = api.EstimateFee(txc.GetHex(), utxos_and_options, utxo1.asset,
      &tx_fee, &utxo_fee, false, effective_fee_rate, 0, 0);
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 36);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 13);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 24);
}

TEST(ElementsTransactionApi, EstimateFee_PeginTx2) {
  // signed pegin tx
  std::string pegin_tx_hex = "02000000000141fb1b60887f17a4d3351880635ff5e76e7941203348955d2675005892ca343b0000004000ffffffff02016d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f010000000000005e7f001600149aa483fc0fd0b1806f25e57903e4cbefb19c20a1016d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f010000000000000029000000000000";
  std::string claim_script = "0014aab2ed7ac90f976a17ddb850c4a0c4e62db49b07";
  std::string btc_tx = "020000000123edd3be8309747693f42e5b6a0a0c57899fca7d6fb5a8d9c4f52fbf7398f03d00000000000000008001a85e00000000000017a914fdfbf873904e8276428aa892576acd028775fbda8700000000";
  std::string txoutproof = "0400c0209ac0009d7289eb8e52ef1883501b35ec6ee8a5bfc83a09000000000000000000e157506b8e9b09bed1b9a9d6045fa26735547d696d9ce230c7f2003d09704594f558e160ce981317486c10c46e0600000c7eab9226e14b446023e58c32f677298a3a60448556b3b6dcf189ea680035de47fcd1a47ece1bda2de728f1ea7ca166338ddd0ab1ee2797c7cd2c9c5d5d07b055ed58f3b21f1324c98e1c09a8f4dd3a3f5b3a7ca8efc48fcbcb41cbf5476c40e9e3abc1c5d78e4b59dde0cfabf3c47d68f2f83893b19c0079b6902525c41e146ad09cb3859268063fd6e6b123036e6c65a975698553d701e04f0801752a4c454ff689b184b1c48c3fdfc33a7e25c2d96f8b9dfcbbb6b970f7da7ecbccd6f3d33141fb1b60887f17a4d3351880635ff5e76e7941203348955d2675005892ca343b04b0b4b9206faf789f1c0659f100a69eea88bfdc66a2b2382fe628fb159304ffd3f7ef70acc4e0438b1fb380da205c7ae0746dfbb451bb2110a9a25e224455719c6c22fa85161cb1df6fc4a870b337d2de35c7cc478c6658147ea8e1d64e7152d172e425e8e3ffd1cfae750d09193292f6e170a1fbfd65d8d43ecc3c5e0932fddf4b338fb84ae565a87fb78bc9d4d5d32ab8bf1fba7880c08c3a8a3a7e3aa22803bdd602";

  // Address1
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("3b34ca92580075265d9548332041796ee7f55f63801835d3a4177f88601bfb41");
  utxo1.vout = 0;
  utxo1.locking_script = Script("0014eb3c0d55b7098a4aef4a18ee1eebcb1ed924a82b");
  utxo1.descriptor = "wpkh(03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6)";
  utxo1.amount = Amount(int64_t{24232});
  utxo1.address_type = AddressType::kP2wpkhAddress;
  utxo1.asset = ConfidentialAssetId("6f0279e9ed041c3d710a9f57d0c02928416460c4b722ae3457a11eec381c526d");
  ElementsUtxoAndOption eutxo1;
  eutxo1.utxo = utxo1;
  eutxo1.is_pegin = true;
  eutxo1.pegin_btc_tx_size = btc_tx.length() / 2;
  eutxo1.pegin_txoutproof_size = txoutproof.length() / 2;
  eutxo1.claim_script = Script(claim_script);
  std::vector<cfd::api::ElementsUtxoAndOption> utxos_and_options{
      eutxo1
  };

  ConfidentialTransactionContext txc(pegin_tx_hex);

  std::vector<cfd::UtxoData> utxos{utxo1};
  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

  // check estimateFee
  double effective_fee_rate = 0.1;
  Amount calc_fee;
  Amount utxo_fee;
  Amount tx_fee;
  ElementsTransactionApi api;
  calc_fee = api.EstimateFee(txc.GetHex(), utxos_and_options, utxo1.asset,
      &tx_fee, &utxo_fee, false, effective_fee_rate, 0, 0);
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 36);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 13);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 24);
}

#endif  // CFD_DISABLE_ELEMENTS
