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
 *  COMP_ABBREV      MSPI
 ******************************************************************************/


#ifndef MSPI_H
#define MSPI_H

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
 *  \brief    Public definitions and declarations for unit MSPI.
 *            For more information read "./mcal/doc/mspi.md" file.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "std_lib/std_callback.h"

#include "mcal.h"
#include "mspi_types.h"
#include "mcom.h"
#include "mspi_mc.h"

#ifdef MCAL_MDMA_INCLUDED
#include "mdma.h"
#include "mdma_types.h"
#endif

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** This define serves for loop protection and sets timeout value in microseconds
*/
#define MSPI_TIMEOUT_VALUE       (10000u)


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** Main MSPI handling structure.
 *  \note This structure is used for storing run-time data used for callback
 *        handling and buffered read/write operations.  All instances of this
 *        structure must be allocated in static memory.
 */
struct MSPI_Handle
{
    /** Pointer to MSPI channel instance */
    const struct MSPI_Channel *channel;

    /** Storage object for operational status of peripheral */
    volatile enum MCAL_Status  status;

    /** Transfer object for storing information about read/write */
    struct MCOM_Transfer       transfer;

    /** Pointer to first instance of callback list */
    MCAL_Callback_t           *cbList;

    /** Variable for storing flags of enabled events for callbacks */
    volatile uint32_t          enabledEvents;

    /** Variable for storing actual configured frequency which is generated on SCLK pin */
    uint32_t                   defaultBaudrate;

    /** Pointer to MDIO_Channel that represents chip select pin */
    const struct MDIO_Channel   *chipSelectPin;

    /** Counter for data to write */
    volatile uint16_t           writeCounter;

    /** Counter for data to read */
    volatile uint16_t           readCounter;

    /** Variable for save number of bits */
    volatile enum MSPI_DataBits dataBits;

    /** Variable for storing polarity settings of clock signal */
    volatile enum MSPI_ClockPolarityCPOL clockPolarity;

    /** Pointer to next handle in case multiple handles share the same (channel) IRQ */
  #ifdef MSPI_MULTIPLE_HANDLES_PER_IRQ
    struct MSPI_Handle        *nextHandle;
  #endif /* MSPI_MULTIPLE_HANDLES_PER_IRQ */

  #ifdef MCAL_MPCM_INCLUDED
    MCAL_Callback_t            mpcmCallback;
  #endif /* MCAL_MPCM_INCLUDED */

  #ifdef MCAL_MDMA_INCLUDED
    struct MDMA_Handle        *mdmaTxHandle;
    struct MDMA_Handle        *mdmaRxHandle;

    MCAL_Callback_t            mdmaTxCallback;
    MCAL_Callback_t            mdmaRxCallback;

    struct MDMA_ChannelConfig  mdmaConfigTx;
    struct MDMA_ChannelConfig  mdmaConfigRx;
    /* holds info if configurations in this handler was set and is valid */
    bool                       mdmaConfigValid;
  #endif
}; /*lint -e9045 "non-hidden definition of type..." not hidden by intention */


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MSPI.
 *  \note This function initializes MSPI handle structure and peripheral HW.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 *  \param channel: pointer to MSPI_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_init(struct MSPI_Handle *handle, const struct MSPI_Channel *channel, const struct MSPI_Config *cfg);


/** Function for enabling of MSPI.
 *  \brief Calling this function will enable MSPI peripheral e.g. in case of
 *         returning from low power mode.
 *  \param handle: pointer to MSPI_Handle instance
 */
extern void MSPI_enable(struct MSPI_Handle *handle);


/** Function for disabling of MSPI.
 *  \brief Calling this function will disable MSPI peripheral e.g. for lowering
 *         of consumption. The peripheral configuration will be retained.
 *  \param handle: pointer to MSPI_Handle instance
 */
extern void MSPI_disable(struct MSPI_Handle *handle);


/** Function for ending communication
 * \note   This function must be called before calling MSPI_transfer(), MSPI_write(), or MSPI_read() functions to
 *         initiate a new transfer within the callback function invoked by MCAL_EVENT_READ_COMPLETE or MCAL_EVENT_WRITE_COMPLETE event.
 *         For callback functions invoked by the MCAL_EVENT_TRANSFER_COMPLETE event, this function must not be called.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 */
extern void MSPI_endCommunication(struct MSPI_Handle *handle);


/** Start of buffered read.
 *  \brief This function starts non-blocking buffered read operation.
 *         Data from MISO are saved to the buffer.
 *  \note  For starting a new read from MSPI callback function invoked by MCAL_EVENT_READ_COMPLETE or MCAL_EVENT_WRITE_COMPLETE event,
 *         it is necessary to call MSPI_endCommunication() before this function.
 *
 *  \param handle: pointer to MSPI_Handle instance
 *  \param buffer: pointer to buffer where data will be placed. This buffer needs to
 *                 be allocated during complete read operation (e.g. static variable).
 *  \param len:    number of data to be read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_read(struct MSPI_Handle *handle, uint8_t buffer[], uint16_t len);


/** Start of buffered write.
 *  \brief This function starts non-blocking buffered write operation.
 *         Data from MISO are read and ignored.
 *  \note  For starting a new write from MSPI callback function invoked by MCAL_EVENT_READ_COMPLETE or MCAL_EVENT_WRITE_COMPLETE event,
 *         it is necessary to call MSPI_endCommunication() before this function.
 *
 *  \param handle: pointer to MSPI_Handle instance
 *  \param buffer: pointer to buffer containing the data to send. This buffer needs to
 *                 be allocated during complete write operation (e.g. static variable).
 *  \param len:    number of data to be written
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_write(struct MSPI_Handle *handle, const uint8_t buffer[], uint16_t len);


/** Start of buffered write and read.
 *  \brief This function starts non-blocking buffered write and read operations.
 *  \note  For starting a new transfer from MSPI callback function invoked by MCAL_EVENT_READ_COMPLETE or MCAL_EVENT_WRITE_COMPLETE event,
 *         it is necessary to call MSPI_endCommunication() before this function.
 *
 *  \param handle:   pointer to MSPI_Handle instance
 *  \param txBuffer: pointer to buffer containing the data to send. This buffer
 *                   needs to be allocated during complete write operation (e.g.
 *                   static variable). Content of buffer is not changed.
 *  \param txLength: number of data to be written
 *  \param rxBuffer: pointer to buffer where data will be placed. This buffer needs to
 *                   be allocated during complete read operation (e.g. static variable).
 *  \param rxLength: number of data to read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_transfer(struct MSPI_Handle *handle, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength);


/** Blocking buffered read.
 *  \note This function performs blocking buffered read operation with 10ms timeout.
 *        Data from MISO are saved to the buffer.
 *  \note It is not recommended to start any new SPI transfer from MSPI callback function while using this function.
 *
 *  \param handle: pointer to MSPI_Handle instance
 *  \param buffer: pointer to buffer where data will be placed. This buffer can be local or static.
 *  \param len:    number of data to be read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_readBlocking(struct MSPI_Handle *handle, uint8_t buffer[], uint16_t len);


/** Blocking buffered write.
 *  \note This function performs blocking buffered write operation with 10ms timeout.
 *        Data from MISO are read and ignored.
 *  \note It is not recommended to start any new SPI transfer from MSPI callback function while using this function.
 *
 *  \param handle: pointer to MSPI_Handle instance
 *  \param buffer: pointer to a buffer containing the data to be sent. This buffer can be local or static.
 *  \param len:    number of data to be written
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_writeBlocking(struct MSPI_Handle *handle, const uint8_t buffer[], uint16_t len);


/** Blocking buffered write and read.
 *  \note This function performs blocking buffered write and read operations with 10ms timeout.
 *  \note It is not recommended to start any new SPI transfer from MSPI callback function while using this function.
 *
 *  \param handle:   pointer to MSPI_Handle instance
 *  \param txBuffer: pointer to a buffer containing the data to be sent. This buffer can be local or static.
 *                   The content of the buffer is not changed.
 *  \param txLength: number of data to be written
 *  \param rxBuffer: pointer to a buffer where data will be placed. This buffer can be local or static.
 *  \param rxLength: number of data to be read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_transferBlocking(struct MSPI_Handle *handle, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength);


/** This function will return actual status of MSPI
 *  \param handle: pointer to MSPI_Handle instance
 *  \return actual status of MSPI
 */
extern enum MCAL_Status MSPI_getStatus(const struct MSPI_Handle *handle);


/** Function for clearing errors.
 *  \brief This function will clear error state and clear peripheral error flags.
 *  \param handle: pointer to MSPI_Handle instance
 */
extern void MSPI_clearError(const struct MSPI_Handle *handle);


/** Function for enabling event.
 *  \brief This function enables specific event for callback functionality.
 *  \param handle: pointer to MSPI_Handle instance
 *  \param callback: pointer to callback instance
 *  \param event: event to be enabled
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_enableEvent(struct MSPI_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event);


/** Function for disabling event.
 *  \brief This function disables specific event for callback functionality.
 *  \param handle: pointer to MSPI_Handle instance
 *  \param callback: pointer to callback instance
 *  \param event: event to be disabled
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_disableEvent(const struct MSPI_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event);


/** Function for checking if event is pending.
 *  \note This function reads status register of peripheral and compares it
 *        with event mask.
 *  \param channel: pointer to MSPI_Channel instance
 *  \param event: event that will be checked
 *  \return true if event is pending
 */
extern bool MSPI_isEvent(const struct MSPI_Channel *channel, enum MCAL_Event event);


/** Function for converting void pointer to MSPI_Handle pointer.
 *  \note If the NDEBUG macro is defined, the returned value is just casted.
 *        Otherwise, the pointer is checked if handle exists and is registered.
 *        This function is useful in callback functions for converting void
 *        object pointer to handle pointer.
 *  \param handlePtr: void pointer to handle
 *  \return casted handle pointer
 */
extern struct MSPI_Handle* MSPI_getHandle(const void *handlePtr);


#ifdef MCAL_MDMA_INCLUDED
/** Function for initializing DMA transfer.
 *  \brief This function initializes DMA transfer.
 *  \param handle: pointer to MSPI_Handle instance
 *  \param txHandle: pointer to MDMA_Handle instance
 *  \param rxHandle: pointer to MDMA_Handle instance
 */
extern void MSPI_initDma(struct MSPI_Handle *handle, struct MDMA_Handle *txHandle, struct MDMA_Handle *rxHandle);


/** Function for starting DMA transfer.
 *  \brief This function starts DMA transfer.
 *  \param handle: pointer to MSPI_Handle instance
 *  \param txBuffer: pointer to buffer containing the data to send. This buffer
 *                   needs to be allocated during complete write operation (e.g.
 *                   static variable). Content of buffer is not changed.
 *  \param txLength: number of data to be written
 *  \param rxBuffer: pointer to buffer where data will be placed. This buffer needs to
 *                   be allocated during complete read operation (e.g. static variable).
 *  \param rxLength: number of data to read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_transferDma(struct MSPI_Handle *handle, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength);

/** Function for starting DMA transfer.
 *  \brief This function starts DMA transfer with the same input parameters (lengths and buffers) as in the previous call of function MSPI_transferDma.
 *  \note The function MSPI_transferDma must be called before using this function.
 *  \param handle: pointer to MSPI_Handle instance
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MSPI_transferDmaContinue(struct MSPI_Handle *handle);

#endif /* MCAL_MDMA_INCLUDED */


/** Function for handling of ISR for one or more peripheral units.
 *  \note This function is not intended to be used by application
 *  \param index: IRQ index representing for which units the interrupt is pending
 */
extern void MSPI_IRQ_Handler(enum MSPI_IRQ_Index index);


/** checks for ongoing receive.
 *  \param channel: pointer to MSPI_Channel instance
 *  \return true in case of ongoing reception otherwise false
 */
extern bool MSPI_isRxOngoing(const struct MSPI_Channel *channel);


/** checks for ongoing transmission.
 *  \param channel: pointer to MSPI_Channel instance
 *  \return true in case of ongoing transmission otherwise false
 */
extern bool MSPI_isTxOngoing(const struct MSPI_Channel *channel);


#ifdef __cplusplus
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MSPI
{
    class Channel
    {
        private:
            struct MSPI_Handle handle;
            const struct MSPI_Channel *channel;
        
        public:
            Channel(void)
            {
                channel = nullptr;
            }

            Channel(const struct MSPI_Channel *channel)
            {
                this->channel = channel;
            }

            void setChannel(const struct MSPI_Channel *channel)
            {
                this->channel = channel;
            }

            const struct MSPI_Channel *getChannel(void)
            {
                return this->channel;
            }
            
            enum MCAL_Error init(const struct MSPI_Config *config)
            {
                return MSPI_init(&this->handle, this->channel, config);
            }

            void enable(void)
            {
                MSPI_enable(&this->handle);
            }

            void disable(void)
            {
                MSPI_disable(&this->handle);
            }
            
            enum MCAL_Error enableEvent(MCAL::Callback *callback, enum MCAL_Event event)
            {
                return MSPI_enableEvent(&this->handle, callback->getCallback(), event);
            }

            enum MCAL_Error disableEvent(MCAL::Callback *callback, enum MCAL_Event event)
            {
                return MSPI_disableEvent(&this->handle, callback->getCallback(), event);
            }

            enum MCAL_Error enableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
            {
                return MSPI_enableEvent(&this->handle, callback, event);
            }

            enum MCAL_Error disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
            {
                return MSPI_disableEvent(&this->handle, callback, event);
            }

            enum MCAL_Error read(uint8_t buffer[], uint16_t len)
            {
                return MSPI_read(&this->handle, buffer, len);
            }

            enum MCAL_Error write(const uint8_t buffer[], uint16_t len)
            {
                return MSPI_write(&this->handle, buffer, len);
            }

            enum MCAL_Error transfer(const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
            {
                return  MSPI_transfer(&this->handle, txBuffer, txLength, rxBuffer, rxLength);
            }

            enum MCAL_Error readBlocking(uint8_t buffer[], uint16_t len)
            {
                return MSPI_readBlocking(&this->handle, buffer, len);
            }

            enum MCAL_Error writeBlocking(const uint8_t buffer[], uint16_t len)
            {
                return MSPI_writeBlocking(&this->handle, buffer, len);
            }

            enum MCAL_Error transferBlocking(const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
            {
                return  MSPI_transferBlocking(&this->handle, txBuffer, txLength, rxBuffer, rxLength);
            }
    };
}

#endif // __cplusplus

#endif // MSPI_H
