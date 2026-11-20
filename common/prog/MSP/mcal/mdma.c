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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MDMA
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mdma.h"

#ifdef MCAL_MDMA_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** Function for registering handle instance to IQR handle table.
 *  \param handle: pointer to MDMA_Handle instance, stored in static memory
 */
static void MDMA_IRQ_registerHandle(struct MDMA_Handle *handle);
static bool MDMA_isInitialized(const struct MDMA_Handle *handle);
static bool MDMA_isChannelEventPending(const struct MDMA_Handle *handle, uint8_t channelNum, uint32_t status);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** Array for storing of MDMA handlers for purpose of interrupt handling */
static struct MDMA_Handle *MDMA_IRQ_HandleTable[MDMA_IRQ_CNT];


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MDMA_init(struct MDMA_Handle *handle, const struct MDMA_Periph *periph, const struct MDMA_Channel *channel)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(periph != NULL);
    MCAL_assert(channel != NULL);
    
    MCAL_RETURN_ON_ERROR(MDMA_configure(periph))

    handle->periph = periph;
    handle->channelCallback = NULL;
    
    /*Remember event channel*/
    handle->channel = channel;

    MDMA_IRQ_registerHandle(handle);
    MDMA_IRQ_enable(periph);

    handle->status = MCAL_STATUS_DISABLED;
    MDMA_enable(handle);
    
    return MCAL_OK;
}


enum MCAL_Error MDMA_initChannel(const struct MDMA_Handle *handle, const struct MDMA_ChannelConfig *cfg)
{
    MCAL_assert(handle != NULL);

    if(!MDMA_isInitialized(handle))
    {
        return MCAL_ERROR_NOT_INITIALIZED;
    }

    return MDMA_configureChannel(handle->channel, cfg);
}


enum MCAL_Status MDMA_getStatus(const struct MDMA_Handle *handle)
{
    MCAL_assert(handle != NULL);
    return handle->status;
}


void MDMA_enable(struct MDMA_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if(MDMA_isInitialized(handle))
    {
        MDMA_enablePeriph(handle->periph);
        handle->status = MCAL_STATUS_READY;
    }
}


void MDMA_disable(struct MDMA_Handle *handle)
{
    MCAL_assert(handle != NULL);
    MDMA_disablePeriph(handle->periph);
    handle->status = MCAL_STATUS_DISABLED;
}


void MDMA_enableChannel(const struct MDMA_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if(MDMA_isInitialized(handle))
    {
        MDMA_enableChannelReg(handle->channel);
    }
}


void MDMA_disableChannel(const struct MDMA_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if(MDMA_isInitialized(handle))
    {
        MDMA_disableChannelReg(handle->channel);
    }
}


enum MCAL_Error MDMA_enableChannelEvent(struct MDMA_Handle *handle, MCAL_Callback_t *callback)
{
    uint8_t channelNum;
    MCAL_assert(handle != NULL);
    
    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));
    
    channelNum = MDMA_getChannelNum(handle->channel);
    MCAL_enableChannelEvent(&handle->channelCallback, callback, channelNum);
    handle->status = MCAL_STATUS_BUSY;
    MDMA_enableChannelEventReg(handle->channel);
    return MCAL_OK;
}


enum MCAL_Error MDMA_disableChannelEvent(const struct MDMA_Handle *handle, MCAL_Callback_t *callback)
{
    uint8_t channelNum;
    MCAL_assert(handle != NULL);
    
    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));
    
    channelNum = MDMA_getChannelNum(handle->channel);
    MDMA_disableChannelEventReg(handle->channel);
    MCAL_disableChannelEvent(callback, channelNum);
    return MCAL_OK;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static bool MDMA_isInitialized(const struct MDMA_Handle *handle)
{
    return (handle->status != MCAL_STATUS_RESET) ? true : false;
}


static bool MDMA_isChannelEventPending(const struct MDMA_Handle *handle, uint8_t channelNum, uint32_t status)
{
    uint32_t eventStatusMask = MDMA_getChannelStatusMaskForEvent(handle->periph, channelNum);
    return ((status & eventStatusMask) != 0u) ? true : false;
}


static void MDMA_IRQ_registerHandle(struct MDMA_Handle *handle)
{
    uint8_t index = (uint8_t) MDMA_IRQ_getIndex(handle->periph);

    if(MDMA_IRQ_HandleTable[index] == NULL)
    {
        MDMA_IRQ_HandleTable[index] = handle;
    }
    else
    {
        struct MDMA_Handle *handleEntry = MDMA_IRQ_HandleTable[index];
        while(handleEntry->nextHandle != NULL)
        {
            handleEntry = handleEntry->nextHandle;
        }
        handleEntry->nextHandle = handle;
    }
    handle->nextHandle = NULL;
}


/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/

void MDMA_IRQ_Handler(enum MDMA_IRQ_Index index)
{
    struct MDMA_Handle *handle = MDMA_IRQ_HandleTable[index];

    while(handle != NULL)
    {
        if(handle->channel != NULL)
        {   
            uint32_t status = MDMA_readStatusReg(handle->periph);
            uint8_t channelNum = MDMA_getChannelNum(handle->channel);
            
            if(MDMA_isChannelEventPending(handle, channelNum, status))
            {
                MDMA_clearChannelEvent(handle->periph, handle->channel);
                handle->status = MCAL_STATUS_READY;
                MCAL_invokeChannelCallbacks(handle->channelCallback, MCAL_MODULE_MDMA, channelNum, NULL);
            }
        }
        
        // Get next handle
        handle = handle->nextHandle;
    }
}
