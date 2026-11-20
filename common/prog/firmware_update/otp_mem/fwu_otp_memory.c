/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Implementation of one-time programable area.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include "fwu_otp_memory.h"
#include "utility/utility.h"
#include "build_version.h"

#if defined(FWU_OTP_MEMORY_START_ADDRESS)
    #include "SegmentDef.h"
    #include "std_lib/std_symbol.h"
  #if !defined(FWU_MEM_DRV_READ_ONLY)
    #include "firmware_update/mem_drv/intflash_drv.h"
    #include "firmware_update/mem_drv/intflash_abs.h"
  #endif
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#ifdef REMOTE_FIRMWARE_UPDATE
#define FWU_FW_ID_EMPTY     {MAL_NO_MODULE, {{0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu}}}
#endif

#if !defined(FWU_MEM_DRV_READ_ONLY) && defined(FWU_OTP_MEMORY_START_ADDRESS) && !defined (PSOC4) && !defined(RA4)
#define MAX_WRITE_BLOCK_SIZE    8U
#endif

/******************************************************************************/
/* PRIVATE TYPE DEFINITION                                                    */
/******************************************************************************/

/*lint -save -e19 -e9018 Useless Declaration - it is not */
/* Note 19: Useless Declaration - it is not
 * Note 9018: union declared [MISRA 2012 Rule 19.2, advisory] - needed and uncritical*/

#ifdef FWU_OTP_MEMORY_START_ADDRESS
    // Declaration of OTP compatible types for production data
    OTP_TYPEDEF_ITEM(struct DBPL_ProductionTime, OTP_ProductionTime_t);
    OTP_TYPEDEF_ITEM(struct DBPL_TracingId, OTP_TracingId_t);
    OTP_TYPEDEF_ITEM(struct FWU_hw_version_s, OTP_hw_version_t);
    
  #ifdef REMOTE_FIRMWARE_UPDATE
    OTP_TYPEDEF_ITEM(struct MAL_fw_id_array, OTP_id_array_t);
  #endif /* REMOTE_FIRMWARE_UPDATE */

  #ifdef DBUS2_APPLIANCE_MSG_IN_APP
    OTP_TYPEDEF_ITEM(struct DBPL_ApplianceData, OTP_ApplianceData_t);
  #endif /*DBUS2_APPLIANCE_MSG_IN_APP */

#endif /* FWU_OTP_MEMORY_START_ADDRESS */
/*lint -restore -e19 -e9018*/



#ifdef FWU_OTP_MEMORY_START_ADDRESS
struct OTP_Data_s
{
    OTP_ProductionTime_t   productionTime;
    OTP_TracingId_t        tracingID;
    OTP_hw_version_t       hardwareVersion;
    
  #ifdef REMOTE_FIRMWARE_UPDATE
    OTP_id_array_t         fwIdArray;
  #endif /* REMOTE_FIRMWARE_UPDATE */
    
  #ifdef DBUS2_APPLIANCE_MSG_IN_APP
    OTP_ApplianceData_t    applianceData;
  #endif /*DBUS2_APPLIANCE_MSG_IN_APP */
};
#endif /* FWU_OTP_MEMORY_START_ADDRESS */


/******************************************************************************/
/* OTP MEMORY DATA DEFINITION                                                 */
/******************************************************************************/

/* Define data only if there is OTP memory segment defined */
#if defined(FWU_OTP_MEMORY_START_ADDRESS)
SDEF_SetSegmentConst(FWU_OTP_MEMORY)
/*Axivion Next Line MisraC2012-8.4 Missing declaration for object definition */
SYMBOL_USED const struct OTP_Data_s OTP_Data =
{
    .productionTime = {.raw_data = {0}},
    .tracingID      = {.raw_data = {0}},
    .hardwareVersion = {.raw_data = {0}}
    
  #ifdef REMOTE_FIRMWARE_UPDATE
   ,.fwIdArray =
    {
        .value =
        {
            .CRC = 0xABCDu, // Dummy CRC value - will be overwritten by patchbin
            .entries =
            {
                {MAL_PRODUCT_APP1, {STDV_ParseIdentifier(UID)}},     // Dummy APP FWID for testing
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY,
                FWU_FW_ID_EMPTY
            }
        }
    }
  #endif /* REMOTE_FIRMWARE_UPDATE */
  
  #ifdef DBUS2_APPLIANCE_MSG_IN_APP
   ,.applianceData  = {{}}
  #endif /* DBUS2_APPLIANCE_MSG_IN_APP */
};
SDEF_SetSegmentConst_Default()
#endif /* FWU_OTP_MEMORY_START_ADDRESS */


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/*lint -save -e928 cast from pointer to pointer */
#ifdef FWU_OTP_MEMORY_START_ADDRESS
bool OTP_writeProductionTime(const struct DBPL_ProductionTime* const productionTime)
{
    return OTP_writeFlash(OTP_Data.productionTime.raw_data, sizeof(struct DBPL_ProductionTime), (const uint8_t*)productionTime);
}


bool OTP_writeTracingID(const struct DBPL_TracingId* const tracingID)
{
    return OTP_writeFlash(OTP_Data.tracingID.raw_data, sizeof(struct DBPL_TracingId), (const uint8_t*)tracingID);
}


bool OTP_writeHardwareVersion(const struct FWU_hw_version_s* const hwVersion)
{
    return OTP_writeFlash(OTP_Data.hardwareVersion.raw_data, sizeof(struct FWU_hw_version_s), (const uint8_t*)hwVersion);
}

#endif /* FWU_OTP_MEMORY_START_ADDRESS */


const struct DBPL_ProductionTime* OTP_getProductionTime(void)
{
  #ifdef FWU_OTP_MEMORY_START_ADDRESS
    return &OTP_Data.productionTime.value;
  #else
    return NULL;
  #endif
}


const struct DBPL_TracingId* OTP_getTracingID(void)
{
  #ifdef FWU_OTP_MEMORY_START_ADDRESS
    return &OTP_Data.tracingID.value;
  #else
    return NULL;
  #endif
}


const struct FWU_hw_version_s* OTP_getHardwareVersion(void)
{
  #ifdef FWU_OTP_MEMORY_START_ADDRESS
    return &OTP_Data.hardwareVersion.value;
  #else
    return NULL;
  #endif
}


#ifdef REMOTE_FIRMWARE_UPDATE
bool OTP_writeFirmwareIDs(const struct MAL_fw_id_array* const firmwareIDs)
{
    return OTP_writeFlash(OTP_Data.fwIdArray.raw_data, (sizeof(struct MAL_fw_id_array)), (const uint8_t*)firmwareIDs);
}


void OTP_initializeFirmwareIDArray(struct MAL_fw_id_array* const firmwareIDArray)
{
    //memset((void*)firmwareIDArray, OTP_UNFLASHED_BYTE, sizeof(struct MAL_fw_id_array));
    UTI_vMemFill((uint8_t *)firmwareIDArray, OTP_UNFLASHED_BYTE, (uint16_t)sizeof(struct MAL_fw_id_array));
}


const struct MAL_fw_id_array* OTP_getFirmwareIDs(void)
{
    return OTP_isEmpty(OTP_Data.fwIdArray.raw_data, sizeof(OTP_id_array_t)) ? NULL : &(OTP_Data.fwIdArray.value);
}
#endif


#ifdef DBUS2_APPLIANCE_MSG_IN_APP
const struct DBPL_ApplianceData* OTP_getApplianceData(void)
{
    return &OTP_Data.applianceData.value;
}
#endif
/*lint -restore -e928 */

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS FOR LOW LEVEL ACCESS                           */
/******************************************************************************/

bool OTP_isEmpty(const uint8_t* addr, uint32_t size)
#if !defined(FWU_OTP_MEMORY_START_ADDRESS)
{
    (void)*addr;
    (void)size;
    return false;
}
#else
{
    for(uint32_t n = 0; n < size; n++)
    {
        if(addr[n] != OTP_UNFLASHED_BYTE)
        {
            return false;
        }
    }
    
    return true;
}
#endif /* !defined(FWU_OTP_MEMORY_START_ADDRESS) */


bool OTP_writeFlash(const uint8_t* addr, uint32_t size, const uint8_t* const data)
#if defined(FWU_MEM_DRV_READ_ONLY) || !defined(FWU_OTP_MEMORY_START_ADDRESS)
{
    (void)*addr;
    (void)size;
    (void)*data;
    return false;
}
#else
{
    /* definition of buffer's size, for PSoC it could be 256 bytes, bigger stack is needed */
#if defined (PSOC4) || defined(RA4) || defined(STM32H7)
    const uint32_t writeBufferSize = MIN_WRITEBLOCK_SIZE;
    uint8_t buffer[MIN_WRITEBLOCK_SIZE];
#else
    const uint32_t writeBufferSize = MAX_WRITE_BLOCK_SIZE;
    uint8_t buffer[MAX_WRITE_BLOCK_SIZE];
#endif // defined (PSOC4)

    uint32_t flashedBytes = 0u;
    uint32_t alignedSize = OTP_ALIGNED_SIZE(size);
    uint32_t actualWriteSize;
    /* Check if the section is empty */
    if(!OTP_isEmpty(addr, alignedSize))
    {
        return false;
    }
    
    DRVIF_initFlash();
    if(!DRVIF_isBusy())
    {
        /* Repeat until all data is flashed */
        while(flashedBytes < alignedSize)
        {
            /* Copy next data to buffer */
            for(uint32_t n = 0; n < writeBufferSize; n++)
            {
                uint32_t actualIndex = flashedBytes + n;
                if(actualIndex < size)
                {
                    buffer[n] = data[actualIndex];
                }
                else
                {
                    buffer[n] = OTP_UNFLASHED_BYTE;
                }
            }
            
            /* Get size for actual cycle */
            if((alignedSize - flashedBytes) > writeBufferSize)
            {
                actualWriteSize = writeBufferSize;
            }
            else
            {
                actualWriteSize = (alignedSize - flashedBytes);
            }
            
            /* Flash data */
            /*lint -e923 -e934 cast from pointer to unsigned int, Taking address of near auto variable - tested to work correctly */
            uint32_t flashedIncrement = ABSIF_write((uint32_t)&addr[flashedBytes], actualWriteSize, buffer);
            
            /* Check if flash is successful and increment counter */
            if(flashedIncrement == 0u)
            {
                break;
            }
            else
            {
                flashedBytes += flashedIncrement;
            }
        }
    }
    DRVIF_deinitFlash();

    return (flashedBytes == alignedSize) ? true : false;
}
#endif /* defined(FWU_MEM_DRV_READ_ONLY) || !defined(FWU_OTP_MEMORY_START_ADDRESS) */
