/********************************************************************************
*   PROJECT          PP PED-Framework
*******************************************************************************/
#ifndef STDDEF_H
#define STDDEF_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *    This file exists to provide code analyser tools with standard library
 *    declarations in ANSII conforming form 
 *    (to inhibit unnecessary  error messages).
 */

#ifndef NULL
    #define NULL ((void *)0u)
#endif

//lint -emacro(413,offsetof)
//lint -emacro(923,offsetof)
#define offsetof(type, member) ((size_t)&(((type *)0)->member)) //lint !e9026 function-like macro

typedef unsigned char wchar_t;

#if defined (UTEST_TYPE_SYSTEM_KEIL_ARM)
    typedef unsigned int size_t;
    typedef unsigned int wint_t;

#elif defined (UTEST_TYPE_SYSTEM_Windows32)
    typedef long unsigned int size_t;
    typedef long unsigned int wint_t;

#elif defined (UTEST_TYPE_SYSTEM_RENESAS_RX)
    typedef unsigned int size_t;
    typedef unsigned int wint_t;

#endif

#endif
