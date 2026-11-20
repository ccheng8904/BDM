/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMPILER         GccArm
 ******************************************************************************/

#ifndef ANSIEXT_H
#define ANSIEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     AnsiExt.h
 *
 *  \brief    Non ANSI C keyword defines.
 *
 *  \details  This file contains defines for non-Compiler tools (e.g. Lint, Axivion, dependency target)
 *            that remove non-Ansi keywords of target compiler.
 *
 */

#if defined (__NOT_CC__) || defined (__NOT_TARGET_CC__)
/* Switch off: Note 9051: macro 'xx' defined with the same name as a C keyword
               yes: that's the intention of the definitions in this file
 */
/*lint -save -e9051 */
    #define __irq
    #define __asm__(x)
    #define __attribute__(x)
    #define __inline inline
/*lint -restore -e9051 */
#else
    // Translate Keil __irq to GCC style
    #define __irq   __attribute__ ((interrupt))

#endif // defined (__NOT_CC__) || defined (__NOT_TARGET_CC__)



#ifdef __cplusplus
}
#endif

#endif // ANSIEXT_H

