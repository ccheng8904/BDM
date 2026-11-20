/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MDIO
 ******************************************************************************/


#ifndef MDIO_TYPES_H
#define MDIO_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and type definitions for unit MDIO.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** Pin mask definitions */
#define MDIO_PIN0_MASK      0x00000001u
#define MDIO_PIN1_MASK      0x00000002u
#define MDIO_PIN2_MASK      0x00000004u
#define MDIO_PIN3_MASK      0x00000008u
#define MDIO_PIN4_MASK      0x00000010u
#define MDIO_PIN5_MASK      0x00000020u
#define MDIO_PIN6_MASK      0x00000040u
#define MDIO_PIN7_MASK      0x00000080u
#define MDIO_PIN8_MASK      0x00000100u
#define MDIO_PIN9_MASK      0x00000200u
#define MDIO_PIN10_MASK     0x00000400u
#define MDIO_PIN11_MASK     0x00000800u
#define MDIO_PIN12_MASK     0x00001000u
#define MDIO_PIN13_MASK     0x00002000u
#define MDIO_PIN14_MASK     0x00004000u
#define MDIO_PIN15_MASK     0x00008000u
#define MDIO_PIN16_MASK     0x00010000u
#define MDIO_PIN17_MASK     0x00020000u
#define MDIO_PIN18_MASK     0x00040000u
#define MDIO_PIN19_MASK     0x00080000u
#define MDIO_PIN20_MASK     0x00100000u
#define MDIO_PIN21_MASK     0x00200000u
#define MDIO_PIN22_MASK     0x00400000u
#define MDIO_PIN23_MASK     0x00800000u
#define MDIO_PIN24_MASK     0x01000000u
#define MDIO_PIN25_MASK     0x02000000u
#define MDIO_PIN26_MASK     0x04000000u
#define MDIO_PIN27_MASK     0x08000000u
#define MDIO_PIN28_MASK     0x10000000u
#define MDIO_PIN29_MASK     0x20000000u
#define MDIO_PIN30_MASK     0x40000000u
#define MDIO_PIN31_MASK     0x80000000u


/** MDIO default configuration structure definitions
 *  \note These definitions are used for initialization of MDIO configuration
 *        structures.
 */
#define MDIO_CFG_INPUT             {MDIO_MODE_DIGITAL_INPUT, MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}
#define MDIO_CFG_INPUT_PULL_UP     {MDIO_MODE_DIGITAL_INPUT, MDIO_PULL_UP, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}
#define MDIO_CFG_INPUT_PULL_DOWN   {MDIO_MODE_DIGITAL_INPUT, MDIO_PULL_DOWN, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}

#define MDIO_CFG_OUTPUT            {MDIO_MODE_DIGITAL_OUTPUT, MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}
#define MDIO_CFG_OUTPUT_PUSH_PULL  {MDIO_MODE_DIGITAL_OUTPUT, MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}
#define MDIO_CFG_OUTPUT_OPEN_DRAIN {MDIO_MODE_DIGITAL_OUTPUT, MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_OPEN_DRAIN, MDIO_OUTPUT_STATE_NO_CHANGE}

#define MDIO_CFG_ANALOG_INPUT      {MDIO_MODE_ANALOG, MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}

#define MDIO_CFG_ALTERNATE_FUNCTION {MDIO_MODE_ALTERNATE_FUNCTION, MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}
#define MDIO_CFG_ALTERNATE_FUNCTION_PULL_UP {MDIO_MODE_ALTERNATE_FUNCTION, MDIO_PULL_UP, MDIO_OUTPUT_TYPE_PUSH_PULL, MDIO_OUTPUT_STATE_NO_CHANGE}

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/** Pin pin enumeration */
enum MDIO_Pin
{
    MDIO_PIN0   =  0,
    MDIO_PIN00  =  MDIO_PIN0,
    MDIO_PIN1   =  1,
    MDIO_PIN01  =  MDIO_PIN1,
    MDIO_PIN2   =  2,
    MDIO_PIN02  =  MDIO_PIN2,
    MDIO_PIN3   =  3,
    MDIO_PIN03  =  MDIO_PIN3,
    MDIO_PIN4   =  4,
    MDIO_PIN04  =  MDIO_PIN4,
    MDIO_PIN5   =  5,
    MDIO_PIN05  =  MDIO_PIN5,
    MDIO_PIN6   =  6,
    MDIO_PIN06  =  MDIO_PIN6,
    MDIO_PIN7   =  7,
    MDIO_PIN07  =  MDIO_PIN7,
    MDIO_PIN8   =  8,
    MDIO_PIN08  =  MDIO_PIN8,
    MDIO_PIN9   =  9,
    MDIO_PIN09  =  MDIO_PIN9,
    MDIO_PIN10  = 10,
    MDIO_PIN11,
    MDIO_PIN12,
    MDIO_PIN13,
    MDIO_PIN14,
    MDIO_PIN15,
    MDIO_PIN16,
    MDIO_PIN17,
    MDIO_PIN18,
    MDIO_PIN19,
    MDIO_PIN20,
    MDIO_PIN21,
    MDIO_PIN22,
    MDIO_PIN23,
    MDIO_PIN24,
    MDIO_PIN25,
    MDIO_PIN26,
    MDIO_PIN27,
    MDIO_PIN28,
    MDIO_PIN29,
    MDIO_PIN30,
    MDIO_PIN31,
};


/** MDIO mode enumeration */
enum MDIO_Mode
{
    MDIO_MODE_DIGITAL_INPUT = 0,       ///< Digital input mode
    MDIO_MODE_DIGITAL_OUTPUT,          ///< Digital output mode
    MDIO_MODE_ALTERNATE_FUNCTION,      ///< Alternate function
    MDIO_MODE_ANALOG                   ///< Analog mode

};


/** MDIO pull type enumeration */
enum MDIO_PullType
{
    MDIO_PULL_DISABLED,                ///< No pull-up/down
    MDIO_PULL_UP,                      ///< Pull-up enabled
    MDIO_PULL_DOWN                     ///< Pull-down enabled
};


/** MDIO output type enumeration */
enum MDIO_OutputType
{
    MDIO_OUTPUT_TYPE_PUSH_PULL,        ///< Push/Pull output (strong drive)
    MDIO_OUTPUT_TYPE_OPEN_DRAIN,       ///< Open drain output
    MDIO_OUTPUT_TYPE_OPEN_SOURCE       ///< Open source output
};


/** MDIO output state enumeration */
enum MDIO_OutputState
{
    MDIO_OUTPUT_STATE_NO_CHANGE = 0,   ///< Output state will not be changed
    MDIO_OUTPUT_STATE_LOW,             ///< Output will be set to LOW
    MDIO_OUTPUT_STATE_HIGH             ///< Output will be set to HIGH
};


/** MDIO generic configuration structure */
struct MDIO_Config
{
    enum MDIO_Mode         mode;           ///< Pin mode configuration
    enum MDIO_PullType     pullType;       ///< Pull type configuration
    enum MDIO_OutputType   outputType;     ///< Output type configuration
    enum MDIO_OutputState  outputState;    ///< Output state set during configuration
};


#ifdef __cplusplus
}
#endif

#endif // MDIO_TYPES_H
