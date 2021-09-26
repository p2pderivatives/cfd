#include "gtest/gtest.h"

#include <stdexcept>
#include <string>
#include "capi/cfdc_internal.h"
#include "cfdc/cfdcapi_block.h"
#include "cfdc/cfdcapi_common.h"

/**
 * @brief testing class.
 */
class cfdcapi_block : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_block, GetBlockData) {
  void* handle = nullptr;
  int ret = CfdCreateSimpleHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);

  const char* block_hex = "00000030957958949bad814d1666ed0d4a005c8aed6b7fd56df5d12c81d584c71e5fae2dfe391f9150dcfb06d54d4eb6621672590bf46bed6893da825c076b841794cec5414e2660ffff7f200000000001020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff0502d5000101ffffffff0200f9029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d50000000000000000266a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf90120000000000000000000000000000000000000000000000000000000000000000000000000";
  void* block_handle = nullptr;
  ret = CfdInitializeBlockHandle(
    handle, kCfdNetworkMainnet, block_hex, &block_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_NE(nullptr, block_handle);
  if (ret == kCfdSuccess) {
    char* block_hash = nullptr;
    ret = CfdGetBlockHash(handle, block_handle, &block_hash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "53fd7b794cf751a148b2be637df6c7daf663f1be509cb35294bd69400fdc694e",
        block_hash);
      CfdFreeStringBuffer(block_hash);
    }
    const char* txid = "c5ce9417846b075c82da9368ed6bf40b59721662b64e4dd506fbdc50911f39fe";

    char* prev_hash = nullptr;
    char* merkle_hash = nullptr;
    uint32_t version = 0;
    uint32_t nonce = 0;
    uint32_t bits = 0;
    uint32_t time_data = 0;
    ret = CfdGetBlockHeaderData(handle, block_handle, &version, &prev_hash,
        &merkle_hash, &time_data, &bits, &nonce);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(805306368, version);
      EXPECT_EQ(1613123137, time_data);
      EXPECT_EQ(0x207fffff, bits);
      EXPECT_EQ(0, nonce);
      EXPECT_STREQ(
        "2dae5f1ec784d5812cd1f56dd57f6bed8a5c004a0ded66164d81ad9b94587995",
        prev_hash);
      EXPECT_STREQ(
        "c5ce9417846b075c82da9368ed6bf40b59721662b64e4dd506fbdc50911f39fe",
        merkle_hash);
      CfdFreeStringBuffer(prev_hash);
      CfdFreeStringBuffer(merkle_hash);
    }

    char* tx = nullptr;
    ret = CfdGetTransactionFromBlock(handle, block_handle, txid, &tx);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "020000000001010000000000000000000000000000000000000000000000000000000000000000ffffffff0502d5000101ffffffff0200f9029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d50000000000000000266a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf90120000000000000000000000000000000000000000000000000000000000000000000000000",
        tx);
      CfdFreeStringBuffer(tx);
    }

    char* proof = nullptr;
    ret = CfdGetTxOutProof(handle, block_handle, txid, &proof);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "00000030957958949bad814d1666ed0d4a005c8aed6b7fd56df5d12c81d584c71e5fae2dfe391f9150dcfb06d54d4eb6621672590bf46bed6893da825c076b841794cec5414e2660ffff7f20000000000100000001fe391f9150dcfb06d54d4eb6621672590bf46bed6893da825c076b841794cec50101",
        proof);
      CfdFreeStringBuffer(proof);
    }

    ret = CfdExistTxidInBlock(handle, block_handle, txid);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdExistTxidInBlock(handle, block_handle,
        "7f5fb624f5cdce391362aa6befea307c4e778e008e799b40ca7119046f26ab31");
    EXPECT_EQ(kCfdNotFoundError, ret);

    uint32_t tx_count = 0;
    ret = CfdGetTxCountInBlock(handle, block_handle, &tx_count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, tx_count);

    char* temp_txid = nullptr;
    ret = CfdGetTxidFromBlock(handle, block_handle, 0, &temp_txid);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(txid, temp_txid);
      CfdFreeStringBuffer(temp_txid);
    }

    ret = CfdFreeBlockHandle(handle, block_handle);
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
