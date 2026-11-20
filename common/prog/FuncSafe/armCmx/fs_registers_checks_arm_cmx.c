/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          GED-Framework NG
 *  PROCESSOR        ARM CortexMx
 *  COMP_ABBREV      FSF
 *
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of CortexMx specific register check functions.
 *
 *  \details  This file contains the Cortex-Mx CPU test to be done
 *            at start-up and during run time.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "FuncSafe/fSafeXS.h"

/* functions implemented in assembler */
extern void FSF_do_xPSRandR14LinkRegisterCheck(void);
extern void FSF_doGeneralPurposeR0_R12RegisterCheck(void);
extern void FSF_doStackPointerR13RegisterCheck(void);


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/** \brief Implementation of StartUp Register check
 *
 * This implementation is based on Self Test Library from STMicroelectronics
 * stm32f0xx_STLcpustartKEIL.s file V1.0.0 from February 2012
 *
 * \param none
 * \return none
 *
 */
void FSF_vRegistercheck(void)
{
    FSF_do_xPSRandR14LinkRegisterCheck();

    FSF_doGeneralPurposeR0_R12RegisterCheck();

    FSF_doStackPointerR13RegisterCheck();
}

/** \brief Implementation of RunTime Register check
 *
 * This implementation is based on Self Test Library from STMicroelectronics
 * stm32f0xx_STLcpurunKEIL.s file V1.0.0 from February 2012
 *
 * \param none
 * \return none
 *
 */
void FSF_vRegOpercheck(void)
{
    FSF_doGeneralPurposeR0_R12RegisterCheck();
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
