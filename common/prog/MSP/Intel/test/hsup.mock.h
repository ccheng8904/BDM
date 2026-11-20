/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW Test
 ******************************************************************************/
#ifndef HSUP_MOCK_H
#define HSUP_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>


namespace hsup_m
{
    #include "hsup.h"

    class hsup_mock_class
    {
    public:
        MAKE_MOCK0(HSUP_vDisableInt        , void());
        MAKE_MOCK0(HSUP_vEnableInt         , void());
        MAKE_MOCK0(HSUP_vGenerateReset     , void());
        MAKE_MOCK0(HSUP_vNop               , void());
        MAKE_MOCK0(HSUP_vEnterLowPowerMode , void());
        MAKE_MOCK1(HSUP_vDelay             , void(uint16_t delay));
    };

    extern hsup_mock_class hsup_mock;
}
using namespace hsup_m;

#endif
