/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
*   PROJECT          MCU Framework
*   MODULE-PREFIX    BASIC
*   PROCESSOR        CMS32M67xx
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file   basicMcXs.c
*
*   \brief  implementation of micro-controller initialization and configuration.
*
*   \details The core clock of the CMS32M67xx is derived from the high speed
*            on-chip oscillator HOCO. The HOCO frequency is selected by the
*            user option byte 0xC2 (FRQSEL), see system_CMS32M67xx.c. The
*            clock tree is therefore only read back here, the frequency is
*            checked against the build configuration (CORE_CLOCK).
*
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "basic.h"
#include "processor.h"
#ifdef MCAL_MPCM_INCLUDED
    #include "mcal/mpcm.h"
    #include "mcal/mcal_assert.h"
#endif
#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
#include "fSafe.h"
#endif

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#if defined(MCAL_MPCM_INCLUDED)

/* Axivion Disable Style MisraC2012-8.4 : Missing declaration for function definition */
const struct MPCM_PowerModesConfig customPowerModes =
{
    /* Check available configurations for your platform in mcpm_mc.h */
    &MPCM_CFG_DEFAULT,              // Take configuration from make process
    &MPCM_CFG_CORE_8M_PCLK_8M_PLLON,// Lowered clock speeds for low-power run
    NULL,                           // No special configuration for sleep mode
    &MPCM_CFG_STOP                  // No special configuration for stop mode
};
/* Axivion Enable Style MisraC2012-8.4 */ 

#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
#if !defined(MCAL_MPCM_INCLUDED)

/** \brief Initializes the system clock of the CMS32M67xx */
static void BASIC_vInitClock(void);

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS AND PRIVATE MACRO FUNCTION DEFINITIONS        */
/******************************************************************************/
/*Axivion Disable MisraC2012-8.7 : Routine can be declared static in primary file. */
void BASIC_vInitPlatform(void)
{
    /* Set the clocks to default state as like after the hardware reset */

  #if defined(MCAL_MPCM_INCLUDED)
    if(MPCM_init(&customPowerModes) != MCAL_OK)
    {
        MCAL_error("Invalid MPCM configuration");
    }
  #else
    BASIC_vInitClock();
  #endif

#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    FSF_vBoot();
#endif
}
/* Axivion Enable Style MisraC2012-8.7 */ 

#if !defined(MCAL_MPCM_INCLUDED)

/**
  * @brief  Initializes the system clock.
  * @note   The clock source and the clock frequency of the CMS32M67xx are
  *         defined by the user option bytes, the system driver only reads the
  *         resulting frequency back (SystemCoreClock).
  * @retval None
  */
static void BASIC_vInitClock(void)
{
    /* Read back the frequency defined by the user option byte FRQSEL */
    SystemCoreClockUpdate();
}

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void BASIC_vConfigPlatform (void)
{
    /* Clock Configuration*/
    BASIC_vInitPlatform();
    
#ifdef APP_VARIANT
    /* Here should be placed all application specific initialization */
    
    /* Set Priority of configurable interrupts */
    // setDefaultInterruptPriority();
    
    // HADC_vInit(0);

    /* all other initializations */
    // ...
#endif
}
