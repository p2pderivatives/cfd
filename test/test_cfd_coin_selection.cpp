#include "gtest/gtest.h"
#include <vector>

#include "cfd/cfd_common.h"
#include "cfd/cfd_utxo.h"
#include "cfd/cfd_transaction.h"
#include "cfd/cfd_transaction_common.h"
#include "cfdcore/cfdcore_address.h"
#include "cfdcore/cfdcore_bytedata.h"
#include "cfdcore/cfdcore_coin.h"
#include "cfdcore/cfdcore_elements_transaction.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_script.h"
#include "cfdcore/cfdcore_util.h"

using cfd::AmountMap;
using cfd::CoinSelection;
using cfd::CoinSelectionOption;
using cfd::TransactionController;
using cfd::Utxo;
using cfd::UtxoFilter;
using cfd::core::Amount;
using cfd::core::AddressType;
using cfd::core::BlockHash;
using cfd::core::ByteData;
using cfd::core::ByteData256;
using cfd::core::Script;
using cfd::core::StringUtil;
using cfd::core::Txid;
using cfd::core::CfdException;
#ifndef CFD_DISABLE_ELEMENTS
using cfd::core::ConfidentialAssetId;
#endif  // CFD_DISABLE_ELEMENTS

static CoinSelection exp_selection(false);
static UtxoFilter exp_filter;

// SelectCoins(Single Asset) =====================================================

static CoinSelectionOption GetBitcoinOption() {
  CoinSelectionOption option;
  option.InitializeTxSizeInfo();
  return option;
}

// FIXME Utxo illegal creating. need set witness_size_max, uscript_size_max
static std::vector<Utxo> GetBitcoinUtxoList() {
  std::vector<Utxo> utxos;
  {
    Txid txid("7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 312500000;
    utxos.push_back(utxo);
  }
  {
    Txid txid("30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 78125000;
    utxos.push_back(utxo);
  }
  {
    Txid txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 1250000000;
    utxos.push_back(utxo);
  }
  {
    Txid txid("8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 39062500;
    utxos.push_back(utxo);
  }
  {
    Txid txid("4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 156250000;
    utxos.push_back(utxo);
  }
  {
    Txid txid("b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 2500000000;
    utxos.push_back(utxo);
  }
  {
    Txid txid("0f093988839178ea5895431241cb4400fb31dd7b665a1a93cbd372336c717e0c");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 5000000000;
    utxos.push_back(utxo);
  }
  return utxos;
}

// KnapsackSolver-----------------------------------------------------------------
TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_targetvalue_0)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(0);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(0);
  bool use_bnb = false;
  CoinSelectionOption option = GetBitcoinOption();
  option.SetEffectiveFeeBaserate(0.0);
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, option,
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 0);
  EXPECT_EQ(select_value.GetSatoshiValue(), 0);
  EXPECT_EQ(fee.GetSatoshiValue(), 0);
  EXPECT_FALSE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_match_utxo)
{
  // 39062500 - 820 - 1500
  Amount target_amount = Amount::CreateBySatoshiAmount(39060180);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 1);
  EXPECT_EQ(select_value.GetSatoshiValue(), 39062500);
  EXPECT_EQ(fee.GetSatoshiValue(), 820);
  if (ret.size() == 1) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(39062500));
  }
  EXPECT_FALSE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_match_utxo2)
{
  // 117187500 - 1640 - 1500
  Amount target_amount = Amount::CreateBySatoshiAmount(117184360);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 2);
  EXPECT_EQ(select_value.GetSatoshiValue(), 117187500);
  EXPECT_EQ(fee.GetSatoshiValue(), 1640);
  if (ret.size() == 2) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(78125000));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(39062500));
  }
  EXPECT_FALSE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_lowest_larger)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(120000000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 1);
  EXPECT_EQ(select_value.GetSatoshiValue(), 156250000);
  EXPECT_EQ(fee.GetSatoshiValue(), 820);
  if (ret.size() == 1) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(156250000));
  }
  EXPECT_FALSE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_ApproximateBestSubset)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(220000000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 2);
  EXPECT_EQ(select_value.GetSatoshiValue(), 234375000);
  EXPECT_EQ(fee.GetSatoshiValue(), 1640);
  if (ret.size() == 2) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(156250000));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(78125000));
  }
  EXPECT_FALSE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_ApproximateBestSubset2)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(460000000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 2);
  EXPECT_EQ(select_value.GetSatoshiValue(), 468750000);
  EXPECT_EQ(fee.GetSatoshiValue(), 1640);
  if (ret.size() == 2) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(312500000));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(156250000));
  }
  EXPECT_FALSE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_ApproximateBestSubset2_lower)
{
  // Amount target_amount = Amount::CreateBySatoshiAmount(460000000);
  Amount target_amount = Amount::CreateBySatoshiAmount(468700000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 1);
  EXPECT_EQ(select_value.GetSatoshiValue(), 1250000000);
  EXPECT_EQ(fee.GetSatoshiValue(), 820);
  if (ret.size() == 1) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(1250000000));
  }
  EXPECT_FALSE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_KnapsackSolver_MinimumFee)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(468700000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1000);
  bool use_bnb = false;
  CoinSelectionOption option = GetBitcoinOption();
  option.SetKnapsackMinimumChange(0);  // minimum change is BnB level
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, option,
      tx_fee, &select_value, &fee, &use_bnb));

  EXPECT_EQ(ret.size(), 2);
  EXPECT_EQ(select_value.GetSatoshiValue(), 468750000);
  EXPECT_EQ(fee.GetSatoshiValue(), 1640);
  if (ret.size() == 2) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(312500000));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(156250000));
  }
  EXPECT_FALSE(use_bnb);
}

// SelectCoinsBnB-----------------------------------------------------------------
struct TestUtxoCoinVector {
  std::string txid;
  uint32_t vout;
  uint64_t amount;
  std::string descriptor;
};

/*
  const reqJson = {
    utxos: [{
      txid: 'eed524525ec8f94348f3d65661501b293b936e8fff4f2ff9ee70818f17367efe',
      vout: 0,
      amount: 155062500,
      descriptor: 'sh(wpkh([ef735203/0\'/0\'/7\']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))',
    }, {
      txid: 'eed524525ec8f94348f3d65661501b293b936e8fff4f2ff9ee70818f17367efe',
      vout: 0,
      amount: 85062500,
      descriptor: 'sh(wpkh([ef735203/0\'/0\'/7\']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))',
    }, {
      txid: 'efd524525ec8f94348f3d65661501b293b936e8fff4f2ff9ee70818f17367efe',
      vout: 0,
      amount: 61062500,
      descriptor: 'sh(wpkh([ef735203/0\'/0\'/7\']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))',
    }, {
      txid: 'ead524525ec8f94348f3d65661501b293b936e8fff4f2ff9ee70818f17367efe',
      vout: 0,
      amount: 39062500,
      descriptor: 'sh(wpkh([ef735203/0\'/0\'/7\']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))',
    }, {
      txid: 'eac524525ec8f94348f3d65661501b293b936e8fff4f2ff9ee70818f17367efe',
      vout: 0,
      amount: 15675000,
      descriptor: 'sh(wpkh([ef735203/0\'/0\'/7\']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))',
    }, {
      txid: '0f59594cfecf8fe1733521e29736352935711f34cd958f34df4a031858f6ecfd',
      vout: 0,
      amount: 14938590,
      descriptor: 'sh(wpkh([ef735203/0\'/0\'/4\']0231c043ae680664a2c5df38cf0d8eab29f1b61ce93855040c613b2f41f7c036af))#pezpv0hm',
    }],
    targetAmount: 100000000,
    feeInfo: {
      feeRate: 2,
      transaction: '02000000000100e1f5050000000017a914e37a3603a4d392f9ecb68b32eac6ba19adc4968f8700000000',
      isElements: false,
    },
  };
*/

static const std::vector<TestUtxoCoinVector> kExtCoinSelectTestVector = {
  {
    "",
    0,
    155062500,
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    85062500,
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    39062500,
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    61062500,
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    15675000,
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    14938590,
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  }
};

TEST(CoinSelection, SelectCoins_Simple_SelectCoinsBnB)
{
  CoinSelection coin_select(true);

  Amount target_value = Amount::CreateBySatoshiAmount(99998500);
  std::vector<Utxo> utxos;
  CoinSelectionOption option_params;
  Amount select_value;
  Amount fee_value;
  std::vector<Utxo> select_utxos;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;

  utxos.resize(kExtCoinSelectTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), "", nullptr,
        &(*ite));
    ++ite;
  }

  option_params.InitializeTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(2);

  EXPECT_NO_THROW((select_utxos = coin_select.SelectCoins(target_value, utxos,
      exp_filter, option_params, tx_fee, &select_value, &fee_value, &use_bnb)));
  EXPECT_EQ(select_utxos.size(), 2);
  EXPECT_EQ(select_value.GetSatoshiValue(), static_cast<int64_t>(100001090));
  EXPECT_EQ(fee_value.GetSatoshiValue(), static_cast<int64_t>(360));
  if (select_utxos.size() == 2) {
    EXPECT_EQ(select_utxos[0].amount, static_cast<int64_t>(85062500));
    EXPECT_EQ(select_utxos[1].amount, static_cast<int64_t>(14938590));
  }
  EXPECT_TRUE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_SelectCoinsBnB_single)
{
  CoinSelection coin_select(true);

  Amount target_value = Amount::CreateBySatoshiAmount(155060800);
  std::vector<Utxo> utxos;
  CoinSelectionOption option_params;
  Amount select_value;
  Amount fee_value;
  std::vector<Utxo> select_utxos;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;

  utxos.resize(kExtCoinSelectTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), "", nullptr,
        &(*ite));
    ++ite;
  }

  option_params.InitializeTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(2);

  EXPECT_NO_THROW((select_utxos = coin_select.SelectCoins(target_value, utxos,
      exp_filter, option_params, tx_fee, &select_value, &fee_value, &use_bnb)));
  EXPECT_EQ(select_utxos.size(), 1);
  EXPECT_EQ(select_value.GetSatoshiValue(), static_cast<int64_t>(155062500));
  EXPECT_EQ(fee_value.GetSatoshiValue(), static_cast<int64_t>(180));
  if (select_utxos.size() == 1) {
    EXPECT_EQ(select_utxos[0].amount, static_cast<int64_t>(155062500));
  }
  EXPECT_TRUE(use_bnb);
}

TEST(CoinSelection, SelectCoins_Simple_SelectCoinsBnB_empty)
{
  // BnB fail. (use knapsack)
  CoinSelection coin_select(true);

  Amount target_value = Amount::CreateBySatoshiAmount(114040000);
  std::vector<Utxo> utxos;
  CoinSelectionOption option_params;
  Amount select_value;
  Amount fee_value;
  std::vector<Utxo> select_utxos;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;

  utxos.resize(kExtCoinSelectTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), "", nullptr,
        &(*ite));
    ++ite;
  }

  option_params.InitializeTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(1);

  EXPECT_NO_THROW((select_utxos = coin_select.SelectCoins(target_value, utxos,
      exp_filter, option_params, tx_fee, &select_value, &fee_value, &use_bnb)));
  EXPECT_EQ(select_utxos.size(), 3);
  EXPECT_EQ(select_value.GetSatoshiValue(), static_cast<int64_t>(115063590));
  EXPECT_EQ(fee_value.GetSatoshiValue(), static_cast<int64_t>(270));
  if (select_utxos.size() == 3) {
    EXPECT_EQ(select_utxos[0].amount, static_cast<int64_t>(61062500));
    EXPECT_EQ(select_utxos[1].amount, static_cast<int64_t>(39062500));
    EXPECT_EQ(select_utxos[2].amount, static_cast<int64_t>(14938590));
  }
  EXPECT_FALSE(use_bnb);
}

// SelectCoins ErrorCase -----------------------------------------------------------------

TEST(CoinSelection, SelectCoins_Simple_Error_utxos_empty)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(100000000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  EXPECT_THROW(std::vector<Utxo> ret = exp_selection.SelectCoins(
      target_amount, std::vector<Utxo>(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Simple_Error_outparameter_nullptr)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(100000000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  EXPECT_THROW(std::vector<Utxo> ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, nullptr, &fee, &use_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Simple_Error_KnapsackSolver_insufficient_funds)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(9500000000);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  EXPECT_THROW(std::vector<Utxo> ret = exp_selection.SelectCoins(
      target_amount, GetBitcoinUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Simple_Error_SelectCoinsBnB_insufficient_funds)
{
  CoinSelection coin_select(true);

  Amount target_value = Amount::CreateBySatoshiAmount(500000000);
  std::vector<Utxo> utxos;
  CoinSelectionOption option_params;
  Amount select_value;
  Amount fee_value;
  std::vector<Utxo> select_utxos;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;

  utxos.resize(kExtCoinSelectTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), "", nullptr,
        &(*ite));
    ++ite;
  }

  option_params.InitializeTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(1);

  EXPECT_THROW((select_utxos = coin_select.SelectCoins(target_value, utxos,
      exp_filter, option_params, tx_fee, &select_value, &fee_value, &use_bnb)), CfdException);
}

TEST(CoinSelection, SelectCoins_Simple_Error_SelectCoinsBnB_effective_value_zero)
{
  CoinSelection coin_select(true);

  Amount target_value = Amount::CreateBySatoshiAmount(5);
  std::vector<Utxo> utxos;
  CoinSelectionOption option_params;
  Amount select_value;
  Amount fee_value;
  std::vector<Utxo> select_utxos;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;

  utxos.resize(kExtCoinSelectTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(10), "", nullptr,
        &(*ite));
    ++ite;
  }

  option_params.InitializeTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(1);

  EXPECT_THROW((select_utxos = coin_select.SelectCoins(target_value, utxos,
      exp_filter, option_params, tx_fee, &select_value, &fee_value, &use_bnb)), CfdException);
}

// CoinSelection Utility -----------------------------------------------------------------
TEST(CoinSelection, Constructor)
{
  EXPECT_NO_THROW(CoinSelection coin_select1);
  EXPECT_NO_THROW(CoinSelection coin_select2(false));
}

TEST(CoinSelection, ConvertToUtxo)
{
  uint64_t block_height = 1;
  BlockHash block_hash("1234567890123456789012345678901234567890123456789012345678901456");
  Txid txid("0034567890123456789012345678901234567890123456789012345678901456");
  uint32_t vout = 2;
  Script locking_script("0014c59664568fad58004e1fac1b275826d8b525e935");
  std::string output_descriptor("wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1)");
  Amount amount = Amount::CreateBySatoshiAmount(20000);
  void* binary_data = &vout;
  Utxo utxo = {};
  // std::vector<uint8_t> script_bytes;

  try {
    CoinSelection::ConvertToUtxo(
      block_height, block_hash, txid, vout, locking_script,
      output_descriptor, amount, binary_data, &utxo);
  } catch (const CfdException& except) {
    EXPECT_STREQ("", except.what());
  }
  EXPECT_NO_THROW((CoinSelection::ConvertToUtxo(
    block_height, block_hash, txid, vout, locking_script,
    output_descriptor, amount, binary_data, &utxo)));

  if (utxo.script_length != 0) {
    // script_bytes.resize(utxo.script_length);
    // memcpy(script_bytes.data(), utxo.locking_script, utxo.script_length);
    EXPECT_STREQ(ByteData(std::vector<uint8_t>(std::begin(utxo.locking_script), std::begin(utxo.locking_script) + utxo.script_length)).GetHex().c_str(), locking_script.GetHex().c_str());
  }
  EXPECT_EQ(utxo.block_height, block_height);
  EXPECT_STREQ(BlockHash(ByteData256(std::vector<uint8_t>(std::begin(utxo.block_hash), std::end(utxo.block_hash)))).GetHex().c_str(), block_hash.GetHex().c_str());
  EXPECT_STREQ(Txid(ByteData256(std::vector<uint8_t>(std::begin(utxo.txid), std::end(utxo.txid)))).GetHex().c_str(), txid.GetHex().c_str());
  EXPECT_EQ(utxo.vout, vout);
  EXPECT_EQ(utxo.script_length, locking_script.GetData().GetDataSize());
  EXPECT_EQ(utxo.address_type, static_cast<uint16_t>(AddressType::kP2wpkhAddress));
  EXPECT_EQ(utxo.witness_size_max, static_cast<uint16_t>(108));
  EXPECT_EQ(utxo.uscript_size_max, static_cast<uint16_t>(0));
  EXPECT_EQ(utxo.amount, amount.GetSatoshiValue());
  EXPECT_EQ(utxo.binary_data, binary_data);

#ifndef CFD_DISABLE_ELEMENTS
  ConfidentialAssetId asset("9934567890123456789012345678901234567890123456789012345678901456");
  memset(&utxo, 0, sizeof(utxo));
  EXPECT_NO_THROW((CoinSelection::ConvertToUtxo(
    block_height, block_hash, txid, vout, locking_script,
    output_descriptor, amount, asset, binary_data, &utxo)));

  if (utxo.script_length != 0) {
    // script_bytes.resize(utxo.script_length);
    // memcpy(script_bytes.data(), utxo.locking_script, utxo.script_length);
    EXPECT_STREQ(ByteData(std::vector<uint8_t>(std::begin(utxo.locking_script), std::begin(utxo.locking_script) + utxo.script_length)).GetHex().c_str(), locking_script.GetHex().c_str());
  }
  EXPECT_EQ(utxo.block_height, block_height);
  EXPECT_STREQ(BlockHash(ByteData256(std::vector<uint8_t>(std::begin(utxo.block_hash), std::end(utxo.block_hash)))).GetHex().c_str(), block_hash.GetHex().c_str());
  EXPECT_STREQ(Txid(ByteData256(std::vector<uint8_t>(std::begin(utxo.txid), std::end(utxo.txid)))).GetHex().c_str(), txid.GetHex().c_str());
  EXPECT_EQ(utxo.vout, vout);
  EXPECT_EQ(utxo.script_length, locking_script.GetData().GetDataSize());
  EXPECT_EQ(utxo.address_type, static_cast<uint16_t>(AddressType::kP2wpkhAddress));
  EXPECT_EQ(utxo.witness_size_max, static_cast<uint16_t>(108));
  EXPECT_EQ(utxo.uscript_size_max, static_cast<uint16_t>(0));
  EXPECT_EQ(utxo.amount, amount.GetSatoshiValue());
  EXPECT_EQ(utxo.binary_data, binary_data);
  EXPECT_STREQ(ConfidentialAssetId(ByteData(std::vector<uint8_t>(std::begin(utxo.asset), std::end(utxo.asset)))).GetHex().c_str(), asset.GetHex().c_str());
#endif                // CFD_DISABLE_ELEMENTS
}

// SelectCoins(With Asset) =====================================================

#ifndef CFD_DISABLE_ELEMENTS
static ConfidentialAssetId exp_dummy_asset_a("aa00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_b("bb00000000000000000000000000000000000000000000000000000000000000");
static ConfidentialAssetId exp_dummy_asset_c("cc00000000000000000000000000000000000000000000000000000000000000");

static CoinSelectionOption GetElementsOption() {
  CoinSelectionOption option;
  option.InitializeConfidentialTxSizeInfo();
  return option;
}

// FIXME Utxo illegal creating. need set witness_size_max, uscript_size_max
static std::vector<Utxo> GetElementsUtxoList() {
  std::vector<Utxo> utxos;
  {
    Txid txid("7ca81dd22c934747f4f5ab7844178445fe931fb248e0704c062b8f4fbd3d500a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 312500000;
    memcpy(utxo.asset, exp_dummy_asset_a.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("30f71f39d210f7ee291b0969c6935debf11395b0935dca84d30c810a75339a0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 78125000;
    memcpy(utxo.asset, exp_dummy_asset_a.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("9e1ead91c432889cb478237da974dd1e9009c9e22694fd1e3999c40a1ef59b0a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 1250000000;
    memcpy(utxo.asset, exp_dummy_asset_a.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("8f4af7ee42e62a3d32f25ca56f618fb2f5df3d4c3a9c59e2c3646c5535a3d40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 39062500;
    memcpy(utxo.asset, exp_dummy_asset_a.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("4d97d0119b90421818bff4ec9033e5199199b53358f56390cb20f8148e76f40a");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 156250000;
    memcpy(utxo.asset, exp_dummy_asset_a.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("b9720ed2265a4ced42425bffdb4ef90a473b4106811a802fce53f7c57487fa0b");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 2500000000;
    memcpy(utxo.asset, exp_dummy_asset_a.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b01");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 26918400;
    memcpy(utxo.asset, exp_dummy_asset_b.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b02");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 750000;
    memcpy(utxo.asset, exp_dummy_asset_b.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b03");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 346430050;
    memcpy(utxo.asset, exp_dummy_asset_b.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000b04");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 18476350;
    memcpy(utxo.asset, exp_dummy_asset_b.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c01");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 37654200;
    memcpy(utxo.asset, exp_dummy_asset_c.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  {
    Txid txid("0000000000000000000000000000000000000000000000000000000000000c02");
    struct Utxo utxo;
    memset(&utxo, 0, sizeof(utxo));
    memcpy(utxo.txid, txid.GetData().GetBytes().data(), 32);
    utxo.vout = 0;
    utxo.amount = 127030000;
    memcpy(utxo.asset, exp_dummy_asset_c.GetData().GetBytes().data(), sizeof(utxo.asset));
    utxos.push_back(utxo);
  }
  return utxos;
}

// KnapsackSolver-----------------------------------------------------------------
TEST(CoinSelection, SelectCoins_KnapsackSolver_targetvalue_0_with_asset)
{
  CoinSelectionOption option = GetElementsOption();
  option.SetEffectiveFeeBaserate(0.0);
  option.SetFeeAsset(exp_dummy_asset_a);
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(0);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(0);
  std::map<std::string, bool> map_searched_bnb;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 0);
  EXPECT_EQ(map_select_value.size(), 1);
  if (map_select_value.size() == 1) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 0);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 0);
  EXPECT_EQ(map_searched_bnb.size(), 1);
  if (map_searched_bnb.size() == 1) {
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_KnapsackSolver_match_utxo_with_asset)
{
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  // 39062500 - 820 - 1500
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(39060180);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 1);
  if (ret.size() == 1) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(39062500));
  }
  EXPECT_EQ(map_select_value.size(), 1);
  if (map_select_value.size() == 1) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 39062500);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 820);
  EXPECT_EQ(map_searched_bnb.size(), 1);
  if (map_searched_bnb.size() == 1) {
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_KnapsackSolver_ApproximateBestSubset_with_asset)
{
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(220000000);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 2);
  if (ret.size() == 1) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(156250000));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(78125000));
  }
  EXPECT_EQ(map_select_value.size(), 1);
  if (map_select_value.size() == 1) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 234375000);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 1640);
  EXPECT_EQ(map_searched_bnb.size(), 1);
  if (map_searched_bnb.size() == 1) {
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
  }
}

// SelectCoinsBnB-----------------------------------------------------------------
struct TestElementsUtxoCoinVector {
  std::string txid;
  uint32_t vout;
  uint64_t amount;
  std::string asset;
  std::string descriptor;
};

static const std::vector<TestElementsUtxoCoinVector> kExtCoinSelectElementsTestVector = {
  {
    "",
    0,
    155062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    85062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    39062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    61062500,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    15675000,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    14938590,
    exp_dummy_asset_a.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    26918400,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    750000,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    346430050,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    18476350,
    exp_dummy_asset_b.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    37654200,
    exp_dummy_asset_c.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  },
  {
    "",
    0,
    127030000,
    exp_dummy_asset_c.GetHex(),
    "sh(wpkh([ef735203/0'/0'/7']022c2409fbf657ba25d97bb3dab5426d20677b774d4fc7bd3bfac27ff96ada3dd1))"
  }
};

TEST(CoinSelection, SelectCoins_SelectCoinsBnB_with_asset)
{
  CoinSelection coin_select(true);
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(99998500);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;

  std::vector<Utxo> utxos;
  utxos.resize(kExtCoinSelectElementsTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectElementsTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), test_data.asset,
        nullptr, &(*ite));
    ++ite;
  }

  CoinSelectionOption option_params;
  option_params.InitializeConfidentialTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(2);
  option_params.SetFeeAsset(exp_dummy_asset_a);

  std::vector<Utxo> select_utxos;
  EXPECT_NO_THROW((select_utxos = coin_select.SelectCoins(map_target_amount, utxos,
      exp_filter, option_params, tx_fee, &map_select_value, &fee, &map_searched_bnb)));
  EXPECT_EQ(select_utxos.size(), 2);
  if (select_utxos.size() == 2) {
    EXPECT_EQ(select_utxos[0].amount, static_cast<int64_t>(85062500));
    EXPECT_EQ(select_utxos[1].amount, static_cast<int64_t>(14938590));
  }
  EXPECT_EQ(map_select_value.size(), 1);
  if (map_select_value.size() == 1) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 100001090);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 364);
  EXPECT_EQ(map_searched_bnb.size(), 1);
  if (map_searched_bnb.size() == 1) {
    EXPECT_TRUE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_SelectCoinsBnB_single_with_asset)
{
  CoinSelection coin_select(true);
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(155060800);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;

  std::vector<Utxo> utxos;
  utxos.resize(kExtCoinSelectElementsTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectElementsTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), test_data.asset,
        nullptr, &(*ite));
    ++ite;
  }

  CoinSelectionOption option_params;
  option_params.InitializeConfidentialTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(2);
  option_params.SetFeeAsset(exp_dummy_asset_a);

  std::vector<Utxo> select_utxos;
  EXPECT_NO_THROW((select_utxos = coin_select.SelectCoins(map_target_amount, utxos,
      exp_filter, option_params, tx_fee, &map_select_value, &fee, &map_searched_bnb)));
  EXPECT_EQ(select_utxos.size(), 1);
  if (select_utxos.size() == 1) {
    EXPECT_EQ(select_utxos[0].amount, static_cast<int64_t>(155062500));
  }
  EXPECT_EQ(map_select_value.size(), 1);
  if (map_select_value.size() == 1) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 155062500);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 182);
  EXPECT_EQ(map_searched_bnb.size(), 1);
  if (map_searched_bnb.size() == 1) {
    EXPECT_TRUE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_SelectCoinsBnB_empty_with_asset)
{
  CoinSelection coin_select(true);
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(114040000);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;

  std::vector<Utxo> utxos;
  utxos.resize(kExtCoinSelectElementsTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectElementsTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), test_data.asset,
        nullptr, &(*ite));
    ++ite;
  }

  CoinSelectionOption option_params;
  option_params.InitializeConfidentialTxSizeInfo();
  option_params.SetEffectiveFeeBaserate(1);
  option_params.SetFeeAsset(exp_dummy_asset_a);

  std::vector<Utxo> select_utxos;
  EXPECT_NO_THROW((select_utxos = coin_select.SelectCoins(map_target_amount, utxos,
      exp_filter, option_params, tx_fee, &map_select_value, &fee, &map_searched_bnb)));
  EXPECT_EQ(select_utxos.size(), 3);
  if (select_utxos.size() == 3) {
    EXPECT_EQ(select_utxos[0].amount, static_cast<int64_t>(61062500));
    EXPECT_EQ(select_utxos[1].amount, static_cast<int64_t>(39062500));
    EXPECT_EQ(select_utxos[2].amount, static_cast<int64_t>(14938590));
  }
  EXPECT_EQ(map_select_value.size(), 1);
  if (map_select_value.size() == 1) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 115063590);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 273);
  EXPECT_EQ(map_searched_bnb.size(), 1);
  if (map_searched_bnb.size() == 1) {
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
  }
}

// multiple asset target -----------------------------------------------------------------
TEST(CoinSelection, SelectCoins_KnapsackSolver_with_multiple_asset)
{
  // Same condition with "KnapsackSolver_match_utxo" and the other
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(39060180);
  map_target_amount[exp_dummy_asset_b.GetHex()] = Amount::CreateBySatoshiAmount(25000000);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 2);
  if (ret.size() == 2) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(26918400));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(78125000));
  }
  EXPECT_EQ(map_select_value.size(), 2);
  if (map_select_value.size() == 2) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 78125000);
    EXPECT_EQ(map_select_value[exp_dummy_asset_b.GetHex()].GetSatoshiValue(), 26918400);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 1640);
  EXPECT_EQ(map_searched_bnb.size(), 2);
  if (map_searched_bnb.size() == 2) {
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_b.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_CoinSelectBnB_with_multiple_asset)
{
  CoinSelection coin_select(true);
  // Same condition with "SelectCoins_SelectCoinsBnB"
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(99997900);
  map_target_amount[exp_dummy_asset_b.GetHex()] = Amount::CreateBySatoshiAmount(346495050);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetEffectiveFeeBaserate(2);
  option.SetFeeAsset(exp_dummy_asset_a);

  std::vector<Utxo> utxos;
  utxos.resize(kExtCoinSelectElementsTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectElementsTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), test_data.asset, nullptr,
        &(*ite));
    ++ite;
  }

  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = coin_select.SelectCoins(
      map_target_amount, utxos, exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 4);
  if (ret.size() == 4) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(346430050));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(750000));
    EXPECT_EQ(ret[2].amount, static_cast<int64_t>(85062500));
    EXPECT_EQ(ret[3].amount, static_cast<int64_t>(14938590));
  }
  EXPECT_EQ(map_select_value.size(), 2);
  if (map_select_value.size() == 2) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 100001090);
    EXPECT_EQ(map_select_value[exp_dummy_asset_b.GetHex()].GetSatoshiValue(), 347180050);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 728);
  EXPECT_EQ(map_searched_bnb.size(), 2);
  if (map_searched_bnb.size() == 2) {
    EXPECT_TRUE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_b.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_with_multiple_asset_fee_only_target)
{
  CoinSelection coin_select(true);
  // Same condition with "SelectCoins_SelectCoinsBnB"
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(99998500);
  map_target_amount[exp_dummy_asset_b.GetHex()] = Amount::CreateBySatoshiAmount(346495050);
  map_target_amount[exp_dummy_asset_c.GetHex()] = Amount::CreateBySatoshiAmount(0);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetEffectiveFeeBaserate(2);
  option.SetFeeAsset(exp_dummy_asset_c);

  std::vector<Utxo> utxos;
  utxos.resize(kExtCoinSelectElementsTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectElementsTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), test_data.asset, nullptr,
        &(*ite));
    ++ite;
  }

  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = coin_select.SelectCoins(
      map_target_amount, utxos, exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 5);
  if (ret.size() == 5) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(85062500));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(14938590));
    EXPECT_EQ(ret[2].amount, static_cast<int64_t>(346430050));
    EXPECT_EQ(ret[3].amount, static_cast<int64_t>(750000));
    EXPECT_EQ(ret[4].amount, static_cast<int64_t>(37654200));
  }
  EXPECT_EQ(map_select_value.size(), 3);
  if (map_select_value.size() == 3) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 100001090);
    EXPECT_EQ(map_select_value[exp_dummy_asset_b.GetHex()].GetSatoshiValue(), 347180050);
    EXPECT_EQ(map_select_value[exp_dummy_asset_c.GetHex()].GetSatoshiValue(), 37654200);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 910);
  EXPECT_EQ(map_searched_bnb.size(), 3);
  if (map_searched_bnb.size() == 3) {
    EXPECT_TRUE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_b.GetHex()]);
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_c.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_with_multiple_asset_not_consider_fee)
{
  CoinSelection coin_select(true);
  // Same condition with "SelectCoins_SelectCoinsBnB"
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(115800000);
  map_target_amount[exp_dummy_asset_b.GetHex()] = Amount::CreateBySatoshiAmount(19226350);
  map_target_amount[exp_dummy_asset_c.GetHex()] = Amount::CreateBySatoshiAmount(99060000);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetEffectiveFeeBaserate(0);
  // not set fee asset

  std::vector<Utxo> utxos;
  utxos.resize(kExtCoinSelectElementsTestVector.size());
  std::vector<Utxo>::iterator ite = utxos.begin();
  for (const auto& test_data : kExtCoinSelectElementsTestVector) {
    Txid txid;
    if (!test_data.txid.empty()) {
      txid = Txid(test_data.txid);
    }
    CoinSelection::ConvertToUtxo(
        txid, test_data.vout, test_data.descriptor,
        Amount::CreateBySatoshiAmount(test_data.amount), test_data.asset, nullptr,
        &(*ite));
    ++ite;
  }

  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = coin_select.SelectCoins(
      map_target_amount, utxos, exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 6);
  if (ret.size() == 6) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(61062500));
    EXPECT_EQ(ret[1].amount, static_cast<int64_t>(39062500));
    EXPECT_EQ(ret[2].amount, static_cast<int64_t>(15675000));
    EXPECT_EQ(ret[3].amount, static_cast<int64_t>(18476350));
    EXPECT_EQ(ret[4].amount, static_cast<int64_t>(750000));
    EXPECT_EQ(ret[5].amount, static_cast<int64_t>(127030000));
  }
  EXPECT_EQ(map_select_value.size(), 3);
  if (map_select_value.size() == 3) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), 115800000);
    EXPECT_EQ(map_select_value[exp_dummy_asset_b.GetHex()].GetSatoshiValue(), 19226350);
    EXPECT_EQ(map_select_value[exp_dummy_asset_c.GetHex()].GetSatoshiValue(), 127030000);
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 0);
  EXPECT_EQ(map_searched_bnb.size(), 3);
  if (map_searched_bnb.size() == 3) {
    EXPECT_TRUE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
    EXPECT_TRUE(map_searched_bnb[exp_dummy_asset_b.GetHex()]);
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_c.GetHex()]);
  }
}

TEST(CoinSelection, SelectCoins_Error_no_target_value_map) {
  AmountMap map_target_amount;
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  option.SetEffectiveFeeBaserate(0.0);
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 0);
  EXPECT_EQ(map_select_value.size(), 0);
  EXPECT_EQ(fee.GetSatoshiValue(), 0);
  EXPECT_EQ(map_searched_bnb.size(), 0);
}

TEST(CoinSelection, SelectCoins_empty_target_value_map_but_calculate_fee) {
  AmountMap map_target_amount;
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  std::vector<Utxo> ret;
  EXPECT_NO_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb));

  EXPECT_EQ(ret.size(), 1);
  if (ret.size() == 1) {
    EXPECT_EQ(ret[0].amount, static_cast<int64_t>(39062500));
  }
  EXPECT_EQ(map_select_value.size(), 1);
  if (map_select_value.size() == 1) {
    EXPECT_EQ(map_select_value[exp_dummy_asset_a.GetHex()].GetSatoshiValue(), static_cast<int64_t>(39062500));
  }
  EXPECT_EQ(fee.GetSatoshiValue(), 820);
  EXPECT_EQ(map_searched_bnb.size(), 1);
  if (map_searched_bnb.size() == 1) {
    EXPECT_FALSE(map_searched_bnb[exp_dummy_asset_a.GetHex()]);
  }
}

// SelectCoins ErrorCase -----------------------------------------------------------------

TEST(CoinSelection, SelectCoins_Error_target_asset_not_contains_utxos) {
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(100000000);
  map_target_amount["0000000000000000000000000000000000000000000000000000000000000000"] = 
    Amount::CreateBySatoshiAmount(1000);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  std::vector<Utxo> ret;
  std::vector<Utxo> tmp;
  EXPECT_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, tmp, exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Error_target_asset_empty_string)
{
  AmountMap map_target_amount;
  map_target_amount[""] = Amount::CreateBySatoshiAmount(39060180);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  std::vector<Utxo> ret;
  EXPECT_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Error_empty_utxo) {
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(100000000);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  std::vector<Utxo> ret;
  EXPECT_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, std::vector<Utxo>(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Error_without_fee_asset)
{
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(39060180);
  AmountMap map_select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  std::vector<Utxo> ret;
  EXPECT_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, &map_select_value, &fee, &map_searched_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Error_no_outparameter)
{
  AmountMap map_target_amount;
  map_target_amount[exp_dummy_asset_a.GetHex()] = Amount::CreateBySatoshiAmount(39060180);
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  std::map<std::string, bool> map_searched_bnb;
  CoinSelectionOption option = GetElementsOption();
  option.SetFeeAsset(exp_dummy_asset_a);
  std::vector<Utxo> ret;
  EXPECT_THROW(ret = exp_selection.SelectCoins(
      map_target_amount, GetElementsUtxoList(), exp_filter, option,
      tx_fee, nullptr, &fee, &map_searched_bnb), CfdException);
}

TEST(CoinSelection, SelectCoins_Error_CoinSelections_Simple_to_multiple_asset_utxo)
{
  Amount target_amount = Amount::CreateBySatoshiAmount(39060180);
  Amount select_value;
  Amount fee;
  Amount tx_fee = Amount::CreateBySatoshiAmount(1500);
  bool use_bnb = false;
  std::vector<Utxo> ret;
  EXPECT_THROW(ret = exp_selection.SelectCoins(
      target_amount, GetElementsUtxoList(), exp_filter, GetBitcoinOption(),
      tx_fee, &select_value, &fee, &use_bnb), CfdException);

  EXPECT_EQ(ret.size(), 0);
  EXPECT_EQ(select_value.GetSatoshiValue(), 0);
  EXPECT_EQ(fee.GetSatoshiValue(), 0);
  EXPECT_FALSE(use_bnb);
}

#endif