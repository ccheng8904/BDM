/**
  ******************************************************************************
  * @file    sh30f9xx_sa0_pca.h
  * @author  sinowealth
  * @version V1.1.0
  * @date    2021-01-06  
  * @brief   This file contains all the functions prototypes for the PCA firmware
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
  * <h2><center>&copy; COPYRIGHT 2021 Sinowealth</center></h2>
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SH30F9XX_SA0_PCA_H
#define __SH30F9XX_SA0_PCA_H

#ifdef __cplusplus
 extern "C" 
 {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sh30f9xx_sa0_libcfg.h"

/** @addtogroup SH30F9xx_sa0_libcfg_StdLib_Driver
  * @{
  */

/** @addtogroup PCA_MODULE
  * @{
  */

/** @defgroup PCA_Group_Constant  Public Constants
  * @{
  */

/** @defgroup PCA_DMA Trigger source 
  * @{
  */ 
#define PCA_DMA_CYCLE_MATCH          (uint32_t)(1 << 24) //Trigger DMA when cycle matching event occurs in PCA counter / timer
#define PCA_DMA_CntOverflow          (uint32_t)(1 << 23) //Trigger DMA when overflow event occurs in PCA counter / timer
#define PCA_DMA_Channel_1            (uint32_t)(1 << 21) //DMA triggered when PCA capture / compare channel 1 event occurs
#define PCA_DMA_Channel_0            (uint32_t)(1 << 20) //DMA triggered when PCA capture / compare channel 0 event occurs

/*!  check PCA DMA Trigger source */
#define IS_PCA_Trg_SOURCE(source)    ((source == PCA_DMA_CYCLE_MATCH) \
                                     ||(source == PCA_DMA_CntOverflow) \
                                     ||(source == PCA_DMA_Channel_1)   \
                                     ||(source == PCA_DMA_Channel_0))

/**
  * @}
  */

/** @defgroup PCA_Interrupt_Flags 
  * @{
  */ 
  
#define PCA_FLAG_CC0IF               (uint16_t)0x0001
#define PCA_FLAG_CC1IF               (uint16_t)0x0002
#define PCA_FLAG_PIF                 (uint16_t)0x0010
#define PCA_FLAG_CIF                 (uint16_t)0x0080

/*!  check PCA Interrupt flag */
#define IS_PCA_GET_ONE_FLAG(FLAG)    ((FLAG == PCA_FLAG_CC0IF)  \
                                     ||(FLAG == PCA_FLAG_CC1IF)  \
                                     ||(FLAG == PCA_FLAG_PIF)    \
                                     ||(FLAG == PCA_FLAG_CIF))

/**
  * @}
  */

/** @defgroup PCA Interrupt SOURCE
  * @{
  */ 

#define PCA_COUNTER_OVERFLOW_INT     (uint16_t)0x0080
#define PCA_COUNTER_CYCLE_INT        (uint16_t)0x0100

/*!  check PCA Interrupt SOURCE */
#define IS_PCA_INT_SOURCE(SOURCE)    ((SOURCE == PCA_COUNTER_OVERFLOW_INT) \
                                     || (SOURCE == PCA_COUNTER_CYCLE_INT))

/**
  * @}
  */

/*!  PCA CLOCK SOURCE */
typedef enum 
{
    PCLK0            =    0,     /*!< Pclock0   */
    ECIxNEGATIVELY   =    1,     /*!< Ecix falling edge */
    LSICLK           =    2,     /*!< LSI 128K RC */
    LSECLK           =    3,     /*!< LSE 32K */
    EXTHIGHOSCDIV8   =    4,     /*!< HSE DIV8 */
    TIM0OVERFLOW     =    5,     /*!< Timer 0 overflow */
}PCA_CLOCK_SOURCE_Type;


/*!  check PCA clock source */
#define IS_PCACLKSOURCE_DIV(div)     (((div) == PCLK0)            \
                                     || ((div) == ECIxNEGATIVELY)   \
                                     || ((div) == LSICLK)           \
                                     || ((div) == LSECLK)           \
                                     || ((div) == EXTHIGHOSCDIV8)   \
                                     || ((div) == TIM0OVERFLOW))

/*!  PCA Counter Mode */
typedef enum 
{
    SINGLEEDGE      =    0,     /*!< The counter counts up */
    DOUBLEEDGE      =    1,     /*!< Counter middle alignment */
}PCA_COUNTERMODE_Type;

/*!  check PCA Counter Mode */
#define IS_PCACOUNTERMODE_DIV(div)   (((div) == SINGLEEDGE) || ((div) == DOUBLEEDGE))

/*!  PCA filtering time In Mode0 */
typedef enum 
{
    FILTERING_NONE        =    0,     /*!< No filtering function */
    FILTERING_8PCLK0      =    1,     /*!< The filtering time is 8*PCLK0 */
    FILTERING_16PCLK0     =    2,     /*!< The filtering time is 16*PCLK0 */
    FILTERING_32PCLK0     =    3,     /*!< The filtering time is 32*PCLK0 */
}PCA_FILTERING_TIMER_Type;

/*!  check PCA filtering time Mode */
#define IS_FILTERING_TIMER_TYPE(type) (((type) == FILTERING_NONE)      \
                                      || ((type) == FILTERING_8PCLK0)  \
                                      || ((type) == FILTERING_16PCLK0) \
                                      || ((type) == FILTERING_32PCLK0))

/*!  PCA Mode selection */
typedef enum 
{
    CAPTUREMODE      =    0,     /*!< capture mode   */
    COMPAREMODE      =    1,     /*!< compare mode */
    FREQOUTPUTMODE   =    2,     /*!< Frequency output */
    PWMMODE          =    3,     /*!< PWM output */
}PCA_MODEL_Type;

/*!  check PCA Mode */
#define IS_PCAMODE_DIV(div)            (((div) == CAPTUREMODE)     \
                                       || ((div) == COMPAREMODE)     \
                                       || ((div) == FREQOUTPUTMODE)  \
                                       || ((div) == PWMMODE)) 

/*!  PCA Capture Mode */
typedef enum 
{
    RISING_EDGE_TRIGGER      =    0,     /*!< Rising edge trigger    */
    FALLING_EDGE_TRIGGER     =    2,     /*!< Falling edge trigger   */
    ARBITRARY_EDGE_TRIGGER   =    3,     /*!< Arbitrary edge trigger */
}PCA_CAPMODE_Type;

/*!  check PCA Capture Mode */
#define IS_PCACAPMODE_DIV(div)         (((div) == RISING_EDGE_TRIGGER)      \
                                       || ((div) == FALLING_EDGE_TRIGGER)    \
                                       || ((div) == ARBITRARY_EDGE_TRIGGER)) 

/*!  PCA compare Mode */
typedef enum 
{
    CONTINUOUS      =    0,     /*!< Continuous compare */
    SINGLE          =    2,     /*!< Single compare */
}PCA_COMPAREMODE_Type;

/*!  check PCA compare Mode */
#define IS_PCACOMPAREMODE_DIV(div)     (((div) == CONTINUOUS) || ((div) == SINGLE))

/*!  PCA compare Mode */
typedef enum 
{
    INITAL_HIGH       =    0,     /*!< The initial level is high */
    INITAL_LOW        =    1,     /*!< The initial level is low */
}PCA_INITAL_LEVEL_Type;

/*!  check PCA initial level */
#define IS_INITAL_LEVEL(level)     (((level) == LEVEL_HIGH) || ((level) == LEVEL_LOW))

/*!  PCA PWM Mode */
typedef enum 
{
    PWM_8BIT             =    0,     /*!< 8 bit PWM  */
    PWM_16BIT            =    1,     /*!< 16 bit PWM  */
    PHASE_PWM_16BIT      =    2,     /*!< 16 bit phase correction PWM */
    PHASEFREQ_PWM_16BIT  =    3,     /*!< 16 bit phase frequency correction PWM */
}PCA_PWMMODEL_Type;

/*!  check PCA PWM Mode */
#define IS_PWMMODEL_DIV(div)            (((div) == PWM_8BIT)           \
                                        || ((div) == PWM_16BIT)        \
                                        || ((div) == PHASE_PWM_16BIT)  \
                                        || ((div) == PHASEFREQ_PWM_16BIT)) 
/*!  PCA PWM Mode */
typedef enum  
{
    ACTIVE_LOW         =    0,     /*!< The active level is low */
    ACTIVE_HIGH        =    1,     /*!< The active level is high */
}PCA_ACTIVE_LEVEL_Type;

/*!  check PCA active level */
#define IS_ACTIVE_LEVEL(level)     (((level) == ACTIVE_LOW) || ((level) == ACTIVE_HIGH))

/*!  PCA Channel */
typedef enum 
{
    PCA_CHANNEL_A      =    0,     /*!< PCA Channel A */
    PCA_CHANNEL_B      =    1,     /*!< PCA Channel B */
}PCA_CHANEEL_Type;

/*!  check PCA Channel */
#define IS_PCACHANEEL_CHANNEL(channel)   (((channel) == PCA_CHANNEL_A) \
                                         || ((channel) == PCA_CHANNEL_B))

/*! @struct  PCA_CFGR_TypeDef
  * @brief   structure for CFGR register
  */ 
typedef struct
{
    uint32_t ClkSel                   : 3;  /*!< CLOCK SELECT*/
    uint32_t rev1                     : 3;  /*!< reserved*/
    uint32_t PCACounterMode           : 1;  /*!< counts up or  Counter middle alignment */
    uint32_t rev2                     : 5;  /*!< reserved */
    uint32_t CCR0UPDATAEN             : 1;  /*!< Comparison module 0 immediately updates the enable bit */
    uint32_t CCR1UPDATAEN             : 1;  /*!< Comparison module 1 immediately updates the enable bit */
    uint32_t rev3                     : 1;  /*!< reserved*/
    uint32_t PUEUPDATAEN              : 1;  /*!< PCA The cycle value immediately updates the enable bit */
    uint32_t ECFSel                   : 2;  /*!< Input signal filtering time (only valid in mode0 mode) */
    uint32_t rev4                     : 14; /*!< reserved*/
}PCA_CFGR_TypeDef;


/*! @struct  PCA_CCMR_TypeDef
  * @brief   structure for CCMR register
  */ 
typedef struct
{
    uint32_t rev5                     : 2;  /*!< reserved */
    uint32_t PCA_TCP                  : 1;  /*!< PCA edge signale indicate*/    
    uint32_t PCA_CEN                  : 1;  /*!< Compare OR capture module function: SH_ENABLE or SH_DISABLE*/    
    uint32_t FunctionSelect           : 2;  /*!< PCA secondary mode select*/    
    uint32_t ModeSelect               : 2;  /*!< PCA mode select:CAPTUREMODE,COMPARE,FREQOUTPUTMODE,PWMMODE*/    
    uint32_t PCA_CC                   : 1;  /*!< Clear counter enable bit when capture occurs*/
    uint32_t rev6                     : 23; /*!< reserved*/
}PCA_CCMR_TypeDef;


/*! @struct  PCA_InitTypeDef
  * @brief   structure for PCA initial
  */ 
typedef struct
{
    /* for CFGR register */
    PCA_CFGR_TypeDef PCA_CFGR;

    /* for FORCE register */
    uint32_t PCA_FORCE;

    /* for CNT register */
    uint32_t PCA_CNT;

    /* for PSC register */
    uint32_t PCA_PSC;

    /* for PR register */
    uint32_t PCA_PR;
    
    /* for CPM0 register */
    PCA_CCMR_TypeDef PCA_CCMR0;

    /* for CPM1 register */
    PCA_CCMR_TypeDef PCA_CCMR1;
    
    /* for CPR0 register */
    uint32_t PCA_CCR0;

    /* for CPR1 register */
    uint32_t PCA_CCR1;

}PCA_InitTypeDef;

/** @defgroup PCA_Periphs
  * @{
  */ 
  
#define IS_PCA_ALL_PERIPH(PERIPH)  ((PERIPH == PCA0)   \
                                   || (PERIPH == PCA1) \
                                   || (PERIPH == PCA2) \
                                   || (PERIPH == PCA3))
/**
  * @}
  */ 

/** @defgroup PCA_Cascade
  * @{
  */    
  
#define IS_PCA_CASCADE_PERIPH(PERIPH)  ((PERIPH == PCA0) || (PERIPH == PCA2))
/**
  * @}
  */ 


/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 
/** @addtogroup PCA_Group_Pub_Funcs
  * @{
  */
/*Fills each PCA_InitStruct member with its default value.*/
void PCA_StructInit(PCA_InitTypeDef *PCA_InitStruct);

/*Initializes the PCAx peripheral according to the specified parameters in the PCA_InitStruct*/
void PCA_Init(PCA_TypeDef *PCAx, const PCA_InitTypeDef *PCA_InitStruct);

/*Clear the PCA's pending flags*/
void PCA_ClearFlag(PCA_TypeDef *PCAx, uint16_t PCA_IntFlag);

/*Clear the PCAx's All pending flag*/
void PCA_ClearAllFlag(PCA_TypeDef *PCAx);

/*Open or close the PCA peripheral*/
void PCA_OnOff(PCA_TypeDef *PCAx, CmdState OnOffState);

/*Open or close the PCA cascade.*/
void PCA_CascadeOnOff(PCA_TypeDef *PCAx, CmdState OnOffState);

/*Set the PCAx Frequency output mode frequency value*/
void PCA_FreqOutValue(PCA_TypeDef *PCAx, PCA_CHANEEL_Type PCAChannel, uint8_t Value);

/*Set the PCAx Forced match control bit  */
void PCA_ForceMatchControl(PCA_TypeDef *PCAx, PCA_CHANEEL_Type PCAChannel, CmdState OnOffState);

/*Set the PCAx PR Register value*/
void PCA_SetPRValue(PCA_TypeDef *PCAx, uint32_t Value);

/*Set the PCAx CCR Register value*/
void PCA_SetCCRValue(PCA_TypeDef *PCAx, PCA_CHANEEL_Type PCAChannel, uint32_t Value);

/*Get the PCAx CCR Register value*/
uint32_t PCA_GetCaptureValue(PCA_TypeDef *PCAx,PCA_CHANEEL_Type PCAChannel);

/*Check whether the specified PCA flag is set or not*/
FlagStatus PCA_GetFlagStatus(PCA_TypeDef *PCAx, uint16_t PCA_IntFlag);

/*Get the status of the trigger level in the capture mode*/
FlagStatus PCA_GetCapterLevelStatus(PCA_TypeDef *PCAx, PCA_CHANEEL_Type PCAChannel);

/*Enable or disable the specified PCA's Overflow interrupts.*/
void PCA_INTConfig(PCA_TypeDef *PCAx, uint32_t IntSource, FunctionalState NewState);

/*Enable or disable the specified PCAx Capture's interrupts.*/
void PCA_CapINTConfig(PCA_TypeDef *PCAx, PCA_CHANEEL_Type PCAChannel, FunctionalState NewState);

/*Enable or disable the PCA's DMA request sources.*/
void PCA_DMAConfig(PCA_TypeDef *PCAx, uint32_t PCA_DMASource, FunctionalState NewState);
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
#endif /*__SH30F9XX_SA0_PCA_H */
/************************ (C) COPYRIGHT Sinowealth *****END OF FILE****/


