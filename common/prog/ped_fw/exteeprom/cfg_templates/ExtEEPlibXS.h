/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          EXTEEPROM
 ******************************************************************************/

#ifndef __ExtEEPlibXS_H
#define __ExtEEPlibXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief      Common module of external EEPROM library - excess header file.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
//#define EXTEEP_TEST

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
/* other processors*/
bool I2C_bInit(void);
bool I2C_bRead(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress);
bool I2C_bWrite(uint16_t uiAddress, const uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress);
bool I2C_bIsReady(uint8_t ucChipAddress);

bool EXTEEP_bIsAnyEepromDefect(void);

#if defined(EXTEEP_TEST)
uint8_t EXTEEP_u8Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
