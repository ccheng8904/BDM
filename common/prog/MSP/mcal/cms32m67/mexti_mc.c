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
 *  PROCESSOR        CMS32M67xx
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
 *  \details  CMS32M67xx external interrupt mapping:
 *
 *             - the pin of an external interrupt channel INTPn is selected by
 *               the register INTnCFG (value = port << 4 | pin)
 *             - the edge is configured by INTM->EGP0 (rising) and INTM->EGN0
 *               (falling), both bits used together select both edges
 *             - the IRQ number is equal to the external interrupt channel
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mexti_mc.h"
#include "mexti.h"

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/* P00CFG ... P57CFG (16 bit spaced) followed by INT0CFG ... INT3CFG */
#define MEXTI_PINCFG_BASE_OFFSET    (0x0800U)
#define MEXTI_INT_CFG_OFFSET        (0x0860U)

#define MEXTI_REG_INT_CFG(index)    (*((volatile uint8_t *)&PORT->P00CFG + MEXTI_INT_CFG_OFFSET + (uint32_t)(index)))

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static uint8_t MEXTI_getPinMask(const struct MEXTI_Channel *channel);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/


void MEXTI_configureTrigger(const struct MEXTI_Channel *channel, enum MEXTI_Trigger trigger)
{
    const enum MDIO_Pin pinNum  = MDIO_getPin(channel->mextiPin);
    const uint32_t pinIdx       = (uint32_t)pinNum;
    const uint8_t pinMask       = MEXTI_getPinMask(channel);

    /* connect the external interrupt channel to the selected pin */
    MEXTI_REG_INT_CFG(channel->irqIndex) = (uint8_t)(((uint32_t)MDIO_getPort(channel->mextiPin)->portIndex << 4)
                                                     | pinIdx);

    /* edge selection, both bits cleared == no trigger */
    INTM->EGP0 &= (uint8_t)(~pinMask);
    INTM->EGN0 &= (uint8_t)(~pinMask);

    if(trigger == MEXTI_TRIGGER_RISING)
    {
        INTM->EGP0 |= pinMask;
    }
    else if(trigger == MEXTI_TRIGGER_FALLING)
    {
        INTM->EGN0 |= pinMask;
    }
    else if(trigger == MEXTI_TRIGGER_RISING_FALLING)
    {
        INTM->EGP0 |= pinMask;
        INTM->EGN0 |= pinMask;
    }
    else
    {
        /* level triggered external interrupts are not supported */
    }
}


void MEXTI_configureChannel(const struct MEXTI_Channel *channel, const struct MEXTI_Config *cfg)
{
    MEXTI_configureTrigger(channel, cfg->trigger);
}


void MEXTI_enableEventReg(const struct MEXTI_Channel *channel)
{
    /* the trigger configuration of the channel has to be done before by
     * MEXTI_configureTrigger, therefore only the NVIC interrupt is enabled */
    NVIC_EnableIRQ((IRQn_Type)((int32_t)INTP0_IRQn + (int32_t)channel->irqIndex));
}


void MEXTI_disableEventReg(const struct MEXTI_Channel *channel)
{
    const uint8_t pinMask = MEXTI_getPinMask(channel);

    INTM->EGP0 &= (uint8_t)(~pinMask);
    INTM->EGN0 &= (uint8_t)(~pinMask);
}


void MEXTI_clearEventReg(const struct MEXTI_Channel *channel)
{
    /* the CMS32M67xx external interrupt request is cleared automatically by
     * entering the interrupt service routine, therefore only the pending
     * interrupt in the NVIC is cleared here */
    NVIC_ClearPendingIRQ((IRQn_Type)((int32_t)INTP0_IRQn + (int32_t)channel->irqIndex));
}


uint32_t MEXTI_getStatusReg(const struct MEXTI_Channel *channel)
{
    /* the external interrupt request flags are not readable, an active edge
     * configuration is reported as pending event */
    const uint8_t pinMask = MEXTI_getPinMask(channel);
    uint32_t status = 0U;

    if(((INTM->EGP0 & pinMask) != 0U) || ((INTM->EGN0 & pinMask) != 0U))
    {
        status = (uint32_t)pinMask;
    }

    return status;
}


uint32_t MEXTI_getStatusRegMaskForEvent(const struct MEXTI_Channel *channel)
{
    return (uint32_t)MEXTI_getPinMask(channel);
}


enum MEXTI_IRQ_Index MEXTI_IRQ_getIndex(const struct MEXTI_Channel *channel)
{
    return channel->irqIndex;
}


void MEXTI_IRQ_enable(const struct MEXTI_Channel *channel)
{
    NVIC_EnableIRQ((IRQn_Type)((int32_t)INTP0_IRQn + (int32_t)channel->irqIndex));
}


const struct MDIO_Channel * MEXTI_getPin(const struct MEXTI_Channel *channel)
{
    return channel->mextiPin;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static uint8_t MEXTI_getPinMask(const struct MEXTI_Channel *channel)
{
    return (uint8_t)(1U << (uint8_t)MDIO_getPin(channel->mextiPin));
}
