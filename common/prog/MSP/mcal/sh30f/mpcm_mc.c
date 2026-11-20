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

#ifdef EXT_CRYSTAL_OSC
    #if ((4U <= EXT_CRYSTAL_OSC) && (EXT_CRYSTAL_OSC <= 12U))
        #define HSE_FREQ    EXT_CRYSTAL_OSC
        static int8_t hseFreqValue = EXT_CRYSTAL_OSC;
    #else
        // HXTAL clock needed but not defined correctly
        #error "EXT_CRYSTAL_OSC out of boundaries (4-12MHz)."
    #endif
#else
    #define HSE_FREQ -1 
    static int8_t hseFreqValue = -1;
#endif
//lint -save -e750 "Info 750: local macro 'RCC_CHECK_HCLK' not referenced [MISRA 2012 Rule 2.5, advisory]"
#define RCC_CHECK_PLLCLKIN(freq) ((4u <= (freq)) && ((freq) <= 24u))
#define RCC_CHECK_PRECLK(freq) ((4u <= (freq)) && ((freq) <= 8u))
#define RCC_CHECK_VCOCLK(freq) ((150u <= (freq)) && ((freq) <= 300u))
#define RCC_CHECK_PLLCLKOUT(freq) ((5u <= (freq)) && ((freq) <= 48u))
#define RCC_CHECK_SYSCLK(freq) ((4u <= (freq)) && ((freq) <= 48u))
#define RCC_CHECK_HCLK(freq) ((1u <= (freq)) && ((freq) <= 48u))
#define RCC_CHECK_PCLK(freq) ((1u <= (freq)) && ((freq) <= 24u))
#define RCC_CHECK_HSE() (hseFreqValue > 0)

#define MAX_SYSCLK 48   // max system output clock frequency
#define MIN_SYSCLK 4    // min system clock frequency when using hse as sys, if using pll minimum is 5
#define MAX_PLLCLK 48   // max PLL output clock frequency
#define MIN_PLLCLK 5    // min PLL output clock frequency
#define MAX_PCLOCK 24   // max APB peripheral clock frequency

#define HSI_24MHz 24
#define LSI_128KHz 128000
#define LSI_DIV4_32KHz 32000
#define LSE_32KHz 32768

#define UNIT_MHz 1000000
#define UNIT_KHz 1000

#define PLLXTPRE_OFFSET    1U
#define PLLF_OFFSET        15U
#define PLLK_OFFSET        1U
#define PLLN_MAX    4U
#define PLLN_MIN    1U
#define PLLF_MAX    46U
#define PLLF_MIN    15U
#define PLLK_MAX    8U
#define PLLK_MIN    1U
//lint -restore -e750
/**************************RCC CLOCK Bit Macro******************************/

enum RCC_CR_CLKON
{
    _RCC_CR_HSION = RCC_CR_HSION,   /* Internal High Speed clock enable */
    _RCC_CR_HSEON = RCC_CR_HSEON,   /* External High Speed clock enable */
    _RCC_CR_PLLON = RCC_CR_PLLON,   /* PLL enable */
    _RCC_CR_LSEON = RCC_CR_LSEON,   /* External Low Speed clock enable */
    _RCC_CR_LSION = RCC_CR_LSION,   /* Internal Low Speed clock enable */
};

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
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_48M_PCLK_24M_PLLON = 
{
    .clkFreq =
    {
        .HCLK_Freq = 48,
        .PCLK0_Freq = 24,
        .PCLK1_Freq = 24,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_24M_PCLK_24M_PLLON = 
{
    .clkFreq =
    {
        .HCLK_Freq = 24,
        .PCLK0_Freq = 24,
        .PCLK1_Freq = 24,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_16M_PCLK_16M_PLLON = 
{
    .clkFreq =
    {
        .HCLK_Freq = 16,
        .PCLK0_Freq = 16,
        .PCLK1_Freq = 16,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_8M_PCLK_8M_PLLON = 
{
    .clkFreq =
    {
        .HCLK_Freq = 8,
        .PCLK0_Freq = 8,
        .PCLK1_Freq = 8,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_4M_PCLK_4M_PLLON = 
{
    .clkFreq =
    {
        .HCLK_Freq = 4,
        .PCLK0_Freq = 4,
        .PCLK1_Freq = 4,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};


const struct MPCM_Config MPCM_CFG_CORE_2M_PCLK_2M_PLLON = 
{
    .clkFreq =
    {
        .HCLK_Freq = 2,
        .PCLK0_Freq = 2,
        .PCLK1_Freq = 2,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};


const struct MPCM_Config MPCM_CFG_CORE_1M_PCLK_1M_PLLON = 
{
    .clkFreq =
    {
        .HCLK_Freq = 1,
        .PCLK0_Freq = 1,
        .PCLK1_Freq = 1,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_CORE_24M_PCLK_24M_PLLOFF = 
{
    .clkFreq =
    {
        .HCLK_Freq = 24,
        .PCLK0_Freq = 24,
        .PCLK1_Freq = 24,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_HSICLK,
    .PLL_Src = MPCM_PLL_SRC_NONE,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};

const struct MPCM_Config MPCM_CFG_STOP = 
{
    .clkFreq =
    {
        .HCLK_Freq = (int8_t)CORE_CLOCK,
        .PCLK0_Freq = (int8_t)PCLOCK,
        .PCLK1_Freq = (int8_t)PCLOCK1,
    },
    .SYSCLK_Src = MPCM_SYSCLK_SRC_PLLCLK,
    .PLL_Src = MPCM_PLL_SRC_HSICLK,
    .HSI_ForceEnable = false,
    .LSI_ForceEnable = false,
    .HSE_ForceEnable = false,
    .LSE_ForceEnable = false,
};


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static int32_t MPCM_ActualClockFreqTable[MPCM_CLOCK_END_INDICATOR];

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void MPCM_setSysClockSource(enum MPCM_SYSCLK_Src sysClkSrc);
static void MPCM_setClockFreqTable(enum MPCM_Clock clockSrc, int32_t clockFreq);
static void MPCM_clearClockFreqTable(void);
static void MPCM_setFlashLatencyAndPrefetchForFreq(int8_t hclkFreq);
static void MPCM_configureHSI(const struct MPCM_Config *cfg);
static void MPCM_configureLSI(const struct MPCM_Config *cfg);
static int8_t MPCM_setHCLKPRE(const struct MPCM_Config *cfg);
static int8_t MPCM_setPCLKxPRE(const struct MPCM_Config *cfg, int8_t pclkx);
static int8_t MPCM_configurePLL(const struct MPCM_Config *cfg);
static int32_t MPCM_getClockFreqTable(enum MPCM_Clock clockSrc);
static enum MCAL_Error MPCM_configureHSE(const struct MPCM_Config *cfg);
static enum MCAL_Error MPCM_configureLSE(const struct MPCM_Config *cfg);
static enum MCAL_Error MPCM_configure_SYSCLK_HCLK_PCLK_PLL(const struct MPCM_Config *cfg);

/* RCC Register Setting Functions */
static void RCC_LOCK(void);
static void RCC_UNLOCK(void);
static void RCC_SET_HPRE(uint8_t value);
static void RCC_SET_PPRE0(uint8_t value);
static void RCC_SET_PPRE1(uint8_t value);
static void RCC_CLK_ENABLE(enum RCC_CR_CLKON clkon);
static void RCC_CLK_DISABLE(enum RCC_CR_CLKON clkon);
static void RCC_SET_PLL_SRC(enum MPCM_PLL_Src pllsrc);
static void RCC_SET_PLL_FACTOR(uint8_t n, uint8_t f, uint8_t k);

/* SYSTICK CSR read function */
static bool SYSTICK_USE_EXT_CLK(void);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MPCM_configure(const struct MPCM_Config *config)
{
    if (config != NULL)
    {
        // Configure HCLK to a unrecommended value, return error
        if ((config->clkFreq.HCLK_Freq != 24) && (config->clkFreq.HCLK_Freq != 48) && (config->clkFreq.HCLK_Freq > 12))
        {
            return MCAL_ERROR;
        }

        // Set safe flash latency for highest clock
        MPCM_setFlashLatencyAndPrefetchForFreq(48);

        // Clear clock table
        MPCM_clearClockFreqTable();

        // Enable HSI
        RCC_CLK_ENABLE(_RCC_CR_HSION);

        // Default setting of SYSCLK
        MPCM_setSysClockSource(MPCM_SYSCLK_SRC_HSICLK);

        // Checking LSI HSI config
        MPCM_configureLSI(config);
        MPCM_configureHSI(config);

        // Configure high speed external oscillator if required
        MCAL_RETURN_ON_ERROR(MPCM_configureHSE(config));

        // Configure low speed external oscillator if required
        MCAL_RETURN_ON_ERROR(MPCM_configureLSE(config));
        
        // Configure SYSCLK, HCLK, PCLK, PLL clocks
        MCAL_RETURN_ON_ERROR(MPCM_configure_SYSCLK_HCLK_PCLK_PLL(config));

        // Switch SYSCLOCK source
        MPCM_setSysClockSource(config->SYSCLK_Src);

        // Set flash latency for actual clock
        MPCM_setFlashLatencyAndPrefetchForFreq(config->clkFreq.HCLK_Freq);
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
            SYSCFG_EnterSTOPMode(SYSCFG_SLEEPEntry_WFI);
            break;
        case MPCM_PMODE_SLEEP:
            SYSCFG_EnterSLEEPMode(SYSCFG_SLEEPEntry_WFI);
            break;
        default:
            // not supported power mode
            break;
    }
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
#ifdef MPCM_CURRENTLY_UNUSED_FUNC
static void MPCM_errorHandler(int8_t err)
{
    (void)err;
    for(;;);
}
#endif

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

static void MPCM_setFlashLatencyAndPrefetchForFreq(int8_t hclkFreq)
{
    uint32_t tmpreg;
    uint32_t FLASH_Latency = FLASH_Latency_2;
    uint32_t Prefetch_value = 0u;

    if (hclkFreq <= 12)
    {
        FLASH_Latency = FLASH_Latency_0;
        Prefetch_value = 1u;
    }
    else if ((hclkFreq > 12) && (hclkFreq <= 24))
    {
        FLASH_Latency = FLASH_Latency_2;
        Prefetch_value = 1u;
    }
    else if ((hclkFreq > 24) && (hclkFreq <= 48))
    {
        FLASH_Latency = FLASH_Latency_1;
        Prefetch_value = 0u;
    }
    else
    {
        // should not be here
    }

    /* Clear FLASH_Latency and Prefetch */
    tmpreg = FLASH->ACR.V32 & (~(FLASH_ACR_LATENCY_Msk | FLASH_ACR_PRFTEN_Msk));
    /* Set the Latency cycles and Prefetch according to the new value */
    tmpreg |= (((uint32_t)0x5AA5u << FLASH_ACR_LOCK_Pos) | (FLASH_Latency << FLASH_ACR_LATENCY_Pos) | (Prefetch_value << FLASH_ACR_PRFTEN_Pos));
    /* Store the new value */
    FLASH->ACR.V32 = tmpreg;

}

/* low internal crystal not tested as sysclk */
static void MPCM_configureLSI(const struct MPCM_Config *cfg)
{
    if (cfg->LSI_ForceEnable)
    {
        RCC_CLK_ENABLE(_RCC_CR_LSION);
        MPCM_setClockFreqTable(MPCM_CLOCK_LSICLK, LSI_128KHz);
        MPCM_setClockFreqTable(MPCM_CLOCK_LSICLK_DIV4, LSI_DIV4_32KHz);
    }
    else
    {
        RCC_CLK_DISABLE(_RCC_CR_LSION);
        MPCM_setClockFreqTable(MPCM_CLOCK_LSICLK, 0);
        MPCM_setClockFreqTable(MPCM_CLOCK_LSICLK_DIV4, 0);
    }
}

static void MPCM_configureHSI(const struct MPCM_Config *cfg)
{
    // Disable HSI24MHz if not required and not force enabled
    // When using HSE as sysclk or pllsrc, HSI maybe not necessary
    if ((cfg->PLL_Src != MPCM_PLL_SRC_HSICLK) &&
        (cfg->SYSCLK_Src != MPCM_SYSCLK_SRC_HSICLK) &&
        (!cfg->HSI_ForceEnable) &&
        ((cfg->PLL_Src == MPCM_PLL_SRC_HSECLK) ||
        (cfg->SYSCLK_Src == MPCM_SYSCLK_SRC_HSECLK))
    )
    {
        RCC_CLK_DISABLE(_RCC_CR_HSION);
        MPCM_setClockFreqTable(MPCM_CLOCK_HSICLK, 0);
    }
    else
    {
        RCC_CLK_ENABLE(_RCC_CR_HSION);
        MPCM_setClockFreqTable(MPCM_CLOCK_HSICLK, HSI_24MHz * UNIT_MHz);
    }
}

/* external crystal not tested */
static enum MCAL_Error MPCM_configureHSE(const struct MPCM_Config *cfg)
{
    int8_t HXTAL_Freq;

    // Enable HXTAL if required or force enabled
    if ((cfg->PLL_Src == MPCM_PLL_SRC_HSECLK)
        || (cfg->SYSCLK_Src == MPCM_SYSCLK_SRC_HSECLK)
        || (cfg->HSE_ForceEnable)
    )
    {
        if (!RCC_CHECK_HSE())
        {
            MPCM_setClockFreqTable(MPCM_CLOCK_HSECLK, -1);
            return MCAL_ERROR_PARAM;
        }

        RCC_CLK_ENABLE(_RCC_CR_HSEON);
        HXTAL_Freq = HSE_FREQ;
    }
    else  // HXTAL_NOT_ENABLED_AND_UNUSED
    {
        RCC_CLK_DISABLE(_RCC_CR_HSEON);
        HXTAL_Freq = 0; // disabled
    }

    MPCM_setClockFreqTable(MPCM_CLOCK_HSECLK, HXTAL_Freq * UNIT_MHz);
    MPCM_setClockFreqTable(MPCM_CLOCK_HSECLK_DIV8, HXTAL_Freq * UNIT_MHz / 8);
    return MCAL_OK;
}

/* external crystal not tested */
static enum MCAL_Error MPCM_configureLSE(const struct MPCM_Config *cfg)
{
    // Enable HXTAL if required or force enabled
    if (cfg->LSE_ForceEnable)
    {
        RCC_CLK_ENABLE(_RCC_CR_LSEON);
        MPCM_setClockFreqTable(MPCM_CLOCK_LSECLK, LSE_32KHz);
    }
    else  // HXTAL_NOT_ENABLED_AND_UNUSED
    {
        RCC_CLK_DISABLE(_RCC_CR_LSEON);
        MPCM_setClockFreqTable(MPCM_CLOCK_LSECLK, 0);
    }

    return MCAL_OK;
}

static int8_t MPCM_setHCLKPRE(const struct MPCM_Config *cfg)
{
    // set them as unavailable in case of error
    MPCM_setClockFreqTable(MPCM_CLOCK_HCLK, -1);
    MPCM_setClockFreqTable(MPCM_CLOCK_SYSCLK, -1);
    MPCM_setClockFreqTable(MPCM_CLOCK_SYSTICK, -1);

    uint8_t idx = 255U;
    int8_t hclkFreq = cfg->clkFreq.HCLK_Freq;
    int8_t sysclkFreq;
    enum MPCM_SYSCLK_Src sysclkSource = cfg->SYSCLK_Src;

    if (sysclkSource == MPCM_SYSCLK_SRC_PLLCLK)
    {
        if (hclkFreq < MIN_PLLCLK)
        {
            for (uint8_t i = 1U; i <= 5U; i++)
            {
                if (((int8_t)MIN_PLLCLK <= (hclkFreq * (int8_t)(1U << i))) && 
                    ((int8_t)MAX_PLLCLK >= (hclkFreq * (int8_t)(1U << i))))
                {
                    idx = i;
                    break;
                }
            }
        }
        else if (
            (hclkFreq >= (int8_t)MIN_PLLCLK) &&
            (hclkFreq <= (int8_t)MAX_PLLCLK))
        {
            idx = 0; // div by 1 << 0
        }
        else
        {
            return -1;
        }
    }
    else if (
        (sysclkSource == MPCM_SYSCLK_SRC_HSICLK) ||
        (sysclkSource == MPCM_SYSCLK_SRC_HSECLK)
    )
    {
        // SYSCLK not lower than 4MHz
        // HCLK not higher than SYSCLK_SYS
        if (
            ((sysclkSource == MPCM_SYSCLK_SRC_HSECLK) && (hseFreqValue < MIN_SYSCLK)) ||
            ((sysclkSource == MPCM_SYSCLK_SRC_HSECLK) && (hseFreqValue < hclkFreq)) ||
            ((sysclkSource == MPCM_SYSCLK_SRC_HSICLK) && (HSI_24MHz < hclkFreq))
        )
        {
            return -2;
        }

        for (uint8_t i = 0U; i <= 5U; i++)
        {
            if (
                (((hclkFreq * (int8_t)(1U << i)) == hseFreqValue) && (sysclkSource == MPCM_SYSCLK_SRC_HSECLK)) ||
                (((hclkFreq * (int8_t)(1U << i)) == HSI_24MHz) && (sysclkSource == MPCM_SYSCLK_SRC_HSICLK))
            )
            {
                idx = i;
                break;
            }
        }
    }
    else
    {
        // lse, lsi not supported as sysclk
        return -3;
    }

    // not found valid idx
    if (idx == 255U)
    {
        return -4;
    }

    // setting registers and freq table
    RCC_SET_HPRE(idx);
    sysclkFreq = (hclkFreq * (int8_t)(1U << idx));
    MPCM_setClockFreqTable(MPCM_CLOCK_SYSCLK, sysclkFreq * UNIT_MHz);

    if (SYSTICK_USE_EXT_CLK())
    {
        MPCM_setClockFreqTable(MPCM_CLOCK_SYSTICK, hclkFreq * UNIT_MHz / 8);
    }
    else
    {
        MPCM_setClockFreqTable(MPCM_CLOCK_SYSTICK, hclkFreq * UNIT_MHz);
    }

    MPCM_setClockFreqTable(MPCM_CLOCK_HCLK, hclkFreq * UNIT_MHz);
    return hclkFreq;
}

static int8_t MPCM_setPCLKxPRE(const struct MPCM_Config *cfg, int8_t pclkx)
{
    uint8_t idx = 255U;
    int8_t hclkFreq = cfg->clkFreq.HCLK_Freq;
    int8_t pclkxFreq = (pclkx == 0) ? cfg->clkFreq.PCLK0_Freq : cfg->clkFreq.PCLK1_Freq;
    enum MPCM_Clock pclkxFreqTabIdx = (pclkx == 1) ? MPCM_CLOCK_PCLK0 : MPCM_CLOCK_PCLK1;

    if ((pclkxFreq > MAX_PCLOCK) || (pclkxFreq > hclkFreq))
    {
        MPCM_setClockFreqTable(pclkxFreqTabIdx, -1);
        return -1;
    }

    for (uint8_t i = 0; i <= 4U; i++)
    {
        if (hclkFreq == ((int8_t)(1U << i) * pclkxFreq))
        {
            idx = i;
            break;
        }
    }

    // no matched idx
    if (idx == 255U)
    {
        MPCM_setClockFreqTable(pclkxFreqTabIdx, -1);
        return -2;
    }

    // setting ppre0/ppre1 registers
    (pclkx == 1) ? RCC_SET_PPRE0(idx) : RCC_SET_PPRE1(idx);
    MPCM_setClockFreqTable(pclkxFreqTabIdx, pclkxFreq * UNIT_MHz);
    return pclkxFreq;
}

static enum MCAL_Error MPCM_configure_SYSCLK_HCLK_PCLK_PLL(const struct MPCM_Config *cfg)
{
    int8_t pllFreq = 0;
    int8_t hclkFreq;
    int8_t pclk1Freq;
    int8_t pclk2Freq;
    
    // Set and save HCLK and SYSCLK frequency
    hclkFreq = MPCM_setHCLKPRE(cfg);
    
    // Set and save PCLK0 frequency
    pclk1Freq = MPCM_setPCLKxPRE(cfg, 0);

    // Set and save PCLK1 frequency
    pclk2Freq = MPCM_setPCLKxPRE(cfg, 1);

    // Set and save PLL frequency
    if (cfg->PLL_Src != MPCM_PLL_SRC_NONE)
    {
        pllFreq = MPCM_configurePLL(cfg);
    }

    // Check setting
    if ((hclkFreq  < 0) || (pclk1Freq < 0) ||
        (pclk2Freq < 0) || (pllFreq   < 0))
    {
        return MCAL_ERROR;
    }
    else
    {
        return MCAL_OK;
    }
}

static void MPCM_setSysClockSource(enum MPCM_SYSCLK_Src sysClkSrc)
{
    RCC_UNLOCK();

    RCC->CR.BIT.SW = (uint8_t)sysClkSrc;

    do {
        IWDT->CLR = 0xAAAA; // Feed Dog
    } while (RCC->CR.BIT.SWS != (uint32_t)sysClkSrc);

    RCC_LOCK();
}

static int8_t MPCM_configurePLL(const struct MPCM_Config *cfg)
{
    RCC_CLK_DISABLE(_RCC_CR_PLLON);
    MPCM_setClockFreqTable(MPCM_CLOCK_PLLCLK, -1);
    MPCM_setClockFreqTable(MPCM_CLOCK_PLL_INCLK, -1);
    MPCM_setClockFreqTable(MPCM_CLOCK_PLL_PRECLK, -1);
    MPCM_setClockFreqTable(MPCM_CLOCK_PLL_VCOCLK, -1);
    MPCM_setClockFreqTable(MPCM_CLOCK_PLL_PLLCLK, -1);

    uint8_t pll_n = 0, pll_f = 0, pll_k = 0;
    uint16_t inclk = 0, preclk = 0, pllclk;
    int16_t vcoclk = 0;

    int8_t sysclk = (int8_t)(MPCM_getClockFreqTable(MPCM_CLOCK_SYSCLK) / UNIT_MHz);

    if (sysclk < MIN_PLLCLK)
    {
        return -1;
    }
    else
    {   /* Warning 571: cast from 'int8_t' (aka 'signed char') to 'uint16_t' (aka 'unsigned short') results in sign extension
           sysclk always positive */
        pllclk = (uint16_t)sysclk;//lint !e571
    }

    if (cfg->PLL_Src == MPCM_PLL_SRC_HSICLK)
    {
        inclk = HSI_24MHz;
        // N = 3 or 4, prefer 4
        // in = 8MHz or 6MHz
        for (uint8_t n = 4U; n >= 3U; n--)
        {
            pll_n = n;
            preclk = inclk / pll_n;

            // start searching a valid pair of k & f
            for (uint8_t k = PLLK_MIN; k <= PLLK_MAX; k++)
            {
                if (RCC_CHECK_VCOCLK(pllclk * 4U * k))
                {
                    vcoclk = ((int16_t)pllclk * 4 * (int16_t)k);
                    for (uint8_t f = PLLF_MIN; f <= PLLF_MAX; f++)
                    {
                        // if found combination, stop enumerating at first hit
                        if ((((uint16_t)vcoclk % f) == 0U) && (((uint16_t)vcoclk / f) == preclk))
                        {
                            pll_f = f;
                            pll_k = k;
                            RCC_CLK_ENABLE(_RCC_CR_HSION);
                            RCC_SET_PLL_FACTOR(pll_n, pll_f, pll_k);
                            RCC_SET_PLL_SRC(MPCM_PLL_SRC_HSICLK);
                            RCC_CLK_ENABLE(_RCC_CR_PLLON);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_INCLK, (int32_t)inclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_PRECLK, (int32_t)preclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_VCOCLK, vcoclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_PLLCLK, sysclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLLCLK, sysclk * UNIT_MHz);
                            return sysclk;
                        }
                    }
                    vcoclk = -1;
                }
            }
            // ending of searching
            if ((vcoclk == -1) && (n == 3U))
            {
                return -2;
            }
        }
    }
    // HSECLK will not be tested, since it's not supported by default
    else if (cfg->PLL_Src == MPCM_PLL_SRC_HSECLK)
    {
        #ifdef EXT_CRYSTAL_OSC // 4MHz~12MHz
            inclk = HSE_FREQ;
            // N=1 recommonded if HSE in a valid pll input range
            pll_n = RCC_CHECK_PRECLK(inclk) ? 1U : 2U;

            if (inclk % pll_n == 0)
            {
                preclk = inclk / pll_n;
            }
            else
            {
                return -3;
            }

            if (!RCC_CHECK_PRECLK(preclk))
            {
                return -4;
            }

            // start searching a valid pair of k & f
            for (uint8_t k = PLLK_MIN; k <= PLLK_MAX; k++)
            {
                if (RCC_CHECK_VCOCLK(pllclk * 4U * k))
                {
                    // vcoclk = pllclk * 4U * k;
                    vcoclk = ((int16_t)pllclk * 4 * (int16_t)k);
                    for (uint8_t f = PLLF_MIN; f <= PLLF_MAX; f++)
                    {
                        // if found combination, stop enumerating at first hit
                        if ((((uint16_t)vcoclk % f) == 0U) && (((uint16_t)vcoclk / f) == preclk))
                        {
                            pll_f = f;
                            pll_k = k;
                            RCC_CLK_ENABLE(_RCC_CR_HSEON);
                            RCC_SET_PLL_FACTOR(pll_n, pll_f, pll_k);
                            RCC_SET_PLL_SRC(MPCM_PLL_SRC_HSECLK);
                            RCC_CLK_ENABLE(_RCC_CR_PLLON);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_INCLK, (int32_t)inclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_PRECLK, (int32_t)preclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_VCOCLK, vcoclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLL_PLLCLK, sysclk * UNIT_MHz);
                            MPCM_setClockFreqTable(MPCM_CLOCK_PLLCLK, sysclk * UNIT_MHz);
                            return sysclk;
                        }
                    }
                    vcoclk = -1;
                }
                // searching reached the end, but found no valid pairs
                else if (k == PLLK_MAX)
                {
                    return -5;
                }
                else
                {
                    vcoclk = -1;
                }
            }
        #else
            // if not defined EXT_CRYSTAL_OSC
            // then shouldn't be here
            if (hseFreqValue <= 0)
            {
                return -6;
            }
        #endif
    }
    else
    {
        return -7;
    }
    
    return -8;
}

static void RCC_CLK_ENABLE(enum RCC_CR_CLKON clkon)
{
    RCC_UNLOCK();

    //Enable SYSCFG Module
    RCC->AHBENR.BIT.SYSCFGEN = 1;
    /* Axivion Disable Style MisraC2012-16.3 : This switch-case is not terminated with "break", last statement is return. */
    switch (clkon)
    {
        case _RCC_CR_HSEON:
        {
            SYSCFG_OSCPinConfig(SYSCFG_OSC_HCrystal);
            /* help crystal oscillator starting */
            RCC->CFGR.BIT.HSESEL = 0X00;
            RCC->HSERFB = 0X03;
            break;
        }
        case _RCC_CR_LSEON:
        {
            SYSCFG_OSCPinConfig(SYSCFG_OSC_LCrystal);
            break;
        }
        case _RCC_CR_PLLON:
        {
            // not setting osc pin
            break;
        }
        case _RCC_CR_HSION:
        case _RCC_CR_LSION:
        default:
            SYSCFG_OSCPinConfig(SYSCFG_OSC_GPIO);
            return;
    }
    /* Axivion Enable Style MisraC2012-16.3 */ 
    RCC->CR.V32 |= (uint32_t)clkon;
    /* clkon << 1 is the clkrdy mask */
    do {
        IWDT->CLR = (uint32_t)0xAAAAU; // Feed Dog
    } while ((RCC->CR.V32 & ((uint32_t)clkon << 1)) == 0U);

    RCC_LOCK();
}

static void RCC_CLK_DISABLE(enum RCC_CR_CLKON clkon)
{
    RCC_UNLOCK();
    RCC->CR.V32 &= ~(uint32_t)clkon;
    RCC_LOCK();
}

static void RCC_LOCK(void)
{
    RCC->RCCLOCK = 0x0; //Unlock RCC Config Register
}

static void RCC_UNLOCK(void)
{
    RCC->RCCLOCK = 0x33CC; //Unlock RCC Config Register
}

static void RCC_SET_HPRE(uint8_t value)
{
    if (value > 5U)
    {
        return;
    }

    RCC_UNLOCK();
    RCC->CFGR.BIT.HPRE = value;
    RCC_LOCK();
}

static void RCC_SET_PPRE0(uint8_t value)
{
    if (value > 4U)
    {
        return;
    }

    RCC_UNLOCK();
    RCC->CFGR.BIT.PPRE0 = value;
    RCC_LOCK();
}

static void RCC_SET_PPRE1(uint8_t value)
{
    if (value > 4U)
    {
        return;
    }

    RCC_UNLOCK();
    RCC->CFGR.BIT.PPRE1 = value;
    RCC_LOCK();
}

static void RCC_SET_PLL_SRC(enum MPCM_PLL_Src pllsrc)
{
    if (pllsrc > MPCM_PLL_SRC_HSECLK)
    {
        return;
    }

    RCC_UNLOCK();
    RCC->CFGR.BIT.PLLSRC = (uint8_t)pllsrc;
    RCC_LOCK();
}

/*
    preclk = pllclkin /    N      >>>>>> PLLXTPRE:1~4
    vcoclk = preclk   *    F      >>>>>> PLLF:15~46
    pllclk = vcoclk   / (4 * K)   >>>>>> PLLK:1~8
    pllclk = (pllclkin * F) / (4 * K * N)
*/
static void RCC_SET_PLL_FACTOR(uint8_t n, uint8_t f, uint8_t k)
{
    RCC_UNLOCK();
    RCC->CFGR.BIT.PLLXTPRE = (uint8_t)(n - PLLXTPRE_OFFSET);
    RCC->CFGR.BIT.PLLF = (uint8_t)(f - PLLF_OFFSET);
    RCC->CFGR.BIT.PLLK = (uint8_t)(k - PLLK_OFFSET);
    RCC_LOCK();
}

/*
    [2]	R/W	CLKSOURCE	
    Indicates the SysTick clock source:
    0 = SysTick uses the implementation defined external reference clock.
    1 = SysTick uses the processor clock.
    If no external clock is provided, this bit reads as 1 and ignores writes.
*/
static bool SYSTICK_USE_EXT_CLK(void)
{
    return ((SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) == 0u) ? true : false;
}
