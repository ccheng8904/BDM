/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Debug_extended component
 *  COMP_ABBREV      DBGX
 ******************************************************************************/

#ifndef DBGX_MSG_LISTS_H
#define DBGX_MSG_LISTS_H

#include "dbgx_msg_handlers.h"
#include "dbus/bustypes.h"
#include "debug_extended/protocol.h"


/******************************************************************************/
/* ADDRESS DEFINITIONS                                                        */
/******************************************************************************/

#define DBGX_SUBSYSTEM_ID                      0x01  //!< DBUS subsystem for broadcast messages
#define DBGX_DBUS_PC_ID                        0xC1  //!< DBUS id


/******************************************************************************/
/* TX-MESSAGE INDEXES                                                         */
/******************************************************************************/

enum {
    DBGX_TXIDX_MSG_SEND_DBGX_DATA_TO_PC
   ,DBGX_TXIDX_COUNT   // Number of Transmit message identifiers
};


/******************************************************************************/
/* TX- AND RX-MESSAGE LENGTH                                                  */
/******************************************************************************/

#define DBGX_DATALEN_MSG_SEND_DBGX_DATA_TO_PC             (uint8_t)96  // Maximum message length for the output messages

#define DBGX_ID_LENGTH                                    (uint8_t)2   // Length of the ID of debug_extended DBus messages

extern const TbusReceiveObject DBGX_tReceiveObject[];
extern TbusTransmitObject DBGX_tTransmitObject[];
extern const uchar DBGX_numberOfElementsInSubsystem;
extern uchar DBGX_transmitFlags[];


#endif // #ifndef DBGX_MSG_LISTS_H

