#include "gtest/gtest.h"
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_elements_address.h"
#include "cfdcore/cfdcore_schnorrsig.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_taproot.h"

using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::ByteData;
using cfd::core::ByteData160;
using cfd::core::CfdException;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorScriptType;
using cfd::core::GetBitcoinAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::SchnorrPubkey;
using cfd::core::Script;
using cfd::core::ScriptBuilder;
using cfd::core::ScriptOperator;
using cfd::core::TaprootScriptTree;
using cfd::core::WitnessVersion;
using cfd::AddressFactory;
using cfd::DescriptorKeyData;
using cfd::DescriptorScriptData;

#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::GetElementsAddressFormatList;
#endif  // CFD_DISABLE_ELEMENTS

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

TEST(AddressFactory, CreateTaprootAddress)
{
  AddressFactory factory;
  Address addr;
  const SchnorrPubkey pubkey = SchnorrPubkey(
    "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb");
  EXPECT_NO_THROW(addr = factory.CreateTaprootAddress(pubkey));
  EXPECT_EQ("bc1pzamhq9jglfxaj0r5ahvatr8uc77u973s5tm04yytdltsey5r8naspp3kr4", addr.GetAddress());

  EXPECT_NO_THROW(addr = factory.CreateTaprootAddress(pubkey.GetByteData256()));
  EXPECT_EQ("bc1pzamhq9jglfxaj0r5ahvatr8uc77u973s5tm04yytdltsey5r8naspp3kr4", addr.GetAddress());

  ScriptBuilder build;
  build.AppendOperator(ScriptOperator::OP_TRUE);
  TaprootScriptTree tree(build.Build());
  EXPECT_NO_THROW(addr = factory.CreateTaprootAddress(tree, pubkey));
  EXPECT_EQ("bc1p3r0p5kdn3yultra5lrzlls74vwgdg057j8rmr4nlj8s8pucss7vsftyvah", addr.GetAddress());
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

#ifndef CFD_DISABLE_ELEMENTS
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
#endif  // CFD_DISABLE_ELEMENTS

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
  
  script = Script("51201777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb");
  EXPECT_NO_THROW(address = factory.GetAddressByLockingScript(script));
  EXPECT_STREQ(address.GetHash().GetHex().c_str(), "1777701648fa4dd93c74edd9d58cfcc7bdc2fa30a2f6fa908b6fd70c92833cfb");
  EXPECT_EQ(address.GetAddressType(), AddressType::kTaprootAddress);
  EXPECT_STREQ(address.GetAddress().c_str(), "bc1pzamhq9jglfxaj0r5ahvatr8uc77u973s5tm04yytdltsey5r8naspp3kr4");
  
  script = Script("512088de1a59b38939f58fb4f8c5ffc3d56390d43e9e91c7b1d67f91e070f3108799");
  EXPECT_NO_THROW(address = factory.GetAddressByLockingScript(script));
  EXPECT_STREQ(address.GetHash().GetHex().c_str(), "88de1a59b38939f58fb4f8c5ffc3d56390d43e9e91c7b1d67f91e070f3108799");
  EXPECT_EQ(address.GetAddressType(), AddressType::kTaprootAddress);
  EXPECT_STREQ(address.GetAddress().c_str(), "bc1p3r0p5kdn3yultra5lrzlls74vwgdg057j8rmr4nlj8s8pucss7vsftyvah");
}


struct TestCfdAddressFactoryDescriptorData {
  std::string descriptor;
  NetType net_type;
  DescriptorScriptType type;
  std::string locking_script;
  uint32_t depth;
  std::string address;
  AddressType address_type;
  std::string redeem_script;
  DescriptorKeyType key_type;
  std::string key;
  std::string tree_string;
  uint32_t multisig_req_sig_num;
};

TEST(AddressFactory, ParseOutputDescriptor) {
  std::vector<TestCfdAddressFactoryDescriptorData> g_test_cfd_descriptor_data {
    {
      "pk(02a5613bd857b7048924264d1e70e08fb2a7e6527d32b7ab1bb993ac59964ff397)#rk5v7uqw",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptPk,
      "2102a5613bd857b7048924264d1e70e08fb2a7e6527d32b7ab1bb993ac59964ff397ac",
      0,
      "1FoG2386FG2tAJS9acMuiDsKy67aGg9MKz",
      AddressType::kP2shAddress,
      "",
      DescriptorKeyType::kDescriptorKeyPublic,
      "02a5613bd857b7048924264d1e70e08fb2a7e6527d32b7ab1bb993ac59964ff397",
      "",
      0
    },
    {
      "pkh(02c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5)",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptPkh,
      "76a91406afd46bcdfd22ef94ac122aa11f241244a37ecc88ac",
      0,
      "1cMh228HTCiwS8ZsaakH8A8wze1JR5ZsP",
      AddressType::kP2pkhAddress,
      "",
      DescriptorKeyType::kDescriptorKeyPublic,
      "02c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5",
      "",
      0
    },
    {
      "wpkh(02f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9)",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptWpkh,
      "00147dd65592d0ab2fe0d0257d571abf032cd9db93dc",
      0,
      "bc1q0ht9tyks4vh7p5p904t340cr9nvahy7u3re7zg",
      AddressType::kP2wpkhAddress,
      "",
      DescriptorKeyType::kDescriptorKeyPublic,
      "02f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9",
      "",
      0
    },
    {
      "sh(wpkh(03fff97bd5755eeea420453a14355235d382f6472f8568a18b2f057a1460297556))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptSh,
      "a914cc6ffbc0bf31af759451068f90ba7a0272b6b33287",
      0,
      "3LKyvRN6SmYXGBNn8fcQvYxW9MGKtwcinN",
      AddressType::kP2shP2wpkhAddress,
      "",
      DescriptorKeyType::kDescriptorKeyPublic,
      "03fff97bd5755eeea420453a14355235d382f6472f8568a18b2f057a1460297556",
      "",
      0
    },
    {
      "combo(0279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798)",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptCombo,
      "0014751e76e8199196d454941c45d1b3a323f1433bd6",
      0,
      "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4",
      AddressType::kP2wpkhAddress,
      "",
      DescriptorKeyType::kDescriptorKeyPublic,
      "0279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798",
      "",
      0
    },
    {
      "sh(wsh(pkh(02e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13)))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptSh,
      "a91455e8d5e8ee4f3604aba23c71c2684fa0a56a3a1287",
      0,
      "39XGHYpYmJV9sGFoGHZeU2rLkY6r1MJ6C1",
      AddressType::kP2shP2wshAddress,
      "76a914c42e7ef92fdb603af844d064faad95db9bcdfd3d88ac",
      DescriptorKeyType::kDescriptorKeyPublic,
      "02e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13",
      "",
      0
    },
    {
      "sh(multi(2,022f01e5e15cca351daff3843fb70f3c2f0a1bdd05e5af888a67784ef3e10a2a01,03acd484e2f0c7f65309ad178a9f559abde09796974c57e714c35f110dfc27ccbe))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptSh,
      "a914a6a8b030a38762f4c1f5cbe387b61a3c5da5cd2687",
      0,
      "3GtEB3yg3r5de2cDJG48SkQwxfxJumKQdN",
      AddressType::kP2shAddress,
      "5221022f01e5e15cca351daff3843fb70f3c2f0a1bdd05e5af888a67784ef3e10a2a012103acd484e2f0c7f65309ad178a9f559abde09796974c57e714c35f110dfc27ccbe52ae",
      DescriptorKeyType::kDescriptorKeyNull,
      "",
      "",
      2
    },
    {
      "wsh(multi(2,03a0434d9e47f3c86235477c7b1ae6ae5d3442d49b1943c2b752a68e2a47e247c7,03774ae7f858a9411e5ef4246b70c65aac5649980be5c17891bbec17895da008cb,03d01115d548e7561b15c38f004d734633687cf4419620095bc5b0f47070afe85a))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptWsh,
      "0020773d709598b76c4e3b575c08aad40658963f9322affc0f8c28d1d9a68d0c944a",
      0,
      "bc1qwu7hp9vckakyuw6htsy244qxtztrlyez4l7qlrpg68v6drgvj39qn4zazc",
      AddressType::kP2wshAddress,
      "522103a0434d9e47f3c86235477c7b1ae6ae5d3442d49b1943c2b752a68e2a47e247c72103774ae7f858a9411e5ef4246b70c65aac5649980be5c17891bbec17895da008cb2103d01115d548e7561b15c38f004d734633687cf4419620095bc5b0f47070afe85a53ae",
      DescriptorKeyType::kDescriptorKeyNull,
      "",
      "",
      2
    },
    {
      "sh(wsh(multi(1,03f28773c2d975288bc7d1d205c3748651b075fbc6610e58cddeeddf8f19405aa8,03499fdf9e895e719cfd64e67f07d38e3226aa7b63678949e6e49b241a60e823e4,02d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e)))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptSh,
      "a914aec509e284f909f769bb7dda299a717c87cc97ac87",
      0,
      "3Hd7YQStg9gYpEt6hgK14ZHUABxSURzeuQ",
      AddressType::kP2shP2wshAddress,
      "512103f28773c2d975288bc7d1d205c3748651b075fbc6610e58cddeeddf8f19405aa82103499fdf9e895e719cfd64e67f07d38e3226aa7b63678949e6e49b241a60e823e42102d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e53ae",
      DescriptorKeyType::kDescriptorKeyNull,
      "",
      "",
      1
    },
    {
      "sh(wsh(pkh(02e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13)))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptSh,
      "a91455e8d5e8ee4f3604aba23c71c2684fa0a56a3a1287",
      0,
      "39XGHYpYmJV9sGFoGHZeU2rLkY6r1MJ6C1",
      AddressType::kP2shP2wshAddress,
      "76a914c42e7ef92fdb603af844d064faad95db9bcdfd3d88ac",
      DescriptorKeyType::kDescriptorKeyPublic,
      "02e493dbf1c10d80f3581e4904930b1404cc6c13900ee0758474fa94abe8c4cd13",
      "",
      0
    },
    {
      "addr(bc1qc7slrfxkknqcq2jevvvkdgvrt8080852dfjewde450xdlk4ugp7szw5tk9)",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptAddr,
      "0020c7a1f1a4d6b4c1802a59631966a18359de779e8a6a65973735a3ccdfdabc407d",
      0,
      "bc1qc7slrfxkknqcq2jevvvkdgvrt8080852dfjewde450xdlk4ugp7szw5tk9",
      AddressType::kP2wshAddress,
      "",
      DescriptorKeyType::kDescriptorKeyNull,
      "",
      "",
      0
    },
    {
      "raw(6a4c4f54686973204f505f52455455524e207472616e73616374696f6e206f7574707574207761732063726561746564206279206d6f646966696564206372656174657261777472616e73616374696f6e2e)#zf2avljj",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptRaw,
      "6a4c4f54686973204f505f52455455524e207472616e73616374696f6e206f7574707574207761732063726561746564206279206d6f646966696564206372656174657261777472616e73616374696f6e2e",
      0,
      "",
      AddressType::kP2shAddress,
      "",
      DescriptorKeyType::kDescriptorKeyNull,
      "",
      "",
      0
    },
    {
      "sh(c:or_i(andor(c:pk_h(xpub661MyMwAqRbcFW31YEwpkMuc5THy2PSt5bDMsktWQcFF8syAmRUapSCGu8ED9W6oDMSgv6Zz8idoc4a6mr8BDzTJY47LJhkJ8UB7WEGuduB/1/0/44),pk_h(xpub69H7F5d8KSRgmmdJg2KhpAK8SR3DjMwAdkxj3ZuxV27CprR9LgpeyGmXUbC6wb7ERfvrnKZjXoUmmDznezpbZb7ap6r1D3tgFxHmwMkQTPH/0/0/44),pk_h(02c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5)),pk_k(02d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e)))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptSh,
      "a9148019628fc9b183af5d52ed7dc5c6d56fe6a6325f87",
      0,
      "3DNLpswxUiEVyRnAA6mZSvAt7iG6bCu33f",
      AddressType::kP2shAddress,
      "6376a914520e6e72bcd5b616bc744092139bd759c31d6bbe88ac6476a91406afd46bcdfd22ef94ac122aa11f241244a37ecc886776a9145ab62f0be26fe9d6205a155403f33e2ad2d31efe8868672102d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e68ac",
      DescriptorKeyType::kDescriptorKeyNull,
      "",
      "",
      0
    },
    {
      "tr(ef514f1aeb14baa6cc57ab3268fb329ca540c48454f7f46771ed731e34ba521a)",
      NetType::kRegtest,
      DescriptorScriptType::kDescriptorScriptTaproot,
      "5120630f2d9ea4cbcc3f04b73fd56a1c7b51fd3192de8e26e7cd893124900033d3d1",
      0,
      "bcrt1pvv8jm84ye0xr7p9h8l2k58rm287nryk73cnw0nvfxyjfqqpn60gssz7u5f",
      AddressType::kTaprootAddress,
      "",
      DescriptorKeyType::kDescriptorKeySchnorr,
      "ef514f1aeb14baa6cc57ab3268fb329ca540c48454f7f46771ed731e34ba521a",
      "",
      0
    },
    {
      "tr([bd16bee5/0]xpub69H7F5d8KSRgmmdJg2KhpAK8SR3DjMwAdkxj3ZuxV27CprR9LgpeyGmXUbC6wb7ERfvrnKZjXoUmmDznezpbZb7ap6r1D3tgFxHmwMkQTPH/0/0/1)",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptTaproot,
      "5120acb99cf4bb397c8aef2fca4f9201053526f5484174fef211ac6ac7fad0a38a48",
      0,
      "bc1p4jueea9m897g4me0ef8eyqg9x5n02jzpwnl0yydvdtrl459r3fyqg8wvnj",
      AddressType::kTaprootAddress,
      "",
      DescriptorKeyType::kDescriptorKeyBip32,
      "xpub6EKMC2gSMfKgSwn7V9VZn7x1MvoeeVzSmmtSJ4z2L2d6R4WxvdQMouokypZHVp4fgKycrrQnGr6WJ5ED5jG9Q9FiA1q5gKYUc8u6JHJhdo8",
      "",
      0
    },
    {
      "tr(ef514f1aeb14baa6cc57ab3268fb329ca540c48454f7f46771ed731e34ba521a,{c:pk_k(8c6f5956c3cc7251d483fc683fa06b22d4e2ddc7496a2590acee36c4a313f816),{c:pk_k([bd16bee5/0]xpub69H7F5d8KSRgmmdJg2KhpAK8SR3DjMwAdkxj3ZuxV27CprR9LgpeyGmXUbC6wb7ERfvrnKZjXoUmmDznezpbZb7ap6r1D3tgFxHmwMkQTPH/0/0/1),thresh(2,c:pk_k(5cbdf0646e5db4eaa398f365f2ea7a0e3d419b7e0330e39ce92bddedcac4f9bc),s:sha256(e38990d0c7fc009880a9c07c23842e886c6bbdc964ce6bdd5817ad357335ee6f),a:hash160(dd69735817e0e3f6f826a9238dc2e291184f0131))}})",
      NetType::kRegtest,
      DescriptorScriptType::kDescriptorScriptTaproot,
      "51204f009acbd8c905be4470df1b92c70be16a71d354ba55cc0e6517853f77d79651",
      0,
      "bcrt1pfuqf4j7ceyzmu3rsmude93ctu948r565hf2ucrn9z7zn7a7hjegskj3rsv",
      AddressType::kTaprootAddress,
      "",
      DescriptorKeyType::kDescriptorKeySchnorr,
      "ef514f1aeb14baa6cc57ab3268fb329ca540c48454f7f46771ed731e34ba521a",
      "{tl(208c6f5956c3cc7251d483fc683fa06b22d4e2ddc7496a2590acee36c4a313f816ac),{tl(208c6f5956c3cc7251d483fc683fa06b22d4e2ddc7496a2590acee36c4a313f816ac),tl(205cbdf0646e5db4eaa398f365f2ea7a0e3d419b7e0330e39ce92bddedcac4f9bcac7c82012088a820e38990d0c7fc009880a9c07c23842e886c6bbdc964ce6bdd5817ad357335ee6f87936b82012088a914dd69735817e0e3f6f826a9238dc2e291184f0131876c935287)}}",
      0
    }
  };

  DescriptorScriptData data;
  for (auto test_data : g_test_cfd_descriptor_data) {
    try {
      AddressFactory factory(test_data.net_type);
      data = factory.ParseOutputDescriptor(test_data.descriptor);
      EXPECT_EQ(test_data.type, data.type);
      EXPECT_EQ(test_data.locking_script, data.locking_script.GetHex());
      EXPECT_EQ(test_data.depth, data.depth);
      EXPECT_EQ(test_data.address, data.address.GetAddress());
      EXPECT_EQ(test_data.address_type, data.address_type);
      EXPECT_EQ(test_data.redeem_script, data.redeem_script.GetHex());
      EXPECT_EQ(test_data.key_type, data.key_type);
      EXPECT_EQ(test_data.key, data.key);
      EXPECT_EQ(test_data.multisig_req_sig_num, data.multisig_req_sig_num);
      if (test_data.tree_string.empty()) {
        EXPECT_FALSE(data.tree.IsValid());
      } else {
        EXPECT_EQ(test_data.tree_string, data.tree.ToString());
      }
    } catch (const std::exception& e) {
      EXPECT_STREQ("", e.what());
      EXPECT_STREQ("", test_data.descriptor.c_str());
    }
  }
}
