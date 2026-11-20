/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*  are protected by international copyright laws.
*
*  All rights reserved. This program and the accompanying materials
*  Please contact copyright holder for licensing information.
*******************************************************************************/

#ifndef MSUP_MOCK_H
#define MSUP_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace msup_m
{
    #include "mcal/msup.h"

    class msup_mock_class
    {
    public:
        MAKE_MOCK1(MSUP_handleTimerExt           , void              ( MSUP_TimeoutHandle *handle ));
        MAKE_MOCK1(MSUP_startTimeMeasureExt      , void              ( MSUP_TimeoutHandle *handle ));
        MAKE_MOCK2(MSUP_setTimeOutExt            , void              ( MSUP_TimeoutHandle *handle, uint32_t timeout ));
        MAKE_MOCK1(MSUP_getMeasuredTimeExt       , uint32_t          ( MSUP_TimeoutHandle *handle ));
        MAKE_MOCK1(MSUP_isTimeOutExt             , bool              ( MSUP_TimeoutHandle *handle ));
        MAKE_MOCK1(MSUP_delay                    , void              ( uint16_t delay ));
        MAKE_MOCK0(MSUP_handleTimer              , void              ( void ));
        MAKE_MOCK0(MSUP_startTimeMeasure         , void              ( void ));
        MAKE_MOCK1(MSUP_setTimeOut               , void              ( uint32_t timeout ));
        MAKE_MOCK0(MSUP_getMeasuredTime          , uint32_t          ( void ));
        MAKE_MOCK0(MSUP_isTimeOut                , bool              ( void ));
        MAKE_MOCK0(HSUP_disableInt               , void              ( void ));
        MAKE_MOCK0(HSUP_enableInt                , void              ( void ));
        MAKE_MOCK0(MSUP_generateReset            , void              ( void ));
    };

    extern msup_mock_class msup_mock;
}

using namespace msup_m;

#endif  // MSUP_MOCK_H
