/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      LPAN
 ******************************************************************************/

#ifndef LOGICPAN_H
#define LOGICPAN_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file 
 *
 *  \brief Public declarations and definitions of logpan library
 * 
 *  \section intro Introduction
 *  
 *  The logical panel library calculates logical events from a physical panel.\n
 *  The following kind of events are supported:\n
 *  The library handles single key events as well as multi key events
 *  
 *  \section Interface Interface of Logical Panel
 *  
 *  \section Config Configuration of Logical Panel
 *  
 *  For configuration the following functions must be defined by application:\n
 *
 *  - _LPAN_bIsKeyMask()
 *  - _LPAN_bIsTimerDown()
 *  - _LPAN_vLoadTimer()
 *  - _LPAN_ucMapGripEvent()
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>




/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
#define DEPLIB_LPAN     ///< dependencies of logical panel libraries (LPAN has no dependencies)

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
enum{
     LPAN_NO_EVENT,             ///< No event found
     LPAN_RELEASED_EV,          ///< event for a released key/grip
     LPAN_PRESSED_EV,           ///< event for a pressed key/grip
     LPAN_DELAY_EV,             ///< event for a delayed key/grip
     LPAN_REPEAT_EV,            ///< event for repeated key/grip
     LPAN_REPEAT_REL_EV         ///< event for a repeated/delayed released key/grip
};

#define LPAN_NUMBER_OF_EV    LPAN_REPEAT_REL_EV ///< Number of possible events (for each Key/grip)

#define LPAN_FIRST_LOGICAL_EVENT   (LPAN_NO_EVENT+1) ///< First logical event, which may be used by application
enum{
     LPAN_RELEASED_STATE, ///< State for a released key/grip
     LPAN_PRESSED_STATE,  ///< State for a pressed key/grip
     LPAN_REPEAT_STATE    ///< State for a repeated key/grip
};

#include "logpanxs.h" //upper definitions shall be known in this include!


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/** \brief Returns next present logical event
 *
 *  If no event was found #LPAN_NO_EVENT will be returned

 * \return Logical event\n
 * \b range: [#LPAN_NO_EVENT..0xFF]
 */
extern uint8_t LPAN_ucGetKeyEvent(void);

#ifdef __cplusplus
}
#endif

#endif

