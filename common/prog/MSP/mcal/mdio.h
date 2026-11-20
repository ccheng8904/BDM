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


#ifndef MDIO_H
#define MDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MDIO.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"

#include <stdint.h>
#include <stdbool.h>

#include "mcal/mcal.h"
#include "mcal/mdio_priv.h"
#include "mcal/mdio_types.h"
#include "mdio_mc.h"


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MDIO_Config MDIO_INPUT;
extern const struct MDIO_Config MDIO_INPUT_PULL_UP;
extern const struct MDIO_Config MDIO_INPUT_PULL_DOWN;

extern const struct MDIO_Config MDIO_OUTPUT;
extern const struct MDIO_Config MDIO_OUTPUT_OPEN_DRAIN;
extern const struct MDIO_Config MDIO_OUTPUT_OPEN_DRAIN_PULL_UP;

extern const struct MDIO_Config MDIO_ALTERNATE_FUNCTION;

extern const struct MDIO_Config MDIO_ANALOG_INPUT;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


/** Initialization function for MDIO.
 *  \note This function initializes all properties on peripheral HW for certain I/O pin.
 *  \param channel: pointer to MDIO_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return void
 */
extern enum MCAL_Error MDIO_init(const struct MDIO_Channel *channel, const struct MDIO_Config *cfg);


/** Function for reading MDIO pin level.
 *  \param channel: pointer to MDIO_Channel instance
 *  \return logical state of the pin
 */
extern bool MDIO_read(const struct MDIO_Channel *channel);


/** Function for writing MDIO pin level.
 *  \param channel: pointer to MDIO_Channel instance
 *  \param pinState: logical level to be written
 */
extern void MDIO_write(const struct MDIO_Channel *channel, bool pinState);


/** Function for setting MDIO pin to HIGH level.
 *  \param channel: pointer to MDIO_Channel instance
 */
extern void MDIO_set(const struct MDIO_Channel *channel);


/** Function for setting MDIO pin to LOW level.
 *  \param channel: pointer to MDIO_Channel instance
 */
extern void MDIO_reset(const struct MDIO_Channel *channel);


/** Function for toggling of MDIO pin level.
 *  \param channel: pointer to MDIO_Channel instance
 */
extern void MDIO_toggle(const struct MDIO_Channel *channel);


/** Function reading of input state of whole port.
 *  \param port: pointer to MDIO_Port instance
 *  \retval logical level of individual pins
 */
extern MDIO_Pins MDIO_readPort(const struct MDIO_Port *port);


/** Function reading of output state set to port.
 *  \param port: pointer to MDIO_Port instance
 *  \retval logical level of individual pins
 */
extern MDIO_Pins MDIO_readPortOutput(const struct MDIO_Port *port);


/** Function writing pin states of whole port.
 *  \param port: pointer to MDIO_Port instance
 *  \param pinStates: level of individual pins to be written
 */
extern void MDIO_writePort(const struct MDIO_Port *port, MDIO_Pins pinStates);


/** Function for setting multiple pins to HIGH level
 *  \param port: pointer to MDIO_Port instance
 *  \param pinMask: masks for pins that will be set to HIGH level
 */
extern void MDIO_setPort(const struct MDIO_Port *port, MDIO_Pins pinMask);


/** Function for setting multiple pins to LOW level
 *  \param port: pointer to MDIO_Port instance
 *  \param pinMask: masks for pins that will be set to LOW level
 */
extern void MDIO_resetPort(const struct MDIO_Port *port, MDIO_Pins pinMask);


/** Function for toggling level of multiple pins
 *  \param port: pointer to MDIO_Port instance
 *  \param pinMask: masks for pins which level will be toggled
 */
extern void MDIO_togglePort(const struct MDIO_Port *port, MDIO_Pins pinMask);


/** Function for configuring the output state of a pin
 *  \param port: pointer to MDIO_Port instance
 *  \param pin: identifier for the pin to configure
 *  \param outputState: the output state of the pin
 */
extern void MDIO_configureOutputState(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_OutputState outputState);


#ifdef __cplusplus
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MDIO
{
    // template <const struct MDIO_Channel tplChannel>
    class Channel
    {
        private:
            const struct MDIO_Channel *channel;
            
        public:
            Channel(const struct MDIO_Channel *channel)
            {
                this->channel = channel;
            }

            enum MCAL_Error init(const struct MDIO_Config *config)
            {
                return MDIO_init(this->channel, config);
            }

            enum MCAL_Error configure(const struct MDIO_Config *config)
            {
                return MDIO_configure(this->channel, config);
            }

            bool read(void)
            {
                return MDIO_read(this->channel);
            }

            void write(bool outputState)
            {
                MDIO_write(this->channel, outputState);
            }

            void set(void)
            {
                MDIO_set(this->channel);
            }

            void reset(void)
            {
                MDIO_reset(this->channel);
            }

            void toggle(void)
            {
                MDIO_toggle(this->channel);
            }
    };
}

#endif // __cplusplus


#endif // MDIO_H
