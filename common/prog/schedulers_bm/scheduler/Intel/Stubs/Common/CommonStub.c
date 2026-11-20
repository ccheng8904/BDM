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


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile uint32 TIM_u32GetCircleMicroSeconds_RET;

volatile  uint8_t *volatile UTI_bGetBit_pucBitField;
volatile  uint8_t   UTI_bGetBit_pucBitField_argument;
volatile uint16_t UTI_bGetBit_uiBitId;
volatile bool UTI_bGetBit_RET;

volatile uint8_t *volatile UTI_vSetBit_pucBitField;
volatile uint8_t   UTI_vSetBit_pucBitField_argument;
volatile uint16_t UTI_vSetBit_uiBitId;
volatile bool UTI_vSetBit_bOnOff;

const TbusObjectTable BAL_tBusObject[STUBVAR_ARRAY_SIZE]={};
const uchar BAL_ucMaxCollisionRetries;
const uchar BAL_ucMaxMissingAckRetries;
const uchar BAL_ucMaxNackRetries;
const uchar BAL_ucNumberOfSubsystems;
BOOL DBPL_bInvalidReadProcess;
BOOL DBPL_bInvalidWriteProcess;
const TbusModuleTable DBPL_tModuleDict[STUBVAR_ARRAY_SIZE]={};
const TbusReceiveObject DBPL_tReceiveAll16BitServiceMessages[STUBVAR_ARRAY_SIZE]={};
const TbusReceiveObject DBPL_tReceiveAllServiceMessages[STUBVAR_ARRAY_SIZE]={};
const TbusReceiveObject DBPL_tReceiveAllServiceMessagesUpdate[STUBVAR_ARRAY_SIZE]={};
const TbusReceiveObject DBPL_tReceiveMandatoryServiceMessages[STUBVAR_ARRAY_SIZE]={};
const uchar DBPL_ucNumberOfElements;
uint16 DBPL_uiPage;
struct DllConfigurationItem DLL_ConfigurationItem;

const TtimerConfig TIM_ptTimer16Config[STUBVAR_ARRAY_SIZE]={};
const TtimerConfig TIM_ptTimer8Config[STUBVAR_ARRAY_SIZE]={};
Ttimer8 TIM_uc16End;
Ttimer8 TIM_uc8End;




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
help += (int) &UTI_bGetBit_pucBitField_argument;
	help += (int) &UTI_vSetBit_pucBitField_argument;

return help;
}




/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
#ifndef TIM_U32GETCIRCLEMICROSECONDS_STUB
uint32 TIM_u32GetCircleMicroSeconds_Stub(void)
{
    return (uint32) TIM_u32GetCircleMicroSeconds_RET;
}
#endif


#ifndef UTI_BGETBIT_STUB
bool UTI_bGetBit_Stub(const  uint8_t * pucBitField, uint16_t uiBitId)
{
    UTI_bGetBit_pucBitField = (volatile  uint8_t *volatile ) pucBitField;
    UTI_bGetBit_uiBitId = uiBitId;
    return (bool) UTI_bGetBit_RET;
}
#endif


#ifndef UTI_VSETBIT_STUB
void UTI_vSetBit_Stub(uint8_t * pucBitField, uint16_t uiBitId, bool bOnOff)
{
    UTI_vSetBit_pucBitField = (volatile uint8_t *)pucBitField;
    UTI_vSetBit_uiBitId = uiBitId;
    UTI_vSetBit_bOnOff = bOnOff;
}
#endif


#ifndef BAL_HANDLETASK_STUB
void BAL_handleTask_Stub(void)
{

}
#endif

#ifndef DBPL_HANDLETASK_STUB
void DBPL_handleTask_Stub(void)
{

}
#endif

#ifndef DLL_HANDLETASK_STUB
void DLL_handleTask_Stub(void)
{

}
#endif

#ifndef TIM_HANDLETASK16_STUB
void TIM_handleTask16_Stub(void)
{

}
#endif

#ifndef WDT_HANDLETASK_STUB
void WDT_handleTask_Stub(void)
{

}
#endif


