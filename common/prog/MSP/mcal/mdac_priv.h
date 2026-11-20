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
 *  COMP_ABBREV      MDAC
 ******************************************************************************/


#ifndef MDAC_PRIV_H
#define MDAC_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdac_priv.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Private definitions and declarations for unit MDAC.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include "mcal.h"
#include "mdac_mc.h"
#include "mdac_types.h"


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
 *  \note This function is used internally by MDAC_init function.
 *  \param periph: pointer to MDAC_Periph instance
 *  \param cfg: pointer to configuration structure
 */
extern enum MCAL_Error MDAC_configure(const struct MDAC_Periph *periph, const struct MDAC_Config *cfg);


/** Function for enabling peripheral by setting control register bits.
 *  \note This function is used internally by MDAC_enable function.
 *  \param periph: pointer to MDAC_Periph instance
 */
extern void MDAC_enablePeriph(const struct MDAC_Periph *periph);


/** Function for disabling peripheral by clearing control register bits.
 *  \note This function is used internally by MDAC_disable function.
 *  \param periph: pointer to MDAC_Periph instance
 */
extern void MDAC_disablePeriph(const struct MDAC_Periph *periph);


/** Function for enabling channel by setting control register bits.
 *  \note This function is used internally by MDAC_enableChannel function.
 *  \param channel: pointer to MDAC_Channel instance
 */
extern void MDAC_enableChannelReg(const struct MDAC_Channel *channel);


/** Function for disabling channel by clearing control register bits.
 *  \note This function is used internally by MDAC_disableChannel function.
 *  \param channel: pointer to MDAC_Channel instance
 */
extern void MDAC_disableChannelReg(const struct MDAC_Channel *channel);


/** Function for configuring channel.
 *  \note This function is used internally by MDAC_initChannel.
 *  \param periph:  pointer to MDAC_Periph instance
 *  \param channel: pointer to MDAC_Channel instance
 *  \return MCAL return code
 */
extern enum MCAL_Error MDAC_configureChannel(const struct MDAC_Periph *periph, const struct MDAC_Channel *channel);


/** Function for getting channel number of specified MDAC_Channel.
 *  \note This function is used internally.
 *  \param channel: pointer to MDAC_Channel instance
 *  \return number of channel
 */
extern uint8_t MDAC_getChannelNum(const struct MDAC_Channel *channel);


/** Function for setting data register of channel for conversion.
 *  \note This function is used internally by MDAC_setConversionData.
 *  \param channel: pointer to MDAC_Channel instance used in conversion
 *  \param data: digital value to be set for conversion
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MDAC_setDataReg(const struct MDAC_Channel *channel, const uint16_t data);


/** Function for software triggering MDAC channel conversion.
 *  \note This function is used internally by MDAC_softwareTrigger.
 *  \param channel: pointer to MDAC_Channel instance used in conversion
 */
extern void MDAC_setSoftwareTrigger(const struct MDAC_Channel *channel);

#ifdef __cplusplus
}
#endif

#endif // MDAC_PRIV_H
