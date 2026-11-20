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


#ifndef TESTDRIVER_TABLE_CONFIG_H
#define TESTDRIVER_TABLE_CONFIG_H

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
 *  \brief    header for the user-customized mipy-driver tables.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>



/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/** \brief    Size of extern MIPY_driver MIPY_ramp_up_table[]
 *
 *  \details  It has to be configured, because the size cannot be calculated via runtime.
 *            But needs to be checked.
 */
#define MIPY_RAMPUP_TABLESIZE       1U

/** \brief    Size of extern MIPY_driver MIPY_tear_down_table[]
 *
 *  \details  It has to be configured, because the size cannot be calculated via runtime.
 *            But needs to be checked.
 */
#define MIPY_TEARDOWN_TABLESIZE     1U

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief    Type for the drivers to start and check.
 *
 *  \details  The incoming uart-message contains indexes to start those drivers.
 *            One could add return values, to log errors.
 */
typedef void (*MIPY_driver)(void);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
//!< User can put here ramp-up drivers for his tests.
extern MIPY_driver MIPY_ramp_up_table[];

//!< User can put here tear-down drivers for his tests.
extern MIPY_driver MIPY_tear_down_table[];

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/** \brief Eample for a test-driver to start HUART
 *
 *  \note HUART2 on F0 was used.
 */
void MIPY_huart_ramp_up_driver(void);

/** \brief Eample for a test-driver to close HUART
 *
 *  \note HUART2 on F0 was used.
 */
void MIPY_huart_tear_down_driver(void);

/** \brief Eample for a HUART-Echo
 *
 *  \note works for HUART2 on F0. For other controllers 
 *        might not possible, because of timings.
 *
 */
void MIPY_UART_Echo3Interrupt(void);

#ifdef __cplusplus
}
#endif

#endif

