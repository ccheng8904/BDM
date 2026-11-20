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


#ifndef TESTDRIVER_INTERFACE_H
#define TESTDRIVER_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  mipy_interface
 *
 *  \brief    Header for the mipy-interface, 
 *            in general public definitions and macro-UART-map.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "LibTypes.h"


// switch off lint advisories for this file: #/## usage, function like macro
/*lint -e9023 -e9024 */
/*lint -e9026 Function-like macro... [MISRA 2012 Directive 4.9, advisory] */


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#define MAP_UART(channel, function)     HUART ## channel ##function
#define MIPY_MAP_UART(channel, function)         MAP_UART(channel, function)

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/** \brief Setups the HUART, choosen for the MIPY-Interface
 *
 *  \note Uses macro-functions to map the functions to the UART
 *
 *  \return bool for successfull initialisation
 */
BOOL MIPY_bInitTask(void);

/** \brief Handley the message-buffer of the MIPY-interface
 *
 *  \note increments, the start-index, handles the overflow correctly.
 *
 *  \return indicator successfull handling
 */
BOOL MIPY_bHandleTask(void);

/** \brief The UART-Receive Interrupt, puts data to the message-buffer.
 *
 *  \note Handles indexes and errors.
 *
 */
void MIPY_receiveInterrupt(void);


#ifdef __cplusplus
}
#endif

#endif
