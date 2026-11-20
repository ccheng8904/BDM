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
 *  COMP_ABBREV      DBG
 ******************************************************************************/


#ifndef DEBUG_API_H
#define DEBUG_API_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug
 *
 *  \brief    Public definitions and declarations for unit debug.
 *
 *  \details  Example for custom messages with info level and stringized number:
 *                DBG_writeMessageLevel(DBG_INFO);
 *                DBG_writeString("Number n is: ");
 *                DBG_writeInt(n);
 *                DBG_writeEndLine();
 *
 *            Alternative using sprintf:
 *                char str[30];
 *                sprintf(str, "Number n is: %d", n);
 *                DBG_writeMessage(DBG_INFO, str);
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "debug/debug_api_config.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#ifndef NDEBUG
    #define DBG_setMark()   {DBG_writeMark(__FILE__, __LINE__);}
#else
    #define DBG_setMark()   do {} while (0)
#endif

#if   defined(DEBUG_INTERFACE_SERIAL) && defined(BSP_HAL)
extern uint8_t DBG_UartCfgIndex;//!< Indicates, which entry in HUARTX_atConfig to use, where X is DEBUG_INTERFACE_SERIAL.
#endif
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum DBG_messageLevel
{
    DBG_PRINT   = 0,
    DBG_INFO    = 1,
    DBG_OK      = 2,
    DBG_WARNING = 3,
    DBG_ERROR   = 4,
    DBG_MARK    = 5
};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

/** \brief    Function for initialization of debug interface.
 *
 *  \details  This function initializes debug interface, which can be ITM, or
 *            serial (UART) interface.
 *  \details  In case of debugging via UART with HAL: Remember to set DBG_UartCfgIndex to the correct value, before calling this function.
 */
extern void DBG_init(void);

/** \brief    Function for uninitialization of debug interface.
 *
 *  \details  This function uninitializes debug interface, which can be ITM, or
 *            serial (UART) interface.
 *  \detail   In case of debugging via UART with HAL: Uninit and Init again, if you changed the core clock of your controller.
 *  \detail   ITM and UART with MCAL: No need to use the function.
 */
extern void DBG_unInit(void);

/** \brief    Function for sending one character via debug interface.
 *
 *  \details  This function sends one character trough debug interface. If the
 *            interface is bussy, function waits until last character is sent.
 *
 *  \param    ch: character to be sent.
 */
extern void DBG_writeChar(char ch);


/** \brief    Function for receiving one character from debug interface.
 *
 *  \details  This function reads one character from debug interface. If input
 *            buffer is empty, function waits for character to be received.
 */
extern char DBG_readChar(void);


/** \brief    Function for sending debug message over debug interface.
 *
 *  \details  This function writes message level string, message and new line
 * 
 *  \param    msgLvl: message level which will be printed before actual message
 *  \param    str:    message string
 */
extern void DBG_writeMessage(enum DBG_messageLevel msgLvl, const char *str);


/** \brief    Function for sending message level string over debug interface.
 *
 *  \param    msgLvl: message enumeration type
 */
extern void DBG_writeMessageLevel(enum DBG_messageLevel msgLvl);


/** \brief    Function for sending string over debug interface.
 *
 *  \param    str: string to be sent, null termination required
 */
extern void DBG_writeString(const char *str);

/** \brief    Function for sending string over debug interface.
 *
 *  \param    str: string to be sent
 *  \param    len: length of the string to be sent
 */
extern void DBG_writeStringLen(const char *str, uint16_t len);

/** \brief    Function for sending line ending over debug interface.
 *
 *  \param    num: number to be sent
 */
extern void DBG_writeEndLine(void);


/** \brief    Functions for sending stringized number over debug interface.
 *
 *  \param    num: number to be sent
 *  \param    base: base for stringized number
 */
extern void DBG_writeNumber(int32_t num, unsigned char base);   // TODO implementation
extern void DBG_writeInt(int32_t num);      ///< base 10
extern void DBG_writeIntHex(int32_t num);   ///< base 16


/** \brief    Functions for writing mark to debug interface.
 *
 *  \param    file: pointer to file string where is mark located 
 *  \param    line: line number of mark
 */
extern void DBG_writeMark(const char *file, int32_t line);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_API_H
