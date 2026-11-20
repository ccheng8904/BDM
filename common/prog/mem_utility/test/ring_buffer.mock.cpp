/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include "ring_buffer.mock.h"
#include <vector>
#include <string>
#include <algorithm>

namespace ring_buffer_m
{
    ring_buffer_mock_class ring_buffer_mock;

    void RBUF_init(struct RBUF_ringBuffer *rb, uint8_t *buffer, RBUF_size_t size)
    {
        vector<uint8_t> vbuf(size);
        ring_buffer_mock.RBUF_init(*rb, vbuf, size);
        std::copy_n(vbuf.begin(), size, buffer);
    }

    void RBUF_reset(struct RBUF_ringBuffer *rb)
    {
        ring_buffer_mock.RBUF_reset(*rb);
    }

    bool RBUF_writeData_u8(struct RBUF_ringBuffer *rb, uint8_t data)
    {
        return ring_buffer_mock.RBUF_writeData_u8(*rb, data);
    }

    bool RBUF_writeData_u16(struct RBUF_ringBuffer *rb, uint16_t data)
    {
        return ring_buffer_mock.RBUF_writeData_u16(*rb, data);
    }

    bool RBUF_writeData_u32(struct RBUF_ringBuffer *rb, uint32_t data)
    {
        return ring_buffer_mock.RBUF_writeData_u32(*rb, data);
    }

    bool RBUF_writeData_u64(struct RBUF_ringBuffer *rb, uint64_t data)
    {
        return ring_buffer_mock.RBUF_writeData_u64(*rb, data);
    }

    RBUF_size_t RBUF_writeBuffN(struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len)
    {
        vector<uint8_t> vdata;
        vdata.assign(data, data + len);
        return ring_buffer_mock.RBUF_writeBuffN(*rb, vdata, len);
    }

    bool RBUF_writeBuff(struct RBUF_ringBuffer *rb, const uint8_t *data, RBUF_size_t len)
    {
        vector<uint8_t> vdata;
        vdata.assign(data, data + len);
        return ring_buffer_mock.RBUF_writeBuffN(*rb, vdata, len);
    }

    bool RBUF_writeStr(struct RBUF_ringBuffer *rb, const char * str)
    {
        string vstr(str);
        return ring_buffer_mock.RBUF_writeStr(*rb, vstr);
    }

    bool RBUF_readData_u8(struct RBUF_ringBuffer *rb, uint8_t *data)
    {
        return ring_buffer_mock.RBUF_readData_u8(*rb, *data);
    }

    bool RBUF_readData_u16(struct RBUF_ringBuffer *rb, uint16_t *data)
    {
        return ring_buffer_mock.RBUF_readData_u16(*rb, *data);
    }

    bool RBUF_readData_u32(struct RBUF_ringBuffer *rb, uint32_t *data)
    {
        return ring_buffer_mock.RBUF_readData_u32(*rb, *data);
    }

    bool RBUF_readData_u64(struct RBUF_ringBuffer *rb, uint64_t *data)
    {
        return ring_buffer_mock.RBUF_readData_u64(*rb, *data);
    }

    RBUF_size_t RBUF_readBuffN(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t len)
    {
        vector<uint8_t> vbuf(len);
        RBUF_size_t ret = ring_buffer_mock.RBUF_readBuffN(*rb, vbuf, len);
        std::copy_n(vbuf.begin(), len, buff);
        return ret;
    }

    bool RBUF_readBuff(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t len)
    {
        vector<uint8_t> vbuf(len);
        bool ret = ring_buffer_mock.RBUF_readBuff(*rb, vbuf, len);
        std::copy_n(vbuf.begin(), len, buff);
        return ret;
    }

    RBUF_size_t RBUF_readAll(struct RBUF_ringBuffer *rb, uint8_t *buff, RBUF_size_t maxLen)
    {
        vector<uint8_t> vbuf(maxLen);
        RBUF_size_t ret = ring_buffer_mock.RBUF_readAll(*rb, vbuf, maxLen);
        std::copy_n(vbuf.begin(), maxLen, buff);
        return ret;
    }

    RBUF_size_t RBUF_getFreeSpace(const struct RBUF_ringBuffer *rb)
    {
        return ring_buffer_mock.RBUF_getFreeSpace(*rb);
    }

    RBUF_size_t RBUF_getBytesToRead(const struct RBUF_ringBuffer *rb)
    {
        return ring_buffer_mock.RBUF_getBytesToRead(*rb);
    }

    RBUF_size_t RBUF_getCapacity(const struct RBUF_ringBuffer *rb)
    {
        return ring_buffer_mock.RBUF_getCapacity(*rb);
    }

}

