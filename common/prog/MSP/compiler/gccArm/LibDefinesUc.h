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

#ifndef LIBDEFINESUC_H
#define LIBDEFINESUC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    This header makes some platform specific definitions
 *
 */


#ifdef __NOT_CC__

    #define ROOT
    #define PACKED   //lint !e9046  ('Typographical ambiguity...', historic macro name cannot be changed)
    #define POSTPACKED

    typedef unsigned int  TbitField; ///< uses this definition for bitfield definitions instead of int!

#else

    #define ROOT         __attribute__((unused))
    #define PACKED
    #define POSTPACKED   __attribute__((packed))

    typedef unsigned int  TbitField; ///< uses this definition for bitfield definitions instead of int!

#endif

#ifdef __cplusplus
}
#endif

#endif
