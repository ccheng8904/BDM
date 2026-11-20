/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB_HAL_INTEL
 *  COMP_ABBREV      HUART
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Public definitions and declarations for unit huart.
*
*/
#ifndef HUART_H
#define HUART_H

#ifdef __cplusplus
static inline "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#define HUART_RX_FULL   (0u)
#define HUART_RX_IDLE   (0u)
#define HUART_RX_BREAK  (0u)
#define HUART_TX_EMPTY  (0u)

#define HUART_NOISE_ERROR      (0u)
#define HUART_FRAMING_ERROR    (0u)
#define HUART_PARITY_ERROR     (0u)
#define HUART_RX_OVERRUN_ERROR (0u)


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/**  This enumeration type provides list of possible parity configurations to be 
*    used when filling HUART<unit>_atConfig structure.
*/
typedef enum huart_parity_enum
{
    HUART_PARITY_NONE = 2,
    HUART_PARITY_EVEN = 0,
    HUART_PARITY_ODD  = 1
} T_HUART_Parity;

/**  This enumeration type provides list of possible stop bit configurations to be 
*    used when filling HUART<unit>_atConfig structure.
*/
typedef enum huart_stop_bits_enum
{
   HUART_STOP_BITS_1   = 0,  //!< 1 stop bit
   HUART_STOP_BITS_0_5 = 1,  //!< 0.5 stop bit
   HUART_STOP_BITS_2   = 2,  //!< 2 stop bits
   HUART_STOP_BITS_1_5 = 3   //!< 1.5 stop bit
} T_HUART_StopBits;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

static inline void HUART0_vInit(uint8_t ucIndex)
{
    (void)ucIndex;
}

static inline  void HUART0_vEnable(void)
{
    
}

static inline  void HUART0_vDisable(void)
{
    
}

static inline bool HUART0_bSetTxData(uint8_t ucTxByte)
{
    (void)ucTxByte;
    return true;
}

static inline void HUART0_vSetTxData(uint8_t ucTxByte)
{
    (void)ucTxByte;
}

static inline void HUART0_vHandleTxEvent(void)
{
    
}

static inline void HUART0_vEnableTxEvent(void)
{
    
}

static inline void HUART0_vDisableTxEvent(void)
{
    
}

static inline bool HUART0_bIsTxEvent(void)
{
    return false;
}

static inline uint8_t HUART0_ucGetRxData(void)
{
    return 0u;
}

static inline void HUART0_vHandleRxEvent(void)
{
    
}

static inline void HUART0_vEnableRxEvent(void) 
{
    
}

static inline void HUART0_vDisableRxEvent(void)
{
    
}

static inline bool HUART0_bIsRxEvent(void)
{
    return false;
}

static inline bool HUART0_bIsTxOnGoing(void)
{
    return false;
}

static inline uint8_t HUART0_ucGetStatus(void)
{
    return 0u;
}

static inline uint8_t HUART0_ucGetRxError(void)
{
    return 0u;
}

static inline bool HUART0_bConfigure(uint8_t ucIndex, uint16_t uiBaudRate, T_HUART_Parity tParity, T_HUART_StopBits tStopBits)
{
    return true;
}

static inline bool HUART0_bIsRxOnGoing(void)
{
    return false;
}


#ifdef __cplusplus
}
#endif

#endif // HUART_H
