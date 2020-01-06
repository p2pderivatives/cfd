#ifndef CFD_DISABLE_ELEMENTS
#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfd/cfd_elements_address.h"
#include "cfd/cfdapi_elements_transaction.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "cfdc/cfdcapi_transaction.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_amount.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_elements_transaction.h"

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
      if (ret == kCfdSuccess) {
        CfdFreeStringBuffer(stack);
      }

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

TEST(cfdcapi_elements_transaction, UnblindTx_VerifySignatureTest) {
  static const char* const kTxHex = "02000000000117c10bbfcd4e89f6c33864ed627aa113f249343f4b2bbe6e86dcc725e0d06cfc010000006a473044022038527c96efaaa29b862c8fe8aa4e96602b03035505ebe1f166dd8b9f3731b7b502207e75d937ca1bb2e2f4208618051eb8aad02ad88a71477d7a6e7ec257f72cb6500121036b70f6598ee5c00ad068c9b86c7a1d5c433767a46db3bc3f9d53417171db1782fdffffff0301bdc7073c43d37ace6b66b02268ece4754fe6c39a985a16ccbe6cf05b89014d7201000000001dcd6500001976a91479975e7d3775b748cbcd5500804518280a2ebbae88ac01bdc7073c43d37ace6b66b02268ece4754fe6c39a985a16ccbe6cf05b89014d7201000000001dcccde80017a9141cd92b989652fbc4c2a92eb1d56456d0ef17d4158701bdc7073c43d37ace6b66b02268ece4754fe6c39a985a16ccbe6cf05b89014d7201000000000000971800000a000000";
  static const char* const kSignature = "62fb98d01be75290a782acf16332e30b23a867826717326620fa521134643b65477c8821722b55c46f4b06e379743f66e96bf848c4bd8664b3244a53920baefc";
  static const char* const kPubkey = "02f8b92edfdac52ab1f081d3caac42b76226d87d828a8e601a3f7ac06441c6a03a";
  // static const char* const kPrivkey = "cQ4oy4iVosJYxNxjdKFEtR6RZxWGx8doFPhNWZRd31g9fVuiQVi3";
  static const char* const kTxid = "fc6cd0e025c7dc866ebe2b4b3f3449f213a17a62ed6438c3f6894ecdbf0bc117";
  static const uint32_t kVout = 1;

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    bool result = false;
    int64_t satoshi = 1000000000;
    ret = CfdConfidentialTxVerifySignature(handle, kTxHex, kSignature, kPubkey,
        NULL, kTxid, kVout, CfdSighashType::kCfdSigHashAll, false, satoshi,
        NULL, CfdWitnessVersion::kCfdWitnessVersionNone, &result);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_TRUE(result);
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

TEST(cfdcapi_elements_transaction, BlindTx_VerifySignatureTest) {
  static const char* const kTxHex = "02000000010117c10bbfcd4e89f6c33864ed627aa113f249343f4b2bbe6e86dcc725e0d06cfc010000006a473044022038527c96efaaa29b862c8fe8aa4e96602b03035505ebe1f166dd8b9f3731b7b502207e75d937ca1bb2e2f4208618051eb8aad02ad88a71477d7a6e7ec257f72cb6500121036b70f6598ee5c00ad068c9b86c7a1d5c433767a46db3bc3f9d53417171db1782fdffffff030bc7b2b8da0c30f37b12580e0bd092bfbe16e28494fe30feab1769ab4135d30a7609c8e4c3012d90840dbed48eeebc253399645119c01125488640f415285e6e7663031651fe4267cf54e606a83e4c741a01df124d4eb915ae37ad9d5191661d74310b1976a91479975e7d3775b748cbcd5500804518280a2ebbae88ac0b8f78a97d70ad5799c1ef2ca5f7f553ec30fdc87392eb2b5a9acc42d72f5900250978d7a93a301c65b0759c8e7f4b4424ff2a4c124ee2467b08e266423faa3afef30249a8537d641e728c2192f66433e125041341b36fa3e3cde5578ad1acb9a3944b17a9141cd92b989652fbc4c2a92eb1d56456d0ef17d4158701bdc7073c43d37ace6b66b02268ece4754fe6c39a985a16ccbe6cf05b89014d7201000000000000971800000a0000000000000043010001cd72e3aa85cc53ce42edd91e03a6b4d3cd6b08d2125f019639f0ae6ee29e7f8539232c60e68020ab948d95e5c70da679309e8a511a1a0ef65c7b5e5f4dfb83c7fd4d0b60230000000000000001c39d00a796090bd9872e87a9d5f06b5c73c4fe64104a59c25536f90813b311c90de520f63a57875a2e9b6111f41f5fb7b4a253c76027af35fbcf7ddb9e5688ebefb04948f349e48e2ef4cd73b17f1d5786822222f2a1e5367bd1d39b139283800bfc4b7d50ce927469541151be53b3518b0fa1e9acb8089072976b1d659e8136c666f0b57cec51775ccd40998ee57ace137e25ee7e066d9d434c0e54304913019db87855de47f7f1e974578dfedb95a92048fafe0dca541bd917ea22d9c02fd58fafc8ee35f016b4b1ebf0051a314038201163b3fc6fa09b7ac0bf45474b216f8e152433433193b6e5db6da465ddd5c0e7d23b6a2e153998e0e936539aa3ce4f1ed448157dff1f420c404c019ec5e86ab18a9b859cc3165a7f104f3a7a9abb6835a62834750f110730e7d16de16cbb7f6607fc4ae04de5baec980e3137766c23568f8bb03473d3e043d3a8d8da0a2613bf27d0ce388ed44b8e1a217b2ef5193d19fb6b943c1b8a1bebfb02b9cea87fec0edbe03ee63a3a1168c53456af1fed9fe7707b2fb58159922cb84e1e28a29d26e036c12c91666096d556eefada060c530a28837f37a456847a26fcee92092837c14144ac3e1f3a84763e40cced0d77dcfe76f537825e08d2441d612d5e80eef617ffa6b96e30825d8905bef96dd4da8457f8d43e3f3c0294443c0b5a5a2a1b2f32f6402873c7de17276ec66b0fddf246fcb305e88a09c45e1c1322d5e152216d09a02654e83b503454dfeacd52129b4956ad300a9d52ca9f955f1babb56a1a676f243ce7ef69cacbfb23cb71907a6b8ff1a42c4769c5298d5fd5027e1c352a582231a5323b02da24ba5c2df5e125554ed0ee1cb5e9e65eda5c683469d264c9f0adbfc5fcbb3459da30ef31dd7cb32a9d5d6d2b412db81df3345f09065151b0190ec07ff0c5573d4c783be1f8aeb27d8042387352d62e808b24b834fef27d1c986da5207e5c9645026bff40577fc167b321fa17b31872bc0f796dc834f66f9302a4ce505f3f1e865a0ba9be615efe25dbdb2c3b61bc03206e6891469ce11c4065a2fcf03cb436d9182d66ce452038ac5f4fd8dc61e20f4a4f8cb7ad81e9a66c8b12d592c445ffe905495bf1e277de55a10da8c48dfdb0db1c2d2be9077c47326d0611dff08131063680fd858ad72e53d3eafe0a85f436eb3e03100a0f98057ddf1c47372ff318bad1b3cae928aeaf98608e397e4d8aa0c2fa594eaa9386e6fc8642077cc6c2f81a3e59704269a556eeb90161ef0f7271b798a5ac6430e0986d6c6c5b0ddeb2ef22c873a338824ba46bd3b634cd30143d66237eed3d89041e38e178f1dee6a5c0da039804face0a90c6c8b58a5b86b402e964678029a71c8c4c82ffba9fdef3c055493eef3e61b5e3aef9cc73816eb3360d2c719b198428f5a904ee1f241b20712aa67f4737fe56ba3884bc0aaaee8faa2d39ccf125f92c35877bb0ecccf0d95376a30464873475f62faadb18013e3cc879c4e42b166d20042980b84aa9be48ef7578664464956caabcbf3ab2cec3f87b0ae1c7d3ebe2234489491ceb0f4b3445308dc4c41f68ada861c95e9f0b0ed2da0b9bdc882923a4ec7118fe6eb1af554aa018c7df6f6987c353ef63017add74324b46c5a6910f8d929ff3d2ec271207fa7220ddaec2c3746fb8a12d49b2b0e8896a08f44e49c58ebe7eae582982e7bd5abd9289da25cddeb8e14c56faf1443be3a51516ac8eb463b643b10bec77052b48397bdae8a3948b19a58a98c2ded30b54abad2d930fcbb4ac74e6557b8764bafc566988071e74e1f516b367fcb6cbc23ce4171dbe8bbdfd8347f121e509052bc1870dd22bdfcbfca952ccd751005dd11649a3ee9db35a65d1975166a29a1186381f4055db5940ffc4034c68360f1c3ef6a20a7aece3e084d8c67eb48dddb3ec4c964c11826172edf44bae8676b2cfcf81cdf2f16e642081944a46bd2ddfffcb0b1862d83ef5592e57409c8f7b6d359a2d0cb1d1fd0d2f55428144764f0127ad78d202c0fdbc6ee5139d33fc78183199115dc21a4a7a006559deda04f01a21d32a41950d324f1b728583daaadd4c355c04a9496e485393803099ce10627f214bf872f1dd3848afe1e884e21db791a596cd7e9eb5cb1ed24ddaee49b90baf83425f48067c367b7038db82ba50ecc5363cc9ef954c583e3eaaaa9579e34a8f28acdc51f857154bfa3db2cbff5b0513f5d91de7195922e4f092602b0c4e2efae95f00030cb8f9a9f717917e279d5c4139f54866e765b3db872b7a6085452bb9a548c3613b458bb41aa80b56e6b47bcc1af86ea391ad446a5d1f3552255645ac224653a52e0ac112c84455979a58bac88eccd346ed99a6ab7fccb98daa062e387fe31501be23406cbd48e44f11801b75dfe93efadc49564ce57afd4cfd39cd1616f8e50b16e8e06311c04e4c98ba4fed7496666e6526cfbc5a9d3121fb25e9744914bfad8c3de1a18b942f7f0dccd89d0ed3a3e4d84d2664acf781e365bb3c2b3e9340db66b8b2a5850535898113f0e9e1e215c70c5241e82c005b2e45e1d73f51b6cc8adf2a0f5e6a931005fd6bd5e572937f79f75b0bc09e6e606e3769b23ee96ddeb3058d7f7fc9c9c0dfce5486032be1478fa5452d9ed019025760543179b002e68f1e9483e35d50bbab770b2639ad95f6667a59451de23f45cc7e50f1ec55374426e16a9ab3d6f8d16da2c7ff020e7972a66bb05bd707ac78c51c2238442eb24cad8db44439388e979714d5a5146c5c1609dbdcce2f5d8040f50dd2b83f57577c6e4795b6e753a58075939429cc4afce88e212e0fb09fe462b81c2b53cf0e7f8c483e5bebc3ed9dd29302d527a8994bf1564d80c5f93e724256f5462eceddfb42643c0f9626c16f04f438ce1838037620a5cb25347603955a29c6ca4a9cc9ca7a6f4b0f70c31bce11a30ddb456284df75774f1e7a43fcece176d91681ecaaaf03163d214a0164ce8408346e32548b04c050ad536e030bd5937a889faf49e58a7541c4a851f7d7e3033cb67736922bd501c9e3f9874ccb15d83814b2289e5b189e465b8e2bf2e2c7fd3809acbb3006c6cc52794efe490c81a9aa47e70041fe83d665501755fdb58aec42b0868bacccc64cdc93718357292a1194bae59dd878d0652a8f3617ac27d70bad6a13ed603dd5cbaa3bad81be71080d5d83b17e17268ab2886305dd1255f71513bfec828b09d8fec5747cdbb04fbac230328554f5c5a1447767be43e3478e6656470df605b8f8f6da8e1180d27ba691e81544c70ee865be596a9189474a4ecd5d747c1dce7b13d6d87548f365e261e9614fa0f23092eadc4736c507735cda4de06d0d26cc1b56e5f73ee90fe5b98bdb13da7deaa2b69ef45ebff11fb996f05bf20da22bf8b0ad42c709a66a96826330468621c11aec2037653676fdff88b8608620c6b66fd6dfe32d9a26e78ddc30af791353018d8ac8932c02750c4c65b7521b5b06ac67c6cfb7208c566e936a22c975542f898bd21c323633dd88de7e2cce6fcac321427616fd0251a4021bba684ec211d086d77c260b34f90e7e5fb6e8fc5d13093d206e968f90bc1ba4d81be9ce628240f45d6b2304d4325e584ff26ffce6c750a8ed717023314394d85522536ecad24329a5accbc07f729b420e51ae2376b91332372ca31340978f92efb651519a5c1b1a51bbe36937cbcc03d275f0b47b24268367116e3e10abd1c3309aa7ce34948cf71e28532a5461e677178b8b502a872a9fd2ce9dcc32ade49e6eacc2fd0d45b5eed217d5dc4eaee1285f7b84273722c11b31e6e5883a2cfafe1ddd932416dd370154dad23cf2cbf19fc457127ac0f798dc4c897baab75e5bdd9b716cc75960b63c046be1ac5899491715399e02e764b5843470cbeecb09593c3fcc219174af4ae3676e42474086de66af619367f2f2d8c35debf4c05e30d977b927c859106e93881eabd412cdb1e9fe57a888a887baa68f1430eb8b024a2cff4e1261862564c41c691fb2a6f23698ac59b337049a9fa7f181aa0e3097da72004ad10cf102e94399b59230e8144be80b3c615a3181e8c5d3a04301000180a7ad67c40d615de0cd11824b672a8de6c641f3838ffff23d44b5bacc7c4c388d2a7f424b83ca04c468823634b0efc5f267a281d2e13bc9d30604a6688062aefd4d0b60230000000000000001199a003a0c07eb5fc7ff995a228aacd2ec3719819a9dec0aa26d10adc9c1a58d836093180ca76a3f8f67ac72668e909264852a2d1427ec85a02544219dc00181ec605ec8cbaa7af879501cd3a4e7398d78af3aaba9d5d6ea0504404ea1312b90c3628bea1d458c296594d773e3fa86b0ae1bd70845a8ac82e8c9386ff2da6d2739f74b221bcd68e3412a20d16bcc951869942da413f7a2cedb06c4fbcf3a89e5898314c35e6ddf3dc657f24f4c828f2c73604d14b312b2962e1b50494d294751f861c9343fcc2e3734545f25219cc48a099881d3bde6cfc64f54a21c934388a1c93f0a396b4cfd356a86e36e63dd9ea10196a10cdac69fc7f38a36fcf91521af60918d3083e57ee9413edec7f660af704e966b45a84cc14e0c56831ca27ef5082c291852f60a31305a36238daee77bb447e988319135e4fc2be7fa0db7b145874164f90589bff507f32f8c4ef27b4697283c955e84925552e8d61419ad08b2b40b55fb729b76034610f5606c930b6d4eb5578b27547554200e9185c27ec84169f2db8341783bbbaffc76b80e557974ae40de2acac657b8d1f03e49ed4ddf298083bcfdea14d01f7a31ea731dd2cfaf2a5a229847ac227d7c14ec1a68e2d748b63182515571c4e41dc69b95c6467316b0be33da252a32b27cf35e0d0a9a759cf9c7ee84489ed1254214c586d8ddb6a228e6498dc7379c49b344b160447c7e01cbfcbe92eab3eba4da9664b5db003486ddfca31864d16fe51851bb043f62723d3ef03452a263e4dd11da93ce9f7dc2db5ae16b64a2ce07d81d4a2bd35d62abbe05ec7b2af6327c99a04c0b9316942fa74878bd91b96bc1304e6f2be7e50ccf7bc866ad37fb34effe83700d3ea93e220d5251dfad1af156b84f4fa9d97dcc62f140c26f8369716d9ceaa5ffc69a9ee647fbdc648da9eabbd6e36a271a00840ce75e9addee57074b866429b98547c3c380b9defef2b65742ed5fa4b4acaa59324ed8cc491e51f4c15a34a3b712c91acb9c5cad43be2e481206f5e8be006eb6d632a31df1bc2bce4e267ae48c75c10d7ad7d54b4a3579bdc8c27cee6f7067a63acba681a34eeaeebee359d82ba0bea46baecdc40c641f9995ca3f7daa9c5679ed160fad6b3755466cd463d3e7a117ba3c311ba7aba451b288a02c3b0c462f21dbc0dfd1cd805d40bcf85d78bfa7a1a689edb599f5d54956d3a11d5f3f2b0b0cb72851605cb7e90f9401e9be9f6f1014a43502dec2291f3b583c99ad4192ee5c3bb01024aebed7d3276cf61bf5bbc495174c6bd8a9ab37a166e9b48da9216d7f476199566827a329ec3ed48892a4b19d7c2be4aa7f0bd1843aea86869615df5ad8cb327874ab9d297270140cf519994c425c4d08700360dd3427e7be91521cfd671f844e28d3e1298c1b81be596e2aafc42e727697b30c981eb70a104d8277cdedf55dccd4ceb95657ddf30d9990ba1d2c67b6ae863c7dbb7d1898cf90181f3375bc7c7ca42fbd6a51d30ea19331fd9fd93a0b68d985505296c89e0d2f38871546c9d6805459f9c26e8f503673823d34a03ba63090c499ad21a1629197f772dea62f4989e8ebde6e18cedfcba7ab3956df479d59a2e19d86b1a3c0ed8fc298871b270a720b6853f6609bc33d095ce2d506b7b32c4f63ea30b4b484c29c616ff6a800aaa080f1374c5f72cb6e186e56b6ecd9de8bbdc79a4282182867eaf41e1e789caadb4a01dd9eafadab1035396434112db932e3d4a9a7c2d5ab635e2917c1ee3242bc98e6b36499a588b90abd7224619e9421c53f710f3bad74db88305c1af4d4bb97438d49a8d46257d7adc3290e96bb05029f78f1cc54cabc21da94c768400a0a7ddcb147df8dc2b6353d261b48a47eccca5ffaff80e9fb5ecea1efa940806777c16c0ccebf5d5c9c8d3d64afc1ceb72aad306390ee4e1ce418317d229547cf7d96fa6c3e72624d138438ac3df68b6cdf4bb54812cd31dac2e5ab2d4090ca8a01de888199b205ab31933227646c8b6e8faddf08dd94438cc0fdf5b4c8ef7c48123b8c9d16a09162895c75469447ee4630a5c52f716ceb3fc73653f56fc90753dcde00c7e1b4e46a9a1f40674fd130dea42f935d9e811261057871e4367ec42b9b6127f3687e10e6777b3e4d537695d01a5053c14a70b6435c624cdcc93c4b9fc68726590d64d8ed3ac9b74609851f868e03568ced113968babeef5d4eb95b33d0c0d196d55f58bb394ed9109c89f3f8317cfdbac738aaeed72afcf146dd5e3f2555e77f0d959263961c55989b01dc47172103c1a27050ffd5272ef700aa1cf24ef2e2c1c640251b64567a55ba813389e5b851764ab6966fcd08f2e33ef77eadaf83d7c734dd849e5ff0e9a18b5739d7322600b0d8b459cefb9eb424a481043e0bd67af17fe15de1269f3f96173925ef0dfef6b39fccf35d9a961f682dd9d976e735c6fc7139e7c398e0af4488a0760766e5dca6d1a3d8641d872ee1614d55d9a31929207adc0e813594f2571eac8160f882ee9ccefdeeb3cb98a2b885871ce17a4262b4c0bc53b21491d86866e5bcce058aa978bd84b0a21ff4e03a6547054e8f41b5b24926ee808c50f6f857bf09e070612b9816cd79eb18cc7ce58f84404d031fd7f8d9433880ec5a40090625e6580337ff84b10de7331a9cf93aa81768fccda2efaa8a9617c788540726662194d17be8cbb7e5799a7766294fa11fcfa34955f5ffefeca4356a58b255104b5a84822789dbab7f99393410eb356dd2694b5c5068566c572c11ca4aac9820461560488691bb11ee2dffbe5b87118ed017d3e4a1fe9c4f1c9bd18b70ccac6691ca104b90d376d6763537ba767caef629d3c940b5f96857d7bd3e1927193e4daba0b6c0c99ed89e2a177e3cc80cd37bfb9613ca9a4aea47eb311353aae84e5fd231531428635178a05e7c59251305c8a0c9ea66d8ad73ea7288379e49ce1f6afd59c13163f00720105810681fe57ca8980f0945c4d8d490e2fd70141485ade4b2f1c9434b7d2593490810b5018c06cdf8729c17cbc17f44a2bbe242e4e6a905e53910139375dfe05f48baa0d5f13ea1830c85c5188206ff68c3578b860fd201b8aa8ef87cbbfc2784d2f0db470bcfe9b693cf2286d7c1834746159e7710e00878ba814ace639765b08fc36a0862b80a06445a05b85e71fa5132e8697a73085afc1ea11a91d8373be39ea60b0e7d6a7cd66872eb91a3dd24aeedaac82f9f3c459f0020dc636b42b55f5b379b50bbca6a6fe80aaa7c8cd629cd735675787f82905eb5139e79337ab0da46a0f56ad6abcf7af9cb0b7f5a9675aae6e9bb11918fbb2f5a9c7efefb8b610ba55b748b4e5b5f58b67a188fbee42e3ddd57a34bf696e9720081e4f510c6b4e928984ac75525bf3c7b22c2dd21e4a2a292e1497b69aa7843fd23f00aee321abfd59a821126e2df88e1bda8a4d6d2dfca3702cb24933b0692a3430e5839663da12b4223ec334fa7be72640c1e7554ad58b08962059cd29270021daef264b0269a24b79b18f041fb8ffc78bf491c9f2665fae997f2373bc4616cf269cdadedd061ed983c076130897e4a43b0db1214a83c18e5be77642fad84e66cc0dfcff4cbc22d025c9c23ad1472a32b77f0b7c3fb9bc85e23ec498baf3cafb14b341699ae65e257835e1b8ceffed3f958077e0678d959e03c4cffdf5baef730b5697f042d1e7d869e3b34c318c2928f56a78bbdb5fcf9ec0db2ad0ebab630a0b010114bea72c5017667021a311fab8a7ac1fee3587624b2561e447ef9f0bdac31e2e923fd5b2affb3d2efc9ce16bd2930682caaeea2b3197b142cab7767c1a043fc3e39ddfcdff3f520d89efd43d06bfbd95055df25bb55a0138bd187ba99cdc4c469f1e4d8da05f68117cbe6c0f56be0b3b2605c4185adf48df8b113210e2752070a1c2409eeda7764c4c0bb66b6c3ef6e1e02de7c13f19ff730edd70fdca25d76c77c839d19665156b6c8a3dee400f68abe17a270fee5207ae82137719936ce29e9351f7cedf02b14a6033d228b2edb522104eca5c27a7567bba3b8ca80b3f1aa8cb9e9e5464a7a73bf09606ecc6d2c9390cdb20000";
  static const char* const kSignature = "070c406795aa6723aa843de99d34f368670cc55325363a6141554adefff36a1a3062962f66a50db9e4a008b18239c35115943d30b848912e49d82aa59ee4fa66";
  static const char* const kPubkey = "036ac6cd5aafa916d80cc2284ad52f637de503444ae1863f985af4ec2297e29e35";
  // static const char* const kPrivkey = "cQUCRSC3QbWjjbRqmCX2EQ8TfueLajg9wk3zVpMPLUwAt8u8gark";
  // 1-of-2 multisig
  static const char* const kScript = "5121036ac6cd5aafa916d80cc2284ad52f637de503444ae1863f985af4ec2297e29e35210229ebd1cac7855ca60b0846bd179ff3d411f807f3f3a43abf498e0a415c94d62252ae";
  static const char* const kTxid = "fc6cd0e025c7dc866ebe2b4b3f3449f213a17a62ed6438c3f6894ecdbf0bc117";
  static const uint32_t kVout = 1;

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    bool result = false;
    // int64_t satoshi = 1000000000;
    const char* value_commitment = "0993c069270bf8d090ce8695b82e52fb2959a9765d987d4ffd7a767b0c5b1c4cbc";
    ret = CfdConfidentialTxVerifySignature(handle, kTxHex, kSignature, kPubkey,
        kScript, kTxid, kVout, CfdSighashType::kCfdSigHashAll, false, 0,
        value_commitment, CfdWitnessVersion::kCfdWitnessVersion0, &result);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_TRUE(result);
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
