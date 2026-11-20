/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          TEST_MODULE
 ******************************************************************************/
#ifndef TESTMODULEXS_H
#define TESTMODULEXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file      TestModuleXS.h
 *
 *  \brief    published application specific definitions and declarations for Test Module.
 *
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** Maximal number of test points, user defined. */
#define TM_MAX_TESTPOINTS        (30)

/** Maximal entries of runtime measurements values, user defined. */
#define TM_MAX_RTIME_ENTRIES     (15)

/**
 * \brief size of debug array (for use with Tcl Test Scripts)
 */
#define TM_DEBUG_ARRAY_SIZE 20

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
extern uint8_t TM_aucDebugArray[];

extern bool TM_bCriticalSection;

/* Test Preset*/
/**
    Do neccessary presets before calling the test server.
\param none
\return
   none
\remarks
   This function is useful only for library tests and has following task :
   - Configure platform if necesary : IO, UART, ADC etc.
   - Load test symbols to be accessed from script (by calling them in a safe way)
   - Do necessary resets to set software in a correct initial state
   - enable system interrupts (mandatory for DBUS)
*/
void TM_vTestPreset (void);

/* Trigger Functions*/
#ifdef TM_TRIGGER_INCLUDED
/**
 *  Set a trigger pin ON or OFF.
 *  \param ucTrgPin - Trigger pin in range 1 to 255
 *  \param btrgOn   - true=ON/false=OFF
 *  \return
 *    none
 *  \remarks
 *    ucTrgPin = 0 clears all triggers
 */
extern void TM_vSetTrg (uint8_t ucTrgPin, bool btrgOn);
#endif //TM_TRIGGER_INCLUDED

#if defined(STM32_ARM)
/* TM_vMEMinit()*/
void TM_vMEMinit (void);

/* TM_vMEMRead()*/
void TM_vMEMRead(void);

/* TM_vTestHandleTask()*/
void TM_vTestHandleTask(void);
#endif //defined(STM32_ARM)

#ifdef __cplusplus
}
#endif

#endif //TESTMODULEXS_H

