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
  uint32_t index = 0;

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

    ret = CfdAddConfidentialTxIn(
      handle, tx_string, "1497e1f146bc5fe00b6268ea16a7069ecb90a2a41a183446d5df8965d2356dc1", 2,
      4294967295U, &tx_string2);
    EXPECT_EQ(kCfdSuccess, ret);

    CfdFreeStringBuffer(tx_string);
    tx_string = tx_string2;
    tx_string2 = nullptr;
  }

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000003bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140200000000ffffffff0000000000", tx_string);

    index = 0;
    ret = CfdGetConfidentialTxInIndex(
      handle, tx_string, "1497e1f146bc5fe00b6268ea16a7069ecb90a2a41a183446d5df8965d2356dc1", 1,
      &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);
  }

  if (ret == kCfdSuccess) {
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
    EXPECT_STREQ("020000000003bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140200000000ffffffff010151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000005f5e10003a630456ab6d50b57981e085abced70e2816289ae2b49a44c2f471b205134c12b1976a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac00000000", tx_string);

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
    EXPECT_STREQ("020000000003bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140200000000ffffffff020151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000005f5e10003a630456ab6d50b57981e085abced70e2816289ae2b49a44c2f471b205134c12b1976a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f010000000071475420001976a914fdd725970db682de970e7669646ed7afb8348ea188ac00000000", tx_string);

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
    ret = CfdGetConfidentialTxOutIndex(
      handle, tx_string, "CTEw7oSCUWDfmfhCEdsB3gsG7D9b4xLCZEq71H8JxRFeBu7yQN3CbSF6qT6J4F7qji4bq1jVSdVcqvRJ", NULL,
      &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxOutIndex(
      handle, tx_string, "", "76a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac", &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxOutIndex(
      handle, tx_string, "2dxZw5iVZ6Pmqoc5Vn8gkUWDGB5dXuMBCmM", NULL,
      &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxOutIndex(
      handle, tx_string, "", "76a914fdd725970db682de970e7669646ed7afb8348ea188ac", &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxOutIndex(
      handle, tx_string, NULL, NULL, &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, index);
  }

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000003bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140200000000ffffffff030151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000005f5e10003a630456ab6d50b57981e085abced70e2816289ae2b49a44c2f471b205134c12b1976a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f010000000071475420001976a914fdd725970db682de970e7669646ed7afb8348ea188ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f01000000000007a120000000000000", tx_string);
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

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxInIndex(
      handle, tx_string, "1497e1f146bc5fe00b6268ea16a7069ecb90a2a41a183446d5df8965d2356dc1", 3,
      &index);
    EXPECT_EQ(kCfdIllegalArgumentError, ret);
    if (ret == kCfdIllegalArgumentError) {
      ret = kCfdSuccess;
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxOutIndex(
      handle, tx_string, NULL, "76a914fdd825970db682de970e7669646ed7afb8348ea188ac", &index);
    EXPECT_EQ(kCfdIllegalArgumentError, ret);
    if (ret == kCfdIllegalArgumentError) {
      ret = kCfdSuccess;
    }
  }

  ret = CfdGetLastErrorCode(handle);
  if (ret != kCfdSuccess) {
    char* str_buffer = NULL;
    ret = CfdGetLastErrorMessage(handle, &str_buffer);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("Failed to parameter. search target is not found.", str_buffer);
    CfdFreeStringBuffer(str_buffer);
    str_buffer = NULL;
  }

  if (tx_string != nullptr) {
    CfdFreeStringBuffer(tx_string);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_elements_transaction, CreateRawTransactionRefine) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;
  int64_t satoshi;
  uint32_t index = 0;

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(handle, kCfdNetworkLiquidv1, 2, 0, nullptr, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));
  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(
      handle, create_handle, "7461b02405414d79e79a5050684a333c922c1136f4bdff5fb94b551394edebbd", 0,
      4294967295U);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(
      handle, create_handle, "1497e1f146bc5fe00b6268ea16a7069ecb90a2a41a183446d5df8965d2356dc1", 1,
      4294967295U);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(
      handle, create_handle, "1497e1f146bc5fe00b6268ea16a7069ecb90a2a41a183446d5df8965d2356dc1", 2,
      4294967295U);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 100000000;
    ret = CfdAddTransactionOutput(
      handle, create_handle, satoshi,
      "CTEw7oSCUWDfmfhCEdsB3gsG7D9b4xLCZEq71H8JxRFeBu7yQN3CbSF6qT6J4F7qji4bq1jVSdVcqvRJ", "",
      "ef47c42d34de1b06a02212e8061323f50d5f02ceed202f1cb375932aa299f751");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 1900000000;
    ret = CfdAddTransactionOutput(
      handle, create_handle, satoshi,
      "2dxZw5iVZ6Pmqoc5Vn8gkUWDGB5dXuMBCmM", "",
      "6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 500000;
    ret = CfdAddConfidentialTxOutput(
      handle, create_handle, satoshi, "", "6a",
      "6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3", "");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 500000;
    ret = CfdAddTransactionOutput(
      handle, create_handle, satoshi,
      nullptr, nullptr,
      "6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeTransaction(handle, create_handle, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  CfdFreeTransactionHandle(handle, create_handle);

  if (ret == kCfdSuccess) {
    EXPECT_STREQ("020000000003bdebed9413554bb95fffbdf436112c923c334a6850509ae7794d410524b061740000000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140100000000ffffffffc16d35d26589dfd54634181aa4a290cb9e06a716ea68620be05fbc46f1e197140200000000ffffffff040151f799a22a9375b31c2f20edce025f0df5231306e81222a0061bde342dc447ef010000000005f5e10003a630456ab6d50b57981e085abced70e2816289ae2b49a44c2f471b205134c12b1976a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f0100000000713fb300001976a914fdd725970db682de970e7669646ed7afb8348ea188ac01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f01000000000007a12000016a01f38611eb688e6fcd06f25e2faf52b9f98364dc14c379ab085f1b57d56b4b1a6f01000000000007a120000000000000", tx_string);
  }

  if (ret == kCfdSuccess) {
    index = 0;
    ret = CfdGetTxInIndex(
      handle, kCfdNetworkLiquidv1, tx_string, "1497e1f146bc5fe00b6268ea16a7069ecb90a2a41a183446d5df8965d2356dc1", 1,
      &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetTxOutIndex(
      handle, kCfdNetworkLiquidv1, tx_string, "CTEw7oSCUWDfmfhCEdsB3gsG7D9b4xLCZEq71H8JxRFeBu7yQN3CbSF6qT6J4F7qji4bq1jVSdVcqvRJ", NULL,
      &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetTxOutIndex(
      handle, kCfdNetworkLiquidv1, tx_string, "", "76a914d08f5ba8874d36cf97d19379b370f1f23ba36d5888ac", &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetTxOutIndex(
      handle, kCfdNetworkLiquidv1, tx_string, "2dxZw5iVZ6Pmqoc5Vn8gkUWDGB5dXuMBCmM", NULL,
      &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetTxOutIndex(
      handle, kCfdNetworkLiquidv1, tx_string, "", "76a914fdd725970db682de970e7669646ed7afb8348ea188ac", &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetTxOutIndex(
      handle, kCfdNetworkLiquidv1, tx_string, NULL, NULL, &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(3, index);
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

  if (tx_string != nullptr) {
    CfdFreeStringBuffer(tx_string);
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

TEST(cfdcapi_elements_transaction, GetTransactionDataByHandle) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static const char* tx_data = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";

  uint32_t txin_count = 0;
  uint32_t txout_count = 0;

  void* tx_handle = NULL;
  ret = CfdInitializeTxDataHandle(handle, kCfdNetworkElementsRegtest, tx_data, &tx_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((tx_handle == handle));
  if (ret == kCfdSuccess) {
    ret = CfdGetTxInCountByHandle(handle, tx_handle, &txin_count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, txin_count);

    ret = CfdGetTxOutCountByHandle(handle, tx_handle, &txout_count);
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
      ret = CfdGetConfidentialTxInfoByHandle(
          handle, tx_handle, &txid, &wtxid, &wit_hash, &size, &vsize, &weight,
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
      ret = CfdGetTxInByHandle(
          handle, tx_handle, 1, &txid, &vout, &sequence, &script_sig);
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
      ret = CfdGetTxInIssuanceInfoByHandle(
          handle, tx_handle, 1, &entropy, &nonce, &asset_amount, &asset_value,
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
      ret = CfdGetConfidentialTxOutSimpleByHandle(
          handle, tx_handle, 3, &asset_string, &value_satoshi, &value_commitment,
          &nonce, &locking_script);
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

    ret = CfdFreeTxDataHandle(handle, tx_handle);
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

TEST(cfdcapi_elements_transaction, GetPeginWitnessData) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static const char* tx_data = "0200000001017926299350fdc2f4d0da1d4f0fbbd3789d29f9dc016358ae42463c0cebf393f30000004000ffffffff020125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a0100000002540ba97c0017a91414b71442e11941fd7807a82eabee13d6ec171ed9870125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000000000003a84000000000000000000060800e40b54020000002025b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a2006226e46111a0b59caaf126043eb5bbf28c34f3a5e332a1fc7b2b73cf188910f16001412dcdeef890f60967896391c95b0e02c9258dfe5fdda060200000000010a945efd42ce42de413aa7398a95c35facc14ec5d35bb23e5f980014e94ab96a620000000017160014ca2041536307bbe086e8c7fe8563e1c9b9b6eb84feffffffe50b46ecadb5cc52a7ef149a23323464353415f02d7b4a943963b26a9beb2a030000000017160014ca2041536307bbe086e8c7fe8563e1c9b9b6eb84feffffff67173609ca4c13662356a2507c71e5d497baeff56a3c42af989f3b270bc870560000000017160014ca2041536307bbe086e8c7fe8563e1c9b9b6eb84feffffff784a9fd151fe2808949fae18afcf52244a77702b9a83950bc7ec52a8239104850000000017160014ca2041536307bbe086e8c7fe8563e1c9b9b6eb84feffffff259618278cecbae1bed8b7806133d14987c3c6118d2744707f509c58ea2c0e870000000017160014ca2041536307bbe086e8c7fe8563e1c9b9b6eb84feffffff5c30c2fdcb6ce0b666120777ec18ce5211dd4741f40f033648432694b0919da50000000017160014a8a7c0032d1d283e39889861b3f05156e379cfb6feffffffbb0f857d4b143c74c7fdb678bf41b65e7e3f2e7644b3613ae6370d21c0882ad60000000017160014a8a7c0032d1d283e39889861b3f05156e379cfb6feffffffbce488c283e07bf364edb5057e020aa3d137d8d6130711dc12f03f35564945680000000017160014ca2041536307bbe086e8c7fe8563e1c9b9b6eb84feffffff258cb927989780ac92a3952ffd1f54e9b65e59fb07219eb106840b5d76b547fb0000000017160014ca2041536307bbe086e8c7fe8563e1c9b9b6eb84feffffffe98ec686efbca9bdd18ae85a3a8235a607e1cfb6138bac1461d400cbbabbe00f0000000017160014a8a7c0032d1d283e39889861b3f05156e379cfb6feffffff0100e40b540200000017a91472c44f957fc011d97e3406667dca5b1c930c4026870247304402206b4de54956e864dfe3ff3a4957e329cf171e919498bb8f98c242bef7b0d5e3350220505355401a500aabf193b93492d6bceb93c3b183034f252d65a139245c7486a601210281465587e09d80f5a7b8ce94bab4a4571dc8cff4483cc9eb89e76ecfa650b6f40247304402200fc48c7b5bd6de74c951250c60e8e6c9d3a605dc557bdc93ce86e85d2f27834a02205d2a8768adad669683416d1126c8537ab1eb36b0e83d5d9e6a583297b7f9d2cb01210281465587e09d80f5a7b8ce94bab4a4571dc8cff4483cc9eb89e76ecfa650b6f40247304402207ad97500fbe6049d559a1e10586cd0b1f02baeb98dc641a971a506a57288aa0002202a6646bc4262904f6d1a9288c12ff586b5a674f5a351dfaba2698c8b8265366f01210281465587e09d80f5a7b8ce94bab4a4571dc8cff4483cc9eb89e76ecfa650b6f4024730440220271e41a1e8f953b6817333e43d6a5e2924b291d52120011a5f7f1fb8049ae41b02200f1a25ed9da813122caadf8edf8d01da190f9407c2b61c27d4b671e07136bce701210281465587e09d80f5a7b8ce94bab4a4571dc8cff4483cc9eb89e76ecfa650b6f402473044022050291184dcd4733de6e6a43d9efb1e21e7d2c563e9138481f04010f3acbb139f02206c01c3bfe4e5b71c4aac524a18f35e25ae7306ca110b3c3b079ae6da2b0a0a5701210281465587e09d80f5a7b8ce94bab4a4571dc8cff4483cc9eb89e76ecfa650b6f402473044022045a188c10aec4f1a3a6c8a3a3c9f7d4dc63b9eacc011839c907d1c5da206a1390220399ca60516204efd9d220eaa0c804867137133c4d70780223fdde699288af3790121031c01fd031bc09b385d138b3b2f44ec04c03934b66f6485f37a17b4899f1b8d7802473044022053621a5c74b313c648d179041c154152372060941d9c9080340eb913358b705602201ac178f639360356ca7d75656d92bd7801d976e74bd5d2e30d6310a94940d0bc0121031c01fd031bc09b385d138b3b2f44ec04c03934b66f6485f37a17b4899f1b8d780247304402207b4a7a271a8fc03e8045ca367cb64046fa06e5b13a105e67efe7dd6571503fcb022072852e1c3f87eeac039601a0df855fb5d65bbdcd3ad95ff96bfc7b534fd89f7601210281465587e09d80f5a7b8ce94bab4a4571dc8cff4483cc9eb89e76ecfa650b6f402473044022037e9f0943a79e155a57526e251cfd39e004552b76c0de892448eb939d2d12fdf02203a02f0045e8f90739eddc06c026c95b4a653aeb89528d851ab75952fd7db07b801210281465587e09d80f5a7b8ce94bab4a4571dc8cff4483cc9eb89e76ecfa650b6f402473044022057a9953ba83d5e710fc64e1c533d81b0913f434b3e1c865cebd6cb106e09fa77022012930afe63ae7f1115a2f3b13039e71387fc2d4ed0e36eaa7be55a754c8c84830121031c01fd031bc09b385d138b3b2f44ec04c03934b66f6485f37a17b4899f1b8d78130e00009700000020fe3b574c1ce6d5cb68fc518e86f7976e599fafc0a2e5754aace7ca16d97a7c78ef9325b8d4f0a4921e060fc5e71435f46a18fa339688142cd4b028c8488c9f8dd1495b5dffff7f200200000002000000024a180a6822abffc3b1080c49016899c6dac25083936df14af12f58db11958ef27926299350fdc2f4d0da1d4f0fbbd3789d29f9dc016358ae42463c0cebf393f3010500000000";

  uint32_t pegin_count = 0;
  char* stack_data = nullptr;

  ret = CfdGetConfidentialTxInPeginWitnessCount(handle, tx_data, 0, &pegin_count);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(6, pegin_count);

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxInPeginWitness(
        handle, tx_data, 0, 0, &stack_data);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("00e40b5402000000", stack_data);
      CfdFreeStringBuffer(stack_data);
      stack_data = nullptr;
    }
  }
  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxInPeginWitness(
        handle, tx_data, 0, 0, &stack_data);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("00e40b5402000000", stack_data);
      CfdFreeStringBuffer(stack_data);
      stack_data = nullptr;
    }
  }
  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxInPeginWitness(
        handle, tx_data, 0, 2, &stack_data);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("06226e46111a0b59caaf126043eb5bbf28c34f3a5e332a1fc7b2b73cf188910f", stack_data);
      CfdFreeStringBuffer(stack_data);
      stack_data = nullptr;
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetConfidentialTxInPeginWitness(
        handle, tx_data, 0, 3, &stack_data);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("001412dcdeef890f60967896391c95b0e02c9258dfe5", stack_data);
      CfdFreeStringBuffer(stack_data);
      stack_data = nullptr;
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
    ret = CfdAddBlindTxOutByAddress(
        handle, blind_handle,
        "CTExCoUri8VzkxbbhqzgsruWJ5zYtmoFXxCWtjiSLAzcMbpEWhHmDrZ66bAb41VsmSKnvJWrq2cfjUw9");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdSetBlindTxOption(
        handle, blind_handle, kCfdBlindOptionMinimumRangeValue, 1);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdSetBlindTxOption(
        handle, blind_handle, kCfdBlindOptionExponent, 0);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdSetBlindTxOption(
        handle, blind_handle, kCfdBlindOptionMinimumBits, 52);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdSetBlindTxOption(
        handle, blind_handle, kCfdBlindOptionCollectBlinder, 1);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeBlindTx(handle, blind_handle, base_tx, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  uint32_t vout = 0;
  int64_t value = 0;
  char* asset = nullptr;
  char* asset_blind_factor = nullptr;
  char* value_blind_factor = nullptr;
  char* issuance_txid = nullptr;
  uint32_t issuance_vout = 0;
  bool is_issuance_asset = false;
  bool is_issuance_token = false;
  const char* empty_factor = "0000000000000000000000000000000000000000000000000000000000000000";
  if (ret == kCfdSuccess) {
    // blind is contains random value.
    // EXPECT_STREQ("", tx_string);
    ret = CfdGetBlindTxBlindData(handle, blind_handle, 0,
        &vout, &asset, &value, &asset_blind_factor,
        &value_blind_factor, &issuance_txid, &issuance_vout,
        &is_issuance_asset, &is_issuance_token);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      // reissuance: 600000000
      EXPECT_EQ(1, vout);
      EXPECT_STREQ("accb7354c07974e00b32e4e5eef55078490141675592ac3610e6101831edb0cd", asset);
      EXPECT_EQ(600000000, value);
      EXPECT_STREQ(empty_factor, asset_blind_factor);
      EXPECT_STRNE(empty_factor, value_blind_factor);
      EXPECT_STREQ("57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f",
          issuance_txid);
      EXPECT_EQ(1, issuance_vout);
      EXPECT_TRUE(is_issuance_asset);
      EXPECT_FALSE(is_issuance_token);
      CfdFreeStringBuffer(asset);
      CfdFreeStringBuffer(asset_blind_factor);
      CfdFreeStringBuffer(value_blind_factor);
      CfdFreeStringBuffer(issuance_txid);
      asset = nullptr;
      asset_blind_factor = nullptr;
      value_blind_factor = nullptr;
      issuance_txid = nullptr;
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetBlindTxBlindData(handle, blind_handle, 2,
        &vout, &asset, &value, &asset_blind_factor,
        &value_blind_factor, &issuance_txid, &issuance_vout,
        &is_issuance_asset, &is_issuance_token);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      // txout[1]: 700000000
      EXPECT_EQ(1, vout);
      EXPECT_STREQ("ed6927df918c89b5e3d8b5062acab2c749a3291bb7451d4267c7daaf1b52ad0b", asset);
      EXPECT_EQ(700000000, value);
      EXPECT_STRNE(empty_factor, asset_blind_factor);
      EXPECT_STRNE(empty_factor, value_blind_factor);
      EXPECT_STREQ("", issuance_txid);
      EXPECT_EQ(0, issuance_vout);
      EXPECT_FALSE(is_issuance_asset);
      EXPECT_FALSE(is_issuance_token);
      CfdFreeStringBuffer(asset);
      CfdFreeStringBuffer(asset_blind_factor);
      CfdFreeStringBuffer(value_blind_factor);
      CfdFreeStringBuffer(issuance_txid);
      asset = nullptr;
      asset_blind_factor = nullptr;
      value_blind_factor = nullptr;
      issuance_txid = nullptr;
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdGetBlindTxBlindData(handle, blind_handle, 4,
        &vout, &asset, &value, &asset_blind_factor,
        &value_blind_factor, &issuance_txid, &issuance_vout,
        &is_issuance_asset, &is_issuance_token);
    EXPECT_EQ(kCfdOutOfRangeError, ret);
    if (ret == kCfdOutOfRangeError) ret = kCfdSuccess;
  }

  if (blind_handle != nullptr) {
    CfdFreeBlindHandle(handle, blind_handle);
  }

  // unblind test
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

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}


TEST(cfdcapi_elements_transaction, BlindDestroyAmountTransaction) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static const char* base_tx = "0200000000020fd1d25bb34f2fb499304dec92571f88f553891aceda376533fb8d76fc64b0ea0000000000ffffffffd2b787c00072aa8a46bf77d04017cfc73665fc2a3945af0ac43ef2ed2eca1e4a0100000000ffffffff0301c7bd3d0980d0c7932a215d0444536159ff4d238f9c852659813df784505809d20100000000000f424000016a0125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000000000002710000001c7bd3d0980d0c7932a215d0444536159ff4d238f9c852659813df784505809d201000000000000000003edd71a5d0cd1e82e48ec3e26bfbfecb053b2a196e3ce56752312cc81e7eaca72016a00000000";

  char* tx_string = nullptr;
  int64_t satoshi;
  void* blind_handle = nullptr;
  ret = CfdInitializeBlindTx(handle, &blind_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    satoshi = 1000000;
    ret = CfdAddBlindTxInData(
        handle, blind_handle,
        "eab064fc768dfb336537dace1a8953f5881f5792ec4d3099b42f4fb35bd2d10f", 0,
        "d209585084f73d815926859c8f234dff59615344045d212a93c7d080093dbdc7",
        "44c2852117c44c7a43833d5a681f09ec6120d889bb70891627d30a122ff24047",
        "3a2916b1ddd3514b32c533116e02b1262fc9df455df55d82a30197f4a81e9580",
        satoshi, nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 10000;
    ret = CfdAddBlindTxInData(
        handle, blind_handle,
        "4a1eca2eedf23ec40aaf45392afc6536c7cf1740d077bf468aaa7200c087b7d2", 1,
        "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225",
        "7afdad5199ae3427783bca4e392ece8525431ec3345d08e231de7fa6bd4938ab",
        "32633edc9ed7acc9407935db0820921abdc446a7c0c0203d279740fc0b26425b",
        satoshi, nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeBlindTx(handle, blind_handle, base_tx, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STRNE(base_tx, tx_string);
  }

  if (blind_handle != nullptr) {
    CfdFreeBlindHandle(handle, blind_handle);
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

TEST(cfdcapi_elements_transaction, AddSignConfidentialTx2) {
  static const char* const kTxData = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int64_t satoshi = 13000000000000;
  char* sighash = nullptr;
  char* tx_string = nullptr;
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
      ret = CfdAddPubkeyHashSign(
        handle, kCfdNetworkElementsRegtest, kTxData,
        "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0, kCfdP2wpkh,
        "03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6", signature,
        true, kCfdSigHashAll, false, &tx_string);
      EXPECT_EQ(kCfdSuccess, ret);
      CfdFreeStringBuffer(signature);
    }

    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0000000000000247304402200268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c20220131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea012103f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d600000000000000000000000000", tx_string);

      uint32_t count = 0;
      ret = CfdGetConfidentialTxInWitnessCount(handle, tx_string, 0, &count);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_EQ(2, count);

      char* stack = nullptr;
      ret = CfdGetConfidentialTxInWitness(handle, tx_string, 0, 1, &stack);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_STREQ("03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6", stack);
      if (ret == kCfdSuccess) {
        CfdFreeStringBuffer(stack);
      }
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

TEST(cfdcapi_elements_transaction, CfdAddConfidentialTxSignWithPrivkeySimple) {
  static const char* const kTxData = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int64_t satoshi = 13000000000000;
  char* tx_string = nullptr;

  ret = CfdAddConfidentialTxSignWithPrivkeySimple(
      handle, kTxData, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
      kCfdP2wpkh, "03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6",
      "cU4KjNUT7GjHm7CkjRjG46SzLrXHXoH3ekXmqa2jTCFPMkQ64sw1",
      satoshi, nullptr, kCfdSigHashAll, false, true, &tx_string);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0000000000000247304402200268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c20220131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea012103f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d600000000000000000000000000", tx_string);
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

TEST(cfdcapi_elements_transaction, AddSignConfidentialTxEmptyWitness) {
  static const char* const kTxData = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";
  // static const char* pubkey1 = "02715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad";
  static const char* privkey1 = "cRVLMWHogUo51WECRykTbeLNbm5c57iEpSegjdxco3oef6o5dbFi";
  // static const char* pubkey2 = "02bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d71";
  static const char* privkey2 = "cQUTZ8VbWNYBEtrB7xwe41kqiKMQPRZshTvBHmkoJGaUfmS5pxzR";
  static const char* multisig_script = "522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52ae";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;
  char* tx_string2 = nullptr;
  ret = CfdAddConfidentialTxSign(
      handle, kTxData, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
      true, "", true, &tx_string);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    char* sighash = nullptr;
    int64_t satoshi = 13000000000000;
    char* signature = nullptr;
    ret = CfdCreateConfidentialSighash(
        handle, tx_string, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        kCfdP2wsh, nullptr, multisig_script, satoshi, nullptr, kCfdSigHashAll, false, &sighash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("d17f091203341a0d1f0101c6d010a40ce0f3cef8a09b2b605b77bb6cfc23359f", sighash);

      ret = CfdCalculateEcSignature(
        handle, sighash, nullptr, privkey2, kCfdNetworkRegtest, true, &signature);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdAddConfidentialTxDerSign(
          handle, tx_string, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
          true, signature, kCfdSigHashAll, false, false, &tx_string2);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }

    if (ret == kCfdSuccess) {
      CfdFreeStringBuffer(tx_string);
      tx_string = nullptr;

      ret = CfdCalculateEcSignature(
        handle, sighash, nullptr, privkey1, kCfdNetworkRegtest, true, &signature);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdAddConfidentialTxDerSign(
          handle, tx_string2, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f",
           0, true, signature, kCfdSigHashAll, false, false, &tx_string);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }

    if (ret == kCfdSuccess) {
      CfdFreeStringBuffer(tx_string2);
      tx_string2 = nullptr;

      ret = CfdAddConfidentialTxSign(
        handle, tx_string, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        true, multisig_script, false, &tx_string2);
      EXPECT_EQ(kCfdSuccess, ret);
    }
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000000004004730440220795dbf165d3197fe27e2b73d57cacfb8d742029c972b109040c7785aee4e75ea022065f7a985efe82eba1d0e0cafd7cf711bb8c65485bddc4e495315dd92bd7e4a790147304402202ce4acde192e4109832d46970b510158d42fc156c92afff137157ebfc2a03e2a02200b7dfd3a92770d79d29b3c55fb6325b22bce0e1362de74b2dac80d9689b5a89b0147522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52ae00000000000000000000000000", tx_string2);
    }
    CfdFreeStringBuffer(sighash);

    CfdFreeStringBuffer(tx_string);
    CfdFreeStringBuffer(tx_string2);
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

TEST(cfdcapi_elements_transaction, AddSignConfidentialTxEmptyWitnessSimple) {
  static const char* const kTxData = "0200000000020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000";
  // static const char* pubkey1 = "02715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad";
  static const char* privkey1 = "cRVLMWHogUo51WECRykTbeLNbm5c57iEpSegjdxco3oef6o5dbFi";
  // static const char* pubkey2 = "02bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d71";
  static const char* privkey2 = "cQUTZ8VbWNYBEtrB7xwe41kqiKMQPRZshTvBHmkoJGaUfmS5pxzR";
  static const char* multisig_script = "522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52ae";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;
  char* tx_string2 = nullptr;
  ret = CfdAddTxSign(
    handle, kCfdNetworkElementsRegtest, kTxData,
    "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
    kCfdP2shP2wsh, "", false, 0, false, true, &tx_string);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    char* sighash = nullptr;
    int64_t satoshi = 13000000000000;
    char* signature = nullptr;
    ret = CfdCreateConfidentialSighash(
        handle, tx_string, "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        kCfdP2shP2wsh, nullptr, multisig_script, satoshi, nullptr, kCfdSigHashAll, false, &sighash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("d17f091203341a0d1f0101c6d010a40ce0f3cef8a09b2b605b77bb6cfc23359f", sighash);

      ret = CfdCalculateEcSignature(
        handle, sighash, nullptr, privkey2, kCfdNetworkRegtest, true, &signature);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdAddTxSign(
          handle, kCfdNetworkElementsRegtest, tx_string,
          "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
          kCfdP2shP2wsh, signature, true, kCfdSigHashAll, false, false, &tx_string2);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }

    if (ret == kCfdSuccess) {
      CfdFreeStringBuffer(tx_string);
      tx_string = nullptr;

      ret = CfdCalculateEcSignature(
        handle, sighash, nullptr, privkey1, kCfdNetworkRegtest, true, &signature);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdAddTxSign(
          handle, kCfdNetworkElementsRegtest, tx_string2,
          "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
          kCfdP2shP2wsh, signature, true, kCfdSigHashAll, false, false, &tx_string);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }

    if (ret == kCfdSuccess) {
      CfdFreeStringBuffer(tx_string2);
      tx_string2 = nullptr;

      ret = CfdAddScriptHashSign(
        handle, kCfdNetworkElementsRegtest, tx_string,
        "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
        kCfdP2shP2wsh, multisig_script, false, &tx_string2);
      EXPECT_EQ(kCfdSuccess, ret);
    }
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a15700000000232200206e67741009d105674a17f86e73433030eea0603c020fb25ea08c6fadd6c2f62dffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000000004004730440220795dbf165d3197fe27e2b73d57cacfb8d742029c972b109040c7785aee4e75ea022065f7a985efe82eba1d0e0cafd7cf711bb8c65485bddc4e495315dd92bd7e4a790147304402202ce4acde192e4109832d46970b510158d42fc156c92afff137157ebfc2a03e2a02200b7dfd3a92770d79d29b3c55fb6325b22bce0e1362de74b2dac80d9689b5a89b0147522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52ae00000000000000000000000000", tx_string2);
    }
    CfdFreeStringBuffer(sighash);

    CfdFreeStringBuffer(tx_string);
    CfdFreeStringBuffer(tx_string2);
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

TEST(cfdcapi_elements_transaction, AddMultisigSignConfidentialTxSimple) {
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
        kCfdP2shP2wsh, nullptr, multisig_script, satoshi, nullptr, kCfdSigHashAll, false, &sighash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("d17f091203341a0d1f0101c6d010a40ce0f3cef8a09b2b605b77bb6cfc23359f", sighash);

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
      ret = CfdFinalizeMultisigSign(
          handle, multisign_handle, kCfdNetworkElementsRegtest, kTxData,
          "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f", 0,
          kCfdP2shP2wsh, multisig_script, &tx_string);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        EXPECT_STREQ("0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a15700000000232200206e67741009d105674a17f86e73433030eea0603c020fb25ea08c6fadd6c2f62dffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac00000000000004004730440220795dbf165d3197fe27e2b73d57cacfb8d742029c972b109040c7785aee4e75ea022065f7a985efe82eba1d0e0cafd7cf711bb8c65485bddc4e495315dd92bd7e4a790147304402202ce4acde192e4109832d46970b510158d42fc156c92afff137157ebfc2a03e2a02200b7dfd3a92770d79d29b3c55fb6325b22bce0e1362de74b2dac80d9689b5a89b0147522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52ae00000000000000000000000000", tx_string);
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
    // Verification Error Case
    int64_t satoshi = 1000000000;
    const char* invalid_pubkey = "024c2f5d47dc96d470f55752ec3edb3b25c15bd5c17be6b73a97f3b3a928cc41fd";
    ret = CfdVerifyConfidentialTxSignature(handle, kTxHex, kSignature,
        invalid_pubkey, NULL, kTxid, kVout, CfdSighashType::kCfdSigHashAll,
        false, satoshi, NULL, CfdWitnessVersion::kCfdWitnessVersionNone);
    EXPECT_EQ(kCfdSignVerificationError, ret);

    // Verification Success Case
    ret = CfdVerifyConfidentialTxSignature(handle, kTxHex, kSignature, kPubkey,
        NULL, kTxid, kVout, CfdSighashType::kCfdSigHashAll, false, satoshi,
        NULL, CfdWitnessVersion::kCfdWitnessVersionNone);
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
    // Verification Success Case
    // int64_t satoshi = 1000000000;
    const char* value_commitment = "0993c069270bf8d090ce8695b82e52fb2959a9765d987d4ffd7a767b0c5b1c4cbc";
    ret = CfdVerifyConfidentialTxSignature(handle, kTxHex, kSignature, kPubkey,
        kScript, kTxid, kVout, CfdSighashType::kCfdSigHashAll, false, 0,
        value_commitment, CfdWitnessVersion::kCfdWitnessVersion0);
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

TEST(cfdcapi_elements_transaction, VerifyConfidentialTxSign) {
  static const char* const kTxHex = "0200000001020f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570000000000ffffffff0f231181a6d8fa2c5f7020948464110fbcc925f94d673d5752ce66d00250a1570100008000ffffffffd8bbe31bc590cbb6a47d2e53a956ec25d8890aefd60dcfc93efd34727554890b0683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000023c346000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b947f6002200d8510dfcf8e2330c0795c771d1e6064daab2f274ac32a6e2708df9bfa893d17a914ef3e40882e17d6e477082fcafeb0f09dc32d377b87010bad521bafdac767421d45b71b29a349c7b2ca2a06b5d8e3b5898c91df2769ed010000000029b9270002cc645552109331726c0ffadccab21620dd7a5a33260c6ac7bd1c78b98cb1e35a1976a9146c22e209d36612e0d9d2a20b814d7d8648cc7a7788ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000000000c350000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000023c3460003ce4c4eac09fe317f365e45c00ffcf2e9639bc0fd792c10f72cdc173c4e5ed8791976a9149bdcb18911fa9faad6632ca43b81739082b0a19588ac0000000000000247304402200268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c20220131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea012103f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d600000000000000000000000000";
  // static const char* const kSignature = "0268633a57723c6612ef217c49bdf804c632a14be2967c76afec4fd5781ad4c2131f358b2381a039c8c502959c64fbfeccf287be7dae710b4446968553aefbea";
  // static const char* const kPubkey = "03f942716865bb9b62678d99aa34de4632249d066d99de2b5a2e542e54908450d6";
  // static const char* const kPrivkey = "cU4KjNUT7GjHm7CkjRjG46SzLrXHXoH3ekXmqa2jTCFPMkQ64sw1";
  static const char* const kTxid = "57a15002d066ce52573d674df925c9bc0f1164849420705f2cfad8a68111230f";
  static const uint32_t kVout = 0;

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    // Verification Success Case
    int64_t satoshi = 13000000000000;
    // const char* value_commitment = "0993c069270bf8d090ce8695b82e52fb2959a9765d987d4ffd7a767b0c5b1c4cbc";
    ret = CfdVerifyConfidentialTxSign(handle, kTxHex,
        kTxid, kVout, "ert1qav7q64dhpx9y4m62rrhpa67trmvjf2ptxfddld",
        kCfdP2wpkhAddress, NULL, satoshi, NULL);
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

TEST(cfdcapi_elements_transaction, VerifyConfidentialTxSign_Commitment) {
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
    ret = CfdVerifyConfidentialTxSign(handle, kTxHex,
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

TEST(cfdcapi_elements_transaction, CfdGetConfidentialValueHex) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    int64_t satoshi = 13000000000000;
    char* value_hex;
    ret = CfdGetConfidentialValueHex(handle, satoshi, false, &value_hex);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0100000bd2cc61d000", value_hex);
      CfdFreeStringBuffer(value_hex);
      value_hex = nullptr;
    }

    ret = CfdGetConfidentialValueHex(handle, satoshi, true, &value_hex);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("00000bd2cc61d000", value_hex);
      CfdFreeStringBuffer(value_hex);
      value_hex = nullptr;
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

TEST(cfdcapi_elements_transaction, CfdGetDefaultBlindingKey) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    const char* master_blinding_key =
        "881a1ab07e99ab0626b4d93b3dddfd16cbc04342ee71aab4da7093e7b853fd80";
    // ert1q0zln07l8vgm5qf4jhzz00668lfs7xssdlxlysh
    const char* locking_script = "001478bf37fbe762374026b2b884f7eb47fa61e3420d";

    char* blinding_key = nullptr;
    ret = CfdGetDefaultBlindingKey(
        handle, master_blinding_key, locking_script, &blinding_key);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
          "95af1be4f929e182442c9f3aa55a3cacde69d1182677f3afd618cdfb4a588742",
          blinding_key);
      CfdFreeStringBuffer(blinding_key);
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

TEST(cfdcapi_elements_transaction, CfdGetAssetCommitment) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    const char* asset =
        "6f1a4b6bd5571b5f08ab79c314dc6483f9b952af2f5ef206cd6f8e68eb1186f3";
    const char* abf = "346dbdba35c19f6e3958a2c00881024503f6611d23d98d270b98ef9de3edc7a3";

    char* commitment = nullptr;
    ret = CfdGetAssetCommitment(handle, asset, abf, &commitment);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
          "0a533b742a568c0b5285bf5bdfe9623a78082d19fac9be1678f7c3adbb48b34d29",
          commitment);
      CfdFreeStringBuffer(commitment);
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

TEST(cfdcapi_elements_transaction, CfdGetValueCommitment) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  if (ret == kCfdSuccess) {
    const char* asset =
        "0a533b742a568c0b5285bf5bdfe9623a78082d19fac9be1678f7c3adbb48b34d29";
    const char* vbf = "fe3357df1f35df75412d9ad86ebd99e622e26019722f316027787a685e2cd71a";

    char* commitment = nullptr;
    ret = CfdGetValueCommitment(handle, int64_t{13000000000000}, asset, vbf, &commitment);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
          "08672d4e2e60f2e8d742552a8bc4ca6335ed214982c7728b4483284169aaae7f49",
          commitment);
      CfdFreeStringBuffer(commitment);
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

TEST(cfdcapi_elements_transaction, CfdAddPeginTest) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;
  int64_t satoshi;

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(handle, kCfdNetworkLiquidv1, 2, 0, nullptr, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));
  if (ret == kCfdSuccess) {
    satoshi = 209998999992700;
    ret = CfdAddTxPeginInput(
        handle, create_handle,
        "4aa201f333e80b8f62ba5b593edb47b4730212e2917b21279f389ba1c14588a3", 2,
        satoshi, "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225",
        "06226e46111a0b59caaf126043eb5bbf28c34f3a5e332a1fc7b2b73cf188910f",
        "0014e8d28b573816ddfcf98578d7b28543e273f5a72a",
        "02000000014578ddc14da3e19445b6e7b4c61d4af711d29e2703161aa9c11e4e6b0ea08843010000006b483045022100eea27e89c3cf2867393263bece040f34c03e0cddfa93a1a18c0d2e4322a37df7022074273c0ab3836affba53737c83673ca6c0d69bffdf722b4accfd7c0a9b2ea4e60121020bfcdbda850cd250c3995dfdb426dc40a9c8a5b378be2bf39f6b0642a783daf2feffffff02281d2418010000001976a914b56872c7b363bfb3f5af84d071ff282cf2abfe3988ac00e1f5050000000017a9141d4796c6e855ae00acecb0c20f65dd8bbeffb1ec87d1000000",
        "03000030ffba1d575800bf37a1ee1962dee7e153c18bcfc93cd013e7c297d5363b36cc2d63d5c4a9fdc746b9d3f4f62995d611c34ee9740ff2b5193ce458fdac6d173800ec402e5affff7f200500000002000000027ce06590120cf8c2bef7726200f0fa655940cadcf62708d7dc9f8f2a417c890b81af4d4299758e7e7a0daa6e7e3d3ec37f97df4ef2392ae5e6d286fc5e7e01d90105");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 200000;
    ret = CfdAddTransactionOutput(
      handle, create_handle, satoshi, nullptr, nullptr,
      "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 209998999792700;
    ret = CfdAddTransactionOutput(
      handle, create_handle, satoshi,
      "CTEyBCf1WzQDpbv6sXLTnFaknGK2UJMHqjyGJjQGq9NEytBR2JLHR9cHJSk4MbLVcKQYWsWERUEYN6R3", "",
      "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeTransaction(handle, create_handle, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("020000000101a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0200004000ffffffff020125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000000000030d4000000125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000befe33c92c3c03f234757d0e00e6a7a7a3b4b2b31fb0328d7b9f755cd1093d9f61892fef3116871976a91435ef6d4b59f26089dfe2abca21408e15fee42a3388ac0000000000000006087c39cc33febe00002025b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a200f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206160014e8d28b573816ddfcf98578d7b28543e273f5a72ae002000000014578ddc14da3e19445b6e7b4c61d4af711d29e2703161aa9c11e4e6b0ea08843010000006b483045022100eea27e89c3cf2867393263bece040f34c03e0cddfa93a1a18c0d2e4322a37df7022074273c0ab3836affba53737c83673ca6c0d69bffdf722b4accfd7c0a9b2ea4e60121020bfcdbda850cd250c3995dfdb426dc40a9c8a5b378be2bf39f6b0642a783daf2feffffff02281d2418010000001976a914b56872c7b363bfb3f5af84d071ff282cf2abfe3988ac00e1f5050000000017a9141d4796c6e855ae00acecb0c20f65dd8bbeffb1ec87d10000009703000030ffba1d575800bf37a1ee1962dee7e153c18bcfc93cd013e7c297d5363b36cc2d63d5c4a9fdc746b9d3f4f62995d611c34ee9740ff2b5193ce458fdac6d173800ec402e5affff7f200500000002000000027ce06590120cf8c2bef7726200f0fa655940cadcf62708d7dc9f8f2a417c890b81af4d4299758e7e7a0daa6e7e3d3ec37f97df4ef2392ae5e6d286fc5e7e01d9010500000000", tx_string);
      CfdFreeStringBuffer(tx_string);
    }
  }

  CfdFreeTransactionHandle(handle, create_handle);

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

TEST(cfdcapi_elements_transaction, CfdAddPegoutTest) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;
  int64_t satoshi;

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(handle, kCfdNetworkLiquidv1, 2, 0, nullptr, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));
  if (ret == kCfdSuccess) {
    ret = CfdAddTransactionInput(
      handle, create_handle, "4aa201f333e80b8f62ba5b593edb47b4730212e2917b21279f389ba1c14588a3", 0,
      4294967293U);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 209998999992700;
    ret = CfdAddTransactionOutput(
      handle, create_handle, satoshi,
      "XBMr6srTXmWuHifFd8gs54xYfiCBsvrksA", "",
      "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    satoshi = 1000000000;
    char* mainchain_address = nullptr;
    ret = CfdAddTxPegoutOutput(
      handle, create_handle,
      "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225", satoshi,
      kCfdNetworkRegtest, kCfdNetworkElementsRegtest,
      "0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206",
      "0214156e4ae9168289b4d0c034da94025121d33ad8643663454885032d77640e3d",
      "cVPA9nh4bHhKXinBCLkJJTD3UgfiizWRykXfFegwZzKMNYAKG9RL",
      "sh(wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y/0/*))",
      0, "030e07d4f657c0c169e04fac5d5a8096adb099874834be59ad1e681e22d952ccda0214156e4ae9168289b4d0c034da94025121d33ad8643663454885032d77640e3d",
      &mainchain_address);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("2N8UxQ5u9YXYFn6Ukj5KGXCMDUZTixKTXHo", mainchain_address);
      CfdFreeStringBuffer(mainchain_address);
    }
  }

  if (ret == kCfdSuccess) {
    satoshi = 7300;
    ret = CfdAddTransactionOutput(
      handle, create_handle, satoshi,
      nullptr, nullptr,
      "5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225");
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    void* err_handle = NULL;
    ret = CfdCreateHandle(&err_handle);
    if (ret == kCfdSuccess) {
      ret = CfdHasPegoutConfidentialTxOut(err_handle, create_handle, 0);
      EXPECT_EQ(kCfdNotFoundError, ret);
      CfdFreeHandle(err_handle);
    }

    ret = CfdHasPegoutConfidentialTxOut(handle, create_handle, 1);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  if (ret == kCfdSuccess) {
    char* addr = nullptr;
    ret = CfdGetPegoutMainchainAddress(
      handle, create_handle, 1, kCfdNetworkRegtest, &addr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("2N8UxQ5u9YXYFn6Ukj5KGXCMDUZTixKTXHo", addr);
      CfdFreeStringBuffer(addr);
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeTransaction(handle, create_handle, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("020000000001a38845c1a19b389f27217b91e2120273b447db3e595bba628f0be833f301a24a0000000000fdffffff030125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000befe33cc397c0017a914001d6db698e75a5a8af771730c4ab258af30546b870125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a01000000003b9aca00009e6a2006226e46111a0b59caaf126043eb5bbf28c34f3a5e332a1fc7b2b73cf188910f17a914a722b257cabc3b8e7d46f8fb293f893f368219da872103700dcb030588ed828d85f645b48971de0d31e8c0244da46710d18681627f5a4a4101044e949dcf8ac2daac82a3e4999ee28e2711661793570c4daab34cd38d76a425d6bfe102f3fea8be12109925fad32c78b65afea4de1d17a826e7375d0e2d00660125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a010000000000001c84000000000000", tx_string);
      CfdFreeStringBuffer(tx_string);
    }
  }

  CfdFreeTransactionHandle(handle, create_handle);

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

TEST(cfdcapi_elements_transaction, CfdSetIssueTest) {
  const char* base_tx = "0200000001017f3da365db9401a4d3facf68d2ccb6372bb714491987e5d035d2b474721078c601000000171600149a417c11cb67e1dc522997f07e1ff89e960d5ff1fdffffff020135e7a177b434ee0799be6dcffc945a1d892f2e0fdfc5975ba0f80d3bdbab9c84010000000002f9c1ec0017a914c9cbab5b0f3430e824b1961bf8e876be43d3fee0870135e7a177b434ee0799be6dcffc945a1d892f2e0fdfc5975ba0f80d3bdbab9c8401000000000000e07400000000000000000247304402207ab059e55e3e4337e88e1a6db00b7549110065eb5770880b1081dcdcdcf1c9a402207a3a0bc7d0d40661f54eff63c67838260a489984138d24eeee04b689f393bf2e012103753cff6c6123d25d99a3d02dc050a2c6b3ea40bcc04029c4330a4d30cb5390770000000000";
  const char* exp_tx = "0200000001017f3da365db9401a4d3facf68d2ccb6372bb714491987e5d035d2b474721078c601000080171600149a417c11cb67e1dc522997f07e1ff89e960d5ff1fdffffff000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000002540be40001000000003b9aca00040135e7a177b434ee0799be6dcffc945a1d892f2e0fdfc5975ba0f80d3bdbab9c84010000000002f9c1ec0017a914c9cbab5b0f3430e824b1961bf8e876be43d3fee0870135e7a177b434ee0799be6dcffc945a1d892f2e0fdfc5975ba0f80d3bdbab9c8401000000000000e07400000107ec1ec7027d89071814d5ccd1f5ea4cee45e598287fc8f59acbb1d9129081dc0100000002540be400001976a914144f003aa8dd6408ba0e8ee91757cf1f1976315c88ac01aaf1579c847497d406605b4ef875a2b37164f4c5b9e5d2a23b2b2a16e132ec0501000000003b9aca00001976a914ae8cab151547d6f6e25b62b41200368dfdabe62b88ac0000000000000247304402207ab059e55e3e4337e88e1a6db00b7549110065eb5770880b1081dcdcdcf1c9a402207a3a0bc7d0d40661f54eff63c67838260a489984138d24eeee04b689f393bf2e012103753cff6c6123d25d99a3d02dc050a2c6b3ea40bcc04029c4330a4d30cb539077000000000000000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(handle, kCfdNetworkLiquidv1, 2, 0,
      base_tx, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));
  if (ret == kCfdSuccess) {
    char* entropy = nullptr;
    char* asset_string = nullptr;
    char* token_string = nullptr;
    ret = CfdSetIssueAsset(
        handle, create_handle,
        "c678107274b4d235d0e587194914b72b37b6ccd268cffad3a40194db65a33d7f", 1,
        nullptr, 10000000000, "2dbH8YS7rqRDM1F7EXrGBXvZywXxuEQtZ2z",
        nullptr, 1000000000, "2dqLgUheB1R4gw7G2DKuxBeMr1jdgxECAoG", nullptr,
        false, &entropy, &asset_string, &token_string);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0a002ed099bd2d52f4bb04d36ebc159c838f0557461d462127845b996e61cb70", entropy);
      EXPECT_STREQ("dc819012d9b1cb9af5c87f2898e545ee4ceaf5d1ccd5141807897d02c71eec07", asset_string);
      EXPECT_STREQ("05ec32e1162a2b3ba2d2e5b9c5f46471b3a275f84e5b6006d49774849c57f1aa", token_string);
      CfdFreeStringBuffer(entropy);
      CfdFreeStringBuffer(asset_string);
      CfdFreeStringBuffer(token_string);
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeTransaction(handle, create_handle, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_tx, tx_string);
      CfdFreeStringBuffer(tx_string);
    }
  }

  CfdFreeTransactionHandle(handle, create_handle);

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

TEST(cfdcapi_elements_transaction, CfdSetReissueTest) {
  const char* base_tx = "0200000000025e67a3542db02871642bdf0923d33ac5bd1105f3421520297f01617c6323918d0000000000ffffffff5e67a3542db02871642bdf0923d33ac5bd1105f3421520297f01617c6323918d0200000000ffffffff03017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c18010000000029b9270003f4f10478f5fae2c77c44fdb2a304109a9ae8ba7cab1c125deba9e618ca737e851976a91484c2ef274919355bb532a5bbdbfa95490c5d749388ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b97049c03b69e5ef61aef08565404bc3c98d6e4dfd1e02eb94138617d9b3eb12ec790885317a91401e940d9dcd77a5043356941641aa2fd6ec6a68887017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c18010000000000002710000000000000";
  const char* exp_tx = "0200000000025e67a3542db02871642bdf0923d33ac5bd1105f3421520297f01617c6323918d0000000000ffffffff5e67a3542db02871642bdf0923d33ac5bd1105f3421520297f01617c6323918d0200008000ffffffffcf9a1b9aeb2de6a7b7c3177433dced0951fd728dffbe8c935ccb80698f2e08c80683fe0819a4f9770c8a7cd5824e82975c825e017aff8ba0d6a5eb4959cf9c6f010000000029b927000004017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c18010000000029b9270003f4f10478f5fae2c77c44fdb2a304109a9ae8ba7cab1c125deba9e618ca737e851976a91484c2ef274919355bb532a5bbdbfa95490c5d749388ac017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c1801000000003b97049c03b69e5ef61aef08565404bc3c98d6e4dfd1e02eb94138617d9b3eb12ec790885317a91401e940d9dcd77a5043356941641aa2fd6ec6a68887017981c1f171d7973a1fd922652f559f47d6d1506a4be2394b27a54951957f6c18010000000000002710000001cdb0ed311810e61036ac9255674101497850f5eee5e4320be07479c05473cbac010000000029b9270003f234757d0e00e6a7a7a3b4b2b31fb0328d7b9f755cd1093d9f61892fef3116871976a91435ef6d4b59f26089dfe2abca21408e15fee42a3388ac00000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* tx_string = nullptr;

  void* create_handle = nullptr;
  ret = CfdInitializeTransaction(handle, kCfdNetworkLiquidv1, 2, 0, base_tx, &create_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == create_handle));
  if (ret == kCfdSuccess) {
    char* asset_string = nullptr;
    ret = CfdSetReissueAsset(
      handle, create_handle,
      "8d9123637c61017f29201542f30511bdc53ad32309df2b647128b02d54a3675e", 2,
      700000000,
      "c8082e8f6980cb5c938cbeff8d72fd5109eddc337417c3b7a7e62deb9a1b9acf",
      "6f9ccf5949eba5d6a08bff7a015e825c97824e82d57c8a0c77f9a41908fe8306",
      "CTEyBCf1WzQDpbv6sXLTnFaknGK2UJMHqjyGJjQGq9NEytBR2JLHR9cHJSk4MbLVcKQYWsWERUEYN6R3",
      nullptr, &asset_string);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("accb7354c07974e00b32e4e5eef55078490141675592ac3610e6101831edb0cd", asset_string);
      CfdFreeStringBuffer(asset_string);
    }
  }

  if (ret == kCfdSuccess) {
    ret = CfdFinalizeTransaction(handle, create_handle, &tx_string);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_tx, tx_string);
      CfdFreeStringBuffer(tx_string);
    }
  }

  CfdFreeTransactionHandle(handle, create_handle);

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

TEST(cfdcapi_elements_transaction, CfdUnblindTxOutData) {
  /*

    {
      "value-minimum": 1,
      "value-maximum": 4503599627370496,
      "ct-exponent": 0,
      "ct-bits": 52,
      "surjectionproof": "010001f6f270a50e47a77305832363b2ab9c7eea39297a4e2872b078552bca67a0fb0fb4e4c5389dd57b344bb70039538148c7c88b661538742f6632c2d4699616169c",
      "valuecommitment": "091dd6cd19781f14385175b586607221d1f11b3f19f149e707810de78f7c7f6f79",
      "assetcommitment": "0bb0852f9c11249a0c8ebb1c1bc3f99c5f643fa3426d4e5c730e702aaf3f2581fc",
      "commitmentnonce": "0398e7cd1cb3c9c13506f91b946f5586cc8fa45f36dde615f2bdb48c1dfe904270",
      "commitmentnonce_fully_valid": true,
      "n": 0,
      "scriptPubKey": {
        "asm": "OP_HASH160 001d6db698e75a5a8af771730c4ab258af30546b OP_EQUAL",
        "hex": "a914001d6db698e75a5a8af771730c4ab258af30546b87",
        "reqSigs": 1,
        "type": "scripthash",
        "addresses": [
          "GhC8ey85yGqMr7e8k2hPcZAE1YvEHffk6F"
        ]
      }
    },
  */
  const char* locking_script = "a914001d6db698e75a5a8af771730c4ab258af30546b87";
  const char* asset_commitment = 
    "0bb0852f9c11249a0c8ebb1c1bc3f99c5f643fa3426d4e5c730e702aaf3f2581fc";
  const char* value_commitment = 
    "091dd6cd19781f14385175b586607221d1f11b3f19f149e707810de78f7c7f6f79";
  const char* nonce_commitment = 
    "0398e7cd1cb3c9c13506f91b946f5586cc8fa45f36dde615f2bdb48c1dfe904270";
  const char* rangeproof = 
    "6033000000000000000121579e0109f21bb7cd0326804abb14059b54fa186892f060384435402393a6589fb54df6ede461ce206af5b13abbe1a564404abf7ab1cc4ed6bd1d93a91728224b21c10a540d76f0ccc047ffa3e06a8abad69687213373d5bcfd4fcf62ecd11c2aff9e1a9d64602cab441e3777d771f88f7e805223a5f192ed3d63b937e93e0e170e6b76e8a05fc5bb46826967d4850dbd2e9128ed615e71149f9aac9304194e968cd8b1ad28a96230b4064795040d16554776ee40b3793bdc9758285df2bc90a5c4be023d206293c071962939160f640ecedbba5a3731aa0aa60afd94f2d9a9d366b4e55ec8c561455ba6a849f8f0df0b19eaf98993655102500a47b76ad320522e4c1f9f829333331017cd0040226f60e426fc00b3844d08e364f6c9f346e3eb7dcf8de6e12a2ac98f11bed6b0c3e06c0bf767e96414ec276a2eb145f64eccb663b46135a19ac17cce785f15b1b2cb7206483329f4c38236ad3cd6cf1646a95ddb94beccb237dc6a854098b139df24f2c8ce7ce7e6d799a434f415ba52ce643bd51619edd786df734b0cee6a5a9ff0442f50a0ca011d8e5b695e6a44016884dbf2f6f681bc3faa5a1a9be514b87ab47183a85bd98b6eff7d5865246b7564a3008cba9b5bcac11e8b3b76dcc38c5915f14a1c7c08a76a31de11d9084cc0553871d0fb9e6ca25b2d191eb292c8f14c1b0cf9c4fedd0a85989a388c675e51003722bae92e356dd680a68f9861fc99be81bdc4f57fa0bf6f63613f80598960aacb8867a94cfbea8d4b776a4771ce1e8b20cf8506cd0524ba85e117085addf4de1d45c2ec6f8e441f74081b792208050ae890a91d5554e37d33ddba2b2c0bbdc2b4a31d0f7eaa1538e5a9fcbd86a87fdf5c6f161c0de5bdc9331b5936b06470c16778077887b3c80a53c5372914d2c3fffd5cb88ee00ffbbbdb10d7ce0fbbca1e725438b7f51a81b611b9f0c4330c3ec3b8d191218fc720c3165305412425f73efacdf44fb0cda9bd1f5290cac09a7f27ca68cb9b485d83c822282cadbfab69ef4f079d1d9e6042033a4a2c8ee9203509f9669f0553ece40d2958d3a733b81232b6ef6c21adc2e2716942588146d6dcadcdc94a13326c6828d04692da1a129e73a3f60cdd30d16b19b29f15a734ddcee491159fedec88a6c986e0243e99b9d2ceed1b9b8c7b3180857f067d8d778f8c0d19ef059f4bf2de3de7f3fc26c86a37cf046fae93cbaaf06e9d95ed73e95957aa69c2680087c6b36d51f93d8e3b9596ab4d793619e70a61edd12c14dc928cc56d1047ea3383b77d8d7527fe43317b21c86a323ed982ed1b766698dcda6c6a91bf50d3f01d7f1d8c20b44441cab3eeb5fac0995298eb7a7a6877bc420119a9acc713e221ddcd2038423c431247dd328372744aea39b9797a9757e46f9d83c5bcce8e2af2baef8416d27a2e5274c21ebdf432f844511081cede5f794ac5e4521d432a6497946d014af706cf431f3437d9d8ad04a3c84cbec65c5596569bdd268a2d7bf0025e6debf4209f7a7a6ae29d9357f84007e72358869caa02ce25813115c98ae6054e9bf48732b55bed6cfe87e7ae05eb435a6eece76046dc2da86d0c33283ff6fb4ffb65e8c943a8c298146c31b48eddc0e0255094c5761d9a8e7b155058efc755579301781355607f369724dd81b58d76977c19a9787ed92760ecce6b2ff0b03093f0936ba6b5872cdf0dcad63f4080f9501a813df248c3484f3445e75f8ca7ab028409bcf63c191b9bd352687397a0cab873dbf6c4fec4cf364cb2e7966f4b6d77e6b9a8477d44d3cf3ef5ecdb1faebd35dff45f39d660accf3bb3fa86cbd021e572eba6014e5c2e48702ab874bde9314c20e9073c20bad928b672249d01d8d8c89b368962a2e9cabca801fb27e59b1a58fd11fcf4d697746e1a7e72a8e32186155dd0f90468e6bb40e39e1d599434e7d504e577f2aee60479a21152c8c64c57f11930611385a605c150cb5c582489dd8e268ba8e1dd4b38fb337f347f3aa953a1d437f69209c2672c755d84957522938f6e69ec39a59725fe786e442a3f39083283dcb6177cde1a4abe38d17c4a45e7b1e78460474bee3eb8ccd4a1d0bd9ac7cc4dcac3dc9263117b0f9e05ff85853d9b28444ee840bd3f674dc2abc53fa5d2db8cd64a1739eec76063ffd93352128135b02377f6ca684b11add9c9434f6060f98c1c0f14ad941c12a4c6ca753afd36f22e353cf7147ab62c7245871a87166b7ca5d5e179953ee4d0328d0c7d69fd4128e06f6e546109732bd7d213e1559b280c0f2304ab874fe87410a77ed8c41378e5530e6ba6eb605f87fc75a4763244433366866ea9227cea868d67dde1287460351f8bc132c398bd4aaeadfe2959394ff96b3cbe1a3265777ea0fe676ecfd71daaf213d93edcc4a83fb9d159f858f61f8c28476430d97fd8879df97bb89914bc8e767a008b952976fb54d44492da79036004c36de129118f8fca1528dae9becd6c1471c9b1690fc00d64af653b1672f0db041ae7a0592a85dff25ca470892f8064b9ed01085a929d7b0ef03775fd5546bdab4ffd494a4af9c35ca0b6d3313c2be2c817522ff79fc54d28baf7707463dfe13423ae368bb1c80e7e2e509617f41c54f57ef67c6a786c28f1578be5a64e36cfb3b63c5e1c07619503619031772505987b515faabf7203b4d4413d6abbf8a6c8f00cd69a24e6c0f9c278b0f166763dbeed28ff447814e2d61c92d31840811f2b4e69323e7a35670b7629dbbe04afc7167ee217fe0f74930cf10ef5c07240c70ff3b36d200c1ce866bf108d5566fbff4860389f532871e2d8075fa28c39c85bd68a95f3a1e23265cbc5ed1657711239a32c4c67245d70eb93082323fcc7f113f912c1e0b818186e044b19dfc22d33c82e2cdfb88ead2f152790f47c1b432bce6d899e9c2dcf03969ab2cafd22e4cf7bbefe10bd2b8fb0078e8b5e006cf898259061efbeaa41aa46bd5cc19d3efe05a8c590bd8212b25f862b635876a9a63cde5fb194358c619fd39ef389b60eb7688513ea4dcf6dacc8cc97456a5131bf8de5b424edff078604456e8b6bce2b1504b8715e301a2957cd79f5f942524af676279645ba9f41f141544b0adac60703027cab8c5d9b51ec8103e2da72c3f7c61803cd94765493506e82921ca469b919cbce716ebe10864655fe6fc7177bba55baf3276007b196c81eb97b1fc7d925eb3c91da45a5ea5c5121f109e73f05937a33ecce115a325914bec1cb5590bc01b57fdba899f5e048ab18fb47c71e8cf351dbcf4140c6961a10ef1c3e322bf760ed222ba83b363977172ea2d7c40e23bcd307e28d69c868af7f1f0ace4965d68ee540ef2ba0b81a698db7239a40cf7845d7bf322e444178921dfcf9c23fbcbfc4d306146ccae6ecb707b7fcc7f43aa92071caba893507117621032d9a2c7ac602f013186b7faaa8596105bfdfe47b6e0e93538f322e7417db403a0288d7a6f635ba1c13f43927061717014c80c88b65840a6ca83d59abce9ef4873e34b936650ab0158d145e89e6aaf2d17fc6a9a8bbf8c01906404aac849189b3865687350764be2872532a2271e05d02f820644a33bf6b64c7bef615d6b8dc27250e58a55039b41c6d97f6d472bed31e76b042d4c79c770c242179acbaa6dc625b4b1e7d6525de8aa4cbc4dfbd5d9562ea60be9eafcda1ca7328c4c9db4064550c5268f9259dfa1a2aa3cffdd02ef19c76d32650da3b22ea83e6c04a063ece816fa9371d7669feb1a9a03eca17a9798eeb1626cb4cc996f2cbc8d8a0c5d78ef3e921a276eb03717a423e5cde35a878bf3961aeb3eacb26332bf7eecb5259074d865478bde6810233217cfd1f91c8c85ffd89285e8579ece30591fac67f863b000d665d4298be4142a0f9991c51ec03b828e7620e71b9eee7753b35328028ba0e73e65191a6516f842c65a423f015f3f8edba667a756f4e479d3f8a6d40a45a6698cf8b6a226649b17192fdc39977d9913eafb917fb9ad25ce26cda7f51e10535d3a29a3fb0fe5fe566f9ab8097d4b6c8f6236c42fde414cf1bdf88f83638d152c3504aa9758778c7107b2b43e3a1b5ade047afe4c527afe7c8bc1ae4559f9f393b6c4c35fbcd5089cb9fc96c43a15c7e052e28073046817b969c0e624bd7c147e3e2de3bc8eac41c160f400568bedeb1549f5b4081e1cbf106eff16c546fd03b552960abc0b64969bcdc85797e14bc73eaa45ca04ddbf228308190eed13503934f60b3b715bc3089241745b1df8439e7f08309fe873ea31ce35a6ccdb634dc9f27ee0cbfff9f4602bcd8f8b10a05f2fd7d46948caad11c09ca71effbe0ee63458874df827d9f652cbf1e55402c2a69b8a483029bf807ad63eb41cc92216d9f171284eb5d7dddb8e5d791d746ec35677a0b0b3eb435f65ecbf8dcd5ae2536eccbd554416e070ce4aaa566e2c5f925e6129617f93e531d17d06a18f1049a16e1e6aff5e46d48e0a6406c5a480c9884690c13ec302b9c03491be7bfe7981663a6716c6bc45ffd45b0c8ec581ec1ad7b1f7f409697bb39b6da1e27cf5355d9b6eca4a306c908083e3a81026ba1be096fdb6d9b7004cc21d439d5c6ce1a45afc951c84c7b26bcf9b4de4aef365ed514edc4a04e6999c1b9fa3f1ee332669228f3cdedd0edec4aa56824f4a64f295835b8830d153b0a0bf3f80c32d11d4b541be45d2c2522503d4bb3440829daf60ff383d71748becce66a78c2dab29fb82a6d7692c0da0326e65b8101a8099f7e2fdb60bdb08ed1e627747492c18b9f2f4653c6a8181cadc56cb684e878acc4583cc2f53ddbb18990291a26a750c422025b29de72d36581ef8fb047e7854cf04b4bba931a7a560d8cb4394a047c44926cee25d0b457e86ffaf4ad4f419df6e6c9d89b8f971310f109eaaa39403a9a5e4dc22ab5ce98afafd4f9b97c4202999ea0b62c20995c4c5b3cd466a99622980fb0478ee471168137cba6266ed7ce5caaaeec8df1921ce3749bcb6a6a488d40bc1f84df8f37847efd4833ec9bc39b5890bcda0ba97acab58ac1e389c752340ad20fbd2d6885f1ce7349376302cb3d5070f30288e55b033c4242f328b9ae4565af08519f052f758b4f6eff1e8fb1ef6d0a96eddc4f6d135cdd930d37ce9e9a0699989e5f54c9104a710070a5458c38af044b6335457d6ffa477fdaa8507b6043d8639e30190613ae5667c6a757522345ee1c1ca73b279255f2cf7b7ddfbdb60fa0e218e0675a9c0fc13d8f043230bae9db8073cd0ae2627c663fdedf7b8fe7341b3817623894874932797320ef582943b8fb5ea96827aef251193bc50dc4400cc7f9bc44d83651c05ff17904367f66d736eede1964f90d7fe7ff086b3849ce32a1554182b1ea8703447a61b2d2d6e2753558f1355a3fd84af25fd89fdf495b641880081d974ca641846c8d4495ce11b79933f1c47a1fef41adc1c7992fb11479ec8d117d06668580cf3558ae681626e50d559bf4147b2b1ec59c4979e19ef851612c3c888d57d512a073c0ff9fa7dfb7e7d9552925baf3e95a83f41a6a5bbd867820e791ae13b0923ebbfca3adeb46323c2c85c3fceb9525b5fc50a078f0b0a9c7820fef73429a617b476e89fa77627fe9816be6ba912da6353092c5953c4ff2911500c4e88f4a0124db5edbdce22cb34235d9c9c9d0e00559eabb222aab6dede02d6b0c46adbf6e9437339e267b26b9f2471e94e41a72a4445ff0a855f72bd04e028f845ec445f13f1d28f173a0494d769b552c85721ac10aed7e8a79b72adf8f5e4f409637a0541579cfa918dd50d3d93d00dabe7dac95675b07ac078fbb31e35e77baa280e6b5b510dfafbc0ee5445524e8a4ddc0c6df98080d74df263bf022566e2791d1fa4a72ebf880852e57b1eac8";

  const char* blinding_key = "66e4df5035a64acef16b4aa52ddc8bebd22b22c9eca150774e355abc72909d83";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  char* asset = nullptr;
  char* asset_blinder = nullptr;
  char* value_blinder = nullptr;
  int64_t amount = 0;

  CfdUnblindTxOutData(handle, blinding_key, locking_script, asset_commitment,
      value_commitment, nonce_commitment, rangeproof,
      &asset, &amount, &asset_blinder, &value_blinder);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("5ac9f65c0efcc4775e0baec4ec03abdde22473cd3cf33c0419ca290e0751b225",
      asset);
    EXPECT_EQ(int64_t{209998999992700}, amount);
    EXPECT_STREQ("6b49938ded88d5c2c335133665158134041769882dd560ca47c14631052a981c",
      asset_blinder);
    EXPECT_STREQ("0e396bcbc4c0b74329712b1b5c7f8c9c4f054996da3748b8820563f68a07dedd",
      value_blinder);
    CfdFreeStringBuffer(asset);
    CfdFreeStringBuffer(asset_blinder);
    CfdFreeStringBuffer(value_blinder);
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
