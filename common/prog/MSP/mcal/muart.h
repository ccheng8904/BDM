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


#ifndef MUART_H
#define MUART_H

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
 *  \brief    Public definitions and declarations for unit MUART.
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

#include "mcal/mcal.h"
#include "mcal/muart_priv.h"
#include "mcal/muart_types.h"
#include "mcal/mcom.h"
#include "muart_mc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** Main MUART handling structure.
 *  \note This structure is used for storing run-time data used for callback
 *        handling and buffered read/write operations.  All instances of this
 *        structure must be allocated in static memory.
 */
struct MUART_Handle
{
    /** Pointer to MUART channel instance */
    const struct MUART_Channel  *channel;
    
    /** Storage object for operational status of peripheral */
    volatile enum MCAL_Status   status;

    /** Variable holding error value for non-blocking operiations */
    enum MCAL_Error             error;

    /** Flag used to inform to finigsh buffered read operation */
    volatile bool               finishRead;
    
    /** transfer object for storing information about read/write */
    struct MCOM_Transfer        transfer;
    
    /** Pointer to first instance of callback list */
    struct STDCB_Callback       *cbList;
    
    /** Variable for storing flags of enabled events for callbacks */
    volatile uint32_t           enabledEvents;
    
    /** Variable for storing actual configured baud rate */
    uint32_t                    defaultBaudrate;
    
    /** pointer to next handle in case multiple channels share the same IRQ */
  #ifdef MUART_MULTIPLE_HANDLES_PER_IRQ
    struct MUART_Handle         *nextHandle;
  #endif /* MUART_MULTIPLE_HANDLES_PER_IRQ */
  
  #ifdef MCAL_MPCM_INCLUDED
    MCAL_Callback_t             mpcmCallback;
  #endif /* MCAL_MPCM_INCLUDED */
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MUART.
 *  \note This function initializes MUART handle structure and peripheral HW.
 *  \param handle: pointer to MUART_Handle instance, stored in static memory
 *  \param channel: pointer to MUART_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return standard MCAL return value
 */
enum MCAL_Error MUART_init(struct MUART_Handle *handle, const struct MUART_Channel *channel, const struct MUART_Config *cfg);


/** Function for enabling of MUART.
 *  \brief Calling this function will enable MUART peripheral e.g. in case of
 *         returning from low power mode.
 *  \param handle: pointer to MUART_Handle instance
 */
extern void MUART_enable(struct MUART_Handle *handle);


/** Function for disabling of MUART.
 *  \brief Calling this function will disable MUART peripheral e.g. for lowering
 *         of consumption. The peripheral configuration will be retained.
 *  \param handle: pointer to MUART_Handle instance
 */
extern void MUART_disable(struct MUART_Handle *handle);


/** Function for setting new default baudrate.
 *  \brief Default baudrate is used when switching between power modes and
 *         clock configurations to maintain operation.
 *  \param handle: pointer to MUART_Handle instance
 *  \param baudrate: new default baudrate
 */
extern void MUART_setNewBaudrate(struct MUART_Handle *handle, uint32_t baudrate);


/** Function configuring RX/TX pins to alternate function.
 *  \param rxPin: pointer MDIO_Channel instance of RX pin
 *  \param txPin: pointer MDIO_Channel instance of TX pin
 */
extern enum MCAL_Error MUART_configurePins(const struct MDIO_Channel *rxPin, const struct MDIO_Channel *txPin);


/** Start of buffered write.
 *  \note This function starts non-blocking buffered write operation.
 *  \param handle: pointer to MUART_Handle instance
 *  \param buffer: pointer to buffer containing the data to send. This buffer
 *                 needs to be allocated during complete write operation.
 *  \param length: number of data to be written
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MUART_write(struct MUART_Handle *handle, const uint8_t buffer[], uint16_t length);


/** Non blocking write of string.
 *  \param handle: pointer to MUART_Handle instance
 *  \param str:    string to be sent - must end with '\0' character
 *  \return standard MCAL return value
 */
static inline enum MCAL_Error MUART_writeStr(struct MUART_Handle *handle, const char *str)
{
    /*lint -e{926} "cast from pointer to pointer [MISRA 2012 Rule 11.3, 11.5 required]" */
    return MUART_write(handle, (const uint8_t*)str, (uint16_t)strlen(str));
}

/** Start of buffered read.
 *  \note This function starts non-blocking buffered read operation.
 *        Read operation can be interrupted by MUART_finishRead function, e.g.
 *        after receiving termination character.
 *  \param handle: pointer to MUART_Handle instance
 *  \param buffer: pointer to buffer where data will be placed. This buffer
 *                 needs to be allocated during complete read operation.
 *  \param length: number of data to read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MUART_read(struct MUART_Handle *handle, uint8_t buffer[], uint16_t length);


/** Termination of non-blocking read process
 *  \note This function ends read operation and triggers read complete event.
 *  \param handle: pointer to MUART_Handle instance
 */
extern void MUART_finishRead(struct MUART_Handle *handle);


/** Function for querying number of bytes to be read
 *  \param handle: pointer to MUART_Handle instance
 *  \return number of bytes to be read
 */
// extern int16_t MUART_getBytesToRead(struct MUART_Handle *handle);


/** Function for querying number of bytes to be written
 *  \param handle: pointer to MUART_Handle instance
 *  \return number of bytes to be written
 */
// extern int16_t MUART_getBytesToWrite(struct MUART_Handle *handle);


/** Function for non-blocking write of one byte.
 *  \param handle: pointer to MUART_Handle instance
 *  \param data: data to write
 *  \return 1 if write was successful, otherwise -1
 */
extern int32_t MUART_writeByte(const struct MUART_Handle *handle, uint8_t data);


/** Function for non-blocking read of one byte.
 *  \param handle: pointer to MUART_Handle instance
 *  \return read data if successful, -1 if there were no data to read
 */
extern int32_t MUART_readByte(const struct MUART_Handle *handle);


/** This function will return overall status of MUART
 *  \param handle: pointer to MUART_Handle instance
 *  \return actual status of MUART
 */
extern enum MCAL_Status MUART_getStatus(const struct MUART_Handle *handle);


/** This function will return read status of MUART
 *  \param handle: pointer to MUART_Handle instance
 *  \return actual status of MUART
 */
extern enum MCAL_Status MUART_getReadStatus(const struct MUART_Handle *handle);


/** This function will return write status of MUART
 *  \param handle: pointer to MUART_Handle instance
 *  \return actual status of MUART
 */
extern enum MCAL_Status MUART_getWriteStatus(const struct MUART_Handle *handle);


extern enum MCAL_Error MUART_getError(const struct MUART_Handle *handle);

/** This function will return actual status non-blocking read operation.
 *  \param handle: pointer to MUART_Handle instance
 *  \return actual read status
 */
// extern enum MUART_TransferStatus MUART_getReadStatus(struct MUART_Handle *handle);


/** This function will return actual status non-blocking write operation.
 *  \param handle: pointer to MUART_Handle instance
 *  \return actual write status
 */
// extern enum MUART_TransferStatus MUART_getWriteStatus(struct MUART_Handle *handle);


/** Function for clearing errors.
 *  \brief This function will clear error state and clear peripheral error flags.
 *  \param handle: pointer to MUART_Handle instance
 */
extern void MUART_clearError(struct MUART_Handle *handle);


/** Function for enabling of event.
 *  \brief This function enables specific event for callback functionality.
 *  \param handle: pointer to MUART_Handle instance
 *  \param cb: optional callback instance for which the event will be enabled
 *  \param event: event to be enabled;
 */
extern enum MCAL_Error MUART_enableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event);


/** Function for disabling of event.
 *  \brief This function disables specific event for callback functionality.
 *  \param handle: pointer to MUART_Handle instance
 *  \param cb: optional callback for which the event will be disabled
 *  \param event: event to be disabled
 */
extern enum MCAL_Error MUART_disableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event);


/** Function for converting void pointer to MUART_Handle pointer.
 *  \note If the NDEBUG macro is defined, the returned value is just casted.
 *        Otherwise, the pointer is checked if handle exists and is registered.
 *        This function is useful in callback functions for converting void
 *        object pointer to handle pointer.
 *  \param handlePtr: void pointer to handle
 *  \return casted handle pointer
 */
extern struct MUART_Handle* MUART_getHandle(const void *handlePtr);


// Just for testing - This can be dangerous in app - blocking without timeout
extern enum MCAL_Error MUART_writeBlocking(const struct MUART_Handle *handle, const uint8_t buffer[], uint16_t length);


#ifdef __cplusplus
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MUART
{
    class Channel
    {
        private:
            /* Handle Container */
            struct MUART_Handle handle;

            /* MUART channel pointer */
            const struct MUART_Channel *channel;
            
        public:       
            /* Handle pointer - internal or external handle */
            struct MUART_Handle *handlePtr;

            Channel(const struct MUART_Channel *channel)
            {
                this->handlePtr = &this->handle;
                this->channel = channel;
            }

            Channel(struct MUART_Handle *handle)
            {
                this->handlePtr = handle;
                this->channel = this->handlePtr->channel;
            }
            
            enum MCAL_Error init(const struct MUART_Config *config)
            {
                return MUART_init(this->handlePtr, this->channel, config);
            }

            void enable(void)
            {
                MUART_enable(this->handlePtr);
            }

            void disable(void)
            {
                MUART_disable(this->handlePtr);
            }

            struct MUART_Handle *getHandle()
            {
                return this->handlePtr;
            }

            enum MCAL_Error enableEvent(MCAL::Callback *callback, enum MCAL_Event event)
            {
                return MUART_enableEvent(this->handlePtr, callback->getCallback(), event);
            }

            enum MCAL_Error disableEvent(MCAL::Callback *callback, enum MCAL_Event event)
            {
                return MUART_disableEvent(this->handlePtr, callback->getCallback(), event);
            }

            enum MCAL_Error enableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
            {
                return MUART_enableEvent(this->handlePtr, callback, event);
            }

            enum MCAL_Error disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
            {
                return MUART_disableEvent(this->handlePtr, callback, event);
            }

            int32_t write(const uint8_t buffer[], uint16_t length)
            {
                return MUART_write(this->handlePtr, buffer, length);
            }

            int32_t write(const char *string)
            {
                return MUART_writeStr(this->handlePtr, string);
            }

            int32_t write(uint8_t data)
            {
                return MUART_writeByte(this->handlePtr, data);
            }

            int32_t read(uint8_t buffer[], uint16_t length)
            {
                return MUART_read(this->handlePtr, buffer, length);
            }

            void finishRead(void)
            {
                MUART_finishRead(this->handlePtr);
            }

            enum MCAL_Error getError()
            {
                return MUART_getError(this->handlePtr);
            }

            void clearError(void)
            {
                return MUART_clearError(this->handlePtr);
            }
    };
}

#endif // __cplusplus

#endif // MUART_H
