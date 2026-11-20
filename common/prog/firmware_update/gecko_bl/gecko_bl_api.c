/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  gecko_bootloader
 *
 *  \brief    Implementation BSH application interface to gecko boot loader.
 *
 *  \details  This Interface is implemented to unify the access to the SiLabs
 *            gecko boot loader.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "std_lib/std_symbol.h"
#include "SegmentDef.h"

/* Note 9018: union '(anonymous)' declared [MISRA 2012 Rule 19.2, advisory]
 * Note 970: use of modifier or type 'int' outside of a typedef [MISRA 2012 Directive 4.6, required]
 * Note 923: explicit cast from 'unsigned long' to 'SEMAILBOX_HOST_TypeDef *' [MISRA 2012 Rule 11.1, required], [MISRA 2012 Rule 11.4, required], [MISRA 2012 Rule 11.6, required]
 */
/*lint -save -e9018 -e970 -e923 */
#include "gecko_bl_api.h"
#include "gecko_bl/api/btl_reset_info.h"
#include "gecko_bl/api/btl_interface.h"
#include "gecko_bl/api/btl_interface_storage.h"
#include "gecko_bl/mbedtls/sl_crypto/sl_se_manager_util.h"
/*lint -restore -e9018 -e970 -e923 */

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/*lint -esym(551,GBTL_sharedData) GBTL_sharedData not accessed */

/** \brief    data which is shared between gecko boot loader and application
 *
 *  \details  The following data is placed in a special segment and is used
 *            to share data between the gecko boot loader and application.
 */
SDEF_SetSegmentRW(FWU_SHARED_DATA) // put it into segment FWU_SHARED_DATA
SYMBOL_USED static BootloaderResetCause_t GBTL_sharedData;
SDEF_SetSegmentRW_Default()

const uint32_t GBTL_keyLen = 64u; ///< key length in bytes
const uint32_t GBTL_vctSize = 4u; ///< size of interrupt vector in byte
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

static const ApplicationProperties_t* GBTL_getApplicationProperties(uint32_t vtbl_start);
static uint32_t GBTL_getCertificateVersion(uint32_t vtbl_start);
static bool GBTL_getPublicKey(uint8_t *key, uint32_t vtbl_start);


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void GBTL_bootGromLoaderAfterReset(void)
{
    GBTL_sharedData.reason    = BOOTLOADER_RESET_REASON_GROM_LOADER;
    GBTL_sharedData.signature = BOOTLOADER_RESET_SIGNATURE_VALID;
}

void GBTL_installAfterReset(void)
{
    GBTL_sharedData.reason    = BOOTLOADER_RESET_REASON_BOOTLOAD;
    GBTL_sharedData.signature = BOOTLOADER_RESET_SIGNATURE_VALID;
}

void GBTL_rebootAndInstall(void)
{
    const int32_t slotID = 0; // just a single slot supported
    (void)bootloader_setImageToBootload(slotID);
    bootloader_rebootAndInstall();
}


bool GBTL_getPublicSignKey(uint8_t *key)
{   /* This ugly implementation is much more complicated as required, as silabs expects a word aligned
       pointer to key parameter!!
     */
    if(key != NULL)
    {   
        const bool deliverSignature = false;
        /* Note 9035: variable length array 'allignedKey' of type 'uint32_t [GBTL_keyLen / sizeof(uint32_t)]' declared [MISRA 2012 Rule 18.8, required] */
        /*lint -e{9035} : false positive, array length is not variable */
        uint32_t allignedKey[GBTL_keyLen/sizeof(uint32_t)]; // 64 bytes word aligned
        /* Taking address of near auto variable 'allignedKey' (arg. no. 2) [MISRA 2012 Rule 1.3, required]
           --> Note 934 is false positive! */
        if (SE_readPubkey(SE_KEY_TYPE_BOOT, (void*)allignedKey, GBTL_keyLen, deliverSignature) == /*lint !e934 */
            SE_RESPONSE_OK)
        {
            for(uint32_t i=0u; i<GBTL_keyLen; i++)
            {   /* cast from pointer to pointer [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]
                   cast uncritical as casted to less stricter alignment */
                key[i] = ((uint8_t*)allignedKey)[i];/*lint !e928 */
            }
            return true;
        }
    }
    return false;
}


bool GBTL_getPublicBootKey(uint8_t *key)
{
    return GBTL_getPublicKey(key, ROOT_REGION);
}

bool GBTL_getPublicAppKey(uint8_t *key)
{
    return GBTL_getPublicKey(key, FWU_APP1_START_ADDRESS);
}


uint32_t GBTL_getBootCertificateVersion(void)
{
    return GBTL_getCertificateVersion(ROOT_REGION);
}


uint32_t GBTL_getAppCertificateVersion(void)
{
    return GBTL_getCertificateVersion(FWU_APP1_START_ADDRESS);
}


const struct STDV_version * GBTL_getBootloaderVersion(void)
{
    const uint32_t version_vector = 7u;

    return *((struct STDV_version**)(version_vector*GBTL_vctSize));//lint !e923 cast tested to work correctly
}



uint32_t GBTL_getRemainingUpdates(void)
{
    const uint32_t mainstage_vector = 10u;
    const MainBootloaderTable_t *mainStageTblablePtr =
    *((MainBootloaderTable_t**)(mainstage_vector*GBTL_vctSize));//lint !e923 cast tested to work correctly
    if(mainStageTblablePtr->remainingApplicationUpgrades == NULL)
    {
        return 0u; // feature not provide
    }
    return mainStageTblablePtr->remainingApplicationUpgrades();

}


bool GBTL_getSE_FirmwareVersion(uint32_t *version)
{
    sl_se_command_context_t cmd_ctx;
    /* Taking address of near auto variable [MISRA 2012 Rule 1.3, required]
       --> Note 934 is false positive! */
    return (sl_se_get_se_version(&cmd_ctx, version) == SE_RESPONSE_OK); /*lint !e934 */
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief    delivers the pointer to application properties
 *
 *  \param    vtbl_start start address of the according vector table (0x00 in case of boot loader)
 *  \return   pointer to application properties
 */
static const ApplicationProperties_t* GBTL_getApplicationProperties(uint32_t vtbl_start)
{
    const uint32_t app_prop_vector = 13u;
    const uint32_t app_prop_addr   = vtbl_start + (uint32_t)(app_prop_vector * GBTL_vctSize);
    return *((ApplicationProperties_t**)(app_prop_addr));//lint !e923 cast tested to work correctly
}


/** \brief    delivers the version of used certificate
 *
 *  \param    vtbl_start start address of the according vector table (0x00 in case of boot loader)
 *  \return   version number of certificate or GBTL_INVALID_CERTIFICATE
 */
static uint32_t GBTL_getCertificateVersion(uint32_t vtbl_start)
{
    const ApplicationProperties_t *app_properties = GBTL_getApplicationProperties(vtbl_start);
    
    if((app_properties ==NULL) || ((app_properties)->cert == NULL))
    {
        return GBTL_INVALID_CERTIFICATE;
    }
    return (app_properties)->cert->version;
}


/** \brief    delivers the public key inside a certificate
 *
 *  \param    vtbl_start start address of the according vector table (0x00 in case of boot loader)
 *  \return   true if key read out successfully, otherwise false
 */
static bool GBTL_getPublicKey(uint8_t *key, uint32_t vtbl_start)
{
    const ApplicationProperties_t *app_properties = GBTL_getApplicationProperties(vtbl_start);
    
    if((app_properties == NULL) || ((app_properties)->cert == NULL) || (key == NULL))
    {
        return false;
    }
    for(uint32_t i=0u; i<GBTL_keyLen; i++)
    {
        key[i] = (app_properties)->cert->key[i];
    }
    return true;
}
