#include "gtest/gtest.h"

#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_key.h"
#include "cfdcore/cfdcore_util.h"
#include "cfd/cfd_transaction_common.h"

using cfd::core::ByteData;
using cfd::core::Pubkey;
using cfd::core::Script;
using cfd::core::ScriptOperator;
using cfd::core::SigHashAlgorithm;
using cfd::core::SigHashType;
using cfd::SignDataType;
using cfd::SignParameter;

static const auto CompareSignparameter = [](const SignParameter& actual,
    const ByteData& expect_data, SignDataType expect_data_type, 
    const Pubkey& expect_related_pubkey, bool expect_der_encode, 
    const SigHashType& expect_sighash_type, 
    const ByteData& expect_signature) -> void {
  ASSERT_TRUE(actual.GetData().Equals(expect_data));
  ASSERT_EQ(actual.GetDataType(), expect_data_type);
  ASSERT_TRUE(actual.GetRelatedPubkey().Equals(expect_related_pubkey));
  ASSERT_EQ(actual.IsDerEncode(), expect_der_encode);
  ASSERT_EQ(actual.GetSigHashType().GetSigHashFlag(),
      expect_sighash_type.GetSigHashFlag());

  ByteData actual_signature = actual.ConvertToSignature();
  ASSERT_TRUE(actual_signature.Equals(expect_signature));
  if (expect_data_type == SignDataType::kOpCode) {
    ASSERT_TRUE(actual.IsOpCode());
    ScriptOperator op = ScriptOperator::Get(actual.GetOpCode().ToCodeString());
    ASSERT_STREQ(actual.GetOpCode().ToCodeString().c_str(),
        op.ToCodeString().c_str());
    ASSERT_STRNE(actual.GetOpCode().ToCodeString().c_str(), "");
  } else {
    ASSERT_FALSE(actual.IsOpCode());
  }
};

TEST(SignParameter, DefaultConstructorTest)
{
  // default constructor
  SignParameter sign_parameter = SignParameter();
  CompareSignparameter(sign_parameter, ByteData(), SignDataType::kBinary,
      Pubkey(), false, SigHashType(), ByteData());
}

TEST(SignParameter, BinaryDataConstructorTest)
{
  const ByteData expect_bytedata("0123456789");
  // constructor
  SignParameter sign_parameter = SignParameter(expect_bytedata);
  CompareSignparameter(sign_parameter, expect_bytedata,
      SignDataType::kBinary, Pubkey(), false, SigHashType(), expect_bytedata);
}

TEST(SignParameter, PubkeyDataConstructorTest)
{
  const Pubkey expect_pubkey("0369809d2cfc44f823249169f44bf9de20fc6ed1d88482540831289947f7e82af5");
  // constructor
  SignParameter sign_parameter = SignParameter(expect_pubkey);
  CompareSignparameter(sign_parameter, expect_pubkey.GetData(),
      SignDataType::kPubkey, Pubkey(), false, SigHashType(),
      expect_pubkey.GetData());
}

TEST(SignParameter, RedeemScriptConstructorTest)
{
  const Script expect_script("2102ddeda4a5b67955c32247c28379cf3461c872e34f96ec94ddd61c66bbcfda1906ac");
  // constructor
  SignParameter sign_parameter = SignParameter(expect_script);
  CompareSignparameter(sign_parameter, expect_script.GetData(),
      SignDataType::kRedeemScript, Pubkey(), false, SigHashType(),
      expect_script.GetData());
}

TEST(SignParameter, OpCodeConstructorTest)
{
  const ByteData expect_bytedata_00("00");
  // constructor
  SignParameter sign_parameter = SignParameter(ScriptOperator::OP_0);
  CompareSignparameter(sign_parameter, expect_bytedata_00,
      SignDataType::kOpCode, Pubkey(), false, SigHashType(), expect_bytedata_00);

  const ByteData expect_bytedata_true("51");
  // constructor
  sign_parameter = SignParameter(ScriptOperator::OP_TRUE);
  CompareSignparameter(sign_parameter, expect_bytedata_true,
      SignDataType::kOpCode, Pubkey(), false, SigHashType(), expect_bytedata_true);

  const ByteData expect_bytedata_msig("ae");
  // constructor
  sign_parameter = SignParameter(ScriptOperator::OP_CHECKMULTISIG);
  CompareSignparameter(sign_parameter, expect_bytedata_msig,
      SignDataType::kOpCode, Pubkey(), false, SigHashType(), expect_bytedata_msig);
}

TEST(SignParameter, StringConstructorTest)
{
  const ByteData expect_bytedata_00("00");
  // constructor
  SignParameter sign_parameter = SignParameter("OP_0");
  CompareSignparameter(sign_parameter, expect_bytedata_00,
      SignDataType::kOpCode, Pubkey(), false, SigHashType(), expect_bytedata_00);

  const ByteData expect_bytedata_true("51");
  // constructor
  sign_parameter = SignParameter("OP_TRUE");
  CompareSignparameter(sign_parameter, expect_bytedata_true,
      SignDataType::kOpCode, Pubkey(), false, SigHashType(), expect_bytedata_true);

  const ByteData expect_bytedata_msig("ae");
  // constructor
  sign_parameter = SignParameter("OP_CHECKMULTISIG");
  CompareSignparameter(sign_parameter, expect_bytedata_msig,
      SignDataType::kOpCode, Pubkey(), false, SigHashType(), expect_bytedata_msig);

  const Script expect_script("2102ddeda4a5b67955c32247c28379cf3461c872e34f96ec94ddd61c66bbcfda1906ac");
  // constructor
  sign_parameter = SignParameter(expect_script.GetHex());
  CompareSignparameter(sign_parameter, expect_script.GetData(),
      SignDataType::kBinary, Pubkey(), false, SigHashType(), expect_script.GetData());

  const Pubkey expect_pubkey("0369809d2cfc44f823249169f44bf9de20fc6ed1d88482540831289947f7e82af5");
  // constructor
  sign_parameter = SignParameter(expect_pubkey.GetHex());
  CompareSignparameter(sign_parameter, expect_pubkey.GetData(),
      SignDataType::kBinary, Pubkey(), false, SigHashType(), expect_pubkey.GetData());
}

TEST(SignParameter, SignDataConstructorTest)
{
  const ByteData expect_message("773420c0ded41a55b1f1205cfb632f08f3f911a53e7338a0dac73ec6cbe3ca471907434d046185abedc5afddc2761a642bccc70af6d22b46394f1d04a8b24226");
  const SigHashType expect_sighashtype(SigHashAlgorithm::kSigHashAll);
  const ByteData expect_signature("30440220773420c0ded41a55b1f1205cfb632f08f3f911a53e7338a0dac73ec6cbe3ca4702201907434d046185abedc5afddc2761a642bccc70af6d22b46394f1d04a8b2422601");
  // constructor
  SignParameter sign_parameter = SignParameter(expect_message, true);
  CompareSignparameter(sign_parameter, expect_message,
      SignDataType::kSign, Pubkey(), true, expect_sighashtype,
      expect_signature);
}

TEST(SignParameter, CopyConstructorTest)
{
  const ByteData expect_bytedata("0123456789ABCDEF");
  // constructor
  SignParameter sign_parameter = SignParameter(expect_bytedata);
  CompareSignparameter(sign_parameter, expect_bytedata,
      SignDataType::kBinary, Pubkey(), false, SigHashType(), expect_bytedata);

  // copy constructor
  SignParameter copy;
  copy = sign_parameter;

  CompareSignparameter(sign_parameter, copy.GetData(), copy.GetDataType(),
      copy.GetRelatedPubkey(), copy.IsDerEncode(), copy.GetSigHashType(),
      expect_bytedata);
}

TEST(SignParameter, RelatedPubkeyTest)
{
  const ByteData expect_message("773420c0ded41a55b1f1205cfb632f08f3f911a53e7338a0dac73ec6cbe3ca471907434d046185abedc5afddc2761a642bccc70af6d22b46394f1d04a8b24226");
  const SigHashType expect_sighashtype(SigHashAlgorithm::kSigHashAll);
  const ByteData expect_signature("30440220773420c0ded41a55b1f1205cfb632f08f3f911a53e7338a0dac73ec6cbe3ca4702201907434d046185abedc5afddc2761a642bccc70af6d22b46394f1d04a8b2422601");
  // constructor
  SignParameter sign_parameter = SignParameter(expect_message, true);
  CompareSignparameter(sign_parameter, expect_message, SignDataType::kSign,
      Pubkey(), true, expect_sighashtype, expect_signature);

  // copy constructor
  SignParameter copy;
  copy = sign_parameter;

  // set related pubkey
  const Pubkey expect_related_pubkey("0369809d2cfc44f823249169f44bf9de20fc6ed1d88482540831289947f7e82af5");
  sign_parameter.SetRelatedPubkey(expect_related_pubkey);
  CompareSignparameter(sign_parameter, expect_message, SignDataType::kSign, 
      expect_related_pubkey, true, expect_sighashtype, expect_signature);

  // test not to match related pubkeys
  ASSERT_FALSE(sign_parameter.GetRelatedPubkey().Equals(
      copy.GetRelatedPubkey()));
}

