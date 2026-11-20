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


#ifndef MI2C_H
#define MI2C_H

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
 *  \brief    Public definitions and declarations for unit MI2C.
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
#include "mcal/mi2c_types.h"
#include "mi2c_mc.h"
#include "mcal/mcom.h"
#include "msup.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** Main MI2C handling structure.
 *  \note This structure is used for storing run-time data used for callback
 *        handling and buffered read/write operations.  All instances of this
 *        structure must be allocated in static memory.
 */
struct MI2C_Handle
{
    /** Pointer to MI2C channel instance */
    const struct MI2C_Channel     *channel;

    /** Storage object for operational status of peripheral */
    volatile enum MCAL_Status     status;

    /** transfer object for storing information about read/write */
    struct MCOM_Transfer          transfer;

    /** Pointer to first instance of callback list */
    struct STDCB_Callback         *cbList;

    /** Variable for storing flags of enabled events for callbacks */
    volatile uint32_t             enabledEvents;

    const struct MI2C_functions   *functions;

    /** Variable for storing actual configured baud rate */
    enum MI2C_Baudrate            defaultBaudrate;

    /** i2c master or slave mode */
    enum MI2C_OperationMode       mode;

    enum MI2C_BaseAddressLength   length;           ///< Address length of slave communication partner

    enum MCAL_Error               error;

    uint16_t                      baseAddress;

    uint16_t                      dataLength;

    uint8_t                       lastRx;

    struct MI2C_Delays            delays;

    volatile enum MI2C_TaskState  taskState;

    volatile enum MI2C_TaskState  prevTaskState;

    bool                          transferInProgress;

    /** pointer to next handle in case multiple channels share the same IRQ */
  #ifdef MI2C_MULTIPLE_HANDLES_PER_IRQ
    struct MI2C_Handle            *nextHandle;
  #endif /* MI2C_MULTIPLE_HANDLES_PER_IRQ */

  #ifdef MCAL_MPCM_INCLUDED
    MCAL_Callback_t                 mpcmCallback;
  #endif /* MCAL_MPCM_INCLUDED */

}; /*lint -e9045 "non-hidden definition of type..." not hidden by intention */


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initialization function for MI2C.
 *  \note This function initializes MI2C handle structure and peripheral HW.
 *  \param handle: pointer to MI2C_Handle instance, stored in static memory
 *  \param channel: pointer to MI2C_Channel instance
 *  \param cfg: pointer to configuration structure
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_init(struct MI2C_Handle *handle, struct MI2C_Channel const *channel, const struct MI2C_Config *cfg);


/** This function will return actual status of MI2C
 *  \param handle: pointer to MI2C_Handle instance
 *  \return actual status of MI2C
 */
extern enum MCAL_Status MI2C_getStatus(const struct MI2C_Handle *handle);


/** This function will return read status of MI2C
 *  \param handle: pointer to MI2C_Handle instance
 *  \return actual read status of MI2C
 */
extern enum MCAL_Status MI2C_getReadStatus(const struct MI2C_Handle *handle);


/** This function will return write status of MI2C
 *  \param handle: pointer to MI2C_Handle instance
 *  \return actual write status of MI2C
 */
extern enum MCAL_Status MI2C_getWriteStatus(const struct MI2C_Handle *handle);


/** This function will return actual error of MI2C
 *  \param handle: pointer to MI2C_Handle instance
 *  \return actual error status of MI2C
 */
extern enum MCAL_Error MI2C_getError(const struct MI2C_Handle *handle);


/** Function for clearing errors.
 *  \brief This function will clear error state and clear peripheral error flags.
 *  \param handle: pointer to MI2C_Handle instance
 */
extern void MI2C_clearError(struct MI2C_Handle *handle);


/** Function for enabling of MI2C.
 *  \brief Calling this function will enable MI2C peripheral e.g. in case of
 *         returning from low power mode.
 *  \param handle: pointer to MI2C_Handle instance
 */
extern void MI2C_enable(struct MI2C_Handle *handle);


/** Function for disabling of MI2C.
 *  \brief Calling this function will disable MI2C peripheral e.g. for lowering
 *         of consumption. The peripheral configuration will be retained.
 *  \param handle: pointer to MI2C_Handle instance
 */
extern void MI2C_disable(struct MI2C_Handle *handle);


/** Sets the address of the slave to communicate to.
 * \note   This interface sets the address of the slave to communicate to. The read/write information is
 *         contained in the address information. After the call of this interface the transmission of the
 *         address starts.
 * \param  handle: pointer to MI2C_Handle instance
 * \param  baseAddress: the slave address (including direction 0 bit) to communicate to
 * \return standard MCAL return value
 *
 */
extern enum MCAL_Error MI2C_startCommunication(struct MI2C_Handle *handle, uint16_t baseAddress);

/** Generates a stop condition on the I2C bus.
 * \param  handle: pointer to MI2C_Handle instance
 * \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_stopCommunication(struct MI2C_Handle *handle);


/** Function informs communication finished.
 * \note   This callback function is called after stop condition appear on data line
 * \param  handle: pointer to MI2C_Handle instance
 */
extern void MI2C_vCommunicationFinished(struct MI2C_Handle *handle);


/** Function for non-blocking write of one byte.
 *  \param handle: pointer to MI2C_Handle instance
 *  \param data: data to write
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_writeByte(struct MI2C_Handle *handle, uint8_t data);


/** Function for non-blocking read of one byte.
 *  \param handle: pointer to MI2C_Handle instance
 *  \return read data if successful, 0 if there were no data to read
 */
extern uint8_t MI2C_readByte(struct MI2C_Handle *handle);


/** Start of buffered write.
 *  \note This function starts buffered write operation.
 *  \param handle: pointer to MI2C_Handle instance
 *  \param baseAddress: slave base address
 *  \param buffer: pointer to buffer containing the data to send. This buffer
 *                 needs to be allocated during complete write operation.
 *  \param length: number of data to be written
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_write(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t buffer[], uint16_t length);


/** Start of buffered read.
 *  \note This function starts buffered read operation.
 *  \param handle: pointer to MI2C_Handle instance
 *  \param baseAddress: slave base address
 *  \param buffer: pointer to buffer where data will be placed. This buffer
 *                 needs to be allocated during complete read operation.
 *  \param length: number of data to read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_read(struct MI2C_Handle *handle, uint16_t baseAddress, uint8_t buffer[], uint16_t length);


/** Start of buffered write and read.
 *  \note This function starts buffered write operation.
 *        After write the communication continues with read operation.
 *  \param handle:   pointer to MI2C_Handle instance
 *  \param baseAddress: slave base address
 *  \param txBuffer: pointer to buffer containing the data to send. This buffer
 *                   needs to be allocated during complete write operation.
 *  \param txLength: number of data to be written
 *  \param rxBuffer: pointer to buffer where data will be placed. This buffer
 *                   needs to be allocated during complete read operation.
 *  \param rxLength: number of data to read
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_transfer(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength);


/** Function generates clock pulses and a stop condition to force a slave to release the bus.
 *  \param handle:   pointer to MI2C_Handle instance
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_forceBusRelease(struct MI2C_Handle *handle);


/** Function for configuring peripheral.
 *  \note This function is used internally by MI2C_init function.
 *  \param channel: pointer to MI2C_Channel instance
 *  \param cfg: pointer to configuration structure
 */
extern void MI2C_configure(const struct MI2C_Channel *channel, const struct MI2C_Config *cfg);


/** Function for configuring communication baudrate.
 *  \param handle:   pointer to MI2C_Handle instance
 *  \param baudrate: baudrate to be configured
 *  \param mode:     master or slave mode
 */
extern void MI2C_configureBaudrate(struct MI2C_Handle *handle, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode);


/** Function for enabling peripheral by setting control register bits.
 *  \note This function is used internally by MI2C_enable function.
 *  \param channel: pointer to MI2C_Channel instance
 */
extern void MI2C_enableChannel(const struct MI2C_Channel *channel);


/** Function for disabling peripheral by clearing control register bits.
 *  \note This function is used internally by MI2C_disable function.
 *  \param channel: pointer to MI2C_Channel instance
 */
extern void MI2C_disableChannel(const struct MI2C_Channel *channel);


/** Function for reading status register of peripheral.
 *  \note If peripheral has more status registers, the returned status value is
 *        combination of all relevant status register values.
 *  \param channel: pointer to MI2C_Channel instance
 *  \return status register(s) value
 */
extern uint32_t MI2C_getStatusReg(const struct MI2C_Channel *channel);


/** Function for clearing error register of peripheral.
 *  \param channel: pointer to MI2C_Channel instance
 */
extern void MI2C_clearErrorReg(const struct MI2C_Channel *channel);


/** Function for getting status mask for event.
 *  \note Returned status mask is used in combination with MXXX_getStatusReg
 *        function, in case multiple events needs to be checked in scope of one
 *        routine. This approach prevents reading actual status register
 *        multiple times, thus in some platform clearing flags by reading it.
 *  \param channel: pointer to MI2C_Channel instance
 *  \param event: event for which the status mask will be returned
 *  \return status mask
 */
extern uint32_t MI2C_getStatusRegMaskForEvent(const struct MI2C_Channel *channel, enum MCAL_Event event);


/** Function for getting error register of peripheral.
 *  \param channel: pointer to MI2C_Channel instance
 *  \return standard MCAL return value
 */
extern enum MCAL_Error MI2C_getChannelError(const struct MI2C_Channel *channel);


/** Function for enabling of event.
 *  \brief This function enables specific event for callback functionality.
 *  \param handle: pointer to MI2C_Handle instance
 *  \param cb: optional callback instance for which the event will be enabled
 *  \param event: event to be enabled;
 */
extern void MI2C_enableEvent(struct MI2C_Handle *handle, struct STDCB_Callback *cb, enum MCAL_Event event);


/** Function for disabling of event.
 *  \brief This function disables specific event for callback functionality.
 *  \param handle: pointer to MI2C_Handle instance
 *  \param cb: optional callback for which the event will be disabled
 *  \param event: event to be disabled
 */
extern void MI2C_disableEvent(struct MI2C_Handle *handle, struct STDCB_Callback *cb, enum MCAL_Event event);


/** Function sets control register bits according to selected event.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MI2C_Channel instance
 *  \param event: event for which the register bits will be set
 */
extern void MI2C_enableEventReg(const struct MI2C_Channel *channel, enum MCAL_Event event);


/** Function clears control register bits according to selected event.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MI2C_Channel instance
 *  \param event: event for which the register bits will be cleared
 */
extern void MI2C_disableEventReg(const struct MI2C_Channel *channel, enum MCAL_Event event);


/** Function for checking if event is pending.
 *  \note This function reads status register of peripheral and compares it
 *        with event mask.
 *  \param channel: pointer to MI2C_Channel instance
 *  \param event: event that will be checked
 *  \return true if event is pending
 */
extern bool MI2C_isEvent(const struct MI2C_Channel *channel, enum MCAL_Event event);


/** Function for error check.
 *  \note This function reads error status of last communication and compares it
 *        with certain error.
 *  \param handle: pointer to MI2C_Handle instance
 *  \param error: error that is to be checked
 *  \return true if error occured
 */
extern bool MI2C_isError(struct MI2C_Handle *handle, enum MCAL_Error error);


/** Function sets control register bits according to selected event.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MI2C_Channel instance
 *  \param event: event for which the register bits will be set
 */
extern void MI2C_IRQ_enableEvent(const struct MI2C_Channel *channel, enum MCAL_Event event);


/** Function clears control register bits according to selected event.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MI2C_Channel instance
 *  \param event: event for which the register bits will be cleared
 */
extern void MI2C_IRQ_disableEvent(const struct MI2C_Channel *channel, enum MCAL_Event event);


/** Function for enabling interrupt for selected peripheral unit.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MI2C_Channel instance
 */
extern void MI2C_IRQ_enable(const struct MI2C_Channel *channel);


/** Function for handling of ISR for one or more peripheral units.
 *  \note This function is not intended to be used by application
 *  \param index: IRQ index representing for which units the interrupt is pending
 */
extern void MI2C_IRQ_Handler(enum MI2C_unitIndex index);


/** Function for registering callback instance to the callback linked list.
 *  \param handle: pointer to MI2C_Handle instance
 *  \param cb: pointer to callback instance allocated in static memory
 *  \return 0 if the operation was successful, otherwise -1;
 */
extern int32_t MI2C_attachCallback(struct MI2C_Handle *handle, struct STDCB_Callback *cb);


/** Function for querying of interrupt index for given channel instance.
 *  \note This function is not intended to be used by application
 *  \param channel: pointer to MI2C_Channel instance
 */
extern enum MI2C_unitIndex MI2C_IRQ_getIndex(const struct MI2C_Channel *channel);


/** Function for converting void pointer to MI2C_Handle pointer.
 *  \note If the NDEBUG macro is defined, the returned value is just casted.
 *        Otherwise, the pointer is checked if handle exists and is registered.
 *        This function is useful in callback functions for converting void
 *        object pointer to handle pointer.
 *  \param handlePtr: void pointer to handle
 *  \return casted handle pointer
 */
extern struct MI2C_Handle* MI2C_getHandle(const void *handlePtr);

/** Function for reading data from peripheral data register.
 *  \note Before calling this function, it must be certain, that data are
 *        available. This is done by querying read event, or inside read IRQ.
 *  \param channel: pointer to MI2C_Channel instance
 *  \return read data from data register;
 */
extern uint8_t MI2C_readDataReg(const struct MI2C_Channel *channel);

/** Function for writing data to peripheral data register.
 *  \note Before calling this function, it must be certain, that data register
 *        is empty. This is done by querying write event or inside write IRQ.
 *  \param channel: pointer to MI2C_Channel instance
 *  \param data: data to be written;
 */
extern void MI2C_writeDataReg(const struct MI2C_Channel *channel, uint32_t data);

/** Function called before start condition on i2c bus
 *  \note This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 *  \param baseAddress: slave base address (including direction 0 bit)
 *  \param isReading: communication direction, true for reading, false for writing
 *  \param addressLength: 7 or 10 bit slave address
 *  \param dataLength: count of data bytes to be transfered
 */
extern void MI2C_beforeCommunication(const struct MI2C_Channel *channel, uint16_t baseAddress, bool isReading, enum MI2C_BaseAddressLength addressLength, uint16_t dataLength);

/** Function called after address was sent on i2c bus
 *  \note This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 */
extern void MI2C_afterCommunication(const struct MI2C_Channel *channel);


/** Function called before byte read from data register
 *  \note This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 */
extern void MI2C_beforeReadByte(const struct MI2C_Channel *channel);


/** Function sets receive mode
 *  \note This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 */
extern void MI2C_setReceiveMode(const struct MI2C_Channel *channel);


/** Function creates start condition on i2c bus
 *  \note This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 *  \return MCAL_Error value
 */
extern enum MCAL_Error MI2C_makeStartCondition(const struct MI2C_Channel *channel);

/** Function sends slave address on i2c bus
 *  \note This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 *  \param baseAddress: slave base address (including direction 0 bit)
 *  \param addressLength: 7 or 10 bit slave address
 *  \return MCAL_Error value
 */
extern enum MCAL_Error MI2C_sendAddress(const struct MI2C_Channel *channel, uint16_t baseAddress, enum MI2C_BaseAddressLength addressLength);

/** Function sends dummy data on i2c bus
 *  \note Aim is to generate clock pulses on SCL line to make data line free 
 *        This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 *  \return MCAL_Error value
 */
extern enum MCAL_Error MI2C_sendDummyData(const struct MI2C_Channel *channel);

/** Function creates stop condition on i2c bus
 *  \note This function is used internally.
 *  \param channel: pointer to MI2C_Channel instance
 *  \return MCAL_Error value
 */
extern enum MCAL_Error MI2C_sendStop(const struct MI2C_Channel *channel);


/** Function sends slave address on i2c bus and immediately generates stop
 *  \note This function can be used for test of slave availability.
 *  \param channel: pointer to MI2C_Channel instance
 *  \param baseAddress: slave base address (including direction 0 bit)
 *  \param addressLength: 7 or 10 bit slave address
 *  \return MCAL_Error value
 */
extern enum MCAL_Error MI2C_quickCommand(const struct MI2C_Channel *channel, uint16_t baseAddress, enum MI2C_BaseAddressLength addressLength);

#ifdef __cplusplus
}
#endif


/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus

namespace MI2C
{
    class Channel
    {
        private:
            struct MI2C_Handle handle;
            const struct MI2C_Channel *channel;
        public:
            Channel(const struct MI2C_Channel *channel)
            {
                this->channel = channel;
            }
            
            enum MCAL_Error init(const struct MI2C_Config *config)
            {
                return MI2C_init(&this->handle, this->channel, config);
            }
            
            void enable(void)
            {
                MI2C_enable(&this->handle);
            }
            
            void disable(void)
            {
                MI2C_disable(&this->handle);
            }
            
            enum MCAL_Error forceBusRelease()
            {
                return MI2C_forceBusRelease(&this->handle);
            }
            
            void configureBaudrate(enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode)
            {
                MI2C_configureBaudrate(&this->handle, baudRate, mode);
            }
            
            enum MCAL_Error transfer(uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
            {
                return MI2C_transfer(&this->handle, baseAddress, txBuffer, txLength, rxBuffer, rxLength);
            }
            
            enum MCAL_Error read(uint16_t baseAddress, uint8_t buffer[], uint16_t length)
            {
                return MI2C_read(&this->handle, baseAddress, buffer, length);
            }
            
            enum MCAL_Error write(uint16_t baseAddress, const uint8_t buffer[], uint16_t length)
            {
                return MI2C_write(&this->handle, baseAddress, buffer, length);
            }
            
            enum MCAL_Error writeByte(uint8_t data)
            {
                return MI2C_writeByte(&this->handle, data);
            }
            
            uint8_t readByte()
            {
                return MI2C_readByte(&this->handle);
            }
            
            void enableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
            {
                MI2C_enableEvent(&this->handle, callback, event);
            }
            
            void enableEvent(MCAL::Callback *callback, enum MCAL_Event event)
            {
                MI2C_enableEvent(&this->handle, callback->getCallback(), event);
            }
            
            void disableEvent(MCAL_Callback_t *callback, enum MCAL_Event event)
            {
                MI2C_disableEvent(&this->handle, callback, event);
            }
            
            void disableEvent(MCAL::Callback *callback, enum MCAL_Event event)
            {
                MI2C_disableEvent(&this->handle, callback->getCallback(), event);
            }
            
            enum MCAL_Error startCommunication(uint16_t baseAddress)
            {
                return MI2C_startCommunication(&this->handle, baseAddress);
            }
            
            enum MCAL_Error stopCommunication()
            {
                return MI2C_stopCommunication(&this->handle);
            }
    };
}

#endif // __cplusplus

#endif // MI2C_H
