### The example of the file process_data.c

```c
/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Process data
 ******************************************************************************/
#include "SegmentDef.h"
#include "std_symbol.h"

SDEF_SetSegmentConst(FWU_PROCESS_DATA)

SYMBOL_USED const uint32_t Data[8] =
    {
        0xFEEDC0DE,
        0xFEEDC0DE,
        0xFEEDC0DE,
        0xFEEDC0DE,
        0xFEEDC0DE,
        0xFEEDC0DE,
        0xFEEDC0DE,
        0xFEEDC0DE
    };
SDEF_SetSegmentConst_Default()
```
[Back to page of firmware_update help](../README.md)