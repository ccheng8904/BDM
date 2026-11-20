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


#ifndef DBGX_MSG_HANDLERS_H
#define DBGX_MSG_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/** \brief  Confirm DBus messages
 */
extern void DBGX_Tx_Msg_Confirm (void);


/** \brief  Receive DBus message: PC -> Micro-Controller
 *
 *  \param  DataLen  Length of data buffer
 *  \param  Data     Pointer to data buffer
 */
extern void DBGX_Rx_MsgSendDbgxDataToController (uint8_t DataLen, uint8_t *Data);

/** \brief  Send DBus Messages: Micro-Controller -> PC
 *
 *    \param  DataLen  Length of data buffer
 *    \param  Data     Pointer to data buffer
 */
extern void DBGX_Tx_MsgSendDbgxDataToPc (uint8_t DataLen, uint8_t *Data);

/** \brief  Set the temporary DBus buffer
 *
 *  \param  TmpBuffer      Address of the allocated memory for the temporary buffer
 *  \param  TmpBufferSize  Size of the temporary buffer
 */
extern void DBGX_SetTmpDBusBuffer(uint8_t *TmpBuffer, uint8_t TmpBufferSize);

/** \brief  Set the input buffer pointer
 *
 *  \param  Data     Address of the allocated memory for the input buffer
 *  \param  DataLen  Length of the allocated memory for the input buffer
 */
extern void DBGX_SetRxDataPointer(uint8_t *Data, uint16_t DataLen);

/** \brief  Send debug_extended output data via DBus
 *
 *   \param  MessageNumber Identification number for the type of message to be sent
 */
extern void DBGX_SendOutputDataToDBus (uint8_t MessageNumber);

#ifdef __cplusplus
}
#endif

#endif // #ifndef DBGX_MSG_HANDLERS_H

