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
 *  COMP_ABBREV      MTIM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MTIM
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "mtim.h"
#include "mdio.h"

#ifdef MCAL_MTIM_NDEBUG
    #define NDEBUG
#endif
#include "mcal_assert.h"
#include "math/mathDiv.h"

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

const struct MTIM_ChannelConfig MTIM_CHANNEL_COMPARE =
{
    .mode = MTIM_CHANNEL_MODE_COMPARE,
    .captureTrigger = MTIM_CHANNEL_CAPTURE_TRIGGER_DISABLED,
    .compareAction = MTIM_CHANNEL_COMPARE_ACTION_NONE
};

const struct MTIM_ChannelConfig MTIM_CHANNEL_PWM =
{
    .mode = MTIM_CHANNEL_MODE_PWM,
    .captureTrigger = MTIM_CHANNEL_CAPTURE_TRIGGER_DISABLED,
    .compareAction = MTIM_CHANNEL_COMPARE_ACTION_NONE
};

const struct MTIM_ChannelConfig MTIM_CHANNEL_CAPTURE_RISING =
{
    .mode = MTIM_CHANNEL_MODE_CAPTURE,
    .captureTrigger = MTIM_CHANNEL_CAPTURE_TRIGGER_RISING,
    .compareAction = MTIM_CHANNEL_COMPARE_ACTION_NONE
};

const struct MTIM_ChannelConfig MTIM_CHANNEL_CAPTURE_FALLING =
{
    .mode = MTIM_CHANNEL_MODE_CAPTURE,
    .captureTrigger = MTIM_CHANNEL_CAPTURE_TRIGGER_FALLING,
    .compareAction = MTIM_CHANNEL_COMPARE_ACTION_NONE
};

const struct MTIM_ChannelConfig MTIM_CHANNEL_CAPTURE_RISING_FALLING =
{
    .mode = MTIM_CHANNEL_MODE_CAPTURE,
    .captureTrigger = MTIM_CHANNEL_CAPTURE_TRIGGER_RISING_FALLING,
    .compareAction = MTIM_CHANNEL_COMPARE_ACTION_NONE
};


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** Function for registering handle instance to IQR handle table.
 *  \param handle: pointer to MTIM_Handle instance, stored in static memory
 */
static void MTIM_registerHandle(struct MTIM_Handle *handle);


static enum MCAL_Error MTIM_configurePin(const struct MDIO_Channel *pin);
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

/** Array for storing handle pointers according to IRQ index.
 */
static struct MTIM_Handle *MTIM_IRQ_HandleTable[MTIM_IRQ_CNT];


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MTIM_initTimer(struct MTIM_Handle *handle, const struct MTIM_Timer *timer, const struct MTIM_TimerConfig *config)
{
    MCAL_assert(handle != NULL);
    MCAL_RETURN_ON_ERROR(MTIM_configureTimer(timer, config));
    
    handle->reloadValue = config->reloadValue;

    if(!MCAL_isInitialized(handle->status))
    {
        handle->timer = timer;
        handle->timerCallback = NULL;
        handle->channelCallback = NULL;
        handle->status = MCAL_STATUS_READY;

        MTIM_IRQ_enable(timer);
        MTIM_registerHandle(handle);
    }

    return MCAL_OK;
}


enum MCAL_Error MTIM_initTimerPWM(struct MTIM_Handle *handle, const struct MTIM_Timer *timer, const struct MTIM_TimerConfigPWM *config)
{
    struct MTIM_TimerConfig timerConfig;
    uint32_t inputFreq;
    uint32_t reqFreq;

    handle->timer = timer;
    inputFreq = MTIM_getTimerInputFreq(handle);
    
    if(config->pwmMode == MTIM_TIMER_PWM_MODE_CENTER_ALIGNED)
    {
        timerConfig.direction = MTIM_TIMER_DIRECTION_UP_DOWN;
        reqFreq = config->frequency * 2u;
    }
    else
    {
        timerConfig.direction = MTIM_TIMER_DIRECTION_DEFAULT;
        reqFreq = config->frequency;
    }

    timerConfig.prescaler = MTDIV_div_32_32(inputFreq, reqFreq) >> 16u;
    timerConfig.prescaler = MTIM_getNextValidPrescaler(timer, timerConfig.prescaler);
    timerConfig.reloadValue = MTDIV_div_32_32(inputFreq, reqFreq * timerConfig.prescaler);
    timerConfig.reloadValue -= (timerConfig.reloadValue == 0xFFFFuL) ? 1uL : 0uL;
    timerConfig.mode = MTIM_TIMER_MODE_TIMER;

    return MTIM_initTimer(handle, timer, &timerConfig);
}


uint32_t MTIM_getTimerInputFreq(const struct MTIM_Handle *handle)
{
    enum MPCM_Clock timerClock = MTIM_getTimerClockBus(handle->timer);
    return MPCM_getClockFreq(timerClock);
}


void MTIM_enable(struct MTIM_Handle *handle)
{
    MCAL_assert(handle != NULL);
    MTIM_enableTimer(handle->timer);
    handle->status = MCAL_STATUS_READY;
}


void MTIM_disable(struct MTIM_Handle *handle)
{
    MCAL_assert(handle != NULL);
    MTIM_disableTimer(handle->timer);
    handle->status = MCAL_STATUS_DISABLED;
}


enum MCAL_Error MTIM_enableTimerEvent(struct MTIM_Handle *handle, MCAL_Callback_t *callback)
{
    MCAL_assert(handle != NULL);
    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));
    
    MCAL_enableEvent(&handle->timerCallback, callback, MCAL_EVENT_UPDATE);
    MTIM_enableTimerEventReg(handle->timer);
    return MCAL_OK;
}


enum MCAL_Error MTIM_disableTimerEvent(const struct MTIM_Handle *handle, MCAL_Callback_t *callback)
{
    MCAL_assert(handle != NULL);
    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));

    MCAL_disableEvent(callback, MCAL_EVENT_UPDATE);
    MTIM_disableTimerEventReg(handle->timer);
    return MCAL_OK;
}


uint32_t MTIM_readTimerCount(const struct MTIM_Handle *handle)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MCAL_isInitialized(handle->status));
   
    return MTIM_readTimerCountReg(handle->timer);
}


void MTIM_writeTimerCount(const struct MTIM_Handle *handle, uint32_t value)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MCAL_isInitialized(handle->status));

    MTIM_writeTimerCountReg(handle->timer, value);
}


enum MCAL_Status MTIM_getStatus(const struct MTIM_Handle *handle)
{
    MCAL_assert(handle != NULL);

    if(handle != NULL)
    {
        return handle->status;
    }
    else
    {
        return MCAL_STATUS_ERROR;
    }
}


struct MTIM_Handle* MTIM_getHandle(const void *handlePtr)
{
    MCAL_assert(handlePtr != NULL);
    
  #ifndef NDEBUG
    struct MTIM_Handle *handle;
    for(uint8_t n = 0; n < (uint8_t)MTIM_IRQ_CNT; n++)
    {
        handle = MTIM_IRQ_HandleTable[n];
        
        while(handle != NULL)
        {
            if((void *)handle == handlePtr)
            {
                return handle;
            }
          #ifdef MTIM_MULTIPLE_HANDLES_PER_IRQ
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
    return (struct MTIM_Handle*)handlePtr;/*lint !e9005 !e926 */
  #endif
}


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS FOR CHANNEL                                    */
/******************************************************************************/

enum MCAL_Error MTIM_initChannel(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, const struct MTIM_ChannelConfig *config)
{
    MCAL_assert(handle != NULL);

    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));
    
    MTIM_disableChannel(channel);

    MCAL_RETURN_ON_ERROR(MTIM_configurePin(channel->pin));

    /* In case of input capture, configure trigger */
    if(config->mode == MTIM_CHANNEL_MODE_CAPTURE)
    {
        MCAL_RETURN_ON_ERROR(MTIM_configureChannelCapture(channel, config->captureTrigger));
    }
    else if(config->mode == MTIM_CHANNEL_MODE_COMPARE)
    {
        MCAL_RETURN_ON_ERROR(MTIM_configureChannelCompare(channel, config->compareAction));
    }
    else if(config->mode == MTIM_CHANNEL_MODE_PWM)
    {
        MCAL_RETURN_ON_ERROR(MTIM_configureChannelPWM(channel));
    }
    else
    {
        /* Nothing to do */
    }

    MTIM_enableChannel(channel);

    return MCAL_OK;
}


enum MCAL_Error MTIM_enableChannelEvent(struct MTIM_Handle *handle, const struct MTIM_Channel *channel, MCAL_Callback_t *callback)
{
    uint8_t channelNum = MTIM_getChannelNum(channel);
    MCAL_assert(handle != NULL);

    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));
        
    MCAL_enableChannelEvent(&handle->channelCallback, callback, channelNum);
    MTIM_enableChannelEventReg(channel);
    return MCAL_OK;
}


enum MCAL_Error MTIM_disableChannelEvent(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, MCAL_Callback_t *callback)
{
    uint8_t channelNum = MTIM_getChannelNum(channel);
    MCAL_assert(handle != NULL);

    MCAL_RETURN_ON_ERROR(MCAL_checkInitialized(handle->status));
    
    MTIM_disableChannelEventReg(channel);
    MCAL_disableChannelEvent(callback, channelNum);
    return MCAL_OK;
}


uint32_t MTIM_readChannelValue(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MCAL_isInitialized(handle->status));
    
    (void)*handle;
    
    return MTIM_readChannelValueReg(channel);
}


void MTIM_writeChannelValue(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, uint32_t value)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MCAL_isInitialized(handle->status));
    
    (void)*handle;

    MTIM_writeChannelValueReg(channel, value);
}


bool MTIM_getChannelPinState(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(MCAL_isInitialized(handle->status));

    (void)*handle;
    
    return MDIO_read(MTIM_getChannelPin(channel));
}


void MTIM_writeChannelPWM(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, uint8_t dutyCycle)
{
    uint32_t compareValue;
    MCAL_assert(handle != NULL);
    MCAL_assert(MCAL_isInitialized(handle->status));
    
    if(dutyCycle == 255u)
    {
        compareValue = handle->reloadValue + 1u;
    }
    else
    {
        compareValue = (handle->reloadValue * dutyCycle) >> 8u;
    }
    MTIM_writeChannelValueReg(channel, compareValue);
}


void MTIM_writeChannelCustomPWM(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, uint16_t dutyCycle, uint16_t maxValue)
{
    uint32_t compareValue;
    MCAL_assert(handle != NULL);
    MCAL_assert(MCAL_isInitialized(handle->status));
    
    if(dutyCycle >= maxValue)
    {
        compareValue = handle->reloadValue + 1u;
    }
    else
    {
        compareValue = MTDIV_div_32_32((handle->reloadValue * dutyCycle), maxValue);
    }
    MTIM_writeChannelValueReg(channel, compareValue);
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void MTIM_registerHandle(struct MTIM_Handle *handle)
{
    enum MTIM_IRQ_Index index = MTIM_IRQ_getIndex(handle->timer);
    
    if(MTIM_IRQ_HandleTable[index] == NULL)
    {
        MTIM_IRQ_HandleTable[index] = handle;
    }
  #ifdef MTIM_MULTIPLE_HANDLES_PER_IRQ
    else
    {
        struct MTIM_Handle *handleEntry = MTIM_IRQ_HandleTable[index];
        while(handleEntry->nextHandle != NULL)
        {
            handleEntry = handleEntry->nextHandle;
        }
        handleEntry->nextHandle = handle;
    }
    handle->nextHandle = NULL;
  #endif
}


static bool MTIM_isTimerEventPending(const struct MTIM_Handle *handle, uint32_t status)
{
    uint32_t eventStatusMask = MTIM_getTimerStatusMaskForEvent(handle->timer);
    return ((status & eventStatusMask) != 0u) ? true : false;
}


static bool MTIM_isChannelEventPending(const struct MTIM_Handle *handle, uint8_t channelNum, uint32_t status)
{
    uint32_t eventStatusMask = MTIM_getChannelStatusMaskForEvent(handle->timer, channelNum);
    return ((status & eventStatusMask) != 0u) ? true : false;
}


static enum MCAL_Error MTIM_configurePin(const struct MDIO_Channel *pin)
{
    static const struct MDIO_Config cfg = MDIO_CFG_ALTERNATE_FUNCTION;

    if(pin != NULL)
    {
        return MDIO_init(pin, &cfg);
    }
    return MCAL_OK;
}

/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/

void MTIM_IRQ_Handler(enum MTIM_IRQ_Index index)
{
    const struct MTIM_Handle *handle = MTIM_IRQ_HandleTable[index];
    
    while(handle != NULL)
    {
        uint32_t status = MTIM_readAndClearEventReg(handle->timer);
        
        if(MTIM_isTimerEventPending(handle, status))
        {
            MCAL_invokeCallbacks(handle->timerCallback, MCAL_MODULE_MTIM, MCAL_EVENT_UPDATE, NULL);
        }
        
        uint8_t channelCount = MTIM_getTimerChannelCount(handle->timer);
        for(uint8_t channelNum = 0u; channelNum <= channelCount; channelNum++)
        {
            if(MTIM_isChannelEventPending(handle, channelNum, status))
            {
                MCAL_invokeChannelCallbacks(handle->channelCallback, MCAL_MODULE_MTIM, channelNum, NULL);
            }
        }
        
        // Get next handle if multiple channels share the same IRQ
      #ifdef MTIM_MULTIPLE_HANDLES_PER_IRQ
        handle = handle->nextHandle;
      #else
        break;
      #endif
    }
}
