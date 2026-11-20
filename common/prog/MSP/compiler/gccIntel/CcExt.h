/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          SW_LIB_TEST_TESTER_LIB
*   COMP_ABBREV      -
*******************************************************************************/


#ifndef CCEXT_H
#define CCEXT_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \ingroup  PEDF
*
*   \brief    Empty CcExt.h file for building and unit testing
*             PC implementations of libraries.
*
*   \details  CcExt.h files define compiler intrinsic defines
*             for non target compiler tools.
*             Since the target platform for code that uses this file
*             is the PC where the MinGW is used nothing has to be done
*             therefore the file is empty (but existing because required by
*             PED Framework design).
*
*/




#ifdef __cplusplus
}
#endif

#endif // CCEXT_H
