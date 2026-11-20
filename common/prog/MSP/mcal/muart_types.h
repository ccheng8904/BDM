/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MUART
 ******************************************************************************/


#ifndef MUART_TYPES_H
#define MUART_TYPES_H

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
 *  \brief    Public definitions and type definitions for unit MUART.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/** MUART default configuration structure definitions
 *  \note These definitions are used for initialization of MUART configuration
 *        structures.
 */
/*lint -e9026 "Function-like macro [MISRA 2012 Directive 4.9, advisory]" */
#define MUART_CFG_8N1(baud)     {MUART_DATA_BITS_8, \
                                 MUART_PARITY_NONE, \
                                 MUART_STOP_BITS_1, \
                                 (baud)}

#define MUART_CFG_8N1_9600      MUART_CFG_8N1(9600u)
#define MUART_CFG_8N1_19200     MUART_CFG_8N1(19200u)
#define MUART_CFG_8N1_57600     MUART_CFG_8N1(57600u)


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** MUART data bits width configuration enumeration */
enum MUART_DataBits
{
    MUART_DATA_BITS_7,          ///< 7-bit data width
    MUART_DATA_BITS_8,          ///< 8-bit data width (default)
    MUART_DATA_BITS_9           ///< 9-bit data width - MiKo: Is this necessary?
};


/** MUART stop bits configuration enumeration */
enum MUART_StopBits
{
    MUART_STOP_BITS_0_5,        ///< 0.5 stop bits
    MUART_STOP_BITS_1,          ///< 1 stop bit (default)
    MUART_STOP_BITS_1_5,        ///< 1.5 stop bits
    MUART_STOP_BITS_2           ///< 2 stop bits
};


/** MUART parity configuration enumeration */
enum MUART_Parity
{
    MUART_PARITY_NONE,          ///< No parity (default)
    MUART_PARITY_ODD,           ///< Odd parity
    MUART_PARITY_EVEN           ///< Even parity
};


/** MUART generic configuration structure */
struct MUART_Config
{
    enum MUART_DataBits     dataBits;   ///< Number of data bits
    enum MUART_Parity       parity;     ///< Parity configuration
    enum MUART_StopBits     stopBits;   ///< Stop bits configuration
    uint32_t                baudrate;   ///< Baud rate configuration
};


#ifdef __cplusplus
}
#endif

#endif // MUART_TYPES_H
