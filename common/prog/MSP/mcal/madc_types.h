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
 *  COMP_ABBREV      MADC
 ******************************************************************************/


#ifndef MADC_TYPES_H
#define MADC_TYPES_H

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
 *  \brief    Public definitions and type definitions for unit MADC.
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

// enum MADC_ScanType
// {
//     MADC_SCAN_TYPE_SW,  ///< Scan is managed by software in interrupts
//     MADC_SCAN_TYPE_HW   ///< Scan is managed by peripheral (not supported on all MCUs)
// };


enum MADC_Trigger
{
    MADC_TRIGGER_SOFTWARE,
    MADC_TRIGGER_HARDWARE
};


enum MADC_Resolution
{
    MADC_RESOLUTION_16_BITS = 0u,
    MADC_RESOLUTION_14_BITS = 2u,
    MADC_RESOLUTION_12_BITS = 4u,
    MADC_RESOLUTION_10_BITS = 6u,
    MADC_RESOLUTION_8_BITS  = 8u,
    MADC_RESOLUTION_6_BITS  = 10u
};


/** MADC generic configuration structure */
struct MADC_Config
{
    enum MADC_Trigger       trigger;
    enum MADC_Resolution    resolution;
    uint16_t                conversionTimeout;
};


#ifdef __cplusplus
}
#endif

#endif // MADC_TYPES_H
