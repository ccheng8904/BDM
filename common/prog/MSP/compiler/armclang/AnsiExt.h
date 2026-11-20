/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PED Framework
 *  COMPILER         KeilArm
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
/* Note 9051: macro '__XXX' defined with the same name as a C keyword [MISRA 2012 Rule 20.4, required]
   False positive, the following names are NOT C-Keywords!!
 */
//lint -save -e9051
    #define __irq
    #define __attribute__(x)
    #define __attribute(x)
    #define __asm(x)
    #define __packed
    #define __align(x)
    #define __int64
    #define __swi(x)
    //RVCT v3.0 renamed the SWI instruction to SVC. The inline assembler still accepts SWI in place of SVC //
    #define __svc(x)
    #define __INTADDR__(x)
    #define __svc_indirect(x)
    #define __svc_indirect_r7(x)
    #define __value_in_regs
    #define __forceinline
    #define __pure
    #define __weak
    #define __glaobal_reg(x)
    #define __softfp
    #define __writeonly
    #define __smc(x)
    #define __inline inline
    #define register
//lint -restore -e9051
    #define ped_static_inline  static   inline            ///< inline keyword for other C99 Compiler



#endif // defined (__NOT_CC__) || defined (__NOT_TARGET_CC__)

#ifdef __cplusplus
}
#endif

#endif // ANSIEXT_H

