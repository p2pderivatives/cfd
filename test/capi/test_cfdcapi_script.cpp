#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfdc/cfdcapi_common.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdc/cfdcapi_script.h"

/**
 * @brief testing class.
 */
class cfdcapi_script : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_script, CfdParseScriptTest_LockingScript_Test) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  // OP_DUP OP_HASH160 <HASH160(038f5d4ee5a661c04de7b715c6b9ac935456419fa9f484470275d1d489f2793301)> OP_EQUALVERIFY OP_CHECKSIG
  const char* locking_script = "76a9142e3f2c7e30abce5b22451184c5e531a1e23c6e1288ac";
  void* item_handle = nullptr;
  uint32_t item_num = 0;;

  ret = CfdParseScript(handle, locking_script, &item_handle, &item_num);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((nullptr == item_handle));
  EXPECT_EQ(5, item_num);

  if (ret == kCfdSuccess) {
    std::vector<std::string> script_items;
    script_items.reserve(item_num);
    for (uint32_t i = 0; i < item_num; ++i) {
      char* pitem = nullptr;
      ret = CfdGetScriptItem(handle, item_handle, i, &pitem);
      EXPECT_EQ(kCfdSuccess, ret);
      std::string item(pitem);
      CfdFreeStringBuffer(pitem);
      if (ret == kCfdSuccess) {
        script_items.push_back(item);
      }
    }

    if (script_items.size() == 5) {
      EXPECT_STREQ(script_items.at(0).c_str(), "OP_DUP");
      EXPECT_STREQ(script_items.at(1).c_str(), "OP_HASH160");
      EXPECT_STREQ(script_items.at(2).c_str(), "2e3f2c7e30abce5b22451184c5e531a1e23c6e12");
      EXPECT_STREQ(script_items.at(3).c_str(), "OP_EQUALVERIFY");
      EXPECT_STREQ(script_items.at(4).c_str(), "OP_CHECKSIG");
    }
  }
  ret = CfdFreeScriptItemHandle(handle, item_handle);
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

TEST(cfdcapi_script, CfdParseScript_ScriptSig_TEST) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  // <signature(304402204b922f2dafdd926b22b0e669fd774a2d5f10f969b8089a1c3a0384ba7ce95f6e02204e71c2a620cf430fa6d7ceaeb40d5298f20eebae3ecb783714a6adc03c66717d[ALL])> <pubkey(038f5d4ee5a661c04de7b715c6b9ac935456419fa9f484470275d1d489f2793301)>
  const char* unlocking_script = "47304402204b922f2dafdd926b22b0e669fd774a2d5f10f969b8089a1c3a0384ba7ce95f6e02204e71c2a620cf430fa6d7ceaeb40d5298f20eebae3ecb783714a6adc03c66717d0121038f5d4ee5a661c04de7b715c6b9ac935456419fa9f484470275d1d489f2793301";
  void* item_handle = nullptr;
  uint32_t item_num = 0;

  ret = CfdParseScript(handle, unlocking_script, &item_handle, &item_num);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((nullptr == item_handle));
  EXPECT_EQ(2, item_num);

  if (ret == kCfdSuccess) {
    std::vector<std::string> script_items;
    script_items.reserve(item_num);
    for (uint32_t i = 0; i < item_num; ++i) {
      char* pitem = nullptr;
      ret = CfdGetScriptItem(handle, item_handle, i, &pitem);
      EXPECT_EQ(kCfdSuccess, ret);
      std::string item(pitem);
      CfdFreeStringBuffer(pitem);
      if (ret == kCfdSuccess) {
        script_items.push_back(item);
      }
    }

    if (script_items.size() == 2) {
      EXPECT_STREQ(script_items.at(0).c_str(), "304402204b922f2dafdd926b22b0e669fd774a2d5f10f969b8089a1c3a0384ba7ce95f6e02204e71c2a620cf430fa6d7ceaeb40d5298f20eebae3ecb783714a6adc03c66717d01");
      EXPECT_STREQ(script_items.at(1).c_str(), "038f5d4ee5a661c04de7b715c6b9ac935456419fa9f484470275d1d489f2793301");
    }
  }
  ret = CfdFreeScriptItemHandle(handle, item_handle);
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

TEST(cfdcapi_script, CfdConvertScriptAsmToHex_ScriptSig_TEST) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  // <signature([ALL])> <pubkey(038f5d4ee5a661c04de7b715c6b9ac935456419fa9f484470275d1d489f2793301)>
  const char* script_asm = "304402204b922f2dafdd926b22b0e669fd774a2d5f10f969b8089a1c3a0384ba7ce95f6e02204e71c2a620cf430fa6d7ceaeb40d5298f20eebae3ecb783714a6adc03c66717d01 038f5d4ee5a661c04de7b715c6b9ac935456419fa9f484470275d1d489f2793301";
  char* script_hex = nullptr;

  ret = CfdConvertScriptAsmToHex(handle, script_asm, &script_hex);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    EXPECT_STREQ(script_hex, "47304402204b922f2dafdd926b22b0e669fd774a2d5f10f969b8089a1c3a0384ba7ce95f6e02204e71c2a620cf430fa6d7ceaeb40d5298f20eebae3ecb783714a6adc03c66717d0121038f5d4ee5a661c04de7b715c6b9ac935456419fa9f484470275d1d489f2793301");
    CfdFreeStringBuffer(script_hex);
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

TEST(cfdcapi_script, CfdConvertScriptAsmToHex_SimpleScript_TEST) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  // OP_9 OP_15 OP_ADD 24 OP_EQUAL 
  const char* script_asm = "OP_9 OP_15 OP_ADD 24 OP_EQUAL ";
  char* script_hex;

  ret = CfdConvertScriptAsmToHex(handle, script_asm, &script_hex);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    EXPECT_STREQ(script_hex, "595f93011887");
    CfdFreeStringBuffer(script_hex);
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