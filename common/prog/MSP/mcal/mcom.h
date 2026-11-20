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


#ifndef MCOM_H
#define MCOM_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MCOM.
 *
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

/*lint -save -e9018 union declared */
/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** MCOM status enumeration */
enum MCOM_Status
{
    MCOM_STATUS_READY = 0,          ///< Buffer is ready
    MCOM_STATUS_BUSY,               ///< Read/write is ongoing
    MCOM_STATUS_COMPLETE            ///< Read/write is complete
    // MCOM_STATUS_ABORTED,
    // MCOM_STATUS_ERROR
};

/*lint -esym(9018,*MCOM_buffer_u*) "The union keyword should not be used. MisraC2012-19.2 advisory" */
/** MCOM read/write buffer structure */
struct MCOM_BufferHandle
{
    volatile enum MCOM_Status   status;     ///< Status of buffer
    union MCOM_buffer_u
    {
        uint8_t                 *readBuffer;    ///< Read/Write buffer pointer
        const uint8_t           *writeBuffer;    ///< Read/Write buffer pointer
    } buffer;
    uint16_t                    length;     ///< Buffer length
    volatile uint16_t           index;      ///< Buffer index
};


/** MCOM write buffer structure */
/*lint -esym(9045,*MCOM_Transfer)  "non-hidden definition of type struct MCOM_Transfer" */
struct MCOM_Transfer
{
    struct MCOM_BufferHandle    readBufferHandle;
    struct MCOM_BufferHandle    writeBufferHandle;
};

/*lint -restore -e9018 */
/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

extern void MCOM_pushByte(struct MCOM_Transfer *transfer, uint8_t data);


extern uint8_t MCOM_popByte(struct MCOM_Transfer *transfer);


extern void MCOM_setWriteBuffer(struct MCOM_Transfer *transfer, const uint8_t *buffer, uint16_t length);


extern void MCOM_setReadBuffer(struct MCOM_Transfer *transfer, uint8_t *buffer, uint16_t length);

#ifndef MCOM_MOCK_H

static inline bool MCOM_isReadReady(const struct MCOM_Transfer *transfer)
{
    return (transfer->readBufferHandle.status == MCOM_STATUS_READY);
}


static inline bool MCOM_isWriteReady(const struct MCOM_Transfer *transfer)
{
    return (transfer->writeBufferHandle.status == MCOM_STATUS_READY);
}


static inline bool MCOM_isReadBusy(const struct MCOM_Transfer *transfer)
{
    return (transfer->readBufferHandle.status == MCOM_STATUS_BUSY);
}


static inline bool MCOM_isWriteBusy(const struct MCOM_Transfer *transfer)
{
    return (transfer->writeBufferHandle.status == MCOM_STATUS_BUSY);
}


static inline bool MCOM_isBusy(const struct MCOM_Transfer *transfer)
{
    return (MCOM_isReadBusy(transfer) || MCOM_isWriteBusy(transfer));
}


static inline bool MCOM_isReadComplete(const struct MCOM_Transfer *transfer)
{
    return (transfer->readBufferHandle.status == MCOM_STATUS_COMPLETE);
}


static inline bool MCOM_isWriteComplete(const struct MCOM_Transfer *transfer)
{
    return (transfer->writeBufferHandle.status == MCOM_STATUS_COMPLETE);
}


static inline void MCOM_setReadComplete(struct MCOM_Transfer *transfer)
{
    transfer->readBufferHandle.status = MCOM_STATUS_COMPLETE;
}


static inline void MCOM_setWriteComplete(struct MCOM_Transfer *transfer)
{
    transfer->writeBufferHandle.status = MCOM_STATUS_COMPLETE;
}


static inline void MCOM_setReadReady(struct MCOM_Transfer *transfer)
{
    transfer->readBufferHandle.status = MCOM_STATUS_READY;
}


static inline void MCOM_setWriteReady(struct MCOM_Transfer *transfer)
{
    transfer->writeBufferHandle.status = MCOM_STATUS_READY;
}


static inline uint16_t MCOM_getNumOfReadData(const struct MCOM_Transfer *transfer)
{
    return transfer->readBufferHandle.index;
}


static inline uint16_t MCOM_getNumOfDataToWrite(const struct MCOM_Transfer *transfer)
{
    if (transfer->writeBufferHandle.length > transfer->writeBufferHandle.index)
    {
        return (uint16_t)(transfer->writeBufferHandle.length - transfer->writeBufferHandle.index);
    }
    return 0;
}
#endif // MCOM_MOCK_H

#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif

#endif // MCOM_H
