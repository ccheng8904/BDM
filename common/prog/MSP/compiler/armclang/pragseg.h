/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMPILER         KeilArm
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file 
 *  \brief This file is used to encapsulate pragma segment definitions
 *
 *  \details Legacy, better use Segmentdef.h for new designs
 *           (C99 standard must provided)
 *
 */


 /*lint -save -e9021       (9021: use of '#undef' is discouraged
                           legacy code working correctly, will be replaced by segDef.h)
 */

 
 /****************** 8 bit timer segments ********************/

#if    defined(DATA_SEG_TIMER8_10MS)

#pragma clang section bss = "TIMER8_10MS"

#undef DATA_SEG_TIMER8_10MS

#elif  defined(DATA_SEG_TIMER8_20MS)

#pragma clang section bss = "TIMER8_20MS"

#undef DATA_SEG_TIMER8_20MS

#elif  defined(DATA_SEG_TIMER8_100MS)

#pragma clang section bss = "TIMER8_100MS"

#undef DATA_SEG_TIMER8_100MS

#elif  defined(DATA_SEG_TIMER8_200MS)

#pragma clang section bss = "TIMER8_200MS"

#undef DATA_SEG_TIMER8_200MS

#elif  defined(DATA_SEG_TIMER8_1S)

#pragma clang section bss = "TIMER8_1S"

#undef DATA_SEG_TIMER8_1S

#elif  defined(DATA_SEG_TIMER8_2S)

#pragma clang section bss = "TIMER8_2S"

#undef DATA_SEG_TIMER8_2S

#elif  defined(DATA_SEG_TIMER8_10S)

#pragma clang section bss = "TIMER8_10S"

#undef DATA_SEG_TIMER8_10S

#elif  defined(DATA_SEG_TIMER8_20S)

#pragma clang section bss = "TIMER8_20S"

#undef DATA_SEG_TIMER8_20S

#elif  defined(DATA_SEG_TIMER8_1M)

#pragma clang section bss = "TIMER8_1M"

#undef DATA_SEG_TIMER8_1M

#elif  defined(DATA_SEG_TIMER8_2M)

#pragma clang section bss = "TIMER8_2M"

#undef DATA_SEG_TIMER8_2M

#elif  defined(DATA_SEG_START8_1)

#pragma clang section bss = "START8_1"

#undef DATA_SEG_START8_1

#elif  defined(DATA_SEG_START8_2)

#pragma clang section bss = "START8_2"

#undef DATA_SEG_START8_2

#elif  defined(DATA_SEG_START8_3)

#pragma clang section bss = "START8_3"

#undef DATA_SEG_START8_3

#elif  defined(DATA_SEG_START8_4)

#pragma clang section bss = "START8_4"

#undef DATA_SEG_START8_4

#elif  defined(DATA_SEG_START8_5)

#pragma clang section bss = "START8_5"

#undef DATA_SEG_START8_5

#elif  defined(DATA_SEG_START8_6)

#pragma clang section bss = "START8_6"

#undef DATA_SEG_START8_6

#elif  defined(DATA_SEG_START8_7)

#pragma clang section bss = "START8_7"

#undef DATA_SEG_START8_7

#elif  defined(DATA_SEG_START8_8)

#pragma clang section bss = "START8_8"

#undef DATA_SEG_START8_8

#elif  defined(DATA_SEG_START8_9)

#pragma clang section bss = "START8_9"

#undef DATA_SEG_START8_9

#elif  defined(DATA_SEG_START8_10)

#pragma clang section bss = "START8_10"

#undef DATA_SEG_START8_10

#elif  defined(DATA_SEG_END8)

#pragma clang section bss = "END8"

#undef DATA_SEG_END8


/****************** 16 bit timer segments ********************/

#elif  defined(DATA_SEG_TIMER16_10MS)

#pragma clang section bss = "TIMER16_10MS"

#undef DATA_SEG_TIMER16_10MS

#elif  defined(DATA_SEG_TIMER16_20MS)

#pragma clang section bss = "TIMER16_20MS"

#undef DATA_SEG_TIMER16_20MS

#elif  defined(DATA_SEG_TIMER16_100MS)

#pragma clang section bss = "TIMER16_100MS"

#undef DATA_SEG_TIMER16_100MS

#elif  defined(DATA_SEG_TIMER16_200MS)

#pragma clang section bss = "TIMER16_200MS"

#undef DATA_SEG_TIMER16_200MS

#elif  defined(DATA_SEG_TIMER16_1S)

#pragma clang section bss = "TIMER16_1S"

#undef DATA_SEG_TIMER16_1S

#elif  defined(DATA_SEG_TIMER16_2S)

#pragma clang section bss = "TIMER16_2S"

#undef DATA_SEG_TIMER16_2S

#elif  defined(DATA_SEG_TIMER16_10S)

#pragma clang section bss = "TIMER16_10S"

#undef DATA_SEG_TIMER16_10S

#elif  defined(DATA_SEG_TIMER16_20S)

#pragma clang section bss = "TIMER16_20S"

#undef DATA_SEG_TIMER16_20S

#elif  defined(DATA_SEG_TIMER16_1M)

#pragma clang section bss = "TIMER16_1M"

#undef DATA_SEG_TIMER16_1M

#elif  defined(DATA_SEG_TIMER16_2M)

#pragma clang section bss = "TIMER16_2M"

#undef DATA_SEG_TIMER16_2M

#elif  defined(DATA_SEG_TIMER16_10M)

#pragma clang section bss = "TIMER16_10M"

#undef DATA_SEG_TIMER16_10M

#elif  defined(DATA_SEG_TIMER16_20M)

#pragma clang section bss = "TIMER16_20M"

#undef DATA_SEG_TIMER16_20M

#elif  defined(DATA_SEG_START16_1)

#pragma clang section bss = "START16_1"

#undef DATA_SEG_START16_1

#elif  defined(DATA_SEG_START16_2)

#pragma clang section bss = "START16_2"

#undef DATA_SEG_START16_2

#elif  defined(DATA_SEG_START16_3)

#pragma clang section bss = "START16_3"

#undef DATA_SEG_START16_3

#elif  defined(DATA_SEG_START16_4)

#pragma clang section bss = "START16_4"

#undef DATA_SEG_START16_4

#elif  defined(DATA_SEG_START16_5)

#pragma clang section bss = "START16_5"

#undef DATA_SEG_START16_5

#elif  defined(DATA_SEG_START16_6)

#pragma clang section bss = "START16_6"

#undef DATA_SEG_START16_6

#elif  defined(DATA_SEG_START16_7)

#pragma clang section bss = "START16_7"

#undef DATA_SEG_START16_7

#elif  defined(DATA_SEG_START16_8)

#pragma clang section bss = "START16_8"

#undef DATA_SEG_START16_8

#elif  defined(DATA_SEG_START16_9)

#pragma clang section bss = "START16_9"

#undef DATA_SEG_START16_9

#elif  defined(DATA_SEG_START16_10)

#pragma clang section bss = "START16_10"

#undef DATA_SEG_START16_10

#elif  defined(DATA_SEG_START16_11)

#pragma clang section bss = "START16_11"

#undef DATA_SEG_START16_11

#elif  defined(DATA_SEG_START16_12)

#pragma clang section bss = "START16_12"

#undef DATA_SEG_START16_12

#elif  defined(DATA_SEG_END16)

#pragma clang section bss = "END16"

#undef DATA_SEG_END16


/****************** Other Segments ********************/

#elif  defined(DATA_SEG_DEFAULT)
#pragma clang section bss = ""
#undef DATA_SEG_DEFAULT

#elif  defined(CONST_SEG_DEFAULT)
#pragma clang section rodata = ""
#undef CONST_SEG_DEFAULT

#else
#include "PragsegXs.h"
#endif

/*lint -restore -e9021 */