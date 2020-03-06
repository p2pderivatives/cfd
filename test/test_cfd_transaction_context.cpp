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
#include "cfd/cfd_address.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"

using cfd::AddressFactory;
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
using cfd::core::ScriptUtil;
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

TEST(TransactionContext, AddTxInOut)
{
  TransactionContext txc(2, 0);
  Address addr("1ELuNB5fLNUcrLzb93oJDPmjxjnsVwhNHn");
  Script locking_script("0014925d4028880bd0c9d68fbc7fc7dfee976698629c");
  Address script_addr(NetType::kMainnet, locking_script);
  OutPoint outpoint = OutPoint(Txid("4aa201f333e80b8f62ba5b593edb47b4730212e2917b21279f389ba1c14588a3"), 2);
  txc.AddTxIn(OutPoint(Txid("4aa201f333e80b8f62ba5b593edb47b4730212e2917b21279f389ba1c14588a3"), 0));
  txc.AddTxIn(outpoint);
  txc.AddTxOut(script_addr, Amount(int64_t{100000000000000}));
  txc.AddTxOut(addr, Amount(int64_t{109998999992700}));

  EXPECT_STREQ(txc.GetHex().c_str(), "0200000002a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0000000000ffffffffa38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0200000000ffffffff0200407a10f35a000017a91419970f64fb36fe3b7b21eca335ff70dde51eb8c8877cf951230b6400001976a914925d4028880bd0c9d68fbc7fc7dfee976698629c88ac00000000");
  EXPECT_STREQ(addr.GetAddress().c_str(), "1ELuNB5fLNUcrLzb93oJDPmjxjnsVwhNHn");

  uint32_t index = 0;
  EXPECT_TRUE(txc.IsFindTxIn(outpoint, &index));
  EXPECT_EQ(index, 1);
  EXPECT_TRUE(txc.IsFindTxOut(addr, &index));
  EXPECT_EQ(index, 1);
  EXPECT_TRUE(txc.IsFindTxOut(script_addr.GetLockingScript(), &index));
  EXPECT_EQ(index, 0);
  EXPECT_EQ(txc.GetTxOutIndex(addr), 1);
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
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(OutPoint(txid, vout), pubkey, sighash_type, amount, WitnessVersion::kVersion0));
  
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
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(OutPoint(txid, vout), witness_script, sighash_type, amount, WitnessVersion::kVersion0));
  
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
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(OutPoint(txid, vout), pubkey, 
      sighash_type));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, OutPoint(txid, vout),
      sighash_type));

  // check signature another pubkey
  Privkey dummy;
  while(privkey.GetHex() == (dummy = Privkey::GenerageRandomKey()).GetHex()) {
    // do nothing
  }
  EXPECT_FALSE(txc.VerifyInputSignature(signature, dummy.GeneratePubkey(), OutPoint(txid, vout), sighash_type));
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
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(OutPoint(txid, vout), redeem_script,
      sighash_type));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, OutPoint(txid, vout),
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
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(OutPoint(txid, vout), pubkey,
      sighash_type, amount, WitnessVersion::kVersion0));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, OutPoint(txid, vout),
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
  EXPECT_NO_THROW(sighash = txc.CreateSignatureHash(OutPoint(txid, vout), redeem_script,
      sighash_type, amount, WitnessVersion::kVersion0));

  ByteData signature;
  EXPECT_NO_THROW(signature = SignatureUtil::CalculateEcSignature(
      ByteData256(sighash.GetBytes()), privkey));

  EXPECT_TRUE(txc.VerifyInputSignature(signature, pubkey, OutPoint(txid, vout),
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
  EXPECT_EQ(ctx.GetVsizeIgnoreTxIn(), 73);
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
      SigHashType(), Amount::CreateBySatoshiAmount(int64_t{3000002000}),
      AddressType::kP2shP2wpkhAddress, true));
  EXPECT_STREQ(ctx.GetHex().c_str(), "02000000000101aca6c902e9569c99e172c22182f943e4ab15f28602ab248f65c864874a9ddc8600000000171600141da984b5779f27a59dfb3499bce3823bf1824941ffffffff01005ed0b20000000017a914eca3232c3a28a4d0c03b374d0f6053e9e546e997870247304402205c8a6b770633449f129579776dfd0a839c9749860e17f7a2bde7927e4b9a8c7202205c5a10a57acef9cadd37d72fece91821db547e9c3447f2b1071f664d393b7a5c012102f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef00000000");
}

TEST(TransactionContext, AddMultisigSign) {
  // P2wpkhMultisig
  TransactionContext ctx("02000000014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff010100000000000000220020301e775bc1c43e8e032960fd0cd5b0009a3389b4f1b5a53072cde02559fa1d6e00000000");
  Script redeem_script("522102f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef2102be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec5210205ffcdde75f262d66ada3dd877c7471f8f8ee9ee24d917c3e18d01cee458bafe53ae");

  Txid txid("ea9d5a9e974af1d167305aa6ee598706d63274e8a40f4f33af97db37a7adde4c");
  uint32_t vout = 0;

  std::vector<SignParameter> signatures;
  ByteData der_bytes1("47ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb");
  ByteData der_bytes2("30440220297e974e4728c6918cfa61f70cfb53d9b92e8a3b1e257003122bef7aecd500d002205d66d23e694fea18d1895a994f46ebd80474811a7509a67073b324f667dab2a601");
  signatures.push_back(SignParameter(der_bytes1, true));
  signatures.push_back(SignParameter(der_bytes2, false));
  signatures[0].SetRelatedPubkey(Pubkey("02be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec5"));
  signatures[1].SetRelatedPubkey(Pubkey("02f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef"));

  EXPECT_NO_THROW(ctx.AddMultisigSign(
      OutPoint(txid, vout), signatures, redeem_script, AddressType::kP2wshAddress));

  EXPECT_STREQ(ctx.GetHex().c_str(), "020000000001014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff010100000000000000220020301e775bc1c43e8e032960fd0cd5b0009a3389b4f1b5a53072cde02559fa1d6e04004730440220297e974e4728c6918cfa61f70cfb53d9b92e8a3b1e257003122bef7aecd500d002205d66d23e694fea18d1895a994f46ebd80474811a7509a67073b324f667dab2a601473044022047ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f0220217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb0169522102f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef2102be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec5210205ffcdde75f262d66ada3dd877c7471f8f8ee9ee24d917c3e18d01cee458bafe53ae00000000");
}

TEST(TransactionContext, AddSign) {
  // P2wpkhMultisig
  TransactionContext ctx("02000000014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff010100000000000000220020301e775bc1c43e8e032960fd0cd5b0009a3389b4f1b5a53072cde02559fa1d6e00000000");
  Script redeem_script("522102f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef2102be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec5210205ffcdde75f262d66ada3dd877c7471f8f8ee9ee24d917c3e18d01cee458bafe53ae");

  Txid txid("ea9d5a9e974af1d167305aa6ee598706d63274e8a40f4f33af97db37a7adde4c");
  uint32_t vout = 0;

  std::vector<SignParameter> signatures;
  ByteData der_bytes1("47ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb");
  ByteData der_bytes2("30440220297e974e4728c6918cfa61f70cfb53d9b92e8a3b1e257003122bef7aecd500d002205d66d23e694fea18d1895a994f46ebd80474811a7509a67073b324f667dab2a601");
  signatures.push_back(SignParameter(ByteData()));
  signatures.push_back(SignParameter(der_bytes2, false));
  signatures.push_back(SignParameter(der_bytes1, true));
  signatures.push_back(SignParameter(redeem_script));
  signatures[1].SetRelatedPubkey(Pubkey("02f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef"));
  signatures[2].SetRelatedPubkey(Pubkey("02be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec5"));

  EXPECT_NO_THROW(ctx.AddSign(OutPoint(txid, vout), signatures, true, true));

  EXPECT_STREQ(ctx.GetHex().c_str(), "020000000001014cdeada737db97af334f0fa4e87432d6068759eea65a3067d1f14a979e5a9dea0000000000ffffffff010100000000000000220020301e775bc1c43e8e032960fd0cd5b0009a3389b4f1b5a53072cde02559fa1d6e04004730440220297e974e4728c6918cfa61f70cfb53d9b92e8a3b1e257003122bef7aecd500d002205d66d23e694fea18d1895a994f46ebd80474811a7509a67073b324f667dab2a601473044022047ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f0220217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb0169522102f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef2102be61f4350b4ae7544f99649a917f48ba16cf48c983ac1599774958d88ad17ec5210205ffcdde75f262d66ada3dd877c7471f8f8ee9ee24d917c3e18d01cee458bafe53ae00000000");
}

TEST(TransactionContext, SequenceApiTestWithKey)
{
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

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  Address address("mtmTFSnUTqGt6AaSqoRemj7ePPZ6YGXWeo");
  // "0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"
  Address address2("bcrt1qg4nrukf07cf4slc0m4h8gq6v2guhzrw29sfnlu");

  Amount fee(int64_t{10000});
  TransactionContext txc(2, 0);
  EXPECT_NO_THROW(txc.AddInput(utxo1));
  txc.AddTxIn(OutPoint(utxo2.txid, utxo2.vout));
  txc.AddTxOut(address, utxo1.amount - fee);
  txc.AddTxOut(address2, utxo2.amount);

  std::vector<cfd::UtxoData> utxos{utxo1, utxo2};
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
  EXPECT_NO_THROW(tx = txc.Finalize());
  EXPECT_EQ(txc.GetFeeAmount().GetSatoshiValue(), fee.GetSatoshiValue());
  EXPECT_STREQ(tx.GetHex().c_str(), "02000000000102a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a47304402205d038d82d3d7043f21b9b3d0e3ea85225ffcb78d02b8531cb1ddbec7e577e7460220367c0b985b1bb76de1f95ce4696ad9717f28ed6a356765cf31a894c42f3bb81201210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffffe3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffff02706f9800000000001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac20bf02000000000016001445663e592ff613587f0fdd6e74034c5239710dca00024730440220572c4f56643d35a229bf632810ee947190414285b790e841f9e664dfd8e1f25d022021d8e4b01c08ad50bf3f1b6edfb25bb9dc8640b36dce1e26bff352097068d07f01210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a2700000000");
}

TEST(TransactionContext, SequenceApiTestWithScript)
{
  AddressFactory factory(NetType::kRegtest);
  // Address1 (p2wsh on p2pk)
  // pubkey: '0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b',
  // privkey: 'cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5'
  UtxoData utxo1;
  utxo1.block_height = 0;
  utxo1.binary_data = nullptr;
  utxo1.txid = Txid("4aa201f333e80b8f62ba5b593edb47b4730212e2917b21279f389ba1c14588a3");
  utxo1.vout = 0;
  utxo1.locking_script = Script("0020a8417024154c9283738a8216def5e28991d3ba3f6d6f8feaecb684321eab02fc");
  utxo1.redeem_script = Script("210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bac");
  utxo1.address = factory.GetAddress("bcrt1q4pqhqfq4fjfgxuu2sgtdaa0z3xga8w3ld4hcl6hvk6zry84tqt7qmyhh6w");
  utxo1.descriptor = "wsh(pk(0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b))";
  utxo1.amount = Amount(int64_t{10000000});
  utxo1.address_type = AddressType::kP2wshAddress;

  // Address2 (2 of 3 multisig)
  // pubkey1: '0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27',
  // privkey1: 'cVtoSAzA814NCpEjz1Gumv2c5jCQ1f8Axcd58NDeds8Wxrn9dMVP'
  // pubkey2: '020bc943cfbc6fb8eb668b3516c920bad7f46bd227032fa4f00b72eb55197f2281',
  // privkey2: 'cQqnH8e9qzHDAVfYHRwH3X7mDrvz9tw3v1EEYw5tfkzWbTpbsgvt'
  // pubkey3: '03ebb70cf8b4adfff5559794d2e972d55c9429dbda25cd5911615dcab422d031ae',
  // privkey3: 'cP3zjeHXgPnu3KJH4nLRbNSKbVnZgb92sPiC9ciJcsnWkubq2ny9'
  std::vector<Pubkey> pubkeys = {
    Pubkey("0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"),
    Pubkey("020bc943cfbc6fb8eb668b3516c920bad7f46bd227032fa4f00b72eb55197f2281"),
    Pubkey("03ebb70cf8b4adfff5559794d2e972d55c9429dbda25cd5911615dcab422d031ae")
  };
  std::vector<Privkey> privkeys = {
    Privkey::FromWif("cVtoSAzA814NCpEjz1Gumv2c5jCQ1f8Axcd58NDeds8Wxrn9dMVP", NetType::kTestnet),
    Privkey::FromWif("cQqnH8e9qzHDAVfYHRwH3X7mDrvz9tw3v1EEYw5tfkzWbTpbsgvt", NetType::kTestnet),
    Privkey::FromWif("cP3zjeHXgPnu3KJH4nLRbNSKbVnZgb92sPiC9ciJcsnWkubq2ny9", NetType::kTestnet)
  };
  UtxoData utxo2;
  utxo2.block_height = 0;
  utxo2.binary_data = nullptr;
  utxo2.txid = Txid("31559192b619fd52b2cc0ca54d33778acae393ed31c453e29301a3919763b9e3");
  utxo2.vout = 1;
  utxo2.redeem_script = ScriptUtil::CreateMultisigRedeemScript(2, pubkeys);
  utxo2.address = factory.CreateP2shAddress(utxo2.redeem_script);
  utxo2.locking_script = utxo2.address.GetLockingScript();
  utxo2.descriptor = "sh(wsh(multi(2,0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27,020bc943cfbc6fb8eb668b3516c920bad7f46bd227032fa4f00b72eb55197f2281,03ebb70cf8b4adfff5559794d2e972d55c9429dbda25cd5911615dcab422d031ae)))";
  utxo2.amount = Amount(int64_t{180000});
  utxo2.address_type = AddressType::kP2shP2wshAddress;

  // Address3 (OP_TRUE)
  UtxoData utxo3;
  utxo3.block_height = 0;
  utxo3.binary_data = nullptr;
  utxo3.txid = Txid("31559192b619fd52b2cc0ca54d33778acae393ed31c453e29301a3919763b9e3");
  utxo3.vout = 2;
  utxo3.locking_script = Script("51");
  // utxo3.address;
  utxo3.descriptor = "raw(51)";
  utxo3.amount = Amount(int64_t{10000});
  utxo3.address_type = AddressType::kP2shAddress;

  // "2dngFLukCWCjXVusBspjAuGsCRwW4eyAtg6";
  Address address("mtmTFSnUTqGt6AaSqoRemj7ePPZ6YGXWeo");
  // "0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27"
  Address address2("bcrt1qg4nrukf07cf4slc0m4h8gq6v2guhzrw29sfnlu");

  TransactionContext txc(2, 0);
  std::vector<cfd::UtxoData> utxos{utxo1, utxo2, utxo3};

  EXPECT_NO_THROW(txc.AddInputs(utxos));
  txc.AddTxOut(address, utxo1.amount);
  txc.AddTxOut(address2, utxo2.amount);

  ByteData tx;

  // sign1 -> fail -> ignore
  OutPoint outpoint1(utxo1.txid, utxo1.vout);
  EXPECT_THROW(txc.SignWithKey(outpoint1,
      Pubkey("0359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567b"),
      Privkey::FromWif(
          "cQSo3DLRNg4G57hRkbo2d2pY3QSuRM9eact7LroG46XyZbZByxi5", NetType::kTestnet)),
      CfdException);

  EXPECT_THROW(txc.Verify(outpoint1), CfdException);
  EXPECT_NO_THROW(txc.IgnoreVerify(outpoint1));

  // sign2 -> manual -> success
  OutPoint outpoint2(utxo2.txid, utxo2.vout);
  SigHashType sighash_type;
  ByteData256 sighash = ByteData256(txc.CreateSignatureHash(outpoint2,
      utxo2.redeem_script, sighash_type, utxo2.amount, WitnessVersion::kVersion0));
  ByteData signature2 = SignatureUtil::CalculateEcSignature(sighash, privkeys[2]);
  ByteData signature1 = SignatureUtil::CalculateEcSignature(sighash, privkeys[1]);
  SignParameter sign2(signature2, true, sighash_type);
  sign2.SetRelatedPubkey(pubkeys[2]);
  SignParameter sign1(signature1, true, sighash_type);
  sign1.SetRelatedPubkey(pubkeys[1]);
  std::vector<SignParameter> signatures = {sign2, sign1};
  txc.AddMultisigSign(
      outpoint2, signatures, utxo2.redeem_script, utxo2.address_type);
  EXPECT_NO_THROW(txc.Verify(outpoint2));

  // sign3 -> ok
  EXPECT_NO_THROW(txc.Verify(OutPoint(utxo3.txid, utxo3.vout)));

  EXPECT_NO_THROW(txc.Verify());

  EXPECT_NO_THROW(tx = txc.Finalize());
  EXPECT_EQ(txc.GetFeeAmount().GetSatoshiValue(), utxo3.amount.GetSatoshiValue());
  EXPECT_STREQ(tx.GetHex().c_str(), "02000000000103a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0000000000ffffffffe3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b69291553101000000232200204c74fe8b7289c82e22bea07e45a4d8c911e770905176c2ed8d1fe005c8b49b2bffffffffe3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b6929155310200000000ffffffff0280969800000000001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac20bf02000000000016001445663e592ff613587f0fdd6e74034c5239710dca0004004730440220329d9b55274f5d77bf9115a07e6272b28945edd846d06656812faf7bb8bbed0f0220306969ce3aa41efa9cdef778d3c649ff196876c6314fe9c9c5daa5edf4b60d10014730440220266222c9508b6f8cd558c6c6327debef845406072939b26d157b2de9f859dfab0220718f2a192eace2ba7a17a1faebfdde067d24185adb470ba45d16bf9c57ce3ecb016952210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a2721020bc943cfbc6fb8eb668b3516c920bad7f46bd227032fa4f00b72eb55197f22812103ebb70cf8b4adfff5559794d2e972d55c9429dbda25cd5911615dcab422d031ae53ae0000000000");
}
