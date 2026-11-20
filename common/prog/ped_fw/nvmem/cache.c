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
 
/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file  cache.c
 *
 *  \brief  Module for caching data of Non-Volatile Memory Library
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "cachexs.h"
#include "cache.h"
#include "utility/utility.h"
/*lint -save -e717*/
#ifdef CACHE_INCLUDE_VEEPROM
#if defined(VEEPROM_LARGE)
/**
 * \brief get size of VEEPROM drawer
 * 
 * \param tDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: size of Drawer-Payload in byte
 */
uint8_t CACHE_ucGetDrawerSizeVEEPROM( tDrawerID tDrawer )
{
   uint8_t ucRetVal;
   tVEEPROMDrawerType tDrawerToCheck;
   if ( tDrawer <= CACHE_tGetNumberOfDrawersVEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableVeeprom[ tDrawer ];
      ucRetVal = tDrawerToCheck.ucSize;
   }
   else
   {
      ucRetVal = UINT8_MAX;
   }
   return ucRetVal;
}

/**
 * \brief get cache address of VEEPROM drawer
 * 
 * \param tDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: address of Drawer-Cache
 */
uint8_t* CACHE_pucGetCacheAddressVEEPROM( tDrawerID tDrawer )
{
   uint8_t* pucRetVal;
   tVEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf3 Outline of function
    * Return cache address of desired entry of partition table.
    */
   if ( tDrawer <= CACHE_tGetNumberOfDrawersVEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableVeeprom[ tDrawer ];
      pucRetVal = tDrawerToCheck.pucDataCacheLocation;
   }
   else
   {
      pucRetVal = NULL;
   }
   return pucRetVal;
}

/**
 * \brief Get Label of drawer with assigned pucCacheAddress
 *
 * \param pucCacheAddress Address where data is cached
 *
 * \return 
 *          - #CACHE_INVALID_DRAWER_IDENTIFIER:  index could not be found.
 *          - != #CACHE_INVALID_DRAWER_IDENTIFIER: index in #CACHE_tPartitionTableVeeprom, which is Label-Number
 */
tDrawerID  CACHE_ucGetLabelVEEPROM( const uint8_t *pucCacheAddress )
{
   tDrawerID tDrawerChecked;
   tDrawerID tNumberOfDrawers = CACHE_tGetNumberOfDrawersVEEPROM();
   tVEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf2 Outline of function
    *
    * For all existent drawer ...
    * ... check, if pucCacheAddress is the same as cacheAddress of CACHE_atPartitionTable that is regarded.
    * ... If equals, return label of drawer.
    *
   * If none found, return "0xFF". Drawers with this identifier does not exist. This is the invalid drawer.
    */
   for (tDrawerChecked = 0; tDrawerChecked < tNumberOfDrawers; tDrawerChecked++)
   {
      tDrawerToCheck = CACHE_tPartitionTableVeeprom[ tDrawerChecked ];
      if ( pucCacheAddress == tDrawerToCheck.pucDataCacheLocation )
      {
         return tDrawerChecked;
      }
   }
   return CACHE_INVALID_DRAWER_IDENTIFIER;
}

/**
 * \brief get number of drawers contained in VEEPROM partition table
 *
 *
 *
 * \return number of drawers defined in partition table
 */
tDrawerID CACHE_tGetNumberOfDrawersVEEPROM( void )
{
   return( CACHE_tNumberOfDrawersVEEPROM );
}
#else

/**
 * \brief get size of VEEPROM drawer
 * 
 * \param ucDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: size of Drawer-Payload in byte
 */
uint8_t CACHE_ucGetDrawerSizeVEEPROM( uint8_t ucDrawer )
{
   uint8_t ucRetVal;
   tVEEPROMDrawerType tDrawerToCheck;
   if ( ucDrawer <= CACHE_ucGetNumberOfDrawersVEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableVeeprom[ ucDrawer ];
      ucRetVal = tDrawerToCheck.ucSize;
   }
   else
   {
      ucRetVal = UINT8_MAX;
   }
   return ucRetVal;
}

/**
 * \brief get cache address of VEEPROM drawer
 * 
 * \param ucDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: address of Drawer-Cache
 */
uint8_t* CACHE_pucGetCacheAddressVEEPROM( uint8_t ucDrawer )
{
   uint8_t* pucRetVal;
   tVEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf3 Outline of function
    * Return cache address of desired entry of partition table.
    */
   if ( ucDrawer <= CACHE_ucGetNumberOfDrawersVEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableVeeprom[ ucDrawer ];
      pucRetVal = tDrawerToCheck.pucDataCacheLocation;
   }
   else
   {
      pucRetVal = NULL;
   }
   return pucRetVal;
}

/**
 * \brief Get Label of drawer with assigned pucCacheAddress
 *
 * \param pucCacheAddress Address where data is cached
 *
 * \return 
 *          - #CACHE_INVALID_DRAWER_IDENTIFIER:  index could not be found.
 *          - != #CACHE_INVALID_DRAWER_IDENTIFIER: index in #CACHE_tPartitionTableVeeprom, which is Label-Number
 */
uint8_t  CACHE_ucGetLabelVEEPROM( const uint8_t *pucCacheAddress )
{
   uint8_t ucDrawerChecked;
   uint8_t ucNumberOfDrawers = CACHE_ucGetNumberOfDrawersVEEPROM();
   tVEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf2 Outline of function
    *
    * For all existent drawer ...
    * ... check, if pucCacheAddress is the same as cacheAddress of CACHE_atPartitionTable that is regarded.
    * ... If equals, return label of drawer.
    *
   * If none found, return "0xFF". Drawers with this identifier does not exist. This is the invalid drawer.
    */
   for (ucDrawerChecked = 0; ucDrawerChecked < ucNumberOfDrawers; ucDrawerChecked++)
   {
      tDrawerToCheck = CACHE_tPartitionTableVeeprom[ ucDrawerChecked ];
      if ( pucCacheAddress == tDrawerToCheck.pucDataCacheLocation )
      {
         return ucDrawerChecked;
      }
   }
   return CACHE_INVALID_DRAWER_IDENTIFIER;
}

/**
 * \brief get number of drawers contained in VEEPROM partition table
 *
 *
 *
 * \return number of drawers defined in partition table
 */
uint8_t CACHE_ucGetNumberOfDrawersVEEPROM( void )
{
   return( CACHE_ucNumberOfDrawersVEEPROM );
}
#endif
#endif
#ifdef CACHE_INCLUDE_EEPROM
#if defined(EXTERNAL_LARGE)
/**
 * \brief get size of EEPROM drawer
 * 
 * \param ucDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: size of Drawer-Payload in byte
 */
uint8_t CACHE_ucGetDrawerSizeEEPROM( tDrawerID tDrawer )
{
   uint8_t ucRetVal;
   tEEPROMDrawerType tDrawerToCheck;
   if ( tDrawer <= CACHE_tGetNumberOfDrawersEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[tDrawer];
      ucRetVal = tDrawerToCheck.ucSize;
   }
   else
   {
      ucRetVal = UINT8_MAX;
   }
   return ucRetVal;
}

/**
 * \brief get cache address of EEPROM drawer
 * 
 * \param ucDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: address of Drawer-Cache
 */
uint8_t* CACHE_pucGetCacheAddressEEPROM( tDrawerID tDrawer )
{
   uint8_t* pucRetVal;
   tEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf3 Outline of function
    * Return cache address of desired entry of partition table.
    */
   if ( tDrawer <= CACHE_tGetNumberOfDrawersEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[tDrawer];
      pucRetVal = tDrawerToCheck.pucDataCacheLocation;
   }
   else
   {
      pucRetVal = NULL;
   }
   return pucRetVal;
}

/**
 * \brief Get Label of drawer with assigned pucCacheAddress
 *
 * \param pucCacheAddress Address where data is cached
 *
 * \return 
 *          - #CACHE_INVALID_DRAWER_IDENTIFIER:  index could not be found.
 *          - != #CACHE_INVALID_DRAWER_IDENTIFIER: index in #CACHE_tPartitionTableEeprom, which is Label-Number
 */
tDrawerID  CACHE_tGetPartitionItemEEPROM( const uint8_t *pucCacheAddress )
{
   tDrawerID tDrawerChecked;
   tDrawerID tNumberOfDrawers = CACHE_tGetNumberOfDrawersEEPROM();
   tEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf2 Outline of function
    *
    * For all existent drawer ...
    * ... check, if pucCacheAddress is the same as cacheAddress of CACHE_atPartitionTable that is regarded.
    * ... If equals, return label of drawer.
    *
   * If none found, return "0xFF". Drawers with this identifier does not exist. This is the invalid drawer.
    */
   for (tDrawerChecked = 0; tDrawerChecked < tNumberOfDrawers; tDrawerChecked++)
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[tDrawerChecked];
      if ( pucCacheAddress == tDrawerToCheck.pucDataCacheLocation )
      {
         return tDrawerChecked;
      }
   }
   return (tDrawerID)(CACHE_INVALID_DRAWER_IDENTIFIER);
}

/**
 * \brief get number of drawers contained in EEPROM partition table
 *
 *
 *
 * \return number of drawers defined in partition table
 */
tDrawerID CACHE_tGetNumberOfDrawersEEPROM( void )
{
   return( CACHE_tNumberOfDrawersEEPROM );
}
uint32_t CACHE_ulGetVirtualAddressEEPROM( tDrawerID tDrawer )
{
   uint32_t ulRetVal;
   tEEPROMDrawerType tDrawerToCheck;
   if ( tDrawer <= CACHE_tGetNumberOfDrawersEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[tDrawer];
      ulRetVal       = tDrawerToCheck.ulAddress;
      CACHE_vModifyEEPROMaddress((uint32_t *)(void *)&ulRetVal, tDrawer);
   }
   else
   {
      ulRetVal = UINT32_MAX;
   }
   return ulRetVal;
}
#else
/**
 * \brief get size of EEPROM drawer
 * 
 * \param ucDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: size of Drawer-Payload in byte
 */
uint8_t CACHE_ucGetDrawerSizeEEPROM( uint8_t ucDrawer )
{
   uint8_t ucRetVal;
   tEEPROMDrawerType tDrawerToCheck;
   if ( ucDrawer <= CACHE_ucGetNumberOfDrawersEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[ucDrawer];
      ucRetVal = tDrawerToCheck.ucSize;
   }
   else
   {
      ucRetVal = UINT8_MAX;
   }
   return ucRetVal;
}

/**
 * \brief get cache address of EEPROM drawer
 * 
 * \param ucDrawer number of drawer that size is requested
 *
 * \return 
 *        - 0: Drawer could not be found
 *        - !0: address of Drawer-Cache
 */
uint8_t* CACHE_pucGetCacheAddressEEPROM( uint8_t ucDrawer )
{
   uint8_t* pucRetVal;
   tEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf3 Outline of function
    * Return cache address of desired entry of partition table.
    */
   if ( ucDrawer <= CACHE_ucGetNumberOfDrawersEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[ ucDrawer ];
      pucRetVal = tDrawerToCheck.pucDataCacheLocation;
   }
   else
   {
      pucRetVal = NULL;
   }
   return pucRetVal;
}

/**
 * \brief Get Label of drawer with assigned pucCacheAddress
 *
 * \param pucCacheAddress Address where data is cached
 *
 * \return 
 *          - #CACHE_INVALID_DRAWER_IDENTIFIER:  index could not be found.
 *          - != #CACHE_INVALID_DRAWER_IDENTIFIER: index in #CACHE_tPartitionTableEeprom, which is Label-Number
 */
uint8_t  CACHE_ucGetPartitionItemEEPROM( const uint8_t *pucCacheAddress )
{
   uint8_t ucDrawerChecked;
   uint8_t ucNumberOfDrawers = CACHE_ucGetNumberOfDrawersEEPROM();
   tEEPROMDrawerType tDrawerToCheck;
   /**
    * \internal
    * \section secSf2 Outline of function
    *
    * For all existent drawer ...
    * ... check, if pucCacheAddress is the same as cacheAddress of CACHE_atPartitionTable that is regarded.
    * ... If equals, return label of drawer.
    *
   * If none found, return "0xFF". Drawers with this identifier does not exist. This is the invalid drawer.
    */
   for (ucDrawerChecked = 0; ucDrawerChecked < ucNumberOfDrawers; ucDrawerChecked++)
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[ ucDrawerChecked ];
      if ( pucCacheAddress == tDrawerToCheck.pucDataCacheLocation )
      {
         return ucDrawerChecked;
      }
   }
   return (uint8_t)(CACHE_INVALID_DRAWER_IDENTIFIER);
}

/**
 * \brief get number of drawers contained in EEPROM partition table
 *
 *
 *
 * \return number of drawers defined in partition table
 */
uint8_t CACHE_ucGetNumberOfDrawersEEPROM( void )
{
   return( CACHE_ucNumberOfDrawersEEPROM );
}
uint32_t CACHE_ulGetVirtualAddressEEPROM( uint8_t ucDrawer )
{
   uint32_t ulRetVal;
   tEEPROMDrawerType tDrawerToCheck;
   if ( ucDrawer <= CACHE_ucGetNumberOfDrawersEEPROM() )
   {
      tDrawerToCheck = CACHE_tPartitionTableEeprom[ucDrawer];
      ulRetVal       = tDrawerToCheck.ulAddress;
      CACHE_vModifyEEPROMaddress((uint32_t *)(void *)&ulRetVal, ucDrawer);
   }
   else
   {
      ulRetVal = UINT32_MAX;
   }
   return ulRetVal;
}
#endif
#endif
/*lint -restore -e717*/
