// Copyright 2019 CryptoGarage
/**
 * @file cfdcapi_elements_transaction.h
 *
 * @brief cfd-capiで利用するElements用Transaction操作のAPI定義
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_TRANSACTION_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_TRANSACTION_H_
#ifndef CFD_DISABLE_ELEMENTS

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif  // 0
#endif  // __cplusplus

#include <stddef.h>
#include <stdint.h>

#include "cfdc/cfdcapi_common.h"
#include "cfdc/cfdcapi_transaction.h"

/**
 * @brief create initialized elements transaction.
 * @param[in] handle        cfd handle.
 * @param[in] version       transaction version.
 * @param[in] locktime      lock time.
 * @param[out] tx_string    transaction hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeConfidentialTx(
    void* handle, uint32_t version, uint32_t locktime, char** tx_string);

/**
 * @brief add elements transaction input.
 * @param[in] handle          cfd handle.
 * @param[in] tx_hex_string   transaction hex.
 * @param[in] txid            utxo txid.
 * @param[in] vout            utxo vout.
 * @param[in] sequence        sequence number.
 * @param[out] tx_string      transaction hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddConfidentialTxIn(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    uint32_t sequence, char** tx_string);

/**
 * @brief add elements transaction output.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] asset_string      asset hex.
 * @param[in] value_satoshi     satoshi value. (Specify 0 if disabled)
 * @param[in] value_commitment  value commitment. (Specify null if disabled)
 * @param[in] address           destination address.(Specify null if disabled)
 * @param[in] direct_locking_script  locking script for direct.
 *                                  (Specify null if disabled.)
 * @param[in] nonce             confidential nonce.(Specify null if disabled)
 * @param[out] tx_string        transaction hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddConfidentialTxOut(
    void* handle, const char* tx_hex_string, const char* asset_string,
    int64_t value_satoshi, const char* value_commitment, const char* address,
    const char* direct_locking_script, const char* nonce, char** tx_string);

/**
 * @brief update elements transaction output.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] index             txout index.
 * @param[in] asset_string      asset hex.
 * @param[in] value_satoshi     satoshi value. (Specify 0 if disabled)
 * @param[in] value_commitment  value commitment. (Specify null if disabled)
 * @param[in] address           destination address.(Specify null if disabled)
 * @param[in] direct_locking_script  locking script for direct.
 *                                  (Specify null if disabled.)
 * @param[in] nonce             confidential nonce.(Specify null if disabled)
 * @param[out] tx_string        transaction hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdUpdateConfidentialTxOut(
    void* handle, const char* tx_hex_string, uint32_t index,
    const char* asset_string, int64_t value_satoshi,
    const char* value_commitment, const char* address,
    const char* direct_locking_script, const char* nonce, char** tx_string);

/**
 * @brief get elements transaction information.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[out] txid             transaction id.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] wtxid            witness transaction id.
 *   If no-witness transaction, return is txid.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] wit_hash         witness hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] size             transaction size.
 * @param[out] vsize            virtual transaction size.
 * @param[out] weight           weight.
 * @param[out] version          transaction version.
 * @param[out] locktime         transaction locktime.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxInfo(
    void* handle, const char* tx_hex_string, char** txid, char** wtxid,
    char** wit_hash, uint32_t* size, uint32_t* vsize, uint32_t* weight,
    uint32_t* version, uint32_t* locktime);

/**
 * @brief get elements transaction input.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] index             txin index.
 * @param[out] txid             transaction id.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] vout             transaction vout
 * @param[out] sequence         sequence number.
 * @param[out] script_sig       unlocking script(script signature).
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxIn(
    void* handle, const char* tx_hex_string, uint32_t index, char** txid,
    uint32_t* vout, uint32_t* sequence, char** script_sig);

/**
 * @brief get elements transaction input witness stack.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] txin_index        txin index.
 * @param[in] stack_index       witness stack index.
 * @param[out] stack_data       witness stack data.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxInWitness(
    void* handle, const char* tx_hex_string, uint32_t txin_index,
    uint32_t stack_index, char** stack_data);

/**
 * @brief get elements transaction input pegin witness stack.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] txin_index        txin index.
 * @param[in] stack_index       witness stack index.
 * @param[out] stack_data       witness stack data.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxInPeginWitness(
    void* handle, const char* tx_hex_string, uint32_t txin_index,
    uint32_t stack_index, char** stack_data);

/**
 * @brief get elements issuance information.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] index             txin index.
 * @param[out] entropy          entropy hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] nonce            nonce hex
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] asset_amount     asset amount value
 * @param[out] asset_value      asset commitment value
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] token_amount     token amount value
 * @param[out] token_value      token commitment value
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] asset_rangeproof  asset rangeproof
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] token_rangeproof  token rangeproof
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTxInIssuanceInfo(
    void* handle, const char* tx_hex_string, uint32_t index, char** entropy,
    char** nonce, int64_t* asset_amount, char** asset_value,
    int64_t* token_amount, char** token_value, char** asset_rangeproof,
    char** token_rangeproof);

/**
 * @brief get elements transaction output.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] index             txout index.
 * @param[out] asset_string     asset hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] value_satoshi     satoshi value.
 * @param[out] value_commitment  value commitment hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] nonce             nonce hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] locking_script    locking script
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] surjection_proof  surjection proof
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] rangeproof        rangeproof
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxOut(
    void* handle, const char* tx_hex_string, uint32_t index,
    char** asset_string, int64_t* value_satoshi, char** value_commitment,
    char** nonce, char** locking_script, char** surjection_proof,
    char** rangeproof);

/**
 * @brief get elements transaction input count.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[out] count            txin count.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxInCount(
    void* handle, const char* tx_hex_string, uint32_t* count);

/**
 * @brief get elements transaction input witness stack count.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] txin_index        txin index.
 * @param[out] count            witness stack count.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxInWitnessCount(
    void* handle, const char* tx_hex_string, uint32_t txin_index,
    uint32_t* count);

/**
 * @brief get elements transaction input pegin witness stack count.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] txin_index        txin index.
 * @param[out] count            witness stack count.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxInPeginWitnessCount(
    void* handle, const char* tx_hex_string, uint32_t txin_index,
    uint32_t* count);

/**
 * @brief get elements transaction output count.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[out] count            txout count.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxOutCount(
    void* handle, const char* tx_hex_string, uint32_t* count);

/**
 * @brief get tx-input index.
 * @param[in] handle          cfd handle.
 * @param[in] tx_hex_string   tx hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[out] index          txin index.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxInIndex(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    uint32_t* index);

/**
 * @brief get tx-input index.
 * @param[in] handle                 cfd handle.
 * @param[in] tx_hex_string          tx hex.
 * @param[in] address                txout address.
 * @param[in] direct_locking_script  txout locking script. (not use address)
 * @param[out] index                 txout index.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialTxOutIndex(
    void* handle, const char* tx_hex_string, const char* address,
    const char* direct_locking_script, uint32_t* index);

/**
 * @brief set reissue transaction data.
 * @param[in] handle                cfd handle.
 * @param[in] tx_hex_string         transaction hex.
 * @param[in] txid                  target txin txid.
 * @param[in] vout                  target txin vout.
 * @param[in] asset_amount          asset amount.
 * @param[in] blinding_nonce        blinding nonce.
 * @param[in] entropy               entropy.
 * @param[in] address               asset destination address.
 *    (either address or lockingScript)
 * @param[in] direct_locking_script  asset for direct locking script.
 *    (either address or lockingScript)
 * @param[out] asset_string         asset hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] tx_string            tx hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetRawReissueAsset(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    int64_t asset_amount, const char* blinding_nonce, const char* entropy,
    const char* address, const char* direct_locking_script,
    char** asset_string, char** tx_string);

/**
 * @brief get issuance blinding key.
 * @param[in] handle                cfd handle.
 * @param[in] master_blinding_key   master blinding key hex.
 * @param[in] txid                  utxo txid.
 * @param[in] vout                  utxo vout.
 * @param[out] blinding_key         blinding key hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetIssuanceBlindingKey(
    void* handle, const char* master_blinding_key, const char* txid,
    uint32_t vout, char** blinding_key);

/**
 * @brief initialize blinding handle.
 * @param[in] handle                cfd handle.
 * @param[out] blind_handle         blinding handle.
 *   Call 'CfdFreeBlindHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeBlindTx(void* handle, void** blind_handle);

/**
 * @brief add blinding data of txin.
 * @param[in] handle              cfd handle.
 * @param[in] blind_handle        blinding handle.
 * @param[in] txid                txin txid.
 * @param[in] vout                txin vout.
 * @param[in] asset_string        asset.(utxo value)
 * @param[in] asset_blind_factor  asset blind factor.(utxo value)
 * @param[in] value_blind_vactor  value blind factor.(utxo value)
 * @param[in] value_satoshi       satoshi value.(utxo value)
 * @param[in] asset_key           asset blinding key(privkey).
 *                                (blind issuance only)
 * @param[in] token_key           token blinding key(privkey).
 *                                (blind issuance only)
 * @return CfdErrorCode
 */
CFDC_API int CfdAddBlindTxInData(
    void* handle, void* blind_handle, const char* txid, uint32_t vout,
    const char* asset_string, const char* asset_blind_factor,
    const char* value_blind_vactor, int64_t value_satoshi,
    const char* asset_key, const char* token_key);

/**
 * @brief add blinding data of txout.
 * @param[in] handle              cfd handle.
 * @param[in] blind_handle        blinding handle.
 * @param[in] index               txout index.
 * @param[in] confidential_key    blinding confidential key.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddBlindTxOutData(
    void* handle, void* blind_handle, uint32_t index,
    const char* confidential_key);

/**
 * @brief blinding transaction.
 * @param[in] handle          cfd handle.
 * @param[in] blind_handle    blinding handle.
 * @param[in] tx_hex_string   transaction hex.
 * @param[out] tx_string      blinding transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeBlindTx(
    void* handle, void* blind_handle, const char* tx_hex_string,
    char** tx_string);

/**
 * @brief free blinding handle.
 * @param[in] handle        handle pointer.
 * @param[in] blind_handle  blinding handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeBlindHandle(void* handle, void* blind_handle);

/**
 * @brief add sign to confidential transaction.
 * @param[in] handle          cfd handle.
 * @param[in] tx_hex_string   transaction hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[in] is_witness      insert to witness stack.
 * @param[in] sign_data_hex   add sign data.
 * @param[in] clear_stack     cleanup scriptSig stack data.
 * @param[out] tx_string      signed transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddConfidentialTxSign(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    bool is_witness, const char* sign_data_hex, bool clear_stack,
    char** tx_string);

/**
 * @brief sign convert to der, and add sign to confidential transaction.
 * @param[in] handle          cfd handle.
 * @param[in] tx_hex_string   transaction hex.
 * @param[in] txid            txin txid.
 * @param[in] vout            txin vout.
 * @param[in] is_witness      insert to witness stack.
 * @param[in] signature       add sign data signature.
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pay flag.
 * @param[in] clear_stack     cleanup scriptSig stack data.
 * @param[out] tx_string      signed transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddConfidentialTxDerSign(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    bool is_witness, const char* signature, int sighash_type,
    bool sighash_anyone_can_pay, bool clear_stack, char** tx_string);

/**
 * @brief append multisig sign to transaction.
 * @param[in] handle            cfd handle.
 * @param[in] multisign_handle  multisig sign handle.
 * @param[in] tx_hex_string     tx hex.
 * @param[in] txid              txin txid.
 * @param[in] vout              txin vout.
 * @param[in] hash_type         hash type.
 * @param[in] witness_script    witness script for segwit.
 * @param[in] redeem_script     redeem script for p2sh.
 * @param[in] clear_stack       cleanup scriptSig stack data.
 * @param[out] tx_string        signed tx hex.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeElementsMultisigSign(
    void* handle, void* multisign_handle, const char* tx_hex_string,
    const char* txid, uint32_t vout, int hash_type, const char* witness_script,
    const char* redeem_script, bool clear_stack, char** tx_string);

/**
 * @brief add keyhash sign with privkey.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     transaction hex.
 * @param[in] txid              txin txid.
 * @param[in] vout              txin vout.
 * @param[in] hash_type         hash type.(p2pkh, p2wpkh, p2sh-p2wpkh)
 * @param[in] pubkey            pubkey hex.
 * @param[in] privkey           privkey (WIF or hex).
 * @param[in] value_satoshi     satoshi value. (Specify 0 if disabled)
 * @param[in] value_commitment  value commitment. (Specify null if disabled)
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pay flag.
 * @param[in] has_grind_r     ec-signature Grind-R flag.
 * @param[out] tx_string      signed transaction hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddConfidentialTxSignWithPrivkeySimple(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    int hash_type, const char* pubkey, const char* privkey,
    int64_t value_satoshi, const char* value_commitment, int sighash_type,
    bool sighash_anyone_can_pay, bool has_grind_r, char** tx_string);

/**
 * @brief create confidential transaction sighash.
 * @param[in] handle            cfd handle.
 * @param[in] tx_hex_string     tx hex.
 * @param[in] txid              txin txid.
 * @param[in] vout              txin vout.
 * @param[in] hash_type         hash type.
 * @param[in] pubkey            pubkey. (Specify null if disabled)
 * @param[in] redeem_script     redeem script. (Specify null if disabled)
 * @param[in] value_satoshi     satoshi value. (Specify 0 if disabled)
 * @param[in] value_commitment  value commitment. (Specify null if disabled)
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pay flag.
 * @param[out] sighash          sighash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdCreateConfidentialSighash(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    int hash_type, const char* pubkey, const char* redeem_script,
    int64_t value_satoshi, const char* value_commitment, int sighash_type,
    bool sighash_anyone_can_pay, char** sighash);

/**
 * @brief get unblind txout.
 * @param[in] handle                cfd handle.
 * @param[in] tx_hex_string         tx hex.
 * @param[in] tx_out_index          target txout index.
 * @param[in] blinding_key          blinding key.
 * @param[out] asset                asset
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] value                satoshi value.
 * @param[out] asset_blind_factor   asset blind factor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] value_blind_factor   value blind factor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdUnblindTxOut(
    void* handle, const char* tx_hex_string, uint32_t tx_out_index,
    const char* blinding_key, char** asset, int64_t* value,
    char** asset_blind_factor, char** value_blind_factor);

/**
 * @brief get unblind issuance.
 * @param[in] handle                     cfd handle.
 * @param[in] tx_hex_string              tx hex.
 * @param[in] tx_in_index                target txin index.
 * @param[in] asset_blinding_key         asset blinding key.
 * @param[in] token_blinding_key         token blinding key.
 * @param[out] asset                     asset
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] asset_value               asset satoshi value.
 * @param[out] asset_blind_factor        asset blind factor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] asset_value_blind_factor  asset value blind factor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] token                     token asset
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] token_value               token satoshi value.
 * @param[out] token_blind_factor        token asset blind factor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] token_value_blind_factor  token value blind factor.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdUnblindIssuance(
    void* handle, const char* tx_hex_string, uint32_t tx_in_index,
    const char* asset_blinding_key, const char* token_blinding_key,
    char** asset, int64_t* asset_value, char** asset_blind_factor,
    char** asset_value_blind_factor, char** token, int64_t* token_value,
    char** token_blind_factor, char** token_value_blind_factor);

/**
 * @brief verify signature data.
 * @param[in] handle                  cfd handle.
 * @param[in] tx_hex                  transaction hex.
 * @param[in] signature               signature hex.
 * @param[in] pubkey                  pubkey hex.
 * @param[in] txid                    txid.
 * @param[in] vout                    vout.
 * @param[in] script                  script hex. (Specify NULL if disabled)
 * @param[in] sighash_type            sighash type. (ref: CfdSighashType)
 * @param[in] sighash_anyone_can_pay  sighash anyone can pay flag.
 * @param[in] value_satoshi           value satoshi. (Specify 0 if disabled)
 * @param[in] value_commitment        value commitment.
 *     (Specify null if disabled)
 * @param[in] witness_version         witness version.
 *     (ref: CfdWitnessVersion. if not require, set kCfdWitnessVersionNone)
 * @return CfdErrorCode
 *     (if failed to verify signature, it returns kCfdSignVerificationError)
 */
CFDC_API int CfdVerifyConfidentialTxSignature(
    void* handle, const char* tx_hex, const char* signature,
    const char* pubkey, const char* script, const char* txid, uint32_t vout,
    int sighash_type, bool sighash_anyone_can_pay, int64_t value_satoshi,
    const char* value_commitment, int witness_version);

/**
 * @brief verify transaction's scriptsig or witness stack.
 * @details support type is p2pkh, p2wpkh, p2sh-p2wpkh,
 *     multisig(for p2sh, p2wsh, p2sh-p2wsh)
 * @param[in] handle                  cfd handle.
 * @param[in] tx_hex                  transaction hex.
 * @param[in] txid                    txid.
 * @param[in] vout                    vout.
 * @param[in] address                 utxo address.
 * @param[in] address_type            address type. (ref: CfdAddressType)
 * @param[in] direct_locking_script   utxo locking script.
 *     (set when address is empty.)
 * @param[in] value_satoshi           value satoshi. (Specify 0 if disabled)
 * @param[in] value_commitment        value commitment.
 *     (Specify null if disabled)
 * @return CfdErrorCode
 *     (if failed to verify signature, it returns kCfdSignVerificationError)
 */
CFDC_API int CfdVerifyConfidentialTxSign(
    void* handle, const char* tx_hex, const char* txid, uint32_t vout,
    const char* address, int address_type, const char* direct_locking_script,
    int64_t value_satoshi, const char* value_commitment);

/**
 * @brief get elements value hex.
 * @param[in] handle                cfd handle.
 * @param[in] value_satoshi         value of satoshi.
 * @param[in] ignore_version_info   ignore version header. (delete top 1byte)
 * @param[out] value_hex            value hex. (8-9 byte (16-18 char))
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetConfidentialValueHex(
    void* handle, int64_t value_satoshi, bool ignore_version_info,
    char** value_hex);

/* 後回し
CFDC_API int CfdAddElementsWitnessStack(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    const char* hex_data, char** tx_string);
CFDC_API int CfdUpdateConfidentialTxIn(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    const char* script_sig, uint32_t sequence, char** tx_string);
CFDC_API int CfdAddPeginWitnessStack(
    void* handle, const char* tx_hex_string, const char* txid, uint32_t vout,
    const char* hex_data, char** tx_string);

*/

#if 0
/*
        * SetRawIssueAsset
          - SetIssueAsset
            - Asset, Token, EntropyをOUTで。
            - TxOutIndexも。
        * get系
*/
#endif

#ifdef __cplusplus
#if 0
{
#endif  // 0
}
#endif  // __cplusplus

#endif  // CFD_DISABLE_ELEMENTS
#endif  // CFD_INCLUDE_CFDC_CFDCAPI_ELEMENTS_TRANSACTION_H_
