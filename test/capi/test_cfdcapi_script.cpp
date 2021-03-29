#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "capi/cfdc_internal.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_script.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_script.h"

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

#ifndef CFD_DISABLE_ELEMENTS
TEST(cfdcapi_script, GetMultisigScriptSig) {
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

  void* multisig_handle = nullptr;
  ret = CfdInitializeMultisigScriptSig(handle, &multisig_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    char* sighash = nullptr;
    int64_t satoshi = 13000000000000;
    char* signature = nullptr;
    char* scriptsig = nullptr;
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
        ret = CfdAddMultisigScriptSigDataToDer(
          handle, multisig_handle, signature, kCfdSigHashAll, false, pubkey1);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }

    if (ret == kCfdSuccess) {
      ret = CfdCalculateEcSignature(
        handle, sighash, nullptr, privkey2, kCfdNetworkRegtest, true, &signature);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdAddMultisigScriptSigDataToDer(
          handle, multisig_handle, signature, kCfdSigHashAll, false, pubkey2);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdFreeStringBuffer(signature);
      }
    }
    if (ret == kCfdSuccess) {
      ret = CfdFinalizeMultisigScriptSig(
          handle, multisig_handle, multisig_script, &scriptsig);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        EXPECT_STREQ("0047304402206fc4cc7e489208a2f4d24f5d35466debab2ce7aa34b5d00e0a9426c9d63529cf02202ec744939ef0b4b629c7d87bc2d017714b52bb86dccb0fd0f10148f62b7a09ba01473044022073ea24720b24c736bcb305a5de2fd8117ca2f0a85d7da378fae5b90dc361d227022004c0088bf1b73a56ae5ec407cf9c330d7206ffbcd0c9bb1c72661726fd4990390147522102bfd7daa5d113fcbd8c2f374ae58cbb89cbed9570e898f1af5ff989457e2d4d712102715ed9a5f16153c5216a6751b7d84eba32076f0b607550a58b209077ab7c30ad52ae", scriptsig);
        CfdFreeStringBuffer(scriptsig);
      }
    }
    CfdFreeStringBuffer(sighash);

    ret = CfdFreeMultisigScriptSigHandle(handle, multisig_handle);
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

TEST(cfdcapi_script, TapscriptTree) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* tree_handle = nullptr;
  ret = CfdInitializeTaprootScriptTree(handle, &tree_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdSetInitialTapLeaf(handle, tree_handle,
        "201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac",
        0xc0);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTapBranchByHash(handle, tree_handle,
        "4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6d");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTapBranchByHash(handle, tree_handle,
        "dc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54");
    EXPECT_EQ(kCfdSuccess, ret);

    uint32_t branch_count = 0;
    ret = CfdGetTapBranchCount(handle, tree_handle, &branch_count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, branch_count);

    uint8_t depth = 0;
    char* branch_hash = nullptr;
    ret = CfdGetTapBranchData(handle, tree_handle,
        1, false, &branch_hash, nullptr, nullptr, &depth);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(0, depth);
      EXPECT_STREQ(
        "dc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54",
        branch_hash);
      CfdFreeStringBuffer(branch_hash);
      branch_hash = nullptr;
    }

    char* witness_program = nullptr;
    char* leaf_hash = nullptr;
    char* control_block = nullptr;
    ret = CfdGetTaprootScriptTreeHash(handle, tree_handle,
        "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
        &witness_program,
        &leaf_hash,
        &control_block);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "3dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d2",
        witness_program);
      EXPECT_STREQ(
        "dfc43ba9fc5f8a9e1b6d6a50600c704bb9e41b741d9ed6de6559a53d2f38e513",
        leaf_hash);
      EXPECT_STREQ(
        "c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54",
        control_block);
      CfdFreeStringBuffer(witness_program);
      witness_program = nullptr;
      CfdFreeStringBuffer(leaf_hash);
      leaf_hash = nullptr;
      CfdFreeStringBuffer(control_block);
      control_block = nullptr;
    }

    char* tweaked_privkey = nullptr;
    ret = CfdGetTaprootTweakedPrivkey(handle, tree_handle,
        "305e293b010d29bf3c888b617763a438fee9054c8cab66eb12ad078f819d9f27",
        &tweaked_privkey);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
          "a7d17bee0b6313cf864a1ac6f203aafd74a40703ffc050f66517e4f83ff41a03",
          tweaked_privkey);
      CfdFreeStringBuffer(tweaked_privkey);
      tweaked_privkey = nullptr;
    }

    char* tree_str = nullptr;
    ret = CfdGetTaprootScriptTreeSrting(handle, tree_handle, &tree_str);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "{{4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6d,tl(201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac)},dc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54}",
        tree_str);
      CfdFreeStringBuffer(tree_str);
      tree_str = nullptr;
    }

    int tmp_ret = CfdFreeTaprootScriptTreeHandle(handle, tree_handle);
    EXPECT_EQ(kCfdSuccess, tmp_ret);
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

TEST(cfdcapi_script, TapscriptTreeFromWitness) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* tree_handle = nullptr;
  ret = CfdInitializeTaprootScriptTree(handle, &tree_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    char* internal_pubkey = nullptr;
    ret = CfdSetTapScriptByWitnessStack(handle, tree_handle,
        "c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54",
        "201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac",
        &internal_pubkey);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
        internal_pubkey);
      CfdFreeStringBuffer(internal_pubkey);
      internal_pubkey = nullptr;
    }

    uint8_t leaf_version = 0;
    char* tapscript = nullptr;
    char* leaf_hash = nullptr;
    ret = CfdGetBaseTapLeaf(handle, tree_handle,
        &leaf_version, &tapscript, &leaf_hash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(0xc0, leaf_version);
      EXPECT_STREQ(
        "201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac",
        tapscript);
      EXPECT_STREQ(
        "dfc43ba9fc5f8a9e1b6d6a50600c704bb9e41b741d9ed6de6559a53d2f38e513",
        leaf_hash);
      CfdFreeStringBuffer(tapscript);
      tapscript = nullptr;
      CfdFreeStringBuffer(leaf_hash);
      leaf_hash = nullptr;
    }

    char* witness_program = nullptr;
    char* control_block = nullptr;
    ret = CfdGetTaprootScriptTreeHash(handle, tree_handle,
        "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
        &witness_program,
        &leaf_hash,
        &control_block);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "3dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d2",
        witness_program);
      EXPECT_STREQ(
        "dfc43ba9fc5f8a9e1b6d6a50600c704bb9e41b741d9ed6de6559a53d2f38e513",
        leaf_hash);
      EXPECT_STREQ(
        "c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54",
        control_block);
      CfdFreeStringBuffer(witness_program);
      witness_program = nullptr;
      CfdFreeStringBuffer(leaf_hash);
      leaf_hash = nullptr;
      CfdFreeStringBuffer(control_block);
      control_block = nullptr;
    }

    char* tree_str = nullptr;
    ret = CfdGetTaprootScriptTreeSrting(handle, tree_handle, &tree_str);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "{{4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6d,tl(201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac)},dc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54}",
        tree_str);
      CfdFreeStringBuffer(tree_str);
      tree_str = nullptr;
    }

    int tmp_ret = CfdFreeTaprootScriptTreeHandle(handle, tree_handle);
    EXPECT_EQ(kCfdSuccess, tmp_ret);
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

TEST(cfdcapi_script, TapscriptTreeAddTree1) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* tree_handle = nullptr;
  ret = CfdInitializeTaprootScriptTree(handle, &tree_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdSetInitialTapLeaf(handle, tree_handle,
        "20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac",
        0xc0);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTapBranchByTapLeaf(handle, tree_handle,
        "51",
        0xc0);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTapBranchByTapLeaf(handle, tree_handle,
        "2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac",
        0xc0);
    EXPECT_EQ(kCfdSuccess, ret);

    uint32_t branch_count = 0;
    ret = CfdGetTapBranchCount(handle, tree_handle, &branch_count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, branch_count);

    uint8_t depth = 0;
    uint8_t leaf_version = 0;
    char* branch_hash = nullptr;
    char* tapscript = nullptr;
    ret = CfdGetTapBranchData(handle, tree_handle,
        0, false, &branch_hash, &leaf_version, &tapscript, &depth);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(0, depth);
      EXPECT_EQ(0xc0, leaf_version);
      EXPECT_STREQ(
        "a85b2107f791b26a84e7586c28cec7cb61202ed3d01944d832500f363782d675",
        branch_hash);
      EXPECT_STREQ(
        "51",
        tapscript);
      CfdFreeStringBuffer(branch_hash);
      branch_hash = nullptr;
      CfdFreeStringBuffer(tapscript);
      tapscript = nullptr;
    }

    ret = CfdGetTapBranchData(handle, tree_handle,
        1, false, &branch_hash, &leaf_version, &tapscript, &depth);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(0, depth);
      EXPECT_EQ(0xc0, leaf_version);
      EXPECT_STREQ(
        "e47f58011f27e9046b8195d0ab6a2acbc68ce281437a8d5132dadf389b2a5ebb",
        branch_hash);
      EXPECT_STREQ(
        "2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac",
        tapscript);
      CfdFreeStringBuffer(branch_hash);
      branch_hash = nullptr;
      CfdFreeStringBuffer(tapscript);
      tapscript = nullptr;
    }

    ret = CfdGetTapBranchData(handle, tree_handle,
        1, true, &branch_hash, nullptr, nullptr, &depth);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(0, depth);
      EXPECT_STREQ(
        "a625d1251a1100263fa9a77b81e9e6f46c2eb8d44b9f27b629875cc102efb0ec",
        branch_hash);
      CfdFreeStringBuffer(branch_hash);
      branch_hash = nullptr;
    }

    char* witness_program = nullptr;
    char* leaf_hash = nullptr;
    char* control_block = nullptr;
    ret = CfdGetTaprootScriptTreeHash(handle, tree_handle,
        "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
        &witness_program,
        &leaf_hash,
        &control_block);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "083a2209e2f0dc07e362bc1dfe195ed974f176358bc986d7c45b48d4deb363f7",
        witness_program);
      EXPECT_STREQ(
        "4691fbb1196f4675241c8958a7ab6378a63aa0cc008ed03d216fd038357f52fd",
        leaf_hash);
      EXPECT_STREQ(
        "c11777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfba85b2107f791b26a84e7586c28cec7cb61202ed3d01944d832500f363782d675e47f58011f27e9046b8195d0ab6a2acbc68ce281437a8d5132dadf389b2a5ebb",
        control_block);
      CfdFreeStringBuffer(witness_program);
      witness_program = nullptr;
      CfdFreeStringBuffer(leaf_hash);
      leaf_hash = nullptr;
      CfdFreeStringBuffer(control_block);
      control_block = nullptr;
    }

    char* tree_str = nullptr;
    ret = CfdGetTaprootScriptTreeSrting(handle, tree_handle, &tree_str);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "{{tl(20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac),tl(51)},tl(2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac)}",
        tree_str);
      CfdFreeStringBuffer(tree_str);
      tree_str = nullptr;
    }

    int tmp_ret = CfdFreeTaprootScriptTreeHandle(handle, tree_handle);
    EXPECT_EQ(kCfdSuccess, tmp_ret);
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

TEST(cfdcapi_script, TapscriptTreeAddTree2) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int tmp_ret = 0;
  void* tree_handle1 = nullptr;
  ret = CfdInitializeTaprootScriptTree(handle, &tree_handle1);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdSetInitialTapLeaf(handle, tree_handle1,
        "20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac",
        0xc0);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTapBranchByTapLeaf(handle, tree_handle1,
        "51",
        0xc0);
    EXPECT_EQ(kCfdSuccess, ret);

    void* tree_handle2 = nullptr;
    ret = CfdInitializeTaprootScriptTree(handle, &tree_handle2);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      ret = CfdSetInitialTapLeaf(handle, tree_handle2,
          "2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac",
          0xc0);
      EXPECT_EQ(kCfdSuccess, ret);

      ret = CfdAddTapBranchByScriptTree(handle, tree_handle2, tree_handle1);
      EXPECT_EQ(kCfdSuccess, ret);

      uint32_t branch_count = 0;
      ret = CfdGetTapBranchCount(handle, tree_handle2, &branch_count);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_EQ(1, branch_count);

      char* tree_str = nullptr;
      ret = CfdGetTaprootScriptTreeSrting(handle, tree_handle2, &tree_str);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        EXPECT_STREQ(
          "{{tl(20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac),tl(51)},tl(2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac)}",
          tree_str);
        CfdFreeStringBuffer(tree_str);
        tree_str = nullptr;
      }

      tmp_ret = CfdFreeTaprootScriptTreeHandle(handle, tree_handle2);
      EXPECT_EQ(kCfdSuccess, tmp_ret);
    }

    tmp_ret = CfdFreeTaprootScriptTreeHandle(handle, tree_handle1);
    EXPECT_EQ(kCfdSuccess, tmp_ret);
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

TEST(cfdcapi_script, TapscriptTreeAddTreeString) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int tmp_ret = 0;
  void* tree_handle1 = nullptr;
  ret = CfdInitializeTaprootScriptTree(handle, &tree_handle1);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdSetInitialTapLeaf(handle, tree_handle1,
        "2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac",
        0xc0);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTapBranchByScriptTreeString(handle, tree_handle1,
        "{tl(20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac),tl(51)}");
    EXPECT_EQ(kCfdSuccess, ret);

    uint32_t branch_count = 0;
    ret = CfdGetTapBranchCount(handle, tree_handle1, &branch_count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, branch_count);

    char* tree_str = nullptr;
    ret = CfdGetTaprootScriptTreeSrting(handle, tree_handle1, &tree_str);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "{{tl(20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac),tl(51)},tl(2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac)}",
        tree_str);
      CfdFreeStringBuffer(tree_str);
      tree_str = nullptr;
    }

    tmp_ret = CfdFreeTaprootScriptTreeHandle(handle, tree_handle1);
    EXPECT_EQ(kCfdSuccess, tmp_ret);
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

TEST(cfdcapi_script, TapscriptTreeRestoreFromString) {
  const char* exp_tree_str = "{{tl(20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac),tl(51)},tl(2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac)}";
  const char* tapscript_str = "20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int tmp_ret = 0;
  void* tree_handle1 = nullptr;
  ret = CfdInitializeTaprootScriptTree(handle, &tree_handle1);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdSetScriptTreeFromString(handle, tree_handle1,
        exp_tree_str, tapscript_str, 0xc0, "");
    EXPECT_EQ(kCfdSuccess, ret);

    uint32_t branch_count = 0;
    ret = CfdGetTapBranchCount(handle, tree_handle1, &branch_count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, branch_count);

    char* tree_str = nullptr;
    ret = CfdGetTaprootScriptTreeSrting(handle, tree_handle1, &tree_str);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(
        "{{tl(20ac52f50b28cdd4d3bcb7f0d5cb533f232e4c4ef12fbf3e718420b84d4e3c3440ac),tl(51)},tl(2057bf643684f6c5c75e1cdf45990036502a0d897394013210858cdabcbb95a05aad205bec1a08fa3443176edd0a08e2a64642f45e57543b62bffe43ec350edc33dc22ac)}",
        tree_str);
      CfdFreeStringBuffer(tree_str);
      tree_str = nullptr;
    }

    uint8_t leaf_version = 0;
    char* tapscript = nullptr;
    char* leaf_hash = nullptr;
    ret = CfdGetBaseTapLeaf(handle, tree_handle1,
        &leaf_version, &tapscript, &leaf_hash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(0xc0, leaf_version);
      EXPECT_STREQ(tapscript_str, tapscript);
      EXPECT_STREQ(
        "4691fbb1196f4675241c8958a7ab6378a63aa0cc008ed03d216fd038357f52fd",
        leaf_hash);
      CfdFreeStringBuffer(tapscript);
      tapscript = nullptr;
      CfdFreeStringBuffer(leaf_hash);
      leaf_hash = nullptr;
    }

    tmp_ret = CfdFreeTaprootScriptTreeHandle(handle, tree_handle1);
    EXPECT_EQ(kCfdSuccess, tmp_ret);
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
