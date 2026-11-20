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
 *  COMP_ABBREV      BLMLP
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/* Testframe for BLmessageLoop unit */


#define UNIT_UNDER_TEST
#include "bsh_stdinc.h"
#include "stdint.h"
volatile uint16_t BLMLP_handleMessageLoop_timeOut;

#include "BLmessageLoop.c" // include the unit under test


const T_TM_Anchor BLMLP_atTestFuncPtr[] = 
{
    (T_TM_Anchor)BLMLP_init,
    (T_TM_Anchor)BLMLP_handleMessageLoop
};

extern const T_TM_Anchor TM_TEST_SupportFuncPtr[];


T_TM_Anchor const * TM_aptAnchor[] =
{
        BLMLP_atTestFuncPtr,
        TM_TEST_SupportFuncPtr
};


#ifndef __NOT_TARGET_CC__
void ITBL_vResetHandler(void)
{
    TM_vInit();
    HSUP_vGenerateReset();
}
#endif




volatile uint8_t BLCOM_sendByte_data;
void BLCOM_sendByte(uint8_t data)
{
    BLCOM_sendByte_data = data;
}


volatile uint8_t BLCOM_sendAck_ack;
void BLCOM_sendAck(uint8_t ack)
{
    BLCOM_sendAck_ack = ack;
}


volatile uint8_t BLCOM_init_TP;
void BLCOM_init(void)
{
    BLCOM_init_TP = 1;
}


volatile uint8_t BLSRV_init_TP;
void BLSRV_init(void)
{
    BLSRV_init_TP = 1;
}


volatile enum BLCOM_rxResult BLSRV_doCommandService_RET;
volatile const uint8_t *BLSRV_doCommandService_buffer;
enum BLCOM_rxResult BLSRV_doCommandService(const uint8_t *buffer)
{
    BLSRV_doCommandService_buffer = buffer;
    return (enum BLCOM_rxResult)BLSRV_doCommandService_RET;
}


volatile enum BLCOM_rxResult BLCOM_receiveFrame_RET;
volatile uint8_t *BLCOM_receiveFrame_buffer;
volatile uint16_t BLCOM_receiveFrame_length;
volatile uint16_t BLCOM_receiveFrame_timeout;
enum BLCOM_rxResult BLCOM_receiveFrame(uint8_t *buffer, uint16_t length, uint16_t timeout)
{
    BLCOM_receiveFrame_buffer = buffer;
    BLCOM_receiveFrame_length = length;
    BLCOM_receiveFrame_timeout = timeout;
    return (enum BLCOM_rxResult)BLCOM_receiveFrame_RET;
}


volatile uint8_t HSUP_vGenerateReset_TP;
void HSUP_vGenerateReset(void)
{
    HSUP_vGenerateReset_TP = 1;
}
