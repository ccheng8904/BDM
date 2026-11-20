/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MPCM
 ******************************************************************************/


#ifndef MPCM_TYPES_H
#define MPCM_TYPES_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and type definitions for module MPCM.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MPCM_PowerMode
{
    MPCM_PMODE_RESET    = 0,    ///< Microcontroller is after reset
    MPCM_PMODE_RUN      = 1,    ///< Normal run mode - clocks are set to standard values
    MPCM_PMODE_LPRUN    = 2,    ///< Low-power run - clocks are lowered, some peripherals may be disabled
    MPCM_PMODE_SLEEP    = 3,    ///< Core is in SLEEP state, almost all peripherals are disabled, RAM retains values
    MPCM_PMODE_STOP     = 4     ///< Core is in STOP state, almost all peripherals are disabled, RAM may not retain values
};


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#endif // MPCM_TYPES_H
