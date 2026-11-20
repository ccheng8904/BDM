/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>
#include <algorithm>

/* Your mocking includes here
   NOTE: The include order is important! lowest level need to be included first.
         Cross include dependencies creates troubles!!*/

using trompeloeil::_;

namespace mcom_test
{
    #include "mcal/mcom.h"

    /************************** local mocks ***********************************
     *  Use macro MOCKI(function-name)(..) to mock internal static interfaces.
     *  Use macro MOCKE(..)(..) to mock interfaces of the unit.
     *  Mocking of static internal interface should be prevented. Use it to break
     *  endless loops and to reduce high complexity.
     */
    class
    {
      // MAKE_MOCK1(XXX_isConvReady      , bool        ( const XXX_Handle *handle ));
    }int_mock;

    class
    {
      // MAKE_MOCK2(XXX_readSample       , uint16_t    ( XXX_Handle *handle, const XXX_Channel *channel ));
    }ext_mock;

    /**************************************************************************/

    // include the implementation of 'mcom' into the current namespace
    #include "mcal/mcom.c"
    
    MCOM_Transfer transferObj;
    constexpr uint16_t blen = 3;
    uint8_t writeBuffer[blen]{0};
    uint8_t readBuffer[blen]{0};


    void initTestData(void)
    {
        std::fill(writeBuffer, writeBuffer + blen, 0);
        std::fill(readBuffer, readBuffer + blen, 0);
        transferObj.readBufferHandle.status = (MCOM_Status)0xFF;
        transferObj.readBufferHandle.buffer.readBuffer = nullptr;
        transferObj.readBufferHandle.length = 0xff;
        transferObj.readBufferHandle.index  = 0xff;
        transferObj.writeBufferHandle.status = (MCOM_Status)0xFF;
        transferObj.writeBufferHandle.buffer.writeBuffer = nullptr;
        transferObj.writeBufferHandle.length = 0xff;
        transferObj.writeBufferHandle.index  = 0xff;
    }

    TEST_SUITE("mcom")
    {
        trompeloeil::sequence seq;
        
        TEST_CASE("MCOM_pushByte")
        {
            initTestData();
            MCOM_setReadBuffer(&transferObj, readBuffer, blen);

            SUBCASE("push sequence")
            {   //void MCOM_pushByte(struct MCOM_Transfer *transfer, uint8_t data)
                MCOM_pushByte(&transferObj, 0x11);
                CHECK(transferObj.readBufferHandle.status == MCOM_STATUS_BUSY);
                CHECK(transferObj.readBufferHandle.index  == 1);
                MCOM_pushByte(&transferObj, 0x22);
                CHECK(transferObj.readBufferHandle.status == MCOM_STATUS_BUSY);
                CHECK(transferObj.readBufferHandle.index  == 2);
                MCOM_pushByte(&transferObj, 0x33);
                CHECK(transferObj.readBufferHandle.status == MCOM_STATUS_COMPLETE);
                CHECK(transferObj.readBufferHandle.index  >= 2);
            }
        }

        TEST_CASE("MCOM_popByte")
        {
            writeBuffer[0] = 0x12;
            writeBuffer[1] = 0x34;
            writeBuffer[2] = 0x56;
            MCOM_setWriteBuffer(&transferObj, writeBuffer, blen);
            SUBCASE("Check....")
            {   //uint8_t MCOM_popByte(struct MCOM_Transfer *transfer)
                CHECK(MCOM_popByte(&transferObj) == 0x12);
                CHECK(transferObj.writeBufferHandle.status == MCOM_STATUS_BUSY);
                CHECK(transferObj.writeBufferHandle.index  == 1);
                CHECK(MCOM_popByte(&transferObj) == 0x34);
                CHECK(transferObj.writeBufferHandle.status == MCOM_STATUS_BUSY);
                CHECK(transferObj.writeBufferHandle.index  == 2);
                CHECK(MCOM_popByte(&transferObj) == 0x56);
                CHECK(transferObj.writeBufferHandle.status == MCOM_STATUS_COMPLETE);
                CHECK(transferObj.writeBufferHandle.index  >= 2);
                CHECK(MCOM_popByte(&transferObj) == 0);

            }
        }

        TEST_CASE("MCOM_setWriteBuffer")
        {
            initTestData();
            SUBCASE("normal initialization")
            {   //void MCOM_setWriteBuffer(struct MCOM_Transfer *transfer, const uint8_t *buffer, uint16_t length)
                MCOM_setWriteBuffer(&transferObj, writeBuffer, blen);
                CHECK(transferObj.writeBufferHandle.status == MCOM_STATUS_BUSY);
                CHECK(transferObj.writeBufferHandle.buffer.writeBuffer == writeBuffer);
                CHECK(transferObj.writeBufferHandle.length == blen);
                CHECK(transferObj.writeBufferHandle.index == 0u);
            }
            SUBCASE("transfer obj is nullptr")
            {   //void MCOM_setWriteBuffer(struct MCOM_Transfer *transfer, const uint8_t *buffer, uint16_t length)
                MCOM_setWriteBuffer(nullptr, writeBuffer, blen);
                CHECK(transferObj.writeBufferHandle.status == 0xffu);
                CHECK(transferObj.writeBufferHandle.buffer.writeBuffer == nullptr);
                CHECK(transferObj.writeBufferHandle.length == 0xffu);
                CHECK(transferObj.writeBufferHandle.index == 0xffu);
            }

        }

        TEST_CASE("MCOM_setReadBuffer")
        {
            initTestData();
            SUBCASE("normal initialization")
            {   //void MCOM_setReadBuffer(struct MCOM_Transfer *transfer, uint8_t *buffer, uint16_t length)
                MCOM_setReadBuffer(&transferObj, readBuffer, blen);
                CHECK(transferObj.readBufferHandle.status == MCOM_STATUS_BUSY);
                CHECK(transferObj.readBufferHandle.buffer.readBuffer == readBuffer);
                CHECK(transferObj.readBufferHandle.length == blen);
                CHECK(transferObj.readBufferHandle.index == 0u);
            }
            SUBCASE("transfer obj is nullptr")
            {   //void MCOM_setReadBuffer(struct MCOM_Transfer *transfer, uint8_t *buffer, uint16_t length)
                MCOM_setReadBuffer(nullptr, readBuffer, blen);
                CHECK(transferObj.readBufferHandle.status == 0xffu);
                CHECK(transferObj.readBufferHandle.buffer.readBuffer == nullptr);
                CHECK(transferObj.readBufferHandle.length == 0xffu);
                CHECK(transferObj.readBufferHandle.index == 0xffu);
            }
        }


        TEST_CASE("MCOM_isReadReady")
        {
            SUBCASE("Check for true and false")
            {   //bool MCOM_isReadReady(const struct MCOM_Transfer *transfer)
                transferObj.readBufferHandle.status = MCOM_STATUS_READY;
                CHECK(MCOM_isReadReady(&transferObj) == true);
                transferObj.readBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isReadReady(&transferObj) == false);
            }
        }

        TEST_CASE("MCOM_isWriteReady")
        {
            SUBCASE("Check for true and false")
            {   //bool MCOM_isWriteReady(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.status = MCOM_STATUS_READY;
                CHECK(MCOM_isWriteReady(&transferObj) == true);
                transferObj.writeBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isWriteReady(&transferObj) == false);
            }
        }

        TEST_CASE("MCOM_isReadBusy")
        {
            SUBCASE("Check for true and false")
            {   //bool MCOM_isReadBusy(const struct MCOM_Transfer *transfer)
                transferObj.readBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isReadBusy(&transferObj) == true);
                transferObj.readBufferHandle.status = MCOM_STATUS_READY;
                CHECK(MCOM_isReadBusy(&transferObj) == false);
            }
        }

        TEST_CASE("MCOM_isWriteBusy")
        {
            SUBCASE("Check....")
            {   //bool MCOM_isWriteBusy(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isWriteBusy(&transferObj) == true);
                transferObj.writeBufferHandle.status = MCOM_STATUS_READY;
                CHECK(MCOM_isWriteBusy(&transferObj) == false);
            }
        }

        TEST_CASE("MCOM_isBusy")
        {
            SUBCASE("read and write both not busy")
            {   //bool MCOM_isBusy(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.status = MCOM_STATUS_READY;
                transferObj.readBufferHandle.status = MCOM_STATUS_READY;
                CHECK(MCOM_isBusy(&transferObj) == false);
            }
            SUBCASE("write busy, read not")
            {   //bool MCOM_isBusy(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.status = MCOM_STATUS_BUSY;
                transferObj.readBufferHandle.status = MCOM_STATUS_READY;
                CHECK(MCOM_isBusy(&transferObj) == true);
            }
            SUBCASE("read busy, write not")
            {   //bool MCOM_isBusy(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.status = MCOM_STATUS_READY;
                transferObj.readBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isBusy(&transferObj) == true);
            }
            SUBCASE("read and write busy")
            {   //bool MCOM_isBusy(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.status = MCOM_STATUS_BUSY;
                transferObj.readBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isBusy(&transferObj) == true);
            }
            
        }

        TEST_CASE("MCOM_isReadComplete")
        {
            SUBCASE("Check for true and false")
            {   //bool MCOM_isReadComplete(const struct MCOM_Transfer *transfer)
                transferObj.readBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isReadComplete(&transferObj) == false);
                transferObj.readBufferHandle.status = MCOM_STATUS_COMPLETE;
                CHECK(MCOM_isReadComplete(&transferObj) == true);
            }
        }

        TEST_CASE("MCOM_isWriteComplete")
        {
            SUBCASE("Check for true and false")
            {   //bool MCOM_isReadComplete(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.status = MCOM_STATUS_BUSY;
                CHECK(MCOM_isWriteComplete(&transferObj) == false);
                transferObj.writeBufferHandle.status = MCOM_STATUS_COMPLETE;
                CHECK(MCOM_isWriteComplete(&transferObj) == true);
            }
        }

        TEST_CASE("MCOM_setReadComplete")
        {
            initTestData();
            SUBCASE("Check status after call")
            {   //void MCOM_setReadComplete(struct MCOM_Transfer *transfer)
                MCOM_setReadComplete(&transferObj);
                CHECK(transferObj.readBufferHandle.status == MCOM_STATUS_COMPLETE);
            }
        }

        TEST_CASE("MCOM_setWriteComplete")
        {   
            initTestData();
            SUBCASE("Check status after call")
            {   //void MCOM_setWriteComplete(struct MCOM_Transfer *transfer)
                MCOM_setWriteComplete(&transferObj);
                CHECK(transferObj.writeBufferHandle.status == MCOM_STATUS_COMPLETE);
            }
        }

        TEST_CASE("MCOM_setReadReady")
        {
            initTestData();
            SUBCASE("Check status after call")
            {   //void MCOM_setReadReady(struct MCOM_Transfer *transfer)
                MCOM_setReadReady(&transferObj);
                CHECK(transferObj.readBufferHandle.status == MCOM_STATUS_READY);
                CHECK(true);
            }
        }

        TEST_CASE("MCOM_setWriteReady")
        {
            initTestData();
            SUBCASE("Check status after call")
            {   //void MCOM_setWriteReady(struct MCOM_Transfer *transfer)
                MCOM_setWriteReady(&transferObj);
                CHECK(transferObj.writeBufferHandle.status == MCOM_STATUS_READY);
                CHECK(true);
            }
        }

        TEST_CASE("MCOM_getNumOfReadData")
        {
            SUBCASE("Check....")
            {   //uint16_t MCOM_getNumOfReadData(const struct MCOM_Transfer *transfer)
                transferObj.readBufferHandle.index = 55;
                CHECK(MCOM_getNumOfReadData(&transferObj) == 55);
            }
        }

        TEST_CASE("MCOM_getNumOfDataToWrite")
        {
            SUBCASE("nothing to write")
            {   //uint16_t MCOM_getNumOfDataToWrite(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.index  = 2;
                transferObj.writeBufferHandle.length = 3;
                CHECK(MCOM_getNumOfDataToWrite(&transferObj) == 1);
            }
            SUBCASE("no plausible index index == length")
            {   //uint16_t MCOM_getNumOfDataToWrite(const struct MCOM_Transfer *transfer)
                transferObj.writeBufferHandle.index  = 3;
                transferObj.writeBufferHandle.length = 3;
                CHECK(MCOM_getNumOfDataToWrite(&transferObj) == 0);
            }

        }

    }
}

