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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MUART
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "muart.h"
#include "mdio.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif /* MCAL_MPCM_INCLUDED */

#ifdef MCAL_MUART_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** Function for registering handle instance to IQR handle table.
 *  \param handle: pointer to MUART_Handle instance, stored in static memory
 */
static void MUART_IRQ_registerHandle(struct MUART_Handle *handle);


/** Function handling write event inside interrupt.
 *  \param handle: pointer to MUART_Handle instance, stored in static memory
 *  \return MCAL_EVENT_WRITE_COMPLETE_MASK if write is complete
 */
static void MUART_handleWriteEvent(struct MUART_Handle *handle);


/** Function handling read event inside interrupt.
 *  \param handle: pointer to MUART_Handle instance, stored in static memory
 *  \return MCAL_EVENT_READ_COMPLETE_MASK if read is complete
 */
static void MUART_handleReadEvent(struct MUART_Handle *handle, uint8_t data);


/** Function for checking if event is pending.
 *  \note Pending event is, when event is enabled using MXXX_enableEvent and
 *        respective status flag in peripheral status register is set.
 *  \param handle: pointer to MUART_Handle instance, stored in static memory
 *  \param event: event that is checked
 *  \param status: status register value
 *  \return true if event is pending
 */
static bool MUART_isEventPending(const struct MUART_Handle *handle, enum MCAL_Event event, uint32_t status);


#ifdef MCAL_MPCM_INCLUDED
/** Callback function from MPCM module.
 *  \param obj: pointer to MUART_Handle instance
 *  \param flags: pending MPCM_STATUS_XXX event flags
 *  \param eventResponse: response from event sender
 */
/*lint -e818 Pointer parameter 'obj' could be declared as pointing to const */
static void MUART_MPCM_CallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    const struct MUART_Handle *handle = MUART_getHandle(obj);
    enum MCAL_Status status = MUART_getStatus(handle);
    
    (void)flags;

    if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_REQ))
    {
        /* if MUART is busy, abort request to change pmode */
        if(status == MCAL_STATUS_BUSY)
        {
            MPCM_abortPowerModeChange();
        }
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_PRE))
    {
        /* if MUART is enabled, disable it until pmode is changed */
        if(status != MCAL_STATUS_DISABLED)
        {
            MUART_disableChannel(handle->channel);
        }
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_POST))
    {
        /* configure and reenable MUART if not disabled by user */
        if(status != MCAL_STATUS_DISABLED)
        {
            MUART_configureBaudrate(handle->channel, handle->defaultBaudrate);
            MUART_enableChannel(handle->channel);
        }
    }
    else
    {
        /* Nothing to do */
    }
}
#endif /* MCAL_MPCM_INCLUDED */


static bool MUART_isEnabled(const struct MUART_Handle *handle)
{
    return (handle->status == MCAL_STATUS_READY) ? true : false;
}

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


static struct MUART_Handle *MUART_IRQ_HandleTable[MUART_IRQ_CNT];


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MUART_init(struct MUART_Handle *handle, const struct MUART_Channel *channel, const struct MUART_Config *cfg)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(channel != NULL);
    MCAL_assert(cfg != NULL);
    
    MCAL_RETURN_ON_ERROR(MUART_configure(channel, cfg));
    MCAL_RETURN_ON_ERROR(MUART_configurePins(channel->rxPin, channel->txPin));

    handle->channel = channel;

    MUART_IRQ_enable(handle->channel);
    MUART_IRQ_registerHandle(handle);

    handle->defaultBaudrate = cfg->baudrate;
    handle->cbList = NULL;
    handle->status = MCAL_STATUS_DISABLED;
    handle->error = MCAL_OK;
    
  #ifdef MCAL_MPCM_INCLUDED
    MCAL_initCallback(&handle->mpcmCallback, MUART_MPCM_CallbackFunction, handle);
    MCAL_RETURN_ON_ERROR(MPCM_enableEvent(&handle->mpcmCallback, MCAL_EVENT_CLOCK_CHANGE_ANY));
  #endif /* MCAL_MPCM_INCLUDED */

    return MCAL_OK;
}


enum MCAL_Status MUART_getStatus(const struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);

    enum MCAL_Status status = handle->status;
    
    if(status == MCAL_STATUS_READY)
    {
        if(handle->error != MCAL_OK)
        {
            status = MCAL_STATUS_ERROR;
        }
        else if(MCOM_isBusy(&handle->transfer))
        {
            status = MCAL_STATUS_BUSY;
        }
        else
        {
            /* Nothing to do */
        }
    }
    
    return status;
}


enum MCAL_Status MUART_getReadStatus(const struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);

    enum MCAL_Status status = handle->status;
    
    if(status == MCAL_STATUS_READY)
    {
        if(handle->error != MCAL_OK)
        {
            status = MCAL_STATUS_ERROR;
        }
        else if(MCOM_isReadBusy(&handle->transfer))
        {
            status = MCAL_STATUS_BUSY;
        }
        else
        {
            /* Nothing to do */
        }
    }
    
    return status;
}

enum MCAL_Status MUART_getWriteStatus(const struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);

    enum MCAL_Status status = handle->status;
    
    if(status == MCAL_STATUS_READY)
    {
        if(MCOM_isWriteBusy(&handle->transfer))
        {
            status = MCAL_STATUS_BUSY;
        }
    }
    
    return status;
}


enum MCAL_Error MUART_getError(const struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);
    return handle->error;
}


void MUART_clearError(struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);

    handle->error = MCAL_OK;
}


void MUART_enable(struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);

    MUART_enableChannel(handle->channel);
    handle->status = MCAL_STATUS_READY;
}


void MUART_disable(struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);

    MUART_disableChannel(handle->channel);
    handle->status = MCAL_STATUS_DISABLED;
}


enum MCAL_Error MUART_read(struct MUART_Handle *handle, uint8_t buffer[], uint16_t length)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(buffer != NULL);
    
    MUART_clearError(handle);
    
    MCAL_RETURN_ON_ERROR(MCAL_checkReady(handle->status));
    
    if(MCOM_isReadReady(&handle->transfer))
    {
        MCOM_setReadBuffer(&handle->transfer, buffer, length);
        MUART_IRQ_enableEvent(handle->channel, MCAL_EVENT_READ_BYTE);
        
        return MCAL_OK;
    }
    else
    {
        return MCAL_ERROR_BUSY;
    }
}


enum MCAL_Error MUART_write(struct MUART_Handle *handle, const uint8_t buffer[], uint16_t length)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(buffer != NULL);
    
    MUART_clearError(handle);
    
    MCAL_RETURN_ON_ERROR(MCAL_checkReady(handle->status));
    
    if(MCOM_isWriteReady(&handle->transfer))
    {
        MCOM_setWriteBuffer(&handle->transfer, buffer, length);
        (void)MUART_enableEvent(handle, NULL, MCAL_EVENT_WRITE_BYTE);
        return MCAL_OK;
    }
    else
    {
        return MCAL_ERROR_BUSY;
    }
}


void MUART_finishRead(struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);
    
    if(MCOM_isReadBusy(&handle->transfer))
    {
        handle->finishRead = true;
    }
}


int32_t MUART_writeByte(const struct MUART_Handle *handle, uint8_t data)
{
    MCAL_assert(handle != NULL);

    if(MUART_isEnabled(handle))
    {
        if(MUART_isEvent(handle->channel, MCAL_EVENT_WRITE_BYTE))
        {
            MUART_writeDataReg(handle->channel, (uint32_t)data);
            return 1;
        }
    }
    
    return -1;
}


int32_t MUART_readByte(const struct MUART_Handle *handle)
{
    MCAL_assert(handle != NULL);

    if(MUART_isEnabled(handle))
    {
        if(MUART_isEvent(handle->channel, MCAL_EVENT_READ_BYTE))
        {
            return (int32_t)MUART_readDataReg(handle->channel);
        }
    }

    return -1;
}


enum MCAL_Error MUART_writeBlocking(const struct MUART_Handle *handle, const uint8_t *buffer, uint16_t length)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(buffer != NULL);
    
    MCAL_RETURN_ON_ERROR(MCAL_checkReady(handle->status));
    
    if(MCOM_isWriteReady(&handle->transfer))
    {   /*Axivion Next Line MisraC2012-14.2 : Loop counter not modified by intention in third clause of loop */ 
        for (uint8_t n = 0; n < length;)
        {
            if(MUART_writeByte(handle, buffer[n]) > 0)
            {
                n++;
            }
        }
        return MCAL_OK;
    }
    else
    {
        return MCAL_ERROR_BUSY;
    }
}


enum MCAL_Error MUART_enableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
{
    uint32_t eventMask;
    
    MCAL_assert(handle != NULL);
    
    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));
    
    if(callback != NULL)
    {
        MCAL_enableEvent(&handle->cbList, callback, event);
    }
    eventMask = MUART_getStatusRegMaskForEvent(handle->channel, event);
    handle->enabledEvents |= eventMask;
    
    MUART_IRQ_enableEvent(handle->channel, event);
    
    return MCAL_OK;
}


enum MCAL_Error MUART_disableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
{
    uint32_t eventMask;
    
    MCAL_assert(handle != NULL);
    
    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));

    MUART_IRQ_disableEvent(handle->channel, event);
    
    eventMask = MUART_getStatusRegMaskForEvent(handle->channel, event);
    handle->enabledEvents &= ~eventMask;
    
    if(callback != NULL)
    {
        MCAL_disableEvent(callback, event);
    }
    return MCAL_OK;
}


struct MUART_Handle* MUART_getHandle(const void *handlePtr)
{
    MCAL_assert(handlePtr != NULL);
    
  #ifndef NDEBUG
    struct MUART_Handle *handle;
    for(uint8_t n = 0; n < (uint8_t)MUART_IRQ_CNT; n++)
    {
        handle = MUART_IRQ_HandleTable[n];
        
        while(handle != NULL)
        {
            if((void *)handle == handlePtr)
            {
                return handle;
            }
          #ifdef MUART_MULTIPLE_HANDLES_PER_IRQ
            handle = handle->nextHandle;
          #else
            break;
          #endif
        }
    }
    return NULL;
  #else
    /* Note 926:  Conversion between object pointer and void pointer
       Note 9005: Cast removes const qualification
       Uncritical in this case */
    return (struct MUART_Handle*)handlePtr;/*lint !e9005 !e926 */
  #endif
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

bool MUART_isEvent(const struct MUART_Channel *channel, enum MCAL_Event event)
{
    uint32_t status = MUART_getStatusReg(channel);
    uint32_t eventStatusMask = MUART_getStatusRegMaskForEvent(channel, event);
    
    return (((status & eventStatusMask) != 0U) ? true : false);
}


static void MUART_IRQ_registerHandle(struct MUART_Handle *handle)
{
    enum MUART_IRQ_Index index = MUART_IRQ_getIndex(handle->channel);
    /* Note 661 MUART_IRQ_getIndex never returns out of bounds index*/
    if(MUART_IRQ_HandleTable[index] == NULL)/*lint !e661 */
    {
        MUART_IRQ_HandleTable[index] = handle;/*lint !e661 */
    }
  #ifdef MUART_MULTIPLE_HANDLES_PER_IRQ
    else
    {
        struct MUART_Handle *handleEntry = MUART_IRQ_HandleTable[index];
        while(handleEntry->nextHandle != NULL)
        {
            handleEntry = handleEntry->nextHandle;
        }
        handleEntry->nextHandle = handle;
    }
    handle->nextHandle = NULL;
  #endif
}


enum MCAL_Error MUART_configurePins(const struct MDIO_Channel *rxPin, const struct MDIO_Channel *txPin)
{
    struct MDIO_Config cfg = MDIO_CFG_ALTERNATE_FUNCTION;
    
    cfg.outputState = MDIO_OUTPUT_STATE_HIGH;
    
    if(txPin != NULL)
    {
        MCAL_RETURN_ON_ERROR(MDIO_init(txPin, &cfg));
    }
    
    cfg.pullType = MDIO_PULL_UP;
    
    if(rxPin != NULL)
    {
        MCAL_RETURN_ON_ERROR(MDIO_init(rxPin, &cfg));
    }
    
    return MCAL_OK;
}


static void MUART_handleReadEvent(struct MUART_Handle *handle, uint8_t data)
{
    
    if(MCOM_isReadBusy(&handle->transfer))
    {
        MCOM_pushByte(&handle->transfer, data);
    }
    if(MCOM_isReadComplete(&handle->transfer) || handle->finishRead)
    {
        struct MCAL_EventData eventData;
        uint16_t readDataLen = MCOM_getNumOfReadData(&handle->transfer);

        handle->finishRead = false;
        MCAL_setEventData_uint16(&eventData, readDataLen);
        MCOM_setReadReady(&handle->transfer);
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MUART, MCAL_EVENT_READ_COMPLETE, &eventData);
    }
}


static void MUART_handleWriteEvent(struct MUART_Handle *handle)
{
    if(MCOM_isWriteBusy(&handle->transfer))
    {
        uint8_t data = MCOM_popByte(&handle->transfer);
        MUART_writeDataReg(handle->channel, (uint32_t)data);
        
        if(MCOM_isWriteComplete(&handle->transfer))
        {
            (void)MUART_disableEvent(handle, NULL, MCAL_EVENT_WRITE_BYTE);
            MCOM_setWriteReady(&handle->transfer);
            MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MUART, MCAL_EVENT_WRITE_COMPLETE, NULL);
        }
    }
}


static void MUART_handleErrorEvent(struct MUART_Handle *handle)
{
    struct MCAL_EventData eventData;
    enum MCAL_Error error = MUART_getChannelError(handle->channel);

    MUART_clearErrorReg(handle->channel);
    
    if(MCOM_isReadBusy(&handle->transfer))
    {
        handle->error = error;
        MCOM_setReadReady(&handle->transfer);
    }

    MCAL_setEventData_error(&eventData, error);
    MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MUART, MCAL_EVENT_ERROR, &eventData);
}


static bool MUART_isEventPending(const struct MUART_Handle *handle, enum MCAL_Event event, uint32_t status)
{
    uint32_t eventStatusMask = MUART_getStatusRegMaskForEvent(handle->channel, event);

    return ((status & eventStatusMask) != 0u) ? true : false;
}


/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/

void MUART_IRQ_Handler(enum MUART_IRQ_Index index)
{
    struct MUART_Handle *handle = NULL;
    if(index < MUART_IRQ_CNT)
    {
        handle = MUART_IRQ_HandleTable[index];
    }
    
    while(handle != NULL)
    {
        uint32_t status = MUART_getStatusReg(handle->channel);
        status &= handle->enabledEvents;
        
        if(MUART_isEventPending(handle, MCAL_EVENT_ERROR, status))
        {
            MUART_handleErrorEvent(handle);
        }
        else if(MUART_isEventPending(handle, MCAL_EVENT_READ_BYTE, status))
        {
            struct MCAL_EventData eventData;
            uint8_t data = (uint8_t)MUART_readDataReg(handle->channel);

            MCAL_setEventData_uint8(&eventData, data);
            MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MUART, MCAL_EVENT_READ_BYTE, &eventData);
            MUART_handleReadEvent(handle, data);
        }
        else
        {
            /* Nothing to do */
        }
        
        if(MUART_isEventPending(handle, MCAL_EVENT_WRITE_BYTE, status))
        {
            MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MUART, MCAL_EVENT_WRITE_BYTE, NULL);
            MUART_handleWriteEvent(handle);
        }

        
        // Get next handle if multiple channels share the same IRQ
      #ifdef MUART_MULTIPLE_HANDLES_PER_IRQ
        handle = handle->nextHandle;
      #else
        break;
      #endif
    }
}
