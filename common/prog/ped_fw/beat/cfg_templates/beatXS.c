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
 *   MODULE-PREFIX    BEAT
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief Application specific implementation / configuration for beat library
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "beat.h"
#include "beatpriv.h"
#include "SegmentDef.h"



#define BEAT_COUNT 4
#define BEAT_TIME  50//500ms


// external defined constants used in library
const uint8_t BEAT_ucBeatCount = BEAT_COUNT;
const uint8_t BEAT_ucTime      = BEAT_TIME;
