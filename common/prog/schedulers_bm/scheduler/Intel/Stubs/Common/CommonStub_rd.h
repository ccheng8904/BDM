/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PTEng
*   COMP_ABBREV      Common
*******************************************************************************/


#ifndef COMMON_RD_STUB_H
#define COMMON_RD_STUB_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Function call redirection for dynamic SW unit test
*             for all undefined functions
*
*/


/******************************************************************************/
/* PUBLIC STUB FUNCTION DECLARATIONS resp.                                    */
/* REDIRECTION OF FUNCTION CALLS TO STUB CALLS                                */
/******************************************************************************/


// Isolate units that are under test, i.e.
//  1)  if any other (='foreign') translation is under test and if this unit shall be stubbed: prevent the other (='foreign') unit from using (=calling) this unit by stubbing this unit's interface
#if (defined (UNIT_UNDER_TEST)) && (defined (FOR_UNIT_UNDER_TEST_STUB_COMMONSTUB))
    #define APPLY_STUBBING_COMMONSTUB_TEST
#endif

#if defined (APPLY_STUBBING_COMMONSTUB_TEST)
    		#define TIM_u32GetCircleMicroSeconds TIM_u32GetCircleMicroSeconds_Stub
		#define UTI_bGetBit UTI_bGetBit_Stub
		#define UTI_vSetBit UTI_vSetBit_Stub
		#define BAL_handleTask BAL_handleTask_Stub
		#define DBPL_handleTask DBPL_handleTask_Stub
		#define DLL_handleTask DLL_handleTask_Stub
		#define TIM_handleTask16 TIM_handleTask16_Stub
		#define WDT_handleTask WDT_handleTask_Stub

#endif




#ifdef __cplusplus
}
#endif

#endif // COMMON_RD_STUB_H
