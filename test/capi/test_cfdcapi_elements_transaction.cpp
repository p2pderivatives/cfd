#ifndef CFD_DISABLE_ELEMENTS
#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "cfdc/cfdcapi_transaction.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_exception.h"

/**
 * @brief testing class.
 */
class cfdcapi_elements_transaction : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_elements_transaction, CreateRawTransaction) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;
  char* tx_string2 = nullptr;
  int64_t satoshi;

  ret = CfdInitializeConfidentialTx(handle, 2, 0, &tx_string);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("0200000000000000000000", tx_string);

    ret = CfdAddConfidentialTxIn(
      handle, tx_string, "7461b02405414d79e79a5050684a333c922c1136f4bdff5fb94b551394edebbd", 0,
      4294967295U, &tx_string2);
    EXPECT_EQ(kCfdSuccess, ret);

    CfdFreeStringBuffer(tx_string);
    tx_string = tx_string2;
    tx_string2 = nullptr;
  }

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000001bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffff0000000000", tx_string);

    ret = CfdAddConfidentialTxIn(
      handle, tx_string, "1497e1f146bc5fe00b6268ea16a7069ecb90a2a41a183446d5df8965d2356dc1", 1,
      4294967295U, &tx_string2);
    EXPECT_EQ(kCfdSuccess, ret);

    CfdFreeStringBuffer(tx_string);
    tx_string = tx_string2;
    tx_string2 = nullptr;
  }

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000002bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffff0000000000", tx_string);

    satoshi = 100000000;
    ret = CfdAddConfidentialTxOut(
      handle, tx_string, "ef47c42d34de1b06a02212e8061323f50d5f02ceed202f1cb375932aa299f751",
      satoshi, nullptr, "CTEw7oSCUWDfmfhCEdsB3gsG7D9b4xLCZEq71H8JxRFeBu7yQN3CbSF6qT6J4F7qji4bq1jVSdVcqvRJ",
      nullptr, nullptr, &tx_string2);
    EXPECT_EQ(kCfdSuccess, ret);

    CfdFreeStringBuffer(tx_string);
    tx_string = tx_string2;
    tx_string2 = nullptr;
  }

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000002bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffff010151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000005f5e10003a630456ab6d50b57981e085abced70e2816289ae2b49a44c2f471b205134c12b1976a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac00000000", tx_string);

    satoshi = 1900500000;
    ret = CfdAddConfidentialTxOut(
      handle, tx_string, "6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3",
      satoshi, nullptr, "2dxZw5iVZ6Pmqoc5Vn8gkUWDGB5dXuMBCmM",
      nullptr, nullptr, &tx_string2);
    EXPECT_EQ(kCfdSuccess, ret);

    CfdFreeStringBuffer(tx_string);
    tx_string = tx_string2;
    tx_string2 = nullptr;
  }

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000002bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffff020151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000005f5e10003a630456ab6d50b57981e085abced70e2816289ae2b49a44c2f471b205134c12b1976a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f010000000071475420001976a914fdd725970db682de970e7669646ed7afb8348ea188ac00000000", tx_string);

    satoshi = 500000;
    ret = CfdAddConfidentialTxOut(
      handle, tx_string, "6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3",
      satoshi, nullptr, nullptr,
      nullptr, nullptr, &tx_string2);
    EXPECT_EQ(kCfdSuccess, ret);

    CfdFreeStringBuffer(tx_string);
    tx_string = tx_string2;
    tx_string2 = nullptr;
  }

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000002bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffff030151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000005f5e10003a630456ab6d50b57981e085abced70e2816289ae2b49a44c2f471b205134c12b1976a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f010000000071475420001976a914fdd725970db682de970e7669646ed7afb8348ea188ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f01000000000007a120000000000000", tx_string);

    CfdFreeStringBuffer(tx_string);
    tx_string = nullptr;
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

TEST(cfdcapi_elements_transaction, GetTransactionData) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static const char* tx_data = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";

  uint32_t txin_count = 0;
  uint32_t txout_count = 0;

  ret = CfdGetConfidentialTxInCount(handle, tx_data, &txin_count);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(2, txin_count);

  ret = CfdGetConfidentialTxOutCount(handle, tx_data, &txout_count);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(4, txout_count);

  char* txid = nullptr;
  char* script_sig = nullptr;
  uint32_t vout = 0;
  uint32_t sequence = 0;
  if (ret == kCfdSuccess) {
    char* wtxid = nullptr;
    char* wit_hash = nullptr;
    uint32_t size = 0;
    uint32_t vsize = 0;
    uint32_t weight = 0;
    uint32_t version = 0;
    uint32_t locktime = 0;
    ret = CfdGetConfidentialTxInfo(
        handle, tx_data, &txid, &wtxid, &wit_hash, &size, &vsize, &weight,
        &version, &locktime);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("cf7783b2b1de646e35186df988a219a17f0317b5c3f3c47fa4ab2d7463ea3992", txid);
      EXPECT_STREQ("cf7783b2b1de646e35186df988a219a17f0317b5c3f3c47fa4ab2d7463ea3992", wtxid);
      EXPECT_STREQ("938e3a9b5bac410e812d08db74c4ef2bc58d1ed99d94b637cab0ac2e9eb59df8", wit_hash);
      EXPECT_EQ(512, size);
      EXPECT_EQ(512, vsize);
      EXPECT_EQ(2048, weight);
      EXPECT_EQ(2, version);
      EXPECT_EQ(0, locktime);

      CfdFreeStringBuffer(txid);
      CfdFreeStringBuffer(wtxid);
      CfdFreeStringBuffer(wit_hash);
      txid = nullptr;
    }
  }
  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxIn(
        handle, tx_data, 1, &txid, &vout, &sequence, &script_sig);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", txid);
      EXPECT_EQ(1, vout);
      EXPECT_STREQ("", script_sig);
      EXPECT_EQ(4294967295U, sequence);

      CfdFreeStringBuffer(txid);
      CfdFreeStringBuffer(script_sig);
      txid = nullptr;
      script_sig = nullptr;
    }
  }
  if (ret == kCfdSuccess) {
    char* entropy = nullptr;
    char* nonce = nullptr;
    char* asset_value = nullptr;
    char* token_value = nullptr;
    int64_t asset_amount = 0;
    int64_t token_amount = 0;
    ret = CfdGetTxInIssuanceInfo(
        handle, tx_data, 1, &entropy, &nonce, &asset_amount, &asset_value,
        &token_amount, &token_value, nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("6f9ccf5949eba5d6a08bff7a015e825c97824e82d57c8a0c77f9a41908fe8306", entropy);
      EXPECT_STREQ("0b8954757234fd3ec9cf0dd6ef0a89d825ec56a9532e7da4b6cb90c51be3bbd8", nonce);
      EXPECT_STREQ("010000000023c34600", asset_value);
      EXPECT_STREQ("", token_value);
      EXPECT_EQ(600000000, asset_amount);
      EXPECT_EQ(0, token_amount);

      CfdFreeStringBuffer(entropy);
      CfdFreeStringBuffer(nonce);
      CfdFreeStringBuffer(asset_value);
      CfdFreeStringBuffer(token_value);
    }
  }

  if (ret == kCfdSuccess) {
    char* asset_string = nullptr;
    int64_t value_satoshi = 0;
    char* value_commitment = nullptr;
    char* nonce = nullptr;
    char* locking_script = nullptr;
    ret = CfdGetConfidentialTxOut(
        handle, tx_data, 3, &asset_string, &value_satoshi, &value_commitment,
        &nonce, &locking_script, nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("accb7354c07974e00b32e4e5eef55078490141675592ac3610e6101831edb0cd", asset_string);
      EXPECT_STREQ("010000000023c34600", value_commitment);
      EXPECT_STREQ("03ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed879", nonce);
      EXPECT_STREQ("76a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac", locking_script);
      EXPECT_EQ(600000000, value_satoshi);

      CfdFreeStringBuffer(asset_string);
      CfdFreeStringBuffer(value_commitment);
      CfdFreeStringBuffer(nonce);
      CfdFreeStringBuffer(locking_script);
    }
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

TEST(cfdcapi_elements_transaction, SetRawReissueAsset) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static const char* base_tx = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100000000ffffffff03017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000000000000";

  char* asset_string = nullptr;
  char* tx_string = nullptr;
  int64_t satoshi = 600000000;
  ret = CfdSetRawReissueAsset(
    handle, base_tx, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 1,
    satoshi, "0b8954757234fd3ec9cf0dd6ef0a89d825ec56a9532e7da4b6cb90c51be3bbd8",
    "6f9ccf5949eba5d6a08bff7a015e825c97824e82d57c8a0c77f9a41908fe8306",
    "CTExCoUri8VzkxbbhqzgsruWJ5zYtmoFXxCWtjiSLAzcMbpEWhHmDrZ66bAb41VsmSKnvJWrq2cfjUw9",
    nullptr, &asset_string, &tx_string);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("accb7354c07974e00b32e4e5eef55078490141675592ac3610e6101831edb0cd", asset_string);
    EXPECT_STREQ("0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000", tx_string);

    CfdFreeStringBuffer(asset_string);
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

TEST(cfdcapi_elements_transaction, GetIssuanceBlindingKey) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* blinding_key = nullptr;
  ret = CfdGetIssuanceBlindingKey(
    handle, "ac2c1e4cce122139bb25abc50599e09738143cc4bc96e55f399a5e1e45d916a9",
    "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 1,
    &blinding_key);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("7d65c7970d836a878a1080399a3c11de39a8e82493e12b1ad154e383661fb77f", blinding_key);

    CfdFreeStringBuffer(blinding_key);
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

TEST(cfdcapi_elements_transaction, BlindTransaction) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static const char* base_tx = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";

  char* tx_string = nullptr;
  int64_t satoshi;
  void* blind_handle = nullptr;
  ret = CfdInitializeBlindTx(handle, &blind_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    satoshi = 999637680;
    ret = CfdAddBlindTxInData(
        handle, blind_handle,
        "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        "186c7f955149a5274b39e24b6a50d1d6479f552f6522d91f3a97d771f1c18179",
        "a10ecbe1be7a5f883d5d45d966e30dbc1beff5f21c55cec76cc21a2229116a9f",
        "ae0f46d1940f297c2dc3bbd82bf8ef6931a2431fbb05b3d3bc5df41af86ae808",
        satoshi, nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 700000000;
    ret = CfdAddBlindTxInData(
        handle, blind_handle,
        "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 1,
        "ed6927df918c89b5e3d8b5062acab2c749a3291bb7451d4267c7daaf1b52ad0b",
        "0b8954757234fd3ec9cf0dd6ef0a89d825ec56a9532e7da4b6cb90c51be3bbd8",
        "62e36e1f0fa4916b031648a6b6903083069fa587572a88b729250cde528cfd3b",
        satoshi, "7d65c7970d836a878a1080399a3c11de39a8e82493e12b1ad154e383661fb77f",
        "7d65c7970d836a878a1080399a3c11de39a8e82493e12b1ad154e383661fb77f");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdAddBlindTxOutData(
        handle, blind_handle, 0,
        "02200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdAddBlindTxOutData(
        handle, blind_handle, 1,
        "02cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdAddBlindTxOutData(
        handle, blind_handle, 3,
        "03ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed879");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeBlindTx(handle, blind_handle, base_tx, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    // blind is contains random value.
    // EXPECT_STREQ(ext_tx, tx_string);
  }

  if (blind_handle != nullptr) {
    CfdFreeBlindHandle(handle, blind_handle);
  }

  // unblind test
  char* asset = nullptr;
  char* asset_blind_factor = nullptr;
  if (ret == kCfdSuccess) {
    int64_t asset_value;
    char* asset_value_blind_factor = nullptr;
    char* token = nullptr;
    int64_t token_value = 0;
    char* token_blind_factor = nullptr;
    char* token_value_blind_factor = nullptr;
    ret = CfdUnblindIssuance(
        handle, tx_string, 1,
        "7d65c7970d836a878a1080399a3c11de39a8e82493e12b1ad154e383661fb77f",
        "7d65c7970d836a878a1080399a3c11de39a8e82493e12b1ad154e383661fb77f",
        &asset, &asset_value, &asset_blind_factor,
        &asset_value_blind_factor, &token, &token_value,
        &token_blind_factor, &token_value_blind_factor);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("accb7354c07974e00b32e4e5eef55078490141675592ac3610e6101831edb0cd", asset);
      EXPECT_EQ(600000000, asset_value);
      // asset is calculate from utxo txin and vout.
      EXPECT_STREQ("0000000000000000000000000000000000000000000000000000000000000000", asset_blind_factor);
      // blind factor is generate from random value.
      EXPECT_STRNE("0000000000000000000000000000000000000000000000000000000000000000", asset_value_blind_factor);
      // EXPECT_STREQ("", token);
      // EXPECT_STREQ("", token_blind_factor);
      // EXPECT_STREQ("", token_value_blind_factor);
      EXPECT_EQ(0, token_value);
      CfdFreeStringBuffer(asset);
      CfdFreeStringBuffer(asset_blind_factor);
      CfdFreeStringBuffer(asset_value_blind_factor);
      CfdFreeStringBuffer(token);
      CfdFreeStringBuffer(token_blind_factor);
      CfdFreeStringBuffer(token_value_blind_factor);
    }
  }

  int64_t value = 0;
  char* value_blind_factor = nullptr;
  if (ret == kCfdSuccess) {
    asset = nullptr;
    asset_blind_factor = nullptr;
    ret = CfdUnblindTxOut(
        handle, tx_string, 0, "6a64f506be6e60b948987aa4d180d2ab05034a6a214146e06e28d4efe101d006",
        &asset, &value, &asset_blind_factor, &value_blind_factor);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("186c7f955149a5274b39e24b6a50d1d6479f552f6522d91f3a97d771f1c18179", asset);
      EXPECT_EQ(999587680, value);
      // blind factor is generate from random value.
      EXPECT_STRNE("0000000000000000000000000000000000000000000000000000000000000000", asset_blind_factor);
      EXPECT_STRNE("0000000000000000000000000000000000000000000000000000000000000000", value_blind_factor);
      CfdFreeStringBuffer(asset);
      CfdFreeStringBuffer(asset_blind_factor);
      CfdFreeStringBuffer(value_blind_factor);
      asset = nullptr;
      asset_blind_factor = nullptr;
      value_blind_factor = nullptr;
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdUnblindTxOut(
        handle, tx_string, 1, "94c85164605f589c4c572874f36b8301989c7fabfd44131297e95824d473681f",
        &asset, &value, &asset_blind_factor, &value_blind_factor);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("ed6927df918c89b5e3d8b5062acab2c749a3291bb7451d4267c7daaf1b52ad0b", asset);
      EXPECT_EQ(700000000, value);
      // blind factor is generate from random value.
      EXPECT_STRNE("0000000000000000000000000000000000000000000000000000000000000000", asset_blind_factor);
      EXPECT_STRNE("0000000000000000000000000000000000000000000000000000000000000000", value_blind_factor);
      CfdFreeStringBuffer(asset);
      CfdFreeStringBuffer(asset_blind_factor);
      CfdFreeStringBuffer(value_blind_factor);
      asset = nullptr;
      asset_blind_factor = nullptr;
      value_blind_factor = nullptr;
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdUnblindTxOut(
        handle, tx_string, 3, "0473d39aa6542e0c1bb6a2343b2319c3e92063dd019af4d47dbf50c460204f32",
        &asset, &value, &asset_blind_factor, &value_blind_factor);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("accb7354c07974e00b32e4e5eef55078490141675592ac3610e6101831edb0cd", asset);
      // blind factor is generate from random value.
      EXPECT_STRNE("0000000000000000000000000000000000000000000000000000000000000000", asset_blind_factor);
      EXPECT_STRNE("0000000000000000000000000000000000000000000000000000000000000000", value_blind_factor);
      EXPECT_EQ(600000000, value);
      CfdFreeStringBuffer(asset);
      CfdFreeStringBuffer(asset_blind_factor);
      CfdFreeStringBuffer(value_blind_factor);
      asset = nullptr;
      asset_blind_factor = nullptr;
      value_blind_factor = nullptr;
    }
  }

  CfdFreeStringBuffer(tx_string);

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

TEST(cfdcapi_elements_transaction, AddSignConfidentialTx) {
  static const char* const kTxData = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int64_t satoshi = 13000000000000;
  char* sighash = nullptr;
  char* tx_string = nullptr;
  char* tx_string2 = nullptr;
  ret = CfdCreateConfidentialSighash(
      handle, kTxData, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
      kCfdP2wpkh, "03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6", nullptr,
      satoshi, nullptr, kCfdSigHashAll, false, &sighash);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("c90939ef311f105806b401bcfa494921b8df297195fc125ebbd91a018c4066b9", sighash);

    // calc
    char* signature = nullptr;
    static const char* privkey = "cU4KjNUT7GjHm7CkjRjG46SzLrXHXoH3ekXmqa2jTCFPMkQ64sw1";
    ret = CfdCalculateEcSignature(
      handle, sighash, nullptr, privkey, kCfdNetworkRegtest, true, &signature);
    EXPECT_EQ(kCfdSuccess, ret);

    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c2131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea", signature);
      ret = CfdAddConfidentialTxDerSign(
        handle, kTxData, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        true, signature, kCfdSigHashAll, false, false, &tx_string);
      EXPECT_EQ(kCfdSuccess, ret);
      CfdFreeStringBuffer(signature);
    }

    if (ret == kCfdSuccess) {
      ret = CfdAddConfidentialTxSign(
        handle, tx_string, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        true, "03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6", false, &tx_string2);
      EXPECT_EQ(kCfdSuccess, ret);
    }
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0000000000000247304402200268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c20220131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea012103f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d600000000000000000000000000", tx_string2);

      uint32_t count = 0;
      ret = CfdGetConfidentialTxInWitnessCount(handle, tx_string2, 0, &count);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_EQ(2, count);

      char* stack = nullptr;
      ret = CfdGetConfidentialTxInWitness(handle, tx_string2, 0, 1, &stack);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_STREQ("03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6", stack);

      CfdFreeStringBuffer(tx_string2);
    }

    CfdFreeStringBuffer(sighash);
  }

  CfdFreeStringBuffer(tx_string);

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

TEST(cfdcapi_elements_transaction, AddMultisigSignConfidentialTx) {
  static const char* const kTxData = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";
  static const char* pubkey1 = "02715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad";
  static const char* privkey1 = "cRVLMWHogUo51WECRykTbeLNbm5c57iEpSegjdxco3oef6o5dbFi";
  static const char* pubkey2 = "02bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d71";
  static const char* privkey2 = "cQUTZ8VbWNYBEtrB7xwe41kqiKMQPRZshTvBHmkoJGaUfmS5pxzR";
  static const char* multisig_script = "522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52ae";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* multisign_handle = nullptr;
  ret = CfdInitializeMultisigSign(handle, &multisign_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    char* sighash = nullptr;
    int64_t satoshi = 13000000000000;
    char* signature = nullptr;
    char* tx_string = nullptr;
    ret = CfdCreateConfidentialSighash(
        handle, kTxData, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        kCfdP2sh, nullptr, multisig_script, satoshi, nullptr, kCfdSigHashAll, false, &sighash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("64878cbcd5c1805659d0747097cbf4b9ec5c187ebd80afa996c8fc95bd650b70", sighash);

      ret = CfdCalculateEcSignature(
        handle, sighash, nullptr, privkey1, kCfdNetworkRegtest, true, &signature);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdAddMultisigSignDataToDer(
          handle, multisign_handle, signature, kCfdSigHashAll, false, pubkey1);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }

    if (ret == kCfdSuccess) {
      ret = CfdCalculateEcSignature(
        handle, sighash, nullptr, privkey2, kCfdNetworkRegtest, true, &signature);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdAddMultisigSignDataToDer(
          handle, multisign_handle, signature, kCfdSigHashAll, false, pubkey2);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }
    if (ret == kCfdSuccess) {
      ret = CfdFinalizeElementsMultisigSign(
          handle, multisign_handle, kTxData,
          "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
          kCfdP2sh, nullptr, multisig_script, false, &tx_string);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        EXPECT_STREQ("0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a15700000000d90047304402206fc4cc7e489208a2f4d24f5d35466debab2ce7aa34b5d00e0a9426c9d63529cf02202ec744939ef0b4b629c7d87bc2d017714b52bb86dccb0fd0f10148f62b7a09ba01473044022073ea24720b24c736bcb305a5de2fd8117ca2f0a85d7da378fae5b90dc361d227022004c0088bf1b73a56ae5ec407cf9c330d7206ffbcd0c9bb1c72661726fd4990390147522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52aeffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000", tx_string);
        CfdFreeStringBuffer(tx_string);
      }
    }
    CfdFreeStringBuffer(sighash);

    ret = CfdFreeMultisigSignHandle(handle, multisign_handle);
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
