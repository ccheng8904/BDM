/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      HWVT
*   PROCESSOR        SH30F9X71
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     weakvectbl.c
*   \brief    Implementation of the HAL Weak Vector Table.
*/
 
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "weakvectbl.h"
#include "IntTblArmCM.h"


void HWVT_vSpuriousIsr(void)
{
    ITBL_vSpuriousIsr(); // call application function  
}
