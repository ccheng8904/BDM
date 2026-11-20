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
 *  COMP_ABBREV      RTG
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  retarget
 *
 *  \brief    Public definitions and declarations for unit retarget.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include <stdbool.h>
#include <stdint.h>
#include "retarget.h"
#include "std_symbol.h"
/*lint -save -e970 -e955 */
#include <stdio.h>
/*lint -restore -e970 -e955 */

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#ifndef RTG_ASSERT_MESSAGE_SIZE
#define RTG_ASSERT_MESSAGE_SIZE 100u
#endif


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

void RTG_writeString(const char *str);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

SYMBOL_WEAK void RTG_init(void)
{
    // No strong retarget definition for initialization
}


SYMBOL_WEAK void RTG_writeChar(char ch)
{
    (void)ch;
    /* No strong retarget definition for writeChar */
    /* Workaroud to not optimize RTG_writeString function */
    volatile bool cond = true;
    while(cond)
    {
        /* Endless loop */
    }
}


SYMBOL_WEAK char RTG_readChar(void)
{
    /* No strong retarget definition for readChar */
    for(;;)
    {
        /* Endless loop */
    }
}


SYMBOL_WEAK void RTG_assert(const char *expr, const char *file, int32_t line)
{
    static char assertMessage[RTG_ASSERT_MESSAGE_SIZE];

    /*lint -e586 function 'snprintf' is deprecated. [MISRA 2012 Rule 21.6, advisory]
                 snprintf doesn´t use in the release variant
    */
#if defined(KEILARM) || defined(ARMCLANG)
    (void)snprintf(assertMessage, sizeof(assertMessage),
        "\"%s\":%d: error: %s", file, line, expr);
#else
    (void)snprintf(assertMessage, sizeof(assertMessage),
        "\"%s\":%ld: error: %s", file, line, expr);
#endif
    
    RTG_writeString(assertMessage);

    for(;;)
    {
        /* Endless loop */
    }
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

void RTG_writeString(const char *str)
{
    while(*str != '\0')
    {
        RTG_writeChar(*str);
        str++;
    }
}
