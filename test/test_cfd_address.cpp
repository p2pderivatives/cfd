#include "gtest/gtest.h"
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_elements_address.h"


// https://qiita.com/yohm/items/477bac065f4b772127c7

// The main function are using gtest's main().

// TEST(test_suite_name, test_name)

using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::CfdException;
using cfd::core::GetBitcoinAddressFormatList;
using cfd::core::GetElementsAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::WitnessVersion;
using cfd::AddressFactory;

TEST(AddressFactory, Constructor)
{
  EXPECT_NO_THROW(AddressFactory factory);
}

TEST(AddressFactory, Constructor_type)
{
  EXPECT_NO_THROW(AddressFactory factory(NetType::kMainnet));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kTestnet));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kRegtest));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kLiquidV1));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kElementsRegtest));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kCustomChain));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kNetTypeNum));

  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");

  {
    AddressFactory factory(NetType::kMainnet);
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kMainnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kTestnet);
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kTestnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kRegtest);
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kRegtest);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kLiquidV1);
    EXPECT_THROW(factory.CreateP2pkhAddress(pubkey), CfdException);
  }
  {
    AddressFactory factory(NetType::kElementsRegtest);
    EXPECT_THROW(factory.CreateP2pkhAddress(pubkey), CfdException);
  }
  {
    AddressFactory factory(NetType::kCustomChain);
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
  {
    AddressFactory factory(NetType::kNetTypeNum);
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
}

TEST(AddressFactory, Constructor_type_witver)
{
  EXPECT_NO_THROW(AddressFactory factory(NetType::kMainnet, WitnessVersion::kVersion0));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kTestnet, WitnessVersion::kVersion1));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kRegtest, WitnessVersion::kVersionNone));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kLiquidV1, WitnessVersion::kVersion0));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kElementsRegtest, WitnessVersion::kVersionNone));

  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");

  {
    AddressFactory factory(NetType::kMainnet, WitnessVersion::kVersion0);
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kMainnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kTestnet, WitnessVersion::kVersion1);
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kTestnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kRegtest, WitnessVersion::kVersionNone);
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kRegtest);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kLiquidV1, WitnessVersion::kVersion0);
    EXPECT_THROW(factory.CreateP2pkhAddress(pubkey), CfdException);
  }
  {
    AddressFactory factory(NetType::kElementsRegtest, WitnessVersion::kVersionNone);
    EXPECT_THROW(factory.CreateP2pkhAddress(pubkey), CfdException);
  }
}


TEST(AddressFactory, Constructor_type_witver_prefix)
{
  EXPECT_NO_THROW(AddressFactory factory(NetType::kMainnet, WitnessVersion::kVersion0, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kTestnet, WitnessVersion::kVersion1, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kRegtest, WitnessVersion::kVersionNone, GetBitcoinAddressFormatList()));

  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    AddressFactory factory(NetType::kMainnet, WitnessVersion::kVersion0, GetBitcoinAddressFormatList());
    Address address = factory.CreateP2wpkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kMainnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersion0);
  }
  {
    AddressFactory factory(NetType::kTestnet, WitnessVersion::kVersion1, GetBitcoinAddressFormatList());
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kTestnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kRegtest, WitnessVersion::kVersionNone, GetBitcoinAddressFormatList());
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kRegtest);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }

  // error
  {
    AddressFactory factory(NetType::kLiquidV1, WitnessVersion::kVersion0, GetBitcoinAddressFormatList());
    EXPECT_THROW((factory.CreateP2wpkhAddress(pubkey)), CfdException);
  }
  {
    AddressFactory factory(NetType::kElementsRegtest, WitnessVersion::kVersionNone, GetBitcoinAddressFormatList());
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
}

TEST(AddressFactory, Constructor_type_prefix)
{
  EXPECT_NO_THROW(AddressFactory factory(NetType::kMainnet, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kTestnet, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(AddressFactory factory(NetType::kRegtest, GetBitcoinAddressFormatList()));

  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    AddressFactory factory(NetType::kMainnet, GetBitcoinAddressFormatList());
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kMainnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }
  {
    AddressFactory factory(NetType::kTestnet, GetBitcoinAddressFormatList());
    Address address = factory.CreateP2wpkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kTestnet);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersion0);
  }
  {
    AddressFactory factory(NetType::kRegtest, GetBitcoinAddressFormatList());
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kRegtest);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }

  // error
  {
    AddressFactory factory(NetType::kLiquidV1, GetBitcoinAddressFormatList());
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
  {
    AddressFactory factory(NetType::kElementsRegtest, GetBitcoinAddressFormatList());
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
}

TEST(AddressFactory, GetAddress)
{
  {
    AddressFactory factory(NetType::kRegtest);
    Address address = factory.GetAddress("bcrt1qshc8er8ycnxn5mamc9m7acaxcasplqunvaw4f6");
    EXPECT_STREQ(address.GetAddress().c_str(), "bcrt1qshc8er8ycnxn5mamc9m7acaxcasplqunvaw4f6");

    EXPECT_THROW(address = factory.GetAddress(""), CfdException);
    EXPECT_THROW(address = factory.GetAddress("AzpwdpR9siiDBs3nG8SNpvGQcLTHP2od4MQPugUTa3zfKHKkAVwx6M4ea2W1JovrbhuErKosFpfeuxf5"), CfdException);
  }
}

TEST(AddressFactory, GetAddressByHash)
{
  {
    AddressFactory factory(NetType::kRegtest);
    Address address = factory.GetAddressByHash(AddressType::kP2pkhAddress, ByteData("c4d571179a455b8976b79cf99183fd2bbafe6bfd"));
    EXPECT_STREQ(address.GetHash().GetHex().c_str(), "c4d571179a455b8976b79cf99183fd2bbafe6bfd");
    EXPECT_EQ(address.GetAddressType(), AddressType::kP2pkhAddress);

    EXPECT_THROW(factory.GetAddressByHash(AddressType::kP2wpkhAddress, ByteData("c4d571179a455b8976b79cf99183fd2bbafe6bfd")), CfdException);
    EXPECT_THROW(factory.GetAddressByHash(AddressType::kP2pkhAddress, ByteData()), CfdException);
  }
}

TEST(AddressFactory, GetAddressByHash160)
{
  {
    AddressFactory factory(NetType::kRegtest);
    Address address = factory.GetAddressByHash(AddressType::kP2pkhAddress, ByteData160("c4d571179a455b8976b79cf99183fd2bbafe6bfd"));
    EXPECT_STREQ(address.GetHash().GetHex().c_str(), "c4d571179a455b8976b79cf99183fd2bbafe6bfd");
    EXPECT_EQ(address.GetAddressType(), AddressType::kP2pkhAddress);

    EXPECT_THROW(factory.GetAddressByHash(AddressType::kP2wpkhAddress, ByteData160("c4d571179a455b8976b79cf99183fd2bbafe6bfd")), CfdException);
  }
}

TEST(AddressFactory, GetSegwitAddressByHash)
{
  {
    AddressFactory factory(NetType::kRegtest);
    Address address = factory.GetSegwitAddressByHash(ByteData("c4d571179a455b8976b79cf99183fd2bbafe6bfd"));
    EXPECT_STREQ(address.GetHash().GetHex().c_str(), "c4d571179a455b8976b79cf99183fd2bbafe6bfd");
    EXPECT_EQ(address.GetAddressType(), AddressType::kP2wpkhAddress);

    EXPECT_THROW(factory.GetSegwitAddressByHash(ByteData()), CfdException);
    EXPECT_THROW(factory.GetSegwitAddressByHash(ByteData("0000")), CfdException);
  }
}

TEST(AddressFactory, CreateP2pkhAddress)
{
  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    AddressFactory factory;
    Address addr = factory.CreateP2pkhAddress(pubkey);
    EXPECT_STREQ(addr.GetAddress().c_str(), "1ELuNB5fLNUcrLzb93oJDPmjxjnsVwhNHn");
    EXPECT_STREQ(addr.GetPubkey().GetHex().c_str(), "027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  }
}

TEST(AddressFactory, CreateP2shAddress)
{
  Script script = Script("76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  {
    AddressFactory factory;
    Address addr = factory.CreateP2shAddress(script);
    EXPECT_STREQ(addr.GetAddress().c_str(), "3KiuMysHQZnQT7F2eiKxyYnUmfpaNc8ezj");
    EXPECT_STREQ(addr.GetScript().GetHex().c_str(), "76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  }
}

TEST(AddressFactory, CreateP2wpkhAddress)
{
  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    AddressFactory factory;
    Address addr = factory.CreateP2wpkhAddress(pubkey);
    EXPECT_STREQ(addr.GetAddress().c_str(), "bc1qjfw5q2ygp0gvn450h3lu0hlwjanfsc5uax7v9q");
    EXPECT_STREQ(addr.GetPubkey().GetHex().c_str(), "027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  }
}

TEST(AddressFactory, CreateP2wshAddress)
{
  Script script = Script("76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  {
    AddressFactory factory;
    Address addr = factory.CreateP2wshAddress(script);
    EXPECT_STREQ(addr.GetAddress().c_str(), "bc1q705w67ldrw5t4l5me7evc4nnf7m6tgq6tc85fr6j6zsuwt68kefqumhlug");
    EXPECT_STREQ(addr.GetScript().GetHex().c_str(), "76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  }
}

TEST(AddressFactory, CreateP2wshMultisigAddress)
{
   std::vector<Pubkey> pubkeys;
   pubkeys.push_back(Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af"));
   pubkeys.push_back(Pubkey("038d87a23b18cee3a3615eb03035e74cafa8388b48c9263bf2b3a4081d51ffcee7"));
   pubkeys.push_back(Pubkey("027bb433e43e5e3526e435bd579289dfd4b9c4acdb2e3e80bd04dcd996edc15c73"));

   AddressFactory factory;
   EXPECT_NO_THROW(Address addr = factory.CreateP2wshMultisigAddress(2, pubkeys));

   std::vector<Pubkey> empty_pubkeys;
   EXPECT_THROW(factory.CreateP2wshMultisigAddress(2, empty_pubkeys), CfdException);
   EXPECT_THROW(factory.CreateP2wshMultisigAddress(0, pubkeys), CfdException);
   EXPECT_THROW(factory.CreateP2wshMultisigAddress(5, pubkeys), CfdException);
}

TEST(AddressFactory, CheckAddressNetType_bitcoin)
{
  AddressFactory factory(NetType::kMainnet, GetBitcoinAddressFormatList());
  Pubkey pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  Address addr;
  EXPECT_NO_THROW(addr = factory.CreateP2pkhAddress(pubkey));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wpkhAddress(pubkey));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  Script script = Script("76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  EXPECT_NO_THROW(addr = factory.CreateP2shAddress(script));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wshAddress(script));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  factory = AddressFactory(NetType::kTestnet, GetBitcoinAddressFormatList());
  EXPECT_NO_THROW(addr = factory.CreateP2pkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wpkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2shAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wshAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  factory = AddressFactory(NetType::kRegtest, GetBitcoinAddressFormatList());
  EXPECT_NO_THROW(addr = factory.CreateP2pkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wpkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2shAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wshAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));
}

TEST(AddressFactory, CheckAddressNetType_elements)
{
  AddressFactory factory(NetType::kLiquidV1, GetElementsAddressFormatList());
  Pubkey pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  Address addr;
  EXPECT_NO_THROW(addr = factory.CreateP2pkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wpkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  Script script = Script("76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  EXPECT_NO_THROW(addr = factory.CreateP2shAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wshAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  factory = AddressFactory(NetType::kElementsRegtest, GetElementsAddressFormatList());
  EXPECT_NO_THROW(addr = factory.CreateP2pkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wpkhAddress(pubkey));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2shAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));

  EXPECT_NO_THROW(addr = factory.CreateP2wshAddress(script));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kMainnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kTestnet));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kLiquidV1));
  EXPECT_TRUE(factory.CheckAddressNetType(addr, NetType::kElementsRegtest));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kCustomChain));
  EXPECT_FALSE(factory.CheckAddressNetType(addr, NetType::kNetTypeNum));
}

TEST(AddressFactory, GetAddressByLockingScript)
{
  AddressFactory factory;
  Script script;
  Address address;

  script = Script("210340b52ae45bc1be5de083f1730fe537374e219c4836400623741d2a874e60590cac");
  EXPECT_NO_THROW(address = factory.GetAddressByLockingScript(script));
  EXPECT_STREQ(address.GetHash().GetHex().c_str(), "49a011f97ba520dab063f309bad59daeb30de101");
  EXPECT_EQ(address.GetAddressType(), AddressType::kP2pkhAddress);
  EXPECT_STREQ(address.GetAddress().c_str(), "17iJ5ssEuAcrD267chtACp6qLzVbPvwrms");

  script = Script("76a91449a011f97ba520dab063f309bad59daeb30de10188ac");
  EXPECT_NO_THROW(address = factory.GetAddressByLockingScript(script));
  EXPECT_STREQ(address.GetHash().GetHex().c_str(), "49a011f97ba520dab063f309bad59daeb30de101");
  EXPECT_EQ(address.GetAddressType(), AddressType::kP2pkhAddress);
  EXPECT_STREQ(address.GetAddress().c_str(), "17iJ5ssEuAcrD267chtACp6qLzVbPvwrms");

  script = Script("a914f1b3a2cc24eba8a741f963b309a7686f3bb6bfb487");
  EXPECT_NO_THROW(address = factory.GetAddressByLockingScript(script));
  EXPECT_STREQ(address.GetHash().GetHex().c_str(), "f1b3a2cc24eba8a741f963b309a7686f3bb6bfb4");
  EXPECT_EQ(address.GetAddressType(), AddressType::kP2shAddress);
  EXPECT_STREQ(address.GetAddress().c_str(), "3Pj1y6dipvm7fjNc1sH6Q5KLPNvqWXWaEw");

  script = Script("0014925d4028880bd0c9d68fbc7fc7dfee976698629c");
  EXPECT_NO_THROW(address = factory.GetAddressByLockingScript(script));
  EXPECT_STREQ(address.GetHash().GetHex().c_str(), "925d4028880bd0c9d68fbc7fc7dfee976698629c");
  EXPECT_EQ(address.GetAddressType(), AddressType::kP2wpkhAddress);
  EXPECT_STREQ(address.GetAddress().c_str(), "bc1qjfw5q2ygp0gvn450h3lu0hlwjanfsc5uax7v9q");

  script = Script("002087cb0bc07de5b5befd7565b2c63fb1681efd8af7bd85a3f0f98a529a5c50a437");
  EXPECT_NO_THROW(address = factory.GetAddressByLockingScript(script));
  EXPECT_STREQ(address.GetHash().GetHex().c_str(), "87cb0bc07de5b5befd7565b2c63fb1681efd8af7bd85a3f0f98a529a5c50a437");
  EXPECT_EQ(address.GetAddressType(), AddressType::kP2wshAddress);
  EXPECT_STREQ(address.GetAddress().c_str(), "bc1qsl9shsrauk6malt4vkevv0a3dq00mzhhhkz68u8e3fff5hzs5smsz3fm4a");
}
