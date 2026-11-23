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
 *  COMP_ABBREV      MTIM
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mtim_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MTIM.
 *
 *  \details  The CMS32M67xx integration of MTIM supports the 32 bit general
 *            purpose timers TMR0 and TMR1 (reload timer) and the 16 bit
 *            four channel timer TM00 (TM40 block, overflow interrupt only).
 *
 */

#ifndef MTIM_MC_H
#define MTIM_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mtim_types.h"
#include "mcal/mdio.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#define MTIM_MULTIPLE_HANDLES_PER_IRQ

/* Peripheral static configuration default values */
#define MTIM_CLOCK_SOURCE_DEFAULT       0U
#define MTIM_SCFG_DEFAULT                { MTIM_CLOCK_SOURCE_DEFAULT }

/* Channel static configuration default values */
#define MTIM_CHANNEL_SCFG_DEFAULT       {}

/* The CMS32M67xx reload timers provide 32 bit timer/counter registers */
#define MTIM_TIMER_MAX_RELOAD_VALUE     0xFFFFFFFFUL

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MTIM_IRQ_Index
{
    MTIM_TMR0_IRQ_INDEX,        ///< 32 bit timer TMR0
    MTIM_TMR1_IRQ_INDEX,        ///< 32 bit timer TMR1
    MTIM_TM00_IRQ_INDEX,        ///< 16 bit timer TM00 channel 0
    MTIM_TM01_IRQ_INDEX,        ///< 16 bit timer TM01 channel 1
    MTIM_TM02_IRQ_INDEX,        ///< 16 bit timer TM02 channel 2
    MTIM_TM03_IRQ_INDEX,        ///< 16 bit timer TM03 channel 3
    MTIM_TM01H_IRQ_INDEX,       ///< 16 bit timer TM01 upper 8 bit
    MTIM_IRQ_CNT
};

enum MTIM_ChannelIndex
{
    MTIM_CHANNEL0_INDEX = 0,
    MTIM_CHANNEL1_INDEX = 1,
    MTIM_CHANNEL2_INDEX = 2,
    MTIM_CHANNEL3_INDEX = 3
};

/*****************************     Timer     *********************************/

struct MTIM_TimerDescriptor
{
    /* Peripheral timer register base address */
    TMR_T                               *regsTMRx;

    /* Number of channels for PWM */
    uint8_t                             channelCount;

    /* Interrupt index */
    enum MTIM_IRQ_Index                 irqIndex;

    /* Interrupt number */
    IRQn_Type                           irqNum;
};

struct MTIM_Timer
{
    const struct MTIM_TimerDescriptor   *desc;

    const struct MDIO_Channel           *pin;
};

struct MTIM_ChannelDescriptor
{
    const struct MTIM_TimerDescriptor   *timerDesc;
    enum MTIM_ChannelIndex              index;
};

struct MTIM_Channel
{
    const struct MTIM_ChannelDescriptor *channelDesc;

    const struct MDIO_Channel           *pin;
};

/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MTIM_TimerDescriptor MTIM_TMR0_Descriptor;
extern const struct MTIM_TimerDescriptor MTIM_TMR1_Descriptor;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // MTIM_MC_H
