#include "gtest/gtest.h"
#include <vector>

#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_transaction_common.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"

using cfd::SignParameter;
using cfd::TransactionContext;
using cfd::Utxo;
using cfd::UtxoData;
using cfd::core::Amount;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::CfdException;
using cfd::core::NetType;
using cfd::core::OutPoint;
using cfd::core::Privkey;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptOperator;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::core::SignatureUtil;
using cfd::core::Transaction;
using cfd::core::Txid;
using cfd::core::WitnessVersion;

TEST(TransactionContext, Constructor_Test) {
  TransactionContext empty_txc;
  EXPECT_STREQ(empty_txc.GetHex().c_str(), "02000000000000000000");

  std::string exp_tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  TransactionContext hex_txc(exp_tx);
  EXPECT_STREQ(hex_txc.GetHex().c_str(), exp_tx.c_str());

  ByteData byte_data(exp_tx);
  TransactionContext bytes_txc(byte_data);
  EXPECT_STREQ(bytes_txc.GetHex().c_str(), exp_tx.c_str());

  TransactionContext copy_context;
  copy_context = bytes_txc;
  EXPECT_STREQ(copy_context.GetHex().c_str(), exp_tx.c_str());

  TransactionContext clone_context(bytes_txc);
  EXPECT_STREQ(clone_context.GetHex().c_str(), exp_tx.c_str());

  Transaction tx(exp_tx);
  TransactionContext from_tx(tx);
  EXPECT_STREQ(from_tx.GetHex().c_str(), exp_tx.c_str());

  EXPECT_EQ(TransactionContext::GetLockTimeDisabledSequence(), 0xffffffffU);
}

TEST(TransactionContext, CreateP2wpkhSignatureHash_Test) {
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  Pubkey pubkey("023e5e7a4a435f526a8b34d54c7355c8e57392b591b5a189ec88731953c568f8da");
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  Amount amount = Amount::CreateBySatoshiAmount(21000000);

  std::string expect_sighash = "a3f0fa9b3e727bb746509b40a7cac316bf36685d55bebd6c7271d6e3a976f12d";
  
  TransactionContext txc(tx);
  ByteData sighash;
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(txid, vout, pubkey, sighash_type, amount, WitnessVersion::kVersion0));
  
  EXPECT_STREQ(sighash.GetHex().c_str(), expect_sighash.c_str());
}

TEST(TransactionContext, CreateP2wshSignatureHash_Test) {
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  Script witness_script("0020aa7566645d0bce0682dca7086e103f00147bb3f4fb4e3f2e6113cb6f485e6789");
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  Amount amount = Amount::CreateBySatoshiAmount(21000000);

  std::string expect_sighash = "4a9fc1813c51bdf4f14020c7ae570b22d9005da65a37cbf9cf10b79712a8dcc9";
  
  TransactionContext txc(tx);
  ByteData sighash;
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(txid, vout, witness_script, sighash_type, amount, WitnessVersion::kVersion0));
  
  EXPECT_STREQ(sighash.GetHex().c_str(), expect_sighash.c_str());
}

TEST(TransactionContext, VerifyInputSignature_TEST_PKH) {
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  TransactionContext txc(tx);
  Privkey privkey = Privkey::GenerageRandomKey();

  // create sighash
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  Pubkey pubkey = privkey.GeneratePubkey();
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  ByteData sighash;
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(txid, vout, pubkey, 
      sighash_type));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, txid, vout,
      sighash_type));

  // check signature another pubkey
  Privkey dummy;
  while(privkey.GetHex() == (dummy = Privkey::GenerageRandomKey()).GetHex()) {
    // do nothing
  }
  EXPECT_FALSE(txc.VerifyInputSignature(signature, dummy.GeneratePubkey(), txid, vout, sighash_type));
}

TEST(TransactionContext, VerifyInputSignature_TEST_SH) {
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  TransactionContext txc(tx);
  Privkey privkey = Privkey::GenerageRandomKey();
  ScriptBuilder sb = ScriptBuilder();

  // create sighash
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  // create 1-of-2 multisig script
  Pubkey pubkey = privkey.GeneratePubkey();
  Script redeem_script = sb
      .AppendOperator(ScriptOperator::OP_1)
      .AppendData(pubkey)
      .AppendData(Pubkey("0229ebd1cac7855ca60b0846bd179ff3d411f807f3f3a43abf498e0a415c94d622"))
      .AppendOperator(ScriptOperator::OP_2)
      .AppendOperator(ScriptOperator::OP_CHECKMULTISIG)
      .Build();
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  ByteData sighash;
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(txid, vout, redeem_script,
      sighash_type));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, txid, vout,
      redeem_script, sighash_type));
}

TEST(TransactionContext, VerifyInputSignature_TEST_WPKH) {
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  TransactionContext txc(tx);
  Privkey privkey = Privkey::GenerageRandomKey();

  // create sighash
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  Pubkey pubkey = privkey.GeneratePubkey();
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  Amount amount = Amount::CreateBySatoshiAmount(21000000);
  ByteData sighash;
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(txid, vout, pubkey,
      sighash_type, amount, WitnessVersion::kVersion0));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, txid, vout,
      sighash_type, amount, WitnessVersion::kVersion0));
}

TEST(TransactionContext, VerifyInputSignature_TEST_WSH) {
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  TransactionContext txc(tx);
  Privkey privkey = Privkey::GenerageRandomKey();
  ScriptBuilder sb = ScriptBuilder();

  // create sighash
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  // create 1-of-2 multisig script
  Pubkey pubkey = privkey.GeneratePubkey();
  Script redeem_script = sb
      .AppendOperator(ScriptOperator::OP_1)
      .AppendData(pubkey)
      .AppendData(Pubkey("0229ebd1cac7855ca60b0846bd179ff3d411f807f3f3a43abf498e0a415c94d622"))
      .AppendOperator(ScriptOperator::OP_2)
      .AppendOperator(ScriptOperator::OP_CHECKMULTISIG)
      .Build();
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  Amount amount = Amount::CreateBySatoshiAmount(21000000);
  ByteData sighash;
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(txid, vout, redeem_script,
      sighash_type, amount, WitnessVersion::kVersion0));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, txid, vout,
      redeem_script, sighash_type, amount, WitnessVersion::kVersion0));
}

TEST(TransactionContext, Input_Output) {
  // input-output test
  UtxoData utxo1;
  utxo1.txid = Txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
  utxo1.vout = 1;
  UtxoData utxo2;
  utxo2.txid = Txid("8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a");
  utxo2.vout = 2;
  UtxoData utxo3;
  utxo3.txid = Txid("b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b");
  utxo3.vout = 3;
  UtxoData utxo4;
  utxo4.txid = Txid("0f093988839178ea5895431241cb4400fb31dd7b665a1a93cbd372336c717e0c");
  utxo4.vout = 4;
  std::vector<UtxoData> utxos;
  utxos.push_back(utxo3);
  utxos.push_back(utxo4);

  TransactionContext ctx(2, 0);

  EXPECT_NO_THROW(ctx.AddInput(utxo1));
  EXPECT_NO_THROW(ctx.AddInput(utxo2, 0xf0ffffff));
  EXPECT_NO_THROW(ctx.AddInputs(utxos));
  std::vector<uint32_t> indexes(2);
  Address address1("3K4cCA6U45jhvBcgc8qEdjHGDGyUMuVRpG");
  Address address2("bc1qjfw5q2ygp0gvn450h3lu0hlwjanfsc5uax7v9q");
  EXPECT_NO_THROW(indexes[0] = ctx.AddTxOut(address1, Amount(0.1)));
  EXPECT_NO_THROW(indexes[1] = ctx.AddTxOut(address2, Amount(0.01)));

  EXPECT_EQ(ctx.GetTxInCount(), 4);
  EXPECT_EQ(ctx.GetTxOutCount(), 2);
  EXPECT_EQ(ctx.GetSizeIgnoreTxIn(), 73);
  if (ctx.GetTxInCount() == 4) {
    auto ref_list = ctx.GetTxInList();
    EXPECT_STREQ(ref_list[0].GetTxid().GetHex().c_str(), utxo1.txid.GetHex().c_str());
    EXPECT_STREQ(ref_list[1].GetTxid().GetHex().c_str(), utxo2.txid.GetHex().c_str());
    EXPECT_STREQ(ref_list[2].GetTxid().GetHex().c_str(), utxo3.txid.GetHex().c_str());
    EXPECT_STREQ(ref_list[3].GetTxid().GetHex().c_str(), utxo4.txid.GetHex().c_str());
    EXPECT_EQ(ref_list[0].GetVout(), 1);
    EXPECT_EQ(ref_list[1].GetVout(), 2);
    EXPECT_EQ(ref_list[2].GetVout(), 3);
    EXPECT_EQ(ref_list[3].GetVout(), 4);
    EXPECT_EQ(ref_list[1].GetSequence(), 0xf0ffffff);
  }
  if (ctx.GetTxOutCount() == 2) {
    EXPECT_EQ(indexes[0], 0);
    EXPECT_EQ(indexes[1], 1);
    auto ref_list = ctx.GetTxOutList();
    EXPECT_STREQ(ref_list[0].GetLockingScript().GetHex().c_str(),
                 address1.GetLockingScript().GetHex().c_str());
    EXPECT_STREQ(ref_list[1].GetLockingScript().GetHex().c_str(),
                 address2.GetLockingScript().GetHex().c_str());
    EXPECT_EQ(ref_list[0].GetValue().GetCoinValue(), 0.1);
    EXPECT_EQ(ref_list[1].GetValue().GetCoinValue(), 0.01);
  }

  UtxoData utxo_stub;
  EXPECT_THROW(ctx.AddInput(utxo_stub), CfdException);
}

TEST(TransactionContext, SignWithPrivkeySimple) {
  // P2shP2wpkh
  TransactionContext ctx("0200000001aca6c902e9569c99e172c22182f943e4ab15f28602ab248f65c864874a9ddc860000000000ffffffff01005ed0b20000000017a914eca3232c3a28a4d0c03b374d0f6053e9e546e9978700000000");

  EXPECT_NO_THROW(ctx.SignWithPrivkeySimple(
      OutPoint(Txid("86dc9d4a8764c8658f24ab0286f215abe443f98221c272e1999c56e902c9a6ac"), 0),
      Pubkey("02f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef"),
      Privkey::FromWif("cRar5dsNEddUTgXuuhsq5p2NRJUKiV58PUHEPgGe1k9CW8CGRzbj", NetType::kTestnet),
      SigHashType(), Amount::CreateBySatoshiAmount(3000002000),
      AddressType::kP2shP2wpkhAddress, true));
  EXPECT_STREQ(ctx.GetHex().c_str(), "02000000000101aca6c902e9569c99e172c22182f943e4ab15f28602ab248f65c864874a9ddc8600000000171600141da984b5779f27a59dfb3499bce3823bf1824941ffffffff01005ed0b20000000017a914eca3232c3a28a4d0c03b374d0f6053e9e546e997870247304402205c8a6b770633449f129579776dfd0a839c9749860e17f7a2bde7927e4b9a8c7202205c5a10a57acef9cadd37d72fece91821db547e9c3447f2b1071f664d393b7a5c012102f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef00000000");
}

TEST(TransactionContext, AddMultisigSign) {
  // P2wpkhMultisig
  TransactionContext ctx("02000000014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff0101000000000000002200201863143c14c5166804bd19203356da136c985678cd4d27a1b8c632960490326200000000");

  std::vector<SignParameter> signatures;
  ByteData der_bytes("47ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb");
  signatures.push_back(SignParameter(der_bytes, true));
  EXPECT_NO_THROW(ctx.AddMultisigSign(
      OutPoint(Txid("ea9d5a9e974af1d167305aa6ee598706d63274e8a40f4f33af97db37a7adde4c"), 0),
      signatures,
      Script("52210205ffcdde75f262d66ada3dd877c7471f8f8ee9ee24d917c3e18d01cee458bafe2102be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec552ae"), AddressType::kP2wshAddress));

  EXPECT_STREQ(ctx.GetHex().c_str(), "020000000001014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff0101000000000000002200201863143c14c5166804bd19203356da136c985678cd4d27a1b8c63296049032620300473044022047ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f0220217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb014752210205ffcdde75f262d66ada3dd877c7471f8f8ee9ee24d917c3e18d01cee458bafe2102be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec552ae00000000");
}
