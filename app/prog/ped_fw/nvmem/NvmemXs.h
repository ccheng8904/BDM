/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      NVMEM
 ******************************************************************************/
#ifndef __NVMEMXS_H
#define __NVMEMXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file NVMEMXS.h
 * published application specific definitions and declarations for Non Volatile 
 * Memory.
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/*~A:Definitions and Declarations */
/**
 * NVMEM_CACHE_BUFFER_SIZE defines the size in bytes of buffer for caching data. This is needed for 
 * initialization of NVMEM_bInit(). This buffer must as large or larger than the largest drawer in
 * CACHE_tPartitionTableVeeprom!
 */
#define NVMEM_CACHE_BUFFER_SIZE 40U
/**
 * Application must adapt this parameter to the number of drawers divided by 8 ((CACHE_tNumberOfDrawersVEEPROM + CACHE_ucNumberOfDrawersEEPROM + 7)/ 8)
 */
#define NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE         75U
//#define TEST_ACTIVE
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

#if defined(EXTERNAL_LARGE)
typedef uint16 tDrawerID;
#endif

/**
 * Application must adapt this buffer to the size of ((largest payload of CACHE_tPartitionTableVeeprom) + 2)
 */
extern uchar NVMEM_aucCacheBuffer[];
/**
 * Application must adapt this buffer to the number of drawers ((CACHE_tNumberOfDrawersVEEPROM + CACHE_ucNumberOfDrawersEEPROM + 7)/ 8)
 */
extern uchar NVMEM_aucDirtyFlags[];
/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/**
 * \brief this function is called from NVMEM_vRefreshRamCache()
 * 
 * this function is called from while in process of refreshing ram-cache to avoid beeing bit from watchdog
 */
void _NVMEM_vResetWatchdogTimer( void );
#if defined(TEST_ACTIVE)
uchar NVMEM_ucTest(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
