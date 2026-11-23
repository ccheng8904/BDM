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
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdma_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MDMA.
 *
 *  \details  The CMS32M67xx DMA controller is configured by the vendor driver
 *            (DMA_Init, DMA_Start, DMA_Stop). The DMA channels are triggered by
 *            the DMA vector table, therefore the channel index is the vector
 *            number (flash, ADC, SPI, I2C, UART0, UART1, EPWM, TIMER0).
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mdma_mc.h"
#include "mdma.h"
#include "mdma_priv.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define MDMA_CHANNEL_COUNT          (8U)
#define MDMA_CTRL_ID_DEFAULT        (0U)

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void MDMA_enableClock(void);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

const struct MDMA_PeriphDescriptor MDMA_Descriptor = {DMA, MDMA1_IRQ_INDEX};

const struct MDMA_ChannelDescriptor MDMA_CH0_Descriptor = {DMA, (MDMA_ChannelIndex)DMA_VECTOR_UART0};
const struct MDMA_ChannelDescriptor MDMA_CH1_Descriptor = {DMA, (MDMA_ChannelIndex)DMA_VECTOR_UART1};
const struct MDMA_ChannelDescriptor MDMA_CH2_Descriptor = {DMA, (MDMA_ChannelIndex)DMA_VECTOR_ADC};
const struct MDMA_ChannelDescriptor MDMA_CH3_Descriptor = {DMA, (MDMA_ChannelIndex)DMA_VECTOR_SPI};

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static uint8_t MDMA_LockList[MDMA_CHANNEL_COUNT] = {0};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint8_t MDMA_getChannelNum(const struct MDMA_Channel *channel)
{
    return (uint8_t)channel->desc->index;
}


void MDMA_enablePeriph(const struct MDMA_Periph *periph)
{
    (void)periph;

    MDMA_enableClock();
}


void MDMA_disablePeriph(const struct MDMA_Periph *periph)
{
    (void)periph;

    CGC_PER10PeriphClockCmd(CGC_PER10Periph_DMA, DISABLE);
}


void MDMA_enableChannelReg(const struct MDMA_Channel *channel)
{
    DMA_Start(channel->desc->index);
}


void MDMA_disableChannelReg(const struct MDMA_Channel *channel)
{
    DMA_Stop(channel->desc->index);
}


enum MCAL_Error MDMA_configure(const struct MDMA_Periph *periph)
{
    (void)periph;

    MDMA_enableClock();

    return MCAL_OK;
}


enum MCAL_Error MDMA_configureChannel(const struct MDMA_Channel *channel, const struct MDMA_ChannelConfig *cfg)
{
    DMA_InitTypeDef dmaInitStruct;

    MDMA_enableClock();

    dmaInitStruct.DMA_Vector        = (DMA_VECTOR_t)channel->desc->index;
    dmaInitStruct.DMA_CtrlId        = MDMA_CTRL_ID_DEFAULT;
    dmaInitStruct.DMA_SrcAddr       = cfg->src.addr;
    dmaInitStruct.DMA_DstAddr       = cfg->dst.addr;
    dmaInitStruct.DMA_BufferSize    = cfg->dataLen;
    dmaInitStruct.DMA_SrcInc        = (cfg->src.increment) ? DMA_SrcInc_Enable : DMA_SrcInc_Disable;
    dmaInitStruct.DMA_DstInc        = (cfg->dst.increment) ? DMA_DstInc_Enable : DMA_DstInc_Disable;
    dmaInitStruct.DMA_DataSize      = (uint16_t)((cfg->src.dataWidth == MDMA_WIDTH_8_BIT)  ? DMA_DataSize_Byte :
                                                ((cfg->src.dataWidth == MDMA_WIDTH_16_BIT) ? DMA_DataSize_HalfWord :
                                                                                             DMA_DataSize_Word));
    dmaInitStruct.DMA_Mode          = (uint8_t)((cfg->mode == MDMA_MODE_SINGLE) ? DMA_Mode_Normal : DMA_Mode_Repeat);
    dmaInitStruct.DMA_RepeatAreaAddr = DMA_RepeatAreaAddr_Dst;
    dmaInitStruct.DMA_RepeatInt     = DMA_RepeatInt_Disable;
    dmaInitStruct.DMA_ChainTrans    = DMA_ChainTrans_None;

    DMA_Init(&dmaInitStruct);

    return MCAL_OK;
}


enum MCAL_Error MDMA_configureChannelVolatileParams(const struct MDMA_Channel *channel, const struct MDMA_ChannelConfig *cfg)
{
    return MDMA_configureChannel(channel, cfg);
}


void MDMA_IRQ_enable(const struct MDMA_Periph *periph)
{
    (void)periph;

    /* The CMS32M67xx DMA is triggered by the peripheral interrupts, there is
     * no dedicated DMA interrupt, therefore the interrupts of the DMA
     * sources are used */
}


uint32_t MDMA_readStatusReg(const struct MDMA_Periph *periph)
{
    return (uint32_t)periph->desc->DMAx->DMAIF0;
}


void MDMA_clearChannelEvent(const struct MDMA_Periph *periph, const struct MDMA_Channel *eventChannel)
{
    (void)periph;
    (void)eventChannel;
}


void MDMA_enableChannelEventReg(const struct MDMA_Channel *channel)
{
    (void)channel;
}


void MDMA_disableChannelEventReg(const struct MDMA_Channel *channel)
{
    (void)channel;
}


uint32_t MDMA_getChannelStatusMaskForEvent(const struct MDMA_Periph *periph, uint8_t channelNum)
{
    (void)periph;

    return (uint32_t)1u << channelNum;
}


void MDMA_configureChannelEventSource(const struct MDMA_Channel *channel, enum MDMA_EventSource eventSource)
{
    (void)channel;
    (void)eventSource;
}


void MDMA_configureChannelPriority(const struct MDMA_Channel *channel, enum MDMA_Priority priority)
{
    (void)channel;
    (void)priority;
}


enum MCAL_Error MDMA_lockChannel(const struct MDMA_Channel *channel)
{
    enum MCAL_Error retVal = MCAL_OK;

    if(MDMA_LockList[(uint8_t)channel->desc->index] == 0u)
    {
        MDMA_LockList[(uint8_t)channel->desc->index] = 1u;
    }
    else
    {
        retVal = MCAL_ERROR_BUSY;
    }

    return retVal;
}


enum MCAL_Error MDMA_unlockChannel(const struct MDMA_Channel *channel)
{
    MDMA_LockList[(uint8_t)channel->desc->index] = 0u;

    return MCAL_OK;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void MDMA_enableClock(void)
{
    CGC_PER10PeriphClockCmd(CGC_PER10Periph_DMA, ENABLE);
}
