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
 *  COMP_ABBREV      RBUF
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  mem_utility
 *
 *  \brief    Public definitions and declarations for unit ring_buffer.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "ring_buffer.h"
#include <stddef.h>
#include <string.h>
#ifdef RBUF_WRITE_REENTRANT
    #include "hsup.h"
#endif


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/



/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

static void    RBUF_push (struct RBUF_ringBuffer *rb, uint8_t data);
static uint8_t RBUF_pop  (struct RBUF_ringBuffer *rb);
static void    RBUF_pushn(const struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len, RBUF_size_t index);
static void    RBUF_popn (struct RBUF_ringBuffer *rb, uint8_t *data, RBUF_size_t len);
static void    RBUF_incrementReadIdx(struct RBUF_ringBuffer *rb);
static void    RBUF_incrementWriteIdx(struct RBUF_ringBuffer *rb);
static void    RBUF_addToIndex(const struct RBUF_ringBuffer *rb, RBUF_size_t *index, RBUF_size_t len);
static void    RBUF_closeWriteOperation(struct RBUF_ringBuffer *rb);
static void    RBUF_disableIrq(void);
static void    RBUF_enableIrq(void);
static RBUF_size_t RBUF_getFreeSpaceIntern(const struct RBUF_ringBuffer *rb);
static RBUF_size_t RBUF_openWriteOperation(struct RBUF_ringBuffer *rb, RBUF_size_t len);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/** symbolized an invalid index into ring buffer */
static const RBUF_size_t RBUF_invalidIndex = (RBUF_size_t)(-1);
#ifdef RBUF_WRITE_REENTRANT
    /** maximum re-entrance level */
    static const uint8_t RBUF_maxWriteReentrance = RBUF_WRITE_REENTRANT;
#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*lint -e934 "Taking address of near auto variable [MISRA 2012 Rule 1.3, required]", relevant for DLLs only */

void RBUF_init(struct RBUF_ringBuffer *rb, uint8_t *buffer, RBUF_size_t size)
{
    rb->buffer = buffer; RBUF_init_Line[0]++;
    rb->size = size; RBUF_init_Line[1]++;
    RBUF_reset(rb); RBUF_init_Line[2]++;
}


void RBUF_reset(struct RBUF_ringBuffer *rb)
{
    rb->write = 0; RBUF_reset_Line[0]++;
    rb->read = 0; RBUF_reset_Line[1]++;
  #ifdef RBUF_WRITE_REENTRANT
    rb->incoming = 0; RBUF_reset_Line[2]++;
    rb->reentered = 0; RBUF_reset_Line[3]++;
  #endif
}


bool RBUF_writeData_u8(struct RBUF_ringBuffer *rb, uint8_t data)
{
    bool result = false; RBUF_writeData_u8_Line[0]++;
    
    RBUF_disableIrq(); RBUF_writeData_u8_Line[1]++;
    if( RBUF_getFreeSpaceIntern(rb) >= sizeof(data))
    {
        result = true; RBUF_writeData_u8_Line[2]++;
        RBUF_push(rb,data); RBUF_writeData_u8_Line[3]++;
    }
    RBUF_enableIrq(); RBUF_writeData_u8_Line[4]++;
    RBUF_writeData_u8_Line[5]++; return  result;
}


bool RBUF_writeData_u16(struct RBUF_ringBuffer *rb, uint16_t data)
{
    bool result = false; RBUF_writeData_u16_Line[0]++;
    
    RBUF_disableIrq(); RBUF_writeData_u16_Line[1]++;
    if( RBUF_getFreeSpaceIntern(rb) >= sizeof(data))
    {
        result = true; RBUF_writeData_u16_Line[2]++;
        RBUF_push(rb,(uint8_t)(data >> 8u)); RBUF_writeData_u16_Line[3]++;
        RBUF_push(rb,(uint8_t)(data)); RBUF_writeData_u16_Line[4]++;
    }
    RBUF_enableIrq(); RBUF_writeData_u16_Line[5]++;
    RBUF_writeData_u16_Line[6]++; return  result;
}

bool RBUF_writeData_u32(struct RBUF_ringBuffer *rb, uint32_t data)
{
    bool result = false; RBUF_writeData_u32_Line[0]++;
    
    RBUF_disableIrq(); RBUF_writeData_u32_Line[1]++;
    if( RBUF_getFreeSpaceIntern(rb) >= sizeof(data))
    {
        result = true; RBUF_writeData_u32_Line[2]++;
        RBUF_push(rb,(uint8_t)(data >> 24u)); RBUF_writeData_u32_Line[3]++;
        RBUF_push(rb,(uint8_t)(data >> 16u)); RBUF_writeData_u32_Line[4]++;
        RBUF_push(rb,(uint8_t)(data >> 8u)); RBUF_writeData_u32_Line[5]++;
        RBUF_push(rb,(uint8_t)(data)); RBUF_writeData_u32_Line[6]++;
    }
    RBUF_enableIrq(); RBUF_writeData_u32_Line[7]++;
    RBUF_writeData_u32_Line[8]++; return  result;
}


bool RBUF_writeData_u64(struct RBUF_ringBuffer *rb, uint64_t data)
{
    bool result = false; RBUF_writeData_u64_Line[0]++;
    
    RBUF_disableIrq(); RBUF_writeData_u64_Line[1]++;
    if( RBUF_getFreeSpaceIntern(rb) >= sizeof(data))
    {
        result = true; RBUF_writeData_u64_Line[2]++;
        (void)RBUF_writeData_u32(rb, (uint32_t)(data >> 32u)); RBUF_writeData_u64_Line[3]++;
        (void)RBUF_writeData_u32(rb, (uint32_t)(data)); RBUF_writeData_u64_Line[4]++;
    }
    RBUF_enableIrq(); RBUF_writeData_u64_Line[5]++;
    RBUF_writeData_u64_Line[6]++; return  result;
}


RBUF_size_t RBUF_writeBuffN(struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len)
{
    if(len == 0u){return 0u;} // length of zero always ok
    
    RBUF_disableIrq(); RBUF_writeBuffN_Line[0]++;
    RBUF_size_t wrlen = RBUF_getFreeSpaceIntern(rb); RBUF_writeBuffN_Line[1]++;
    if( wrlen > len){wrlen = len; RBUF_writeBuffN_Line[2]++;}
    /* Start protected writing into ring buffer
       Protect incommig data to be read before finished */
    RBUF_size_t myIndex = RBUF_openWriteOperation(rb, wrlen); RBUF_writeBuffN_Line[3]++;
    RBUF_enableIrq(); RBUF_writeBuffN_Line[4]++;
    
    RBUF_pushn(rb, data, wrlen, myIndex); RBUF_writeBuffN_Line[5]++;
    RBUF_closeWriteOperation(rb); RBUF_writeBuffN_Line[6]++;
    RBUF_writeBuffN_Line[7]++; return  wrlen;
}


bool RBUF_writeBuff(struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len)
{
    if(len == 0u){return true;} // length of zero always ok

    /* Start protected writing into ring buffer
       Protect incommig data to be read before finished */
    RBUF_size_t myIndex = RBUF_openWriteOperation(rb, len); RBUF_writeBuff_Line[0]++;
    
    if(myIndex != RBUF_invalidIndex)
    {
        RBUF_pushn(rb, data, len, myIndex); RBUF_writeBuff_Line[1]++;
        RBUF_closeWriteOperation(rb); RBUF_writeBuff_Line[2]++;
        RBUF_writeBuff_Line[3]++; return  true;
    }
    RBUF_writeBuff_Line[4]++; return  false;
}


bool RBUF_writeStr(struct RBUF_ringBuffer *rb, const char * str)
{
    RBUF_size_t len = (RBUF_size_t)(strlen(str)); RBUF_writeStr_Line[0]++; // without terminating zero
    /*Axivion Next Line MisraC2012-11.5: Tested to work correctly on all platforms */
    RBUF_writeStr_Line[1]++; return  RBUF_writeBuff(rb, (const uint8_t*)(const void*)str, len);
}



bool RBUF_readData_u8(struct RBUF_ringBuffer *rb, uint8_t *data)
{
    if(RBUF_getBytesToRead(rb) >= (RBUF_size_t)sizeof(*data))
    {
        *data = RBUF_pop(rb); RBUF_readData_u8_Line[0]++;
        RBUF_readData_u8_Line[1]++; return  true;
    }
    RBUF_readData_u8_Line[2]++; return  false; // not enough data available
}


bool RBUF_readData_u16(struct RBUF_ringBuffer *rb, uint16_t *data)
{
    if(RBUF_getBytesToRead(rb) >= (RBUF_size_t)sizeof(*data))
    {
        *data  = (uint16_t)RBUF_pop(rb) << 8u; RBUF_readData_u16_Line[0]++;
        *data += (uint16_t)RBUF_pop(rb); RBUF_readData_u16_Line[1]++;
        RBUF_readData_u16_Line[2]++; return  true;
    }
    RBUF_readData_u16_Line[3]++; return  false; // not enough data available
}


bool RBUF_readData_u32(struct RBUF_ringBuffer *rb, uint32_t *data)
{
    if(RBUF_getBytesToRead(rb) >= (RBUF_size_t)sizeof(*data))
    {
        *data  = (uint32_t)RBUF_pop(rb) << 24u; RBUF_readData_u32_Line[0]++;
        *data += (uint32_t)RBUF_pop(rb) << 16u; RBUF_readData_u32_Line[1]++;
        *data += (uint32_t)RBUF_pop(rb) << 8u; RBUF_readData_u32_Line[2]++;
        *data += (uint32_t)RBUF_pop(rb); RBUF_readData_u32_Line[3]++;
        RBUF_readData_u32_Line[4]++; return  true;
    }
    RBUF_readData_u32_Line[5]++; return  false; // not enough data available
}


bool RBUF_readData_u64(struct RBUF_ringBuffer *rb, uint64_t *data)
{
    uint32_t hi,lo; RBUF_readData_u64_Line[0]++;
    
    if(RBUF_getBytesToRead(rb) >= (RBUF_size_t)sizeof(*data))
    {
        (void)RBUF_readData_u32(rb,&hi); RBUF_readData_u64_Line[1]++;
        (void)RBUF_readData_u32(rb,&lo); RBUF_readData_u64_Line[2]++;
        *data  = (uint64_t)hi << 32u; RBUF_readData_u64_Line[3]++;
        *data += (uint64_t)lo; RBUF_readData_u64_Line[4]++;
        RBUF_readData_u64_Line[5]++; return  true;
    }
    RBUF_readData_u64_Line[6]++; return  false; // not enough data available
}


RBUF_size_t RBUF_readBuffN(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t len)
{
    RBUF_size_t rdLen = RBUF_getBytesToRead(rb); RBUF_readBuffN_Line[0]++;
    
    if(rdLen > len){rdLen = len; RBUF_readBuffN_Line[1]++;}
    RBUF_popn(rb, buff, rdLen); RBUF_readBuffN_Line[2]++;
    RBUF_readBuffN_Line[3]++; return  rdLen;
}


bool RBUF_readBuff(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t len)
{
    if(RBUF_getBytesToRead(rb) >= len)
    {
        RBUF_popn(rb, buff, len); RBUF_readBuff_Line[0]++;
        RBUF_readBuff_Line[1]++; return  true;
    }
    else
    {
        RBUF_readBuff_Line[2]++; return  false; // not enough data available
    }
}


RBUF_size_t RBUF_readAll(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t maxLen)
{
    RBUF_size_t len = RBUF_getBytesToRead(rb); RBUF_readAll_Line[0]++;
    
    if (len > maxLen){len = maxLen; RBUF_readAll_Line[1]++;}
    RBUF_popn(rb, buff, len); RBUF_readAll_Line[2]++;
    RBUF_readAll_Line[3]++; return  len;
}


RBUF_size_t RBUF_getFreeSpace(const struct RBUF_ringBuffer *rb)
{
    RBUF_disableIrq(); RBUF_getFreeSpace_Line[0]++;
    RBUF_size_t space = RBUF_getFreeSpaceIntern(rb); RBUF_getFreeSpace_Line[1]++;
    RBUF_enableIrq(); RBUF_getFreeSpace_Line[2]++;
    RBUF_getFreeSpace_Line[3]++; return  space;
}


RBUF_size_t RBUF_getBytesToRead(const struct RBUF_ringBuffer *rb)
{
    RBUF_size_t writeIdx = rb->write; RBUF_getBytesToRead_Line[0]++;
  #ifdef RBUF_WRITE_REENTRANT
    if(rb->reentered > 0u)
    {   // incoming data reception
        writeIdx = rb->incoming; RBUF_getBytesToRead_Line[1]++;
    }
  #endif
    if (writeIdx >= rb->read)
    {
        RBUF_getBytesToRead_Line[2]++; return  (RBUF_size_t)(writeIdx - rb->read);
    }
    else
    {
        RBUF_getBytesToRead_Line[3]++; return  (RBUF_size_t)((rb->size + writeIdx) - rb->read);
    }
}


RBUF_size_t RBUF_getCapacity(const struct RBUF_ringBuffer *rb)
{
    RBUF_getCapacity_Line[0]++; return  rb->size - 1U;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/


/** pushes a single byte to the head of the ring buffer.
 *  \note The caller is responsible to make sure that at least one byte is free
 *        in ring buffer
 *
 *  \param rb   pointer to ring buffer object
 *  \param data byte to push
 */
static void RBUF_push(struct RBUF_ringBuffer *rb, uint8_t data)
{
    rb->buffer[rb->write] = data; RBUF_push_Line[0]++;
    RBUF_incrementWriteIdx(rb); RBUF_push_Line[1]++;
}


/** pushes a buffer of bytes to the head of the ring buffer.
 *  \note The caller is responsible to make sure that enough space is available
 *        in ring buffer
 *
 *  \param rb     pointer to ring buffer object
 *  \param data   buffer to push
 *  \param len    number of bytes to push
 *  \param index  position in ring buffer to copy data
 */
static void RBUF_pushn(const struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len, RBUF_size_t index)
{
    for(RBUF_size_t i=0; i<len; i++)
    {
        rb->buffer[index] = data[i]; RBUF_pushn_Line[0]++;
        RBUF_addToIndex(rb, &index,1u); RBUF_pushn_Line[1]++;
    }
}


/** pops a single byte form the tail of ring buffer.
 *  \note The caller is responsible to make sure that at least one byte is available
 *        in the ring buffer
 *
 *  \param rb   pointer to ring buffer object
 *  \return     byte form tail of ring buffer
 */
static uint8_t RBUF_pop(struct RBUF_ringBuffer *rb)
{
    uint8_t data = rb->buffer[rb->read]; RBUF_pop_Line[0]++;
    
    RBUF_incrementReadIdx(rb); RBUF_pop_Line[1]++;
    RBUF_pop_Line[2]++; return  data;
}


/** pops a number of byte form the tail of ring buffer.
 *  \note The caller is responsible to make sure that enough bytes are available
 *        in the ring buffer
 *
 *  \param rb   pointer to ring buffer object
 *  \param data data buffer where to pop data
 */
static void RBUF_popn(struct RBUF_ringBuffer *rb, uint8_t *data, RBUF_size_t len)
{
    for(RBUF_size_t i=0; i<len; i++)
    {
        data[i] = rb->buffer[rb->read]; RBUF_popn_Line[0]++;
        RBUF_incrementReadIdx(rb); RBUF_popn_Line[1]++;
    }
}


/** increments the write index of the ring buffer.
 *  \param rb   pointer to ring buffer object
 */
static void RBUF_incrementWriteIdx(struct RBUF_ringBuffer *rb)
{
    RBUF_addToIndex(rb, &rb->write,1u); RBUF_incrementWriteIdx_Line[0]++;
  #ifdef RBUF_WRITE_REENTRANT
    if(rb->reentered == 0u)
    {   //no nested write operation ongoing!
        rb->incoming = rb->write; RBUF_incrementWriteIdx_Line[1]++;
    }
  #endif
}


/** increments the read index of the ring buffer.
 *  \param rb   pointer to ring buffer object
 */
static void RBUF_incrementReadIdx(struct RBUF_ringBuffer *rb)
{
    RBUF_addToIndex(rb, &rb->read,1u); RBUF_incrementReadIdx_Line[0]++;
}


/** adds a value to a specified ring buffer index.
 *  \param rb   pointer to ring buffer object
 *  \param[out] pointer to the index to modify
 *  \param len  value to be added to selected index
 */
static void RBUF_addToIndex(const struct RBUF_ringBuffer *rb, RBUF_size_t *index, RBUF_size_t len)
{
    *index += len; RBUF_addToIndex_Line[0]++;
    if(*index >= rb->size){*index -= rb->size; RBUF_addToIndex_Line[1]++;}
}


/** Opens a write operation
 *  \details If enabled nested writing to buffer is possible
 *  \param rb   pointer to ring buffer object
 *  \param len  number of bytes for requested write operation
 *  \return start index in ring buffer where to copy data, or
 *          \sa RBUF_invalidIndex when no enough space available.
 */
static RBUF_size_t RBUF_openWriteOperation(struct RBUF_ringBuffer *rb, RBUF_size_t len)
{
    RBUF_size_t in_index = RBUF_invalidIndex; RBUF_openWriteOperation_Line[0]++;
    
    RBUF_disableIrq(); RBUF_openWriteOperation_Line[1]++;
    if (len <= RBUF_getFreeSpaceIntern(rb))
    {
      #ifdef RBUF_WRITE_REENTRANT
        if(rb->reentered < RBUF_maxWriteReentrance)
        {
            rb->reentered++; RBUF_openWriteOperation_Line[2]++;
      #else
        {
      #endif
            in_index = rb->write; RBUF_openWriteOperation_Line[3]++;
            RBUF_addToIndex(rb, &rb->write,len); RBUF_openWriteOperation_Line[4]++;
        }
    }
    RBUF_enableIrq(); RBUF_openWriteOperation_Line[5]++;
    RBUF_openWriteOperation_Line[6]++; return  in_index;
}



/** Closes a write operation
 *  \details If enabled nested writing to buffer is possible
 *  \param rb     pointer to ring buffer object
 */
static void RBUF_closeWriteOperation(struct RBUF_ringBuffer *rb)
{ /*lint --e{715,818} rb not accessed*/
  #ifdef RBUF_WRITE_REENTRANT
    RBUF_disableIrq(); RBUF_closeWriteOperation_Line[0]++;
    rb->reentered--; RBUF_closeWriteOperation_Line[1]++;
    if(rb->reentered == 0u)
    {   //no nested write operation ongoing!
        rb->incoming = rb->write; RBUF_closeWriteOperation_Line[2]++;
    }
    RBUF_enableIrq(); RBUF_closeWriteOperation_Line[3]++;
  #endif
}


/** returns the number of free bytes inside ring buffer.
 *  \note The caller is responsible to make to prevent re-entrance! 
 *
 *  \param rb     pointer to ring buffer object
 *  \return       free space in ring buffer
 */
static RBUF_size_t RBUF_getFreeSpaceIntern(const struct RBUF_ringBuffer *rb)
{
    if (rb->read > rb->write)
    {
        RBUF_getFreeSpaceIntern_Line[0]++; return  (RBUF_size_t)((rb->read - rb->write) -1u);
    }
    else
    {
        RBUF_getFreeSpaceIntern_Line[1]++; return  (RBUF_size_t)(((rb->size + rb->read) - rb->write) - 1u);
    }
}


/** disables interrupts when option RBUF_WRITE_REENTRANT is enabled */
static void RBUF_disableIrq(void)
{
  #ifdef RBUF_WRITE_REENTRANT
    HSUP_vDisableInt(); RBUF_disableIrq_Line[0]++;
  #endif
}


/** enables interrupts when option RBUF_WRITE_REENTRANT is enabled */
static void RBUF_enableIrq(void)
{
  #ifdef RBUF_WRITE_REENTRANT
    HSUP_vEnableInt(); RBUF_enableIrq_Line[0]++;
  #endif
}
