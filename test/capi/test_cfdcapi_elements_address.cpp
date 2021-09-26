#ifndef CFD_DISABLE_ELEMENTS
#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_elements_address.h"
#include "cfdc/cfdcapi_transaction.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_exception.h"

/**
 * @brief testing class.
 */
class cfdcapi_elements_address : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_elements_address, ConfidentialAddress) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static const char* kAddress = "Q7wegLt2qMGhm28vch6VTzvpzs8KXvs4X7";
  static const char* kConfidentialKey = "025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357";
  static const char* kConfidentialAddr = "VTpvKKc1SNmLG4H8CnR1fGJdHdyWGEQEvdP9gfeneJR7n81S5kiwNtgF7vrZjC8mp63HvwxM81nEbTxU";
  static int kNetworkType = kCfdNetworkLiquidv1;

  char* confidential_address = nullptr;
  ret = CfdCreateConfidentialAddress(
      handle, kAddress, kConfidentialKey, &confidential_address);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ(kConfidentialAddr, confidential_address);

    char* addr = nullptr;
    char* key = nullptr;
    int net_type = kCfdNetworkMainnet;
    ret = CfdParseConfidentialAddress(
        handle, confidential_address, &addr, &key, &net_type);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(kAddress, addr);
      EXPECT_STREQ(kConfidentialKey, key);
      EXPECT_EQ(kNetworkType, net_type);
      CfdFreeStringBuffer(addr);
      CfdFreeStringBuffer(key);
    }

    CfdFreeStringBuffer(confidential_address);
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

TEST(cfdcapi_elements_address, GetPeginAddress) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  struct CfdCapiTestPeginAddressData {
    const char* name;
    const char* fedpeg_script;
    const char* pubkey;
    const char* redeem_script;
    int net_type;
    int hash_type;
    const char* exp_address;
    const char* exp_claim_script;
    const char* exp_tweaked_fedpeg_script;
  };

  std::vector<CfdCapiTestPeginAddressData> test_case = {
    {
      "pubkey - p2sh-p2wsh - mainnet",
      "522102baae8e066e4f2a1da4b731017697bb8fcacc60e4569f3ec27bc31cf3fb13246221026bccd050e8ecf7a702bc9fb63205cfdf278a22ba8b1f1d3ca3d8e5b38465a9702103430d354b89d1fbe43eb54ea138a4aee1076e4c54f4c805f62f9cee965351a1d053ae",
      "027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af",
      nullptr,
      kCfdNetworkMainnet,
      kCfdP2shP2wsh,
      "39cTKhjjh9YWDQT5hhSRkQwjvmpc4d1C7k",
      "0014925d4028880bd0c9d68fbc7fc7dfee976698629c",
      "522103e3b215b75e015a5948efb043079d325a90e68b19112211ae3c1ff62366d441732102779396d5c2348c33bcbdcfd87bf59646ccbebc94bacf4750a9c5245dd297213021036416a1c936d3dc84747d5e544c200578cccfb6ec62dda48df79a0a6a8c7e63fa53ae"
    },
    {
      "pubkey - p2wsh - regtest",
      "522102baae8e066e4f2a1da4b731017697bb8fcacc60e4569f3ec27bc31cf3fb13246221026bccd050e8ecf7a702bc9fb63205cfdf278a22ba8b1f1d3ca3d8e5b38465a9702103430d354b89d1fbe43eb54ea138a4aee1076e4c54f4c805f62f9cee965351a1d053ae",
      "027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af",
      nullptr,
      kCfdNetworkRegtest,
      kCfdP2wsh,
      "bcrt1q0hjhyc0gz7uvpxu9gnu3l2052sgu2petwvxd3k205utwvvg2m8uskdv2fw",
      "0014925d4028880bd0c9d68fbc7fc7dfee976698629c",
      "522103e3b215b75e015a5948efb043079d325a90e68b19112211ae3c1ff62366d441732102779396d5c2348c33bcbdcfd87bf59646ccbebc94bacf4750a9c5245dd297213021036416a1c936d3dc84747d5e544c200578cccfb6ec62dda48df79a0a6a8c7e63fa53ae"
    },
    {
      "redeemScript - p2wsh - regtest",
      "522102baae8e066e4f2a1da4b731017697bb8fcacc60e4569f3ec27bc31cf3fb13246221026bccd050e8ecf7a702bc9fb63205cfdf278a22ba8b1f1d3ca3d8e5b38465a9702103430d354b89d1fbe43eb54ea138a4aee1076e4c54f4c805f62f9cee965351a1d053ae",
      nullptr,
      "76a914c3bec9f6c51c3d1f0fd00c54fe41b5d8014ed8eb88ac",
      kCfdNetworkRegtest,
      kCfdP2wsh,
      "bcrt1q9utw2ueyxk4qt2dtgkxpq0fl2taxahzryl7eaxxqqnkn8c8fl5es03ts9p",
      "00209977a2e4cab77248f01bc51a671078b21bf732f87221533e8650f63380d761f8",
      "522103d3fb966979e40c3469129d28925cabe99f17e670d9a97a4fd90a63b0f9c1f9ec210388cfea4c5025108e117a6e6de4c53df7aa524da943f9f13afe1654cd17f46d7c210293e1513108d40b612ea15f2154179333fa6fb3ec25d821047b62d303a286ed6c53ae"
    },
    {
      "redeemScript - p2sh-p2wsh - mainnet",
      "522102baae8e066e4f2a1da4b731017697bb8fcacc60e4569f3ec27bc31cf3fb13246221026bccd050e8ecf7a702bc9fb63205cfdf278a22ba8b1f1d3ca3d8e5b38465a9702103430d354b89d1fbe43eb54ea138a4aee1076e4c54f4c805f62f9cee965351a1d053ae",
      nullptr,
      "522103a7bd50beb3aff9238336285c0a790169eca90b7ad807abc4b64897ca1f6dedb621039cbaf938d050dd2582e4c2f56d1f75cfc9d165f2f3270532363d9871fb7be14252ae",
      kCfdNetworkMainnet,
      kCfdP2shP2wsh,
      "3DZHAW3TmdwfGuJTGKatD7XpCNJvnX6GiE",
      "0020c45384fa00fe363ed60968fff46541c89bc1766686c279ffdf0a335b80cad728",
      "52210272d86fcc18fc129a3fe72ed268356735a176f01ba1bb6b5a6e5181735570fca021021909156e0a206a5a8f47bee2418eebd6db0ecae9b4810d761117fa7891f86f7021026e90023fe74aff9f5a26c76ca88eb19fd4477ae43cebb9d2e81e197961b263b753ae"
    }
  };

  for (const auto& test_data : test_case) {
    SCOPED_TRACE(test_data.name);

    char* pegin_address = nullptr;
    char* claim_script = nullptr;
    char* tweaked_fedpeg_script = nullptr;
    ret = CfdGetPeginAddress(
        handle, test_data.net_type, test_data.fedpeg_script,
        test_data.hash_type, test_data.pubkey, test_data.redeem_script,
        &pegin_address, &claim_script, &tweaked_fedpeg_script);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(test_data.exp_address, pegin_address);
      EXPECT_STREQ(test_data.exp_claim_script, claim_script);
      EXPECT_STREQ(test_data.exp_tweaked_fedpeg_script, tweaked_fedpeg_script);
      CfdFreeStringBuffer(pegin_address);
      CfdFreeStringBuffer(claim_script);
      CfdFreeStringBuffer(tweaked_fedpeg_script);
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

TEST(cfdcapi_elements_address, GetPegoutAddress) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  struct CfdCapiTestPegoutAddressData {
    const char* name;
    const char* descriptor;
    uint32_t counter;
    int mainchain_net_type;
    int elements_net_type;
    int address_type;
    const char* exp_address;
    const char* exp_base_descriptor;
  };

  std::vector<CfdCapiTestPegoutAddressData> test_case = {
    {
      "extkey - p2pkh - regtest",
      "tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2pkhAddress,
      "n3Na7mek1zAStRxvt7RPrNCpZhDErMPkGw",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "descriptor - p2pkh - regtest",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2pkhAddress,
      "n3Na7mek1zAStRxvt7RPrNCpZhDErMPkGw",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "deriveDescriptor - p2pkh - regtest",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y/0/*)",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2pkhAddress,
      "n3Na7mek1zAStRxvt7RPrNCpZhDErMPkGw",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "extkey - p2pkh - mainnet",
      "xpub67v4wfueMiZVkc7UbutFgPiptQw4kkNs89ooNMrwht8xEjnZZim1rNZHhEdrLejB99fiBdnWNNAB8hmUK7tCo5Ua6UtHzwVLj2Bzpch7vB2",
      0,
      kCfdNetworkMainnet,
      kCfdNetworkLiquidv1,
      kCfdP2pkhAddress,
      "1MMxsm4QG8NRHqaFZaUTFQQ9c9dEHUPWnD",
      "pkh(xpub67v4wfueMiZVkc7UbutFgPiptQw4kkNs89ooNMrwht8xEjnZZim1rNZHhEdrLejB99fiBdnWNNAB8hmUK7tCo5Ua6UtHzwVLj2Bzpch7vB2)"
    },
    {
      "descriptor - p2pkh - mainnet",
      "pkh(xpub67v4wfueMiZVkc7UbutFgPiptQw4kkNs89ooNMrwht8xEjnZZim1rNZHhEdrLejB99fiBdnWNNAB8hmUK7tCo5Ua6UtHzwVLj2Bzpch7vB2)",
      0,
      kCfdNetworkMainnet,
      kCfdNetworkLiquidv1,
      kCfdP2pkhAddress,
      "1MMxsm4QG8NRHqaFZaUTFQQ9c9dEHUPWnD",
      "pkh(xpub67v4wfueMiZVkc7UbutFgPiptQw4kkNs89ooNMrwht8xEjnZZim1rNZHhEdrLejB99fiBdnWNNAB8hmUK7tCo5Ua6UtHzwVLj2Bzpch7vB2)"
    },
    {
      "deriveDescriptor - p2pkh - mainnet",
      "pkh(xpub67v4wfueMiZVkc7UbutFgPiptQw4kkNs89ooNMrwht8xEjnZZim1rNZHhEdrLejB99fiBdnWNNAB8hmUK7tCo5Ua6UtHzwVLj2Bzpch7vB2/0/*)",
      0,
      kCfdNetworkMainnet,
      kCfdNetworkLiquidv1,
      kCfdP2pkhAddress,
      "1MMxsm4QG8NRHqaFZaUTFQQ9c9dEHUPWnD",
      "pkh(xpub67v4wfueMiZVkc7UbutFgPiptQw4kkNs89ooNMrwht8xEjnZZim1rNZHhEdrLejB99fiBdnWNNAB8hmUK7tCo5Ua6UtHzwVLj2Bzpch7vB2)"
    },
    {
      "extkey - p2wpkh - regtest",
      "tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2wpkhAddress,
      "bcrt1qa77w63m523kq82z4fn3d5f7qxqxfm4pmdthkdf",
      "wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "descriptor - p2wpkh - regtest",
      "wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2wpkhAddress,
      "bcrt1qa77w63m523kq82z4fn3d5f7qxqxfm4pmdthkdf",
      "wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "deriveDescriptor - p2wpkh - regtest",
      "wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y/0/*)",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2wpkhAddress,
      "bcrt1qa77w63m523kq82z4fn3d5f7qxqxfm4pmdthkdf",
      "wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "extkey - p2sh-p2wpkh - regtest",
      "tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2shP2wpkhAddress,
      "2N8UxQ5u9YXYFn6Ukj5KGXCMDUZTixKTXHo",
      "sh(wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y))"
    },
    {
      "descriptor - p2sh-p2wpkh - regtest",
      "sh(wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y))",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2shP2wpkhAddress,
      "2N8UxQ5u9YXYFn6Ukj5KGXCMDUZTixKTXHo",
      "sh(wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y))"
    },
    {
      "deriveDescriptor - p2sh-p2wpkh - regtest",
      "sh(wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y/0/*))",
      0,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2shP2wpkhAddress,
      "2N8UxQ5u9YXYFn6Ukj5KGXCMDUZTixKTXHo",
      "sh(wpkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y))"
    },
    {
      "extkey - p2pkh - regtest - count 1",
      "tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y",
      1,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2pkhAddress,
      "mjii5Qey7jLxtvsV5f6BaFeVaqfeqcKXzL",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "descriptor - p2pkh - regtest - count 1",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)",
      1,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2pkhAddress,
      "mjii5Qey7jLxtvsV5f6BaFeVaqfeqcKXzL",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "deriveDescriptor - p2pkh - regtest - count 1",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y/0/*)",
      1,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2pkhAddress,
      "mjii5Qey7jLxtvsV5f6BaFeVaqfeqcKXzL",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
    {
      "deriveDescriptor - p2pkh - regtest - count max",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y/0/*)",
      0x7fffffff,
      kCfdNetworkRegtest,
      kCfdNetworkElementsRegtest,
      kCfdP2pkhAddress,
      "mfkLVoxUp9K2Q1KeRr3Ewnn414jLSCTGmn",
      "pkh(tpubDASgDECJvTMzUgS7GkSCxQAAWPveW7BeTPSvbi1wpUe1Mq1v743FRw1i7vTavjAb3D3Y8geCTYw2ezgiVS7SFXDXS6NpZmvr6XPjPvg632y)"
    },
  };

  for (const auto& test_data : test_case) {
    SCOPED_TRACE(test_data.name);

    char* mainchain_address = nullptr;
    char* base_descriptor = nullptr;
    ret = CfdGetPegoutAddress(
        handle, test_data.mainchain_net_type, test_data.elements_net_type,
        test_data.descriptor, test_data.counter, test_data.address_type,
        &mainchain_address, &base_descriptor);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(test_data.exp_address, mainchain_address);
      EXPECT_STREQ(test_data.exp_base_descriptor, base_descriptor);
      CfdFreeStringBuffer(mainchain_address);
      CfdFreeStringBuffer(base_descriptor);
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

#endif  // CFD_DISABLE_ELEMENTS
