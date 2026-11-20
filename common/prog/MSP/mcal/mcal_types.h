/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      MCAL
 ******************************************************************************/


#ifndef MCAL_TYPES_H
#define MCAL_TYPES_H


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public type definitions unit MCAL.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "std_lib/std_callback.h"


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
//lint -esym(9045,*MCAL_EventData)     "non-hidden definition of type struct MCAL_EventData"

/** \brief    MCAL generic status of the module.
 *
 *  \details  This enumeration is used to query actual status of peripheral
 *            using MXXX_getStatus() function.
 */
enum MCAL_Status
{
    MCAL_STATUS_RESET = 0,  ///< Initial state - not initialized
    MCAL_STATUS_READY,      ///< Peripheral is ready for operation
    MCAL_STATUS_ERROR,      ///< Peripheral is in error state
    MCAL_STATUS_BUSY,       ///< Some activity is ongoing
    MCAL_STATUS_DISABLED    ///< Peripheral is initialized but is disabled
};


/** \brief    MCAL generic error value enumeration.
 *
 *  \details  This enumeration is mostly used as return value from functions performing
 *            operations that can result as OK or ERROR, based on input parameters
 *            or actual state of peripheral.
 */
enum MCAL_Error
{
    MCAL_OK = 0,            ///< Operation was successful
    MCAL_ERROR,             ///< Unspecified error occurred
    MCAL_ERROR_BUSY,        ///< Operation failed because peripheral is in use
    MCAL_ERROR_PARAM,       ///< Invalid required function parameter
    MCAL_ERROR_CONFIG,      ///< Invalid or unsupported peripheral configuration
    MCAL_ERROR_PRECISION,   ///< Required parameter cannot be achieved in peripheral
    MCAL_ERROR_EVENT,       ///< Unsupported event
    MCAL_ERROR_CALLBACK,    ///< Error while initialing or registering callback
    MCAL_ERROR_CHANNEL,     ///< Channel is incompatible with peripheral unit
    MCAL_ERROR_NOT_READY,   ///< Peripheral is not ready to perform operation
    MCAL_ERROR_NOT_INITIALIZED, ///< Peripheral has not been initialized before use
    MCAL_ERROR_TIMEOUT,     ///< Timeout value has been reached for blocking function

    /* MUART specific errors */
    MCAL_ERROR_FRAMING,     ///< Framing error on RX line has been detected
    MCAL_ERROR_OVERFLOW,    ///< Input data has been rewritten before processing
    MCAL_ERROR_PARITY,      ///< Parity bit not corresponding to received data
    MCAL_ERROR_NOISE,       ///< Noise during reception has been detected 

    /* MI2C specific errors */
    MCAL_ERROR_NOT_RESPONDING,  ///< Slave does not respond to read/write attempt
    MCAL_ERROR_ARLO,        ///< Arbitration lost,
    MCAL_BUS_ERROR,         ///< Generic HW error

    /* MSPI specific errors */


    MCAL_ERROR_INVALID = -1 ///< Invalid error handling from application side
};


enum MCAL_Event
{
    MCAL_EVENT_NONE                 = 0x00000000,   ///< No event - used during initialization
    
    /* Trigger event */
    MCAL_EVENT_TRIGGER              = 0x00000001,   ///< Universal event
    
    MCAL_EVENT_UPDATE               = 0x00000002,   ///< Universal event
    
    MCAL_EVENT_END_OF_CONVERSION    = 0x00000004,
    MCAL_EVENT_END_OF_SEQUENCE      = 0x00000008,
    
    /* Communication events */
    MCAL_EVENT_READ_BYTE            = 0x00000010,   ///< 1 Byte has been read. Event data is received data byte
    MCAL_EVENT_WRITE_BYTE           = 0x00000020,   ///< 1 Byte has been written. Event data are always 0
    MCAL_EVENT_READ_COMPLETE        = 0x00000040,   ///< Buffered read has been completed. Event data contains number of read bytes
    MCAL_EVENT_WRITE_COMPLETE       = 0x00000080,   ///< Buffered write has been completed. Event data contains number of written bytes
    MCAL_EVENT_TRANSFER_COMPLETE    = 0x00100000,   ///< Transfer has been completed.
    
    /* Power mode change events. Event data contains new power mode number */
    MCAL_EVENT_PMODE_CHANGE_REQ     = 0x00000100,   ///< Request to change power mode
    MCAL_EVENT_PMODE_CHANGE_PRE     = 0x00000200,   ///< Before change of power mode
    MCAL_EVENT_PMODE_CHANGE_POST    = 0x00000400,   ///< After change of power mode
    MCAL_EVENT_PMODE_CHANGE_ANY     = 0x00000700,   ///< This element is used to query any power mode change event
    
    /* Clock configuration change events. Event data contains new power mode number */
    MCAL_EVENT_CLOCK_CHANGE_REQ     = 0x00001000,   ///< Request to change clock configuration
    MCAL_EVENT_CLOCK_CHANGE_PRE     = 0x00002000,   ///< Before change of clock configuration
    MCAL_EVENT_CLOCK_CHANGE_POST    = 0x00004000,   ///< After change of clock configuration
    MCAL_EVENT_CLOCK_CHANGE_ANY     = 0x00007000,   ///< This element is used to query any clock change event

    /* i2c communication events */
    MCAL_EVENT_START                = 0x00010000,   ///< start condition was generated
    MCAL_EVENT_STOP                 = 0x00020000,   ///< stop condition was generated
    MCAL_EVENT_ACKNOWLEDGE          = 0x00040000,    ///< acknowledge from communication partner
    MCAL_EVENT_NOT_ACKNOWLEDGE      = 0x00080000,   ///< not acknowledge from communication partner

    /* Channel event */
    MCAL_EVENT_CHANNEL              = 0x20000000,

    /* Error event */
    MCAL_EVENT_ERROR                = 0x40000000   ///< Event data contain MCAL_Error number
    
    /* Any events */
    // MCAL_EVENT_ANY                  = 0xFFFFFFFF    ///< Any of above events
};


enum MCAL_Module
{
    MCAL_MODULE_MPCM,
    MCAL_MODULE_MDIO,
    MCAL_MODULE_MEXTI,
    MCAL_MODULE_MUART,
    MCAL_MODULE_MSPI,
    MCAL_MODULE_MI2C,
    MCAL_MODULE_MADC,
    MCAL_MODULE_MTIM,
    MCAL_MODULE_MWDT,
    MCAL_MODULE_MDMA,
    MCAL_MODULE_MDAC
};


enum MCAL_DataType
{
    MCAL_DATA_TYPE_NONE,
    MCAL_DATA_TYPE_UINT8,
    MCAL_DATA_TYPE_UINT16,
    MCAL_DATA_TYPE_UINT32,
    MCAL_DATA_TYPE_INT8,
    MCAL_DATA_TYPE_INT16,
    MCAL_DATA_TYPE_INT32,
    MCAL_DATA_TYPE_ERROR_VALUE
};


/*lint -esym(9045,MCAL_EventData*)  "non-hidden definition of type struct MCAL_EventData" */
/*lint -esym(9018,*MCAL_data_u)  "union '...' declared [MISRA 2012 Rule 19.2, advisory]" */
struct MCAL_EventData
{
    enum MCAL_DataType  dataType;
    union MCAL_data_u
    {
        uint8_t         uint8;
        uint16_t        uint16;
        uint32_t        uint32;
        int8_t          int8;
        int16_t         int16;
        int32_t         int32;
        enum MCAL_Error error;
    } data;
};


struct MCAL_EventResponse
{
    enum MCAL_Module        eventSource;
    enum MCAL_Event         eventType;
    struct MCAL_EventData   eventData;
};


typedef void (*MCAL_CallbackFunction_t) (void* objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse);

typedef struct STDCB_Callback MCAL_Callback_t;


#endif // MCAL_TYPES_H
