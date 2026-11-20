/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *   PROJECT          Generic SW
 *   COMP_ABBREV      BEAT
 ******************************************************************************/

#ifndef BEAT_H
#define BEAT_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *   \brief  Public definitions and declarations of beat library
 *
 *
 * \section BeatIntro 1. Introduction
 * 
 * The Beat Library is used to generate a configurable beat for an electronic control.
 * Especially for panel modules it may be used to syncronice different kind of blinking.
 * SW-modules which even does not know each other, may blink syncronously.
 * So blinking of text and LEDs may be synchroniced by an underlaying layer.
 * 
 * 
 * \section BeatInterface   2. Library Interface
 * 
 * The Beat-Library has a quite marrow application interface
 * 
 * <TABLE>
 * <TR><TD><b>Function</TD>  <TD>Description</b></TD></TR>
 * <TR><TD>   BEAT_ucHandleTask() </TD><TD> Task handler of beat library </TD></TR>
 * <TR><TD>   BEAT_ucGetBeat()    </TD><TD> Returnes the current beat counter.\n
 *                                          The Beat counter counts from 0 to  #BEAT_ucBeatCount-1 and starts
 *                                          again from zero.</TD></TR>
 * </TABLE>
 * 
 * 
 * \image html BeatIntro.gif "Periodicity of Beat Counter (BEAT_ucBeatCounter) returned by  BEAT_ucGetBeat()"
 * 
 * \section BeatConfig   3. Library Configuration
 * 
 * The configuration of beat library very easy.
 * First of all the value of #BEAT_ucBeatCount may be changes, the default value is 4.\n
 * 
 * 
 * <TABLE>
 * <TR><TD><b>Function</TD>  <TD>Description</b></TD></TR>
 * <TR><TD>   #BEAT_ucBeatCount </TD><TD> Periode of internal beatcounter.\n
 *                                        The beat counter counts from 0 to #BEAT_ucBeatCount-1.\n
 *                                        \b Default: 4</TD></TR>
 * 
 * <TR><TD>   #BEAT_ucTime      </TD><TD> Appoints the frequency of the beat counter.\n
 *                                        \b Default: 50 (500ms for one tick of beat counter)</TD></TR>
 * <TR><TD>   BEAT_ucBeatTimer  </TD><TD> SW-Timer of beat library. The timer segment may be changed.\n
 *                                        \b Default: 10ms Timer\n
 *                                        \n
 *                                        \note The timer has no declaration in beatpriv.h to prevent warning:\n
 *                                               "Different segments in declaration and definition"
 * </TD></TR>
 * </TABLE>
 * 
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"


#define DEP_BEAT MOD_TIM, SCH_END_MARK///< Dependencies of beat library (old style definitions used by scheduler)
#define DEPLIB_BEAT MOD_TIM, ///< Dependencies of beat library (definitions used by scheduler)


/** Task handler of beat library. 

The Task has to be called until #TASK_TERMINATE is returned.\n
The library is initialised when function returns #TASK_INITIALISED or #TASK_TERMINATE.

\return Current state of library (info/command for scheduler)\n
  \b Type: uint8_t\n
  \b Range: [#TASK_NOT_INITIALISED, #TASK_INITIALISED, #TASK_TERMINATE]
*/

extern uint8_t BEAT_ucHandleTask(void);



/** Returns current value of beat counter

This interface returns the current value of beat counter.\n
The Beat counter counts from 0 to #BEAT_ucBeatCount-1 and starts again with 0.\n

\image html BeatIntro.gif "Periodicity of Beat Counter (BEAT_ucBeatCounter) returned by  BEAT_ucGetBeat()"

\return Value of beat counter\n
   \b Type: uint8_t\n
   \b Range: 0.. #BEAT_ucBeatCount-1
*/
extern uint8_t BEAT_ucGetBeat(void);


#ifdef __cplusplus
}
#endif

#endif

