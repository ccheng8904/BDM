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

#include "hsup.mock.h"

namespace hsup_m
{
    hsup_mock_class hsup_mock;

    void HSUP_vDisableInt()
    {
        hsup_mock.HSUP_vDisableInt();
    }

    void HSUP_vEnableInt()
    {
        hsup_mock.HSUP_vEnableInt();
    }

    void HSUP_vGenerateReset()
    {
        hsup_mock.HSUP_vGenerateReset();
    }

    void HSUP_vNop()
    {
        hsup_mock.HSUP_vNop();
    }

    void HSUP_vEnterLowPowerMode()
    {
        hsup_mock.HSUP_vEnterLowPowerMode();
    }

    void HSUP_vDelay(uint16_t delay)
    {
        hsup_mock.HSUP_vDelay(delay);
    }

}

