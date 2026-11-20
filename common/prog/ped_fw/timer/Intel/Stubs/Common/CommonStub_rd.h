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
            #define TIM_uiGetCircleMicroSeconds TIM_uiGetCircleMicroSeconds_Stub
        #define _TIM_bGetInitConfig _TIM_bGetInitConfig_Stub
        #define STIM_SchedulerHandleTask STIM_SchedulerHandleTask_Stub
		#define HSUP_vEnableInt HSUP_vEnableInt_Stub
        #define HSUP_vDisableInt HSUP_vDisableInt_Stub
		#define STIM_InitModule STIM_InitModule_Stub

#endif




#ifdef __cplusplus
}
#endif

#endif // COMMON_RD_STUB_H
