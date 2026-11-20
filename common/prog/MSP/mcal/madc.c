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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MADC
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "madc.h"
#include "mdio.h"
#include "msup.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif /* MCAL_MPCM_INCLUDED */

#ifdef MCAL_MADC_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void MADC_IRQ_registerHandle(struct MADC_Handle *handle);
static bool MADC_isInitialized(const struct MADC_Handle *handle);
static bool MADC_isBusy(const struct MADC_Handle *handle);
static void MADC_setReady(struct MADC_Handle *handle);
static void MADC_setBusy(struct MADC_Handle *handle);
static bool MADC_isConversionCorrupted(const struct MADC_Handle *handle);
static bool MADC_isEventConversion(const struct MADC_Handle *handle);
static void MADC_setRegularConversion(struct MADC_Handle *handle);
static void MADC_clearRegularConversion(struct MADC_Handle *handle);
static bool MADC_isChannelConversionComplete(const struct MADC_Handle *handle);
static uint32_t MADC_readConversionResult(const struct MADC_Handle *handle);
static void MADC_startRegularConversion(struct MADC_Handle *handle, const struct MADC_Channel *channel);
static void MADC_restartEventConversion(struct MADC_Handle *handle);
static bool MADC_isScanComplete(const struct MADC_Handle *handle);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** Array for storing of MADC handlers for purpose of interrupt handling */
static struct MADC_Handle *MADC_IRQ_HandleTable[MADC_IRQ_CNT];


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MADC_init(struct MADC_Handle *handle, const struct MADC_Periph *periph, const struct MADC_Config *cfg)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(periph != NULL);
    
    MCAL_RETURN_ON_ERROR(MADC_configure(periph, cfg))

    handle->periph = periph;
    handle->channelCallback = NULL;
    handle->adcCallback = NULL;
    handle->downscale = (uint8_t)cfg->resolution;
    handle->reentrancyCounter = 0u;
    handle->scanChannels = NULL;
    handle->scanResults = NULL;
    handle->currentResult = 0u;
    handle->scanChannelCount = 0u;
    handle->sampleTimeout = cfg->conversionTimeout;
    handle->sampleValidity = MCAL_OK;
    handle->regularConversion = false;

    MADC_IRQ_registerHandle(handle);
    MADC_IRQ_enable(periph);

    MADC_enablePeriph(handle->periph);
    handle->status = MCAL_STATUS_READY;
    
    return MCAL_OK;
}


enum MCAL_Status MADC_getStatus(const struct MADC_Handle *handle)
{
    return handle->status;
}


void MADC_enable(struct MADC_Handle *handle)
{
    if(MADC_isInitialized(handle))
    {
        MADC_enablePeriph(handle->periph);
        handle->status = MCAL_STATUS_READY;
    }
}

void MADC_disable(struct MADC_Handle *handle)
{
    
    MADC_disablePeriph(handle->periph);
    handle->status = MCAL_STATUS_DISABLED;
}


uint16_t MADC_readSample(struct MADC_Handle *handle, const struct MADC_Channel *channel)
{
    uint16_t conversionResult;

    handle->reentrancyCounter +=1u; //++ causes MISRA issue in Axivion
    handle->sampleValidity = MCAL_OK;
    struct MSUP_TimeoutHandle timeoutHandle;

    do
    {
        MADC_startRegularConversion(handle, channel);

        if(handle->sampleTimeout != 0U)
        {
            MSUP_setTimeOutExt(&timeoutHandle, (uint32_t)handle->sampleTimeout);
        }

        while(!MADC_isChannelConversionComplete(handle))
        {
            /* Wait for end of conversion (should not take long) */
            if(MOCKI(MADC_isConversionCorrupted)(handle))
            {
                break;
            }
            else if((handle->sampleTimeout != 0U) && MSUP_isTimeOutExt(&timeoutHandle))
            {
                handle->sampleValidity = MCAL_ERROR_TIMEOUT;
                break;
            }
            else
            {
                /* empty else intended  */
            }
        }
        
        conversionResult = (uint16_t)MADC_readConversionResult(handle);

    } while(MOCKI(MADC_isConversionCorrupted)(handle));

    MADC_clearRegularConversion(handle);
    
    handle->reentrancyCounter -=1u; //-- causes MISRA issue in Axivion

    if(handle->reentrancyCounter == 0u)
    {
        MADC_restartEventConversion(handle);
    }

    return conversionResult;
}


bool MADC_isConversionResultValid(const struct MADC_Handle *handle)
{
    return (handle->sampleValidity == MCAL_OK);
}


enum MCAL_Error MADC_enableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback)
{
    uint8_t channelNum;

    MCAL_assert(handle != NULL);
    MCAL_assert(MADC_isInitialized(handle));
    
    channelNum = MADC_getChannelNum(channel);
    MADC_enablePeriphEvent(handle->periph);
    MCAL_enableChannelEvent(&handle->channelCallback, callback, channelNum);

    return MCAL_OK;
}


enum MCAL_Error MADC_disableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback)
{
    uint8_t channelNum;

    MCAL_assert(handle != NULL);
    MCAL_assert(MADC_isInitialized(handle));

    if(callback != NULL)
    {
        channelNum = MADC_getChannelNum(channel);
        MCAL_disableChannelEvent(callback, channelNum);
    }

    uint32_t eventMask = 0xFFFFFFFFu;
    bool isChannelCbEnabled = MCAL_isEventEnabled(&handle->channelCallback, eventMask);
    eventMask = MCAL_getEventMask(MCAL_EVENT_END_OF_SEQUENCE);
    bool isScanCbEnabled = MCAL_isEventEnabled(&handle->adcCallback, eventMask);

    if(!isChannelCbEnabled && !isScanCbEnabled)
    {
        MADC_disablePeriphEvent(handle->periph); // Disable IRQ generation for end of conversion
    }
    
    return MCAL_OK;
}


enum MCAL_Error MADC_enablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MADC_isInitialized(handle));

    MADC_enablePeriphEvent(handle->periph);
    MADC_enableScanEvent(handle->periph);
    MCAL_enableEvent(&handle->adcCallback, callback, MCAL_EVENT_END_OF_SEQUENCE);

    return MCAL_OK;
}


enum MCAL_Error MADC_disablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MADC_isInitialized(handle));
    
    if(callback != NULL)
    {
        MCAL_disableEvent(callback, MCAL_EVENT_END_OF_SEQUENCE);
    }

    uint32_t eventMask = 0xFFFFFFFFu;
    bool isChannelCbEnabled = MCAL_isEventEnabled(&handle->channelCallback, eventMask);
    eventMask = MCAL_getEventMask(MCAL_EVENT_END_OF_SEQUENCE);
    bool isScanCbEnabled = MCAL_isEventEnabled(&handle->adcCallback, eventMask);

    if(!isChannelCbEnabled && !isScanCbEnabled)
    {
        MADC_disablePeriphEvent(handle->periph); // Disable IRQ generation for end of conversion
    }

    if(!isScanCbEnabled)
    {
        MADC_disableScanEvent(handle->periph); // Disable IRQ generation for end of sequence
    }
    
    return MCAL_OK;
}


enum MCAL_Error MADC_startScanConversion(struct MADC_Handle *handle, const struct MADC_Channel *channels[], uint8_t channelCount, uint16_t *results)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MADC_isInitialized(handle));

    handle->scanChannels     = channels;
    handle->scanResults      = results;
    handle->scanChannelCount = channelCount;
    handle->currentResult    = 0u;
    MADC_setBusy(handle);
    
    MADC_enablePeriphEvent(handle->periph);
    MADC_enableScanEvent(handle->periph);
    MCAL_RETURN_ON_ERROR(MADC_startPeriphScanConversion(handle->periph, channels, channelCount));

    return MCAL_OK;
}


enum MCAL_Error MADC_readSampleWithValidation(struct MADC_Handle *handle, const struct MADC_Channel *channel, uint16_t *result)
{
    *result = MOCKE(MADC_readSample)(handle, channel);

    return handle->sampleValidity;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static bool MADC_isInitialized(const struct MADC_Handle *handle)
{
    return (handle->status != MCAL_STATUS_RESET) ? true : false;
}


static bool MADC_isBusy(const struct MADC_Handle *handle)
{
    return (handle->status == MCAL_STATUS_BUSY) ? true : false;
}


static void MADC_setReady(struct MADC_Handle *handle)
{
    handle->status = MCAL_STATUS_READY;
}


static void MADC_setBusy(struct MADC_Handle *handle)
{
    handle->status = MCAL_STATUS_BUSY;
}


static bool MADC_isConversionCorrupted(const struct MADC_Handle *handle)
{
    return !handle->regularConversion;
}


static bool MADC_isEventConversion(const struct MADC_Handle *handle)
{
    return !handle->regularConversion;
}


static void MADC_setRegularConversion(struct MADC_Handle *handle)
{
    MADC_disablePeriphEvent(handle->periph);
    handle->regularConversion = true;
}


static void MADC_clearRegularConversion(struct MADC_Handle *handle)
{
    MADC_clearScanEventFlag(handle->periph);
    handle->regularConversion = false;
}


static bool MADC_isChannelConversionComplete(const struct MADC_Handle *handle)
{
    return MADC_isPeriphEvent(handle->periph);
}


static uint32_t MADC_readConversionResult(const struct MADC_Handle *handle)
{
    return ((uint32_t)(MADC_readResultReg(handle->periph, handle->currentResult)) >> (uint32_t)handle->downscale);
}


static bool MADC_isScanComplete(const struct MADC_Handle *handle)
{
    return (MADC_isScanEvent(handle->periph));
}


/** Function for registering handle instance to IQR handle table.
 *  \param handle: pointer to MADC_Handle instance, stored in static memory
 */
static void MADC_IRQ_registerHandle(struct MADC_Handle *handle)
{
    uint8_t index = (uint8_t) MADC_IRQ_getIndex(handle->periph);

    if(MADC_IRQ_HandleTable[index] == NULL)
    {
        MADC_IRQ_HandleTable[index] = handle;
    }
    else
    {
        struct MADC_Handle *handleEntry = MADC_IRQ_HandleTable[index];
        while(handleEntry->nextHandle != NULL)
        {
            handleEntry = handleEntry->nextHandle;
        }
        handleEntry->nextHandle = handle;
    }
    handle->nextHandle = NULL;
}


static void MADC_restartEventConversion(struct MADC_Handle *handle)
{
    if(handle->scanChannels != NULL)
    {
        MADC_enablePeriphEvent(handle->periph);
        MADC_enableScanEvent(handle->periph);
        (void)MADC_startPeriphScanConversion(handle->periph, handle->scanChannels, handle->scanChannelCount);
    }
    else
    {
        MADC_setReady(handle);
    }
}


static void MADC_startRegularConversion(struct MADC_Handle *handle, const struct MADC_Channel *channel)
{
    MADC_setRegularConversion(handle);

    if(MADC_isBusy(handle))
    {
        MADC_stopPeriphConversion(handle->periph);
    }
    else
    {
        MADC_setBusy(handle);
    }
    
    (void)MADC_startPeriphScanConversion(handle->periph, &channel, 1u);
}


/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/

void MADC_IRQ_Handler(enum MADC_IRQ_Index index)
{
    struct MADC_Handle *handle = MADC_IRQ_HandleTable[index];
    struct MCAL_EventData eventData;
    uint16_t conversionResult;
    uint8_t channelNum;

    while(handle != NULL)
    {
        if(MADC_isEventConversion(handle))
        {
            while((MADC_isChannelConversionComplete(handle) || MADC_isScanComplete(handle)) && (handle->currentResult < handle->scanChannelCount))
            {
                conversionResult = (uint16_t)MADC_readConversionResult(handle);

                // Copy data to output
                if(handle->scanResults != NULL)
                {
                    handle->scanResults[handle->currentResult] = conversionResult;
                }

                if(handle->channelCallback != NULL)
                {
                    /* Get channel number of actually converted channel */
                    channelNum = MADC_getChannelNum(handle->scanChannels[handle->currentResult]);

                    /* Execute channel callbacks */
                    MCAL_setEventData_uint16(&eventData, conversionResult);
                    MCAL_invokeChannelCallbacks(handle->channelCallback, MCAL_MODULE_MADC, channelNum, &eventData);
                }
        
                MADC_clearConversionEventFlag(handle->periph);
                handle->currentResult += 1u;
            }

            if(MADC_isScanComplete(handle) && (handle->currentResult >= handle->scanChannelCount))
            {
                /* Execute peripheral callbacks */
                MCAL_invokeCallbacks(handle->adcCallback, MCAL_MODULE_MADC, MCAL_EVENT_END_OF_SEQUENCE, NULL);
        
                MADC_clearScanEventFlag(handle->periph);
                handle->currentResult = 0u;
            }
        }
        
        // Get next handle
        handle = handle->nextHandle;
    }
}
