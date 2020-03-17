#include "gtest/gtest.h"

#include <stdexcept>
#include <string>

#include "capi/cfdc_internal.h"
#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_elements_transaction.h"
#include "cfdc/cfdcapi_ledger.h"
#include "cfdcore/cfdcore_exception.h"
#include "cfdcore/cfdcore_script.h"

/**
 * @brief testing class.
 */
class cfdcapi_ledger : public ::testing::Test {
 protected:
  virtual void SetUp() { }
  virtual void TearDown() { }
};

#ifndef CFD_DISABLE_ELEMENTS
TEST(cfdcapi_ledger, CfdSerializeConfidentialTxForLedger) {
  static const char* const kTxData = "020000000101e46aa5d11fb7f2243e75bbd90ad94e5ae0d1992c4a5c0d6ba71a77e73df8e23b00000000171600142851f8219e77b0f3ae8421a8274168c743e574d2ffffffff02016d521c38ec1ea15734ae22b7c46064412829c0d0579f0a713d1c04ede979026f0100000000000005dc00000b547995d08f8fa9e87e122548afc2ea80f552aee8e45a44df4cb81292fed26530086bd721b7048f58fc925f56b0642e107a492c9509b475186f7fd573e47a500437020129d9733769531b80adf2a6ab2f8e212beb26f94ebc2afdb43d5b04cac794b41976a914af991f5a938a1767f93adac12bdf8f42156a4ecc88ac00000000000002483045022100917ccdbe993dbc4c0bfe0839bf7df16eb481a175c60a7badbb676145e75f924a022012a868a2286420a3d0793e241c4f767a95c985db7c44a0c8a0e872843fc230c8012102bff933ba22e896a0cc0a0834f11c6afb63995605d4def392776a26535cea8e9b00000043010001bd2e6fb69a23e881376f7c692de1aebfd700a905c8a30b493a855685e939f870adf7d6deae5197ddac0e685b0030e76488ec549d4467a4fe1d8e814b3a453878fd4d0b60230000000000000001ea10015893fbe5cadd09e8a2944bb85a3ac04a65a345fbfc5598d5590624603335faec0351b03a46b5a9cc763ee1769badde123ec6aa62c9c0f9f62a868f8c38b4e20a2abd02e74ec5076b38d7893d8a7eb6677b1fa4e7b26716672e059d4b2d4bb965eadc0b1f2670ac51e8e97d9d5c1774c07ea5555ccbb27f7ebc3d7aa0a8eaf2229780587d2289724cc9721d9b0db71f799d59422f95dda24aa357253fb1b411f0538646120a92227e13897dcc8627a6a915f568e9a4ad56b165002c4e9f8db6d2f5c3b6fa5f81851a7c0d70360e4a6504eab5761d79a992f1a9f94360b20027c6fe190deb6bd1b15014f33d6c958b3d8b33b2a7f8385674a4c043fbdffbecba492c5fc8e0bbf2f6d5221a6aae302454b902e679f7e28805a7c351a4e0ce238857ff53c27dc390f3d5ee56d288e92e88da4f51a38e509886fdfba9c957eed31d1549af668bcac6e62886416e4a0ac99c873d03779c5a19ee154ba8c4b12e689e095ea606c024d7beec052e451113dfe1cbcedc182f5434c490d867c3d1aa0d56845b098c87372131e898f74068c4ee43ace49029fd2903b69e0c7fe01a19fae0436880077818cca0c2a8f01975eb3580ce89ae8ffae76bf01ee77a576736eed52849b9584a87eef8b05446d34a33e77ccf117a6aaf3acb0e290e8a3cd5020a6ffe32879d81dd473d0c960052d67825341b1de30b8cea5fe94b9f024bcd6ac2f5963d0ee69137b08714b168d4121837e490bab2815e384d0d15c6d98d990ca61bf648e6fbe89c4b5b22eb7ad68e1688f17fbccadc1ea232f7c4cc10348b9f32ac1ac966c85a7c30e7d2f5582d2d327f106197f19994b23c76026fe034f2e90cb0c9c85ae5693d3f1f9adf9391f70a3d50e0b2ff0e5fe6062e2e62689e5eebb3cb1728b0f553bc5bf288f178240ec4c2028387be30cbfc7d95219e6aa14df11e850c1eed06c3b4e6af58619f4ee93e826f548f6d4841e345215b97b90356d641c367fb478a7207e170fba9d9cb1eb75bab69ccff6f83c28d48132432571074503da5123f09f406a0b632b03d18d2a6320da8b51d46b13daedf69054f352a29bb7e5b95e14f0fc8f2f53478a61d2e754aacaeb7d1287eee4d74b0892263d09bd0423c50940527d404bbe1135c683465553d558f79c00e6cd88ba833fe72732cb624e7c432433236e2f80c5d29665d909059ad1b1dd17b8a5e1cef0bb6a3ff10908435956c82b8347e0871b36d0f1d7b46c12ac2454af4412ac5675621b6feca07730fd14cc0f3d50d6485e0b59ae6abaa064e0514b24c57abf5bb20740b15ef6d64f249bbda993c8f90a5235a7170572cc5e213fea65cb9617ade120349cefaf10e0a945e3ac039021a2dc3652070cc4bdfcd5e2d451bd6bf28f25e4e68f5832b21566829fd37500ac1ae6429132e97278a2bf82074500f5676dcff3adfdefc88daf1a81ab23db5fe7089154219a258518fb204b3fe9ad572bc47fd4c5a200a14593cc7efe24b7d98144ae35d4e6328c76c1807168a6d799ed3aad5f4586bc1437d7cdf38b026b9e131f39d0bba348b6598b7fbc9f2c9ec0bcc03c28329b416bffa285644031d489e86d43ef12b3f42e76c36e5ae9df1f3ac0f1d9cdd5d4d7c446b19d1d8d9a58c93ae27ff8daab5e49104b991b38ef4dd848d9898d1814ebf391bf57a9314f298268c5fa884c114e83a87210aa8b483be9c3f5a4eb345af951bfde6157bfbbfc7fae28297a534f2651d5c4f11314b29859510d8fa99127435c30c2e99a308a68e6b699967fe5b5ab35146b758e876450c1a03b9b90fcb7cc05929680ed282d93e53c1d56590928b547e10222db9ba6f2d8d0afaa93af871e4a8f9d9e69f07fede10ec6f088fd5195349e566fa420583822371ab2778327019548de0ed9cd4e412b14fed14f768a6b5f11ac26637779dac567a2372ad033fbc9a3ada97b10d6bde95fbe665215068a6a9f29872d102a14c7edcd0ccd19f47c09ddef7aae4d1a2bd6050806c662e4669391996c869bf94d3863589f96b02be28d045ed7eaa92c399289de5b193fd4db5da347fee11eb9b1ac901a2ad542496b8c869e0c55ef27d8b7b4905fd24d652789dcc89a59368f0ca0f0ea8c99b4d12c14c35e77a96d4a0f0f38044fa51125991d655c85988e150f7bc3b8f9c69e6b1df46feafdd74aef1e91b7075f45dfaab28adac7a0cb3bfb5b11bc21e3db880448ff78f6f32335d218e2d9ee5a99b64a2649c66d7b7385bc9a7ae3fa44f937128e589f4fe99ea601e40c1b61c53c06657ea9097be13d546453d5bd2864db7f26ed5167d9268584165798aef967260cf8f96dfb9054c07013e1fbf59f550c43017f753ff4ee7afcd47e0e7f682376cff563778e062362dca0c7b1a7ffcce268f42832d75a42b9358720ef827a218221810fc8c28f10d5fab56fd5cce327b1c8d5f6b94225a97855a85783cd9e0b15dc05317d4d70cce6c010205319985ae0e6cd9d24e71c7904d3e8aece8b90aa81eca459c4f2627556dbffa0126b37bd07307e7bb8b01d53cd5f68900ed066392f376806b56790afc249766cd98f9bcc3aaa69c22fb5acdbd320e550131ce16654ede5077043db69a541b8568c715bfda43ea72673a46d1657318629dd83843c60fd5f296f5e26f5aede395d4f46812d977b1cc8a0f1396319a196268f1904e7c9b10e4c191b5f233074dc047c8a1d4733802789a22c54b8776d877dd8995b6d1e8d22f2efd4260a50eb4fc86b9c144242630217f1f2ea75981f4cc02025332c1683a60228eb73d10ceb179a59d11c00b55976fe4ae48e4a5b2a6ac4718923c511d241c63574065959620653cfde157ae5494ff4d2eca43ee70741f13eb8e19875fdc265e52087973e51d374ff7f1ae8eb386347749d39c886f670fd580ce2f24c1092433d76c7cc030a6c5495665dbe0909eccf8e0f77b268fc4b87fcac541599ae1ceace33f50b02d952a7c60a1dbfb75dfcba104f41c85a7133b2d97617874d8ba8f10ed5b5d1d5798d495384088a6d6a6a4af3c3bb755c5806bac3ad273af1dd58a6ce1bd5f5a9f9d2e2b24e2ab75ea9fd8b631ea29f07d1e90517f1690a110530aac7b29c8fe468afc2be93d953d5b172e7c8668a2658f0c95c96043ce03e5f57857e80011117ecbb6f10dd0e0c3deb269b24cf46f5d4ac875e8bb461151cfc66b69ad8d15d2a4ebbc247a20983d94b6f92701ab25bcdd2b768ea44b3c009c150d8aa71fc4440ac0ab6622e29fcc15629a82f3e07bca6eb6900f2670b219ef2ba12edc1a76b6892efb025e38a7a7b1f0a6ac36caa6abf4851ca811e795ba710709d788532026bd134f7ae9c5f3972a9fbec5991f9bb1747eb4064c66eb2e4161c901befab36a9bac48d9b77516a36a722dd705d0da3db493f72bd7195ee8ff99b4626b857e9e34fd6f29f077eb2add160266f7364db4f2b6c39331352f95e7377e10629f9478b5ee57165ff0cbeff7c6481c7be82cd245c462d0ec9c79e35fbc6dd44c427c2a85ce5b79543ea50dd909d59f7de80467aa0e88c8dfeabe74961db74ed4fe9f04454d21f4aad89c26da4a6f5299565edcb1991f9c9f28271604b6540d0556241e67a4d613f4fb0c52ef437c8da0c5e3bbe4798c8659cd393bfc7d10a13edc9c18ab6809cde93ae805c6ee2b3af72ac570678bb1dc0268ad034b18092b8bd9c6f97af6ef0b9d958099b5fe4ba0dd08f004e7d36aee2eedea403a004486eeb22ed71cebdb3413106c7bbd826a7c6c4efe4cc0deb1861212d2bb535c3adefba28ebf3623bf2782bd4600fbcc6a9df20002a47c54cc660a6b27211a8d3dd0444e1a2298260ea6ffa6088ace1590ae7ee9951f7f620e55b49beb0ec8ad1262c98b1ec1b4fa75fd1ff4931b71388e07b31d699246ef6fe59bba6f69b7111ea73f51e685fea3798d2d74fedb900c9ff6ba2cabd150fdf936eca041eb27c38c529767ab14f9b1f9b694c3f2d103779fa3c5d2c75fd287be2fe4169dc05951895b884247e65e8a88ac6628e1abada705287ecb1b3441402dbd6bd0550042e5ae6502d771b844ff46eaae6c61bee5f0ecbd3c61df";

  static const char* const kExpTxData = "0200000001e46aa5d11fb7f2243e75bbd90ad94e5ae0d1992c4a5c0d6ba71a77e73df8e23b00000000ffffffff026f0279e9ed041c3d710a9f57d0c02928416460c4b722ae3457a11eec381c526d00000000000005dc006f0279e9ed041c3d710a9f57d0c02928416460c4b722ae3457a11eec381c526d0000000000032e741976a914af991f5a938a1767f93adac12bdf8f42156a4ecc88ac";

  static const char* const metadata1_asset = "6F0279E9ED041C3D710A9F57D0C02928416460C4B722AE3457A11EEC381C526D";
  static const char* const metadata1_value = "00000000000005DC";
  static const char* const metadata2_asset = metadata1_asset;
  static const char* const metadata2_value = "0000000000032E74";

  void* handle = NULL;
  int ret = CfdCreateHandle(&handle);
  EXPECT_EQ(kCfdSuccess, ret);
  EXPECT_FALSE((NULL == handle));

  void* serialize_handle = nullptr;
  ret = CfdInitializeTxSerializeForLedger(handle, &serialize_handle);
  EXPECT_EQ(kCfdSuccess, ret);

  if (ret == kCfdSuccess) {
    char* serialize_hex = nullptr;

    ret = CfdAddTxOutMetaDataForLedger(
        handle, serialize_handle, 0, metadata1_asset, metadata1_value, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdAddTxOutMetaDataForLedger(
        handle, serialize_handle, 1, metadata2_asset, metadata2_value, nullptr);
    EXPECT_EQ(kCfdSuccess, ret);

    ret = CfdFinalizeTxSerializeForLedger(
        handle, serialize_handle, kCfdNetworkLiquidv1, kTxData, false, true, &serialize_hex);
    EXPECT_EQ(kCfdSuccess, ret);
    if (ret == kCfdSuccess) {
      EXPECT_STREQ(kExpTxData, serialize_hex);
      CfdFreeStringBuffer(serialize_hex);
    }

    ret = CfdFreeTxSerializeForLedger(handle, serialize_handle);
    EXPECT_EQ(kCfdSuccess, ret);
  }

  ret = CfdGetLastErrorCode(handle);
  if (ret != kCfdSuccess) {
    char* str_buffer = NULL;
    ret = CfdGetLastErrorMessage(handle, &str_buffer);
    EXPECT_EQ(kCfdSuccess, ret);
    EXPECT_STREQ("", str_buffer);
    CfdFreeStringBuffer(str_buffer);
    str_buffer = NULL;
  }

  ret = CfdFreeHandle(handle);
  EXPECT_EQ(kCfdSuccess, ret);
}
#endif  // CFD_DISABLE_ELEMENTS