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


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile uint32_t BLMAL_eraseBlock_addr;
volatile bool BLMAL_eraseBlock_RET;

volatile uint32_t BLMAL_isInternalMemory_addr;
volatile bool BLMAL_isInternalMemory_RET;

volatile uint32_t BLMAL_readBuffer_addr;
volatile uint32_t BLMAL_readBuffer_size;
volatile uint8_t *volatile BLMAL_readBuffer_buffer;
volatile uint8_t   BLMAL_readBuffer_buffer_argument;
volatile bool BLMAL_readBuffer_RET;

volatile uint32_t BLMAL_writeBuffer_addr;
volatile uint32_t BLMAL_writeBuffer_size;
volatile  uint8_t *volatile BLMAL_writeBuffer_buffer;
volatile  uint8_t   BLMAL_writeBuffer_buffer_argument;
volatile bool BLMAL_writeBuffer_RET;

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

volatile uint16 TIM_uiGetCircleMicroSeconds_RET;

//const T_HWDT_Config HWDT_atConfig[STUBVAR_ARRAY_SIZE]={};
const TtimerConfig TIM_ptTimer16Config[STUBVAR_ARRAY_SIZE]={{0}};
const TtimerConfig TIM_ptTimer8Config[STUBVAR_ARRAY_SIZE]={{0}};
Ttimer8 TIM_uc16End;
Ttimer8 TIM_uc8End;

volatile uint16_t BLMLP_handleMessageLoop_timeOut;


const uint8_t BLSRV_TestCmdStoreData[]      = {0x0A, 0x22, 0x55, 0xAA};
const uint8_t BLSRV_TestCmdJumpLoaderData[] = {0x1A, 0x00, 0x44, 0xBB};
const uint8_t BLSRV_TestCmdInvalid[]        = {0x7, 0x00, 0x44, 0xBB};
const uint8_t BLSRV_TestAddress[]           = {0x11, 0x22, 0x33, 0x44};
uint8_t BLSRV_TestBuffer[60];
const uint8_t BLCOM_TestBuffer[] = "Hello World";
uint8_t       BLCOM_TestFrameBuffer[10];
volatile BOOL _TIM_bGetInitConfig_RET;

/******************************************************************************/
/* FUNCTION POINTER LIST to reference functions under test and                */
/* pointer argument variables use function to prevent linker optimization     */
/******************************************************************************/
static int COMMONSTUB_use_of_pointer_argument_vars(void);

const T_TM_Anchor COMMONSTUB_atTestFuncPtr[] = 
{
    (T_TM_Anchor) COMMONSTUB_use_of_pointer_argument_vars,
    (T_TM_Anchor) BLCOM_TestBuffer,
    (T_TM_Anchor) BLCOM_TestFrameBuffer,
    (T_TM_Anchor) BLSRV_TestCmdStoreData,
    (T_TM_Anchor) BLSRV_TestCmdJumpLoaderData,
    (T_TM_Anchor) BLSRV_TestCmdInvalid,
    (T_TM_Anchor) BLSRV_TestAddress
};


static int COMMONSTUB_use_of_pointer_argument_vars (void)
{
int help = 0;
help += (int) &BLMAL_readBuffer_buffer_argument;
	help += (int) &BLMAL_writeBuffer_buffer_argument;
	help += (int) &CRC_Calc_data_argument;
	help += (int) &CRC_Update_data_argument;

return help;
}




/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
#ifndef BLMAL_ERASEBLOCK_STUB
bool BLMAL_eraseBlock_Stub(uint32_t addr)
{
    BLMAL_eraseBlock_addr = addr;
    return (bool) BLMAL_eraseBlock_RET;
}
#endif


#ifndef BLMAL_INIT_STUB
void BLMAL_init_Stub(void)
{
}
#endif


#ifndef BLMAL_ISINTERNALMEMORY_STUB
bool BLMAL_isInternalMemory_Stub(uint32_t addr)
{
    BLMAL_isInternalMemory_addr = addr;
    return (bool) BLMAL_isInternalMemory_RET;
}
#endif


#ifndef BLMAL_READBUFFER_STUB
bool BLMAL_readBuffer_Stub(uint32_t addr, uint32_t size, uint8_t * buffer)
{
    BLMAL_readBuffer_addr = addr;
    BLMAL_readBuffer_size = size;
    BLMAL_readBuffer_buffer = (volatile uint8_t *)buffer;
    return (bool) BLMAL_readBuffer_RET;
}
#endif


#ifndef BLMAL_WRITEBUFFER_STUB
bool BLMAL_writeBuffer_Stub(uint32_t addr, uint32_t size, const  uint8_t * buffer)
{
    BLMAL_writeBuffer_addr = addr;
    BLMAL_writeBuffer_size = size;
    BLMAL_writeBuffer_buffer = (volatile  uint8_t *volatile ) buffer;
    return (bool) BLMAL_writeBuffer_RET;
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

volatile uint8_t HSUP_vGenerateReset_TP;
#ifndef HSUP_VGENERATERESET_STUB
void HSUP_vGenerateReset_Stub(void)
{
    HSUP_vGenerateReset_TP = 1;
}
#endif


#ifndef HWDT_VTRIGGER_STUB
void HWDT_vTrigger_Stub(void)
{
}
#endif


#ifndef TIM_UIGETCIRCLEMICROSECONDS_STUB
uint16 TIM_uiGetCircleMicroSeconds_Stub(void)
{
    return (uint16) TIM_uiGetCircleMicroSeconds_RET;
}
#endif



uint32_t volatile BLCOM_initUart_cnt;
void BLCOM_initUart(void)
{
    BLCOM_initUart_cnt++;
}


uint32_t volatile BLCOM_enableUart_cnt;
void BLCOM_enableUart(void)
{
    BLCOM_enableUart_cnt++;
}
volatile uint16_t BLCOM_Configure_baudrate;
void BLCOM_Configure(uint16_t baudrate)
{
    BLCOM_Configure_baudrate = baudrate;
}
volatile uint8_t BLCOM_setTxData_data;
volatile uint8_t BLCOM_setTxData_index;
volatile uint8_t BLCOM_setTxData_buffer[256];
void BLCOM_setTxData(uint8_t data)
{
    BLCOM_setTxData_data = data;
    BLCOM_setTxData_buffer[BLCOM_setTxData_index++] = data;
    if ( BLCOM_setTxData_index >= sizeof(BLCOM_setTxData_buffer) )
    {
        BLCOM_setTxData_index = 0;
    }
}
volatile bool BLCOM_isTxOnGoing_RET;
bool BLCOM_isTxOnGoing(void)
{
    return BLCOM_isTxOnGoing_RET;
}
volatile bool BLCOM_isDataAvailable_RET;
bool BLCOM_isDataAvailable(void)
{
    return BLCOM_isDataAvailable_RET;
}
volatile bool BLCOM_isReceiveError_RET;
bool BLCOM_isReceiveError(void)
{
    return BLCOM_isReceiveError_RET;
}
volatile uint8_t BLCOM_getRxData_index;
volatile uint8_t BLCOM_getRxData_buffer[256];
uint8_t BLCOM_getRxData(void)
{
    uint8_t data = BLCOM_getRxData_buffer[BLCOM_getRxData_index++];
    if ( BLCOM_getRxData_index >= sizeof(BLCOM_getRxData_buffer) )
    {
        BLCOM_getRxData_index = 0;
    }
    return data;
}

#ifndef _TIM_BGETINITCONFIG_STUB
BOOL _TIM_bGetInitConfig_Stub(void)
{
    return (BOOL) _TIM_bGetInitConfig_RET;
}
#endif
