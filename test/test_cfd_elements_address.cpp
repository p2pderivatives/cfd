#ifndef CFD_DISABLE_ELEMENTS
#include "gtest/gtest.h"
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_elements_address.h"
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
using cfd::core::ConfidentialKey;
using cfd::core::GetElementsAddressFormatList;
using cfd::core::GetBitcoinAddressFormatList;
using cfd::core::NetType;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::WitnessVersion;
using cfd::ElementsAddressFactory;
using cfd::core::ElementsConfidentialAddress;

TEST(ElementsAddressFactory, Constructor)
{
  EXPECT_NO_THROW(ElementsAddressFactory factory);
}

TEST(ElementsAddressFactory, Constructor_type)
{
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kLiquidV1));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kElementsRegtest));
}

TEST(ElementsAddressFactory, Constructor_type_prefix)
{
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kMainnet, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kTestnet, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kRegtest, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kLiquidV1, GetElementsAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList()));

  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    ElementsAddressFactory factory(NetType::kLiquidV1, GetElementsAddressFormatList());
    Address address = factory.CreateP2wpkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kLiquidV1);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersion0);
  }
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kElementsRegtest);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }

  // error
  {
    ElementsAddressFactory factory(NetType::kMainnet, GetElementsAddressFormatList());
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
  {
    ElementsAddressFactory factory(NetType::kTestnet, GetElementsAddressFormatList());
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
  {
    ElementsAddressFactory factory(NetType::kRegtest, GetElementsAddressFormatList());
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
}

TEST(ElementsAddressFactory, Constructor_type_witver)
{
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kLiquidV1, WitnessVersion::kVersion0));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kElementsRegtest, WitnessVersion::kVersionNone));
}


TEST(ElementsAddressFactory, Constructor_type_witver_prefix)
{
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kMainnet, WitnessVersion::kVersion0, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kTestnet, WitnessVersion::kVersion1, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kRegtest, WitnessVersion::kVersionNone, GetBitcoinAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kLiquidV1, WitnessVersion::kVersion0, GetElementsAddressFormatList()));
  EXPECT_NO_THROW(ElementsAddressFactory factory(NetType::kElementsRegtest, WitnessVersion::kVersionNone, GetElementsAddressFormatList()));

  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    ElementsAddressFactory factory(NetType::kLiquidV1, WitnessVersion::kVersion0, GetElementsAddressFormatList());
    Address address = factory.CreateP2wpkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kLiquidV1);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersion0);
  }
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, WitnessVersion::kVersionNone, GetElementsAddressFormatList());
    Address address = factory.CreateP2pkhAddress(pubkey);
    EXPECT_EQ(address.GetNetType(), NetType::kElementsRegtest);
    EXPECT_EQ(address.GetWitnessVersion(), WitnessVersion::kVersionNone);
  }

  // error
  {
    ElementsAddressFactory factory(NetType::kMainnet, WitnessVersion::kVersion0, GetElementsAddressFormatList());
    EXPECT_THROW((factory.CreateP2wpkhAddress(pubkey)), CfdException);
  }
  {
    ElementsAddressFactory factory(NetType::kTestnet, WitnessVersion::kVersion1, GetElementsAddressFormatList());
    EXPECT_THROW((factory.CreateP2wpkhAddress(pubkey)), CfdException);
  }
  {
    ElementsAddressFactory factory(NetType::kRegtest, WitnessVersion::kVersionNone, GetElementsAddressFormatList());
    EXPECT_THROW((factory.CreateP2pkhAddress(pubkey)), CfdException);
  }
}

TEST(ElementsAddressFactory, GetAddress)
{
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address address = factory.GetAddress("XVHzywLC5fPS8N1216n3b2fngqqrPzKYx9");
    EXPECT_STREQ(address.GetAddress().c_str(), "XVHzywLC5fPS8N1216n3b2fngqqrPzKYx9");

    EXPECT_THROW(Address address = factory.GetAddress(""), CfdException);
    EXPECT_THROW(Address address = factory.GetAddress("AzpwdpR9siiDBs3nG8SNpvGQcLTHP2od4MQPugUTa3zfKHKkAVwx6M4ea2W1JovrbhuErKosFpfeuxf5"), CfdException);
    EXPECT_THROW(Address address = factory.GetAddress("bcrt1qshc8er8ycnxn5mamc9m7acaxcasplqunvaw4f6"), CfdException);
  }
}
TEST(ElementsAddressFactory, GetAddressByHash)
{
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address address = factory.GetAddressByHash(AddressType::kP2pkhAddress, ByteData("c4d571179a455b8976b79cf99183fd2bbafe6bfd"));
    EXPECT_STREQ(address.GetHash().GetHex().c_str(), "c4d571179a455b8976b79cf99183fd2bbafe6bfd");
    EXPECT_EQ(address.GetAddressType(), AddressType::kP2pkhAddress);

    EXPECT_THROW(factory.GetAddressByHash(AddressType::kP2wpkhAddress, ByteData()), CfdException);
  }
}

TEST(ElementsAddressFactory, GetAddressByHash160)
{
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address address = factory.GetAddressByHash(AddressType::kP2pkhAddress, ByteData160("c4d571179a455b8976b79cf99183fd2bbafe6bfd"));
    EXPECT_STREQ(address.GetHash().GetHex().c_str(), "c4d571179a455b8976b79cf99183fd2bbafe6bfd");
    EXPECT_EQ(address.GetAddressType(), AddressType::kP2pkhAddress);

    EXPECT_THROW(factory.GetAddressByHash(AddressType::kP2wpkhAddress, ByteData160()), CfdException);
  }
}

TEST(ElementsAddressFactory, GetSegwitAddressByHash)
{
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address address = factory.GetSegwitAddressByHash(ByteData("c4d571179a455b8976b79cf99183fd2bbafe6bfd"));
    EXPECT_STREQ(address.GetHash().GetHex().c_str(), "c4d571179a455b8976b79cf99183fd2bbafe6bfd");
    EXPECT_EQ(address.GetAddressType(), AddressType::kP2wpkhAddress);

    EXPECT_THROW(factory.GetSegwitAddressByHash(ByteData()), CfdException);
    EXPECT_THROW(factory.GetSegwitAddressByHash(ByteData("0000")), CfdException);
  }
}

TEST(ElementsAddressFactory, CreateP2pkhAddress)
{
  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address addr = factory.CreateP2pkhAddress(pubkey);
    EXPECT_STREQ(addr.GetAddress().c_str(), "2dnmekh8NBmNX3Ckwte5CArjcsHLYdthCg3");
    EXPECT_STREQ(addr.GetPubkey().GetHex().c_str(), "027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  }
}

TEST(ElementsAddressFactory, CreateP2shAddress)
{
  Script script = Script("76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address addr = factory.CreateP2shAddress(script);
    EXPECT_STREQ(addr.GetAddress().c_str(), "XVP8HajT7CDjhT16N5eGvKqYpyrc4vsuv8");
    EXPECT_STREQ(addr.GetScript().GetHex().c_str(), "76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  }
}

TEST(ElementsAddressFactory, CreateP2wpkhAddress)
{
  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address addr = factory.CreateP2wpkhAddress(pubkey);
    EXPECT_STREQ(addr.GetAddress().c_str(), "ert1qjfw5q2ygp0gvn450h3lu0hlwjanfsc5udafvh6");
    EXPECT_STREQ(addr.GetPubkey().GetHex().c_str(), "027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  }
}

TEST(ElementsAddressFactory, CreateP2wshAddress)
{
  Script script = Script("76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest, GetElementsAddressFormatList());
    Address addr = factory.CreateP2wshAddress(script);
    EXPECT_STREQ(addr.GetAddress().c_str(), "ert1q705w67ldrw5t4l5me7evc4nnf7m6tgq6tc85fr6j6zsuwt68kefqherg6e");
    EXPECT_STREQ(addr.GetScript().GetHex().c_str(), "76a914ef286e6af39de178d88b32e120f716b53753808c88ac");
  }
}

TEST(ElementsAddressFactory, CreateP2wshMultisigAddress)
{
   std::vector<Pubkey> pubkeys;
   pubkeys.push_back(Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af"));
   pubkeys.push_back(Pubkey("038d87a23b18cee3a3615eb03035e74cafa8388b48c9263bf2b3a4081d51ffcee7"));
   pubkeys.push_back(Pubkey("027bb433e43e5e3526e435bd579289dfd4b9c4acdb2e3e80bd04dcd996edc15c73"));

   ElementsAddressFactory factory(NetType::kLiquidV1);
   EXPECT_NO_THROW(Address addr = factory.CreateP2wshMultisigAddress(2, pubkeys));

   std::vector<Pubkey> empty_pubkeys;
   EXPECT_THROW(factory.CreateP2wshMultisigAddress(2, empty_pubkeys), CfdException);
   EXPECT_THROW(factory.CreateP2wshMultisigAddress(0, pubkeys), CfdException);
   EXPECT_THROW(factory.CreateP2wshMultisigAddress(5, pubkeys), CfdException);
}

TEST(ElementsAddressFactory, GetConfidentialAddress)
{
  ConfidentialKey pubkey = ConfidentialKey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  {
    Address address("ex1q7s3nr3qcaaz30wnyfttwnlyedqddgwfhclhncc", GetElementsAddressFormatList());
    ElementsConfidentialAddress conf_address = ElementsAddressFactory::GetConfidentialAddress(address, pubkey);

    EXPECT_STREQ(conf_address.GetConfidentialKey().GetHex().c_str(), pubkey.GetHex().c_str());
    EXPECT_STREQ(conf_address.GetUnblindedAddress().GetAddress().c_str(), address.GetAddress().c_str());
  }
  {
    Address address("XRpicZNrFZumBMhRV5BSYW28pGX7JyY1ua", GetElementsAddressFormatList());
    ElementsConfidentialAddress conf_address = ElementsAddressFactory::GetConfidentialAddress(address, pubkey);

    EXPECT_STREQ(conf_address.GetConfidentialKey().GetHex().c_str(), pubkey.GetHex().c_str());
    EXPECT_STREQ(conf_address.GetUnblindedAddress().GetAddress().c_str(), address.GetAddress().c_str());
  }

  // error
  Address err_address("bcrt1qjylqhy5pm2ck75ppqxk5ue2swsuk7dx9mmfljp");
  EXPECT_THROW(ElementsAddressFactory::GetConfidentialAddress(err_address, pubkey), CfdException);
  Address err_address2("2NDBYYaMywkXcQ4cupdKQdoyrm9iHHSLtdy");
  EXPECT_THROW(ElementsAddressFactory::GetConfidentialAddress(err_address2, pubkey), CfdException);
}

TEST(ElementsAddressFactory, CreatePegInAddress1)
{
  Pubkey pubkey = Pubkey("027592aab5d43618dda13fba71e3993cd7517a712d3da49664c06ee1bd3d1f70af");
  Script fedpegscript("522102baae8e066e4f2a1da4b731017697bb8fcacc60e4569f3ec27bc31cf3fb13246221026bccd050e8ecf7a702bc9fb63205cfdf278a22ba8b1f1d3ca3d8e5b38465a9702103430d354b89d1fbe43eb54ea138a4aee1076e4c54f4c805f62f9cee965351a1d053ae");
  {
    ElementsAddressFactory factory(NetType::kLiquidV1);
    EXPECT_NO_THROW(Address address = factory.CreatePegInAddress(AddressType::kP2wpkhAddress, pubkey, fedpegscript));
    // error
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, pubkey, Script()), CfdException);
  }
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest);
    EXPECT_NO_THROW(Address address = factory.CreatePegInAddress(AddressType::kP2wpkhAddress, pubkey, fedpegscript));
    // error
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, pubkey, Script()), CfdException);
  }
}

TEST(ElementsAddressFactory, CreatePegInAddress2)
{
  Script claim_script("76a914c3bec9f6c51c3d1f0fd00c54fe41b5d8014ed8eb88ac");
  Script fedpegscript("522102baae8e066e4f2a1da4b731017697bb8fcacc60e4569f3ec27bc31cf3fb13246221026bccd050e8ecf7a702bc9fb63205cfdf278a22ba8b1f1d3ca3d8e5b38465a9702103430d354b89d1fbe43eb54ea138a4aee1076e4c54f4c805f62f9cee965351a1d053ae");
  {
    ElementsAddressFactory factory(NetType::kLiquidV1);
    EXPECT_NO_THROW(Address address = factory.CreatePegInAddress(AddressType::kP2wpkhAddress, claim_script, fedpegscript));
    // error
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, claim_script, Script()), CfdException);
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, Script(), fedpegscript), CfdException);
  }
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest);
    EXPECT_NO_THROW(Address address = factory.CreatePegInAddress(AddressType::kP2wpkhAddress, claim_script, fedpegscript));
    // error
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, claim_script, Script()), CfdException);
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, Script(), fedpegscript), CfdException);
  }
}

TEST(ElementsAddressFactory, CreatePegInAddress3)
{
  Script tweak_fedpegscript("522102baae8e066e4f2a1da4b731017697bb8fcacc60e4569f3ec27bc31cf3fb13246221026bccd050e8ecf7a702bc9fb63205cfdf278a22ba8b1f1d3ca3d8e5b38465a9702103430d354b89d1fbe43eb54ea138a4aee1076e4c54f4c805f62f9cee965351a1d053ae");
  {
    ElementsAddressFactory factory(NetType::kLiquidV1);
    EXPECT_NO_THROW(Address address = factory.CreatePegInAddress(AddressType::kP2wpkhAddress, tweak_fedpegscript));
    // error
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, Script()), CfdException);
  }
  {
    ElementsAddressFactory factory(NetType::kElementsRegtest);
    EXPECT_NO_THROW(Address address = factory.CreatePegInAddress(AddressType::kP2wpkhAddress, tweak_fedpegscript));
    // error
    EXPECT_THROW(factory.CreatePegInAddress(AddressType::kP2wpkhAddress, Script()), CfdException);
  }
}
#endif
