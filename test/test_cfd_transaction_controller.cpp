#include "gtest/gtest.h"
#include <vector>

#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfd/cfd_transaction.h"

using cfd::core::Amount;
using cfd::core::CfdException;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::Txid;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::TransactionController;

TEST(TransactionController, CreateP2wpkhSignatureHash_Test)
{
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  Pubkey pubkey("023e5e7a4a435f526a8b34d54c7355c8e57392b591b5a189ec88731953c568f8da");
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  Amount amount = Amount::CreateBySatoshiAmount(21000000);

  std::string expect_sighash = "a3f0fa9b3e727bb746509b40a7cac316bf36685d55bebd6c7271d6e3a976f12d";
  
  TransactionController txc(tx);
  std::string sighash;
  EXPECT_NO_THROW(sighash = txc.CreateP2wpkhSignatureHash(txid, vout, pubkey, sighash_type, amount));
  
  EXPECT_STREQ(sighash.c_str(), expect_sighash.c_str());
}

TEST(TransactionController, CreateP2wshSignatureHash_Test)
{
  // input-only transaction
  std::string tx = "0200000001efcdab89674523010000000000000000000000000000000000000000000000000000000000ffffffff01406f4001000000001976a9144b8fe2da0c979ec6027dc2287e65569f41d483ec88ac00000000";
  Txid txid("0000000000000000000000000000000000000000000000000123456789abcdef");
  uint32_t vout = 0;
  Script witness_script("0020aa7566645d0bce0682dca7086e103f00147bb3f4fb4e3f2e6113cb6f485e6789");
  SigHashType sighash_type(SigHashAlgorithm::kSigHashAll);
  Amount amount = Amount::CreateBySatoshiAmount(21000000);

  std::string expect_sighash = "4a9fc1813c51bdf4f14020c7ae570b22d9005da65a37cbf9cf10b79712a8dcc9";
  
  TransactionController txc(tx);
  std::string sighash;
  EXPECT_NO_THROW(sighash = txc.CreateP2wshSignatureHash(txid, vout, witness_script, sighash_type, amount));
  
  EXPECT_STREQ(sighash.c_str(), expect_sighash.c_str());
}