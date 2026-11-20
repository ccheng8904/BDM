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
*   AUTHOR           Alexander Kiefl
*   CREATED          04.06.2014
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Global processor include for STMS32F0xx.
*/
#ifndef __PROCESSOR_H
#define __PROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsh_stdinc.h"

#if  defined(UNIT_TEST)
#warning "Processor derivative not defined, consider possibly required register-stubs."
#endif

#ifdef __cplusplus
}
#endif

#endif // __PROCESSOR_H

