/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      SYMBOL
 ******************************************************************************/


#ifndef STD_SYMBOL_H
#define STD_SYMBOL_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  standardLibrary
 *
 *  \brief    Public compiler attributes  definitions.
 *
 *  \details  This file holds compiler dependent definitions for attributes.
 *            Currently supported compilers: ARM compiler 4, 5 and 6 (armclang),
 *            GCC (GNU) and IAR.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/*
 * Intel Compiler
 */
#if defined(__NOT_CC__)
    #define SYMBOL_USED
    #define SYMBOL_WEAK
    #define SYMBOL_PACKED
    #define SYMBOL_NO_RETURN    _Noreturn
    #define SYMBOL_ASM
    #define SYMBOL_NO_INIT
    #define SYMBOL_IRQ


/*
 * ARM Compiler 4/5
 */
#elif defined (__CC_ARM)
    #define SYMBOL_USED         __attribute__((used))
    #define SYMBOL_WEAK         __attribute__((weak))
    #define SYMBOL_PACKED       __attribute__((packed))
    #define SYMBOL_NO_RETURN    __declspec(noreturn)
    #define SYMBOL_ASM          __asm
    #define SYMBOL_NO_INIT      __attribute__((section("NO_INIT"),zero_init))
    #define SYMBOL_IRQ

/*
 * ARM Compiler 6 (armclang)
 */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #define SYMBOL_USED         __attribute__((used))
    #define SYMBOL_WEAK         __attribute__((weak))
    #define SYMBOL_PACKED       __attribute__((packed))
    #define SYMBOL_NO_RETURN    __attribute__((noreturn))
    #define SYMBOL_ASM          __asm
    #define SYMBOL_NO_INIT      __attribute__((section( ".bss.NO_INIT")))
    #define SYMBOL_IRQ

/*
 * GNU Compiler
 */
#elif defined (__GNUC__)
    #define SYMBOL_USED         __attribute__((used))
    #define SYMBOL_WEAK         __attribute__((weak))
    #define SYMBOL_PACKED       __attribute__((packed, aligned(1)))
    #define SYMBOL_NO_RETURN    __attribute__((noreturn))
    #define SYMBOL_ASM          __asm
    #define SYMBOL_NO_INIT      //tbd. maybe __attribute__((section(".noinit")))
    #define SYMBOL_IRQ

/*
 * IAR Compiler
 */
#elif defined (__IAR_SYSTEMS_ICC__)
    #define SYMBOL_USED         __root
    #define SYMBOL_WEAK         __weak
    #define SYMBOL_PACKED       __packed
    #define SYMBOL_NO_RETURN    __noreturn
    #define SYMBOL_ASM          __asm
    #define SYMBOL_NO_INIT      __no_init

#else
    #error "Unsupported compiler"
#endif


#ifdef __cplusplus
}
#endif

#endif // STD_SYMBOL_H
