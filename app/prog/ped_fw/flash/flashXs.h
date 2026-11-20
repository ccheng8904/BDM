/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          FLASH_LIB
 ******************************************************************************/
#ifndef FLASHLIBXS_H
#define FLASHLIBXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     FlashXS.h
 *  \brief    published application specific definitions and declarations for Flash Library
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
//#define FLASH_TEST
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
#if defined(FLASH_TEST)
uint8_t FLASH_u8Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif //FLASHLIBXS_H

