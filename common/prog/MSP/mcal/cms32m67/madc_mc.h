/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MADC
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     madc_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MADC.
 *
 */

#ifndef MADC_MC_H
#define MADC_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/madc_types.h"
#include "mcal/mdio.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/* Peripheral static configuration default values */
#define MADC_SAMPLE_TIME_DEFAULT            100U     ///< sample and hold time in ADC clocks
#define MADC_CLOCK_DIVIDER_DEFAULT          ADC_CLK_DIV_32
#define MADC_SCFG_DEFAULT       {                                   \
                                    MADC_SAMPLE_TIME_DEFAULT,       \
                                    MADC_CLOCK_DIVIDER_DEFAULT,     \
                                    MADC_TRIGGER_SOFTWARE,          \
                                }


/* Channel static configuration default values */
#define MADC_CHANNEL_SAMPLE_TIME_DEFAULT    MADC_CHANNEL_SAMPLE_TIME_SLOW

#define MADC_CHANNEL_SCFG_DEFAULT           {MADC_CHANNEL_SAMPLE_TIME_DEFAULT}


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MADC_IRQ_Index
{
    MADC0_IRQ_INDEX,
    MADC_IRQ_CNT
};

typedef uint32_t            MADC_ClockDivider;
typedef uint32_t            MADC_SampleTime;

enum MADC_TriggerPolarity
{
    MADC_TRIGGER_POLARITY_DISABLED          = 0U,
    MADC_TRIGGER_POLARITY_RISING            = 1U,
    MADC_TRIGGER_POLARITY_FALLING           = 2U,
    MADC_TRIGGER_POLARITY_RISING_FALLING    = 3U
};


struct MADC_PeriphStaticConfig
{
    MADC_SampleTime                         sampleTime;
    MADC_ClockDivider                       clockDivider;
    enum MADC_Trigger                       triggerSource;
};


struct MADC_PeriphDescriptor
{
    ADC_Type                                *ADCx;
    enum MADC_IRQ_Index                     irqIndex;
};


struct MADC_Periph
{
    const struct MADC_PeriphDescriptor      *desc;
    const struct MADC_PeriphStaticConfig    scfg;
};


enum MADC_ChannelSampleTime
{
    MADC_CHANNEL_SAMPLE_TIME_SLOW   = 0U,
    MADC_CHANNEL_SAMPLE_TIME_FAST   = 1U
};


struct MADC_ChannelStaticConfig
{
    enum MADC_ChannelSampleTime     sampleTime;
};

enum MADC_ChannelIndex
{
    MADC_CHANNEL0   = 0,
    MADC_CHANNEL1   = 1,
    MADC_CHANNEL2   = 2,
    MADC_CHANNEL3   = 3,
    MADC_CHANNEL4   = 4,
    MADC_CHANNEL5   = 5,
    MADC_CHANNEL6   = 6,
    MADC_CHANNEL7   = 7,
    MADC_CHANNEL8   = 8,
    MADC_CHANNEL9   = 9,
    MADC_CHANNEL10  = 10,
    MADC_CHANNEL11  = 11,
    MADC_CHANNEL12  = 12,
    MADC_CHANNEL13  = 13,
    MADC_CHANNEL14  = 14,
    MADC_CHANNEL15  = 15,
    MADC_CHANNEL16  = 16,
    MADC_CHANNEL17  = 17,
    MADC_CHANNEL18  = 18,
    MADC_CHANNEL19  = 19,
    MADC_CHANNEL20  = 20,
    MADC_CHANNEL21  = 21,
    MADC_CHANNEL22  = 22,
    MADC_CHANNEL23  = 23,
    MADC_CHANNEL24  = 24,
    MADC_CHANNEL25  = 25,
    MADC_CHANNEL26  = 26
};

enum MADC_NumOfConversion
{
    MADC_NumOfConversion_1         =    0,     /* set number of ADC conversion */
    MADC_NumOfConversion_2         =    1,     /* set number of ADC conversion */
    MADC_NumOfConversion_3         =    2,     /* set number of ADC conversion */
    MADC_NumOfConversion_4         =    3,     /* set number of ADC conversion */
    MADC_NumOfConversion_5         =    4,     /* set number of ADC conversion */
    MADC_NumOfConversion_6         =    5,     /* set number of ADC conversion */
    MADC_NumOfConversion_7         =    6,     /* set number of ADC conversion */
    MADC_NumOfConversion_8         =    7,     /* set number of ADC conversion */
};
struct MADC_ChannelDescriptor
{
    const struct MADC_PeriphDescriptor      *periphDesc;
    const struct MDIO_Channel               *pin;
    enum MADC_ChannelIndex                  index;
};


struct MADC_Channel
{
    const struct MADC_ChannelDescriptor     *desc;
    const struct MADC_ChannelStaticConfig   scfg;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MADC_PeriphDescriptor MADC0_Descriptor;

extern const struct MADC_ChannelDescriptor MADC0_IN0_PA0_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN1_PA1_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN2_PA2_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN3_PA4_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN4_PB7_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN5_PB8_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN6_PC12_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN7_PC13_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN8_PC14_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN9_PC15_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN10_PD0_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN11_PD1_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN12_PD2_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN13_PD3_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN14_PD4_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN15_PD7_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN16_PD8_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN17_PA11_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN18_PA12_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN19_PA13_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN20_PA14_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN21_PC10_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN22_PC11_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN23_PD9_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN24_PD10_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN25_PD11_Descriptor;
extern const struct MADC_ChannelDescriptor MADC0_IN26_PD12_Descriptor;


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Function enabling of DMA event generation.
 *  \param periph: pointer to peripheral instance
 */
void MADC_enableDMA(const struct MADC_Periph *periph);


#ifdef __cplusplus
}
#endif

#endif // MADC_MC_H
