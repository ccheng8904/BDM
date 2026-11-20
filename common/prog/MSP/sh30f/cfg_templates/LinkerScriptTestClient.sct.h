/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT           Generic SW
 *  PLATFORM          sh30f
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*
    This is a Keil-Arm scatter file template for platform sh30f
    It is used for test application only
 */


LR_ROM ROM1_START
{
    ER_ROM +0
    {
        * (+RO)
    }
}

LR_RAM RAM1_START
{
    ER_RW +0
    {
        * (+RW)
    }

    ER_ZI +0
    {
        * (+ZI)
    }

    // Include all timer segments
    #include "timer/timer_seg.h"

    ARM_LIB_HEAP +0  EMPTY HEAPSIZE
    {
    }
}

// Stack Region
LR_STACK RAM1_END + 1 - STACKSIZE
{
    ARM_LIB_STACK RAM1_END + 1 EMPTY - STACKSIZE
    {
    }
}

// Throw link exception if LR_RAM overlaps the stack
// Note: In exception message  '>' used in stead of '<' (linker issue)
ScatterAssert(RAM1_SIZE > (STACKSIZE + ImageLength(LR_RAM)))
