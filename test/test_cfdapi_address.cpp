#include "gtest/gtest.h"

#include <map>
#include <string>
#include <exception>
#include <vector>

#include "cfd/cfd_transaction.h"
#include "cfd/cfd_address.h"
#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfdapi_address.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_transaction.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_hdwallet.h"

using cfd::api::AddressApi;
using cfd::DescriptorScriptData;
using cfd::core::Address;
using cfd::core::AddressType;
using cfd::core::DescriptorKeyType;
using cfd::core::DescriptorScriptType;
using cfd::core::NetType;
using cfd::core::Script;
using cfd::core::CfdException;


struct TestCfdAddressApiDescriptorData {
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
  uint32_t multisig_req_sig_num;
};

TEST(AddressApi, ParseOutputDescriptor) {
  std::vector<TestCfdAddressApiDescriptorData> g_test_cfd_descriptor_data {
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
      1
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
      0
    },
    {
      "sh(wsh(c:or_i(andor(c:pk_h(xpub661MyMwAqRbcFW31YEwpkMuc5THy2PSt5bDMsktWQcFF8syAmRUapSCGu8ED9W6oDMSgv6Zz8idoc4a6mr8BDzTJY47LJhkJ8UB7WEGuduB/1/0/44),pk_h(xpub69H7F5d8KSRgmmdJg2KhpAK8SR3DjMwAdkxj3ZuxV27CprR9LgpeyGmXUbC6wb7ERfvrnKZjXoUmmDznezpbZb7ap6r1D3tgFxHmwMkQTPH/0/0/44),pk_h(02c6047f9441ed7d6d3045406e95c07cd85c778e4b8cef3ca7abac09b95c709ee5)),pk_k(02d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e))))",
      NetType::kMainnet,
      DescriptorScriptType::kDescriptorScriptSh,
      "a914a7a9f411001e3e3db96d7f02fc9ab1d0dc6aa69187",
      0,
      "3GyYN9WnJBoMn8M5tuqVcFJq1BvbAcdPAt",
      AddressType::kP2shP2wshAddress,
      "6376a914520e6e72bcd5b616bc744092139bd759c31d6bbe88ac6476a91406afd46bcdfd22ef94ac122aa11f241244a37ecc886776a9145ab62f0be26fe9d6205a155403f33e2ad2d31efe8868672102d7924d4f7d43ea965a465ae3095ff41131e5946f3c85f79e44adbcf8e27e080e68ac",
      DescriptorKeyType::kDescriptorKeyNull,
      "",
      0
    }
  };

  DescriptorScriptData data;
  AddressApi factory;
  for (auto test_data : g_test_cfd_descriptor_data) {
    try {
      data = factory.ParseOutputDescriptor(
          test_data.descriptor, test_data.net_type);

      EXPECT_EQ(test_data.type, data.type);
      EXPECT_EQ(test_data.locking_script, data.locking_script.GetHex());
      EXPECT_EQ(test_data.depth, data.depth);
      EXPECT_EQ(test_data.address, data.address.GetAddress());
      EXPECT_EQ(test_data.address_type, data.address_type);
      EXPECT_EQ(test_data.redeem_script, data.redeem_script.GetHex());
      EXPECT_EQ(test_data.key_type, data.key_type);
      EXPECT_EQ(test_data.key, data.key);
      EXPECT_EQ(test_data.multisig_req_sig_num, data.multisig_req_sig_num);
    } catch (const std::exception& e) {
      EXPECT_STREQ("", e.what());
      EXPECT_STREQ("", test_data.descriptor.c_str());
    }
  }
}
