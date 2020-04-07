#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "capi/cfdc_internal.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_script.h"

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
