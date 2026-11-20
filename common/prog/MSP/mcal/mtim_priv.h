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
 *  COMP_ABBREV      MTIM
 ******************************************************************************/


#ifndef MTIM_PRIV_H
#define MTIM_PRIV_H

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
 *  \brief    Private definitions and declarations for unit MTIM.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "mcal/mcal.h"
#include "mcal/mtim_types.h"
#include "mtim_mc.h"
#include "mcal/mpcm.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS FOR TIMER                                    */
/******************************************************************************/

/** Function for configuring timer peripheral.
 *  \note This function is used internally by MTIM_init function.
 *  \param timer: pointer to MTIM_Timer instance
 *  \param config: pointer to configuration structure
 */
extern enum MCAL_Error MTIM_configureTimer(const struct MTIM_Timer *timer, const struct MTIM_TimerConfig *config);


/** Function for enabling peripheral by setting control register bits.
 *  \note This function is used internally by MTIM_enable function.
 *  \param timer: pointer to MTIM_Timer instance
 */
extern void MTIM_enableTimer(const struct MTIM_Timer *timer);


/** Function for disabling peripheral by clearing control register bits.
 *  \note This function is used internally by MTIM_disable function.
 *  \param timer: pointer to MTIM_Timer instance
 */
extern void MTIM_disableTimer(const struct MTIM_Timer *timer);


/** Function for enabling timer event by setting control register bits.
 *  \note This function is used internally by MTIM_enableTimerEvent function.
 *  \param timer: pointer to MTIM_Timer instance
 */
extern void MTIM_enableTimerEventReg(const struct MTIM_Timer *timer);


/** Function for disabling timer event by clearing control register bits.
 *  \note This function is used internally by MTIM_disableTimerEvent function.
 *  \param timer: pointer to MTIM_Timer instance
 */
extern void MTIM_disableTimerEventReg(const struct MTIM_Timer *timer);


/** The function reads the timer status register and clears interrupt event flags in an atomic way.
 *  \note In case the timer's (and its channels) status flags are distributed
 *        across multiple registers, this function combines these registers
 *        into one 32-bit value.
 *  \param timer: pointer to MTIM_Timer instance
*/
extern uint32_t MTIM_readAndClearEventReg(const struct MTIM_Timer *timer);


/** Function for retrieving status mask for timer event.
 *  \param timer: pointer to MTIM_Timer instance
 */
extern uint32_t MTIM_getTimerStatusMaskForEvent(const struct MTIM_Timer *timer);


/** Function for reading timer count register.
 *  \note This function is used internally by MTIM_readTimerCount function.
 *  \param timer: pointer to MTIM_Timer instance
 *  \return current timer count value
 */
extern uint32_t MTIM_readTimerCountReg(const struct MTIM_Timer *timer);


/** Function for writing timer count register.
 *  \note This function is used internally by MTIM_writeTimerCount function.
 *  \param timer: pointer to MTIM_Timer instance
 *  \param value: new timer count to be written
 */
extern void MTIM_writeTimerCountReg(const struct MTIM_Timer *timer, uint32_t value);


/** Function for querying number of channels for timer unit.
 *  \param timer: pointer to MTIM_Timer instance
 *  \return Number of channels for timer instances
 */
extern uint8_t MTIM_getTimerChannelCount(const struct MTIM_Timer *timer);


/** Function for getting clock bus enumeration type for given timer unit.
 *  \param timer: pointer to MTIM_Timer instance
 *  \return Clock bus enumeration value
 */
extern enum MPCM_Clock MTIM_getTimerClockBus(const struct MTIM_Timer *timer);


/** Function for getting next valid prescaler value for given timer unit.
 *  \param timer: pointer to MTIM_Timer instance
 *  \param prescaler: requested prescaler value
 *  \return next valid prescaler value
 */
extern uint32_t MTIM_getNextValidPrescaler(const struct MTIM_Timer *timer, uint32_t prescaler);


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS FOR CHANNEL                                  */
/******************************************************************************/

/** Function for configuring peripheral.
 *  \note This function is used internally by MTIM_init function.
 *  \param channel: pointer to MTIM_Channel instance
 *  \param config: pointer to configuration structure
 */
extern enum MCAL_Error MTIM_configureChannel(const struct MTIM_Channel *channel, const struct MTIM_ChannelConfig *config);


extern enum MCAL_Error MTIM_configureChannelMode(const struct MTIM_Channel *channel, enum MTIM_ChannelMode mode);


extern enum MCAL_Error MTIM_configureChannelCapture(const struct MTIM_Channel *channel, enum MTIM_ChannelCaptureTrigger trigger);


extern enum MCAL_Error MTIM_configureChannelCompare(const struct MTIM_Channel *channel, enum MTIM_ChannelCompareAction action);


extern enum MCAL_Error MTIM_configureChannelPWM(const struct MTIM_Channel *channel);


extern void MTIM_enableChannel(const struct MTIM_Channel *channel);


extern void MTIM_disableChannel(const struct MTIM_Channel *channel);



/** Function for enabling channel event by setting control register bits.
 *  \note This function is used internally by MTIM_enableChannelEvent function.
 *  \param channel: pointer to MTIM_Channel instance
 */
extern void MTIM_enableChannelEventReg(const struct MTIM_Channel *channel);


/** Function for disabling channel event by clearing control register bits.
 *  \note This function is used internally by MTIM_disableChannelEvent function.
 *  \param channel: pointer to MTIM_Channel instance
 */
extern void MTIM_disableChannelEventReg(const struct MTIM_Channel *channel);


/** Function for retrieving status mask for channel event.
 *  \param channel: pointer to MTIM_Timer instance
 *  \return status mask for channel event
 */
extern uint32_t MTIM_getChannelStatusMaskForEvent(const struct MTIM_Timer *timer, uint8_t channelNum);


/** Function for retrieving channel number for channel instance.
 *  \param channel: pointer to MTIM_Channel instance
 *  \return channel number
 */
extern uint8_t MTIM_getChannelNum(const struct MTIM_Channel *channel);


/** Function for reading channel value register.
 *  \note This function is used internally by MTIM_readChannelValue function.
 *  \param channel: pointer to MTIM_Channel instance
 *  \return current channel value
 */
extern uint32_t MTIM_readChannelValueReg(const struct MTIM_Channel *channel);


/** Function for writing channel count register.
 *  \note This function is used internally by MTIM_writeChannelValue function.
 *  \param channel: pointer to MTIM_Channel instance
 *  \param value: new channel value to be written
 */
extern void MTIM_writeChannelValueReg(const struct MTIM_Channel *channel, uint32_t value);


/** Function for retrieving MDIO pin instance for specific timer channel.
 *  \param channel: pointer to MTIM_Channel instance
 *  \return MDIO_channel instance
 */
extern const struct MDIO_Channel *MTIM_getChannelPin(const struct MTIM_Channel *channel);


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS FOR INTERRUPTS                               */
/******************************************************************************/

/** Function for enabling interrupt for selected peripheral unit.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MTIM_Timer instance
 */
extern void MTIM_IRQ_enable(const struct MTIM_Timer *timer);


/** Function for handling of ISR for one or more peripheral units.
 *  \note This function is not intended to be used by application
 *  \param index: IRQ index representing for which units the interrupt is pending
 */
extern void MTIM_IRQ_Handler(enum MTIM_IRQ_Index index);


/** Function for querying of interrupt index for given channel instance.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MTIM_Timer instance
 */
extern enum MTIM_IRQ_Index MTIM_IRQ_getIndex(const struct MTIM_Timer *timer);


#ifdef __cplusplus
}
#endif

#endif // MTIM_PRIV_H
