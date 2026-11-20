/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          NVMEM
 ******************************************************************************/
#ifndef CACHE_PRIV_H
#define CACHE_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file cachePriv.h
 *
 *  \brief   Definitions and declarations of Caching Data in NVMEM
 *
 *   This definitions are used in configuration file cacheXS.c
 */
 
 /******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"

/* TYPE DEFINITIONS ***********************************************************************************/
/**
 * structure containing required information of drawer for virtual EEPROM
 */
typedef struct
{
   uint8_t *pucDataCacheLocation;    /**< pointing to where data is cached in RAM */
   uint8_t ucSize;                  /**< size of this drawer*/
}tVEEPROMDrawerType;
/**
 * structure containing required information of drawer for external EEPROM
 */
typedef struct
{
   uint8_t *pucDataCacheLocation;    /**< pointing to where data is cached in RAM */
   uint8_t ucSize;                  /**< size of this drawer*/
   uint32_t ulAddress;               /**< address in external EEPROM memory */
}tEEPROMDrawerType;


#ifdef __cplusplus
}
#endif

#endif // CACHE_PRIV_H
