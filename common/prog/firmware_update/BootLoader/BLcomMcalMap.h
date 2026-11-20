/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


#ifndef BLCOM_MCALMAP_H
#define BLCOM_MCALMAP_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    maps the uart functions for BLcom 
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "mcal_channels.h"

#ifndef __PCGCC__
#ifndef DBUSCAN_INCLUDED

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
extern const struct MUART_Config MuartHandleConfig;


/******************************************************************************/
/* INLINE FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/** Initializes the UART
 *  \note This interface must be called before any other.
 */
static inline void BLCOM_initUart(void)
{
    const struct MUART_Channel *channel = &BP2_MUART_CHANNEL;
    (void)MUART_configure(channel, &MuartHandleConfig);
    (void)MUART_configurePins(channel->rxPin, channel->txPin);
}


/** Enables the UART
 *  \note This interface must be called before any other.
 */
static inline void BLCOM_enableUart(void)
{
    MUART_enableChannel(&BP2_MUART_CHANNEL);
}


/** sets the UART baud rate
 *
 *  \param baudrate: Baud rate in 100 Bits per second (e.g. 9600 Bd -> uiBaudRate = 96).
 */
static inline void BLCOM_Configure(uint16_t baudrate)
{
    MUART_configureBaudrate(&BP2_MUART_CHANNEL, baudrate*100uL);
}


/** writes a single byte into transmit register
 *
 *  \param data data byte to transmit
 */
static inline void BLCOM_setTxData(uint8_t data)
{
    MUART_writeDataReg(&BP2_MUART_CHANNEL, (uint32_t)data);
}


/** returns true is a transmit is in progress
 *
 *  \return true is transmission is in progress, else false
 */
static inline bool BLCOM_isTxOnGoing(void)
{
    return !(MUART_isEvent(&BP2_MUART_CHANNEL, MCAL_EVENT_WRITE_BYTE));
}


/** returns whether new data is available or not 
 *
 *  \return true if received data available, else false
 */
static inline bool BLCOM_isDataAvailable(void)
{
    return MUART_isEvent(&BP2_MUART_CHANNEL, MCAL_EVENT_READ_BYTE);
}


/** returns whether receive error happened during last reception or not 
 *
 *  \return true if receive error happened, else false
 */
static inline bool BLCOM_isReceiveError(void)
{
    return ((MCAL_OK != MUART_getChannelError(&BP2_MUART_CHANNEL)) ? true : false);
}


/** reads a data byte from the receive register
 *
 *  \details this interface returns a received byte after
 *           \sa BLCOM_isDataAvailable returned true.
 * \return last received byte
 */
static inline uint8_t BLCOM_getRxData(void)
{
    return (uint8_t)MUART_readDataReg(&BP2_MUART_CHANNEL);
}


#endif /* DBUSCAN_INCLUDED */


/** Generate the Reset
 */
static inline void BLCOM_generateReset(void)
{
    MSUP_generateReset();
}


/** Trigger the watchdog
 */
static inline void BLCOM_triggerWdg(void)
{
    MWDT_trigger();
}


/** Initializes local configuration structure for watchdog
 */
static inline void BLCOM_initWdgCfgStruct(void)
{
    MWDT_initConfigStruct();
}


#else // __PCGCC__

extern void BLCOM_initUart(void);
extern void BLCOM_enableUart(void);
extern void BLCOM_Configure(uint16_t baudrate);
extern void BLCOM_setTxData(uint8_t data);
extern bool BLCOM_isTxOnGoing(void);
extern bool BLCOM_isDataAvailable(void);
extern bool BLCOM_isReceiveError(void);
extern uint8_t BLCOM_getRxData(void);
extern void BLCOM_generateReset(void);
extern void BLCOM_triggerWdg(void);

#endif // __PCGCC__


#ifdef __cplusplus
}
#endif

#endif // BLCOM_MCALMAP_H
