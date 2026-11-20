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
 *  COMP_ABBREV      MCOM
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MCOM
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "mcom.h"

#ifdef MCAL_MCOM_NDEBUG
    #define NDEBUG
#endif
#include "mcal_assert.h"


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void MCOM_pushByte(struct MCOM_Transfer *transfer, uint8_t data)
{
    MCAL_assert(transfer);
    struct MCOM_BufferHandle *bufferHandle = &transfer->readBufferHandle;

    if(bufferHandle->index < bufferHandle->length)
    {
        bufferHandle->buffer.readBuffer[bufferHandle->index] = data;
        bufferHandle->index +=1u; /* ++ creates axivion issue due to volatile declaration */
    }
    if(bufferHandle->index >= bufferHandle->length)
    {
        bufferHandle->status = MCOM_STATUS_COMPLETE;
    }
}


uint8_t MCOM_popByte(struct MCOM_Transfer *transfer)
{
    MCAL_assert(transfer);
    struct MCOM_BufferHandle *bufferHandle = &transfer->writeBufferHandle;
    uint8_t data = 0;

    if(bufferHandle->index < bufferHandle->length)
    {
        data = bufferHandle->buffer.writeBuffer[bufferHandle->index];
        bufferHandle->index +=1u; /* ++ creates axivion issue due to volatile declaration */
    }
    if(bufferHandle->index >= bufferHandle->length)
    {
        bufferHandle->status = MCOM_STATUS_COMPLETE;
    }
    
    return data;
}


void MCOM_setWriteBuffer(struct MCOM_Transfer *transfer,
                         const uint8_t *buffer, uint16_t length)
{
    MCAL_assert(transfer != NULL);
    MCAL_assert(buffer != NULL);
    
    if(transfer != NULL)
    {
        transfer->writeBufferHandle.status = MCOM_STATUS_BUSY;
        transfer->writeBufferHandle.buffer.writeBuffer = buffer;
        transfer->writeBufferHandle.length = length;
        transfer->writeBufferHandle.index = 0u;
    }
}


void MCOM_setReadBuffer(struct MCOM_Transfer *transfer,
                        uint8_t *buffer, uint16_t length)
{
    MCAL_assert(transfer != NULL);
    MCAL_assert(buffer != NULL);
    
    if(transfer != NULL)
    {
        transfer->readBufferHandle.status = MCOM_STATUS_BUSY;
        transfer->readBufferHandle.buffer.readBuffer = buffer;
        transfer->readBufferHandle.length = length;
        transfer->readBufferHandle.index = 0u;
    }
}

