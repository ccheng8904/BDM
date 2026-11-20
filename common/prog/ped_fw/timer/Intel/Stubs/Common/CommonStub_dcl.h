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
#include "hsup.h"
#include "hsupMc.h"
#include "LibTypes.h"
#include "SegmentDef.h"
#include "std_lib/std_callback.h"
#include "system_timer.h"
#include "timerlib.h"

/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
extern uint16 TIM_uiGetCircleMicroSeconds_Stub(void);
extern BOOL _TIM_bGetInitConfig_Stub(void);
extern void STIM_SchedulerHandleTask_Stub(void);
extern void HSUP_vEnableInt_Stub(void);
extern int32_t STIM_InitModule_Stub(void);
extern void STIM_SchedulerHandleTask_Stub(void);



#ifdef __cplusplus
}
#endif

#endif // COMMONSTUB_DCL_STUB_H
