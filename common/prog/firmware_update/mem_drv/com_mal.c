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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief Implementation of common flash functions.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

#define COMA_UNIT // do not parse functions using COMA_wrUnit_t
#include "com_mal.h"
#include <string.h>

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
DRVIF_RAMCODE bool COMA_isAddrRangeValidx(
        const struct COMA_memSeg_s* memSegs, uint32_t segCount, uint32_t addr, uint32_t range)
{
    for(uint32_t i = 0; i<segCount; i++) // do for all memory segments
    {
        if(COMA_isAddrRangeValid(&memSegs[i], addr, range))
        {
            return true;
        }
    }
    return false;
}


DRVIF_RAMCODE bool COMA_isBlockAddrValidx(
        const struct COMA_memSeg_s* memSegs, uint32_t segCount, uint32_t addr)
{
    for(uint32_t i = 0; i<segCount; i++) // do for all memory segments
    {
        if(COMA_isAddrRangeValid(&memSegs[i], addr, 0u))
        {
            const uint32_t COMA_BlockMask = (uint32_t)(memSegs[i].blockSize-1u);
            return (((COMA_BlockMask & addr) == 0u) ? true : false);
        }
    }
    return false;
}

DRVIF_RAMCODE uint32_t COMA_getBlockSize(
        const struct COMA_memSeg_s* memSegs, uint32_t segCount, uint32_t addr)
{
    for(uint32_t i = 0; i<segCount; i++) // do for all memory segments
    {
        if(COMA_isAddrRangeValid(&memSegs[i], addr, 0u))
        {
            return memSegs[i].blockSize;
        }
    }
    return 0u; // no valid block address
}

DRVIF_RAMCODE bool COMA_read(uint32_t addr, uint32_t size, uint8_t* buffer)
{
    /*lint -e{923} "cast from unsigned int to pointer [MISRA 2012 Rule 11.1, required]", required
           Cast tested to work correctly */
    if(memcpy((void*)buffer,(const void*)addr, size)){}
    return true;
}
