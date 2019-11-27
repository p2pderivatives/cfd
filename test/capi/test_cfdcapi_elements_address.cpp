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

  char* tx_string = nullptr;
  char* tx_string2 = nullptr;
  int64_t satoshi;
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

#endif  // CFD_DISABLE_ELEMENTS
