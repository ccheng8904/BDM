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
 *  COMP_ABBREV      MEXTI
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MEXTI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "mexti.h"

#ifdef MCAL_MEXTI_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** Function for registering handle instance to IQR handle table.
 *  \param handle: pointer to MEXTI_Handle handle instance, stored in static memory
 */
static void MEXTI_IRQ_registerHandle(struct MEXTI_Handle *handle);


/** Function for checking if event is pending.
 *  \note Pending event is, when event is enabled using MXXX_enableEvent and
 *        respective status flag in peripheral status register is set.
 *  \param handle: pointer to MEXTI_Handle instance, stored in static memory
 *  \param status: status register value
 *  \return true if event is pending
 */
static bool MEXTI_isEventPending(const struct MEXTI_Handle *handle, uint32_t status);


/** Function for checking if event is enabled.
 *  \return true if event is enabled
 */
static bool MEXTI_isEventEnabled(const struct MEXTI_Handle *handle)
{
    return handle->enabledEvent;
}


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static struct MEXTI_Handle *MEXTI_IRQ_HandleTable[MEXTI_IRQ_CNT];


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MEXTI_init(struct MEXTI_Handle *handle, const struct MEXTI_Channel *channel, const struct MEXTI_Config *cfg)
{
    MCAL_assert(handle != NULL);
    
    handle->channel = channel;
    
    MEXTI_configureChannel(channel, cfg);
    
    MEXTI_IRQ_enable(handle->channel);
    
    MEXTI_IRQ_registerHandle(handle);
    
    handle->cbList = NULL;
    
    return MCAL_OK;
}


void MEXTI_enableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *callback)
{
    MCAL_assert(handle != NULL);
    
    MCAL_enableEvent(&handle->cbList, callback, MCAL_EVENT_TRIGGER);
    handle->enabledEvent = true; 
    MEXTI_enableEventReg(handle->channel);
}


void MEXTI_disableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *callback)
{
    MCAL_assert(handle != NULL);
    
    MEXTI_disableEventReg(handle->channel);
    MCAL_disableEvent(callback, MCAL_EVENT_TRIGGER);
    handle->enabledEvent = false; 
}


bool MEXTI_isEvent(const struct MEXTI_Channel *channel)
{
    uint32_t status = MEXTI_getStatusReg(channel);
    uint32_t eventStatusMask = MEXTI_getStatusRegMaskForEvent(channel);
    
    return (((status & eventStatusMask) != 0U) ? true : false);
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void MEXTI_IRQ_registerHandle(struct MEXTI_Handle *handle)
{
    enum MEXTI_IRQ_Index index = MEXTI_IRQ_getIndex(handle->channel);
    /* Note 661 MEXTI_IRQ_getIndex never returns out of bounds index*/
    if(MEXTI_IRQ_HandleTable[index] == NULL) /*lint !e661 */
    {
        MEXTI_IRQ_HandleTable[index] = handle;/*lint !e661 */
    }
  #ifdef MEXTI_MULTIPLE_HANDLES_PER_IRQ
    else
    {
        struct MEXTI_Handle *handleEntry = MEXTI_IRQ_HandleTable[index];
        while(handleEntry->nextHandle != NULL)
        {
            handleEntry = handleEntry->nextHandle;
        }
        handleEntry->nextHandle = handle;
    }
    handle->nextHandle = NULL;
  #endif
}


static bool MEXTI_isEventPending(const struct MEXTI_Handle *handle, uint32_t status)
{
    uint32_t eventStatusMask = MEXTI_getStatusRegMaskForEvent(handle->channel);
    
    if(MEXTI_isEventEnabled(handle))
    {
        if((status & eventStatusMask) != 0u )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/

void MEXTI_IRQ_Handler(enum MEXTI_IRQ_Index index)
{
    const struct MEXTI_Handle *handle = MEXTI_IRQ_HandleTable[index];
    
    while(handle != NULL)
    {
        uint32_t status = MEXTI_getStatusReg(handle->channel);
        
        if(MEXTI_isEventPending(handle, status))
        {
            MEXTI_clearEventReg(handle->channel);
            MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MEXTI, MCAL_EVENT_TRIGGER, NULL);
        }
        
        // Get next handle if multiple channels share the same IRQ
      #ifdef MEXTI_MULTIPLE_HANDLES_PER_IRQ
        handle = handle->nextHandle;
      #else
        break;
      #endif
    }
}
