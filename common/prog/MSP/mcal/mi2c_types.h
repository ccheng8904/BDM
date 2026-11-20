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


#ifndef MI2C_TYPES_H
#define MI2C_TYPES_H

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
 *  \brief    Public definitions and type definitions for unit MI2C.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** baudrate type enumeration */
enum MI2C_Baudrate
{
    MI2C_BAUDRATE_STANDARD  = 0,  ///< bit rate up to 100 kbit/s
    MI2C_BAUDRATE_FAST,           ///< bit rate up to 400 kbit/s
    MI2C_BAUDRATE_FAST_PLUS       ///< bit rate up to 1 Mbit/s
};


/**< MI2C chip address lengths enumeration */
enum MI2C_BaseAddressLength
{
    MI2C_ADDRESS_LENGTH_7_BITS = 0,        ///< address length 7 bits
    MI2C_ADDRESS_LENGTH_10_BITS            ///< address length 10 bits
};


enum MI2C_TaskState
{
   MI2C_TS_UNINITIALIZED         = 0,
   MI2C_TS_READY                    ,
   MI2C_TS_CHIP_ADDRESS_SENT_READ   ,
   MI2C_TS_RECEIVE_DATA             ,
   MI2C_TS_SEND_DATA                ,
   MI2C_TS_BEFORE_STOP              ,
   MI2C_TS_AFTER_STOP
};


/** MI2C operational mode enumeration */
enum MI2C_OperationMode
{
    MI2C_WORK_AS_MASTER,
    MI2C_WORK_AS_SLAVE
};


enum MI2C_Type
{
    MI2C_TYPE_SW,
    MI2C_TYPE_HW
};


/** MI2C generic configuration structure */
struct MI2C_Config
{
    enum MI2C_Baudrate           baudrate;     ///< Baud rate configuration
    enum MI2C_OperationMode      mode;         ///< i2c master or slave mode
    uint16_t                     ownAddress;   ///< in case of i2c slave mode
    enum MI2C_Type               commType;     ///< HW or SW generated communication signal
    bool                         intPullUps;   ///< enable/disable internal SDA,SCL pull up resistors
};


struct MI2C_Channel;
struct MI2C_Handle;
struct MI2C_Config;
/** Pointers to basic interface functions */
struct MI2C_functions
{
    enum MCAL_Error (*init)(struct MI2C_Handle *handle, const struct MI2C_Config *cfg);
    void (*enable)(struct MI2C_Handle *handle);
    void (*disable)(struct MI2C_Handle *handle);
    enum MCAL_Error (*startCommunication)(struct MI2C_Handle *handle, uint16_t baseAddress);
    enum MCAL_Error (*stopCommunication)(struct MI2C_Handle *handle);
    enum MCAL_Error (*writeByte)(struct MI2C_Handle *handle, uint8_t data);
    uint8_t (*readByte)(struct MI2C_Handle *handle);
    enum MCAL_Error (*write)(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t buffer[], uint16_t length);
    enum MCAL_Error (*read)(struct MI2C_Handle *handle, uint16_t baseAddress, uint8_t buffer[], uint16_t length);
    enum MCAL_Error (*transfer)(struct MI2C_Handle *handle, uint16_t baseAddress, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength);
    enum MCAL_Error (*forceBusRelease)(struct MI2C_Handle *handle);
    void (*configureBaudrate)(struct MI2C_Handle *handle, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode);
};


struct MI2C_Delays
{
    uint16_t delayH;
    uint16_t delayL;
};

#ifdef __cplusplus
}
#endif

#endif // MI2C_TYPES_H
