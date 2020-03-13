#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_key.h"
#include "cfdc/cfdcapi_address.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_exception.h"

/**
 * @brief testing class.
 */
class cfdcapi_key : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_key, CfdCalculateEcSignature) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static constexpr const char* kSighash = "9b169f5af064cc2a0dac08d8be3c9e8bc3d3e1a3f3e2a44f0c3e4ecf23d56cf2";
  static constexpr const char* kPrivkey = "cU4KjNUT7GjHm7CkjRjG46SzLrXHXoH3ekXmqa2jTCFPMkQ64sw1";
  static constexpr const char* kExtSignature = "0bc7f08a2a8a5446e7483db1b46184ba3cc79d78a3452a72c5bc712cc7efb51f58af044d646c1fd4f755d49db26faa203937bc66c569047a7d3d3da531826060";
  static constexpr const int kNetwork = kCfdNetworkRegtest;

  char* signature = nullptr;
  ret = CfdCalculateEcSignature(handle, kSighash, nullptr, kPrivkey,
    kNetwork, true, &signature);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ(kExtSignature, signature);
    CfdFreeStringBuffer(signature);
    signature = NULL;
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

TEST(cfdcapi_key, CfdEncodeSignatureByDer) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static constexpr const char* kSignature = "47ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb";
  static constexpr const char* kDerSignature = "3044022047ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f0220217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb01";

  char* der_signature = nullptr;
  ret = CfdEncodeSignatureByDer(handle, kSignature, kCfdSigHashAll, false, &der_signature);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ(kDerSignature, der_signature);
    CfdFreeStringBuffer(der_signature);
    der_signature = NULL;
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

TEST(cfdcapi_key, CfdDecodeSignatureFromDer) {
  static constexpr const char* kSignature = "47ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb";
  static constexpr const char* kDerSignature = "3044022047ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f0220217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb01";

  void* handle = nullptr;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((nullptr == handle));

  char* signature = nullptr;
  int sighash = 0;
  bool is_anyone_can_pay = false;
  ret = CfdDecodeSignatureFromDer(handle, kDerSignature, &signature,
      &sighash, &is_anyone_can_pay);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ(kSignature, signature);
    EXPECT_EQ(kCfdSigHashAll, sighash);
    EXPECT_FALSE(is_anyone_can_pay);
    CfdFreeStringBuffer(signature);
    signature = nullptr;
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

TEST(cfdcapi_key, CfdNormalizeSignatureTest) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  // FIXME(fujita-cg): replace test data
  // this test data comes from https://www.pebblewind.com/entry/2018/04/27/232427
  if (ret == kCfdSuccess) {
    const char* signature = "c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5f67f6cf81a19873091aa7c9578fa2e96490e9bfc78ae7e9798004e8252c06287";
    char* normalized;
    ret = CfdNormalizeSignature(handle, signature, &normalized);
    EXPECT_EQ(kCfdSuccess, ret);
    const char* expect = "c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee509809307e5e678cf6e55836a8705d16871a040ea369a21a427d2100a7d75deba";
    EXPECT_STREQ(normalized, expect);
    if (ret == kCfdSuccess) {
      CfdFreeStringBuffer(normalized);
      normalized = NULL;
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

TEST(cfdcapi_key, PrivkeyAndPubkeyTest) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static constexpr const int kNetwork = kCfdNetworkRegtest;

  bool is_compressed = false;
  int network_type = kCfdNetworkRegtest;
  char* pubkey = nullptr;
  char* pubkey2 = nullptr;
  char* privkey = nullptr;
  char* privkey2 = nullptr;
  char* wif = nullptr;
  char* wif2 = nullptr;
  ret = CfdCreateKeyPair(handle, true, kNetwork, &pubkey, &privkey, &wif);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_EQ(66, strlen(pubkey));
    EXPECT_EQ(64, strlen(privkey));
    EXPECT_EQ(52, strlen(wif));

    ret = CfdGetPrivkeyFromWif(handle, wif, kNetwork, &privkey2);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(privkey, privkey2);
      CfdFreeStringBuffer(privkey2);
    }

    ret = CfdGetPubkeyFromPrivkey(handle, privkey, nullptr, true, &pubkey2);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(pubkey, pubkey2);
      CfdFreeStringBuffer(pubkey2);
    }

    CfdFreeStringBuffer(pubkey);
    CfdFreeStringBuffer(privkey);
    CfdFreeStringBuffer(wif);
  }

  ret = CfdCreateKeyPair(handle, false, kNetwork, &pubkey, &privkey, &wif);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_EQ(130, strlen(pubkey));
    EXPECT_EQ(64, strlen(privkey));
    EXPECT_EQ(51, strlen(wif));

    ret = CfdGetPrivkeyFromWif(handle, wif, kNetwork, &privkey2);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(privkey, privkey2);
      CfdFreeStringBuffer(privkey2);
    }

    ret = CfdParsePrivkeyWif(
        handle, wif, &privkey2, &network_type, &is_compressed);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_EQ(network_type, kCfdNetworkTestnet);
      EXPECT_FALSE(is_compressed);
      EXPECT_STREQ(privkey, privkey2);
      CfdFreeStringBuffer(privkey2);
    }

    ret = CfdGetPrivkeyWif(handle, privkey, kNetwork, false, &wif2);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(wif, wif2);
      CfdFreeStringBuffer(wif2);
    }

    ret = CfdGetPubkeyFromPrivkey(handle, privkey, nullptr, false, &pubkey2);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(pubkey, pubkey2);
      CfdFreeStringBuffer(pubkey2);
    }

    CfdFreeStringBuffer(pubkey);
    CfdFreeStringBuffer(privkey);
    CfdFreeStringBuffer(wif);
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

TEST(cfdcapi_key, ExtkeyTest) {
  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  static constexpr const char* kSeed = "0e09fbdd00e575b654d480ae979f24da45ef4dee645c7dc2e3b30b2e093d38dda0202357754cc856f8920b8e31dd02e9d34f6a2b20dc825c6ba90f90009085e1";
  static constexpr const int kNetwork = kCfdNetworkMainnet;

  char* pubkey = nullptr;
  char* privkey = nullptr;
  char* wif = nullptr;
  char* extprivkey1 = nullptr;
  char* extprivkey2 = nullptr;
  char* extprivkey3 = nullptr;
  char* extprivkey4 = nullptr;
  char* extpubkey1 = nullptr;
  char* key_path_data = nullptr;
  ret = CfdCreateExtkeyFromSeed(
      handle, kSeed, kNetwork, kCfdExtPrivkey, &extprivkey1);
  EXPECT_EQ(kCfdSuccess, ret);
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("xprv9s21ZrQH143K38XAstQ4D3hCGbgydJgNff6CcwmkrWTBxksb2G4CsqAywJCKbTdywfCpmpJyxqf77iKK1ju1J982iP2PriifaNZLMbyPQCx", extprivkey1);

    ret = CfdCreateExtkeyFromParentPath(handle, extprivkey1, "m/44'",
        kNetwork, kCfdExtPrivkey, &extprivkey2);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("xprv9tviYANkXM1CY831VtMFKFn6LP6aMHf1kvtCZyTL9YbyMwTR2BSmJaEoqw59BZdQhLSx9ZxyKsRUeCetxA2xZ34eupBqZUsifnWyLJJ16j3", extprivkey2);

    ret = CfdCreateExtPubkey(handle, extprivkey2, kNetwork,
        &extpubkey1);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("xpub67v4wfueMiZVkc7UbutFgPiptQw4kkNs89ooNMrwht8xEjnZZim1rNZHhEdrLejB99fiBdnWNNAB8hmUK7tCo5Ua6UtHzwVLj2Bzpch7vB2", extpubkey1);

    ret = CfdCreateExtkeyFromParentPath(handle, extprivkey2, "0h/0h/2",
        kNetwork, kCfdExtPrivkey, &extprivkey3);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("xprvA1YYKkMiZaDHRY4dmXjcP3js7ATJQAwt9gozTvi69etziyBAAENQN4w7sS3uBaF7rgXvP3sUtKFju7p3PosjNkRDuqqSFfxTjjEhgx6ejVZ", extprivkey3);

    ret = CfdGetPrivkeyFromExtkey(handle, extprivkey3,
        kNetwork, &privkey, &wif);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("597da1afc4218445ba9428c1c790a30fd21d5c4a932fa580b99dda7ec0887472", privkey);
    EXPECT_STREQ("KzDfmSzt1XqZh5m4sQPBqhpiTGncQ2xvXuWnKGMqR9gVHGSbVJP2", wif);

    ret = CfdGetPubkeyFromExtkey(handle, extprivkey3,
        kNetwork, &pubkey);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("031d7463018f867de51a27db866f869ceaf52abab71827a6051bab8a0fd020f4c1", pubkey);

    ret = CfdGetParentExtkeyPathData(
        handle, extprivkey2, "0h/0h/2", kCfdExtPrivkey, &key_path_data, &extprivkey4);
    EXPECT_EQ(kCfdSuccess, ret);
  }
  if (ret == kCfdSuccess) {
    EXPECT_STREQ("[03af54a0/0h/0h/2]", key_path_data);
    EXPECT_STREQ(extprivkey3, extprivkey4);

    char* version = nullptr;
    char* fingerprint = nullptr;
    char* chain_code = nullptr;
    uint32_t depth = 0;
    uint32_t child_number = 0;
    ret = CfdGetExtkeyInformation(handle, extprivkey2, &version,
        &fingerprint, &chain_code, &depth, &child_number);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ("0488ade4", version);
      EXPECT_STREQ("03af54a0", fingerprint);
      EXPECT_STREQ("16ddac07d3c3110f0292136af4bc476323e87b6da49ac0b8eef5bcde17e8a672", chain_code);
      EXPECT_EQ(1, depth);
      EXPECT_EQ(2147483692, child_number);
      CfdFreeStringBuffer(version);
      CfdFreeStringBuffer(fingerprint);
      CfdFreeStringBuffer(chain_code);
    }
  }

  CfdFreeStringBuffer(extprivkey1);
  CfdFreeStringBuffer(extprivkey2);
  CfdFreeStringBuffer(extprivkey3);
  CfdFreeStringBuffer(extpubkey1);
  CfdFreeStringBuffer(privkey);
  CfdFreeStringBuffer(wif);
  CfdFreeStringBuffer(pubkey);

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
