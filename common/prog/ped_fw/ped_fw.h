/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      --
 ******************************************************************************/


#ifndef PED_FW_H
#define PED_FW_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Standard include file for PED-Framework
 */


#include "AnsiExt.h" 
#include "CcExt.h"  
#if !defined(UNIT_TESTFRAMING)
#include "testSupport/TestSupport.h"
#elif !defined (VC_DYNT_TEST)
#include "testSupport/vcast_test_support.h"
#endif


#ifdef __cplusplus
}
#endif

#endif
