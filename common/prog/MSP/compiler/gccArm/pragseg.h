/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED-Framework
 *  COMPILER         GccArm
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file This file is used to encapsulate pragma segment definitions
 *
 *  \details Legacy, better use Segdef.h for new designs
 *           (C99 standard must provided)
 *
 *  \ingroup  PED-FW
 */


 /*lint -save -e9021       (9021: use of '#undef' is discouraged
                           legacy code working correctly, will be replaced by segDef.h)
 */



/****************** 8 bit timer segments ********************/

#if    defined(DATA_SEG_TIMER8_10MS)

__attribute__ ((section(".TIMER8_10MS")))

#undef DATA_SEG_TIMER8_10MS

#elif  defined(DATA_SEG_TIMER8_20MS)

__attribute__ ((section(".TIMER8_20MS")))

#undef DATA_SEG_TIMER8_20MS

#elif  defined(DATA_SEG_TIMER8_100MS)

__attribute__ ((section(".TIMER8_100MS")))

#undef DATA_SEG_TIMER8_100MS

#elif  defined(DATA_SEG_TIMER8_200MS)

__attribute__ ((section(".TIMER8_200MS")))

#undef DATA_SEG_TIMER8_200MS

#elif  defined(DATA_SEG_TIMER8_1S)

__attribute__ ((section(".TIMER8_1S")))

#undef DATA_SEG_TIMER8_1S

#elif  defined(DATA_SEG_TIMER8_2S)

__attribute__ ((section(".TIMER8_2S")))

#undef DATA_SEG_TIMER8_2S

#elif  defined(DATA_SEG_TIMER8_10S)

__attribute__ ((section(".TIMER8_10S")))

#undef DATA_SEG_TIMER8_10S

#elif  defined(DATA_SEG_TIMER8_20S)

__attribute__ ((section(".TIMER8_20S")))

#undef DATA_SEG_TIMER8_20S

#elif  defined(DATA_SEG_TIMER8_1M)

__attribute__ ((section(".TIMER8_1M")))

#undef DATA_SEG_TIMER8_1M

#elif  defined(DATA_SEG_TIMER8_2M)

__attribute__ ((section(".TIMER8_2M")))

#undef DATA_SEG_TIMER8_2M

#elif  defined(DATA_SEG_START8_1)

__attribute__ ((section(".START8_1")))

#undef DATA_SEG_START8_1

#elif  defined(DATA_SEG_START8_2)

__attribute__ ((section(".START8_2")))

#undef DATA_SEG_START8_2

#elif  defined(DATA_SEG_START8_3)

__attribute__ ((section(".START8_3")))

#undef DATA_SEG_START8_3

#elif  defined(DATA_SEG_START8_4)

__attribute__ ((section(".START8_4")))

#undef DATA_SEG_START8_4

#elif  defined(DATA_SEG_START8_5)

__attribute__ ((section(".START8_5")))

#undef DATA_SEG_START8_5

#elif  defined(DATA_SEG_START8_6)

__attribute__ ((section(".START8_6")))

#undef DATA_SEG_START8_6

#elif  defined(DATA_SEG_START8_7)

__attribute__ ((section(".START8_7")))

#undef DATA_SEG_START8_7

#elif  defined(DATA_SEG_START8_8)

__attribute__ ((section(".START8_8")))

#undef DATA_SEG_START8_8

#elif  defined(DATA_SEG_START8_9)

__attribute__ ((section(".START8_9")))

#undef DATA_SEG_START8_9

#elif  defined(DATA_SEG_START8_10)

__attribute__ ((section(".START8_10")))

#undef DATA_SEG_START8_10

#elif  defined(DATA_SEG_END8)

__attribute__ ((section(".END8")))

#undef DATA_SEG_END8


/****************** 16 bit timer segments ********************/

#elif  defined(DATA_SEG_TIMER16_10MS)

__attribute__ ((section(".TIMER16_10MS")))

#undef DATA_SEG_TIMER16_10MS

#elif  defined(DATA_SEG_TIMER16_20MS)

__attribute__ ((section(".TIMER16_20MS")))

#undef DATA_SEG_TIMER16_20MS

#elif  defined(DATA_SEG_TIMER16_100MS)

__attribute__ ((section(".TIMER16_100MS")))

#undef DATA_SEG_TIMER16_100MS

#elif  defined(DATA_SEG_TIMER16_200MS)

__attribute__ ((section(".TIMER16_200MS")))

#undef DATA_SEG_TIMER16_200MS

#elif  defined(DATA_SEG_TIMER16_1S)

__attribute__ ((section(".TIMER16_1S")))

#undef DATA_SEG_TIMER16_1S

#elif  defined(DATA_SEG_TIMER16_2S)

__attribute__ ((section(".TIMER16_2S")))

#undef DATA_SEG_TIMER16_2S

#elif  defined(DATA_SEG_TIMER16_10S)

__attribute__ ((section(".TIMER16_10S")))

#undef DATA_SEG_TIMER16_10S

#elif  defined(DATA_SEG_TIMER16_20S)

__attribute__ ((section(".TIMER16_20S")))

#undef DATA_SEG_TIMER16_20S

#elif  defined(DATA_SEG_TIMER16_1M)

__attribute__ ((section(".TIMER16_1M")))

#undef DATA_SEG_TIMER16_1M

#elif  defined(DATA_SEG_TIMER16_2M)

__attribute__ ((section(".TIMER16_2M")))

#undef DATA_SEG_TIMER16_2M

#elif  defined(DATA_SEG_TIMER16_10M)

__attribute__ ((section(".TIMER16_10M")))

#undef DATA_SEG_TIMER16_10M

#elif  defined(DATA_SEG_TIMER16_20M)

__attribute__ ((section(".TIMER16_20M")))

#undef DATA_SEG_TIMER16_20M

#elif  defined(DATA_SEG_START16_1)

__attribute__ ((section(".START16_1")))

#undef DATA_SEG_START16_1

#elif  defined(DATA_SEG_START16_2)

__attribute__ ((section(".START16_2")))

#undef DATA_SEG_START16_2

#elif  defined(DATA_SEG_START16_3)

__attribute__ ((section(".START16_3")))

#undef DATA_SEG_START16_3

#elif  defined(DATA_SEG_START16_4)

__attribute__ ((section(".START16_4")))

#undef DATA_SEG_START16_4

#elif  defined(DATA_SEG_START16_5)

__attribute__ ((section(".START16_5")))

#undef DATA_SEG_START16_5

#elif  defined(DATA_SEG_START16_6)

__attribute__ ((section(".START16_6")))

#undef DATA_SEG_START16_6

#elif  defined(DATA_SEG_START16_7)

__attribute__ ((section(".START16_7")))

#undef DATA_SEG_START16_7

#elif  defined(DATA_SEG_START16_8)

__attribute__ ((section(".START16_8")))

#undef DATA_SEG_START16_8

#elif  defined(DATA_SEG_START16_9)

__attribute__ ((section(".START16_9")))

#undef DATA_SEG_START16_9

#elif  defined(DATA_SEG_START16_10)

__attribute__ ((section(".START16_10")))

#undef DATA_SEG_START16_10

#elif  defined(DATA_SEG_START16_11)

__attribute__ ((section(".START16_11")))

#undef DATA_SEG_START16_11

#elif  defined(DATA_SEG_START16_12)

__attribute__ ((section(".START16_12")))

#undef DATA_SEG_START16_12

#elif  defined(DATA_SEG_END16)

__attribute__ ((section(".END16")))

#undef DATA_SEG_END16


/****************** Other Segments ********************/

#elif  defined(DATA_SEG_DEFAULT)


#undef DATA_SEG_DEFAULT

#elif  defined(CONST_SEG_DEFAULT)


#undef CONST_SEG_DEFAULT


#elif defined(DATA_SEG_SHORT)


#undef DATA_SEG_SHORT


/* Bootloader segments */
#elif defined (CONST_SEG_BL_VECTORS)

__attribute__ ((section(".BL_VECTORS")))

#undef  CONST_SEG_BL_VECTORS

#elif  defined(CODE_SEG_BL_INIT)

__attribute__ ((section(".BL_INIT")))

#undef CODE_SEG_BL_INIT

#elif  defined(CODE_SEG_DEFAULT)


#undef CODE_SEG_DEFAULT

/****************** Firmware Update Segments ********************/

/*APP FIXVECTORS segments */
#elif  defined(CONST_SEG_FIXVECTORS)
__attribute__ ((section(".FIXVECTORS")))
#undef CONST_SEG_FIXVECTORS

/*APP VECTORS segments */
#elif  defined(CONST_SEG_VECTORS)
__attribute__ ((section(".VECTORS")))
#undef CONST_SEG_VECTORS

/*SW_MODULE_HEADER*/
#elif  defined(CONST_SEG_SW_MODULE_HEADER)
__attribute__ ((section(".SW_MODULE_HEADER")))
#undef CONST_SEG_SW_MODULE_HEADER

/*MILO segments */
/*EXCHANGE_MEMORY_MAP*/
#elif  defined(CONST_SEG_EXCHANGE_MEMORY_MAP)
__attribute__ ((section(".EXCHANGE_MEMORY_MAP")))
#undef CONST_SEG_EXCHANGE_MEMORY_MAP

/*PERMANENT_EXCHANGE_MEMORY_OPTIONBYTES*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_OPTIONBYTES)
__attribute__ ((section(".PERMANENT_EXCHANGE_MEMORY_OPTIONBYTES")))
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_OPTIONBYTES

/*PERMANENT_EXCHANGE_MEMORY_VERSIONING*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_VERSIONING)
__attribute__ ((section(".PERMANENT_EXCHANGE_MEMORY_VERSIONING")))
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_VERSIONING

/*PERMANENT_EXCHANGE_MEMORY_PARTITIONTABLE*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_PARTITIONTABLE)
__attribute__ ((section(".PERMANENT_EXCHANGE_MEMORY_PARTITIONTABLE")))
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_PARTITIONTABLE

/*PERMANENT_EXCHANGE_MEMORY_USERDATA*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_USERDATA)
__attribute__ ((section(".PERMANENT_EXCHANGE_MEMORY_USERDATA")))
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_USERDATA

/*VOLATILE_EXCHANGE_MEMORY_MILO_BOOTMODULE*/
#elif  defined(DATA_SEG_VOLATILE_EXCHANGE_MEMORY_MILO)
__attribute__ ((section(".VOLATILE_EXCHANGE_MEMORY_MILO")))
#undef DATA_SEG_VOLATILE_EXCHANGE_MEMORY_MILO

/*VOLATILE_EXCHANGE_MEMORY_ITBL_OFFSET*/
#elif  defined(DATA_SEG_VOLATILE_EXCHANGE_MEMORY_ITBL_OFFSET)
__attribute__ ((section(".VOLATILE_EXCHANGE_MEMORY_ITBL_OFFSET")))
#undef DATA_SEG_VOLATILE_EXCHANGE_MEMORY_ITBL_OFFSET

#else
#include "PragsegXs.h"
#endif

#ifdef ALIGNMENT
__attribute__ ((aligned(ALIGNMENT)))   
#endif

/*lint -restore -e9021 */
