/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MicroPython-Board Testdriver Interface
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MIPY_Interface
 *
 *  \brief    Implementation of testdriver_table_config.
 *
 *  \details  A detailed file description that may span several lines and
 *            that has to be terminated by an empty line.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "testdriver_table_config.h"
#include "testdriver_interface.h"
#include "huart.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
MIPY_driver MIPY_ramp_up_table[] = 
{
    MIPY_huart_ramp_up_driver
};

MIPY_driver MIPY_tear_down_table[] = 
{
    MIPY_huart_tear_down_driver
};


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/


void MIPY_huart_ramp_up_driver(void)
{
    HUART1_vInit(0U);
    HUART1_vEnable();
    HUART1_vEnableRxEvent();
}

void MIPY_huart_tear_down_driver(void)
{
    HUART1_vDisable();
}

void MIPY_UART_Echo3Interrupt(void) {
    
    uint8_t data = MIPY_MAP_UART(1, _ucGetRxData());
    MIPY_MAP_UART(1, _vSetTxData(data));    //test Uart2
}
