#include "gtest/gtest.h"
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_address.h"

// https://qiita.com/yohm/items/477bac065f4b772127c7

// The main function are using gtest's main().

// TEST(test_suite_name, test_name)

using cfd::core::Address;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::WitnessVersion;
using cfd::AddressFactory;

TEST(AddressFactory, CreateP2pkhAddress)
{
  AddressFactory factory;
  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  Address addr = factory.CreateP2pkhAddress(pubkey);
  EXPECT_STREQ(addr.GetAddress().c_str(), "1ELuNB5fLNUcrLzb93oJDPmjxjnsVwhNHn");
}
