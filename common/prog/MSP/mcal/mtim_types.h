/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MTIM
 ******************************************************************************/


#ifndef MTIM_TYPES_H
#define MTIM_TYPES_H

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
 *  \brief    Public definitions and type definitions for unit MTIM.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

enum MTIM_TimerDirection
{
    MTIM_TIMER_DIRECTION_DEFAULT,
    MTIM_TIMER_DIRECTION_UP,
    MTIM_TIMER_DIRECTION_DOWN,
    MTIM_TIMER_DIRECTION_UP_DOWN
};


enum MTIM_TimerMode
{
    MTIM_TIMER_MODE_TIMER,      ///< Source of signal is clock bus
    MTIM_TIMER_MODE_COUNTER,    ///< Source of signal is
    MTIM_TIMER_MODE_ENCODER,    ///< Quadrature encoder mode
    __MTIM_TIMER_MODE_COUNT
};


enum MTIM_TimerPwmMode
{
    MTIM_TIMER_PWM_MODE_NORMAL,         ///< Default timer counting direction
    MTIM_TIMER_PWM_MODE_CENTER_ALIGNED  ///< Timer in up-down counting mode
};


/** MTIM timer configuration structure */
struct MTIM_TimerConfig
{
    uint32_t                    prescaler;
    uint32_t                    reloadValue;
    enum MTIM_TimerDirection    direction;
    enum MTIM_TimerMode         mode;
};


/** MTIM timer configuration structure for PWM */
struct MTIM_TimerConfigPWM
{
    uint32_t                    frequency;      ///< Desired frequency
    enum MTIM_TimerPwmMode      pwmMode;
};


enum MTIM_ChannelMode
{
    MTIM_CHANNEL_MODE_CAPTURE,
    MTIM_CHANNEL_MODE_COMPARE,
    MTIM_CHANNEL_MODE_PWM,
    __MTIM_CHANNEL_MODE_COUNT
};


enum MTIM_ChannelCaptureTrigger
{
    MTIM_CHANNEL_CAPTURE_TRIGGER_DISABLED,
    MTIM_CHANNEL_CAPTURE_TRIGGER_RISING,
    MTIM_CHANNEL_CAPTURE_TRIGGER_FALLING,
    MTIM_CHANNEL_CAPTURE_TRIGGER_RISING_FALLING,
    __MTIM_CHANNEL_CAPTURE_TRIGGER_COUNT
};


enum MTIM_ChannelCompareAction
{
    MTIM_CHANNEL_COMPARE_ACTION_NONE,
    MTIM_CHANNEL_COMPARE_ACTION_CLEAR,
    MTIM_CHANNEL_COMPARE_ACTION_SET,
    MTIM_CHANNEL_COMPARE_ACTION_TOGGLE,
    __MTIM_CHANNEL_COMPARE_ACTION_COUNT
};


/** MTIM channel configuration structure */
struct MTIM_ChannelConfig
{
    enum MTIM_ChannelMode           mode;
    enum MTIM_ChannelCaptureTrigger captureTrigger;
    enum MTIM_ChannelCompareAction  compareAction;
};


#ifdef __cplusplus
}
#endif

#endif // MTIM_TYPES_H
