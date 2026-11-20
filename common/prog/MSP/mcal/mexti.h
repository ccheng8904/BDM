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


#ifndef MEXTI_H
#define MEXTI_H

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
 *  \brief    Public definitions and declarations for unit MEXTI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include "std_lib/std_callback.h"
// #include "mcal.h"
// #include "mdio.h"
#include "mexti_types.h"
#include "mexti_mc.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** Main MEXTI handling structure.
 *  \note This structure is used for storing run-time data used for callback
 *        and event handling. All instances of this structure must be allocated
 *        in static memory.
 */
struct MEXTI_Handle
{
    /** Pointer to MEXTI channel instance */
    const struct MEXTI_Channel  *channel;
    
    /** Pointer to first instance of callback list */
    struct STDCB_Callback       *cbList;
    
    /** Variable for storing flags of enabled events for callbacks */
    volatile bool               enabledEvent;
    
    /** pointer to next handle in case multiple channels share the same IRQ */
  #ifdef MEXTI_MULTIPLE_HANDLES_PER_IRQ
    struct MEXTI_Handle         *nextHandle;
  #endif /* MEXTI_MULTIPLE_HANDLES_PER_IRQ */
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MEXTI.
 *  \note This function initializes MEXTI handle structure and peripheral HW.
 *  \param handle: pointer to MEXTI_Handle instance, stored in static memory
 *  \param channel: pointer to MEXTI_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MEXTI_init(struct MEXTI_Handle *handle,
    const struct MEXTI_Channel *channel,
    const struct MEXTI_Config *cfg);


/** Function for enabling of event.
 *  \brief This function enables event for callback functionality.
 *  \param handle: pointer to MEXTI_Handle instance
 *  \param cb: optional callback instance for which the event will be enabled
 *  \param event: event to be enabled;
 */
extern void MEXTI_enableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *callback);


/** Function for disabling of event.
 *  \brief This function disables event for callback functionality.
 *  \param handle: pointer to MEXTI_Handle instance
 *  \param cb: optional callback for which the event will be disabled
 */
extern void MEXTI_disableEvent(struct MEXTI_Handle *handle, MCAL_Callback_t *callback);


/** Function for configuring peripheral.
 *  \note This function is used internally by MEXTI_init function.
 *  \param channel: pointer to MEXTI_Channel instance
 *  \param cfg: pointer to configuration structure
 */
extern void MEXTI_configureChannel(const struct MEXTI_Channel *channel, const struct MEXTI_Config *cfg);


/** Function for reading status register of peripheral.
 *  \note If peripheral has more status registers, the returned status value is
 *        combination of all relevant status register values.
 *  \param channel: pointer to MEXTI_Channel instance
 *  \return status register(s) value
 */
extern uint32_t MEXTI_getStatusReg(const struct MEXTI_Channel *channel);


/** Function for getting status mask for event.
 *  \note Returned status mask is used in combination with MXXX_getStatusReg
 *        function.
 *  \param channel: pointer to MEXTI_Channel instance
 *  \return status mask
 */
extern uint32_t MEXTI_getStatusRegMaskForEvent(const struct MEXTI_Channel *channel);


/** Function for enabling interrupt for selected peripheral unit.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MEXTI_Channel instance
 */
extern void MEXTI_IRQ_enable(const struct MEXTI_Channel *channel);


/** Function for handling of ISR for one or more peripheral units.
 *  \note This function is not intended to be used by application
 *  \param index: IRQ index representing for which units the interrupt is pending
 */
extern void MEXTI_IRQ_Handler(enum MEXTI_IRQ_Index index);


/** Function for querying of interrupt index for given channel instance.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MEXTI_Channel instance
 */
extern enum MEXTI_IRQ_Index MEXTI_IRQ_getIndex(const struct MEXTI_Channel *channel);


/** Function for querying pin state.
 *  \param channel: pointer to MEXTI_Channel instance
 *  \return logical state of pin
 */
extern bool MEXTI_getPinState(const struct MEXTI_Channel *mexti);


/** Function for checking if event is pending.
 *  \param channel: pointer to MEXTI_Channel instance
 *  \return true if event is pending
 */
extern bool MEXTI_isEvent(const struct MEXTI_Channel *channel);


/** Function sets control register bits to enable interrupt event.
 *  \param channel: pointer to MEXTI_Channel instance
 */
extern void MEXTI_enableEventReg(const struct MEXTI_Channel *channel);


/** Function clears control register bits to disable interrupt event.
 *  \param channel: pointer to MEXTI_Channel instance
 */
extern void MEXTI_disableEventReg(const struct MEXTI_Channel *channel);


/** Function clears interrupt flag bits to prevent repeated handling.
 *  \param channel: pointer to MEXTI_Channel instance
 */
extern void MEXTI_clearEventReg(const struct MEXTI_Channel *channel);


/** The function returns the pointer to MDIO channel, which represents the MEXTI pin.
 *  \param channel: pointer to MEXTI_Channel instance
 */
extern const struct MDIO_Channel * MEXTI_getPin(const struct MEXTI_Channel *channel);


#ifdef __cplusplus
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MEXTI
{
    class Channel
    {
        private:
            struct MEXTI_Handle handle;
            const struct MEXTI_Channel *channel;
        
        public:
            Channel(const struct MEXTI_Channel *channel)
            {
                this->channel = channel;
            }
            
            enum MCAL_Error init(const struct MEXTI_Config *config)
            {
                return MEXTI_init(&this->handle, this->channel, config);
            }
            
            void enableEvent(MCAL_Callback_t *callback)
            {
                MEXTI_enableEvent(&this->handle, callback);
            }
            
            void enableEvent(MCAL::Callback *callback)
            {
                MEXTI_enableEvent(&this->handle, callback->getCallback());
            }
            
            void enableEvent(void)
            {
                MEXTI_enableEvent(&this->handle, (struct STDCB_Callback *) nullptr);
            }
            
            void disableEvent(MCAL_Callback_t *callback)
            {
                MEXTI_disableEvent(&this->handle, callback);
            }
            
            void disableEvent(MCAL::Callback *callback)
            {
                MEXTI_disableEvent(&this->handle, callback->getCallback());
            }
            
            void disableEvent(void)
            {
                MEXTI_disableEvent(&this->handle, (struct STDCB_Callback *) nullptr);
            }
    };
}

#endif // __cplusplus

#endif // MEXTI_H
