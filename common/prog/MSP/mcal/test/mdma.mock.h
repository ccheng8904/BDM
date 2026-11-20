/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*  are protected by international copyright laws.
*
*  All rights reserved. This program and the accompanying materials
*  Please contact copyright holder for licensing information.
*******************************************************************************/

#ifndef MDMA_MOCK_H
#define MDMA_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace mdma_mock
{
    #include "mcal/mdma.h"

    class mdma_mock_class
    {
    public:
        MAKE_MOCK3(MDMA_init                     , MCAL_Error        ( MDMA_Handle &handle, const MDMA_Periph &periph, const MDMA_Channel &channel ));
        MAKE_MOCK2(MDMA_initChannel              , MCAL_Error        ( const MDMA_Handle &handle, const MDMA_ChannelConfig &cfg ));
        MAKE_MOCK1(MDMA_enable                   , void              ( MDMA_Handle &handle ));
        MAKE_MOCK1(MDMA_disable                  , void              ( MDMA_Handle &handle ));
        MAKE_MOCK1(MDMA_getStatus                , MCAL_Status       ( const MDMA_Handle &handle ));
        MAKE_MOCK1(MDMA_clearError               , void              ( MDMA_Handle &handle ));
        MAKE_MOCK1(MDMA_IRQ_getIndex             , MDMA_IRQ_Index    ( const MDMA_Periph &periph ));
        MAKE_MOCK1(MDMA_IRQ_Handler              , void              ( MDMA_IRQ_Index index ));
        MAKE_MOCK2(MDMA_enableChannelEvent       , MCAL_Error        ( MDMA_Handle &handle, MCAL_Callback_t &callback ));
        MAKE_MOCK2(MDMA_disableChannelEvent      , MCAL_Error        ( const MDMA_Handle &handle, MCAL_Callback_t &callback ));
        MAKE_MOCK2(MDMA_startEventConversion     , MCAL_Error        ( MDMA_Handle &handle, const MDMA_Channel &channel ));
        MAKE_MOCK1(MDMA_enableChannel            , void              ( const MDMA_Handle &handle ));
        MAKE_MOCK1(MDMA_disableChannel           , void              ( const MDMA_Handle &handle ));
    };

    extern mdma_mock_class mdma_mock;
}

using namespace mdma_mock;

#endif  // MDMA_MOCK_H
