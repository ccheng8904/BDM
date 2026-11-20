/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Memory Abstraction Layer
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation of unit moduleAdministration.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "moduleAdministration.h"
#include "stdcrc/StdCrc.h"
#include "utility/utility.h"
#include "mal_driver.h"
#include "firmware_update/BootManager/BootManagerSharedData.h"
#include "fwu_opt_bytes.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include "otp_mem/fwu_otp_memory.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static const struct MAL_partition_s * MAL_searchMemModule
(
    enum MAL_ModuleEnum          module,
    const struct MAL_partition_s *partitionTbl,
    uint8_t                      maxTableLen
);


static void MAL_createModuleDictionary(void);
static void MAL_copyPatToDictionary(const struct MAL_partition_s * pTbl, uint8_t max_size);
static bool MAL_copyModuleHeaderToRAM(const struct MAL_partition_s * partition,
                                      struct MAL_ModuleHeader_s * moduleHeaderInRam);
static bool MAL_isInDictionary(enum  MAL_ModuleEnum mod_number);
#if defined(VARIANT_BOOTMANAGER)
    static uint32_t MAL_getModuleSize(const struct MAL_partition_s * partition);
    static bool MAL_isModuleCrcValid(const struct MAL_partition_s * partition);
    static bool MAL_isConsistencyOk(const struct MAL_partition_s * partition);
    static bool MAL_isApplicationIntegrityOk(void);
    static bool MAL_isIntegrityOk(const struct MAL_partition_s * partition);
    static bool MAL_isExludedFromIntegrityCheck(const struct MAL_partition_s * partition);
    static uint32_t MAL_getBuildId(const struct MAL_partition_s * partition);
    static bool MAL_handleIntegrityError(const struct MAL_partition_s * partition,
                                         enum BMDAT_errorFlags err_flag);
    static uint32_t MAL_convertArrayToU32(const uint8_t* intArray);
#endif
#ifdef FWU_FLEX_PARTITION_USED
    static const struct MAL_partition_s * MAL_GetFlexPartitionTable(void);
#endif //FWU_FLEX_PARTITION_USED
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** dictionary saving all available memory modules */
#ifdef FWU_FLEX_PARTITION_USED
static enum    MAL_ModuleEnum  MAL_moduleDictionary[MAL_FLEX_PARTITION_ENTRIES + MAL_FIX_PARTITION_ENTRIES];
#else // only FIX partition used
static enum    MAL_ModuleEnum  MAL_moduleDictionary[MAL_FIX_PARTITION_ENTRIES];
#endif //FWU_FLEX_PARTITION_USED
static uint8_t MAL_ModuleCount; ///< number of available modules


#ifdef FWU_FLEX_PARTITION_USED
    static const struct  MAL_partition_s * MAL_flexPatRef;
#endif //FWU_FLEX_PARTITION_USED

#if defined(VARIANT_BOOTMANAGER)
/*lint -esym(9003,MAL_maxFragments) could define variable at block scope
 *                 [MISRA 2012 Rule 8.9, advisory]
 */
    #ifdef FWU_FLEX_PARTITION_USED
        static const uint8_t MAL_maxFragments = MAL_FLEX_PARTITION_ENTRIES;
    #else
        static const uint8_t MAL_maxFragments = MAL_FIX_PARTITION_ENTRIES;
    #endif
#endif
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void MAL_initPartition(void)
{
    MAL_ModuleCount = 0u;
  #ifdef FWU_FLEX_PARTITION_USED
    MAL_flexPatRef = MAL_GetFlexPartitionTable();
  #endif
}


const struct MAL_partition_s * MAL_GetMemModule(enum MAL_ModuleEnum module)
{
    const struct MAL_partition_s *partition;
  #ifdef FWU_FLEX_PARTITION_USED
    partition = NULL;
    if (module != MAL_FLEX_PARTITION) // no flex table entry in flex table allowed!
    {   /* try flex partition */
        partition = MAL_searchMemModule( module, MAL_flexPatRef,
                                        (uint8_t)MAL_FLEX_PARTITION_ENTRIES );
    }
    if ( partition == NULL )
  #endif
    {   /* no result, so try fixed table */
        partition = MAL_searchMemModule( module, MAL_DataAnchor.partitionTblFix,
                                        (uint8_t)MAL_FIX_PARTITION_ENTRIES );
    }
    return partition;
}



uint8_t MAL_GetMemModuleCount(void)
{
    MAL_createModuleDictionary();
    return MAL_ModuleCount;
}


const struct MAL_partition_s * MAL_GetMemModuleByIndex(uint8_t index)
{
    MAL_createModuleDictionary();

    if (index < MAL_ModuleCount)
    {
        return MAL_GetMemModule(MAL_moduleDictionary[index]);
    }
    else
    {   /* index out of range */
        return NULL;
    }
}


uint32_t MAL_getPartitionSize(const struct MAL_partition_s * partition)
{
    uint32_t partitionSize = 0u;
    if(partition != NULL)
    {
        partitionSize = (uint32_t)((partition->physical_end.addr - partition->physical_start.addr) + 1u);
    }
    return partitionSize;
}


const struct MAL_ModuleHeader_s * MAL_PartitionToHeader( const struct MAL_partition_s *partition )
{
    static struct MAL_ModuleHeader_s moduleHeaderInRam;
    static const struct MAL_partition_s *actualPartition = NULL;
    if(partition == NULL)
    {
        return NULL;
    }
    if(actualPartition != partition)
    {
        if(MAL_copyModuleHeaderToRAM(partition, &moduleHeaderInRam) != true)
        {
            return NULL;
        }
        actualPartition = partition;
    }
    return &moduleHeaderInRam;
}


bool MAL_isBuildInApplication(const struct MAL_partition_s * partition)
{
    if(partition != NULL)
    {
        enum MAL_ModuleEnum moduleId = partition->mod_number;
        return ((moduleId == MAL_PROGRAMMER_ID) || (moduleId == MAL_BP2_LOADER_ID));
    }
    else
    {   // no valid partition given
        return false;
    }

}


#if defined(VARIANT_BOOTMANAGER)
bool MAL_isModuleIntegrityOk(const struct MAL_partition_s * partitionToStart)
{
    if(MAL_isBuildInApplication(partitionToStart))
    {   // check integrity of build-in application
        return MAL_isIntegrityOk(partitionToStart);
    }
    else
    {   // check the integrity of product application
        return MAL_isApplicationIntegrityOk();
    }
}
#endif


#if defined(APP_VARIANT)
bool MAL_writeFirmwareIDs(void)
{
    /*lint -save -e934 Taking address of near auto variable [MISRA 2012 Rule 1.3, required], not a problem on known MCUs  */
    struct MAL_fw_id_array firmwareIdTable;
    uint8_t fwidToWrite = 0;
    uint8_t moduleCount = MAL_GetMemModuleCount();
    uint32_t CrcLength = (sizeof(struct MAL_fw_id_entry) * FWU_FW_ID_MAX_COUNT);

    /* Initialize firmwareIdTable with empty entries */
    OTP_initializeFirmwareIDArray(&firmwareIdTable);

    /* Go trough each partition/module header and append FW ID */
    for(uint8_t moduleIndex = 0; moduleIndex < moduleCount; moduleIndex++)
    {
        const struct MAL_partition_s *partition = MAL_GetMemModuleByIndex(moduleIndex);
        const struct MAL_ModuleHeader_s * header = MAL_PartitionToHeader(partition);
        
        /* Built in applications does not require FWID to be written */
        if(!MAL_isBuildInApplication(partition))
        {
            /* Problem with partition/module header should be caught by BM */
            if((partition != NULL) && (header != NULL))
            {
                if((partition->mod_attributes & MAL_CTX_MF_NO_COMPATIBILITY_CHECK) == 0u)
                {
                    /* mod_attributes does not contain module flag bit MAL_CTX_MF_NO_COMPATIBILITY_CHECK
                     * module's sw_version ID will be part of FWID table */
                    firmwareIdTable.entries[fwidToWrite].mod_number = partition->mod_number;
                    UTI_vMemCopy(header->sw_version.version.ID, firmwareIdTable.entries[fwidToWrite].firmware_id.id, STDV_ID_LENGTH);
                    fwidToWrite++;
                }
            }
        }
        
        /* Stop if it is reached a maximum number of FW ID that has to be written */
        if(fwidToWrite == FWU_FW_ID_MAX_COUNT)
        {
            // while(1); // Should this be error?
            break;
        }
    }
    /* calculate the CRC over all FW ID table which has to be written to the OTP area */
    /*lint -e928 cast from pointer to pointer [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]: tested, it works correctly */
    firmwareIdTable.CRC = CRC_Calc(CrcLength, (const uint8_t*)firmwareIdTable.entries);

    return OTP_writeFirmwareIDs(&firmwareIdTable);
    /*lint -restore -e934 */
}
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief    This interface returns the pointer the the requested partition
 *            entry.
 *
 *  \param    module:   enumerator of requested module
 *
 *  \return   pointer to requested partition in case of success, else null
 */
static const struct MAL_partition_s * MAL_searchMemModule
(
    enum MAL_ModuleEnum          module,
    const struct MAL_partition_s * partitionTbl,
    uint8_t                      maxTableLen
)
{
    if(partitionTbl == NULL){return NULL;} // no valid partition table
    for (uint8_t index = 0u; index < maxTableLen; index++)
    {
        if (partitionTbl[index].mod_number == MAL_NO_MODULE)
        {   // end of table reached --> nothing found
            return NULL;
        }
        if ( partitionTbl[index].mod_number == module )
        {
            return &partitionTbl[index];
        }
    }

    return NULL;
}


/** Creates a dictionary with all modules (Fix- and Flex-Table) */
static void MAL_createModuleDictionary(void)
{
    if(MAL_ModuleCount == 0u) // dictionary already initialized?
    {
      #ifdef FWU_FLEX_PARTITION_USED
        MAL_copyPatToDictionary(MAL_flexPatRef, MAL_FLEX_PARTITION_ENTRIES);
      #endif
        MAL_copyPatToDictionary(MAL_DataAnchor.partitionTblFix,  MAL_FIX_PARTITION_ENTRIES);
    }
}


/** Copies a partition table to the dictionary
 *  \details Duplicated entries will be filtered
 *
 *  \param pTbl: pointer to a partition table to copyright
 *
 *  \param max_size: maximum entries in table
 */
static void MAL_copyPatToDictionary(const struct MAL_partition_s * pTbl, uint8_t max_size)
{
    if(pTbl == NULL){return;} // no valid partition table
    for(uint8_t i =0u; i < max_size; i++)
    {
        if (pTbl[i].mod_number == MAL_NO_MODULE) {break;} // end has been found
        if (MAL_isInDictionary(pTbl[i].mod_number)){continue;} // module already in dictionary
        MAL_moduleDictionary[MAL_ModuleCount] = pTbl[i].mod_number;
        MAL_ModuleCount++;
    }
}


/** \brief   Copies a module header to the RAM
 *
 *  \details Creates a duplicate of SW module header in RAM
 *
 *  \param partition:           pointer to a partition table item
 *  \param moduleHeaderInRam:   pointer to a SW module header in RAM
 *
 *  \return bool: true if memory read function proceed without any errors
 */

static bool MAL_copyModuleHeaderToRAM(const struct MAL_partition_s * partition, struct MAL_ModuleHeader_s * moduleHeaderInRam)
{
    enum MAL_Status status = MAL_ST_ERROR;
    if(MEMDRV_open(partition->driver_id) == MAL_ST_OK)
    {
        /* cast is OK as alignment of word is stricter than pointer of byte */
        status = MEMDRV_read(partition->physical_start.addr, sizeof(struct MAL_ModuleHeader_s), (uint8_t*)moduleHeaderInRam);/*lint !e928 */
        enum MAL_Status closeStatus = MEMDRV_close();
        status = (status == MAL_ST_OK) ? closeStatus : status;
    }
    return ((status == MAL_ST_OK) ? true : false);
}


/** Checks if an entry already is present in dictionary
 *
 *  \param mod_number: module number to check
 *  \return true if module is already in dictionary, otherwise false
 */
static bool MAL_isInDictionary(enum  MAL_ModuleEnum mod_number)
{
    for(uint8_t i = 0u; i< (uint8_t)(MAL_ModuleCount); i++)
    {
        if(MAL_moduleDictionary[i] == mod_number){return true;}

    }
    return false; // module id not found in dictionary
}


#ifdef FWU_FLEX_PARTITION_USED
/** delivers a pointer to the flash partition table
 *
 *  \details This interface returns the pointer to the flex partition table
             It also checks the validity of the table. In case of corrupted
             flex table NULL will be returned
    \return pointer to flex partition table or NULL in case of corrupted partition table
 */
static const struct MAL_partition_s * MAL_GetFlexPartitionTable(void)
{
    const struct MAL_partition_s *flexPat;
    flexPat = MAL_searchMemModule( MAL_FLEX_PARTITION,
                                     MAL_DataAnchor.partitionTblFix,
                                     (uint8_t)MAL_FIX_PARTITION_ENTRIES
                                    );
   #if defined(VARIANT_BOOTMANAGER)
    // return pointer to flex partition table if not corrupted, otherwise NULL
    return MAL_isIntegrityOk(flexPat) ? flexPat : NULL;
  #else
    // integrity check is done already in boot manager
    return flexPat;
  #endif
}
#endif


#if defined(VARIANT_BOOTMANAGER)
/** \brief    returns the size of a module inside a defined partition
 *
 *  \details  If the module size is bigger than the host partition
 *            zero will be returned.
 *
 *  \param    partition:   pointer to requested partition
 *
 *  \return   module length or zero in case of corrupted header
 */
static uint32_t MAL_getModuleSize(const struct MAL_partition_s * partition)
{
    const struct MAL_ModuleHeader_s * header = MAL_PartitionToHeader(partition);
    uint32_t modSize = 0u;
    if(header != NULL) // invalid partition
    {
        uint32_t partSize = MAL_getPartitionSize(partition);
        modSize = (header->size <= partSize) ? header->size : 0u;
    }
    return modSize;
}


/** \brief    This interface checks the integrity of an application module
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \note     Interface \sa MAL_initPartition must be called first
 *  \return   true if application integrity is OK, otherwise false
 */
static bool MAL_isApplicationIntegrityOk(void)
{
    const uint8_t modCount = MAL_GetMemModuleCount();
    for(uint8_t modIndex = 0u; modIndex < modCount; modIndex++)
    {   // do for all partition entries
        const struct MAL_partition_s *partition = MAL_GetMemModuleByIndex(modIndex);
        if(MAL_isBuildInApplication(partition))
        {   // skip build-in application, as they are independent of product app
            continue;
        }
        if(!MAL_isIntegrityOk(partition))
        {
            return false;
        }
        if(!MAL_isConsistencyOk(partition))
        {
            return false;
        }
    }
    return true;
}


/** \brief    This interface checks the integrity of a single module
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \return   true  - if module integrity is OK
 *                    or  module is excluded from integrity check
 *            false - other cases (not valid CRC check, not valid pointer to partition)
 */
static bool MAL_isIntegrityOk(const struct MAL_partition_s * partition)
{
    if(partition != NULL)
    {
        if(MAL_isExludedFromIntegrityCheck(partition))
        {
            return true;
        }
        else
        {
            return MAL_isModuleCrcValid(partition);
        }
    }
    else
    {
        return false;
    }
}


/** \brief    This interface validates the CRC of a module inside partition
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \note     Interface \sa MAL_initPartition must be called first
 *  \return   true, if module is not altered, otherwise false
 */
static bool MAL_isModuleCrcValid(const struct MAL_partition_s * partition)
{
    const uint32_t CrcSize  = sizeof(uint32_t); // reserved size for expected CRC
    uint32_t modSize  = MAL_getModuleSize(partition); // module size in bytes
    if(modSize > CrcSize)
    {
        CRC_crc_t CrcCalc = 0u;
        uint32_t  size         = modSize - CrcSize;
        /* "Conversion between object pointer and void pointer [void*->uint8_t*] MisraC2012-11.5"
           "Pointer arithmetic not allowed [+]  MisraC2012  18.4", but uncritical.*/

        const uint8_t  *startAddr    = (uint8_t*)partition->physical_start.ptr + CrcSize;/*lint !e926 !e9016 */ 
        CRC_crc_t CrcExpected  = (CRC_crc_t)MAL_PartitionToHeader(partition)->crc_expected;
        
      #ifdef FWU_MEM_DRV_READ_ONLY
        if(MEMDRV_open(partition->driver_id) == MAL_ST_OK) 
        {   // memory driver available --> read function must be used
            /*lint -e{934} Taking address of near auto variable [MISRA 2012 Rule 1.3, required], not a problem on known MCUs */
            if(MEMDRV_calcCrc((uint32_t)startAddr, size, &CrcCalc) != MAL_ST_OK)/*lint !e923 cast form pointer to uint32_t not dangerous */
            {
                (void)MEMDRV_close();
                return MAL_handleIntegrityError(partition, BMDAT_ER_CRC);
            }
            (void)MEMDRV_close();
        }
        else
      #endif
        {   //lint -e{926}   "Conversion between object pointer and void pointer MisraC2012 11.5" */
            CrcCalc = CRC_Calc(size, startAddr);/*lint !e926 */
        }

        if(CrcCalc == CrcExpected)
        {
            return true;
        }
    }
    return MAL_handleIntegrityError(partition, BMDAT_ER_CRC);
}


/** \brief    This interface checks if the consistency of memory fragments
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \return   true if module fragments are consistent or for unfragmented modules, otherwise false
 */
static bool MAL_isConsistencyOk(const struct MAL_partition_s * partition)
{
    if(partition != NULL)
    {
        for(uint8_t depModCnt=0u; depModCnt < MAL_maxFragments; depModCnt++)
        {
            uint32_t buildId = MAL_getBuildId(partition);
            if(partition->dep_module == MAL_NO_MODULE)
            {   //+++ normal loop exit +++
                return true;
            }
          #ifdef FWU_FLEX_PARTITION_USED
            if(partition->mod_number == MAL_FLEX_PARTITION)
            {   // fragmented flex table not allowed
                break;
            }
          #endif
            /* handle next module fragment */
            partition = MAL_GetMemModule(partition->dep_module);
            if(buildId != MAL_getBuildId(partition))
            {   // fragmented module is not consistent!
                break;
            }
        }
    }
    return MAL_handleIntegrityError(partition, BMDAT_ER_CONSISTENCY);
}


/** \brief    Checks if a the integrity check is disabled for module.
 *
 *  \details  For modules outside internal flash it is possible to do
 *            integrity check, as BM does have routines to access it.
 *            It is also possible to disable integrity check by using
 *            MAL_CTX_MF_NO_INTEGRITY_CHECK module attribute in partition
  *            entry of specific module.
*
 *  \param    partition:   pointer to requested partition
 *
 *  \return   true if integrity check is disabled for module
 */
static bool MAL_isExludedFromIntegrityCheck(const struct MAL_partition_s * partition)
{
    if((partition->mod_attributes & MAL_CTX_MF_NO_INTEGRITY_CHECK) != 0u)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/** \brief    sets integrity error flags and returns if error is catastrophic or not
 *
 *  \param    partition:   pointer to partition table entry
 *  \param    err_flag:    the error flag to be handled
 *
 *  \return   usually false, true if integrity error handling is disabled
 */
static bool MAL_handleIntegrityError(const struct MAL_partition_s * partition,
                                     enum BMDAT_errorFlags err_flag)
{
    BMDAT_setErrorFlag(err_flag);
    BMDAT_setErrorModule((partition == NULL) ? MAL_NO_MODULE : partition->mod_number);
  #ifndef INTEGRITY_CHECK_DISABLE
    return false;
  #else
    return true; // return always true when integrity checks are disabled
  #endif
}


/** \brief    Returns the build-id of a module in a defined partition
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \return   the build id, or zero in case of null pointer given
 */
static uint32_t MAL_getBuildId(const struct MAL_partition_s * partition)
{
    if(partition != NULL)
    {
        const struct MAL_ModuleHeader_s * header = MAL_PartitionToHeader(partition);
        if(header != NULL)
        {
            return MAL_convertArrayToU32(header->sw_version.version.build);
        }
    }
    return 0u;
}


static uint32_t MAL_convertArrayToU32(const uint8_t* intArray)
{
    uint32_t result = 0u;
    for(uint32_t i = 0u; i< sizeof(uint32_t); i++)
    {
        const uint32_t bitsInByte = 8u;
        result <<= bitsInByte;
        result += intArray[i];
    }
    return result;
}
#endif // defined(VARIANT_BOOTMANAGER)
