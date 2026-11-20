/**
  ******************************************************************************
  * @file    sh30f9xx_sa0_rcc.c
  * @author  sinowealth
  * @version V1.1.0
  * @date    2020-11-25  
  * @brief   This file provides reset and clock module's APIs
  *         
  *  @verbatim
  *
  *          ===================================================================
  *                                   How to use this driver
  *          ===================================================================
  *          
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * SINOWEALTH IS SUPPLYING THIS SOFTWARE FOR USE EXCLUSIVELY SH_ON SINOWEALTH'S 
  * MICROCONTROLLER PRODUCTS. IT IS PROTECTED UNDER APPLICABLE COPYRIGHT LAWS. 
  * THIS SOFTWARE IS FOR GUIDANCE IN ORDER TO SAVE TIME. AS A RESULT, SINOWEALTH 
  * SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES 
  * WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR
  * THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN 
  * CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2020 Sinowealth</center></h2>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sh30f9xx_sa0_rcc.h"
#include "system_sh30f9xx_sa0.h"

/** @addtogroup SH30F9xx_sa0_libcfg_StdLib_Driver
  * @{
  */

/** @addtogroup RCC_MODULE RCC
  *  RCC driver Mode
  * @{
  */

/** @defgroup RCC_Group_Pub_Funcs  Public Functions
  * @brief   RCC Public Functions
  * @{
  */

/**
  ******************************************************************************
  * @function  RCC_GetClocksFreq
  * @brief     Returns the frequencies of different on chip clocks.
  * @version   V1.1.0
  * @date      25-November-2020
  * @param     Clocks: pointer to a RCC_Clocks_TypeDef structure which will hold
  *            the clocks frequencies.
  * @retval    None
  ******************************************************************************
  */
void RCC_GetClocksFreq(RCC_Clocks_TypeDef *Clocks)
{
    uint8_t tmp = 0;
    uint8_t pllmul_k = 0;
    uint8_t pllmul_f = 0;
    uint8_t pllxtpre = 0;
    uint8_t pllsrc = 0;
    uint8_t hpre_div = 0;
    uint32_t sysClk = 0;

    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CR.BIT.SWS;
    hpre_div = RCC->CFGR.BIT.HPRE;

    switch (tmp)
    {
    case RCC_SYS_SRC_HSI: /* HSI used as system clock */
        sysClk = HSI_VALUE;
        break;
    case RCC_SYS_SRC_PLL: /* PLL used as system clock */
        pllsrc = RCC->CFGR.BIT.PLLSRC;
        pllmul_f = RCC->CFGR.BIT.PLLF + 15;
        pllmul_k = RCC->CFGR.BIT.PLLK + 1;
        pllxtpre = RCC->CFGR.BIT.PLLXTPRE + 1;
        if (pllsrc) //HSE as PLL Clock input
        {
            sysClk = HSE_VALUE / pllxtpre * pllmul_f / pllmul_k / 4;
        }
        else //HSI as PLL Clock input
        {
            sysClk = HSI_VALUE / pllxtpre * pllmul_f / pllmul_k / 4;
        }
        if (RCC->CISTR.BIT.PLLCSMF)
        {
            sysClk = HSI_VALUE;
        }
        break;
    case RCC_SYS_SRC_LSE: /* LSE used as system clock */
        if (RCC->CISTR.BIT.LSECSMF)
        {
            sysClk = LSI_VALUE_DIV4;
        }
        else
        {
            sysClk = LSE_VALUE;
        }
        break;
    case RCC_SYS_SRC_HSE: /* HSE used as system clock */
        if (RCC->CISTR.BIT.HSECSMF)
        {
            sysClk = HSI_VALUE;
        }
        else
        {
            sysClk = HSE_VALUE;
        }
        break;
    case RCC_SYS_SRC_LSI: /* LSI used as system clock */
        sysClk = LSI_VALUE;
        break;
    case RCC_SYS_SRC_LSI_DIV4: /* LSI DIV4 used as system clock */
        sysClk = LSI_VALUE_DIV4;
        break;
    default: /* HSI used as system clock */
        sysClk = HSI_VALUE;
        break;
    }

    Clocks->sysFreq = sysClk;

    //Calculate HCLK
    switch (hpre_div)
    {
    case 0:
        Clocks->hclkFreq = sysClk;
        break;
    case 1:
        Clocks->hclkFreq = sysClk / 2;
        break;
    case 2:
        Clocks->hclkFreq = sysClk / 4;
        break;
    case 3:
        Clocks->hclkFreq = sysClk / 8;
        break;
    case 4:
        Clocks->hclkFreq = sysClk / 16;
        break;
    case 5:
        Clocks->hclkFreq = sysClk / 32;
        break;
    default:
        Clocks->hclkFreq = sysClk / 32;
        break;
    }

    /*Calculate PCLK0 PCLK1*/
    Clocks->pclk0Freq = Clocks->hclkFreq >> (RCC->CFGR.BIT.PPRE0 > 4 ? 4 : (RCC->CFGR.BIT.PPRE0));
    Clocks->pclk1Freq = Clocks->hclkFreq >> (RCC->CFGR.BIT.PPRE1 > 4 ? 4 : (RCC->CFGR.BIT.PPRE1));
}

/**
  * @brief     open or close AHB modules' clock gate
  * @param  AHBModules:  AHB module bits @ref RCC_AHB_Type
  *     This parameter can be any combination of following values:
  *     @arg @b  RCC_AHB_SYSCFG:  for SYSCFG Module
  *     @arg @b  RCC_AHB_CRC   :  for CRC Module
  *     @arg @b  RCC_AHB_DMA   :  for DMA Config Module
  *@param OnOffState  open or close related clock gate
  *     @arg @b  SH_ON  open related clock gate
  *     @arg @b  SH_OFF  close related clock gate
  * @retval   None
   */
void RCC_AHBPeriphClockOnOff(RCC_AHB_Type AHBModules, CmdState OnOffState)
{
    assert_param(IS_AHB_MODULES(AHBModules));
    assert_param(IS_CMD_STATE(OnOffState));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    if (OnOffState == SH_ON)
    {
        RCC->AHBENR.V32 |= AHBModules;
    }
    else
    {
        RCC->AHBENR.V32 &= ~AHBModules;
    }

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief  open or close APB1 modules' clock gate
  * @param  APB1Modules:  APB1 module bits @ref RCC_APB1_Type
  *   This parameter can be any combination of following values:
  *     @arg @b RCC_APB1_UART0:    for UART0 Module
  *     @arg @b RCC_APB1_UART1:    for UART1 Module 
  *     @arg @b RCC_APB1_UART2:    for UART2 Module 
  *     @arg @b RCC_APB1_UART3:    for UART3 Module
  *     @arg @b RCC_APB1_SPI0 :    for SPI0  Module
  *     @arg @b RCC_APB1_SPI1 :    for SPI1  Module
  *     @arg @b RCC_APB1_TWI0 :    for TWI0  Module
  *     @arg @b RCC_APB1_LED  :    for LED   Module
  *     @arg @b RCC_APB1_LCD  :    for LCD   Module
  *     @arg @b RCC_APB1_TK   :    for TK    Module
  *     @arg @b RCC_APB1_HLV  :    for HLV   Module
  *     @arg @b RCC_APB1_NTEST:    for NTEST Module
  *@param OnOffState  open or close related clock gate
  *     @arg @b  SH_ON  open related clock gate
  *     @arg @b  SH_OFF  close related clock gate
  * @retval  None
  */
void RCC_APB1PeriphClockOnOff(RCC_APB1_Type APB1Modules, CmdState OnOffState)
{
    assert_param(IS_APB1_MODULES(APB1Modules));
    assert_param(IS_CMD_STATE(OnOffState));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    if (OnOffState == SH_ON)
    {
        RCC->APB1ENR.V32 |= APB1Modules;
    }
    else
    {
        RCC->APB1ENR.V32 &= ~APB1Modules;
    }

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief      open or close APB0 modules' clock gate
  * @param  APB0Modules:  APB0 module bits @ref RCC_APB0_Type
  *   This parameter can be any combination of following values:
  *     @arg @b    RCC_APB0_TIM0: for TIM0 Module
  *     @arg @b    RCC_APB0_TIM1: for TIM1 Module
  *     @arg @b    RCC_APB0_TIM2: for TIM2 Module
  *     @arg @b    RCC_APB0_TIM3: for TIM3 Module
  *     @arg @b    RCC_APB0_PWM0: for PWM0 Module
  *     @arg @b    RCC_APB0_PWM1: for PWM1 Module
  *     @arg @b    RCC_APB0_PWM2: for PWM2 Module
  *     @arg @b    RCC_APB0_PWM3: for PWM3 Module
  *     @arg @b    RCC_APB0_PCA0: for PCA0 Module
  *     @arg @b    RCC_APB0_PCA1: for PCA1 Module
  *     @arg @b    RCC_APB0_PCA2: for PCA2 Module
  *     @arg @b    RCC_APB0_PCA3: for PCA3 Module
  *     @arg @b    RCC_APB0_EXTI: for EXTI Module
  *     @arg @b    RCC_APB0_WWDT: for WWDT Module
  *     @arg @b    RCC_APB0_ADC : for ADC  Module
  *@param OnOffState  open or close related clock gate
  *     @arg @b  SH_ON  open related clock gate
  *     @arg @b  SH_OFF  close related clock gate
  * @retval None
  */
void RCC_APB0PeriphClockOnOff(RCC_APB0_Type APB0Modules, CmdState OnOffState)
{
    assert_param(IS_APB0_MODULES(APB0Modules));
    assert_param(IS_CMD_STATE(OnOffState));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    if (OnOffState == SH_ON)
    {
        RCC->APB0ENR.V32 |= APB0Modules;
    }
    else
    {
        RCC->APB0ENR.V32 &= ~APB0Modules;
    }

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief     reset AHB modules
  * @param  AHBModules: AHB module bits @ref RCC_AHB_Type
  *   This parameter can be any combination of following values:
  *     @arg @b  RCC_AHB_SYSCFG:  for SYSCFG Module
  *     @arg @b  RCC_AHB_CRC   :  for CRC Module
  *     @arg @b  RCC_AHB_DMA   :  for DMA Config Module
  * @retval   None
  */
void RCC_AHBPeriphReset(uint32_t AHBModules)
{
    assert_param(IS_AHB_MODULES(AHBModules));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->AHBRSTR.V32 = AHBModules;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief    reset APB0 modules
  * @param  APB0Modules:  APB0 module bits @ref RCC_APB0_Type
  *   This parameter can be any combination of following values:
  *     @arg @b  RCC_APB0_TIM0: for TIM0 Module
  *     @arg @b  RCC_APB0_TIM1: for TIM1 Module
  *     @arg @b  RCC_APB0_TIM2: for TIM2 Module
  *     @arg @b  RCC_APB0_TIM3: for TIM3 Module
  *     @arg @b  RCC_APB0_PWM0: for PWM0 Module
  *     @arg @b  RCC_APB0_PWM1: for PWM1 Module
  *     @arg @b  RCC_APB0_PWM2: for PWM2 Module
  *     @arg @b  RCC_APB0_PWM3: for PWM3 Module
  *     @arg @b  RCC_APB0_PCA0: for PCA0 Module
  *     @arg @b  RCC_APB0_PCA1: for PCA1 Module
  *     @arg @b  RCC_APB0_PCA2: for PCA2 Module
  *     @arg @b  RCC_APB0_PCA3: for PCA3 Module
  *     @arg @b  RCC_APB0_EXTI: for EXTI Module
  *     @arg @b  RCC_APB0_WWDT: for WWDT Module
  *     @arg @b  RCC_APB0_ADC : for ADC  Module
  * @retval  None
  */
void RCC_APB0PeriphReset(uint32_t APB0Modules)
{
    assert_param(IS_APB0_MODULES(APB0Modules));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->APB0RSTR.V32 = APB0Modules;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief  reset APB1 modules
  * @param  APB1Modules:  APB1 module bits @ref RCC_APB1_Type
  *   This parameter can be any combination of following values:
  *     @arg @b RCC_APB1_UART0:    for UART0 Module
  *     @arg @b RCC_APB1_UART1:    for UART1 Module 
  *     @arg @b RCC_APB1_UART2:    for UART2 Module 
  *     @arg @b RCC_APB1_UART3:    for UART3 Module
  *     @arg @b RCC_APB1_SPI0 :    for SPI0  Module
  *     @arg @b RCC_APB1_SPI1 :    for SPI1  Module
  *     @arg @b RCC_APB1_TWI0 :    for TWI0  Module
  *     @arg @b RCC_APB1_LED  :    for LED   Module
  *     @arg @b RCC_APB1_LCD  :    for LCD   Module
  *     @arg @b RCC_APB1_TK   :    for TK    Module
  *     @arg @b RCC_APB1_HLV  :    for HLV   Module
  *     @arg @b RCC_APB1_NTEST:    for NTEST Module
  * @retval None
  */
void RCC_APB1PeriphReset(uint32_t APB1Modules)
{
    assert_param(IS_APB1_MODULES(APB1Modules));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->APB1RSTR.V32 = APB1Modules;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief     Get RCC reset flag
  * @param  ResetFlag:   RCC reset flag mask
  *     @arg @b  RCC_RST_PIN
  *     @arg @b  RCC_RST_LVR
  *     @arg @b  RCC_RST_POWERON
  *     @arg @b  RCC_RST_SOFTWARE
  *     @arg @b  RCC_RST_IWDT
  *     @arg @b  RCC_RST_WWDT
  * @retval  FlagStatus RCC reset flag
  *     @arg @b  SH_SET    the flag is setted
  *     @arg @b  RESET  the flag is cleared
  */
FlagStatus RCC_GetResetFlag(RCC_RESET_Type ResetFlag)
{
    assert_param(IS_RST_MODULES(ResetFlag));

    return ((RCC->RSTSTR.V32 & ResetFlag) ? SH_SET : SH_RESET);
}

/**
  * @brief      clear RCC reset flags
  * @param  ResetFlags:   RCC reset flag clear bits
  *     @arg @b  RCC_RST_PIN
  *     @arg @b  RCC_RST_LVR
  *     @arg @b  RCC_RST_POWERON
  *     @arg @b  RCC_RST_SOFTWARE
  *     @arg @b  RCC_RST_IWDT
  *     @arg @b  RCC_RST_WWDT
   * @retval None
  */
void RCC_ClearResetFlag(RCC_RESET_Type ResetFlags)
{
    assert_param(IS_RST_MODULES(ResetFlags));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->RSTCLR.V32 = ResetFlags;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief open or close CSM modules
  * @param OnOffState   open or close related clock gate
  *     @arg @b  SH_ON  open related clock gate
  *     @arg @b  SH_OFF close related clock gate
  * @retval   None
  */
void RCC_CSMModuleOnOff(CmdState OnOffState)
{
    assert_param(IS_CMD_STATE(OnOffState));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->CR.BIT.CSMON = OnOffState;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief     Get RCC CSM flag
  * @param  CSMFlag:   RCC CSM flag mask
  *     @arg @b  RCC_CSM_LSE_RF
  *     @arg @b  RCC_CSM_HSE_RF
  *     @arg @b  RCC_CSM_PLL_RF
  *     @arg @b  RCC_CSM_LSE_IF
  *     @arg @b  RCC_CSM_HSE_IF
  *     @arg @b  RCC_CSM_PLL_IF
  * @retval  FlagStatus RCC CSM flag
  *     @arg @b  SH_SET    the flag is setted
  *     @arg @b  SH_RESET  the flag is cleared
  */
FlagStatus RCC_GetCsmFStatus(RCC_CSMF_Type CSMFlag)
{
    assert_param(IS_CSM_FLAG(CSMFlag));

    return ((RCC->CISTR.V32 & CSMFlag) ? SH_SET : SH_RESET);
}

/**
  * @brief  Clear RCC clock interrupt flag
  * @param  CLK_INTFLG_CLR: CICCLR Flag
  *     @arg @b  RCC_CSM_CICCLR_CSMC   
  *     @arg @b  RCC_CSM_CICCLR_PLLRDYC
  *     @arg @b  RCC_CSM_CICCLR_HSERDYC
  *     @arg @b  RCC_CSM_CICCLR_LSERDYC
  * @retval null
  */
void RCC_ClearITPendingBit(RCC_CICCLR_Type CLK_INTFLG_CLR)
{
    assert_param(IS_CICCLR_FLAG(CLK_INTFLG_CLR));
    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();
    if (CLK_INTFLG_CLR == RCC_CSM_CICCLR_CSMC)
    {
        RCC->CICLR.BIT.CSMC = 1;
    }
    else if (CLK_INTFLG_CLR == RCC_CSM_CICCLR_PLLRDYC)
    {
        RCC->CICLR.BIT.PLLRDYC = 1;
    }
    else if (CLK_INTFLG_CLR == RCC_CSM_CICCLR_HSERDYC)
    {
        RCC->CICLR.BIT.HSERDYC = 1;
    }
    else
    {
        RCC->CICLR.BIT.LSERDYC = 1;
    }
    /* Unlock RCC registers */
    RCC_REGS_LOCK();
}

/**
  * @brief RCC interrupt enable configure
  * @param RDYINTIE
  *     @arg @b  RCC_CIENR_LSERDYIE  
  *     @arg @b  RCC_CIENR_HSERDYIE 
  *     @arg @b  RCC_CIENR_PLLRDYIE
  * @param NewState
  *     @arg @b  SH_DISABLE  
  *     @arg @b  SH_ENABLE 
  * @retval  null
  */
void RCC_INTConfig(RCC_CIENR_Type RDYINTIE, FunctionalState NewState)
{
    assert_param(IS_CIENR_REGISTER(RDYINTIE));
    assert_param(IS_FUNCTION_STATE(NewState));

    RCC_REGS_UNLOCK();
    if (NewState != SH_DISABLE)
    {
        RCC->CIENR.V32 |= RDYINTIE;
    }
    else
    {
        RCC->CIENR.V32 &= (~RDYINTIE);
    }
    RCC_REGS_LOCK();
}

/**
  * @brief Configure PLL on or off
  * @param OnOffState   open or close related clock gate
  *     @arg @b  SH_ON  open related clock gate
  *     @arg @b  SH_OFF close related clock gate
  * @retval   None
  */
void RCC_PLLONorOFF(CmdState OnOffState)
{
    assert_param(IS_CMD_STATE(OnOffState));

    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->CR.BIT.PLLON = OnOffState;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}
/**
  * @}
  */

/**
  * @}
  */
 
/**
  * @}
  */

/************************ (C) COPYRIGHT Sinowealth *****END OF FILE****/
