/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#ifndef MADC_PRIV_MOCK_H
#define MADC_PRIV_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace madc_priv_m
{
    #include "mcal/madc_priv.h"

    class madc_priv_mock_class
    {
    public:
        MAKE_MOCK2(MADC_configure                , MCAL_Error        ( const MADC_Periph *periph, const MADC_Config *config ));
        MAKE_MOCK1(MADC_enablePeriph             , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_disablePeriph            , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_IRQ_enable               , void              ( const MADC_Periph *periph ));
        MAKE_MOCK2(MADC_readResultReg            , uint16_t          ( const MADC_Periph *periph, uint8_t index ));
        MAKE_MOCK1(MADC_isPeriphEvent            , bool              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_stopPeriphConversion     , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_enablePeriphEvent        , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_disablePeriphEvent       , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_getChannelNum            , uint8_t           ( const MADC_Channel *channel ));
        MAKE_MOCK3(MADC_startPeriphScanConversion, MCAL_Error        ( const MADC_Periph *periph, const MADC_Channel *const channels[], uint8_t channelCount ));
        MAKE_MOCK1(MADC_enableScanEvent          , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_disableScanEvent         , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_clearScanEventFlag       , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_clearConversionEventFlag , void              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_isScanEvent              , bool              ( const MADC_Periph *periph ));
        MAKE_MOCK1(MADC_IRQ_getIndex             , MADC_IRQ_Index    ( const MADC_Periph *periph ));
    };

    extern madc_priv_mock_class madc_priv_mock;
}

using namespace madc_priv_m;

#endif  // MADC_PRIV_MOCK_H
