/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          FLASH_LIB
 ******************************************************************************/

#ifndef FLASHMC_H
#define FLASHMC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/  
/** \file     FlashMc.h
 *
 *  \brief    Public declarations and definitions of flash types and defines
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "flash.h"

/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
extern tStatusCode FLASH_tFunctionStatus;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/* not for public use*/
bool FLASH_bEnableAccessMc(void);
bool FLASH_bDisableAccessMc(void);
bool FLASH_bWriteMc(tAddress *ptAddr, tWriteData *ptData, uint16_t uiSize);
bool FLASH_bWriteByteMc(tAddress *ptAddr, uint8_t ucData);
bool FLASH_bReadMc(tAddress *ptAddr, tReadData *ptData, uint16_t uiSize);
bool FLASH_bEraseMc(tAddress *ptAddress);

#ifdef __cplusplus
}
#endif

#endif //FLASHMC_H
