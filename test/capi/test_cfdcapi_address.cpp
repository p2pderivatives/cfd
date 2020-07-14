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

  {
    // p2sh-p2wsh miniscript descriptor
    const char* descriptor = "sh(wsh(c:or_i(andor(c:pk_h(xpub661MyMwAqRbcFW31YEwpkMuc5THy2PSt5bDMsktWQcFF8syAmRUapSCGu8ED9W6oDMSgv6Zz8idoc4a6mr8BDzTJY47LJhkJ8UB7WEGuduB/1/0/*),pk_h(xpub69H7F5d8KSRgmmdJg2KhpAK8SR3DjMwAdkxj3ZuxV27CprR9LgpeyGmXUbC6wb7ERfvrnKZjXoUmmDznezpbZb7ap6r1D3tgFxHmwMkQTPH/0/0/*),pk_h(02c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5)),pk_k(02d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e))))";
    int net_type = kCfdNetworkMainnet;
    void* descriptor_handle = nullptr;
    uint32_t max_index = 0;
  
    ret = CfdParseDescriptor(handle, descriptor, net_type, "44", &descriptor_handle, &max_index);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_FALSE((nullptr == descriptor_handle));
    EXPECT_EQ(1, max_index);
  
    if (ret == kCfdSuccess) {
      assert_desc_data(handle, descriptor_handle, 0, max_index, 0,
          kCfdDescriptorScriptSh, "a914a7a9f411001e3e3db96d7f02fc9ab1d0dc6aa69187", "3GyYN9WnJBoMn8M5tuqVcFJq1BvbAcdPAt",
          kCfdP2shP2wsh, "0020e29b7f3e543d581c99c92b59d45218b008b82c2d406bba3c7384d52e568124aa",
          kCfdDescriptorKeyNull, "", "", "",
          false, 0, 0);
      assert_desc_data(handle, descriptor_handle, 1, max_index, 1,
          kCfdDescriptorScriptWsh, "0020e29b7f3e543d581c99c92b59d45218b008b82c2d406bba3c7384d52e568124aa", "bc1qu2dh70j584vpexwf9dvag5sckqytstpdgp4m50rnsn2ju45pyj4qudazmh",
          kCfdP2wsh, "6376a914520e6e72bcd5b616bc744092139bd759c31d6bbe88ac6476a91406afd46bcdfd22ef94ac122aa11f241244a37ecc886776a9145ab62f0be26fe9d6205a155403f33e2ad2d31efe8868672102d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e68ac",
          kCfdDescriptorKeyNull, "", "", "",
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
  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_address, CfdGetDescriptorChecksum) {
  std::string descriptor;
  std::string checksum;
  char* output_descriptor = nullptr;
  int ret;

  descriptor = "wpkh(02f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9)";
  ret = CfdGetDescriptorChecksum(nullptr, kCfdNetworkMainnet,
      descriptor.c_str(), &output_descriptor);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == output_descriptor));
  if (kCfdSuccess == ret) {
    checksum = "#8zl0zxma";
    EXPECT_STREQ((descriptor + checksum).c_str(), output_descriptor);
    CfdFreeStringBuffer(output_descriptor);
    output_descriptor = nullptr;
  }

  descriptor = "addr(bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4)";
  ret = CfdGetDescriptorChecksum(nullptr, kCfdNetworkMainnet,
      descriptor.c_str(), &output_descriptor);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == output_descriptor));
  if (kCfdSuccess == ret) {
    checksum = "#uyjndxcw";
    EXPECT_STREQ((descriptor + checksum).c_str(), output_descriptor);
    CfdFreeStringBuffer(output_descriptor);
    output_descriptor = nullptr;
  }

#ifndef CFD_DISABLE_ELEMENTS
  descriptor = "addr(ert1q57etrknhl75e64jmqrvl0vwzu39xjpagaw9ynw)";
  ret = CfdGetDescriptorChecksum(nullptr, kCfdNetworkElementsRegtest,
      descriptor.c_str(), &output_descriptor);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == output_descriptor));
  if (kCfdSuccess == ret) {
    checksum = "#87kymh3n";
    EXPECT_STREQ((descriptor + checksum).c_str(), output_descriptor);
    CfdFreeStringBuffer(output_descriptor);
    output_descriptor = nullptr;
  }
#endif  // CFD_DISABLE_ELEMENTS
}

TEST(cfdcapi_address, CfdGetAddressInfoTest) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  struct CfdGetAddressExpectData {
    const char* address;
    const char* locking_script;
    const char* hash;
    int network;
    int hash_type;
    int witness_version;
  };
  const struct CfdGetAddressExpectData exp_datas[] = {
    { // HashType(P2PKH) mainnet
      "1PMycacnJaSqwwJqjawXBErnLsZ7RkXUAs",
      "76a914f54a5851e9372b87810a8e60cdd2e7cfd80b6e3188ac",
      "f54a5851e9372b87810a8e60cdd2e7cfd80b6e31",
      kCfdNetworkMainnet,
      kCfdP2pkh,
      kCfdWitnessVersionNone,
    },
    { // HashType(P2PKH) testnet
      "mjawtDFWiNppWUqczgQevgyg6Hg7J8Uxcg",
      "76a9142ca1d2e7214b16725cf6310867460633a061edcb88ac",
      "2ca1d2e7214b16725cf6310867460633a061edcb",
      kCfdNetworkTestnet,
      kCfdP2pkh,
      kCfdWitnessVersionNone,
    },
    { // HashType(P2SH) mainnet
      "3KxE77EHe1ip6WGRifwr9fZ5WBDGsLyWFz",
      "a914c852ac34a1c76b63a279c97502c9ccc4e3cb9e8b87",
      "c852ac34a1c76b63a279c97502c9ccc4e3cb9e8b",
      kCfdNetworkMainnet,
      kCfdP2sh,
      kCfdWitnessVersionNone,
    },
    { // HashType(P2SH) testnet
      "2NEbifo1SsiCYMQhGxGCg3tcTzR8xHuhqeH",
      "a914ea3ae70e53e6e2813002738cba26bd0cfcdecb0687",
      "ea3ae70e53e6e2813002738cba26bd0cfcdecb06",
      kCfdNetworkTestnet,
      kCfdP2sh,
      kCfdWitnessVersionNone,
    },
    { // HashType(P2SH-P2WPKH) mainnet
      "3JvL6Ymt8MVWiCNHC7oWU6nLeHNJKLZGLN",
      "a914bcfeb728b584253d5f3f70bcb780e9ef218a68f487",
      "bcfeb728b584253d5f3f70bcb780e9ef218a68f4",
      kCfdNetworkMainnet,
      kCfdP2sh,
      kCfdWitnessVersionNone,
    },
    { // HashType(P2WPKH) mainnet
      "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4",
      "0014751e76e8199196d454941c45d1b3a323f1433bd6",
      "751e76e8199196d454941c45d1b3a323f1433bd6",
      kCfdNetworkMainnet,
      kCfdP2wpkh,
      kCfdWitnessVersion0,
    },
    { // HashType(P2WPKH) testnet
      "tb1qw508d6qejxtdg4y5r3zarvary0c5xw7kxpjzsx",
      "0014751e76e8199196d454941c45d1b3a323f1433bd6",
      "751e76e8199196d454941c45d1b3a323f1433bd6",
      kCfdNetworkTestnet,
      kCfdP2wpkh,
      kCfdWitnessVersion0,
    },
    { // HashType(P2WSH) mainnet
      "bc1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3qccfmv3",
      "00201863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262",
      "1863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262",
      kCfdNetworkMainnet,
      kCfdP2wsh,
      kCfdWitnessVersion0,
    },
    { // HashType(P2WSH) testnet
      "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sl5k7",
      "00201863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262",
      "1863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262",
      kCfdNetworkTestnet,
      kCfdP2wsh,
      kCfdWitnessVersion0,
    },
#ifndef CFD_DISABLE_ELEMENTS
    { // Elements HashType(P2PKH) liquidv1
      "QKXGAM4Cvd1fvLEz5tbq4YwNRzTjdMWi2q",
      "76a914f42331c418ef4517ba644ad6e9fc99681ad4393788ac",
      "f42331c418ef4517ba644ad6e9fc99681ad43937",
      kCfdNetworkLiquidv1,
      kCfdP2pkh,
      kCfdWitnessVersionNone,
    },
    { // Elements HashType(P2SH) regtest
      "XRpicZNrFZumBMhRV5BSYW28pGX7JyY1ua",
      "a9149ec42b6cfa1b0bc3f55f07af29867057cb0b8a2e87",
      "9ec42b6cfa1b0bc3f55f07af29867057cb0b8a2e",
      kCfdNetworkElementsRegtest,
      kCfdP2sh,
      kCfdWitnessVersionNone,
    },
    { // Elements HashType(P2WPKH) regtest
      "ert1q57etrknhl75e64jmqrvl0vwzu39xjpagaw9ynw",
      "0014a7b2b1da77ffa99d565b00d9f7b1c2e44a6907a8",
      "a7b2b1da77ffa99d565b00d9f7b1c2e44a6907a8",
      kCfdNetworkElementsRegtest,
      kCfdP2wpkh,
      kCfdWitnessVersion0,
    },
    { // Elements HashType(P2WSH) liquidv1
      "ex1q6tayh53l97qhs7fr98x8msgmn82egptfhpkyn53vkt22lrxswztsgnpmxp",
      "0020d2fa4bd23f2f8178792329cc7dc11b99d5940569b86c49d22cb2d4af8cd07097",
      "d2fa4bd23f2f8178792329cc7dc11b99d5940569b86c49d22cb2d4af8cd07097",
      kCfdNetworkLiquidv1,
      kCfdP2wsh,
      kCfdWitnessVersion0,
    },
#endif  // CFD_DISABLE_ELEMENTS
  };
  size_t list_size = sizeof(exp_datas) / sizeof(struct CfdGetAddressExpectData);

  for (size_t idx = 0; idx < list_size; ++idx) {
    int network = 0;
    int hash_type = 0;
    int witness_version = 0;
    char* locking_script = nullptr;
    char* hash = nullptr;
    ret = CfdGetAddressInfo(
        handle, exp_datas[idx].address, &network, &hash_type,
        &witness_version, &locking_script, &hash);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(kCfdSuccess, ret);
      EXPECT_EQ(exp_datas[idx].network, network);
      EXPECT_EQ(exp_datas[idx].hash_type, hash_type);
      EXPECT_EQ(exp_datas[idx].witness_version, witness_version);
      EXPECT_STREQ(exp_datas[idx].locking_script, locking_script);
      EXPECT_STREQ(exp_datas[idx].hash, hash);
      CfdFreeStringBuffer(locking_script);
      CfdFreeStringBuffer(hash);
    } else {
      char* message = nullptr;
      ret = CfdGetLastErrorMessage(handle, &message);
      if (ret == kCfdSuccess) {
        EXPECT_STREQ("", message);
        CfdFreeStringBuffer(message);
      }
    }
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}