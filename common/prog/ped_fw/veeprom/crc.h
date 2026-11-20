/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          VEEPROM
 ******************************************************************************/

#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     crc.h
 *
 *  \brief    Public definitions and declarations for crc.
 *
 */

void CRC_vUpdateCrc( uint8_t* pucCrc, uint8_t ucData );

#ifdef __cplusplus
}
#endif

#endif
