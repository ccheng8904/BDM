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
 *  PROCESSOR        CortexMx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    definitions for Cortex M generic part of vector table.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "SegmentDef.h"
/* Axivion Disable Style MisraC2012-5.4: "Macro identifiers shall be distinct" */
#define ITBL_WEAK   __attribute__((weak)) __attribute__((alias("ITBL_vSpuriousIsr")))
#include "IntTblArmCM.h"
#include "processor.h"
#include "firmware_update/BootManager/BootManagerSharedData.h"
/* Axivion Enable Style MisraC2012-5.4 */

#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    #include "FuncSafe/fSafe.h"
    #include "FuncSafe/fSafeXS.h"
    #include "firmware_update/BootManager/BootManagerSharedDataUc.h"
#endif

#if defined(MKV5X) || defined(MKE14Z)
    #include "hwdt.h"
#endif
#include "basic/basic.h"
#include "int_prio.h"


/* Axivion Disable Style MisraC2012-11.1: "Conversion between function pointer and other type" */
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** \brief    Interrupt dispatching function
 *
 *  \details  Interrupt dispatching is needed for Cortex M0, e.g. for Rom-Loader
 *            and Boot-Manager
 */
#ifdef ITBL_DISPATCHER
static void ITBL_vDispatcher(void);
#endif


/** \brief    Startup function for RAM initialization callin main/Main function.
 *
 *  \details  Compiler dependent RAM initialization function of data and BSS
 *            memory segment. In case NO_RAM_INIT is defined, RAM is not 
 *            initialized and "Main" function is executed.
 */
static void ITBL_main(void);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/** \brief    Extern variables for GCC RAM initialization
 *
 *  \details  These variables are defined by linker based on linker script
 */
#if defined(__GNUC__) && !defined(__ARMCC_VERSION) && !defined(NO_RAM_INIT)
    /* Axivion Disable Style MisraC2012-21.2: "A reserved identifier or macro name shall not be declared" */
    extern uint32_t __etext;
    extern uint32_t __data_start__;
    extern uint32_t __data_end__;
    extern uint32_t __bss_start__;
    extern uint32_t __bss_end__;
    /* Axivion Enable Style MisraC2012-21.2 */

    #define ETEXT       (&__etext)
    #define DATA_START  (&__data_start__)
    #define DATA_END    (&__data_end__)
    #define BSS_START   (&__bss_start__)
    #define BSS_END     (&__bss_end__)
    #define BSS_SIZE    ((uint32_t)BSS_END - (uint32_t)BSS_START)
    #define DATA_SIZE   ((uint32_t)DATA_END - (uint32_t)DATA_START)
#endif


/* Axivion Disable Style MisraC2012-5.5: "Identifiers shall be distinct from macro names" */
/** \brief    FPU register definition
 *
 *  \details  Define CPACR register if processor header is not CMSIS conform
 */
#if defined(ARM_HW_FPU_PRESENT) && !defined(CPACR)
    #define CPACR   SCB->CPACR
    #ifndef CPACR_CP10_MASK
        #define CPACR_CP10_MASK                   0x300000U      ///< Access privileges for coprocessor
        #define CPACR_CP11_MASK                   0xc00000U      ///< Access privileges for coprocessor
    #endif
#endif
/* Axivion Enable Style MisraC2012-5.5 */


/** \brief    VTOR register value definition
 *
 *  \details  If ABSOLUTE_ADDR_IN_VTOR is defined, An absolute address placed
 *            into VTOR, it happen in programmer running from RAM and also NXP
 *            MKV5x needs to setup VTOR this way. Othervise an address offset
 *            is placed to VTOR - common for STM.
 */
#if !defined(CORTEX_M0) && !defined(NO_ISR_SUPPORT)
    #if defined(ABSOLUTE_ADDR_IN_VTOR) || !defined(need_fixed_vectors)
        #define VTOR_VALUE (uint32_t)ITBL_coreVectors
    #else
        #define VTOR_VALUE (uint32_t)(((uint32_t)ITBL_coreVectors)-((uint32_t)ITBL_fixedVectors));
    #endif
#endif



/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/


/** \brief    Fixed interrupt table array definition.
 *
 *  \details  In case of Cortex M0 the interrupt vectors are fixed to one
 *            possition. In this case the dispatcher function has to be used to
 *            retarget interrupt function calls.
 */
#ifdef ITBL_DISPATCHER
SDEF_SetSegmentConst(vectorsFixed)
/** unmovable vector table, including non maskable interrupts */
const ITBL_fptr SYMBOL_USED ITBL_fixedVectors[] =
{
   (ITBL_fptr)STACK_TOP  //lint !e9029 !e923 works fine on CortexMx
   ,ITBL_vResetHandler
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher, ITBL_vDispatcher
   ,ITBL_vDispatcher, ITBL_vDispatcher
};
#else // ITBL_DISPATCHER
#ifdef need_fixed_vectors
SDEF_SetSegmentConst(vectorsFixed)
/** unmovable vector table, including non maskable interrupts */
const ITBL_fptr SYMBOL_USED ITBL_fixedVectors[] =
{
   (ITBL_fptr)STACK_TOP  /*lint !e9029 !e923 works fine on CortexMx */
   ,ITBL_vResetHandler
/* Currently definition not possible as it overlaps with boot loader */
//   ,NMI_Handler,
//   ,HardFault_Handler
};
#endif // need_fixed_vectors
#endif //ITBL_DISPATCHER


/** \brief    Core interrupt table array definition.
 *
 *  \details  Interrupt table consists of ITBL_coreVectors - common for all
 *            Cortex M devices and from ITBL_peripheralVectors defined in
 *            IntTbl.c. This interrupt table is usually located at the start
 *            of the application.
 */
#ifndef NO_ISR_SUPPORT
#if defined(CORTEX_M0) || defined(CORTEX_M0_PLUS)
SDEF_SetSegmentConst(vectorsCore)
/** Cortex M0 and M0+ core vector table*/
const ITBL_fptr SYMBOL_USED ITBL_coreVectors[] =
{
// ||Handler function name||    ||Priority||        ||Address offset||
   /*lint -e{923, 9029}  cast tested to work correctly for ARM CortexMx */
   // AXIVION Next Line MisraC2012-11.1
   (ITBL_fptr)STACK_TOP         //  none              0x0000
   // Cortex internal interrupts
   ,ITBL_vResetHandler          //  -3                0x0004
   ,NMI_Handler                 //  -2                0x0008
   ,HardFault_Handler           //  -1                0x000C
   ,ITBL_vSpuriousIsr           //  none
   ,ITBL_vSpuriousIsr           //  none
   ,ITBL_vSpuriousIsr           //  none
   ,ITBL_vSpuriousIsr           //  none
   ,ITBL_vSpuriousIsr           //  none
   ,ITBL_vSpuriousIsr           //  none
   ,ITBL_vSpuriousIsr           //  none
   ,SVC_Handler                 //   3                0x002C
   ,ITBL_vSpuriousIsr           //   4                0x0030
   ,ITBL_vSpuriousIsr           //  none
   ,PendSV_Handler              //   5                0x0038
   ,SysTick_Handler             //   6                0x003C

};

#else /// Cortex M3, M4, M4F, M33, M7
#ifdef BTM_FIRMWARE_UPDATE
extern void ITBL_slAppProperties (void);
#endif

SDEF_SetSegmentConst(vectorsCore)
/** Cortex M3, M4, M4F and M23 core vector table*/
const ITBL_fptr SYMBOL_USED ITBL_coreVectors[] =
{
// ||Handler function name||  ||Priority||        ||Address offset||
   /*lint -e{923, 9029}  cast tested to work correctly for ARM CortexMx */
   // AXIVION Next Line MisraC2012-11.1
   (ITBL_fptr)STACK_TOP         // none               0x0000
   // Cortex internal interrupts
   ,ITBL_vResetHandler          // -3                 0x0004
   ,NMI_Handler                 // -2                 0x0008
   ,HardFault_Handler           // -1                 0x000C
   ,MemManage_Handler           //  0                 0x0010
   ,BusFault_Handler            //  1                 0x0014
   ,UsageFault_Handler          //  2                 0x0018
   ,ITBL_vSpuriousIsr           // none
   ,ITBL_vSpuriousIsr           // none
   ,ITBL_vSpuriousIsr           // none
   ,ITBL_vSpuriousIsr           // none
   ,SVC_Handler                 //  3                 0x002C
   ,DebugMon_Handler            //  4                 0x0030
  #ifdef BTM_FIRMWARE_UPDATE
   ,ITBL_slAppProperties
  #else
   ,ITBL_vSpuriousIsr           // none
  #endif
   ,PendSV_Handler              //  5                 0x0038
   ,SysTick_Handler             //  6                 0x003C
};
SDEF_SetSegmentConst_Default()
#endif
#else
#ifdef need_ModuleHeader
SDEF_SetSegmentConst(vectorsCore)
const ITBL_fptr SYMBOL_USED ITBL_coreVectors[] =
{
   /*lint -e{923, 9029}  cast tested to work correctly for ARM CortexMx */
   // AXIVION Next Line MisraC2012-11.1
   (ITBL_fptr)STACK_TOP
   ,ITBL_vResetHandler
   ,NMI_Handler
   ,HardFault_Handler
};
SDEF_SetSegmentConst_Default()
#endif //need_ModuleHeader
#endif //NO_ISR_SUPPORT


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
#if !defined(NO_ISR_SUPPORT) || defined(need_ModuleHeader)
void ITBL_vSpuriousIsr(void)
{
    /* Call user modifiable Spurious Interrupt Handle function */
    SpuriousIsr_Handler(__get_IPSR());
}
#endif

void ITBL_vResetHandler(void)
{
    __set_MSP((uint32_t)STACK_TOP);  // set main stack pointer to top of ram

#if defined(MKV5X) || defined(MKE14Z)
#if defined(VARIANT_BOOTMANAGER) || defined(VARIANT_ROMLOADER)
    // Watchdog registers are programmable but must be configured within 128 bus clocks after a reset
    // Watchdog is enabled after a reset by default
    HWDT_vInit(0);
#endif
#endif

#if !defined(CORTEX_M0) && !defined(NO_ISR_SUPPORT)
    /* Set VTOR in case not Cortex M0 and ISR support is enabled */
    SCB->VTOR = VTOR_VALUE;  //lint !e923 works fine on cortexM
#endif

#if defined(ARM_HW_FPU_PRESENT)
    /* if HW FPU unit present, need to be allowed before C environment initialization */
    CPACR |= (CPACR_CP11_MASK|CPACR_CP10_MASK);  //lint !e923 works fine on CortexMx
#endif

#if defined(RA4)
    BASIC_vEccInit();   // Registers for ECC and Parity check must be configured before a main function
#endif

#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && !defined(TH_FSCOM) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    // this function has to be modified if any additional measures are
    // needed to put device in Safe State after reset
    FSF_vSetApplicationSafeState(); // at this point device must be put into Safe State

    /* For BootManager is necessary to move start of RAM checking area, because
     * the beginning of RAM is used to exchange information between BM -
     * PROGRAMMMER - LROMLOADER.. Size of this area is currently 20 bytes but we
     * have to align to 64 bytes because RAM_BLOCKSIZE is 16 of size of uint32_t
     * so 16*4=64 */
    uint32_t u32ExcludedData = 0u;

    if (BMDAT_isUninitDataChecked())
    {
        u32ExcludedData = RAM_SIZE_OF_EXCLUDED_DATA;
    }

    FSF_vRamcheck(FSF_atRamSegments, FSF_u32GetNumberOfRamSegments(), u32ExcludedData);

    /* Uninitialized data (SharedData + UninitData) are already checked(after HW reset)
     * We must set flag and during next (SW) reset data will not be checked*/
    BMDAT_setUninitDataChecked();

  #ifdef WIRED_FIRMWARE_UPDATE
    BMDAT_setIsrTableAddr((size_t)BL_APPLICATION_ROM_START); //lint !e9048 lint problem using UINT32_C
  #endif

#endif  // FEATURE_FUNCTIONAL_SAFETY_ENABLED

#if defined(APP_VARIANT) && !defined(NO_ISR_SUPPORT)
    /* sets the interrupts priorities */
    INTP_setApplicationPriority();
#endif

    /* Perform RAM initialization if required and call main/Main function */
    ITBL_main();

    for(;;) {} // workaround for compiler to not put push/pop instructions
}

#ifndef NO_FIRMWARE_UPDATE
void ITBL_StartApplication(uint32_t startAddress)
{
    const ITBL_fptr *itbl_app = (ITBL_fptr*)startAddress; //lint !e923 works fine on cortexM

    BMDAT_setIsrTableAddr(startAddress);
    // set main stack pointer to top of ram
    itbl_app[1](); // call the application
}
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

#ifdef ITBL_DISPATCHER
static void ITBL_vDispatcher(void)
{
    const ITBL_fptr *isr_table = (ITBL_fptr*)BMDAT_getIsrTableAddr(); //lint !e923 works fine on cortexM
    isr_table[__get_IPSR()]();
}
#endif

/* No ram init - calls "Main" function */
#if defined(NO_RAM_INIT)
    static void ITBL_main(void) { Main(); }

/* GCC startup */
#elif defined(__GNUC__) && !defined(__ARMCC_VERSION)
    /* Application main routine */
    extern int main(void);/*lint !e970 int is standard return value and allowed here */
    /* The static objects constructors initializer */
    /* Axivion Next Line MisraC2012-21.2: "A reserved identifier or macro name shall not be declared" */
    extern void __libc_init_array(void);

    static void ITBL_main(void)
    {
        uint32_t const *src = ETEXT;
        uint32_t *dst = DATA_START;
        
        /* Copy data segment from ROM to RAM */
        /*lint -e{923, 9033}  cast tested to work correctly for ARM CortexMx */
        for(uint32_t index = 0u; index < (DATA_SIZE/sizeof(uint32_t)); index++)
        {
            dst[index] = src[index];
        }
        
        dst = BSS_START;
        /* Zero out bss data segment */
        /*lint -e{923, 9033}  cast tested to work correctly for ARM CortexMx */
        for(uint32_t index = 0u; index < (BSS_SIZE/sizeof(uint32_t)); index++)
        {
            dst[index] = 0u;
        }
        
        /* Call static objects constructors */
        __libc_init_array();
        
        /* Jump to main application */
        (void)main();
        for(;;) {} /* Endless loop - no return from main */
    }

/* Keil ARM startup */
#else
    static void ITBL_main(void) { __main(); }
#endif
/* Axivion Enable Style MisraC2012-11.1 */

