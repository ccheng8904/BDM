/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/
#include <doctest/doctest.h>
#include "math/mathDiv.c"

bool operator==(const MTDIV_Result& lhs, const MTDIV_Result& rhs)
{
    return ((lhs.quotient == rhs.quotient) && (lhs.remainder == rhs.remainder));
}

TEST_SUITE("mathDiv")
{
    TEST_CASE("try any values")
    {
        CHECK(MTDIV_div_mod(43,6) == (MTDIV_Result){7,1});
    }
    TEST_CASE("dividend is zero") 
    {
        CHECK(MTDIV_div_mod(0,6) == (MTDIV_Result){0,0});
    }
    TEST_CASE("quotient is zero remainder not zero") 
    {
        CHECK(MTDIV_div_mod(5,6) == (MTDIV_Result){0,5});
    }
}
