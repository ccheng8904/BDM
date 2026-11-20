/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Boot-Manager
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  BootManager
 *
 *  \brief    Implementation of Boot-Manager-Main
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "firmware_update/mal/moduleAdministration.h"
#include "firmware_update/mal/mal_driver.h"
#include "BootManagerModule.h"
#include "IntTbl.h"
#include "stdcrc/StdCrc.h"
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
#include "FuncSafe/fSafeXS.h"
#endif
#include "FuncSafe/fs_psm.h"
#include "firmware_update/mal/fwu_opt_bytes.h"
#include "firmware_update/otp_mem/fwu_otp_memory.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
#ifdef VARIANT_BOOTMANAGER
static void BMMOD_LoadModule (void);
static bool BMMOD_isStdvIdEqual(const uint8_t* id1, const uint8_t* id2);
static bool BMMOD_isFirmwareIdConsistent(void);
static bool BMMOD_isFirmwareIdCompatible(void);
#endif


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#ifdef VARIANT_BOOTMANAGER
static const struct MAL_partition_s * BMMOD_partition;
static const struct MAL_ModuleHeader_s * BMMOD_header;
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
const struct MAL_ModuleHeader_s * BMMOD_GetMemModuleHeader(enum MAL_ModuleEnum module)
{
    MAL_initPartition();
    return MAL_PartitionToHeader(MAL_GetMemModule(module));
}


#ifdef VARIANT_BOOTMANAGER
bool BMMOD_bInitMemoryModule(enum MAL_ModuleEnum ModuleId)
{
    FSF_DoProgramCounterMonitoring(FSF_BM_SHARED_DATA_INITIALIZED, FSF_BM_MEMORY_MODULE_INITIALIZED);
    MAL_initPartition();
    BMMOD_partition = MAL_GetMemModule(ModuleId);
    BMMOD_header = MAL_PartitionToHeader(BMMOD_partition);
    return (BMMOD_partition == NULL) ? false : true;
}


void BMMOD_startModule(void)
{
    FSF_DoProgramCounterMonitoring(FSF_BM_IS_MODULE_FW_ID_VALID, FSF_BM_START_MODULE);

    /* reload correct module to be started */
    BMMOD_header = MAL_PartitionToHeader(BMMOD_partition);

    BMMOD_LoadModule();

    ITBL_StartApplication( (size_t)(uint32_t)BMMOD_header->ISRtbl_ptr); /*lint !e923 !e507 cast tested to work correctly */
}


bool BMMOD_isModuleStartable(void)
{
    FSF_DoProgramCounterMonitoring(FSF_BM_MEMORY_MODULE_INITIALIZED, FSF_BM_IS_MODULE_STARTABLE);

    // check module flags
    if( BMMOD_partition->mod_attributes & MAL_CTX_MF_EXECUTABLE )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool BMMOD_isModuleFirmwareIdValid(void)
{
    FSF_DoProgramCounterMonitoring(FSF_BM_IS_MODULE_INTEGRITY_OK, FSF_BM_IS_MODULE_FW_ID_VALID);

    if (MAL_isBuildInApplication(BMMOD_partition))
    {
        /* Only FW IDs of update-able modules need to be checked */
        return true;
    }
    else
    {
        return ((BMMOD_isFirmwareIdConsistent() && BMMOD_isFirmwareIdCompatible()) ? true : false);
    }
}


bool BMMOD_isModuleIntegrityOk(void)
{
    FSF_DoProgramCounterMonitoring(FSF_BM_IS_MODULE_STARTABLE, FSF_BM_IS_MODULE_INTEGRITY_OK);

    if(FWU_isIntegCheckDisabledCalc())
    {
        // disabled calculation
        return true;
    }
    else
    {
        return MAL_isModuleIntegrityOk(BMMOD_partition);
    }
}

#endif
/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
#ifdef VARIANT_BOOTMANAGER

/* Check OTP FW ID table CRC */
static bool BMMOD_isFirmwareIdConsistent(void)
{
    const struct MAL_fw_id_array  * FwIdArrayToCheck = OTP_getFirmwareIDs();

    /* No need to check CRC if FW ID table is empty */
    if (FwIdArrayToCheck == NULL){return true;}

    CRC_crc_t FwCrcExpected = FwIdArrayToCheck->CRC;
    // get length of checked area for CRC
    uint32_t CrcLength = (sizeof(struct MAL_fw_id_entry) * FWU_FW_ID_MAX_COUNT);
    /*lint -save -e928 cast from pointer to pointer [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]: tested, it works correctly */
    CRC_crc_t CrcCalc = CRC_Calc(CrcLength, (const uint8_t*)FwIdArrayToCheck->entries);
    /*lint -restore -e928*/
    return ((CrcCalc == FwCrcExpected) ? true : false);
}


/** check if FW IDs of all application partitions are supported */
static bool BMMOD_isFirmwareIdCompatible(void)
{
    const struct MAL_fw_id_array  * FwIdArrayToCheck = OTP_getFirmwareIDs();
    const struct MAL_fw_id_entry  * FwIdEntryToCheck = FwIdArrayToCheck->entries;
    const struct MAL_partition_s  * partition;
    const struct MAL_ModuleHeader_s  * moduleHeader;

    /* OTP_getFirmwareIDs returns NULL when FW ID table is empty*/
    if (FwIdArrayToCheck != NULL)
    {
        for (uint8_t entryIndex = 0u; entryIndex < FWU_FW_ID_MAX_COUNT; entryIndex++)
        {
            /* Check only those non-empty entries */
            if (FwIdEntryToCheck[entryIndex].mod_number != MAL_NO_MODULE)
            {
                /* Find corresponding partition based on moduleId of the entry */
                partition = MAL_GetMemModule(FwIdEntryToCheck[entryIndex].mod_number);
                moduleHeader = MAL_PartitionToHeader(partition);

                /* moduleHeader = NULL if mod_number is unknown, check only those entries with valid mod_number*/
                if (moduleHeader != NULL)
                {
                    if (!BMMOD_isStdvIdEqual(FwIdEntryToCheck[entryIndex].firmware_id.id, moduleHeader->sw_version.version.ID))
                    {
                        /* Compatibility check fails if FW ID from module header is inconsistent with that in OTP memory */
                        return false;
                    }
                }
            }
        }
    }

    return true;
}


/** compares memory
 *  \param mem1 memory block one
 *  \param mem2 memory block two
 *  \return true if memory is equal otherwise false
 */
static bool BMMOD_isStdvIdEqual(const uint8_t* id1, const uint8_t* id2)
{
    for(uint8_t i=0u; i<STDV_ID_LENGTH; i++)
    {
        if(id1[i] != id2[i]){return false;}
    }
    return true;
}


/** \brief    loads the selected module to its start address if required.
 *
 *  \note     Expands the application to the specified load address.
 *            If application need to be executed directly at their physical
 *            address, the function just returns.
 *
 */
static void BMMOD_LoadModule(void)
{


    if (BMMOD_partition->physical_start.addr != BMMOD_header->executionAddress)
    {
        /* load the module to RAM */
        uint8_t *exePtr = (uint8_t*)BMMOD_header->executionAddress; /*lint !e923 cast tested to work correctly */

        for(const uint8_t *ptr  =  BMMOD_partition->physical_start.ptr; /*lint !e926 cast tested to work correctly */
            ptr <= (const uint8_t*)BMMOD_partition->physical_end.ptr; /*lint !e946 !e926 relational tested to work correctly */
            ptr++)
        {
            *exePtr++ = *ptr;/*lint !e9049 misra adv. inc. comined with other operation*/
        }
    }
}

#endif
