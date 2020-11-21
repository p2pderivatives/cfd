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
  EXPECT_EQ(calc_fee.GetSatoshiValue(), 7660);
  EXPECT_EQ(tx_fee.GetSatoshiValue(), 1500);
  EXPECT_EQ(utxo_fee.GetSatoshiValue(), 6160);

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
  EXPECT_EQ(estimate_fee.GetSatoshiValue(), 8280);

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
  EXPECT_STREQ(tx.GetHex().c_str(), "02000000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a473044022059cdb9324ed00b687a40d79b571b6fdd6a6722f1c06990cb59d7f6285c3fe10a022019c6a8effa8d2424c7776360dbebb7bf4109dc8ae4a1f365096140cfe866d9b001210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff03f058dd62b22102001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac0000e941cc6b010016001445663e592ff613587f0fdd6e74034c5239710dcab806000000000000160014f330ed8383f8afdc977dd88600eb8ff120ba15e4024730440220472e5bfb0d4d76fbe3a5803582d6255c8d50281c83952db666bf51a52090ddf702202022598b524d76e26dff12a0bb9e5a97548c396675166bb8a6b559768a29ff9501210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a2702473044022040d76c0f8e164c1a8623e299fc7a04a29138a40557ea07a7f535a14d0c684cda02201d33ee4ae92e673f0b6b5cb55bc5d5eef7b6eca31b1fb636fe92dce50a99daf801210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b0000000000");
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
  EXPECT_EQ(estimate_fee.GetSatoshiValue(), 8280);

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
  EXPECT_STREQ(tx.GetHex().c_str(), "02000000000103e3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0100000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a473044022011d7db13ea4f19e2768dcd172638bc425d7bfb96d0da1b13dda3eccbeb2dca6e02203b0d30ffb300391da30f53f2c4827383b7199e4b2ad4eae7e26120cef2edc63f01210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffff03f0187a10f35a00001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac00008d49fd1a070016001445663e592ff613587f0fdd6e74034c5239710dcab806000000000000160014f330ed8383f8afdc977dd88600eb8ff120ba15e40247304402204aa7696f33a2fbf5177d1eea9539aea9b668cb7f71f4cc040f679a1595dd369a02203c3f88e4e18d0514d303a78026c22cfdc9a1c8f62e8a2083ed48d8df9f2fc15a01210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27024730440220090a80c1f5a5c0299725342894a972fe1841d5a82c0836bb304d88b59f5ca96d02201399cbb2264b7d00a59c2c9534169688ba8830e69b9bb7a1709bfea50deebc8d01210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b0000000000");
  EXPECT_EQ(txc.GetVsize(), 413);

  uint32_t minimum_fee = txc.GetVsize() * static_cast<uint32_t>(effective_fee_rate);
  EXPECT_EQ(minimum_fee, 8260);
}
