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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Definitions and declarations of generic part of the debug_extended
 *            component: Interface layer
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdint.h>
#include "bsh_stdinc.h"
#include "hsup.h"

#include "debug_extended/interface.h"
#include "debug/debug_api.h"
#include "mem_utility/ring_buffer.h"
#include "mcal/mcal_includes.h"
#include "sts_sim_test_uty.h"
#include "dbgx_msg_handlers.h"


/******************************************************************************/
/* PRIVATE PREPROCESSOR DEFINITIONS                                           */
/******************************************************************************/

#define DBGI_UART_FIFO_DIVIDER          (uint8_t)5
#define DBGI_UART_FIFO_DIVIDER_MIN      (uint8_t)2
#define DBGI_UART_TOO_SLOW              "!~ MUART TOO SLOW \n\n"
#define DBGI_MCAL_UART_ERROR            "!~ ERROR IN MCAL / MUART \n\n"

#define DBGI_DBUS_SEND_REGULAR_OUTPUT   (uint8_t)0
#define DBGI_DBUS_TOO_SLOW              "!~ DBUS TOO SLOW \n"


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

struct RBUF_ringBuffer DBGI_UartRingBuffer;
struct RBUF_ringBuffer DBGI_RamRingBuffer;
struct RBUF_ringBuffer DBGI_DBusRingBuffer;

static struct MUART_Handle DBGI_UartHandle;
const struct MUART_Channel *UART_FTDI = NULL;

static uint8_t DBGI_ReceiveInterface = 0;
static uint8_t *DBGI_Rx_Data = NULL;


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

void DBGI_MuartHandleCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse);

/******************************************************************************/
/* PRIVATE DATA DECLARATIONS                                                  */
/******************************************************************************/

static MCAL_Callback_t DBGI_MuartHandleCallback;
static uint16_t        DBGI_UartMaxBytesToSend  = 1;
static uint16_t        DBGI_UartBytesToSend     = 1;
static bool            DBGI_UartStatusCbActive  = false;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

int32_t DBGI_SendInterfaceBytes(uint8_t InterfaceIndex, const uint8_t *Bytes, uint16_t Len)
{
    const uint8_t Newline = (uint8_t)'\n';

    switch(InterfaceIndex){
        case DBGI_INTERFACE_NONE:
            break;

        case DBGI_INTERFACE_DBGX_UART:
            HSUP_vDisableInt();

            // Check, if there is enough space in the ring buffer, for the new byte string
            if(RBUF_getFreeSpace(&DBGI_UartRingBuffer) < (Len + (uint16_t)sizeof(Newline)))
            {
                // Create a gap, between new and old data, to ensure the UART data to be sent (right now) are not overwritten
                DBGI_UartRingBuffer.write = (DBGI_UartRingBuffer.read + DBGI_UartBytesToSend) % DBGI_UartRingBuffer.size;
                (void)RBUF_writeBuff(&DBGI_UartRingBuffer, (const uint8_t *) DBGI_UART_TOO_SLOW, (uint8_t)sizeof(DBGI_UART_TOO_SLOW));
            }

            (void)RBUF_writeBuff(&DBGI_UartRingBuffer, Bytes, Len);
            (void)RBUF_writeBuff(&DBGI_UartRingBuffer, &Newline, (uint8_t)sizeof(Newline));

            if(!DBGI_UartStatusCbActive)
            {
                DBGI_UartStatusCbActive = true;
#if !defined STS_SIM_TEST_CI
                (void)MUART_write(&DBGI_UartHandle, &Newline, (uint8_t)sizeof(Newline));  // Start MUART callback
#endif
            }
            HSUP_vEnableInt();

            break;

        case DBGI_INTERFACE_DBUS:
            HSUP_vDisableInt();

            if(RBUF_getFreeSpace(&DBGI_DBusRingBuffer) < (Len + (uint16_t)sizeof(Newline)))
            {
                DBGI_DBusRingBuffer.read = DBGI_DBusRingBuffer.write;  // Reset DBus ring buffer manually
                (void)RBUF_writeBuff(&DBGI_DBusRingBuffer, (const uint8_t *)DBGI_DBUS_TOO_SLOW, (uint8_t)sizeof(DBGI_DBUS_TOO_SLOW));
            }

            (void)RBUF_writeBuff(&DBGI_DBusRingBuffer, Bytes, Len);
            (void)RBUF_writeBuff(&DBGI_DBusRingBuffer, &Newline, (uint8_t)sizeof(Newline));

            DBGX_SendOutputDataToDBus(DBGI_DBUS_SEND_REGULAR_OUTPUT);
            HSUP_vEnableInt();

            break;

        case DBGI_INTERFACE_RAM:
            #ifndef RBUF_WRITE_REENTRANT  // If the mem_util component is not reentrant, then implement it here
                HSUP_vDisableInt();
            #endif

            (void)RBUF_writeBuff(&DBGI_RamRingBuffer, Bytes, Len);
            (void)RBUF_writeBuff(&DBGI_RamRingBuffer, &Newline, (uint8_t)sizeof(Newline));

            // Adjust the read pointer manually. The data will not be read out by any function
            DBGI_RamRingBuffer.read = DBGI_RamRingBuffer.write;

            #ifndef RBUF_WRITE_REENTRANT
                HSUP_vEnableInt();
            #endif

            break;

        case DBGI_INTERFACE_SIMPLE_DEBUG:
            HSUP_vDisableInt();
            DBG_writeStringLen((const char*)Bytes, Len); //lint !e926 cast tested ok
            DBG_writeChar('\r');
            DBG_writeChar('\n');
            HSUP_vEnableInt();

            break;

        default:
            /* No interface is selected. Therefore do nothing */
            break;
    }

    STS_logStringAndU8ArrayHex("--- DBGI_SendInterfaceBytes: Bytes: ", Bytes, Len);
    STS_logStringAndU8Hex("--- DBGI_SendInterfaceBytes: InterfaceIndex: ", InterfaceIndex);

    return DBGI_STAT_NO_ERROR;
}

void DBGI_SetRingBuffer(struct RBUF_ringBuffer *RingBuffer, uint8_t *Buffer, uint16_t BufferSize)
{
    RBUF_init(RingBuffer, Buffer, BufferSize);
}

void DBGI_SetUartConfig(const struct MUART_Channel *Channel, uint16_t Baudrate)
{
#if !defined STS_SIM_TEST_CI
    if(UART_FTDI == NULL)
    {
        UART_FTDI = Channel;
        struct MUART_Config muartCfg = MUART_CFG_8N1(Baudrate);

        // Set the number of bytes to be sent via the UART in a single UART_write command
        if(DBGI_UartRingBuffer.size > (uint16_t)1000)
        {
            DBGI_UartMaxBytesToSend = DBGI_UartRingBuffer.size / DBGI_UART_FIFO_DIVIDER;
        }
        else
        {
            DBGI_UartMaxBytesToSend = DBGI_UartRingBuffer.size / DBGI_UART_FIFO_DIVIDER_MIN;
        }

        (void)MUART_init(&DBGI_UartHandle, UART_FTDI, &muartCfg);

        MCAL_initCallback(&DBGI_MuartHandleCallback, DBGI_MuartHandleCallbackFunction, (void*)&DBGI_UartHandle);
        (void)MUART_enableEvent(&DBGI_UartHandle, &DBGI_MuartHandleCallback, MCAL_EVENT_ERROR);
        (void)MUART_enableEvent(&DBGI_UartHandle, &DBGI_MuartHandleCallback, MCAL_EVENT_WRITE_COMPLETE);
        (void)MUART_enableEvent(&DBGI_UartHandle, &DBGI_MuartHandleCallback, MCAL_EVENT_READ_BYTE);

        MUART_enable(&DBGI_UartHandle);
    }
#endif
}

void DBGI_SetReceiveInterface(uint8_t ReceiveInterface)
{
    DBGI_ReceiveInterface = ReceiveInterface;
}

int32_t DBGI_GetInterfaceBytes(char *String, uint16_t MaxLen)
{
    int32_t LenOfReadString = -1;

    switch(DBGI_ReceiveInterface){
        case DBGI_INTERFACE_NONE:
            // No input interface selected
            break;

        case DBGI_INTERFACE_DBGX_UART:
            (void)MUART_read(&DBGI_UartHandle, (uint8_t *)String, MaxLen);  /*lint !e926 Type is OK here */
            DBGI_Rx_Data = (uint8_t *)String;  /*lint !e926 Type is OK here */
            LenOfReadString = -2;
            break;

        case DBGI_INTERFACE_DBUS:
            DBGX_SetRxDataPointer((uint8_t *)String, MaxLen);  /*lint !e926 Type is OK here */
            LenOfReadString = -2;
            break;

        case DBGI_INTERFACE_RAM:
            // No input possibility planned
            break;

        case DBGI_INTERFACE_SIMPLE_DEBUG:
            for(uint16_t index = 0; index < MaxLen; index++)
            {
                String[index] = DBG_readChar();
                if(String[index] == (char)0x0A)
                {
                    LenOfReadString = (int32_t)index;
                    break;
                }
            }
            break;

        default:
            /* Wrong interface index - do nothing */
            break;
    }
    return LenOfReadString;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief  Handles the interrupts for the UART interface
 *  \note   This function is called by the MCAL/MUART after an event has occurred
 *
 *  \param  objPtr         Pointer to the UART object
 *  \param  eventFlags     Event flag of the UART event
 *  \param  eventResponse  Event return value of the UART event
 */
//lint -esym(818,objPtr) variable objPtr can not be pointer to const --> MCAL_initCallback requires void*
void DBGI_MuartHandleCallbackFunction(void *objPtr, uint32_t eventFlags, const struct MCAL_EventResponse *eventResponse)
{
#if !defined STS_SIM_TEST_CI
    (void)objPtr; // Intentionally not used
    static uint16_t DBGI_LenOfReadString = 0;

    switch(eventFlags)
    {
        case (uint32_t)MCAL_EVENT_ERROR:
            DBGX_handleErrWithParam_Gen(DBGI_ERR_MUART_EVENT_ERROR, (uint32_t)MCAL_getEventType(eventResponse));
            (void)MUART_write(&DBGI_UartHandle, (const uint8_t*) &DBGI_MCAL_UART_ERROR, (uint16_t)sizeof(DBGI_MCAL_UART_ERROR)); //lint !e928 cast tested ok
            break;

        case (uint32_t)MCAL_EVENT_WRITE_COMPLETE:
            // Adapt the read pointer, for the data that was transmitted just now
            DBGI_UartRingBuffer.read = (DBGI_UartRingBuffer.read + DBGI_UartBytesToSend) % DBGI_UartRingBuffer.size;

            DBGI_UartBytesToSend = (uint16_t)RBUF_getBytesToRead(&DBGI_UartRingBuffer);

            // Limit the bytes to be sent
            if(DBGI_UartBytesToSend > DBGI_UartMaxBytesToSend)
            {
                DBGI_UartBytesToSend = DBGI_UartMaxBytesToSend;
            }

            // If the ring buffer end is reached, just write the data until the linear buffer end
            // Do not create a ring here, to start at the buffer beginning
            if(DBGI_UartBytesToSend > (DBGI_UartRingBuffer.size - DBGI_UartRingBuffer.read))
            {
                DBGI_UartBytesToSend = DBGI_UartRingBuffer.size - DBGI_UartRingBuffer.read;
            }

            if(DBGI_UartBytesToSend > (uint16_t)0)
            {
                (void)MUART_write(&DBGI_UartHandle, &DBGI_UartRingBuffer.buffer[DBGI_UartRingBuffer.read], DBGI_UartBytesToSend);
            }
            else
            {
                DBGI_UartStatusCbActive = false;
            }
            break;

        case (uint32_t)MCAL_EVENT_READ_BYTE:
            DBGI_LenOfReadString ++;
            if((uint8_t)'\r' == MCAL_getEventData_uint8(eventResponse))
            {
                MUART_finishRead(&DBGI_UartHandle);
                DBGP_getProtString_CB(DBGI_Rx_Data, DBGI_LenOfReadString);
                DBGI_LenOfReadString = 0;
            }
            break;

        default:
            // Do not transmit in case of any other event
            break;
    }
#endif
}

