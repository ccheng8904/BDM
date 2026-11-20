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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file    dbgx_msg_lists.c
 *
 *  \ingroup Debug_extended DBus access
 *
 *  \brief   Debug_extended-internal module for the lists of Rx- and Tx-DBus-messages
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "dbgx_msg_lists.h"


/******************************************************************************/
/* MESSAGE IDENTIFIERS - RECEIVE / TRANSMIT                                   */
/******************************************************************************/

#define DBGX_MSGID_MSG_SEND_DBGX_DATA_TO_CONTROLLER            0xB9FE

#define DBGX_MSGID_MSG_SEND_DBGX_DATA_TO_PC                    0xB9FF


/******************************************************************************/
/* RECEIVE TABLE                                                              */
/******************************************************************************/

const TbusReceiveObject DBGX_tReceiveObject[] = {
    {{MSB_BYTE, 0x00, DBGX_MSGID_MSG_SEND_DBGX_DATA_TO_CONTROLLER}, DBGX_Rx_MsgSendDbgxDataToController}
};


/******************************************************************************/
/* TRANSMIT TABLE                                                             */
/******************************************************************************/

TbusTransmitObject DBGX_tTransmitObject[] = {
  {{DBGX_DATALEN_MSG_SEND_DBGX_DATA_TO_PC + DBGX_ID_LENGTH, DBGX_DBUS_PC_ID, DBGX_MSGID_MSG_SEND_DBGX_DATA_TO_PC},
    DBGX_DATALEN_MSG_SEND_DBGX_DATA_TO_PC,
    DBGX_Tx_MsgSendDbgxDataToPc,
    DBGX_Tx_Msg_Confirm}
};

const uchar DBGX_numberOfElementsInSubsystem = (uint8_t)DBGX_TXIDX_COUNT;
uchar DBGX_transmitFlags[((uchar)DBGX_TXIDX_COUNT + 7U) / 8U ];

