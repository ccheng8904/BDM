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
 *  PROCESSOR        CMS32M67xx
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
 *  \details  CMS32M67xx timer mapping:
 *
 *             - TMR0 / TMR1 are 32 bit down counters with reload register.
 *               CON.TMREN enables the timer, CON.TMRIE the overflow interrupt,
 *               CON.TMRSZ selects the 32 bit mode, CON.TMRPRE the prescaler
 *               (1, 16, 256).
 *             - the clocks are enabled by CGC_PER11Periph_TIMER01.
 */

/*lint -save -e715 Symbol * not referenced [MISRA 2012 Rule 2.7, required] */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "math/mathDiv.h"
#include "mtim_mc.h"
#include "mtim_priv.h"
#include "msup.h"

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

#define MTIM_CON_ENABLE_MASK        (TMR_CON_TMREN_Msk)
#define MTIM_CON_INT_MASK           (TMR_CON_TMRIE_Msk)
#define MTIM_CON_32BIT_MASK         (TMR_CON_TMRSZ_Msk)

/* prescaler values defined by TMR_CON_TMRPRE */
#define MTIM_PRESCALER_1            1U
#define MTIM_PRESCALER_16           16U
#define MTIM_PRESCALER_256          256U

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static uint32_t MTIM_getPrescalerBits(uint32_t prescaler);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
/* "Side effect in initializer list, volatile access": false positive, as volatile register is not accessed */
/*Axivion Disable Style MisraC2012-13.1  : false positive, no volatile access, just pointer used */

/* Switch off: Note 923: cast from 'unsigned int' to 'TMR_T *' */
//lint -emacro(923,TMR0,TMR1)
const struct MTIM_TimerDescriptor MTIM_TMR0_Descriptor =
{
    TMR0, 1u, MTIM_TMR0_IRQ_INDEX, TIMER0_IRQn
};

const struct MTIM_TimerDescriptor MTIM_TMR1_Descriptor =
{
    TMR1, 1u, MTIM_TMR1_IRQ_INDEX, TIMER1_IRQn
};

/*Axivion Enable Style MisraC2012-13.1 */
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static const uint32_t MTIM_TimerEventTable[] =
{
    TMR_RIS_RIS_Msk
};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS FOR TIMER                                      */
/******************************************************************************/

void MTIM_enableTimer(const struct MTIM_Timer *timer)
{
    timer->desc->regsTMRx->CON |= MTIM_CON_ENABLE_MASK;
}


void MTIM_disableTimer(const struct MTIM_Timer *timer)
{
    timer->desc->regsTMRx->CON &= ~MTIM_CON_ENABLE_MASK;
}


enum MCAL_Error MTIM_configureTimer(const struct MTIM_Timer *timer, const struct MTIM_TimerConfig *config)
{
    uint32_t conReg;

    MTIM_disableTimer(timer);

    /* enable the clock of the timer unit */
    CGC_PER11PeriphClockCmd(CGC_PER11Periph_TIMER01, ENABLE);

    conReg = timer->desc->regsTMRx->CON;
    conReg &= ~(MTIM_CON_32BIT_MASK | TMR_CON_TMRPRE_Msk | TMR_CON_TMRMS_Msk);

    /* the CMS32M67xx reload timer counts down in continuous mode, the
     * up/down and one shot modes are not supported */
    conReg |= MTIM_CON_32BIT_MASK;

    conReg |= MTIM_getPrescalerBits(config->prescaler);

    timer->desc->regsTMRx->CON = conReg;

    /* reload value, the timer counts from the reload value down to zero */
    timer->desc->regsTMRx->LOAD   = config->reloadValue;
    timer->desc->regsTMRx->BGLOAD = config->reloadValue;
    /* the counter value is read only, LOAD and BGLOAD define the count */

    MTIM_enableTimer(timer);

    return MCAL_OK;
}


void MTIM_enableTimerEventReg(const struct MTIM_Timer *timer)
{
    timer->desc->regsTMRx->CON |= MTIM_CON_INT_MASK;
}


void MTIM_disableTimerEventReg(const struct MTIM_Timer *timer)
{
    timer->desc->regsTMRx->CON &= ~MTIM_CON_INT_MASK;
}


enum MTIM_IRQ_Index MTIM_IRQ_getIndex(const struct MTIM_Timer *timer)
{
    return timer->desc->irqIndex;
}


void MTIM_IRQ_enable(const struct MTIM_Timer *timer)
{
    NVIC_EnableIRQ(timer->desc->irqNum);
}


uint32_t MTIM_readTimerCountReg(const struct MTIM_Timer *timer)
{
    return timer->desc->regsTMRx->VAL;
}


void MTIM_writeTimerCountReg(const struct MTIM_Timer *timer, uint32_t value)
{
    /* the counter value is read only, reload is done by BGLOAD */
    timer->desc->regsTMRx->BGLOAD = value;
    (void)value;
}


uint32_t MTIM_readAndClearEventReg(const struct MTIM_Timer *timer)
{
    const uint32_t status = timer->desc->regsTMRx->RIS;

    /* the CMS32M67xx clears all timer interrupt flags by writing ICLR */
    timer->desc->regsTMRx->ICLR = status;

    return status;
}


uint32_t MTIM_getTimerStatusMaskForEvent(const struct MTIM_Timer *timer)
{
    (void)timer;

    return MTIM_TimerEventTable[0];
}


enum MPCM_Clock MTIM_getTimerClockBus(const struct MTIM_Timer *timer)
{
    (void)timer;

    /* the reload timers are clocked by the main clock */
    return MPCM_CLOCK_PCLK0;
}


uint32_t MTIM_getNextValidPrescaler(const struct MTIM_Timer *timer, uint32_t prescaler)
{
    (void)timer;

    if(prescaler <= MTIM_PRESCALER_1)
    {
        return MTIM_PRESCALER_1;
    }
    else if(prescaler <= MTIM_PRESCALER_16)
    {
        return MTIM_PRESCALER_16;
    }
    else
    {
        return MTIM_PRESCALER_256;
    }
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
    (void)channel;
    (void)trigger;

    /* capture is not available for the 32 bit reload timers */
    return MCAL_ERROR;
}


enum MCAL_Error MTIM_configureChannelCompare(const struct MTIM_Channel *channel, enum MTIM_ChannelCompareAction action)
{
    (void)channel;
    (void)action;

    return MCAL_ERROR;
}


enum MCAL_Error MTIM_configureChannelPWM(const struct MTIM_Channel *channel)
{
    (void)channel;

    return MCAL_ERROR;
}


void MTIM_enableChannel(const struct MTIM_Channel *channel)
{
    (void)channel;
}


void MTIM_disableChannel(const struct MTIM_Channel *channel)
{
    (void)channel;
}


uint32_t MTIM_readChannelValueReg(const struct MTIM_Channel *channel)
{
    return channel->channelDesc->timerDesc->regsTMRx->VAL;
}


void MTIM_writeChannelValueReg(const struct MTIM_Channel *channel, uint32_t value)
{
    channel->channelDesc->timerDesc->regsTMRx->LOAD = value;
}


void MTIM_enableChannelEventReg(const struct MTIM_Channel *channel)
{
    (void)channel;
}


void MTIM_disableChannelEventReg(const struct MTIM_Channel *channel)
{
    (void)channel;
}


uint8_t MTIM_getTimerChannelCount(const struct MTIM_Timer *timer)
{
    return timer->desc->channelCount;
}


uint32_t MTIM_getChannelStatusMaskForEvent(const struct MTIM_Timer *timer, uint8_t channelNum)
{
    (void)timer;
    (void)channelNum;

    return 0u;
}


const struct MDIO_Channel *MTIM_getChannelPin(const struct MTIM_Channel *channel)
{
    return channel->pin;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static uint32_t MTIM_getPrescalerBits(uint32_t prescaler)
{
    uint32_t bits;

    if(prescaler <= MTIM_PRESCALER_1)
    {
        bits = TMR_CLK_DIV_1;
    }
    else if(prescaler <= MTIM_PRESCALER_16)
    {
        bits = TMR_CLK_DIV_16;
    }
    else
    {
        bits = TMR_CLK_DIV_256;
    }

    return bits;
}
