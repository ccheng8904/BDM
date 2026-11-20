/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PTEng
*   UNIT-PREFIX      Common
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Default stub implementation of undefined functions.
*
*/



/******************************************************************************/
/* INCLUDES                                                                   */
/* (necessary  if this file is provided as stand alone translation unit       */
/* in an application project)                                                 */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "CommonStub_dcl.h"

#include "SegmentDef.h"
#include "firmware_update/BootManager/BootManagerSharedData.h"
#include "firmware_update/BootManager/BootManagerSharedDataUc.h"
#include "firmware_update/fwu_version.h"
#include "firmware_update/mal/mal_driver.h"
#include "firmware_update/mal/mal_statuses.h"
#include "firmware_update/mal/moduleAdministration.h"
#include "firmware_update/mal/module_administration_cfg.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include "mal_command.h"
#include "mal_driver.h"
#include "moduleAdministration.h"
#include "std_lib/std_symbol.h"
#include "stdcrc/StdCrc.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile enum BMDAT_errorFlags BMDAT_setErrorFlag_error;

volatile enum MAL_ModuleEnum BMDAT_setErrorModule_errorModule;

volatile uint32_t CRC_Calc_len;
volatile  uint8_t *volatile CRC_Calc_data;
volatile  uint8_t   CRC_Calc_data_argument;
volatile CRC_crc_t CRC_Calc_RET;

volatile CRC_crc_t CRC_Finish_crc;
volatile CRC_crc_t CRC_Finish_RET;

volatile CRC_crc_t CRC_Init_RET;

volatile CRC_crc_t CRC_Update_crc;
volatile uint32_t CRC_Update_len;
volatile  uint8_t *volatile CRC_Update_data;
volatile  uint8_t   CRC_Update_data_argument;
volatile CRC_crc_t CRC_Update_RET;

volatile uint32_t MEMDRV_calcCrc_addr;
volatile uint32_t MEMDRV_calcCrc_size;
volatile CRC_crc_t *volatile MEMDRV_calcCrc_crc;
volatile CRC_crc_t   MEMDRV_calcCrc_crc_argument;
volatile enum MAL_Status MEMDRV_calcCrc_RET;

volatile enum MAL_Status MEMDRV_close_RET;

volatile uint32_t MEMDRV_eraseRange_addr;
volatile uint32_t MEMDRV_eraseRange_size;
volatile enum MAL_Status MEMDRV_eraseRange_RET;

volatile enum MAL_Status MEMDRV_flush_RET;

volatile enum MEMDRV_timeout_e MEMDRV_getTimeout_RET;

volatile enum MAL_DriverID_e MEMDRV_open_driverId;
volatile enum MAL_Status MEMDRV_open_RET;

volatile uint32_t MEMDRV_read_addr;
volatile uint32_t MEMDRV_read_size;
volatile uint8_t *volatile MEMDRV_read_data;
volatile uint8_t   MEMDRV_read_data_argument;
volatile enum MAL_Status MEMDRV_read_RET;

volatile uint32_t MEMDRV_write_addr;
volatile uint32_t MEMDRV_write_size;
volatile  uint8_t *volatile MEMDRV_write_data;
volatile  uint8_t   MEMDRV_write_data_argument;
volatile enum MAL_Status MEMDRV_write_RET;

volatile void *volatile memcpy_dest;
volatile  void *volatile memcpy_source;
volatile size_t memcpy_count;
volatile void *volatile memcpy_RET;

volatile  char *volatile strlen_str;
volatile  char   strlen_str_argument;
volatile size_t strlen_RET;

// BMDAT_sharedData_s BMDAT_sharedData;
const struct MAL_DataAnchor_s MAL_DataAnchor =
{
    MAL_partitionFix,
    MAL_partitionFlex
};
const struct FWU_hw_version_s MAL_hw_version[1];
const struct MAL_partition_s MAL_partitionFix[] =
{
    // Rom-Loder (LRO)
    {
        .mod_name            = "Rom-Loader_LRO",
        .physical_start      = {.addr = 0},
        .physical_end        = {.addr = 0xFFF},
        .mod_number          = MAL_BP2_LOADER_ID,
        .driver_id           = MAL_NoDriver,
        .mod_attributes      = MAL_CTX_MF_EXECUTABLE,
        .dep_module          = MAL_NO_MODULE
    },
    {
        .mod_name            = "Application",
        .physical_start      = {.addr = 0x1000},
        .physical_end        = {.addr = 0x1FFF},
        .mod_number          = MAL_PRODUCT_APP1,
        .driver_id           = MAL_NoDriver,
        .mod_attributes      = MAL_CTX_MF_RWEX,
        .dep_module          = MAL_NO_MODULE
    },
#ifdef FWU_FLEX_PARTITION_USED
    // Flex Partition table
    {
        .mod_name            = "FlexPartition",
        .physical_start      = {.addr = 0x2000},
        .physical_end        = {.addr = 0x2FFF},
        .mod_number          = MAL_FLEX_PARTITION,
        .driver_id           = 0x0815,
        .mod_attributes      = MAL_CTX_MF_RWE,
        .dep_module          = MAL_PRODUCT_APP2 // not allowed!!
    },
#endif
    {
        .mod_number          = MAL_NO_MODULE,
    }
};

#ifdef FWU_FLEX_PARTITION_USED
const struct MAL_partition_s MAL_partitionFlex[] =
{
    {
        .mod_name            = "Rom-Loader_LRO",
        .physical_start      = {.addr = 0},
        .physical_end        = {.addr = 0x7FF},
        .mod_number          = MAL_BP2_LOADER_ID,
        .driver_id           = 0x0815,
        .mod_attributes      = MAL_CTX_MF_RWEX,
        .dep_module          = MAL_NO_MODULE
    },
    {
        .mod_name            = "Application",
        .physical_start      = {.addr = 0x1000},
        .physical_end        = {.addr = 0x1FFF},
        .mod_number          = MAL_PRODUCT_APP1,
        .driver_id           = 0x0815,
        .mod_attributes      = MAL_CTX_MF_RWEX,
        .dep_module          = MAL_PRODUCT_APP2
    },
    {
        .mod_name            = "App_part2",
        .physical_start      = {.addr = 0x3000},
        .physical_end        = {.addr = 0x3FFF},
        .mod_number          = MAL_PRODUCT_APP2,
        .driver_id           = 0x0815,
        .mod_attributes      = MAL_CTX_MF_RWE,  // it is just a part of application --> no execution
        .dep_module          = MAL_NO_MODULE
    }
};
#endif

struct MAL_ModuleHeader_s MDHD_ModuleHeader = {{0}};

const  struct MEMDRV_driver_s * const MEMDRV_driverTable[STUBVAR_ARRAY_SIZE]={0};



/******************************************************************************/
/* FUNCTION POINTER LIST to reference functions under test and                */
/* pointer argument variables use function to prevent linker optimization     */
/******************************************************************************/
static int COMMONSTUB_use_of_pointer_argument_vars(void);

const T_TM_Anchor COMMONSTUB_atTestFuncPtr[] = 
{
    (T_TM_Anchor) COMMONSTUB_use_of_pointer_argument_vars
};


static int COMMONSTUB_use_of_pointer_argument_vars (void)
{
int help = 0;
help += (int) &CRC_Calc_data_argument;
	help += (int) &CRC_Update_data_argument;
	help += (int) &MEMDRV_calcCrc_crc_argument;
	help += (int) &MEMDRV_read_data_argument;
	help += (int) &MEMDRV_write_data_argument;
	help += (int) &strlen_str_argument;
	help += (int) &MDHD_ModuleHeader;

return help;
}




/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
#ifndef BMDAT_SETERRORFLAG_STUB
void BMDAT_setErrorFlag_Stub(enum BMDAT_errorFlags error)
{
    BMDAT_setErrorFlag_error = error;
}
#endif


#ifndef BMDAT_SETERRORMODULE_STUB
void BMDAT_setErrorModule_Stub(enum MAL_ModuleEnum errorModule)
{
    BMDAT_setErrorModule_errorModule = errorModule;
}
#endif


#ifndef CRC_CALC_STUB
CRC_crc_t CRC_Calc_Stub(uint32_t len, const  uint8_t * data)
{
    CRC_Calc_len = len;
    CRC_Calc_data = (volatile  uint8_t *volatile ) data;
    return (CRC_crc_t) CRC_Calc_RET;
}
#endif


#ifndef CRC_FINISH_STUB
CRC_crc_t CRC_Finish_Stub(CRC_crc_t crc)
{
    CRC_Finish_crc = crc;
    return (CRC_crc_t) CRC_Finish_RET;
}
#endif


#ifndef CRC_INIT_STUB
CRC_crc_t CRC_Init_Stub(void)
{
    return (CRC_crc_t) CRC_Init_RET;
}
#endif


#ifndef CRC_UPDATE_STUB
CRC_crc_t CRC_Update_Stub(CRC_crc_t crc, uint32_t len, const  uint8_t * data)
{
    CRC_Update_crc = crc;
    CRC_Update_len = len;
    CRC_Update_data = (volatile  uint8_t *volatile ) data;
    return (CRC_crc_t) CRC_Update_RET;
}
#endif


#ifndef MEMDRV_CALCCRC_STUB
enum MAL_Status MEMDRV_calcCrc_Stub(uint32_t addr, uint32_t size, CRC_crc_t * crc)
{
    MEMDRV_calcCrc_addr = addr;
    MEMDRV_calcCrc_size = size;
    MEMDRV_calcCrc_crc = (volatile CRC_crc_t *)crc;
    *crc = MEMDRV_calcCrc_crc_argument;
    return (enum MAL_Status) MEMDRV_calcCrc_RET;
}
#endif


#ifndef MEMDRV_CLOSE_STUB
enum MAL_Status MEMDRV_close_Stub(void)
{
    return (enum MAL_Status) MEMDRV_close_RET;
}
#endif


#ifndef MEMDRV_ERASERANGE_STUB
enum MAL_Status MEMDRV_eraseRange_Stub(uint32_t addr, uint32_t size)
{
    MEMDRV_eraseRange_addr = addr;
    MEMDRV_eraseRange_size = size;
    return (enum MAL_Status) MEMDRV_eraseRange_RET;
}
#endif


#ifndef MEMDRV_FLUSH_STUB
enum MAL_Status MEMDRV_flush_Stub(void)
{
    return (enum MAL_Status) MEMDRV_flush_RET;
}
#endif


#ifndef MEMDRV_GETTIMEOUT_STUB
enum MEMDRV_timeout_e MEMDRV_getTimeout_Stub(void)
{
    return (enum MEMDRV_timeout_e) MEMDRV_getTimeout_RET;
}
#endif


#ifndef MEMDRV_OPEN_STUB
enum MAL_Status MEMDRV_open_Stub(enum MAL_DriverID_e driverId)
{
    MEMDRV_open_driverId = driverId;
    return (enum MAL_Status) MEMDRV_open_RET;
}
#endif


#ifndef MEMDRV_READ_STUB
enum MAL_Status MEMDRV_read_Stub(uint32_t addr, uint32_t size, uint8_t * data)
{
    MEMDRV_read_addr = addr;
    MEMDRV_read_size = size;
    MEMDRV_read_data = (volatile uint8_t *)data;
    return (enum MAL_Status) MEMDRV_read_RET;
}
#endif


#ifndef MEMDRV_WRITE_STUB
enum MAL_Status MEMDRV_write_Stub(uint32_t addr, uint32_t size, const  uint8_t * data)
{
    MEMDRV_write_addr = addr;
    MEMDRV_write_size = size;
    MEMDRV_write_data = (volatile  uint8_t *volatile ) data;
    return (enum MAL_Status) MEMDRV_write_RET;
}
#endif



#ifndef MEMCPY_STUB
void *  memcpy_Stub(void * dest, const  void * source, size_t count)
{
    memcpy_dest = (volatile void *)dest;
    memcpy_source = (volatile  void *volatile ) source;
    memcpy_count = count;
    return (void * ) memcpy_RET;
}
#endif


#ifndef STRLEN_STUB
size_t strlen_Stub(const  char * str)
{
    strlen_str = (volatile  char *volatile ) str;
    return (size_t) strlen_RET;
}
#endif



