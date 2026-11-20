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
 *  COMP_ABBREV      STE
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief Application specific implementation / configuration for stateeng lib
 *
 */

#include "bsh_stdinc.h"
#include "stateeng.h"


#define STE_HISTORY_DEPTH 8



const uchar  STE_ucHistoryDepth = STE_HISTORY_DEPTH;
const void  *STE_pvHistoryTable[STE_HISTORY_DEPTH];

