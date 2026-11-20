/**************************************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 **************************************************************************************************
 *   PROJECT          Generic SW / GD32F330
 *   COMP_ABBREV      ---
 *   AUTHOR           Tomas Hrasok
 *   CREATED          03.03.2023
***************************************************************************************************/

/**************************************************************************************************/
/* DOCUMENTATION                                                                                  */
/**************************************************************************************************/
/** \file    features.h
*
*   \brief  Definitions of features for HAL for GD32F330xx
*
*/
#ifndef __FEATURES_H
#define __FEATURES_H

#ifdef __cplusplus
extern "C" {
#endif

/***** GPIO *****/
#if defined(GPIOA)
#define HDIO_PA_PRESENT
#endif
#if defined(GPIOB)
#define HDIO_PB_PRESENT
#endif
#if defined(GPIOC)
#define HDIO_PC_PRESENT
#endif
#if defined(GPIOD)
#define HDIO_PD_PRESENT
#endif
#if defined(GPIOE)
#define HDIO_PE_PRESENT
#endif
#if defined(GPIOF)
#define HDIO_PF_PRESENT
#endif
#if defined(GPIOG)
#define HDIO_PG_PRESENT
#endif
#if defined(GPIOH)
#define HDIO_PH_PRESENT
#endif
#if defined(GPIOI)
#define HDIO_PI_PRESENT
#endif

/***** UART *****/
#if defined(UART0)
#define MUART0_PRESENT
#endif
#if defined(UART1)
#define MUART1_PRESENT
#endif
#if defined(UART2)
#define MUART2_PRESENT
#endif
#if defined(UART3)
#define MUART3_PRESENT
#endif

/***** I2C *****/
#if defined(TWI0)
#define MI2C0_PRESENT
#endif

/***** SPI *****/
#if defined(SPI0)
#define MSPI0_PRESENT
#endif
#if defined(SPI1)
#define MSPI1_PRESENT
#endif

/***** timers  *****/
#if defined(PCA0)
#define MTIM_PCA0_PRESENT
#endif
#if defined(PCA1)
#define MTIM_PCA1_PRESENT
#endif
#if defined(PCA2)
#define MTIM_PCA2_PRESENT
#endif
#if defined(PCA3)
#define MTIM_PCA3_PRESENT
#endif

#ifdef __cplusplus
}
#endif

#endif
