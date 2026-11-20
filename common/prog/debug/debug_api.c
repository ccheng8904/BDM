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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug
 *
 *  \brief    Public definitions and declarations for unit retarget.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "processor.h"
#include <stdint.h>
#include <stdio.h>
#include "debug/debug_api.h"

#if defined(DEBUG_RETARGET_STDIO) || defined(DEBUG_RETARGET_ASSERT)
  #include "retarget/retarget.h"
#endif

#if defined(DEBUG_INTERFACE_SERIAL) && defined(BSP_HAL)
  #include "huart.h"
#endif

#if defined(DEBUG_INTERFACE_SERIAL) && defined(BSP_MCAL)
  #include "mcal/muart.h"
#endif

#if defined(DEBUG_INTERFACE_STDIO)
  #include "stdio.h"
#endif

#if defined(DEBUG_INTERFACE_RTT)
  #include "debug/RTT/SEGGER_RTT.h"
#endif

/*Fine to switch lint off in this file. Just for debug purposes. NOT for release. For Debug it works fine.
 * Some of the errors result from the fact, that lint does not see, that e.g sprintf is in stdio from the compiler manufacturer.
 * Some of the errors result from the fact, lint does not like the solution with macros for functions.
 * -> How else to do it to make it convenient for users?*/
/*lint -save -e534 -e586 -e718 -e746 -e750 -e786 -e934 -e970 -e9023 -e9024 -e9026 -e9039 -e9049*/
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#if defined(DEBUG_INTERFACE_ITM) && !defined(ITM)
  #error Debug interface (ITM) not available for selected uC. Check debug_api_config.h
#endif

#if defined(DEBUG_INTERFACE_SERIAL) && defined(BSP_HAL)
    #define _DBG_HUART_vInit(huartNum, index)                            HUART ## huartNum ## _vInit(index)
    #define DBG_HUART_vInit(huartNum, index)                             _DBG_HUART_vInit(huartNum, index)
    
    #define _DBG_HUART_bConfigure(huartNum, index, baud, parity, stopb)  HUART ## huartNum ## _bConfigure(index, baud, parity, stopb)
    #define DBG_HUART_bConfigure(huartNum, index, baud, parity, stopb)   _DBG_HUART_bConfigure(huartNum, index, baud, parity, stopb)

    #define _DBG_HUART_vEnable(huartNum)                                 HUART ## huartNum ## _vEnable()
    #define DBG_HUART_vEnable(huartNum)                                  _DBG_HUART_vEnable(huartNum)

    #define _DBG_HUART_vDisable(huartNum)                                HUART ## huartNum ## _vDisable()
    #define DBG_HUART_vDisable(huartNum)                                 _DBG_HUART_vDisable(huartNum)

    #define _DBG_HUART_ucGetRxData(huartNum)                             HUART ## huartNum ## _ucGetRxData()
    #define DBG_HUART_ucGetRxData(huartNum)                              _DBG_HUART_ucGetRxData(huartNum)

    #define _DBG_HUART_bSetTxData(huartNum, data)                        HUART ## huartNum ## _bSetTxData(data)
    #define DBG_HUART_bSetTxData(huartNum, data)                         _DBG_HUART_bSetTxData(huartNum, data)

    #define _DBG_HUART_bIsRxEvent(huartNum)                              HUART ## huartNum ## _bIsRxEvent()
    #define DBG_HUART_bIsRxEvent(huartNum)                               _DBG_HUART_bIsRxEvent(huartNum)

    #define _DBG_HUART_bIsTxEvent(huartNum)                              HUART ## huartNum ## _bIsTxEvent()
    #define DBG_HUART_bIsTxEvent(huartNum)                               _DBG_HUART_bIsTxEvent(huartNum)
#endif


#ifdef DEBUG_CONSOLE_PRETTY
    // Defines for terminal colors (Putty)
    // uncomment other, if needed
    #define COLOR_RESET   "\x1B[0m"
    #define COLOR_RED     "\x1B[31m"
    #define COLOR_GREEN   "\x1B[32m"
    #define COLOR_YELLOW  "\x1B[33m"
    #define COLOR_BLUE    "\x1B[34m"
//    #define COLOR_MAGENTA "\x1B[35m" 
//    #define COLOR_CYAN    "\x1B[36m"
    #define COLOR_WHITE   "\x1B[37m"
#endif


#if defined(GCCARM)
    typedef int dbg_stdio_int;
#else // defined(KEILARM) || defined(ARMCLANG)
    typedef int32_t dbg_stdio_int;
#endif

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static const char *DBG_messageLevelStr[] =
#ifdef DEBUG_CONSOLE_PRETTY
{
    "         ",
    "[" COLOR_WHITE  " INFO " COLOR_RESET "] ",
    "[" COLOR_GREEN  "  OK  " COLOR_RESET "] ",
    "[" COLOR_YELLOW " WARN " COLOR_RESET "] ",
    "[" COLOR_RED    "!ERROR" COLOR_RESET "] ",
    "[" COLOR_BLUE   " MARK " COLOR_RESET "] "
};
#else
{
    "         ",
    "[ INFO ] ",
    "[  OK  ] ",
    "[ WARN ] ",
    "[!ERROR] ",
    "[ MARK ] "
};
#endif


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void DBG_writeString(const char *str)
{
    while(*str != '\0')
    {
        DBG_writeChar(*str++);
    };
}


void DBG_writeInt(int32_t num)
{
    char str[10];
    //itoa(str, num, 10U);
    sprintf(str, "%d", (dbg_stdio_int)num);    // lint doesn't like sprintf because it does not see, that the header is provided with the compiler
    DBG_writeString(str);
}


void DBG_writeIntHex(int32_t num)
{
    char str[10];
    sprintf(str, "%x", (dbg_stdio_int)num);    // lint doesn't like sprintf because it does not see, that the header is provided with the compiler
    DBG_writeString("0x");
    DBG_writeString(str);
}


void DBG_writeMessageLevel(enum DBG_messageLevel msgLvl)
{
    DBG_writeString(DBG_messageLevelStr[(uint8_t)msgLvl]);
}


void DBG_writeEndLine(void)
{
#ifdef DEBUG_ENDLINE_CR_LF
    DBG_writeChar('\r');
#endif
    DBG_writeChar('\n');
}


void DBG_writeMessage(enum DBG_messageLevel msgLvl, const char *str)
{
    DBG_writeMessageLevel(msgLvl);
    DBG_writeString(str);
    DBG_writeEndLine();
}


void DBG_writeMark(const char *file, int32_t line)
{
    DBG_writeMessageLevel(DBG_MARK);
    DBG_writeString(file);
    DBG_writeChar(' ');
    DBG_writeInt(line);
    DBG_writeEndLine();
}

#if defined(DEBUG_INTERFACE_STDIO)

    void DBG_init(void)
    {
        /* Nothing to initialize */
    }

    void DBG_unInit(void)
    {
        /* Nothing to uninitialize */
    }

    void DBG_writeChar(char ch)
    {
        putchar(ch);
        fflush(NULL);
    }

    void DBG_writeStringLen(const char *str, uint16_t len)
    {
        for(uint16_t i = 0; i < len; i++)
        {
            DBG_writeChar(str[i]);
        }
    }

    char DBG_readChar(void)
    {
        int32_t ch;
        do
        {
            ch = getchar();
        }
        while(ch == -1);
        return (char)ch;
    }

#elif defined(DEBUG_INTERFACE_ITM)

    volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;

    void DBG_init(void)
    {
        /* Nothing to initialize */
    }

    void DBG_unInit(void)
    {
        /* Nothing to uninitialize */
    }

    void DBG_writeChar(char ch)
    {
        ITM_SendChar((uint32_t) ch);
    }

    void DBG_writeStringLen(const char *str, uint16_t len)
    {
        for(uint16_t i = 0; i < len; i++)
        {
            DBG_writeChar(str[i]);
        }
    }

    char DBG_readChar(void)
    {
        int32_t ch;
        do
        {
            ch = ITM_ReceiveChar();
        }
        while(ch == -1);
        return (char)ch;
    }

#elif defined(DEBUG_INTERFACE_RTT)

    void DBG_init(void)
    {
        /* Nothing to initialize */
    }

    void DBG_unInit(void)
    {
        /* Nothing to uninitialize */
    }

    void DBG_writeChar(char ch)
    {
        SEGGER_RTT_PutChar(0, ch);
    }

    void DBG_writeStringLen(const char *str, uint16_t len)
    {
        SEGGER_RTT_Write(0, str, len);
    }

    char DBG_readChar(void)
    {
        int32_t ch;
        do
        {
            ch = SEGGER_RTT_GetKey();
        }
        while(ch == -1);
        return (char)ch;
    }

#elif defined(DEBUG_INTERFACE_SERIAL) && defined(BSP_HAL)

    uint8_t DBG_UartCfgIndex = 0;

    void DBG_init(void)
    {
        DBG_HUART_vInit(DEBUG_UART_CHANNEL, 0);
        DBG_HUART_vEnable(DEBUG_UART_CHANNEL);
        DBG_HUART_bConfigure(DEBUG_UART_CHANNEL, DBG_UartCfgIndex, (uint16_t)(DEBUG_UART_BAUTRATE/100), HUART_PARITY_NONE, HUART_STOP_BITS_1);
    }

    void DBG_unInit(void)
    {
        DBG_HUART_vDisable(DEBUG_UART_CHANNEL);
    }

    void DBG_writeChar(char ch)
    {
        while(!DBG_HUART_bIsTxEvent(DEBUG_UART_CHANNEL))
        {
            // Wait to send byte
        }
        DBG_HUART_bSetTxData(DEBUG_UART_CHANNEL, (uint8_t)ch);
    }

    void DBG_writeStringLen(const char *str, uint16_t len)
    {
        for(uint16_t i = 0; i < len; i++)
        {
            DBG_writeChar(str[i]);
        }
    }

    char DBG_readChar(void)
    {
        while(!DBG_HUART_bIsRxEvent(DEBUG_UART_CHANNEL))
        {
            // Wait to receive byte
        }
        return (char)DBG_HUART_ucGetRxData(DEBUG_UART_CHANNEL);
    }

#elif defined(DEBUG_INTERFACE_SERIAL) && defined(BSP_MCAL)
    #error "MCAL NOT SUPPORTED FOR NOW."
#endif


/** \brief    Functions for retargeting standard IO functions to debug interface.
 *
 *  \details  These functions are used to retarget printf and scanf functions
 *            to use debug inteface.
 */
#ifdef DEBUG_RETARGET_STDIO
void RTG_writeChar(char ch)
{
    DBG_writeChar(ch);
}

char RTG_readChar(void)
{
    char ch = DBG_readChar();
#if defined(DEBUG_ECHO_ENABLE)
    DBG_writeChar(ch);
#endif
    return ch;
}
#endif


#ifdef DEBUG_RETARGET_ASSERT
void RTG_assert(const char *expr, const char *file, int32_t line)
{
    DBG_writeMessageLevel(DBG_ERROR);
    DBG_writeString("Assertion failed. expression: \"");
    DBG_writeString(expr);
    DBG_writeString("\" file: ");
    DBG_writeString(file);
    DBG_writeString(" line: ");
    DBG_writeInt(line);
    DBG_writeEndLine();
}
#endif

/*lint -restore -e534 -e586 -e718 -e746 -e750 -e786 -e934 -e970 -e9023 -e9024 -e9026 -e9039 -e9049*/
/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
