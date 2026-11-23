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
 *  PROCESSOR        CMS32M67xx
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
 *  \details  CMS32M67xx watchdog mapping:
 *
 *             - the watchdog is configured by the user option byte 0xC0:
 *               WDTON (enable), WDCS2..0 (overflow time 2^n / fIL),
 *               WINDOW1..0 (window open period), WDTINT and WDSTBYON
 *             - it is clocked by the low speed internal oscillator
 *               fIL = 15kHz
 *             - the watchdog is serviced by writing WDTE
 *             - the debug behaviour is configured by CGC WDTCFG1
 *
 *            Because the timeout is defined by the option byte, this driver
 *            verifies only the requested timeout against the option byte
 *            configuration of the project (see system_CMS32M67xx.c).
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

/* convert milliseconds to microseconds */
#define MWDT_MS_TO_US                   1000U

/* fIL of the CMS32M67xx in Hz */
#define MWDT_LSI_FREQ_HZ                15000u

/* the option byte WDCS2..0 selects 2^6/fIL ... 2^16/fIL */
#define MWDT_MIN_OVERFLOW_EXPONENT      6u
#define MWDT_MAX_OVERFLOW_EXPONENT      16u

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static const struct MWDT_Config *MWDT_LocalMwdtCfg;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


enum MCAL_Error MWDT_initWDT(const struct MWDT_Config *config)
{
    uint32_t requiredTimeUs;

    MWDT_LocalMwdtCfg = config;

    if((MWDT_LocalMwdtCfg->mode != MWDT_NORMAL_MODE) &&
       (MWDT_LocalMwdtCfg->mode != MWDT_WINDOW_MODE))
    {
        return MCAL_ERROR_PARAM;
    }

    requiredTimeUs = (uint32_t)config->upperTimeLimit * MWDT_MS_TO_US;

    /* the achievable timeout range defined by the option byte 0xC0 */
    if((requiredTimeUs < ((1UL << MWDT_MIN_OVERFLOW_EXPONENT) * MWDT_MS_TO_US / MWDT_LSI_FREQ_HZ)) ||
       (requiredTimeUs > ((1UL << MWDT_MAX_OVERFLOW_EXPONENT) * MWDT_MS_TO_US / MWDT_LSI_FREQ_HZ)))
    {
        return MCAL_ERROR_PARAM;
    }

    if(config->debugConfig == MWDT_DEBUG_STOPPED)
    {
        WDT_DISABLE_DEBUG();
    }
    else
    {
        WDT_ENABLE_DEBUG();
    }

    /* service the watchdog */
    WDT_Restart();

    return MCAL_OK;
}


void MWDT_triggerWDT(void)
{
    /* keep the local pointer alive for the debug configuration */
    if(MWDT_LocalMwdtCfg != NULL)
    {
        WDT_Restart();
    }
}

enum MCAL_Error MWDT_unlock(void)
{
    /* feature not available for CMS32M67xx, the watchdog configuration is
     * done by the user option bytes only */
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
