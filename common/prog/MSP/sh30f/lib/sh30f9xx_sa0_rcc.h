/**
  ******************************************************************************
  * @file    sh30f9xx_sa0_rcc.h
  * @author  sinowealth
  * @version V1.1.0
  * @date    2020-11-25  
  * @brief   This file provides reset and clock module's APIs
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
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SH30F9XX_SA0_RCC_H
#define __SH30F9XX_SA0_RCC_H

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/
#include "sh30f9xx_sa0_libcfg.h"

/** @addtogroup SH30F9xx_sa0_libcfg_StdLib_Driver
  * @{
  */

/** @addtogroup RCC_MODULE
  * @{
  */

/** @defgroup RCC_Group_Constant  Public Constants
  * @{
  */
/*!< CLOCK SOURCE CONSTANT */
typedef enum
{
    RCC_SYS_SRC_HSI = 0,
    RCC_SYS_SRC_PLL = 1,
    RCC_SYS_SRC_LSE = 2,
    RCC_SYS_SRC_HSE = 3,
    RCC_SYS_SRC_LSI = 4,
    RCC_SYS_SRC_LSI_DIV4 = 5,
} RCC_SysSource_Type;

typedef enum
{
    PLL_SRC_HSI = 0,
    PLL_SRC_HSE = 1,
} RCC_PLL_SRC_Type;

/*! @struct  RCC_Clocks_TypeDef
  *  RCC structure of clock frequency datas, used to get current system's clock frequency
  */
typedef struct
{
    uint32_t sysFreq;   /*!<  System clock frequency, unit is Hz */
    uint32_t hclkFreq;  /*!< AHB bus clock frequency, unit is Hz */
    uint32_t pclk0Freq; /*!< APB0 bus clock frequency, unit is Hz */
    uint32_t pclk1Freq; /*!< APB1 bus clock frequency, unit is Hz */
} RCC_Clocks_TypeDef;

/*!  RCC AHB Modules */
typedef enum
{
    RCC_AHB_SYSCFG = RCC_AHBENR_SYSCFGEN_Msk,
    RCC_AHB_CRC    = RCC_AHBENR_CRCEN_Msk,
    RCC_AHB_DMA    = RCC_AHBENR_DMAEN_Msk,
} RCC_AHB_Type;

/*! All AHB  Modules */
#define RCC_AHB_ALL                 (0x000F)

/*! check AHB Module source */
#define IS_AHB_MODULES(m)           ((((m)&RCC_AHB_ALL) != 0) \
                                    && (((m) & (~RCC_AHB_ALL)) == 0))

/*!  RCC APB0 Modules */
typedef enum
{
    RCC_APB0_TIM0 = RCC_APB0ENR_TIM0EN_Msk,
    RCC_APB0_TIM1 = RCC_APB0ENR_TIM1EN_Msk,
    RCC_APB0_TIM2 = RCC_APB0ENR_TIM2EN_Msk,
    RCC_APB0_TIM3 = RCC_APB0ENR_TIM3EN_Msk,
    RCC_APB0_PWM0 = RCC_APB0ENR_PWM0EN_Msk,
    RCC_APB0_PWM1 = RCC_APB0ENR_PWM1EN_Msk,
    RCC_APB0_PWM2 = RCC_APB0ENR_PWM2EN_Msk,
    RCC_APB0_PWM3 = RCC_APB0ENR_PWM3EN_Msk,
    RCC_APB0_PCA0 = RCC_APB0ENR_PCA0EN_Msk,
    RCC_APB0_PCA1 = RCC_APB0ENR_PCA1EN_Msk,
    RCC_APB0_PCA2 = RCC_APB0ENR_PCA2EN_Msk,
    RCC_APB0_PCA3 = RCC_APB0ENR_PCA3EN_Msk,
    RCC_APB0_EXTI = RCC_APB0ENR_EXTIEN_Msk,
    RCC_APB0_WWDT = RCC_APB0ENR_WWDTEN_Msk,
    RCC_APB0_ADC  = RCC_APB0ENR_ADCEN_Msk,
} RCC_APB0_Type;

/*! All APB0  Modules */
#define RCC_APB0_ALL                (0x07FFF)

/*! check APB0 clock source */
#define IS_APB0_MODULES(m)          ((((m)&RCC_APB0_ALL) != 0) \
                                    && (((m) & (~RCC_APB0_ALL)) == 0))

/*!  RCC APB1 Modules */
typedef enum
{
    RCC_APB1_UART0 = RCC_APB1ENR_UART0EN_Msk,
    RCC_APB1_UART1 = RCC_APB1ENR_UART1EN_Msk,
    RCC_APB1_UART2 = RCC_APB1ENR_UART2EN_Msk,
    RCC_APB1_UART3 = RCC_APB1ENR_UART3EN_Msk,
    RCC_APB1_SPI0  = RCC_APB1ENR_SPI0EN_Msk,
    RCC_APB1_SPI1  = RCC_APB1ENR_SPI1EN_Msk,
    RCC_APB1_TWI0  = RCC_APB1ENR_TWI0EN_Msk,
    RCC_APB1_LED   = RCC_APB1ENR_LEDEN_Msk,
    RCC_APB1_LCD   = RCC_APB1ENR_LCDEN_Msk,
    RCC_APB1_TK    = RCC_APB1ENR_TKEN_Msk,
    RCC_APB1_HLV   = RCC_APB1ENR_HLVEN_Msk,
} RCC_APB1_Type;

/*! All APB1  Modules */
#define RCC_APB1_ALL                (0x07FF)

/*! check APB1 clock source */
#define IS_APB1_MODULES(m)          ((((m)&RCC_APB1_ALL) != 0) \
                                    && (((m) & (~RCC_APB1_ALL)) == 0))

/*!  RCC RSTSTR Modules */
typedef enum
{
    RCC_RST_PIN      = RCC_RSTCLR_PINRSTFC_Msk,
    RCC_RST_LVR      = RCC_RSTCLR_LVRSTFC_Msk,
    RCC_RST_POWERON  = RCC_RSTCLR_PORSTFC_Msk,
    RCC_RST_SOFTWARE = RCC_RSTCLR_SWRSTFC_Msk,
    RCC_RST_IWDT     = RCC_RSTCLR_IWDTRSTFC_Msk,
    RCC_RST_WWDT     = RCC_RSTCLR_WWDTRSTFC_Msk,
} RCC_RESET_Type;

/*! All RST  Modules */
#define RCC_RST_ALL                 (0x003F)

/*! check RST clock source */
#define IS_RST_MODULES(m)           ((((m)&RCC_RST_ALL) != 0) \
                                    && (((m) & (~RCC_RST_ALL)) == 0))

/*!  RCC CIENR Register */
typedef enum
{
    RCC_CIENR_LSERDYIE = RCC_CIENR_LSERDYIE_Msk,
    RCC_CIENR_HSERDYIE = RCC_CIENR_HSERDYIE_Msk,
    RCC_CIENR_PLLRDYIE = RCC_CIENR_PLLRDYIE_Msk,
} RCC_CIENR_Type;

/*! All CIENR  Register */
#define RCC_CIENR_ALL               (0x001C)

/*! check CIENR source */
#define IS_CIENR_REGISTER(m)        ((((m)&RCC_CIENR_ALL) != 0) \
                                    && (((m) & (~RCC_CIENR_ALL)) == 0))

/*!  RCC CSM Modules */
typedef enum
{
    RCC_CSM_LSE_RF = RCC_CISTR_LSERDYIF_Msk,
    RCC_CSM_HSE_RF = RCC_CISTR_HSERDYIF_Msk,
    RCC_CSM_PLL_RF = RCC_CISTR_PLLRDYIF_Msk,

    RCC_CSM_LSE_IF = RCC_CISTR_LSECSMF_Msk,
    RCC_CSM_HSE_IF = RCC_CISTR_HSECSMF_Msk,
    RCC_CSM_PLL_IF = RCC_CISTR_PLLCSMF_Msk,
} RCC_CSMF_Type;

/*! All CSM  Modules */
#define RCC_CSMF_ALL                (0x00FC)

/*! check APB1 clock source */
#define IS_CSM_FLAG(m)              ((((m)&RCC_CSMF_ALL) != 0) \
                                    && (((m) & (~RCC_CSMF_ALL)) == 0))

/*!  Clear CICCLR Flag */
typedef enum
{
    RCC_CSM_CICCLR_CSMC    = RCC_CICLR_CSMC_Msk,
    RCC_CSM_CICCLR_PLLRDYC = RCC_CICLR_PLLRDYC_Msk,
    RCC_CSM_CICCLR_HSERDYC = RCC_CICLR_HSERDYC_Msk,
    RCC_CSM_CICCLR_LSERDYC = RCC_CICLR_LSERDYC_Msk,
} RCC_CICCLR_Type;

/*! All CSM  CICCLR */
#define RCC_CICCLR_ALL              (0x009C)

/*! check CSM  CICCLR */
#define IS_CICCLR_FLAG(m)           ((((m)&RCC_CICCLR_ALL) != 0) \
                                    && (((m) & (~RCC_CICCLR_ALL)) == 0))

/** 
  *@brief  Unlock the RCC module's regsiter. RCC registers can be modified.
  */
#define RCC_REGS_UNLOCK()           (RCC->RCCLOCK = 0x33CC)

/** 
  *@brief  Lock the RCC module's regsiter. RCC registers cannot be modified.
  */
#define RCC_REGS_LOCK()             (RCC->RCCLOCK = 0)

/** @addtogroup RCC_Group_Pub_Funcs
  * @{
  */
/* get current clock requency*/
void RCC_GetClocksFreq(RCC_Clocks_TypeDef *Clocks);

/* configure AHB modules' clock gate */
void RCC_AHBPeriphClockOnOff(RCC_AHB_Type AHBModules, CmdState OnOffState);

/* configure APB1 modules' clock gate */
void RCC_APB1PeriphClockOnOff(RCC_APB1_Type APB1Modules, CmdState OnOffState);

/* configure APB0 modules' clock gate */
void RCC_APB0PeriphClockOnOff(RCC_APB0_Type APB0Modules, CmdState OnOffState);

/* reset AHB modules*/
void RCC_AHBPeriphReset(uint32_t AHBModules);

/* reset APB0 modules*/
void RCC_APB0PeriphReset(uint32_t APB0Modules);

/* reset APB1 modules*/
void RCC_APB1PeriphReset(uint32_t APB1Modules);

/* Get RCC reset flag*/
FlagStatus RCC_GetResetFlag(RCC_RESET_Type ResetFlag);

/* clear RCC reset flags*/
void RCC_ClearResetFlag(RCC_RESET_Type ResetFlags);

/* CSM mould on/off */
void RCC_CSMModuleOnOff(CmdState OnOffState);

/* get Clock interrupt flags*/
FlagStatus RCC_GetCsmFStatus(RCC_CSMF_Type CSMFlag);

/* Clear RCC clock interrupt flag */
void RCC_ClearITPendingBit(RCC_CICCLR_Type CLK_INTFLG_CLR);

/* RCC interrupt enable configure */
void RCC_INTConfig(RCC_CIENR_Type RDYINTIE, FunctionalState NewState);

/* Configure PLL on or off */
void RCC_PLLONorOFF(CmdState OnOffState);
/**
  * @}
  */
#ifdef __cplusplus
}
#endif
/**
  * @}
  */

/**
  * @}
  */

 /**
  * @}
  */
#endif /*__SH30F9XX_SA0_RCC_H */
/************************ (C) COPYRIGHT Sinowealth *****END OF FILE****/
