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
 

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdma_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MDMA.
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

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
static DMA_TypeDef *MDMA_getRegs(const struct MDMA_Periph *periph);
static void MDMA_Init(const DMA_CHNO_Type ChannelNO, const DMA_InitTypeDef *InitCfg);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
/* Axivion Disable Style MisraC2012-11.3 : Conversion between pointers to object type */
/* Axivion Disable Style MisraC2012-11.8 : Cast removes volatile qualification */
/* Axivion Disable Style MisraC2012-10.3 : Inappropriate implicit conversion from uint32_t to uint8_t */
/* Axivion Disable Style MisraC2012-18.4 : Pointer arithmetic not allowed [+] */


/* Note 929: explicit cast from 'volatile uint32_t *' (aka 'volatile unsigned int *') to 'DMA_CH_TypeDef *' [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required] */
/* Note 9005: cast drops volatile qualifier [MISRA 2012 Rule 11.8, required] */
//lint -save -e929 -e9005
const struct MDMA_PeriphDescriptor MDMA_Descriptor = {DMA, MDMA1_IRQ_INDEX};
const struct MDMA_ChannelDescriptor MDMA_CH0_Descriptor = {DMA_CH0, DMA_CH_NO_0};
const struct MDMA_ChannelDescriptor MDMA_CH1_Descriptor = {DMA_CH1, DMA_CH_NO_1};
const struct MDMA_ChannelDescriptor MDMA_CH2_Descriptor = {DMA_CH2, DMA_CH_NO_2};
const struct MDMA_ChannelDescriptor MDMA_CH3_Descriptor = {DMA_CH3, DMA_CH_NO_3};
//lint -restore -e929 -e9005
/*Axivion Disable MisraC2012-13.1 : Side effect in initializer list */
#if defined(MSPI0_PRESENT)
const struct MDMA_Link MDMA_MSPI0_Link_TX = 
{
    (uint32_t)&SPI0->TDR, 
    MDMA_WIDTH_8_BIT,
    false
};
const struct MDMA_Link MDMA_MSPI0_Link_RX = 
{
    (uint32_t)&SPI0->RDR, 
    MDMA_WIDTH_8_BIT,
    false
};
#endif

#if defined(MSPI1_PRESENT)
const struct MDMA_Link MDMA_MSPI1_Link_TX = 
{
    (uint32_t)&SPI1->TDR, 
    MDMA_WIDTH_8_BIT,
    false
};

const struct MDMA_Link MDMA_MSPI1_Link_RX = 
{
    (uint32_t)&SPI1->RDR, 
    MDMA_WIDTH_8_BIT,
    false
};
#endif
/* Axivion Enable Style MisraC2012-13.1 */ 

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static uint8_t MDMA_LockList[4]={0};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

uint8_t MDMA_getChannelNum(const struct MDMA_Channel *channel)
{
    return (uint8_t)channel->desc->index;
}


/* Note 9029: an unsigned value and an enum value cannot be used together as operands to |= [MISRA 2012 Rule 10.4, required] */
//lint -save -e9029
void MDMA_enablePeriph(const struct MDMA_Periph *periph)
{
    (void)periph;
    
    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->AHBENR.V32 |= (uint32_t)RCC_AHB_DMA;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}


void MDMA_disablePeriph(const struct MDMA_Periph *periph)
{
    (void)periph;
        
    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    RCC->AHBENR.V32 &= ~(uint32_t)RCC_AHB_DMA;

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}
//lint -restore -e9029

void MDMA_enableChannelReg(const struct MDMA_Channel *channel)
{ 
    /* Note 9034: cannot assign 'unsigned8' to different essential type 'enum (DMA_CHNO_Type)' [MISRA 2012 Rule 10.3, required] */
    /* Note 9029: an unsigned value and a signed value cannot be used together as operands to |= [MISRA 2012 Rule 10.4, required] */
    //lint -save -e9034 -e9029
    DMA_CHx((uint32_t)channel->desc->index)->CCR.BIT.EN = (uint32_t)SH_ON;
    if(((uint32_t)ADC + ADC_Sequence_0) != channel->desc->dma_t->SAR)
    {
        /* clear interrupt flags */
        DMA->IFCR.V32 = (((((uint32_t) 1) << DMA_IFCR_CBEIF_Pos) \
                        | (((uint32_t) 1) << DMA_IFCR_CTCIF_Pos) \
                        | (((uint32_t) 1) << DMA_IFCR_CHTIF_Pos) \
                        | (((uint32_t) 1) << DMA_IFCR_CTEIF_Pos))
                        << (uint32_t)channel->desc->index);

        /* software trigger to start DMA */
        DMA->CSR.BIT.SWTRG |= (uint8_t)(1u << (uint32_t)channel->desc->index);
    }
    //lint -restore -e9034 -e9029
}

void MDMA_disableChannelReg(const struct MDMA_Channel *channel)
{
    DMA_CHx((uint32_t)channel->desc->index)->CCR.BIT.EN = (uint32_t)SH_OFF;
}


enum MCAL_Error MDMA_configure(const struct MDMA_Periph *periph)
{
    (void)periph;
    return MCAL_OK;
}


enum MCAL_Error MDMA_configureChannel(const struct MDMA_Channel *channel, const struct MDMA_ChannelConfig *cfg)
{
    DMA_InitTypeDef dmaInitStruct = {0};

    dmaInitStruct.Reload = SH_DISABLE;
    dmaInitStruct.Priority = (uint8_t)channel->scfg.priority;
    dmaInitStruct.StreamSelect = (uint8_t)channel->scfg.eventSource;
    dmaInitStruct.BurstIdle = BURST_IDLE_32CYCLE;
    dmaInitStruct.OneBurstLen = (uint32_t)BURST_LEN_1UNIT;
    dmaInitStruct.BurstCount = (uint32_t)cfg->dataLen - 1U;
    dmaInitStruct.DestAddrType = (uint32_t)DMA_ADDR_INC;
    dmaInitStruct.SourceAddrType = (uint32_t)DMA_ADDR_INC;
    dmaInitStruct.DestUnitSize = (uint8_t)cfg->dst.dataWidth;
    dmaInitStruct.SourceUnitSize = (uint8_t)cfg->src.dataWidth;
    dmaInitStruct.DestAddr = (uint32_t)cfg->dst.addr;
    dmaInitStruct.SourceAddr = (uint32_t)cfg->src.addr;

    if (cfg->src.addr == (uint32_t)((uint32_t)ADC + ADC_Sequence_0))
    {
        dmaInitStruct.TriggerMode = (uint32_t)DMA_TRGMODE_TOEND;
    }

    if(cfg->direction == MDMA_DIRECTION_MEM_TO_MEM)
    {
        dmaInitStruct.TriggerMode = (uint32_t)DMA_TRGMODE_TOEND;
    }

    if (cfg->dst.increment == true)
    {
        dmaInitStruct.DestAddrType = (uint32_t)DMA_ADDR_INC;
    }
    else
    {
        dmaInitStruct.DestAddrType = (uint32_t)DMA_ADDR_FIXED;
    }
    
    if (cfg->src.increment == true)
    {
        dmaInitStruct.SourceAddrType = (uint32_t)DMA_ADDR_INC;
    }
    else
    {
        dmaInitStruct.SourceAddrType = (uint32_t)DMA_ADDR_FIXED;
    }

    if (cfg->mode == MDMA_MODE_SINGLE)
    {
        dmaInitStruct.Reload = SH_DISABLE;
    }
    else
    {
        dmaInitStruct.Reload = SH_ENABLE;
    }

    MDMA_Init(channel->desc->index, &dmaInitStruct);

    return MCAL_OK;
}


enum MCAL_Error MDMA_configureChannelVolatileParams(const struct MDMA_Channel *channel, const struct MDMA_ChannelConfig *cfg)
{
    /* TODO  needs to be investigated, which parameter changes after send/receive and therefore needs to be configured again */
    return MDMA_configureChannel(channel, cfg);
}


uint32_t MDMA_readStatusReg(const struct MDMA_Periph *periph)
{
    return MDMA_getRegs(periph)->IFSR.V32;
}


void MDMA_clearChannelEvent(const struct MDMA_Periph *periph, const struct MDMA_Channel *eventChannel)
{
    /* Note 9034: cannot assign 'unsigned8' to different essential type 'enum (DMA_CHNO_Type)' [MISRA 2012 Rule 10.3, required] */
    //lint -save -e9034
    MDMA_getRegs(periph)->IFCR.V32 = ((uint32_t)DMA_TRANS_COMPLETE << MDMA_getChannelNum(eventChannel));
    MDMA_getRegs(periph)->IFCR.V32 = ((uint32_t)DMA_TRANS_ERROR << MDMA_getChannelNum(eventChannel));
    //lint -restore -e9034
}


uint32_t MDMA_getChannelStatusMaskForEvent(const struct MDMA_Periph *periph, uint8_t channelNum)
{
    (void)periph;
    return (uint32_t)DMA_TRANS_COMPLETE << channelNum;
}


/* Note 9034: cannot assign 'unsigned8' to different essential type 'enum (DMA_CHNO_Type)' [MISRA 2012 Rule 10.3, required] */
/* Note 9029: an unsigned value and an enum value cannot be used together as operands to |= [MISRA 2012 Rule 10.4, required] */
//lint -save -e9034 -e9029
void MDMA_enableChannelEventReg(const struct MDMA_Channel *channel)
{
    uint32_t ccr;
    ccr = DMA_CHx(MDMA_getChannelNum(channel))->CCR.V32;

    /* Enable the selected DMA interrupts */
    ccr |= (uint32_t)DMA_INT_COMPLETE;
    ccr |= (uint32_t)DMA_INT_ERROR;

    /* disable enable bit first */
    DMA_CHx(MDMA_getChannelNum(channel))->CCR.V32 &= (~DMA_CCR0_EN_Msk);
    DMA_CHx(MDMA_getChannelNum(channel))->CCR.V32 = ccr;
}


void MDMA_disableChannelEventReg(const struct MDMA_Channel *channel)
{
    uint32_t ccr;
    ccr = DMA_CHx(MDMA_getChannelNum(channel))->CCR.V32;

    /* Disable the selected DMA interrupts */
    ccr &= ~(uint32_t)DMA_INT_COMPLETE;
    ccr &= ~(uint32_t)DMA_INT_ERROR;

    /* disable enable bit first */
    DMA_CHx(MDMA_getChannelNum(channel))->CCR.V32 &= (~DMA_CCR0_EN_Msk);
    DMA_CHx(MDMA_getChannelNum(channel))->CCR.V32 = ccr;


}
//lint -restore -e9034 -e9029


enum MDMA_IRQ_Index MDMA_IRQ_getIndex(const struct MDMA_Periph *periph)
{
    return periph->desc->irqIndex;
}


void MDMA_IRQ_enable(const struct MDMA_Periph *periph)
{
    (void)periph;
    NVIC_EnableIRQ(DMA_CH0_1_IRQn);
    NVIC_EnableIRQ(DMA_CH2_3_IRQn);
}


void MDMA_configureChannelEventSource(const struct MDMA_Channel *channel, enum MDMA_EventSource eventSource)
{
    channel->desc->dma_t->CCR.BIT.STRMSEL = (uint8_t)eventSource;
}


void MDMA_configureChannelPriority(const struct MDMA_Channel *channel, enum MDMA_Priority priority)
{
    channel->desc->dma_t->CCR.BIT.PL = (uint8_t)priority;
}


/* Note 9050: dependence placed on operator precedence (operators '||' and '==') [MISRA 2012 Rule 12.1, required] */
//lint -save -e9050
enum MCAL_Error MDMA_lockChannel(const struct MDMA_Channel *channel)
{
    bool_t ret = DMA_IsBusy(channel->desc->index);
    if (ret == SH_TRUE || MDMA_LockList[channel->desc->index] == 1u)
    {
        return MCAL_ERROR_BUSY;
    }
    else
    {
        MDMA_LockList[channel->desc->index] = 1u;
        return MCAL_OK;
    }
}
//lint -restore -e9050

enum MCAL_Error MDMA_unlockChannel(const struct MDMA_Channel *channel)
{
    MDMA_LockList[channel->desc->index] = 0u;
    return MCAL_OK;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static void MDMA_Init(const DMA_CHNO_Type ChannelNO, const DMA_InitTypeDef *InitCfg)
{
    DMA_CH_TypeDef *pDMAChannel;

    /* config DMA ChannelNO */
    pDMAChannel = DMA_CHx((uint32_t)ChannelNO);

    /* disable DMA EN first */
    pDMAChannel->CCR.V32 &= ~((uint32_t)DMA_CCR0_EN_Msk);

    pDMAChannel->NPKT = (uint16_t)InitCfg->BurstCount;
    pDMAChannel->DAR = InitCfg->DestAddr;
    pDMAChannel->SAR = InitCfg->SourceAddr;

    /* enable / disable reload function */
    DMA->CSR.V32 |= (uint32_t)InitCfg->Reload << ((uint32_t)DMA_CSR_RELOAD_Pos + (uint32_t)ChannelNO);

    /* set burst idle cycles */
    DMA->CSR.BIT.BURSTIDLE = (uint8_t)InitCfg->BurstIdle;

    pDMAChannel->CCR.V32 = *((uint32_t *)InitCfg);
}


static DMA_TypeDef *MDMA_getRegs(const struct MDMA_Periph *periph)
{
    return periph->desc->DMAx;
}
/* Axivion Enable Style MisraC2012-11.8 */
/* Axivion Enable Style MisraC2012-11.3 */
/* Axivion Enable Style MisraC2012-10.3 */
/* Axivion Enable Style MisraC2012-18.4 */
