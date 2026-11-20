/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MWDT
 *  PROCESSOR        SH30F9X71
 ******************************************************************************/
 

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mwdt_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MWDT.
 *
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "mwdt.h"
#include "mwdt_mc.h"
#include "mwdt_priv.h"
#include "math/mathDiv.h"


#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

// Predefined macros for IWDG prescaler 
#define MWDT_IWDT_PRESCALER_512         7U
// Predefined macros for WWDG prescaler 
#define MWDT_WWDG_PRESCALER_128         7U
// Convert seconds to milliseconds
#define MWDT_SEC_TO_MS                  1000U
// Convert milliseconds to microseconds
#define MWDT_MS_TO_US                   1000U
// The minimum prescaler value for IWDG
#define MWDT_IWDT_MIN_PRESCALER         4u
// The minimum prescaler value for WWDG
#define MWDG_MIN_PRESCALER_WWDG         1u
// LSI frequency in kHz
#define MWDT_IWDT_LSI_IN_KHZ            128u
// Max ticks in independent watch dog
#define MWDT_IWDT_MAX_TICKS             0xFFFU
// Max ticks in window watch dog
#define MWDT_WWDG_MAX_TICKS             0xFFU
// Key to trigger the iwdt
#define MWDT_IWDT_KEY_TO_TRIGGER        0xAAAAU
// Key to trigger the wwdt
#define MWDT_WWDT_KEY_TO_TRIGGER        0x5555U


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static const struct MWDT_Config *MWDT_LocalMwdtCfg;

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

//Configures Independent Watchdog
static enum MCAL_Error MWDT_initIWDG(const struct MWDT_Config *config);

//Configures Window Watchdog
static enum MCAL_Error MWDT_initWWDG(const struct MWDT_Config *config);

//Returns calculated time in us for Independent Watchdog from virtual registers
static uint32_t MWDT_getActualTimeUsIWDG(uint32_t reloadValue, uint32_t prescaler);
//Returns calculated time in us for Window Watchdog from virtual registers
static uint32_t MWDT_getActualTimeUsWWDG(uint32_t reloadValue, uint32_t prescaler);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


enum MCAL_Error MWDT_initWDT(const struct MWDT_Config *config)
{
    MWDT_LocalMwdtCfg = config;
    if (MWDT_LocalMwdtCfg->mode == MWDT_NORMAL_MODE)
    {
        //activate independent watchdog mode IWDG
        return(MWDT_initIWDG(MWDT_LocalMwdtCfg));
    }
    else if (MWDT_LocalMwdtCfg->mode == MWDT_WINDOW_MODE)
    {
        //activate window watchdog mode WWDG
        return(MWDT_initWWDG(MWDT_LocalMwdtCfg));
    }
    else
    {
        return MCAL_ERROR_PARAM;
    }
}


void MWDT_triggerWDT(void)
{
    // Trigger independent Watchdog IWDG
    IWDT->CLR = MWDT_IWDT_KEY_TO_TRIGGER;
    if (MWDT_LocalMwdtCfg->mode == MWDT_WINDOW_MODE)
    {
        // Reload window watchdog value
        WWDT->CLR = MWDT_WWDT_KEY_TO_TRIGGER;
    }
}

enum MCAL_Error MWDT_unlock(void)
{
    // feature not available for SH30F9X71
    return MCAL_OK;
}

void MWDT_initConfigStructMWDT(void)
{
    /* If MWDT_init() is not called, variable "MWDT_LocalMwdtCfg" is pointer which is not initialized
       and it causes HardFault because it points out of memory.
       Therefore "mwdtCfg" is created as default configuration. */
    static const struct MWDT_Config mwdtCfg =
    {
        MWDT_NORMAL_MODE,
        MWDT_100MS_UPPER_TIMELIMIT,
        MWDT_100MS_LOWER_TIMELIMIT,
        MWDT_DEBUG_STOPPED
    };

    MWDT_LocalMwdtCfg = &mwdtCfg;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static enum MCAL_Error MWDT_initIWDG(const struct MWDT_Config *config)
{
    uint32_t max_timeout_ms;         // maximum achievable timeout with current clock
    uint32_t min_timeout_ms;         // minimum achievable timeout with current clock
    uint8_t prescalerCorrection = 0U;
    uint16_t percent;
    uint32_t prescaler;
    uint32_t reloadValue;
    uint32_t actualTimeUs;
     /* Calculate maximum IWDG timeout achievable at current clock */
    max_timeout_ms = (((MWDT_IWDT_MIN_PRESCALER << MWDT_IWDT_PRESCALER_512) * MWDT_IWDT_MAX_TICKS ) / MWDT_IWDT_LSI_IN_KHZ);
    /* Calculate minimum IWDG tick (step) achievable at current clock */
    min_timeout_ms = (max_timeout_ms >> MWDT_IWDT_PRESCALER_512) / MWDT_IWDT_MAX_TICKS;
    min_timeout_ms = (min_timeout_ms < 1u) ? 1u : min_timeout_ms;

    if ((config->upperTimeLimit > max_timeout_ms) || (config->upperTimeLimit < min_timeout_ms))
    {
        /* Required watchdog timeout is not achievable with current PCLOCK */
        return MCAL_ERROR_PARAM;
    }
    // Get the most precise (lowest) prescaler settings
    while ((config->upperTimeLimit <= (max_timeout_ms >> 1)) && (prescalerCorrection < MWDT_IWDT_PRESCALER_512))
    {
        prescalerCorrection++;                 // decreasing prescaler
        max_timeout_ms = max_timeout_ms >> 1;  // timeout divided by 2
    }

    /* Calculate IWDG_reload_value according required IWDG timeout */
    reloadValue = (uint32_t)(((uint32_t)config->upperTimeLimit * MWDT_IWDT_MAX_TICKS) / max_timeout_ms);
    /* Calculate IWDG counter prescaler */
    prescaler   = (uint32_t)MWDT_IWDT_PRESCALER_512 - prescalerCorrection; 

    actualTimeUs = MWDT_getActualTimeUsIWDG(reloadValue, prescaler);
    percent = MWDT_getPrecisionInPercent(actualTimeUs, config->upperTimeLimit);
    if(MWDT_PRECISION_LIMIT_CHECK < percent)
    {
        return MCAL_ERROR_PRECISION;
    }
    /* Write the IWDG prescaler and reload register*/
    IWDT->CR.V32 = (((uint32_t)IWDT_WriteAccess_Enable << IWDT_CR_LOCK_Pos) \
                | (prescaler << IWDT_CR_IWDTPR_Pos) \
                | (reloadValue << IWDT_CR_IWDTRLR_Pos));

    /* reload the watch dog cnt register */
    IWDT->CLR = MWDT_IWDT_KEY_TO_TRIGGER;
    if (config->debugConfig == MWDT_DEBUG_STOPPED)
    {
        /* Stop IWDG counter clock if core is halted */
        SYSCFG->DBGCR.V32 |= ((uint32_t)IWDT_WriteAccess_Enable << 16) | DBG_Periph_IWDT;
    }
    return MCAL_OK;
}

static enum MCAL_Error MWDT_initWWDG(const struct MWDT_Config *config)
{
    uint32_t Periph_Clock_Hz;
    uint32_t max_timeout_ms;         // maximum achievable timeout with current clock
    uint32_t min_timeout_ms;         // minimum achievable timeout with current clock
    uint32_t windowValue;            // reload value for WWDG
    uint16_t percent;
    uint32_t prescaler;
    uint32_t actualTimeUs;
    uint32_t reloadValue;
    uint8_t prescalerCorrection = 0U;

    /* Get WWDG clock, WWDG clock = PCLK /256 */
  #ifdef MCAL_MPCM_INCLUDED
    Periph_Clock_Hz = (uint32_t)(MPCM_getClockFreq(MPCM_CLOCK_PCLK0)) >> 8;
  #else
    Periph_Clock_Hz = (PCLOCK * 1000000u) >> 8;
  #endif
    /* Calculate maximum WWDG timeout achievable at current clock */
    max_timeout_ms = (((1U << MWDT_WWDG_PRESCALER_128) * MWDT_WWDG_MAX_TICKS * MWDT_SEC_TO_MS) / Periph_Clock_Hz);
    /* Calculate minimum WWDG tick (step) achievable at current clock */
    min_timeout_ms = (max_timeout_ms >> MWDT_WWDG_PRESCALER_128) / MWDT_WWDG_MAX_TICKS;
    min_timeout_ms = (min_timeout_ms < 1u) ? 1u : min_timeout_ms;

    if ((config->upperTimeLimit > max_timeout_ms) || (config->upperTimeLimit < min_timeout_ms))
    {
        /* Required watchdog timeout is not achievable with current PCLOCK */
        return MCAL_ERROR_PARAM;
    }

    // Get the most precise (lowest) prescaler settings
    while ((config->upperTimeLimit <= (max_timeout_ms >> 1)) && (prescalerCorrection < MWDT_WWDG_PRESCALER_128))
    {
        prescalerCorrection++;                 // decreasing prescaler
        max_timeout_ms = max_timeout_ms >> 1;  // timeout divided by 2
    }

    /* Calculate IWDG_reload_value according required IWDG timeout */
    reloadValue = (uint32_t)(((uint32_t)config->upperTimeLimit * MWDT_WWDG_MAX_TICKS) / max_timeout_ms);
    /* Calculate IWDG counter prescaler */
    prescaler   = (uint32_t)MWDT_WWDG_PRESCALER_128 - prescalerCorrection; 
    /* Calculate windowValue according required WWDG window timeout */
    windowValue = reloadValue - (((uint32_t)config->lowerTimeLimit * MWDT_WWDG_MAX_TICKS) / max_timeout_ms);

    actualTimeUs = MWDT_getActualTimeUsWWDG(reloadValue,prescaler);
    percent = MWDT_getPrecisionInPercent(actualTimeUs, config->upperTimeLimit);
    if(MWDT_PRECISION_LIMIT_CHECK < percent)
    {
        return MCAL_ERROR_PRECISION;
    }
    /* Write the IWDG prescaler and reload register to the max value*/
    IWDT->CR.V32 = (((uint32_t)IWDT_WriteAccess_Enable << IWDT_CR_LOCK_Pos) \
                | ((uint32_t)MWDT_IWDT_PRESCALER_512 << IWDT_CR_IWDTPR_Pos) \
                | ((uint32_t)MWDT_IWDT_MAX_TICKS << IWDT_CR_IWDTRLR_Pos));
    /* reload the watch dog cnt register */
    IWDT->CLR = MWDT_IWDT_KEY_TO_TRIGGER;
    /* enable clock */
    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();
    RCC->APB0ENR.V32 |= (uint32_t)RCC_APB0_WWDT;
    /* Lock RCC registers */
    RCC_REGS_LOCK();
    WWDT->CR.V32 = (((uint32_t)IWDT_WriteAccess_Enable << WWDT_CR_LOCK_Pos) \
                   | ((uint32_t)0x0U << WWDT_CR_WWDTIE_Pos) \
                   | ((uint32_t)0x1U << WWDT_CR_WWDTON_Pos) \
                   | (prescaler << WWDT_CR_WWDTPR_Pos) | (reloadValue & 0xFFU));

    /* Set WWDTWTR[7:0] bits according to WindowValue value */
    WWDT->WTR.V32 = (((uint32_t)IWDT_WriteAccess_Enable << WWDT_WTR_LOCK_Pos) | windowValue);
    if (config->debugConfig == MWDT_DEBUG_STOPPED)
    {
        /* Stop WWDG counter clock if core is halted */
        SYSCFG->DBGCR.V32 |= ((uint32_t)IWDT_WriteAccess_Enable << 16) | DBG_Periph_WWDT;
        /* Stop IWDG counter clock if core is halted */
        SYSCFG->DBGCR.V32 |= ((uint32_t)IWDT_WriteAccess_Enable << 16) | DBG_Periph_IWDT;
    }
    return MCAL_OK;
}


static uint32_t MWDT_getActualTimeUsIWDG(uint32_t reloadValue, uint32_t prescaler)
{
    uint8_t  iwdgPrescalerReg;
    uint32_t iwdgPrescaler;
    uint32_t iwdgTimeMs;
    // Possible setting for IWDG prescaler: 4, 8, 16, 32, 64, 128, 256 and 512
    iwdgPrescalerReg = (uint8_t)prescaler;
    iwdgPrescaler = MWDT_IWDT_MIN_PRESCALER;    // 4u
    while(iwdgPrescalerReg)
    {
        iwdgPrescaler <<= 1u;
        iwdgPrescalerReg--;
    }
    iwdgPrescaler *= reloadValue;
    iwdgTimeMs = MTDIV_div_32_32(iwdgPrescaler, MWDT_IWDT_LSI_IN_KHZ);
    return iwdgTimeMs * MWDT_MS_TO_US;
}

static uint32_t MWDT_getActualTimeUsWWDG(uint32_t reloadValue, uint32_t prescaler)
{
    uint8_t  wwdgTimerBase;
    uint32_t wwdgPrescaler;
    uint32_t wwdgTimeUs;
  #ifdef MCAL_MPCM_INCLUDED
    uint32_t wwdgClockMHz = MTDIV_div_32_32(MPCM_getClockFreq(MPCM_CLOCK_PCLK0), 1000000u);
  #else
    uint32_t wwdgClockMHz = PCLOCK;
  #endif

    wwdgTimerBase = (uint8_t)prescaler;
    // Possible setting for WWDG prescaler: 1, 2, 4, 8, 16, 32, 64 and 128
    wwdgPrescaler = MWDG_MIN_PRESCALER_WWDG;
    while(wwdgTimerBase)
    {
        wwdgPrescaler <<= 1u;
        wwdgTimerBase--;
    }
    wwdgPrescaler <<= 8u;    // wwdgPrescaler * 256
    wwdgPrescaler *= reloadValue;
    wwdgTimeUs     = MTDIV_div_32_32(wwdgPrescaler, wwdgClockMHz);
    return wwdgTimeUs;
}
