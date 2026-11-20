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
#ifndef CACHE_H
#define CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file cache.h
 *
 *  \brief  Interfaces for the caching of Non Volatile Memory.
 *
 * \page pageTitle
 * \chapter chaTitle Functionality for caching data of non volatile
 *
 * Some technologies of non volatile memory (e.g. FLASH) are not too fast in reading / writing data. So
 * it is a good idea to speed up the process of reading. This is the aim of this module.
 *
 * \section Interface
 * Following functions are provided by this library:
 *  -  CACHE_ucGetDrawerSize()
 *  -  CACHE_ucGetLabel()
 *  -  CACHE_ucGetCacheAddress()
 */
 
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "cachepriv.h"
#include "utility/utility.h"
#if defined(CACHE_INCLUDE_VEEPROM)
#include "VeepromMcXS.h"
#endif
#if defined(EXTERNAL_LARGE)
#include "nvmem/NvmemXs.h"
#endif
#if defined(VEEPROM_LARGE) || defined(EXTERNAL_LARGE)
/**
 * \brief the one and only invalid drawer identifier
 */
#define CACHE_INVALID_DRAWER_IDENTIFIER                          0xFFFFU
#else
/**
 * \brief the one and only invalid drawer identifier
 */
#define CACHE_INVALID_DRAWER_IDENTIFIER                          0xFFU
#endif //defined(VEEPROM_LARGE) || defined(EXTERNAL_LARGE)

#ifdef CACHE_INCLUDE_VEEPROM
/**
 * \brief partition table defining partition of the VEEPROM
 *
 * The entries of this array define the location where data is cached,
 * length of each portion of data that is stored in one drawer. The index of 
 * the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       Partition Table for VEEPROM contains 3 drawers.
 *       A Flag, and two arrays with given size
 *       tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] = 
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag)}, 
 *          {&XXX_pucData[0], 16},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData)},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data: 
 *        - CACHE_tPartitionTableVeeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableVeeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableVeeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and 
 *       - Cache::CACHE_pucGetCacheAddress()
 */
extern const tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ];
/**
 * \brief bitfield storing if cached data is valid
 *
 * bit 0 shows validity of cached value(s) for drawer 0, bit 1 for drawer 1, ... bit n for drawer n.
 * If bit is set, the cached value for this drawer is valid, otherwise not.
 */
extern uint8_t CACHE_aucDrawerCacheValidVeeprom[];
/**
 * \brief checks, if value for data-cache is valid
 *
 * \todo replace with real function (better for optimisation)
 *
 * return TRUE, if desired "cache-valid-bit" is set, otherwise FALSE
 */
#define CACHE_bIsCacheValidVeeprom(ucDrawer) UTI_TEST( (uint8_t)CACHE_aucDrawerCacheValidVeeprom[ (ucDrawer)/8U ], \
                                                        1U << ((ucDrawer)%8U)  )
/**
 * \brief sets the specified data-cache-bit
 *
 * \todo replace with real function (better for optimisation)
 *
 * This shows that the cached value(s) is/are valid.
 */
#define CACHE_vSetCacheValidVeeprom(ucDrawer)   UTI_SET( CACHE_aucDrawerCacheValidVeeprom[ (ucDrawer)/8U ], \
                                                         1U << ((ucDrawer)%8U))
/**
 *\brief unset the specified data-cache-bit
 *
 * \todo replace with real function (better for optimisation)
 *
 * This shows that the cached value(s) is/are invalid
 */
#define CACHE_vSetCacheInvalidVeeprom(ucDrawer) UTI_RES( CACHE_aucDrawerCacheValidVeeprom[ (ucDrawer)/8U ], \
                                                         1U << ((ucDrawer)%8U))

#if defined(VEEPROM_LARGE)
/**
 * \brief number of drawers of VEEPROM partition table
 */
extern const tDrawerID CACHE_tNumberOfDrawersVEEPROM;
#else
/**
 * \brief number of drawers of VEEPROM partition table
 */
extern const uint8_t CACHE_ucNumberOfDrawersVEEPROM;
#endif //defined(VEEPROM_LARGE)
#endif //CACHE_INCLUDE_VEEPROM

#ifdef CACHE_INCLUDE_EEPROM
/**
 * \brief partition table defining partition of the EEPROM
 *
 * The entries of this array define the location where data is cached and its
 * length. The index within the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       Partition Table of Eeprom contains 3 drawers: one Flag and 2 arrays of given size
 *       tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] = 
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag), ulAddEEP_1}, 
 *          {&XXX_pucData[0], 16, ulAddEEP_2},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData), ulAddEEP_3},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data: 
 *        - CACHE_tPartitionTableEeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableEeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableEeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and 
 *       - Cache::CACHE_pucGetCacheAddress()
 *
 */
extern const tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ];
extern uint8_t CACHE_aucDrawerCacheValidEeprom[];
/**
 * \brief checks, if value for data-cache is valid
 *
 * \todo replace with real function (better for optimisation)
 *
 * return TRUE, if desired "cache-valid-bit" is set, otherwise FALSE
 */
static inline bool CACHE_bIsCacheValidEeprom( uint16_t u16Drawer )
{
    return (UTI_TEST( (uint8_t)(CACHE_aucDrawerCacheValidEeprom[ (u16Drawer)/8U ]), 1U << ((u16Drawer)%8U) ) != 0U);
}

/**
 * \brief sets the specified data-cache-bit
 *
 * \todo replace with real function (better for optimisation)
 *
 * This shows that the cached value(s) is/are valid.
 */
#define CACHE_vSetCacheValidEeprom(ucDrawer)   UTI_SET( CACHE_aucDrawerCacheValidEeprom[ (ucDrawer)/8U ], \
                                                        1U << ((ucDrawer)%8U))
/**
 *\brief unset the specified data-cache-bit
 *
 * \todo replace with real function (better for optimisation)
 *
 * This shows that the cached value(s) is/are invalid
 */
#define CACHE_vSetCacheInvalidEeprom(ucDrawer) UTI_RES( CACHE_aucDrawerCacheValidEeprom[ (ucDrawer)/8U ], \
                                                        1U << ((ucDrawer)%8U))
#if defined(EXTERNAL_LARGE)
/**
 * \brief number of drawers of EEPROM partition table
 */
extern const tDrawerID CACHE_tNumberOfDrawersEEPROM;
#else
/**
 * \brief number of drawers of EEPROM partition table
 */
extern const uint8_t CACHE_ucNumberOfDrawersEEPROM;
#endif //defined(EXTERNAL_LARGE)
#endif //CACHE_INCLUDE_EEPROM

#ifdef CACHE_INCLUDE_VEEPROM
#if defined(VEEPROM_LARGE)
uint8_t  CACHE_ucGetDrawerSizeVEEPROM( tDrawerID tDrawer );
tDrawerID  CACHE_ucGetLabelVEEPROM(const uint8_t *pucCacheAddress );
uint8_t* CACHE_pucGetCacheAddressVEEPROM( tDrawerID tDrawer );
tDrawerID CACHE_tGetNumberOfDrawersVEEPROM( void );
#else //not defined(VEEPROM_LARGE) part
uint8_t  CACHE_ucGetDrawerSizeVEEPROM( uint8_t ucDrawer );
uint8_t  CACHE_ucGetLabelVEEPROM(const uint8_t *pucCacheAddress );
uint8_t* CACHE_pucGetCacheAddressVEEPROM( uint8_t ucDrawer );
uint8_t CACHE_ucGetNumberOfDrawersVEEPROM( void );
#endif //defined(VEEPROM_LARGE)
#endif //CACHE_INCLUDE_VEEPROM

#ifdef CACHE_INCLUDE_EEPROM
#if defined(EXTERNAL_LARGE)
uint8_t CACHE_ucGetDrawerSizeEEPROM( tDrawerID tDrawer );
uint8_t* CACHE_pucGetCacheAddressEEPROM( tDrawerID tDrawer );
tDrawerID  CACHE_tGetPartitionItemEEPROM( const uint8_t *pucCacheAddress );
tDrawerID CACHE_tGetNumberOfDrawersEEPROM( void );
uint32_t CACHE_ulGetVirtualAddressEEPROM( tDrawerID tDrawer ); ///< gets the virtual EEPROM address for certain drawer ID
#else //not defined(EXTERNAL_LARGE) part
uint8_t CACHE_ucGetDrawerSizeEEPROM( uint8_t ucDrawer );
uint8_t* CACHE_pucGetCacheAddressEEPROM( uint8_t ucDrawer );
uint8_t  CACHE_ucGetPartitionItemEEPROM( const uint8_t *pucCacheAddress );
uint8_t CACHE_ucGetNumberOfDrawersEEPROM( void );
uint32_t CACHE_ulGetVirtualAddressEEPROM( uint8_t ucDrawer ); ///< gets the virtual EEPROM address for certain drawer ID
#endif //defined(EXTERNAL_LARGE)
#endif //CACHE_INCLUDE_EEPROM

#ifdef __cplusplus
}
#endif

#endif //CACHE_H
