#include "gtest/gtest.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "capi/cfdc_internal.h"
#include "cfd/cfd_transaction_common.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_transaction.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_script.h"

using cfd::Utxo;
using cfd::core::ByteData;
using cfd::core::ByteData256;

extern std::vector<Utxo> CfdGetElementsUtxoListByC(bool use_asset);

/**
 * @brief testing class.
 */
class cfdcapi_transaction : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_transaction, CreateRawTransaction) {
  static const char* exp_tx = "0100000002fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac11000000";

  void* handle = nullptr;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(
      handle, kCfdNetworkMainnet, 1, 17, nullptr, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));

  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(
        handle, create_handle, "9f96ade4b41d5433f4eda31e1738ec2b36f6e7d1420d94a6af99801a88f7f7ff", 0,
        4294967278);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(
        handle, create_handle, "8ac60eb9575db5b2d987e29f301b5b819ea83a5c6579d282d189cc04b8e151ef", 1,
        0xffffffff);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionOutput(
        handle, create_handle, 112340000, "1Cu32FVupVCgHkMMRJdYJugxwo2Aprgk7H",
        nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionOutput(
        handle, create_handle, 223450000, "16TZ8J6Q5iZKBWizWzFAYnrsaox5Z5aBRV",
        nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    char* tx = nullptr;
    ret = CfdFinalizeTransaction(handle, create_handle, &tx);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_tx, tx);
      CfdFreeStringBuffer(tx);
    }
  }

  ret = CfdFreeTransactionHandle(handle, create_handle);
  EXPECT_EQ(kCfdSuccess, ret);

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

TEST(cfdcapi_transaction, GetTransaction) {
  static const char* exp_tx = "0100000000010136641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e0100000023220020a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54ffffffff0200e9a435000000001976a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688acc0832f05000000001976a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac080047304402206ac44d672dac41f9b00e28f4df20c52eeb087207e8d758d76d92c6fab3b73e2b0220367750dbbe19290069cba53d096f44530e4f98acaa594810388cf7409a1870ce01473044022068c7946a43232757cbdf9176f009a928e1cd9a1a8c212f15c1e11ac9f2925d9002205b75f937ff2f9f3c1246e547e54f62e027f64eefa2695578cc6432cdabce271502473044022059ebf56d98010a932cf8ecfec54c48e6139ed6adb0728c09cbe1e4fa0915302e022007cd986c8fa870ff5d2b3a89139c9fe7e499259875357e20fcbb15571c76795403483045022100fbefd94bd0a488d50b79102b5dad4ab6ced30c4069f1eaa69a4b5a763414067e02203156c6a5c9cf88f91265f5a942e96213afae16d83321c8b31bb342142a14d16381483045022100a5263ea0553ba89221984bd7f0b13613db16e7a70c549a86de0cc0444141a407022005c360ef0ae5a5d4f9f2f87a56c1546cc8268cab08c73501d6b3be2e1e1a8a08824730440220525406a1482936d5a21888260dc165497a90a15669636d8edca6b9fe490d309c022032af0c646a34a44d1f4576bf6a4a74b67940f8faa84c7df9abe12a01a11e2b4783cf56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae00000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* txid = nullptr;
  char* wtxid = nullptr;
  uint32_t size = 0;
  uint32_t vsize = 0;
  uint32_t weight = 0;
  uint32_t version = 0;
  uint32_t locktime = 0;
  ret = CfdGetTxInfo(
      handle, kCfdNetworkMainnet, exp_tx, &txid, &wtxid, &size, &vsize,
      &weight, &version, &locktime);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(800, size);
  EXPECT_EQ(316, vsize);
  EXPECT_EQ(1262, weight);
  EXPECT_EQ(1, version);
  EXPECT_EQ(0, locktime);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("27eae69aff1dd4388c0fa05cbbfe9a3983d1b0b5811ebcd4199b86f299370aac", txid);
    EXPECT_STREQ("65dab5dd46a501fc695822c73d779067f2feb7c49dc47d39f86fdb2e3960b3bd", wtxid);
    CfdFreeStringBuffer(txid);
    CfdFreeStringBuffer(wtxid);
  }

  char* txin_txid = nullptr;
  char* script_sig = nullptr;
  uint32_t vout = 0;
  uint32_t sequence = 0;
  ret = CfdGetTxIn(
      handle, kCfdNetworkMainnet, exp_tx, 0,
      &txin_txid, &vout, &sequence, &script_sig);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("6eb98797a21c6c10aa74edf29d618be109f48a8e94c694f3701e08ca69186436", txin_txid);
    EXPECT_EQ(1, vout);
    EXPECT_EQ(4294967295, sequence);
    EXPECT_STREQ("220020a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54", script_sig);
    CfdFreeStringBuffer(txin_txid);
    CfdFreeStringBuffer(script_sig);
  }

  char* stack_data = nullptr;
  ret = CfdGetTxInWitness(
      handle, kCfdNetworkMainnet, exp_tx, 0, 7, &stack_data);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae", stack_data);
    CfdFreeStringBuffer(stack_data);
  }

  int64_t value_satoshi = 0;
  char* locking_script = nullptr;
  ret = CfdGetTxOut(
      handle, kCfdNetworkMainnet, exp_tx, 1,
      &value_satoshi, &locking_script);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_EQ(87000000, value_satoshi);
    EXPECT_STREQ("76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac", locking_script);
    CfdFreeStringBuffer(locking_script);
  }

  uint32_t count = 0;
  ret = CfdGetTxInCount(
      handle, kCfdNetworkMainnet, exp_tx, &count);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(1, count);

  ret = CfdGetTxInWitnessCount(
      handle, kCfdNetworkMainnet, exp_tx, 0, &count);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(8, count);

  ret = CfdGetTxOutCount(
      handle, kCfdNetworkMainnet, exp_tx, &count);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(2, count);

  uint32_t index = 0;
  ret = CfdGetTxInIndex(
      handle, kCfdNetworkMainnet, exp_tx,
      "6eb98797a21c6c10aa74edf29d618be109f48a8e94c694f3701e08ca69186436", 1, &index);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(0, index);

  ret = CfdGetTxOutIndex(
      handle, kCfdNetworkMainnet, exp_tx, "16AQVuBMt818u2HBcbxztAZTT2VTDKupPS", "", &index);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(0, index);

  ret = CfdGetTxOutIndex(
      handle, kCfdNetworkMainnet, exp_tx, "", "76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac", &index);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(1, index);

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

TEST(cfdcapi_transaction, UpdateTxOutAmount) {
  static const char* exp_tx = "0100000000010136641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e0100000023220020a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54ffffffff0200e9a435000000001976a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688acc0832f05000000001976a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac080047304402206ac44d672dac41f9b00e28f4df20c52eeb087207e8d758d76d92c6fab3b73e2b0220367750dbbe19290069cba53d096f44530e4f98acaa594810388cf7409a1870ce01473044022068c7946a43232757cbdf9176f009a928e1cd9a1a8c212f15c1e11ac9f2925d9002205b75f937ff2f9f3c1246e547e54f62e027f64eefa2695578cc6432cdabce271502473044022059ebf56d98010a932cf8ecfec54c48e6139ed6adb0728c09cbe1e4fa0915302e022007cd986c8fa870ff5d2b3a89139c9fe7e499259875357e20fcbb15571c76795403483045022100fbefd94bd0a488d50b79102b5dad4ab6ced30c4069f1eaa69a4b5a763414067e02203156c6a5c9cf88f91265f5a942e96213afae16d83321c8b31bb342142a14d16381483045022100a5263ea0553ba89221984bd7f0b13613db16e7a70c549a86de0cc0444141a407022005c360ef0ae5a5d4f9f2f87a56c1546cc8268cab08c73501d6b3be2e1e1a8a08824730440220525406a1482936d5a21888260dc165497a90a15669636d8edca6b9fe490d309c022032af0c646a34a44d1f4576bf6a4a74b67940f8faa84c7df9abe12a01a11e2b4783cf56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae00000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int64_t value_satoshi = 76543210;
  char* tx_hex = nullptr;
  ret = CfdUpdateTxOutAmount(
      handle, kCfdNetworkMainnet, exp_tx, 1, value_satoshi, &tx_hex);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("0100000000010136641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e0100000023220020a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54ffffffff0200e9a435000000001976a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688aceaf48f04000000001976a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac080047304402206ac44d672dac41f9b00e28f4df20c52eeb087207e8d758d76d92c6fab3b73e2b0220367750dbbe19290069cba53d096f44530e4f98acaa594810388cf7409a1870ce01473044022068c7946a43232757cbdf9176f009a928e1cd9a1a8c212f15c1e11ac9f2925d9002205b75f937ff2f9f3c1246e547e54f62e027f64eefa2695578cc6432cdabce271502473044022059ebf56d98010a932cf8ecfec54c48e6139ed6adb0728c09cbe1e4fa0915302e022007cd986c8fa870ff5d2b3a89139c9fe7e499259875357e20fcbb15571c76795403483045022100fbefd94bd0a488d50b79102b5dad4ab6ced30c4069f1eaa69a4b5a763414067e02203156c6a5c9cf88f91265f5a942e96213afae16d83321c8b31bb342142a14d16381483045022100a5263ea0553ba89221984bd7f0b13613db16e7a70c549a86de0cc0444141a407022005c360ef0ae5a5d4f9f2f87a56c1546cc8268cab08c73501d6b3be2e1e1a8a08824730440220525406a1482936d5a21888260dc165497a90a15669636d8edca6b9fe490d309c022032af0c646a34a44d1f4576bf6a4a74b67940f8faa84c7df9abe12a01a11e2b4783cf56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae00000000", tx_hex);
    CfdFreeStringBuffer(tx_hex);
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

TEST(cfdcapi_transaction, CfdAddSignWithPrivkeySimple) {
  static const char* tx_hex = "0100000002fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac11000000";
  static const char* exp_tx_hex = "01000000000102fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a010000001716001473a4fc7f4c3cd762c86986f61abb7274d3914bf5ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac000247304402205c933cb3e81a1cb298e62e237169714bbc3a73c071d5c95d4b8e96b658913d8a022034075443df871165b5c998ead5e728d269182edde68b8b23a9c52a91d69293d0012102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad11000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;
  ret = CfdAddSignWithPrivkeySimple(
      handle, kCfdNetworkMainnet, tx_hex,
      "8ac60eb9575db5b2d987e29f301b5b819ea83a5c6579d282d189cc04b8e151ef", 1,
      kCfdP2shP2wpkh, "02715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad",
      "cRVLMWHogUo51WECRykTbeLNbm5c57iEpSegjdxco3oef6o5dbFi",
      112340000, kCfdSigHashAll, false, true, &tx_string);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    EXPECT_STREQ(exp_tx_hex, tx_string);
    CfdFreeStringBuffer(tx_string);
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

TEST(cfdcapi_transaction, CfdCreateSighash) {
  static const char* tx_hex = "0100000002fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac11000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* sighash = nullptr;
  ret = CfdCreateSighash(
      handle, kCfdNetworkMainnet, tx_hex, "8ac60eb9575db5b2d987e29f301b5b819ea83a5c6579d282d189cc04b8e151ef",
      1, kCfdP2wpkh, "025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357", "",
      600000000, kCfdSigHashAll, false, &sighash);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("c37af31116d1b27caf68aae9e3ac82f1477929014d5b917657d0eb49478cb670", sighash);
    CfdFreeStringBuffer(sighash);
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

TEST(cfdcapi_transaction, VerifySignature) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  // p2wpkh
  ret = CfdVerifySignature(
      handle, kCfdNetworkMainnet,
      "0100000001aca6c902e9569c99e172c22182f943e4ab15f28602ab248f65c864874a9ddc860000000000ffffffff01005ed0b20000000017a914eca3232c3a28a4d0c03b374d0f6053e9e546e9978700000000",
      "772fe60c82f16e05ce9e5411bb3666f7366f327233af611ed6a4067f23248a865d111f26e2b5306df6dade2524589ce868c0c0468359ed385c9ff3d79c600549",
      kCfdP2wpkh,
      "02f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef",
      nullptr,
      "86dc9d4a8764c8658f24ab0286f215abe443f98221c272e1999c56e902c9a6ac", 0,
      kCfdSigHashAll, false, int64_t{3000002000}, nullptr);
  EXPECT_EQ(kCfdSuccess, ret);

  // p2wpkh: fail
  ret = CfdVerifySignature(
      handle, kCfdNetworkMainnet,
      "0100000001aca6c902e9569c99e172c22182f943e4ab15f28602ab248f65c864874a9ddc860000000000ffffffff01005ed0b20000000017a914eca3232c3a28a4d0c03b374d0f6053e9e546e9978700000000",
      "772fe60c82f16e05ce9e5411bb8666f7366f327233af611ed6a4067f23248a865d111f26e2b5306df6dade2524589ce868c0c0468359ed385c9ff3d79c600549",
      kCfdP2wpkh,
      "02f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef",
      nullptr,
      "86dc9d4a8764c8658f24ab0286f215abe443f98221c272e1999c56e902c9a6ac", 0,
      kCfdSigHashAll, false, int64_t{3000002000}, nullptr);
  EXPECT_EQ(kCfdSignVerificationError, ret);

  // p2wsh
  ret = CfdVerifySignature(
      handle, kCfdNetworkMainnet,
      "0100000002fe3dc9208094f3ffd12645477b3dc56f60ec4fa8e6f5d67c565d1c6b9216b36e0000000000ffffffff0815cf020f013ed6cf91d29f4202e8a58726b1ac6c79da47c23d1bee0a6925f80000000000ffffffff0100f2052a010000001976a914a30741f8145e5acadf23f751864167f32e0963f788ac00000000",
      "27dc95ad6b740fe5129e7e62a75dd00f291a2aeb1200b84b09d9e3789406b6c01a9ecd315dd6a0e632ab20bbb98948bc0c6fb204f2c286963bb48517a7058e27",
      kCfdP2wsh,
      "026dccc749adc2a9d0d89497ac511f760f45c47dc5ed9cf352a58ac706453880ae",
      "21026dccc749adc2a9d0d89497ac511f760f45c47dc5ed9cf352a58ac706453880aeadab210255a9626aebf5e29c0e6538428ba0d1dcf6ca98ffdf086aa8ced5e0d0215ea465ac",
      "f825690aee1b3dc247da796cacb12687a5e802429fd291cfd63e010f02cf1508", 0,
      kCfdSigHashSingle, false, int64_t{4900000000}, nullptr);
  EXPECT_EQ(kCfdSuccess, ret);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_transaction, VerifyTxSign) {
  static const char* const kTxHex = "02000000000102a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a000000006a47304402205d038d82d3d7043f21b9b3d0e3ea85225ffcb78d02b8531cb1ddbec7e577e7460220367c0b985b1bb76de1f95ce4696ad9717f28ed6a356765cf31a894c42f3bb81201210359bc91953b251ae501758673b9d6dd78eafa327190741536025d92217a3f567bffffffffe3b9639791a30193e253c431ed93e3ca8a77334da50cccb252fd19b692915531000000001716001445663e592ff613587f0fdd6e74034c5239710dcaffffffff02706f9800000000001976a9149157a10c10924d7550ee7079cda55db1d11a278a88ac20bf02000000000016001445663e592ff613587f0fdd6e74034c5239710dca00024730440220572c4f56643d35a229bf632810ee947190414285b790e841f9e664dfd8e1f25d022021d8e4b01c08ad50bf3f1b6edfb25bb9dc8640b36dce1e26bff352097068d07f01210206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a2700000000";
  // Address2 (pattern-x1)
  // pubkey: '0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27',
  // privkey: 'cVtoSAzA814NCpEjz1Gumv2c5jCQ1f8Axcd58NDeds8Wxrn9dMVP'
  // utxo2.txid = Txid("31559192b619fd52b2cc0ca54d33778acae393ed31c453e29301a3919763b9e3");
  // utxo2.vout = 0;
  // utxo2.locking_script = Script("a9145d54db96a28f844a744e393fcd699d6f825b284187");
  // utxo2.address = factory.GetAddress("2N1kiV9NkmZetZ3j7FuWGkBZxubBMPLxJ16");
  // utxo2.descriptor = "sh(wpkh(0206d4fabad19c61ffb180fa8a6d0f973e11485e60115557179786f7ea5d806a27))";
  // utxo2.amount = Amount(int64_t{180000});
  // utxo2.address_type = AddressType::kP2shP2wpkhAddress;
  static const char* const kTxid = "31559192b619fd52b2cc0ca54d33778acae393ed31c453e29301a3919763b9e3";
  static const uint32_t kVout = 0;

  void* handle = nullptr;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((nullptr == handle));

  if (ret == kCfdSuccess) {
    // Verification Success Case
    int64_t satoshi = 180000;
    ret = CfdVerifyTxSign(handle, kCfdNetworkMainnet, kTxHex,
        kTxid, kVout, "2N1kiV9NkmZetZ3j7FuWGkBZxubBMPLxJ16",
        kCfdP2shP2wpkhAddress, nullptr, satoshi, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdGetLastErrorCode(handle);
  if (ret != kCfdSuccess) {
    char* str_buffer = nullptr;
    ret = CfdGetLastErrorMessage(handle, &str_buffer);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("", str_buffer);
    CfdFreeStringBuffer(str_buffer);
    str_buffer = nullptr;
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

#ifndef CFD_DISABLE_ELEMENTS
TEST(cfdcapi_transaction, VerifyTxSign_Elements) {
  static const char* const kTxHex = "020000000101b7fc3ad65a21649fdf9a225a6165a2f945e895f2eac6b2bbc1d2f3681080f8030000000000ffffffff030b3584c0a40110d0a208aad09ca9be67dbe5fc7343b6287a8abb122439def1cc8c094d982039de127c6ffb3a012d7a54cb790baf1e923f48307eefd189aaa830dd4f03536ffcfc5365ae010225b8011f8b94e495574b4d0527350fef11fdbd93dbe21b17a914a3949e9a8b0b813db67c8fc5ad14194a297979cd870bcf3a513f93f3098858ba760efdcea670e9675930b210b9b7e5c33a87ebe3823d08e6b041a5120606213de33e800d38636a5a18277badf9f9393db822b15f3973aa03eaa4f0b5baacb04a6d8b37b5e70584550be4962178e20a9c0aec5faef855ccc617a914a3949e9a8b0b813db67c8fc5ad14194a297979cd870125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000000000002710000000000000000002473044022072505e593132327ae41604e5ada810fa73b2951cf0ec7a6126b837eae60b0e4902201f3b18231af61311d2f98846bdea632acff87eee02f35e3880b92c14a0eb6418012103f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d60043010001033a6e5463aeaf8460ef48520746c9a162f4b715eb6d4f00cb975e3b7a01e57092012829e2e1e87787fe5e04512e0b68500f97ae8771a44c05a87d3646e9215bfd4d0b602300000000000000014d3500fbe175b8e442af9d8322a0c892e2bca2c25561799c01c07f66cbb936e9c90404338f40bcf348912cc25855cbd38621076efd088b36b1933baf9513aea1bbd522551c82a478a7fa34e1dcda8ef648761031332ad5388ccbc171f9475c9fde056908bdec0b0c408c41691c4bed297da4b1740bbf0f81eb2ccf0f112ba6ca2c6f1d6bf3c1cecc613aaac01153bfaf7906df82402dcc8d0867eecd212fee82827e1ab5fb7ab448b36bed57bc9edb153544a961ee5e22a33385336e77f2b67dd6274c4d0232635c3112f7cb9dbb758beeac7db0ca1945c3497d1187aee1106181b2b2bd8c5602b1460085d2ac1319d0df500169a9344528feed3304ad212678e79946ebcf915cbbb9be47b0e850e0022c058e95e6e18d73227d354910b6d2ef7435c8a5f6e1355c2e35173e7bef86924a14a0f3e747562af1898a8c65ac52ba31bd860e81090f3cc1da296557685c4529681b25066d4db6febed3cbc3252e87df247a96a6f8c44550ebaf0610b3ebfdc80c744d41b4cb80b17d3ffa95024fac8ec40cbad4a2e442479e334021ee50218dabfec1d68bfda180d7d509e0d2da389f589ee6024791c2557b4175aae469e7e11b2779d1a8433ffe99ac7db6cbc7a4296c1eba50636839b1a378ad27c9fca08859689d5f08103c45f8de0eedabe7987afa9dd3aeb970cb8276c22fd329f182b8200b1d1a4baa88f2a82e54b40ae07cb10bd7c9bad09e59b461ea0f955791ace19af8dd90cb7b55cd09d319abc75686d15606b88356cf1f6838f59d61d473d4d128821d7628ac3c9b9068e65acfbab69fcbe498ee115fcb1cb3133d51d66e4727a4c37a56b66b940d40f4aaa16be4fc7a8c58d1df13614905472354a996f463c0102503a5191c4677a9666f3190555022a1f447d0cc45f65a845dd8a0a9fd9835948fe4aee490ead426df02327ce189458c85d08aad605810e5b9372023992040213e0df83c1d775c248edd7095271f88d065c073a15b37c3455b23ae946fa4cb7e40a14dc8810614deacf8ccab5634bce2f6475c35867c0c71add1d2af013269bc0973d8a54a78b2f6234c9520ea490f188aa493d612f5bef278cfb9f48e525f7eb202f3603bfe5d978b1233447872dff46a21b1d19ec8a8f6a9c4dc1c4ee3ebb4124b4c2a9d7d28705eecb04b9c592cee292155c7204a5a3f781cbc970d9549f5afba68d2e1ddc4b08ee3af6085716c7af487c92a1abe79d7c9f5027cb03190c9f47e05db5896cd94b119c53de617533dd7bdce57e9f3ce69cf8f25b6bcfce6d8abdcf14e098551d8f72256c2c04a5fb835f5b9f5ba1c7a5989a1c93e181240cb175e34ecedb9f2ef23614054f44cbaacb732e5529f02a7622c2ffaf811d205ed2be83f723ff8ba4ff23dbca65aabad5196c44c37982bbd5a54f9b38e776b52b8e817ca2f2c246dcd7b68922f9b2b4e8792c4a00c02a81de950afa532ce77ded61182f8fb65a49d416ffd697a8bab26feff05c9e0e7bcf028fd4775566f5864c4cdd51e425756369d9d4478ca027907511cedab3dcd098f9c9a019714a6ce3b40f54fcfaa6e035c4c8e566bb050ef9b9f4ac995155f2fcda379f94a3cf2819e5d57221ebe63af2dbcf319942728ada876379a0d98991d25125168307e089bd2bc24843d0b58f958d75b37f097dced7d7579d94346127d7a2029c1966ccf7e80ede17511da539ed2e4cb7b65f644e906f9c57c5a17ec9375ab5b9b867d1ab68a00371cbb5fa209d6badae3728723e0dff89cd5f9bd38dec57321b43b32c747b0f645e02df0ce9d946a723243d53a85232c4f26853b2d788c7f5c6e9974da549af5cf23813eaf88a409198729f12f5b83cccff34b6153e0d3adb79a3ce11dde701807519db5df63939d4ed44abd34971bc14a477ed35a7596907753477aabdfc15265f2625787d8a152f9b5deabc5371682657b0c9bd4dc20ca95e388de3454ec80ded9b9b268ec55e2f0816be7e9b979a81685e2885bce1f0c873d6f268fda1a8d4fe8f736e95842fc9b05f84ed1d14a67566dfe931fa13ffff2672d9f9d30b13e62ecca4ff20f462be0626c0b93493446ff90d7ff1bddd867c8c6d9ccee603ab0fb0896544a20f2f9a5f5928e026bfd0ea93cc4c26abcf6159fa2c15245f2f3190cfc201316985a17b666e16230598a84e4bc31a6ae90491ea1f4550f96360778342c84c494c9faa073624396d8ca2e1ced3b545959040be0a5cb9d8bedd9584ba2db6f3bbf2dca734806076cb406432793f7b3ba7bc4ac35325e97d1f48d219c9e0e5a1555cb49cd97fdbfda3034ef08073a490624e2e1b76450e091d878c3caf9fe066680b96af0db14d9939c489dfd387b2e86b89ecd4bf1e6cc4c7a7f0e4533c6dcb510bec483faac1a9bc73cf2e1e595e8ffad98c7c400601b56840766f4d7934b2cbd6aa70bf6ebd18436d23f963b1a7711b3966415e50a0b0c67cb6de142e1a1ba5d7dfa77542c44bbbaa0e95ed0e702a506fdab23ee7aaa7843083266693dfb6c31be76e0bc3b33fa18b11e95888a18cdc4793ae7bc1a4686c546a52c9843fa19739522cdbbf874ec59d0c38b6a4b3aedbfbe72d82b8efba6deac3cbdb2f18ac25de8c8f095f1c921431e6a7c342fb17e8d3a5cb809521765de4adb001ae63d92e109317d090425dcdc197dfdbe7440b857824ef6128397653e20c33f995b4c782db036b420945b4c2c3d953ead32378939e9b74537e2dd26cd360bac1dbf752f66f1ddbb03eede3c76efc619bb06ebac2a0cfbb6c40f9a2762995e0911950c07eb7b5ca642234e0c0df99db32dfe99253003c052db72d4f744e423cc28620e1742363e5745ba759d15b62f77bb9c6534bcc13ee8ef1381219ac07219f8a75caa256ab7c3fffd0c40f93556541c929a1754c289235c4f80b68f0cd0043c9e0c2922e512a0730a80c2b4bf038ee9d5ece4e5dbc1f84258a81da43e4921278f1e4aeffa6cbfb8b2f83e9dc756811a00a44a48498b7f0f1b8ab9b1808914cb66b3fd41e6d4ff6579747b2174b4651c66382fd6000b6db2dec4763f61667115113469b73de1d911af384a36f7e4c960e10102e015b8d1250af9ea7981e79d84b49f714aa3210fb6e7f9b2a860d9df18c981f69b479bfd744a6c054df476919407bd227c58c76f244c8b8ea4265912e95b4285da68be013e3888b92d57b8689ad1ef5c6f0d3c0bed3cf25e86931e7c4bf14042d8ca9b7c323b2875b981eb9b5883c4e3bd75233cd9ae19568a3adede390bcfa787067596ab704c9b057d6a91b77c514de5c034a96cb31db3393ead266aab3eaebf2fbeffa63c4202c5febaa3f2b848878a5c2235619071c10f6146868e735971c8a53f59d3235b0e3b4a460473e7173f29d42982f69cef40207d43e2fd308a7ff6421044c8960c991f33a9d3334a8d63109251f672fd89f75ae1d9b32559b73c5a22e019f46a503f9dfb5bc82deb5103dde9e91f145c7507ad55b21e0ff8f96b5dcb6222dc8c5e428ddd9e9c7658a8ba8d318aa6180e75c260bb83e16898a68711be33c0ba8906e3233c8c1f31c9fc75857655ef26ba7b53952dab096a48ccf9258490a0b6e05304da43a43878eb0740fe1f952b9028d450b904ebca2d00d036bd38a435ffcf73d795397a84ec18f72a93cdddd4b8dfdac4c6c7877e16b480e819857c8e18920beb0ee09e6c3b2b6baadcffeac776591744394eb512d5814c1f68ff0c73e2bb3e2f4e0186282cd8da6b8e96e1d5bca47c98329608edaebe36c01895638a7c728cf871d2f19d5833234d2277d3acfd60ebd6c4add790eee3758bd840bc22962f1fa2227ae2853813cc36b5be46e37b246f6a9b0a21b6159a82b5f92800941e33df02188761bc730c2b284aa15dd4545ae0f4c4229a4d8c9b415a691a355e1858a26c7433195a42e55c020cbc0efcb51b560d561715c546091d655dd5cac04b4ce0986941229591f33f7c96e9acef7e878043c49a4a420de859ff20695196c37660511276e43705f73751d68f7ef1929b3ab8e32faabc3c83ac13466c543650220aa2669a3ed7fe34d6f6e6ddb44e390b7338a8a3056a8643010001ff410fe519866271fcc3d47b83120429858ebaa008dce79ae35b4455d7ec3f3045ffab1a938bbf628ff8c14f87f08943acca6bdeebfb06c18b58ec8f4d1d9c51fd4d0b60230000000000000001e20700490755f44beb25382ec7d71b25290f105ca685a456ebd1a7c560386ec9d03fbcb13ae429d8a04902b5daf4ef10010551a2848a31c42a43dc4037705a3ccd2e329f4ae6b02bebe80e58062b374e35d099147cdb36dcbf174ced965c6697a4187f68eb482706f30b24c4312a93a576f07398cd3c5a001645885aa6fdd659a1aeebfd51fe2fe0fc8f26ca9071ee7480de50a9c0637a9c551fedb0215045a888c40d4629109d8c93be540df83c991ac8c1ac9c3e2bb798fcea1c4a4791925c01d349e4ee5832b6e3d53a6d2dc2193b78d97b3b0dd951a846d48d83ceb067518e558214fa17837b747285b80aa92200c5340bdfb727e55d72c26dbc7816073a3654b084022e2d951463a0838f4683ec74184f18af1c0fddbebe292b6615c3c12b7bc381943f2a0968df482be55dd45ce19491349f2cb982935c092cd281f24e3ccffea71f3c776dc79cd4338fc93e393d92de1456166bc1019c1252932f43408338c3a84716dc842cea2a069b057a968d73892429ffd02225550bef7faa19e580cda00ec18d1d60d8e338a9cab95aaa857c579904eea03873c96b23bdd7b2d7aa39d402d5e81cf3585d390c990ba6d786ff2e8183c15e8ca4faeebefd7630b54179a6edea020f6de31ba0b8f36173ba0caf6eace25d9aaaf2ec1ec3cf6a87caef74787447017a2f661598ca5252baccedcf3b7cab0d1bf3d13d69651d92c0dda2baed6f979b5bf5b1f9e4b60592da53cad89bdc53111aafa7a9d8874e7d9f145e128162b709db2557456a1f06789ff8508bce78b47fcd6d63914753e3c5002b1a3a31cf5ac6e42ab6638499c0b964681e854e7284a9fad3a96e5c53ead3ea652e9d0af6e6b86fd920edb6187fe22a03f47fb617ab4232155fd249922d4893d2c786abb074bd399b210e4461c75169f13b0ddedc25c4def03f9a7e6e58e1a9575fe59556cefab31114c3b59fcfe80883920aede6ca6db4db539367894bf9c0a465a0448c6b2f370a3c41e1a9790dcad74918f41dcc7f568559401bc4a471102e43489a731328aad4cb8f9cb459298569a724048b2e879964143837eee75e8a4906fc3bfa22581589f3ca9f6b9958c46a30747e54b5c7fe66f510c77d658458f2311140287b9fc421a48e17073707087f37c1098e9d60b67bac01f1b5b2feef6c1902bbe5581a1b13836f555e63f6fefcc715f4b818acb499ac17e9a3633bf97e975ac49cbf76aaa1ef85411a3fb9062195e202d03a6aed9b652bcb380424032cbaadc3a8b808414836fe68f29b62d27a22abdb5a618191707b442e9dba525fb13698c1af1d00db5927e8178eb7e69ecd21b3199b3eb10ce7d9619428070fa17664b0b8134b45c6c532c74fb32b581a63f3af2d675a74ab467703f4dfede85c7f570fe65c21f71c709db4ae94e4b6fb92314aafb88f953dc8ed3b28da983910413fc9bdc78627c98b519bcaec8de7cf21abbe44deea9c3bd4ead89433bbffaf7a00f1712c09601df224c9635612f97df35812b64fe88a6055d7a971611b358ff7c65320a4eec533a007824fbfb1f0640f5634a0875189321a692fda6e4b39c2e339e3072e181dd228603c82d232f3e7370d344512ea0c0ae8428835974600bbb5870bb922de9f47be0c4fc2f3632f4ae44c7029589f3463ea4418e51c4038788942306cf7715ff8a09bd27413211db78d165fd0a698968f1f1b1023feccb850c99596933da86be7cea9590ec25d487739a25f1552a7f06f8265111dbd65b20241557234a6ddff88a42222e3622c2bb8c0020ed5e21cbce129b734df3cccb008386f78eab530f9625117a0d6d29a396e849564a0c74ec2198da0200dd80a5071fb23f282d0ef2c9bb0290bbad54f91fb3175d38b66cd7b729e370f64948fccdd69703c99196afef66b2187f7a8b9ad13a012cd344e43dc16b3cfc28a680a9764c150c93bf1db12ccb98414aaefa426ef91360acdad7cc13be34660751a8af8f11975ebfb646e8fe5293b51553dfe22d8be3c7d1ab1c4850a362ce11d12b2048a64e8b6398eb5e2078a14eebd532542917b337033b6e82b35a8630cccf170bd73f7e6868634409f1c3351fc83ad399afb3847fbaa7beb4c534bbdbd87df1cb49c9462901f46f00d9e4a1c02c8d817ed31a8e77cdc271ca8f05498951dba32abc0153637790377917ce9d872a4853c67ea639befdc9873ebeb66c30b803d9866c8118e5c7ced668fab3a80b2d4b68f8a387dd0efe3b493888cd02479c7186eea9b58f6b4b8dca92dea056ba78a81d35b29e2586a25385e3574ba4e28ce2702fe1d781b38aae3dd7ab7375014e631421fedfef38b00bc2e622422c3e5cfaf92180c20903e1ab4e983ef8fa20201f47674ce3f85f37691c611155367075ed9e131606105973b428d2e2f7badd32bb4d460735aa5d5774835d5b25369a3959bc71e60a53696c7473101c76a36fcf84192dd3f8ed934c7485033519d1704c78aecda9c8abf0dc118879779dbf4ca888dfd4dad2a6e4bb832e0283563a6057237122f325c72eef7dc89adc842b6f305ed9b2e7b24723754a1d3a26e149b90ba51cb0207d46ccd0a69bd63ad1e95f641069f69639bdccea03428601606bc85a5f2b747a3476e3449ff2aeed12214468353a5326e322354fb56c3f1dac4fa61584ddc38c5a54c5fb8ec332085f50ebfffde12c46e0283656fd954d121191dea53d19fe287e8aee935011fa7b3a240afdb24db95f3ae77c86462028f5b054ef8af36d3f7b4453ad50f470fec39f95901a57d3b3bb01e952269983475f1d13c5a5921a8709332fa9586a0fd8f82f87d06e66e9ec2272fd67a9cfcc1f48b7a1da9a3af716216b811f03fcf4a12c8e832c4e36bcb83f7cae5dd17a385cb9a679d9cc64be8d9f972ea022cd9625664180e98c3f292e62b56b5bea3b9845ce720482f5fb6931dfbd35aa3fc816a776a4553360ec18888b1d9d9acb07edeb5908955373fe6984abc8dbc6724cfec5a908fd606169d0c5ff914e0db3f1a566602dba4265c67f3233b1623b016306d8d3bb0d8c2f47e101fb626c42849c9cd83b5dd7d148ff4a488b6929e52048cf0a61d8e2d506e5d6e70c8beac188de2c14bf8f7c385461257886baf4bf3ad78e16c657e3de62e5cb42015a2cce88516887e1995018a51fa5a2a6688613dde7edf213266e5520b73336cc775ea542908f3f76e67d2792e73afb3429aeae188e39aceb3d17652740ae37fb3639c16888336bc9f1cfea546d3443214d153405e03216ae0ebb79b949ead937daf5587409cf53b6be2428c289dedd6194c2a42719660bc3ca3706f8ba4141734b6e2734ec0e90bb267fc59b92e91a38f554097193494073fb7559b6571994dc5381b5b4ca4443665cf4da2f41dbc737b554a4266c5a40bccc36f94585788eaceddbcc6a082cc1265811c7546f562743264fd8c6926b4f28a7136583057701b8386ba7821ffb12ba742d8a475765c58c1ab68d44adf9a45127a9d0a456152f2ec1317c3c7dda48944fa8595d0e86662c311e6970bb6e51589105def2032e775d5025553d3f44c89044272f870aa7b19a7f71ae0e5d184b6cb7d9e697ae8a56ac1d5619e0d9f1673072928e201975ec84417c83c0fd611cde590995d7d44ad1dbc09c98e178e48bc861b37e46c5257c8a66f6323a1a33958b14eb811ab6c8827dbf16955d01b5ce46bd1e1b19afd48a70310dfbc54d8bb6ed68ec4612c71145bff3a1833e1bb8c52962ec51219abbb58de0ba4c6ca3105fc2c181809102df98ba0e6c22ed21c6d5b30fa2432e8065d5b2b98b95800d6e5600aef541990321bf28be3cff705457c9c00d2a727352e92d102b15a9f7105457b27f93111bf4552ae1588e69e7656e2f1cb723c969c6e8a886564bee122eab57b145fbb2781dea3c099633a80141dfddefa16d93ed7becedff15f196dbd8adff8c47affc10d75aec5e9e03828e371787276193cae56253fc54eb9d1bf925152ad5f3b671f3944f9f61ab35f52b3790c655dc6f0f30ce33169b563f85057b1235fbd62c1d0f9ae9642c639c951bde2baf544117687ab8a3682206ab35b010000";
  // ./cfd-cli verifysigntransaction -file txv.txt -elements -txid 03f8801068f3d2c1bbb2c6eaf295e845f9a265615a229adf9f64215ad63afcb7 -vout 0 -address ert1qav7q64dhpx9y4m62rrhpa67trmvjf2ptxfddld -addresstype "p2wpkh" -commitment 08b7057c10af7e696c1927584b006fbc3e7e914d4e7ac29f1876bf8d4a64276736
  static const char* const kTxid = "03f8801068f3d2c1bbb2c6eaf295e845f9a265615a229adf9f64215ad63afcb7";
  static const uint32_t kVout = 0;

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    // Verification Success Case
    const char* value_commitment = "08b7057c10af7e696c1927584b006fbc3e7e914d4e7ac29f1876bf8d4a64276736";
    ret = CfdVerifyTxSign(handle, kCfdNetworkLiquidv1, kTxHex,
        kTxid, kVout, "ert1qav7q64dhpx9y4m62rrhpa67trmvjf2ptxfddld",
        kCfdP2wpkhAddress, nullptr, 0, value_commitment);
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

// FundRawTransaction(BTC) =====================================================

TEST(cfdcapi_transaction, FundRawTransaction_BTC1) {
  constexpr const char* kDescriptor = "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))";
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  auto convert_to_byte = [](const uint8_t* byte_array, size_t size) -> std::string {
    std::vector<uint8_t> bytes(size);
    memcpy(bytes.data(), byte_array, bytes.size());
    std::vector<uint8_t> reverse_buffer(bytes.crbegin(), bytes.crend());
    return ByteData(reverse_buffer).GetHex();
  };

  void* fund_handle = nullptr;
  std::vector<Utxo> utxos = CfdGetElementsUtxoListByC(true);
  double effective_fee_rate = 10.0;
  double long_term_fee_rate = 10.0;
  double dust_fee_rate = -1;
  int64_t knapsack_min_change = -1;

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(
      handle, kCfdNetworkMainnet, 1, 17, nullptr, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));

  char* tx = nullptr;
  int64_t amount = 112340000;
  const char* txid1 = "9f96ade4b41d5433f4eda31e1738ec2b36f6e7d1420d94a6af99801a88f7f7ff";
  uint32_t vout1 = 0;
  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(handle, create_handle, txid1, vout1, 0xfffffffe);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdAddTransactionOutput(
        handle, create_handle, amount, "bc1qcevcszwsnmd2ew857n2mnwq7gsf62ujrutrfch",
        nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdFinalizeTransaction(handle, create_handle, &tx);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("0100000001fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff01202cb20600000000160014c6598809d09edaacb8f4f4d5b9b81e4413a5724311000000", tx);
    ret = CfdFreeTransactionHandle(handle, create_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  uint32_t target_asset_count = 1;
  if (ret == kCfdSuccess) {
    ret = CfdInitializeFundRawTx(
        handle, kCfdNetworkMainnet, target_asset_count, nullptr, &fund_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_NE(nullptr, fund_handle);
  }

  if (ret == kCfdSuccess) {
    for (const auto& utxo : utxos) {
      std::string txid = convert_to_byte(utxo.txid, sizeof(utxo.txid));
      ret = CfdAddUtxoForFundRawTx(
          handle, fund_handle,
          Txid(txid).GetHex().c_str(), utxo.vout,
          utxo.amount,
          kDescriptor,
          "");
      EXPECT_EQ(kCfdSuccess, ret);
    }

    ret = CfdAddTxInForFundRawTx(handle, fund_handle,
        "9f96ade4b41d5433f4eda31e1738ec2b36f6e7d1420d94a6af99801a88f7f7ff", 0,
        amount,  kDescriptor, "", false, false, false, 0, "");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTargetAmountForFundRawTx(handle, fund_handle, 0, 65976100,
        "", "bc1qdnf34k9c255nfa9anjx0sj5ne0t6f80p72wpce");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxIsBlind,
        0, 0, true);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxDustFeeRate,
        0, dust_fee_rate, false);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxLongTermFeeRate,
        0, long_term_fee_rate, false);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxKnapsackMinChange,
        knapsack_min_change, 0, false);
    EXPECT_EQ(kCfdSuccess, ret);

    int64_t tx_fee_amount = 0;
    uint32_t append_txout_count = 0;
    char* output_tx_hex = nullptr;
    ret = CfdFinalizeFundRawTx(handle, fund_handle, tx,
        effective_fee_rate, &tx_fee_amount, &append_txout_count,
        &output_tx_hex);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2520, tx_fee_amount);
    EXPECT_EQ(1, append_txout_count);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0100000002fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff0a9a33750a810cd384ca5d93b09513f1eb5d93c669091b29eef710d2391ff7300000000000feffffff02202cb20600000000160014c6598809d09edaacb8f4f4d5b9b81e4413a57243f00da804000000001600146cd31ad8b8552934f4bd9c8cf84a93cbd7a49de111000000", output_tx_hex);
      CfdFreeStringBuffer(output_tx_hex);
    }

    char* address = nullptr;
    ret = CfdGetAppendTxOutFundRawTx(handle, fund_handle, 0, &address);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("bc1qdnf34k9c255nfa9anjx0sj5ne0t6f80p72wpce", address);
    if (ret == kCfdSuccess) {
      CfdFreeStringBuffer(address);
    }
  }
  ret = CfdFreeFundRawTxHandle(handle, fund_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  CfdFreeStringBuffer(tx);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

// FundRawTransaction(multi Asset) =====================================================

#ifndef CFD_DISABLE_ELEMENTS
static constexpr const char* exp_dummytx_asset_ca = "aa00000000000000000000000000000000000000000000000000000000000000";
static constexpr const char* exp_dummytx_asset_cb = "bb00000000000000000000000000000000000000000000000000000000000000";
static constexpr const char* exp_dummytx_asset_cc = "cc00000000000000000000000000000000000000000000000000000000000000";

// reserved address key
// {    // ex1qdnf34k9c255nfa9anjx0sj5ne0t6f80p5rne4e, bc1qdnf34k9c255nfa9anjx0sj5ne0t6f80p72wpce
//   pubkey: '0367ebd58ffca93a4f4a2a9c3cc24009101f39449e0993ca08b05fdbfb6e32a01b',
//   privkey: 'cTmuwbL1JXexLBLUoQd2nVo3mggkgkJ1RSRYj7cbiUsa5CYeQCkr'
// }, { // ex1q3tlca6nma70vvrf46up5ktjguxqwj0zamt7ktn
//   pubkey: '0202acaad1eee2b86a44687825e535d61a67f9276db720165fc296cdb1f6f92b9a',
//   privkey: 'cNaupUdFeVnWoWvqP9tStbpLKdTtUe2zgAm7L6vFxQVhk2JwgzRQ'
// }, { // ex1q0xdg60c3y5dk5m05hg2k52xavjkedx53t3k40m
//   pubkey: '02ba35856676b525fead8de496d15ed080bd8442cc2672f5cac81be1fd29e92fba',
//   privkey: 'cP12ycK4NMfT1Nur6HcQMvbiihMYquESYv5nE7jvHXpwK57Mj9Ey'
// }

// addr: ex1qcevcszwsnmd2ew857n2mnwq7gsf62ujrkz734h, bc1qcevcszwsnmd2ew857n2mnwq7gsf62ujrutrfch
// pubkey: '03ac6b4b32eb27ec1c13c6180dce6b0016e46a070bdac96b4d76f5da16ab66ae8f',
// privkey: 'cSzUivMfRpXotHt27jTKpP6uJyxmQFbkMkQnvMPSq4gBdySxFvAR'

TEST(cfdcapi_transaction, FundRawTransaction_Asset1) {
  constexpr const char* kDescriptor = "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))#4z2vy08x";
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  auto convert_to_byte = [](const uint8_t* byte_array, size_t size) -> std::string {
    if (size == 32) {
      std::vector<uint8_t> bytes(size);
      memcpy(bytes.data(), byte_array, bytes.size());
      std::vector<uint8_t> reverse_buffer(bytes.crbegin(), bytes.crend());
      return ByteData(reverse_buffer).GetHex();
    } else {
      std::vector<uint8_t> bytes(32);
      memcpy(bytes.data(), byte_array + 1, bytes.size());
      std::vector<uint8_t> reverse_buffer(bytes.crbegin(), bytes.crend());
      return ByteData(reverse_buffer).GetHex();
    }
  };

  void* fund_handle = nullptr;
  std::vector<Utxo> utxos = CfdGetElementsUtxoListByC(true);
  double effective_fee_rate = -1;
  double long_term_fee_rate = -1;
  double dust_fee_rate = -1;
  int64_t knapsack_min_change = -1;

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(
      handle, kCfdNetworkLiquidv1, 1, 17, nullptr, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));

  char* tx = nullptr;
  int64_t amount = 112340000;
  const char* txid1 = "9f96ade4b41d5433f4eda31e1738ec2b36f6e7d1420d94a6af99801a88f7f7ff";
  uint32_t vout1 = 0;
  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(handle, create_handle, txid1, vout1, 0xfffffffe);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdAddTransactionOutput(
        handle, create_handle, amount, "ex1qcevcszwsnmd2ew857n2mnwq7gsf62ujrkz734h",
        nullptr, exp_dummytx_asset_ca);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdFinalizeTransaction(handle, create_handle, &tx);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("010000000001fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff010100000000000000000000000000000000000000000000000000000000000000aa010000000006b22c2000160014c6598809d09edaacb8f4f4d5b9b81e4413a5724311000000", tx);
    ret = CfdFreeTransactionHandle(handle, create_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  uint32_t target_asset_count = 3;
  if (ret == kCfdSuccess) {
    ret = CfdInitializeFundRawTx(
        handle, kCfdNetworkLiquidv1, target_asset_count,
        exp_dummytx_asset_ca, &fund_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_NE(nullptr, fund_handle);
  }

  if (ret == kCfdSuccess) {
    for (const auto& utxo : utxos) {
      std::string txid = convert_to_byte(utxo.txid, sizeof(utxo.txid));
      ret = CfdAddUtxoForFundRawTx(
          handle, fund_handle,
          Txid(txid).GetHex().c_str(), utxo.vout,
          utxo.amount,
          kDescriptor,
          convert_to_byte(utxo.asset, sizeof(utxo.asset)).c_str());
      EXPECT_EQ(kCfdSuccess, ret);
    }

    ret = CfdAddTxInForFundRawTx(handle, fund_handle,
        "9f96ade4b41d5433f4eda31e1738ec2b36f6e7d1420d94a6af99801a88f7f7ff", 0,
        amount,  kDescriptor, exp_dummytx_asset_ca,
        false, false, false, 0, "");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTargetAmountForFundRawTx(handle, fund_handle, 0, 115800000,
        exp_dummytx_asset_ca, "ex1qdnf34k9c255nfa9anjx0sj5ne0t6f80p5rne4e");
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdAddTargetAmountForFundRawTx(handle, fund_handle, 1, 347180040,
        exp_dummytx_asset_cb, "ex1q3tlca6nma70vvrf46up5ktjguxqwj0zamt7ktn");
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdAddTargetAmountForFundRawTx(handle, fund_handle, 2, 37654100,
        exp_dummytx_asset_cc, "ex1q0xdg60c3y5dk5m05hg2k52xavjkedx53t3k40m");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxIsBlind,
        0, 0, true);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxDustFeeRate,
        0, dust_fee_rate, false);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxLongTermFeeRate,
        0, long_term_fee_rate, false);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxKnapsackMinChange,
        knapsack_min_change, 0, false);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxBlindExponent,
        0, 0, false);
    EXPECT_EQ(kCfdSuccess, ret);
    ret = CfdSetOptionFundRawTx(handle, fund_handle, kCfdFundTxBlindMinimumBits,
        52, 0, false);
    EXPECT_EQ(kCfdSuccess, ret);

    int64_t tx_fee_amount = 0;
    uint32_t append_txout_count = 0;
    char* output_tx_hex = nullptr;
    ret = CfdFinalizeFundRawTx(handle, fund_handle, tx,
        effective_fee_rate, &tx_fee_amount, &append_txout_count,
        &output_tx_hex);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(109700, tx_fee_amount);
    EXPECT_EQ(3, append_txout_count);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("010000000006fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff0a9a33750a810cd384ca5d93b09513f1eb5d93c669091b29eef710d2391ff7300000000000feffffff0ad4a335556c64c3e2599c3a4c3ddff5b28f616fa55cf2323d2ae642eef74a8f0000000000feffffff030b0000000000000000000000000000000000000000000000000000000000000000000000feffffff020b0000000000000000000000000000000000000000000000000000000000000000000000feffffff010c0000000000000000000000000000000000000000000000000000000000000000000000feffffff050100000000000000000000000000000000000000000000000000000000000000aa010000000006b22c2000160014c6598809d09edaacb8f4f4d5b9b81e4413a572430100000000000000000000000000000000000000000000000000000000000000aa01000000000001ac8400000100000000000000000000000000000000000000000000000000000000000000bb010000000014b18c12001600148aff8eea7bef9ec60d35d7034b2e48e180e93c5d0100000000000000000000000000000000000000000000000000000000000000cc0100000000023e8eb800160014799a8d3f11251b6a6df4ba156a28dd64ad969a910100000000000000000000000000000000000000000000000000000000000000aa010000000006fa7728001600146cd31ad8b8552934f4bd9c8cf84a93cbd7a49de111000000", output_tx_hex);
      CfdFreeStringBuffer(output_tx_hex);
    }

    for (uint32_t index = 0; index < append_txout_count; ++index) {
      char* address = nullptr;
      ret = CfdGetAppendTxOutFundRawTx(handle, fund_handle, index, &address);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        if (index == 0) {
          EXPECT_STREQ("ex1q3tlca6nma70vvrf46up5ktjguxqwj0zamt7ktn", address);
        } else if (index == 1) {
          EXPECT_STREQ("ex1q0xdg60c3y5dk5m05hg2k52xavjkedx53t3k40m", address);
        } else if (index == 2) {
          EXPECT_STREQ("ex1qdnf34k9c255nfa9anjx0sj5ne0t6f80p5rne4e", address);
        }
      }
      CfdFreeStringBuffer(address);
    }
  }
  ret = CfdFreeFundRawTxHandle(handle, fund_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  CfdFreeStringBuffer(tx);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_transaction, VerifyTxSignElements) {
  static const char* const kTxHex = "0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0000000000000247304402200268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c20220131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea012103f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d600000000000000000000000000";
  // static const char* const kSignature = "0268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c2131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea";
  // static const char* const kPubkey = "03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6";
  // static const char* const kPrivkey = "cU4KjNUT7GjHm7CkjRjG46SzLrXHXoH3ekXmqa2jTCFPMkQ64sw1";
  static const char* const kTxid = "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f";
  static const uint32_t kVout = 0;

  void* handle = nullptr;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((nullptr == handle));

  if (ret == kCfdSuccess) {
    // Verification Success Case
    int64_t satoshi = 13000000000000;
    // const char* value_commitment = "0993c069270bf8d090ce8695b82e52fb2959a9765d987d4ffd7a767b0c5b1c4cbc";
    ret = CfdVerifyTxSign(handle, kCfdNetworkLiquidv1, kTxHex,
        kTxid, kVout, "ert1qav7q64dhpx9y4m62rrhpa67trmvjf2ptxfddld",
        kCfdP2wpkhAddress, nullptr, satoshi, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdGetLastErrorCode(handle);
  if (ret != kCfdSuccess) {
    char* str_buffer = nullptr;
    ret = CfdGetLastErrorMessage(handle, &str_buffer);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("", str_buffer);
    CfdFreeStringBuffer(str_buffer);
    str_buffer = nullptr;
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

#endif  // CFD_DISABLE_ELEMENTS
