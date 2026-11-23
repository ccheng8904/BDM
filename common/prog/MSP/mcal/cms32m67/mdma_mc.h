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
 *  COMP_ABBREV      MDMA
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdma_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MDMA.
 *
 *  \details  The CMS32M67xx DMA is provided by the vendor driver (DMA_...).
 *            The application of the framework does not use the MDMA module,
 *            therefore only the interface required by the framework is
 *            provided here.
 *
 */

#ifndef MDMA_MC_H
#define MDMA_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mdma_types.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define MDMA_CHANNEL_SCFG_DEFAULT           {MDMA_PRIORITY_LOW,MDMA_EVENT_SOURCE_FUNC0}


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MDMA_IRQ_Index
{
    MDMA1_IRQ_INDEX,
    MDMA_IRQ_CNT
};


struct MDMA_PeriphDescriptor
{
    DMA_Type                *DMAx;
    enum MDMA_IRQ_Index     irqIndex;
};


struct MDMA_Periph
{
    const struct MDMA_PeriphDescriptor      *desc;
};


enum MDMA_EventSource
{
    MDMA_EVENT_SOURCE_FUNC0   = 0,
    MDMA_EVENT_SOURCE_FUNC1   = 1,
    MDMA_EVENT_SOURCE_FUNC2   = 2,
    MDMA_EVENT_SOURCE_FUNC3   = 3,
    MDMA_EVENT_SOURCE_FUNC4   = 4,
    MDMA_EVENT_SOURCE_FUNC5   = 5,
    MDMA_EVENT_SOURCE_FUNC6   = 6,
    MDMA_EVENT_SOURCE_FUNC7   = 7,
};


enum MDMA_Priority
{
    MDMA_PRIORITY_LOW       = 0u,
    MDMA_PRIORITY_MED       = 1u,
    MDMA_PRIORITY_HIGH      = 2u,
    MDMA_PRIORITY_VERY_HIGH = 3u
};


struct MDMA_ChannelStaticConfig
{
    enum MDMA_Priority      priority;
    enum MDMA_EventSource   eventSource;
};

/* the DMA channels are selected by the DMA vector number */
#ifndef MDMA_CHANNEL_INDEX_TYPE_DEFINED
#define MDMA_CHANNEL_INDEX_TYPE_DEFINED
typedef uint32_t MDMA_ChannelIndex;
#endif

struct MDMA_ChannelDescriptor
{
    DMA_Type                *dma_t;
    MDMA_ChannelIndex       index;
};


struct MDMA_Channel
{
    const struct MDMA_ChannelDescriptor     *desc;
    const struct MDMA_ChannelStaticConfig   scfg;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MDMA_PeriphDescriptor MDMA_Descriptor;

extern const struct MDMA_ChannelDescriptor MDMA_CH0_Descriptor;
extern const struct MDMA_ChannelDescriptor MDMA_CH1_Descriptor;
extern const struct MDMA_ChannelDescriptor MDMA_CH2_Descriptor;
extern const struct MDMA_ChannelDescriptor MDMA_CH3_Descriptor;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
void MDMA_configureChannelEventSource(const struct MDMA_Channel *channel, enum MDMA_EventSource eventSource);
void MDMA_configureChannelPriority(const struct MDMA_Channel *channel, enum MDMA_Priority priority);
enum MCAL_Error MDMA_lockChannel(const struct MDMA_Channel *channel);
enum MCAL_Error MDMA_unlockChannel(const struct MDMA_Channel *channel);


#ifdef __cplusplus
}
#endif

#endif // MDMA_MC_H
