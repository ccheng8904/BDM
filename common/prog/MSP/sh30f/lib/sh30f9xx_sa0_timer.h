/**
  ******************************************************************************
  * @file    sh30f9xx_sa0_TIMER.h
  * @author  sinowealth
  * @version V1.1.0
  * @date    2020-12-14  
  * @brief   This file contains all the functions prototypes for the TIMER firmware
  *          library.
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
#ifndef __SH30F9XX_SA0_TIMER_H
#define __SH30F9XX_SA0_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "sh30f9xx_sa0_libcfg.h"

/** @addtogroup SH30F9xx_sa0_libcfg_StdLib_Driver
  * @{
  */

/** @addtogroup TIM_MODULE
  * @{
  */

    /* Exported types ------------------------------------------------------------*/
/**
*@brief TIMER_registers
*/
typedef struct
{
    union {
        __IO  uint32_t  V32;                                /*!< 0000H */
        struct 
        {
            __IO  uint32_t STR       : 1;  /*!< [b0~b0]*/
            __IO  uint32_t rev0      : 2;  /*!< [b2~b1]*/
            __IO  uint32_t OPM       : 1;  /*!< [b3~b3]*/
            __IO  uint32_t CLKS      : 2;  /*!< [b5~b4]*/
            __IO  uint32_t rev1      : 1;  /*!< [b6~b6]*/
            __IO  uint32_t IE        : 1;  /*!< [b7~b7]*/
            __IO  uint32_t TRIGEN    : 1;  /*!< [b8~b8]*/
            __IO  uint32_t ETEN      : 1;  /*!< [b9~b9]*/
            __IO  uint32_t TC        : 1;  /*!< [b10~b10]*/
            __IO  uint32_t rev2      : 1;  /*!< [b11~b11]*/
            __IO  uint32_t ECF       : 2;  /*!< [b13~b12]*/
            __IO  uint32_t rev3      :18;  /*!< [b31~b14]*/
        }BIT;
    }CR;                               /*!< 0000H */
    __IO uint32_t TCNT;                                    /*!< 0004H */
    __IO uint32_t TPR;                                     /*!< 0008H */
    __IO uint32_t PSQ;                                     /*!< 000CH */
    union {
        __IO  uint32_t V32;                                /*!< 0010H */
        struct 
        {
            __I   uint32_t TF        : 1;  /*!< [b0~b0]*/
            __IO  uint32_t rev0      :15;  /*!< [b15~b1]*/
            __O   uint32_t TFC       : 1;  /*!< [b16~b16]*/
            __IO  uint32_t rev1      :15;  /*!< [b31~b17]*/
        }BIT;
    }TIMINTF;                               /*!< 0010H */
}TIMER_TypeDef;

#ifdef   TIM0
#undef   TIM0
#define  TIM0 ((TIMER_TypeDef *)TIM0_BASE)
#endif

#ifdef   TIM1
#undef   TIM1
#define  TIM1 ((TIMER_TypeDef *)TIM1_BASE)
#endif

#ifdef   TIM2
#undef   TIM2
#define  TIM2 ((TIMER_TypeDef *)TIM2_BASE)
#endif

#ifdef   TIM3
#undef   TIM3
#define  TIM3 ((TIMER_TypeDef *)TIM3_BASE)
#endif

/** @defgroup TIM_Exported_Types
  * @{
  */

typedef struct
{
    uint32_t TIM_Prescaler; /*!< Specifie the TIM prescaler value.
                                This parameter can be 0 to 0xFFFF */

    uint32_t TIM_Period;    /*!< Specifie the TIM period value.
                                This parameter can be 0 to 0xFFFF */

    uint16_t TIM_CLKSource; /*!< Specifie the TIM's clock sources.
                                This parameter can be a value of @ref TIM_Clock_Sources */

    uint16_t TIM_OPMode;    /*!< Specifie the TIM OP mode.
                                This parameter can be a value of @ref TIM_OP_Mode */

    uint16_t TIM_TCMode;    /*!< Specifie the Compare output.
                                This parameter can be a value of @ref TIM_TC_Mode */

    uint16_t TIM_ECFFilter; /*!< Specifie the Compare output.
                                This parameter can be a value of @ref TIM_ECF_Mode */
} TIM_InitTypeDef;

/** @defgroup TIM_FLITER_TIMER
  * @{
  */

#define TIM_FLITER_TIMER_NONE       (uint16_t)0x0000
#define TIM_FLITER_TIMER_8PCLK      (uint16_t)0x1000
#define TIM_FLITER_TIMER_16PCLK     (uint16_t)0x2000
#define TIM_FLITER_TIMER_32PCLK     (uint16_t)0x3000
#define IS_TIM_FLITER_TIMER(TIMER)  ((TIMER == TIM_FLITER_TIMER_NONE)     \
                                    || (TIMER == TIM_FLITER_TIMER_8PCLK)  \
                                    || (TIMER == TIM_FLITER_TIMER_16PCLK) \
                                    || (TIMER == TIM_FLITER_TIMER_32PCLK))

/* Exported constants --------------------------------------------------------*/

/** @defgroup TIM_Exported_Constants
  * @{
  */

#define IS_TIM_ALL_PERIPH(PERIPH)   ((PERIPH == TIM0) || (PERIPH == TIM1)  \
                                    || (PERIPH == TIM2) || (PERIPH == TIM3))

/** @defgroup TIM_Clock_Sources
  * @{
  */

#define TIM_CLKSource_PCLK          (uint16_t)0x0000
#define TIM_CLKSource_INPUT         (uint16_t)0x0010
#define TIM_CLKSource_LSI           (uint16_t)0x0020
#define TIM_CLKSource_LSE           (uint16_t)0x0030
#define IS_TIM_CLK_SOURCE(SOURCE)   ((SOURCE == TIM_CLKSource_PCLK) \
                                    || (SOURCE == TIM_CLKSource_LSI) \
                                    || (SOURCE == TIM_CLKSource_LSE) \
                                    || (SOURCE == TIM_CLKSource_INPUT))

/**
  * @}
  */

/** @defgroup TIM_OP_Mode
  * @{
  */

#define TIM_OPMode_Continue         (uint16_t)0x0000
#define TIM_OPMode_OnePulse         (uint16_t)0x0008
#define IS_TIM_OP_MODE(MODE)        ((MODE == TIM_OPMode_Continue) \
                                    || (MODE == TIM_OPMode_OnePulse))

/** @defgroup TIM_TC_Mode
  * @{
  */

#define TIM_TCMode_Disable          (uint16_t)0x0000
#define TIM_TCMode_Enable           (uint16_t)0x0400
#define IS_TIM_TC_MODE(MODE)        ((MODE == TIM_TCMode_Enable) \
                                    || (MODE == TIM_TCMode_Disable))




/**
  * @}
  */

/**
  * @}
  */
 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/* Reset the TIMx peripheral registers to their default reset values */
void TIM_Reset(TIMER_TypeDef *TIMx);

/* Fills each TIM_InitStruct member with its default value */
void TIM_StructInit(TIM_InitTypeDef *TIM_InitStruct);

/* Initializes the TIMx peripheral according to the specified parameters in the TIM_InitStruct */
void TIM_Init(TIMER_TypeDef *TIMx, TIM_InitTypeDef *TIM_InitStruct);

/* Enable or disable the specified TIM interrupt */
void TIM_INTConfig(TIMER_TypeDef *TIMx, FunctionalState NewState);

/* Open or close the TIM peripheral */
void TIM_OnOff(TIMER_TypeDef *TIMx, CmdState OnOffState);

/* Enable or disable the TIMx's output pin */
void TIM_OutPutConfig(TIMER_TypeDef *TIMx, FunctionalState NewState);

/* Enable or disable the TIMx's trigger CM0+ core function */
void TIM_CM0EventTrigger(TIMER_TypeDef *TIMx, FunctionalState NewState);

/* Enable or disable the TIMx's trigger function */
void TIM_ExtPeripheralsTrigger(TIMER_TypeDef *TIMx, FunctionalState NewState);

/* Check whether the specified TIM flag is set or not */
FlagStatus TIM_GetFlagStatus(TIMER_TypeDef *TIMx);

/* Clear the TIMx's pending flag */
void TIM_ClearFlag(TIMER_TypeDef *TIMx);

/* Get the TIMx Period Register value */
uint32_t TIM_GetCounter(TIMER_TypeDef *TIMx);

/* Get the TIMx PSQ Register value */
uint32_t TIM_GetPrescaler(TIMER_TypeDef *TIMx);

/* Get the TIMx Period Register value */
uint32_t TIM_GetPeriod(TIMER_TypeDef *TIMx);

/* Set the TIMx Counter Register value */
void TIM_SetCounter(TIMER_TypeDef *TIMx, uint32_t Counter);

/* Set the TIMx Prescaler Register value */
void TIM_SetPrescaler(TIMER_TypeDef *TIMx, uint32_t Prescaler);

/* Set the TIMx Period Register value */
void TIM_SetPeriod(TIMER_TypeDef *TIMx, uint32_t Period);

#ifdef __cplusplus
}
#endif

#endif /*__SH30F9XX_SA0_TIMER_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Sinowealth *****END OF FILE****/
