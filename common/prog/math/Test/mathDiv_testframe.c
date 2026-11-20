/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      CRC
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/* Testframe for stdCrc unit */
#include "bsh_stdinc.h"
#include "hsup.h"
#include "math/mathDiv.c" // include the unit under test



const T_TM_Anchor MTDIV_atTestFuncPtr[] = 
{
    (T_TM_Anchor)MTDIV_div_32_16

};

extern const T_TM_Anchor TM_TEST_SupportFuncPtr[];


T_TM_Anchor const * TM_aptAnchor[] =
{
        MTDIV_atTestFuncPtr,
        TM_TEST_SupportFuncPtr
};

#ifndef __NOT_TARGET_CC__
void ITBL_vResetHandler(void)
{
    TM_vInit();
    HSUP_vGenerateReset();
}
#endif
