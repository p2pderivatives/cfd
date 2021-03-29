#include "gtest/gtest.h"

#include <map>
#include <string>
#include <vector>

#include "cfd/cfd_transaction.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_hdwallet.h"

using cfd::TransactionContext;
using cfd::CoinSelectionOption;
using cfd::CoinSelection;
using cfd::UtxoData;
using cfd::UtxoFilter;
using cfd::AddressFactory;
using cfd::api::TransactionApi;
using cfd::core::Amount;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::NetType;
using cfd::core::ExtPubkey;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::OutPoint;
using cfd::core::Script;
using cfd::core::Txid;
using cfd::core::CfdException;

TEST(TransactionApi, EstimateFee_CheckRealValue) {
  AddressFactory factory(NetType::kRegtest);
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
  utxo1.address = factory.GetAddress("n3gq7EMkVLyrpSxVxKLFX8qsqiDC5DcqfW");
  utxo1.descriptor = "pkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo1.amount = Amount(int64_t{10000000});
  utxo1.address_type = AddressType::kP2pkhAddress;

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
  utxo2.address = factory.GetAddress("2N1kiV9NkmZetZ3j7FuWGkBZxubBMPLxJ16");
  utxo2.descriptor = "sh(wpkh(0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27))";
  utxo2.amount = Amount(int64_t{180000});
  utxo2.address_type = AddressType::kP2shP2wpkhAddress;

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
  utxo3.address = factory.GetAddress("bcrt1q7vcwmqurlzhae9mamzrqp6u07yst590y5ujv3w");
  utxo3.descriptor = "wpkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo3.amount = Amount(int64_t{10000000});
  utxo3.address_type = AddressType::kP2wpkhAddress;

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  Address address("mtmTFSnUTqGt6AaSqoRemj7ePPZ6YGXWeo");
  // "0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"
  Address address2("bcrt1qg4nrukf07cf4slc0m4h8gq6v2guhzrw29sfnlu");

  Amount fee(int64_t{10000});
  TransactionContext txc(2, 0);
  EXPECT_NO_THROW(txc.AddInput(utxo1));
  txc.AddTxIn(OutPoint(utxo2.txid, utxo2.vout));
  txc.AddTxIn(OutPoint(utxo3.txid, utxo3.vout));
  txc.AddTxOut(address, utxo1.amount + utxo3.amount - fee);
  txc.AddTxOut(address2, utxo2.amount);

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};
  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

  // check estimateFee
  double effective_fee_rate = 20.0;
  Amount calc_fee;
  Amount utxo_fee;
  Amount tx_fee;
  TransactionApi api;
  calc_fee = api.EstimateFee(
      txc.GetHex(), utxos, &tx_fee, &utxo_fee, effective_fee_rate);
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 7740);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 1540);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 6200);

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
  EXPECT_EQ(txc.GetFeeAmount().GetSatoshiValue(), fee.GetSatoshiValue());
  EXPECT_STREQ(tx.GetHex().c_str(), "02000000000103a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a47304402200710e3ab10644ebe6bc8934899743cb7a3d127411b4db8ffcdd6a75c03c917760220143a55a35926d9d719d1eed902bcf7dbd5f54fe9830ea653d14895a80535a33f01210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffffe3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffff02f0053101000000001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac20bf02000000000016001445663e592ff613587f0fdd6e74034c5239710dca000247304402204583781dada5f2173ef1c60e8d1d93df0edec0fb6aaf46f41639b04446dddb7e02200ff89839302e4325c54ef818ce8fd2dfa3d1f2a62a3fc67ddd62d89f2a07078501210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a270247304402207e3807d4a520cdd219791a4a7637d37ce6ae172c03292e525d3bf6ea88d356930220713ddd154bf5c7bcb67f2bf44d39a8fba6048cca9af80a0abf13ee61206cb40d01210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b00000000");
  EXPECT_EQ(txc.GetVsize(), 382);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate);
  EXPECT_EQ(minimum_fee, 7640);
}

TEST(TransactionApi, EstimateFee_TaprootSchnorr) {
  std::string tx_hex = "0200000000010116d975e4c2cea30f72f4f5fe528f5a0727d9ea149892a50c030d44423088ea2f0000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d50000000000";
  AddressFactory factory(NetType::kRegtest);
  // Address1
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("2fea883042440d030ca5929814ead927075a8f52fef5f4720fa3cec2e475d916");
  utxo1.vout = 0;
  utxo1.locking_script = Script("51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb");
  // utxo1.redeem_script = Script("");
  utxo1.address = factory.GetAddressByLockingScript(utxo1.locking_script);
  utxo1.descriptor = "raw(51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb)";
  utxo1.amount = Amount(int64_t{2499999000});
  utxo1.address_type = AddressType::kTaprootAddress;

  TransactionContext txc(tx_hex);
  std::vector<cfd::UtxoData> utxos{utxo1};
  EXPECT_NO_THROW(txc.CollectInputUtxo(utxos));

  // check estimateFee
  double effective_fee_rate = 2.0;
  Amount calc_fee;
  Amount utxo_fee;
  Amount tx_fee;
  TransactionApi api;
  calc_fee = api.EstimateFee(
      txc.GetHex(), utxos, &tx_fee, &utxo_fee, effective_fee_rate);
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 101*2);  // upper
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 43*2);  // upper
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 58*2);
}

TEST(TransactionApi, FundRawTransaction_MillionAmountValue) {
  AddressFactory factory(NetType::kRegtest);
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
  utxo1.address = factory.GetAddress("n3gq7EMkVLyrpSxVxKLFX8qsqiDC5DcqfW");
  utxo1.descriptor = "pkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo1.amount = Amount(int64_t{300000000000000});
  utxo1.address_type = AddressType::kP2pkhAddress;

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
  utxo2.address = factory.GetAddress("2N1kiV9NkmZetZ3j7FuWGkBZxubBMPLxJ16");
  utxo2.descriptor = "sh(wpkh(0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27))";
  utxo2.amount = Amount(int64_t{400000000000000});
  utxo2.address_type = AddressType::kP2shP2wpkhAddress;

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
  utxo3.address = factory.GetAddress("bcrt1q7vcwmqurlzhae9mamzrqp6u07yst590y5ujv3w");
  utxo3.descriptor = "wpkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo3.amount = Amount(int64_t{300000000000000});
  utxo3.address_type = AddressType::kP2wpkhAddress;

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  Address address("mtmTFSnUTqGt6AaSqoRemj7ePPZ6YGXWeo");
  // "0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"
  Address address2("bcrt1qg4nrukf07cf4slc0m4h8gq6v2guhzrw29sfnlu");
  Address address3("bcrt1q7vcwmqurlzhae9mamzrqp6u07yst590y5ujv3w");

  Amount fee(int64_t{10000});
  TransactionContext txc(2, 0);
  txc.AddTxOut(address, utxo1.amount + utxo3.amount - fee);
  txc.AddTxOut(address2, utxo2.amount);

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};

  double effective_fee_rate = 20.0;
  CoinSelectionOption option;
  std::vector<UtxoData> selected_txin_utxos;
  Amount target_value;
  Amount estimate_fee;


  TransactionApi api;
  auto tx_obj = api.FundRawTransaction(
      txc.GetHex(), utxos, target_value, selected_txin_utxos, address3.GetAddress(),
      effective_fee_rate, &estimate_fee, nullptr, &option, nullptr, NetType::kRegtest);
  EXPECT_EQ(estimate_fee.GetSatoshiValue(), 8360);

  txc = TransactionContext(tx_obj.GetHex());
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
  EXPECT_STREQ(tx.GetHex().c_str(), "02000000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a4730440220644d3014684db32df93e6e342e2d0872aa6272d3a9cdce021138592ef78fc2a8022054a2a85c6339dee9578d9989920100f0660bd45b9cf7b14444f012bf5ee0bd7601210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff03f058dd62b22102001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac0000e941cc6b010016001445663e592ff613587f0fdd6e74034c5239710dca6806000000000000160014f330ed8383f8afdc977dd88600eb8ff120ba15e40247304402202ece1dae47832c07863b5052133c9af652e9521e0fbaf4ef7558ca45ec739ca902205ad29147c0f5f16277a7a00391f8b5d43b9b6c149721cdb4591ba18c701dbd5f01210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a2702473044022058e88c67379c5b5c49dfef09eb069833ba9ae7cd6f8a5f9bdcf657ed8ad91b820220039739df97dc8488a93f61000cbade6d48df737238519660a02071795aabd74801210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b0000000000");
  EXPECT_EQ(txc.GetVsize(), 413);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate);
  EXPECT_EQ(minimum_fee, 8260);
}


TEST(TransactionApi, FundRawTransaction_LimitAmountValue) {
  AddressFactory factory(NetType::kRegtest);
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
  utxo1.address = factory.GetAddress("n3gq7EMkVLyrpSxVxKLFX8qsqiDC5DcqfW");
  utxo1.descriptor = "pkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo1.amount = Amount(int64_t{50000000000000});
  utxo1.address_type = AddressType::kP2pkhAddress;

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
  utxo2.address = factory.GetAddress("2N1kiV9NkmZetZ3j7FuWGkBZxubBMPLxJ16");
  utxo2.descriptor = "sh(wpkh(0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27))";
  utxo2.amount = Amount(int64_t{2000000000000000});
  utxo2.address_type = AddressType::kP2shP2wpkhAddress;

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
  utxo3.address = factory.GetAddress("bcrt1q7vcwmqurlzhae9mamzrqp6u07yst590y5ujv3w");
  utxo3.descriptor = "wpkh(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b)";
  utxo3.amount = Amount(int64_t{50000000000000});
  utxo3.address_type = AddressType::kP2wpkhAddress;

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  Address address("mtmTFSnUTqGt6AaSqoRemj7ePPZ6YGXWeo");
  // "0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"
  Address address2("bcrt1qg4nrukf07cf4slc0m4h8gq6v2guhzrw29sfnlu");
  Address address3("bcrt1q7vcwmqurlzhae9mamzrqp6u07yst590y5ujv3w");

  Amount fee(int64_t{10000});
  TransactionContext txc(2, 0);
  txc.AddTxOut(address, utxo1.amount + utxo3.amount - fee);
  txc.AddTxOut(address2, utxo2.amount);

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};

  double effective_fee_rate = 20.0;
  CoinSelectionOption option;
  std::vector<UtxoData> selected_txin_utxos;
  Amount target_value;
  Amount estimate_fee;


  TransactionApi api;
  auto tx_obj = api.FundRawTransaction(
      txc.GetHex(), utxos, target_value, selected_txin_utxos, address3.GetAddress(),
      effective_fee_rate, &estimate_fee, nullptr, &option, nullptr, NetType::kRegtest);
  EXPECT_EQ(estimate_fee.GetSatoshiValue(), 8360);

  txc = TransactionContext(tx_obj.GetHex());
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
  EXPECT_STREQ(tx.GetHex().c_str(), "02000000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a473044022029f4ce58206fe107b1dd6cbaf3246391a55cdd639cce10fdf545a8c131e17ebf022022b47d656c8666b188c6b16f9e10f6718bbbd420c390fe7a039f9c4323596d1c01210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff03f0187a10f35a00001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac00008d49fd1a070016001445663e592ff613587f0fdd6e74034c5239710dca6806000000000000160014f330ed8383f8afdc977dd88600eb8ff120ba15e40247304402204f83d997acfadec69e45d1719794850ebc3f5bee41057aeefefed8e678e0ed6802206c7653837d7b70343b0dac4f47170b261885c4fcad35b346d0da02d0cc342b7b01210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a270247304402206089cce85471b83b7afafc4c8363ca10124d831914a27817fa77e27e5a5f5b2e02200cf2c185a1caa93f84ecd25cc4b67cbf04ba3857ca4310688245a53491b9e6f601210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b0000000000");
  EXPECT_EQ(txc.GetVsize(), 413);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate);
  EXPECT_EQ(minimum_fee, 8260);
}
