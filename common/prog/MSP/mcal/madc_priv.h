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
 *  COMP_ABBREV      MADC
 ******************************************************************************/


#ifndef MADC_PRIV_H
#define MADC_PRIV_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Private definitions and declarations for unit MADC.
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
#include "madc_mc.h"
#include "madc_types.h"


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
 *  \note This function is used internally by MADC_init function.
 *  \param periph: pointer to MADC_Periph instance
 *  \param config: pointer to configuration structure
 */
enum MCAL_Error MADC_configure(const struct MADC_Periph *periph, const struct MADC_Config *config);


/** Function for enabling peripheral by setting control register bits.
 *  \note This function is used internally by MADC_enable function.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_enablePeriph(const struct MADC_Periph *periph);


/** Function for disabling peripheral by clearing control register bits.
 *  \note This function is used internally by MADC_disable function.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_disablePeriph(const struct MADC_Periph *periph);


/** Function for enabling interrupt for selected peripheral unit.
 *  \note This function is not intended to be used by application.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_IRQ_enable(const struct MADC_Periph *periph);


/** Function for reading ADC conversion result from particular ADC result register.
 *  \note This function is used internally by MADC_readConversionResult.
 *  \param periph: pointer to MADC_Periph instance
 *  \param index: index of ADC result register
 */
extern uint16_t MADC_readResultReg(const struct MADC_Periph *periph, uint8_t index);


/** Function for determining if MADC_Periph has ongoing ADC channel conversion.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern bool MADC_isPeriphEvent(const struct MADC_Periph *periph);


/** Function for stoping ongoing ADC conversion on specified MADC_Periph.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_stopPeriphConversion(const struct MADC_Periph *periph); 


/** Function for enabling interrupt generation on completion of ADC channel conversion.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_enablePeriphEvent(const struct MADC_Periph *periph);


/** Function for disabling interrupt generation on completion of ADC channel conversion.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_disablePeriphEvent(const struct MADC_Periph *periph);


/** Function for getting channel number of specified MADC_Channel.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern uint8_t MADC_getChannelNum(const struct MADC_Channel *channel);


/** Function for starting ADC conversion.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern enum MCAL_Error MADC_startPeriphScanConversion(const struct MADC_Periph *periph, const struct MADC_Channel *const channels[], uint8_t channelCount);


/** Function for enabling interrupt generation on completion of ADC scan.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_enableScanEvent(const struct MADC_Periph *periph);


/** Function for disabling interrupt generation on completion of ADC scan.
 *  \note This function is used internally. 
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_disableScanEvent(const struct MADC_Periph *periph);


/** Function for clearing scan event interrupt flag.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_clearScanEventFlag(const struct MADC_Periph *periph);


/** Function for clearing channel conversion event interrupt flag.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern void MADC_clearConversionEventFlag(const struct MADC_Periph *periph);


/** Function for determining if MADC_Periph has ongoing ADC scan.
 *  \note This function is used internally.
 *  \param periph: pointer to MADC_Periph instance
 */
extern bool MADC_isScanEvent(const struct MADC_Periph *periph);

/** Function for querying of interrupt index for given channel instance.
 *  \note This function is used internally.
 *  \param channel: pointer to MADC_Channel instance
 */
extern enum MADC_IRQ_Index MADC_IRQ_getIndex(const struct MADC_Periph *periph);


#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif

#endif // MADC_PRIV_H
