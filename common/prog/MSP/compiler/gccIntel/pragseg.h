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
 *  COMPILER         gccIntel
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
/****************** 8 bit timer segments ********************/

#if    defined(DATA_SEG_TIMER8_10MS)
#undef DATA_SEG_TIMER8_10MS

#elif  defined(DATA_SEG_TIMER8_20MS)
#undef DATA_SEG_TIMER8_20MS

#elif  defined(DATA_SEG_TIMER8_100MS)
#undef DATA_SEG_TIMER8_100MS

#elif  defined(DATA_SEG_TIMER8_200MS)
#undef DATA_SEG_TIMER8_200MS

#elif  defined(DATA_SEG_TIMER8_1S)
#undef DATA_SEG_TIMER8_1S

#elif  defined(DATA_SEG_TIMER8_2S)
#undef DATA_SEG_TIMER8_2S

#elif  defined(DATA_SEG_TIMER8_10S)
#undef DATA_SEG_TIMER8_10S

#elif  defined(DATA_SEG_TIMER8_20S)
#undef DATA_SEG_TIMER8_20S

#elif  defined(DATA_SEG_TIMER8_1M)
#undef DATA_SEG_TIMER8_1M

#elif  defined(DATA_SEG_TIMER8_2M)
#undef DATA_SEG_TIMER8_2M

#elif  defined(DATA_SEG_START8_1)
#undef DATA_SEG_START8_1

#elif  defined(DATA_SEG_START8_2)
#undef DATA_SEG_START8_2

#elif  defined(DATA_SEG_START8_3)
#undef DATA_SEG_START8_3

#elif  defined(DATA_SEG_START8_4)
#undef DATA_SEG_START8_4

#elif  defined(DATA_SEG_START8_5)
#undef DATA_SEG_START8_5

#elif  defined(DATA_SEG_START8_6)
#undef DATA_SEG_START8_6

#elif  defined(DATA_SEG_START8_7)
#undef DATA_SEG_START8_7

#elif  defined(DATA_SEG_START8_8)
#undef DATA_SEG_START8_8

#elif  defined(DATA_SEG_START8_9)
#undef DATA_SEG_START8_9

#elif  defined(DATA_SEG_START8_10)
#undef DATA_SEG_START8_10

#elif  defined(DATA_SEG_END8)
#undef DATA_SEG_END8


/****************** 16 bit timer segments ********************/

#elif  defined(DATA_SEG_TIMER16_10MS)
#undef DATA_SEG_TIMER16_10MS

#elif  defined(DATA_SEG_TIMER16_20MS)
#undef DATA_SEG_TIMER16_20MS

#elif  defined(DATA_SEG_TIMER16_100MS)
#undef DATA_SEG_TIMER16_100MS

#elif  defined(DATA_SEG_TIMER16_200MS)
#undef DATA_SEG_TIMER16_200MS

#elif  defined(DATA_SEG_TIMER16_1S)
#undef DATA_SEG_TIMER16_1S

#elif  defined(DATA_SEG_TIMER16_2S)
#undef DATA_SEG_TIMER16_2S

#elif  defined(DATA_SEG_TIMER16_10S)
#undef DATA_SEG_TIMER16_10S

#elif  defined(DATA_SEG_TIMER16_20S)
#undef DATA_SEG_TIMER16_20S

#elif  defined(DATA_SEG_TIMER16_1M)
#undef DATA_SEG_TIMER16_1M

#elif  defined(DATA_SEG_TIMER16_2M)
#undef DATA_SEG_TIMER16_2M

#elif  defined(DATA_SEG_TIMER16_10M)
#undef DATA_SEG_TIMER16_10M

#elif  defined(DATA_SEG_TIMER16_20M)
#undef DATA_SEG_TIMER16_20M

#elif  defined(DATA_SEG_START16_1)
#undef DATA_SEG_START16_1

#elif  defined(DATA_SEG_START16_2)
#undef DATA_SEG_START16_2

#elif  defined(DATA_SEG_START16_3)
#undef DATA_SEG_START16_3

#elif  defined(DATA_SEG_START16_4)
#undef DATA_SEG_START16_4

#elif  defined(DATA_SEG_START16_5)
#undef DATA_SEG_START16_5

#elif  defined(DATA_SEG_START16_6)
#undef DATA_SEG_START16_6

#elif  defined(DATA_SEG_START16_7)
#undef DATA_SEG_START16_7

#elif  defined(DATA_SEG_START16_8)
#undef DATA_SEG_START16_8

#elif  defined(DATA_SEG_START16_9)
#undef DATA_SEG_START16_9

#elif  defined(DATA_SEG_START16_10)
#undef DATA_SEG_START16_10

#elif  defined(DATA_SEG_START16_11)
#undef DATA_SEG_START16_11

#elif  defined(DATA_SEG_START16_12)
#undef DATA_SEG_START16_12

#elif  defined(DATA_SEG_END16)
#undef DATA_SEG_END16


/****************** Other Segments ********************/

#elif  defined(DATA_SEG_DEFAULT)
#undef DATA_SEG_DEFAULT

#elif  defined(CONST_SEG_DEFAULT)
#undef CONST_SEG_DEFAULT

/*Data segment short is not supported for STM32*/
#elif defined(DATA_SEG_SHORT)
#undef DATA_SEG_SHORT


/************* Bootloader segments ************************/

// for Rx100 (UlBr 2016-03-28: belongs to bootloader segments?)
#elif   defined(CONST_SEG_DATAFLASH)
#undef  CONST_SEG_DATAFLASH

#elif defined (CONST_SEG_BL_VECTORS)
#undef  CONST_SEG_BL_VECTORS

#elif  defined(CODE_SEG_BL_INIT)
#undef CODE_SEG_BL_INIT

#elif  defined(CODE_SEG_DEFAULT)
#undef CODE_SEG_DEFAULT

/****************** Firmware Update Segments ********************/

/*APP FIXVECTORS segments */
#elif  defined(CONST_SEG_FIXVECTORS)
#undef CONST_SEG_FIXVECTORS

/*APP VECTORS segments */
#elif  defined(CONST_SEG_VECTORS)
#undef CONST_SEG_VECTORS

/*SW_MODULE_HEADER*/
#elif  defined(CONST_SEG_SW_MODULE_HEADER)
#undef CONST_SEG_SW_MODULE_HEADER

/*MILO segments */
/*EXCHANGE_MEMORY_MAP*/
#elif  defined(CONST_SEG_EXCHANGE_MEMORY_MAP)
#undef CONST_SEG_EXCHANGE_MEMORY_MAP

/*PERMANENT_EXCHANGE_MEMORY_EXTERNAL_VECTOR*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_EXTERNAL_VECTOR)
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_EXTERNAL_VECTOR

/*PERMANENT_EXCHANGE_MEMORY_VERSIONING*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_VERSIONING)
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_VERSIONING

/*PERMANENT_EXCHANGE_MEMORY_PARTITIONTABLE*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_PARTITIONTABLE)
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_PARTITIONTABLE

/*PERMANENT_EXCHANGE_MEMORY_USERDATA*/
#elif  defined(CONST_SEG_PERMANENT_EXCHANGE_MEMORY_USERDATA)
#undef CONST_SEG_PERMANENT_EXCHANGE_MEMORY_USERDATA

/*VOLATILE_EXCHANGE_MEMORY_MILO_BOOTMODULE*/
#elif  defined(DATA_SEG_VOLATILE_EXCHANGE_MEMORY_MILO)
#undef DATA_SEG_VOLATILE_EXCHANGE_MEMORY_MILO

#else
//#include "PragsegXs.h"
#endif

