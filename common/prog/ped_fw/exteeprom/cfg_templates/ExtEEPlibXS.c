/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          EXTEEPROM
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief          Common module of external EEPROM library - excess template .
 *                  This is the excess template, which may be modified in any
 *                  project to make room for system dependent
 *                  implementations/adaptations
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "utility/utility.h"
#include "ExtEEPlib.h"
#include "ExtEEPlibXS.h"
#if defined (I2C_BY_MCAL)
#if defined(MCAL_MI2C_INCLUDED)
#include "mcal/mdio.h"
#include "mcal/mi2c.h"
#include "mcal/msup.h"
#include "mcal_channels.h"
#include <string.h>
#else
#error "To use external EEPROM you have to install common:MCAL component !"
#endif
#else
#include "hi2c.h"
#include "hi2c01xs.h"
#include "hsup.h"
#endif
#if defined (DEBUG) && (DEBUG != 0u)
#include "Assert.h"
#endif

//Info 760: redundant macro 'MEMORY_ADDRESS_LENGTH' defined identically
//lint -save -e760
/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/

/******************************************************************************/
/* DEFINES                                                                    */
/******************************************************************************/
#define EEPROM_2_KBIT
//#define EEPROM_4_KBIT
//#define EEPROM_8_KBIT
//#define EEPROM_16_KBIT
//#define EEPROM_32_KBIT
//#define EEPROM_64_KBIT
#define ACCESS_WRITE         (uint8_t)(0x00)    ///< Address offset for write access
#if !defined(MCAL_MI2C_INCLUDED) || !defined(I2C_BY_MCAL)
#define ACCESS_READ          (uint8_t)(0x01)    ///< Address offset for read access
#endif
#define I2C_CHIP1_BASE_ADDRESS (uint8_t)0xA0    ///< Base address of chip 1
#define I2C_CHIP2_BASE_ADDRESS (uint8_t)0xA2    ///< Base address of chip 2
#define I2C_CHIP3_BASE_ADDRESS (uint8_t)0xA4    ///< Base address of chip 3
#define I2C_CHIP4_BASE_ADDRESS (uint8_t)0xA6    ///< Base address of chip 4
#define CHIPS_COUNT                       4u    ///< number of available chips

/* Select MCAL I2C communication method */
#if defined(CMS32M67XX) || defined(GD32F33) || defined(MSPM0) || defined(GD32E50X) || defined(GD32F30)
#define MI2C_COMMUNICATION_TYPE_HARDWARE
#define BUFFERSIZE             64u
#endif



#if defined(EEPROM_4_KBIT)
#define UPPER_ADDRESS_MASK     ((uint16_t)0x0100)
#elif defined(EEPROM_8_KBIT)
#define UPPER_ADDRESS_MASK     ((uint16_t)0x0300)
#elif defined(EEPROM_16_KBIT)
#define UPPER_ADDRESS_MASK     ((uint16_t)0x0700)
#endif

#ifndef __DOXYGEN__

/******************************************************************************/
/* STATIC FUNCTION PROTOTYPES                                                 */
/******************************************************************************/
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress);
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)
static void EXTEEP_vWriteEnable(void);
static void EXTEEP_vWriteDisable(void);
#endif
#endif

/******************************************************************************/
/* INITIALIZED VARIABLES                                                      */
/******************************************************************************/
const type_Communicator EXTEEP_atCommunicators[] =
{
    {
        // I2C bus communicator
        I2C_bInit   ,
        I2C_bRead   ,
        I2C_bWrite  ,
        I2C_bIsReady
    }
}
;
const uint8_t EXTEEP_ucCommunicatorsCount = (uint8_t) UTI_NELEMENTS(EXTEEP_atCommunicators);
chipInfoType EXTEEP_tChipInfo[CHIPS_COUNT] =
{
    /* I2C bus, address range 0 - 32767 */
    {
        1u, //ucNumber
        I2C_CHIP1_BASE_ADDRESS, //ucBaseAddress
#if defined(EEPROM_2_KBIT)
        255U,   //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_4_KBIT)
        511U,   //uiMaxAddress
        8U,     //ucPageSize
#elif defined(EEPROM_8_KBIT)
        1023U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_16_KBIT)
        2047U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_32_KBIT)
        4095U,  //uiMaxAddress
        32U,    //ucPageSize
#elif defined(EEPROM_64_KBIT)
        8191U, //uiMaxAddress
        32U,    //ucPageSize
#else
    #error "Undefined EEPROM type !"
#endif
        &EXTEEP_atCommunicators[0], //ptCommunicator
        false, //bIsChipCorrupt
        {
            //tTaskParam
            0, //uiStartAddress
            0, //uiBytesCount
            NULL  //pucData
        }
    }
    ,
    /* I2C bus, address range 32768 - 65535 */
    {
        2u, //ucNumber
        I2C_CHIP2_BASE_ADDRESS, //ucBaseAddress
#if defined(EEPROM_2_KBIT)
        255U,   //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_4_KBIT)
        511U,   //uiMaxAddress
        8U,     //ucPageSize
#elif defined(EEPROM_8_KBIT)
        1023U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_16_KBIT)
        2047U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_32_KBIT)
        4095U,  //uiMaxAddress
        32U,    //ucPageSize
#elif defined(EEPROM_64_KBIT)
        8191U,  //uiMaxAddress
        32U,    //ucPageSize
#else
    #error "Undefined EEPROM type !"
#endif
        &EXTEEP_atCommunicators[0], //ptCommunicator
        false, //bIsChipCorrupt
        {
            //tTaskParam
            0, //uiStartAddress
            0, //uiBytesCount
            NULL  //pucData
        }
    }
    ,
    /* I2C bus, address range 65536 - 98303 */
    {
        3u, //ucNumber
        I2C_CHIP3_BASE_ADDRESS, //ucBaseAddress
#if defined(EEPROM_2_KBIT)
        255U,   //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_4_KBIT)
        511U,   //uiMaxAddress
        8U,     //ucPageSize
#elif defined(EEPROM_8_KBIT)
        1023U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_16_KBIT)
        2047U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_32_KBIT)
        4095U,  //uiMaxAddress
        32U,    //ucPageSize
#elif defined(EEPROM_64_KBIT)
        8191U,  //uiMaxAddress
        32U,    //ucPageSize
#else
    #error "Undefined EEPROM type !"
#endif
        &EXTEEP_atCommunicators[0], //ptCommunicator
        false, //bIsChipCorrupt
        {
            //tTaskParam
            0, //uiStartAddress
            0, //uiBytesCount
            NULL  //pucData
        }
    }
    ,
    /* I2C bus, address range 98304 - 131 071 */
    {
        4u, //ucNumber
        I2C_CHIP4_BASE_ADDRESS, //ucBaseAddress
#if defined(EEPROM_2_KBIT)
        255U,   //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_4_KBIT)
        511U,   //uiMaxAddress
        8U,     //ucPageSize
#elif defined(EEPROM_8_KBIT)
        1023U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_16_KBIT)
        2047U,  //uiMaxAddress
        16U,    //ucPageSize
#elif defined(EEPROM_32_KBIT)
        4095U,  //uiMaxAddress
        32U,    //ucPageSize
#elif defined(EEPROM_64_KBIT)
        8191U,  //uiMaxAddress
        32U,    //ucPageSize
#else
    #error "Undefined EEPROM type !"
#endif
        &EXTEEP_atCommunicators[0], //ptCommunicator
        false, //bIsChipCorrupt
        {
            //tTaskParam
            0, //uiStartAddress
            0, //uiBytesCount
            NULL  //pucData
        }
    }
};
#ifdef GSW_USE_SINGLE_MEMORY_CHIP

#else
const uint32_t EXTEEP_ulMaxAddress = (CHIPS_COUNT * 32768u) - 1u;  //< gets top usable address of external memory
const uint8_t EXTEEP_ucChipCount = (uint8_t)UTI_NELEMENTS(EXTEEP_tChipInfo);
#endif

#if defined(EEPROM_4_KBIT) || defined(EEPROM_8_KBIT) || defined(EEPROM_16_KBIT)
static uint8_t EXTEEP_ucUpperAddress;
#endif

#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)
/* external EEPROM write enable/disable pin */
#if defined(RA4)
const struct MDIO_Channel MDIO_writeEnablePin =
{
    .port = &MDIO1,
    .pin  = MDIO_PIN02,
    .scfg = {
            .altFunc       = IOPORT_PERIPHERAL_IO,
            .pinConf       = IOPORT_CFG_PORT_DIRECTION_OUTPUT,
            .driveStrength = MDIO_DRIVE_STRENGTH_LOW}
};
#elif defined(RA2)
const struct MDIO_Channel MDIO_writeEnablePin =
{
    .port = &MDIO4,
    .pin  = MDIO_PIN09,
    .scfg = {
            .altFunc       = IOPORT_PERIPHERAL_IO,
            .pinConf       = IOPORT_CFG_PORT_DIRECTION_OUTPUT,
            .driveStrength = MDIO_DRIVE_STRENGTH_LOW}
};
#elif defined(GD32F33)
const struct MDIO_Channel MDIO_writeEnablePin =
{
    .port = &MDIOA,
    .pin  = MDIO_PIN3,
    .scfg = {
            .altFunc     = MDIO_AF0,
            .speed       = MDIO_SPEED_HIGH
            }
};
#elif defined(GD32F30)
const struct MDIO_Channel MDIO_writeEnablePin =
{
    .port = &MDIOA,
    .pin  = MDIO_PIN3,
    .scfg = {
            .AFMode     = MDIO_AFMODE_DEFAULT,
            .speed       = MDIO_SPEED_HIGH
            }
};
#elif defined(MSPM0)
const struct MDIO_Channel MDIO_writeEnablePin =
{
    .port = &MDIOA,
    .pin  = MDIO_PIN22,
    .scfg = {
            .periphFunc    = MDIO_PF1,
            .direction     = MDIO_OUT,
            .driveStrength = MDIO_DRIVE_STRENGTH_LOW
            },
    .pinCM = &IOMUX->SECCFG.PINCM[22]
};
#elif defined(GD32E50X)
const struct MDIO_Channel MDIO_writeEnablePin =
{
    .port = &MDIOA,
    .pin  = MDIO_PIN3,
    .scfg = {
            .AFMode     = MDIO_AFMODE_DEFAULT,
            .speed      = MDIO_SPEED_HIGH,
            .AFMask     = MDIO_AFMASK_DEFAULT
            }
}
#else
#warning "Please define write enable channel for selected derivative !"
#endif

const struct MDIO_Config  MDIO_extEEPcfg =
{
    .mode        = MDIO_MODE_DIGITAL_OUTPUT,
    .pullType    = MDIO_PULL_DISABLED,
    .outputType  = MDIO_OUTPUT_TYPE_OPEN_DRAIN,
    .outputState = MDIO_OUTPUT_STATE_HIGH
};

struct MI2C_Handle MI2C_extEEPhandle;

#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
const struct MI2C_Config MI2C_extEEPCfg =
{
    .baudrate   = MI2C_BAUDRATE_STANDARD,
    .mode       = MI2C_WORK_AS_MASTER,
    .ownAddress = 0u,
    .commType   = MI2C_TYPE_HW,
    .intPullUps = false
};
#else
const struct MI2C_Config MI2C_extEEPCfg =
{
    .baudrate   = MI2C_BAUDRATE_STANDARD,
    .mode       = MI2C_WORK_AS_MASTER,
    .ownAddress = 0u,
    .commType   = MI2C_TYPE_SW,
    .intPullUps = false
};
#endif
#if defined(RA4)
const struct MI2C_Channel MI2C_extEEPChannel =
{
    .mi2c        = NULL,
    .sdaPin      = &MDIO101,
    .sclPin      = &MDIO100,
    .scfg        = {.clkSrc = MI2C_CLOCK_SOURCE_PCLK}
};
#elif defined(RA2)
const struct MI2C_Channel MI2C_extEEPChannel =
{
    .mi2c        = NULL,
    .sdaPin      = &MDIO411,
    .sclPin      = &MDIO410
};
#elif defined(GD32F33)
#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
/* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_HW */
#define MI2C_extEEPChannel MI2C0_SDA_PA10_SCL_PA9
#else
/* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_SW */
 const struct MI2C_Channel MI2C_extEEPChannel =
{
    .mi2c        = NULL,
    .sdaPin      = &MDIOA10,
    .sclPin      = &MDIOA9,
    .scfg        = {.clkSrc = MI2C_CLOCK_SOURCE_PCLK}
};
#endif
#elif defined(GD32F30)
#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
/* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_HW */
#define MI2C_extEEPChannel MI2C0_SDA_PB9_SCL_PB8
#else
/* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_SW */
 const struct MI2C_Channel MI2C_extEEPChannel =
{
    .mi2c        = NULL,
    .sdaPin      = &MDIOA10,
    .sclPin      = &MDIOA9,
    .scfg        = {.clkSrc = MI2C_CLOCK_SOURCE_PCLK}
};
#endif
#elif defined(MSPM0)
#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
 /* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_HW */
 const struct MI2C_Channel MI2C_extEEPChannel =
  {
      .mi2c        = &MI2C0,
      .sdaPin      = &MDIOA0_I2C0_SDA_IN,
      .sclPin      = &MDIOA1_I2C0_SCL_IN,
      .scfg        = {.clkSrc = MI2C_CLOCK_SOURCE_BUSCLK}
  };
#else
 /* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_SW */
 const struct MI2C_Channel MI2C_extEEPChannel =
  {
      .mi2c        = NULL,
      .sdaPin      = &MDIOA0,
      .sclPin      = &MDIOA1,
      .scfg        = {.clkSrc = MI2C_CLOCK_SOURCE_BUSCLK}
  };
#endif
#elif defined(GD32E50X)
#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
/* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_HW */
#define MI2C_extEEPChannel MI2C0_SDA_PB7_SCL_PB6
#else
/* configuration for MI2C_extEEPCfg.commType   = MI2C_TYPE_SW */
 const struct MI2C_Channel MI2C_extEEPChannel =
{
    .mi2c        = NULL,
    .sdaPin      = &MDIOB7,
    .sclPin      = &MDIOB6,
    .scfg        = {.clkSrc = MI2C_CLOCK_SOURCE_PCLK}
};
#endif
#else
#warning "Please define mi2c channel for selected derivative !"
#endif
#endif


#ifdef EXTEEP_TEST
#define ITEM_COUNT          256u
#define EXTEEP_INIT           0u
#define EXTEEP_READY          1u
#define EXTEEP_READ           2u
#define EXTEEP_WRITE          3u
#define EXTEEP_TASKPENDING    4u

uchar EXTEEP_aucReadData[ITEM_COUNT] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#if 1
/* 1. data version*/
uchar EXTEEP_aucWriteData[ITEM_COUNT] =
{
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
#else
/* 2. data version*/
const uchar EXTEEP_aucWriteData[ITEM_COUNT] =
{
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF
};
#endif
#endif

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/
#ifndef __DOXYGEN__
#if defined(EEPROM_2_KBIT)
/* static bool I2C_bWritePageBytes()*/
/* writes some bytes into one memory page */

#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)

#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
#define MEMORY_ADDRESS_LENGTH    1u
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    static uint8_t writeBuffer[BUFFERSIZE];
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    writeBuffer[0] = MSUP_getLowByte(uiAddress);
    (void)memcpy((void*)&writeBuffer[1],(const void*)pucData, uiLength);

    funcRetVal = MI2C_write(&MI2C_extEEPhandle, (uint16_t)ucChipAddress >> 1u, writeBuffer, uiLength + MEMORY_ADDRESS_LENGTH);
    while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
    {
        /* wait until interrupts finish */
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    funcRetVal = MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)ACCESS_WRITE));
    if ( funcRetVal == MCAL_OK )
    {
        funcRetVal = MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getLowByte(uiAddress));
        if ( funcRetVal == MCAL_OK )
        {
            for (uint16_t uiCounter = 0u; uiCounter < uiLength; uiCounter++)
            {
                funcRetVal = MI2C_writeByte(&MI2C_extEEPhandle, pucData[uiCounter]);
                if ( funcRetVal != MCAL_OK )
                {
                    /* data write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                    _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                    break;
                }
            }
            MI2C_extEEPhandle.status = MCAL_STATUS_READY;
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    /* generate stop condition */
    (void)MI2C_stopCommunication(&MI2C_extEEPhandle);

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif

#else /* I2C by HAL */
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    uint16_t  uiCounter;
    bool  bRetVal;
    bRetVal = HI2C0_bSetAddr((uint8_t)(ucChipAddress | ACCESS_WRITE));
    if ( bRetVal != false )
    {
        bRetVal = HI2C0_bSetTxData(HSUP_ucGetLowByte(uiAddress), false);
        if ( bRetVal != false )
        {
            for (uiCounter = 0u; uiCounter < uiLength; uiCounter++)
            {
                bRetVal = HI2C0_bSetTxData(pucData[uiCounter], false);
                if ( bRetVal == false )
                {
                    /* data write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                    _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                    break;
                }
                else
                {
                    /* All is OK*/
                }
            }
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    /* generate stop condition */
    HI2C0_vSendStop();
    return(bRetVal);
}
#endif


#elif defined(EEPROM_4_KBIT) || defined(EEPROM_8_KBIT) || defined(EEPROM_16_KBIT)
/* static bool I2C_bWritePageBytes()*/
/* writes some bytes into one memory page */
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)

#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
#define MEMORY_ADDRESS_LENGTH    1u
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    static uint8_t writeBuffer[BUFFERSIZE];
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    writeBuffer[0] = MSUP_getLowByte(uiAddress);
    (void)memcpy((void*)&writeBuffer[1],(const void*)pucData, uiLength);

    EXTEEP_ucUpperAddress = MSUP_getLowByte((uint16_t)((uiAddress & UPPER_ADDRESS_MASK) >> 7));
    funcRetVal = MI2C_write(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)EXTEEP_ucUpperAddress) >> 1u, writeBuffer, uiLength + MEMORY_ADDRESS_LENGTH);
    while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
    {
        /* wait until interrupts finish */
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    EXTEEP_ucUpperAddress = MSUP_getLowByte((uint16_t)((uiAddress & UPPER_ADDRESS_MASK) >> 7));
    funcRetVal = MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)EXTEEP_ucUpperAddress | (uint16_t)ACCESS_WRITE));
    if ( funcRetVal == MCAL_OK )
    {
        funcRetVal = MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getLowByte(uiAddress));
        if ( funcRetVal == MCAL_OK )
        {
            for (uint16_t uiCounter = 0u; uiCounter < uiLength; uiCounter++)
            {
                funcRetVal = MI2C_writeByte(&MI2C_extEEPhandle, pucData[uiCounter]);
                if ( funcRetVal != MCAL_OK )
                {
                    /* data write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                    _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                    break;
                }
            }
            MI2C_extEEPhandle.status = MCAL_STATUS_READY;
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    /* generate stop condition */
    (void)MI2C_stopCommunication(&MI2C_extEEPhandle);

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif

#else /* I2C by HAL */
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    uint16_t  uiCounter;
    bool  bRetVal;
    EXTEEP_ucUpperAddress = HSUP_ucGetLowByte((uint16_t)((uiAddress & UPPER_ADDRESS_MASK) >> 7));
    bRetVal = HI2C0_bSetAddr((uint8_t)(ucChipAddress | EXTEEP_ucUpperAddress | ACCESS_WRITE));
    if ( bRetVal != false )
    {
        bRetVal = HI2C0_bSetTxData(HSUP_ucGetLowByte(uiAddress), false);
        if ( bRetVal != false )
        {
            for (uiCounter = 0u; uiCounter < uiLength; uiCounter++)
            {
                bRetVal = HI2C0_bSetTxData(pucData[uiCounter], false);
                if ( bRetVal == false )
                {
                    /* data write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                    _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                    break;
                }
                else
                {
                    /* All is OK*/
                }
            }
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    /* generate stop condition */
    HI2C0_vSendStop();
    return(bRetVal);
}
#endif
#elif defined(EEPROM_32_KBIT) || defined(EEPROM_64_KBIT)
/* static bool I2C_bWritePageBytes()*/
/* writes some bytes into one memory page */
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)

#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
#define MEMORY_ADDRESS_LENGTH    2u
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    static uint8_t writeBuffer[BUFFERSIZE];
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    writeBuffer[0] = MSUP_getHighByte(uiAddress);
    writeBuffer[1] = MSUP_getLowByte(uiAddress);
    (void)memcpy((void*)&writeBuffer[2],(const void*)pucData, uiLength);

    funcRetVal = MI2C_write(&MI2C_extEEPhandle, (uint16_t)ucChipAddress >> 1u, writeBuffer, uiLength + MEMORY_ADDRESS_LENGTH);
    while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
    {
        /* wait until interrupts finish */
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    funcRetVal = MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)ACCESS_WRITE));
    if ( funcRetVal == MCAL_OK )
    {
        funcRetVal = MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getHighByte(uiAddress));
        if ( funcRetVal == MCAL_OK )
        {
            funcRetVal = MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getLowByte(uiAddress));
            if ( funcRetVal == MCAL_OK )
            {
                for (uint16_t uiCounter = 0u; uiCounter < uiLength; uiCounter++)
                {
                    funcRetVal = MI2C_writeByte(&MI2C_extEEPhandle, pucData[uiCounter]);
                    if ( funcRetVal != MCAL_OK )
                    {
                        /* data write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                        break;
                    }
                }
                MI2C_extEEPhandle.status = MCAL_STATUS_READY;
            }
            else
            {
                /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
            }
        }
        else
        {
            /* upper address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    /* generate stop condition */
    (void)MI2C_stopCommunication(&MI2C_extEEPhandle);

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif

#else /* I2C by HAL */
static bool I2C_bWritePageBytes(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    uint16_t uiCounter;
    bool bRetVal;
    bRetVal = HI2C0_bSetAddr((uint8_t)(ucChipAddress | ACCESS_WRITE));
    if ( bRetVal != false )
    {
        bRetVal = HI2C0_bSetTxData(HSUP_HIGH_BYTE(uiAddress), false);
        if ( bRetVal != false )
        {
            bRetVal = HI2C0_bSetTxData(HSUP_LOW_BYTE(uiAddress), false);
            if ( bRetVal != false )
            {
                for (uiCounter = 0u; uiCounter < uiLength; uiCounter++)
                {
                    bRetVal = HI2C0_bSetTxData(pucData[uiCounter], false);
                    if ( bRetVal == false )
                    {
                        /* data write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                        break;
                    }
                    else
                    {
                        /* All is OK*/
                    }
                }
            }
            else
            {
                /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
            }
        }
        else
        {
            /* upper address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    /* generate stop condition */
    HI2C0_vSendStop();
    return(bRetVal);
}
#endif
#else
#error"Unknown EEPROM size, define its write driver !"
#endif
#endif

#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)
/* Function initializes write protect pin */
static void EXTEEP_initPin(void)
{
    (void)MDIO_init(&MDIO_writeEnablePin, &MDIO_extEEPcfg);
}

/* Function enables write to external EEPROM. */
static void EXTEEP_vWriteEnable(void)
{
    MDIO_reset(&MDIO_writeEnablePin);
}

/* Function disables write to external EEPROM. */
static void EXTEEP_vWriteDisable(void)
{
    MDIO_set(&MDIO_writeEnablePin);
}
#endif

/******************************************************************************/
/* PUBLIC FUNCTIONS                                                           */
/******************************************************************************/
/* I2C_bInit()*/
/** Hardware I2C interface initialization
*
*\param   void
*\return
*   #true    - operation successful \n
*   #false   - operation failed
*\remarks
* This function is called from library so you must not call it manually from your code
*/
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)
bool I2C_bInit(void)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    EXTEEP_initPin();
    funcRetVal = MI2C_init(&MI2C_extEEPhandle, &MI2C_extEEPChannel, &MI2C_extEEPCfg);
    if(funcRetVal == MCAL_OK)
    {
        funcRetVal = MI2C_forceBusRelease(&MI2C_extEEPhandle);
    }
    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else /* I2C by HAL */
bool I2C_bInit(void)
{
    bool bRetVal;
    HI2C0_vInit(0);   //SDA,SCL lines initialization
    HI2C0_vInitPin(); //Write protect pin initialization
    bRetVal = HI2C0_bForceBusRelease();
    return(bRetVal);
}
#endif
/* I2C_bRead()*/
/** Generates communication impulses for reading through I2C bus
*
*\param   uiAddress - start address for read
*\param   pucData - byte buffer for read data
*\param   uiLength  - number of bytes to read
*\param   ucChipAddress - hard address of chip
*\return
*   #true    - operation successful \n
*   #false   - operation failed
*\remarks
*   - ulAddress  : Address value must be within valid addresses interval.
*   - pucData != NULL  : Read buffer must be valid.
*   - uiLength  > 0    : At least one data must be read.
*   - ucChipAddress    : number of chip which is really connected on I2C bus
*
* This function is called from library so you must not call it manually from your code
*/
#if defined(EEPROM_2_KBIT)
/* I2C_bRead()*/
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)

#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
#define MEMORY_ADDRESS_LENGTH    1u
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    uint8_t u8Address = MSUP_getLowByte(uiAddress);
    enum MCAL_Error funcRetVal = MCAL_ERROR;

    if(MI2C_write(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress >> 1u), &u8Address, MEMORY_ADDRESS_LENGTH) == MCAL_OK)
    {
        while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
        {
            /* wait until interrupts finish */
        }
        funcRetVal = MI2C_read(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress >> 1U), pucData, uiLength); //convert to 7-bit address
        while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
        {
            /* wait until interrupts finish */
        }
    }
    else
    {
        /* chip address and memory lower address write error */
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal = MCAL_ERROR;

    if ( MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)ACCESS_WRITE)) == MCAL_OK )
    {
        if ( MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getLowByte(uiAddress)) == MCAL_OK )
        {
            funcRetVal = MI2C_read(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress >> 1U), pucData, uiLength); //convert to 7-bit address
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif

#else /* I2C by HAL */
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    uint16_t uiCounter;
    bool bRetVal;
    bRetVal        = false;
    if ( HI2C0_bSetAddr((uint8_t)(ucChipAddress | ACCESS_WRITE)) != false )
    {
        if ( HI2C0_bSetTxData(HSUP_ucGetLowByte(uiAddress), false) != false )
        {
            if ( HI2C0_bSetAddr((uint8_t)(ucChipAddress | ACCESS_READ)) != false )
            {
                for (uiCounter = 0u; uiCounter < (uiLength - 1u); uiCounter++)
                {
                    HI2C0_vTriggerReceive(false);
                    pucData[uiCounter] = HI2C0_ucGetRxData();
                }
                uiCounter = uiLength - 1u;
                HI2C0_vTriggerReceive(true);
                pucData[uiCounter] = HI2C0_ucGetRxData();
                bRetVal = true;
            }
            else
            {
                /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
            }
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    return(bRetVal);
}
#endif
#elif defined(EEPROM_4_KBIT) || defined(EEPROM_8_KBIT) || defined(EEPROM_16_KBIT)
/* I2C_bRead()*/
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)

#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
#define MEMORY_ADDRESS_LENGTH    1u
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    uint8_t u8Address = MSUP_getLowByte(uiAddress);
    enum MCAL_Error funcRetVal = MCAL_ERROR;

    EXTEEP_ucUpperAddress = HSUP_ucGetLowByte((uint16_t)((uiAddress & UPPER_ADDRESS_MASK) >> 7));
    if(MI2C_write(&MI2C_extEEPhandle, (((uint16_t)ucChipAddress | (uint16_t)EXTEEP_ucUpperAddress) >> 1u), &u8Address, MEMORY_ADDRESS_LENGTH) == MCAL_OK)
    {
        while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
        {
            /* wait until interrupts finish */
        }
        funcRetVal = MI2C_read(&MI2C_extEEPhandle, (((uint16_t)ucChipAddress | (uint16_t)EXTEEP_ucUpperAddress) >> 1U), pucData, uiLength); //convert to 7-bit address
        while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
        {
            /* wait until interrupts finish */
        }
    }
    else
    {
        /* chip address and memory lower address write error */
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    enum MCAL_Error funcRetVal = MCAL_ERROR;
    bool bRetVal;

    EXTEEP_ucUpperAddress = HSUP_ucGetLowByte((uint16_t)((uiAddress & UPPER_ADDRESS_MASK) >> 7));
    if ( MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)EXTEEP_ucUpperAddress | (uint16_t)ACCESS_WRITE)) == MCAL_OK )
    {
        if ( MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getLowByte(uiAddress)) == MCAL_OK )
        {
            funcRetVal = MI2C_read(&MI2C_extEEPhandle, (((uint16_t)ucChipAddress | (uint16_t)EXTEEP_ucUpperAddress) >> 1U), pucData, uiLength); //convert to 7-bit address
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif

#else /* I2C by HAL */
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    uint16_t uiCounter;
    bool bRetVal;
    bRetVal        = false;
    EXTEEP_ucUpperAddress = HSUP_ucGetLowByte((uint16_t)((uiAddress & UPPER_ADDRESS_MASK) >> 7));
    if ( HI2C0_bSetAddr((uint8_t)(ucChipAddress | EXTEEP_ucUpperAddress | ACCESS_WRITE)) != false )
    {
        if ( HI2C0_bSetTxData(HSUP_ucGetLowByte(uiAddress), false) != false )
        {
            if ( HI2C0_bSetAddr((uint8_t)(ucChipAddress | EXTEEP_ucUpperAddress | ACCESS_READ)) != false )
            {
                for (uiCounter = 0u; uiCounter < (uiLength - 1u); uiCounter++)
                {
                    HI2C0_vTriggerReceive(false);
                    pucData[uiCounter] = HI2C0_ucGetRxData();
                }
                uiCounter = uiLength - 1u;
                HI2C0_vTriggerReceive(true);
                pucData[uiCounter] = HI2C0_ucGetRxData();
                bRetVal = true;
            }
            else
            {
                /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
            }
        }
        else
        {
            /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    return(bRetVal);
}
#endif
#elif defined(EEPROM_32_KBIT) || defined(EEPROM_64_KBIT)
/* I2C_bRead()*/
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)

#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)
#define MEMORY_ADDRESS_LENGTH    2u
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    uint8_t address[2];
    enum MCAL_Error funcRetVal = MCAL_ERROR;

    address[0] = MSUP_getHighByte(uiAddress);
    address[1] = MSUP_getLowByte(uiAddress);
    if(MI2C_write(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress >> 1u), address, MEMORY_ADDRESS_LENGTH) == MCAL_OK)
    {
        while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
        {
            /* wait until interrupts finish */
        }
        funcRetVal = MI2C_read(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress >> 1U), pucData, uiLength); //convert to 7-bit address
        while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
        {
            /* wait until interrupts finish */
        }
    }
    else
    {
        /* chip address and memory lower address write error */
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal = MCAL_ERROR;

    if ( MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)ACCESS_WRITE)) == MCAL_OK )
    {
        if ( MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getHighByte(uiAddress)) == MCAL_OK )
        {
            if ( MI2C_writeByte(&MI2C_extEEPhandle, MSUP_getLowByte(uiAddress)) == MCAL_OK )
            {
                funcRetVal = MI2C_read(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress >> 1U), pucData, uiLength); //convert to 7-bit address
            }
            else
            {
                /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
            }
        }
        else
        {
            /* upper address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif

#else /* I2C by HAL */
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    uint16_t uiCounter;
    bool bRetVal;
    bRetVal = false;
    if ( HI2C0_bSetAddr((uint8_t)(ucChipAddress | ACCESS_WRITE)) != false )
    {
        if ( HI2C0_bSetTxData(HSUP_HIGH_BYTE(uiAddress), false) != false )
        {
            if ( HI2C0_bSetTxData(HSUP_LOW_BYTE(uiAddress), false) != false )
            {
                if ( HI2C0_bSetAddr((uint8_t)(ucChipAddress | ACCESS_READ)) != false )
                {
                    for (uiCounter = 0u; uiCounter < (uiLength - 1); uiCounter++)
                    {
                        HI2C0_vTriggerReceive(false);
                        pucData[uiCounter] = HI2C0_ucGetRxData();
                    }
                    uiCounter = uiLength - 1u;
                    HI2C0_vTriggerReceive(true);
                    pucData[uiCounter] = HI2C0_ucGetRxData();
                    bRetVal = true;
                }
                else
                {
                    /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                    _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                }
            }
            else
            {
                /* lower address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
                _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
            }
        }
        else
        {
            /* upper address byte write error*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        }
    }
    else
    {
        /* base address write error*/
#if defined (DEBUG) && (DEBUG != 0u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    return(bRetVal);
}
#endif
#else
#error"Unknown EEPROM size, define its read driver !"
#endif

/* I2C_bWrite()*/
/** Generates communication impulses for writing through I2C bus
*
*\param   uiAddress - start address for write
*\param   pucData - byte buffer which has to be written from
*\param   uiLength  - number of bytes to write
*\param   ucChipAddress - hard address of chip
*\return
*   #true    - operation successful \n
*   #false   - operation failed
*\remarks
*   - ulAddress  : Address value must be within valid addresses interval.
*   - pucData != NULL  : Read buffer must be valid.
*   - uiLength  > 0    : At least one data must be read.
*   - ucChipAddress    : number of chip which is really connected on I2C bus
*
* This function is called from library so you must not call it manually from your code
*/
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)
bool I2C_bWrite(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    EXTEEP_vWriteEnable(); //reset WC signal
    bRetVal = I2C_bWritePageBytes(uiAddress, pucData, uiLength, ucChipAddress);
    EXTEEP_vWriteDisable(); //set WC signal
    return(bRetVal);
}
#else /* I2C by HAL */
bool I2C_bWrite(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress)
{
    bool bRetVal;
    HI2C0_vWriteEnable(); //reset WC signal
    bRetVal = I2C_bWritePageBytes(uiAddress, pucData, uiLength, ucChipAddress);
    HI2C0_vWriteDisable(); //set WC signal
    return(bRetVal);

}
#endif
/* I2C_bIsReady()*/
/** Informs whether given chip is ready for read/write
*
*\param   ucChipAddress - hard address of chip
*\return
*   #true    - chip is ready to accept next commands \n
*   #false   - chip is unaccessible or write process is pending
*\remarks
*   - ucChipAddress    : number of chip which is really connected on I2C bus
*
*  Remember that write process takes about 5 milliseconds. During this process is chip unaccessible.\n
*  This function is called from library so you must not call it manually from your code
*/
#if defined(MCAL_MI2C_INCLUDED) && defined(I2C_BY_MCAL)
#if defined(MI2C_COMMUNICATION_TYPE_HARDWARE)

#if defined(MSPM0)
bool I2C_bIsReady(uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    funcRetVal = MI2C_quickCommand(MI2C_extEEPhandle.channel, ((uint16_t)ucChipAddress | (uint16_t)ACCESS_WRITE), MI2C_ADDRESS_LENGTH_7_BITS);
    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#else
bool I2C_bIsReady(uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal;
    //Info 845: the right operand to | always evaluates to 0
    funcRetVal = MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)ACCESS_WRITE));//lint !e845
    while(MI2C_extEEPhandle.status != MCAL_STATUS_READY)
    {
        /* wait until interrupts finish */
    }
    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif

#else
bool I2C_bIsReady(uint8_t ucChipAddress)
{
    bool bRetVal;
    enum MCAL_Error funcRetVal;

    funcRetVal = MI2C_startCommunication(&MI2C_extEEPhandle, ((uint16_t)ucChipAddress | (uint16_t)ACCESS_WRITE));
    if ( funcRetVal == MCAL_OK )
    {
        (void)MI2C_stopCommunication(&MI2C_extEEPhandle);
    }
    MI2C_extEEPhandle.status = MCAL_STATUS_READY;

    bRetVal = (funcRetVal == MCAL_OK) ? true : false;
    return bRetVal;
}
#endif
#else /* I2C by HAL */
bool I2C_bIsReady(uint8_t ucChipAddress)
{
    bool bRetVal;
    bRetVal = HI2C0_bSetAddr((uint8_t)(ucChipAddress | ACCESS_WRITE));
    if ( bRetVal != false )
    {
        HI2C0_vSendStop();
    }
    return(bRetVal);
}
#endif
/* EXTEEP_bIsAnyEepromDefect()*/
bool EXTEEP_bIsAnyEepromDefect(void)
{
    bool bReturnValue;
#ifdef GSW_USE_SINGLE_MEMORY_CHIP
    bReturnValue = EXTEEP_tChipInfo[0].bIsChipCorrupt;
#else
    bReturnValue = false;
    for(uint8_t index = 0; index < EXTEEP_ucChipCount; index++)
{
    bReturnValue = bReturnValue || EXTEEP_tChipInfo[index].bIsChipCorrupt;
}
#endif
    return (bReturnValue);
}

#if defined(EXTEEP_TEST)
/* EXTEEP_u8Test() */
uint8_t EXTEEP_u8Test(void)
{
    static volatile uint8_t  EXTEEP_u8TestCase   = EXTEEP_INIT;
    static volatile uint16_t EXTEEP_uiBytesCount = 1;
    static volatile uint32_t EXTEEP_ulAddress    = 0;
    static volatile uint8_t  EXTEEP_ucPosition   = 0; //current position to EXTEEP_aucWriteData[]
    static volatile uint8_t  bInitResult;
    static volatile bool     bReadResult;
    static volatile bool     bWriteResult;
           uint16_t u16Counter;

    switch (EXTEEP_u8TestCase)
    {

    case EXTEEP_INIT:
    {
        bInitResult = EXTEEP_ucHandleTask();
        if (bInitResult == TASK_INITIALISED)
        {
            EXTEEP_u8TestCase = EXTEEP_READY;
        }
        break;
    }
    default:
    case EXTEEP_READY:
    {
        break;
    }
    case EXTEEP_WRITE:
    {
        bWriteResult = EXTEEP_bWrite(EXTEEP_ulAddress, &EXTEEP_aucWriteData[EXTEEP_ucPosition], EXTEEP_uiBytesCount);
        if ( bWriteResult != false )
        {
            EXTEEP_u8TestCase = EXTEEP_TASKPENDING;
        }
        else
        {
            EXTEEP_u8TestCase = EXTEEP_READY;
        }
        break;
    }
    case EXTEEP_READ:
    {
        /*~K:clear read buffer*/
        for ( u16Counter = 0u; u16Counter < ITEM_COUNT; u16Counter++)
        {
            EXTEEP_aucReadData[u16Counter] = 0;
        }
        bReadResult = EXTEEP_bRead(EXTEEP_ulAddress, EXTEEP_aucReadData,EXTEEP_uiBytesCount);
        if ( bReadResult != false )
        {
            EXTEEP_u8TestCase = EXTEEP_TASKPENDING;
        }
        else
        {
            EXTEEP_u8TestCase = EXTEEP_READY;
        }
        break;
    }
    case EXTEEP_TASKPENDING:
    {
        if ( EXTEEP_tGetStatus() != EEP_READY )
        {
            (void)EXTEEP_ucHandleTask();
        }
        else
        {
            EXTEEP_u8TestCase = EXTEEP_READY;
        }
        break;
    }
    }
    return EXTEEP_u8TestCase;

}
#endif

//lint -restore -e760
