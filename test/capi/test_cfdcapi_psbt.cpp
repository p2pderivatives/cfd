#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfd/cfd_utxo.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_psbt.h"
#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_transaction.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_util.h"

using cfd::core::HashUtil;
using cfd::core::StringUtil;
using cfd::core::Privkey;
using cfd::core::CryptoUtil;
using cfd::core::SigHashType;

/**
 * @brief testing class.
 */
class cfdcapi_psbt : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

struct CfdUtxo {
  const char* txid;
  uint32_t vout;
  int64_t amount;
  const char* descriptor;
  const char* full_utxo_tx;
};

static void CfdcapiPsbtDumpLog(int result, void* handle) {
  if (result != kCfdSuccess) {
    char* output = nullptr;
    int ret = CfdGetLastErrorMessage(handle, &output);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("", output);
    }
  }
}

TEST(cfdcapi_psbt, CreatePsbt) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAP0+AQIAAAAG+DTafLXhg/yBX3hGInKEZg+koKWDviv7SlNb9CXlMd4BAAAAAP////9a72o7znYklRrluejAkI6P90ch7t96CZTVvx78fe6CgQIAAAAA/////6kt1klSeJ79FESTjpzlsQbnKACJxNxKeLPafHoradk/AwAAAAD//////Pcse7iIGUWVXkj1TmCBtRkBg2SE5cSKmSUGECnEBdEEAAAAAP////+gvhpNmiLIMtLSCjoITwENCamrQfs+Le29RusBBFePuQUAAAAA/////3GhQb9bZT4sQ3czBdaajJh2lERwyh4p5LA6CVPEvraVAQAAAAD/////AoDw+gIAAAAAFgAUsyK93OYzuFGsc3CrRU8LNnoGVOWA8PoCAAAAABYAFMq4xTpuj8ApbRzTkVowfVHEkaVVAAAAAAABAR+AlpgAAAAAABYAFJYsTgjzNtOvvDQVydNZrhBARwUgIgYCVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv4YKnBHYCwAAIAAAACAAAAAgAEAAAABAAAAAAEBH4CWmAAAAAAAFgAUi/CdYLfjTzSCfY28scdjkKkWyoIiBgInRM+yQ24VYEDsHI/oQtnvmhjLQK1B8xJyU5DDX3vTaxgqcEdgLAAAgAAAAIAAAACAAQAAAAIAAAAAAQEfgJaYAAAAAAAWABTx0+5ngpIl64ksyrAeIvand+fiHiIGAufo3CNvoCQ2lAjSzk2FCASCYavCl7gRYE2gh61x0ThVGCpwR2AsAACAAAAAgAAAAIABAAAAAwAAAAABAR+AlpgAAAAAABYAFBnWX4MosiBtmXB4VmDsDTSAj7B1IgYDPYdL8ZtpfPbGOWWVR6WDuGcwFkpbbbAb8goU65tq20QYKnBHYCwAAIAAAACAAAAAgAEAAAAEAAAAAAEBH4CWmAAAAAAAFgAUEreVSnXvwqIOhuMtwteGR9ZwB3kiBgL7Bhcw2948gGtKF6mfRUyBKCrs7m1G9Kl1qUzf06BlBBgqcEdgLAAAgAAAAIAAAACAAQAAAAUAAAAAAQEfgPD6AgAAAAAWABSXipBGDkRnGlL0mgm7WcxnlLY8iSIGA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rsGJ1rbYYsAACAAAAAgAAAAIABAAAAAQAAAAAiAgNHO/yMdwwbIgoueq5LrfbA1+rykCjVsp00OAErsonvgRgqcEdgLAAAgAAAAIAAAACAAAAAAAIAAAAAIgIDZHSv8mM8NRhlU5+1K2K51vueTiNXZijh8KCnmTRY4GwYnWtthiwAAIAAAACAAAAAgAAAAAACAAAAAA==";
  const char* exp_psbt = "70736274ff0100fd3e010200000006f834da7cb5e183fc815f7846227284660fa4a0a583be2bfb4a535bf425e531de0100000000ffffffff5aef6a3bce7624951ae5b9e8c0908e8ff74721eedf7a0994d5bf1efc7dee82810200000000ffffffffa92dd64952789efd1444938e9ce5b106e7280089c4dc4a78b3da7c7a2b69d93f0300000000fffffffffcf72c7bb8881945955e48f54e6081b51901836484e5c48a9925061029c405d10400000000ffffffffa0be1a4d9a22c832d2d20a3a084f010d09a9ab41fb3e2dedbd46eb0104578fb90500000000ffffffff71a141bf5b653e2c43773305d69a8c9876944470ca1e29e4b03a0953c4beb6950100000000ffffffff0280f0fa0200000000160014b322bddce633b851ac7370ab454f0b367a0654e580f0fa0200000000160014cab8c53a6e8fc0296d1cd3915a307d51c491a555000000000001011f8096980000000000160014962c4e08f336d3afbc3415c9d359ae1040470520220602565248460b3c186decf13db06f0724fd50b47cc3e489c30076c8363d5038cefe182a7047602c000080000000800000008001000000010000000001011f80969800000000001600148bf09d60b7e34f34827d8dbcb1c76390a916ca822206022744cfb2436e156040ec1c8fe842d9ef9a18cb40ad41f312725390c35f7bd36b182a7047602c000080000000800000008001000000020000000001011f8096980000000000160014f1d3ee67829225eb892ccab01e22f6a777e7e21e220602e7e8dc236fa024369408d2ce4d8508048261abc297b811604da087ad71d13855182a7047602c000080000000800000008001000000030000000001011f809698000000000016001419d65f8328b2206d9970785660ec0d34808fb0752206033d874bf19b697cf6c639659547a583b86730164a5b6db01bf20a14eb9b6adb44182a7047602c000080000000800000008001000000040000000001011f809698000000000016001412b7954a75efc2a20e86e32dc2d78647d6700779220602fb061730dbde3c806b4a17a99f454c81282aecee6d46f4a975a94cdfd3a06504182a7047602c000080000000800000008001000000050000000001011f80f0fa0200000000160014978a90460e44671a52f49a09bb59cc6794b63c89220603e3d244a3967e0b87765fda86c5ff38885f74993953b9584388aef30b26af6aec189d6b6d862c0000800000008000000080010000000100000000220203473bfc8c770c1b220a2e7aae4badf6c0d7eaf29028d5b29d3438012bb289ef81182a7047602c00008000000080000000800000000002000000002202036474aff2633c351865539fb52b62b9d6fb9e4e23576628e1f0a0a7993458e06c189d6b6d862c0000800000008000000080000000000200000000";
  char* output_base64 = nullptr;
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "", "", 2, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    int64_t amount1 = 0x989680;
    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "de31e525f45b534afb2bbe83a5a0a40f6684722246785f81fc83e1b57cda34f8", 1,
        0xffffffff, amount1, "",
        "wpkh([2a704760/44h/0h/0h/1/1]02565248460b3c186decf13db06f0724fd50b47cc3e489c30076c8363d5038cefe)",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "8182ee7dfc1ebfd594097adfee2147f78f8e90c0e8b9e51a952476ce3b6aef5a", 2,
        0xffffffff, amount1, "",
        "wpkh([2a704760/44h/0h/0h/1/2]022744cfb2436e156040ec1c8fe842d9ef9a18cb40ad41f312725390c35f7bd36b)",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "3fd9692b7a7cdab3784adcc4890028e706b1e59c8e934414fd9e785249d62da9", 3,
        0xffffffff, amount1, "",
        "wpkh([2a704760/44h/0h/0h/1/3]02e7e8dc236fa024369408d2ce4d8508048261abc297b811604da087ad71d13855)",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "d105c429100625998ac4e58464830119b581604ef5485e95451988b87b2cf7fc", 4,
        0xffffffff, amount1, "",
        "wpkh([2a704760/44h/0h/0h/1/4]033d874bf19b697cf6c639659547a583b86730164a5b6db01bf20a14eb9b6adb44)",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "b98f570401eb46bded2d3efb41aba9090d014f083a0ad2d232c8229a4d1abea0", 5,
        0xffffffff, amount1, "",
        "wpkh([2a704760/44h/0h/0h/1/5]02fb061730dbde3c806b4a17a99f454c81282aecee6d46f4a975a94cdfd3a06504)",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "95b6bec453093ab0e4291eca70449476988c9ad6053377432c3e655bbf41a171", 1,
        0xffffffff, 0x2faf080, "",
        "wpkh([9d6b6d86/44h/0h/0h/1/1]03e3d244a3967e0b87765fda86c5ff38885f74993953b9584388aef30b26af6aec)",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    uint32_t index = 0;
    ret = CfdAddPsbtTxOutWithPubkey(handle, psbt_handle,
        50000000, "",
        "wpkh([2a704760/44h/0h/0h/0/2]03473bfc8c770c1b220a2e7aae4badf6c0d7eaf29028d5b29d3438012bb289ef81)",
        &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);

    ret = CfdAddPsbtTxOutWithPubkey(handle, psbt_handle,
        50000000, "",
        "wpkh([9d6b6d86/44h/0h/0h/0/2]036474aff2633c351865539fb52b62b9d6fb9e4e23576628e1f0a0a7993458e06c)",
        &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);

    ret = CfdGetPsbtData(handle, psbt_handle, &output_base64, &output);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output_base64);
      EXPECT_STREQ(exp_psbt, output);
      CfdFreeStringBuffer(output_base64);
      output_base64 = nullptr;
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, ParsePsbtBase64) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAF4CAAAAAWkv08JB0xYYGWvDa23k9riJEU5CDpvd+cu+Ux5aVE1UAAAAAAD/////ARBLzR0AAAAAIgAgPK0GGd5n5iR6dqECgTY1wFNFfGuk/eSsH/2BSNcOS8wAAAAAAAEBIAhYzR0AAAAAF6kUlF+1A5GnBjfB/8Wrf7ZTCMLyMXWHAQMEAQAAAAEEIgAgnE2ssl67itqLuxrduGnepNgXDMlR8dlpSyFU4VgydskBBUdSIQOlEvX1nA55AfxHjtY1Pu929E+cssGFv7z38Lm7dnFr8yED9NRzYU6VSsT1UY5vfLMwe0+EdD4TftTuy19PtkPCO0dSriIGA6US9fWcDnkB/EeO1jU+73b0T5yywYW/vPfwubt2cWvzGCpwR2AsAACAAAAAgAAAAIAAAAAACwAAACIGA/TUc2FOlUrE9VGOb3yzMHtPhHQ+E37U7stfT7ZDwjtHGJ1rbYYsAACAAAAAgAAAAIAAAAAACwAAAAABACIAIDytBhneZ+YkenahAoE2NcBTRXxrpP3krB/9gUjXDkvMAQFHUiECkG05n2277MiY1LjeP0l0dMhqQfLPNtcfleXKBrB0hnshAmIseXTsMt51r6NzOwmmwz0N6lFLKfqsz7CZF3T0YiJCUq4iAgJiLHl07DLeda+jczsJpsM9DepRSyn6rM+wmRd09GIiQhida22GLAAAgAAAAIAAAACAAAAAAAwAAAAiAgKQbTmfbbvsyJjUuN4/SXR0yGpB8s821x+V5coGsHSGexgqcEdgLAAAgAAAAIAAAACAAAAAAAwAAAAA";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, exp_psbt_base64, "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, ParsePsbtHex) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt = "70736274ff0100fd3e010200000006f834da7cb5e183fc815f7846227284660fa4a0a583be2bfb4a535bf425e531de0100000000ffffffff5aef6a3bce7624951ae5b9e8c0908e8ff74721eedf7a0994d5bf1efc7dee82810200000000ffffffffa92dd64952789efd1444938e9ce5b106e7280089c4dc4a78b3da7c7a2b69d93f0300000000fffffffffcf72c7bb8881945955e48f54e6081b51901836484e5c48a9925061029c405d10400000000ffffffffa0be1a4d9a22c832d2d20a3a084f010d09a9ab41fb3e2dedbd46eb0104578fb90500000000ffffffff71a141bf5b653e2c43773305d69a8c9876944470ca1e29e4b03a0953c4beb6950100000000ffffffff0280f0fa0200000000160014b322bddce633b851ac7370ab454f0b367a0654e580f0fa0200000000160014cab8c53a6e8fc0296d1cd3915a307d51c491a555000000000001011f8096980000000000160014962c4e08f336d3afbc3415c9d359ae1040470520220602565248460b3c186decf13db06f0724fd50b47cc3e489c30076c8363d5038cefe182a7047602c000080000000800000008001000000010000000001011f80969800000000001600148bf09d60b7e34f34827d8dbcb1c76390a916ca822206022744cfb2436e156040ec1c8fe842d9ef9a18cb40ad41f312725390c35f7bd36b182a7047602c000080000000800000008001000000020000000001011f8096980000000000160014f1d3ee67829225eb892ccab01e22f6a777e7e21e220602e7e8dc236fa024369408d2ce4d8508048261abc297b811604da087ad71d13855182a7047602c000080000000800000008001000000030000000001011f809698000000000016001419d65f8328b2206d9970785660ec0d34808fb0752206033d874bf19b697cf6c639659547a583b86730164a5b6db01bf20a14eb9b6adb44182a7047602c000080000000800000008001000000040000000001011f809698000000000016001412b7954a75efc2a20e86e32dc2d78647d6700779220602fb061730dbde3c806b4a17a99f454c81282aecee6d46f4a975a94cdfd3a06504182a7047602c000080000000800000008001000000050000000001011f80f0fa0200000000160014978a90460e44671a52f49a09bb59cc6794b63c89220603e3d244a3967e0b87765fda86c5ff38885f74993953b9584388aef30b26af6aec189d6b6d862c0000800000008000000080010000000100000000220203473bfc8c770c1b220a2e7aae4badf6c0d7eaf29028d5b29d3438012bb289ef81182a7047602c00008000000080000000800000000002000000002202036474aff2633c351865539fb52b62b9d6fb9e4e23576628e1f0a0a7993458e06c189d6b6d862c0000800000008000000080000000000200000000";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, exp_psbt, "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdGetPsbtData(handle, psbt_handle, nullptr, &output);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  CfdcapiPsbtDumpLog(ret, handle);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, ConvertPsbtFromTx) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_tx = "0200000006f834da7cb5e183fc815f7846227284660fa4a0a583be2bfb4a535bf425e531de0100000000ffffffff5aef6a3bce7624951ae5b9e8c0908e8ff74721eedf7a0994d5bf1efc7dee82810200000000ffffffffa92dd64952789efd1444938e9ce5b106e7280089c4dc4a78b3da7c7a2b69d93f0300000000fffffffffcf72c7bb8881945955e48f54e6081b51901836484e5c48a9925061029c405d10400000000ffffffffa0be1a4d9a22c832d2d20a3a084f010d09a9ab41fb3e2dedbd46eb0104578fb90500000000ffffffff71a141bf5b653e2c43773305d69a8c9876944470ca1e29e4b03a0953c4beb6950100000000ffffffff0280f0fa0200000000160014b322bddce633b851ac7370ab454f0b367a0654e580f0fa0200000000160014cab8c53a6e8fc0296d1cd3915a307d51c491a55500000000";
  const char* exp_psbt_base64 = "cHNidP8BAP0+AQIAAAAG+DTafLXhg/yBX3hGInKEZg+koKWDviv7SlNb9CXlMd4BAAAAAP////9a72o7znYklRrluejAkI6P90ch7t96CZTVvx78fe6CgQIAAAAA/////6kt1klSeJ79FESTjpzlsQbnKACJxNxKeLPafHoradk/AwAAAAD//////Pcse7iIGUWVXkj1TmCBtRkBg2SE5cSKmSUGECnEBdEEAAAAAP////+gvhpNmiLIMtLSCjoITwENCamrQfs+Le29RusBBFePuQUAAAAA/////3GhQb9bZT4sQ3czBdaajJh2lERwyh4p5LA6CVPEvraVAQAAAAD/////AoDw+gIAAAAAFgAUsyK93OYzuFGsc3CrRU8LNnoGVOWA8PoCAAAAABYAFMq4xTpuj8ApbRzTkVowfVHEkaVVAAAAAAAAAAAAAAAAAA==";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "", exp_psbt_tx, 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    uint32_t psbt_version = 0;
    uint32_t txin_count = 0;
    uint32_t txout_count = 0;
    ret = CfdGetPsbtGlobalData(handle, psbt_handle, &psbt_version,
        &output, &txin_count, &txout_count);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(0, psbt_version);
      EXPECT_EQ(6, txin_count);
      EXPECT_EQ(2, txout_count);
      EXPECT_STREQ(exp_psbt_tx, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, JoinPsbt) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHAQMEAQAAAAEEFgAUlixOCPM206+8NBXJ01muEEBHBSAiBgJWUkhGCzwYbezxPbBvByT9ULR8w+SJwwB2yDY9UDjO/hgqcEdgLAAAgAAAAIAAAACAAQAAAAEAAAAAAQC/AgAAAAHG0uo24ugCtS3axmXay+0vgxtSY0WeHKc09clF11FeQAAAAABqRzBEAiARuWx9LQ0ujcs3E44YrMRgdSllrdnLh4ffXDSd8OKuZgIgLpOvMbZPUWblYFgZVV2r7Fe+eUMA+ts3BS8x3d6pkFwBIQPj0kSjln4Lh3Zf2obF/ziIX3SZOVO5WEOIrvMLJq9q7P////8BeFHNHQAAAAAZdqkUjSBEOpGWnjvKDiQM0P/k3JjGPeKIrAAAAAAiBgLZ9oiPKFoVpqGICiICwrIwpC13z2LaakispBmCYs2jxxida22GLAAAgAAAAIAAAACAAAAAAAEAAAAAIgIDRzv8jHcMGyIKLnquS632wNfq8pAo1bKdNDgBK7KJ74EYKnBHYCwAAIAAAACAAAAAgAAAAAACAAAAACICA2R0r/JjPDUYZVOftStiudb7nk4jV2Yo4fCgp5k0WOBsGJ1rbYYsAACAAAAAgAAAAIAAAAAAAgAAAAA=";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "cHNidP8BAFwCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8AAAAAAAABAPYCAAAAAAEB8Zk/6OcYlULuRQYljhcCAb4pJwPNJ1rLCezhZnL9hIsAAAAAFxYAFKye+AsnrxydlcHbXXYTGTIrxC/F/////wIIBBAkAQAAABYAFAneKgQxy7NET8IsrZ2aD9CWOXIQAOH1BQAAAAAXqRRQn1mF9OkKFPuQ45MW/bTzrJdTB4cCRzBEAiAeB99yHDMiQZ6PNtB+6uR5WXW6DZ0ZYwyjzT3A1JZxcgIgFUKOe+BrZWdQFTkFC9eRo4DwC73bxQl+qXunvkAXEUoBIQJK70Ox1ax7pQFJmNY86sWDlZ0f3GbqJpnNhO6vgqKDBgAAAAABASAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwEDBAEAAAABBBYAFJYsTgjzNtOvvDQVydNZrhBARwUgIgYCVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv4YKnBHYCwAAIAAAACAAAAAgAEAAAABAAAAAAEAvwIAAAABxtLqNuLoArUt2sZl2svtL4MbUmNFnhynNPXJRddRXkAAAAAAakcwRAIgEblsfS0NLo3LNxOOGKzEYHUpZa3Zy4eH31w0nfDirmYCIC6TrzG2T1Fm5WBYGVVdq+xXvnlDAPrbNwUvMd3eqZBcASED49JEo5Z+C4d2X9qGxf84iF90mTlTuVhDiK7zCyavauz/////AXhRzR0AAAAAGXapFI0gRDqRlp47yg4kDND/5NyYxj3iiKwAAAAAIgYC2faIjyhaFaahiAoiAsKyMKQtd89i2mpIrKQZgmLNo8cYnWtthiwAAIAAAACAAAAAgAAAAAABAAAAAA==", "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdJoinPsbt(handle, psbt_handle, "cHNidP8BAEgCAAAAAAIA4fUFAAAAABYAFLMivdzmM7hRrHNwq0VPCzZ6BlTlAOH1BQAAAAAWABTKuMU6bo/AKW0c05FaMH1RxJGlVQAAAAAAIgIDRzv8jHcMGyIKLnquS632wNfq8pAo1bKdNDgBK7KJ74EYKnBHYCwAAIAAAACAAAAAgAAAAAACAAAAACICA2R0r/JjPDUYZVOftStiudb7nk4jV2Yo4fCgp5k0WOBsGJ1rbYYsAACAAAAAgAAAAIAAAAAAAgAAAAA=");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, SignPsbt) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHIgICVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv5HMEQCICmYYqZ6m0VNbNpf7jSlLZCJv6AkRE2IAxw0qY4pi9vKAiBLR/z1B7gJVBCOA3VnP9vdCExfu5lPbJUXIPLrL2u4ZgEBAwQBAAAAAQQWABSWLE4I8zbTr7w0FcnTWa4QQEcFICIGAlZSSEYLPBht7PE9sG8HJP1QtHzD5InDAHbINj1QOM7+GCpwR2AsAACAAAAAgAAAAIABAAAAAQAAAAABAL8CAAAAAcbS6jbi6AK1LdrGZdrL7S+DG1JjRZ4cpzT1yUXXUV5AAAAAAGpHMEQCIBG5bH0tDS6NyzcTjhisxGB1KWWt2cuHh99cNJ3w4q5mAiAuk68xtk9RZuVgWBlVXavsV755QwD62zcFLzHd3qmQXAEhA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rs/////wF4Uc0dAAAAABl2qRSNIEQ6kZaeO8oOJAzQ/+TcmMY94oisAAAAACIGAtn2iI8oWhWmoYgKIgLCsjCkLXfPYtpqSKykGYJizaPHGJ1rbYYsAACAAAAAgAAAAIAAAAAAAQAAAAAiAgNHO/yMdwwbIgoueq5LrfbA1+rykCjVsp00OAErsonvgRgqcEdgLAAAgAAAAIAAAACAAAAAAAIAAAAAIgIDZHSv8mM8NRhlU5+1K2K51vueTiNXZijh8KCnmTRY4GwYnWtthiwAAIAAAACAAAAAgAAAAAACAAAAAA==";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHAQMEAQAAAAEEFgAUlixOCPM206+8NBXJ01muEEBHBSAiBgJWUkhGCzwYbezxPbBvByT9ULR8w+SJwwB2yDY9UDjO/hgqcEdgLAAAgAAAAIAAAACAAQAAAAEAAAAAAQC/AgAAAAHG0uo24ugCtS3axmXay+0vgxtSY0WeHKc09clF11FeQAAAAABqRzBEAiARuWx9LQ0ujcs3E44YrMRgdSllrdnLh4ffXDSd8OKuZgIgLpOvMbZPUWblYFgZVV2r7Fe+eUMA+ts3BS8x3d6pkFwBIQPj0kSjln4Lh3Zf2obF/ziIX3SZOVO5WEOIrvMLJq9q7P////8BeFHNHQAAAAAZdqkUjSBEOpGWnjvKDiQM0P/k3JjGPeKIrAAAAAAiBgLZ9oiPKFoVpqGICiICwrIwpC13z2LaakispBmCYs2jxxida22GLAAAgAAAAIAAAACAAAAAAAEAAAAAIgIDRzv8jHcMGyIKLnquS632wNfq8pAo1bKdNDgBK7KJ74EYKnBHYCwAAIAAAACAAAAAgAAAAAACAAAAACICA2R0r/JjPDUYZVOftStiudb7nk4jV2Yo4fCgp5k0WOBsGJ1rbYYsAACAAAAAgAAAAIAAAAAAAgAAAAA=", "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdSignPsbt(handle, psbt_handle, "KwNwembMPPQpgFfbhb5WPCgENwhnTaNQjf3a6cBuBiZ993Gu5gaR", true);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, CombinePsbt) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHIgICVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv5HMEQCICmYYqZ6m0VNbNpf7jSlLZCJv6AkRE2IAxw0qY4pi9vKAiBLR/z1B7gJVBCOA3VnP9vdCExfu5lPbJUXIPLrL2u4ZgEBAwQBAAAAAQQWABSWLE4I8zbTr7w0FcnTWa4QQEcFICIGAlZSSEYLPBht7PE9sG8HJP1QtHzD5InDAHbINj1QOM7+GCpwR2AsAACAAAAAgAAAAIABAAAAAQAAAAABAL8CAAAAAcbS6jbi6AK1LdrGZdrL7S+DG1JjRZ4cpzT1yUXXUV5AAAAAAGpHMEQCIBG5bH0tDS6NyzcTjhisxGB1KWWt2cuHh99cNJ3w4q5mAiAuk68xtk9RZuVgWBlVXavsV755QwD62zcFLzHd3qmQXAEhA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rs/////wF4Uc0dAAAAABl2qRSNIEQ6kZaeO8oOJAzQ/+TcmMY94oisAAAAACICAtn2iI8oWhWmoYgKIgLCsjCkLXfPYtpqSKykGYJizaPHRzBEAiBm7JVulng81hIsnAdzKjrpkxtpq9r/HzYsKO1fqWco0wIgahbUrkb+1I09BJJzdhO9m1H+PiRTtERuRHxlvyyWfhABIgYC2faIjyhaFaahiAoiAsKyMKQtd89i2mpIrKQZgmLNo8cYnWtthiwAAIAAAACAAAAAgAAAAAABAAAAACICA0c7/Ix3DBsiCi56rkut9sDX6vKQKNWynTQ4ASuyie+BGCpwR2AsAACAAAAAgAAAAIAAAAAAAgAAAAAiAgNkdK/yYzw1GGVTn7UrYrnW+55OI1dmKOHwoKeZNFjgbBida22GLAAAgAAAAIAAAACAAAAAAAIAAAAA";
  char* output = nullptr;
  // tx: 0200000002267ffd76eae6b6c13ffb86ceabaa24d42485520233c2c3805e0ad764709578c00100000000ffffffff7d25462a42ff2cb1ba5401e5893259db3ca4bcf5cf6ab9715d35163d31c9ecc00000000000ffffffff0200e1f50500000000160014b322bddce633b851ac7370ab454f0b367a0654e500e1f50500000000160014cab8c53a6e8fc0296d1cd3915a307d51c491a55500000000
  ret = CfdCreatePsbtHandle(
    handle, net_type, "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHIgICVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv5HMEQCICmYYqZ6m0VNbNpf7jSlLZCJv6AkRE2IAxw0qY4pi9vKAiBLR/z1B7gJVBCOA3VnP9vdCExfu5lPbJUXIPLrL2u4ZgEBAwQBAAAAAQQWABSWLE4I8zbTr7w0FcnTWa4QQEcFICIGAlZSSEYLPBht7PE9sG8HJP1QtHzD5InDAHbINj1QOM7+GCpwR2AsAACAAAAAgAAAAIABAAAAAQAAAAABAL8CAAAAAcbS6jbi6AK1LdrGZdrL7S+DG1JjRZ4cpzT1yUXXUV5AAAAAAGpHMEQCIBG5bH0tDS6NyzcTjhisxGB1KWWt2cuHh99cNJ3w4q5mAiAuk68xtk9RZuVgWBlVXavsV755QwD62zcFLzHd3qmQXAEhA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rs/////wF4Uc0dAAAAABl2qRSNIEQ6kZaeO8oOJAzQ/+TcmMY94oisAAAAACIGAtn2iI8oWhWmoYgKIgLCsjCkLXfPYtpqSKykGYJizaPHGJ1rbYYsAACAAAAAgAAAAIAAAAAAAQAAAAAiAgNHO/yMdwwbIgoueq5LrfbA1+rykCjVsp00OAErsonvgRgqcEdgLAAAgAAAAIAAAACAAAAAAAIAAAAAIgIDZHSv8mM8NRhlU5+1K2K51vueTiNXZijh8KCnmTRY4GwYnWtthiwAAIAAAACAAAAAgAAAAAACAAAAAA==", "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdCombinePsbt(handle, psbt_handle, "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHAQMEAQAAAAEEFgAUlixOCPM206+8NBXJ01muEEBHBSAiBgJWUkhGCzwYbezxPbBvByT9ULR8w+SJwwB2yDY9UDjO/hgqcEdgLAAAgAAAAIAAAACAAQAAAAEAAAAAAQC/AgAAAAHG0uo24ugCtS3axmXay+0vgxtSY0WeHKc09clF11FeQAAAAABqRzBEAiARuWx9LQ0ujcs3E44YrMRgdSllrdnLh4ffXDSd8OKuZgIgLpOvMbZPUWblYFgZVV2r7Fe+eUMA+ts3BS8x3d6pkFwBIQPj0kSjln4Lh3Zf2obF/ziIX3SZOVO5WEOIrvMLJq9q7P////8BeFHNHQAAAAAZdqkUjSBEOpGWnjvKDiQM0P/k3JjGPeKIrAAAAAAiAgLZ9oiPKFoVpqGICiICwrIwpC13z2LaakispBmCYs2jx0cwRAIgZuyVbpZ4PNYSLJwHcyo66ZMbaava/x82LCjtX6lnKNMCIGoW1K5G/tSNPQSSc3YTvZtR/j4kU7REbkR8Zb8sln4QASIGAtn2iI8oWhWmoYgKIgLCsjCkLXfPYtpqSKykGYJizaPHGJ1rbYYsAACAAAAAgAAAAIAAAAAAAQAAAAAiAgNHO/yMdwwbIgoueq5LrfbA1+rykCjVsp00OAErsonvgRgqcEdgLAAAgAAAAIAAAACAAAAAAAIAAAAAIgIDZHSv8mM8NRhlU5+1K2K51vueTiNXZijh8KCnmTRY4GwYnWtthiwAAIAAAACAAAAAgAAAAAACAAAAAA==");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, FinalizePsbt) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHAQcXFgAUlixOCPM206+8NBXJ01muEEBHBSABCGsCRzBEAiApmGKmeptFTWzaX+40pS2Qib+gJERNiAMcNKmOKYvbygIgS0f89Qe4CVQQjgN1Zz/b3QhMX7uZT2yVFyDy6y9ruGYBIQJWUkhGCzwYbezxPbBvByT9ULR8w+SJwwB2yDY9UDjO/gABAL8CAAAAAcbS6jbi6AK1LdrGZdrL7S+DG1JjRZ4cpzT1yUXXUV5AAAAAAGpHMEQCIBG5bH0tDS6NyzcTjhisxGB1KWWt2cuHh99cNJ3w4q5mAiAuk68xtk9RZuVgWBlVXavsV755QwD62zcFLzHd3qmQXAEhA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rs/////wF4Uc0dAAAAABl2qRSNIEQ6kZaeO8oOJAzQ/+TcmMY94oisAAAAAAEHakcwRAIgZuyVbpZ4PNYSLJwHcyo66ZMbaava/x82LCjtX6lnKNMCIGoW1K5G/tSNPQSSc3YTvZtR/j4kU7REbkR8Zb8sln4QASEC2faIjyhaFaahiAoiAsKyMKQtd89i2mpIrKQZgmLNo8cAIgIDRzv8jHcMGyIKLnquS632wNfq8pAo1bKdNDgBK7KJ74EYKnBHYCwAAIAAAACAAAAAgAAAAAACAAAAACICA2R0r/JjPDUYZVOftStiudb7nk4jV2Yo4fCgp5k0WOBsGJ1rbYYsAACAAAAAgAAAAIAAAAAAAgAAAAA=";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHIgICVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv5HMEQCICmYYqZ6m0VNbNpf7jSlLZCJv6AkRE2IAxw0qY4pi9vKAiBLR/z1B7gJVBCOA3VnP9vdCExfu5lPbJUXIPLrL2u4ZgEBAwQBAAAAAQQWABSWLE4I8zbTr7w0FcnTWa4QQEcFICIGAlZSSEYLPBht7PE9sG8HJP1QtHzD5InDAHbINj1QOM7+GCpwR2AsAACAAAAAgAAAAIABAAAAAQAAAAABAL8CAAAAAcbS6jbi6AK1LdrGZdrL7S+DG1JjRZ4cpzT1yUXXUV5AAAAAAGpHMEQCIBG5bH0tDS6NyzcTjhisxGB1KWWt2cuHh99cNJ3w4q5mAiAuk68xtk9RZuVgWBlVXavsV755QwD62zcFLzHd3qmQXAEhA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rs/////wF4Uc0dAAAAABl2qRSNIEQ6kZaeO8oOJAzQ/+TcmMY94oisAAAAACICAtn2iI8oWhWmoYgKIgLCsjCkLXfPYtpqSKykGYJizaPHRzBEAiBm7JVulng81hIsnAdzKjrpkxtpq9r/HzYsKO1fqWco0wIgahbUrkb+1I09BJJzdhO9m1H+PiRTtERuRHxlvyyWfhABIgYC2faIjyhaFaahiAoiAsKyMKQtd89i2mpIrKQZgmLNo8cYnWtthiwAAIAAAACAAAAAgAAAAAABAAAAACICA0c7/Ix3DBsiCi56rkut9sDX6vKQKNWynTQ4ASuyie+BGCpwR2AsAACAAAAAgAAAAIAAAAAAAgAAAAAiAgNkdK/yYzw1GGVTn7UrYrnW+55OI1dmKOHwoKeZNFjgbBida22GLAAAgAAAAIAAAACAAAAAAAIAAAAA", "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdIsFinalizedPsbt(handle, psbt_handle);
    EXPECT_EQ(kCfdSignVerificationError, ret);

    ret = CfdIsFinalizedPsbtInput(handle, psbt_handle, "c078957064d70a5e80c3c23302528524d424aaabce86fb3fc1b6e6ea76fd7f26", 1);
    EXPECT_EQ(kCfdSignVerificationError, ret);

    ret = CfdIsFinalizedPsbtInput(handle, psbt_handle, "c0ecc9313d16355d71b96acff5bca43cdb593289e50154bab12cff422a46257d", 0);
    EXPECT_EQ(kCfdSignVerificationError, ret);

    ret = CfdVerifyPsbtTxIn(handle, psbt_handle, "c078957064d70a5e80c3c23302528524d424aaabce86fb3fc1b6e6ea76fd7f26", 1);
    EXPECT_EQ(kCfdSignVerificationError, ret);

    ret = CfdFinalizePsbt(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdIsFinalizedPsbt(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdIsFinalizedPsbtInput(handle, psbt_handle, "c078957064d70a5e80c3c23302528524d424aaabce86fb3fc1b6e6ea76fd7f26", 1);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdIsFinalizedPsbtInput(handle, psbt_handle, "c0ecc9313d16355d71b96acff5bca43cdb593289e50154bab12cff422a46257d", 0);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdVerifyPsbtTxIn(handle, psbt_handle, "c078957064d70a5e80c3c23302528524d424aaabce86fb3fc1b6e6ea76fd7f26", 1);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, ExtractPsbtTransaction) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_tx = "02000000000102267ffd76eae6b6c13ffb86ceabaa24d42485520233c2c3805e0ad764709578c00100000017160014962c4e08f336d3afbc3415c9d359ae1040470520ffffffff7d25462a42ff2cb1ba5401e5893259db3ca4bcf5cf6ab9715d35163d31c9ecc0000000006a473044022066ec956e96783cd6122c9c07732a3ae9931b69abdaff1f362c28ed5fa96728d302206a16d4ae46fed48d3d0492737613bd9b51fe3e2453b4446e447c65bf2c967e10012102d9f6888f285a15a6a1880a2202c2b230a42d77cf62da6a48aca4198262cda3c7ffffffff0200e1f50500000000160014b322bddce633b851ac7370ab454f0b367a0654e500e1f50500000000160014cab8c53a6e8fc0296d1cd3915a307d51c491a555024730440220299862a67a9b454d6cda5fee34a52d9089bfa024444d88031c34a98e298bdbca02204b47fcf507b80954108e0375673fdbdd084c5fbb994f6c951720f2eb2f6bb866012102565248460b3c186decf13db06f0724fd50b47cc3e489c30076c8363d5038cefe0000000000";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "cHNidP8BAJoCAAAAAiZ//Xbq5rbBP/uGzquqJNQkhVICM8LDgF4K12RwlXjAAQAAAAD/////fSVGKkL/LLG6VAHliTJZ2zykvPXParlxXTUWPTHJ7MAAAAAAAP////8CAOH1BQAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5QDh9QUAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVUAAAAAAAEA9gIAAAAAAQHxmT/o5xiVQu5FBiWOFwIBviknA80nWssJ7OFmcv2EiwAAAAAXFgAUrJ74CyevHJ2VwdtddhMZMivEL8X/////AggEECQBAAAAFgAUCd4qBDHLs0RPwiytnZoP0JY5chAA4fUFAAAAABepFFCfWYX06QoU+5Djkxb9tPOsl1MHhwJHMEQCIB4H33IcMyJBno820H7q5HlZdboNnRljDKPNPcDUlnFyAiAVQo574GtlZ1AVOQUL15GjgPALvdvFCX6pe6e+QBcRSgEhAkrvQ7HVrHulAUmY1jzqxYOVnR/cZuommc2E7q+CooMGAAAAAAEBIADh9QUAAAAAF6kUUJ9ZhfTpChT7kOOTFv2086yXUweHAQcXFgAUlixOCPM206+8NBXJ01muEEBHBSABCGsCRzBEAiApmGKmeptFTWzaX+40pS2Qib+gJERNiAMcNKmOKYvbygIgS0f89Qe4CVQQjgN1Zz/b3QhMX7uZT2yVFyDy6y9ruGYBIQJWUkhGCzwYbezxPbBvByT9ULR8w+SJwwB2yDY9UDjO/gABAL8CAAAAAcbS6jbi6AK1LdrGZdrL7S+DG1JjRZ4cpzT1yUXXUV5AAAAAAGpHMEQCIBG5bH0tDS6NyzcTjhisxGB1KWWt2cuHh99cNJ3w4q5mAiAuk68xtk9RZuVgWBlVXavsV755QwD62zcFLzHd3qmQXAEhA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rs/////wF4Uc0dAAAAABl2qRSNIEQ6kZaeO8oOJAzQ/+TcmMY94oisAAAAAAEHakcwRAIgZuyVbpZ4PNYSLJwHcyo66ZMbaava/x82LCjtX6lnKNMCIGoW1K5G/tSNPQSSc3YTvZtR/j4kU7REbkR8Zb8sln4QASEC2faIjyhaFaahiAoiAsKyMKQtd89i2mpIrKQZgmLNo8cAIgIDRzv8jHcMGyIKLnquS632wNfq8pAo1bKdNDgBK7KJ74EYKnBHYCwAAIAAAACAAAAAgAAAAAACAAAAACICA2R0r/JjPDUYZVOftStiudb7nk4jV2Yo4fCgp5k0WOBsGJ1rbYYsAACAAAAAgAAAAIAAAAAAAgAAAAA=", "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    ret = CfdExtractPsbtTransaction(handle, psbt_handle, &output);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_tx, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, UsecaseMultisig) {
  // const char* psbt_utxo_sh_wsh_multi = "02000000000102f1993fe8e7189542ee4506258e170201be292703cd275acb09ece16672fd848b0100000017160014c1768dd714526732f54de6c8581a78e385d0900affffffffc6d2ea36e2e802b52ddac665dacbed2f831b5263459e1ca734f5c945d7515e4001000000171600141305fd9c86bb2cd2d671928388eccd2b1140aa5affffffff010858cd1d0000000017a914945fb50391a70637c1ffc5ab7fb65308c2f2317587024730440220032350deae8e8fc60a45800508b09f475ba6043b40ce87f8020a6cc532e401ab02205979d8ad394e19ef9881bb987f77dab465f1112a746ca17a509215c2dbfea93201210206d743464ae738be5ba6d07dd434bed3f24af32f9fad805ffb9a241ec56ea24802473044022051f8776ba287c7424b2d783ebde964e25bde5fec4728735b8b455c806daae16602207988cabc3475109e264a7d378873d0fcee78959c46facdafb099af4ca6eace94012103e241f36e5e17a599c465ec82f3520e6d7a0506cc862de06ff4ba26b255a2f62c00000000";

  auto path1 = "44h/0h/0h/0/12";
  auto key1 = "02906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b";
  auto key2 = "02622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f4622242";
  auto out_multisig = "522102906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b2102622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f462224252ae";
  // auto addr = "tb1q8jksvxw7vlnzg7nk5ypgzd34cpf52lrt5n77ftqllkq534cwf0xqjsj24p";

  auto in_path = "44h/0h/0h/0/11";
  auto key_in1 = "ac9aa506601b23ea7362dfcb9350ff3c2edeb67b334258d6489889e67dfa417f";
  auto pubkey_in1 = "03a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3";
  auto key_in2 = "973e1d45dca188b0c0ac67bcc5de3ccdbe479d7505bdc43c57231c75f8db55e3";
  auto pubkey_in2 = "03f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b47";
  auto multisig = "522103a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf32103f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b4752ae";

  const char* txid = "544d545a1e53becbf9dd9b0e424e1189b8f6e46d6bc36b191816d341c2d32f69";
  uint32_t vout = 0;

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "", "", 2, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    // psbt_utxo_sh_wsh_multi
    ret = CfdAddPsbtTxInWithScript(handle, psbt_handle,
        txid, vout,
        0xffffffff, 499996680,
        "a914945fb50391a70637c1ffc5ab7fb65308c2f2317587",
        multisig, "", "");
    EXPECT_EQ(kCfdSuccess, ret);
    CfdcapiPsbtDumpLog(ret, handle);

    ret = CfdSetPsbtSighashType(handle, psbt_handle,
        txid, vout,
        1);
    EXPECT_EQ(kCfdSuccess, ret);
    CfdcapiPsbtDumpLog(ret, handle);

    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    char* psbt1 = nullptr;
    char* psbt2 = nullptr;
    char* base64 = nullptr;
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("cHNidP8BADMCAAAAAWkv08JB0xYYGWvDa23k9riJEU5CDpvd+cu+Ux5aVE1UAAAAAAD/////AAAAAAAAAQEgCFjNHQAAAAAXqRSUX7UDkacGN8H/xat/tlMIwvIxdYcBAwQBAAAAAQQiACCcTayyXruK2ou7Gt24ad6k2BcMyVHx2WlLIVThWDJ2yQEFR1IhA6US9fWcDnkB/EeO1jU+73b0T5yywYW/vPfwubt2cWvzIQP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7R1KuAA==", output);

      void* psbt_handle1 = nullptr;
      ret = CfdCreatePsbtHandle(
        handle, net_type, output, "", 0, 0, &psbt_handle1);
      EXPECT_EQ(kCfdSuccess, ret);
      CfdcapiPsbtDumpLog(ret, handle);
      if (ret == kCfdSuccess) {
        ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle1,
            txid, vout,
            pubkey_in1, "2a704760", in_path);
        EXPECT_EQ(kCfdSuccess, ret);

        ret = CfdGetPsbtData(handle, psbt_handle1, &psbt1, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);

        ret = CfdFreePsbtHandle(handle, psbt_handle1);
        EXPECT_EQ(kCfdSuccess, ret);
      }

      void* psbt_handle2 = nullptr;
      ret = CfdCreatePsbtHandle(
        handle, net_type, output, "", 0, 0, &psbt_handle2);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle2,
            txid, vout,
            pubkey_in2, "9d6b6d86", in_path);
        EXPECT_EQ(kCfdSuccess, ret);

        ret = CfdGetPsbtData(handle, psbt_handle2, &psbt2, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);

        ret = CfdFreePsbtHandle(handle, psbt_handle2);
        EXPECT_EQ(kCfdSuccess, ret);
      }

      ret = CfdFreeStringBuffer(output);
      EXPECT_EQ(kCfdSuccess, ret);
      output = nullptr;
    }

    ret = CfdAddPsbtTxOutWithScript(handle, psbt_handle, 499993360,
        "00203cad0619de67e6247a76a102813635c053457c6ba4fde4ac1ffd8148d70e4bcc",
        out_multisig, "", nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetPsbtTxOutBip32Pubkey(handle, psbt_handle, 0,
        key1, "2a704760", path1);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetPsbtTxOutBip32Pubkey(handle, psbt_handle, 0,
        key2, "9d6b6d86", path1);
    EXPECT_EQ(kCfdSuccess, ret);

    if ((psbt1 != nullptr) && (psbt2 != nullptr)) {
      ret = CfdJoinPsbt(handle, psbt_handle, psbt1);
      EXPECT_EQ(kCfdSuccess, ret);

      ret = CfdJoinPsbt(handle, psbt_handle, psbt2);
      EXPECT_EQ(kCfdSuccess, ret);

      ret = CfdGetPsbtData(handle, psbt_handle, &base64, &output);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        EXPECT_STREQ("70736274ff01005e0200000001692fd3c241d31618196bc36b6de4f6b889114e420e9bddf9cbbe531e5a544d540000000000ffffffff01104bcd1d000000002200203cad0619de67e6247a76a102813635c053457c6ba4fde4ac1ffd8148d70e4bcc00000000000101200858cd1d0000000017a914945fb50391a70637c1ffc5ab7fb65308c2f23175870103040100000001042200209c4dacb25ebb8ada8bbb1addb869dea4d8170cc951f1d9694b2154e1583276c9010547522103a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf32103f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b4752ae220603a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3182a7047602c0000800000008000000080000000000b000000220603f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b47189d6b6d862c0000800000008000000080000000000b00000000010147522102906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b2102622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f462224252ae220202622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f4622242189d6b6d862c0000800000008000000080000000000c000000220202906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b182a7047602c0000800000008000000080000000000c00000000", output);
        EXPECT_STREQ("cHNidP8BAF4CAAAAAWkv08JB0xYYGWvDa23k9riJEU5CDpvd+cu+Ux5aVE1UAAAAAAD/////ARBLzR0AAAAAIgAgPK0GGd5n5iR6dqECgTY1wFNFfGuk/eSsH/2BSNcOS8wAAAAAAAEBIAhYzR0AAAAAF6kUlF+1A5GnBjfB/8Wrf7ZTCMLyMXWHAQMEAQAAAAEEIgAgnE2ssl67itqLuxrduGnepNgXDMlR8dlpSyFU4VgydskBBUdSIQOlEvX1nA55AfxHjtY1Pu929E+cssGFv7z38Lm7dnFr8yED9NRzYU6VSsT1UY5vfLMwe0+EdD4TftTuy19PtkPCO0dSriIGA6US9fWcDnkB/EeO1jU+73b0T5yywYW/vPfwubt2cWvzGCpwR2AsAACAAAAAgAAAAIAAAAAACwAAACIGA/TUc2FOlUrE9VGOb3yzMHtPhHQ+E37U7stfT7ZDwjtHGJ1rbYYsAACAAAAAgAAAAIAAAAAACwAAAAABAUdSIQKQbTmfbbvsyJjUuN4/SXR0yGpB8s821x+V5coGsHSGeyECYix5dOwy3nWvo3M7CabDPQ3qUUsp+qzPsJkXdPRiIkJSriICAmIseXTsMt51r6NzOwmmwz0N6lFLKfqsz7CZF3T0YiJCGJ1rbYYsAACAAAAAgAAAAIAAAAAADAAAACICApBtOZ9tu+zImNS43j9JdHTIakHyzzbXH5XlygawdIZ7GCpwR2AsAACAAAAAgAAAAIAAAAAADAAAAAA=", base64);

        ret = CfdFreeStringBuffer(output);
        EXPECT_EQ(kCfdSuccess, ret);
        output = nullptr;
        ret = CfdFreeStringBuffer(base64);
        EXPECT_EQ(kCfdSuccess, ret);
        base64 = nullptr;
      }
    }
    if (psbt1 != nullptr) {
      ret = CfdFreeStringBuffer(psbt1);
      EXPECT_EQ(kCfdSuccess, ret);
      psbt1 = nullptr;
    }
    if (psbt2 != nullptr) {
      ret = CfdFreeStringBuffer(psbt2);
      EXPECT_EQ(kCfdSuccess, ret);
      psbt2 = nullptr;
    }

    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      // sign psbt
      void* psbt_handle1 = nullptr;
      ret = CfdCreatePsbtHandle(
        handle, net_type, output, "", 0, 0, &psbt_handle1);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        ret = CfdSignPsbt(handle, psbt_handle1, key_in1, true);
        EXPECT_EQ(kCfdSuccess, ret);

        ret = CfdGetPsbtData(handle, psbt_handle1, &psbt1, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);

        ret = CfdFreePsbtHandle(handle, psbt_handle1);
        EXPECT_EQ(kCfdSuccess, ret);
      }

      void* psbt_handle2 = nullptr;
      ret = CfdCreatePsbtHandle(
        handle, net_type, output, "", 0, 0, &psbt_handle2);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        // ret = CfdSignPsbt(handle, psbt_handle2, key_in2, true);
        // EXPECT_EQ(kCfdSuccess, ret);
        // CfdcapiPsbtDumpLog(ret, handle);
        // Same processing.
        int sighash_type = 0;
        ret = CfdGetPsbtSighashType(handle, psbt_handle2, txid, vout, &sighash_type);
        EXPECT_EQ(kCfdSuccess, ret);
        EXPECT_EQ(1, sighash_type);

        int64_t amount = 0;
        char* script = nullptr;
        char* tx = nullptr;
        ret = CfdGetPsbtUtxoData(handle, psbt_handle2, txid, vout, &amount, nullptr, &script, nullptr, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdcapiPsbtDumpLog(ret, handle);
        if (ret == kCfdSuccess) {
          ret = CfdGetPsbtGlobalData(handle, psbt_handle2, nullptr, &tx, nullptr, nullptr);
          EXPECT_EQ(kCfdSuccess, ret);
          if (ret == kCfdSuccess) {
            char* sighash = nullptr;
            ret = CfdCreateSighash(handle, net_type, tx, txid, vout, kCfdP2shP2wsh,
                pubkey_in2, script, amount, sighash_type, false, &sighash);
            EXPECT_EQ(kCfdSuccess, ret);
            if (ret == kCfdSuccess) {
              char* sig = nullptr;
              ret = CfdCalculateEcSignature(handle, sighash, key_in2, nullptr, net_type, true, &sig);
              EXPECT_EQ(kCfdSuccess, ret);
              if (ret == kCfdSuccess) {
                char* der_sig = nullptr;
                ret = CfdEncodeSignatureByDer(handle, sig, sighash_type, false, &der_sig);
                EXPECT_EQ(kCfdSuccess, ret);
                if (ret == kCfdSuccess) {
                  ret = CfdSetPsbtSignature(handle, psbt_handle2, txid, vout, pubkey_in2, der_sig);
                  EXPECT_EQ(kCfdSuccess, ret);

                  ret = CfdFreeStringBuffer(der_sig);
                  EXPECT_EQ(kCfdSuccess, ret);
                  der_sig = nullptr;
                }
                ret = CfdFreeStringBuffer(sig);
                EXPECT_EQ(kCfdSuccess, ret);
                sig = nullptr;
              }
              ret = CfdFreeStringBuffer(sighash);
              EXPECT_EQ(kCfdSuccess, ret);
              sighash = nullptr;
            }
            ret = CfdFreeStringBuffer(tx);
            EXPECT_EQ(kCfdSuccess, ret);
            tx = nullptr;
          }
          ret = CfdFreeStringBuffer(script);
          EXPECT_EQ(kCfdSuccess, ret);
          script = nullptr;
        }

        ret = CfdGetPsbtData(handle, psbt_handle2, &psbt2, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);

        ret = CfdFreePsbtHandle(handle, psbt_handle2);
        EXPECT_EQ(kCfdSuccess, ret);
      }

      ret = CfdFreeStringBuffer(output);
      EXPECT_EQ(kCfdSuccess, ret);
      output = nullptr;
    }

    if ((psbt1 != nullptr) && (psbt2 != nullptr)) {
      ret = CfdCombinePsbt(handle, psbt_handle, psbt1);
      EXPECT_EQ(kCfdSuccess, ret);

      ret = CfdCombinePsbt(handle, psbt_handle, psbt2);
      EXPECT_EQ(kCfdSuccess, ret);

      ret = CfdGetPsbtData(handle, psbt_handle, &base64, &output);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        EXPECT_STREQ("70736274ff01005e0200000001692fd3c241d31618196bc36b6de4f6b889114e420e9bddf9cbbe531e5a544d540000000000ffffffff01104bcd1d000000002200203cad0619de67e6247a76a102813635c053457c6ba4fde4ac1ffd8148d70e4bcc00000000000101200858cd1d0000000017a914945fb50391a70637c1ffc5ab7fb65308c2f2317587220203a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3473044022020635937e05170d83dc3213adb3a6eae66713008e4abc38b4912c5680dec4f0c022033c08a71a30757b08463d530ec058ed7401968fb30bef63d4549721a0e485b8401220203f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b4747304402205d274a7887de3efade84a4a349c4c36f589b55623b1027b7da6dac89c178563402206a03afebbbb6089f1e37fac8f999a4015161c8920144fd53112da05804cd43cc010103040100000001042200209c4dacb25ebb8ada8bbb1addb869dea4d8170cc951f1d9694b2154e1583276c9010547522103a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf32103f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b4752ae220603a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3182a7047602c0000800000008000000080000000000b000000220603f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b47189d6b6d862c0000800000008000000080000000000b00000000010147522102906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b2102622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f462224252ae220202622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f4622242189d6b6d862c0000800000008000000080000000000c000000220202906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b182a7047602c0000800000008000000080000000000c00000000", output);
        EXPECT_STREQ("cHNidP8BAF4CAAAAAWkv08JB0xYYGWvDa23k9riJEU5CDpvd+cu+Ux5aVE1UAAAAAAD/////ARBLzR0AAAAAIgAgPK0GGd5n5iR6dqECgTY1wFNFfGuk/eSsH/2BSNcOS8wAAAAAAAEBIAhYzR0AAAAAF6kUlF+1A5GnBjfB/8Wrf7ZTCMLyMXWHIgIDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/NHMEQCICBjWTfgUXDYPcMhOts6bq5mcTAI5KvDi0kSxWgN7E8MAiAzwIpxowdXsIRj1TDsBY7XQBlo+zC+9j1FSXIaDkhbhAEiAgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7R0cwRAIgXSdKeIfePvrehKSjScTDb1ibVWI7ECe32m2sicF4VjQCIGoDr+u7tgifHjf6yPmZpAFRYciSAUT9UxEtoFgEzUPMAQEDBAEAAAABBCIAIJxNrLJeu4rai7sa3bhp3qTYFwzJUfHZaUshVOFYMnbJAQVHUiEDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/MhA/TUc2FOlUrE9VGOb3yzMHtPhHQ+E37U7stfT7ZDwjtHUq4iBgOlEvX1nA55AfxHjtY1Pu929E+cssGFv7z38Lm7dnFr8xgqcEdgLAAAgAAAAIAAAACAAAAAAAsAAAAiBgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7Rxida22GLAAAgAAAAIAAAACAAAAAAAsAAAAAAQFHUiECkG05n2277MiY1LjeP0l0dMhqQfLPNtcfleXKBrB0hnshAmIseXTsMt51r6NzOwmmwz0N6lFLKfqsz7CZF3T0YiJCUq4iAgJiLHl07DLeda+jczsJpsM9DepRSyn6rM+wmRd09GIiQhida22GLAAAgAAAAIAAAACAAAAAAAwAAAAiAgKQbTmfbbvsyJjUuN4/SXR0yGpB8s821x+V5coGsHSGexgqcEdgLAAAgAAAAIAAAACAAAAAAAwAAAAA", base64);

        ret = CfdFreeStringBuffer(output);
        EXPECT_EQ(kCfdSuccess, ret);
        output = nullptr;
        ret = CfdFreeStringBuffer(base64);
        EXPECT_EQ(kCfdSuccess, ret);
        base64 = nullptr;
      }
    }
    if (psbt1 != nullptr) {
      ret = CfdFreeStringBuffer(psbt1);
      EXPECT_EQ(kCfdSuccess, ret);
      psbt1 = nullptr;
    }
    if (psbt2 != nullptr) {
      ret = CfdFreeStringBuffer(psbt2);
      EXPECT_EQ(kCfdSuccess, ret);
      psbt2 = nullptr;
    }

    ret = CfdIsFinalizedPsbt(handle, psbt_handle);
    EXPECT_EQ(kCfdSignVerificationError, ret);

    ret = CfdSetPsbtFinalizeScript(handle, psbt_handle,
        txid, vout,
        "00473044022020635937e05170d83dc3213adb3a6eae66713008e4abc38b4912c5680dec4f0c022033c08a71a30757b08463d530ec058ed7401968fb30bef63d4549721a0e485b840147304402205d274a7887de3efade84a4a349c4c36f589b55623b1027b7da6dac89c178563402206a03afebbbb6089f1e37fac8f999a4015161c8920144fd53112da05804cd43cc0147522103a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf32103f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b4752ae");
    EXPECT_EQ(kCfdSuccess, ret);
    CfdcapiPsbtDumpLog(ret, handle);

    ret = CfdIsFinalizedPsbt(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    CfdcapiPsbtDumpLog(ret, handle);

    ret = CfdClearPsbtSignData(handle, psbt_handle,
        txid, vout);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdExtractPsbtTransaction(handle, psbt_handle, &output);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("02000000000101692fd3c241d31618196bc36b6de4f6b889114e420e9bddf9cbbe531e5a544d5400000000232200209c4dacb25ebb8ada8bbb1addb869dea4d8170cc951f1d9694b2154e1583276c9ffffffff01104bcd1d000000002200203cad0619de67e6247a76a102813635c053457c6ba4fde4ac1ffd8148d70e4bcc0400473044022020635937e05170d83dc3213adb3a6eae66713008e4abc38b4912c5680dec4f0c022033c08a71a30757b08463d530ec058ed7401968fb30bef63d4549721a0e485b840147304402205d274a7887de3efade84a4a349c4c36f589b55623b1027b7da6dac89c178563402206a03afebbbb6089f1e37fac8f999a4015161c8920144fd53112da05804cd43cc0147522103a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf32103f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b4752ae00000000", output);
      ret = CfdFreeStringBuffer(output);
      EXPECT_EQ(kCfdSuccess, ret);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}


TEST(cfdcapi_psbt, CreatePsbtNotUseDescriptor) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAP0+AQIAAAAG+DTafLXhg/yBX3hGInKEZg+koKWDviv7SlNb9CXlMd4BAAAAAP////9a72o7znYklRrluejAkI6P90ch7t96CZTVvx78fe6CgQIAAAAA/////6kt1klSeJ79FESTjpzlsQbnKACJxNxKeLPafHoradk/AwAAAAD//////Pcse7iIGUWVXkj1TmCBtRkBg2SE5cSKmSUGECnEBdEEAAAAAP////+gvhpNmiLIMtLSCjoITwENCamrQfs+Le29RusBBFePuQUAAAAA/////3GhQb9bZT4sQ3czBdaajJh2lERwyh4p5LA6CVPEvraVAQAAAAD/////AoDw+gIAAAAAFgAUsyK93OYzuFGsc3CrRU8LNnoGVOWA8PoCAAAAABYAFMq4xTpuj8ApbRzTkVowfVHEkaVVAAAAAAABAR+AlpgAAAAAABYAFJYsTgjzNtOvvDQVydNZrhBARwUgIgYCVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv4YKnBHYCwAAIAAAACAAAAAgAEAAAABAAAAAAEBH4CWmAAAAAAAFgAUi/CdYLfjTzSCfY28scdjkKkWyoIiBgInRM+yQ24VYEDsHI/oQtnvmhjLQK1B8xJyU5DDX3vTaxgqcEdgLAAAgAAAAIAAAACAAQAAAAIAAAAAAQEfgJaYAAAAAAAWABTx0+5ngpIl64ksyrAeIvand+fiHiIGAufo3CNvoCQ2lAjSzk2FCASCYavCl7gRYE2gh61x0ThVGCpwR2AsAACAAAAAgAAAAIABAAAAAwAAAAABAR+AlpgAAAAAABYAFBnWX4MosiBtmXB4VmDsDTSAj7B1IgYDPYdL8ZtpfPbGOWWVR6WDuGcwFkpbbbAb8goU65tq20QYKnBHYCwAAIAAAACAAAAAgAEAAAAEAAAAAAEBH4CWmAAAAAAAFgAUEreVSnXvwqIOhuMtwteGR9ZwB3kiBgL7Bhcw2948gGtKF6mfRUyBKCrs7m1G9Kl1qUzf06BlBBgqcEdgLAAAgAAAAIAAAACAAQAAAAUAAAAAAQEfgPD6AgAAAAAWABSXipBGDkRnGlL0mgm7WcxnlLY8iSIGA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rsGJ1rbYYsAACAAAAAgAAAAIABAAAAAQAAAAAiAgNHO/yMdwwbIgoueq5LrfbA1+rykCjVsp00OAErsonvgRgqcEdgLAAAgAAAAIAAAACAAAAAAAIAAAAAIgIDZHSv8mM8NRhlU5+1K2K51vueTiNXZijh8KCnmTRY4GwYnWtthiwAAIAAAACAAAAAgAAAAAACAAAAAA==";
  const char* exp_psbt = "70736274ff0100fd3e010200000006f834da7cb5e183fc815f7846227284660fa4a0a583be2bfb4a535bf425e531de0100000000ffffffff5aef6a3bce7624951ae5b9e8c0908e8ff74721eedf7a0994d5bf1efc7dee82810200000000ffffffffa92dd64952789efd1444938e9ce5b106e7280089c4dc4a78b3da7c7a2b69d93f0300000000fffffffffcf72c7bb8881945955e48f54e6081b51901836484e5c48a9925061029c405d10400000000ffffffffa0be1a4d9a22c832d2d20a3a084f010d09a9ab41fb3e2dedbd46eb0104578fb90500000000ffffffff71a141bf5b653e2c43773305d69a8c9876944470ca1e29e4b03a0953c4beb6950100000000ffffffff0280f0fa0200000000160014b322bddce633b851ac7370ab454f0b367a0654e580f0fa0200000000160014cab8c53a6e8fc0296d1cd3915a307d51c491a555000000000001011f8096980000000000160014962c4e08f336d3afbc3415c9d359ae1040470520220602565248460b3c186decf13db06f0724fd50b47cc3e489c30076c8363d5038cefe182a7047602c000080000000800000008001000000010000000001011f80969800000000001600148bf09d60b7e34f34827d8dbcb1c76390a916ca822206022744cfb2436e156040ec1c8fe842d9ef9a18cb40ad41f312725390c35f7bd36b182a7047602c000080000000800000008001000000020000000001011f8096980000000000160014f1d3ee67829225eb892ccab01e22f6a777e7e21e220602e7e8dc236fa024369408d2ce4d8508048261abc297b811604da087ad71d13855182a7047602c000080000000800000008001000000030000000001011f809698000000000016001419d65f8328b2206d9970785660ec0d34808fb0752206033d874bf19b697cf6c639659547a583b86730164a5b6db01bf20a14eb9b6adb44182a7047602c000080000000800000008001000000040000000001011f809698000000000016001412b7954a75efc2a20e86e32dc2d78647d6700779220602fb061730dbde3c806b4a17a99f454c81282aecee6d46f4a975a94cdfd3a06504182a7047602c000080000000800000008001000000050000000001011f80f0fa0200000000160014978a90460e44671a52f49a09bb59cc6794b63c89220603e3d244a3967e0b87765fda86c5ff38885f74993953b9584388aef30b26af6aec189d6b6d862c0000800000008000000080010000000100000000220203473bfc8c770c1b220a2e7aae4badf6c0d7eaf29028d5b29d3438012bb289ef81182a7047602c00008000000080000000800000000002000000002202036474aff2633c351865539fb52b62b9d6fb9e4e23576628e1f0a0a7993458e06c189d6b6d862c0000800000008000000080000000000200000000";
  char* output_base64 = nullptr;
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "", "", 2, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    int64_t amount1 = 0x989680;
    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "de31e525f45b534afb2bbe83a5a0a40f6684722246785f81fc83e1b57cda34f8", 1,
        0xffffffff, amount1, "0014962c4e08f336d3afbc3415c9d359ae1040470520",
        "",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    CfdcapiPsbtDumpLog(ret, handle);

    ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle,
        "de31e525f45b534afb2bbe83a5a0a40f6684722246785f81fc83e1b57cda34f8", 1,
        "02565248460b3c186decf13db06f0724fd50b47cc3e489c30076c8363d5038cefe", "2a704760", "44h/0h/0h/1/1");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "8182ee7dfc1ebfd594097adfee2147f78f8e90c0e8b9e51a952476ce3b6aef5a", 2,
        0xffffffff, amount1, "00148bf09d60b7e34f34827d8dbcb1c76390a916ca82",
        "",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle,
        "8182ee7dfc1ebfd594097adfee2147f78f8e90c0e8b9e51a952476ce3b6aef5a", 2,
        "022744cfb2436e156040ec1c8fe842d9ef9a18cb40ad41f312725390c35f7bd36b", "2a704760", "44h/0h/0h/1/2");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "3fd9692b7a7cdab3784adcc4890028e706b1e59c8e934414fd9e785249d62da9", 3,
        0xffffffff, amount1, "0014f1d3ee67829225eb892ccab01e22f6a777e7e21e",
        "",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle,
        "3fd9692b7a7cdab3784adcc4890028e706b1e59c8e934414fd9e785249d62da9", 3,
        "02e7e8dc236fa024369408d2ce4d8508048261abc297b811604da087ad71d13855", "2a704760", "44h/0h/0h/1/3");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "d105c429100625998ac4e58464830119b581604ef5485e95451988b87b2cf7fc", 4,
        0xffffffff, amount1, "001419d65f8328b2206d9970785660ec0d34808fb075",
        "",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle,
        "d105c429100625998ac4e58464830119b581604ef5485e95451988b87b2cf7fc", 4,
        "033d874bf19b697cf6c639659547a583b86730164a5b6db01bf20a14eb9b6adb44", "2a704760", "44h/0h/0h/1/4");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "b98f570401eb46bded2d3efb41aba9090d014f083a0ad2d232c8229a4d1abea0", 5,
        0xffffffff, amount1, "001412b7954a75efc2a20e86e32dc2d78647d6700779",
        "",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle,
        "b98f570401eb46bded2d3efb41aba9090d014f083a0ad2d232c8229a4d1abea0", 5,
        "02fb061730dbde3c806b4a17a99f454c81282aecee6d46f4a975a94cdfd3a06504", "2a704760", "44h/0h/0h/1/5");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxInWithPubkey(handle, psbt_handle,
        "95b6bec453093ab0e4291eca70449476988c9ad6053377432c3e655bbf41a171", 1,
        0xffffffff, 0x2faf080, "0014978a90460e44671a52f49a09bb59cc6794b63c89",
        "",
        nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdSetPsbtTxInBip32Pubkey(handle, psbt_handle,
        "95b6bec453093ab0e4291eca70449476988c9ad6053377432c3e655bbf41a171", 1,
        "03e3d244a3967e0b87765fda86c5ff38885f74993953b9584388aef30b26af6aec", "9d6b6d86", "44h/0h/0h/1/1");
    EXPECT_EQ(kCfdSuccess, ret);

    uint32_t index = 0;
    ret = CfdAddPsbtTxOutWithPubkey(handle, psbt_handle,
        50000000, "0014b322bddce633b851ac7370ab454f0b367a0654e5",
        "",
        &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(0, index);

    ret = CfdSetPsbtTxOutBip32Pubkey(handle, psbt_handle, index,
        "03473bfc8c770c1b220a2e7aae4badf6c0d7eaf29028d5b29d3438012bb289ef81", "2a704760", "44h/0h/0h/0/2");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtTxOutWithPubkey(handle, psbt_handle,
        50000000, "0014cab8c53a6e8fc0296d1cd3915a307d51c491a555",
        "",
        &index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(1, index);

    ret = CfdSetPsbtTxOutBip32Pubkey(handle, psbt_handle, index,
        "036474aff2633c351865539fb52b62b9d6fb9e4e23576628e1f0a0a7993458e06c", "9d6b6d86", "44h/0h/0h/0/2");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdGetPsbtData(handle, psbt_handle, &output_base64, &output);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output_base64);
      EXPECT_STREQ(exp_psbt, output);
      CfdFreeStringBuffer(output_base64);
      output_base64 = nullptr;
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, FundPsbt) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* exp_psbt_base64 = "cHNidP8BAP2GAQIAAAAH+DTafLXhg/yBX3hGInKEZg+koKWDviv7SlNb9CXlMd4BAAAAAP////9a72o7znYklRrluejAkI6P90ch7t96CZTVvx78fe6CgQIAAAAA/////6kt1klSeJ79FESTjpzlsQbnKACJxNxKeLPafHoradk/AwAAAAD//////Pcse7iIGUWVXkj1TmCBtRkBg2SE5cSKmSUGECnEBdEEAAAAAP////+gvhpNmiLIMtLSCjoITwENCamrQfs+Le29RusBBFePuQUAAAAA/////3GhQb9bZT4sQ3czBdaajJh2lERwyh4p5LA6CVPEvraVAQAAAAD/////PEJ8leyO15eW8T031SX76GhUXNkaHrDYn/kJgHMfOosHAAAAAP////8DgPD6AgAAAAAWABSzIr3c5jO4UaxzcKtFTws2egZU5YDw+gIAAAAAFgAUyrjFOm6PwCltHNORWjB9UcSRpVXykZgAAAAAABYAFCtuFro44oBQD4CyxXBu8C841ZCBAAAAAAABAR+AlpgAAAAAABYAFJYsTgjzNtOvvDQVydNZrhBARwUgIgYCVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv4YKnBHYCwAAIAAAACAAAAAgAEAAAABAAAAAAEBH4CWmAAAAAAAFgAUi/CdYLfjTzSCfY28scdjkKkWyoIiBgInRM+yQ24VYEDsHI/oQtnvmhjLQK1B8xJyU5DDX3vTaxgqcEdgLAAAgAAAAIAAAACAAQAAAAIAAAAAAQEfgJaYAAAAAAAWABTx0+5ngpIl64ksyrAeIvand+fiHiIGAufo3CNvoCQ2lAjSzk2FCASCYavCl7gRYE2gh61x0ThVGCpwR2AsAACAAAAAgAAAAIABAAAAAwAAAAABAR+AlpgAAAAAABYAFBnWX4MosiBtmXB4VmDsDTSAj7B1IgYDPYdL8ZtpfPbGOWWVR6WDuGcwFkpbbbAb8goU65tq20QYKnBHYCwAAIAAAACAAAAAgAEAAAAEAAAAAAEBH4CWmAAAAAAAFgAUEreVSnXvwqIOhuMtwteGR9ZwB3kiBgL7Bhcw2948gGtKF6mfRUyBKCrs7m1G9Kl1qUzf06BlBBgqcEdgLAAAgAAAAIAAAACAAQAAAAUAAAAAAQEfgPD6AgAAAAAWABSXipBGDkRnGlL0mgm7WcxnlLY8iSIGA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rsGJ1rbYYsAACAAAAAgAAAAIABAAAAAQAAAAABAR+AlpgAAAAAABYAFBzoeOOg2js0MIeX/ey3YiH4VBivIgYDwCMlwyj+1iKp2I+PUxjgUmGjw6lntyEdfWdlfiten74YKnBHYCwAAIAAAACAAAAAgAEAAAAHAAAAACICA0c7/Ix3DBsiCi56rkut9sDX6vKQKNWynTQ4ASuyie+BGCpwR2AsAACAAAAAgAAAAIAAAAAAAgAAAAAiAgNkdK/yYzw1GGVTn7UrYrnW+55OI1dmKOHwoKeZNFjgbBida22GLAAAgAAAAIAAAACAAAAAAAIAAAAAIgIC9/DX0AKJt8Wlgbw1J2BAw0jeSPxBQGfzHqJq2VwAVQwYKnBHYCwAAIAAAACAAAAAgAEAAABkAAAAAA==";
  char* output = nullptr;
  ret = CfdCreatePsbtHandle(
    handle, net_type, "cHNidP8BAP0+AQIAAAAG+DTafLXhg/yBX3hGInKEZg+koKWDviv7SlNb9CXlMd4BAAAAAP////9a72o7znYklRrluejAkI6P90ch7t96CZTVvx78fe6CgQIAAAAA/////6kt1klSeJ79FESTjpzlsQbnKACJxNxKeLPafHoradk/AwAAAAD//////Pcse7iIGUWVXkj1TmCBtRkBg2SE5cSKmSUGECnEBdEEAAAAAP////+gvhpNmiLIMtLSCjoITwENCamrQfs+Le29RusBBFePuQUAAAAA/////3GhQb9bZT4sQ3czBdaajJh2lERwyh4p5LA6CVPEvraVAQAAAAD/////AoDw+gIAAAAAFgAUsyK93OYzuFGsc3CrRU8LNnoGVOWA8PoCAAAAABYAFMq4xTpuj8ApbRzTkVowfVHEkaVVAAAAAAABAR+AlpgAAAAAABYAFJYsTgjzNtOvvDQVydNZrhBARwUgIgYCVlJIRgs8GG3s8T2wbwck/VC0fMPkicMAdsg2PVA4zv4YKnBHYCwAAIAAAACAAAAAgAEAAAABAAAAAAEBH4CWmAAAAAAAFgAUi/CdYLfjTzSCfY28scdjkKkWyoIiBgInRM+yQ24VYEDsHI/oQtnvmhjLQK1B8xJyU5DDX3vTaxgqcEdgLAAAgAAAAIAAAACAAQAAAAIAAAAAAQEfgJaYAAAAAAAWABTx0+5ngpIl64ksyrAeIvand+fiHiIGAufo3CNvoCQ2lAjSzk2FCASCYavCl7gRYE2gh61x0ThVGCpwR2AsAACAAAAAgAAAAIABAAAAAwAAAAABAR+AlpgAAAAAABYAFBnWX4MosiBtmXB4VmDsDTSAj7B1IgYDPYdL8ZtpfPbGOWWVR6WDuGcwFkpbbbAb8goU65tq20QYKnBHYCwAAIAAAACAAAAAgAEAAAAEAAAAAAEBH4CWmAAAAAAAFgAUEreVSnXvwqIOhuMtwteGR9ZwB3kiBgL7Bhcw2948gGtKF6mfRUyBKCrs7m1G9Kl1qUzf06BlBBgqcEdgLAAAgAAAAIAAAACAAQAAAAUAAAAAAQEfgPD6AgAAAAAWABSXipBGDkRnGlL0mgm7WcxnlLY8iSIGA+PSRKOWfguHdl/ahsX/OIhfdJk5U7lYQ4iu8wsmr2rsGJ1rbYYsAACAAAAAgAAAAIABAAAAAQAAAAAiAgNHO/yMdwwbIgoueq5LrfbA1+rykCjVsp00OAErsonvgRgqcEdgLAAAgAAAAIAAAACAAAAAAAIAAAAAIgIDZHSv8mM8NRhlU5+1K2K51vueTiNXZijh8KCnmTRY4GwYnWtthiwAAIAAAACAAAAAgAAAAAACAAAAAA==", "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    void* fund_handle = nullptr;
    ret = CfdInitializeFundPsbt(handle, &fund_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      uint32_t utxo_list_num = 5;
      CfdUtxo utxo_list[] = {
        {
           "8b3a1f738009f99fd8b01e1ad95c5468e8fb25d5373df19697d78eec957c423c", 7, 10000000,
           "wpkh([2a704760/44'/0'/0'/1/7]03c02325c328fed622a9d88f8f5318e05261a3c3a967b7211d7d67657e2b5e9fbe)",
           nullptr
        },
        {
           "0e5b0e16148da878ee8d9f0524ac0962a22dbbb66e2dc8a6237a1d4c8c4ea9cb", 8, 10000000,
           "wpkh([2a704760/44'/0'/0'/1/8]02f505e0b5885959bd2f7e68dd73915940a5540e05c41a42f6c598ceb21bdc55f3)",
           nullptr
        },
        {
           "016b36a0a9df54d55f7b907aba8fdd3d90d797eee2bc46c5ed7dced41218e674", 9, 10000000,
           "wpkh([2a704760/44'/0'/0'/1/9]02f7af7e1c3c8627e0e662ba04ec003879d9aeaeba5a02a1e24804ef4d7b497db4)",
           nullptr
        },
        {
           "dcc65ebfb8535f96f8a51e07d5c0a94f965000bd5765159a3cc27e4edd658c69", 10, 10000000,
           "wpkh([2a704760/44'/0'/0'/1/10]03c04e76f9bc7d6433a6eb7c0c3c446213c5f361dae22929483f23718e1cee4ece)",
           nullptr
        },
        {
           "0230e7471501f9432f727583f2bc17c11f5c1132140319ba2d7a5a0145684c73", 11, 10000000,
           "wpkh([2a704760/44'/0'/0'/1/11]03798ff13e5d17c6f8c1d69ff18d516613be2fa52149b3a8c60ef959d1075a5889)",
           nullptr
        }
      };
      for (uint32_t index=0; index<utxo_list_num; ++index) {
        ret = CfdFundPsbtAddToUtxoList(handle, fund_handle, utxo_list[index].txid,
            utxo_list[index].vout, utxo_list[index].amount, nullptr, utxo_list[index].descriptor,
            nullptr, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);
      }

      ret = CfdSetOptionFundPsbt(handle, fund_handle, kCfdPsbtFundEstimateFeeRate,
          0, 2.0, false);
      EXPECT_EQ(kCfdSuccess, ret);
      ret = CfdSetOptionFundPsbt(handle, fund_handle, kCfdPsbtFundKnapsackMinChange,
          0, 0, false);
      EXPECT_EQ(kCfdSuccess, ret);

      uint32_t used_utxo_num = 0;
      int64_t fee = 0;
      ret = CfdFinalizeFundPsbt(handle, psbt_handle, fund_handle,
          "wpkh([2a704760/44'/0'/0'/1/100]02f7f0d7d00289b7c5a581bc35276040c348de48fc414067f31ea26ad95c00550c)",
          &fee, &used_utxo_num);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_EQ(1, used_utxo_num);
      EXPECT_EQ(1166, fee);

      uint32_t utxo_index = 0;
      char* txid = nullptr;
      char* descriptor = nullptr;
      uint32_t vout = 0;
      int64_t amount = 0;
      for (uint32_t index=0; index<used_utxo_num; ++index) {
        ret = CfdGetFundPsbtUsedUtxo(handle, fund_handle, index, &utxo_index,
            &txid, &vout, &amount, nullptr, &descriptor, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret == kCfdSuccess) {
          EXPECT_STREQ(utxo_list[utxo_index].txid, txid);
          EXPECT_EQ(utxo_list[utxo_index].vout, vout);
          EXPECT_EQ(utxo_list[utxo_index].amount, amount);
          EXPECT_STREQ(utxo_list[utxo_index].descriptor, descriptor);

          ret = CfdFreeStringBuffer(txid);
          EXPECT_EQ(kCfdSuccess, ret);
          txid = nullptr;
          ret = CfdFreeStringBuffer(descriptor);
          EXPECT_EQ(kCfdSuccess, ret);
          descriptor = nullptr;
        }
      }

      ret = CfdFreeFundPsbt(handle, fund_handle);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    ret = CfdGetPsbtData(handle, psbt_handle, &output, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_psbt_base64, output);
      CfdFreeStringBuffer(output);
      output = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, GetPsbtInput) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* psbt_base64 = "cHNidP8BAF4CAAAAAWkv08JB0xYYGWvDa23k9riJEU5CDpvd+cu+Ux5aVE1UAAAAAAD/////ARBLzR0AAAAAIgAgPK0GGd5n5iR6dqECgTY1wFNFfGuk/eSsH/2BSNcOS8wAAAAAAAEBIAhYzR0AAAAAF6kUlF+1A5GnBjfB/8Wrf7ZTCMLyMXWHIgIDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/NHMEQCICBjWTfgUXDYPcMhOts6bq5mcTAI5KvDi0kSxWgN7E8MAiAzwIpxowdXsIRj1TDsBY7XQBlo+zC+9j1FSXIaDkhbhAEiAgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7R0cwRAIgXSdKeIfePvrehKSjScTDb1ibVWI7ECe32m2sicF4VjQCIGoDr+u7tgifHjf6yPmZpAFRYciSAUT9UxEtoFgEzUPMAQEDBAEAAAABBCIAIJxNrLJeu4rai7sa3bhp3qTYFwzJUfHZaUshVOFYMnbJAQVHUiEDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/MhA/TUc2FOlUrE9VGOb3yzMHtPhHQ+E37U7stfT7ZDwjtHUq4iBgOlEvX1nA55AfxHjtY1Pu929E+cssGFv7z38Lm7dnFr8xgqcEdgLAAAgAAAAIAAAACAAAAAAAsAAAAiBgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7Rxida22GLAAAgAAAAIAAAACAAAAAAAsAAAAAAQAiACA8rQYZ3mfmJHp2oQKBNjXAU0V8a6T95Kwf/YFI1w5LzAEBR1IhApBtOZ9tu+zImNS43j9JdHTIakHyzzbXH5XlygawdIZ7IQJiLHl07DLeda+jczsJpsM9DepRSyn6rM+wmRd09GIiQlKuIgICYix5dOwy3nWvo3M7CabDPQ3qUUsp+qzPsJkXdPRiIkIYnWtthiwAAIAAAACAAAAAgAAAAAAMAAAAIgICkG05n2277MiY1LjeP0l0dMhqQfLPNtcfleXKBrB0hnsYKnBHYCwAAIAAAACAAAAAgAAAAAAMAAAAAA==";
  ret = CfdCreatePsbtHandle(
    handle, net_type, psbt_base64, "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    uint32_t txin_index = 0;

    char* txid = nullptr;
    uint32_t vout = 0;
    char* script = nullptr;
    ret = CfdGetPsbtUtxoDataByIndex(handle, psbt_handle, 0, &txid, &vout, nullptr, nullptr, &script, nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("544d545a1e53becbf9dd9b0e424e1189b8f6e46d6bc36b191816d341c2d32f69", txid);
      EXPECT_EQ(0, vout);
      EXPECT_STREQ("522103a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf32103f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b4752ae", script);

      ret = CfdGetPsbtTxInIndex(handle, psbt_handle, txid, vout, &txin_index);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_EQ(0, txin_index);

      int64_t amount = 0;
      char* locking_script = nullptr;
      char* descriptor = nullptr;
      char* tx = nullptr;
      ret = CfdGetPsbtUtxoData(handle, psbt_handle, txid, vout, &amount, &locking_script, nullptr, &descriptor, &tx);
      EXPECT_EQ(kCfdSuccess, ret);
      if (ret == kCfdSuccess) {
        EXPECT_EQ(499996680, amount);
        EXPECT_STREQ("a914945fb50391a70637c1ffc5ab7fb65308c2f2317587", locking_script);
        EXPECT_STREQ("sh(wsh(multi(2,[2a704760/44'/0'/0'/0/11]03a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3,[9d6b6d86/44'/0'/0'/0/11]03f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b47)))", descriptor);
        EXPECT_EQ(nullptr, tx);
        ret = CfdFreeStringBuffer(locking_script);
        EXPECT_EQ(kCfdSuccess, ret);
        ret = CfdFreeStringBuffer(descriptor);
        EXPECT_EQ(kCfdSuccess, ret);
        ret = CfdFreeStringBuffer(tx);
        EXPECT_EQ(kCfdSuccess, ret);
      }

      ret = CfdFreeStringBuffer(txid);
      EXPECT_EQ(kCfdSuccess, ret);
      ret = CfdFreeStringBuffer(script);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    const char* pubkey = "03a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3";
    ret = CfdIsFindPsbtPubkeyRecord(handle, psbt_handle, kCfdPsbtRecordInputSignature, txin_index, pubkey);
    EXPECT_EQ(kCfdSuccess, ret);

    char* signature = nullptr;
    ret = CfdGetPsbtPubkeyRecord(handle, psbt_handle,
        kCfdPsbtRecordInputSignature, txin_index, pubkey, &signature);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("3044022020635937e05170d83dc3213adb3a6eae66713008e4abc38b4912c5680dec4f0c022033c08a71a30757b08463d530ec058ed7401968fb30bef63d4549721a0e485b8401", signature);
      ret = CfdFreeStringBuffer(signature);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    char* fingerprint = nullptr;
    char* path = nullptr;
    ret = CfdGetPsbtBip32Data(handle, psbt_handle, kCfdPsbtRecordInputBip32, txin_index, pubkey, &fingerprint, &path);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("2a704760", fingerprint);
      EXPECT_STREQ("44'/0'/0'/0/11", path);
      ret = CfdFreeStringBuffer(fingerprint);
      EXPECT_EQ(kCfdSuccess, ret);
      ret = CfdFreeStringBuffer(path);
      EXPECT_EQ(kCfdSuccess, ret);
    }
    void* pubkey_list_handle = nullptr;
    uint32_t list_num = 0;
    ret = CfdGetPsbtPubkeyList(handle, psbt_handle, kCfdPsbtRecordInputSignature,
        txin_index, &list_num, &pubkey_list_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      char* pk = nullptr;
      fingerprint = nullptr;
      path = nullptr;
      for (uint32_t index=0; index<list_num; ++index) {
        ret = CfdGetPsbtPubkeyListData(handle, pubkey_list_handle, index, &pk, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret == kCfdSuccess) {
          if (index == 0) {
            EXPECT_STREQ("03a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3", pk);
          } else if (index == 1) {
            EXPECT_STREQ("03f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b47", pk);
          }
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
        }
        ret = CfdGetPsbtPubkeyListBip32Data(handle, pubkey_list_handle, index, &pk, &fingerprint, &path);
        EXPECT_EQ(kCfdIllegalArgumentError, ret);
        if (ret == kCfdSuccess) {
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
          ret = CfdFreeStringBuffer(fingerprint);
          EXPECT_EQ(kCfdSuccess, ret);
          fingerprint = nullptr;
          ret = CfdFreeStringBuffer(path);
          EXPECT_EQ(kCfdSuccess, ret);
          path = nullptr;
        }
      }
      ret = CfdFreePsbtPubkeyList(handle, pubkey_list_handle);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    pubkey_list_handle = nullptr;
    list_num = 0;
    ret = CfdGetPsbtPubkeyList(handle, psbt_handle, kCfdPsbtRecordInputBip32,
        txin_index, &list_num, &pubkey_list_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      char* pk = nullptr;
      fingerprint = nullptr;
      path = nullptr;
      for (uint32_t index=0; index<list_num; ++index) {
        ret = CfdGetPsbtPubkeyListData(handle, pubkey_list_handle, index, &pk, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret == kCfdSuccess) {
          if (index == 0) {
            EXPECT_STREQ("03a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3", pk);
          } else if (index == 1) {
            EXPECT_STREQ("03f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b47", pk);
          }
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
        }
        ret = CfdGetPsbtPubkeyListBip32Data(handle, pubkey_list_handle, index, &pk, &fingerprint, &path);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdcapiPsbtDumpLog(ret, handle);
        if (ret == kCfdSuccess) {
          if (index == 0) {
            EXPECT_STREQ("03a512f5f59c0e7901fc478ed6353eef76f44f9cb2c185bfbcf7f0b9bb76716bf3", pk);
            EXPECT_STREQ("2a704760", fingerprint);
            EXPECT_STREQ("44'/0'/0'/0/11", path);
          } else if (index == 1) {
            EXPECT_STREQ("03f4d473614e954ac4f5518e6f7cb3307b4f84743e137ed4eecb5f4fb643c23b47", pk);
            EXPECT_STREQ("9d6b6d86", fingerprint);
            EXPECT_STREQ("44'/0'/0'/0/11", path);
          }
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
          ret = CfdFreeStringBuffer(fingerprint);
          EXPECT_EQ(kCfdSuccess, ret);
          fingerprint = nullptr;
          ret = CfdFreeStringBuffer(path);
          EXPECT_EQ(kCfdSuccess, ret);
          path = nullptr;
        }
      }
      ret = CfdFreePsbtPubkeyList(handle, pubkey_list_handle);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    ret = CfdIsFindPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeInput, 0, "01");
    EXPECT_EQ(kCfdSuccess, ret);

    char* value = nullptr;
    ret = CfdGetPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeInput, 0, "01", &value);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0858cd1d0000000017a914945fb50391a70637c1ffc5ab7fb65308c2f2317587", value);

      ret = CfdFreeStringBuffer(value);
      EXPECT_EQ(kCfdSuccess, ret);
      value = nullptr;
    }

    const char* unknown_key = "99";
    const char* exp_unknown_value = "0011223344";
    ret = CfdIsFindPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeInput, 0, unknown_key);
    EXPECT_EQ(kCfdNotFoundError, ret);

    ret = CfdAddPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeInput, 0, unknown_key, exp_unknown_value);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdIsFindPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeInput, 0, unknown_key);
    EXPECT_EQ(kCfdSuccess, ret);

    value = nullptr;
    ret = CfdGetPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeInput, 0, unknown_key, &value);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_unknown_value, value);

      ret = CfdFreeStringBuffer(value);
      EXPECT_EQ(kCfdSuccess, ret);
      value = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, GetPsbtOutput) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkTestnet;
  void* psbt_handle = nullptr;
  const char* psbt_base64 = "cHNidP8BAF4CAAAAAWkv08JB0xYYGWvDa23k9riJEU5CDpvd+cu+Ux5aVE1UAAAAAAD/////ARBLzR0AAAAAIgAgPK0GGd5n5iR6dqECgTY1wFNFfGuk/eSsH/2BSNcOS8wAAAAAAAEBIAhYzR0AAAAAF6kUlF+1A5GnBjfB/8Wrf7ZTCMLyMXWHIgIDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/NHMEQCICBjWTfgUXDYPcMhOts6bq5mcTAI5KvDi0kSxWgN7E8MAiAzwIpxowdXsIRj1TDsBY7XQBlo+zC+9j1FSXIaDkhbhAEiAgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7R0cwRAIgXSdKeIfePvrehKSjScTDb1ibVWI7ECe32m2sicF4VjQCIGoDr+u7tgifHjf6yPmZpAFRYciSAUT9UxEtoFgEzUPMAQEDBAEAAAABBCIAIJxNrLJeu4rai7sa3bhp3qTYFwzJUfHZaUshVOFYMnbJAQVHUiEDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/MhA/TUc2FOlUrE9VGOb3yzMHtPhHQ+E37U7stfT7ZDwjtHUq4iBgOlEvX1nA55AfxHjtY1Pu929E+cssGFv7z38Lm7dnFr8xgqcEdgLAAAgAAAAIAAAACAAAAAAAsAAAAiBgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7Rxida22GLAAAgAAAAIAAAACAAAAAAAsAAAAAAQAiACA8rQYZ3mfmJHp2oQKBNjXAU0V8a6T95Kwf/YFI1w5LzAEBR1IhApBtOZ9tu+zImNS43j9JdHTIakHyzzbXH5XlygawdIZ7IQJiLHl07DLeda+jczsJpsM9DepRSyn6rM+wmRd09GIiQlKuIgICYix5dOwy3nWvo3M7CabDPQ3qUUsp+qzPsJkXdPRiIkIYnWtthiwAAIAAAACAAAAAgAAAAAAMAAAAIgICkG05n2277MiY1LjeP0l0dMhqQfLPNtcfleXKBrB0hnsYKnBHYCwAAIAAAACAAAAAgAAAAAAMAAAAAA==";
  ret = CfdCreatePsbtHandle(
    handle, net_type, psbt_base64, "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    const char* pubkey = "02622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f4622242";
    ret = CfdIsFindPsbtPubkeyRecord(handle, psbt_handle, kCfdPsbtRecordOutputBip32, 0, pubkey);
    EXPECT_EQ(kCfdSuccess, ret);

    char* signature = nullptr;
    ret = CfdGetPsbtPubkeyRecord(handle, psbt_handle,
        kCfdPsbtRecordOutputBip32, 0, pubkey, &signature);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("9d6b6d862c0000800000008000000080000000000c000000", signature);
      ret = CfdFreeStringBuffer(signature);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    char* fingerprint = nullptr;
    char* path = nullptr;
    ret = CfdGetPsbtBip32Data(handle, psbt_handle, kCfdPsbtRecordOutputBip32, 0, pubkey, &fingerprint, &path);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("9d6b6d86", fingerprint);
      EXPECT_STREQ("44'/0'/0'/0/12", path);
      ret = CfdFreeStringBuffer(fingerprint);
      EXPECT_EQ(kCfdSuccess, ret);
      ret = CfdFreeStringBuffer(path);
      EXPECT_EQ(kCfdSuccess, ret);
    }
    void* pubkey_list_handle = nullptr;
    uint32_t list_num = 0;
    ret = CfdGetPsbtPubkeyList(handle, psbt_handle, kCfdPsbtRecordOutputBip32,
        0, &list_num, &pubkey_list_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, list_num);
    if (ret == kCfdSuccess) {
      char* pk = nullptr;
      fingerprint = nullptr;
      path = nullptr;
      for (uint32_t index=0; index<list_num; ++index) {
        ret = CfdGetPsbtPubkeyListData(handle, pubkey_list_handle, index, &pk, nullptr);
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret == kCfdSuccess) {
          if (index == 0) {
            EXPECT_STREQ("02622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f4622242", pk);
          } else if (index == 1) {
            EXPECT_STREQ("02906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b", pk);
          }
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
        }
        ret = CfdGetPsbtPubkeyListBip32Data(handle, pubkey_list_handle, index, &pk, &fingerprint, &path);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdcapiPsbtDumpLog(ret, handle);
        if (ret == kCfdSuccess) {
          if (index == 0) {
            EXPECT_STREQ("02622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f4622242", pk);
            EXPECT_STREQ("9d6b6d86", fingerprint);
            EXPECT_STREQ("44'/0'/0'/0/12", path);
          } else if (index == 1) {
            EXPECT_STREQ("02906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b", pk);
            EXPECT_STREQ("2a704760", fingerprint);
            EXPECT_STREQ("44'/0'/0'/0/12", path);
          }
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
          ret = CfdFreeStringBuffer(fingerprint);
          EXPECT_EQ(kCfdSuccess, ret);
          fingerprint = nullptr;
          ret = CfdFreeStringBuffer(path);
          EXPECT_EQ(kCfdSuccess, ret);
          path = nullptr;
        }
      }
      ret = CfdFreePsbtPubkeyList(handle, pubkey_list_handle);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    char* value = nullptr;
    ret = CfdGetPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeOutput, 0, "01", &value);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("522102906d399f6dbbecc898d4b8de3f497474c86a41f2cf36d71f95e5ca06b074867b2102622c7974ec32de75afa3733b09a6c33d0dea514b29faaccfb0991774f462224252ae", value);

      ret = CfdFreeStringBuffer(value);
      EXPECT_EQ(kCfdSuccess, ret);
      value = nullptr;
    }

    const char* unknown_key = "98";
    const char* exp_unknown_value = "0011223355";
    ret = CfdIsFindPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeOutput, 0, unknown_key);
    EXPECT_EQ(kCfdNotFoundError, ret);

    ret = CfdAddPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeOutput, 0, unknown_key, exp_unknown_value);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdIsFindPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeOutput, 0, unknown_key);
    EXPECT_EQ(kCfdSuccess, ret);

    value = nullptr;
    ret = CfdGetPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeOutput, 0, unknown_key, &value);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_unknown_value, value);

      ret = CfdFreeStringBuffer(value);
      EXPECT_EQ(kCfdSuccess, ret);
      value = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_psbt, GetPsbtGlobal) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  int net_type = kCfdNetworkMainnet;
  void* psbt_handle = nullptr;
  const char* psbt_base64 = "cHNidP8BAF4CAAAAAWkv08JB0xYYGWvDa23k9riJEU5CDpvd+cu+Ux5aVE1UAAAAAAD/////ARBLzR0AAAAAIgAgPK0GGd5n5iR6dqECgTY1wFNFfGuk/eSsH/2BSNcOS8wAAAAAAAEBIAhYzR0AAAAAF6kUlF+1A5GnBjfB/8Wrf7ZTCMLyMXWHIgIDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/NHMEQCICBjWTfgUXDYPcMhOts6bq5mcTAI5KvDi0kSxWgN7E8MAiAzwIpxowdXsIRj1TDsBY7XQBlo+zC+9j1FSXIaDkhbhAEiAgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7R0cwRAIgXSdKeIfePvrehKSjScTDb1ibVWI7ECe32m2sicF4VjQCIGoDr+u7tgifHjf6yPmZpAFRYciSAUT9UxEtoFgEzUPMAQEDBAEAAAABBCIAIJxNrLJeu4rai7sa3bhp3qTYFwzJUfHZaUshVOFYMnbJAQVHUiEDpRL19ZwOeQH8R47WNT7vdvRPnLLBhb+89/C5u3Zxa/MhA/TUc2FOlUrE9VGOb3yzMHtPhHQ+E37U7stfT7ZDwjtHUq4iBgOlEvX1nA55AfxHjtY1Pu929E+cssGFv7z38Lm7dnFr8xgqcEdgLAAAgAAAAIAAAACAAAAAAAsAAAAiBgP01HNhTpVKxPVRjm98szB7T4R0PhN+1O7LX0+2Q8I7Rxida22GLAAAgAAAAIAAAACAAAAAAAsAAAAAAQAiACA8rQYZ3mfmJHp2oQKBNjXAU0V8a6T95Kwf/YFI1w5LzAEBR1IhApBtOZ9tu+zImNS43j9JdHTIakHyzzbXH5XlygawdIZ7IQJiLHl07DLeda+jczsJpsM9DepRSyn6rM+wmRd09GIiQlKuIgICYix5dOwy3nWvo3M7CabDPQ3qUUsp+qzPsJkXdPRiIkIYnWtthiwAAIAAAACAAAAAgAAAAAAMAAAAIgICkG05n2277MiY1LjeP0l0dMhqQfLPNtcfleXKBrB0hnsYKnBHYCwAAIAAAACAAAAAgAAAAAAMAAAAAA==";
  ret = CfdCreatePsbtHandle(
    handle, net_type, psbt_base64, "", 0, 0, &psbt_handle);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    const char* pubkey = "xpub6EkLrUTiaMiLbMAkbLN2BdH4hWkCQT7fLQf3Q6Ymx3gAqbuFeSKHfTMVDtjcsuRtEFqJbAsjYFZMrqeDLgRSsn4yuQygK44HWPrnA7gZC2C";
    ret = CfdAddPsbtGlobalXpubkey(handle, psbt_handle,
        pubkey, "b7665978", "0/44");
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddPsbtGlobalXpubkey(handle, psbt_handle,
        "[ae05dbb7/0/44']xpub6JNQxQDHv2vcUQiXjggbaGYZg3nmxX6ojMcJPSs4KfLSLnMBCg8VbJUh5n4to2SwLWXdSXnHBkUQx1fVnJ9oKYjPPYAQehjWRpx6ErQyykX", nullptr, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdIsFindPsbtPubkeyRecord(handle, psbt_handle, kCfdPsbtRecordGloalXpub, 0, pubkey);
    EXPECT_EQ(kCfdSuccess, ret);

    char* signature = nullptr;
    ret = CfdGetPsbtPubkeyRecord(handle, psbt_handle,
        kCfdPsbtRecordGloalXpub, 0, pubkey, &signature);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("b7665978000000002c000000", signature);
      ret = CfdFreeStringBuffer(signature);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    char* fingerprint = nullptr;
    char* path = nullptr;
    ret = CfdGetPsbtBip32Data(handle, psbt_handle, kCfdPsbtRecordGloalXpub, 0, pubkey, &fingerprint, &path);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("b7665978", fingerprint);
      EXPECT_STREQ("0/44", path);
      ret = CfdFreeStringBuffer(fingerprint);
      EXPECT_EQ(kCfdSuccess, ret);
      ret = CfdFreeStringBuffer(path);
      EXPECT_EQ(kCfdSuccess, ret);
    }
    void* pubkey_list_handle = nullptr;
    uint32_t list_num = 0;
    ret = CfdGetPsbtPubkeyList(handle, psbt_handle, kCfdPsbtRecordGloalXpub,
        0, &list_num, &pubkey_list_handle);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(2, list_num);
    if (ret == kCfdSuccess) {
      char* pk = nullptr;
      char* pk_hex = nullptr;
      fingerprint = nullptr;
      path = nullptr;
      for (uint32_t index=0; index<list_num; ++index) {
        ret = CfdGetPsbtPubkeyListData(handle, pubkey_list_handle, index, &pk, &pk_hex);
        EXPECT_EQ(kCfdSuccess, ret);
        if (ret == kCfdSuccess) {
          if (index == 0) {
            EXPECT_STREQ("xpub6EkLrUTiaMiLbMAkbLN2BdH4hWkCQT7fLQf3Q6Ymx3gAqbuFeSKHfTMVDtjcsuRtEFqJbAsjYFZMrqeDLgRSsn4yuQygK44HWPrnA7gZC2C", pk);
            EXPECT_STREQ("0488b21e04a53a8ff30000002c839fb0d66f1887db167cdc530ab98e871d8b017ebcb198568874b6c98516364e03f1e767c0555ce0105b2a76d0f8b19b6d33a147f82f75a05c4c09580c39694fd3", pk_hex);
          } else if (index == 1) {
            EXPECT_STREQ("xpub6JNQxQDHv2vcUQiXjggbaGYZg3nmxX6ojMcJPSs4KfLSLnMBCg8VbJUh5n4to2SwLWXdSXnHBkUQx1fVnJ9oKYjPPYAQehjWRpx6ErQyykX", pk);
            EXPECT_STREQ("0488b21e0691fe4d298000002cb26a08008723cc8f19ac08bce635c087d63d738b63c33e62186d43cf3a5805f302e9156620b5b29e8272e86f1d81fb07d6c57c557cbc25218dfde33ab8cea06b7b", pk_hex);
          }
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
          ret = CfdFreeStringBuffer(pk_hex);
          EXPECT_EQ(kCfdSuccess, ret);
          pk_hex = nullptr;
        }
        ret = CfdGetPsbtPubkeyListBip32Data(handle, pubkey_list_handle, index, &pk, &fingerprint, &path);
        EXPECT_EQ(kCfdSuccess, ret);
        CfdcapiPsbtDumpLog(ret, handle);
        if (ret == kCfdSuccess) {
          if (index == 0) {
            EXPECT_STREQ("xpub6EkLrUTiaMiLbMAkbLN2BdH4hWkCQT7fLQf3Q6Ymx3gAqbuFeSKHfTMVDtjcsuRtEFqJbAsjYFZMrqeDLgRSsn4yuQygK44HWPrnA7gZC2C", pk);
            EXPECT_STREQ("b7665978", fingerprint);
            EXPECT_STREQ("0/44", path);
          } else if (index == 1) {
            EXPECT_STREQ("xpub6JNQxQDHv2vcUQiXjggbaGYZg3nmxX6ojMcJPSs4KfLSLnMBCg8VbJUh5n4to2SwLWXdSXnHBkUQx1fVnJ9oKYjPPYAQehjWRpx6ErQyykX", pk);
            EXPECT_STREQ("ae05dbb7", fingerprint);
            EXPECT_STREQ("0/44'", path);
          }
          ret = CfdFreeStringBuffer(pk);
          EXPECT_EQ(kCfdSuccess, ret);
          pk = nullptr;
          ret = CfdFreeStringBuffer(fingerprint);
          EXPECT_EQ(kCfdSuccess, ret);
          fingerprint = nullptr;
          ret = CfdFreeStringBuffer(path);
          EXPECT_EQ(kCfdSuccess, ret);
          path = nullptr;
        }
      }
      ret = CfdFreePsbtPubkeyList(handle, pubkey_list_handle);
      EXPECT_EQ(kCfdSuccess, ret);
    }

    char* value = nullptr;
    ret = CfdGetPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeGlobal, 0, "00", &value);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0200000001692fd3c241d31618196bc36b6de4f6b889114e420e9bddf9cbbe531e5a544d540000000000ffffffff01104bcd1d000000002200203cad0619de67e6247a76a102813635c053457c6ba4fde4ac1ffd8148d70e4bcc00000000", value);

      ret = CfdFreeStringBuffer(value);
      EXPECT_EQ(kCfdSuccess, ret);
      value = nullptr;
    }

    const char* unknown_key = "97";
    const char* exp_unknown_value = "0011223344";
    ret = CfdIsFindPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeGlobal, 0, unknown_key);
    EXPECT_EQ(kCfdNotFoundError, ret);

    ret = CfdAddPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeGlobal, 0, unknown_key, exp_unknown_value);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdIsFindPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeGlobal, 0, unknown_key);
    EXPECT_EQ(kCfdSuccess, ret);

    value = nullptr;
    ret = CfdGetPsbtRecord(handle, psbt_handle, kCfdPsbtRecordTypeGlobal, 0, unknown_key, &value);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(exp_unknown_value, value);

      ret = CfdFreeStringBuffer(value);
      EXPECT_EQ(kCfdSuccess, ret);
      value = nullptr;
    }

    ret = CfdFreePsbtHandle(handle, psbt_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}
