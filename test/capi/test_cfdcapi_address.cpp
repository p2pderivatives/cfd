#include "gtest/gtest.h"

#include <string>
#include <stddef.h>

#include "capi/cfdc_internal.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_address.h"

/**
 * @brief testing class.
 */
class cfdcapi_address : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_address, CfdParseDescriptorTest) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  auto asset_cstr_eq = [](std::string expect, const char* actual) {
    if (expect.empty()) {
      EXPECT_STREQ(NULL, actual);
    } else {
      EXPECT_STREQ(expect.c_str(), actual);
    }
  };

  auto assert_desc_data = [&asset_cstr_eq](
      void* handle, void* descriptor_handle, uint32_t index,
      uint32_t exp_max_index, uint32_t exp_depth, int exp_script_type,
      std::string exp_locking_script, std::string exp_address, int exp_hash_type, std::string exp_redeem_script,
      int exp_key_type, std::string exp_pubkey, std::string exp_ext_pubkey,
      std::string exp_ext_privkey, bool exp_is_multisig, uint32_t exp_max_key_num,
      uint32_t exp_req_sig_num) {
    uint32_t depth = 0, max_index = 0, max_key_num = 0, req_sig_num = 0;
    int script_type = 0, hash_type = 0, key_type = 0;
    char* locking_script = nullptr;
    char* address = nullptr;
    char* redeem_script = nullptr;
    char* pubkey = nullptr;
    char* ext_pubkey = nullptr;
    char* ext_privkey = nullptr;
    bool is_multisig = false;

    int ret = CfdGetDescriptorData(handle, descriptor_handle, index, &max_index, &depth,
        &script_type, &locking_script, &address, &hash_type, &redeem_script,
        &key_type, &pubkey, &ext_pubkey, &ext_privkey, &is_multisig,
        &max_key_num, &req_sig_num);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(exp_max_index, max_index);
    EXPECT_EQ(exp_depth, depth);
    EXPECT_EQ(exp_script_type, script_type);
    asset_cstr_eq(exp_locking_script, locking_script);
    asset_cstr_eq(exp_address, address);
    EXPECT_EQ(exp_hash_type, hash_type);
    asset_cstr_eq(exp_redeem_script, redeem_script);
    EXPECT_EQ(exp_key_type, key_type);
    asset_cstr_eq(exp_pubkey, pubkey);
    asset_cstr_eq(exp_ext_pubkey, ext_pubkey);
    asset_cstr_eq(exp_ext_privkey, ext_privkey);
    EXPECT_EQ(exp_is_multisig, is_multisig);
    EXPECT_EQ(exp_max_key_num, max_key_num);
    EXPECT_EQ(exp_req_sig_num, req_sig_num);
    CfdFreeStringBuffer(locking_script);
    CfdFreeStringBuffer(address);
    CfdFreeStringBuffer(redeem_script);
    CfdFreeStringBuffer(pubkey);
    CfdFreeStringBuffer(ext_pubkey);
    CfdFreeStringBuffer(ext_privkey);
  };

  auto assert_multisig_keys = [&asset_cstr_eq](
      void* handle, void* descriptor_handle, uint32_t key_index,
      uint32_t exp_key_type, std::string exp_pubkey, std::string exp_ext_pubkey,
      std::string exp_ext_privkey) {
    int key_type = 0;
    char* pubkey = nullptr;
    char* ext_pubkey = nullptr;
    char* ext_privkey = nullptr;

    int ret = CfdGetDescriptorMultisigKey(handle, descriptor_handle, key_index,
        &key_type, &pubkey, &ext_pubkey, &ext_privkey);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_EQ(exp_key_type, key_type);
    asset_cstr_eq(exp_pubkey, pubkey);
    asset_cstr_eq(exp_ext_pubkey, ext_pubkey);
    asset_cstr_eq(exp_ext_privkey, ext_privkey);
    CfdFreeStringBuffer(pubkey);
    CfdFreeStringBuffer(ext_pubkey);
    CfdFreeStringBuffer(ext_privkey);
  };

  // p2ms(p2sh multi) descriptor
  {
    const char* descriptor = "wsh(multi(1,xpub661MyMwAqRbcFW31YEwpkMuc5THy2PSt5bDMsktWQcFF8syAmRUapSCGu8ED9W6oDMSgv6Zz8idoc4a6mr8BDzTJY47LJhkJ8UB7WEGuduB/1/0/*,xpub69H7F5d8KSRgmmdJg2KhpAK8SR3DjMwAdkxj3ZuxV27CprR9LgpeyGmXUbC6wb7ERfvrnKZjXoUmmDznezpbZb7ap6r1D3tgFxHmwMkQTPH/0/0/*))";
    int net_type = kCfdNetworkMainnet;
    void* descriptor_handle = nullptr;
    uint32_t max_index = 0;
  
    ret = CfdParseDescriptor(handle, descriptor, net_type, "0", &descriptor_handle, &max_index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_FALSE((nullptr == descriptor_handle));
    EXPECT_EQ(0, max_index);
  
    if (ret == kCfdSuccess) {
      assert_desc_data(handle, descriptor_handle, 0, max_index, 0,
          kCfdDescriptorScriptWsh, "002064969d8cdca2aa0bb72cfe88427612878db98a5f07f9a7ec6ec87b85e9f9208b", "bc1qvjtfmrxu524qhdevl6yyyasjs7xmnzjlqlu60mrwepact60eyz9s9xjw0c",
          kCfdP2wsh, "51210205f8f73d8a553ad3287a506dbd53ed176cadeb200c8e4f7d68a001b1aed871062102c04c4e03921809fcbef9a26da2d62b19b2b4eb383b3e6cfaaef6370e7514477452ae",
          kCfdDescriptorKeyNull, "", "", "",
          true, 2, 1);

      // key data
      assert_multisig_keys(handle, descriptor_handle, 0,
          kCfdDescriptorKeyBip32,
          "0205f8f73d8a553ad3287a506dbd53ed176cadeb200c8e4f7d68a001b1aed87106",
          "xpub6BgWskLoyHmAUeKWgUXCGfDdCMRXseEjRCMEMvjkedmHpnvWtpXMaCRm8qcADw9einPR8o2c49ZpeHRZP4uYwGeMU2T63G7uf2Y1qJavrWQ",
          "");
      assert_multisig_keys(handle, descriptor_handle, 1,
          kCfdDescriptorKeyBip32,
          "02c04c4e03921809fcbef9a26da2d62b19b2b4eb383b3e6cfaaef6370e75144774",
          "xpub6EKMC2gSMfKgQJ3iNMZVNB4GLH1Dc4hNPah1iMbbztxdUPRo84MMcTgkPATWNRyzr7WifKrt5VvQi4GEqRwybCP1LHoXBKLN6cB15HuBKPE",
          "");
    }
    ret = CfdFreeDescriptorHandle(handle, descriptor_handle);
    ASSERT_EQ(ret, kCfdSuccess);

    ret = CfdGetLastErrorCode(handle);
    if (ret != kCfdSuccess) {
      char* str_buffer = NULL;
      ret = CfdGetLastErrorMessage(handle, &str_buffer);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_STREQ("", str_buffer);
      CfdFreeStringBuffer(str_buffer);
      str_buffer = NULL;
    }
  }

#ifndef CFD_DISABLE_ELEMENTS
  {
    // pkh descriptor
    const char* descriptor = "pkh(02c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5)";
    int net_type = kCfdNetworkLiquidv1;
    void* descriptor_handle = nullptr;
    uint32_t max_index = 0;
  
    ret = CfdParseDescriptor(handle, descriptor, net_type, "", &descriptor_handle, &max_index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_FALSE((nullptr == descriptor_handle));
    EXPECT_EQ(0, max_index);
  
    if (ret == kCfdSuccess) {
      assert_desc_data(handle, descriptor_handle, 0, max_index, 0,
          kCfdDescriptorScriptPkh, "76a91406afd46bcdfd22ef94ac122aa11f241244a37ecc88ac", "PwsjpD1YkjcfZ95WGVZuvGfypkKmpogoA3",
          kCfdP2pkh, "",
          kCfdDescriptorKeyPublic, "02c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5", "", "",
          false, 0, 0);
    }
    ret = CfdFreeDescriptorHandle(handle, descriptor_handle);
    ASSERT_EQ(ret, kCfdSuccess);

    ret = CfdGetLastErrorCode(handle);
    if (ret != kCfdSuccess) {
      char* str_buffer = NULL;
      ret = CfdGetLastErrorMessage(handle, &str_buffer);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_STREQ("", str_buffer);
      CfdFreeStringBuffer(str_buffer);
      str_buffer = NULL;
    }
  }

  {
    // p2sh-p2wsh(pkh) descriptor
    const char* descriptor = "sh(wsh(pkh(02e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13)))";
    int net_type = kCfdNetworkLiquidv1;
    void* descriptor_handle = nullptr;
    uint32_t max_index = 0;
  
    ret = CfdParseDescriptor(handle, descriptor, net_type, "", &descriptor_handle, &max_index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_FALSE((nullptr == descriptor_handle));
    EXPECT_EQ(2, max_index);
  
    if (ret == kCfdSuccess) {
      assert_desc_data(handle, descriptor_handle, 0, max_index, 0,
          kCfdDescriptorScriptSh, "a91455e8d5e8ee4f3604aba23c71c2684fa0a56a3a1287", "Gq1mmExLuSEwfzzk6YtUxJ769grv6T5Tak",
          kCfdP2shP2wsh, "0020fc5acc302aab97f821f9a61e1cc572e7968a603551e95d4ba12b51df6581482f",
          kCfdDescriptorKeyNull, "", "", "",
          false, 0, 0);
      assert_desc_data(handle, descriptor_handle, 1, max_index, 1,
          kCfdDescriptorScriptWsh, "0020fc5acc302aab97f821f9a61e1cc572e7968a603551e95d4ba12b51df6581482f", "ex1ql3dvcvp24wtlsg0e5c0pe3tju7tg5cp428546jap9dga7evpfqhs0htdlf",
          kCfdP2wsh, "76a914c42e7ef92fdb603af844d064faad95db9bcdfd3d88ac",
          kCfdDescriptorKeyNull, "", "", "",
          false, 0, 0);
      assert_desc_data(handle, descriptor_handle, 2, max_index, 2,
          kCfdDescriptorScriptPkh, "76a914c42e7ef92fdb603af844d064faad95db9bcdfd3d88ac", "QF9hGPQMVAPc8RxTHALgSvNPWEjGbL9bse",
          kCfdP2pkh, "",
          kCfdDescriptorKeyPublic, "02e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13", "", "",
          false, 0, 0);
    }
    ret = CfdFreeDescriptorHandle(handle, descriptor_handle);
    ASSERT_EQ(ret, kCfdSuccess);

    ret = CfdGetLastErrorCode(handle);
    if (ret != kCfdSuccess) {
      char* str_buffer = NULL;
      ret = CfdGetLastErrorMessage(handle, &str_buffer);
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_STREQ("", str_buffer);
      CfdFreeStringBuffer(str_buffer);
      str_buffer = NULL;
    }
  }
#endif  // CFD_DISABLE_ELEMENTS
  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}