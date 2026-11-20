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


#ifndef MDMA_H
#define MDMA_H

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
 *  \brief    Public definitions and declarations for unit MDMA.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "std_lib/std_callback.h"
#include "mdma_types.h"
#include "mdma_priv.h"
#include "mdma_mc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/** Main MDMA handling structure.
 *  \note This structure is used for storing run-time data used for callback
 *        handling and scan of multiple channels. All instances of this
 *        structure must be allocated in static memory.
 */
struct MDMA_Handle
{
    /** Pointer to MDMA peripheral instance */
    const struct MDMA_Periph    *periph;

    /** Pointer to interrupted channel */
    const struct MDMA_Channel   *channel;

    /** Pointer to first instance of callback list */
    struct STDCB_Callback       *channelCallback;
    
    /** Storage object for operational status of MDMA */
    volatile enum MCAL_Status   status;

    /** pointer to next handle */
    struct MDMA_Handle          *nextHandle;
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MDMA.
 *  \note This function initializes MDMA handle structure and peripheral HW.
 *  \param handle: pointer to MDMA_Handle instance, stored in static memory
 *  \param periph: pointer to MDMA_Periph instance
 *  \param channel: pointer to MDMA_Channel instance
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MDMA_init(struct MDMA_Handle *handle, const struct MDMA_Periph *periph, const struct MDMA_Channel *channel);


/** Initialization function for MDMA channel.
 *  \note This function initializes MDMA handle structure and peripheral HW.
 *  \param handle: pointer to MDMA_Handle instance, stored in static memory
 *  \param cfg: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MDMA_initChannel(const struct MDMA_Handle *handle, const struct MDMA_ChannelConfig *cfg);


/** Function for enabling MDMA.
 *  \brief Calling this function will enable MDMA peripheral e.g. in case of
 *         returning from low power mode.
 *  \param handle: pointer to MDMA_Handle instance
 */
extern void MDMA_enable(struct MDMA_Handle *handle);


/** Function for disabling of MDMA.
 *  \brief Calling this function will disable MDMA peripheral e.g. for lowering
 *         of consumption. The peripheral configuration will be retained.
 *  \param handle: pointer to MDMA_Handle instance
 */
extern void MDMA_disable(struct MDMA_Handle *handle);


/** This function will return actual status of MDMA
 *  \param handle: pointer to MDMA_Handle instance
 *  \return actual status of MDMA
 */
extern enum MCAL_Status MDMA_getStatus(const struct MDMA_Handle *handle);


/** Function for clearing errors.
 *  \brief This function will clear error state and clear peripheral error flags.
 *  \param handle: pointer to MDMA_Handle instance
 */
extern void MDMA_clearError(struct MDMA_Handle *handle);


/** Function for querying on interrupt index for given peripheral instance.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MDMA_Periph instance
 *  \return IRQ index
 */
extern enum MDMA_IRQ_Index MDMA_IRQ_getIndex(const struct MDMA_Periph *periph);


/** Function for handling ISR for one or more peripheral units.
 *  \note This function is not intended to be used by application
 *  \param index: IRQ index representing for which units the interrupt is pending
 */
extern void MDMA_IRQ_Handler(enum MDMA_IRQ_Index index);


/** Function for enabling channel event
 *  \param handle: pointer to MDMA_Handle instance
 *  \param callback: callback instance for which the event will be enabled
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MDMA_enableChannelEvent(struct MDMA_Handle *handle, MCAL_Callback_t *callback);


/** Function for disabling channel event
 *  \param handle: pointer to MDMA_Handle instance
 *  \param callback: callback instance for which the event will be disabled
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MDMA_disableChannelEvent(const struct MDMA_Handle *handle, MCAL_Callback_t *callback);


/** Function for start of non-blocking read of one sample.
 *  Result of conversion will be returned via callback.
 *  \param handle: pointer to MDMA_Handle instance
 *  \param channel: pointer to MDMA_Channel instance
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MDMA_startEventConversion(struct MDMA_Handle *handle, const struct MDMA_Channel *channel);


/** Function for enabling of DMA channel.
 *  \param handle: pointer to MDMA_Handle instance
 */
extern void MDMA_enableChannel(const struct MDMA_Handle *handle);


/** Function for disabling of DMA channel.
 *  \param handle: pointer to MDMA_Handle instance
 */
extern void MDMA_disableChannel(const struct MDMA_Handle *handle);


#ifdef __cplusplus
}
#endif


#endif // MDMA_H
