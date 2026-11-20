/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          SW_LIB
*   COMP_ABBREV      LPAN
*******************************************************************************/

#ifndef __LOGPANXS_H
#define __LOGPANXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
 /** \file logpanXS.h
   published application specific definitions and declarations for logical panel

   */

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/** Dependencies of library after configuration

If the configuration changes the dependencies of logical panel, this shall adapted here.\n
The following example add a dependency to module EEP:\n
\code
#define DEP_MPX     DEPLIB_LPAN  MOD_EEP, SCH_END_MARK
\endcode
\note There is no comma after DEPLIB_LPAN
*/
#define DEP_LPAN     DEPLIB_LPAN MOD_TIM, SCH_END_MARK

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
enum{
     EV_KEY_FREEZER=  LPAN_FIRST_LOGICAL_EVENT,
     EV_KEY_FREEZER_RELEASE,
     EV_KEY_SUPER_FREEZER,
     EV_KEY_COOLER,
     EV_KEY_COOLER_RELEASE,
     EV_KEY_SUPER_COOLER,
     EV_KEY_ALARM,
     EV_CHILLER_GRIP,
     EV_CHILLER_GRIP_RELEASE,
     EV_SALES_ROOM_GRIP,
     EV_TEST_GRIP,
     EV_TEST_GRIP_RELEASE,
     EV_SELF_TEST_GRIP,
     EV_SELF_TEST_GRIP_RELEASE,
     EV_FIRST_NON_KEY_EVENT
};


#ifdef __cplusplus
}
#endif

#endif


