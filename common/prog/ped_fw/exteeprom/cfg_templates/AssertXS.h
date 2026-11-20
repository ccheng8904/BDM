/*****************************************************************************************************************
*   Copyright (C) 2016 BSH Hausgeraete GmbH, Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
******************************************************************************************************************
*   PROJECT          Generic SW
*   MODULE-PREFIX    ASSERT
*   AUTHOR           emka
*   CREATED          2006-02-27
******************************************************************************************************************
*   %PCMS_HEADER_SUBSTITUTION_START:%
*   ARCHIVE          %PP:%:%PI:%
*   PRESENT REVISION %PR:%
*   STATUS           %PS:%
*   %PCMS_HEADER_SUBSTITUTION_END:%
*****************************************************************************************************************/


/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file AssertXS.h
    published application specific definitions and declarations for Assert                                 
 */

#ifndef __ASSERTXS_H
#define __ASSERTXS_H

#ifdef __cplusplus
extern "C" {
#endif

/* DEFINITIONS ****************************************************************************************/
/** Number of assert loggings. 
\remarks
   - This must be suitably set by the program using the asserts functionality. 
   - The number of entries is by default expected to be less or equal to 256 entries */
#define ASSERT_LOG_NUM         4
/** 
   Maximal filename length allowed by this project. 
\remarks 
   - This size can be suitably defined by any project using the assert functionality for debugging.
*/
#define ASSERT_MAXLEN_FILENAME        15



#ifdef __cplusplus
}
#endif

#endif

