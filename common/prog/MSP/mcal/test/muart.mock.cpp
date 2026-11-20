/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*  are protected by international copyright laws.
*
*  All rights reserved. This program and the accompanying materials
*  Please contact copyright holder for licensing information.
*******************************************************************************/

#include "muart.mock.h"

namespace muart_m
{
    muart_mock_class muart_mock;

    enum MCAL_Error MUART_init(struct MUART_Handle *handle, const struct MUART_Channel *channel, const struct MUART_Config *cfg)
    {
        return muart_mock.MUART_init(*handle, *channel, *cfg);
    }

    void MUART_enable(struct MUART_Handle *handle)
    {
        muart_mock.MUART_enable(*handle);
    }

    void MUART_disable(struct MUART_Handle *handle)
    {
        muart_mock.MUART_disable(*handle);
    }

    void MUART_setNewBaudrate(struct MUART_Handle *handle, uint32_t baudrate)
    {
        muart_mock.MUART_setNewBaudrate(*handle, baudrate);
    }

    enum MCAL_Error MUART_configurePins(const struct MDIO_Channel *rxPin, const struct MDIO_Channel *txPin)
    {
        return muart_mock.MUART_configurePins(*rxPin, *txPin);
    }

    enum MCAL_Error MUART_write(struct MUART_Handle *handle, const uint8_t buffer[], uint16_t length)
    {
        return muart_mock.MUART_write(*handle, length);
    }

    enum MCAL_Error MUART_read(struct MUART_Handle *handle, uint8_t buffer[], uint16_t length)
    {
        return muart_mock.MUART_read(*handle, length);
    }

    void MUART_finishRead(struct MUART_Handle *handle)
    {
        muart_mock.MUART_finishRead(*handle);
    }

    int32_t MUART_writeByte(const struct MUART_Handle *handle, uint8_t data)
    {
        return muart_mock.MUART_writeByte(*handle, data);
    }

    int32_t MUART_readByte(const struct MUART_Handle *handle)
    {
        return muart_mock.MUART_readByte(*handle);
    }

    enum MCAL_Status MUART_getStatus(const struct MUART_Handle *handle)
    {
        return muart_mock.MUART_getStatus(*handle);
    }

    enum MCAL_Status MUART_getReadStatus(const struct MUART_Handle *handle)
    {
        return muart_mock.MUART_getReadStatus(*handle);
    }

    enum MCAL_Status MUART_getWriteStatus(const struct MUART_Handle *handle)
    {
        return muart_mock.MUART_getWriteStatus(*handle);
    }

    enum MCAL_Error MUART_getError(const struct MUART_Handle *handle)
    {
        return muart_mock.MUART_getError(*handle);
    }

    void MUART_clearError(struct MUART_Handle *handle)
    {
        muart_mock.MUART_clearError(*handle);
    }

    enum MCAL_Error MUART_enableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
    {
        return muart_mock.MUART_enableEvent(*handle, *callback, event);
    }

    enum MCAL_Error MUART_disableEvent(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
    {
        return muart_mock.MUART_disableEvent(*handle, *callback, event);
    }

    struct MUART_Handle* MUART_getHandle(const void *handlePtr)
    {
        return muart_mock.MUART_getHandle(*handlePtr);
    }

    enum MCAL_Error MUART_writeBlocking(const struct MUART_Handle *handle, const uint8_t buffer[], uint16_t length)
    {
        return muart_mock.MUART_writeBlocking(*handle, length);
    }

}

