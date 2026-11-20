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
 *  PROCESSOR        SH30F9X71
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
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "madc_mc.h"
#include "mcal_channels.h"
#include "mcal_assert.h"
#include "msup.h"

/* Note 9029: an unsigned value and a signed value cannot be used together as operands to | [MISRA 2012 Rule 10.4, required] */
/* Note 9031: cannot assign a composite expression of type 'unsigned8' to an object of wider type 'unsigned32' [MISRA 2012 Rule 10.6, required] */
/* Note 9032: right operand to |= is a composite expression of type 'unsigned8' which is smaller than the left operand of type 'unsigned32' */
/* Note 9033: cannot cast 'unsigned32' to different essential type 'unknown' [MISRA 2012 Rule 10.8, required] */
/* Note 9034: cannot assign 'unsigned32' to narrower essential type 'unsigned16' [MISRA 2012 Rule 10.3, required] */
//lint -save -e9029 -e9034 -e9033 -e9032 -e9031

/* Axivion Disable Style MisraC2012-12.2 : Right operand of shift operator has value 16, but should be in range 0 to 15 */
/* Axivion Disable Style MisraC2012-14.2 : Side effect in loop condition */
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define MADC_SEQ1_START_CHANNEL     4u
#define MADC_RESULT_SHIFT           4u     //Shift left value to get 16-bit result
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static ADC_TypeDef *MADC_getRegs(const struct MADC_Periph *periph);
static void MADC_enablePeriphClock(const struct MADC_Periph *periph);
static void MADC_ChannelConfig(ADC_TypeDef *adcx, uint8_t ADC_Channel, uint8_t sequence);
/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
const struct MADC_PeriphDescriptor MADC0_Descriptor = {ADC, MADC0_IRQ_INDEX, ADC_Prescaler_1};
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
const struct MADC_ChannelDescriptor MADC0_IN20_PA14_Descriptor   = {&MADC0_Descriptor, &MDIOA14_MADC0_AN20, MADC_CHANNEL20};
const struct MADC_ChannelDescriptor MADC0_IN21_PC10_Descriptor   = {&MADC0_Descriptor, &MDIOC10_MADC0_AN21, MADC_CHANNEL21};
const struct MADC_ChannelDescriptor MADC0_IN22_PC11_Descriptor   = {&MADC0_Descriptor, &MDIOC11_MADC0_AN22, MADC_CHANNEL22};
const struct MADC_ChannelDescriptor MADC0_IN23_PD9_Descriptor    = {&MADC0_Descriptor, &MDIOD9_MADC0_AN23,  MADC_CHANNEL23};
const struct MADC_ChannelDescriptor MADC0_IN24_PD10_Descriptor   = {&MADC0_Descriptor, &MDIOD10_MADC0_AN24, MADC_CHANNEL24};
const struct MADC_ChannelDescriptor MADC0_IN25_PD11_Descriptor   = {&MADC0_Descriptor, &MDIOD11_MADC0_AN25, MADC_CHANNEL25};
const struct MADC_ChannelDescriptor MADC0_IN26_PD12_Descriptor   = {&MADC0_Descriptor, &MDIOD12_MADC0_AN26, MADC_CHANNEL26};

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static const uint8_t adcSeq[8] = {ADC_Sequence_0, ADC_Sequence_1, ADC_Sequence_2, ADC_Sequence_3, \
                    ADC_Sequence_4, ADC_Sequence_5, ADC_Sequence_6, ADC_Sequence_7};

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint8_t MADC_getChannelNum(const struct MADC_Channel *channel)
{
    return (uint8_t)channel->desc->index;
}


void MADC_enablePeriph(const struct MADC_Periph *periph)
{
    /* Enable the ADC peripheral */
    MSUP_setReg32(&(MADC_getRegs(periph)->ADCON1.V32), ADC_ADCON1_ADON_Msk);
    MSUP_delay(10);
}


void MADC_disablePeriph(const struct MADC_Periph *periph)
{
    /* Disable the ADC peripheral */
    MSUP_clearReg32(&(MADC_getRegs(periph)->ADCON1.V32), ADC_ADCON1_ADON_Msk);
}


enum MCAL_Error MADC_configure(const struct MADC_Periph *periph, const struct MADC_Config *config)
{
    (void)*config;
    uint32_t tempReg1 = 0;
    uint32_t tempReg2 = 0;
    MADC_enablePeriphClock(periph);

    
    tempReg1 |=  ((uint32_t)ADC_ConversionMode_Single << (uint32_t)ADC_ADCON1_ADCTU_Pos)    \
                 | ((uint32_t)ADC_Resolution_12Bit << (uint32_t)ADC_ADCON1_MOD_Pos)                       \
                 | ((uint32_t)ADC_RefVoltage_AVDD << (uint32_t)ADC_ADCON1_REFC_Pos)                \
                 | ((uint32_t)periph->scfg.triggerSource << (uint32_t)ADC_ADCON1_ADSTRS_Pos);

    MSUP_writeReg32(&(MADC_getRegs(periph)->ADCON1.V32), ADC_ADCON1_Msk_ALL, tempReg1);

    tempReg2 |= (((uint32_t)periph->desc->clockPrescaler) << (uint32_t)ADC_ADCON2_TADC_Pos)     \
                 | ((uint32_t)ADC_NumOfConversion_1 << (uint32_t)ADC_ADCON2_ADMAXCH_Pos)        \
                 | ((uint32_t)periph->scfg.sampleTime << (uint32_t)ADC_ADCON2_TS_Pos)           \
                 | ((uint32_t)ADC_TwoSampleGap_4Cycles << (uint32_t)ADC_ADCON2_TGAP_Pos);

    MSUP_writeReg32(&(MADC_getRegs(periph)->ADCON2.V32), ADC_ADCON2_Msk_ALL, tempReg2);

    MADC_getRegs(periph)->ADGAPON = 0u;
    return MCAL_OK;
}


void MADC_stopPeriphConversion(const struct MADC_Periph *periph)
{
    /* Stop conversion of the specified ADC peripheral. */
    MSUP_clearReg32(&(MADC_getRegs(periph)->ADCON1.V32), ADC_ADCON1_ADSOC_Msk);
}


enum MCAL_Error MADC_startPeriphScanConversion(const struct MADC_Periph *periph, const struct MADC_Channel *const channels[], uint8_t channelCount)
{
    volatile uint8_t i = 0;
    /* unexpected number of channels */
    if(channelCount > 8u)
    {
        return MCAL_ERROR_PARAM;
    }
    
    MADC_disablePeriph(periph);
    

    /* Axivion Disable Style MisraC2012-13.3 : Increment of volatile object */
    /* Axivion Disable Style MisraC2012-13.2 : Unsequenced volatile accesses */
    for(i = 0; i < channelCount; i++)
    {
        volatile uint8_t index = (uint8_t)channels[i]->desc->index;
        MADC_ChannelConfig(MADC_getRegs(periph), (uint8_t)index, adcSeq[i]);
    }
    /* Axivion Enable Style MisraC2012-13.2 */
    /* Axivion Enable Style MisraC2012-13.3 */

    /* Configure the ADC conversion channel numbers */
    /* This parameter can be 0 to 7 */
    MADC_getRegs(periph)->ADCON2.BIT.ADMAXCH = (uint32_t)channelCount - 1;

    MADC_enablePeriph(periph);

    if(periph->scfg.triggerSource == ADC_Trigger_Software)
    {
        /* Start conversion of the specified ADC peripheral. */
        MADC_getRegs(periph)->ADCON1.V32 |= ADC_ADCON1_ADSOC_Msk;
    }


    return MCAL_OK;
}


uint16_t MADC_readResultReg(const struct MADC_Periph *periph, uint8_t index)
{
    volatile uint16_t retAdcValue;
    retAdcValue = (uint16_t)(*(volatile uint32_t *)((uint32_t)MADC_getRegs(periph) + adcSeq[index]));
    return (uint16_t)(retAdcValue << MADC_RESULT_SHIFT);
}


bool MADC_isPeriphEvent(const struct MADC_Periph *periph)
{
    return ((MADC_getRegs(periph)->ADINTF.V32 & ADC_FLAG_EOC) != 0u) ? true : false;
}


void MADC_enablePeriphEvent(const struct MADC_Periph *periph)
{
   (void)*periph;
}


void MADC_disablePeriphEvent(const struct MADC_Periph *periph)
{
    (void)*periph;
}


bool MADC_isScanEvent(const struct MADC_Periph *periph)
{
    (void) periph;
    /* entering IRQ function means the scanning finished */
    return true;
}


void MADC_enableScanEvent(const struct MADC_Periph *periph)
{
    MSUP_setReg32(&(MADC_getRegs(periph)->ADCON1.V32), ADC_ADCON1_ADIE_Msk);
}


void MADC_disableScanEvent(const struct MADC_Periph *periph)
{
    MSUP_clearReg32(&(MADC_getRegs(periph)->ADCON1.V32), ADC_ADCON1_ADIE_Msk);
}


void MADC_clearScanEventFlag(const struct MADC_Periph *periph)
{
    MSUP_setReg32(&(MADC_getRegs(periph)->ADINTF.V32), (ADC_FLAG_EOC | ADC_FLAG_ADG | ADC_FLAG_ADL) << 16);
}


void MADC_clearConversionEventFlag(const struct MADC_Periph *periph)
{
    (void)*periph;
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
    MADC_getRegs(periph)->ADCON1.BIT.ADDE = 1;
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
static ADC_TypeDef *MADC_getRegs(const struct MADC_Periph *periph)
{
    return periph->desc->ADCx;
}


static void MADC_enablePeriphClock(const struct MADC_Periph *periph)
{
    (void)*periph;

    /* Enable ADC clock */
    RCC_REGS_UNLOCK();
    RCC->APB0ENR.V32 |= (uint32_t)RCC_APB0_ADC;
    RCC_REGS_LOCK();
}



static void MADC_ChannelConfig(ADC_TypeDef *adcx, uint8_t channel, uint8_t sequence)
{
    uint8_t pos;

    pos = (sequence >> 2) - 3;
    if (pos < MADC_SEQ1_START_CHANNEL)
    {
        MSUP_writeReg32(&(adcx->SEQCHSEL0.V32), ADC_SEQCHSEL0_SEQCH0_Msk << ( pos << 3 ), channel << ( pos << 3 ));
    }
    else
    {
        pos -= MADC_SEQ1_START_CHANNEL;
        MSUP_writeReg32(&(adcx->SEQCHSEL1.V32), ADC_SEQCHSEL1_SEQCH4_Msk << ( pos << 3 ), channel << ( pos << 3 ));
    }
}
/* Axivion Enable Style MisraC2012-12.2 */
/* Axivion Enable Style MisraC2012-14.2 */

//lint -restore -e9029 -e9034 -e9033 -e9032 -e9031
