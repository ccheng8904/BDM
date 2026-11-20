/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include "mcom.mock.h"

namespace mcom_m
{
    mcom_mock_class mcom_mock;

    void MCOM_pushByte(struct MCOM_Transfer *transfer, uint8_t data)
    {
        mcom_mock.MCOM_pushByte(transfer, data);
    }

    uint8_t MCOM_popByte(struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_popByte(transfer);
    }

    void MCOM_setWriteBuffer(struct MCOM_Transfer *transfer, const uint8_t *buffer, uint16_t length)
    {
        mcom_mock.MCOM_setWriteBuffer(transfer, buffer, length);
    }

    void MCOM_setReadBuffer(struct MCOM_Transfer *transfer, uint8_t *buffer, uint16_t length)
    {
        mcom_mock.MCOM_setReadBuffer(transfer, buffer, length);
    }

    bool MCOM_isReadReady(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_isReadReady(transfer);
    }

    bool MCOM_isWriteReady(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_isWriteReady(transfer);
    }

    bool MCOM_isReadBusy(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_isReadBusy(transfer);
    }

    bool MCOM_isWriteBusy(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_isWriteBusy(transfer);
    }

    bool MCOM_isBusy(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_isBusy(transfer);
    }

    bool MCOM_isReadComplete(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_isReadComplete(transfer);
    }

    bool MCOM_isWriteComplete(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_isWriteComplete(transfer);
    }

    void MCOM_setReadComplete(struct MCOM_Transfer *transfer)
    {
        mcom_mock.MCOM_setReadComplete(transfer);
    }

    void MCOM_setWriteComplete(struct MCOM_Transfer *transfer)
    {
        mcom_mock.MCOM_setWriteComplete(transfer);
    }

    void MCOM_setReadReady(struct MCOM_Transfer *transfer)
    {
        mcom_mock.MCOM_setReadReady(transfer);
    }

    void MCOM_setWriteReady(struct MCOM_Transfer *transfer)
    {
        mcom_mock.MCOM_setWriteReady(transfer);
    }

    uint16_t MCOM_getNumOfReadData(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_getNumOfReadData(transfer);
    }

    uint16_t MCOM_getNumOfDataToWrite(const struct MCOM_Transfer *transfer)
    {
        return mcom_mock.MCOM_getNumOfDataToWrite(transfer);
    }

}

