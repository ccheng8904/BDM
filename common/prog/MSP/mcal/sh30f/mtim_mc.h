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
 *  PROCESSOR        SH30F9x71
 ******************************************************************************/


#ifndef MTIM_MC_H
#define MTIM_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mtim_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MTIM.
 *
 */


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
#define MTIM_PCA_CLOCK_SOURCE_DEFAULT       PCLK0

#define MTIM_TIMER_SCFG_PCA_DEFAULT         { MTIM_PCA_CLOCK_SOURCE_DEFAULT}

/* Channel static configuration default values */
#define MTIM_CHANNEL_SCFG_DEFAULT           {}


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
enum MTIM_IRQ_Index
{
    MTIM_PCA0_IRQ_INDEX,
    MTIM_PCA1_IRQ_INDEX,
    MTIM_PCA2_IRQ_INDEX,
    MTIM_PCA3_IRQ_INDEX,
    MTIM_IRQ_CNT
};

enum MTIM_ChannelIndex
{
    MTIM_CHANNEL0_INDEX = 0,
    MTIM_CHANNEL1_INDEX = 1,
};

struct MTIM_ClockEnable
{
    volatile uint32_t   *enableReg;
    uint32_t            enableMask;
};


/*****************************       PCA       ********************************/

struct MTIM_PcaStaticConfig
{
    PCA_CLOCK_SOURCE_Type      clockSource;
};

/*lint -esym(9018,*MTIM_regs_u)  "union '...' declared [MISRA 2012 Rule 19.2, advisory]" */
struct MTIM_PcaDescriptor
{
    /* Peripheral PCA registers base address */
    PCA_TypeDef                            *regsPCAx;
     
    /* Number of channels for PWM */
    uint8_t                                 pcaChannelCount;
    
    /* Structure used for enabling of clock */
    const struct MTIM_ClockEnable           clockEnable;
    
    /* Interrupt index */
    enum MTIM_IRQ_Index                     pcaIrqIndex;
};

struct MTIM_Pca
{
    const struct MTIM_PcaDescriptor       *desc;

    const struct MDIO_Channel             *pin;

    /* PCA static configuration */
    const struct MTIM_PcaStaticConfig     scfg;
};

struct MTIM_Timer
{
    const struct MTIM_Pca     pca;
};
/* extensible MTIM_Timer struct, could include additional MTIM PWM in future
struct MTIM_Timer
{
    union {
        const struct MTIM_Pca     pca;
        const struct MTIM_Tim     tim;
        const struct MTIM_Pwm     Pwm;
    }MTimer;
};*/

struct MTIM_ChannelDescriptor
{
    const struct MTIM_PcaDescriptor     *pcaDesc;
    volatile uint32_t                   *countReg;
    enum MTIM_ChannelIndex               index;
};
/* extensible channel struct, could include additional MTIM PWM channeles in future
struct MTIM_ChannelDescriptor
{
    union {
        const struct MTIM_PcaDescriptor     *pcaDesc;
        const struct MTIM_TimDescriptor     *timDesc;
        const struct MTIM_PwmDescriptor     *pwmDesc;
    }desc;
    volatile uint32_t                   *countReg;
    enum MTIM_ChannelIndex               index;
};*/

struct MTIM_Channel

{
    const struct MTIM_ChannelDescriptor     *channelDesc;
    
    const struct MDIO_Channel               *pin;
};

/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
extern const struct MTIM_PcaDescriptor MTIM_PCA0_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA0_CH0_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA0_CH1_Descriptor;

extern const struct MTIM_PcaDescriptor MTIM_PCA1_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA1_CH0_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA1_CH1_Descriptor;

extern const struct MTIM_PcaDescriptor MTIM_PCA2_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA2_CH0_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA2_CH1_Descriptor;

extern const struct MTIM_PcaDescriptor MTIM_PCA3_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA3_CH0_Descriptor;
extern const struct MTIM_ChannelDescriptor MTIM_PCA3_CH1_Descriptor;

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
extern enum MCAL_Error MTIM_configurePcaClockSource(const struct MTIM_Pca *pca, PCA_CLOCK_SOURCE_Type clockSource);

#ifdef __cplusplus
}
#endif

#endif // MTIM_MC_H
