/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          Generic SW
*   COMP_ABBREV      JOG
*******************************************************************************/

#ifndef JOGLIBXS_H
#define JOGLIBXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file joglibXS.h
 *  \brief Application specific definitions and declarations of joglib.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/


/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/

/** \brief project specific dependencies
 *
 *  Application specific dependencies may be added: 
 *  \code #define DEP_JOG  DEPLIB_JOG  MOD_EEP, MOD_DLL, SCH_END_MARK \endcode
 */
#define DEP_JOG DEPLIB_JOG SCH_END_MARK


#ifdef __cplusplus
}
#endif

#endif

