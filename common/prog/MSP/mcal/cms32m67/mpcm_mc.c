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
 *  COMP_ABBREV      MPCM
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mpcm_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MPCM.
 *
 *  \details  Clock and power management mapping for the CMS32M67xx:
 *
 *             - the core clock is derived from the high speed on-chip
 *               oscillator HOCO, the frequency itself is selected by the user
 *               option byte 0xC2 (FRQSEL), the flash access time is switched
 *               accordingly by the option byte logic
 *             - the peripheral clocks are equal to the core clock
 *             - low power modes: SLEEP (WFI) and DEEP SLEEP are supported
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mpcm_mc.h"
#include "mpcm.h"
#include "mpcm_priv.h"

#ifdef MCAL_MPCM_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"

/*
    Tested worked.
    Note 9033: cannot cast 'unsigned8' to different essential type 'signed8'
*/
//lint -save -e801 -e923 -e9033

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define UNIT_MHz 1000000

/* valid HOCO frequencies of the CMS32M67xx (see user option byte 0xC2) */
#define CMS32M67_HOCO_72MHz     72
#define CMS32M67_HOCO_36MHz     36
#define CMS32M67_HOCO_24MHz     24
#define CMS32M67_HOCO_12MHz     12
#define CMS32M67_HOCO_6MHz      6
#define CMS32M67_HOCO_64MHz     64
#define CMS32M67_HOCO_32MHz     32
#define CMS32M67_HOCO_16MHz     16
#define CMS32M67_HOCO_8MHz      8
#define CMS32M67_HOCO_4MHz      4
#define CMS32M67_HOCO_2MHz      2
#define CMS32M67_HOCO_1MHz      1

#define CMS32M67_LSI_15KHz     15000

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

const struct MPCM_Config MPCM_CFG_DEFAULT = 
{
    .clkFreq =
    {
        .HCLK_Freq = (int8_t)CORE_CLOCK,
        .PCLK0_Freq = (int8_t)PCLOCK,
        .PCLK1_Freq = (int8_t)PCLOCK1,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_48M_PCLK_24M_PLLON = 
{
    .clkFreq = { .HCLK_Freq = 48, .PCLK0_Freq = 24, .PCLK1_Freq = 24 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_24M_PCLK_24M_PLLON = 
{
    .clkFreq = { .HCLK_Freq = 24, .PCLK0_Freq = 24, .PCLK1_Freq = 24 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_16M_PCLK_16M_PLLON = 
{
    .clkFreq = { .HCLK_Freq = 16, .PCLK0_Freq = 16, .PCLK1_Freq = 16 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_8M_PCLK_8M_PLLON = 
{
    .clkFreq = { .HCLK_Freq = 8, .PCLK0_Freq = 8, .PCLK1_Freq = 8 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_4M_PCLK_4M_PLLON = 
{
    .clkFreq = { .HCLK_Freq = 4, .PCLK0_Freq = 4, .PCLK1_Freq = 4 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_2M_PCLK_2M_PLLON = 
{
    .clkFreq = { .HCLK_Freq = 2, .PCLK0_Freq = 2, .PCLK1_Freq = 2 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_1M_PCLK_1M_PLLON = 
{
    .clkFreq = { .HCLK_Freq = 1, .PCLK0_Freq = 1, .PCLK1_Freq = 1 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_24M_PCLK_24M_PLLOFF = 
{
    .clkFreq = { .HCLK_Freq = 24, .PCLK0_Freq = 24, .PCLK1_Freq = 24 },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_STOP = 
{
    .clkFreq =
    {
        .HCLK_Freq = (int8_t)CORE_CLOCK,
        .PCLK0_Freq = (int8_t)PCLOCK,
        .PCLK1_Freq = (int8_t)PCLOCK1,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = true, .LSI_ForceEnable = false,
    .HSE_ForceEnable = false, .LSE_ForceEnable = false,
};


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static int32_t MPCM_ActualClockFreqTable[MPCM_CLOCK_END_INDICATOR];

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void MPCM_setClockFreqTable(enum MPCM_Clock clockSrc, int32_t clockFreq);
static void MPCM_clearClockFreqTable(void);
static int32_t MPCM_getClockFreqTable(enum MPCM_Clock clockSrc);
static bool MPCM_isValidHocoFreq(int8_t hclkFreq);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MPCM_configure(const struct MPCM_Config *config)
{
    if(config != NULL)
    {
        if(!MPCM_isValidHocoFreq(config->clkFreq.HCLK_Freq))
        {
            return MCAL_ERROR;
        }

        MPCM_clearClockFreqTable();

        /* the CMS32M67xx clock tree is defined by the user option bytes, the
         * actual frequencies are taken from the system driver */
        SystemCoreClockUpdate();

        MPCM_setClockFreqTable(MPCM_CLOCK_HSICLK, (int32_t)SystemCoreClock);
        MPCM_setClockFreqTable(MPCM_CLOCK_SYSCLK, (int32_t)SystemCoreClock);
        MPCM_setClockFreqTable(MPCM_CLOCK_HCLK, (int32_t)SystemCoreClock);
        MPCM_setClockFreqTable(MPCM_CLOCK_PCLK0, (int32_t)SystemAPBClock);
        MPCM_setClockFreqTable(MPCM_CLOCK_PCLK1, (int32_t)SystemAPBClock);
        MPCM_setClockFreqTable(MPCM_CLOCK_SYSTICK, (int32_t)SystemCoreClock);

        if(config->LSI_ForceEnable)
        {
            MPCM_setClockFreqTable(MPCM_CLOCK_LSICLK, CMS32M67_LSI_15KHz);
        }
    }

    return MCAL_OK;
}

/*
    return clock frequency in Hz
*/
uint32_t MPCM_getClockFreq(enum MPCM_Clock clockSrc)
{
    int32_t freq = MPCM_getClockFreqTable(clockSrc);
    // Check if clock bus does not have error (-1)
    return (freq > 0) ? ((uint32_t)freq) : 0u;
}

void MPCM_setPowerModeReg(enum MPCM_PowerMode powerMode)
{
    switch (powerMode)
    {
        case MPCM_PMODE_RESET:
        case MPCM_PMODE_RUN:
        case MPCM_PMODE_LPRUN:
            break;
        case MPCM_PMODE_STOP:
            /* CMS32M67xx deep sleep mode, wake up by interrupt only */
            CGC->PMUCTL = 0x01;
            __WFI();
            CGC->PMUCTL = 0x00;
            break;
        case MPCM_PMODE_SLEEP:
            __WFI();
            break;
        default:
            // not supported power mode
            break;
    }
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static int32_t MPCM_getClockFreqTable(enum MPCM_Clock clockSrc)
{
    return MPCM_ActualClockFreqTable[(uint8_t)clockSrc];
}

static void MPCM_setClockFreqTable(enum MPCM_Clock clockSrc, int32_t clockFreq)
{
    MPCM_ActualClockFreqTable[(uint8_t)clockSrc] = clockFreq;
}

static void MPCM_clearClockFreqTable(void)
{
    for (uint8_t i = 0; i < (uint8_t)MPCM_CLOCK_END_INDICATOR; i++)
    {
        MPCM_ActualClockFreqTable[i] = 0;
    }
}

static bool MPCM_isValidHocoFreq(int8_t hclkFreq)
{
    bool valid;

    if((hclkFreq == CMS32M67_HOCO_72MHz) || (hclkFreq == CMS32M67_HOCO_36MHz) ||
       (hclkFreq == CMS32M67_HOCO_24MHz) || (hclkFreq == CMS32M67_HOCO_12MHz) ||
       (hclkFreq == CMS32M67_HOCO_6MHz)  || (hclkFreq == CMS32M67_HOCO_64MHz) ||
       (hclkFreq == CMS32M67_HOCO_32MHz) || (hclkFreq == CMS32M67_HOCO_16MHz) ||
       (hclkFreq == CMS32M67_HOCO_8MHz)  || (hclkFreq == CMS32M67_HOCO_4MHz)  ||
       (hclkFreq == CMS32M67_HOCO_2MHz)  || (hclkFreq == CMS32M67_HOCO_1MHz))
    {
        valid = true;
    }
    else
    {
        valid = false;
    }

    return valid;
}
