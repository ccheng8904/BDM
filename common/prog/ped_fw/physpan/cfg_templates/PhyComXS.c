/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 * PROJECT          Generic SW
 * COMP_ABBREV      PHYP
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  Physpan
 *
 *  \brief    Implementation of PhyComXS.
 *
 *  \details  A detailed file description that may span several lines and
 *            that has to be terminated by an empty line.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "physpan.h"
#include "phyppriv.h"


/******************************************************************************/
/* PUPBLIC DATA DEFINITIONS                                                   */
/******************************************************************************/
Tpanel PHYP_tKeyState; ///< saves the key state, one bit for each key


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

Tpanel PHYP_tGetKeys(void)
{
   return PHYP_tKeyState;
}


