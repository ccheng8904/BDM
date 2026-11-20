/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MDAC
 ******************************************************************************/


#ifndef MDAC_TYPES_H
#define MDAC_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdac_types.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and type definitions for unit MDAC.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "mcal_types.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MDAC_Trigger
{
    MDAC_TRIGGER_OFF,
    MDAC_TRIGGER_SOFTWARE,
    MDAC_TRIGGER_HARDWARE
};


struct MDAC_Config
{
    enum MDAC_Trigger       trigger;
};

#ifdef __cplusplus
}
#endif

#endif // MDAC_TYPES_H
