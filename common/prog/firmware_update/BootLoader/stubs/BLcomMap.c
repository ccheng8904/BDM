/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/

/* Stub-Functions for BLcomMAP */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#ifdef MCAL_INCLUDED
  #include "firmware_update/BootLoader/BLcomMcalMap.h"
#else
  #include "firmware_update/BootLoader/BLcomMap.h"
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

