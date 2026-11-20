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
 *  COMP_ABBREV      MPX
 ******************************************************************************/
#ifndef MPXLIB_H
#define MPXLIB_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*lint -save -e9059 "C comment contains C++ comment"
 * Codeblock with comments for doxygen needed. Tested with various parsers to work correctly
 */
/**
 * \file 
 * 
 * \brief Public declarations and definitions of Multiplexer library
 *
 * \details \section intro 1. Introduction
 *
 *  The multiplexer library is designed to drive a display multiplexer.\n
 *  Although an input multiplexing is not implemented, the interfaces for this appliance are fixed.
 *  The Input multiplexer can be implemented easily using the same timer interrupt as output multiplexer.\n
 *  \n
 *  There are two different interface groups to set a multiplexer output:
 *  \n
 *  1. Using the Identifier
 *  - MPX_vSet(uint8_t ucIdentifier, bool bOnOff)
 *  - MPX_bGet(uint8_t ucIdentifier)
 *  - MPX_vToggle(uint8_t ucIdentifier)
 *  
 *  2. Using Column and Lines
 *  - MPX_vSetPoint(uint8_t ucColumn, uint8_t ucLine, bool bOnOff)
 *  - MPX_bGetPoint(uint8_t ucColumn, uint8_t ucLine)
 *  - MPX_vTogglePoint(uint8_t ucColumn, uint8_t ucLine)
 *  
 *  \note If possible use the first interface (better design).\n
 *        So upper layers will be more resistant against multiplexer changes.
 *  
 *  \n
 *  
 *  The following picture shows an example of multiplexer with 7 lines and 8 columns. This library supports Idintifiers form 0..255 multiplexing.\n
 *  There are two different ways to address an output:\n
 *  - colume, line
 *  - Id (number in intersection point)
 *  
 *  
 *  \image html MpxMatrix.gif "Picture 1: Example of a 7x8 multiplexer (principle illustration)"
 *  
 *  \section configu 2. Configuration
 *  
 *  For Configuration mpxXS.c and mpxXS.h may be adapted for application.\n
 *  
 *  \subsection configuxsc 2.1 Adaptation of mpxXS.c
 *  
 *  The following values must set by application in mpxXS.c (if known at compile time):\n
 *  \code
 *  #define MPX_COLUMN_COUNT 8
 *  #define MPX_LINE_COUNT   7
 *  \endcode
 *  \n
 *  
 *  Next step is defining the function MPX_bGetInitConfig()
 *  If e.g. number of columns or lines has to be read from eeprom this shall be done in this function
 *  (Runtime-Configuration). As the size of MPX_pucDataBuffer cannot changed at runtime, the buffer has to be
 *  defined for the largest variant.\n
 *  The other work of this function is to initialise the timer interrupt for the multiplexer.
 *  
 *  \code
 *  bool MPX_bGetInitConfig(void)
 *  {
 *    M_MPX_vSetTimerMode(0);//compare, pin under port control
 *    M_MPX_vEnableEvent();  //enable interrupts
 *    return TRUE;
 *  {
 *  \endcode
 *  
 *  The heart of multiplexer is the timer interrupt:
 *  
 *  \code
 *  void M_MPX_vHandleEvent()
 *  {
 *   
 *    M_MPX_vSetChannel((TcolumnMask)(1<<MPX_ucColumn));
 *    M_MPX_vSetLine(tGetColumnData(MPX_ucColumn));
 *    M_MPX_vLoadTimer(MPX_MULTIPLEX_TIME);
 *    
 *    if (++MPX_ucColumn >= MPX_COLUMN_COUNT)
 *        MPX_ucColumn = 0;
 *  }
 *  \endcode
 *  \n
 *  \n
 *  
 *  
 *  \subsection configuxsh 2.2 Adaptation of mpxXS.h
 *  
 *  As mpxXS.h is included in mpxlib.h it is possible to publish application specific definitions and declarations.\n
 *  Overwriting the function MPX_bGetInitConfig() can create new dependencies of multiplexer.
 *  If e.g. eeprom is read in this function, the definition of #DEP_MPX shall be changed:\n
 *  \code
 *  #define DEP_MPX     DEPLIB_MPX  MOD_EEP, SCH_END_MARK
 *  \endcode
 *  \note There is no comma after DEPLIB_MPX
 *  
 *  
 *  \subsection configenhanced 2.3 Enhanced Configuration
 *  
 *  If you need (column specific) volume adjustment please contact author of this library.\n
 *  
 *  
 */
/*lint -restore -e9059 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "mpx/mpxXs.h"


#define DEPLIB_MPX      ///< dependencies of multiplexer libraries (MPX has no dependencies)


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/** Taskhandler of Multiplexer

The Task has to be called until TASK_TERMINATE will be returned.\n
The task is initialised when TASK_INITIALISED or TASK_TERMINATE will be returned.

\return \n
     Current state of multiplexer (info/command for scheduler)\n
  \b Type: uint8_t\n
  \b Range: [TASK_NOT_INITIALISED, TASK_INITIALISED, TASK_TERMINATE]
*/
extern uint8_t MPX_ucHandleTask(void);

/** Set/resets a point in the matrix

This Function set/resets the point with ID \<ucIdentifier\>.

\param ucIdentifier: \n
       Identifier of point to set\n
    \b Type: uint8_t\n
    \b Range: [0..MPX_COLUMN_COUNT*MPX_LINE_COUNT -1]

\param bOnOff: \n
       New state for point\n
   \b Type: bool\n
   \b Range: 0 = OFF, all others = ON

*/
extern void MPX_vSet(uint8_t ucIdentifier, bool bOnOff);


/** Returns a point in the matrix

This Function returns the point with ID \<ucIdentifier\>.

\param ucIdentifier: \n
       Identifier of point to set\n
    \b Type: uint8_t\n
    \b Range: [0..MPX_COLUMN_COUNT*MPX_LINE_COUNT -1]

\return State of point \n
   \b Type: bool\n
   \b Range: 0 = OFF, all others = ON

*/

extern bool MPX_bGet(uint8_t ucIdentifier);

/** Toggles a point in the matrix

This Function toggles the point with ID \<ucIdentifier\>.

\param ucIdentifier: \n
       Identifier of point to set\n
    \b Type: uint8_t\n
    \b Range: [0..MPX_COLUMN_COUNT*MPX_LINE_COUNT -1]

*/
extern void MPX_vToggle(uint8_t ucIdentifier);


/** Set/resets a point in the matrix

This Function set/resets the point in column \<ucColumn\> and line \<ucLine\>.

\param ucColumn: \n
       Column of point to set\n
    \b Type: uint8_t\n
    \b Range: [0..MPX_COLUMN_COUNT-1]

\param ucLine: \n
       Line of point to set\n
    \b Type: uint8_t\n
    \b Range: [0..MPX_COLUMN_LINE-1]

\param bOnOff: \n
       New state for point\n
   \b Type: bool\n
   \b Range: 0 = OFF, all others = ON

\note It is recommended to use function #MPX_vSet().\n
      This will enhance SW design and saves some byte code.
      Normally upper layers need not know somthing about columns and lines.
*/
extern void MPX_vSetPoint(uint8_t ucColumn, uint8_t ucLine, bool bOnOff);

/** Returns a point in the matrix

This Function returns the point in column \<ucColumn\> and line \<ucLine\>.

\param ucColumn: \n
       Column of point to read\n
    \b Type: uint8_t
    \b Range: [0..MPX_COLUMN_COUNT-1]

\param ucLine: \n
       Line of point to read\n
    \b Type: uint8_t
    \b Range: [0..MPX_COLUMN_LINE-1]

\return State of point \n
   \b Type: bool\n
   \b Range: 0 = OFF, all others = ON

\note It is recommended to use function #MPX_bGet().\n
      This will enhance SW design and saves some byte code.
      Normally upper layers need not know somthing about columns and lines.

*/
extern bool MPX_bGetPoint(uint8_t ucColumn, uint8_t ucLine);


/** Toggles a point in the matrix

This Function toggles the point in column \<ucColumn\> and line \<ucLine\>.

\param ucColumn: \n
       Column of point to toggle\n
    \b Type: uint8_t\n
    \b Range: [0..MPX_COLUMN_COUNT-1]

\param ucLine: \n
       Line of point to toggle\n
    \b Type: uint8_t\n
    \b Range: [0..MPX_COLUMN_LINE-1]

\note It is recommended to use function #MPX_vToggle().\n
      This will enhance SW design and saves some byte code.
      Normally upper layers need not know somthing about columns and lines.

*/
extern void MPX_vTogglePoint(uint8_t ucColumn, uint8_t ucLine);


/** Copies coplete dataset to the multiplex buffer

This function replaces hole multiplex buffer with content of buffer \<pucBuffer\>.\n

\param pucBuffer: \n
       Address of source buffer to bo copied to multiplex buffer\n
    \b Type: const uint8_t*


*/
extern void MPX_vSetMpxBuffer(const uint8_t* pucBuffer);


/** Returns address of internal data buffer


\return Address of multiplexer buffer\n
    \b Type: const uint8_t*

*/
extern const uint8_t* MPX_pucGetMpxBuffer(void);


/** Clears MPX buffer
  
  This function clears the hole MPX buffer, it may be used for power management.
*/
extern void MPX_vClearMpx(void);


/** Returns the status of Multiplexer
    
    This function returns true if the internal countown is elapsed.\n
    Use this to handle powermangement, or for input multiplexers.

    \return 0 for not ready, all others for ready

    \sa MPX_vSetCountDown()
*/
extern bool MPX_bIsReady(void);


/** Sets the value of the countdown
    
    This function sets the value of the countdown.\n
    The Function MPX_bIsReady will return FALSE until \<ucValue\> interrupts happens.\n
    Use this to handle powermangement, or for input multiplexers.



\note The multiplexer interrupt is responsible to count down #MPX_ucCountDown.

    \sa MPX_bIsReady()
*/
extern void MPX_vSetCountDown(uint8_t ucValue);


/** Returns the result from input multiplexer

This function returns the result of the multiplexed input.

\return multiplexed input\n
        \b Type: TmpxOut

\note This function has to be implemented by application, the library just fixes the interface.
      It needs some (application specific) time until the result of input multiplexer is stable.
      Use the function MPX_bIsReady() to solve this problem.
*/
extern TmpxOut MPX_tGetInput(void);

#ifdef __cplusplus
}
#endif

#endif

