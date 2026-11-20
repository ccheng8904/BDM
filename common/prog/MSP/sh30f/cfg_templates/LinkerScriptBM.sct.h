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
    This is a Keil-Arm scatter file template for platform sh30f.
    It will be processed by a C-Preprocessor before interpreted by the linker
 */

LR_BOOT ROOT_REGION
{
    ER_FIXED_VECTORS ROOT_REGION
    {
        *(vectorsFixed)
    }
}

// Special regions for firmware update
LR_FWU0 ROOT_REGION +0x10
{
    ER_FWU_DATA_ANCHOR +0
    {
        *(FWU_DATA_ANCHOR, +FIRST)
        *(FWU_OPT_BYTES)
    }
}
LR_ROM +0
{
    ER_ROM +0
    {
        *(+RO)
    }
}


#ifdef NO_RAM_INIT
LR_RAM TRASH_AREA_START // put the initializer to trash
#else
LR_RAM +0
#endif
{
    ER_FWU_SHARED_DATA (RAM1_START) UNINIT
    {
        *(FWU_SHARED_DATA)
    }

    ER_UNINIT_DATA (RAM1_START + FWU_SHARED_DATA_SIZE) UNINIT
    {
       *(UNINIT_DATA)
    }

    ER_NO_INIT (RAM1_START + FWU_SHARED_DATA_SIZE + UNINIT_DATA_SIZE) UNINIT
    {
        *(NO_INIT)
    }

    ER_RW +0
    {
        *(+RW)
    }

    ER_ZI +0
    {
        *(+ZI)
    }
    ARM_LIB_HEAP +0  EMPTY HEAPSIZE
    {
    }
}


#ifdef FWU_OTP_MEMORY_START_ADDRESS
LR_OTP FWU_OTP_MEMORY_START_ADDRESS FWU_OTP_MEMORY_SIZE
{
    ER_OTP FWU_OTP_MEMORY_START_ADDRESS FWU_OTP_MEMORY_SIZE
    {
        *(FWU_OTP_MEMORY)
    }
}
#endif

#ifdef FWU_FLEX_PARTITION_USED
// separated Flex-Partition table
LR_FWU1 FWU_FLEX_PARTITION_START FWU_FLEX_PARTITION_SIZE
{
    ER_FWU_FLEX_PARTITION +0 FWU_FLEX_PARTITION_SIZE
    {
        *(FWU_FLEX_PARTITION_HEADER, +FIRST)
        *(FWU_FLEX_PARTITION)
    }
}
#endif


// Stack Region
LR_STACK RAM1_END + 1 - STACKSIZE
{
    ARM_LIB_STACK RAM1_END + 1 EMPTY - STACKSIZE
    {
    }
}

// Throw link exception if LR_RAM overlaps the stack
ScatterAssert(RAM1_SIZE > STACKSIZE + ImageLength(LR_RAM))
// Throw link exception if all ROM-LRs exceed available ROM size
ScatterAssert(ROM1_SIZE > ImageLength(LR_BOOT) + ImageLength(LR_FWU0) + ImageLength(LR_ROM)
#ifndef NO_RAM_INIT
  + LoadLength(LR_RAM)                    // consider RAM initializers
#endif
)
