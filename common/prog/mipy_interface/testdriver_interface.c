/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MicroPython-Board Testdriver Interface
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MIPY_Interface
 *
 *  \brief    Implementation of testdriver_interface.
 *
 *  \details  Delivers function to execute driver-functions, via UART.
 *            It handles a custom message-format with errorhandling.
 *            Also interrupt-vectors and scheduler-Tasks.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "huart.h"
#include "mipy_interface/testdriver_table_config.h"
#include "testdriver_interface.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/



/** \brief    The Size of the MIPY-message-buffer.
 *
 *  \details  Messages are stored here.
 */
#define MIPY_BUFFER_SIZE 20U

/** \brief    Contains the default message-length.
 *
 *  \details  Is by default 2: What to do or what happened, and info about it.
 */
#define MIPY_MESSAGE_LENGTH 2U


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \brief    Enumerates the message-types for MIPY-messages
 *
 *  \details  Implements: ramp-up/tear-down/ack/error.
 */
enum MIPY_MessageType
{
    MIPY_call_ramp_up,
    MIPY_call_tear_down,
    
    MIPY_ack,
    MIPY_error,
    
    MIPY_MessageType_size
};

/** \brief    Enumerates the ack-types for MIPY-messages
 *
 *  \details  Implements: Success and Errors.
 */
enum MIPY_ACKType
{
    MIPY_executed,
    MIPY_messageTypeError,
    MIPY_huartError,
    MIPY_indexError,
    
    MIPY_ACKType_size
};




/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
/** \brief Evaluates buffer to execute corresponding drivers.
 *
 *  \note By default MIPY_MESSAGE_LENGTH is 2
 *
 *  \param buffer   array with size of a MIPY_MESSAGE
 *  \return indicator if package was valid or invalid
 */
static enum MIPY_ACKType MIPY_processReceiveBuffer(const uint8_t buffer[MIPY_MESSAGE_LENGTH]);

/** \brief Handles the buffer with messages from start to end.
 *
 *  \note When the end index has run over the length, its recognized from function above
 *
 *  \param start    start of index to handle
 *  \param end      end of index to handle
 */
static void MIPY_handleBuffer(uint8_t start, uint8_t end);

/** \brief Sends a mipy_interface message with size.
 *
 *  \note In major acknowledges or errors are sent
 *
 *  \param buffer   pointer to start of message
 *  \param size     size of the message
 */
static void MIPY_sendBuffer(const uint8_t* buffer, uint8_t size);

/** \brief A wrap for HAL-Function to receive Rx-Data
 *
 *  \note Uses macro-function to have variable HUART
 *
 *  \return data from Rx
 */
static inline uint8_t MIPY_getRxData(void);

/** \brief A wrap for HAL-Function to transmit Tx-Data
 *
 *  \note Uses macro-function to have variable HUART
 *
 *  \param data     to transmit
 */
static inline void MIPY_setTxData(uint8_t data);

/** \brief Will check index and return wether ramp-up-function can be executed.
 *
 *  \note index is checked by MIPY_checkIndex
 *
 *  \param index   index of function to execute
 *  \return wether function was executed
 */
static enum MIPY_ACKType MIPY_checkRampUpIndex(uint8_t index); 

/** \brief Will check index and return wether tear-down-function can be executed.
 *
 *  \note index is checked by MIPY_checkIndex
 *
 *  \param index   index of function to execute
 *  \return wether function was executed
 */
static enum MIPY_ACKType MIPY_checkTearDownIndex(uint8_t index);

/** \brief Will check index and return verification.
 *
 *  \note just a check: index<size
 *
 *  \param size   size of function table
 *  \param index  index of function to check
 *  \return wether function can be executed
 */
static enum MIPY_ACKType MIPY_checkIndex(uint32_t size, uint8_t index);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
//!< Message receive buffer Size x Message-Length.
static uint8_t MIPY_receiveBuffer[MIPY_BUFFER_SIZE][MIPY_MESSAGE_LENGTH];

//!< End index of the message-buffer, is incremented for incoming mssage .
static uint8_t MIPY_buffer_end_index = 0U;

//!< Start index of the message-buffer, is incremented for processed message. 
static uint8_t MIPY_buffer_start_index = 0U;



/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void MIPY_receiveInterrupt(void) 
{
    static uint8_t message_no=0U; 
    static uint8_t message_index=0U;
    
    uint8_t error = MIPY_MAP_UART(MIPY_UART_CHANNEL, _ucGetRxError)();
    
    #ifdef DBM_UART_REMOVE_OVERRUN_ERROR
        #if   (MIPY_UART_CHANNEL == 0)
        USART1_ICR
        #elif (MIPY_UART_CHANNEL == 1)
        USART2_ICR
        #elif (MIPY_UART_CHANNEL == 2)
        USART3_ICR
        #else
        #error "defined uart not supported"
        #endif
        = (HUART_RX_OVERRUN_ERROR|HUART_NOISE_ERROR|HUART_FRAMING_ERROR|HUART_PARITY_ERROR);
    #endif
    
    if((error > (uint8_t)0U)) 
    {
        if (message_index == (uint8_t)0U)
        {
            MIPY_receiveBuffer[message_no][message_index] = (uint8_t)MIPY_error;
            message_index++;
        }
        else if (message_index == (uint8_t)1U)
        {
            MIPY_receiveBuffer[message_no][message_index-(uint8_t)1U] = (uint8_t)MIPY_error;
        }
        else 
        {
        }
        
        MIPY_receiveBuffer[message_no][message_index] = error;
        message_index++;
        
    } 
    else {
        uint8_t in = MIPY_getRxData();
        
        MIPY_receiveBuffer[message_no][message_index] = in;
        
        message_index++;
    }
    
    if(message_no == (uint8_t)MIPY_BUFFER_SIZE) 
    {
        message_no = 0U;
    }
    
    if(message_index == (uint8_t)MIPY_MESSAGE_LENGTH) 
    {
        message_index = 0U;
        MIPY_buffer_end_index++;
        MIPY_buffer_end_index = (MIPY_buffer_end_index)%((uint8_t)MIPY_BUFFER_SIZE);
        message_no++;
    }
    
}

BOOL MIPY_bHandleTask(void) 
{
    
    if(MIPY_buffer_end_index !=  MIPY_buffer_start_index) 
    {
        if(MIPY_buffer_end_index > MIPY_buffer_start_index) 
        {
            MIPY_handleBuffer(MIPY_buffer_start_index, MIPY_buffer_end_index);
        } 
        else 
        {
            MIPY_handleBuffer(MIPY_buffer_start_index, MIPY_BUFFER_SIZE);
            MIPY_handleBuffer(0, MIPY_buffer_end_index);
        }
    }
    
    return TRUE;
}

BOOL MIPY_bInitTask(void) 
{
    MIPY_MAP_UART(MIPY_UART_CHANNEL, _vInit)(0U);
    MIPY_MAP_UART(MIPY_UART_CHANNEL, _vEnable)();
    MIPY_MAP_UART(MIPY_UART_CHANNEL, _vEnableRxEvent)();
    return TRUE;
}



/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static void MIPY_handleBuffer(uint8_t start, uint8_t end)
{
    enum MIPY_ACKType ack;
    for(uint8_t i=start; i<end; i++) {
        ack = MIPY_processReceiveBuffer(MIPY_receiveBuffer[i]);
        uint8_t ack_buffer[MIPY_MESSAGE_LENGTH] = {(uint8_t)MIPY_ack, (uint8_t)ack};
        MIPY_sendBuffer(ack_buffer, MIPY_MESSAGE_LENGTH);
    }
    MIPY_buffer_start_index = end;
    
}

static enum MIPY_ACKType MIPY_processReceiveBuffer(const uint8_t buffer[MIPY_MESSAGE_LENGTH]) 
{
    enum MIPY_ACKType ack = MIPY_messageTypeError;
    
    if(buffer[0U] == (uint8_t)MIPY_call_ramp_up)
    {
        ack = MIPY_checkRampUpIndex(buffer[1U]);
        if(ack == MIPY_executed) {
            MIPY_ramp_up_table[buffer[1U]]();
        }
    }
    else if(buffer[0U] == (uint8_t)MIPY_call_tear_down) 
    {
        ack = MIPY_checkTearDownIndex(buffer[1U]);
        if(ack == MIPY_executed) {
            MIPY_tear_down_table[buffer[1U]]();
        }
    } 
    else if(buffer[0U] == (uint8_t)MIPY_error)  
    {
        ack = MIPY_huartError;
    } else {
        ack = MIPY_messageTypeError;
    }
    
    return ack;
}

static void MIPY_sendBuffer(const uint8_t* buffer, uint8_t size) {
    for(uint32_t i=0U; i<size; i++) {
        MIPY_setTxData(*buffer);
        buffer++;
    }
}

static enum MIPY_ACKType MIPY_checkRampUpIndex(uint8_t index) {
    
    uint32_t size = MIPY_RAMPUP_TABLESIZE;
    return MIPY_checkIndex(size, index);
}

static enum MIPY_ACKType MIPY_checkTearDownIndex(uint8_t index) {
    
    uint32_t size = MIPY_TEARDOWN_TABLESIZE;
    return MIPY_checkIndex(size, index);
}

static enum MIPY_ACKType MIPY_checkIndex(uint32_t maxindex, uint8_t index) {

    if((uint32_t)index >= maxindex) {
        return MIPY_indexError;
    }
    else {
        return MIPY_executed;
    }
}

static inline uint8_t MIPY_getRxData(void)
{
    #if defined(RX100) || defined(RX200)
        MIPY_MAP_UART(MIPY_UART_CHANNEL, _vClearRxEvent());
    #endif
    
    return MIPY_MAP_UART(MIPY_UART_CHANNEL, _ucGetRxData());
}

static inline void MIPY_setTxData(uint8_t data)
{
    MIPY_MAP_UART(MIPY_UART_CHANNEL, _vSetTxData(data));
}



