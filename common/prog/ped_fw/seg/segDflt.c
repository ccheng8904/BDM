/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PP PED-Framework
*   COMP_ABBREV      SEG
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of over-ridable entries of segment library
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "seg.h"
#include "segpriv.h"




void SEG_vClearAll(void)//clears segments and blinkbuffer
{
    uint8_t   i = SEG_BUFFER_LENGTH;
    uint16_t  j = SEG_uiNumberOfSegments;
    
    while (i)
    {
        i--;
        SEG_pucBlinkField[i]=0u;//delete blink buffer
    }
    
    while (j)
    {
        j--;
        _SEG_vSetSegment(j, 0u);//delete segment for segment
    }
}
