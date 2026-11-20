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


#ifndef BSH_STDINC_H
#define BSH_STDINC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief BSH Standard include file.
 *  This file shall be included by all compilation units
 */


#include "LibDefines.h"

#include LD_STRINGIZE(VARIANT.h) // include the header for current variant
#include "ped_fw.h"              // standard include for PED Framework

/* General definition to provide manual mocking for off-target unit test */
#ifndef MOCK_IT
    #define MOCKI(function)  (function)
    #define MOCKE(function)  (function)
#else
    #define MOCKI(function)  (int_mock.function)
    #define MOCKE(function)  (ext_mock.function)
#endif

#ifdef __cplusplus
}
#endif

#endif
