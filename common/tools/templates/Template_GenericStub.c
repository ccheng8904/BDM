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
 *  COMP_ABBREV      TM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    This file defines the so called 'function anchor array'
*             that is needed to prompt the linker
*             to include the functions under test.
*             if linking for the dynamic SW unit test.
*
*   \details  The anchor array contains a function pointer to every function
*             that is defined in any unit under test i.e. to every function that
*             is potentially going to be tested in the dynamic SW unit test.
*             If the project is linked for the dynamic SW unit test
*             these functions won't be called by the scheduler i.e.
*             they would be interpreted as not used and therefore not linked.
*             To make the linker interpret them as used (and therefore link them)
*             if the project is linked for the dynamic SW unit test
*             the anchor array is used in the TM_vInit function
*             called by BASIC_vUsrInitAppl.
*
*/


#include "bsh_stdinc.h"


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/


/******************************************************************************/
/* EXTERNAL ANCHOR DECLARATIONS                                               */
/******************************************************************************/
<$AllExtAnchorDeclarations>


/******************************************************************************/
/* ANCHOR FOR TEST POINT ENUMERATION                                          */
/******************************************************************************/
//volatile T_TM_TestPointsEnum TM_tDummy;

#ifdef TESTCLIENT_FOR_TARGET
#include "hsup.h"
void ITBL_vResetHandler(void)
{
    TM_vInit();
    HSUP_vGenerateReset();
}
#endif
/******************************************************************************/
/* ANCHOR ARRAY DEFINITION                                                    */
/* This array references the function pointer arrays of units under test      */
/* to prevent optimisation.                                                   */
/******************************************************************************/

T_TM_Anchor const * TM_aptAnchor[] =
{
<$GlobalAnchor>
    (T_TM_Anchor*)(0x00)
};
