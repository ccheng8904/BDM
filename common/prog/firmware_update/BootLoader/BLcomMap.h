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


#ifndef BLCOMMAP_H
#define BLCOMMAP_H

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
#define BOOTLOADER // needed in huart.h to use TC (transmission complete) flag
#include "huart.h"
#include "hsup.h"
#include "hwdt.h"

#ifndef __PCGCC__

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#if !defined(BP2_UART_CHANNEL)
    #error "no valid definition for UART-Mapping"
#endif

/* Switch off: Note 9024: Usage of #/## operator in macro
               Note 9023: Multiple use of '#/##' operators
               Note 9026: Function-like macro */
/*lint -save -e9023 -e9024 -e9026 */

#define MAP_UART(channel, function)  HUART ## channel ##function
#define BLCOM_MAP_UART(channel, function) MAP_UART(channel, function)
/*lint -restore -e9023 -e9024 -e9026 */




/******************************************************************************/
/* INLINE FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/** Initializes the UART
 *  \note This interface must be called before any other.
 */
static inline void BLCOM_initUart(void)
{
    BLCOM_MAP_UART(BP2_UART_CHANNEL,_vInit(0));
}


/** Enables the UART
 *  \note This interface must be called before any other.
 */
static inline void BLCOM_enableUart(void)
{
    BLCOM_MAP_UART(BP2_UART_CHANNEL,_vEnable());
}


/** sets the UART baud rate
 *
 *  \param baudrate: Baud rate in 100 Bits per second (e.g. 9600 Bd -> uiBaudRate = 96).
 */
static inline void BLCOM_Configure(uint16_t baudrate)
{
  #if !defined(PSOC4)
    (void)BLCOM_MAP_UART(BP2_UART_CHANNEL,_bConfigure(0u,baudrate,HUART_PARITY_EVEN,HUART_STOP_BITS_1));
  #else
    (void)BLCOM_MAP_UART(BP2_UART_CHANNEL,_bConfigure(0u,baudrate,HUART_PARITY_EVEN,HUART_STOP_BITS_1_5));
  #endif
}


/** writes a single byte into transmit register
 *
 *  \param data data byte to transmit
 */
static inline void BLCOM_setTxData(uint8_t data)
{
    BLCOM_MAP_UART(BP2_UART_CHANNEL,_vSetTxData(data));
}


/** returns true is a transmit is in progress
 *
 *  \return true is transmission is in progress, else false
 */
static inline bool BLCOM_isTxOnGoing(void)
{
    return ((BLCOM_MAP_UART(BP2_UART_CHANNEL,_bIsTxEvent())) ? false : true);
}


/** returns whether new data is available or not 
 *
 *  \return true if received data available, else false
 */
static inline bool BLCOM_isDataAvailable(void)
{
    return ((BLCOM_MAP_UART(BP2_UART_CHANNEL,_bIsRxEvent())) ? true : false);
}


/** returns whether receive error happened during last reception or not 
 *
 *  \return true if receive error happened, else false
 */
static inline bool BLCOM_isReceiveError(void)
{
    return ((BLCOM_MAP_UART(BP2_UART_CHANNEL,_ucGetRxError()) == 0U) ? false : true);
}


/** reads a data byte from the receive register
 *
 *  \details this interface returns a received byte after
 *           \sa BLCOM_isDataAvailable returned true.
 * \return last received byte
 */
static inline uint8_t BLCOM_getRxData(void)
{
    #if defined(RX100) || defined(RX200) || defined(RA4) || defined(RA2)
        BLCOM_MAP_UART(BP2_UART_CHANNEL,_vClearRxEvent());
    #endif
    
    return BLCOM_MAP_UART(BP2_UART_CHANNEL,_ucGetRxData());
}


/** Generate the Reset
 */
static inline void BLCOM_generateReset(void)
{
    HSUP_vGenerateReset();
}


/** Trigger the watchdog
 */
static inline void BLCOM_triggerWdg(void)
{
    HWDT_vTrigger();
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

#endif // BLCOMMAP_H
