/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PTEng
*   COMP_ABBREV      Common
*******************************************************************************/


#ifndef COMMONSTUB_DCL_STUB_H
#define COMMONSTUB_DCL_STUB_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Common Stub function declaration for dynamic SW unit test.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/* (necessary  if this file is provided as stand alone translation unit       */
/* in an application project)                                                 */
/******************************************************************************/
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

/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
extern void BMDAT_setErrorFlag_Stub(enum BMDAT_errorFlags error);
extern void BMDAT_setErrorModule_Stub(enum MAL_ModuleEnum errorModule);
extern CRC_crc_t CRC_Calc_Stub(uint32_t len, const  uint8_t * data);
extern CRC_crc_t CRC_Finish_Stub(CRC_crc_t crc);
extern CRC_crc_t CRC_Init_Stub(void);
extern CRC_crc_t CRC_Update_Stub(CRC_crc_t crc, uint32_t len, const  uint8_t * data);
extern enum MAL_Status MEMDRV_calcCrc_Stub(uint32_t addr, uint32_t size, CRC_crc_t * crc);
extern enum MAL_Status MEMDRV_close_Stub(void);
extern enum MAL_Status MEMDRV_eraseRange_Stub(uint32_t addr, uint32_t size);
extern enum MAL_Status MEMDRV_flush_Stub(void);
extern enum MEMDRV_timeout_e MEMDRV_getTimeout_Stub(void);
extern enum MAL_Status MEMDRV_open_Stub(enum MAL_DriverID_e driverId);
extern enum MAL_Status MEMDRV_read_Stub(uint32_t addr, uint32_t size, uint8_t * data);
extern enum MAL_Status MEMDRV_write_Stub(uint32_t addr, uint32_t size, const  uint8_t * data);
extern void *  memcpy_Stub(void * dest, const  void * source, size_t count);
extern size_t strlen_Stub(const  char * str);



#ifdef __cplusplus
}
#endif

#endif // COMMONSTUB_DCL_STUB_H
