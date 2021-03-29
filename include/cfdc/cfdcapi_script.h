/* Copyright 2019 CryptoGarage */
/**
 * @file cfdcapi_script.h
 *
 * @brief API definition file of Script function for used in cfd-capi
 */
#ifndef CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_
#define CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif /* __cplusplus */

#include "cfdc/cfdcapi_common.h"

/**
 * @brief parse script and return script item handle.
 * @param[in] handle              cfd handle.
 * @param[in] script              script string.
 * @param[out] script_item_handle script item handle.
 * @param[out] script_item_num    num of items contains script.
 * @return CfdErrorCode
 */
CFDC_API int CfdParseScript(
    void* handle, const char* script, void** script_item_handle,
    uint32_t* script_item_num);

/**
 * @brief get script item from script item handle.
 * @param[in] handle              cfd handle.
 * @param[in] script_item_handle  script item handle.
 * @param[in] index               index of script item.
 * @param[out] script_item        string of script item.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetScriptItem(
    void* handle, void* script_item_handle, uint32_t index,
    char** script_item);

/**
 * @brief free script item handle.
 * @param[in] handle             handle pointer.
 * @param[in] script_item_handle handle of getting script item.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeScriptItemHandle(void* handle, void* script_item_handle);

/**
 * @brief Convert script asm format to hex data.
 * @param[in] handle        cfd handle.
 * @param[in] script_asm    assembly format script string.
 * @param[out] script_hex    converted hex string.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdConvertScriptAsmToHex(
    void* handle, const char* script_asm, char** script_hex);

/**
 * @brief initialized for multisig scriptsig.
 * @param[in] handle          cfd handle.
 * @param[out] multisig_handle  multisig scriptsig handle.
 *   Call 'CfdFreeMultisigScriptSigHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeMultisigScriptSig(
    void* handle, void** multisig_handle);

/**
 * @brief append for multisig scriptsig data.
 * @param[in] handle            cfd handle.
 * @param[in] multisig_handle   multisig scriptsig handle.
 * @param[in] signature         signature hex.
 * @param[in] related_pubkey    pubkey hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddMultisigScriptSigData(
    void* handle, void* multisig_handle, const char* signature,
    const char* related_pubkey);

/**
 * @brief append for multisig scriptsig data.
 * @param[in] handle                  cfd handle.
 * @param[in] multisig_handle         multisig scriptsig handle.
 * @param[in] signature               signature hex.
 * @param[in] sighash_type            sighash type.
 * @param[in] sighash_anyone_can_pay  sighash anyone can pey flag.
 * @param[in] related_pubkey          pubkey hex.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddMultisigScriptSigDataToDer(
    void* handle, void* multisig_handle, const char* signature,
    int sighash_type, bool sighash_anyone_can_pay, const char* related_pubkey);

/**
 * @brief create multisig scriptsig.
 * @param[in] handle           handle pointer.
 * @param[in] multisig_handle  multisig scriptsig handle.
 * @param[in] redeem_script    redeemScript for multisig.
 * @param[out] scriptsig       generate scriptsig.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdFinalizeMultisigScriptSig(
    void* handle, void* multisig_handle, const char* redeem_script,
    char** scriptsig);

/**
 * @brief free multisig scriptsig handle.
 * @param[in] handle            handle pointer.
 * @param[in] multisig_handle   multisig scriptsig handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeMultisigScriptSigHandle(
    void* handle, void* multisig_handle);

/*---------*/
/* taproot */
/*---------*/
/**
 * @brief initialized for taproot script tree.
 * 
 * Next call is CfdSetInitialTapLeaf() or
 * CfdSetTapScriptByWitnessStack().
 * @param[in] handle            cfd handle.
 * @param[out] tree_handle      taproot script tree handle.
 *   Call 'CfdFreeTaprootScriptTreeHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdInitializeTaprootScriptTree(void* handle, void** tree_handle);

/**
 * @brief Set a initial tapleaf.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] tapscript         taproot script.
 * @param[in] leaf_version      taproot leaf version.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetInitialTapLeaf(
    void* handle, void* tree_handle, const char* tapscript,
    uint8_t leaf_version);

/**
 * @brief Set a initial tapbranch by hash.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] hash              taproot script.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetInitialTapBranchByHash(
    void* handle, void* tree_handle, const char* hash);

/**
 * @brief Set a script tree from string.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] tree_string       tree string.
 * @param[in] tapscript         taproot script.
 * @param[in] leaf_version      taproot leaf version.
 * @param[in] control_nodes     control node list.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetScriptTreeFromString(
    void* handle, void* tree_handle, const char* tree_string,
    const char* tapscript, uint8_t leaf_version, const char* control_nodes);

/**
 * @brief Set a tapscript tree from witness stack.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] control_block     control data into witness stack.
 * @param[in] tapscript         tapscript into witness stack.
 * @param[in] internal_pubkey   internal schnorr public key from control stack.
 * @return CfdErrorCode
 */
CFDC_API int CfdSetTapScriptByWitnessStack(
    void* handle, void* tree_handle, const char* control_block,
    const char* tapscript, char** internal_pubkey);

/**
 * @brief append for taproot script tree branch hash.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] branch_hash       branch hash.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTapBranchByHash(
    void* handle, void* tree_handle, const char* branch_hash);

/**
 * @brief append for taproot script tree.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] branch_tree       tree handle by branch.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTapBranchByScriptTree(
    void* handle, void* tree_handle, void* branch_tree);

/**
 * @brief append for taproot script tree.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] tree_string       tree string.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTapBranchByScriptTreeString(
    void* handle, void* tree_handle, const char* tree_string);

/**
 * @brief append for tapleaf.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] tapscript         taproot script.
 * @param[in] leaf_version      taproot leaf version.
 * @return CfdErrorCode
 */
CFDC_API int CfdAddTapBranchByTapLeaf(
    void* handle, void* tree_handle, const char* tapscript,
    uint8_t leaf_version);

/**
 * @brief Get base tapleaf data.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[out] leaf_version     taproot leaf version. Set to 0 if not tapleaf.
 * @param[out] tapscript        taproot script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] tap_leaf_hash    tapleaf hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetBaseTapLeaf(
    void* handle, void* tree_handle, uint8_t* leaf_version, char** tapscript,
    char** tap_leaf_hash);

/**
 * @brief Get tapbranch count.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[out] branch_count     tapbranch count.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTapBranchCount(
    void* handle, void* tree_handle, uint32_t* branch_count);

/**
 * @brief Get tapbranch data.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] index_from_leaf   index from base tapleaf.
 * @param[in] is_root_data      true is getting combined hash.
 * @param[out] branch_hash      tapbranch hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] leaf_version     taproot leaf version. Set to 0 if not tapleaf.
 * @param[out] tapscript        taproot script.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] depth_by_leaf_or_end     target branch depth.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTapBranchData(
    void* handle, void* tree_handle, uint8_t index_from_leaf,
    bool is_root_data, char** branch_hash, uint8_t* leaf_version,
    char** tapscript, uint8_t* depth_by_leaf_or_end);

/**
 * @brief Get tapbranch handle.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] index_from_leaf   index from base tapleaf.
 * @param[out] branch_hash      tapbranch hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] branch_tree_handle   tapbranch handle.
 *   Call 'CfdFreeTaprootScriptTreeHandle' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTapBranchHandle(
    void* handle, void* tree_handle, uint8_t index_from_leaf,
    char** branch_hash, void** branch_tree_handle);

/**
 * @brief Get tapscript hash.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] internal_pubkey   internal schnorr public key.
 * @param[out] hash             tapscript hash. (witness program)
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] tap_leaf_hash    tapleaf hash.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @param[out] control_block    taproot control data. (for witness stack)
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTaprootScriptTreeHash(
    void* handle, void* tree_handle, const char* internal_pubkey, char** hash,
    char** tap_leaf_hash, char** control_block);

/**
 * @brief Get tapscript tweaked privkey.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[in] internal_privkey  internal private key.
 * @param[out] tweaked_privkey  tweaked privkey.
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTaprootTweakedPrivkey(
    void* handle, void* tree_handle, const char* internal_privkey,
    char** tweaked_privkey);

/**
 * @brief Get tapscript hash.
 * @param[in] handle            cfd handle.
 * @param[in] tree_handle       taproot script tree handle.
 * @param[out] tree_string      taproot string. (proposal)
 *   If 'CfdFreeStringBuffer' is implemented,
 *   Call 'CfdFreeStringBuffer' after you are finished using it.
 * @return CfdErrorCode
 */
CFDC_API int CfdGetTaprootScriptTreeSrting(
    void* handle, void* tree_handle, char** tree_string);

/**
 * @brief free taproot script tree handle.
 * @param[in] handle            handle pointer.
 * @param[in] tree_handle       taproot script tree handle.
 * @return CfdErrorCode
 */
CFDC_API int CfdFreeTaprootScriptTreeHandle(void* handle, void* tree_handle);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif /* __cplusplus */

#endif /* CFD_INCLUDE_CFDC_CFDCAPI_SCRIPT_H_ NOLINT */
