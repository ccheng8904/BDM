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
 *  COMP_ABBREV      MDIO
 *  PROCESSOR        SH30F9X71
 ******************************************************************************/
 

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdio_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MDIO.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mdio.h"
#include "mdio_mc.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

const struct MDIO_Port MDIOA = {GPIOA, MDIO_PORTA_INDEX};
const struct MDIO_Port MDIOB = {GPIOB, MDIO_PORTB_INDEX};
const struct MDIO_Port MDIOC = {GPIOC, MDIO_PORTC_INDEX};
const struct MDIO_Port MDIOD = {GPIOD, MDIO_PORTD_INDEX};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void MDIO_enablePort(const struct MDIO_Port *port)
{
    (void)*port;
}


const struct MDIO_Port *MDIO_getPort(const struct MDIO_Channel *channel)
{
    return channel->port;
}


enum MDIO_Pin MDIO_getPin(const struct MDIO_Channel *channel)
{
    return channel->pin;
}


void MDIO_configureOutputType(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_OutputType outputType)
{
    /* get GPIO output type */
    GPIOOType_TypeDef outPutT;
    if (outputType == MDIO_OUTPUT_TYPE_PUSH_PULL )
    {
        outPutT = GPIO_OType_PP;
    }
    else if (outputType == MDIO_OUTPUT_TYPE_OPEN_DRAIN)
    {
        outPutT = GPIO_OType_OD;
    }
    else
    {
        return;
    }

    /* Output driver configuration, Set to default configuration, weak driver */
    port->GPIOx->ODRVR.V32 &= ~((uint32_t)GPIO_ODRVR_ODRVR_SINK_Pos << ((uint16_t)pin * 2U));
    port->GPIOx->ODRVR.V32 |= ((uint32_t)GPIO_ODrv_NORMAL << ((uint16_t)pin * 2U));

    /* Output type configuration */
    port->GPIOx->OTYPER.V32 &= ~((uint32_t)GPIO_OTYPER_OT_Msk << (uint8_t)pin);
    port->GPIOx->OTYPER.V32 |= (uint16_t)(((uint16_t)outPutT) << (uint8_t)pin);
}


void MDIO_configurePullType(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_PullType pullType)
{
    /* get GPIO configuration pullup pulldown */
    GPIOPuPd_TypeDef pupd;
    if (pullType == MDIO_PULL_UP)
    {
        pupd = GPIO_PuPd_UP;
    }
    else
    {
        pupd = GPIO_PuPd_NOPULL;
    }

    /* Pull-up Pull-down configuration   GPIO_CFG_PUPDR_PHDR0_Msk */
    port->GPIOx->PUPDR.V32 &= ~((uint32_t)GPIO_PUPDR_PHDR0_Msk << ((uint16_t)pin * 2U));
    port->GPIOx->PUPDR.V32 |= (((uint32_t)pupd) << ((uint16_t)pin * 2U));
}


void MDIO_configureMode(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_Mode mode)
{
    GPIOMode_TypeDef gpioMode;
    if (mode == MDIO_MODE_DIGITAL_INPUT)
    {
        gpioMode = GPIO_Mode_IN;
    }
    else if (mode == MDIO_MODE_DIGITAL_OUTPUT)
    {
        gpioMode = GPIO_Mode_OUT;
    }
    else
    {
        return;
    }
    
    port->GPIOx->MODER.V32 &= ~((uint32_t)GPIO_MODER_MODER0_Msk << ((uint8_t)pin));
    port->GPIOx->MODER.V32 |= ((uint32_t)gpioMode << (uint8_t)pin);
}


void MDIO_configureSpeed(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_Speed speed)
{
    /* get GPIO configuration mode */
    uint32_t mode = (port->GPIOx->MODER.V32 & ((uint32_t)GPIO_MODER_MODER0_Msk << ((uint8_t)pin))) >> (uint8_t)pin;

    /* set GPIO output type */
    if (mode == (uint32_t)GPIO_Mode_OUT)
    {
        /* Output driver configuration, Set to default configuration, weak driver */
        port->GPIOx->ODRVR.V32 &= ~((uint32_t)GPIO_ODRVR_ODRVR_SINK_Pos << ((uint16_t)pin * 2U));
        port->GPIOx->ODRVR.V32 |= ((uint32_t)speed << ((uint16_t)pin * 2U));
    }
}


void MDIO_configureAlternateFunction(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_AltFunc altFunc)
{
    if ((uint16_t)pin > 0x07U)
    {
        port->GPIOx->AFRH.V32 &= (~((uint32_t)0x0F << (((uint16_t)pin & 0x07U) * 4U)));
        port->GPIOx->AFRH.V32 |= ((uint32_t)altFunc << (((uint16_t)pin & 0x07U) * 4U));
    }
    else
    {
        port->GPIOx->AFRL.V32 &= (~((uint32_t)0x0F << ((uint16_t)pin * 4U)));
        port->GPIOx->AFRL.V32 |= ((uint32_t)altFunc << ((uint16_t)pin * 4U));
    }
}


enum MCAL_Error MDIO_configure(const struct MDIO_Channel *channel, const struct MDIO_Config *cfg)
{
    const struct MDIO_Port* port = MDIO_getPort(channel);
    const enum MDIO_Pin pin = MDIO_getPin(channel);

    /* GPIO init */
    MDIO_configureSpeed(port, pin, channel->scfg.speed);
    MDIO_configureOutputState(port, pin, cfg->outputState);
    MDIO_configureOutputType(port, pin, cfg->outputType);
    MDIO_configurePullType(port, pin, cfg->pullType);
    MDIO_configureMode(port, pin, cfg->mode);

    /* GPIO Alternate function */
    MDIO_configureAlternateFunction(port, pin, channel->scfg.altFunc);

    return MCAL_OK;
}


MDIO_Pins MDIO_readPort(const struct MDIO_Port *port)
{
    return (MDIO_Pins)(port->GPIOx->IDR);
}


MDIO_Pins MDIO_readPortOutput(const struct MDIO_Port *port)
{
    return (MDIO_Pins)(port->GPIOx->ODR);
}


void MDIO_writePort(const struct MDIO_Port *port, MDIO_Pins pinStates)
{
    port->GPIOx->ODR = pinStates;
}


void MDIO_setPort(const struct MDIO_Port *port, MDIO_Pins pinMask)
{
    port->GPIOx->BSRR.V32 = pinMask;
}


void MDIO_resetPort(const struct MDIO_Port *port, MDIO_Pins pinMask)
{
    port->GPIOx->BSRR.V32 = (uint32_t)pinMask << 16;
}
