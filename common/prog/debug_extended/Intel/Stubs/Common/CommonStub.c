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

#include "LibTypes.h"
#include "SegmentDef.h"
#include "debug/debug_api.h"
#include "debug/debug_api_config.h"
#include "debug_extended/api.h"
#include "debug_extended/error.h"
#include "debug_extended/interface.h"
#include "debug_extended/protocol.h"
#include "hsup.h"
#include "hsupMc.h"
#include "mem_utility/ring_buffer.h"
#include "ped_fw/stdcrc/CrcCom.h"
#include "sts_sim_test_uty.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile char DBG_writeChar_ch;

volatile uint32 TIM_u32GetCircleMicroSeconds_RET;


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
#ifndef DBG_WRITECHAR_STUB
void DBG_writeChar_Stub(char ch)
{
    DBG_writeChar_ch = ch;
}
#endif

#ifndef DBG_WRITESTRINGLEN_STUB
void DBG_writeStringLen_Stub(const char * str, uint16_t len)
{
}
#endif


#ifndef DBG_READCHAR_STUB
char DBG_readChar_Stub(void)
{
    return '0';
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


#ifndef HSUP_VGENERATERESET_STUB
void HSUP_vGenerateReset_Stub(void)
{
}
#endif


#ifndef TIM_U32GETCIRCLEMICROSECONDS_STUB
uint32 TIM_u32GetCircleMicroSeconds_Stub(void)
{
    return (uint32) TIM_u32GetCircleMicroSeconds_RET;
}
#endif


#ifndef MUART_INIT_STUB
uint8_t MUART_init_Stub(struct MUART_Handle *handle, const struct MUART_Channel *channel, const struct MUART_Config *cfg)
{
    return 1;
}
#endif


#ifndef MUART_ENABLE_STUB
uint8_t MUART_enable_Stub(struct MUART_Handle *handle)
{
    return 1;
}
#endif


#ifndef MUART_ENABLEEVENT_STUB
uint8_t MUART_enableEvent_Stub(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
{
    return 1;
}
#endif


#ifndef MUART_WRITE_STUB
enum MCAL_Error MUART_write_Stub(struct MUART_Handle *handle, const uint8_t *buffer, uint16_t length)
{
    return 1;
}
#endif


#ifndef MUART_WRITESTR_STUB
uint8_t MUART_writeStr_Stub(struct MUART_Handle *handle, const char *str)
{
    return 1;
}
#endif


#ifndef MUART_READ_STUB
enum MCAL_Error MUART_read_Stub(struct MUART_Handle *handle, uint8_t *buffer, uint16_t length)
{
    return 1;
}
#endif


#ifndef MCAL_INITCALLBACK_STUB
void MCAL_initCallback_Stub(MCAL_Callback_t *callback, MCAL_CallbackFunction_t callbackFunction, void *objPtr)
{

}
#endif

