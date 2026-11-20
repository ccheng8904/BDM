/*******************************************************************************
 *  Copyright (c) 2021 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCU Framework
 *  COMP_ABBREV      __ASM
 ******************************************************************************/

#ifndef FS_ASM_MACROS_H
#define FS_ASM_MACROS_H

#if defined(KEILARM)

#ifndef   __ASM_LABEL
  #define __ASM_LABEL(label)       label
#endif

#ifndef   __ASM_DIRECTIVE1
  #define __ASM_DIRECTIVE1  require8
#endif

#ifndef   __ASM_DIRECTIVE2
  #define __ASM_DIRECTIVE2  preserve8
#endif

#ifndef   __ASM_DIRECTIVE_THUMB
  #define __ASM_DIRECTIVE_THUMB  thumb
#endif

#ifndef   __ASM_DIRECTIVE_ALIGN
  #define __ASM_DIRECTIVE_ALIGN(alignVal)  area |.text|, code, readonly, align=alignVal
#endif

#ifndef   __ASM_FUNCTION_L1
#define __ASM_FUNCTION_L1(funName) 
#endif

#ifndef   __ASM_FUNCTION_L2
#define __ASM_FUNCTION_L2(funName) 
#endif

#ifndef   __ASM_FUNCTION_L3
#define __ASM_FUNCTION_L3 
#endif

#ifndef   __ASM_FUNCTION
  #define __ASM_FUNCTION(funName)  funName proc
#endif

#ifndef   __ASM_END_FUNCTION
  #define __ASM_END_FUNCTION(funName)  endp
#endif

#ifndef   __ASM_END_FILE
  #define __ASM_END_FILE  end
#endif

#ifndef   __ASM_MACRO_DEF
  #define __ASM_MACRO_DEF(name,value)   name       equ     value
#endif

#ifndef   __ASM_IMPORT
  #define __ASM_IMPORT(name) import name
#endif

#ifndef   __ASM_EXPORT
  #define __ASM_EXPORT(name) export name
#endif

#elif defined (GCCARM) || defined (ARMCLANG)

#ifndef   __ASM_LABEL
  #define __ASM_LABEL(label)       label:
#endif

#ifndef   __ASM_DIRECTIVE1
  #define __ASM_DIRECTIVE1   .syntax unified
#endif

#ifndef   __ASM_DIRECTIVE2
  #define __ASM_DIRECTIVE2  .text
#endif

#ifndef   __ASM_DIRECTIVE_THUMB
  #define __ASM_DIRECTIVE_THUMB  .thumb
#endif

#ifndef   __ASM_DIRECTIVE_ALIGN
  #define __ASM_DIRECTIVE_ALIGN(alignVal)  .align alignVal
#endif

#ifndef   __ASM_FUNCTION_L1
  #ifdef ARMCLANG
    #define __ASM_FUNCTION_L1(funName) 
  #else
    #define __ASM_FUNCTION_L1(funName) .func funName, funName
  #endif
#endif

#ifndef   __ASM_FUNCTION_L2
    #define __ASM_FUNCTION_L2(funName) .type funName, "function"
#endif

#ifndef   __ASM_FUNCTION_L3
#define __ASM_FUNCTION_L3 .thumb_func
#endif

#ifndef   __ASM_FUNCTION
  #define __ASM_FUNCTION(funName) funName:
#endif

#ifndef   __ASM_END_FUNCTION
  #ifdef ARMCLANG
    #define __ASM_END_FUNCTION(funName)   .L##funName##_end0: \
    .size funName, .L##funName##_end0-##funName \

  #else
    #define __ASM_END_FUNCTION(funName)   .endfunc \

  #endif
#endif

#ifndef   __ASM_END_FILE
  #define __ASM_END_FILE   .end
#endif

#ifndef   __ASM_MACRO_DEF
  #define __ASM_MACRO_DEF(name,value)   .set name,  value
#endif

#ifndef   __ASM_IMPORT
  #define __ASM_IMPORT(name) .extern name
#endif

#ifndef   __ASM_EXPORT
  #define __ASM_EXPORT(name) .global name
#endif

#else
  # error "Unsupported compiler"
#endif

#if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS)
#ifndef   __ASM_PUSH
  #define __ASM_PUSH push  
#endif

#ifndef   __ASM_POP
  #define __ASM_POP pop 
#endif
#else

#ifndef   __ASM_PUSH
  #define __ASM_PUSH stmdb  sp!,
#endif

#ifndef   __ASM_POP
  #define __ASM_POP ldmia  sp!,
#endif

#endif
#endif //FS_ASM_MACROS_H