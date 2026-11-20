/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#ifndef MADC_MOCK_H
#define MADC_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace madc_m
{
    #include "mcal/madc.h"

    class madc_mock_class
    {
    public:
        MAKE_MOCK3(MADC_init                     , MCAL_Error        ( MADC_Handle *handle, const MADC_Periph *periph, const MADC_Config *cfg ));
        MAKE_MOCK1(MADC_enable                   , void              ( MADC_Handle *handle ));
        MAKE_MOCK1(MADC_disable                  , void              ( MADC_Handle *handle ));
        MAKE_MOCK1(MADC_getStatus                , MCAL_Status       ( const MADC_Handle *handle ));
        MAKE_MOCK1(MADC_clearError               , void              ( MADC_Handle *handle ));
        MAKE_MOCK1(MADC_IRQ_Handler              , void              ( MADC_IRQ_Index index ));
        MAKE_MOCK2(MADC_readSample               , uint16_t          ( MADC_Handle *handle, const MADC_Channel *channel ));
        MAKE_MOCK1(MADC_isConversionResultValid  , bool              ( const MADC_Handle *handle ));
        MAKE_MOCK3(MADC_enableChannelEvent       , MCAL_Error        ( MADC_Handle *handle, const MADC_Channel *channel, MCAL_Callback_t *callback ));
        MAKE_MOCK3(MADC_disableChannelEvent      , MCAL_Error        ( MADC_Handle *handle, const MADC_Channel *channel, MCAL_Callback_t *callback ));
        MAKE_MOCK2(MADC_enablePeriphScanEvent    , MCAL_Error        ( MADC_Handle *handle, MCAL_Callback_t *callback ));
        MAKE_MOCK2(MADC_disablePeriphScanEvent   , MCAL_Error        ( MADC_Handle *handle, MCAL_Callback_t *callback ));
        MAKE_MOCK4(MADC_startScanConversion      , MCAL_Error        ( MADC_Handle *handle, const MADC_Channel *channels[], uint8_t channelCount, uint16_t *results ));
        MAKE_MOCK3(MADC_readSampleWithValidation , MCAL_Error        ( MADC_Handle *handle, const MADC_Channel *channel, uint16_t *result ));
    };

    extern madc_mock_class madc_mock;
}

using namespace madc_m;

#endif  // MADC_MOCK_H
