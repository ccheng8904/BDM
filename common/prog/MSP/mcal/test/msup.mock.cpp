/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*  are protected by international copyright laws.
*
*  All rights reserved. This program and the accompanying materials
*  Please contact copyright holder for licensing information.
*******************************************************************************/

#include "msup.mock.h"

namespace msup_m
{
    msup_mock_class msup_mock;

    void MSUP_handleTimerExt(struct MSUP_TimeoutHandle *handle)
    {
        msup_mock.MSUP_handleTimerExt(handle);
    }

    void MSUP_startTimeMeasureExt(struct MSUP_TimeoutHandle *handle)
    {
        msup_mock.MSUP_startTimeMeasureExt(handle);
    }

    void MSUP_setTimeOutExt(struct MSUP_TimeoutHandle *handle, uint32_t timeout)
    {
        msup_mock.MSUP_setTimeOutExt(handle, timeout);
    }

    uint32_t MSUP_getMeasuredTimeExt(struct MSUP_TimeoutHandle *handle)
    {
        return msup_mock.MSUP_getMeasuredTimeExt(handle);
    }

    bool MSUP_isTimeOutExt(struct MSUP_TimeoutHandle *handle)
    {
        return msup_mock.MSUP_isTimeOutExt(handle);
    }

    void MSUP_delay(uint16_t delay)
    {
        msup_mock.MSUP_delay(delay);
    }

    void MSUP_handleTimer(void)
    {
        msup_mock.MSUP_handleTimer();
    }

    void MSUP_startTimeMeasure(void)
    {
        msup_mock.MSUP_startTimeMeasure();
    }

    void MSUP_setTimeOut(uint32_t timeout)
    {
        msup_mock.MSUP_setTimeOut(timeout);
    }

    uint32_t MSUP_getMeasuredTime(void)
    {
        return msup_mock.MSUP_getMeasuredTime();
    }

    bool MSUP_isTimeOut(void)
    {
        return msup_mock.MSUP_isTimeOut();
    }

    void HSUP_disableInt(void)
    {
        msup_mock.HSUP_disableInt();
    }

    void HSUP_enableInt(void)
    {
        msup_mock.HSUP_enableInt();
    }

    void MSUP_generateReset(void)
    {
        msup_mock.MSUP_generateReset();
    }

}

