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
 ******************************************************************************/

#if defined(__ARMCC_VERSION) && !defined(__NOT_CC__)

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  retarget
 *
 *  \brief    Function definitions for ARM CC system calls retargeting.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "retarget.h"
#include <stdio.h>


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* RETARGET FUNCTION DEFINITONS                                               */
/******************************************************************************/


/* Turn off semihosting */
//#pragma import(__use_no_semihosting_swi)


__attribute__((weak)) int fputc(int ch, FILE *f)
{
    (void)f;
    RTG_writeChar((char)ch);
    return (ch);
}


__attribute__((weak)) int fgetc(FILE *f) 
{
    (void)f;
    return (int)RTG_readChar();
}


__attribute__((weak)) void _ttywrch(int ch)
{
    RTG_writeChar((char)ch);
}


__attribute__((weak,noreturn)) void __aeabi_assert(const char *expr, const char *file, int line) 
{
    RTG_assert(expr, file, line);
    while(1)
    {
        /* Endless loop */
    };
}


__attribute__((weak,noreturn)) void _sys_exit(void)
{
    while(1)
    {
        /* Endless loop */
    };
}


__attribute__((weak)) void _sys_command_string(char *cmd, int len)
{
    (void)*cmd;
    (void)len;
}

#endif // defined(__ARMCC_VERSION)
