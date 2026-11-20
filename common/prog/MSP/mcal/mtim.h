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


#ifndef MTIM_H
#define MTIM_H

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
 *  \brief    Public definitions and declarations for unit MTIM.
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
#include "mtim_priv.h"
#include "mtim_types.h"
#include "mtim_mc.h"


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MTIM_ChannelConfig MTIM_CHANNEL_COMPARE;
extern const struct MTIM_ChannelConfig MTIM_CHANNEL_PWM;

extern const struct MTIM_ChannelConfig MTIM_CHANNEL_CAPTURE_RISING;
extern const struct MTIM_ChannelConfig MTIM_CHANNEL_CAPTURE_FALLING;
extern const struct MTIM_ChannelConfig MTIM_CHANNEL_CAPTURE_RISING_FALLING;


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** Main MTIM handling structure.
 *  \note This structure is used for storing run-time data used for callback
 *        handling and scan of multiple channels. All instances of this
 *        structure must be allocated in static memory.
 */
struct MTIM_Handle
{
    /** Pointer to MTIM peripheral instance */
    const struct MTIM_Timer     *timer;
    
    /** Pointer to first instance of timer callback list */
    MCAL_Callback_t       *timerCallback;
    
    /** Pointer to first instance of channel callback list */
    MCAL_Callback_t       *channelCallback;
    
    /** Storage object for operational status of peripheral */
    volatile enum MCAL_Status   status;
    
    /** Timer reload value */
    uint32_t                    reloadValue;
    
    /** Pointer to next handle in case multiple channels share the same IRQ */
  #ifdef MTIM_MULTIPLE_HANDLES_PER_IRQ
    struct MTIM_Handle     *nextHandle;
  #endif /* MTIM_MULTIPLE_HANDLES_PER_IRQ */
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS FOR TIMER                                     */
/******************************************************************************/

/** Initialization function for MTIM.
 *  \note This function initializes MTIM handle structure and peripheral HW.
 *  \param handle: pointer to MTIM_Handle instance, stored in static memory
 *  \param channel: pointer to MTIM_Channel instance
 *  \param config: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MTIM_initTimer(struct MTIM_Handle *handle, const struct MTIM_Timer *timer, const struct MTIM_TimerConfig *config);


/** Initialization function for MTIM optimized for PWM usage.
 *  \note This function initializes MTIM handle structure and peripheral HW.
 *  \param handle: pointer to MTIM_Handle instance, stored in static memory
 *  \param channel: pointer to MTIM_Channel instance
 *  \param config: pointer to configuration structure for PWM
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MTIM_initTimerPWM(struct MTIM_Handle *handle, const struct MTIM_Timer *timer, const struct MTIM_TimerConfigPWM *config);


/** Function for enabling of MTIM.
 *  \brief Calling this function will enable MTIM peripheral e.g. in case of
 *         returning from low power mode.
 *  \param handle: pointer to MTIM_Handle instance
 */
extern void MTIM_enable(struct MTIM_Handle *handle);


/** Function for disabling of MTIM.
 *  \brief Calling this function will disable MTIM peripheral e.g. for lowering
 *         of consumption. The peripheral configuration will be retained.
 *  \param handle: pointer to MTIM_Handle instance
 */
extern void MTIM_disable(struct MTIM_Handle *handle);


/** Function for enabling of event for timer.
 *  \brief This function enables event for callback functionality.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param callback: optional callback instance for which the event will be enabled
 */
extern enum MCAL_Error MTIM_enableTimerEvent(struct MTIM_Handle *handle, MCAL_Callback_t *callback);


/** Function for disabling of event for timer.
 *  \brief This function disables event for callback functionality.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param callback: optional callback for which the event will be disabled
 */
extern enum MCAL_Error MTIM_disableTimerEvent(const struct MTIM_Handle *handle, MCAL_Callback_t *callback);


/** This function will return actual status of MTIM
 *  \param handle: pointer to MTIM_Handle instance
 *  \return actual status of MTIM
 */
extern enum MCAL_Status MTIM_getStatus(const struct MTIM_Handle *handle);


/** Function for converting void pointer to MTIM_Handle pointer.
 *  \note If the NDEBUG macro is defined, the returned value is just casted.
 *        Otherwise, the pointer is checked if handle exists and is registered.
 *        This function is useful in callback functions for converting void
 *        object pointer to handle pointer.
 *  \param handle: void pointer to handle
 *  \return casted handle pointer
 */
extern struct MTIM_Handle* MTIM_getHandle(const void *handlePtr);


/** Function for reading timer count.
 *  \param handle: pointer to MTIM_Handle instance
 *  \return current timer count value
 */
extern uint32_t MTIM_readTimerCount(const struct MTIM_Handle *handle);


/** Function for writing timer count.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param value: new value to be written
 */
extern void MTIM_writeTimerCount(const struct MTIM_Handle *handle, uint32_t value);


/** Function reading input frequency of timer.
 *  \param handle: pointer to MTIM_Handle instance
 *  \return Frequency of input clock for timer in Hz.
 */
extern uint32_t MTIM_getTimerInputFreq(const struct MTIM_Handle *handle);


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS FOR CHANNEL                                   */
/******************************************************************************/

/** Initialization function for MTIM channel.
 *  \note This function initializes MTIM handle structure and peripheral HW.
 *  \param handle: pointer to MTIM_Handle instance, stored in static memory
 *  \param channel: pointer to MTIM_Channel instance
 *  \param config: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MTIM_initChannel(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, const struct MTIM_ChannelConfig *config);


/** Function for enabling of event for channel.
 *  \brief This function enables event for callback functionality.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param callback: optional callback instance for which the event will be enabled
 */
extern enum MCAL_Error MTIM_enableChannelEvent(struct MTIM_Handle *handle, const struct MTIM_Channel *channel, MCAL_Callback_t *callback);


/** Function for disabling of event for channel.
 *  \brief This function disables event for callback functionality.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param callback: optional callback for which the event will be disabled
 */
extern enum MCAL_Error MTIM_disableChannelEvent(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, MCAL_Callback_t *callback);


/** Function for reading channel value.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param channel: pointer to MTIM_Channel instance
 *  \return current channel value
 */
extern uint32_t MTIM_readChannelValue(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel);


/** Function for writing channel value.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param channel: pointer to MTIM_Channel instance
 *  \param value: new value to be written
 */
extern void MTIM_writeChannelValue(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, uint32_t value);


/** Function for writing channel duty cycle.
 *  \param handle: pointer to MTIM_Handle instance
 *  \param channel: pointer to MTIM_Channel instance
 *  \param dutyCycle: new value to be written: 0 = 0%, 255 = 100%
 */
extern void MTIM_writeChannelPWM(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, uint8_t dutyCycle);


/** Function for writing channel duty cycle.
 *  \note Similar like MTIM_writeChannelPWM, but with custom resolution.
 *        This function uses division instead of bit shifting that might
 *        take longer on MCUs without hardware divider (Cortex M0/M0+)
 *  \param handle: pointer to MTIM_Handle instance
 *  \param channel: pointer to MTIM_Channel instance
 *  \param dutyCycle: new value to be written with custom resolution
 *  \param maxValue: maximum value for 100% duty cycle
 */
extern void MTIM_writeChannelCustomPWM(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel, uint16_t dutyCycle, uint16_t maxValue);


/** Function for reading actual pin state 
 *  \param handle: pointer to MTIM_Handle instance
 *  \param channel: pointer to MTIM_Channel instance
 *  \return pin state level
 */
extern bool MTIM_getChannelPinState(const struct MTIM_Handle *handle, const struct MTIM_Channel *channel);


#ifdef __cplusplus
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MTIM
{
    class Timer
    {
        private:
            struct MTIM_Handle handle;

        public:
            Timer(void)
            {
                this->handle.timer = nullptr;
            }

            Timer(const struct MTIM_Timer *timer)
            {
                this->handle.timer = timer;
            }

            void set(const struct MTIM_Timer *timer)
            {
                this->handle.timer = timer;
            }

            const struct MTIM_Timer *get(void)
            {
                return this->handle.timer;
            }
            
            enum MCAL_Error init(const struct MTIM_TimerConfig *config)
            {
                return MTIM_initTimer(&this->handle, this->handle.timer, config);
            }

            enum MCAL_Error initTimerPWM(const struct MTIM_TimerConfigPWM *config)
            {
                return MTIM_initTimerPWM(&this->handle, this->handle.timer, config);
            }
            
            void enable(void)
            {
                MTIM_enable(&this->handle);
            }
            
            void disable(void)
            {
                MTIM_disable(&this->handle);
            }
            
            enum MCAL_Error enableEvent(MCAL_Callback_t *callback = nullptr)
            {
                return MTIM_enableTimerEvent(&this->handle, callback);
            }
            
            enum MCAL_Error disableEvent(MCAL_Callback_t *callback = nullptr)
            {
                return MTIM_disableTimerEvent(&this->handle, callback);
            }

            enum MCAL_Error enableEvent(MCAL::Callback *callback = nullptr)
            {
                return MTIM_enableTimerEvent(&this->handle, callback->getCallback());
            }
            
            enum MCAL_Error disableEvent(MCAL::Callback *callback = nullptr)
            {
                return MTIM_disableTimerEvent(&this->handle, callback->getCallback());
            }
            
            uint32_t readCount(void)
            {
                return MTIM_readTimerCount(&this->handle);
            }
            
            void writeCount(uint32_t value)
            {
                MTIM_writeTimerCount(&this->handle, value);
            }
            
            uint32_t getInputFreq(void)
            {
                return MTIM_getTimerInputFreq(&this->handle);
            }
            
            struct MTIM_Handle *getHandle(void)
            {
                return &this->handle;
            }
    };
    
    
    class Channel
    {
        private:
            struct MTIM_Handle *handle;
            const struct MTIM_Channel *channel;
            
        public:
            Channel(void)
            {
                this->handle = nullptr;
                this->channel = nullptr;
            }

            Channel(Timer *timer, const struct MTIM_Channel *channel)
            {
                this->handle = timer->getHandle();
                this->channel = channel;
            }
            
            Channel(struct MTIM_Handle *handle, const struct MTIM_Channel *channel)
            {
                this->handle = handle;
                this->channel = channel;
            }
            
            void setChannel(const struct MTIM_Channel *channel)
            {
                this->channel = channel;
            }

            const struct MTIM_Channel *getChannel(void)
            {
                return this->channel;
            }

            void setHandle(struct MTIM_Handle *handle)
            {
                this->handle = handle;
            }

            enum MCAL_Error init(const struct MTIM_ChannelConfig *config)
            {
                return MTIM_initChannel(this->handle, this->channel, config);
            }
            
            enum MCAL_Error enableEvent(MCAL_Callback_t *callback = nullptr)
            {
                return MTIM_enableChannelEvent(this->handle, this->channel, callback);
            }
            
            enum MCAL_Error disableEvent(MCAL_Callback_t *callback = nullptr)
            {
                return MTIM_disableChannelEvent(this->handle, this->channel, callback);
            }

            enum MCAL_Error enableEvent(MCAL::Callback *callback = nullptr)
            {
                return MTIM_enableChannelEvent(this->handle, this->channel, callback->getCallback());
            }
            
            enum MCAL_Error disableEvent(MCAL::Callback *callback = nullptr)
            {
                return MTIM_disableChannelEvent(this->handle, this->channel, callback->getCallback());
            }
            
            uint32_t readValue(void)
            {
                return MTIM_readChannelValue(this->handle, this->channel);
            }
            
            void writeValue(uint32_t value)
            {
                MTIM_writeChannelValue(this->handle, this->channel, value);
            }
            
            void writePWM(uint8_t value)
            {
                MTIM_writeChannelPWM(this->handle, this->channel, value);
            }

            void writeCustomPWM(uint16_t dutyCycle, uint16_t maxValue)
            {
                MTIM_writeChannelCustomPWM(this->handle, this->channel, dutyCycle, maxValue);
            }
    };
    
}

#endif // __cplusplus

#endif // MTIM_H
