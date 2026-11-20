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
    This is a Keil-Arm scatter file template for platform sh30f ramLoader.
    It will be processed by a C-Preprocessor before interpreted by the linker
 */

LR_BOOT ROOT_REGION
{
    ER_FIXED_VECTORS ROOT_REGION
    {
        *(vectorsFixed)
    }
}


LR_ROM ROM1_START ROM1_SIZE
{
    ER_VECTORS_CORE +0
    {
        *(vectorsCore)
    }
    ER_ROM +0
    {
        * (+RO)
    }
}


LR_RAM TRASH_AREA_START // put the initializer to trash
{
    ER_FWU_SHARED_DATA RAM1_START UNINIT
    {
       * (FWU_SHARED_DATA)
    }
    ER_RW +0
    {
        * (+RW)
    }
    ER_ZI +0
    {
        * (+ZI)
    }
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
ScatterAssert(RAM1_SIZE > STACKSIZE + ImageLength(LR_RAM))
