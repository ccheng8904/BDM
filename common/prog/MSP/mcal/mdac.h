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


#ifndef MDAC_H
#define MDAC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdac.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MDAC.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include "mdac_types.h"
#include "mdac_priv.h"
#include "mdac_mc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/** Main MDAC handling structure.
 *  \note This structure is used for storing run-time data. All instances of this
 *        structure must be allocated in static memory.
 */
struct MDAC_Handle
{
    /** Pointer to MDAC peripheral instance */
    const struct MDAC_Periph            *periph;

    /** Pointer to channel */
    const struct MDAC_Channel           *channel;
    
    /** Storage object for operational status of peripheral */
    volatile enum MCAL_Status           status;
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MDAC.
 *  \note This function initializes MDAC handle structure and peripheral HW.
 *  \param handle: pointer to MDAC_Handle instance, stored in static memory
 *  \param periph: pointer to MDAC_Periph instance
 *  \param channel: pointer to MDAC_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MDAC_init(struct MDAC_Handle *handle, const struct MDAC_Periph *periph, const struct MDAC_Channel *channel, const struct MDAC_Config *cfg);


/** Function for enabling MDAC.

 *  \brief Calling this function will enable MDAC peripheral e.g. in case of
 *         returning from low power mode.
 *  \param handle: pointer to MDAC_Handle instance
 */
extern void MDAC_enable(struct MDAC_Handle *handle);


/** Function for disabling MDAC.

 *  \brief Calling this function will disable MDAC peripheral e.g. for lowering
 *         of consumption. The peripheral configuration will be retained.
 *  \param handle: pointer to MDAC_Handle instance
 */
extern void MDAC_disable(struct MDAC_Handle *handle);


/** Initialization function for DAC channel.

 *  \note This function initializes MDAC handle structure and peripheral HW.
 *  \param handle: pointer to MDAC_Handle instance, stored in static memory
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MDAC_initChannel(const struct MDAC_Handle *handle);


/** Function for enabling of DAC channel.
 *  \param handle: pointer to MDAC_Handle instance
 */
extern void MDAC_enableChannel(const struct MDAC_Handle *handle);


/** Function for disabling of DAC channel.
 *  \param handle: pointer to MDAC_Handle instance
 */
extern void MDAC_disableChannel(const struct MDAC_Handle *handle);


/** Function for setting data for conversion in channel.
 *  \param handle: pointer to MDAC_Handle instance
 *  \param data: digital value to be set for conversion
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MDAC_writeChannel(const struct MDAC_Handle *handle, const uint16_t data);


/** This function will return actual status of MDAC
 *  \param handle: pointer to MDAC_Handle instance
 *  \return actual status of MDAC. 
 *      MCAL_STATUS_RESET - not initialized; 
 *      MCAL_STATUS_DISABLED - initialized but disabled;
 *      MCAL_STATUS_READY - peripheral is ready for operation;
 */
extern enum MCAL_Status MDAC_getStatus(const struct MDAC_Handle *handle);


/** This function triggers the MDAC conversion in software trigger mode
 *  \param handle: pointer to MDAC_Handle instance
 */
extern void MDAC_softwareTrigger(const struct MDAC_Handle *handle);

#ifdef __cplusplus
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

#endif // __cplusplus

#endif // MDAC_H
