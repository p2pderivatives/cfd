#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_exception.h"

/**
 * @brief testing class.
 */
class cfdcapi_elements_transaction : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_elements_transaction, CfdGetSupportedFunction) {
  uint64_t functions = 0;
  int ret = CfdGetSupportedFunction(&functions);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(1,  functions & 0x01);

  ret = CfdGetSupportedFunction(NULL);
  EXPECT_EQ(kCfdIllegalArgumentError, ret);
}
