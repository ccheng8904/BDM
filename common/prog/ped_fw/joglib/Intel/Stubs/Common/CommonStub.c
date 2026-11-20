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

#include "LibDefines.h"
#include "joglib/joglibXS.h"
#include "joglib.h"
#include "joglibpriv.h"
#include "timer/timerlib.h"
#include "hsup.h"
#include "SegmentDef.h"
#include "LibTypes.h"
#include "SegmentDef.h"
#include "hsupMc.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile uint16 TIM_uiGetCircleMicroSeconds_RET;

Ttimer8 TIM_uc8End;
Ttimer8 TIM_uc16End;
const TtimerConfig TIM_ptTimer8Config[STUBVAR_ARRAY_SIZE]={};
const TtimerConfig TIM_ptTimer16Config[STUBVAR_ARRAY_SIZE]={};



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

return help;
}




/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
#ifndef TIM_UIGETCIRCLEMICROSECONDS_STUB
uint16 TIM_uiGetCircleMicroSeconds_Stub(void)
{
    return (uint16) TIM_uiGetCircleMicroSeconds_RET;
}
#endif


#ifndef HSUP_VDISABLEINT_STUB
void HSUP_vDisableInt_Stub(void)
{
}
#endif


#ifndef HSUP_VENABLEINT_STUB
void HSUP_vEnableInt_Stub(void)
{
}
#endif



