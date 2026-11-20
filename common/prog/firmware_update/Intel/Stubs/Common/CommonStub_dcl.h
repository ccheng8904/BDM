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
#include "BLcom.h"
#include "BLcomMap.h"
#include "BLcommands.h"
#include "BLconfig.h"
#include "BLmal.h"
#include "BLmessageLoop.h"
#include "BLservices.h"
#include "SegmentDef.h"
#include "hsup.h"
#include "hsupMc.h"
#include "hwdt.h"
#include "LibTypes.h"
#include "stdbool.h"
#include "stdcrc/StdCrc.h"
#include "timer/timerlib.h"
#include "utility/utility.h"

/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
extern bool BLMAL_eraseBlock_Stub(uint32_t addr);
extern void BLMAL_init_Stub(void);
extern bool BLMAL_isInternalMemory_Stub(uint32_t addr);
extern bool BLMAL_readBuffer_Stub(uint32_t addr, uint32_t size, uint8_t * buffer);
extern bool BLMAL_writeBuffer_Stub(uint32_t addr, uint32_t size, const  uint8_t * buffer);
extern CRC_crc_t CRC_Calc_Stub(uint32_t len, const  uint8_t * data);
extern CRC_crc_t CRC_Finish_Stub(CRC_crc_t crc);
extern CRC_crc_t CRC_Init_Stub(void);
extern CRC_crc_t CRC_Update_Stub(CRC_crc_t crc, uint32_t len, const  uint8_t * data);
extern void HSUP_vGenerateReset_Stub(void);
extern void HWDT_vTrigger_Stub(void);
extern uint16 TIM_uiGetCircleMicroSeconds_Stub(void);
extern BOOL _TIM_bGetInitConfig_Stub(void);



extern volatile uint16_t BLSRV_jumpLoader_address;
extern volatile  uint16_t BLSRV_jumpLoader_length;
extern volatile  enum BLCOM_rxResult BLSRV_jumpLoader_RET;
extern volatile  uint16_t BLSRV_storeData_address;
extern volatile  uint16_t BLSRV_storeData_length;
extern volatile  enum BLCOM_rxResult BLSRV_storeData_RET;

extern volatile uint16_t BLMLP_handleMessageLoop_timeOut;

extern uint8_t BLSRV_TestBuffer[60];
extern const uint8_t BLCOM_TestBuffer[];
extern uint8_t       BLCOM_TestFrameBuffer[];
#ifdef __cplusplus
}
#endif

#endif // COMMONSTUB_DCL_STUB_H
