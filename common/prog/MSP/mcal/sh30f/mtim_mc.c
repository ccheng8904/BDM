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
 *  COMP_ABBREV      MTIM
 *  PROCESSOR        SH30f9x71
 ******************************************************************************/
 

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mtim_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MTIM.
 *
 */

/*lint -save -e715 Symbol * not referenced [MISRA 2012 Rule 2.7, required] */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "math/mathDiv.h"
#include "mtim_mc.h"
#include "mtim_priv.h"
#include "msup.h"
#include "utility/utility.h"
#include "IntTbl.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif

#ifdef MCAL_MTIM_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define PCA_WRITE_UNLOCK_KEY        0x33CC

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static volatile uint32_t *MTIM_getPcaChannelCCMR_Reg(const struct MTIM_ChannelDescriptor *channelDesc);
static PCA_TypeDef *MTIM_getPcaChannelRegs(const struct MTIM_Channel *channel);
static void MTIM_enablePcaClock(const struct MTIM_Pca *timer);
static enum MCAL_Error MTIM_configurePcaPrescaler(const struct MTIM_Pca *pca, uint32_t prescaler);
static enum MCAL_Error MTIM_configurePcaReloadValue(const struct MTIM_Pca *pca, uint32_t reloadValue);
static enum MCAL_Error MTIM_configurePcaDirection(const struct MTIM_Pca *pca, enum MTIM_TimerDirection direction);
#ifdef CURRENTLY_UNUSED_INTERFACE
static enum MCAL_Error MTIM_configureTimerMode(const struct MTIM_Timer *timer, enum MTIM_TimerMode mode);
#endif

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
/* "Side effect in initializer list, volatile access": false positive, as volatile register is not accessed */
/*Axivion Disable Style MisraC2012-13.1  : false positive, no volatile access, just pointer used */

/* Switch off: Note 923: cast from 'unsigned int' to 'TIM_TypeDef *' */
/*                       cast from 'unsigned int' to 'RCU_TypeDef *' */
//lint -emacro(923,TIM_PCA0,TIM_PCA1,TIM_PCA2,TIM_PCA3)
//lint -emacro(923,RCC)
const struct MTIM_PcaDescriptor MTIM_PCA0_Descriptor = 
{
    PCA0, 2u, {&RCC->APB0ENR.V32,RCC_APB0ENR_PCA0EN_Msk}, MTIM_PCA0_IRQ_INDEX
};
const struct MTIM_ChannelDescriptor MTIM_PCA0_CH0_Descriptor    = {&MTIM_PCA0_Descriptor, &PCA0->CCR0.V32, MTIM_CHANNEL0_INDEX};
const struct MTIM_ChannelDescriptor MTIM_PCA0_CH1_Descriptor    = {&MTIM_PCA0_Descriptor, &PCA0->CCR1.V32, MTIM_CHANNEL1_INDEX};

const struct MTIM_PcaDescriptor MTIM_PCA1_Descriptor = 
{
    PCA1, 2u, {&RCC->APB0ENR.V32,RCC_APB0ENR_PCA1EN_Msk}, MTIM_PCA1_IRQ_INDEX
};
const struct MTIM_ChannelDescriptor MTIM_PCA1_CH0_Descriptor    = {&MTIM_PCA1_Descriptor, &PCA1->CCR0.V32, MTIM_CHANNEL0_INDEX};
const struct MTIM_ChannelDescriptor MTIM_PCA1_CH1_Descriptor    = {&MTIM_PCA1_Descriptor, &PCA1->CCR1.V32, MTIM_CHANNEL1_INDEX};

const struct MTIM_PcaDescriptor MTIM_PCA2_Descriptor = 
{
    PCA2, 2u, {&RCC->APB0ENR.V32,RCC_APB0ENR_PCA2EN_Msk}, MTIM_PCA2_IRQ_INDEX
};
const struct MTIM_ChannelDescriptor MTIM_PCA2_CH0_Descriptor    = {&MTIM_PCA2_Descriptor, &PCA2->CCR0.V32, MTIM_CHANNEL0_INDEX};
const struct MTIM_ChannelDescriptor MTIM_PCA2_CH1_Descriptor    = {&MTIM_PCA2_Descriptor, &PCA2->CCR1.V32, MTIM_CHANNEL1_INDEX};

const struct MTIM_PcaDescriptor MTIM_PCA3_Descriptor = 
{
    PCA3, 2u, {&RCC->APB0ENR.V32,RCC_APB0ENR_PCA3EN_Msk}, MTIM_PCA3_IRQ_INDEX
};
const struct MTIM_ChannelDescriptor MTIM_PCA3_CH0_Descriptor    = {&MTIM_PCA3_Descriptor, &PCA3->CCR0.V32, MTIM_CHANNEL0_INDEX};
const struct MTIM_ChannelDescriptor MTIM_PCA3_CH1_Descriptor    = {&MTIM_PCA3_Descriptor, &PCA3->CCR1.V32, MTIM_CHANNEL1_INDEX};

/*Axivion Enable Style MisraC2012-13.1 */
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static const uint32_t MTIM_PcaChannelEventTable[] =
{
    PCA_SR_CC0IF_Msk,
    PCA_SR_CC1IF_Msk
};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS FOR TIMER                                      */
/******************************************************************************/

enum MCAL_Error MTIM_configurePcaClockSource(const struct MTIM_Pca *pca, PCA_CLOCK_SOURCE_Type clockSource)
{
    uint32_t pcaCFGR = pca->desc->regsPCAx->CFGR.V32;

    pca->desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    pcaCFGR &= ~(PCA_CFGR_CPS_Msk);
    pcaCFGR |= (uint32_t)clockSource;
    pca->desc->regsPCAx->CFGR.V32 = pcaCFGR;

    pca->desc->regsPCAx->LCKR = 0;
    return MCAL_OK;
}

void MTIM_enableTimer(const struct MTIM_Timer *timer)
{
    timer->pca.desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    timer->pca.desc->regsPCAx->CR |= PCA_CR_PEN_Msk;

    timer->pca.desc->regsPCAx->LCKR = 0;
}


void MTIM_disableTimer(const struct MTIM_Timer *timer)
{
    timer->pca.desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    timer->pca.desc->regsPCAx->CR &= ~PCA_CR_PEN_Msk;

    timer->pca.desc->regsPCAx->LCKR = 0;
}


enum MCAL_Error MTIM_configureTimer(const struct MTIM_Timer *timer, const struct MTIM_TimerConfig *config)
{
    MTIM_disableTimer(timer);


    MTIM_enablePcaClock(&(timer->pca));
    
    //initialize prescaler, value, and direction
    MCAL_RETURN_ON_ERROR(MTIM_configurePcaPrescaler(&(timer->pca), config->prescaler));
    MCAL_RETURN_ON_ERROR(MTIM_configurePcaReloadValue(&(timer->pca), config->reloadValue));
    MCAL_RETURN_ON_ERROR(MTIM_configurePcaDirection(&(timer->pca), config->direction));

    /* Perform static configuration */
    MCAL_RETURN_ON_ERROR(MTIM_configurePcaClockSource(&(timer->pca), timer->pca.scfg.clockSource));

    MTIM_enableTimer(timer);
    return MCAL_OK;
}


void MTIM_enableTimerEventReg(const struct MTIM_Timer *timer)
{
    timer->pca.desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    timer->pca.desc->regsPCAx->CFGR.V32 |= PCA_CFGR_CIE_Msk;

    timer->pca.desc->regsPCAx->LCKR= 0;
}


void MTIM_disableTimerEventReg(const struct MTIM_Timer *timer)
{
    timer->pca.desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    timer->pca.desc->regsPCAx->CFGR.V32 &= ~PCA_CFGR_CIE_Msk;

    timer->pca.desc->regsPCAx->LCKR = 0;
}


enum MTIM_IRQ_Index MTIM_IRQ_getIndex(const struct MTIM_Timer *timer)
{
    return timer->pca.desc->pcaIrqIndex;
}


void MTIM_IRQ_enable(const struct MTIM_Timer *timer)
{
    static const IRQn_Type irqTable[MTIM_IRQ_CNT] =
    {
        [MTIM_PCA0_IRQ_INDEX]       = PCA0_IRQn,
        [MTIM_PCA1_IRQ_INDEX]       = PCA1_IRQn,
        [MTIM_PCA2_IRQ_INDEX]       = PCA2_IRQn,
        [MTIM_PCA3_IRQ_INDEX]       = PCA3_IRQn
    };
    
    IRQn_Type irq = irqTable[(uint8_t)timer->pca.desc->pcaIrqIndex];

    NVIC_EnableIRQ(irq);
}


uint32_t MTIM_readTimerCountReg(const struct MTIM_Timer *timer)
{
    uint32_t timerCount;
    timerCount = timer->pca.desc->regsPCAx->CNT.V32;
    return timerCount;
}


void MTIM_writeTimerCountReg(const struct MTIM_Timer *timer, uint32_t value)
{
    timer->pca.desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    timer->pca.desc->regsPCAx->CNT.V32 = value;

    timer->pca.desc->regsPCAx->LCKR = 0;
}


uint32_t MTIM_readAndClearEventReg(const struct MTIM_Timer *timer)
{
    uint32_t status = timer->pca.desc->regsPCAx->SR.V32;
    timer->pca.desc->regsPCAx->SR.V32 |= ((status<<16u) & (PCA_SR_CIFC_Msk | PCA_SR_PIFC_Msk | PCA_SR_CC1IFC_Msk | PCA_SR_CC0IFC_Msk));
    return status;
}


uint32_t MTIM_getTimerStatusMaskForEvent(const struct MTIM_Timer *timer)
{
    return PCA_SR_CIF_Msk;
}

enum MPCM_Clock MTIM_getTimerClockBus(const struct MTIM_Timer *timer)
{
    uint32_t  clkSource = timer->pca.desc->regsPCAx->CFGR.BIT.CPS;
    const enum MPCM_Clock clocks[] =
    {
        MPCM_CLOCK_APB1,
        MPCM_CLOCK_END_INDICATOR,
        MPCM_CLOCK_LSICLK,
        MPCM_CLOCK_LSECLK,
        MPCM_CLOCK_HSECLK_DIV8
    };
    if(clkSource < (uint32_t)TIM0OVERFLOW)
    {
        return clocks[clkSource];
    }
    else
    {
        return MPCM_CLOCK_END_INDICATOR;
    }
}


uint32_t MTIM_getNextValidPrescaler(const struct MTIM_Timer *timer, uint32_t prescaler)
{
    return (prescaler <= 0xFFFFu) ? (prescaler + 1u) : 0xFFFFu;
}


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS FOR CHANNEL                                    */
/******************************************************************************/

uint8_t MTIM_getChannelNum(const struct MTIM_Channel *channel)
{
    return (uint8_t)channel->channelDesc->index;
}

enum MCAL_Error MTIM_configureChannelCapture(const struct MTIM_Channel *channel, enum MTIM_ChannelCaptureTrigger trigger)
{   
    PCA_TypeDef *pcaRegs;
    static const uint8_t captureTriggerTable[__MTIM_CHANNEL_CAPTURE_TRIGGER_COUNT] =
    {
        [MTIM_CHANNEL_CAPTURE_TRIGGER_DISABLED]         = 0u,                             //equal to RISING_EDGE_TRIGGER
        [MTIM_CHANNEL_CAPTURE_TRIGGER_RISING]           = (uint8_t)RISING_EDGE_TRIGGER,
        [MTIM_CHANNEL_CAPTURE_TRIGGER_FALLING]          = (uint8_t)FALLING_EDGE_TRIGGER,
        [MTIM_CHANNEL_CAPTURE_TRIGGER_RISING_FALLING]   = (uint8_t)ARBITRARY_EDGE_TRIGGER
    };

    uint32_t regMask;
    uint32_t regValue;

    volatile uint32_t *regCCMRx = MTIM_getPcaChannelCCMR_Reg(channel->channelDesc);
    pcaRegs = MTIM_getPcaChannelRegs(channel);

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    //not clean counter when capture
    *regCCMRx &= ~PCA_CCMR0_CC_Msk;

    //compare mode configure
    regMask = (uint32_t)PCA_CCMR0_SM_Msk;
    regValue = (uint32_t)CAPTUREMODE << PCA_CCMR0_SM_Pos;
    MSUP_writeReg32(regCCMRx, regMask, regValue);

    //trigger configure
    regMask = (uint32_t)PCA_CCMR0_FS_Msk;
    regValue = (uint32_t)captureTriggerTable[trigger] << PCA_CCMR0_FS_Pos;
    MSUP_writeReg32(regCCMRx, regMask, regValue);

    pcaRegs->LCKR = 0;
    return MCAL_OK;
}


enum MCAL_Error MTIM_configureChannelCompare(const struct MTIM_Channel *channel, enum MTIM_ChannelCompareAction action)
{
    (void)action;  //only support toggle

    PCA_TypeDef *pcaRegs;
    volatile uint32_t *regCCMRx = MTIM_getPcaChannelCCMR_Reg(channel->channelDesc);
    pcaRegs = MTIM_getPcaChannelRegs(channel);
    uint32_t regMask;
    uint32_t regValue;

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    //compare mode configure
    regMask = (uint32_t)PCA_CCMR0_SM_Msk;
    regValue = (uint32_t)COMPAREMODE << PCA_CCMR0_SM_Pos;
    MSUP_writeReg32(regCCMRx, regMask, regValue);

    //continuous match configure
    regMask = (uint32_t)PCA_CCMR0_FS_Msk;
    regValue = (uint32_t)CONTINUOUS << PCA_CCMR0_FS_Pos;
    MSUP_writeReg32(regCCMRx, regMask, regValue);

    //initial output level is low
    regMask = (uint32_t)PCA_CCMR0_TCP_Msk;
    regValue = (uint32_t)INITAL_LOW << PCA_CCMR0_TCP_Pos;
    MSUP_writeReg32(regCCMRx, regMask, regValue);

    pcaRegs->LCKR = 0;
    return MCAL_OK;
}


enum MCAL_Error MTIM_configureChannelPWM(const struct MTIM_Channel *channel)
{
    PCA_TypeDef *pcaRegs;
    volatile uint32_t *regCCMRx = MTIM_getPcaChannelCCMR_Reg(channel->channelDesc);
    pcaRegs = MTIM_getPcaChannelRegs(channel);
    uint32_t regMask;
    uint32_t regValue;

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    //PWM mode configure
    regMask = (uint32_t)PCA_CCMR0_SM_Msk;
    regValue = (uint32_t)PWMMODE << PCA_CCMR0_SM_Pos;
    MSUP_writeReg32(regCCMRx, regMask, regValue);

    //pwm mode selection
    /*PCAx counting mode selection has higher priority than PWM mode selection*/
    if(pcaRegs->CFGR.BIT.SDEN != 0u)  //up-counting
    {
        //16 bit PWM2 sawtoolth wave
        regMask = (uint32_t)PCA_CCMR0_FS_Msk;
        regValue = (uint32_t)PWM_16BIT << PCA_CCMR0_FS_Pos;
        MSUP_writeReg32(regCCMRx, regMask, regValue);
    }
    else
    {
        //16 bit PWM4 triangular wave
        regMask = (uint32_t)PCA_CCMR0_FS_Msk;
        regValue = (uint32_t)PWM_16BIT << PCA_CCMR0_FS_Pos;
        MSUP_writeReg32(regCCMRx, regMask, regValue);
    }

    //output level is low(no inversion)
    regMask = (uint32_t)PCA_CCMR0_TCP_Msk;
    regValue = (uint32_t)ACTIVE_LOW << PCA_CCMR0_TCP_Pos;
    MSUP_writeReg32(regCCMRx, regMask, regValue);

    pcaRegs->LCKR = 0;
    return MCAL_OK;
}


void MTIM_enableChannel(const struct MTIM_Channel *channel)
{
    PCA_TypeDef *pcaRegs;
    volatile uint32_t *regCCMRx = MTIM_getPcaChannelCCMR_Reg(channel->channelDesc);
    pcaRegs = MTIM_getPcaChannelRegs(channel);

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    *regCCMRx |= (uint32_t)PCA_CCMR0_CEN_Msk;
    
    pcaRegs->LCKR = 0;
}


void MTIM_disableChannel(const struct MTIM_Channel *channel)
{
    PCA_TypeDef *pcaRegs;
    volatile uint32_t *regCCMRx = MTIM_getPcaChannelCCMR_Reg(channel->channelDesc);
    pcaRegs = MTIM_getPcaChannelRegs(channel);

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    *regCCMRx &= (uint32_t)(~PCA_CCMR0_CEN_Msk);
    
    pcaRegs->LCKR = 0;
}


uint32_t MTIM_readChannelValueReg(const struct MTIM_Channel *channel)
{
    return *channel->channelDesc->countReg;
}


void MTIM_writeChannelValueReg(const struct MTIM_Channel *channel, uint32_t value)
{
    PCA_TypeDef *pcaRegs;
    pcaRegs = MTIM_getPcaChannelRegs(channel);

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    *channel->channelDesc->countReg = value;

    pcaRegs->LCKR = 0;
}


void MTIM_enableChannelEventReg(const struct MTIM_Channel *channel)
{
    /* Enable channel output */
    PCA_TypeDef *pcaRegs;
    volatile uint32_t *regCCMRx = MTIM_getPcaChannelCCMR_Reg(channel->channelDesc);
    pcaRegs = MTIM_getPcaChannelRegs(channel);

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    *regCCMRx |= (uint32_t)PCA_CCMR0_CCIE_Msk;
    
    pcaRegs->LCKR = 0;
}


void MTIM_disableChannelEventReg(const struct MTIM_Channel *channel)
{
    /* Enable channel output */
    PCA_TypeDef *pcaRegs;
    volatile uint32_t *regCCMRx = MTIM_getPcaChannelCCMR_Reg(channel->channelDesc);
    pcaRegs = MTIM_getPcaChannelRegs(channel);

    pcaRegs->LCKR = PCA_WRITE_UNLOCK_KEY;

    *regCCMRx &= (uint32_t)(~PCA_CCMR0_CCIE_Msk);
    
    pcaRegs->LCKR = 0;
}


uint8_t MTIM_getTimerChannelCount(const struct MTIM_Timer *timer)
{
    return timer->pca.desc->pcaChannelCount;
}


uint32_t MTIM_getChannelStatusMaskForEvent(const struct MTIM_Timer *timer, uint8_t channelNum)
{
    return MTIM_PcaChannelEventTable[channelNum];
}


const struct MDIO_Channel *MTIM_getChannelPin(const struct MTIM_Channel *channel)
{
    return channel->pin;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static volatile uint32_t *MTIM_getPcaChannelCCMR_Reg(const struct MTIM_ChannelDescriptor *channelDesc)
{    
    volatile uint32_t *regCCMR;
    /*lint -e{9016} "advisory: Pointer arithmetic not allowed [+=] MisraC2012 18.4" Tested to work correctly */
    if(channelDesc->index == MTIM_CHANNEL0_INDEX)
    {
        regCCMR = &(channelDesc->pcaDesc->regsPCAx->CCMR0.V32);
    }
    else
    {
        regCCMR = &(channelDesc->pcaDesc->regsPCAx->CCMR1.V32);
    }
    return regCCMR;
}

static enum MCAL_Error MTIM_configurePcaPrescaler(const struct MTIM_Pca *pca, uint32_t prescaler)
{
    pca->desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    pca->desc->regsPCAx->PSC = prescaler - 1u;

    pca->desc->regsPCAx->LCKR = 0;
    return MCAL_OK;
}

static enum MCAL_Error MTIM_configurePcaReloadValue(const struct MTIM_Pca *pca, uint32_t reloadValue)
{
    pca->desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    pca->desc->regsPCAx->PR.V32 = reloadValue;

    pca->desc->regsPCAx->LCKR = 0;
    return MCAL_OK;
}

//pca only support up and up_down
static enum MCAL_Error MTIM_configurePcaDirection(const struct MTIM_Pca *pca, enum MTIM_TimerDirection direction)
{
    // Choose counting direction
    pca->desc->regsPCAx->LCKR = PCA_WRITE_UNLOCK_KEY;

    if(direction == MTIM_TIMER_DIRECTION_UP_DOWN)
    {
        //pca->desc->regsPCAx->CFGR.V32 |= PCA_CFGR_SDEN_Msk;
        pca->desc->regsPCAx->CFGR.BIT.SDEN = (uint32_t)DOUBLEEDGE;
    }
    else if(direction == MTIM_TIMER_DIRECTION_UP)
    {
        pca->desc->regsPCAx->CFGR.BIT.SDEN = (uint32_t)SINGLEEDGE;
    }
    else
    {
        // down counting or default - nothing to set
    }

    pca->desc->regsPCAx->LCKR = 0;
    return MCAL_OK;
}

#ifdef CURRENTLY_UNUSED_INTERFACE
static enum MCAL_Error MTIM_configureTimerMode(const struct MTIM_Timer *timer, enum MTIM_TimerMode mode)
{
    if(mode == MTIM_TIMER_MODE_ENCODER)
    {
        return MCAL_ERROR;
    }
    
    return MCAL_OK;
}
#endif

static void MTIM_enablePcaClock(const struct MTIM_Pca *timer)
{
    //disable RCC register modification protection
    RCC_REGS_UNLOCK();
    
    *timer->desc->clockEnable.enableReg |= timer->desc->clockEnable.enableMask;

    //enable RCC register modification protection
    RCC_REGS_LOCK();
}

static PCA_TypeDef *MTIM_getPcaChannelRegs(const struct MTIM_Channel *channel)
{
    return channel->channelDesc->pcaDesc->regsPCAx;
}
