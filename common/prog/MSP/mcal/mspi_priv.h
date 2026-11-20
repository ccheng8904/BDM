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
 *  COMP_ABBREV      MSPI
 ******************************************************************************/


#ifndef MSPI_PRIV_H
#define MSPI_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mspi_priv.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Private definitions and declarations for unit MSPI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "mspi_types.h"
#include "mspi_mc.h"


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Function for configuring peripheral.
 *  \note This function is used internally by MSPI_init function.
 *  \param channel: pointer to MSPI_Channel instance
 *  \param     cfg: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_configure(const struct MSPI_Channel *channel, const struct MSPI_Config *cfg);


/** Function for configuring peripheral.
 *  \note This function changes clock frequency on SCK (clock) pin.
 *        Desired value cannot be achieved on all platforms.
 *        The clock frequency is configured to be equal or lower than desired value.
 *  \param  channel: pointer to MSPI_Channel instance
 *  \param baudrate: baudrate to be configured
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_configureBaudrate(const struct MSPI_Channel *channel, uint32_t baudrate);


/** Function for enabling peripheral by setting control register bits.
 *  \note This function is used internally by MSPI_enable function.
 *  \param channel: pointer to MSPI_Channel instance
 */
extern void MSPI_enableChannel(const struct MSPI_Channel *channel);


/** Function for disabling peripheral by clearing control register bits.
 *  \note This function is used internally by MSPI_disable function.
 *  \param channel: pointer to MSPI_Channel instance
 */
extern void MSPI_disableChannel(const struct MSPI_Channel *channel);


/** Function for writing data to peripheral data register.
 *  \note Before calling this function it must be certain that data register
 *        is empty. This is done either by querying write event or internal write IRQ.
 *  \param channel: pointer to MSPI_Channel instance
 *  \param    data: data to be written
 */
extern void MSPI_writeDataReg(const struct MSPI_Channel *channel, uint32_t data);


/** Function for reading data from peripheral data register.
 *  \note Before calling this function it must be certain that data are
 *        available. This is done either by querying read event or internal read IRQ.
 *  \param channel: pointer to MSPI_Channel instance
 *  \return read data from data register
 */
extern uint16_t MSPI_readDataReg(const struct MSPI_Channel *channel);


/** Function for reading peripheral status register.
 *  \note If peripheral has more status registers, returned status value is
 *        combination of all relevant status register values.
 *  \param channel: pointer to MSPI_Channel instance
 *  \return actual value of status register(s)
 */
extern uint32_t MSPI_getStatusReg(const struct MSPI_Channel *channel);


/** Function for clearing peripheral error register.
 *  \param channel: pointer to MSPI_Channel instance
 */
extern void MSPI_clearErrorReg(const struct MSPI_Channel *channel);


/** Function for getting status mask for event.
 *  \note Returned status mask is used in combination with MXXX_getStatusReg
 *        function, in case multiple events need to be checked in scope of one
 *        routine. This approach prevents reading actual status register multiple
 *        times which for some platforms causes clearing flags (by reading them).
 *  \param event: event for which status mask will be returned
 *  \return event status mask
 */
extern uint32_t MSPI_getStatusRegMaskForEvent(enum MCAL_Event event);


/** Function for querying interrupt index for given channel instance.
 *
 *  \param channel: pointer to MSPI_Channel instance
 */
extern enum MSPI_IRQ_Index MSPI_IRQ_getIndex(const struct MSPI_Channel *channel);


/** Function for enabling interrupt for given peripheral unit.
 *
 *  \param channel: pointer to MSPI_Channel instance
 */
extern void MSPI_IRQ_enable(const struct MSPI_Channel *channel);


/** Function sets control register bits for given event.
 *
 *  \param channel: pointer to MSPI_Channel instance
 *  \param   event: event for which register bits will be set
 */
extern void MSPI_IRQ_enableEvent(const struct MSPI_Channel *channel, enum MCAL_Event event);


/** Function clears control register bits for given event.
 *
 *  \param channel: pointer to MSPI_Channel instance
 *  \param   event: event for which register bits will be cleared
 */
extern void MSPI_IRQ_disableEvent(const struct MSPI_Channel *channel, enum MCAL_Event event);


#ifdef __cplusplus
}
#endif

#endif // MSPI_PRIV_H
