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
/** \file     mpcm_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MPCM.
 *
 *  \details  The clock tree of the CMS32M67xx (Cmsemicon) consists of the
 *            high speed internal oscillator HOCO (up to 72MHz), that is
 *            selected by the user option bytes at 0xC2, and the LSI clock
 *            used by the watchdog.
 *
 */

#ifndef MPCM_MC_H
#define MPCM_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include <stdint.h>
#include <stdbool.h>
#include "mcal/mpcm_types.h"
/*
    Supplemental 891: previous declaration is here
*/
//lint -save -e891 -e9046 -e891
#include "mcal/mdio.h"
//lint -restore -e891 -e9046 -e891

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/* the CMS32M67xx runs the core, the flash and the peripherals from HOCO */
#define MPCM_CORE_CLOCK MPCM_CLOCK_HCLK

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MPCM_PLL_Src
{
    MPCM_PLL_SRC_HSICLK,
    MPCM_PLL_SRC_HSECLK,
    MPCM_PLL_SRC_NONE,
};

enum MPCM_SYSCLK_Src
{
    MPCM_SYSCLK_SRC_HSICLK = 0,
    MPCM_SYSCLK_SRC_PLLCLK,
    MPCM_SYSCLK_SRC_HSECLK,
};


enum MPCM_Clock
{
    MPCM_CLOCK_HSECLK       = 0,
    MPCM_CLOCK_HSECLK_DIV8  = 1,
    MPCM_CLOCK_HSICLK       = 2,
    MPCM_CLOCK_PLLCLK       = 3,
    MPCM_CLOCK_LSECLK       = 4,
    MPCM_CLOCK_LSICLK       = 5,
    MPCM_CLOCK_RC128K       = MPCM_CLOCK_LSICLK,
    MPCM_CLOCK_LSICLK_DIV4  = 6,
    MPCM_CLOCK_RC128K_DIV4  = MPCM_CLOCK_LSICLK_DIV4,
    MPCM_CLOCK_SYSCLK       = 7,
    MPCM_CLOCK_AHB          = 8,
    MPCM_CLOCK_HCLK         = MPCM_CLOCK_AHB,
    MPCM_CLOCK_FCLK         = MPCM_CLOCK_AHB,
    MPCM_CLOCK_APB0         = 9,
    MPCM_CLOCK_PCLK0        = MPCM_CLOCK_APB0,
    MPCM_CLOCK_APB1         = 10,
    MPCM_CLOCK_PCLK1        = MPCM_CLOCK_APB1,
    MPCM_CLOCK_SYSTICK      = 11,
    MPCM_CLOCK_PLL_INCLK,
    MPCM_CLOCK_PLL_PRECLK,
    MPCM_CLOCK_PLL_VCOCLK,
    MPCM_CLOCK_PLL_PLLCLK,
    MPCM_CLOCK_END_INDICATOR,
};


struct MPCM_ClockFreq
{
    int8_t HCLK_Freq;       ///< Core clock (AHB bus)
    int8_t PCLK0_Freq;      ///< Peripheral clock (APB0 bus)
    int8_t PCLK1_Freq;      ///< Peripheral clock (APB1 bus)
};


struct MPCM_Config
{
    struct MPCM_ClockFreq   clkFreq;    ///< Clock tree frequency configuration

    enum MPCM_PLL_Src       PLL_Src;    ///< Source to PLL selection
    enum MPCM_SYSCLK_Src    SYSCLK_Src; ///< Source to SYSCLK selection

    bool HSI_ForceEnable;     ///< Force enable of high speed internal clock
    bool LSI_ForceEnable;     ///< Force enable of low speed internal clock
    bool HSE_ForceEnable;     ///< Force enable of high speed internal clock
    bool LSE_ForceEnable;     ///< Force enable of low speed external clock
};

/*MCO Function not supported by CMS32M67xx.
  To maintain consistent memory size for this struct across C and C++ environments, 
  a padding member is included.*/
struct MPCM_ClockOutputChannel {uint8_t unused;}; 

/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

/* Default configuration based on build parameters */
extern const struct MPCM_Config MPCM_CFG_DEFAULT;


/* Configuration with PLL enabled */
extern const struct MPCM_Config MPCM_CFG_CORE_48M_PCLK_24M_PLLON;
extern const struct MPCM_Config MPCM_CFG_CORE_24M_PCLK_24M_PLLON;
extern const struct MPCM_Config MPCM_CFG_CORE_16M_PCLK_16M_PLLON;
extern const struct MPCM_Config MPCM_CFG_CORE_8M_PCLK_8M_PLLON;
extern const struct MPCM_Config MPCM_CFG_CORE_4M_PCLK_4M_PLLON;
extern const struct MPCM_Config MPCM_CFG_CORE_2M_PCLK_2M_PLLON;
extern const struct MPCM_Config MPCM_CFG_CORE_1M_PCLK_1M_PLLON;


/* Configuration with PLL disabled */
extern const struct MPCM_Config MPCM_CFG_CORE_24M_PCLK_24M_PLLOFF;

/* Configuration for stop mode. No special configuration for sleep is required */
extern const struct MPCM_Config MPCM_CFG_STOP;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif // MPCM_MC_H
