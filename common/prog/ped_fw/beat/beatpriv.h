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

#ifndef BEATPRIV_H
#define BEATPRIV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief Private definitions and declarations of beat library
 *         (needed for configuration)
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"



/** Periodicity of beat counter

This is the periode of the beat counter. 
The beat counter counts from 0 to #BEAT_ucBeatCount and starts again with zero.\n
The content of beat counter is returned with function BEAT_ucGetBeat().\n
\n
\b Range: 1..255\n
\n
\b Default: 4\n

\note This constant may be overwritten!

\sa BEAT_ucGetBeat()

*/
extern const uchar BEAT_ucBeatCount;



/** Time for one tick of beat counter

This is the time for one tick of the beat counter.\n
The default value of 50 with the default timer segment of BEAT_ucBeatTimer
causes a time of 500ms for each tick of the beat counter.

\n
\b Range: 2..255\n
\n
\b Default: 50 (500ms)\n
\b Scale: Depends on timer segment of BEAT_ucBeatTimer (default = 10ms)

\note This constant may be overwritten!\n
      Because of accuracy of SW-timers (+0/-1 tick) this value should not be too small.\n
      A value of 10 has accuracy of +0/-10%.

\sa BEAT_ucGetBeat()

*/
extern const uchar BEAT_ucTime;


#ifdef __cplusplus
}
#endif

#endif
