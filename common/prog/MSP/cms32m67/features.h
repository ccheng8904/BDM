/**************************************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 **************************************************************************************************
 *   PROJECT          Generic SW / CMS32M67xx
 *   COMP_ABBREV      ---
***************************************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file    features.h
*
*   \brief  Definitions of features for HAL for CMS32M67xx
*
*/
#ifndef __FEATURES_H
#define __FEATURES_H

#ifdef __cplusplus
extern "C" {
#endif

/***** GPIO *****/
#if defined(CMS32M67XX)
#define HDIO_P0_PRESENT
#define HDIO_P1_PRESENT
#define HDIO_P2_PRESENT
#define HDIO_P3_PRESENT
#if defined(CMS32M67XX_HAS_PORT4)
#define HDIO_P4_PRESENT
#endif
#if defined(CMS32M67XX_HAS_PORT5)
#define HDIO_P5_PRESENT
#endif
#endif

/***** UART *****/
#if defined(UART0)
#define MUART0_PRESENT
#endif
#if defined(UART1)
#define MUART1_PRESENT
#endif

/***** I2C *****/
#if defined(I2CA)
#define MI2C0_PRESENT
#endif

/***** SPI *****/
#if defined(SSP0)
#define MSPI0_PRESENT
#endif

/***** timers  *****/
#if defined(TMR0)
#define MTIM_TMR0_PRESENT
#endif
#if defined(TMR1)
#define MTIM_TMR1_PRESENT
#endif

#ifdef __cplusplus
}
#endif

#endif
