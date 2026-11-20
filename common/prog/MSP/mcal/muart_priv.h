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
 *  COMP_ABBREV      MUART
 ******************************************************************************/


#ifndef MUART_PRIV_H
#define MUART_PRIV_H

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
 *  \brief    Public definitions and declarations for unit MUART.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "mcal/muart_types.h"
#include "muart_mc.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/** Function for configuring peripheral.
 *  \note This function is used internally by MUART_init function.
 *  \param channel: pointer to MUART_Channel instance
 *  \param cfg: pointer to configuration structure
 */
enum MCAL_Error MUART_configure(const struct MUART_Channel *channel, const struct MUART_Config *cfg);


/** Function for configuring peripheral.
 *  \note This function does not change default baudrate. When the baudrate
 *        needs to be maintained in different power modes, use function
 *        MUART_setNewBaudrate instead.
 *  \param channel: pointer to MUART_Channel instance
 *  \param baudrate: baudrate to be configured
 */
extern void MUART_configureBaudrate(const struct MUART_Channel *channel, uint32_t baudrate);


/** Function for enabling peripheral by setting control register bits.
 *  \note This function is used internally by MUART_enable function.
 *  \param channel: pointer to MUART_Channel instance
 */
extern void MUART_enableChannel(const struct MUART_Channel *channel);


/** Function for disabling peripheral by clearing control register bits.
 *  \note This function is used internally by MUART_disable function.
 *  \param channel: pointer to MUART_Channel instance
 */
extern void MUART_disableChannel(const struct MUART_Channel *channel);


/** Function for reading data from peripheral data register.
 *  \note Before calling this function, it must be certain, that data are
 *        available. This is done by querying read event, or inside read IRQ.
 *  \param channel: pointer to MUART_Channel instance
 *  \return read data from data register;
 */
extern uint16_t MUART_readDataReg(const struct MUART_Channel *channel);


/** Function for writing data to peripheral data register.
 *  \note Before calling this function, it must be certain, that data register
 *        is empty. This is done by querying write event or inside write IRQ.
 *  \param channel: pointer to MUART_Channel instance
 *  \param data: data to be written;
 */
extern void MUART_writeDataReg(const struct MUART_Channel *channel, uint32_t data);


/** Function for reading status register of peripheral.
 *  \note If peripheral has more status registers, the returned status value is
 *        combination of all relevant status register values.
 *  \param channel: pointer to MUART_Channel instance
 *  \return status register(s) value
 */
extern uint32_t MUART_getStatusReg(const struct MUART_Channel *channel);


/** Function for clearing error register of peripheral.
 *  \param channel: pointer to MUART_Channel instance
 */
extern void MUART_clearErrorReg(const struct MUART_Channel *channel);


/** Function for clearing error register of peripheral.
 *  \param channel: pointer to MUART_Channel instance
 */
extern enum MCAL_Error MUART_getChannelError(const struct MUART_Channel *channel);


/** Function for getting status mask for event.
 *  \note Returned status mask is used in combination with MXXX_getStatusReg
 *        function, in case multiple events needs to be checked in scope of one
 *        routine. This approach prevents reading actual status register
 *        multiple times, thus in some platform clearing flags by reading it.
 *  \param channel: pointer to MUART_Channel instance
 *  \param event: event for which the status mask will be returned
 *  \return status mask
 */
extern uint32_t MUART_getStatusRegMaskForEvent(const struct MUART_Channel *channel, enum MCAL_Event event);


/** Function for checking if event is pending.
 *  \note This function reads status register of peripheral and compares it
 *        with event mask.
 *  \param channel: pointer to MUART_Channel instance
 *  \param event: event that will be checked
 *  \return true if event is pending
 */
extern bool MUART_isEvent(const struct MUART_Channel *channel, enum MCAL_Event event);


/** Function sets control register bits according to selected event.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MUART_Channel instance
 *  \param event: event for which the register bits will be set
 */
extern void MUART_IRQ_enableEvent(const struct MUART_Channel *channel, enum MCAL_Event event);


/** Function clears control register bits according to selected event.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MUART_Channel instance
 *  \param event: event for which the register bits will be cleared
 */
extern void MUART_IRQ_disableEvent(const struct MUART_Channel *channel, enum MCAL_Event event);


/** Function for enabling interrupt for selected peripheral unit.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MUART_Channel instance
 */
extern void MUART_IRQ_enable(const struct MUART_Channel *channel);


/** Function for handling of ISR for one or more peripheral units.
 *  \note This function is not intended to be used by application
 *  \param index: IRQ index representing for which units the interrupt is pending
 */
extern void MUART_IRQ_Handler(enum MUART_IRQ_Index index);


// Legacy support (DBus idle detection)
extern bool MUART_isRxOngoing(const struct MUART_Channel *channel);


extern enum MUART_IRQ_Index MUART_IRQ_getIndex(const struct MUART_Channel *channel);


#ifdef __cplusplus
}
#endif

#endif // MUART_PRIV_H
