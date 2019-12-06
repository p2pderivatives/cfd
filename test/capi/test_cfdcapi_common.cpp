#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "cfdc/cfdcapi_common.h"
#include "capi/cfdc_internal.h"
#include "cfdcore/cfdcore_exception.h"

/**
 * @brief testing class.
 */
class cfdcapi_common : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

TEST(cfdcapi_common, CfdGetSupportedFunction) {
  uint64_t functions = 0;
  int ret = CfdGetSupportedFunction(&functions);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_EQ(1,  functions & 0x01);

  ret = CfdGetSupportedFunction(NULL);
  EXPECT_EQ(kCfdIllegalArgumentError, ret);
}

TEST(cfdcapi_common, CfdCreateHandle) {
  int ret = CfdCreateHandle(NULL);
  EXPECT_EQ(kCfdIllegalArgumentError, ret);

  void* handle = NULL;
  ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  ret = CfdFreeHandle(NULL);
  EXPECT_EQ(kCfdSuccess, ret);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_common, CfdFreeBuffer) {
  int ret = CfdFreeBuffer(NULL);
  EXPECT_EQ(kCfdSuccess, ret);

  void* buffer = malloc(1);
  ret = CfdFreeBuffer(buffer);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_common, CfdFreeStringBuffer) {
  int ret = CfdFreeStringBuffer(NULL);
  EXPECT_EQ(kCfdSuccess, ret);

  char* buffer = static_cast<char*>(malloc(1));
  ret = CfdFreeStringBuffer(buffer);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_common, CfdGetLastErrorCode) {
  int ret = CfdGetLastErrorCode(NULL);
  EXPECT_EQ(kCfdSuccess, ret);

  void* handle = NULL;
  ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  ret = CfdGetLastErrorCode(handle);
  EXPECT_EQ(kCfdSuccess, ret);

  cfd::capi::SetLastError(handle, kCfdOutOfRangeError, "dummy");
  ret = CfdGetLastErrorCode(handle);
  EXPECT_EQ(kCfdOutOfRangeError, ret);

  cfd::capi::SetLastFatalError(handle, "dummy");
  ret = CfdGetLastErrorCode(handle);
  EXPECT_EQ(kCfdUnknownError, ret);

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

TEST(cfdcapi_common, CfdGetLastErrorMessage) {
  int ret = CfdGetLastErrorMessage(NULL, NULL);
  EXPECT_EQ(kCfdIllegalArgumentError, ret);

  char* str_buffer = NULL;
  ret = CfdGetLastErrorMessage(NULL, &str_buffer);
  EXPECT_EQ(kCfdSuccess, ret);
  CfdFreeStringBuffer(str_buffer);
  str_buffer = NULL;

  void* handle = NULL;
  ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  ret = CfdGetLastErrorMessage(handle, &str_buffer);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_STREQ("", str_buffer);
  CfdFreeStringBuffer(str_buffer);
  str_buffer = NULL;

  cfd::capi::SetLastFatalError(handle, "dummy");
  ret = CfdGetLastErrorMessage(handle, &str_buffer);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_STREQ("dummy", str_buffer);
  CfdFreeStringBuffer(str_buffer);
  str_buffer = NULL;

  cfd::capi::SetLastError(handle,
    cfd::core::CfdException(cfd::core::CfdError::kCfdInternalError,
      "dummy_cfdexcept"));
  ret = CfdGetLastErrorMessage(handle, &str_buffer);
  EXPECT_EQ(kCfdSuccess, ret);
  ret = CfdGetLastErrorCode(handle);
  EXPECT_EQ(kCfdInternalError, ret);
  EXPECT_STREQ("dummy_cfdexcept", str_buffer);
  CfdFreeStringBuffer(str_buffer);
  str_buffer = NULL;

  cfd::capi::SetLastFatalError(handle, std::runtime_error("dummy_except"));
  ret = CfdGetLastErrorMessage(handle, &str_buffer);
  EXPECT_EQ(kCfdSuccess, ret);
  ret = CfdGetLastErrorCode(handle);
  EXPECT_EQ(kCfdUnknownError, ret);
  EXPECT_STREQ("dummy_except", str_buffer);
  CfdFreeStringBuffer(str_buffer);
  str_buffer = NULL;

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}

// last test.
/* comment out.
TEST(cfdcapi_common, CfdFinalize) {
  int ret = CfdInitialize();
  EXPECT_EQ(kCfdSuccess, ret);
  ret = CfdFinalize(false);
  EXPECT_EQ(kCfdSuccess, ret);
}
*/
