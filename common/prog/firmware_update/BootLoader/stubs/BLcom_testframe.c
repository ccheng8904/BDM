/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      BLCOM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/* Testframe for BLcom unit */

#define UNIT_UNDER_TEST
#include "bsh_stdinc.h"
#include "stdint.h"
#include "stdbool.h"

static bool BLCOM_isTimeOut_Stub(void);
static enum BLCOM_rxResult BLCOM_checkRxResult_Stub(uint8_t receivedCheckSum);
static uint32_t BLCOM_checkRxResult_(uint8_t receivedCheckSum);
static uint32_t BLCOM_receiveFrame_(void);

#include "BLcom.c" // include the unit under test

static const uint8_t BLCOM_TestBuffer[] = "Hello World";
static uint8_t       BLCOM_TestFrameBuffer[10];

const T_TM_Anchor BLCOM_atTestFuncPtr[] = 
{
    (T_TM_Anchor)BLCOM_init,
    (T_TM_Anchor)BLCOM_sendByte,
    (T_TM_Anchor)BLCOM_sendAck,
    (T_TM_Anchor)BLCOM_sendBuffer,
    (T_TM_Anchor)BLCOM_receiveFrame,
    (T_TM_Anchor)BLCOM_receiveFrame_,
    (T_TM_Anchor)BLCOM_setBaudrate,
    (T_TM_Anchor)BLCOM_isTimeOut,
    (T_TM_Anchor)BLCOM_checkRxResult,
    (T_TM_Anchor)BLCOM_checkRxResult_,
    (T_TM_Anchor)BLCOM_TestBuffer,
    (T_TM_Anchor)BLCOM_TestFrameBuffer
};

extern const T_TM_Anchor TM_TEST_SupportFuncPtr[];


T_TM_Anchor const * TM_aptAnchor[] =
{
        BLCOM_atTestFuncPtr,
        TM_TEST_SupportFuncPtr
};


#ifndef __NOT_TARGET_CC__
void ITBL_vResetHandler(void)
{
    TM_vInit();
    HSUP_vGenerateReset();
}
#endif


/** Internal function stubs */
volatile bool BLCOM_isTimeOut_RET;
static bool BLCOM_isTimeOut_Stub(void)
{
    return (bool)BLCOM_isTimeOut_RET;
}


volatile enum BLCOM_rxResult BLCOM_checkRxResult_RET;
volatile uint8_t BLCOM_checkRxResult_receivedCheckSum;
static enum BLCOM_rxResult BLCOM_checkRxResult_Stub(uint8_t receivedCheckSum)
{
    BLCOM_checkRxResult_receivedCheckSum = receivedCheckSum;
    return (enum BLCOM_rxResult)BLCOM_checkRxResult_RET;
}

static uint32_t BLCOM_checkRxResult_(uint8_t receivedCheckSum)
{
    return (uint32_t)BLCOM_checkRxResult(receivedCheckSum);
}



uint8_t *BLCOM_receiveFrame_arg_buffer;
uint16_t BLCOM_receiveFrame_arg_length;
uint16_t BLCOM_receiveFrame_arg_timeout;
static uint32_t BLCOM_receiveFrame_(void)
{
    return (uint32_t)BLCOM_receiveFrame(BLCOM_receiveFrame_arg_buffer, BLCOM_receiveFrame_arg_length, BLCOM_receiveFrame_arg_timeout);
}


volatile uint16_t TIM_uiGetCircleMicroSeconds_RET;
uint16_t TIM_uiGetCircleMicroSeconds(void)
{
    return (uint16_t)TIM_uiGetCircleMicroSeconds_RET;
}

volatile BOOL _TIM_bGetInitConfig_RET;
volatile uint32_t _TIM_bGetInitConfig_cnt;
BOOL _TIM_bGetInitConfig(void)
{
    _TIM_bGetInitConfig_cnt++;
    return (BOOL)_TIM_bGetInitConfig_RET;
}


volatile uint32_t HWDT_vTrigger_cnt;
void HWDT_vTrigger(void)
{
    HWDT_vTrigger_cnt++;
}

