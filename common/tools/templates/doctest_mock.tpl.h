/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  This file is generated automatically
 *  It might be adapted manually
 ******************************************************************************/


#ifndef <$UNIT_NAME>_MOCK_H
#define <$UNIT_NAME>_MOCK_H

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>

namespace <$unit_name>_m
{
    #include "<$unit_inc>"

    class <$unit_name>_mock_class
    {
    public:
<$MOCK_MACROS>
    };

    extern <$unit_name>_mock_class <$unit_name>_mock;
}

using namespace <$unit_name>_m;

#endif  // <$UNIT_NAME>_MOCK_H
