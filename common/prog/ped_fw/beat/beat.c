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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/**  \file
 *   \brief  Implementation of beat library
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "beat.h"
#include "beatpriv.h"
#include "SegmentDef.h"


static uint8_t BEAT_ucBeatCounter;///< Beat counter is returned by BEAT_ucGetBeat()

//lint -esym(9003,BEAT_ucBeatTimer) could define variable 'BEAT_ucBeatTimer' , advisory
SDEF_SetSegmentRW(TIMER8_10MS)
static SDEF_NO_INIT uint8_t BEAT_ucBeatTimer;// The timing of beat library based on this counter
SDEF_SetSegmentRW_Default()


uint8_t BEAT_ucHandleTask(void)
{
   if (BEAT_ucBeatTimer == 0u)//if timer down
   {
      BEAT_ucBeatTimer = BEAT_ucTime;
      if (++BEAT_ucBeatCounter>=BEAT_ucBeatCount)//increment beat counter, if max beatcounter reached
      {
         BEAT_ucBeatCounter = 0;//start with zero
      }
   }
   return TASK_INITIALISED;
}


uint8_t BEAT_ucGetBeat(void)
{
   return BEAT_ucBeatCounter;
}
