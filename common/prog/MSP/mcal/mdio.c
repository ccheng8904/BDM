/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MDIO
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of mdio.
 *
 *  \details  A detailed file description that may span several lines and
 *            that has to be terminated by an empty line.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mdio.h"

#ifdef MCAL_MDIO_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

const struct MDIO_Config MDIO_INPUT                     = MDIO_CFG_INPUT;
const struct MDIO_Config MDIO_INPUT_PULL_UP             = MDIO_CFG_INPUT_PULL_UP;
const struct MDIO_Config MDIO_INPUT_PULL_DOWN           = MDIO_CFG_INPUT_PULL_DOWN;

const struct MDIO_Config MDIO_OUTPUT                    = MDIO_CFG_OUTPUT;
const struct MDIO_Config MDIO_OUTPUT_OPEN_DRAIN         = MDIO_CFG_OUTPUT_OPEN_DRAIN;
// const struct MDIO_Config MDIO_OUTPUT_OPEN_DRAIN_PULL_UP = ;

const struct MDIO_Config MDIO_ALTERNATE_FUNCTION        = MDIO_CFG_ALTERNATE_FUNCTION;

const struct MDIO_Config MDIO_ANALOG_INPUT              = MDIO_CFG_ANALOG_INPUT;


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MDIO_init(const struct MDIO_Channel *channel, const struct MDIO_Config *cfg)
{
    
    MDIO_enablePort(MDIO_getPort(channel));
    
    MCAL_RETURN_ON_ERROR(MDIO_configure(channel, cfg));
    
    return MCAL_OK;
}


void MDIO_configureOutputState(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_OutputState outputState)
{
    MDIO_Pins pinMask = MDIO_getPinMask(pin);
    
    if(outputState == MDIO_OUTPUT_STATE_LOW)
    {
        MDIO_resetPort(port, pinMask);
    }
    else if(outputState == MDIO_OUTPUT_STATE_HIGH)
    {
        MDIO_setPort(port, pinMask);
    }
    else
    {
        // Nothing to do
    }
}


void MDIO_set(const struct MDIO_Channel *channel)
{
    MDIO_Pins pinMask = MDIO_getPinMask(MDIO_getPin(channel));
    MDIO_setPort(MDIO_getPort(channel), pinMask);
}


void MDIO_reset(const struct MDIO_Channel *channel)
{
    MDIO_Pins pinMask = MDIO_getPinMask(MDIO_getPin(channel));
    MDIO_resetPort(MDIO_getPort(channel), pinMask);
}


void MDIO_toggle(const struct MDIO_Channel *channel)
{
    MDIO_Pins pinMask = MDIO_getPinMask(MDIO_getPin(channel));
    MDIO_togglePort(MDIO_getPort(channel), pinMask);
}


bool MDIO_read(const struct MDIO_Channel *channel)
{
    MDIO_Pins pinMask = MDIO_getPinMask(MDIO_getPin(channel));
    return ((MDIO_readPort(MDIO_getPort(channel)) & pinMask) != 0U);
}


void MDIO_write(const struct MDIO_Channel *channel, bool pinState)
{
    MDIO_Pins pinMask = MDIO_getPinMask(MDIO_getPin(channel));
    const struct MDIO_Port *port = MDIO_getPort(channel);
    
    if(pinState)
    {
        MDIO_setPort(port, pinMask);
    }
    else
    {
        MDIO_resetPort(port, pinMask);
    }
}


#if !defined(MDIO_HAS_FEATURE_ATOMIC_SET)
void MDIO_setPort(const struct MDIO_Port *port, MDIO_Pins pinMask)
{
    MDIO_writePort(port, MDIO_readPortOutput(port) | pinMask);
}
#endif


#if !defined(MDIO_HAS_FEATURE_ATOMIC_RESET)
void MDIO_resetPort(const struct MDIO_Port *port, MDIO_Pins pinMask)
{
    MDIO_writePort(port, MDIO_readPortOutput(port) & ~pinMask);
}
#endif


