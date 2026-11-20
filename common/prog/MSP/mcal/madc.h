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


#ifndef MADC_H
#define MADC_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MADC.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "std_lib/std_callback.h"
#include "madc_types.h"
#include "madc_priv.h"
#include "madc_mc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/** Main MADC handling structure.
 *  \note This structure is used for storing run-time data used for callback
 *        handling and scan of multiple channels. All instances of this
 *        structure must be allocated in static memory.
 */
struct MADC_Handle
{
    /** Pointer to MADC peripheral instance */
    const struct MADC_Periph    *periph;

    /** Pointer to first instance of callback list */
    MCAL_Callback_t       *adcCallback;

    /** Pointer to first instance of callback list */
    MCAL_Callback_t       *channelCallback;
    
    /** Storage object for operational status of peripheral */
    volatile enum MCAL_Status   status;

    /** Value used for downscaling of 16-bit result */
    uint8_t                     downscale;

    /** Flag that is used to determine if regular conversion is ongoing */
    volatile bool               regularConversion;

    /** pointer to next handle */
    struct MADC_Handle          *nextHandle;

    /** Counter for how many times regular conversion function has been reentered */
    volatile uint8_t            reentrancyCounter;

    /** Pointer to array where scan channels are specified */
    const struct MADC_Channel   **scanChannels;

    /** Pointer to array where scan results are stored */
    uint16_t                    *scanResults;

    /** Number of array element where current result is stored */
    volatile uint8_t            currentResult;

    /** Lenght of scan results array */
    uint8_t                     scanChannelCount;

    /** The time limit(specified in microseconds), after which a single channel conversion result 
     *  is marked as invalid if the conversion has not finished 
     */
    uint16_t                    sampleTimeout;

    /** Variable for checking the validity of the single channel conversion result */
    enum MCAL_Error             sampleValidity;
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MADC.
 *  \note This function initializes MADC handle structure and peripheral HW.
 *  \param handle: pointer to MADC_Handle instance, stored in static memory
 *  \param channel: pointer to MADC_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MADC_init(struct MADC_Handle *handle, const struct MADC_Periph *periph, const struct MADC_Config *cfg);


/** Initialization function for MADC channel.
 *  \note This function initializes MADC handle structure and peripheral HW.
 *  \param handle: pointer to MADC_Handle instance, stored in static memory
 *  \param channel: pointer to MADC_Channel instance
 *  \param config: pointer to configuration structure
 *  \return standard MCAL return value
 */
// extern enum MCAL_Error MADC_initChannel(struct MADC_Handle *handle, const struct MADC_Channel *channel);


/** Function for enabling of MADC.
 *  \brief Calling this function will enable MADC peripheral e.g. in case of
 *         returning from low power mode.
 *  \param handle: pointer to MADC_Handle instance
 */
extern void MADC_enable(struct MADC_Handle *handle);


/** Function for disabling of MADC.
 *  \brief Calling this function will disable MADC peripheral e.g. for lowering
 *         of consumption. The peripheral configuration will be retained.
 *  \param handle: pointer to MADC_Handle instance
 */
extern void MADC_disable(struct MADC_Handle *handle);


/** This function will return actual status of MADC
 *  \param handle: pointer to MADC_Handle instance
 *  \return actual status of MADC
 */
extern enum MCAL_Status MADC_getStatus(const struct MADC_Handle *handle);


/** Function for clearing errors.
 *  \brief This function will clear error state and clear peripheral error flags.
 *  \param handle: pointer to MADC_Handle instance
 */
extern void MADC_clearError(struct MADC_Handle *handle);


/** Function for handling of ISR for one or more peripheral units.
 *  \note This function is not intended to be used by application
 *  \param index: IRQ index representing for which units the interrupt is pending
 */
extern void MADC_IRQ_Handler(enum MADC_IRQ_Index index);


/** Single sample conversion function.
 *  \param handle: pointer to MADC_Handle instance
 *  \param channel: pointer to analog channel instance
 *  \return conversion result
 */
extern uint16_t MADC_readSample(struct MADC_Handle *handle, const struct MADC_Channel *channel);


/** Function for checking the latest single channel conversion result validity
 *  \param handle: pointer to MADC_Handle instance
 *  \return conversion result validity
 */
extern bool MADC_isConversionResultValid(const struct MADC_Handle *handle);


/** Function for enabling channel event
 *  \param handle: pointer to MADC_Handle instance
 *  \param channel: pointer to analog channel instance
 *  \param callback: callback instance for which the event will be enabled
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MADC_enableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback);


/** Function for disabling channel event
 *  \param handle: pointer to MADC_Handle instance
 *  \param channel: pointer to analog channel instance
 *  \param callback: callback instance for which the event will be disabled
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MADC_disableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback);


/** Function for enabling peripheral scan event
 *  \param handle: pointer to MADC_Handle instance
 *  \param callback: callback instance for which the event will be enabled
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MADC_enablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback);


/** Function for disabling peripheral scan event
 *  \param handle: pointer to MADC_Handle instance
 *  \param callback: callback instance for which the event will be enabled
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MADC_disablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback);


/** Function for starting of non-blocking conversion of channel/s.
 *  \param handle: pointer to MADC_Handle instance
 *  \param channels: pointer to array of analog channels instances used in scanning
 *  \param channelCount: length of array specified in channels
 *  \param results: pointer to array where results are stored
 *  \return MCAL_OK if successful
 */
extern enum MCAL_Error MADC_startScanConversion(struct MADC_Handle *handle, const struct MADC_Channel *channels[], uint8_t channelCount, uint16_t *results);

/** Function for reading single channel conversion result along with its validity.
 *  \param handle: pointer to MADC_Handle instance
 *  \param channel: pointer to analog channel instance
 *  \param result: pointer to variable where result is stored
 *  \return MCAL_OK if result is valid
 */
extern enum MCAL_Error MADC_readSampleWithValidation(struct MADC_Handle *handle, const struct MADC_Channel *channel, uint16_t *result);

#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MADC
{     
    class Periph
    {
        private:
            struct MADC_Handle handle;
            
        public:
            const struct MADC_Periph *getPeriph()
            {
                return this->handle.periph;
            }
            
            void setPeriph(const struct MADC_Periph *periph)
            {
                this->handle.periph = periph;
            }
            
            Periph(const struct MADC_Periph *periph)
            {
                this->setPeriph(periph);
            }
            
            enum MCAL_Error init(const struct MADC_Config *config)
            {
                return MADC_init(&this->handle, this->getPeriph(), config);
            }

            struct MADC_Handle *getHandle(void)
            {
                return &this->handle;
            }
    };

    class Channel
    {
        private:
            struct MADC_Handle *handle;
            const struct MADC_Channel *channel;
            
        public:
            Channel(Periph *periph, const struct MADC_Channel *channel)
            {
                this->handle = periph->getHandle();
                this->channel = channel;
            }
            
            enum MCAL_Error init()
            {
                return MCAL_OK;
                // MADC_initChannel(this->channel);
            }
            
            uint16_t readSample()
            {
                return MADC_readSample(this->handle, this->channel);
            }

            const struct MADC_Channel *getChannel()
            {
                return this->channel;
            }
    };
}

#endif // __cplusplus

#endif // MADC_H
