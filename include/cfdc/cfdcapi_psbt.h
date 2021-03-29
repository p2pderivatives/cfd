/* Copyright 2021 CryptoGarage */
/**
 * @file cfdcapi_psbt.h
 *
 * @brief API definition file of PSBT for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_PSBT_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_PSBT_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include "cfdc/cfdcapi_address.h"
#include "cfdc/cfdcapi_common.h"

/** PSBT record type */
enum CfdPsbtRecordType {
  /** record type: global */
  kCfdPsbtRecordTypeGlobal = 1,
  /** record type: input */
  kCfdPsbtRecordTypeInput = 2,
  /** record type: output */
  kCfdPsbtRecordTypeOutput = 3,
};

/** PSBT record kind */
enum CfdPsbtRecordKind {
  /** record: input signature */
  kCfdPsbtRecordInputSignature = 1,
  /** record: input bip32 */
  kCfdPsbtRecordInputBip32 = 2,
  /** record: output bip32 */
  kCfdPsbtRecordOutputBip32 = 3,
  /** record: global xpub bip32 */
  kCfdPsbtRecordGloalXpub = 4,
  /** record: input final witness */
  kCfdPsbtRecordFinalWitness = 5,
  /** record: input unknown keys */
  kCfdPsbtRecordInputUnknownKeys = 6,
  /** record: output unknown keys */
  kCfdPsbtRecordOutputUnknownKeys = 7,
  /** record: global unknown keys */
  kCfdPsbtRecordGlobalUnknownKeys = 8,
};

/** fund psbt option */
enum CfdFundPsbtOption {
  /** dust fee rate (double) */
  kCfdPsbtFundEstimateFeeRate = 1,
  /** dust fee rate (double) */
  kCfdPsbtFundDustFeeRate = 2,
  /** longterm fee rate (double) */
  kCfdPsbtFundLongTermFeeRate = 3,
  /** knapsack min change (int64) */
  kCfdPsbtFundKnapsackMinChange = 4,
  /** use blind fee (bool) */
  kCfdPsbtFundIsBlind = 11,
  /** blind option: exponent */
  kCfdPsbtFundBlindExponent = 12,
  /** blind option: minBits */
  kCfdPsbtFundBlindMinimumBits = 13,
};

/**
 * @brief Create PSBT handle.
 * @param[in,out] handle      cfd handle.
 * @param[in] net_type        network type.
 * @param[in] psbt_string     psbt base64 or hex. (if empty, using tx data)
 * @param[in] tx_hex_string   transaction hex. (if empty, using tx version)
 * @param[in] version         transaction version.
 * @param[in] locktime        transaction lock time.
 * @param[out] psbt_handle    psbt handle.
 *   Call 'CfdFreePsbtHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreatePsbtHandle(
    void* handle, int net_type, const char* psbt_string,
    const char* tx_hex_string, uint32_t version, uint32_t locktime,
    void** psbt_handle);

/**
 * @brief Free PSBT handle.
 * @param[in,out] handle        cfd handle.
 * @param[in,out] psbt_handle   psbt handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreePsbtHandle(void* handle, void* psbt_handle);

/**
 * @brief Get a base64 encoded PSBT.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[out] psbt_base64      psbt by base64.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] psbt_hex         psbt by hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtData(
    void* handle, void* psbt_handle, char** psbt_base64, char** psbt_hex);

/**
 * @brief Get PSBT data.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[out] psbt_version     psbt version.
 * @param[out] base_tx          base transaction.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] txin_count       num of tx input.
 * @param[out] txout_count      num of tx output.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtGlobalData(
    void* handle, void* psbt_handle, uint32_t* psbt_version, char** base_tx,
    uint32_t* txin_count, uint32_t* txout_count);

/**
 * @brief Join a PSBT.
 * @param[in,out] handle        cfd handle.
 * @param[in,out] psbt_handle   psbt handle.
 * @param[in] psbt_join_base64  join target psbt base64.
 * @return CfdErrorCode
 */
CFDC_API int CfdJoinPsbt(
    void* handle, void* psbt_handle, const char* psbt_join_base64);

/**
 * @brief Sign to PSBT.
 * @param[in,out] handle        cfd handle.
 * @param[in,out] psbt_handle   psbt handle.
 * @param[in] privkey           privkey. (hex or wif)
 * @param[in] has_grind_r       grind_r flag.
 * @return CfdErrorCode
 */
CFDC_API int CfdSignPsbt(
    void* handle, void* psbt_handle, const char* privkey, bool has_grind_r);

/**
 * @brief Combine a PSBT.
 * @param[in,out] handle            cfd handle.
 * @param[in,out] psbt_handle       psbt handle.
 * @param[in] psbt_combine_base64   combine target psbt base64.
 * @return CfdErrorCode
 */
CFDC_API int CfdCombinePsbt(
    void* handle, void* psbt_handle, const char* psbt_combine_base64);

/**
 * @brief Finalize a PSBT.
 * @param[in,out] handle        cfd handle.
 * @param[in,out] psbt_handle   psbt handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizePsbt(void* handle, void* psbt_handle);

/**
 * @brief Extract a PSBT.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[out] transaction      transaction hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdExtractPsbtTransaction(
    void* handle, void* psbt_handle, char** transaction);

/**
 * @brief Check finalized PSBT.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @retval kCfdSuccess                  finalized.
 * @retval kCfdSignVerificationError    invalid finalized.
 * @retval other                        illegal error.
 */
CFDC_API int CfdIsFinalizedPsbt(void* handle, void* psbt_handle);

/**
 * @brief Check finalized PSBT input.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @retval kCfdSuccess                  finalized.
 * @retval kCfdSignVerificationError    invalid finalized.
 * @retval other                        illegal error.
 */
CFDC_API int CfdIsFinalizedPsbtInput(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout);

/**
 * @brief Add PSBT input with pubkey.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[in] sequence          sequence.
 * @param[in] amount            utxo amount.
 * @param[in] locking_script    utxo locking script.
 * @param[in] descriptor        descriptor.
 * @param[in] full_tx_hex       full transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddPsbtTxInWithPubkey(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    uint32_t sequence, int64_t amount, const char* locking_script,
    const char* descriptor, const char* full_tx_hex);

/**
 * @brief Add PSBT input with script.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[in] sequence          sequence.
 * @param[in] amount            utxo amount.
 * @param[in] locking_script    utxo locking script.
 * @param[in] redeem_script     utxo redeem script.
 * @param[in] descriptor        descriptor.
 * @param[in] full_tx_hex       full transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddPsbtTxInWithScript(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    uint32_t sequence, int64_t amount, const char* locking_script,
    const char* redeem_script, const char* descriptor,
    const char* full_tx_hex);

/**
 * @brief set PSBT input utxo.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[in] amount            utxo amount.
 * @param[in] locking_script    utxo locking script.
 * @param[in] full_tx_hex       full transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetPsbtTxInUtxo(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* locking_script, const char* full_tx_hex);

/**
 * @brief Set PSBT input bip32 pubkey.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[in] pubkey            pubkey. (direct or path data)
 * @param[in] fingerprint       fingerprint.
 * @param[in] bip32_path        bip32_path.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetPsbtTxInBip32Pubkey(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    const char* pubkey, const char* fingerprint, const char* bip32_path);

/**
 * @brief Set PSBT signature.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[in] pubkey            pubkey.
 * @param[in] der_signature     der encoded signature.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetPsbtSignature(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    const char* pubkey, const char* der_signature);

/**
 * @brief Set PSBT sighash type.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[in] sighash_type      sighash type.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetPsbtSighashType(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int sighash_type);

/**
 * @brief Set PSBT finalize input script.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[in] scriptsig         scripsig format script.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetPsbtFinalizeScript(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    const char* scriptsig);

/**
 * @brief Clear PSBT sign data.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @return CfdErrorCode
 */
CFDC_API int CfdClearPsbtSignData(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout);

/**
 * @brief Get PSBT sighash type.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[out] sighash_type     sighash type.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtSighashType(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int* sighash_type);

/**
 * @brief Get PSBT input utxo data.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[out] amount           amount.
 * @param[out] locking_script   locking script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] redeem_script    redeem script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] descriptor       descriptor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] full_tx_hex      full transaction hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtUtxoData(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    int64_t* amount, char** locking_script, char** redeem_script,
    char** descriptor, char** full_tx_hex);

/**
 * @brief Get PSBT input utxo data by index.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] index             input index.
 * @param[out] txid             txid.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] vout             vout.
 * @param[out] amount           amount.
 * @param[out] locking_script   locking script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] redeem_script    redeem script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] descriptor       descriptor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] full_tx_hex      full transaction hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtUtxoDataByIndex(
    void* handle, void* psbt_handle, uint32_t index, char** txid,
    uint32_t* vout, int64_t* amount, char** locking_script,
    char** redeem_script, char** descriptor, char** full_tx_hex);

/**
 * @brief Add PSBT output with pubkey.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] amount            txout amount.
 * @param[in] locking_script    txout locking script. (if empty, use descriptor)
 * @param[in] descriptor        descriptor.
 * @param[out] index            txout index.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddPsbtTxOutWithPubkey(
    void* handle, void* psbt_handle, int64_t amount,
    const char* locking_script, const char* descriptor, uint32_t* index);

/**
 * @brief Add PSBT output with script.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] amount            txout amount.
 * @param[in] locking_script    txout locking script. (if empty, use descriptor)
 * @param[in] redeem_script     txout redeem script.
 * @param[in] descriptor        descriptor.
 * @param[out] index            txout index.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddPsbtTxOutWithScript(
    void* handle, void* psbt_handle, int64_t amount,
    const char* locking_script, const char* redeem_script,
    const char* descriptor, uint32_t* index);

/**
 * @brief Set PSBT output bip32 pubkey.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] index             txout index.
 * @param[in] pubkey            pubkey. (direct or path data)
 * @param[in] fingerprint       fingerprint.
 * @param[in] bip32_path        bip32_path.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetPsbtTxOutBip32Pubkey(
    void* handle, void* psbt_handle, uint32_t index, const char* pubkey,
    const char* fingerprint, const char* bip32_path);

/**
 * @brief Get PSBT input index.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @param[out] index            txin index.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtTxInIndex(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout,
    uint32_t* index);

/**
 * @brief Get PSBT with pubkey record.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] kind              PSBT kind. (see CfdPsbtRecordKind)
 * @param[in] index             input or output index.
 * @param[in] pubkey            pubkey or xpubkey.
 * @param[out] value            record value.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtPubkeyRecord(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    const char* pubkey, char** value);  // (txin:signature, key. txout:key)

/**
 * @brief Find PSBT with pubkey record.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] kind              PSBT kind. (see CfdPsbtRecordKind)
 * @param[in] index             input or output index.
 * @param[in] pubkey            pubkey or xpubkey.
 * @return CfdErrorCode
 */
CFDC_API int CfdIsFindPsbtPubkeyRecord(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    const char* pubkey);

/**
 * @brief Get PSBT bip32 key data.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] kind              PSBT kind. (see CfdPsbtRecordKind)
 * @param[in] index             input or output index.
 * @param[in] pubkey            pubkey or xpubkey.
 * @param[out] fingerprint      fingerprint.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] bip32_path       bip32 path.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtBip32Data(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    const char* pubkey, char** fingerprint, char** bip32_path);

/**
 * @brief Get PSBT pubkey list handle.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] kind              PSBT kind. (see CfdPsbtRecordKind)
 * @param[in] index             input or output index.
 * @param[out] list_num             pubkey list count.
 * @param[out] pubkey_list_handle   pubkey list handle.
 *   Call 'CfdFreePsbtPubkeyList' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtPubkeyList(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    uint32_t* list_num, void** pubkey_list_handle);

/**
 * @brief Get PSBT pubkey list data.
 * @param[in,out] handle            cfd handle.
 * @param[in] pubkey_list_handle    pubkey list handle.
 * @param[in] index                 list index.
 * @param[out] pubkey               pubkey or xpubkey. (require)
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] pubkey_hex           pubkey or xpubkey by hex string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtPubkeyListData(
    void* handle, void* pubkey_list_handle, uint32_t index, char** pubkey,
    char** pubkey_hex);

/**
 * @brief Get PSBT pubkey list data.
 * @param[in,out] handle            cfd handle.
 * @param[in] pubkey_list_handle    pubkey list handle.
 * @param[in] index                 list index.
 * @param[out] pubkey               pubkey or xpubkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] fingerprint          fingerprint.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] bip32_path           bip32 path.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtPubkeyListBip32Data(
    void* handle, void* pubkey_list_handle, uint32_t index, char** pubkey,
    char** fingerprint, char** bip32_path);

/**
 * @brief Free PSBT pubkey list.
 * @param[in,out] handle            cfd handle.
 * @param[in] pubkey_list_handle    pubkey list handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreePsbtPubkeyList(void* handle, void* pubkey_list_handle);

/**
 * @brief Get PSBT data list handle.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] kind              PSBT kind. (see CfdPsbtRecordKind)
 * @param[in] index             input or output index.
 * @param[out] list_num             pubkey list count.
 * @param[out] pubkey_list_handle   pubkey list handle.
 *   Call 'CfdFreePsbtPubkeyList' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtByteDataList(
    void* handle, void* psbt_handle, int kind, uint32_t index,
    uint32_t* list_num, void** pubkey_list_handle);

/**
 * @brief Get PSBT data list item.
 * @param[in,out] handle            cfd handle.
 * @param[in] data_list_handle      data list handle.
 * @param[in] index                 list index.
 * @param[out] data                 data.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtByteDataItem(
    void* handle, void* data_list_handle, uint32_t index, char** data);

/**
 * @brief Free PSBT data list.
 * @param[in,out] handle            cfd handle.
 * @param[in] data_list_handle      data list handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreePsbtByteDataList(void* handle, void* data_list_handle);

/**
 * @brief Add PSBT global bip32 xpubkey.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] xpubkey           xpubkey. (direct or path data)
 * @param[in] fingerprint       fingerprint.
 * @param[in] bip32_path        bip32_path.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddPsbtGlobalXpubkey(
    void* handle, void* psbt_handle, const char* xpubkey,
    const char* fingerprint, const char* bip32_path);

/**
 * @brief Set PSBT redeem script or witness script.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] type              psbt type. (see CfdPsbtRecordType)
 * @param[in] index             index.
 * @param[in] redeem_script     redeem script
 * @return CfdErrorCode
 */
CFDC_API int CfdSetPsbtRedeemScript(
    void* handle, void* psbt_handle, int type, uint32_t index,
    const char* redeem_script);

/**
 * @brief Add PSBT record.
 * @param[in,out] handle    cfd handle.
 * @param[in] psbt_handle   psbt handle.
 * @param[in] type          psbt type. (see CfdPsbtRecordType)
 * @param[in] index         index. (if type is global, this field is not use.)
 * @param[in] key           key hex.
 * @param[in] value         value hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddPsbtRecord(
    void* handle, void* psbt_handle, int type, uint32_t index, const char* key,
    const char* value);

/**
 * @brief Get PSBT record.
 * @param[in,out] handle    cfd handle.
 * @param[in] psbt_handle   psbt handle.
 * @param[in] type          psbt type. (see CfdPsbtRecordType)
 * @param[in] index         index. (if type is global, this field is not use.)
 * @param[in] key           key hex.
 * @param[out] value        value hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetPsbtRecord(
    void* handle, void* psbt_handle, int type, uint32_t index, const char* key,
    char** value);

/**
 * @brief Find PSBT record.
 * @param[in,out] handle    cfd handle.
 * @param[in] psbt_handle   psbt handle.
 * @param[in] type          psbt type. (see CfdPsbtRecordType)
 * @param[in] index         index. (if type is global, this field is not use.)
 * @param[in] key           key hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdIsFindPsbtRecord(
    void* handle, void* psbt_handle, int type, uint32_t index,
    const char* key);

/**
 * @brief Verify PSBT input.
 * @param[in,out] handle        cfd handle.
 * @param[in] psbt_handle       psbt handle.
 * @param[in] txid              txid.
 * @param[in] vout              vout.
 * @return CfdErrorCode
 */
CFDC_API int CfdVerifyPsbtTxIn(
    void* handle, void* psbt_handle, const char* txid, uint32_t vout);

/**
 * @brief Initialize fund PSBT handle.
 * @param[in] handle            cfd handle.
 * @param[out] fund_handle      fund PSBT handle.
 *   Call 'CfdFreeFundPsbt' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeFundPsbt(void* handle, void** fund_handle);

/**
 * @brief Add utxo on fund PSBT.
 * @param[in] handle                cfd handle.
 * @param[in] fund_handle           fund PSBT handle.
 * @param[in] txid                  utxo txid.
 * @param[in] vout                  utxo vout.
 * @param[in] amount                utxo amount.
 * @param[in] asset                 utxo asset.
 * @param[in] descriptor            utxo output descriptor.
 * @param[in] scriptsig_template    utxo scriptsig template.
 * @param[in] full_utxo_tx          utxo full transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdFundPsbtAddToUtxoList(
    void* handle, void* fund_handle, const char* txid, uint32_t vout,
    int64_t amount, const char* asset, const char* descriptor,
    const char* scriptsig_template, const char* full_utxo_tx);

/**
 * @brief Set fund PSBT optional parameter.
 * @param[in] handle            cfd handle.
 * @param[in] fund_handle       fund PSBT handle.
 * @param[in] key               key. (see CfdFundPsbtOption)
 * @param[in] int64_value       int64 value.
 * @param[in] double_value      double value.
 * @param[in] bool_value        bool value.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetOptionFundPsbt(
    void* handle, void* fund_handle, int key, int64_t int64_value,
    double double_value, bool bool_value);

/**
 * @brief Finalize fund PSBT.
 * @param[in] handle                cfd handle.
 * @param[in,out] psbt_handle   psbt handle.
 * @param[in] fund_handle           fund PSBT handle.
 * @param[in] change_address_descriptor  change address descriptor.
 * @param[out] tx_fee               tx fee.
 * @param[out] used_utxo_count      used utxo count.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeFundPsbt(
    void* handle, void* psbt_handle, void* fund_handle,
    const char* change_address_descriptor, int64_t* tx_fee,
    uint32_t* used_utxo_count);

/**
 * @brief Get used utxo by fund PSBT.
 * @param[in] handle                cfd handle.
 * @param[in] fund_handle           fund PSBT handle.
 * @param[in] index                 used utxo index.
 * @param[out] utxo_index           set utxo list index.
 * @param[out] txid                 utxo txid.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] vout                 utxo vout.
 * @param[out] amount               utxo amount.
 * @param[out] asset                utxo asset.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] descriptor           utxo descriptor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] scriptsig_template   utxo scriptsig template.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetFundPsbtUsedUtxo(
    void* handle, void* fund_handle, uint32_t index, uint32_t* utxo_index,
    char** txid, uint32_t* vout, int64_t* amount, char** asset,
    char** descriptor, char** scriptsig_template);

/**
 * @brief Free fund PSBT.
 * @param[in] handle                cfd handle.
 * @param[in] fund_handle           fund PSBT handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeFundPsbt(void* handle, void* fund_handle);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_PSBT_H_ NOLINT */
