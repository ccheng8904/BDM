/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


#ifndef BLCOM_H
#define BLCOM_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \ingroup  firmware_update
 *
 *  \brief    Public declarations of Boot-Loader Communication
 *
 *  \details  This unit is responsible for the Boot-Loader communication.
 *            It provides interfaces for configuration, sending and receiving.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "stdbool.h"


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** \brief  enumeration for receive result. */
enum BLCOM_rxResult
{
    BLCOM_rxSuccess, ///< successful reception
    BLCOM_rxError,   ///< receiver error or checksum wrong
    BLCOM_rxTimeOut  ///< time-out occurred
};


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** initializes the communication unit
 *
 * \note must be called before any other interface.
 */
void BLCOM_init(void);


/** sends a single byte and waits until transmission complete
 *
 * \param data: data byte to send
 * \note this function triggers the watchdog.
 */
void BLCOM_sendByte(uint8_t data);


/** sends an acknowledge
 *
 *  \details For ROM-Loader an acknowledge will net be send before any
 *           successful receive happened.
 *
 * \param ack: acknowledge to be send
 * \note this function triggers the watchdog.
 */
void BLCOM_sendAck(uint8_t ack);


/** sends a buffer of bytes
 *
 *  \param buffer: pointer to the data buffer to send.
 *  \param length: number of bytes to send.
 *  \note this function triggers the watchdog.
 */
void BLCOM_sendBuffer(const uint8_t *buffer, uint16_t length);


/** \brief    receive a frame of bytes including checksum.
 *
 *  \param    buffer:  pointer of buffer to save received data
 *  \param    length:  number of bytes to be received (not including checksum)
 *  \param    timeout: receive time-out in micro seconds
 *
 *  \note  caller is responsible, that buffer is big enough for at least length
 *         bytes.
 *          
 *  \return   either BLCOM_rxSuccess, BLCOM_rxError, BLCOM_rxTimeOut
 */
enum BLCOM_rxResult BLCOM_receiveFrame(uint8_t *buffer, uint16_t length, uint16_t timeout);


/** Configure the baud rate
 *
 *  \param baudrate: Baud rate in 100 Bits per second (e.g. 9600 Bd -> uiBaudRate = 96).
 */
void BLCOM_setBaudrate(uint16_t baudrate);


#ifdef __cplusplus
}
#endif

#endif // BLCOM_H
