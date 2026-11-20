/*******************************************************************************
 *  Copyright (c) 2020 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MI2C
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MI2C
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
//#include <assert.h>
#include "mi2c.h"
#include "mdio.h"
#include "utility.h"
#include "timer/timerlib.h"
#include "math/mathDiv.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif /* MCAL_MPCM_INCLUDED */

#ifdef MCAL_MI2C_NDEBUG
    #define NDEBUG
#endif
#include "mcal_assert.h"
/*lint -save -e522 there shall be not dead code Rule 2.2 */
/*lint -save -e613 Possible use of null pointer */
/*lint -save -e931 both sides have side effects */
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** Function for registering handle instance to IQR handle table.
 *  \param handle: pointer to MI2C_Handle instance, stored in static memory
 */
static void MI2C_IRQ_registerHandle(struct MI2C_Handle *handle);


/** Function for checking if event is pending.
 *  \note Pending event is, when event is enabled using MXXX_enableEvent and
 *        respective status flag in peripheral status register is set.
 *  \param handle: pointer to MI2C_Handle instance, stored in static memory
 *  \param event: event that is checked
 *  \param status: status register value
 *  \return true if event is pending
 */
static bool MI2C_isEventPending(const struct MI2C_Handle *handle, enum MCAL_Event event, uint32_t status);

static enum MCAL_Error MI2C_init_SWI2C(struct MI2C_Handle *handle, const struct MI2C_Config *cfg);
static void MI2C_enable_SWI2C(struct MI2C_Handle *handle);
static void MI2C_disable_SWI2C(struct MI2C_Handle *handle);
static enum MCAL_Error MI2C_startCommunication_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress);
static enum MCAL_Error MI2C_stopCommunication_SWI2C(struct MI2C_Handle *handle);
static enum MCAL_Error MI2C_writeByte_SWI2C(struct MI2C_Handle *handle, uint8_t dataByte);
static uint8_t MI2C_readByte_SWI2C(struct MI2C_Handle *handle);
static enum MCAL_Error MI2C_write_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t buffer[], uint16_t length);
static enum MCAL_Error MI2C_read_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, uint8_t buffer[], uint16_t length);
static enum MCAL_Error MI2C_transfer_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength);
static enum MCAL_Error MI2C_forceBusRelease_SWI2C(struct MI2C_Handle *handle);
static void MI2C_configureBaudrate_SWI2C(struct MI2C_Handle *handle, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode);
static void MI2C_waitForSlave_SWI2C(struct MI2C_Handle *handle);
static void MI2C_makeStopCondition_SWI2C(struct MI2C_Handle *handle);
static void MI2C_triggerReceive_SWI2C(struct MI2C_Handle *handle, bool last);
static uint8_t MI2C_getRxData_SWI2C(struct MI2C_Handle const *handle);
static void MI2C_bitDelay_SWI2C(uint16_t delay);
static void MI2C_set(const struct MDIO_Channel *channel);
static void MI2C_reset(const struct MDIO_Channel *channel);
static bool MI2C_get(const struct MDIO_Channel *channel);

static enum MCAL_Error MI2C_init_HWI2C(struct MI2C_Handle *handle, const struct MI2C_Config *cfg);
static void MI2C_enable_HWI2C(struct MI2C_Handle *handle);
static void MI2C_disable_HWI2C(struct MI2C_Handle *handle);
static void MI2C_configureBaudrate_HWI2C(struct MI2C_Handle *handle, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode);
static enum MCAL_Error MI2C_startCommunication_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress);
static enum MCAL_Error MI2C_stopCommunication_HWI2C(struct MI2C_Handle *handle);
static enum MCAL_Error MI2C_writeByte_HWI2C(struct MI2C_Handle *handle, uint8_t dataByte);
static uint8_t MI2C_readByte_HWI2C(struct MI2C_Handle *handle);
static enum MCAL_Error MI2C_write_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t buffer[], uint16_t length);
static enum MCAL_Error MI2C_read_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, uint8_t buffer[], uint16_t length);
static enum MCAL_Error MI2C_transfer_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength);
static enum MCAL_Error MI2C_forceBusRelease_HWI2C(struct MI2C_Handle *handle);
static enum MCAL_Error MI2C_handleEvents(struct MI2C_Handle *handle, enum MCAL_Event event);

/** Function configuring sda/scl pins to alternate function.
 *  \param channel: pointer MI2C_Channel instance
 */
static enum MCAL_Error MI2C_configurePins(const struct MI2C_Channel *channel);
static bool MI2C_isInitialized(struct MI2C_Handle *handle);

#ifdef MCAL_MPCM_INCLUDED
/** Callback function from MPCM module.
 *  \param obj: pointer to MI2C_Handle instance
 *  \param flags: pending MPCM_STATUS_XXX event flags
 *  \param data: new power mode
 *  \return -1 if UART is busy during MPCM_STATUS_PMODE_REQ_ALL, otherwise 0
 */
/*lint -e818 Pointer parameter 'obj' could be declared as pointing to const */
static void MI2C_MPCM_CallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    struct MI2C_Handle *handle = MI2C_getHandle(obj);
    enum MCAL_Status status = MI2C_getStatus(handle);
    
    (void)flags;
    
    if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_REQ))
    {
        /* if MI2C is busy, interrupt request to change pmode by returning -1 */
        if(status == MCAL_STATUS_BUSY)
        {
            // Possiblity to interrupt power mode change
        }
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_PRE))
    {
        /* if MI2C is enabled, disable it until pmode is changed */
        if(status != MCAL_STATUS_DISABLED)
        {
            MI2C_disableChannel(handle->channel);
        }
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_POST))
    {
        /* configure and reenable MI2C if not disabled by user */
        if(status != MCAL_STATUS_DISABLED)
        {
            MI2C_configureBaudrate(handle, handle->defaultBaudrate, handle->mode);
            MI2C_enableChannel(handle->channel);
        }
    }
    else
    {
        /* Nothing to do */
    }
}
#endif /* MCAL_MPCM_INCLUDED */


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#define MDIO_CFG_I2C_FUNCTION {MDIO_MODE_ALTERNATE_FUNCTION, MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_OPEN_DRAIN, MDIO_OUTPUT_STATE_NO_CHANGE}
#define SDA_TEST_COUNT        10u
#define LSB                    1u
#define ADDRESS_10_BIT_ID   0x1Eu
#define NO_MATTER_BITS         3u


static struct MI2C_Handle *MI2C_IRQ_HandleTable[MI2C_IRQ_CNT];


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MI2C_init(struct MI2C_Handle *handle, struct MI2C_Channel const *channel, const struct MI2C_Config *cfg)
{
    static const struct MI2C_functions SWI2C_functions = 
    {
        MI2C_init_SWI2C,
        MI2C_enable_SWI2C,
        MI2C_disable_SWI2C,
        MI2C_startCommunication_SWI2C,
        MI2C_stopCommunication_SWI2C,
        MI2C_writeByte_SWI2C,
        MI2C_readByte_SWI2C,
        MI2C_write_SWI2C,
        MI2C_read_SWI2C,
        MI2C_transfer_SWI2C,
        MI2C_forceBusRelease_SWI2C,
        MI2C_configureBaudrate_SWI2C
    };
    static const struct MI2C_functions HWI2C_functions = 
    {
        MI2C_init_HWI2C,
        MI2C_enable_HWI2C,
        MI2C_disable_HWI2C,
        MI2C_startCommunication_HWI2C,
        MI2C_stopCommunication_HWI2C,
        MI2C_writeByte_HWI2C,
        MI2C_readByte_HWI2C,
        MI2C_write_HWI2C,
        MI2C_read_HWI2C,
        MI2C_transfer_HWI2C,
        MI2C_forceBusRelease_HWI2C,
        MI2C_configureBaudrate_HWI2C
    };

    MCAL_assert(handle != (struct MI2C_Handle *)NULL);
    MCAL_assert(channel != (struct MI2C_Channel *)NULL);
    MCAL_assert(cfg != (const struct MI2C_Config *)NULL);

    handle->channel         = channel;
    handle->mode            = cfg->mode;
    handle->defaultBaudrate = cfg->baudrate;
    handle->length          = MI2C_ADDRESS_LENGTH_7_BITS;
    handle->functions       = (cfg->commType == MI2C_TYPE_SW) ? &SWI2C_functions : &HWI2C_functions;
    handle->cbList          = NULL;
    handle->enabledEvents   = 0;
    handle->error           = MCAL_OK;
    handle->baseAddress     = 0;
    handle->lastRx          = 0;
    handle->delays.delayH   = 0;
    handle->delays.delayL   = 0;
    handle->taskState       = MI2C_TS_UNINITIALIZED;
    handle->prevTaskState   = MI2C_TS_UNINITIALIZED;
    handle->transferInProgress = false;
#ifdef MI2C_MULTIPLE_HANDLES_PER_IRQ
    handle->nextHandle      = NULL;
#endif /* MI2C_MULTIPLE_HANDLES_PER_IRQ */

    if(handle->functions->init(handle, cfg) != MCAL_OK)
    {
        return MCAL_ERROR;
    }
    handle->status = MCAL_STATUS_DISABLED;

#ifdef MCAL_MPCM_INCLUDED
    MCAL_initCallback(&handle->mpcmCallback, MI2C_MPCM_CallbackFunction, handle);
    MCAL_RETURN_ON_ERROR(MPCM_enableEvent(&handle->mpcmCallback, MCAL_EVENT_CLOCK_CHANGE_ANY));
#endif /* MCAL_MPCM_INCLUDED */

    return MCAL_OK;
}


enum MCAL_Status MI2C_getStatus(const struct MI2C_Handle *handle)
{
    MCAL_assert(handle != NULL);

    enum MCAL_Status status = handle->status;

    if(status == MCAL_STATUS_READY)
    {
        if((handle->error != MCAL_OK) && (handle->error != MCAL_ERROR_NOT_RESPONDING))
        {
            status = MCAL_STATUS_ERROR;
        }
        else if(MCOM_isBusy(&handle->transfer))
        {
            status = MCAL_STATUS_BUSY;
        }
        else
        {
            /* satisfy Lint */
        }
    }
    return status;
}


enum MCAL_Status MI2C_getReadStatus(const struct MI2C_Handle *handle)
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
            /* satisfy Lint */
        }
    }
    return status;
}


enum MCAL_Status MI2C_getWriteStatus(const struct MI2C_Handle *handle)
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


enum MCAL_Error MI2C_getError(const struct MI2C_Handle *handle)
{
    MCAL_assert(handle != NULL);
    return handle->error;
}


void MI2C_clearError(struct MI2C_Handle *handle)
{
    MCAL_assert(handle != NULL);

    handle->error = MCAL_OK;
}


void MI2C_enable(struct MI2C_Handle *handle)
{
    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if(MI2C_isInitialized(handle) != false)
    {
        handle->functions->enable(handle);
    }
}


void MI2C_disable(struct MI2C_Handle *handle)
{
    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( MI2C_isInitialized(handle) != false )
    {
        handle->functions->disable(handle);
    }
}


void MI2C_vCommunicationFinished(struct MI2C_Handle *handle)
{
    if( (handle->prevTaskState == MI2C_TS_CHIP_ADDRESS_SENT_READ) || (handle->prevTaskState == MI2C_TS_RECEIVE_DATA) )
    {
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MI2C, MCAL_EVENT_READ_COMPLETE, NULL);
    }
    else
    {
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MI2C, MCAL_EVENT_WRITE_COMPLETE, NULL);
    }
    MCOM_setReadReady(&handle->transfer);
    MCOM_setWriteReady(&handle->transfer);
    handle->status = MCAL_STATUS_READY;
}


enum MCAL_Error MI2C_startCommunication(struct MI2C_Handle *handle, uint16_t baseAddress)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( MI2C_getStatus(handle) == MCAL_STATUS_READY )
    {
        handle->status = MCAL_STATUS_BUSY;
        retVal = handle->functions->startCommunication(handle, baseAddress);
    }
    return retVal;
}


enum MCAL_Error MI2C_stopCommunication(struct MI2C_Handle *handle)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( MI2C_isInitialized(handle) != false )
    {
        retVal = handle->functions->stopCommunication(handle);
    }
    return retVal;
}


enum MCAL_Error MI2C_writeByte(struct MI2C_Handle *handle, uint8_t data)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( MI2C_isInitialized(handle) != false )
    {
        retVal = handle->functions->writeByte(handle, data);
    }
    
    return retVal;
}


uint8_t MI2C_readByte(struct MI2C_Handle *handle)
{
    uint8_t retVal = 0;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( MI2C_isInitialized(handle) != false )
    {
        retVal = handle->functions->readByte(handle);
    }
    
    return retVal;
}


enum MCAL_Error MI2C_write(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t buffer[], uint16_t length)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( (MI2C_isInitialized(handle) != false) &&
        (buffer != (const uint8_t *)NULL)     &&
        (length > 0U)
      )
    {
        handle->status = MCAL_STATUS_BUSY;
        baseAddress <<= 1u;  /* add 0 direction bit to address */
        retVal = handle->functions->write(handle, baseAddress, buffer, length);
    }

    return retVal;
}


enum MCAL_Error MI2C_read(struct MI2C_Handle *handle, uint16_t baseAddress, uint8_t buffer[], uint16_t length)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( (MI2C_isInitialized(handle) != false) &&
        (buffer != (uint8_t *)NULL)           &&
        (length > 0U)
        )
    {
        handle->status = MCAL_STATUS_BUSY;
        baseAddress <<= 1u;  /* add 0 direction bit to address */
        retVal = handle->functions->read(handle, baseAddress, buffer, length);
    }
    
    return retVal;
}


enum MCAL_Error MI2C_transfer(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( (MI2C_isInitialized(handle) != false) &&
        (txBuffer != (uint8_t const *)NULL)   &&
        (rxBuffer != (uint8_t *)NULL)         &&
        (txLength > 0U)                       &&
        (rxLength > 0U)
        )
    {
        handle->status = MCAL_STATUS_BUSY;
        baseAddress <<= 1u;  /* add 0 direction bit to address */
        retVal = handle->functions->transfer(handle, baseAddress, txBuffer, txLength, rxBuffer, rxLength);
    }

    return retVal;

}


enum MCAL_Error MI2C_forceBusRelease(struct MI2C_Handle *handle)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( MI2C_isInitialized(handle) != false )
    {
        handle->status = MCAL_STATUS_BUSY;
        retVal = handle->functions->forceBusRelease(handle);
        handle->status = MCAL_STATUS_READY;
    }
    
    return retVal;
}


void MI2C_configureBaudrate(struct MI2C_Handle *handle, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode)
{

    MCAL_assert( (handle != (struct MI2C_Handle *)NULL) && (handle->functions != (struct MI2C_functions *)NULL));

    if( MI2C_isInitialized(handle) != false )
    {
        handle->functions->configureBaudrate(handle, baudRate, mode);
    }
}


void MI2C_enableEvent(struct MI2C_Handle *handle, struct STDCB_Callback *cb, enum MCAL_Event event)
{
    uint32_t eventMask;

    MCAL_assert(handle != (struct MI2C_Handle *)NULL);
    
    if (MCAL_checkInitialized(handle->status) == MCAL_OK)
    {
        if(cb != NULL)
        {
            MCAL_enableEvent(&handle->cbList, cb, event);
        }
        eventMask = MI2C_getStatusRegMaskForEvent(handle->channel, event);
        handle->enabledEvents |= eventMask;

        MI2C_IRQ_enableEvent(handle->channel, event);
    }
}


void MI2C_disableEvent(struct MI2C_Handle *handle, struct STDCB_Callback *cb, enum MCAL_Event event)
{
    uint32_t eventMask;

    MCAL_assert(handle != (struct MI2C_Handle *)NULL);

    if (MCAL_checkInitialized(handle->status) == MCAL_OK)
    {
        if(cb != NULL)
        {
            MCAL_disableEvent(cb, event);
        }
        eventMask = MI2C_getStatusRegMaskForEvent(handle->channel, event);
        handle->enabledEvents &= ~eventMask;

        MI2C_IRQ_disableEvent(handle->channel, event);
    }
}


bool MI2C_isEvent(const struct MI2C_Channel *channel, enum MCAL_Event event)
{
    MCAL_assert(channel != (const struct MI2C_Channel *)NULL);

    uint32_t status = MI2C_getStatusReg(channel);
    uint32_t eventStatusMask = MI2C_getStatusRegMaskForEvent(channel, event);
    
    return (((status & eventStatusMask) != 0U) ? true : false);
}


/*lint -save -e818 Pointer parameter could be declared as pointing to const */
bool MI2C_isError(struct MI2C_Handle *handle, enum MCAL_Error error)
{
    MCAL_assert(handle != (struct MI2C_Handle *)NULL);

    return (handle->error == error);
}
/*lint -restore -e818 */


int32_t MI2C_attachCallback(struct MI2C_Handle *handle, struct STDCB_Callback *cb)
{
    MCAL_assert((handle != (struct MI2C_Handle *)NULL) && (cb != (struct STDCB_Callback *)NULL));
    
    if(handle != NULL)
    {
        return STDCB_AttachCallback(&handle->cbList, cb);
    }
    else
    {
        return -1;
    }
}


struct MI2C_Handle* MI2C_getHandle(const void *handlePtr)
{
    MCAL_assert(handlePtr != NULL);
    
  #ifndef NDEBUG
    struct MI2C_Handle *handle;
    for(uint8_t n = 0; n < (uint8_t)MI2C_IRQ_CNT; n++)
    {
        handle = MI2C_IRQ_HandleTable[n];
        
        while(handle != NULL)
        {
            if((void *)handle == handlePtr)
            {
                return handle;
            }
          #ifdef MI2C_MULTIPLE_HANDLES_PER_IRQ
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
    return (struct MI2C_Handle*)handlePtr;/*lint !e9005 !e926 */
  #endif
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/* functions for software created I2C */
static enum MCAL_Error MI2C_init_SWI2C(struct MI2C_Handle *handle, const struct MI2C_Config *cfg)
{
    const struct MI2C_Channel *channel = handle->channel;

    MI2C_configureBaudrate_SWI2C(handle, cfg->baudrate, cfg->mode);
    
    MI2C_initPort(channel, cfg->intPullUps);

    /* preset for port latch */
    MI2C_input(channel->sclPin);
    MI2C_input(channel->sdaPin);
    handle->taskState = MI2C_TS_READY;
    return MCAL_OK;
}


static void MI2C_enable_SWI2C(struct MI2C_Handle *handle)
{
    handle->status = MCAL_STATUS_READY;
}


static void MI2C_disable_SWI2C(struct MI2C_Handle *handle)
{
    handle->status = MCAL_STATUS_DISABLED;
}


static enum MCAL_Error MI2C_startCommunication_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress)
{
    const struct MI2C_Channel *channel = handle->channel;
    uint8_t addressByte;
    enum MCAL_Error retVal;
    
    /* no errors so far */
    handle->error = MCAL_OK;
    handle->baseAddress = baseAddress;
    
    MI2C_set(channel->sdaPin);
    MI2C_set(channel->sclPin);

    if(handle->length == MI2C_ADDRESS_LENGTH_7_BITS)
    {
        addressByte = (uint8_t)baseAddress;
    }
    else
    {
        addressByte = MSUP_getHighByte(baseAddress);
    }

    /* generate start condition */
    MI2C_output(channel->sdaPin);
    MI2C_output(channel->sclPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayH);
    MI2C_reset(channel->sdaPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayH);
    MI2C_reset(channel->sclPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayL);
   /* transmit address */
    retVal = MI2C_writeByte_SWI2C(handle, addressByte);
    if( (retVal != MCAL_OK) && (handle->length == MI2C_ADDRESS_LENGTH_10_BITS) )
    {
        addressByte = (uint8_t)baseAddress;
        retVal = MI2C_writeByte_SWI2C(handle, addressByte);
    }
    handle->status = MCAL_STATUS_READY;
    return retVal;
}


static void MI2C_bitDelay_SWI2C(uint16_t delay)
{
    /** Delay must be adjusted to MI2C_Baudrate, not more than 400Khz */
    uint16_t counter;
    for(counter = 0u; counter < delay; counter++)
    {
        HSUP_vNop(); //replace later by MSUP_Nop
    }
}


static enum MCAL_Error MI2C_writeByte_SWI2C(struct MI2C_Handle *handle, uint8_t dataByte)
{
    const struct MI2C_Channel *channel = handle->channel;
    uint8_t counter;
    uint32_t enabledEvents;

    /* transmit all 8 data bits */
    counter = 8u;
    do
    {
        /* send each bit, MSB first */

        if ((dataByte & 0x80u) != 0u)
        {
            MI2C_set(channel->sdaPin);
        }
        else
        {
            MI2C_reset(channel->sdaPin);
        }
        dataByte = (uint8_t)(dataByte << 1u);

        MI2C_DataSetupTimeDelay();
        /* generate clock */
        MI2C_set(channel->sclPin);
        MI2C_bitDelay_SWI2C(handle->delays.delayH);
        MI2C_reset(channel->sclPin);
        MI2C_bitDelay_SWI2C(handle->delays.delayL);
        --counter;
    }
    while (counter > 0u);

    /* listen for ACK */
    MI2C_set(channel->sdaPin);
    MI2C_input(channel->sdaPin);
    MI2C_waitForSlave_SWI2C(handle);

    if(MI2C_get(channel->sdaPin) != false)
    {
        /* ack didn't happen, may be nothing out there */
        handle->error = MCAL_ERROR_NOT_RESPONDING;
    }
    MI2C_reset(channel->sclPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayL);
    MI2C_set(channel->sdaPin);
    MI2C_output(channel->sdaPin);

    if (handle->error != MCAL_OK)
    {
        /* generate stop condition */
        MI2C_makeStopCondition_SWI2C(handle);
    }
    /* call event handler */

    enabledEvents = handle->enabledEvents;
    if ( (enabledEvents & MCAL_getEventMask(MCAL_EVENT_WRITE_BYTE)) != 0u )
    {
        //HI2C0_vHandleEvent();  ToDo
    }
    return handle->error;

}


static void MI2C_waitForSlave_SWI2C(struct MI2C_Handle *handle)
 {
    const struct MI2C_Channel *channel = handle->channel;
    /* maximum waitstate delay */
    static const uint8_t MI2C_maxWaitState = 0xFFu;
    uint8_t wait = MI2C_maxWaitState;

    MI2C_input(channel->sclPin);
    MI2C_set(channel->sclPin);

    while (MI2C_get(channel->sclPin) == false)
    {
        wait--;

        if (wait == 0u) {
            /* SCL stuck? ... */
            handle->error = MCAL_ERROR_TIMEOUT;
            break;
        }
    }
    MI2C_output(channel->sclPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayH);

}


static void MI2C_makeStopCondition_SWI2C(struct MI2C_Handle *handle)
{
    const struct MI2C_Channel *channel = handle->channel;
    /* generate stop condition */
    MI2C_output(channel->sdaPin);
    MI2C_reset(channel->sdaPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayL);
    MI2C_waitForSlave_SWI2C(handle);
    MI2C_set(channel->sdaPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayH);

    /* release bus */
    MI2C_input(channel->sclPin);
    MI2C_input(channel->sdaPin);
}


static void MI2C_triggerReceive_SWI2C(struct MI2C_Handle *handle, bool last)
{
    const struct MI2C_Channel *channel = handle->channel;
    uint8_t counter;
    uint32_t enabledEvents;

    /* switch to input since we want to receive data */
    MI2C_input(channel->sdaPin);

    /* receive the bits -- starting with the MSB */
    counter = 8u;

    do
    {
        MI2C_set(channel->sclPin);
        MI2C_bitDelay_SWI2C(handle->delays.delayH);

        handle->lastRx <<= 1u;

        if(MI2C_get(channel->sdaPin) != false)
        {
            handle->lastRx |= 1u;
        }
        MI2C_reset(channel->sclPin);
        MI2C_bitDelay_SWI2C(handle->delays.delayL);
        --counter;

    }
    while (counter > 0u);

    /* send ACK according to the stop flag */
    MI2C_output(channel->sdaPin);

    if (last != false)
    {
        /* no acknowledge */
        MI2C_set(channel->sdaPin);
    }
    else
    {
        /* acknowledge */
        MI2C_reset(channel->sdaPin);
    }
    MI2C_waitForSlave_SWI2C(handle);
    MI2C_reset(channel->sclPin);
    MI2C_set(channel->sdaPin);
    MI2C_bitDelay_SWI2C(handle->delays.delayL);

    /* call event handler */

    enabledEvents = handle->enabledEvents;
    if(enabledEvents & MCAL_getEventMask(MCAL_EVENT_READ_BYTE))
    {
        //HI2C0_vHandleEvent(); ToDo
    }
}


static uint8_t MI2C_getRxData_SWI2C(struct MI2C_Handle const *handle)
{
    return handle->lastRx;
}


static uint8_t MI2C_readByte_SWI2C(struct MI2C_Handle *handle)
{
    MI2C_triggerReceive_SWI2C(handle, true);
    return MI2C_getRxData_SWI2C(handle);
}


static enum MCAL_Error MI2C_write_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t buffer[], uint16_t length)
{
    uint32_t counter;
    enum MCAL_Error retVal;
    

    if( (uint8_t)(MSUP_getHighByte(baseAddress) >> NO_MATTER_BITS) != ADDRESS_10_BIT_ID)
    {
        handle->length = MI2C_ADDRESS_LENGTH_7_BITS;
        UTI_RES(baseAddress,(uint16_t)LSB);
    }
    else
    {
        handle->length = MI2C_ADDRESS_LENGTH_10_BITS;
        UTI_RES(baseAddress,(uint16_t)BIT_8);
    }

    retVal = MI2C_startCommunication_SWI2C(handle, baseAddress);
    if(retVal == MCAL_OK)
    {
        for (counter = 0; counter < length; counter++)
        {
            retVal = MI2C_writeByte_SWI2C(handle, buffer[counter]);
            if(retVal != MCAL_OK)
            {
                break;
            }
        }
        if(handle->transferInProgress == false)
        {
            if(retVal == MCAL_OK)
            {
                retVal = MI2C_stopCommunication_SWI2C(handle);
            }
            else
            {
                (void)MI2C_stopCommunication_SWI2C(handle);
            }
        }
    }
    handle->status = (retVal == MCAL_OK) ? MCAL_STATUS_READY : MCAL_STATUS_ERROR;
    return retVal;
}


static enum MCAL_Error MI2C_read_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, uint8_t buffer[], uint16_t length)
{
    uint16_t counter;
    enum MCAL_Error retVal;

    if( (uint8_t)(MSUP_getHighByte(baseAddress) >> NO_MATTER_BITS) != ADDRESS_10_BIT_ID)
    {
        handle->length = MI2C_ADDRESS_LENGTH_7_BITS;
        UTI_SET(baseAddress,(uint16_t)LSB);
    }
    else
    {
        handle->length = MI2C_ADDRESS_LENGTH_10_BITS;
        UTI_SET(baseAddress,(uint16_t)BIT_8);
    }

    retVal = MI2C_startCommunication_SWI2C(handle, baseAddress);
    if(retVal == MCAL_OK)
    {
        for (counter = 0u; counter < (length - 1u); counter++)
        {
            MI2C_triggerReceive_SWI2C(handle, false);
            buffer[counter] = MI2C_getRxData_SWI2C(handle);
        }
        MI2C_triggerReceive_SWI2C(handle, true);
        buffer[counter] = MI2C_getRxData_SWI2C(handle);
        retVal = MI2C_stopCommunication_SWI2C(handle);
    }
    else
    {
        (void)MI2C_stopCommunication_SWI2C(handle);
    }
    handle->status = (retVal == MCAL_OK) ? MCAL_STATUS_READY : MCAL_STATUS_ERROR;
    return retVal;
}


static enum MCAL_Error MI2C_transfer_SWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
{
    enum MCAL_Error retVal;

    handle->transferInProgress = true;
    retVal = MI2C_write_SWI2C(handle, baseAddress, txBuffer, txLength);
    handle->transferInProgress = false;
    if(retVal == MCAL_OK)
    {
        retVal = MI2C_read_SWI2C(handle, baseAddress, rxBuffer, rxLength);
    }
    handle->status = (retVal == MCAL_OK) ? MCAL_STATUS_READY : MCAL_STATUS_ERROR;
    return retVal;
}

/*lint -save -e818 Pointer parameter could be declared as pointing to const */
static enum MCAL_Error MI2C_stopCommunication_SWI2C(struct MI2C_Handle *handle)
{
    const struct MI2C_Channel *channel = handle->channel;
    /* Send stop-condition.  Changing of SDA during SCL = OUTPUT_HIGH is */
    /* only allowed while sending start- or stop-condition.  For generating */
    /* stop-condition both SDA and SCL must be LOW */
    MI2C_reset(channel->sclPin);
    MI2C_reset(channel->sdaPin);
    MI2C_output(channel->sclPin);
    MI2C_output(channel->sdaPin);

    MI2C_bitDelay_SWI2C(handle->delays.delayL);  /* clock low period                     */
    MI2C_set(channel->sclPin);            /* change SCL edge to OUTPUT_HIGH level */
    MI2C_bitDelay_SWI2C(handle->delays.delayH);  /* Stop condition setup time            */
    MI2C_set(channel->sdaPin);            /* change SDA edge to OUTPUT_HIGH level */

    MI2C_input(channel->sclPin);
    MI2C_input(channel->sdaPin);
    return MCAL_OK;
}
/*lint -restore -e818 */


/*lint -save -e818 Pointer parameter could be declared as pointing to const */
static enum MCAL_Error MI2C_forceBusRelease_SWI2C(struct MI2C_Handle *handle)
{
    const struct MI2C_Channel *channel = handle->channel;
    uint16_t counter;
    bool sdaStatus;

    MI2C_output(channel->sclPin);
    MI2C_input(channel->sdaPin);

    for (counter = 0u; counter < 15u; counter++)
    {
        //HI2C0_vClrSCL();
        MI2C_reset(channel->sclPin);          /* security: send some clocks to free the bus-lines */
        MI2C_bitDelay_SWI2C(handle->delays.delayL);  /* clock low period */
        MI2C_set(channel->sclPin);
        MI2C_bitDelay_SWI2C(handle->delays.delayH);
    }
    (void)MI2C_stopCommunication_SWI2C(handle);

    /* check BUS-lines */
    for (counter = 0u; counter < SDA_TEST_COUNT; counter++)
    {
        sdaStatus = MI2C_get(channel->sdaPin);

        if ( sdaStatus != false)
        {
            break;
        }
    }
    return (sdaStatus != false) ? MCAL_OK : MCAL_ERROR ;
}
/*lint -restore -e818 */


static void MI2C_configureBaudrate_SWI2C(struct MI2C_Handle *handle, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode)
{
    (void)mode;

    const struct MI2C_Channel *channel = handle->channel;
    const uint16_t targetBitTime_ns = (baudRate == MI2C_BAUDRATE_STANDARD) ? 10000u : 2500u;
    const uint16_t usToNs = 1000u;
    const uint8_t bitsPerByte = 8u;
    const uint16_t singleMeasureTimeout_us = 5000;
    const uint8_t delayOversampling = 8u;

    uint32_t actualTimeMeasure_us;
    uint16_t cycleCounter;
    uint16_t dataTime_ns;
    uint16_t clockTime_ns;
    uint16_t timeoutTime_ns;
    uint16_t bitTime_ns;
    uint16_t delayTicksPerNs;
    uint16_t lowTimeCorrection_ns = 0;
    uint16_t highTimeCorrection_ns = 0;

    /* Set both pins to input so calibration does not output anything to bus */
    MI2C_input(channel->sdaPin);
    MI2C_input(channel->sclPin);

    /* Measure time of timeout handling */
    MSUP_setTimeOut(singleMeasureTimeout_us);
    for(cycleCounter = 0u; cycleCounter < usToNs; cycleCounter++)
    {
        if(MSUP_isTimeOut())
        {
            break;
        }
    }
    actualTimeMeasure_us = MSUP_getMeasuredTime();
    timeoutTime_ns = MTDIV_div_32_16((actualTimeMeasure_us * usToNs), cycleCounter);

    /* Measure time of SDA pin manipulation */
    MSUP_setTimeOut(singleMeasureTimeout_us);
    for(cycleCounter = 0u; cycleCounter < usToNs; cycleCounter++)
    {
        uint8_t dataByte = 0xAAu;

        /* dummy transmit all 8 data bits */
        for(uint8_t counter = bitsPerByte; counter > 0u; counter--)
        {
            /* send each bit, MSB first */

            if ((dataByte & 0x80u) != 0u)
            {
                MI2C_set(channel->sdaPin);
            }
            else
            {
                MI2C_reset(channel->sdaPin);
            }
            dataByte = (uint8_t)(dataByte << 1u);

            MI2C_DataSetupTimeDelay();
        }

        if(MSUP_isTimeOut())
        {
            break;
        }
    }
    actualTimeMeasure_us = MSUP_getMeasuredTime();
    dataTime_ns = MTDIV_div_32_16((actualTimeMeasure_us * usToNs), cycleCounter) - timeoutTime_ns;

    /* Measure time of SCL pin manipulation */
    MSUP_setTimeOut(singleMeasureTimeout_us);
    for(cycleCounter = 0u; cycleCounter < usToNs; cycleCounter++)
    {
        for(uint8_t counter = bitsPerByte; counter > 0u; counter--)
        {
            /* generate clock */
            MI2C_set(channel->sclPin);
            MI2C_bitDelay_SWI2C(0u);
            MI2C_reset(channel->sclPin);
            MI2C_bitDelay_SWI2C(0u);
        }

        if(MSUP_isTimeOut())
        {
            break;
        }
    }
    actualTimeMeasure_us = MSUP_getMeasuredTime();
    clockTime_ns = MTDIV_div_32_16((actualTimeMeasure_us * usToNs), cycleCounter) - timeoutTime_ns;

    /* Measure time of delay function */
    MSUP_startTimeMeasure();
    MI2C_bitDelay_SWI2C(usToNs * delayOversampling);
    delayTicksPerNs = (uint16_t)(MSUP_getMeasuredTime() / delayOversampling);

    /* Calculate actual bit time and delay constant */
    bitTime_ns = (uint16_t)(clockTime_ns + dataTime_ns) / bitsPerByte;
    
    /* Calculate time delay correction values */
    if(targetBitTime_ns > bitTime_ns)
    {
        uint16_t timeCorrection_ns = targetBitTime_ns - bitTime_ns;
        uint16_t dataBitTime_us = dataTime_ns / bitsPerByte;

        if(timeCorrection_ns > dataBitTime_us)
        {
            lowTimeCorrection_ns = (uint16_t)(timeCorrection_ns - dataBitTime_us) / 2u;
        }
        highTimeCorrection_ns = timeCorrection_ns - lowTimeCorrection_ns;
    }

    /* Set correction times */
    handle->delays.delayL = MTDIV_div_32_16(lowTimeCorrection_ns, delayTicksPerNs);
    handle->delays.delayH = MTDIV_div_32_16(highTimeCorrection_ns, delayTicksPerNs);
}


static void MI2C_set(const struct MDIO_Channel *channel)
{
    MDIO_set(channel);
}


static void MI2C_reset(const struct MDIO_Channel *channel)
{
    MDIO_reset(channel);
}


static bool MI2C_get(const struct MDIO_Channel *channel)
{
    return MDIO_read(channel);
}

/* end of functions for software created I2C */


/******************************************************************************/
/* Functions for hardware created I2C */
/******************************************************************************/


static enum MCAL_Error MI2C_init_HWI2C(struct MI2C_Handle *handle, const struct MI2C_Config *cfg)
{
    const struct MI2C_Channel *channel = handle->channel;

    MCAL_RETURN_ON_ERROR(MI2C_configurePins(channel));
    MI2C_configure(channel, cfg);
    MI2C_IRQ_enable(channel);
    MI2C_IRQ_registerHandle(handle);
    handle->taskState = MI2C_TS_READY;

    return MCAL_OK;
}


static void MI2C_enable_HWI2C(struct MI2C_Handle *handle)
{
    MI2C_enableChannel(handle->channel);
    handle->status = MCAL_STATUS_READY;
}


static void MI2C_disable_HWI2C(struct MI2C_Handle *handle)
{
    MI2C_disableChannel(handle->channel);
    handle->status = MCAL_STATUS_DISABLED;
}


static enum MCAL_Error MI2C_startCommunication_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress)
{
    const struct MI2C_Channel *channel = handle->channel;
    enum MCAL_Error retVal            = MCAL_ERROR;
    bool isReading                     = false;
    enum MCAL_Error result;

    handle->error  = MCAL_OK;
    handle->baseAddress = baseAddress;
    if(handle->length == MI2C_ADDRESS_LENGTH_7_BITS)
    {
        isReading = ((uint16_t)UTI_TEST(baseAddress, (uint16_t)LSB) != 0U ) ? true : false;
    }
    else if(handle->length == MI2C_ADDRESS_LENGTH_10_BITS)
    {
        isReading = (UTI_TEST(baseAddress, BIT_8) != 0U ) ? true : false;
    }
    else
    {
        /* some non expected state */
    }

    MI2C_beforeCommunication(channel, baseAddress, isReading, handle->length, handle->dataLength);
    result = MI2C_makeStartCondition(channel);
    if (result == MCAL_OK)
    {
        handle->taskState = (isReading == true) ? MI2C_TS_CHIP_ADDRESS_SENT_READ : MI2C_TS_SEND_DATA;
        result = MI2C_sendAddress(channel, baseAddress, handle->length);
        if (result == MCAL_OK)
        {
            retVal = MCAL_OK;
        }
        else if (result == MCAL_ERROR_NOT_RESPONDING)
        {
            handle->prevTaskState = handle->taskState;
            handle->taskState = MI2C_TS_AFTER_STOP;
            handle->error = result;
            MI2C_sendStopReg(channel);
        }
        else
        {
            handle->error = result;
        }

    }
    else
    {
        handle->error = result;
    }
    MI2C_afterCommunication(channel);
    return retVal;
}


/*lint -save -e818 Pointer parameter could be declared as pointing to const */
static enum MCAL_Error MI2C_writeByte_HWI2C(struct MI2C_Handle *handle, uint8_t dataByte)
{
    struct MI2C_Channel const *channel = handle->channel;

    MI2C_writeDataReg(channel, (uint32_t)dataByte);
    return MCAL_OK;
}


static uint8_t MI2C_readByte_HWI2C(struct MI2C_Handle *handle)
{
    struct MI2C_Channel const *channel = handle->channel;
    uint8_t readByte;

    readByte = MI2C_readDataReg(channel);
    return readByte;
}
/*lint -restore -e818 */


static enum MCAL_Error MI2C_write_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t buffer[], uint16_t length)
{
    enum MCAL_Error retVal;

    handle->dataLength = length;
    MCOM_setWriteBuffer(&handle->transfer, buffer, length);

    if( (uint8_t)(MSUP_getHighByte(baseAddress) >> NO_MATTER_BITS) != ADDRESS_10_BIT_ID)
    {
        handle->length = MI2C_ADDRESS_LENGTH_7_BITS;
        UTI_RES(baseAddress,(uint16_t)LSB);
    }
    else
    {
        handle->length = MI2C_ADDRESS_LENGTH_10_BITS;
        UTI_RES(baseAddress,(uint16_t)BIT_8);
    }

    retVal = MI2C_startCommunication_HWI2C(handle, baseAddress);
    return retVal;
}


static enum MCAL_Error MI2C_read_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, uint8_t buffer[], uint16_t length)
{
    enum MCAL_Error retVal;

    handle->dataLength = length;
    MCOM_setReadBuffer(&handle->transfer, buffer, length);

    if( (uint8_t)(MSUP_getHighByte(baseAddress) >> NO_MATTER_BITS) != ADDRESS_10_BIT_ID)
    {
        handle->length = MI2C_ADDRESS_LENGTH_7_BITS;
        UTI_SET(baseAddress,(uint16_t)LSB);
    }
    else
    {
        handle->length = MI2C_ADDRESS_LENGTH_10_BITS;
        UTI_SET(baseAddress,(uint16_t)BIT_8);
    }

    retVal = MI2C_startCommunication_HWI2C(handle, baseAddress);
    return retVal;
}


static enum MCAL_Error MI2C_transfer_HWI2C(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
{
    enum MCAL_Error retVal = MCAL_ERROR;

    if(handle != NULL)
    {
        if((MCOM_isReadReady(&handle->transfer) || MCOM_isReadComplete(&handle->transfer)) && (MCOM_isWriteReady(&handle->transfer) ||  MCOM_isWriteComplete(&handle->transfer)))
        {
            MCOM_setReadBuffer (&handle->transfer, rxBuffer, rxLength);
            MCOM_setWriteBuffer(&handle->transfer, txBuffer, txLength);
            retVal = MI2C_write_HWI2C(handle, baseAddress, txBuffer, txLength);
        }
    }
    return retVal;

}


static enum MCAL_Error MI2C_forceBusRelease_HWI2C(struct MI2C_Handle *handle)
{
    const struct MI2C_Channel *channel = handle->channel;
    enum MCAL_Error retVal = MCAL_ERROR;
    enum MCAL_Error result;

    handle->error = MCAL_OK;
    MI2C_beforeCommunication(channel, 0xFFFFU, true, MI2C_ADDRESS_LENGTH_7_BITS, 0);
    /* Generate start condition */
    result = MI2C_makeStartCondition(channel);
    if (result == MCAL_OK)
    {
        result = MI2C_sendDummyData(channel);
        if (result == MCAL_OK)
        {
            retVal = MCAL_OK;
        }
    }
    else
    {
        handle->error = result;
    }
    if(retVal == MCAL_OK)
    {
        retVal = MI2C_stopCommunication_HWI2C(handle);
    }
    else
    {
        (void)MI2C_stopCommunication_HWI2C(handle);
    }

    MI2C_afterCommunication(channel);

    return retVal;
}


static void MI2C_configureBaudrate_HWI2C(struct MI2C_Handle *handle, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode)
{
    MI2C_setBaudrate(handle->channel, baudRate, mode);
} //lint !e818 Pointer parameter 'handle' could be declared as pointing to const


static enum MCAL_Error MI2C_stopCommunication_HWI2C(struct MI2C_Handle *handle)
{
    const struct MI2C_Channel *channel = handle->channel;

    handle->error = MI2C_sendStop(channel);
    return handle->error;
}

/******************************************************************************/
/* End of functions for hardware created I2C */
/******************************************************************************/

static void MI2C_IRQ_registerHandle(struct MI2C_Handle *handle)
{
    enum MI2C_unitIndex index = MI2C_IRQ_getIndex(handle->channel);
    
    if(MI2C_IRQ_HandleTable[index] == NULL)
    {
        MI2C_IRQ_HandleTable[index] = handle;
    }
  #ifdef MI2C_MULTIPLE_HANDLES_PER_IRQ
    else
    {
        struct MI2C_Handle *handleEntry = MI2C_IRQ_HandleTable[index];
        while(handleEntry->nextHandle != NULL)
        {
            handleEntry = handleEntry->nextHandle;
        }
        handleEntry->nextHandle = handle;
    }
    handle->nextHandle = NULL;
  #endif
}


static bool MI2C_isEventPending(const struct MI2C_Handle *handle, enum MCAL_Event event, uint32_t status)
{
    uint32_t eventStatusMask = MI2C_getStatusRegMaskForEvent(handle->channel, event);
    
    if((status & eventStatusMask) != 0u)
    {
        return true;
    }
    else
    {
        return false;
    }
}


static enum MCAL_Error MI2C_configurePins(const struct MI2C_Channel *channel)
{
    static struct MDIO_Config cfg = MDIO_CFG_I2C_FUNCTION;

    if(channel->sdaPin != NULL)
    {
        MCAL_RETURN_ON_ERROR(MDIO_init(channel->sdaPin, &cfg));
    }
    if(channel->sclPin != NULL)
    {
        MCAL_RETURN_ON_ERROR(MDIO_init(channel->sclPin, &cfg));
    }
    return MCAL_OK;
}


/*lint -save -e818 Pointer parameter could be declared as pointing to const */
static bool MI2C_isInitialized(struct MI2C_Handle *handle)
{
    return (handle->status != MCAL_STATUS_RESET) ? true : false;
}
/*lint -restore -e818 */


static void MI2C_handleErrorEvent(struct MI2C_Handle *handle)
{
    struct MCAL_EventData eventData;
    enum MCAL_Error error = MI2C_getChannelError(handle->channel);

    MI2C_clearErrorReg(handle->channel);

    if(MCOM_isReadBusy(&handle->transfer))
    {
        handle->error = error;
        MCOM_setReadReady(&handle->transfer);
    }

    MCAL_setEventData_error(&eventData, error);
    MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MI2C, MCAL_EVENT_ERROR, &eventData);
}


static enum MCAL_Error MI2C_handleEvents(struct MI2C_Handle *handle, enum MCAL_Event event)
{
    enum MCAL_Error retVal = MCAL_OK;

        switch (handle->taskState)
        {
            default:
            case MI2C_TS_UNINITIALIZED:
            case MI2C_TS_READY:
            {
                break;
            }
            case MI2C_TS_SEND_DATA: // state after data byte was sent
            {
                if (event == MCAL_EVENT_ACKNOWLEDGE)
                {
                    MI2C_clearAckFlag(handle->channel);
                    handle->prevTaskState = MI2C_TS_SEND_DATA;
                    if ((MCOM_isWriteComplete(&handle->transfer) == false) && (MCOM_getNumOfDataToWrite(&handle->transfer) > 0u))
                    {
                        (void)MI2C_writeByte_HWI2C(handle, MCOM_popByte(&handle->transfer));
                    }
                    if ((MCOM_isWriteComplete(&handle->transfer) == true) || (MCOM_isWriteReady (&handle->transfer)))
                    {
                        handle->taskState = MI2C_TS_BEFORE_STOP;
                    }
                }
                else if(event == MCAL_EVENT_NOT_ACKNOWLEDGE)
                {
                    MI2C_clearNackFlag(handle->channel);
                    handle->prevTaskState = MI2C_TS_SEND_DATA;
                    handle->taskState = MI2C_TS_AFTER_STOP;
                    handle->error = MCAL_ERROR_NOT_RESPONDING;
                    MI2C_sendStopReg(handle->channel);
                }
                else
                {
                    /* some non expected state */
                    handle->error = MCAL_ERROR;
                }
                break;
            }
            case MI2C_TS_CHIP_ADDRESS_SENT_READ: // state after chip address with read access was sent
            {
                if (event == MCAL_EVENT_ACKNOWLEDGE)
                {
                    //do not clear ack here, needed for stop condition
                    MI2C_setReceiveMode(handle->channel);
                    MI2C_clearAckFlag(handle->channel);
                    handle->prevTaskState = MI2C_TS_CHIP_ADDRESS_SENT_READ;
                    handle->taskState = MI2C_TS_RECEIVE_DATA;
                    MI2C_IRQ_enableEvent(handle->channel, MCAL_EVENT_READ_BYTE);
                }
                else if(event == MCAL_EVENT_NOT_ACKNOWLEDGE)
                {
                    MI2C_clearNackFlag(handle->channel);
                    handle->prevTaskState = MI2C_TS_CHIP_ADDRESS_SENT_READ;
                    handle->taskState = MI2C_TS_AFTER_STOP;
                    handle->error = MCAL_ERROR_NOT_RESPONDING;
                    MI2C_sendStopReg(handle->channel);
                }
                else
                {
                    /* some non expected state */
                    handle->error = MCAL_ERROR;
                }
                break;
            }
            case MI2C_TS_RECEIVE_DATA:
            {
                if (event == MCAL_EVENT_READ_BYTE)
                {
                    /* Must read out data not to block further progress. */
                    MI2C_beforeReadByte(handle->channel);
                    handle->lastRx = MI2C_readByte_HWI2C(handle);
                    MCOM_pushByte(&handle->transfer, handle->lastRx);
                    MI2C_clearRxFlag(handle->channel);
                    if (MCOM_isReadComplete(&handle->transfer) == false)
                    {
                        MI2C_sendAckReg(handle->channel);
                    }
                    else
                    {
                        handle->prevTaskState = MI2C_TS_RECEIVE_DATA;
                        handle->taskState = MI2C_TS_BEFORE_STOP;
                        MI2C_sendNackReg(handle->channel);
                        MI2C_IRQ_disableEvent(handle->channel, MCAL_EVENT_READ_BYTE);
                        MI2C_clearNackFlag(handle->channel);
                        retVal = MI2C_stopCommunication_HWI2C(handle);
                        handle->taskState = MI2C_TS_READY;
                        /* raise callback function */
                        MI2C_vCommunicationFinished(handle);
                    }
               }
               else
                {
                    /* some non expected state */
                   handle->error = MCAL_ERROR;
                }
                break;
            }
            case MI2C_TS_BEFORE_STOP:  //state after last byte was sent by master to slave or last byte was received from slave
            {
                if (event == MCAL_EVENT_ACKNOWLEDGE)
                {
                    MI2C_clearAckFlag(handle->channel);

                    if((handle->prevTaskState == MI2C_TS_SEND_DATA) && (MCOM_isReadBusy(&handle->transfer) == true))
                    {  /* transfer function was called */
                        handle->prevTaskState = MI2C_TS_BEFORE_STOP;
                        (void)MI2C_read_HWI2C(handle, handle->baseAddress, handle->transfer.readBufferHandle.buffer.readBuffer, handle->transfer.readBufferHandle.length);
                    }
                    else
                    {
                        /* all bytes sent, stop generation after the current byte transfer */
                        (void)MI2C_stopCommunication_HWI2C(handle);
                        /* raise callback function */
                        MI2C_vCommunicationFinished(handle);
                        handle->prevTaskState = MI2C_TS_BEFORE_STOP;
                        handle->taskState = MI2C_TS_READY;
                    }
                }
                if (event == MCAL_EVENT_NOT_ACKNOWLEDGE)
                {
                    MI2C_clearNackFlag(handle->channel);
                    if(handle->prevTaskState != MI2C_TS_RECEIVE_DATA)
                    {
                        handle->error = MCAL_ERROR_NOT_RESPONDING;
                    }
                    /* all bytes sent, stop generation after the current byte transfer */
                    (void)MI2C_stopCommunication_HWI2C(handle);
                    /* raise callback function */
                    MI2C_vCommunicationFinished(handle);
                    handle->prevTaskState = MI2C_TS_BEFORE_STOP;
                    handle->taskState = MI2C_TS_READY;
                }
                break;
            }
            case MI2C_TS_AFTER_STOP: //state after stop condition was sent
            {
                if (event == MCAL_EVENT_STOP)
                {
                    MI2C_clearStopFlag(handle->channel);
                    MI2C_vCommunicationFinished(handle);
                    handle->prevTaskState = MI2C_TS_AFTER_STOP;
                    handle->taskState = MI2C_TS_READY;
                }
                else
                {
                    /* some non expected state */
                    handle->error = MCAL_ERROR;
                }
                break;
            }
        }

    return retVal;
}

/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/

void MI2C_IRQ_Handler(enum MI2C_unitIndex index)
{
    struct MI2C_Handle *handle = MI2C_IRQ_HandleTable[index];
    
    while(handle != NULL)
    {
        uint32_t status = MI2C_getStatusReg(handle->channel);
        
        if(MI2C_isEventPending(handle, MCAL_EVENT_ERROR, status))
        {
            MI2C_handleErrorEvent(handle);
        }
        else
        {
            if(MI2C_isEventPending(handle, MCAL_EVENT_READ_BYTE, status))
            {
                (void)MI2C_handleEvents(handle, MCAL_EVENT_READ_BYTE);
            }
            if(MI2C_isEventPending(handle, MCAL_EVENT_STOP, status))
            {
                (void)MI2C_handleEvents(handle, MCAL_EVENT_STOP);
            }
            if(MI2C_isEventPending(handle, MCAL_EVENT_ACKNOWLEDGE, status))
            {
                (void)MI2C_handleEvents(handle, MCAL_EVENT_ACKNOWLEDGE);
            }
            if(MI2C_isEventPending(handle, MCAL_EVENT_NOT_ACKNOWLEDGE, status))
            {
                (void)MI2C_handleEvents(handle, MCAL_EVENT_NOT_ACKNOWLEDGE);
            }
        }

        // Get next handle if multiple channels share the same IRQ
      #ifdef MI2C_MULTIPLE_HANDLES_PER_IRQ
        handle = handle->nextHandle;
      #else
        break;
      #endif
    }
}

/*lint -restore -e522 */
/*lint -restore -e613 */
/*lint -restore -e931 */
