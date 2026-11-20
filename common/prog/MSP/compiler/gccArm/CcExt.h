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
 *  COMPILER         GCC ARM
 ******************************************************************************/

#ifndef CCEXT_H
#define CCEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* This header defines target specific definitions, which are predefined
 * by the target compiler, for other parsers as well.
 * Other parsers may be doxygen, static code analysers, ...
 */

#if defined (__NOT_CC__) || defined (__NOT_TARGET_CC__)
/* Definitions for NON target compilers */

    #define __arm__       //lint !e9046  ('Typographical ambiguity...', macro name defined by Keil cannot be changed)
    #define __GNUC__ 4
    #define __GNUC_MINOR__ 0

    /* for common limits.h */
    #define __CHAR_IS_SIGNED__
    #define __CHAR_IS_8BIT__
    #define __SHORT_IS_16BIT__
    #define __INT_IS_32BIT__
    #define __LONG_IS_32BIT__

#endif

#ifdef __cplusplus
}
#endif

#endif
