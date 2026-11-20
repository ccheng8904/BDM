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
 *  COMP_ABBREV      MDMA
 *  PROCESSOR        SH30F9X71
 ******************************************************************************/


#ifndef MDMA_MC_H
#define MDMA_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdma_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MDMA.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mdma_types.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define MDMA_CHANNEL_SCFG_DEFAULT           {MDMA_PRIORITY_LOW,MDMA_EVENT_SOURCE_FUNC0}


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MDMA_IRQ_Index
{
    MDMA1_IRQ_INDEX,
    MDMA_IRQ_CNT
};


struct MDMA_PeriphDescriptor
{
    DMA_TypeDef             *DMAx;
    enum MDMA_IRQ_Index     irqIndex;
};


struct MDMA_Periph
{
    const struct MDMA_PeriphDescriptor      *desc;
};


enum MDMA_EventSource
{
    MDMA_EVENT_SOURCE_FUNC0   = 0, /* DMA stream function 0 */   
    MDMA_EVENT_SOURCE_FUNC1   = 1, /* DMA stream function 1 */
    MDMA_EVENT_SOURCE_FUNC2   = 2, /* DMA stream function 2 */
    MDMA_EVENT_SOURCE_FUNC3   = 3, /* DMA stream function 3 */
    MDMA_EVENT_SOURCE_FUNC4   = 4, /* DMA stream function 4 */
    MDMA_EVENT_SOURCE_FUNC5   = 5, /* DMA stream function 5 */
    MDMA_EVENT_SOURCE_FUNC6   = 6, /* DMA stream function 6 */
    MDMA_EVENT_SOURCE_FUNC7   = 7, /* DMA stream function 7 */

    MDMA_EVENT_SOURCE_CH0_UART0_TX     = MDMA_EVENT_SOURCE_FUNC0, /*!< Channel0 Function0 */
    MDMA_EVENT_SOURCE_CH0_SPI0_TX      = MDMA_EVENT_SOURCE_FUNC1, /*!< Channel0 Function1 */
    MDMA_EVENT_SOURCE_CH0_EXTI0        = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel0 Function2 */
    MDMA_EVENT_SOURCE_CH0_EXTI1        = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel0 Function2 */
    MDMA_EVENT_SOURCE_CH0_EXTI8        = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel0 Function2 */
    MDMA_EVENT_SOURCE_CH0_EXTI9        = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel0 Function2 */
    MDMA_EVENT_SOURCE_CH0_TIM0         = MDMA_EVENT_SOURCE_FUNC3, /*!< Channel0 Function3 */
    MDMA_EVENT_SOURCE_CH0_UART2_RX     = MDMA_EVENT_SOURCE_FUNC4, /*!< Channel0 Function4 */
    MDMA_EVENT_SOURCE_CH0_PCA0         = MDMA_EVENT_SOURCE_FUNC5, /*!< Channel0 Function5 */
    MDMA_EVENT_SOURCE_CH0_LED          = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel0 Function6 */
    MDMA_EVENT_SOURCE_CH0_TK           = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel0 Function6 */
    MDMA_EVENT_SOURCE_CH0_PWM0         = MDMA_EVENT_SOURCE_FUNC7, /*!< Channel0 Function7 */

    MDMA_EVENT_SOURCE_CH1_UART0_RX  = MDMA_EVENT_SOURCE_FUNC0, /*!< Channel1 Function0 */
    MDMA_EVENT_SOURCE_CH1_SPI0_RX   = MDMA_EVENT_SOURCE_FUNC1, /*!< Channel1 Function1 */
    MDMA_EVENT_SOURCE_CH1_EXTI2     = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel1 Function2 */
    MDMA_EVENT_SOURCE_CH1_EXTI3     = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel1 Function2 */
    MDMA_EVENT_SOURCE_CH1_EXTI10    = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel1 Function2 */
    MDMA_EVENT_SOURCE_CH1_EXTI11    = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel1 Function2 */
    MDMA_EVENT_SOURCE_CH1_TIM1      = MDMA_EVENT_SOURCE_FUNC3, /*!< Channel1 Function3 */
    MDMA_EVENT_SOURCE_CH1_UART2_TX  = MDMA_EVENT_SOURCE_FUNC4, /*!< Channel1 Function4 */
    MDMA_EVENT_SOURCE_CH1_PCA1      = MDMA_EVENT_SOURCE_FUNC5, /*!< Channel1 Function5 */
    MDMA_EVENT_SOURCE_CH1_LED       = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel1 Function6 */
    MDMA_EVENT_SOURCE_CH1_TK        = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel1 Function6 */
    MDMA_EVENT_SOURCE_CH1_PWM1      = MDMA_EVENT_SOURCE_FUNC7, /*!< Channel1 Function7 */ 

    MDMA_EVENT_SOURCE_CH2_UART1_TX  = MDMA_EVENT_SOURCE_FUNC0, /*!< Channel2 Function0 */
    MDMA_EVENT_SOURCE_CH2_SPI1_TX   = MDMA_EVENT_SOURCE_FUNC1, /*!< Channel2 Function1 */
    MDMA_EVENT_SOURCE_CH2_EXTI4     = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel2 Function2 */
    MDMA_EVENT_SOURCE_CH2_EXTI5     = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel2 Function2 */
    MDMA_EVENT_SOURCE_CH2_EXTI12    = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel2 Function2 */
    MDMA_EVENT_SOURCE_CH2_EXTI13    = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel2 Function2 */
    MDMA_EVENT_SOURCE_CH2_TIM2      = MDMA_EVENT_SOURCE_FUNC3, /*!< Channel2 Function3 */  
    MDMA_EVENT_SOURCE_CH2_UART3_RX  = MDMA_EVENT_SOURCE_FUNC4, /*!< Channel2 Function4 */  
    MDMA_EVENT_SOURCE_CH2_PCA2      = MDMA_EVENT_SOURCE_FUNC5, /*!< Channel2 Function5 */  
    MDMA_EVENT_SOURCE_CH2_ADC       = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel2 Function6 */  
    MDMA_EVENT_SOURCE_CH2_TK        = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel2 Function6 */  
    MDMA_EVENT_SOURCE_CH2_PWM2      = MDMA_EVENT_SOURCE_FUNC7, /*!< Channel2 Function7 */ 

    MDMA_EVENT_SOURCE_CH3_UART1_RX  = MDMA_EVENT_SOURCE_FUNC0, /*!< Channel 3 Function 0 */
    MDMA_EVENT_SOURCE_CH3_SPI1_RX   = MDMA_EVENT_SOURCE_FUNC1, /*!< Channel 3 Function 1 */
    MDMA_EVENT_SOURCE_CH3_EXTI6     = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel 3 Function 2 */
    MDMA_EVENT_SOURCE_CH3_EXTI7     = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel 3 Function 2 */
    MDMA_EVENT_SOURCE_CH3_EXTI14    = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel 3 Function 2 */
    MDMA_EVENT_SOURCE_CH3_EXTI15    = MDMA_EVENT_SOURCE_FUNC2, /*!< Channel 3 Function 2 */
    MDMA_EVENT_SOURCE_CH3_TIM3      = MDMA_EVENT_SOURCE_FUNC3, /*!< Channel 3 Function 3 */
    MDMA_EVENT_SOURCE_CH3_UART3_TX  = MDMA_EVENT_SOURCE_FUNC4, /*!< Channel 3 Function 4 */
    MDMA_EVENT_SOURCE_CH3_PCA3      = MDMA_EVENT_SOURCE_FUNC5, /*!< Channel 3 Function 5 */
    MDMA_EVENT_SOURCE_CH3_ADC       = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel 3 Function 6 */
    MDMA_EVENT_SOURCE_CH3_TK        = MDMA_EVENT_SOURCE_FUNC6, /*!< Channel 3 Function 6 */
    MDMA_EVENT_SOURCE_CH3_PWM3      = MDMA_EVENT_SOURCE_FUNC7, /*!< Channel 3 Function 7 */
};



enum MDMA_Priority
{
    MDMA_PRIORITY_LOW       = 0u,
    MDMA_PRIORITY_MED       = 1u,
    MDMA_PRIORITY_HIGH      = 2u,
    MDMA_PRIORITY_VERY_HIGH = 3u
};


struct MDMA_ChannelStaticConfig
{
    enum MDMA_Priority      priority;
    enum MDMA_EventSource   eventSource;
};

typedef DMA_CHNO_Type   MDMA_ChannelIndex;

struct MDMA_ChannelDescriptor
{
    DMA_CH_TypeDef          *dma_t;
    MDMA_ChannelIndex       index;
};


struct MDMA_Channel
{
    const struct MDMA_ChannelDescriptor     *desc;
    const struct MDMA_ChannelStaticConfig   scfg;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MDMA_PeriphDescriptor MDMA_Descriptor;

extern const struct MDMA_ChannelDescriptor MDMA_CH0_Descriptor;
extern const struct MDMA_ChannelDescriptor MDMA_CH1_Descriptor;
extern const struct MDMA_ChannelDescriptor MDMA_CH2_Descriptor;
extern const struct MDMA_ChannelDescriptor MDMA_CH3_Descriptor;

#if defined(MSPI0_PRESENT)
extern const struct MDMA_Link MDMA_MSPI0_Link_TX;
extern const struct MDMA_Link MDMA_MSPI0_Link_RX;
#endif

#if defined(MSPI1_PRESENT)
extern const struct MDMA_Link MDMA_MSPI1_Link_TX;
extern const struct MDMA_Link MDMA_MSPI1_Link_RX;
#endif



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
void MDMA_configureChannelEventSource(const struct MDMA_Channel *channel, enum MDMA_EventSource eventSource);
void MDMA_configureChannelPriority(const struct MDMA_Channel *channel, enum MDMA_Priority priority);
enum MCAL_Error MDMA_lockChannel(const struct MDMA_Channel *channel);
enum MCAL_Error MDMA_unlockChannel(const struct MDMA_Channel *channel);


#ifdef __cplusplus
}
#endif

#endif // MDMA_MC_H
