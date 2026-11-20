/*******************************************************************************
 *  Copyright (c) 2020 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MSPI
 ******************************************************************************/


#ifndef MSPI_TYPES_H
#define MSPI_TYPES_H

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
 *  \brief    Public definitions and type definitions for unit MSPI.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "mdio_types.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/** Desired frequency in Hz to be generated on SCLK pin of MSPI channel */
#define MSPI_FREQUENCY_100K   (100000u)
#define MSPI_FREQUENCY_250K   (250000u)
#define MSPI_FREQUENCY_500K   (500000u)
#define MSPI_FREQUENCY_1M    (1000000u)
#define MSPI_FREQUENCY_2M    (2000000u)
#define MSPI_FREQUENCY_4M    (4000000u)
#define MSPI_FREQUENCY_8M    (8000000u)
#define MSPI_FREQUENCY_10M  (10000000u)
#define MSPI_FREQUENCY_16M  (16000000u)
#define MSPI_FREQUENCY_20M  (20000000u)


/** MSPI default configuration structure definitions
 *  \note These definitions are used for initialization of MSPI configuration
 *        structures.
 */
/*lint -e9026 "Function-like macro [MISRA 2012 Directive 4.9, advisory]" */
#define MSPI_CFG_DEFAULT    { MSPI_DATA_BITS_8,                  \
                              MSPI_CLOCK_POLARITY_1,             \
                              MSPI_CLOCK_PHASE_1,                \
                              MSPI_DATA_INVERT_DISABLED,         \
                              MSPI_FRAME_FORMAT_MSB_FIRST,       \
                              MSPI_CHIP_SELECT_MANUAL,           \
                              MSPI_FREQUENCY_100K,               \
                              MDIO_PULL_UP                       \
                            }; //lint !e823 "definition of macro ends in semi-colon"

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/** MSPI data bits width configuration enumeration */
enum MSPI_DataBits
{
    MSPI_DATA_BITS_8,            ///< 8-bit data width (default)
    MSPI_DATA_BITS_16            ///< 16-bit data width
};


/** MSPI clock polarity configuration enumeration */
enum MSPI_ClockPolarityCPOL
{
    MSPI_CLOCK_POLARITY_0 = 0, ///< CPOL0 - Pin SCLK has log.0 if no transmission
    MSPI_CLOCK_POLARITY_1 = 1  ///< CPOL1 - Pin SCLK has log.1 if no transmission (default)
};


/** MSPI sampling phase configuration enumeration */
enum MSPI_ClockPhaseCPHA
{
    MSPI_CLOCK_PHASE_0 = 0,  ///< CPHA0 - Data are captured (sampled) on first edge of SCLK pin and changed on second edge
    MSPI_CLOCK_PHASE_1 = 1   ///< CPHA1 - Data are captured (sampled) on second edge of SCLK pin and changed on first edge (default)
};


/** MSPI data invert configuration enumeration */
enum MSPI_DataInvert
{
    MSPI_DATA_INVERT_DISABLED,   ///< Data are not inverted for transmitter and receiver (default)
    MSPI_DATA_INVERT_ENABLED     ///< Data are inverted for transmitter and receiver
};


/** MSPI format select */
enum MSPI_FrameFormat
{
    MSPI_FRAME_FORMAT_MSB_FIRST, ///< Data is transmitted / received with the MSB first
    MSPI_FRAME_FORMAT_LSB_FIRST  ///< Data is transmitted / received with the LSB first
};


/** MSPI chip select mode **/
enum MSPI_ChipSelectMode
{
    // Signal for chip select pin has to be generated manually by user.
    MSPI_CHIP_SELECT_MANUAL,
    // Signal for chip select pin will be generated automatically. Slave chip is active low.
    MSPI_CHIP_SELECT_AUTO_LOW,
    // Signal for chip select pin will be generated automatically. Slave chip is active high.
    MSPI_CHIP_SELECT_AUTO_HIGH
};

/** MSPI DMA direction configuration enumeration */
enum MSPI_DmaDirection
{
    MSPI_DMA_DIRECTION_TX, ///< Tx buffer DMA enabled
    MSPI_DMA_DIRECTION_RX, ///< Rx buffer DMA enabled
    MSPI_DMA_DIRECTION_TX_RX ///< Tx and Rx buffer DMA enabled
};


/** MSPI generic configuration structure */
struct MSPI_Config
{
    enum MSPI_DataBits          dataBits;         ///< Number of data bits
    enum MSPI_ClockPolarityCPOL clockPolarity;    ///< Level on SCLK pin if nothing transmit
    enum MSPI_ClockPhaseCPHA    clockPhase;       ///< Determines the polarity of SCLK
    enum MSPI_DataInvert        dataInvert;       ///< Enable/Disable data inversion
    enum MSPI_FrameFormat       frameFormat;      ///< Format for transmit and receive with MSB or LSB first (Most/Least Significant Bit)
    enum MSPI_ChipSelectMode    chipSelMode;      ///< Setting for chip select
    uint32_t                    clockFreq;        ///< Generated frequency on SCLK pin
    enum MDIO_PullType          misoPullResistor; ///< Enable/Disable internal pull up/down resistor on MISO pin
};


#ifdef __cplusplus
}
#endif

#endif // MSPI_TYPES_H
