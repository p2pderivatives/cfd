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
#include "cfdcore/cfdcore_transaction.h"
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

TEST(cfdcapi_transaction, SignTransactionTest) {
  void* handle = nullptr;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  struct CfdSignTransactionTestData {
    int network;
    const char* txid;
    uint32_t vout;
    const char* descriptor;
    int64_t utxo_amount;
    const char* redeem_script;
    const char* tapscript_control;
    const char* tapleaf_hash;
    const char* annex;
    const char* aux_rand;
    const char* pubkey;
    const char* privkey;
    int sighash_type;
    const char* txout_address;
    int64_t amount;
    const char* signed_tx;
    bool use_direct_sign;
    int hash_type;
  };
  const struct CfdSignTransactionTestData exp_datas[] = {
    {  // p2wpkh
      kCfdNetworkRegtest,
      "cd6adc252632eb0768ac6407e586cc74bfed739d6c8b9efa55305eb37cbd76dd", 0,
      "wpkh(023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c44)",
      2500000000,
      nullptr, nullptr, nullptr, nullptr, nullptr,
      "023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c44",
      "28190dff04c40f130b7be4a597213338cb8860ff38bbab5236bd1b3ff8472554",
      kCfdSigHashAll,
      "bcrt1p8hh955u8526hjqhn5m5a5pmhymgecmxgerrmqj70tgvhk25mq8fq50z666",
      2499999000,
      "02000000000101dd76bd7cb35e3055fa9e8b6c9d73edbf74cc86e50764ac6807eb322625dc6acd0000000000ffffffff0118f50295000000002251203dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d20247304402201db912bc61dab1c6117b0aec2965ea1b2d1caa42a1372adc16c8cf673f1187d7022062667d8a976b197f7ba33299365eeb68c1e45fa2a255411672d89f7afab12cb20121023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c4400000000",
      false, kCfdP2wpkh
    },
    {  // p2wpkh auto sign
      kCfdNetworkRegtest,
      "cd6adc252632eb0768ac6407e586cc74bfed739d6c8b9efa55305eb37cbd76dd", 0,
      "wpkh(023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c44)",
      2500000000,
      nullptr, nullptr, nullptr, nullptr, nullptr,
      "023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c44",
      "28190dff04c40f130b7be4a597213338cb8860ff38bbab5236bd1b3ff8472554",
      kCfdSigHashAll,
      "bcrt1p8hh955u8526hjqhn5m5a5pmhymgecmxgerrmqj70tgvhk25mq8fq50z666",
      2499999000,
      "02000000000101dd76bd7cb35e3055fa9e8b6c9d73edbf74cc86e50764ac6807eb322625dc6acd0000000000ffffffff0118f50295000000002251203dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d20247304402201db912bc61dab1c6117b0aec2965ea1b2d1caa42a1372adc16c8cf673f1187d7022062667d8a976b197f7ba33299365eeb68c1e45fa2a255411672d89f7afab12cb20121023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c4400000000",
      true, kCfdP2wpkh
    },
    {  // p2wsh-multisig
      kCfdNetworkRegtest,
      "cd6adc252632eb0768ac6407e586cc74bfed739d6c8b9efa55305eb37cbd76dd", 0,
      "wsh(multi(1,023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c44,03ebb70cf8b4adfff5559794d2e972d55c9429dbda25cd5911615dcab422d031ae))",
      2500000000,
      "5121023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c442103ebb70cf8b4adfff5559794d2e972d55c9429dbda25cd5911615dcab422d031ae52ae",
      nullptr, nullptr, nullptr, nullptr,
      "023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c44",
      "28190dff04c40f130b7be4a597213338cb8860ff38bbab5236bd1b3ff8472554",
      kCfdSigHashAll,
      "bcrt1p8hh955u8526hjqhn5m5a5pmhymgecmxgerrmqj70tgvhk25mq8fq50z666",
      2499999000,
      "02000000000101dd76bd7cb35e3055fa9e8b6c9d73edbf74cc86e50764ac6807eb322625dc6acd0000000000ffffffff0118f50295000000002251203dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d20300473044022050da44f8a8e8ac494b79840c4c7ef7d55a296ffa0b98a5b9d2150964ba6d555802207ae9ec8a4f060440279fa9129ef480132cab1fc638c534ef31af1feda6b5d6f401475121023179b32721d07deb06cade59f56dedefdc932e89fde56e998f7a0e93a3e30c442103ebb70cf8b4adfff5559794d2e972d55c9429dbda25cd5911615dcab422d031ae52ae00000000",
      false, kCfdP2wsh
    },
    {  // taproot schnorr
      kCfdNetworkRegtest,
      "2fea883042440d030ca5929814ead927075a8f52fef5f4720fa3cec2e475d916", 0,
      "raw(51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb)",
      2499999000,
      nullptr, nullptr, nullptr, nullptr, nullptr,
      "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
      "305e293b010d29bf3c888b617763a438fee9054c8cab66eb12ad078f819d9f27",
      kCfdSigHashAll,
      "bcrt1qze8fshg0eykfy7nxcr96778xagufv2w429wx40",
      2499998000,
      "0200000000010116d975e4c2cea30f72f4f5fe528f5a0727d9ea149892a50c030d44423088ea2f0000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d5014161f75636003a870b7a1685abae84eedf8c9527227ac70183c376f7b3a35b07ebcbea14749e58ce1a87565b035b2f3963baa5ae3ede95e89fd607ab7849f208720100000000",
      false, kCfdTaproot
    },
    {  // taproot schnorr auto sign
      kCfdNetworkRegtest,
      "2fea883042440d030ca5929814ead927075a8f52fef5f4720fa3cec2e475d916", 0,
      "raw(51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb)",
      2499999000,
      nullptr, nullptr, nullptr, nullptr, nullptr,
      "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
      "305e293b010d29bf3c888b617763a438fee9054c8cab66eb12ad078f819d9f27",
      kCfdSigHashAll,
      "bcrt1qze8fshg0eykfy7nxcr96778xagufv2w429wx40",
      2499998000,
      "0200000000010116d975e4c2cea30f72f4f5fe528f5a0727d9ea149892a50c030d44423088ea2f0000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d5014161f75636003a870b7a1685abae84eedf8c9527227ac70183c376f7b3a35b07ebcbea14749e58ce1a87565b035b2f3963baa5ae3ede95e89fd607ab7849f208720100000000",
      true, kCfdTaproot
    },
    {  // taproot schnorr rand
      kCfdNetworkRegtest,
      "2fea883042440d030ca5929814ead927075a8f52fef5f4720fa3cec2e475d916", 0,
      "raw(51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb)",
      2499999000,
      nullptr, nullptr, nullptr, nullptr,
      "2fea883042440d030ca5929814ead927075a8f52fef5f4720fa3cec2e475d916",
      "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
      "305e293b010d29bf3c888b617763a438fee9054c8cab66eb12ad078f819d9f27",
      kCfdSigHashAll,
      "bcrt1qze8fshg0eykfy7nxcr96778xagufv2w429wx40",
      2499998000,
      "0200000000010116d975e4c2cea30f72f4f5fe528f5a0727d9ea149892a50c030d44423088ea2f0000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d5014151df55894d1a024c244e20ecedc39cae39fa6d43653305b7f32605eea6359415a7ceef44c52a2f26be2e06d33d79c2e90b5dfaebcb4f79e242134121e0b9579e0100000000",
      false, kCfdTaproot
    },
    {  // tapscript
      kCfdNetworkRegtest,
      "195d6c18afaa6c33dcc2da86c6c0cd3f93ec2b44e4c8e9be00c7000eafa1805b", 0,
      "raw(51203dee5a5387a2b57902f3a6e9da077726d19c6cc8c8c7b04bcf5a197b2a9b01d2)",
      2499999000,
      "201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac",
      "c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d54",
      "dfc43ba9fc5f8a9e1b6d6a50600c704bb9e41b741d9ed6de6559a53d2f38e513",
      nullptr, nullptr,
      "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb",
      "305e293b010d29bf3c888b617763a438fee9054c8cab66eb12ad078f819d9f27",
      kCfdSigHashAll,
      "bcrt1qze8fshg0eykfy7nxcr96778xagufv2w429wx40",
      2499998000,
      "020000000001015b80a1af0e00c700bee9c8e4442bec933fcdc0c686dac2dc336caaaf186c5d190000000000ffffffff0130f1029500000000160014164e985d0fc92c927a66c0cbaf78e6ea389629d50341f5aa6b260f9df687786cd3813ba83b476e195041bccea800f2571212f4aae9848a538b6175a4f8ea291d38e351ea7f612a3d700dca63cd3aff05d315c5698ee90122201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfbac61c01777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb4d18084bb47027f47d428b2ed67e1ccace5520fdc36f308e272394e288d53b6ddc82121e4ff8d23745f3859e8939ecb0a38af63e6ddea2fff97a7fd61a1d2d5400000000",
      false, kCfdTaproot
    },
  };
  size_t list_size = sizeof(exp_datas) / sizeof(struct CfdSignTransactionTestData);

  for (size_t idx = 0; idx < list_size; ++idx) {
    void* create_handle = nullptr;
    char* sighash = nullptr;
    char* sig = nullptr;
    char* tmp_sig = nullptr;
    ret = CfdInitializeTransaction(
        handle, exp_datas[idx].network, 2, 0, nullptr, &create_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_FALSE((NULL == create_handle));
    if (ret == kCfdSuccess) {
      ret = CfdSetTransactionUtxoData(handle, create_handle,
          exp_datas[idx].txid, exp_datas[idx].vout, exp_datas[idx].utxo_amount,
          nullptr, exp_datas[idx].descriptor, nullptr, nullptr, nullptr, true);
      EXPECT_EQ(kCfdSuccess, ret);

      ret = CfdAddTransactionOutput(handle, create_handle,
          exp_datas[idx].amount, exp_datas[idx].txout_address, nullptr, nullptr);
      EXPECT_EQ(kCfdSuccess, ret);

      bool has_tapscript = false;
      if (ret != kCfdSuccess) {
        // do nothing
      } else if (exp_datas[idx].use_direct_sign) {
        ret = CfdAddSignWithPrivkeyByHandle(handle, create_handle,
            exp_datas[idx].txid, exp_datas[idx].vout, exp_datas[idx].privkey,
            exp_datas[idx].sighash_type, false, true, exp_datas[idx].aux_rand,
            exp_datas[idx].annex);
        EXPECT_EQ(kCfdSuccess, ret);
      } else {
        uint32_t code_separator_pos = cfd::core::kDefaultCodeSeparatorPosition;
        ret = CfdCreateSighashByHandle(handle, create_handle,
            exp_datas[idx].txid, exp_datas[idx].vout,
            exp_datas[idx].sighash_type, false,
            (exp_datas[idx].redeem_script) ? nullptr : exp_datas[idx].pubkey,
            exp_datas[idx].redeem_script,
            exp_datas[idx].tapleaf_hash, code_separator_pos,
            exp_datas[idx].annex, &sighash);
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret != kCfdSuccess) {
          // do nothing
        } else if (strlen(exp_datas[idx].pubkey) == 64) {
          // taproot/tapscript
          ret = CfdSignSchnorr(handle, sighash,
              exp_datas[idx].privkey, exp_datas[idx].aux_rand, &tmp_sig);
          EXPECT_EQ(kCfdSuccess, ret);
          ret = CfdAddSighashTypeInSchnorrSignature(handle, tmp_sig,
              exp_datas[idx].sighash_type, false, &sig);
          EXPECT_EQ(kCfdSuccess, ret);

          if (exp_datas[idx].redeem_script != nullptr) {
            has_tapscript = true;
            ret = CfdAddTxSignByHandle(handle, create_handle,
                exp_datas[idx].txid, exp_datas[idx].vout,
                exp_datas[idx].hash_type, sig, false,
                exp_datas[idx].sighash_type, false, true);
            EXPECT_EQ(kCfdSuccess, ret);

            ret = CfdAddTaprootSignByHandle(handle, create_handle,
                exp_datas[idx].txid, exp_datas[idx].vout,
                nullptr, exp_datas[idx].redeem_script,
                exp_datas[idx].tapscript_control, exp_datas[idx].annex);
            EXPECT_EQ(kCfdSuccess, ret);
          } else {
            ret = CfdAddTaprootSignByHandle(handle, create_handle,
                exp_datas[idx].txid, exp_datas[idx].vout,
                sig, nullptr, nullptr, exp_datas[idx].annex);
            EXPECT_EQ(kCfdSuccess, ret);
          }
        } else if (exp_datas[idx].redeem_script != nullptr) {
          // script
          ret = CfdCalculateEcSignature(handle, sighash,
              exp_datas[idx].privkey, nullptr,
              exp_datas[idx].network, true, &sig);
          EXPECT_EQ(kCfdSuccess, ret);

          // multisig first
          ret = CfdAddTxSignByHandle(handle, create_handle,
              exp_datas[idx].txid, exp_datas[idx].vout,
              exp_datas[idx].hash_type, "", false,
              exp_datas[idx].sighash_type, false, true);
          EXPECT_EQ(kCfdSuccess, ret);

          ret = CfdAddTxSignByHandle(handle, create_handle,
              exp_datas[idx].txid, exp_datas[idx].vout,
              exp_datas[idx].hash_type, sig, true,
              exp_datas[idx].sighash_type, false, false);
          EXPECT_EQ(kCfdSuccess, ret);

          ret = CfdAddScriptHashLastSignByHandle(handle, create_handle,
              exp_datas[idx].txid, exp_datas[idx].vout,
              exp_datas[idx].hash_type, exp_datas[idx].redeem_script);
          EXPECT_EQ(kCfdSuccess, ret);
        } else {
          ret = CfdCalculateEcSignature(handle, sighash,
              exp_datas[idx].privkey, nullptr,
              exp_datas[idx].network, true, &sig);
          EXPECT_EQ(kCfdSuccess, ret);

          ret = CfdAddPubkeyHashSignByHandle(handle, create_handle,
            exp_datas[idx].txid, exp_datas[idx].vout,
            exp_datas[idx].hash_type, exp_datas[idx].pubkey,
            sig, true, exp_datas[idx].sighash_type, false);
          EXPECT_EQ(kCfdSuccess, ret);
        }

        if (sighash != nullptr) {
          CfdFreeStringBuffer(sighash);
          sighash = nullptr;
        }
        if (sig != nullptr) {
          CfdFreeStringBuffer(sig);
          sig = nullptr;
        }
        if (tmp_sig != nullptr) {
          CfdFreeStringBuffer(tmp_sig);
          tmp_sig = nullptr;
        }
      }

      if (ret == kCfdSuccess) {
        char* tx_hex = nullptr;
        ret = CfdFinalizeTransaction(handle, create_handle, &tx_hex);
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret == kCfdSuccess) {
          EXPECT_STREQ(exp_datas[idx].signed_tx, tx_hex);
          CfdFreeStringBuffer(tx_hex);
        }
      }

      if (ret == kCfdSuccess) {
        ret = CfdVerifyTxSignByHandle(handle, create_handle,
            exp_datas[idx].txid, exp_datas[idx].vout);
        if ((ret != kCfdSuccess) && has_tapscript) {
          char* err_msg = NULL;
          CfdGetLastErrorMessage(handle, &err_msg);
          const char* tapscript_err_msg = "The script analysis of tapscript is not supported.";
          EXPECT_STREQ(tapscript_err_msg, err_msg);
          if (strcmp(tapscript_err_msg, err_msg) == 0) {
            ret = kCfdSuccess;
          }
          CfdFreeStringBuffer(err_msg);
          err_msg = NULL;
        }
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret != kCfdSuccess) {
          EXPECT_EQ(-1, idx);
        }
      }

      int tmp_ret = CfdFreeTransactionHandle(handle, create_handle);
      EXPECT_EQ(kCfdSuccess, tmp_ret);
    }

#if 0
    ret = CfdGetLastErrorCode(handle);
    if (ret != kCfdSuccess) {
      char* str_buffer = NULL;
      ret = CfdGetLastErrorMessage(handle, &str_buffer);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_STREQ("", str_buffer);
      CfdFreeStringBuffer(str_buffer);
      str_buffer = NULL;
      break;
    }
#endif
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

TEST(cfdcapi_transaction, GetTransactionByHandle) {
  static const char* exp_tx = "0100000000010136641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e0100000023220020a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54ffffffff0200e9a435000000001976a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688acc0832f05000000001976a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac080047304402206ac44d672dac41f9b00e28f4df20c52eeb087207e8d758d76d92c6fab3b73e2b0220367750dbbe19290069cba53d096f44530e4f98acaa594810388cf7409a1870ce01473044022068c7946a43232757cbdf9176f009a928e1cd9a1a8c212f15c1e11ac9f2925d9002205b75f937ff2f9f3c1246e547e54f62e027f64eefa2695578cc6432cdabce271502473044022059ebf56d98010a932cf8ecfec54c48e6139ed6adb0728c09cbe1e4fa0915302e022007cd986c8fa870ff5d2b3a89139c9fe7e499259875357e20fcbb15571c76795403483045022100fbefd94bd0a488d50b79102b5dad4ab6ced30c4069f1eaa69a4b5a763414067e02203156c6a5c9cf88f91265f5a942e96213afae16d83321c8b31bb342142a14d16381483045022100a5263ea0553ba89221984bd7f0b13613db16e7a70c549a86de0cc0444141a407022005c360ef0ae5a5d4f9f2f87a56c1546cc8268cab08c73501d6b3be2e1e1a8a08824730440220525406a1482936d5a21888260dc165497a90a15669636d8edca6b9fe490d309c022032af0c646a34a44d1f4576bf6a4a74b67940f8faa84c7df9abe12a01a11e2b4783cf56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae00000000";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* tx_handle = NULL;
  ret = CfdInitializeTxDataHandle(handle, kCfdNetworkMainnet, exp_tx, &tx_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((tx_handle == handle));
  if (ret == kCfdSuccess) {
    char* txid = nullptr;
    char* wtxid = nullptr;
    uint32_t size = 0;
    uint32_t vsize = 0;
    uint32_t weight = 0;
    uint32_t version = 0;
    uint32_t locktime = 0;
    ret = CfdGetTxInfoByHandle(
        handle, tx_handle, &txid, &wtxid, &size, &vsize,
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
    ret = CfdGetTxInByHandle(
        handle, tx_handle, 0,
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
    ret = CfdGetTxInWitnessByHandle(
        handle, tx_handle, kCfdTxWitnessStackNormal, 0, 7, &stack_data);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae", stack_data);
      CfdFreeStringBuffer(stack_data);
    }

    int64_t value_satoshi = 0;
    char* locking_script = nullptr;
    ret = CfdGetTxOutByHandle(
        handle, tx_handle, 1, &value_satoshi, &locking_script, NULL);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(87000000, value_satoshi);
      EXPECT_STREQ("76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac", locking_script);
      CfdFreeStringBuffer(locking_script);
    }

    uint32_t count = 0;
    ret = CfdGetTxInCountByHandle(
        handle, tx_handle, &count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, count);

    ret = CfdGetTxInWitnessCountByHandle(
        handle, tx_handle, kCfdTxWitnessStackNormal, 0, &count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(8, count);

    ret = CfdGetTxOutCountByHandle(
        handle, tx_handle, &count);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, count);

    uint32_t index = 0;
    ret = CfdGetTxInIndexByHandle(
        handle, tx_handle,
        "6eb98797a21c6c10aa74edf29d618be109f48a8e94c694f3701e08ca69186436", 1, &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);

    ret = CfdGetTxOutIndexByHandle(
        handle, tx_handle, "16AQVuBMt818u2HBcbxztAZTT2VTDKupPS", "", &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);

    ret = CfdGetTxOutIndexByHandle(
        handle, tx_handle, "", "76a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac", &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);

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

  // p2pkh
  ret = CfdVerifySignature(
      handle, kCfdNetworkMainnet,
      "01000000019c53cb2a6118530aaa345b799aeb7e4e5055de41ac5b2dd2ce47419624c57b580000000000ffffffff0130ea052a010000001976a9143cadb10040e9e7002bbd9d0620f5f79c05603ffd88ac00000000",
      "3c1cffcc8908ab1911303f102c41e5c677488346851288360b0d309ab99557207ac2c9c6aec9d8bae187a1eea843dda423edff216c568efad231e4249c77ffe1",
      kCfdP2pkh,
      "02f56451fc1fd9040652ff9a700cf914ad1df1c8f9e82f3fe96ca01b6cd47293ef",
      nullptr,
      "587bc524964147ced22d5bac41de55504e7eeb9a795b34aa0a5318612acb539c", 0,
      kCfdSigHashAll, false, int64_t{1000}, nullptr);
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

TEST(cfdcapi_transaction, VerifySignature_Elements) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  // p2pkh: Elements
  ret = CfdVerifySignature(
      handle, kCfdNetworkLiquidv1,
      "020000000104a76c51c1a0ad26df3eae8e8391a9da3cecea24edabcda10436cd3e550cd2f9e20100000000fdffffff37ae0d4fdb2e319449897e6defb3826632a5eaaab84ac5b4539979c4b5e042ea0100000017160014077042bf149434cda92751a2ca017eb35b1ae29cfdffffff08c67897576709ee329620a9d63f0e6f662ddaecceac73e5a8807f9c9fcb88430100000017160014a65e7f101ff5f4f4106603b884d2c631717f61d8fdffffff8daed986c59703a36742a38b9c379afa2489532a876da63e9cd1fe61577c0a740000000000fdffffff030a151e78448b0efec64069c8d11ac9d494635c81c68be60dd8dc5d00b61e66937009f2c7d4ec124a14c9b6e4a2311b4081944d7c493da119117fff8118f09b5c20c703bc52778cf3328c1bccc647e890df68294bcd38632d39a10e53d0757453a6c1b81976a914027aa64642c68f9e3d2d06c3141484ef1704fa8c88ac0b8a04b307c5bb5d383137600d5815347f6fb3f712672ff9c104af1eed3f7b2c9909e02b1ef9d285f8bd1b310f30facc9b9b267f18692ebe675fba02b2eda9febd67025eb7264392705673c88a2dea01a2325354ebb60a5cf045af41351077bb0a0e4917a914df01567680d27699b7d84dafa2bdabb5ada034d5870125b251070e29ca19043cf33ccd7324e2ddab03ecc4ae0b5e77c4fc0e5cf6c95a01000000000000b3740000dd070000000000000000000000000000000000008304000b13f3245f12994306c0732649e27622e0460517da75dae7bf3c3976bb7ab9cbed4719e56eb21b7fb65c2b938b61a6a53f368195b128c3178feec7ddaff3a9cc64e59fe0bfad6cb9506211c945fbba0be14a5457dab662094539b82d80ff2f50a492abaa7fe406810879833f2452651e0f6b51faaa5a0e507293330bfa16ea6283fd4d0b60230000000000000001c2b101c36161ea32674a76fc474e5a45d8bf234dc0dadaf96837b9b3117a63c06128b58861827accd9e5f4f64e61b1efdbc4b7757f7baa8d7072757e4bb65c6045cd70ff8b1ab4b9f1e82283dae8c61651351f56695a752f9f5bcc0ea785fdfce0d49f38c541912bcd55df51dc09cc161b35a0b04e0df400b122d4d4a393dbde8933ea7cea3c5be1caff45f3a760cd51315101d27456da3fb01acbf1af4a53a668ea7e5ebfe456ceef332b3878a21acd717a82557d2d353f39789893403987dadec79a934fd4d62418bcf5af3c9fb4250f9ae1456de1b110f25e9c713fc26470bd0acf2095799e3a73d91e4e187b30731d062ac917f6261df0c58e579a075019a8cb1e88f95fe509456510009ef5e0f9502ac426c264806f5bf1374ba826677b8b1b06fd00c060cf130bf1943a0a85d7b9993d937a1321aed730c13cc9a22316e57a526b0f5ec2e4f1355ef1d4820afa35d9d2c441fe128da31ca8f18b27c4e6fc5d73bf68165aaa4ad066aefee3cd342b85ec8b806433e58b49f300340b140bb042c63959f52a0b68155318f38065521c1564bbc80306fdbabd70c0805cebdd50e3f78ff5d55b53be8443bac0013ea25c0d1e5b9cf4edeaee946a266424c5ce45099b5f14ec0ec9ae9d1fafbdbdfcb608e3b3d6bc26ceb1e32d9535817f0ef1e90dbbd6013e94686fd846d50fc9b9312bd3016d81c0513c4c09078b3199f799dc1791f4e02e635de7295ed0e8bcf9971fa0cdb7067f71fe6226e6c5d6c9911fc5865a74a05d9f596c29e9ca2be83663f0ff1e41a5ec8ddc01c667026eb778c6b1f0d4f3d2dc0c937841238d8ded60a3f4a012a859b66d40bcd260acc6068bad56c8c3834ba4d1b4f778213d2d8365fa068b86613e3da1105123efc4b5ba934f532f5b6cb4af2e4828a1a560d3cd720eeef41f0fda6c466a6993c65ab9223cd3f0a09d99a6d7bac4974a586ea0bad4bc646c456e7df4ef84dda95943784cef2710653a9bdeacedbb7a541db754ea6a15ee9d38eac49a1c9e1ad1f38f4ebaa782da4b1e6fde7b66cd81ab60c002080dea754dabb296d4408e926a45f7f1301d551432d8c8e1294fdd307d9ee2a77e395e3684bae5a9b5afe76a389d67ee0913ef8832967955c0434fef85270411f5c60379512231c428bc03cdec09f0334659ebae151e2f7449f538b37c8c758fed826ab10da63a136938f12e8ad9c3cb76489c0610b8c1908820cc8dda284dda57e1e4c1e4f16c904aae0fc491019a273037d036ee0aa88cb43b4682fa8026933200ead8e0ad1792b2a9f2e177a58321f97ca0db36b6e1ce2661608b8d0bb654d43f676678ab2994436cac1f4c593abf0f11bc5dd737e2d645a6e92ecf7597b139268e5c565e24e7e0987358ae46900df03bc971b7a856181d551c149d6750459c56f1905799ef0552e0148418898839f5ed3167ec2fc1e0184514cd243c830e9092c6cb66a51f45d6a12b363adbc669b7aadf4d0b5cc2bf9922d6c1364dfdaca171fb6c3fd001fd1b053f42a24e9776a75daba64701a7248da6c7d4719c069364f86cb2f364b3f4e8fab3452545a24764bad1db29bc271bb8869c4dbf031f3786a12339ee562451c2b66d1af7ddbd7d65f6d06fafa06422be1312d144c1bc649e98fe665d0e5842cca596ea3a14a85d230da00e1f2880dfcd482535a617b4eb9d762d6771a90420bea9fbeb211e8794bbc77d353287a40c0b17ad86964a96318831fc815d92c5e6dd89968931f7487c86415cc1d600aa27bd164e3c4d0a15c01a7377d5a85ca3262f4462b945414bb9e4714f28fa6c8b0e57db2cf29b012a1730e976edb2f27ac24386bae63cd6022425fd8c2453aa6cd98e401ed8cdd6e86836d917c2bdd3e66b7c71c8a1628587b46c710df92fe74e16711179bc8eb19bae77cde2dc680fb2d862b74bba838621de9573c0b9875578441eb65e266b82db937e092e5260a42ac07659f7100aae03ee02977743a6249c685232851768a022fe95b7f35fa080d81f7de31a456d1a01d252b4e8860b48cfda15080616ce9f628c9ca670225102315ce92da545ddabce9943f257ff61f6ffc220840f1b29a64a8eada361386cdebc03d146f7682149e7bfcb3338430da40226714ff35a31aff60bdac47a0816806a1de48eeaaffbab88e82cc643d4ceb527f5c94bce1659bb56301460bb718f3ead9323345a7ecec1b8f0fd6297ef7d3850430fa8b90e75d45db87a265d3457fbefd6951c7d628496f06e75373c96f2eb97809b10c0143809cc28174b0a949f4dc8df6e6eb22896f26e5331e6ab66d7e0bc0347da39f0e7a3bd2c3369538bdea29a7d76461832dee3fe593bd598cb51a8b1966b135e3a84b8f4ec9a174b2be7cd0f6315d4ca03a958d2e88c1be4e19fc517111cd2a90c4fffc76583e6c361f5fabac6f0aac54cccaa02015e5cdb7f99b84b8026f5667c5f9b57a4f57cad899b7be8ce66b817a0483091378a8db14579cf1e9568ce2c4af3ed872193f449fe05bac79a7ea92743ffa72cf212f8da9679f733c89fee4b78dfe66a982c79cdd16e97b3a316f3191542a022a517f49566edb5bdbafe8a63fd844d5f6763e42e46bdc2c7de22705b8644253397c17480dc0c9484909ae99f3c5038ace18bddce9be9cae3b86312af4ec937a5e05e1a119a2dcc57391d291b25ffe3dff7da88451e5b9270ce7bac6086197a8a2796bf953b3b0fe70b1a53035ee560d302a9ab55f2926e7f888086367c9cf469d4b0a276525e567f73a7b55b48f0ddb36e1a7c1b97d2ee008e61c2bc635e6db9585b01dca600d81b044bb1af230c9ed6e658eb8c3c4b35e1d9773295ec57eec12ebf7da3135beb7e7de0ca086f254770ee0df3341271c6df96b10b3569fc479fbfe1cce62504612d38e30fc7b70fb22aab65ab178fbb095e04767a0198e3dcb6dd6aa04e538453d9e933e8c56bb16078ed85490963fb99ec7279b5d46618d2e596c5c057367d6ef3dfde0f6e61d9d0c9ec076c6ee1fe588463cb596921b5f130b278c1ba13ebec24cda89b4d8b25ac475c34de025b4a04a319367a5745a9963095a9f560e57b61c442a3b02c46302d2f8464966b4fdb28caa592da61695f292f76f69c3120ddd9308f6cc13c2008eee3ea874914cb1a8357949829fd8afd68ca1f854393d6aeb26bfc130d1e6c62d45b0ef5eb6700b7c0f5aa066ab985a825b65212affcc901278fe79f3b6cb45015f0010e92fb190758a64729a8b46f299564eb59a2ddb03057d97843723318f6ddf0f7e9254235dc49318dac7c028df6f680491ea1ab28a820a95d35f1937a9f035bb6cfb56050989900ef809fd55961d10832c2abd79e565d80131908a84d53f8a51752ed00abbb9ae41e6ffecc93fabc19373effa546cdf715c26456238f06651bdf4ffda9c0d77ada10354030480301f09ec383db5bca36deb002c482202e574e63cb5156d09bf511fae0a72125bb61d3efd2197f84e029558a3dd1cde325df7f639d4b030373807ca64fb49100ae21c632fca67ce4821132bd5c488517b5ef45a76013dd41d4449fd585bb128ee9eef39ceef57854e52a9cd61fd5122d8a249454e4f6cae75f37f616b20b5b340c789e5599541af4778d85efaa849e25b5224029179026245d5ced747e6871fa5a200898fedbae2954c452e267ab2a001b3ebd5920390e79687af5c012f53479c37a57897e578cf0e1fc3b4f038a84d72b3ca5363257a48919d74c606b1ed4361c5a1f7610ad70b5ee40f248a6c6822880e8ed4901b6ec0bca46aff4623273d256078701af941f4925df504961632c7e427c0dfd8607c5d1acc55c2f30ead6ea485512965f2a1d6015e6c4cf71453df81f70ced854eb0b23839223c5e18d1b63bbef7d79bc49a720e4fc2fe06e459d81340dc36f186c353295eb548838bfdd22876e34e2ea3170bb09104b77640e89dbd1048e6685df2d46bbc8ef682157d91f518d6ea083c649dbfeb23956164407b325424850ee2c0a98e6d2c3f19a1aeb4a5e6c38cf6792022ea6d5116671a1dee15006f2bbcb45b2f335e803c04d04fe089aae6c2151d30cd8e7d58304000ec0e7f42f42e6df0b144f11c0d5a3f6ad7cf84936e9d5a38fa266c3f7a742d4011f95f54553243fb35972ec66b650d781dad6ff672cbd8a935bd477b838595380ffe1586bf1bcee43e0ff18d252263250e952d39b0f215ebb69cf0c23a6b1a1d2b686c01c5d79fc5b486a88afbe466b4d4bc8e559b0b1f6432c98d8e0844cafecfd4d0b6023000000000000000111cb01acd74ab4034d001feb7cd326bb4f078d678ff1f7d281d3b1262864f97e6dbd42fcc90e63b1bfeacb96f2ee0b5889a2c1550467af8f940791eb2af806628252ff52a77cb2a48cc7b8ecfb0cf6db934e90d812bb4d51c73f84b32a7e9c481f89d56f24a2e7d36b2f6932f2ffc8eea9f9798f25f6f03e51729f4164171ce6475a9d0b9da3fa04922745ceaae12327bd7e40ff22f17d01dc4a6148674c4fcff4a3974fc48b8fb924f0545392d1b329301fba3a5e71fb541e8be0a96ad79bd2cd586bc43fecbc705e61240b58a21dbe0d2e0f0dab734ac0be7655e08917fa62fec78cda4cc83c50e8ae5089f94e14bf70f6f1698c23e914003f3633ef257cfe997327cce8bdf7f2adff2070785e651334e5a0ca97d5e8528e0b47e88570e152a18026c99658cc3350174bc83d887192fdf6221c938f25d09d64bddf98f4313c4f67909e9aeef61a633e2c708a1dc5f995882a780eb93f9db0bd97470b77fb61b0263abdc6232b654da092bbfe22f486b90a7c5898f83ec595a003a3ea66c4288cc6e529262d6226675a7009dd1bd762491a509225651c356b60ffdd1f36a6cc7c24ee719b941f02f2a88406e93d0d0b81c75ba33ce4821f7009c21274af21a5f61fd105b25cfa857df61b2cdd29ce0eeeec86faa52ead261dd3b1326d52d96945e8b15974d2c4be711813f7130234dafbb7715cc4dd8ed69d3c4ca62f0bf448bbf8a32e52df9334a094bfa9f577eab017de57bbbe028fc08014a2f3ee55d9df9cadff760dcab6d06257c814c031bb11c836a8363f261982a7ce73392f82fcb6bdd5968835f657831e7dd1b0252a2112a45c403bf2975bebabfaccc365eb09bb25abfc3b80797cb2058268b9899ba642570d17ce05b8d9a304eaeb4a61b6fec6dc189f54f3d337df46918a76bc7f1ce9846ede724eaef34d4f02b6da0a293aa3a60b306b7c937bc2fd11f58d08440287b0c7793649ecb23b69181af9912f83afb283af17f48f05c79fad79b2aafadb64309a29ee3d08521f5b08d075eca726e2731c6cf85209c9341ee65878b7f86d85503b96c115f8cd603dfebfd24d9d6fb1df2cd92a32bfbb5eb926c280f902cb5bff0ca683a64a5b215fc2f377995363f42080bf96d953153a640762f91a4e68fff1c62c382eba70301dea1823345c9b5e52c5e2488881a76f5aa29f30a24fbc720ef24e1811640e44d0e92fafffde3bb7c3c8c8e813569d89604afbd7e09bd99418dbe31457a33d528924fa51291b62be9e8bb4fea91578296d1ffda0eaf0549347010e64181271b52036613153b61b8e83ea1aeff860ecf4af953639ff008a92a8c2e15d846ac75da55c1732277273ba070235f40d60554181f17b17b05158260e62b0f3cb7a11fb92e20dea7eda89ded7a69b16a359cfe9b3f334d369def50b1700d250bf003cfe23af5a2ee44b7ec2109650c1d88b33edc4f9404cdec8916f84366b36a79e05b4083f201ac59e688dd1ba03e2ea48fd6a42f8ae524a48b6ebb4b4326c667ec75f79e88af078ca667caacc06af34ca42eacabf441dff6fd80a8d6daa89e184d80e9516df2544a539723c9ed18cbaecbc94557e3a31b6ca5b0ecd15d1839c0f193405f2a461e7a520f414d056864f0ff6591d6f0bac63746f9887b5cad36b208160fe008d4eae56384019f92de1ecc2ca53be8055c900ad94521440bd99646f365ef944b6caf92e2614454ae032cb52e59bdc4aff6710a978a4bb6c3ff4eaa478f6a40c96fdb26268cbb422b6ee7d6fda354de6ca7cde33456762a04db65389ff0c10f2852158b543abd9c26e0f1bd0ae04f7e40452f9b4d5d5aab0947856ba73202a5ac577b5a9a329299849b4304436289f271b8bb8f7da91f9c9d4d4fb2d707c3b0ccbf1820c4723ad5a6a1e4d79dbd62ab086ef409c3fc8b5e11bc7e8aa187b7d576d50fbfb755f1dbb8fa7658d52da16a8a7575747a841b52ad6d89c53bfc8afaf0f4d3041c774e05b40bbbaa4fccffa93054efa769d1566313c7f840a0ef9b91c45ac79f1b5e12c866157e4203f8860af2fcb26d2ff665d8e4e2baec6d7c05d5ba45cbcdb0c270b7e0b992e54ceeed41397a11d8478ced4f98ed161e335c4133372d1e4bcbcfa87fdb3fb0930cd99e4af6ac732ea164a328ee5445c374b59e9dfcd665399ed8d795b53dfca7059eb1364eac50d4a8e85ea72803069979843ebdb49436a3db93144b0438f0b15cf1d30b171a3352a6d3b2007fd7a4b97c62a20dd801944cd1e1e924a887fdb41fadee7a571a2d824f3c3925a8f32faa7308190c77b7da7516c81d2ba0302c4eb0763442b7873d85530080ec023aebc7191336c420db7b9c066eb6067f66fa2f6b03a510a1ccf301585aba7ba442b7f9be6182bf60db46a3e2ff380b350b7cf1432f530ce7d3503fd34195f74ee3292a6db28943d0362ee7de7f3bfc7103f074ef0bdb5c5741ed54ff27a11f1159fc2ec4334bad8050464df238403fb0b5d814f65fed6a6ed76bb11868abcf36509fafa05b04fbb80185a58022d87badafe4c09cc475566b4b883558f459c97652eb82510b07a1ad530342573d36722c35e3a3fecdb616a72942e1e889f7fbbed4c7f2fa30c1a1bdfb92fa55e047d7d51f89633f21fdd74315731ccf0c1ca647135cbea7db9a62f4b3cb43a92b1b8b03e910d2e709c9c8a0fe25041f72120a51a2c9ab4155c01685132a9cfb62718a483fdcfb87bfb705dd5a85b10c8bba2c4df81a63d10480812f42a2a2d57ef71c6b685f0faa79d08d01ffab71037f0276b5b966756532470d115d63e6bcca2e420b04f628017b022c95cb991663b85be23fa70014bea539bc02c033711fa3ac026813f233941917507b1bd2dad073668f92e8697834ac12fcf6da847535afa019726fbe5acfbb3df32f231322b11069a46d0e883d62ca1766d1f91063be7e2e55d2a875ff5a24bef810bf068edd93863bb21a03d478d8bf78ac902614aeb19a8be6eb82fff7db2588d93cbe55628b1abcfce379b9cebadd718f0cb96b72f859bc7e944777f182c0ad160d912b5e43cf3b7ae27b894a4b4f0c704ee9fbfb87e63b9d812a8277634e8e0960d0c736230d78d91d7268129562a5fc7674321833fc1085e826dec7bb6c7d98172302ea2abe016f0e38d75e0fb895f364378a5d8479879dba16d6248c6decf84e0ed58dcdc13c0b2c96fadd15cbc5bf92a6268bf1672b339581e90259accd515f66eb207ee962bab98179ea5d0438b40bbac52b248ef293d13beb22cdc081a1c7bbbbb4c0060e6ad1240ff46c1d43450839626365e768c3b8510e31c76a6166e023dad31077e9f6afea1dc9686454f25a60eb6cf7b999c51a005171ed8435588451de5f6ab831ede004f7e6be1d807cb45e45b82298764657ea17692c6198b27f5f93ece31297443dbed66bbe878fc7b2cecf11cfdab409c764e77015fb5bf8dc5add2986569e225f1890ef28c519e90f028e453b51a48d7896c0804c3297241d415bccd7e7cee0a859fc648635652e033311e59c70f08d588cb1fd1cd8cf0ed261d47d9cca08b777fe6e4d02ea586369a9f1e3f5dcfece64066eb3ce15f7c1e1fba956ed4aa3f9e9bec7e27f1dac19f4af425030a56ee318f7199959f26ab641de25ba563a2d1159528d4329ce84ce3bdff56306f7437f552e15a90072cd4b5f785fcca79b6b57e5adb7dfb5a957c8c9c42a44770336c0e6f57a584af3ecc0c08526ef33a044f14610decde7aa82741a764290f6cb9940152663aa118bd0fff7341b1e336823ccb6815f9404e3f4874c99e0d335d814516b8cf75a7931c5b344ab27366c08482b17b4374fbd4a61385143fbbec73845157ea9894472b0613f0892087fc7eb2cc92b3e47100ef3b2b404ad9dae0c447c53d7fa1907f2dca4c106ee66a4bd6a5136b1e303e8a50827b8a9a65c551a84bee01de37c30cb3e00073c641da19a3f405e94f22ff7ab713d9f3d94dd7a2831291db9b886d53ba7430ff863df094262ef8fe8cfae9e6dfc1aef29d7ba0ce49060c829b7f85ddb9138ef76b51df1bf51e1eddfc057895d113bc4cc98d9a964b22a2ca0000",
      "11cff5458ca362dbb540be2226244fd262f3eadaa005eac2a367b70d136413e71c9981f607a5eb7f1283d26d2860495fafdad53c5699944fe8b01dadb4aa0f7d",
      kCfdP2pkh,
      "0331e9b0c6b7f3798bb1b5a6b90c5e2e27c2906cbfd063a3c97b6031ee062ef745",
      nullptr,
      "ea42e0b5c4799953b4c54ab8aaeaa5326682b3ef6d7e894994312edb4f0dae37", 1,
      kCfdSigHashAll, false, int64_t{0}, nullptr);
  EXPECT_EQ(kCfdSuccess, ret);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

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
    EXPECT_EQ(2570, tx_fee_amount);
    EXPECT_EQ(1, append_txout_count);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0100000002fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff0a9a33750a810cd384ca5d93b09513f1eb5d93c669091b29eef710d2391ff7300000000000feffffff02202cb20600000000160014c6598809d09edaacb8f4f4d5b9b81e4413a57243be0da804000000001600146cd31ad8b8552934f4bd9c8cf84a93cbd7a49de111000000", output_tx_hex);
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
    EXPECT_EQ(109860, tx_fee_amount);
    EXPECT_EQ(3, append_txout_count);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("010000000006fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000feffffff0a9a33750a810cd384ca5d93b09513f1eb5d93c669091b29eef710d2391ff7300000000000feffffff0ad4a335556c64c3e2599c3a4c3ddff5b28f616fa55cf2323d2ae642eef74a8f0000000000feffffff030b0000000000000000000000000000000000000000000000000000000000000000000000feffffff020b0000000000000000000000000000000000000000000000000000000000000000000000feffffff010c0000000000000000000000000000000000000000000000000000000000000000000000feffffff050100000000000000000000000000000000000000000000000000000000000000aa010000000006b22c2000160014c6598809d09edaacb8f4f4d5b9b81e4413a572430100000000000000000000000000000000000000000000000000000000000000aa01000000000001ad2400000100000000000000000000000000000000000000000000000000000000000000bb010000000014b18c12001600148aff8eea7bef9ec60d35d7034b2e48e180e93c5d0100000000000000000000000000000000000000000000000000000000000000cc0100000000023e8eb800160014799a8d3f11251b6a6df4ba156a28dd64ad969a910100000000000000000000000000000000000000000000000000000000000000aa010000000006fa7688001600146cd31ad8b8552934f4bd9c8cf84a93cbd7a49de111000000", output_tx_hex);
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
