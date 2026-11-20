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
#include "firmware_update/mal/mal_statuses.h"
#include "stdcrc/StdCrc.h"
#include "firmware_update/mal/mal_driver.h"
#include "mem_drv.h"
#include "mem_utility/ring_buffer.h"
#include "utility.h"
#include "hwdt.h"
#include "hsup.h"
#include "firmware_update/mem_drv/com_mal.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include "firmware_update/mem_drv/intflash_drv.h"
#include "LibTypes.h"
#include "hsupMc.h"
#include "SegmentDef.h"

/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
extern bool RBUF_writeBuff_Stub(struct RBUF_ringBuffer * rb, const  uint8_t * data, RBUF_size_t len);
extern void HSUP_vDisableInt_Stub(void);
extern void HSUP_vEnableInt_Stub(void);
extern RBUF_size_t RBUF_writeBuffN_Stub(struct RBUF_ringBuffer * rb, const  uint8_t * data, RBUF_size_t len);
extern CRC_crc_t CRC_Calc_Stub(uint32_t len, const  uint8_t * data);
extern void HWDT_vTrigger_Stub(void);
extern CRC_crc_t CRC_Finish_Stub(CRC_crc_t crc);
extern void RBUF_init_Stub(struct RBUF_ringBuffer * rb, uint8_t * buffer, RBUF_size_t size);
extern RBUF_size_t RBUF_getBytesToRead_Stub(const  struct RBUF_ringBuffer * rb);
extern bool COMA_read_Stub(uint32_t addr, uint32_t size, uint8_t * buffer);
extern RBUF_size_t RBUF_readBuffN_Stub(struct RBUF_ringBuffer * rb, uint8_t * buff, RBUF_size_t len);
extern void RBUF_reset_Stub(struct RBUF_ringBuffer * rb);
extern CRC_crc_t CRC_Update_Stub(CRC_crc_t crc, uint32_t len, const  uint8_t * data);
extern CRC_crc_t CRC_Init_Stub(void);
extern uint32_t DRVIF_checkErase_Stub(uint32_t stub_param_1, uint32_t stub_param_2);
extern uint32_t DRVIF_erase_Stub(uint32_t stub_param_1, uint32_t stub_param_2);
extern void DRVIF_deinitFlash_Stub(void);
extern void DRVIF_initFlash_Stub(void);
extern bool DRVIF_isAddrRangeValid_Stub(uint32_t stub_param_1, uint32_t stub_param_2);
extern bool DRVIF_isBusy_Stub(void);
extern bool DRVIF_isError_Stub(void);
extern void DRVIF_resetFlash_Stub(void);
extern uint32_t DRVIF_write_Stub(uint32_t stub_param_1, uint32_t stub_param_2, const uint8_t* stub_param_3);
extern bool DRVIF_read_Stub(uint32_t stub_param_1, uint32_t stub_param_2, uint8_t* stub_param_3);


extern struct MEMDRV_driver_s XXX_driver;


#ifdef __cplusplus
}
#endif

#endif // COMMONSTUB_DCL_STUB_H
