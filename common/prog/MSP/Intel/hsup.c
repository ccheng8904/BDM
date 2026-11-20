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

#include "hsup.h"
#include <stdint.h>

static uint32_t HSUP_xMirror(uint32_t value, uint32_t width);
static uint32_t HSUP_reverseByteOrder(uint32_t value);


uint32_t HSUP_u32Mirror(uint32_t value)
{
    return  HSUP_xMirror( value, sizeof(value));
}

uint16_t HSUP_u16Mirror(uint16_t value)
{
    return (uint16_t)(HSUP_xMirror( value, sizeof(value)));
}

uint8_t HSUP_ucMirror(uint8_t value)
{
    return (uint8_t)(HSUP_xMirror( value, sizeof(value)));
}


uint32_t HSUP_ulProcToBigEndian(uint32_t value)
{
    return HSUP_reverseByteOrder(value);
}

uint16_t HSUP_uiProcToBigEndian(uint16_t value)
{
    return ((value << 8u) | (value >> 8u));
}



static uint32_t HSUP_xMirror(uint32_t value, uint32_t width)
{
    value = HSUP_reverseByteOrder(value);
    
    value >>= ((sizeof(uint32_t) - width) * 8);
    value = ((value & 0xaaaaaaaau) >> 1u) | ((value & 0x55555555u) << 1u);
    value = ((value & 0xccccccccu) >> 2u) | ((value & 0x33333333u) << 2u);
    value = ((value & 0xf0f0f0f0u) >> 4u) | ((value & 0x0f0f0f0fu) << 4u);
    return value;
}

static uint32_t HSUP_reverseByteOrder(uint32_t value)
{
    return ((value >> 24u) & 0x000000ffu) |   // move byte 3 to byte 0
           ((value << 8u)  & 0x00ff0000u) |   // move byte 1 to byte 2
           ((value >> 8u)  & 0x0000ff00u) |   // move byte 2 to byte 1
           ((value << 24u) & 0xff000000u);    // byte 0 to byte 3
}

