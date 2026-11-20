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
*   AUTHOR           Jing Liu
*   CREATED          24.05.2023
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file   basicMcXs.c
*
*   \brief  implementation of micro-controller initialization and configuration.
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

/*******************************************************************************
*-------------------------------------------------------------------------------
*        System Clock source                    | PLL (HSI/HSE)
*-------------------------------------------------------------------------------
*        SYSCLK(MHz)                            | 4 - 48 MHz
*-------------------------------------------------------------------------------
*        AHB(MHz) [CORE_CLOCK]                  | max 48 MHz
*-------------------------------------------------------------------------------
*        APB0, APB1(MHz)                        | max 24 MHz
*-------------------------------------------------------------------------------
*        AHB Prescaler                          | 2^0 - 2^5 range
*-------------------------------------------------------------------------------
*        APB0 Prescaler                         | 2^0 - 2^4 range
*-------------------------------------------------------------------------------
*        APB1 Prescaler                         | 2^0 - 2^4 range
*-------------------------------------------------------------------------------
*        HXTAL Frequency(MHz)                   | 4-12 MHz
*-------------------------------------------------------------------------------
*        PLLXTPRE                               | auto
*-------------------------------------------------------------------------------
*        PLLF                                   | auto
*-------------------------------------------------------------------------------
*        PLLK                                   | auto
*-------------------------------------------------------------------------------
*        Flash Latency(WS)                      | auto
*-------------------------------------------------------------------------------
*******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
//lint -esym( 750, *_SYSCLK) "message: symbol not used..."
#define MAX_SYSCLK    48U    // max PLL output clock frequency
#define MIN_SYSCLK    4U     // min PLL output clock frequency
#define MAX_PCLOCK    24U    // max APB peripheral clock frequency

//lint -esym( 750, FLASH_*) "message: symbol not used..."
#define FLASH_ACR_LATENCY_0WS          ((uint32_t)0x5AA50000)
#define FLASH_ACR_LATENCY_1WS          ((uint32_t)0x5AA50001)
#define FLASH_ACR_LATENCY_2WS          ((uint32_t)0x5AA50002)
#define FLASH_ACR_LATENCY_3WS          ((uint32_t)0x5AA50003)

//lint -esym( 750, SYSCFG_*) "message: symbol not used..."
#define SYSCFG_SAFR_XTAL1_XTAL2_GPIO_MODE       ((uint32_t)0x5AA50000)
#define SYSCFG_SAFR_XTAL1_XTAL2_EXT_OSC_MODE    ((uint32_t)0x5AA50001)

//lint -esym( 750, RCC_*) "message: symbol not used..."
#define RCC_CFGR_HPRE_DIV1      0U
#define RCC_CFGR_HPRE_DIV2      1U
#define RCC_CFGR_HPRE_DIV4      2U
#define RCC_CFGR_HPRE_DIV8      3U
#define RCC_CFGR_HPRE_DIV16     4U
#define RCC_CFGR_HPRE_DIV32     5U

#define RCC_CFGR_HSESEL_NORMAL  0U
#define RCC_HSERFB_RFBSEL_500K  3U

#define RCC_CFGR_PLLSRC_HSI  0U
#define RCC_CFGR_PLLSRC_HSE  1U

#define RCC_CFGR_PPRE0_DIV1     0U
#define RCC_CFGR_PPRE0_DIV2     1U
#define RCC_CFGR_PPRE0_DIV4     2U
#define RCC_CFGR_PPRE0_DIV8     3U
#define RCC_CFGR_PPRE0_DIV16    4U

#define RCC_CFGR_PPRE1_DIV1     0U
#define RCC_CFGR_PPRE1_DIV2     1U
#define RCC_CFGR_PPRE1_DIV4     2U
#define RCC_CFGR_PPRE1_DIV8     3U
#define RCC_CFGR_PPRE1_DIV16    4U

//lint -esym( 750, HSI) "message: symbol not used..."
#define HSI             24U

/* define SRC_CLK based on HSI or HSE */
//lint -esym( 750, SRC_CLK) "message: symbol not used..."
#ifdef EXT_CRYSTAL_OSC
    #define SRC_CLK   EXT_CRYSTAL_OSC
#else
    #define SRC_CLK   HSI
#endif

//lint -esym( 750, *PLL*) "message: symbol not used..."
#define PLLXTPRE_OFFSET    1U
#define PLLF_OFFSET        15U
#define PLLK_OFFSET        1U

/* PLL div/mul calculation and HPRE calculation */
#ifndef EXT_CRYSTAL_OSC
    #if (CORE_CLOCK == 48U)
        #define N_PLLXTPRE    4U  //first divider, range: 1 to 4, result after this division should be between 4-8MHz
        #define F_PLLF        32U //multiplier, range: 15 to 46, result after this multiplication should be between 150-300MHz
        #define K_PLLK        1U  //second divider, range: 1 to 8, result after this division should be between 16-192MHz,
                                  //in the end a divider of 4 is applied to produce the final PLLCLK between 4-48MHz
        #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
        #define SYSCLK          CORE_CLOCK
    #elif (CORE_CLOCK == 36U)
        #error "Unrecommended core_clock"
    #elif (CORE_CLOCK == 24U)
        #define N_PLLXTPRE    4U  //first divider
        #define F_PLLF        32U //multiplier
        #define K_PLLK        2U  //second divider
        #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
        #define SYSCLK          CORE_CLOCK
    #elif (CORE_CLOCK == 12U)
        #define N_PLLXTPRE    4U  //first divider
        #define F_PLLF        32U //multiplier
        #define K_PLLK        4U  //second divider
        #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
        #define SYSCLK          CORE_CLOCK
    #elif (CORE_CLOCK == 8U)
        #define N_PLLXTPRE    3U  //first divider
        #define F_PLLF        32U //multiplier
        #define K_PLLK        8U  //second divider
        #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
        #define SYSCLK          CORE_CLOCK
    #elif (CORE_CLOCK == 6U)
        #define N_PLLXTPRE    4U  //first divider
        #define F_PLLF        28U //multiplier
        #define K_PLLK        7U  //second divider
        #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
        #define SYSCLK          CORE_CLOCK
    #else
        #error "Unsupported core_clock."
    #endif
#else
    #if ((4U <= SRC_CLK) && (SRC_CLK <= 12U))
        #if (SRC_CLK == 8U)
            #if (CORE_CLOCK == 48U)
                #define N_PLLXTPRE    1U  //first divider
                #define F_PLLF        24U //multiplier
                #define K_PLLK        1U  //second divider
                #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
                #define SYSCLK          CORE_CLOCK
            #elif (CORE_CLOCK == 36U)
                #error "Unrecommended core_clock"
            #elif (CORE_CLOCK == 24U)
                #define N_PLLXTPRE    1U  //first divider
                #define F_PLLF        24U //multiplier
                #define K_PLLK        2U  //second divider
                #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
                #define SYSCLK          CORE_CLOCK
            #elif (CORE_CLOCK == 12U)
                #define N_PLLXTPRE    1U  //first divider
                #define F_PLLF        24U //multiplier
                #define K_PLLK        4U  //second divider
                #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
                #define SYSCLK          CORE_CLOCK
            #else
                #error "Unsupported core_clock."
            #endif
        #elif (SRC_CLK == 12U)
            #if (CORE_CLOCK == 48U)
                #define N_PLLXTPRE    2U  //first divider
                #define F_PLLF        32U //multiplier
                #define K_PLLK        1U  //second divider
                #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
                #define SYSCLK          CORE_CLOCK
            #elif (CORE_CLOCK == 24U)
                #define N_PLLXTPRE    2U  //first divider
                #define F_PLLF        32U //multiplier
                #define K_PLLK        2U  //second divider
                #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
                #define SYSCLK          CORE_CLOCK
            #elif (CORE_CLOCK == 12U)
                #define N_PLLXTPRE    2U  //first divider
                #define F_PLLF        32U //multiplier
                #define K_PLLK        4U  //second divider
                #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
                #define SYSCLK          CORE_CLOCK
            #else
                #error "Unsupported core_clock."
            #endif
        #elif (SRC_CLK == 4U)
            #if (CORE_CLOCK == 36U)
                #error "Unrecommended core_clock"
            #elif (CORE_CLOCK == 12U)
                #define N_PLLXTPRE    1U  //first divider
                #define F_PLLF        36U //multiplier
                #define K_PLLK        3U  //second divider
                #define RCC_HPRE_PSC    RCC_CFGR_HPRE_DIV1   /*!< SYSCLK divided by 1  */
                #define SYSCLK          CORE_CLOCK
            #else
                #error "Unsupported core_clock."
            #endif
        #endif
    #else
        #error "EXT_CRYSTAL_OSC out of boundaries (4-12MHz)."
    #endif
#endif


/* FLASH_ACR_LATENCY calculation */
#if ( SYSCLK <= 12U )
    #define FLASH_ACR_LATENCY_VALUE    FLASH_ACR_LATENCY_0WS
    #if !defined(MCAL_MPCM_INCLUDED)
        #define PREFETCH_VALUE             1u
    #endif
#elif (( SYSCLK <= 24U ) && ( 12U < SYSCLK ))
    #define FLASH_ACR_LATENCY_VALUE    FLASH_ACR_LATENCY_2WS
    #if !defined(MCAL_MPCM_INCLUDED)
        #define PREFETCH_VALUE             1u
    #endif
#elif (( SYSCLK <= MAX_SYSCLK ) && ( 24U < SYSCLK ))
    #define FLASH_ACR_LATENCY_VALUE    FLASH_ACR_LATENCY_1WS
    #if !defined(MCAL_MPCM_INCLUDED)
        #define PREFETCH_VALUE             0u
    #endif
#else
    #error "No valid CORE_CLOCK defined!"
#endif

/* PCLOCK (APB0) calculation */
#ifndef PCLOCK
    #if (CORE_CLOCK < MAX_PCLOCK)
        #define PCLOCK    CORE_CLOCK
    #else
        #define PCLOCK    MAX_PCLOCK
    #endif
#endif

#define DIV1 (CORE_CLOCK / PCLOCK)
#if ( (CORE_CLOCK % PCLOCK) == 0U ) && ( PCLOCK <= MAX_PCLOCK )
    #if DIV1 == 1U
        #define RCC_APB0_PSC       RCC_CFGR_PPRE0_DIV1
    #elif DIV1 == 2U
        #define RCC_APB0_PSC       RCC_CFGR_PPRE0_DIV2
    #elif DIV1 == 4U
        #define RCC_APB0_PSC       RCC_CFGR_PPRE0_DIV4
    #elif DIV1 == 8U
        #define RCC_APB0_PSC       RCC_CFGR_PPRE0_DIV8
    #elif DIV1 == 16U
        #define RCC_APB0_PSC       RCC_CFGR_PPRE0_DIV16
    #else
        #error "No valid value of peripheral clock defined. PCLOCK must be correctly defined."
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK must be correctly defined."
#endif

/* PCLOCK1 (APB1) calculation */
#ifndef PCLOCK1
    #if (CORE_CLOCK < MAX_PCLOCK)
        #define PCLOCK1    CORE_CLOCK
    #else
        #define PCLOCK1    MAX_PCLOCK
    #endif
#endif

#define DIV2 (CORE_CLOCK / PCLOCK1)
#if ( (CORE_CLOCK % PCLOCK1) == 0U ) && ( PCLOCK1 <= MAX_PCLOCK )
    #if DIV2 == 1U
        #define RCC_APB1_PSC       RCC_CFGR_PPRE1_DIV1
    #elif DIV2 == 2U
        #define RCC_APB1_PSC       RCC_CFGR_PPRE1_DIV2
    #elif DIV2 == 4U
        #define RCC_APB1_PSC       RCC_CFGR_PPRE1_DIV4
    #elif DIV2 == 8U
        #define RCC_APB1_PSC       RCC_CFGR_PPRE1_DIV8
    #elif DIV2 == 16U
        #define RCC_APB1_PSC       RCC_CFGR_PPRE1_DIV16
    #else
        #error "No valid value of peripheral clock defined. PCLOCK1 must be correctly defined."
    #endif
#else
    #error "No valid peripheral clock defined. PCLOCK1 must be correctly defined."
#endif

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

/** \brief Sets the PLL */
static void BASIC_vSetPLL(uint32_t divFactorPLL, uint32_t mulFactorPLL, uint32_t divFactorPLL2);

/** \brief Initializes clock based on make configuration */
static void BASIC_vInitClock(void);

/** \brief Enable the Prefetch feature. */
static void BASIC_vEnablePrefetch(void);

#endif /* !defined(MCAL_MPCM_INCLUDED) */

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS AND PRIVATE MACRO FUNCTION DEFINITIONS   v    */
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
    BASIC_vEnablePrefetch();
  #endif

#if defined(FEATURE_FUNCTIONAL_SAFETY_ENABLED) && \
    (defined(VARIANT_BOOTMANAGER) || defined(APP_VARIANT))
    FSF_vBoot();
#endif
}
/* Axivion Enable Style MisraC2012-8.7 */ 

#if !defined(MCAL_MPCM_INCLUDED)

/**
  * @brief  Enable the Prefetch feature.
  * @note   None
  * @retval None
  */
static void BASIC_vEnablePrefetch(void)
{
    /* Clear PRFTEN bit */
    uint32_t tmpreg = FLASH->ACR.V32 & (~FLASH_ACR_PRFTEN_Msk);
    /* Set PRFTEN bit according to the New State value */
    tmpreg |= (((uint32_t)0x5AA5U << FLASH_ACR_LOCK_Pos) | ((uint32_t)PREFETCH_VALUE << FLASH_ACR_PRFTEN_Pos));
    /* Store the new value */
    FLASH->ACR.V32 = tmpreg;
}

static void BASIC_vInitClock(void)
{

    /* Set Flash latency */
    FLASH->ACR.V32 = FLASH_ACR_LATENCY_VALUE;

    /* Unlock RCC Configuration Register */
    RCC_REGS_UNLOCK();

    /* Enable SYSCFG Module */
    RCC->AHBENR.BIT.SYSCFGEN = 1U;

    /* Unlock SYSCFG SAFR register and set XTAL1 and XTAL2 pins to GPIO mode */
    SYSCFG->SAFR.V32 = SYSCFG_SAFR_XTAL1_XTAL2_GPIO_MODE;

    /* Enable HSI */
    RCC->CR.BIT.HSION = 1U;

    /* Wait till HSI Ready */
    while (RCC->CR.BIT.HSIRDY == 0U) {}

    /* Set HSI as PLL clock source */
    RCC->CFGR.BIT.PLLSRC = RCC_CFGR_PLLSRC_HSI;

#ifdef EXT_CRYSTAL_OSC

    /*Unlock SYSCFG SAFR register and set XTAL1 and XTAL2 pins to oscillator mode */
    SYSCFG->SAFR.V32 = SYSCFG_SAFR_XTAL1_XTAL2_EXT_OSC_MODE;

    /* Recommended setting for driving the crystal oscillator */
    RCC->CFGR.BIT.HSESEL = RCC_CFGR_HSESEL_NORMAL;
    RCC->HSERFB = RCC_HSERFB_RFBSEL_500K;

    /* Enable HSE */
    RCC->CR.V32 |= RCC_CR_HSEON;

    /* Wait till HSE ready */
    while (RCC->CR.V32 & RCC_CR_HSERDY == 0U) {}
#endif

    BASIC_vSetPLL(N_PLLXTPRE, F_PLLF, K_PLLK);

    /* LOCK RCC */
    RCC_REGS_LOCK();
}


static void BASIC_vSetPLL(uint32_t divFactorPLL, uint32_t mulFactorPLL, uint32_t divFactorPLL2)
{
    /* Set PLL dividers and multiplier */
    RCC->CFGR.BIT.PLLXTPRE = (uint8_t)(divFactorPLL - PLLXTPRE_OFFSET); //PREFCLK = PLLCLKIN / N
    RCC->CFGR.BIT.PLLF = (uint8_t)(mulFactorPLL - PLLF_OFFSET);         //VCOCLK = PRECLK * F
    RCC->CFGR.BIT.PLLK = (uint8_t)(divFactorPLL2 - PLLK_OFFSET);        //PLLCLK = (VCOCLK / K) * (1 / 4)

#ifdef EXT_CRYSTAL_OSC
    /* Set HSE as PLL clock source */
    RCC->CFGR.BIT.PLLSRC = RCC_CFGR_PLLSRC_HSE;
#endif

    /* Enable PLL clock */
    RCC->CR.V32 |= RCC_CR_PLLON;

    /* Wait till PLL circuit ready */
    while ((RCC->CR.V32 & RCC_CR_PLLRDY) == 0U) {}

    /* Switch SYSCLK to PLL */
    RCC->CR.BIT.SW = (uint32_t)SYSTEM_CLK_SRC_PLL;

    /* Wait for SYSCLK to switch to PLL */
    while (RCC->CR.BIT.SWS != (uint32_t)SYSTEM_CLK_SRC_PLL) {}

    /* Set AHB, APB0(PCLOCK0) and APB1(PCLOCK1) pre-scaler */
    RCC->CFGR.BIT.HPRE = RCC_HPRE_PSC;
    RCC->CFGR.BIT.PPRE0 = RCC_APB0_PSC;
    RCC->CFGR.BIT.PPRE1 = RCC_APB1_PSC;
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

