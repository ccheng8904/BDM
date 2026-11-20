/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include "madc.mock.h"

namespace madc_m
{
    madc_mock_class madc_mock;

    enum MCAL_Error MADC_init(struct MADC_Handle *handle, const struct MADC_Periph *periph, const struct MADC_Config *cfg)
    {
        return madc_mock.MADC_init(handle, periph, cfg);
    }

    void MADC_enable(struct MADC_Handle *handle)
    {
        madc_mock.MADC_enable(handle);
    }

    void MADC_disable(struct MADC_Handle *handle)
    {
        madc_mock.MADC_disable(handle);
    }

    enum MCAL_Status MADC_getStatus(const struct MADC_Handle *handle)
    {
        return madc_mock.MADC_getStatus(handle);
    }

    void MADC_clearError(struct MADC_Handle *handle)
    {
        madc_mock.MADC_clearError(handle);
    }

    void MADC_IRQ_Handler(enum MADC_IRQ_Index index)
    {
        madc_mock.MADC_IRQ_Handler(index);
    }

    uint16_t MADC_readSample(struct MADC_Handle *handle, const struct MADC_Channel *channel)
    {
        return madc_mock.MADC_readSample(handle, channel);
    }

    bool MADC_isConversionResultValid(const struct MADC_Handle *handle)
    {
        return madc_mock.MADC_isConversionResultValid(handle);
    }

    enum MCAL_Error MADC_enableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback)
    {
        return madc_mock.MADC_enableChannelEvent(handle, channel, callback);
    }

    enum MCAL_Error MADC_disableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback)
    {
        return madc_mock.MADC_disableChannelEvent(handle, channel, callback);
    }

    enum MCAL_Error MADC_enablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback)
    {
        return madc_mock.MADC_enablePeriphScanEvent(handle, callback);
    }

    enum MCAL_Error MADC_disablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback)
    {
        return madc_mock.MADC_disablePeriphScanEvent(handle, callback);
    }

    enum MCAL_Error MADC_startScanConversion(struct MADC_Handle *handle, const struct MADC_Channel *channels[], uint8_t channelCount, uint16_t *results)
    {
        return madc_mock.MADC_startScanConversion(handle, channels, channelCount, results);
    }

    enum MCAL_Error MADC_readSampleWithValidation(struct MADC_Handle *handle, const struct MADC_Channel *channel, uint16_t *result)
    {
        return madc_mock.MADC_readSampleWithValidation(handle, channel, result);
    }
}

