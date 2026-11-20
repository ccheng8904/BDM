/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include "madc_priv.mock.h"

namespace madc_priv_m
{
    madc_priv_mock_class madc_priv_mock;

    enum MCAL_Error MADC_configure(const struct MADC_Periph *periph, const struct MADC_Config *config)
    {
        return madc_priv_mock.MADC_configure(periph, config);
    }

    void MADC_enablePeriph(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_enablePeriph(periph);
    }

    void MADC_disablePeriph(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_disablePeriph(periph);
    }

    void MADC_IRQ_enable(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_IRQ_enable(periph);
    }

    uint16_t MADC_readResultReg(const struct MADC_Periph *periph, uint8_t index)
    {
        return madc_priv_mock.MADC_readResultReg(periph, index);
    }

    bool MADC_isPeriphEvent(const struct MADC_Periph *periph)
    {
        return madc_priv_mock.MADC_isPeriphEvent(periph);
    }

    void MADC_stopPeriphConversion(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_stopPeriphConversion(periph);
    }

    void MADC_enablePeriphEvent(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_enablePeriphEvent(periph);
    }

    void MADC_disablePeriphEvent(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_disablePeriphEvent(periph);
    }

    uint8_t MADC_getChannelNum(const struct MADC_Channel *channel)
    {
        return madc_priv_mock.MADC_getChannelNum(channel);
    }

    enum MCAL_Error MADC_startPeriphScanConversion(const struct MADC_Periph *periph, const struct MADC_Channel *const channels[], uint8_t channelCount)
    {
        return madc_priv_mock.MADC_startPeriphScanConversion(periph, channels, channelCount);
    }

    void MADC_enableScanEvent(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_enableScanEvent(periph);
    }

    void MADC_disableScanEvent(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_disableScanEvent(periph);
    }

    void MADC_clearScanEventFlag(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_clearScanEventFlag(periph);
    }

    void MADC_clearConversionEventFlag(const struct MADC_Periph *periph)
    {
        madc_priv_mock.MADC_clearConversionEventFlag(periph);
    }

    bool MADC_isScanEvent(const struct MADC_Periph *periph)
    {
        return madc_priv_mock.MADC_isScanEvent(periph);
    }

    enum MADC_IRQ_Index MADC_IRQ_getIndex(const struct MADC_Periph *periph)
    {
        return madc_priv_mock.MADC_IRQ_getIndex(periph);
    }

}

