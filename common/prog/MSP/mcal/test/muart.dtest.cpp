/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*  are protected by international copyright laws.
*
*  All rights reserved. This program and the accompanying materials
*  Please contact copyright holder for licensing information.
*******************************************************************************/

#include <doctest/doctest.h>
/* Your mocking includes here */

namespace muart_test
{
    // include the implementation of 'muart' into the current namespace
    #include "mcal/muart.c"

    TEST_SUITE("muart test suit")
    {
        /* do here the common stuff e.g. initialization
         * it is executed before each test case.        */
        SUBTEST("MUART_init: Check...")
        {   //enum MCAL_Error MUART_init(struct MUART_Handle *handle, const struct MUART_Channel *channel, const struct MUART_Config *cfg)
            //enum MCAL_Error ret = MUART_init(&handle, &channel, &cfg);
            CHECK(true);
        }

        SUBTEST("MUART_enable: Check...")
        {   //void MUART_enable(struct MUART_Handle *handle)
            //MUART_enable(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_disable: Check...")
        {   //void MUART_disable(struct MUART_Handle *handle)
            //MUART_disable(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_setNewBaudrate: Check...")
        {   //void MUART_setNewBaudrate(struct MUART_Handle *handle, uint32_t baudrate)
            //MUART_setNewBaudrate(&handle, baudrate);
            CHECK(true);
        }

        SUBTEST("MUART_configurePins: Check...")
        {   //enum MCAL_Error MUART_configurePins(const struct MDIO_Channel *rxPin, const struct MDIO_Channel *txPin)
            //enum MCAL_Error ret = MUART_configurePins(&rxPin, &txPin);
            CHECK(true);
        }

        SUBTEST("MUART_write: Check...")
        {   //enum MCAL_Error MUART_write(struct MUART_Handle *handle, const uint8_t buffer[], uint16_t length)
            //enum MCAL_Error ret = MUART_write(&handle, length);
            CHECK(true);
        }

        SUBTEST("MUART_read: Check...")
        {   //enum MCAL_Error MUART_read(struct MUART_Handle *handle, uint8_t buffer[], uint16_t length)
            //enum MCAL_Error ret = MUART_read(&handle, length);
            CHECK(true);
        }

        SUBTEST("MUART_finishRead: Check...")
        {   //void MUART_finishRead(struct MUART_Handle *handle)
            //MUART_finishRead(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_writeByte: Check...")
        {   //int32_t MUART_writeByte(const struct MUART_Handle *handle, uint8_t data)
            //int32_t ret = MUART_writeByte(&handle, data);
            CHECK(true);
        }

        SUBTEST("MUART_readByte: Check...")
        {   //int32_t MUART_readByte(const struct MUART_Handle *handle)
            //int32_t ret = MUART_readByte(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_getStatus: Check...")
        {   //enum MCAL_Status MUART_getStatus(const struct MUART_Handle *handle)
            //enum MCAL_Status ret = MUART_getStatus(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_getReadStatus: Check...")
        {   //enum MCAL_Status MUART_getReadStatus(const struct MUART_Handle *handle)
            //enum MCAL_Status ret = MUART_getReadStatus(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_getWriteStatus: Check...")
        {   //enum MCAL_Status MUART_getWriteStatus(const struct MUART_Handle *handle)
            //enum MCAL_Status ret = MUART_getWriteStatus(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_getError: Check...")
        {   //enum MCAL_Error MUART_getError(const struct MUART_Handle *handle)
            //enum MCAL_Error ret = MUART_getError(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_clearError: Check...")
        {   //void MUART_clearError(struct MUART_Handle *handle)
            //MUART_clearError(&handle);
            CHECK(true);
        }

        SUBTEST("MUART_enableEvent: Check...")
        {   //enum MCAL_Error MUART_enableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
            //enum MCAL_Error ret = MUART_enableEvent(&handle, &callback, event);
            CHECK(true);
        }

        SUBTEST("MUART_disableEvent: Check...")
        {   //enum MCAL_Error MUART_disableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
            //enum MCAL_Error ret = MUART_disableEvent(&handle, &callback, event);
            CHECK(true);
        }

        SUBTEST("MUART_getHandle: Check...")
        {   //struct MUART_Handle* MUART_getHandle(const void *handlePtr)
            //struct MUART_Handle* ret = MUART_getHandle(&handlePtr);
            CHECK(true);
        }

        SUBTEST("MUART_writeBlocking: Check...")
        {   //enum MCAL_Error MUART_writeBlocking(const struct MUART_Handle *handle, const uint8_t buffer[], uint16_t length)
            //enum MCAL_Error ret = MUART_writeBlocking(&handle, length);
            CHECK(true);
        }

    }
}

