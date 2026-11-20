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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     dbgx_msg_handlers.c
 *
 *  \ingroup  Debug_extended DBus access
 *
 *  \brief    Rx- and Tx-DBus-Messages
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "mem_utility/ring_buffer.h"
#include "dbgx_msg_handlers.h"
#include "dbgx_msg_lists.h"
#include "dbus/bal.h"
#include "protocol.h"
#include "interface.h"
#include <string.h>


/******************************************************************************/
/* PRIVATE PREPROCESSOR DEFINITIONS                                           */
/******************************************************************************/

#define DBGX_NEW_LINE              '\n'


/******************************************************************************/
/* LOCALS                                                                     */
/******************************************************************************/

static uint8_t *DBGX_TmpDataBuffer; // temporary buffer for Tx Msg responses
static uint8_t DBGX_TmpDataBufferSize;
static bool DBGX_TransferDataToDBus = false;
static uint8_t DBGX_MessageNumber = 0;

static uint8_t *DBGX_Rx_Data = NULL;
static uint8_t DBGX_Rx_MaxDataLen = 0;


/******************************************************************************/
/* FUNCTION PROTOTYPES                                                        */
/******************************************************************************/

void DBGX_WriteDBusDataToBuffer (uint8_t MessageNumber);
void DBGX_TransmitDBusMessage(uint8_t MessageNumber);


/******************************************************************************/
/* TRANSMIT FUNCTIONS                                                         */
/******************************************************************************/

void DBGX_Tx_MsgSendDbgxDataToPc (uint8_t DataLen, uint8_t *Data)
{
    for (uint8_t i = 0; i < DataLen; i++)
    {
        Data[i] = DBGX_TmpDataBuffer[i];
    }
}


/******************************************************************************/
/* CONFIRM FUNCTIONS                                                         */
/******************************************************************************/

void DBGX_Tx_Msg_Confirm (void)
{
    if(RBUF_getBytesToRead(&DBGI_DBusRingBuffer) > (uint16_t)0)
    {
        DBGX_WriteDBusDataToBuffer(DBGX_MessageNumber);
    }
    else
    {
        DBGX_TransferDataToDBus = false;
    }
}


/******************************************************************************/
/* RECEIVE FUNCTIONS                                                          */
/******************************************************************************/

//lint -esym(818,Data) variable Data can not be pointer to const --> signature from DBus*
void DBGX_Rx_MsgSendDbgxDataToController (uint8_t DataLen, uint8_t *Data)
{
    if(DataLen > DBGX_Rx_MaxDataLen)
    {
        DataLen = DBGX_Rx_MaxDataLen;
    }
    (void)memcpy((void *)DBGX_Rx_Data, (void *)Data, DataLen);  //lint !e923 !e920  both casts are ok
    DBGX_Rx_Data[DataLen] = (uint8_t)DBGX_NEW_LINE;
    DataLen ++;

    DBGP_getProtString_CB(DBGX_Rx_Data, DataLen);
}

/******************************************************************************/
/* PUBLIC FUNCTIONS                                                           */
/******************************************************************************/

void DBGX_SetTmpDBusBuffer(uint8_t *TmpBuffer, uint8_t TmpBufferSize)
{
    DBGX_TmpDataBuffer = TmpBuffer;
    DBGX_TmpDataBufferSize = TmpBufferSize;
}

void DBGX_SetRxDataPointer(uint8_t *Data, uint16_t DataLen)
{
    DBGX_Rx_Data = Data;
    DBGX_Rx_MaxDataLen = (uint8_t)(DataLen - sizeof(DBGX_NEW_LINE)); // Subtract 1 to manually add the null character
}

void DBGX_SendOutputDataToDBus (uint8_t MessageNumber)
{
    if(!DBGX_TransferDataToDBus)  // Start transmission, if not yet ongoing
    {
        DBGX_TransferDataToDBus = true;
        DBGX_WriteDBusDataToBuffer(MessageNumber);
    }
}


/******************************************************************************/
/* PRIVAT FUNCTIONS                                                           */
/******************************************************************************/

void DBGX_WriteDBusDataToBuffer (uint8_t MessageNumber)
{
    uint8_t TmpData;
    uint8_t MessageLength;

    DBGX_MessageNumber = MessageNumber;  // Save message number to use it in the confirm function

    for(MessageLength = 0; MessageLength < DBGX_TmpDataBufferSize; MessageLength++)
    {
        (void)RBUF_readData_u8(&DBGI_DBusRingBuffer, &TmpData); // Automatically increments read pointer in ring buffer
        if(TmpData != (uint8_t)'\n')
        {
            DBGX_TmpDataBuffer[MessageLength] = TmpData;
        }
        else
        {
            break;
        }
    }

    // Set dbus message output length
    DBGX_tTransmitObject[DBGX_MessageNumber].ucDataLen = MessageLength;
    DBGX_tTransmitObject[DBGX_MessageNumber].tBusIdentifier.ucMessageLength = MessageLength + 2U;
    DBGX_TransmitDBusMessage(DBGX_MessageNumber);
}


void DBGX_TransmitDBusMessage(uint8_t MessageNumber)
{
#if !defined STS_SIM_TEST_CI
    switch(MessageNumber)
    {
        case 0:
            BAL_vTransmitMessage (DBGX_SUBSYSTEM_ID, DBGX_TXIDX_MSG_SEND_DBGX_DATA_TO_PC);
            break;
        default:
            //@@ToDo: Error handling
            break;
    }
#endif
}
