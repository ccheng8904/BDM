/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      TM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief  Public definitions and declarations for TestSupport
 *
 *  This header defines interfaces to support the unit test
 */

#ifndef TESTSUPPORT_H
#define TESTSUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************/
/* INCLUDES                                                                                       */
/**************************************************************************************************/
#include <stdint.h>
#if defined(UNIT_UNDER_TEST)
#include "testsupport/TestSupportXS.h"
#endif


/*lint -save -e9026
  Switch off message(s)
  9026: 'Function-like macro...'        reason: cannot be a function
 */
#if (! defined (UNIT_UNDER_TEST))
/*=======================================================================================*/
/* Switch off any module test instrumentation                                            */
/*=======================================================================================*/
//lint -e{665} "Macro parameter not embraced"
#define  TESTUB(function)   function ///< Call the stub function in case of module test
#define  TP(tp)                  ///< Set the script defined test point in case of module test
#define  TPS(tp)                 ///< Set a stack test point in case of module test
#define  TPW(tp)                 ///< Increment a watch point

#define  TPF(tp)                 ///< Set a flag test point in case of module test


#define  T_CONST     const       ///< In case of test 'const' is rmoved to modify value during test

#else //UNIT_UNDER_TEST
/*=======================================================================================*/
/* Switch on the module test instrumentation                                             */
/*=======================================================================================*/
#define  TESTUB(function)   function ## _Stub

#define  TP(tp)     TM_vSetFlexPoint(tp)     // Set the script defined test point in case of module test
#define  TPS(tp)    TM_vSetStackPoint(tp)    // Set a stack test point in case of module test
#define  TPW(tp)    TM_vSetWatchPoint(tp)    // Increment a watch point

#define  TPF(tp)    TM_vSetFagPoint(tp)      // Set a flag test point in case of module test


#define  T_CONST                             // In case of test 'const' is rmoved to modify value during test

#endif //UNIT_UNDER_TEST

/**************************************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                                        */
/**************************************************************************************************/
//lint -esym(9058,T_TM_Defines)   "tag.. unused outside of typedefs" reason: used in case of unit test
typedef enum T_TM_Defines {
    TM_WATCH_POINT_END = 0,      ///< Marks end of watch point definitions
    TM_NO_WATCH_POINT  = 0x7FFF  ///< Return value of TM_uiFindWatchIndex indicates that test point is not a watch point
} T_TM_Defines;



/** Type for test points
 *  If set to uint8_t just low byte of test point enumeration will be used.
 *  If you are running out of memory better use watch points and stack
*/
typedef uint16_t T_TM_TestPoints;

/** Type for anchor
 *  This type is used for function pointer array to prevent linker optimisation
*/
typedef void (*T_TM_Anchor)(void);


#if !defined(UNIT_TEST) && !defined(TESTSUPPORT_MODULE)

#define TM_vInit()               ///< Initilise test support library if module test is active
#define TM_vResetTestPoints()    ///< Initilise all test point arrays if module test is active

#else
/**************************************************************************************************/
/* DEFINITIONS                                                                                    */
/**************************************************************************************************/
#define  TESTUB_(function)   function ## _Stub    // Call the stub function in case of module test

#define  TP_(tp)     TM_vSetFlexPoint(tp)     // Set the script defined test point in case of module test
#define  TPS_(tp)    TM_vSetStackPoint(tp)    // Set a stack test point in case of module test
#define  TPW_(tp)    TM_vSetWatchPoint(tp)    // Increment a watch point

#define  TPF_(tp)    TM_vSetFagPoint(tp)      // Set a flag test point in case of module test
#define  TPI_(tp)    TM_vIncTestPoint(tp)     // Increment an incrment point in case of module test


typedef void (*TTestPointerHandler)(uint16_t uiTestPoint);

/**************************************************************************************************/
/* DATA DECLARATION                                                                               */
/**************************************************************************************************/
/** Array holds pointer to function pointer array of each module under test.
 *  This anchor is accessed in TM_vInit() and prevent the linker optimisation
*/
extern const T_TM_Anchor* TM_aptAnchor[];


/** Array for test points.
*   For each defined testpoint an array element is needed.
*   In case of big modules or if more or even all modules are under test big array size is required.
*   This array is used by test point macros TPI() and TPF()
*/
extern T_TM_TestPoints   TM_atTestPoints[];


/** Stack for the reached test points.
*   The stack must be big enough for all reached test points during a test step
*   The stack usage can be very high if loops are used inside the code
*   To limit the RAM usage in case of long loops count points should be used.
*
*/
extern uint16_t TM_atTestPointStack[];


/** Array for watch points.
*   This is used to combine advantage of stack points and increment points.
*   If a reached TPS() point can be found in array TM_atWatchPointDef[] the value with the according index
*   will be incremented in TM_atWatchPoints[] instead setting a stack entry.
*   Using macro TPW() just counts defined watch points and never do an enty in stack.
*/
extern uint16_t  TM_atWatchPoints[];

/** Array for counter point definitions.
*   \sa TM_atWatchPoints
*   In this array a list of count points can be defined.
*   The list must be terminated by zero.
*/
extern uint16_t  TM_atWatchPointDef[];

extern const TTestPointerHandler TM_atTestPointHandler[]; ///< Array for test point handlers
extern const uint8_t TM_ucCountOfTpHandlers;                ///< Number of test point handlers
extern const uint16_t TM_uiTpArraySize;                     ///< Size of test point array
extern const uint16_t TM_uiTpStackSize;                     ///< Size of test point stack
extern const uint16_t TM_uiTpWatchArraySize;                ///< Number of watch points + 1

/**************************************************************************************************/
/* FUNCTION DECLARATION                                                                           */
/**************************************************************************************************/
extern void TM_vInit(void); ///< Initialise test support library

/** Clears all test point arrays.
*   Test point type (TM_ucTpSelect) and defined watch points (TM_atWatchPoints) are not touched
*/
extern void TM_vResetTestPoints(void);

extern void TM_vSetFlexPoint (uint16_t uiTestPoint);          ///< Set the script defined test point in case of module test
extern void TM_vSetStackPoint(uint16_t uiTestPoint);          ///< Set a stack test point in case of module test
extern void TM_vSetWatchPoint(uint16_t uiTestPoint);          ///< Increment a watch point

extern void TM_vSetFagPoint(T_TM_TestPoints tTestPoint);    ///< Set a flag test point in case of module test
extern void TM_vIncTestPoint(T_TM_TestPoints tTestPoint);   ///< Increment an incrment point in case of module test

#endif //UNIT_TEST
/*lint -restore -e9026*/

#ifdef __cplusplus
}
#endif

#endif

