/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#ifndef RING_BUFFER_MOCK_H
#define RING_BUFFER_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace ring_buffer_m
{
    #include "mem_utility/ring_buffer.h"
    using std::vector;
    using std::string;

    class ring_buffer_mock_class
    {
    public:
        MAKE_MOCK3(RBUF_init,                  void (RBUF_ringBuffer &rb, vector<uint8_t> & buffer, RBUF_size_t size));
        MAKE_MOCK1(RBUF_reset,                 void (RBUF_ringBuffer &rb));
        MAKE_MOCK2(RBUF_writeData_u8,          bool (RBUF_ringBuffer &rb, uint8_t  data));
        MAKE_MOCK2(RBUF_writeData_u16,         bool (RBUF_ringBuffer &rb, uint16_t data));
        MAKE_MOCK2(RBUF_writeData_u32,         bool (RBUF_ringBuffer &rb, uint32_t data));
        MAKE_MOCK2(RBUF_writeData_u64,         bool (RBUF_ringBuffer &rb, uint64_t data));
        MAKE_MOCK3(RBUF_writeBuffN,     RBUF_size_t (RBUF_ringBuffer &rb, vector<uint8_t> const & data, RBUF_size_t len));
        MAKE_MOCK3(RBUF_writeBuff,             bool (RBUF_ringBuffer &rb, vector<uint8_t> const & data, RBUF_size_t len));
        MAKE_MOCK2(RBUF_writeStr,              bool (RBUF_ringBuffer &rb, const string & str));
        MAKE_MOCK2(RBUF_readData_u8,           bool (RBUF_ringBuffer &rb, uint8_t  &data));
        MAKE_MOCK2(RBUF_readData_u16,          bool (RBUF_ringBuffer &rb, uint16_t &data));
        MAKE_MOCK2(RBUF_readData_u32,          bool (RBUF_ringBuffer &rb, uint32_t &data));
        MAKE_MOCK2(RBUF_readData_u64,          bool (RBUF_ringBuffer &rb, uint64_t &data));
        MAKE_MOCK3(RBUF_readBuffN,      RBUF_size_t (RBUF_ringBuffer &rb, vector<uint8_t> & buff, RBUF_size_t len));
        MAKE_MOCK3(RBUF_readBuff,              bool (RBUF_ringBuffer &rb, vector<uint8_t> & buff, RBUF_size_t len));
        MAKE_MOCK3(RBUF_readAll,        RBUF_size_t (RBUF_ringBuffer &rb, vector<uint8_t> & buff, RBUF_size_t maxLen));
        MAKE_MOCK1(RBUF_getFreeSpace,   RBUF_size_t (const RBUF_ringBuffer &rb));
        MAKE_MOCK1(RBUF_getBytesToRead, RBUF_size_t (const RBUF_ringBuffer &rb));
        MAKE_MOCK1(RBUF_getCapacity,    RBUF_size_t (const RBUF_ringBuffer &rb));
    };

    extern ring_buffer_mock_class ring_buffer_mock;
}

using namespace ring_buffer_m;

#endif  // RING_BUFFER_MOCK_H
