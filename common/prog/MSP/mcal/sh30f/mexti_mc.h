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
 *  COMP_ABBREV      MEXTI
 *  PROCESSOR        SH30F9X71
 ******************************************************************************/


#ifndef MEXTI_MC_H
#define MEXTI_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mexti_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MEXTI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mexti_types.h"
#include "mcal/mdio.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

// Multiple MEXTI handle instances share the same IRQ
#define MEXTI_MULTIPLE_HANDLES_PER_IRQ


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MEXTI_IRQ_Index
{
    MEXTI0_IRQ_INDEX,
    MEXTI1_IRQ_INDEX,
    MEXTI2_IRQ_INDEX,
    MEXTI3_IRQ_INDEX,
    MEXTI9_4_IRQ_INDEX,
    MEXTI15_10_IRQ_INDEX,
    MEXTI_IRQ_CNT
};


struct MEXTI_Channel
{
    const struct MDIO_Channel   *mextiPin;
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

extern void MEXTI_configureTrigger(const struct MEXTI_Channel *channel, enum MEXTI_Trigger trigger);


#ifdef __cplusplus
}
#endif

#endif // MEXTI_MC_H
