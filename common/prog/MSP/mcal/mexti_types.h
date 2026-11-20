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
 *  COMP_ABBREV      MEXTI
 ******************************************************************************/


#ifndef MEXTI_TYPES_H
#define MEXTI_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and type definitions for unit MEXTI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


enum MEXTI_Trigger
{
    MEXTI_TRIGGER_DISABLE,
    MEXTI_TRIGGER_RISING,
    MEXTI_TRIGGER_FALLING,
    MEXTI_TRIGGER_RISING_FALLING,
    MEXTI_TRIGGER_LOW_LEVEL,
    MEXTI_TRIGGER_HIGH_LEVEL
};


struct MEXTI_Config
{
    enum MEXTI_Trigger  trigger;
};


#ifdef __cplusplus
}
#endif

#endif // MUART_TYPES_H
