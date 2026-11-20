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
*   COMP_ABBREV      ASSERT
*******************************************************************************/


#ifndef __ASSERTXS_H
#define __ASSERTXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file AssertXS.h
    published application specific definitions and declarations for Assert                                 
 */

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** Number of assert loggings. 
\remarks
   - This must be suitably set by the program using the asserts functionality. 
   - The number of entries is by default expected to be less or equal to 256 entries */
#define ASSERT_LOG_NUM         4u
/** 
   Maximal filename length allowed by this project. 
\remarks 
   - This size can be suitably defined by any project using the assert functionality for debugging.
*/
#define ASSERT_MAXLEN_FILENAME        15u



#ifdef __cplusplus
}
#endif

#endif

