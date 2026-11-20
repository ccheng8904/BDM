/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#ifndef MCOM_MOCK_H
#define MCOM_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace mcom_m
{
    #include "mcal/mcom.h"

    class mcom_mock_class
    {
    public:
        MAKE_MOCK2(MCOM_pushByte                 , void              ( MCOM_Transfer *transfer, uint8_t data ));
        MAKE_MOCK1(MCOM_popByte                  , uint8_t           ( MCOM_Transfer *transfer ));
        MAKE_MOCK3(MCOM_setWriteBuffer           , void              ( MCOM_Transfer *transfer, const uint8_t *buffer, uint16_t length ));
        MAKE_MOCK3(MCOM_setReadBuffer            , void              ( MCOM_Transfer *transfer, uint8_t *buffer, uint16_t length ));
        MAKE_MOCK1(MCOM_isReadReady              , bool              ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_isWriteReady             , bool              ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_isReadBusy               , bool              ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_isWriteBusy              , bool              ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_isBusy                   , bool              ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_isReadComplete           , bool              ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_isWriteComplete          , bool              ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_setReadComplete          , void              ( MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_setWriteComplete         , void              ( MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_setReadReady             , void              ( MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_setWriteReady            , void              ( MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_getNumOfReadData         , uint16_t          ( const MCOM_Transfer *transfer ));
        MAKE_MOCK1(MCOM_getNumOfDataToWrite      , uint16_t          ( const MCOM_Transfer *transfer ));
    };

    extern mcom_mock_class mcom_mock;
}

using namespace mcom_m;

#endif  // MCOM_MOCK_H
