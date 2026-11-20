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
 *  COMP_ABBREV      MEXTI
 *  PROCESSOR        SH30F9X71
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mexti_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MEXTI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mexti_mc.h"
#include "mexti.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define MEXTI_STATUS_MASK_PIN0  0x00000001U

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static uint32_t MEXTI_getPinMask(const struct MEXTI_Channel *channel);
static void MEXTI_enableClock(const struct MEXTI_Channel *channel);


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


void MEXTI_configureTrigger(const struct MEXTI_Channel *channel, enum MEXTI_Trigger trigger)
{
    uint8_t portNum = (uint8_t)MDIO_getPort(channel->mextiPin)->portIndex;
    uint8_t pinNum = (uint8_t)MDIO_getPin(channel->mextiPin);
    uint32_t pinMask = 1UL << pinNum;

    // set EXTI port source for selected pin
    // Note 9030: cannot cast an unsigned value to an enum type [MISRA 2012 Rule 10.5, advisory]
    EXTI_PinConfig((EXTI_Port_Type)portNum, (EXTI_PIN_Type)pinNum);//lint !e9030
        
    // set to default
    EXTI->TMSR &= (~pinMask);//edge trigger
    EXTI->RTSR &= (~pinMask);//disable rising trigger
    EXTI->FTSR &= (~pinMask);//disable falling trigger

    if(trigger == MEXTI_TRIGGER_RISING)
    {
        EXTI->RTSR |= pinMask;
    }
    else if(trigger == MEXTI_TRIGGER_FALLING)
    {
        EXTI->FTSR |= pinMask;
    }
    else if(trigger == MEXTI_TRIGGER_RISING_FALLING)
    {
        EXTI->RTSR |= pinMask;
        EXTI->FTSR |= pinMask;
    }
    else if(trigger == MEXTI_TRIGGER_LOW_LEVEL)
    {
        EXTI->TMSR |= pinMask;
        EXTI->FTSR |= pinMask;

    }
    else if(trigger == MEXTI_TRIGGER_HIGH_LEVEL)
    {
        EXTI->TMSR |= pinMask;
        EXTI->RTSR |= pinMask;
    }
    else
    {
        //assert
    }
}


void MEXTI_configureChannel(const struct MEXTI_Channel *channel, const struct MEXTI_Config *cfg)
{
    MEXTI_enableClock(channel);
    MEXTI_configureTrigger(channel, cfg->trigger);
}


void MEXTI_enableEventReg(const struct MEXTI_Channel *channel)
{
    uint32_t pinMask = MEXTI_getPinMask(channel);
    
    EXTI->IMR |= pinMask;
}


void MEXTI_disableEventReg(const struct MEXTI_Channel *channel)
{
    uint32_t pinMask = MEXTI_getPinMask(channel);
    
    EXTI->IMR &= ~pinMask;
}


void MEXTI_clearEventReg(const struct MEXTI_Channel *channel)
{
    uint32_t pinMask = MEXTI_getPinMask(channel);
    
    EXTI->PR.V32 |= (uint32_t)(pinMask << 16);
}


uint32_t MEXTI_getStatusReg(const struct MEXTI_Channel *channel)
{
    (void)*channel;
    uint32_t status;
    
    status  = EXTI->PR.V32 & 0xFFFFu;
    
    return status;
}


uint32_t MEXTI_getStatusRegMaskForEvent(const struct MEXTI_Channel *channel)
{
    uint8_t pinNum = (uint8_t)MDIO_getPin(channel->mextiPin);
    uint32_t statusMask = (uint32_t)MEXTI_STATUS_MASK_PIN0 << pinNum;
    
    return statusMask;
}


enum MEXTI_IRQ_Index MEXTI_IRQ_getIndex(const struct MEXTI_Channel *channel)
{
    enum MDIO_Pin pin = MDIO_getPin(channel->mextiPin);
    
    if(pin == MDIO_PIN0)
    {
        return MEXTI0_IRQ_INDEX;
    }
    else if(pin == MDIO_PIN1)
    {
        return MEXTI1_IRQ_INDEX;
    }
    else if(pin == MDIO_PIN2)
    {
        return MEXTI2_IRQ_INDEX;
    }
    else if(pin == MDIO_PIN3)
    {
        return MEXTI3_IRQ_INDEX;
    }
    else if(pin < MDIO_PIN10)
    {
        return MEXTI9_4_IRQ_INDEX;
    }
    else
    {
        return MEXTI15_10_IRQ_INDEX;
    }
}


void MEXTI_IRQ_enable(const struct MEXTI_Channel *channel)
{
    enum MEXTI_IRQ_Index index = MEXTI_IRQ_getIndex(channel);
    
    if(index == MEXTI0_IRQ_INDEX)
    {
        NVIC_EnableIRQ(EXTI0_IRQn);
    }
    else if(index == MEXTI1_IRQ_INDEX)
    {
        NVIC_EnableIRQ(EXTI1_IRQn);
    }
    else if(index == MEXTI2_IRQ_INDEX)
    {
        NVIC_EnableIRQ(EXTI2_IRQn);
    }
    else if(index == MEXTI3_IRQ_INDEX)
    {
        NVIC_EnableIRQ(EXTI3_IRQn);
    }
    else if(index == MEXTI9_4_IRQ_INDEX)
    {
        NVIC_EnableIRQ(EXTI9_4_IRQn);
    }
    else
    {
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
}


const struct MDIO_Channel * MEXTI_getPin(const struct MEXTI_Channel *channel)
{
    return channel->mextiPin;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static uint32_t MEXTI_getPinMask(const struct MEXTI_Channel *channel)
{
    uint8_t pinNum = (uint8_t)MDIO_getPin(channel->mextiPin);
    return (uint32_t)1u << pinNum;
}


static void MEXTI_enableClock(const struct MEXTI_Channel *channel)
{
    (void)*channel;
    RCC_APB0PeriphClockOnOff(RCC_APB0_EXTI, SH_ON);
}
