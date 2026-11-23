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
 *  PROCESSOR        CMS32M67xx
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
 *  \details  MDIO mapping for the CMS32M67xx (Cmsemicon) port logic.
 *
 *            CMS32M67xx port registers:
 *             - Pm      port output latch
 *             - PMm     port mode            (0 = output, 1 = input)
 *             - PUm     port pull-up         (1 = pull-up on)
 *             - PDm     port pull-down       (1 = pull-down on)
 *             - POMm    port output mode     (1 = N-ch open drain)
 *             - PMCm    port mode control    (1 = analog function)
 *             - PSETm/PCLRm atomic set/clear
 *             - PxyCFG  pin function select  (0 = GPIO), one byte per pin,
 *                       16 bit spaced (0x0800 + (port << 5) + (pin << 1))
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mdio.h"
#include "mdio_mc.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define MDIO_PORT_REG(port)     ((port)->PORTx)
#define MDIO_PORT_IDX(port)     ((uint32_t)(port)->portIndex)

#define MDIO_REG_P(port)        (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->P0    + MDIO_PORT_IDX(port))))
#define MDIO_REG_PM(port)       (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->PM0   + MDIO_PORT_IDX(port))))
#define MDIO_REG_PU(port)       (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->PU0   + MDIO_PORT_IDX(port))))
#define MDIO_REG_PD(port)       (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->PD0   + MDIO_PORT_IDX(port))))
#define MDIO_REG_POM(port)      (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->POM0  + MDIO_PORT_IDX(port))))
#define MDIO_REG_PMC(port)      (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->PMC0  + MDIO_PORT_IDX(port))))
#define MDIO_REG_PSET(port)     (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->PSET0 + MDIO_PORT_IDX(port))))
#define MDIO_REG_PCLR(port)     (*((volatile uint8_t *)(&MDIO_PORT_REG(port)->PCLR0 + MDIO_PORT_IDX(port))))

/* the pin configuration registers are 16 bit spaced, starting with P00CFG */
#define MDIO_REG_PINCFG(port, pin) \
    (*((volatile uint8_t *)&MDIO_PORT_REG(port)->P00CFG + (MDIO_PORT_IDX(port) << 5) + ((uint32_t)(pin) << 1)))


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static uint8_t MDIO_getPinMask8(enum MDIO_Pin pin)
{
    return (uint8_t)(1U << (uint8_t)pin);
}


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

const struct MDIO_Port MDIOA = {PORT, MDIO_PORTA_INDEX};
const struct MDIO_Port MDIOB = {PORT, MDIO_PORTB_INDEX};
const struct MDIO_Port MDIOC = {PORT, MDIO_PORTC_INDEX};
const struct MDIO_Port MDIOD = {PORT, MDIO_PORTD_INDEX};
const struct MDIO_Port MDIOE = {PORT, MDIO_PORTE_INDEX};
const struct MDIO_Port MDIOF = {PORT, MDIO_PORTF_INDEX};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void MDIO_enablePort(const struct MDIO_Port *port)
{
    /* The CMS32M67xx port pins do not have a dedicated clock enable */
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
    const uint8_t pinMask = MDIO_getPinMask8(pin);

    if(outputType == MDIO_OUTPUT_TYPE_OPEN_DRAIN)
    {
        MDIO_REG_POM(port) |= pinMask;
    }
    else if(outputType == MDIO_OUTPUT_TYPE_PUSH_PULL)
    {
        MDIO_REG_POM(port) &= (uint8_t)(~pinMask);
    }
    else
    {
        /* open source output is not available, keep the configuration */
    }
}


void MDIO_configurePullType(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_PullType pullType)
{
    const uint8_t pinMask = MDIO_getPinMask8(pin);

    /* pull-up and pull-down are exclusive, first disable both of them */
    MDIO_REG_PU(port) &= (uint8_t)(~pinMask);
    MDIO_REG_PD(port) &= (uint8_t)(~pinMask);

    if(pullType == MDIO_PULL_UP)
    {
        MDIO_REG_PU(port) |= pinMask;
    }
    else if(pullType == MDIO_PULL_DOWN)
    {
        MDIO_REG_PD(port) |= pinMask;
    }
    else
    {
        /* no pull resistor */
    }
}


void MDIO_configureMode(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_Mode mode)
{
    const uint8_t pinMask = MDIO_getPinMask8(pin);

    if(mode == MDIO_MODE_ANALOG)
    {
        MDIO_REG_PMC(port) |= pinMask;
        MDIO_REG_PM(port)  |= pinMask;
    }
    else if(mode == MDIO_MODE_DIGITAL_INPUT)
    {
        MDIO_REG_PMC(port) &= (uint8_t)(~pinMask);
        MDIO_REG_PM(port)  |= pinMask;
    }
    else
    {
        /* digital output and alternate function outputs */
        MDIO_REG_PMC(port) &= (uint8_t)(~pinMask);
        MDIO_REG_PM(port)  &= (uint8_t)(~pinMask);
    }
}


void MDIO_configureSpeed(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_Speed speed)
{
    /* The CMS32M67xx port pins do not provide a configurable output speed */
    (void)port;
    (void)pin;
    (void)speed;
}


void MDIO_configureAlternateFunction(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_AltFunc altFunc)
{
    MDIO_REG_PINCFG(port, pin) = (uint8_t)altFunc;
}


enum MCAL_Error MDIO_configure(const struct MDIO_Channel *channel, const struct MDIO_Config *cfg)
{
    const struct MDIO_Port *port = MDIO_getPort(channel);
    const enum MDIO_Pin pin     = MDIO_getPin(channel);

    MDIO_configurePullType(port, pin, MDIO_PULL_DISABLED);
    MDIO_configureOutputType(port, pin, cfg->outputType);
    MDIO_configureMode(port, pin, cfg->mode);

    /* select peripheral function, 0 = GPIO */
    MDIO_configureAlternateFunction(port, pin, channel->scfg.altFunc);

    MDIO_configurePullType(port, pin, cfg->pullType);
    MDIO_configureOutputState(port, pin, cfg->outputState);

    return MCAL_OK;
}


MDIO_Pins MDIO_readPort(const struct MDIO_Port *port)
{
    /* PMS selects the digital input level of the pin for reading */
    MDIO_PORT_REG(port)->PMS = 0x01U;

    return (MDIO_Pins)MDIO_REG_P(port);
}


MDIO_Pins MDIO_readPortOutput(const struct MDIO_Port *port)
{
    return (MDIO_Pins)MDIO_REG_P(port);
}


void MDIO_writePort(const struct MDIO_Port *port, MDIO_Pins pinStates)
{
    MDIO_REG_P(port) = (uint8_t)pinStates;
}






void MDIO_togglePort(const struct MDIO_Port *port, MDIO_Pins pinMask)
{
    MDIO_REG_P(port) ^= (uint8_t)pinMask;
}
