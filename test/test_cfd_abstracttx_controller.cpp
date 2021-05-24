#include "gtest/gtest.h"

#include "cfd/cfd_transaction_common.h"
#include "cfdcore/cfdcore_transaction.h"

using cfd::AbstractTransactionController;
using cfd::core::Transaction;

class TestTransactionController : public AbstractTransactionController {
 public:
  TestTransactionController() {
    if (tx_address_ == nullptr) {
      tx_address_ = new Transaction();
    }
  }
  TestTransactionController(uint32_t lock_time) {
    const int32_t version = 2;
    if (tx_address_ == nullptr) {
      tx_address_ = new Transaction(version, lock_time);
    }
  }
  virtual ~TestTransactionController() {
    if (tx_address_ != nullptr){
      delete tx_address_;
    }
  }
};

static constexpr uint32_t expect_absttx_default_sequence_num = 0xffffffffU;
static constexpr uint32_t expect_absttx_enablelt_sequence_num = 0xfffffffeU;

TEST(AbstractTransactionController, DefaultConstructorTest) {
  // cfd::Initialize();
  const std::string expect_tx_hex = "02000000000000000000";
  TestTransactionController txc = TestTransactionController();
  EXPECT_STREQ(txc.GetHex().c_str(), expect_tx_hex.c_str());
  EXPECT_EQ(txc.GetDefaultSequence(), expect_absttx_default_sequence_num);
  EXPECT_EQ(txc.GetLockTimeDisabledSequence(),
      expect_absttx_default_sequence_num);
}

TEST(AbstractTransactionController, EnableLockTimeTest) {
  const std::string expect_tx_hex = "02000000000090000000";
  const uint32_t lock_time = 144;
  TestTransactionController txc = TestTransactionController(lock_time);
  EXPECT_STREQ(txc.GetHex().c_str(), expect_tx_hex.c_str());
  EXPECT_EQ(txc.GetDefaultSequence(), expect_absttx_enablelt_sequence_num);
  EXPECT_EQ(txc.GetLockTimeDisabledSequence(),
      expect_absttx_default_sequence_num);
}