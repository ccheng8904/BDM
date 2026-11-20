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
#include "bal.h"
#include "bustypes.h"
#include "dbusdll.h"
#include "dbuspresentation.h"
#include "dbuspresentation_ecu_types.h"
#include "hwdt.h"
#include "LibDefines.h"
#include "LibTypes.h"
#include "ped_fw/watchdogtimer/watchdogtimer.h"
#include "scheduler.h"
#include "scheduler/cfg_templates/scheduler_cfg.h"
#include "scheduler/scheduler_defines.h"
#include "scheduler_cfg.h"
#include "scheduler_time_monitor.h"
#include "scheduler_types.h"
#include "stdbool.h"
#include "timerlib.h"
#include "utility.h"

/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
extern uint32 TIM_u32GetCircleMicroSeconds_Stub(void);
extern bool UTI_bGetBit_Stub(const  uint8_t * pucBitField, uint16_t uiBitId);
extern void UTI_vSetBit_Stub(uint8_t * pucBitField, uint16_t uiBitId, bool bOnOff);
extern void BAL_handleTask_Stub(void);
extern void DBPL_handleTask_Stub(void);
extern void DLL_handleTask_Stub(void);
extern void TIM_handleTask16_Stub(void);
extern void WDT_handleTask_Stub(void);


#ifdef __cplusplus
}
#endif

#endif // COMMONSTUB_DCL_STUB_H
