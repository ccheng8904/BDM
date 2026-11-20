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
 *  COMP_ABBREV      DBGI
 ******************************************************************************/

#ifndef DEBUG_EXTENDED_INTERFACE_H
#define DEBUG_EXTENDED_INTERFACE_H

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Public definitions and declarations of generic part of the
 *            debug_extended component: Interface layer
 *
 *  \details  
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include "api.h"
#include "mcal_channels.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* PUBLIC PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/

/* Interfaces: */
#define DBGI_INTERFACE_NONE          UINT8_C(0x00)  // Explicit type cast is intentially not used. Proprocessor would have problems
#define DBGI_INTERFACE_DBGX_UART     UINT8_C(0x01)  // Explicit type cast is intentially not used. Proprocessor would have problems
#define DBGI_INTERFACE_DBUS          UINT8_C(0x02)  // Explicit type cast is intentially not used. Proprocessor would have problems
#define DBGI_INTERFACE_RAM           UINT8_C(0x03)  // Explicit type cast is intentially not used. Proprocessor would have problems
#define DBGI_INTERFACE_SIMPLE_DEBUG  UINT8_C(0x04)  // Explicit type cast is intentially not used. Proprocessor would have problems

/* Return stati: */
#define DBGI_STAT_NO_ERROR        ((int32_t)0)
#define DBGI_STAT_ERROR           ((int32_t)1)

/***************************
   * Unique Error Codes   */

#define DBGI_ERR_MUART_EVENT_ERROR     ((uint32_t)0x00000000 | DBGX_ERR_SOLVING_NONE)

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

extern struct RBUF_ringBuffer DBGI_UartRingBuffer;
extern struct RBUF_ringBuffer DBGI_RamRingBuffer;
extern struct RBUF_ringBuffer DBGI_DBusRingBuffer;

extern const struct MUART_Channel *UART_FTDI;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief  Sends the debug_extended data to the debug_extended interface input buffer
 *
 *  \param  InterfaceIndex  Index of the configured interface
 *  \param  Bytes           Pointer to the array of debug_extended data payload bytes to be sent
 *  \param  Len             Number of debug_extended data payload bytes to be sent
 *  \return Status: >= 0: number of bytes sent; < 0: Error
 */
extern int32_t DBGI_SendInterfaceBytes(uint8_t InterfaceIndex, uint8_t const *Bytes, uint16_t Len);

/** \brief  Set the configuration for the ring buffer
 *
 *  \param  RingBuffer  Address of the allocated memory of the ring buffer structure
 *  \param  Buffer      Address of the allocated memory of the data buffer of the ring buffer
 *  \param  BufferSize  Size of the buffer
 */
extern void DBGI_SetRingBuffer(struct RBUF_ringBuffer *RingBuffer, uint8_t *Buffer, uint16_t BufferSize);

/** \brief  Set the configuration for the UART interface
 *
 *  \param  Channel   Channel configuration for the MUART (e.g. &MUART3_RX_PB11_TX_PB10)
 *  \param  Baudrate  Baudrate for the UART interface
 */
extern void DBGI_SetUartConfig(const struct MUART_Channel *Channel, uint16_t Baudrate);

/** \brief  Set the interface for the input data
 *
 *  \param  ReceiveInterface  Interface, which should be used for input data
 */
extern void DBGI_SetReceiveInterface(uint8_t ReceiveInterface);

/** \brief  Read the input data from the interface into the user´s data buffer
 *  \note   This function controls if the input data is read (non-)cooperative
 *
 *  \param  String  Address of the data buffer, to store the input data
 *  \param  MaxLen  Length of the input string
 *                  -1  if there is no valid interface selected
 *                  -2  if the read process in handled cooperative (callback mechanism)
 */
extern int32_t DBGI_GetInterfaceBytes(char *String, uint16_t MaxLen);

#ifdef __cplusplus
}
#endif

#endif   /* From: ifndef DEBUG_EXTENDED_INTERFACE_H */


