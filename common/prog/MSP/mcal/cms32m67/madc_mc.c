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
/** \file     madc_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MADC.
 *
 *  \details  The CMS32M67xx ADC (12 bit, 27 channels) is configured and
 *            started by the vendor driver (ADC_... functions), this file
 *            maps the MCAL interface to it. The result of a channel is
 *            available in the corresponding data register of the ADC block.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "madc_mc.h"
#include "mcal_channels.h"
#include "mcal_assert.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

/* number of channels of one scan sequence */
#define MADC_MAX_SCAN_CHANNELS          8u

/* offset of the ADC data registers in the ADC register block */
#define MADC_DATA_OFFSET                0x80u

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static ADC_Type *MADC_getRegs(const struct MADC_Periph *periph);
static void MADC_enablePeriphClock(const struct MADC_Periph *periph);
static uint32_t MADC_getChannelMask(uint8_t channel);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const struct MADC_PeriphDescriptor MADC0_Descriptor = {ADC, MADC0_IRQ_INDEX};
const struct MADC_ChannelDescriptor MADC0_IN0_PA0_Descriptor    = {&MADC0_Descriptor, &MDIOA0_MADC0_AN0,  MADC_CHANNEL0};
const struct MADC_ChannelDescriptor MADC0_IN1_PA1_Descriptor    = {&MADC0_Descriptor, &MDIOA1_MADC0_AN1,  MADC_CHANNEL1};
const struct MADC_ChannelDescriptor MADC0_IN2_PA2_Descriptor    = {&MADC0_Descriptor, &MDIOA2_MADC0_AN2,  MADC_CHANNEL2};
const struct MADC_ChannelDescriptor MADC0_IN3_PA4_Descriptor    = {&MADC0_Descriptor, &MDIOA4_MADC0_AN3,  MADC_CHANNEL3};
const struct MADC_ChannelDescriptor MADC0_IN4_PB7_Descriptor    = {&MADC0_Descriptor, &MDIOB7_MADC0_AN4,  MADC_CHANNEL4};
const struct MADC_ChannelDescriptor MADC0_IN5_PB8_Descriptor    = {&MADC0_Descriptor, &MDIOB8_MADC0_AN5,  MADC_CHANNEL5};
const struct MADC_ChannelDescriptor MADC0_IN6_PC12_Descriptor   = {&MADC0_Descriptor, &MDIOC12_MADC0_AN6, MADC_CHANNEL6};
const struct MADC_ChannelDescriptor MADC0_IN7_PC13_Descriptor   = {&MADC0_Descriptor, &MDIOC13_MADC0_AN7, MADC_CHANNEL7};
const struct MADC_ChannelDescriptor MADC0_IN8_PC14_Descriptor   = {&MADC0_Descriptor, &MDIOC14_MADC0_AN8, MADC_CHANNEL8};
const struct MADC_ChannelDescriptor MADC0_IN9_PC15_Descriptor   = {&MADC0_Descriptor, &MDIOC15_MADC0_AN9, MADC_CHANNEL9};
const struct MADC_ChannelDescriptor MADC0_IN10_PD0_Descriptor   = {&MADC0_Descriptor, &MDIOD0_MADC0_AN10,  MADC_CHANNEL10};
const struct MADC_ChannelDescriptor MADC0_IN11_PD1_Descriptor   = {&MADC0_Descriptor, &MDIOD1_MADC0_AN11,  MADC_CHANNEL11};
const struct MADC_ChannelDescriptor MADC0_IN12_PD2_Descriptor   = {&MADC0_Descriptor, &MDIOD2_MADC0_AN12,  MADC_CHANNEL12};
const struct MADC_ChannelDescriptor MADC0_IN13_PD3_Descriptor   = {&MADC0_Descriptor, &MDIOD3_MADC0_AN13,  MADC_CHANNEL13};
const struct MADC_ChannelDescriptor MADC0_IN14_PD4_Descriptor   = {&MADC0_Descriptor, &MDIOD4_MADC0_AN14,  MADC_CHANNEL14};
const struct MADC_ChannelDescriptor MADC0_IN15_PD7_Descriptor   = {&MADC0_Descriptor, &MDIOD7_MADC0_AN15,  MADC_CHANNEL15};
const struct MADC_ChannelDescriptor MADC0_IN16_PD8_Descriptor   = {&MADC0_Descriptor, &MDIOD8_MADC0_AN16,  MADC_CHANNEL16};
const struct MADC_ChannelDescriptor MADC0_IN17_PA11_Descriptor  = {&MADC0_Descriptor, &MDIOA11_MADC0_AN17, MADC_CHANNEL17};
const struct MADC_ChannelDescriptor MADC0_IN18_PA12_Descriptor  = {&MADC0_Descriptor, &MDIOA12_MADC0_AN18, MADC_CHANNEL18};
const struct MADC_ChannelDescriptor MADC0_IN19_PA13_Descriptor  = {&MADC0_Descriptor, &MDIOA13_MADC0_AN19, MADC_CHANNEL19};
const struct MADC_ChannelDescriptor MADC0_IN20_PA14_Descriptor  = {&MADC0_Descriptor, &MDIOA14_MADC0_AN20, MADC_CHANNEL20};
const struct MADC_ChannelDescriptor MADC0_IN21_PC10_Descriptor  = {&MADC0_Descriptor, &MDIOC10_MADC0_AN21, MADC_CHANNEL21};
const struct MADC_ChannelDescriptor MADC0_IN22_PC11_Descriptor  = {&MADC0_Descriptor, &MDIOC11_MADC0_AN22, MADC_CHANNEL22};
const struct MADC_ChannelDescriptor MADC0_IN23_PD9_Descriptor   = {&MADC0_Descriptor, &MDIOD9_MADC0_AN23,  MADC_CHANNEL23};
const struct MADC_ChannelDescriptor MADC0_IN24_PD10_Descriptor  = {&MADC0_Descriptor, &MDIOD10_MADC0_AN24, MADC_CHANNEL24};
const struct MADC_ChannelDescriptor MADC0_IN25_PD11_Descriptor  = {&MADC0_Descriptor, &MDIOD11_MADC0_AN25, MADC_CHANNEL25};
const struct MADC_ChannelDescriptor MADC0_IN26_PD12_Descriptor  = {&MADC0_Descriptor, &MDIOD12_MADC0_AN26, MADC_CHANNEL26};

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint8_t MADC_getChannelNum(const struct MADC_Channel *channel)
{
    return (uint8_t)channel->desc->index;
}


void MADC_enablePeriph(const struct MADC_Periph *periph)
{
    MADC_enablePeriphClock(periph);

    /* enable the ADC peripheral */
    ADC_Start();
}


void MADC_disablePeriph(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_Stop();
}


enum MCAL_Error MADC_configure(const struct MADC_Periph *periph, const struct MADC_Config *config)
{
    const struct MADC_PeriphStaticConfig *scfg = &periph->scfg;

    (void)*config;

    MADC_enablePeriphClock(periph);

    /* high speed mode, single conversion mode and reference source */
    ADC_ConfigRunMode(ADC_MODE_HIGH, ADC_CONVERT_SINGLE, scfg->clockDivider, scfg->sampleTime);
    ADC_ConfigVREF(ADC_VREFP_VDD);

    /* hardware controlled channel switching, charge/discharge disabled */
    ADC_ConfigChannelSwitchMode(ADC_SWITCH_HARDWARE);
    ADC_DisableChargeAndDischarge();

    return MCAL_OK;
}


void MADC_stopPeriphConversion(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_Stop();
}


enum MCAL_Error MADC_startPeriphScanConversion(const struct MADC_Periph *periph, const struct MADC_Channel *const channels[], uint8_t channelCount)
{
    uint32_t channelMask = 0u;
    uint8_t idx;

    if((channelCount == 0u) || (channelCount > MADC_MAX_SCAN_CHANNELS))
    {
        return MCAL_ERROR_PARAM;
    }

    ADC_Stop();
    ADC_DisableScanChannel(0xFFFFFFFFu);

    for(idx = 0u; idx < channelCount; idx++)
    {
        channelMask |= MADC_getChannelMask((uint8_t)channels[idx]->desc->index);
    }

    ADC_EnableScanChannel(channelMask);
    ADC_Start();

    if(periph->scfg.triggerSource == MADC_TRIGGER_SOFTWARE)
    {
        /* the conversion is started by the start of the ADC (software trigger) */
    }

    return MCAL_OK;
}


uint16_t MADC_readResultReg(const struct MADC_Periph *periph, uint8_t index)
{
    const volatile uint32_t *dataReg = (const volatile uint32_t *)((uint32_t)MADC_getRegs(periph) + MADC_DATA_OFFSET);

    return (uint16_t)(dataReg[index] & 0x0FFFu);
}


bool MADC_isPeriphEvent(const struct MADC_Periph *periph)
{
    return ((MADC_getRegs(periph)->RIS) != 0u) ? true : false;
}


void MADC_enablePeriphEvent(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_EnableCompareInt();
}


void MADC_disablePeriphEvent(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_DisableCompareInt();
}


bool MADC_isScanEvent(const struct MADC_Periph *periph)
{
    return ((MADC_getRegs(periph)->RIS) != 0u) ? true : false;
}


void MADC_enableScanEvent(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_EnableChannelInt(0xFFFFFFFFu);
}


void MADC_disableScanEvent(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_DisableChannelInt(0xFFFFFFFFu);
}


void MADC_clearScanEventFlag(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_ClearCompareIntFlag();
}


void MADC_clearConversionEventFlag(const struct MADC_Periph *periph)
{
    (void)*periph;

    ADC_ClearCompareIntFlag();
}


enum MADC_IRQ_Index MADC_IRQ_getIndex(const struct MADC_Periph *periph)
{
    return periph->desc->irqIndex;
}


void MADC_IRQ_enable(const struct MADC_Periph *periph)
{
    (void)*periph;

    NVIC_EnableIRQ(ADC_IRQn);
}


void MADC_enableDMA(const struct MADC_Periph *periph)
{
    (void)*periph;

    CGC_PER10PeriphClockCmd(CGC_PER10Periph_DMA, ENABLE);
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static ADC_Type *MADC_getRegs(const struct MADC_Periph *periph)
{
    return periph->desc->ADCx;
}


static void MADC_enablePeriphClock(const struct MADC_Periph *periph)
{
    (void)*periph;

    CGC_PER13PeriphClockCmd(CGC_PER13Periph_ADCEN, ENABLE);
}


static uint32_t MADC_getChannelMask(uint8_t channel)
{
    return (uint32_t)1u << channel;
}

/******************************************************************************/
/* VENDOR DRIVER EXTENSIONS                                                   */
/******************************************************************************/
/* The conversion/compare interrupt flag is cleared by the ICLR register, the
 * vendor driver provides no function for it. */
void ADC_ClearCompareIntFlag(void)
{
    ADC->LOCK = ADC_LOCK_WRITE_KEY;
    ADC->ICLR = 0x80000001U;    /* clear the channel 0 and compare 31 flags */
    ADC->LOCK = 0x00U;
}