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
 *  COMP_ABBREV      MDMA
 ******************************************************************************/


#ifndef MDMA_PRIV_H
#define MDMA_PRIV_H

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
 *  \brief    Private definitions and declarations for unit MDMA.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "mcal.h"
#include "mdma_mc.h"
#include "mdma_types.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Function for configuring peripheral.
 *  \note This function is used internally by MDMA_initPeriph function.
 *  \param periph: pointer to struct instance
 *  \return MCAL return code
 */
enum MCAL_Error MDMA_configure(const struct MDMA_Periph *periph);


/** Function for enabling peripheral by setting control register bits.
 *  \note This function is used internally by MDMA_enable function.
 *  \param periph: pointer to struct instance
 */
extern void MDMA_enablePeriph(const struct MDMA_Periph *periph);


/** Function for disabling peripheral by clearing control register bits.
 *  \note This function is used internally by MDMA_disable function.
 *  \param periph: pointer to struct instance
 */
extern void MDMA_disablePeriph(const struct MDMA_Periph *periph);


/** Function for enabling interrupt for selected peripheral unit.
 *  \note This function is not intended to be used by application.
 *  \param periph: pointer to MDMA_Periph instance
 */
extern void MDMA_IRQ_enable(const struct MDMA_Periph *periph);


/** Function reading status register of DMA unit and all channels.
 *  \note In case status flags for DMA unit and its channels are dsistributed
 *        across multiple registers, this function combines these registers
 *        into one 32-bit value.
 *  \param periph: pointer to struct instance
 */
extern uint32_t MDMA_readStatusReg(const struct MDMA_Periph *periph);


/** Function for clearing interrupt channel event flags.
 *  \param periph: pointer to struct instance
 *  \param eventChannel: pointer to struct instance
 */
extern void MDMA_clearChannelEvent(const struct MDMA_Periph *periph, const struct MDMA_Channel *eventChannel);


/** Function for configuring channel.
 *  \param channel: pointer to channel instance
 *  \param cfg: channel configuration structure
 *  \return MCAL return code
 */
extern enum MCAL_Error MDMA_configureChannel(const struct MDMA_Channel *channel, const struct MDMA_ChannelConfig *cfg);


/** Function for repeated configuration of channel parameters that only change after send/receive.
 *  \param channel: pointer to channel instance
 *  \param cfg: channel configuration structure
 *  \return MCAL return code
 */
extern enum MCAL_Error MDMA_configureChannelVolatileParams(const struct MDMA_Channel *channel, const struct MDMA_ChannelConfig *cfg);

/** Function for enabling channel register.
 *  \param channel: pointer to channel instance
 */
extern void MDMA_enableChannelReg(const struct MDMA_Channel *channel);


/** Function for disabling channel register.
 *  \param channel: pointer to channel instance
 */
extern void MDMA_disableChannelReg(const struct MDMA_Channel *channel);


/** Function for enabling channel event register.
 *  \param channel: pointer to channel instance
 */
extern void MDMA_enableChannelEventReg(const struct MDMA_Channel *channel);


/** Function for disabling channel event register.
 *  \param channel: pointer to channel instance
 */
extern void MDMA_disableChannelEventReg(const struct MDMA_Channel *channel);


/** Function for retrieving status mask for channel event.
 *  \param periph: pointer to MDMA_Channel instance
 *  \param channelNum: channel number
 *  \return status mask for channel event
 */
extern uint32_t MDMA_getChannelStatusMaskForEvent(const struct MDMA_Periph *periph, uint8_t channelNum);


/** Function for getting channel number.
 *  \param channel: pointer to channel instance
 *  \return channel number
 */
extern uint8_t MDMA_getChannelNum(const struct MDMA_Channel *channel);



#ifdef __cplusplus
}
#endif

#endif // MDMA_PRIV_H
