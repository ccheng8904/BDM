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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdac.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MDAC
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mdac.h"

#ifdef MCAL_MDAC_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static bool MDAC_isInitialized(const struct MDAC_Handle *handle);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MDAC_init(struct MDAC_Handle *handle, const struct MDAC_Periph *periph, const struct MDAC_Channel *channel, const struct MDAC_Config *cfg)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(periph != NULL);
    MCAL_assert(channel != NULL);
    
    MCAL_RETURN_ON_ERROR(MDAC_configure(periph, cfg))

    handle->periph = periph;
    /* Remember channel */
    handle->channel = channel;
    
    handle->status = MCAL_STATUS_DISABLED;
    MDAC_enable(handle);
    
    return MCAL_OK;
}


void MDAC_enable(struct MDAC_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if (MDAC_isInitialized(handle))
    {
        MDAC_enablePeriph(handle->periph);
        handle->status = MCAL_STATUS_READY;
    }
}


void MDAC_disable(struct MDAC_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if (!MDAC_isInitialized(handle)) {
        MDAC_disablePeriph(handle->periph);
        handle->status = MCAL_STATUS_DISABLED;
    }
}


enum MCAL_Error MDAC_initChannel(const struct MDAC_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if (!MDAC_isInitialized(handle))
    {
        return MCAL_ERROR_NOT_INITIALIZED;
    }
    
    return MDAC_configureChannel(handle->periph, handle->channel);
}


void MDAC_enableChannel(const struct MDAC_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if (MDAC_isInitialized(handle))
    {
        MDAC_enableChannelReg(handle->channel);
    }
}


void MDAC_disableChannel(const struct MDAC_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if (MDAC_isInitialized(handle))
    {
        MDAC_disableChannelReg(handle->channel);
    }
}


enum MCAL_Error MDAC_writeChannel(const struct MDAC_Handle *handle, const uint16_t data)
{
    MCAL_assert(handle != NULL);
    if (MDAC_getStatus(handle) == MCAL_STATUS_READY)
    {
        return MDAC_setDataReg(handle->channel, data);
    }

    return MCAL_ERROR_NOT_READY;
}


enum MCAL_Status MDAC_getStatus(const struct MDAC_Handle *handle)
{
    return handle->status;
}


void MDAC_softwareTrigger(const struct MDAC_Handle *handle)
{
    MDAC_setSoftwareTrigger(handle->channel);
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static bool MDAC_isInitialized(const struct MDAC_Handle *handle)
{
    return (handle->status != MCAL_STATUS_RESET) ? true : false;
}

/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/

