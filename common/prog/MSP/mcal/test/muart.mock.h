/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*  are protected by international copyright laws.
*
*  All rights reserved. This program and the accompanying materials
*  Please contact copyright holder for licensing information.
*******************************************************************************/

#ifndef MUART_MOCK_H
#define MUART_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace muart_m
{
    #include "mcal/muart.h"

    class muart_mock_class
    {
    public:
        MAKE_MOCK3(MUART_init                    , MCAL_Error        ( MUART_Handle &handle, const MUART_Channel &channel, const MUART_Config &cfg ));
        MAKE_MOCK1(MUART_enable                  , void              ( MUART_Handle &handle ));
        MAKE_MOCK1(MUART_disable                 , void              ( MUART_Handle &handle ));
        MAKE_MOCK2(MUART_setNewBaudrate          , void              ( MUART_Handle &handle, uint32_t baudrate ));
        MAKE_MOCK2(MUART_configurePins           , MCAL_Error        ( const MDIO_Channel &rxPin, const MDIO_Channel &txPin ));
        MAKE_MOCK3(MUART_write                   , MCAL_Error        ( MUART_Handle &handle, const uint8_t buffer[], uint16_t length ));
        MAKE_MOCK3(MUART_read                    , MCAL_Error        ( MUART_Handle &handle, uint8_t buffer[], uint16_t length ));
        MAKE_MOCK1(MUART_finishRead              , void              ( MUART_Handle &handle ));
        MAKE_MOCK2(MUART_writeByte               , int32_t           ( const MUART_Handle &handle, uint8_t data ));
        MAKE_MOCK1(MUART_readByte                , int32_t           ( const MUART_Handle &handle ));
        MAKE_MOCK1(MUART_getStatus               , MCAL_Status       ( const MUART_Handle &handle ));
        MAKE_MOCK1(MUART_getReadStatus           , MCAL_Status       ( const MUART_Handle &handle ));
        MAKE_MOCK1(MUART_getWriteStatus          , MCAL_Status       ( const MUART_Handle &handle ));
        MAKE_MOCK1(MUART_getError                , MCAL_Error        ( const MUART_Handle &handle ));
        MAKE_MOCK1(MUART_clearError              , void              ( MUART_Handle &handle ));
        MAKE_MOCK3(MUART_enableEvent             , MCAL_Error        ( MUART_Handle &handle, MCAL_Callback_t &callback, MCAL_Event event ));
        MAKE_MOCK3(MUART_disableEvent            , MCAL_Error        ( MUART_Handle &handle, MCAL_Callback_t &callback, MCAL_Event event ));
        MAKE_MOCK1(MUART_getHandle               , MUART_Handle*     ( const void &handlePtr ));
        MAKE_MOCK3(MUART_writeBlocking           , MCAL_Error        ( const MUART_Handle &handle, const uint8_t buffer[], uint16_t length ));
    };

    extern muart_mock_class muart_mock;
}

using namespace muart_m;

#endif  // MUART_MOCK_H
