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


#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  \mem_utility
 *
 *  \brief    Public definitions and declarations for unit ring_buffer.
 *
 *  \details  
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stddef.h> // needed for unit test
#include <stdbool.h>


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

typedef uint_least16_t RBUF_size_t;


/** structure for ring buffer object */
struct RBUF_ringBuffer
{
    uint8_t *buffer;        ///< points to reserved data memory
    RBUF_size_t write;      ///< write index, new data will be placed here
    RBUF_size_t read;       ///< read index data available form here to incoming-1
    RBUF_size_t size;       ///< data available in reserved buffer
  #ifdef RBUF_WRITE_REENTRANT
    RBUF_size_t incoming;   ///< points to incoming data, area reserved up to write index (needed for re-entrance)
    uint8_t     reentered;  ///< counter for nested re-entrance
  #endif
};



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief Initializes the ring buffer and its data structure
 *
 *  \note This interface must be called before any other interface may be used
 *
 *  \param rb     pointer to ring buffer structure
 *  \param buffer pointer to data buffer
 *  \param size   size of data buffer object
 */
extern void RBUF_init(struct RBUF_ringBuffer *rb, uint8_t *buffer, RBUF_size_t size);

/** \brief Resets the ring buffer
 *  \details by resetting the ring buffer data structure it also clears the buffer
 *
 *  \param rb     pointer to ring buffer object
 */
extern void RBUF_reset(struct RBUF_ringBuffer *rb);


/** Writes an unsigned byte to the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data a data byte to write to the buffer
 *  \return true if data has been written successfully false otherwise
 */
extern bool RBUF_writeData_u8(struct RBUF_ringBuffer *rb, uint8_t data);

/** Writes an unsigned word to the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data word to write to the buffer
 *  \return true if data has been written successfully, false otherwise
 */
extern bool RBUF_writeData_u16(struct RBUF_ringBuffer *rb, uint16_t data);


/** Writes an unsigned dword to the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data dword to write to the buffer
 *  \return true if data has been written successfully, false otherwise
 */
extern bool RBUF_writeData_u32(struct RBUF_ringBuffer *rb, uint32_t data);


/** Writes an unsigned qword to the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data a data byte to write to the buffer
 *  \return true if data has been written successfully, false otherwise
 */
extern bool RBUF_writeData_u64(struct RBUF_ringBuffer *rb, uint64_t data);


/** Writes a maximum possible number of bytes to the ring buffer.
 *  \details If data does not fit to ringbuffer maximum amount of data will be written.
 *  \param rb   pointer to ring buffer object
 *  \param data pointer to the data to write to the ring buffer
 *  \return number of bytes written to ring buffer.
 */
extern RBUF_size_t RBUF_writeBuffN(struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len);


/** Writes a number of bytes to the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data pointer to the data to write to the ring buffer
 *  \return true if data has been written successfully, false otherwise
 */
extern bool RBUF_writeBuff(struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len);


/** Writes a string without terminating zero to the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param str pointer to the data to write to the ring buffer
 *  \return true if string has been written successfully, false otherwise
 */
extern bool RBUF_writeStr(struct RBUF_ringBuffer *rb, const char * str);


/** Reads an unsigned byte from the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data[out] pointer to data byte to hold data form ring buffer
 *  \return true if read was successfully, false otherwise
 */
extern bool RBUF_readData_u8(struct RBUF_ringBuffer *rb, uint8_t *data);


/** Reads an unsigned word from the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data[out] pointer to word to hold data form ring buffer
 *  \return true if read was successfully, false otherwise
 */
extern bool RBUF_readData_u16(struct RBUF_ringBuffer *rb, uint16_t *data);


/** Reads an unsigned dword from the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data[out] pointer to dword to hold data form ring buffer
 *  \return true if read was successfully, false otherwise
 */
extern bool RBUF_readData_u32(struct RBUF_ringBuffer *rb, uint32_t *data);


/** Reads an unsigned qword from the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data[out] pointer to qword to hold data form ring buffer
 *  \return true if read was successfully, false otherwise
 */
extern bool RBUF_readData_u64(struct RBUF_ringBuffer *rb, uint64_t *data);


/** Reads a maximum number of bytes from the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data[out] pointer to buffer to hold data form ring buffer
 *  \param len  maximum number of bytes to read from ring buffer
 *  \return number of read bytes
 *  \note the caller is responsible to hold data bytes specified by len
 */
extern RBUF_size_t RBUF_readBuffN(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t len);


/** Reads a number of bytes from the ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data[out] pointer to buffer to hold data form ring buffer
 *  \param len  number of bytes to read from ring buffer
 *  \return true if read was successfully, false when not enough data available
 *  \note the caller is responsible to hold data bytes specified by len
 */
extern bool RBUF_readBuff(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t len);


/** Reads complete content of ring buffer.
 *  \param rb   pointer to ring buffer object
 *  \param data[out] pointer to data byte to hold data form ring buffer
 *  \param maxLen    maximum length to read
 *  \return number of bytes which has been read
 *  \note the caller is responsible to hold data baytes specified by maxLen
 */
extern RBUF_size_t RBUF_readAll(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t maxLen);


/** Returns the free space in ring buffer in bytes
 *  \param rb   pointer to ring buffer object
 *  \return free space in buffer
 */
extern RBUF_size_t RBUF_getFreeSpace(const struct RBUF_ringBuffer *rb);


/** Deliver the number of available bytes in ring buffer
 *  \param rb   pointer to ring buffer object
 *  \return number of available bytes
 */
extern RBUF_size_t RBUF_getBytesToRead(const struct RBUF_ringBuffer *rb);


/** returns the maximum available space in ring buffer
 *  \param rb   pointer to ring buffer object
 *  \return capacity of ring buffer
 */
extern RBUF_size_t RBUF_getCapacity(const struct RBUF_ringBuffer *rb);


/** Deliver whether the buffer is full or not
 *  \param rb   pointer to ring buffer object
 *  \return true if buffer is full, otherwise false
 */
static inline bool RBUF_isBufferFull(const struct RBUF_ringBuffer *rb)
{
    return (RBUF_getFreeSpace(rb) == 0U) ? true : false;
}


#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif
/******************************************************************************/
/* PUBLIC CLASS DECLARATIONS                                                  */
/******************************************************************************/
#ifdef __cplusplus


namespace RBUF
{
    template <RBUF_size_t bufferSize>
    class RingBuffer
    {
        private:
            struct RBUF_ringBuffer ringBuffer;
            uint8_t buffer[bufferSize];

        public:
            RingBuffer()
            {
                RBUF_init(&this->ringBuffer, this->buffer, bufferSize);
            }

            void reset()
            {
                RBUF_reset(&this->ringBuffer);
            }

            bool write(uint8_t data)
            {
                return RBUF_writeData_u8(&this->ringBuffer, data);
            }

            bool write(uint16_t data)
            {
                return RBUF_writeData_u16(&this->ringBuffer, data);
            }

            bool write(uint32_t data)
            {
                return RBUF_writeData_u32(&this->ringBuffer, data);
            }

            bool write(uint64_t data)
            {
                return RBUF_writeData_u64(&this->ringBuffer, data);
            }

            RBUF_size_t writeN(const uint8_t buffer[], RBUF_size_t len)
            {
                return RBUF_writeBuffN(&this->ringBuffer, buffer, len);
            }

            bool write(const uint8_t buffer[], RBUF_size_t len)
            {
                return RBUF_writeBuff(&this->ringBuffer, buffer, len);
            }

            bool write(const char * str)
            {
                return RBUF_writeStr(&this->ringBuffer, str);
            }

            bool read(uint8_t *data)
            {
                return RBUF_readData_u8(&this->ringBuffer, data);
            }
            
            bool read(uint16_t *data)
            {
                return RBUF_readData_u16(&this->ringBuffer, data);
            }

            bool read(uint32_t *data)
            {
                return RBUF_readData_u32(&this->ringBuffer, data);
            }

            bool read(uint64_t *data)
            {
                return RBUF_readData_u64(&this->ringBuffer, data);
            }

            RBUF_size_t readN(uint8_t buffer[], RBUF_size_t len)
            {
                return RBUF_readBuffN(&this->ringBuffer, buffer, len);
            }

            bool read(uint8_t buffer[], RBUF_size_t len)
            {
                return RBUF_readBuff(&this->ringBuffer, buffer, len);
            }

            bool readAll(uint8_t buffer[], RBUF_size_t maxLen)
            {
                return RBUF_readAll(&this->ringBuffer, buffer, maxLen);
            }

            RBUF_size_t getBytesToRead()
            {
                return RBUF_getBytesToRead(&this->ringBuffer);
            }
    };
}

#endif // __cplusplus


#endif // RING_BUFFER_H
