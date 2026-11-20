/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW Test
 ******************************************************************************/

#include <doctest/doctest.h>
#include "test/hsup.mock.h"
#include <vector>
#include <algorithm>
#ifndef RBUF_WRITE_REENTRANT
    #define RBUF_WRITE_REENTRANT 5
#endif

namespace ring_buffer_test
{
    // include the implementation of 'ring_buffer' into the current namespace
    #include "mem_utility/ring_buffer.c"

    using std::vector;



    bool operator==(const RBUF_ringBuffer& lhs, const RBUF_ringBuffer& rhs)
    {
        return ((lhs.write == rhs.write) && (lhs.read   == rhs.read)   && (lhs.incoming  == rhs.incoming) &&
                (lhs.size  == rhs.size)  && (lhs.buffer == rhs.buffer) && (lhs.reentered == rhs.reentered));
    }



    TEST_SUITE("ring-buffer")
    {   
        trompeloeil::sequence rbuf_seq;
        constexpr RBUF_size_t bsize = 100;
        RBUF_ringBuffer rbuf;
        uint8_t buffer[bsize];
        const uint8_t randomData[]={21, 147, 205, 209, 25, 44, 42, 171, 231, 229, 203, 85, 238, 6, 81, 17};
        const RBUF_ringBuffer init_value_rbuf{.buffer=buffer, .write=0, .read=0, .size=bsize,  .incoming=0, .reentered=0};
        const RBUF_ringBuffer random_rbuf{.buffer=buffer, .write=42, .read=17, .size=bsize,  .incoming=17, .reentered=17};
        

        TEST_CASE("ring-buffer tests part 1 (reset and init)")
        {
            rbuf = random_rbuf;

            SUBCASE("RBUF_init: check the initialization of ring buffer structure")
            {
                RBUF_init(&rbuf,buffer,bsize);
                CHECK(rbuf == init_value_rbuf);
            }
            SUBCASE("RBUF_reset: check if buffer indices are zeroed after reset")
            {
                rbuf = random_rbuf;
                RBUF_reset(&rbuf);
                CHECK(rbuf == init_value_rbuf);
            }
        }

        TEST_CASE("ring-buffer tests part 2 (free space and buffer content)")
        {
            RBUF_init(&rbuf,buffer,bsize);

            SUBCASE("RBUF_getCapacity: check the reserved space")
            {
                CHECK(RBUF_getCapacity(&rbuf) == bsize-1);
            }

            SUBCASE("RBUF_getFreeSpace")
            {
                REQUIRE_CALL(hsup_mock, HSUP_vDisableInt()) .IN_SEQUENCE(rbuf_seq);
                REQUIRE_CALL(hsup_mock, HSUP_vEnableInt())  .IN_SEQUENCE(rbuf_seq);
            
                SUBCASE("initial space")
                {
                    CHECK(RBUF_getFreeSpace(&rbuf) == bsize-1);
                }

                SUBCASE("single byte written")
                {
                    rbuf.write = 1;
                    rbuf.incoming = 1;
                    CHECK(RBUF_getFreeSpace(&rbuf) == bsize-2);
                }

                SUBCASE("buffer nearly full")
                {
                    rbuf.write = bsize-2;
                    rbuf.incoming = bsize-2;
                    CHECK(RBUF_getFreeSpace(&rbuf) == 1);
                }

                SUBCASE("buffer full")
                {
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    CHECK(RBUF_getFreeSpace(&rbuf) == 0);
                }

                SUBCASE("wrap around, one byte free")
                {
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    rbuf.read = 1;
                    CHECK(RBUF_getFreeSpace(&rbuf) == 1);
                }

                SUBCASE("wrap around, only 1 byte used")
                {
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    rbuf.read = bsize-2;
                    CHECK(RBUF_getFreeSpace(&rbuf) == bsize-2);
                }
            }
            SUBCASE("RBUF_getBytesToRead")
            {
                REQUIRE_CALL(hsup_mock, HSUP_vDisableInt()) .IN_SEQUENCE(rbuf_seq);
                REQUIRE_CALL(hsup_mock, HSUP_vEnableInt())  .IN_SEQUENCE(rbuf_seq);

                SUBCASE("buffer yet initialized")
                {
                    CHECK(RBUF_getBytesToRead(&rbuf) == 0);
                }

                SUBCASE("single byte in buffer")
                {
                    rbuf.write = 1;
                    CHECK(RBUF_getBytesToRead(&rbuf) == 1);
                }

                SUBCASE("buffer full")
                {
                    rbuf.write = bsize-1;
                    CHECK(RBUF_getBytesToRead(&rbuf) == bsize-1);
                }

                SUBCASE("wrap around")
                {
                    rbuf.read = bsize-1;
                    rbuf.write = 0;
                    CHECK(RBUF_getBytesToRead(&rbuf) == 1);
                }

                SUBCASE("incoming data --> write re-entrance")
                {
                    rbuf.read = 0;
                    rbuf.write = 100;
                    rbuf.incoming = 42;
                    rbuf.reentered = 1;
                    CHECK(RBUF_getBytesToRead(&rbuf) == 42);
                }

                SUBCASE("incoming data --> write re-entrance, wrap around")
                {
                    rbuf.read = bsize-1;
                    rbuf.write = 100;
                    rbuf.incoming = 1;
                    rbuf.reentered = 1;
                    CHECK(RBUF_getBytesToRead(&rbuf) == 2);
                }
            }


        }

        TEST_CASE("ring-buffer tests part 3 write)")
        {
            RBUF_init(&rbuf,buffer,bsize);
            std::fill(buffer, buffer + bsize, 0);
            SUBCASE("RBUF_writeData_u8")
            {
                    REQUIRE_CALL(hsup_mock, HSUP_vDisableInt()) .IN_SEQUENCE(rbuf_seq);
                    REQUIRE_CALL(hsup_mock, HSUP_vEnableInt())  .IN_SEQUENCE(rbuf_seq);

                SUBCASE("Buffer free")
                {
                    RBUF_writeData_u8(&rbuf, 42u);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == 1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == 1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[0] == 42);
                }
                SUBCASE("Buffer nearly full")
                {
                    rbuf.write = bsize-2;
                    rbuf.incoming = bsize-2;
                    CHECK(RBUF_writeData_u8(&rbuf, 42u) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-2] == 42);
                }

                SUBCASE("Buffer full")
                {
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    CHECK(RBUF_writeData_u8(&rbuf, 42u) == false);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-1] == 0);
                    CHECK(rbuf.buffer[bsize-2] == 0);
                }

                SUBCASE("Buffer wrapped around")
                {
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    rbuf.read = 1;
                    CHECK(RBUF_writeData_u8(&rbuf, 42u) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == 0);
                    CHECK(rbuf.read == 1);
                    CHECK(rbuf.incoming == 0);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-1] == 42);
                }
                SUBCASE("reentrant")
                {
                    rbuf.reentered = 1;
                    CHECK(RBUF_writeData_u8(&rbuf, 42u) == true);
                }
            }

            SUBCASE("RBUF_writeData_u16")
            {
                REQUIRE_CALL(hsup_mock, HSUP_vDisableInt()) .IN_SEQUENCE(rbuf_seq);
                REQUIRE_CALL(hsup_mock, HSUP_vEnableInt())  .IN_SEQUENCE(rbuf_seq);

                SUBCASE("Buffer free")
                {
                    CHECK(RBUF_writeData_u16(&rbuf, 0x1234u) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == 2);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == 2);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[0] == 0x12);
                    CHECK(rbuf.buffer[1] == 0x34);
                }

                SUBCASE("Buffer nearly full")
                {
                    rbuf.write = bsize-3;
                    rbuf.incoming = bsize-3;
                    CHECK(RBUF_writeData_u16(&rbuf, 0x1234u) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-3] == 0x12);
                    CHECK(rbuf.buffer[bsize-2] == 0x34);
                }

                SUBCASE("Buffer full")
                {
                    rbuf.write = bsize-2;
                    rbuf.incoming = bsize-2;
                    CHECK(RBUF_writeData_u16(&rbuf, 0x1234u) == false);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-2);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-2);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-3] == 0u); // nothing written here, as buffer is full
                    CHECK(rbuf.buffer[bsize-2] == 0u);
                    CHECK(rbuf.buffer[bsize-1] == 0u);
                }

                SUBCASE("Buffer wrapped around")
                {
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    rbuf.read = 2;
                    CHECK(RBUF_writeData_u16(&rbuf, 0x1234u) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == 1);
                    CHECK(rbuf.read == 2);
                    CHECK(rbuf.incoming == 1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-1] == 0x12);
                    CHECK(rbuf.buffer[0      ] == 0x34);
                }
            }
            SUBCASE("RBUF_writeData_u32")
            {
                REQUIRE_CALL(hsup_mock, HSUP_vDisableInt()) .IN_SEQUENCE(rbuf_seq);
                REQUIRE_CALL(hsup_mock, HSUP_vEnableInt())  .IN_SEQUENCE(rbuf_seq);

                SUBCASE("Buffer free")
                {
                    CHECK(RBUF_writeData_u32(&rbuf, 0x12345678u) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == 4);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == 4);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[0] == 0x12);
                    CHECK(rbuf.buffer[1] == 0x34);
                    CHECK(rbuf.buffer[2] == 0x56);
                    CHECK(rbuf.buffer[3] == 0x78);
                }

                SUBCASE("Buffer nearly full")
                {
                    rbuf.write = bsize-5;
                    rbuf.incoming = bsize-5;
                    CHECK(RBUF_writeData_u32(&rbuf, 0x12345678u) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-5] == 0x12);
                    CHECK(rbuf.buffer[bsize-4] == 0x34);
                    CHECK(rbuf.buffer[bsize-3] == 0x56);
                    CHECK(rbuf.buffer[bsize-2] == 0x78);
                }

                SUBCASE("Buffer full")
                {
                    rbuf.write = bsize-4;
                    rbuf.incoming = bsize-4;
                    CHECK(RBUF_writeData_u32(&rbuf, 0x12345678u) == false);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-4);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-4);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-4] == 0u); // nothing written here, as buffer is full                
                    CHECK(rbuf.buffer[bsize-3] == 0u); // nothing written here, as buffer is full
                    CHECK(rbuf.buffer[bsize-2] == 0u);
                    CHECK(rbuf.buffer[bsize-1] == 0u);
                }
            }
            SUBCASE("RBUF_writeData_u64")
            {
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());

                SUBCASE("Buffer free")
                {
                    CHECK(RBUF_writeData_u64(&rbuf, 0x0123456789abcdefULL) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == 8);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == 8);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[0] == 0x01);
                    CHECK(rbuf.buffer[1] == 0x23);
                    CHECK(rbuf.buffer[2] == 0x45);
                    CHECK(rbuf.buffer[3] == 0x67);
                    CHECK(rbuf.buffer[4] == 0x89);
                    CHECK(rbuf.buffer[5] == 0xab);
                    CHECK(rbuf.buffer[6] == 0xcd);
                    CHECK(rbuf.buffer[7] == 0xef);
                }


                SUBCASE("Buffer nearly full")
                {
                    rbuf.write = bsize-9;
                    rbuf.incoming = bsize-9;
                    CHECK(RBUF_writeData_u64(&rbuf, 0x0123456789abcdefULL) == true);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-9] == 0x01);
                    CHECK(rbuf.buffer[bsize-8] == 0x23);
                    CHECK(rbuf.buffer[bsize-7] == 0x45);
                    CHECK(rbuf.buffer[bsize-6] == 0x67);
                    CHECK(rbuf.buffer[bsize-5] == 0x89);
                    CHECK(rbuf.buffer[bsize-4] == 0xab);
                    CHECK(rbuf.buffer[bsize-3] == 0xcd);
                    CHECK(rbuf.buffer[bsize-2] == 0xef);
                }

                SUBCASE("Buffer full")
                {
                    rbuf.write = bsize-8;
                    rbuf.incoming = bsize-8;
                    CHECK(RBUF_writeData_u64(&rbuf, 0x0123456789abcdefULL) == false);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-8);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-8);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-4] == 0u); // nothing written here, as buffer is full                
                    CHECK(rbuf.buffer[bsize-3] == 0u); // nothing written here, as buffer is full
                    CHECK(rbuf.buffer[bsize-2] == 0u);
                    CHECK(rbuf.buffer[bsize-1] == 0u);
                }
            }
            SUBCASE("RBUF_writeBuffN: zero length")
            {
                CHECK(RBUF_writeBuffN(&rbuf, randomData, 0u) == 0);
                CHECK(rbuf.size == bsize);
                CHECK(rbuf.write == 0);
                CHECK(rbuf.read == 0);
                CHECK(rbuf.incoming == 0);
                CHECK(rbuf.reentered == 0);
            }

            SUBCASE("RBUF_writeBuffN: write some data into empty buffer")
            {
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());

                SUBCASE("RBUF_writeBuffN: write some data into empty buffer")
                {
                    CHECK(RBUF_writeBuffN(&rbuf, randomData, 7u) == 7);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == 7);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == 7);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[0] == 21 );
                    CHECK(rbuf.buffer[1] == 147);
                    CHECK(rbuf.buffer[2] == 205);
                    CHECK(rbuf.buffer[3] == 209);
                    CHECK(rbuf.buffer[4] == 25 );
                    CHECK(rbuf.buffer[5] == 44 );
                    CHECK(rbuf.buffer[6] == 42 );
                    CHECK(rbuf.buffer[7] == 0  );
                }

                SUBCASE("write some data into nearly full buffer")
                {
                    rbuf.write = bsize-8;
                    rbuf.incoming = bsize-8;
                    CHECK(RBUF_writeBuffN(&rbuf, randomData, 7u) == 7);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-9] == 0  );
                    CHECK(rbuf.buffer[bsize-8] == 21 );
                    CHECK(rbuf.buffer[bsize-7] == 147);
                    CHECK(rbuf.buffer[bsize-6] == 205);
                    CHECK(rbuf.buffer[bsize-5] == 209);
                    CHECK(rbuf.buffer[bsize-4] == 25 );
                    CHECK(rbuf.buffer[bsize-3] == 44 );
                    CHECK(rbuf.buffer[bsize-2] == 42 );
                    CHECK(rbuf.buffer[bsize-1] == 0  );
                }

                SUBCASE("try to write data, data don't fit")
                {
                    rbuf.write = bsize-7;
                    rbuf.incoming = bsize-7;
                    CHECK(RBUF_writeBuffN(&rbuf, randomData, 7u) == 6);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-8] == 0  );
                    CHECK(rbuf.buffer[bsize-7] == 21 );
                    CHECK(rbuf.buffer[bsize-6] == 147);
                    CHECK(rbuf.buffer[bsize-5] == 205);
                    CHECK(rbuf.buffer[bsize-4] == 209);
                    CHECK(rbuf.buffer[bsize-3] == 25 );
                    CHECK(rbuf.buffer[bsize-2] == 44 );
                    CHECK(rbuf.buffer[bsize-1] == 0  );
                    CHECK(rbuf.buffer[bsize-1] == 0  );
                }

                SUBCASE("try to write data, buffer full")
                {
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    CHECK(RBUF_writeBuffN(&rbuf, randomData, 7u) == 0);
                    CHECK(rbuf.size == bsize);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.reentered == 0);
                    CHECK(rbuf.buffer[bsize-8] == 0  );
                    CHECK(rbuf.buffer[bsize-7] == 0  );
                    CHECK(rbuf.buffer[bsize-6] == 0  );
                    CHECK(rbuf.buffer[bsize-5] == 0  );
                    CHECK(rbuf.buffer[bsize-4] == 0  );
                    CHECK(rbuf.buffer[bsize-3] == 0  );
                    CHECK(rbuf.buffer[bsize-2] == 0  );
                    CHECK(rbuf.buffer[bsize-1] == 0  );
                    CHECK(rbuf.buffer[bsize-1] == 0  );
                }
            }
            SUBCASE("RBUF_writeBuffN: reentered writing, enough space")
            {
                rbuf.write = 50;
                rbuf.incoming = 25;
                rbuf.reentered = 1;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());
                CHECK(RBUF_writeBuffN(&rbuf, randomData, 7u) == 7);
                CHECK(rbuf.size == bsize);
                CHECK(rbuf.write == 57);
                CHECK(rbuf.read == 0);
                CHECK(rbuf.incoming == 25);
                CHECK(rbuf.reentered == 1);
                CHECK(rbuf.buffer[49] ==   0);                
                CHECK(rbuf.buffer[50] ==  21);
                CHECK(rbuf.buffer[51] == 147);
                CHECK(rbuf.buffer[52] == 205);
                CHECK(rbuf.buffer[53] == 209);
                CHECK(rbuf.buffer[54] == 25 );
                CHECK(rbuf.buffer[55] == 44 );
                CHECK(rbuf.buffer[56] == 42 );
                CHECK(rbuf.buffer[57] == 0  );
            }

            SUBCASE("RBUF_writeBuffN: reentered overflow")
            {
                rbuf.write = 50;
                rbuf.incoming = 25;
                rbuf.reentered = RBUF_WRITE_REENTRANT;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());
                CHECK(RBUF_writeBuffN(&rbuf, randomData, 7u) == 0);
            }


            SUBCASE("RBUF_writeBuff")
            {
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());

                SUBCASE("special case:write zero length")
                {
                    CHECK(RBUF_writeBuff(&rbuf, randomData, 0) == true);
                    CHECK(rbuf.write == 0);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == 0);
                }
                SUBCASE("enough space")
                {
                    CHECK(RBUF_writeBuff(&rbuf, randomData, 7u) == true);
                    CHECK(rbuf.write == 7);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == 7);
                    CHECK(rbuf.buffer[0] ==  21);
                    CHECK(rbuf.buffer[1] == 147);
                    CHECK(rbuf.buffer[2] == 205);
                    CHECK(rbuf.buffer[3] == 209);
                    CHECK(rbuf.buffer[4] == 25 );
                    CHECK(rbuf.buffer[5] == 44 );
                    CHECK(rbuf.buffer[6] == 42 );
                    CHECK(rbuf.buffer[7] == 0  );
                }

                SUBCASE("just enough space")
                {
                    rbuf.write = bsize-8;

                    CHECK(RBUF_writeBuff(&rbuf, randomData, 7u) == true);
                    CHECK(rbuf.write == bsize-1);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.incoming == bsize-1);
                    CHECK(rbuf.buffer[bsize-8] ==  21);
                    CHECK(rbuf.buffer[bsize-7] == 147);
                    CHECK(rbuf.buffer[bsize-6] == 205);
                    CHECK(rbuf.buffer[bsize-5] == 209);
                    CHECK(rbuf.buffer[bsize-4] == 25 );
                    CHECK(rbuf.buffer[bsize-3] == 44 );
                    CHECK(rbuf.buffer[bsize-2] == 42 );
                    CHECK(rbuf.buffer[bsize-1] == 0  );
                }

                SUBCASE("not enough space")
                {
                    rbuf.write = bsize-7;

                    CHECK(RBUF_writeBuff(&rbuf, randomData, 7u) == false);
                    CHECK(rbuf.write == bsize-7);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.buffer[bsize-8] == 0 );
                    CHECK(rbuf.buffer[bsize-7] == 0 );
                    CHECK(rbuf.buffer[bsize-6] == 0 );
                    CHECK(rbuf.buffer[bsize-5] == 0 );
                    CHECK(rbuf.buffer[bsize-4] == 0 );
                    CHECK(rbuf.buffer[bsize-3] == 0 );
                    CHECK(rbuf.buffer[bsize-2] == 0 );
                    CHECK(rbuf.buffer[bsize-1] == 0 );
                }

                SUBCASE("wrap around")
                {
                    rbuf.write = bsize-6;
                    rbuf.read = 2;
                    CHECK(RBUF_writeBuff(&rbuf, randomData, 7u) == true);
                    CHECK(rbuf.write == 1);
                    CHECK(rbuf.read == 2);
                    CHECK(rbuf.buffer[bsize-7] == 0  );
                    CHECK(rbuf.buffer[bsize-6] == 21 );
                    CHECK(rbuf.buffer[bsize-5] == 147);
                    CHECK(rbuf.buffer[bsize-4] == 205);
                    CHECK(rbuf.buffer[bsize-3] == 209);
                    CHECK(rbuf.buffer[bsize-2] == 25 );
                    CHECK(rbuf.buffer[bsize-1] == 44 );
                    CHECK(rbuf.buffer[0      ] == 42  );
                }
            }
            SUBCASE("RBUF_writeStr")
            {
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());

                SUBCASE("enough space")
                {
                    rbuf.buffer[11] = 42; // for checking if string terminator is NOT written
                    CHECK(RBUF_writeStr(&rbuf, "Hello World") == true);
                    CHECK(rbuf.write == 11);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.buffer[0]  == 'H');
                    CHECK(rbuf.buffer[1]  == 'e');
                    CHECK(rbuf.buffer[2]  == 'l');
                    CHECK(rbuf.buffer[3]  == 'l');
                    CHECK(rbuf.buffer[4]  == 'o');
                    CHECK(rbuf.buffer[5]  == ' ');
                    CHECK(rbuf.buffer[6]  == 'W');
                    CHECK(rbuf.buffer[7]  == 'o');
                    CHECK(rbuf.buffer[8]  == 'r');
                    CHECK(rbuf.buffer[9]  == 'l');
                    CHECK(rbuf.buffer[10] == 'd');
                    CHECK(rbuf.buffer[11] == 42); // no string terminator!
                }

                SUBCASE("just enough space")
                {
                    rbuf.size = 12;
                    rbuf.buffer[11] = 42; // for checking if string terminator is NOT written
                    CHECK(RBUF_writeStr(&rbuf, "Hello World") == true);
                    CHECK(rbuf.write == 11);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.buffer[0]  == 'H');
                    CHECK(rbuf.buffer[1]  == 'e');
                    CHECK(rbuf.buffer[2]  == 'l');
                    CHECK(rbuf.buffer[3]  == 'l');
                    CHECK(rbuf.buffer[4]  == 'o');
                    CHECK(rbuf.buffer[5]  == ' ');
                    CHECK(rbuf.buffer[6]  == 'W');
                    CHECK(rbuf.buffer[7]  == 'o');
                    CHECK(rbuf.buffer[8]  == 'r');
                    CHECK(rbuf.buffer[9]  == 'l');
                    CHECK(rbuf.buffer[10] == 'd');
                    CHECK(rbuf.buffer[11] == 42); // no string terminator!
                }
                SUBCASE("not enough space")
                {
                    rbuf.size = 11;
                    CHECK(RBUF_writeStr(&rbuf, "Hello World") == false);
                    CHECK(rbuf.write == 0);
                    CHECK(rbuf.read == 0);
                    CHECK(rbuf.buffer[0]  == 0);
                    CHECK(rbuf.buffer[1]  == 0);
                    CHECK(rbuf.buffer[2]  == 0);
                    CHECK(rbuf.buffer[3]  == 0);
                    CHECK(rbuf.buffer[4]  == 0);
                }
            }

        }
        TEST_CASE("ring-buffer tests part 4 read operations)")
        {
            uint8_t readBuffer[bsize];
            RBUF_init(&rbuf,buffer,bsize);
            std::fill(buffer, buffer + bsize, 0);
            std::fill(readBuffer, readBuffer + bsize, 0);

            SUBCASE("RBUF_readData_u8")
            {
                uint8_t data = 42;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());
            
                SUBCASE("nothing to read")
                {
                    CHECK(RBUF_readData_u8(&rbuf, &data) == false );
                    CHECK(data == 42);
                }

                SUBCASE("data available")
                {
                    (void)RBUF_writeData_u8(&rbuf, 0x34);
                    CHECK(RBUF_readData_u8(&rbuf, &data) == true );
                    CHECK(data == 0x34);
                }

                SUBCASE("RBUF_readData_u8: data available at the end of buffer")
                {
                    rbuf.read = bsize-2;
                    rbuf.write = bsize-2;
                    rbuf.incoming = bsize-2;
                    CHECK(RBUF_writeData_u8(&rbuf, 0x34) == true );
                    CHECK(RBUF_readData_u8(&rbuf, &data) == true );
                    CHECK(data == 0x34);
                }
            }
            SUBCASE("RBUF_readData_u16")
            {
                uint16_t data = 0x0815;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());

                SUBCASE("nothing to read")
                {
                    CHECK(RBUF_readData_u16(&rbuf, &data) == false );
                    CHECK(data == 0x0815);
                }

                SUBCASE("data available")
                {
                    (void)RBUF_writeData_u16(&rbuf, 0x1234);
                    CHECK(RBUF_readData_u16(&rbuf, &data) == true );
                    CHECK(data == 0x1234);
                }

                SUBCASE("data available, wrapped")
                {
                    rbuf.read = bsize-1;
                    rbuf.write = bsize-1;
                    rbuf.incoming = bsize-1;
                    (void)RBUF_writeData_u16(&rbuf, 0x1234);
                    CHECK(RBUF_readData_u16(&rbuf, &data) == true );
                    CHECK(data == 0x1234);
                }
            }
            SUBCASE("RBUF_readData_u32")
            {
                uint32_t data = 0x47110815;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());

                SUBCASE("nothing to read")
                {
                    CHECK(RBUF_readData_u32(&rbuf, &data) == false );
                    CHECK(data == 0x47110815UL);
                }

                SUBCASE("data available")
                {
                    (void)RBUF_writeData_u32(&rbuf, 0x12345678UL);
                    CHECK(RBUF_readData_u32(&rbuf, &data) == true );
                    CHECK(data == 0x12345678UL);
                }
                
                SUBCASE("data available, wrapped")
                {
                    rbuf.read = bsize-2;
                    rbuf.write = bsize-2;
                    rbuf.incoming = bsize-2;
                    (void)RBUF_writeData_u32(&rbuf, 0x12345678UL);
                    CHECK(RBUF_readData_u32(&rbuf, &data) == true );
                    CHECK(data == 0x12345678UL);
                }
            }
            SUBCASE("RBUF_readData_u64")
            {
                uint64_t data = 0x4711081512345678ULL;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());

                SUBCASE("nothing to read")
                {
                    CHECK(RBUF_readData_u64(&rbuf, &data) == false );
                    CHECK(data == 0x4711081512345678ULL);
                }

                SUBCASE("data available")
                {
                    (void)RBUF_writeData_u64(&rbuf, 0xfedcba9876543210ULL);
                    CHECK(RBUF_readData_u64(&rbuf, &data) == true );
                    CHECK(data == 0xfedcba9876543210ULL);
                }

                SUBCASE("data available, wrapped")
                {
                    rbuf.read = bsize-4;
                    rbuf.write = bsize-4;
                    rbuf.incoming = bsize-4;
                    (void)RBUF_writeData_u64(&rbuf, 0xfedcba9876543210ULL);
                    CHECK(RBUF_readData_u64(&rbuf, &data) == true );
                    CHECK(data == 0xfedcba9876543210ULL);
                }
            }

            SUBCASE("RBUF_readBuffN")
            {
                uint64_t data = 0x4711081512345678ULL;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());
                SUBCASE("no data available")
                {
                    CHECK(RBUF_readBuffN(&rbuf, readBuffer,bsize) == 0);
                }
                
                SUBCASE("some data available")
                {
                    (void)RBUF_writeData_u64(&rbuf, data);
                    CHECK(RBUF_readBuffN(&rbuf, readBuffer,bsize) == 8);
                    CHECK(readBuffer[0] == 0x47);
                    CHECK(readBuffer[1] == 0x11);
                    CHECK(readBuffer[2] == 0x08);
                    CHECK(readBuffer[3] == 0x15);
                    CHECK(readBuffer[4] == 0x12);
                    CHECK(readBuffer[5] == 0x34);
                    CHECK(readBuffer[6] == 0x56);
                    CHECK(readBuffer[7] == 0x78);
                }
                SUBCASE("some wrapped data available")
                {
                    rbuf.read = bsize-4;
                    rbuf.write = bsize-4;
                    rbuf.incoming = bsize-4;
                    (void)RBUF_writeData_u64(&rbuf, data);
                    CHECK(RBUF_readBuffN(&rbuf, readBuffer,bsize) == 8);
                    CHECK(readBuffer[0] == 0x47);
                    CHECK(readBuffer[1] == 0x11);
                    CHECK(readBuffer[2] == 0x08);
                    CHECK(readBuffer[3] == 0x15);
                    CHECK(readBuffer[4] == 0x12);
                    CHECK(readBuffer[5] == 0x34);
                    CHECK(readBuffer[6] == 0x56);
                    CHECK(readBuffer[7] == 0x78);
                }

                SUBCASE("some data available, max read 4 bytes")
                {
                    (void)RBUF_writeData_u64(&rbuf, data);
                    CHECK(RBUF_readBuffN(&rbuf, readBuffer,4) == 4);
                    CHECK(readBuffer[0] == 0x47);
                    CHECK(readBuffer[1] == 0x11);
                    CHECK(readBuffer[2] == 0x08);
                    CHECK(readBuffer[3] == 0x15);
                    CHECK(readBuffer[4] == 0x00);
                    CHECK(readBuffer[5] == 0x00);
                    CHECK(readBuffer[6] == 0x00);
                    CHECK(readBuffer[7] == 0x00);
                }

                SUBCASE("8 data bytes available, max read 12 bytes")
                {
                    (void)RBUF_writeData_u64(&rbuf, data);
                    CHECK(RBUF_readBuffN(&rbuf, readBuffer,12) == 8);
                    CHECK(readBuffer[0] == 0x47);
                    CHECK(readBuffer[1] == 0x11);
                    CHECK(readBuffer[2] == 0x08);
                    CHECK(readBuffer[3] == 0x15);
                    CHECK(readBuffer[4] == 0x12);
                    CHECK(readBuffer[5] == 0x34);
                    CHECK(readBuffer[6] == 0x56);
                    CHECK(readBuffer[7] == 0x78);
                }

            }

            SUBCASE("RBUF_readBuff")
            {
                uint64_t data = 0x4711081512345678ULL;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());
                SUBCASE("no data available")
                {
                    CHECK(RBUF_readBuff(&rbuf, readBuffer,1) == false);
                }
                SUBCASE("data available, but not enough")
                {
                    (void)RBUF_writeData_u64(&rbuf, data);
                    CHECK(RBUF_readBuff(&rbuf, readBuffer,12) == false);
                }
                SUBCASE("data available")
                {
                    (void)RBUF_writeData_u64(&rbuf, data);
                    CHECK(RBUF_readBuff(&rbuf, readBuffer,8) == true);
                    CHECK(readBuffer[0] == 0x47);
                    CHECK(readBuffer[1] == 0x11);
                    CHECK(readBuffer[2] == 0x08);
                    CHECK(readBuffer[3] == 0x15);
                    CHECK(readBuffer[4] == 0x12);
                    CHECK(readBuffer[5] == 0x34);
                    CHECK(readBuffer[6] == 0x56);
                    CHECK(readBuffer[7] == 0x78);
                }

            }
            SUBCASE("RBUF_readAll")
            {
                uint64_t data = 0x4711081512345678ULL;
                ALLOW_CALL(hsup_mock, HSUP_vDisableInt());
                ALLOW_CALL(hsup_mock, HSUP_vEnableInt());
                SUBCASE("no data available")
                {
                    CHECK(RBUF_readAll(&rbuf, readBuffer,10) == 0);
                }
                SUBCASE("some data available")
                {
                    (void)RBUF_writeData_u64(&rbuf, data);
                    CHECK(RBUF_readAll(&rbuf, readBuffer,10) == 8);
                }
                SUBCASE("to much data for buffer, maximum amount of data read")
                {
                    (void)RBUF_writeData_u64(&rbuf, data);                    
                    CHECK(RBUF_readAll(&rbuf, readBuffer,6) == 6);
                }


            }

        }

    }
}