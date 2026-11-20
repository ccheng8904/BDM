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
 /**
 * \file NVMEM.c
 * \brief non volatile memory library
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "cacheXS.h"
#include "NVMEMXS.h"
#include "cache.h"
#include "cachepriv.h"
#include "nvmem.h"
#include "TestModuleExt.h"
#ifdef ASSERT_INCLUDED
#include "pedAssert.h"
#endif
#ifdef CACHE_INCLUDE_VEEPROM
#include "veeprom/VEEPROM.h"
#include "veeprom/VEEPROMXS.h"
#endif
#ifdef CACHE_INCLUDE_EEPROM
#include "exteeprom/ExtEEPlib.h"
#endif

/*lint -save -e954  Pointer variable could be declared as pointing to const */

#ifdef CACHE_INCLUDE_VEEPROM
static void    NVMEM_vRefreshRamCacheVEEPROM    ( void );
#endif
#ifdef CACHE_INCLUDE_EEPROM
static void    NVMEM_vRefreshRamCacheEEPROM     ( void );
#endif
#ifdef ASSERT_INCLUDED
static uint8_t NVMEM_ucLongestDrawer;
#endif
#if defined(RX100) || defined(RX200) || defined(CORTEX_M4) || (defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM))
static eNVMEMtaskStateType NVMEM_tTaskState = NVMEM_UNINITIALIZED; ///< holds task state of pending process

#ifdef CACHE_INCLUDE_VEEPROM
static bool                NVMEM_bFlushDefragEnabled = true; ///< enables/disables possible defragmentation process
#if ((defined(EXTERNAL_LARGE)) || ((defined(RX100) || defined(RX200) || defined(STM32G0) || defined(STM32G4) || defined(STM32L4) || defined(STM32F3) || defined(STM32F4) || defined(GD32F33) || defined(GD32F303)) && defined(VEEPROM_LARGE)))
static tDrawerID NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER; ///< cache place that parameters were given to VEEPROM library and write is pending

#else
static uint8_t NVMEM_ucPendingID = CACHE_INVALID_DRAWER_IDENTIFIER; ///< cache place that parameters were given to VEEPROM library and write is pending

#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
#if defined(EXTERNAL_LARGE)
static tDrawerID NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER; ///< cache place that write is pending

#else
static uint8_t NVMEM_ucPendingID = CACHE_INVALID_DRAWER_IDENTIFIER; ///< cache place that write is pending

#endif
#endif
#endif
#endif


#ifdef CACHE_INCLUDE_VEEPROM
#if defined(VEEPROM_LARGE)

/**
 * \brief refresh cached values for speed up reading out values
 */
static void    NVMEM_vRefreshRamCacheVEEPROM    ( void )
{
   tDrawerID  tPosition;
   // first clear all drawer-cache-valid bits - set all invalid.
   for (tPosition = 0U; tPosition < ( ( CACHE_tNumberOfDrawersVEEPROM + 7U ) / 8U ); tPosition++)
   {
      CACHE_aucDrawerCacheValidVeeprom[tPosition] = 0;
   }
   (void)VEEPROM_bReadWholeActivePage();
}
#else
/**
 * \brief refresh cached values for speed up reading out values
 */
static void    NVMEM_vRefreshRamCacheVEEPROM    ( void )
{
   uint8_t  ucPosition;

   // first clear all drawer-cache-valid bits - set all invalid.
   for (ucPosition = 0; ucPosition < (uint8_t)( ( CACHE_ucNumberOfDrawersVEEPROM + 7U ) / 8U ); ucPosition++)
   {
      CACHE_aucDrawerCacheValidVeeprom[ucPosition] = 0;
   }
   (void)VEEPROM_bReadWholeActivePage();
}
#endif
#endif
#ifdef CACHE_INCLUDE_EEPROM
#if defined(EXTERNAL_LARGE)
/**
 * \brief refresh cached values for speed up reading out values
 */
static void    NVMEM_vRefreshRamCacheEEPROM         ( void )
{
   tDrawerID tDrawerIdent;
   uint8_t     ucDrawerSize;
   tDrawerID tPosition;
   bool      bResult;
   uint32_t ulEepromAddress;
   uint8_t* pucCacheLoc;

   /* time measuring */
   //HDIO_vSetDirD5(HDIO_OUTPUT);
   //HDIO_vSetPortD5(true);
   // first clear all drawer-cache-valid bits - set all invalid.
   for (tPosition = 0U; tPosition < ( ( CACHE_tNumberOfDrawersEEPROM + 7U ) / 8U ); tPosition++)
   {
      CACHE_aucDrawerCacheValidEeprom[tPosition] = 0;
   }
   /**
    * \internal
    * \section secSf6 Outline of function
    * for every drawer ...
    */
   for (tDrawerIdent = 0; tDrawerIdent < CACHE_tNumberOfDrawersEEPROM; tDrawerIdent++)
   {
      /* time measuring */
      //HDIO_vSetPortD5(!HDIO_bGetPortD5());
      _NVMEM_vResetWatchdogTimer();
      /**
       * \internal
       * ... try to get payload from EEPROM
       */

      /**
       * \internal
       * ...... get DrawerSize
       */
      ucDrawerSize = CACHE_ucGetDrawerSizeEEPROM( tDrawerIdent );
      /**
       * \internal
       * ...... get address in external EEPROM
       */
      ulEepromAddress = CACHE_ulGetVirtualAddressEEPROM( tDrawerIdent );
      bResult = EXTEEP_bRead(ulEepromAddress,NVMEM_aucCacheBuffer, (uint16_t) ucDrawerSize);

      if ( bResult != false )
      {
         do
         {
            (void)EXTEEP_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }
         while (EXTEEP_tGetStatus() != EEP_READY);

         /**
          * \internal
          * ...... get Cache-Location
          */
         pucCacheLoc = CACHE_pucGetCacheAddressEEPROM( tDrawerIdent );

         /**
          * \internal
          * ...... copy buffer to cache-location that is assigned to desired label
          * ...... remember that cached value is valid
          */
         for (tPosition = 0;
              tPosition < (tDrawerID)ucDrawerSize;
              tPosition++)
         {
            if ( pucCacheLoc[tPosition] != NVMEM_aucCacheBuffer[tPosition] )
            {
               pucCacheLoc[tPosition] = NVMEM_aucCacheBuffer[tPosition];
            }
         }
         /**
          * \internal
          * after cache update mark cache value is valid
          * it does not matter if cache location had to be updated or not
          */
         CACHE_vSetCacheValidEeprom( tDrawerIdent );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__REFRESH_RAM_CACHE__CACHED_VALUE_FOUND);

      }
      else
      {
         ASSERT_vEXPR(false);
         /**
          * \internal
          * if data could not be read, 
          * remember that cached value is 
          * invalid.
          */

         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__REFRESH_RAM_CACHE__CACHED_VALUE_FOUND_NOT);
      }
   }
   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__REFRESH_RAM_CACHE_END);
   /* time measuring */
   //HDIO_vSetPortD5(false);
}
#else
/**
 * \brief refresh cached values for speed up reading out values
 */
static void    NVMEM_vRefreshRamCacheEEPROM         ( void )
{
   uint8_t  ucDrawerIdent;
   uint8_t  ucDrawerSize;
   uint8_t  ucPosition;
   bool   bResult;
   uint32_t ulEepromAddress;
   uint8_t* pucCacheLoc    ;

   /* time measuring */
   //HDIO_vSetDirD5(HDIO_OUTPUT);
   //HDIO_vSetPortD5(true);
   // first clear all drawer-cache-valid bits - set all invalid.
   for (ucPosition = 0U; ucPosition < ( ( CACHE_ucNumberOfDrawersEEPROM + 7U ) / 8U ); ucPosition++)
   {
      CACHE_aucDrawerCacheValidEeprom[ucPosition] = 0U;
   }
   /**
    * \internal
    * \section secSf6 Outline of function
    * for every drawer ...
    */
   for (ucDrawerIdent = 0; ucDrawerIdent < CACHE_ucNumberOfDrawersEEPROM; ucDrawerIdent++)
   {
      /* time measuring */
      //HDIO_vSetPortD5(!HDIO_bGetPortD5());
      _NVMEM_vResetWatchdogTimer();
      /**
       * \internal
       * ... try to get payload from EEPROM
       */

      /**
       * \internal
       * ...... get DrawerSize
       */
      ucDrawerSize = CACHE_ucGetDrawerSizeEEPROM( ucDrawerIdent );
      /**
       * \internal
       * ...... get address in external EEPROM
       */
      ulEepromAddress = CACHE_ulGetVirtualAddressEEPROM( ucDrawerIdent );
      bResult = EXTEEP_bRead(ulEepromAddress,NVMEM_aucCacheBuffer, (uint16_t) ucDrawerSize);

      if ( bResult )
      {
         do
         {
            (void)EXTEEP_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }
         while (EXTEEP_tGetStatus() != EEP_READY);

         /**
          * \internal
          * ...... get Cache-Location
          */
         pucCacheLoc = CACHE_pucGetCacheAddressEEPROM( ucDrawerIdent );

         /**
          * \internal
          * ...... copy buffer to cache-location that is assigned to desired label
          * ...... remember that cached value is valid
          */
         for (ucPosition = 0;
              ucPosition < ucDrawerSize;
              ucPosition++)
         {
            if ( pucCacheLoc[ucPosition] != NVMEM_aucCacheBuffer[ucPosition] )
            {
               pucCacheLoc[ucPosition] = NVMEM_aucCacheBuffer[ucPosition];
            }
         }
         /**
          * \internal
          * after cache update mark cache value is valid
          * it does not matter if cache location had to be updated or not
          */
         CACHE_vSetCacheValidEeprom( ucDrawerIdent );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__REFRESH_RAM_CACHE__CACHED_VALUE_FOUND);

      }
      else
      {
         ASSERT_vEXPR(false);
         /**
          * \internal
          * if data could not be read, 
          * remember that cached value is 
          * invalid.
          */

         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__REFRESH_RAM_CACHE__CACHED_VALUE_FOUND_NOT);
      }
   }
   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__REFRESH_RAM_CACHE_END);
   /* time measuring */
   //HDIO_vSetPortD5(false);
}
#endif
#endif


/* Global Functions *//* NVMEM_bInit() */bool NVMEM_bInit ( void )
{
#if defined (CACHE_INCLUDE_VEEPROM) || defined (CACHE_INCLUDE_EEPROM)
   bool bInitStatus = true;
#else
   bool bInitStatus = false;
#endif
#ifdef ASSERT_INCLUDED
   uint16 tCheckedDrawer;
#if defined(RX100) || defined(RX200) || defined(CORTEX_M4) || (defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM))
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
#if defined(VEEPROM_LARGE)

#if defined(EXTERNAL_LARGE)
   ASSERT_vEXPR( (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM) <= (NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8) );

#else
   ASSERT_vEXPR( (CACHE_tNumberOfDrawersVEEPROM + CACHE_ucNumberOfDrawersEEPROM) <= (NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8) );

#endif
#else
#if defined(EXTERNAL_LARGE)
   ASSERT_vEXPR( (CACHE_ucNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM) <= (NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8) );

#else
   ASSERT_vEXPR( ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM) <= (uint16_t)((uint16_t)NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8U) );

#endif
#endif
#else
#if defined(VEEPROM_LARGE)

   ASSERT_vEXPR( CACHE_tNumberOfDrawersVEEPROM <= (NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8) );

#else
   ASSERT_vEXPR( CACHE_ucNumberOfDrawersVEEPROM <= (NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8) );

#endif
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
#if defined(EXTERNAL_LARGE)
   ASSERT_vEXPR( CACHE_tNumberOfDrawersEEPROM <= (NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8) );

#else
   ASSERT_vEXPR( CACHE_ucNumberOfDrawersEEPROM <= (NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE * 8) );

#endif
#endif
#endif
#endif
#endif
   /**
    * call all lower initialization routines
    */
   //defined by default
#ifdef CACHE_INCLUDE_VEEPROM
   /* debug code */#ifdef ASSERT_INCLUDED
#if defined(VEEPROM_LARGE)

   for ( tCheckedDrawer = 0;
         tCheckedDrawer < CACHE_tNumberOfDrawersVEEPROM;
         tCheckedDrawer++)
   {
      if ( CACHE_tPartitionTableVeeprom[tCheckedDrawer].ucSize > NVMEM_ucLongestDrawer)
      {
         NVMEM_ucLongestDrawer = CACHE_tPartitionTableVeeprom[tCheckedDrawer].ucSize;
      }
   }
   //! \todo add check, if ucLongestDrawer fits in NVMEM_aucCacheBuffer 
   ASSERT_vEXPR( NVMEM_CACHE_BUFFER_SIZE >= NVMEM_ucLongestDrawer );
#else
   for ( tCheckedDrawer = 0;
         tCheckedDrawer < (uint16)CACHE_ucNumberOfDrawersVEEPROM;
         tCheckedDrawer++)
   {
      if ( CACHE_tPartitionTableVeeprom[tCheckedDrawer].ucSize > NVMEM_ucLongestDrawer)
      {
         NVMEM_ucLongestDrawer = CACHE_tPartitionTableVeeprom[tCheckedDrawer].ucSize;
      }
   }
   //! \todo add check, if ucLongestDrawer fits in NVMEM_aucCacheBuffer 
   ASSERT_vEXPR( NVMEM_CACHE_BUFFER_SIZE >= NVMEM_ucLongestDrawer );
#endif
#endif
   bInitStatus = (bool)((bool)bInitStatus && (bool)VEEPROM_bInit());
   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_PROC_INCLUDED_IN_INIT);
   NVMEM_vRefreshRamCacheVEEPROM();
#endif

#ifdef CACHE_INCLUDE_EEPROM
   bInitStatus = (bool)(bInitStatus && ((EXTEEP_ucHandleTask() != TASK_NOT_INITIALISED) ? true : false)); //initialisation of library
   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__EEPROM_PROC_INCLUDED_IN_INIT);
   NVMEM_vRefreshRamCacheEEPROM();
#endif
#if defined(RX100) || defined(RX200) || defined(CORTEX_M4) || (defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM))
   if ( bInitStatus )
   {
      NVMEM_tTaskState = NVMEM_READY;
   }
#endif
   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__INITEND);
   return bInitStatus;
}
/* NVMEM_bWrite() */#if defined(VEEPROM_LARGE)

#if defined(EXTERNAL_LARGE)

/* Virtual EEPROM large */
bool  NVMEM_bWrite( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData )
{
   uint8_t     ucDataLen;
   bool      bRetVal = false;
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   uint32_t     ulAddressEEPROM;
   tDrawerID tPartitionItem;

#else
   tDrawerID tPartitionItem;

#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   uint32_t     ulAddressEEPROM;
   tDrawerID tPartitionItem;

#endif
#endif
   uint8_t *pucCache;
   bool   bDifferentData = false;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( tPartitionItem );
         bRetVal = VEEPROM_bWrite( tPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(tPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      if ( (tDrawerID)tCachePlace < (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM) )
      {
         tPartitionItem = (tDrawerID)tCachePlace - CACHE_tNumberOfDrawersVEEPROM;

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(tPartitionItem) == false) )
         {
            // comment: cache has be updated already.
             CACHE_vSetCacheValidEeprom( tPartitionItem );

            ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(tPartitionItem);
            bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
            if ( bRetVal != false )
            {
               do
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }
               while (EXTEEP_tGetStatus() != EEP_READY);
               bRetVal = true;
            }
            else
            {
               ASSERT_vEXPR(false);
            }
         }
         else
         {
            // cached data is up to date
            bRetVal = true;

         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( tPartitionItem );
         bRetVal = VEEPROM_bWrite( tPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(tPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (tDrawerID)tCachePlace < CACHE_tNumberOfDrawersEEPROM )
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
      {
         // comment: cache has be updated already.
          CACHE_vSetCacheValidEeprom( tPartitionItem );

         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(tPartitionItem);
         bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
         if ( bRetVal != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            bRetVal = true;
         }
         else
         {
            ASSERT_vEXPR(false);
         }
      }
      else
      {
         // cached data is up to date
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#else
/* Virtual EEPROM large, external EEPROM normal version */
bool  NVMEM_bWrite( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData )
{
   uint8_t     ucDataLen;
   bool      bRetVal = false;
#ifdef CACHE_INCLUDE_VEEPROM
   tDrawerID tPartitionItem;

#endif
#ifdef CACHE_INCLUDE_EEPROM
   uint32_t ulAddressEEPROM;
   uint8_t ucPartitionItem;

#endif
   uint8_t *pucCache;
   bool   bDifferentData = false;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( tPartitionItem );
         bRetVal = VEEPROM_bWrite( tPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(tPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      if ( (tDrawerID)tCachePlace < ((tDrawerID)CACHE_tNumberOfDrawersVEEPROM + (tDrawerID)CACHE_ucNumberOfDrawersEEPROM) )
      {
         ucPartitionItem = (uint8_t)((tDrawerID)tCachePlace - (tDrawerID)CACHE_tNumberOfDrawersVEEPROM);

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
         {
            // comment: cache has be updated already.
            {//braces due to compiler problem for HC08
             CACHE_vSetCacheValidEeprom( ucPartitionItem );
            }

            ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);

            bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
            if ( bRetVal != false )
            {
               do
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }
               while (EXTEEP_tGetStatus() != EEP_READY);
               bRetVal = true;
            }
            else
            {
               ASSERT_vEXPR(false);
            }
         }
         else
         {
            // cached data is up to date
            bRetVal = true;

         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( tPartitionItem );
         bRetVal = VEEPROM_bWrite( tPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(tPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (uint8_t)tCachePlace < CACHE_ucNumberOfDrawersEEPROM )
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
      {
         // comment: cache has be updated already.
         {//braces due to compiler problem for HC08
          CACHE_vSetCacheValidEeprom( ucPartitionItem );
         }

         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);

         bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
         if ( bRetVal != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            bRetVal = true;
         }
         else
         {
            ASSERT_vEXPR(false);
         }
      }
      else
      {
         // cached data is up to date
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#endif
#else
#if defined(EXTERNAL_LARGE)

/* Virtual EEPROM normal, external EEPROM large version */
bool  NVMEM_bWrite( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData )
{
   uint8_t ucDataLen;
   bool  bRetVal = false;
#ifdef CACHE_INCLUDE_VEEPROM
   uint8_t ucPartitionItem;
#endif
#ifdef CACHE_INCLUDE_EEPROM
   uint32_t ulAddressEEPROM;
   tDrawerID tPartitionItem;

#endif
   uint8_t *pucCache;
   bool   bDifferentData = false;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( ucPartitionItem );
         bRetVal = VEEPROM_bWrite( ucPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(ucPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      if ( (tDrawerID)tCachePlace < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + (tDrawerID)CACHE_tNumberOfDrawersEEPROM) )
      {
         tPartitionItem = (tDrawerID)((tDrawerID)tCachePlace - (tDrawerID)CACHE_ucNumberOfDrawersVEEPROM);

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
         {
            // comment: cache has be updated already.
            {//braces due to compiler problem for HC08
             CACHE_vSetCacheValidEeprom( tPartitionItem );
            }

            ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(tPartitionItem);

            bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
            if ( bRetVal != false )
            {
               do
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }
               while (EXTEEP_tGetStatus() != EEP_READY);
               bRetVal = true;
            }
            else
            {
               ASSERT_vEXPR(false);
            }
         }
         else
         {
            // cached data is up to date
            bRetVal = true;

         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( ucPartitionItem );
         bRetVal = VEEPROM_bWrite( ucPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(ucPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (tDrawerID)tCachePlace < CACHE_tNumberOfDrawersEEPROM )
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(tPartitionItem) == false) )
      {
         // comment: cache has be updated already.
         {//braces due to compiler problem for HC08
          CACHE_vSetCacheValidEeprom( tPartitionItem );
         }

         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(tPartitionItem);

         bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
         if ( bRetVal != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            bRetVal = true;
         }
         else
         {
            ASSERT_vEXPR(false);
         }
      }
      else
      {
         // cached data is up to date
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#else
/* Normal version */
bool  NVMEM_bWrite( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData )
{
   uint8_t ucPartitionItem;
   uint8_t ucDataLen;
   bool  bRetVal = false;
#ifdef CACHE_INCLUDE_EEPROM
   uint32_t ulAddressEEPROM;
#endif
   uint8_t *pucCache;
   bool   bDifferentData = false;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( ucPartitionItem );
         bRetVal = VEEPROM_bWrite( ucPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(ucPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      if ( (uint16_t)tCachePlace < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM) )
      {
         ucPartitionItem = (uint8_t)((uint16_t)tCachePlace - (uint16_t)CACHE_ucNumberOfDrawersVEEPROM);

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
         {
            // comment: cache has be updated already.
            {//braces due to compiler problem for HC08
             CACHE_vSetCacheValidEeprom( ucPartitionItem );
            }

            ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);

            bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
            if ( bRetVal != false )
            {
               do
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }
               while (EXTEEP_tGetStatus() != EEP_READY);
               bRetVal = true;
            }
            else
            {
               ASSERT_vEXPR(false);
            }
         }
         else
         {
            // cached data is up to date
            bRetVal = true;

         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )

      {
         CACHE_vSetCacheValidVeeprom( ucPartitionItem );
         bRetVal = VEEPROM_bWrite( ucPartitionItem, pucData, ucDataLen);
         if (bRetVal == false)
         {
             CACHE_vSetCacheInvalidVeeprom(ucPartitionItem);
         }
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);
      }
      else
      {
         // cached data is up to date
         bRetVal = true;
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (uint8_t)tCachePlace < CACHE_ucNumberOfDrawersEEPROM )
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
      {
         // comment: cache has be updated already.
         {//braces due to compiler problem for HC08
          CACHE_vSetCacheValidEeprom( ucPartitionItem );
         }

         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);

         bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
         if ( bRetVal )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            bRetVal = true;
         }
         else
         {
            ASSERT_vEXPR(false);
         }
      }
      else
      {
         // cached data is up to date
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#endif
#endif
/* NVMEM_bRead() */
#if defined(VEEPROM_LARGE)

#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM large, external EEPROM large version */
bool  NVMEM_bRead( tCacheLocationIdentifier tCachePlace, uint8_t *pucData )
{
   uint8_t ucDataLen;
   bool  bRetVal = false;
   tDrawerID tPartitionItem;

   uint8_t *pucCache;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(tPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
             pucData[ ucIndex ] = pucCache[ ucIndex ];

         }
         bRetVal = true;

      }
   }
   else
   {
      if ( (tDrawerID)tCachePlace < (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM) )
      {
         tPartitionItem = ((tDrawerID)tCachePlace - CACHE_tNumberOfDrawersVEEPROM);

         if ( CACHE_bIsCacheValidEeprom(tPartitionItem) != false )
         {
            ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
            pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
            for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
            {
                pucData[ ucIndex ] = pucCache[ ucIndex ];

            }
            bRetVal = true;
         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }

#else
   /* only virtual EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(tPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (tDrawerID)tCachePlace < CACHE_tNumberOfDrawersEEPROM )
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      if ( CACHE_bIsCacheValidEeprom(tPartitionItem) != false )
      {
         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;
      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif

   return bRetVal;
}
#else
/* Virtual EEPROM large, external EEPROM normal version */
bool  NVMEM_bRead( tCacheLocationIdentifier tCachePlace, uint8_t *pucData )
{
   uint8_t ucDataLen;
   bool  bRetVal = false;
#ifdef CACHE_INCLUDE_VEEPROM
   tDrawerID tPartitionItem;

#endif
#ifdef CACHE_INCLUDE_EEPROM
   uint8_t ucPartitionItem;

#endif
   const uint8_t *pucCache;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(tPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
             pucData[ ucIndex ] = pucCache[ ucIndex ];

         }
         bRetVal = true;

      }
   }
   else
   {
      if ( (tDrawerID)tCachePlace < ((tDrawerID)CACHE_tNumberOfDrawersVEEPROM + (tDrawerID)CACHE_ucNumberOfDrawersEEPROM) )
      {
         ucPartitionItem = (uint8_t)((tDrawerID)tCachePlace - (tDrawerID)CACHE_tNumberOfDrawersVEEPROM);

         if ( CACHE_bIsCacheValidEeprom(ucPartitionItem) != false )
         {
            ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
            pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
            for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
            {
                pucData[ ucIndex ] = pucCache[ ucIndex ];

            }
            bRetVal = true;
         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }

#else
   /* only virtual EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(tPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (uint8_t)tCachePlace < CACHE_ucNumberOfDrawersEEPROM )
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      if ( CACHE_bIsCacheValidEeprom(ucPartitionItem) != false )
      {
         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;
      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif

   return bRetVal;
}
#endif
#else
#if defined(EXTERNAL_LARGE)
/* Normal version */
bool  NVMEM_bRead( tCacheLocationIdentifier tCachePlace, uint8_t *pucData )
{
   uint8_t ucDataLen;
   bool  bRetVal = false;
#ifdef CACHE_INCLUDE_VEEPROM
   uint8_t ucPartitionItem;

#endif
#ifdef CACHE_INCLUDE_EEPROM
   tDrawerID tPartitionItem;

#endif
   uint8_t *pucCache;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(ucPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
             pucData[ ucIndex ] = pucCache[ ucIndex ];

         }
         bRetVal = true;

      }
   }
   else
   {
      if ( (tDrawerID)tCachePlace < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + (tDrawerID)CACHE_tNumberOfDrawersEEPROM) )
      {
         tPartitionItem = (tDrawerID)((tDrawerID)tCachePlace - (tDrawerID)CACHE_ucNumberOfDrawersVEEPROM);

         if ( CACHE_bIsCacheValidEeprom(tPartitionItem) != false )
         {
            ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
            pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
            for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
            {
                pucData[ ucIndex ] = pucCache[ ucIndex ];

            }
            bRetVal = true;
         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }

#else
   /* only virtual EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(ucPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (tDrawerID)tCachePlace < CACHE_tNumberOfDrawersEEPROM )
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      if ( CACHE_bIsCacheValidEeprom(tPartitionItem) != false )
      {
         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;
      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif

   return bRetVal;
}
#else
/* Normal version */
bool  NVMEM_bRead( tCacheLocationIdentifier tCachePlace, uint8_t *pucData )
{
   uint8_t ucPartitionItem;
   uint8_t ucDataLen;
   bool  bRetVal = false;
   uint8_t *pucCache;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(ucPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
             pucData[ ucIndex ] = pucCache[ ucIndex ];

         }
         bRetVal = true;

      }
   }
   else
   {
      if ( (uint16_t)tCachePlace < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM) )
      {
         ucPartitionItem = (uint8_t)((uint16_t)tCachePlace - (uint16_t)CACHE_ucNumberOfDrawersVEEPROM);

         if ( CACHE_bIsCacheValidEeprom(ucPartitionItem) != false )
         {
            ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
            pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
            for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
            {
                pucData[ ucIndex ] = pucCache[ ucIndex ];

            }
            bRetVal = true;
         }
      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }

#else
   /* only virtual EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      if ( CACHE_bIsCacheValidVeeprom(ucPartitionItem) != 0u )
      {
         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;

      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (uint8_t)tCachePlace < CACHE_ucNumberOfDrawersEEPROM )
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      if ( CACHE_bIsCacheValidEeprom(ucPartitionItem) != false )
      {
         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            pucData[ ucIndex ] = pucCache[ ucIndex ];
         }
         bRetVal = true;
      }
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif

   return bRetVal;
}
#endif
#endif
/* VEEPROM_vAfterCacheVariableSet() */
#if defined(VEEPROM_LARGE)

/* Large version */
/**
Callback function is called by VEEPROM library when function VEEPROM_bReadWholeActivePage() sets valid value from flash page into cache variable.
*/
void VEEPROM_vAfterCacheVariableSet( tDrawerID tDrawer )
{
#ifdef CACHE_INCLUDE_VEEPROM
   CACHE_vSetCacheValidVeeprom( tDrawer );

#endif

}
#else
/* Normal version */
/**
Callback function is called by VEEPROM library when function VEEPROM_bReadWholeActivePage() sets valid value from flash page into cache variable.
*/
void VEEPROM_vAfterCacheVariableSet( uint8_t ucDrawerIdent)
{
#ifdef CACHE_INCLUDE_VEEPROM
   CACHE_vSetCacheValidVeeprom( ucDrawerIdent );

#else
   (void)ucDrawerIdent;
#endif

}
#endif
#if defined(RX100) || defined(RX200) || defined(CORTEX_M4) || (defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM))
/* NVMEM_bWriteStart() */
#if defined(VEEPROM_LARGE)
#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM large, external EEPROM large version */
bool  NVMEM_bWriteStart( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData )
{
   tDrawerID tPartitionItem;
   uint8_t     ucDataLen;
   bool      bRetVal = false;
   uint8_t     *pucCache;
   bool      bDifferentData = false;
   uint8_t     ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )

      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( tPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      if ( (uint16_t)tCachePlace < ((uint16_t)CACHE_tNumberOfDrawersVEEPROM + (uint16_t)CACHE_tNumberOfDrawersEEPROM) )
      {
         ucPartitionItem = (uint8_t)((uint16_t)tCachePlace - (uint16_t)CACHE_ucNumberOfDrawersVEEPROM);

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
         {
            // comment: cache has be updated already.
            {//braces due to compiler problem for HC08
             CACHE_vSetCacheValidEeprom( ucPartitionItem );
            }

            UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
            TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

         }
         else
         {
            // cached data is up to date

         }
         bRetVal = true;

      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )
      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( tPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( tCachePlace < CACHE_tNumberOfDrawersEEPROM )
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
      {
         // comment: cache has be updated already.
         {//braces due to compiler problem for HC08
          CACHE_vSetCacheValidEeprom( ucPartitionItem );
         }

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         //TM_SET_TESPOINT (NVMEM_TP__EEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;

   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#else
/* Virtual EEPROM large, external EEPROM short version */
bool  NVMEM_bWriteStart( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData )
{
   tDrawerID tPartitionItem;
   uint8_t     ucDataLen;
   bool      bRetVal = false;
   uint8_t     *pucCache;
   bool      bDifferentData = false;
   uint8_t     ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )

      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( tPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      if ( (uint16_t)tCachePlace < ((uint16_t)CACHE_tNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM) )
      {
         ucPartitionItem = (uint8_t)((uint16_t)tCachePlace - (uint16_t)CACHE_ucNumberOfDrawersVEEPROM);

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
         {
            // comment: cache has be updated already.
            {//braces due to compiler problem for HC08
             CACHE_vSetCacheValidEeprom( ucPartitionItem );
            }

            UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
            TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

         }
         else
         {
            // cached data is up to date

         }
         bRetVal = true;

      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (tDrawerID)tCachePlace < (tDrawerID)CACHE_tNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(tPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(tPartitionItem) == 0u) )
      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( tPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, true );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (uint8_t)tCachePlace < CACHE_ucNumberOfDrawersEEPROM )
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
      {
         // comment: cache has be updated already.
         {//braces due to compiler problem for HC08
          CACHE_vSetCacheValidEeprom( ucPartitionItem );
         }

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         //TM_SET_TESPOINT (NVMEM_TP__EEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;

   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#endif
#else
#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM short, external EEPROM large version */
bool  NVMEM_bWriteStart( tCacheLocationIdentifier tCachePlace, const uint8_t       *pucData )
{
   uint8_t ucDataLen;
   bool  bRetVal = false;
#ifdef CACHE_INCLUDE_VEEPROM
   uint8_t ucPartitionItem;

#endif
#ifdef CACHE_INCLUDE_EEPROM
   tDrawerID tPartitionItem;

#endif
   uint8_t *pucCache;
   bool   bDifferentData = false;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )

      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( ucPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      if ( (tDrawerID)tCachePlace < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + (tDrawerID)CACHE_tNumberOfDrawersEEPROM) )
      {
         tPartitionItem = (tDrawerID)((tDrawerID)tCachePlace - (tDrawerID)CACHE_ucNumberOfDrawersVEEPROM);

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(tPartitionItem) == false) )
         {
            // comment: cache has be updated already.
            {//braces due to compiler problem for HC08
             CACHE_vSetCacheValidEeprom( tPartitionItem );
            }

            UTI_vSetBit( NVMEM_aucDirtyFlags, (tDrawerID) tCachePlace, ON );
            TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

         }
         else
         {
            // cached data is up to date

         }
         bRetVal = true;

      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )
      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( ucPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (tDrawerID)tCachePlace < CACHE_tNumberOfDrawersEEPROM )
   {
      tPartitionItem = (tDrawerID)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(tPartitionItem) == false) )
      {
         // comment: cache has be updated already.
         {//braces due to compiler problem for HC08
          CACHE_vSetCacheValidEeprom( tPartitionItem );
         }

         UTI_vSetBit( NVMEM_aucDirtyFlags, (tDrawerID) tCachePlace, true );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         //TM_SET_TESPOINT (NVMEM_TP__EEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;

   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#else
/* Both short versions */
bool  NVMEM_bWriteStart( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData )
{
   uint8_t ucPartitionItem;
   uint8_t ucDataLen;
   bool  bRetVal = false;
   uint8_t *pucCache;
   bool   bDifferentData = false;
   uint8_t  ucIndex;

   ASSERT_vEXPR( pucData != NULL );



#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
   /* both virtual and native EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )

      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( ucPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      if ( (uint16_t)tCachePlace < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM) )
      {
         ucPartitionItem = (uint8_t)((uint16_t)tCachePlace - (uint16_t)CACHE_ucNumberOfDrawersVEEPROM);

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
         /* comment - check if new data is different from data cached. Also update cached values */
         /**
          * \internal
          * \subsection sub1_1 Check data
          *  if cached value is valid ...
          * ... get pointer to where data of label are cached
          * ... check, if new data to write is different from cached data.
          * ... if cached value is different, 
          *     - remember that we have different data and
          *     - update cached value
          * ... if data is not different, stored data is already up to date. Return.
          */

         for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
         {
            if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
            {
               // remember that cached value is different -> data to store is new
               bDifferentData = true;
               // and update cached value
               pucCache[ ucIndex ] = pucData[ ucIndex ];

            }
         }
         if ( (bDifferentData != false) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
         {
            // comment: cache has be updated already.
            {//braces due to compiler problem for HC08
             CACHE_vSetCacheValidEeprom( ucPartitionItem );
            }

            UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, ON );
            TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

#if 0
            ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);
            bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
            if ( bRetVal != false )
            {
               do
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }
               while (EXTEEP_tGetStatus() != EEP_READY);
               bRetVal = true;
            }
            else
            {
               ASSERT_vEXPR(false);
            }
#endif
         }
         else
         {
            // cached data is up to date

         }
         bRetVal = true;

      }
      else
      {
         /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
         ASSERT_vEXPR( false );
      }
   }
#else
   /* only virtual EEPROM defined */
   if ( (uint16_t)tCachePlace < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //tCachePlace is indexed from 0
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeVEEPROM(ucPartitionItem);

      ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
      pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData != false) || (CACHE_bIsCacheValidVeeprom(ucPartitionItem) == 0u) )
      {
         // comment: cache has be updated already.

         CACHE_vSetCacheValidVeeprom( ucPartitionItem );

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, true );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

      }
      else
      {
         // cached data is up to date
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;
   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
   /* only native EEPROM defined */
   if ( (uint8_t)tCachePlace < CACHE_ucNumberOfDrawersEEPROM )
   {
      ucPartitionItem = (uint8_t)tCachePlace;
      ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
      pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
      /* comment - check if new data is different from data cached. Also update cached values */
      /**
       * \internal
       * \subsection sub1_1 Check data
       *  if cached value is valid ...
       * ... get pointer to where data of label are cached
       * ... check, if new data to write is different from cached data.
       * ... if cached value is different, 
       *     - remember that we have different data and
       *     - update cached value
       * ... if data is not different, stored data is already up to date. Return.
       */

      for ( ucIndex = 0; ucIndex < ucDataLen; ucIndex++ )
      {
         if ( pucData[ ucIndex ] != pucCache[ ucIndex ] )
         {
            // remember that cached value is different -> data to store is new
            bDifferentData = true;
            // and update cached value
            pucCache[ ucIndex ] = pucData[ ucIndex ];

         }
      }
      if ( (bDifferentData) || (CACHE_bIsCacheValidEeprom(ucPartitionItem) == false) )
      {
         // comment: cache has be updated already.
         {//braces due to compiler problem for HC08
          CACHE_vSetCacheValidEeprom( ucPartitionItem );
         }

         UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t) tCachePlace, (bool)ON );
         TM_SET_TESPOINT ((uint16_t)NVMEM_TP__VEEPROM_WAS_WRITTEN);

#if 0
         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);
         bRetVal = EXTEEP_bWrite(ulAddressEEPROM, pucData, (uint16_t) ucDataLen);
         if ( bRetVal != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            bRetVal = true;
         }
         else
         {
            ASSERT_vEXPR(false);
         }
#endif
      }
      else
      {
         // cached data is up to date
         //TM_SET_TESPOINT (NVMEM_TP__EEPROM_WAS_UP_TO_DATE);

      }
      bRetVal = true;

   }
   else
   {
      /* this case can occur only if the type "tCacheLocationIdentifier" is wrong defined */
      ASSERT_vEXPR( false );
   }

#else
   /* EEPROM undefined */
#endif
#endif


   TM_SET_TESPOINT ((uint16_t)NVMEM_TP__WRITEEND);
   return bRetVal;
}
#endif
#endif
/* NVMEM_ucHandleTask() */
#if defined(VEEPROM_LARGE)
#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM large version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static tDrawerID NVMEM_tCounter;
   tDrawerID tPartitionItem;
   uint8_t ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_tCounter = 0; NVMEM_tCounter < (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM); NVMEM_tCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, NVMEM_tCounter) != 0 )
            {
               /* both virtual and native EEPROM defined */
               if ( NVMEM_tCounter < CACHE_tNumberOfDrawersVEEPROM )  //tCounter is indexed from 0
               {
                  if ( VEEPROM_tGetStatus() == VEEP_READY )
                  {
                     tPartitionItem = NVMEM_tCounter;
                     ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( tPartitionItem );

                     ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                     pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
                     if ( VEEPROM_bWriteStart(
                            tPartitionItem,
                            (const uint8_t *)pucCache,
                            ucDataLen         ) != false )
                     {
                        UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_tCounter, OFF );

                        NVMEM_tPendingID = tPartitionItem;
                        NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                        break; //from for
                     }
                     else
                     {
                        /* assert */
                        /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
               else
               {
                  if ( NVMEM_tCounter < (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM) )
                  {
                     if ( EXTEEP_tGetStatus() == EEP_READY )
                     {
                        tPartitionItem = (NVMEM_tCounter - CACHE_tNumberOfDrawersVEEPROM);

                        ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
                        pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
                        ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(tPartitionItem);
                        if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                        {
                           UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_tCounter, OFF );

                           NVMEM_tPendingID = tPartitionItem;
                           NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                           break; //from for
                        }
                        else
                        {
                           /* assert */
                           /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                           _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                        }
                     }
                     else
                     {
                        /* assert */
                        /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#else
/* only virtual EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static tDrawerID NVMEM_tCounter;
   uint8_t ucDataLen;

   uint8_t *pucCache;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_tCounter = 0; NVMEM_tCounter < CACHE_tNumberOfDrawersVEEPROM; NVMEM_tCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)NVMEM_tCounter) != 0 )
            {
               if ( VEEPROM_tGetStatus() == VEEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( NVMEM_tCounter );

                  ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                  pucCache = CACHE_pucGetCacheAddressVEEPROM( NVMEM_tCounter );
                  if ( VEEPROM_bWriteStart(
                         NVMEM_tCounter,
                         (const uint8_t *)pucCache,
                         ucDataLen         ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)NVMEM_tCounter, OFF );

                     NVMEM_tPendingID = NVMEM_tCounter;
                     NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */
                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }

            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;
            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static tDrawerID NVMEM_tCounter;
   uint8_t ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_tCounter = 0; NVMEM_tCounter < CACHE_tNumberOfDrawersEEPROM; NVMEM_tCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, NVMEM_tCounter) != 0 )
            {
               if ( EXTEEP_tGetStatus() == EEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeEEPROM( NVMEM_tCounter );
                  pucCache = CACHE_pucGetCacheAddressEEPROM( NVMEM_tCounter );
                  ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM( NVMEM_tCounter );
                  if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_tCounter, OFF );
                     NVMEM_tPendingID = NVMEM_tCounter;
                     NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */
                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }
            }
            else
            {
               /* no flag is set */
            }
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#endif

#else
/* Virtual EEPROM large version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static tDrawerID NVMEM_tCounter;
   tDrawerID tPartitionItem;
   uint8_t ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_tCounter = 0; NVMEM_tCounter < (CACHE_tNumberOfDrawersVEEPROM + (tDrawerID)CACHE_ucNumberOfDrawersEEPROM); NVMEM_tCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, NVMEM_tCounter) != 0 )
            {
               /* both virtual and native EEPROM defined */
               if ( NVMEM_tCounter < CACHE_tNumberOfDrawersVEEPROM )  //tCounter is indexed from 0
               {
                  if ( VEEPROM_tGetStatus() == VEEP_READY )
                  {
                     tPartitionItem = NVMEM_tCounter;
                     ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( tPartitionItem );

                     ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                     pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
                     if ( VEEPROM_bWriteStart(
                            tPartitionItem,
                            (const uint8_t *)pucCache,
                            ucDataLen         ) != false )
                     {
                        UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_tCounter, OFF );

                        NVMEM_tPendingID = tPartitionItem;
                        NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                        break; //from for
                     }
                     else
                     {
                        /* assert */
                        /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
               else
               {
                  if ( NVMEM_tCounter < (CACHE_tNumberOfDrawersVEEPROM + (tDrawerID)CACHE_ucNumberOfDrawersEEPROM) )
                  {
                     if ( EXTEEP_tGetStatus() == EEP_READY )
                     {
                        tPartitionItem = (NVMEM_tCounter - CACHE_tNumberOfDrawersVEEPROM);

                        ucDataLen = CACHE_ucGetDrawerSizeEEPROM( (uint8_t)tPartitionItem );
                        pucCache = CACHE_pucGetCacheAddressEEPROM( (uint8_t)tPartitionItem );
                        ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM((uint8_t)tPartitionItem);
                        if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                        {
                           UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_tCounter, OFF );

                           NVMEM_tPendingID = tPartitionItem;
                           NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                           break; //from for
                        }
                        else
                        {
                           /* assert */
                           /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                           _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                        }
                     }
                     else
                     {
                        /* assert */
                        /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#else
/* only virtual EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static tDrawerID NVMEM_tCounter;
   uint8_t ucDataLen;

   const uint8_t *pucCache;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_tCounter = 0; NVMEM_tCounter < CACHE_tNumberOfDrawersVEEPROM; NVMEM_tCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)NVMEM_tCounter) != false )
            {
               if ( VEEPROM_tGetStatus() == VEEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( NVMEM_tCounter );

                  ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                  pucCache = CACHE_pucGetCacheAddressVEEPROM( NVMEM_tCounter );
                  if ( VEEPROM_bWriteStart( NVMEM_tCounter, pucCache, ucDataLen ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)NVMEM_tCounter, false );
                     NVMEM_tPendingID = NVMEM_tCounter;
                     NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */
                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }

            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;
            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM

/* only native EEPROM defined */uint8_t NVMEM_ucHandleTask(void)
{
   static uint8_t NVMEM_ucCounter;
   uint8_t ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_ucCounter = 0; NVMEM_ucCounter < CACHE_ucNumberOfDrawersEEPROM; NVMEM_ucCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter) != 0 )
            {
               if ( EXTEEP_tGetStatus() == EEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeEEPROM( NVMEM_ucCounter );
                  pucCache = CACHE_pucGetCacheAddressEEPROM( NVMEM_ucCounter );
                  ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM( NVMEM_ucCounter );
                  if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter, OFF );
                     NVMEM_tPendingID = (tDrawerID)NVMEM_ucCounter;
                     NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }
            }
            else
            {
               /* no flag is set */
            }
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#endif

#endif
#else
#if defined(EXTERNAL_LARGE)
/* External EEPROM large version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static uint16_t NVMEM_uiCounter;
   uint8_t       ucPartitionItem;
   tDrawerID   tPartitionItem;
   uint8_t       ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_uiCounter = 0; NVMEM_uiCounter < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM); NVMEM_uiCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, NVMEM_uiCounter) != 0 )
            {
               /* both virtual and native EEPROM defined */
               if ( NVMEM_uiCounter < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //uiCounter is indexed from 0
               {
                  if ( VEEPROM_tGetStatus() == VEEP_READY )
                  {
                     ucPartitionItem = (uint8_t)NVMEM_uiCounter;
                     ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( ucPartitionItem );

                     ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                     pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
                     if ( VEEPROM_bWriteStart(
                            ucPartitionItem,
                            (const uint8_t *)pucCache,
                            ucDataLen         ) != false )
                     {
                        UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_uiCounter, OFF );
                        NVMEM_tPendingID = (tDrawerID)ucPartitionItem;
                        NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                        break; //from for
                     }
                     else
                     {
                        /* assert */
                        /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
               else
               {
                  if ( (tDrawerID)NVMEM_uiCounter < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + (tDrawerID)CACHE_tNumberOfDrawersEEPROM) )
                  {
                     if ( EXTEEP_tGetStatus() == EEP_READY )
                     {
                        tPartitionItem = (tDrawerID)((tDrawerID)NVMEM_uiCounter - (tDrawerID)CACHE_ucNumberOfDrawersVEEPROM);

                        ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
                        pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
                        ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(tPartitionItem);
                        if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                        {
                           UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_uiCounter, OFF );
                           NVMEM_tPendingID = tPartitionItem;
                           NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                           break; //from for
                        }
                        else
                        {
                           /* assert */
                           /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                           _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                        }
                     }
                     else
                     {
                        /* assert */
                        /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#else
/* only virtual EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static uint8_t NVMEM_ucCounter;
   uint8_t ucDataLen;

   uint8_t *pucCache;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_ucCounter = 0; NVMEM_ucCounter < CACHE_ucNumberOfDrawersVEEPROM; NVMEM_ucCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter) != 0 )
            {
               if ( VEEPROM_tGetStatus() == VEEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( NVMEM_ucCounter );

                  ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                  pucCache = CACHE_pucGetCacheAddressVEEPROM( NVMEM_ucCounter );
                  if ( VEEPROM_bWriteStart(
                         NVMEM_ucCounter  ,
                         (const uint8_t *)pucCache,
                         ucDataLen         ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter, OFF );
                     NVMEM_ucPendingID = NVMEM_ucCounter;
                     NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */
                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }

            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_ucPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static tDrawerID NVMEM_tCounter;
   uint8_t ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_tCounter = 0; NVMEM_tCounter < CACHE_tNumberOfDrawersEEPROM; NVMEM_tCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, NVMEM_tCounter) != false )
            {
               if ( EXTEEP_tGetStatus() == EEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeEEPROM( NVMEM_tCounter );
                  pucCache = CACHE_pucGetCacheAddressEEPROM( NVMEM_tCounter );
                  ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM( NVMEM_tCounter );
                  if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_tCounter, false );
                     NVMEM_tPendingID = NVMEM_tCounter;
                     NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */
                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }
            }
            else
            {
               /* no flag is set */
            }
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_tPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#endif
#else
/* Both short versions */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static uint16_t NVMEM_uiCounter;
   uint8_t ucPartitionItem;
   uint8_t ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_uiCounter = 0; NVMEM_uiCounter < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM); NVMEM_uiCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, NVMEM_uiCounter) != 0U )
            {
               /* both virtual and native EEPROM defined */
               if ( NVMEM_uiCounter < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //uiCounter is indexed from 0
               {
                  if ( VEEPROM_tGetStatus() == VEEP_READY )
                  {
                     ucPartitionItem = (uint8_t)NVMEM_uiCounter;
                     ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( ucPartitionItem );

                     ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                     pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
                     if ( VEEPROM_bWriteStart(
                            ucPartitionItem,
                            (const uint8_t *)(const void *)pucCache,
                            ucDataLen         ) != false )
                     {
                        UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_uiCounter, OFF );
                        NVMEM_ucPendingID = ucPartitionItem;
                        NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                        break; //from for
                     }
                     else
                     {
                        /* assert */
                        /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
               else
               {
                  if ( NVMEM_uiCounter < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM) )
                  {
                     if ( EXTEEP_tGetStatus() == EEP_READY )
                     {
                        ucPartitionItem = (uint8_t)(NVMEM_uiCounter - (uint16_t)CACHE_ucNumberOfDrawersVEEPROM);

                        ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
                        pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
                        ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);
                        if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                        {
                           UTI_vSetBit( NVMEM_aucDirtyFlags, NVMEM_uiCounter, OFF );
                           NVMEM_ucPendingID = ucPartitionItem;
                           NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                           break; //from for
                        }
                        else
                        {
                           /* assert */
                           /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                           _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                        }
                     }
                     else
                     {
                        /* assert */
                        /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                        _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     }
                  }
                  else
                  {
                     /* assert */
                     /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }

               }
            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_ucPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_ucPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#else
/* only virtual EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static uint8_t NVMEM_ucCounter;
   uint8_t ucDataLen;

   uint8_t *pucCache;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_ucCounter = 0; NVMEM_ucCounter < CACHE_ucNumberOfDrawersVEEPROM; NVMEM_ucCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter) != false )
            {
               if ( VEEPROM_tGetStatus() == VEEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( NVMEM_ucCounter );

                  ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
                  pucCache = CACHE_pucGetCacheAddressVEEPROM( NVMEM_ucCounter );
                  if ( VEEPROM_bWriteStart(
                         NVMEM_ucCounter  ,
                         (const uint8_t *)(void *)pucCache,
                         ucDataLen         ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter, false );
                     NVMEM_ucPendingID = NVMEM_ucCounter;
                     NVMEM_tTaskState = NVMEM_VEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */
                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }

            }
            else
            {
               /* no flag is set */
            }
         }

         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         if ( VEEPROM_tGetStatus() == VEEP_READY )
         {
            NVMEM_ucPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
uint8_t NVMEM_ucHandleTask(void)
{
   static uint8_t NVMEM_ucCounter;
   uint8_t ucDataLen;

   uint8_t *pucCache;
   uint32_t ulAddressEEPROM;

   switch (NVMEM_tTaskState)
   {
      case NVMEM_UNINITIALIZED:
      {
         if ( NVMEM_bInit() != false )
         {
            NVMEM_tTaskState = NVMEM_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case NVMEM_READY:
      {
         for (NVMEM_ucCounter = 0; NVMEM_ucCounter < CACHE_ucNumberOfDrawersEEPROM; NVMEM_ucCounter++)
         {
            if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter)  )
            {
               if ( EXTEEP_tGetStatus() == EEP_READY )
               {
                  ucDataLen = CACHE_ucGetDrawerSizeEEPROM( NVMEM_ucCounter );
                  pucCache = CACHE_pucGetCacheAddressEEPROM( NVMEM_ucCounter );
                  ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM( NVMEM_ucCounter );
                  if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)NVMEM_ucCounter, (bool)OFF );
                     NVMEM_ucPendingID = NVMEM_ucCounter;
                     NVMEM_tTaskState = NVMEM_EXTEEPROM_WRITING;
                     break; //from for
                  }
                  else
                  {
                     /* assert */
                     /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  }
               }
               else
               {
                  /* assert */
                  /* previous process not finished */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               }
            }
            else
            {
               /* no flag is set */
            }
         }
         break;
      }
      case NVMEM_EXTEEPROM_WRITING:
      {
         if ( EXTEEP_tGetStatus() == EEP_READY )
         {
            NVMEM_ucPendingID = CACHE_INVALID_DRAWER_IDENTIFIER;

            NVMEM_tTaskState = NVMEM_READY;
         }
         break;
      }
      case NVMEM_VEEPROM_WRITING:
      case NVMEM_DEFRAGMENTING  :
      {
         /* assert */
         /* impossible enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         break;
      }
      default:
      {
         /* assert */
         /* Unknown enumeration value */
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(TASK_INITIALISED);
}
#endif
#endif
#endif
#endif

/* NVMEM_bFlushBuffer() */
#if defined(VEEPROM_LARGE)
#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM large version, external EEPROM large version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   tDrawerID tCounter;
   bool bRetVal = true;
   tDrawerID tPartitionItem;
   uint8_t     ucDataLen;
   uint32_t     ulAddressEEPROM;
   uint8_t *pucCache;

   for (tCounter = 0; tCounter < (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM); tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != 0 )
      {
         if ( tCounter < CACHE_tNumberOfDrawersVEEPROM )  //uiCounter is indexed from 0
         {
            /* virtual EEPROM write */
            /* finish possible other process */
            while ( VEEPROM_tGetStatus() != VEEP_READY )
            {
               (void)VEEPROM_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }

            tPartitionItem = tCounter;
            ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( tPartitionItem );

            ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
            pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
            if ( NVMEM_bFlushDefragEnabled != false )
            {
               if ( VEEPROM_bWrite( tPartitionItem,
                            (const uint8_t *)pucCache,
                            ucDataLen               ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, tCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               if ( VEEPROM_uiGetFreeSpace() >= ( ucDataLen + VEEPROM_ucOffsetDrawerPayload ) )
               {
                  if ( VEEPROM_bWrite( tPartitionItem,
                               (const uint8_t *)pucCache,
                               ucDataLen               ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, tCounter, OFF );
                  }
                  else
                  {
                     /* assert */
                     /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     bRetVal = false;
                  }
               }
               else
               {
                  /* not enough space */
                  bRetVal = false;
               }
            }

         }
         else
         {
            if ( tCounter < (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM) )
            {
               /* native EEPROM write */
               /* finish possible other process */
               while ( EXTEEP_tGetStatus() != EEP_READY )
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }

               tPartitionItem = (tCounter - CACHE_tNumberOfDrawersVEEPROM);

               ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
               pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
               ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM( tPartitionItem );

               if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
               {
                  do
                  {
                     (void)EXTEEP_ucHandleTask();
                     _NVMEM_vResetWatchdogTimer();
                  }
                  while (EXTEEP_tGetStatus() != EEP_READY);
                  UTI_vSetBit( NVMEM_aucDirtyFlags, tCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }

            }
            else
            {
               /* assert */
               /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            }
         }
      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#else
/* only virtual EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   tDrawerID tCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;

   uint8_t *pucCache;

   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersVEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != 0 )
      {
         /* virtual EEPROM write */
         /* finish possible other process */
         while ( VEEPROM_tGetStatus() != VEEP_READY )
         {
            (void)VEEPROM_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( tCounter );

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( tCounter );
         if ( NVMEM_bFlushDefragEnabled != false )
         {
            if ( VEEPROM_bWrite( tCounter,
                         (const uint8_t *)pucCache,
                         ucDataLen               ) != false )
            {
               UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)tCounter, OFF );
            }
            else
            {
               /* assert */
               /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               bRetVal = false;
            }
         }
         else
         {
            if ( VEEPROM_uiGetFreeSpace() >= ( ucDataLen + VEEPROM_ucOffsetDrawerPayload ) )
            {
               if ( VEEPROM_bWrite( tCounter,
                            (const uint8_t *)pucCache,
                            ucDataLen               ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)tCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               /* not enough space */
               bRetVal = false;
            }
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   tDrawerID tCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;
   uint32_t ulAddressEEPROM;
   uint8_t *pucCache;

   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != 0 )
      {
         /* native EEPROM write */
         /* finish possible other process */
         while ( EXTEEP_tGetStatus() != EEP_READY )
         {
            (void)EXTEEP_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tCounter );
         pucCache = CACHE_pucGetCacheAddressEEPROM( tCounter );
         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM( tCounter );

         if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)tCounter, OFF );
         }
         else
         {
            /* assert */
            /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            bRetVal = false;
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#endif

#else
/* Virtual EEPROM large version,external EEPROM short version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   tDrawerID tCounter;
   bool bRetVal = true;
   tDrawerID tPartitionItem;
   uint8_t     ucDataLen;
   uint32_t     ulAddressEEPROM;
   uint8_t *pucCache;

   for (tCounter = 0; tCounter < (CACHE_tNumberOfDrawersVEEPROM + (tDrawerID)CACHE_ucNumberOfDrawersEEPROM); tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != 0 )
      {
         if ( tCounter < CACHE_tNumberOfDrawersVEEPROM )  //uiCounter is indexed from 0
         {
            /* virtual EEPROM write */
            /* finish possible other process */
            while ( VEEPROM_tGetStatus() != VEEP_READY )
            {
               (void)VEEPROM_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }

            tPartitionItem = tCounter;
            ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( tPartitionItem );

            ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
            pucCache = CACHE_pucGetCacheAddressVEEPROM( tPartitionItem );
            if ( NVMEM_bFlushDefragEnabled != false )
            {
               if ( VEEPROM_bWrite( tPartitionItem,
                            (const uint8_t *)pucCache,
                            ucDataLen               ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, tCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               if ( VEEPROM_uiGetFreeSpace() >= ( ucDataLen + VEEPROM_ucOffsetDrawerPayload ) )
               {
                  if ( VEEPROM_bWrite( tPartitionItem,
                               (const uint8_t *)pucCache,
                               ucDataLen               ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, tCounter, OFF );
                  }
                  else
                  {
                     /* assert */
                     /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     bRetVal = false;
                  }
               }
               else
               {
                  /* not enough space */
                  bRetVal = false;
               }
            }

         }
         else
         {
            if ( tCounter < (CACHE_tNumberOfDrawersVEEPROM + (tDrawerID)CACHE_ucNumberOfDrawersEEPROM) )
            {
               /* native EEPROM write */
               /* finish possible other process */
               while ( EXTEEP_tGetStatus() != EEP_READY )
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }

               tPartitionItem = (tCounter - CACHE_tNumberOfDrawersVEEPROM);

               ucDataLen = CACHE_ucGetDrawerSizeEEPROM( (uint8_t)tPartitionItem );
               pucCache = CACHE_pucGetCacheAddressEEPROM( (uint8_t)tPartitionItem );
               ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM((uint8_t)tPartitionItem);

               if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
               {
                  do
                  {
                     (void)EXTEEP_ucHandleTask();
                     _NVMEM_vResetWatchdogTimer();
                  }
                  while (EXTEEP_tGetStatus() != EEP_READY);
                  UTI_vSetBit( NVMEM_aucDirtyFlags, tCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }

            }
            else
            {
               /* assert */
               /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            }
         }
      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#else
/* only virtual EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   tDrawerID tCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;

   const uint8_t *pucCache;

   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersVEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != false )
      {
         /* virtual EEPROM write */
         /* finish possible other process */
         while ( VEEPROM_tGetStatus() != VEEP_READY )
         {
            (void)VEEPROM_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( tCounter );

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( tCounter );
         if ( NVMEM_bFlushDefragEnabled != false )
         {
            if ( VEEPROM_bWrite( tCounter, pucCache, ucDataLen ) != false )
            {
               UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)tCounter, false );
            }
            else
            {
               /* assert */
               /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               bRetVal = false;
            }
         }
         else
         {
            if ( VEEPROM_uiGetFreeSpace() >= ( (uint16_t)ucDataLen + (uint16_t)VEEPROM_ucOffsetDrawerPayload ) )
            {
               if ( VEEPROM_bWrite( tCounter, pucCache, ucDataLen ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)tCounter, false );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               /* not enough space */
               bRetVal = false;
            }
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   uint8_t ucCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;
   uint32_t ulAddressEEPROM;
   uint8_t *pucCache;

   for (ucCounter = 0; ucCounter < CACHE_ucNumberOfDrawersEEPROM; ucCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)ucCounter) != 0 )
      {
         /* native EEPROM write */
         /* finish possible other process */
         while ( EXTEEP_tGetStatus() != EEP_READY )
         {
            (void)EXTEEP_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucCounter );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucCounter );
         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucCounter);

         if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)ucCounter, OFF );
         }
         else
         {
            /* assert */
            /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            bRetVal = false;
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#endif

#endif
#else
#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM short version, external EEPROM large version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   uint16_t      uiCounter;
   bool      bRetVal = true;
   uint8_t     ucPartitionItem;
   tDrawerID tPartitionItem;
   uint8_t     ucDataLen;
   uint32_t     ulAddressEEPROM;
   uint8_t *pucCache;

   for (uiCounter = 0; uiCounter < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM); uiCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, uiCounter) != 0 )
      {
         if ( uiCounter < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //uiCounter is indexed from 0
         {
            /* virtual EEPROM write */
            /* finish possible other process */
            while ( VEEPROM_tGetStatus() != VEEP_READY )
            {
               (void)VEEPROM_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }

            ucPartitionItem = (uint8_t)uiCounter;
            ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( ucPartitionItem );

            ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
            pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
            if ( NVMEM_bFlushDefragEnabled != false )
            {
               if ( VEEPROM_bWrite( ucPartitionItem,
                            (const uint8_t *)pucCache,
                            ucDataLen               ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, uiCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               if ( VEEPROM_uiGetFreeSpace() >= ( ucDataLen + VEEPROM_ucOffsetDrawerPayload ) )
               {
                  if ( VEEPROM_bWrite( ucPartitionItem,
                               (const uint8_t *)pucCache,
                               ucDataLen               ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, uiCounter, OFF );
                  }
                  else
                  {
                     /* assert */
                     /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     bRetVal = false;
                  }
               }
               else
               {
                  /* not enough space */
                  bRetVal = false;
               }
            }

         }
         else
         {
            if ( uiCounter < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM) )
            {
               /* native EEPROM write */
               /* finish possible other process */
               while ( EXTEEP_tGetStatus() != EEP_READY )
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }

               tPartitionItem = (tDrawerID)(uiCounter - CACHE_tNumberOfDrawersVEEPROM);

               ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tPartitionItem );
               pucCache = CACHE_pucGetCacheAddressEEPROM( tPartitionItem );
               ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(tPartitionItem);

               if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
               {
                  do
                  {
                     (void)EXTEEP_ucHandleTask();
                     _NVMEM_vResetWatchdogTimer();
                  }
                  while (EXTEEP_tGetStatus() != EEP_READY);
                  UTI_vSetBit( NVMEM_aucDirtyFlags, uiCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }

            }
            else
            {
               /* assert */
               /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            }
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#else
/* only virtual EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   uint8_t ucCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;

   uint8_t *pucCache;

   for (ucCounter = 0; ucCounter < CACHE_ucNumberOfDrawersVEEPROM; ucCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)ucCounter) != 0 )
      {
         /* virtual EEPROM write */
         /* finish possible other process */
         while ( VEEPROM_tGetStatus() != VEEP_READY )
         {
            (void)VEEPROM_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( ucCounter );

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( ucCounter );
         if ( NVMEM_bFlushDefragEnabled != false )
         {
            if ( VEEPROM_bWrite( ucCounter,
                         (const uint8_t *)pucCache,
                         ucDataLen               ) != false )
            {
               UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)ucCounter, OFF );
            }
            else
            {
               /* assert */
               /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               bRetVal = false;
            }
         }
         else
         {
            if ( VEEPROM_uiGetFreeSpace() >= ( ucDataLen + VEEPROM_ucOffsetDrawerPayload ) )
            {
               if ( VEEPROM_bWrite( ucCounter,
                            (const uint8_t *)pucCache,
                            ucDataLen               ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)ucCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               /* not enough space */
               bRetVal = false;
            }
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   tDrawerID tCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;
   uint32_t ulAddressEEPROM;
   uint8_t *pucCache;

   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != false )
      {
         /* native EEPROM write */
         /* finish possible other process */
         while ( EXTEEP_tGetStatus() != EEP_READY )
         {
            (void)EXTEEP_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( tCounter );
         pucCache = CACHE_pucGetCacheAddressEEPROM( tCounter );
         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM( tCounter );

         if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)tCounter, false );
         }
         else
         {
            /* assert */
            /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            bRetVal = false;
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#endif
#else
/* Both short versions */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   uint16_t uiCounter;
   bool bRetVal = true;
   uint8_t ucPartitionItem;
   uint8_t ucDataLen;
   uint32_t ulAddressEEPROM;
   uint8_t *pucCache;

   for (uiCounter = 0; uiCounter < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM); uiCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, uiCounter) != 0U )
      {
         if ( uiCounter < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM )  //uiCounter is indexed from 0
         {
            /* virtual EEPROM write */
            /* finish possible other process */
            while ( VEEPROM_tGetStatus() != VEEP_READY )
            {
               (void)VEEPROM_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }

            ucPartitionItem = (uint8_t)uiCounter;
            ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( ucPartitionItem );

            ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
            pucCache = CACHE_pucGetCacheAddressVEEPROM( ucPartitionItem );
            if ( NVMEM_bFlushDefragEnabled != false )
            {
               if ( VEEPROM_bWrite( ucPartitionItem,
                            (const uint8_t *)(const void *)pucCache,
                            ucDataLen               ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, uiCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               if ( VEEPROM_uiGetFreeSpace() >= ( (uint16_t)ucDataLen + (uint16_t)VEEPROM_ucOffsetDrawerPayload ) )
               {
                  if ( VEEPROM_bWrite( ucPartitionItem,
                               (const uint8_t *)(const void *)pucCache,
                               ucDataLen               ) != false )
                  {
                     UTI_vSetBit( NVMEM_aucDirtyFlags, uiCounter, OFF );
                  }
                  else
                  {
                     /* assert */
                     /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                     _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                     bRetVal = false;
                  }
               }
               else
               {
                  /* not enough space */
                  bRetVal = false;
               }
            }


         }
         else
         {
            if ( uiCounter < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM) )
            {
               /* native EEPROM write */
               /* finish possible other process */
               while ( EXTEEP_tGetStatus() != EEP_READY )
               {
                  (void)EXTEEP_ucHandleTask();
                  _NVMEM_vResetWatchdogTimer();
               }

               ucPartitionItem = (uint8_t)(uiCounter - (uint16_t)CACHE_ucNumberOfDrawersVEEPROM);

               ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucPartitionItem );
               pucCache = CACHE_pucGetCacheAddressEEPROM( ucPartitionItem );
               ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucPartitionItem);

               if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
               {
                  do
                  {
                     (void)EXTEEP_ucHandleTask();
                     _NVMEM_vResetWatchdogTimer();
                  }
                  while (EXTEEP_tGetStatus() != EEP_READY);
                  UTI_vSetBit( NVMEM_aucDirtyFlags, uiCounter, OFF );
               }
               else
               {
                  /* assert */
                  /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }

            }
            else
            {
               /* assert */
               /* cache place counter out of range */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            }
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#else
/* only virtual EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   uint8_t ucCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;

   uint8_t *pucCache;

   for (ucCounter = 0; ucCounter < CACHE_ucNumberOfDrawersVEEPROM; ucCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)ucCounter) != false )
      {
         /* virtual EEPROM write */
         /* finish possible other process */
         while ( VEEPROM_tGetStatus() != VEEP_READY )
         {
            (void)VEEPROM_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeVEEPROM( ucCounter );

         ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
         pucCache = CACHE_pucGetCacheAddressVEEPROM( ucCounter );
         if ( NVMEM_bFlushDefragEnabled != false )
         {
            if ( VEEPROM_bWrite( ucCounter,
                         (const uint8_t *)(void *)pucCache,
                         ucDataLen               ) != false )
            {
               UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)ucCounter, false );
            }
            else
            {
               /* assert */
               /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
               bRetVal = false;
            }
         }
         else
         {
            if ( VEEPROM_uiGetFreeSpace() >= ( (uint16_t)ucDataLen + (uint16_t)VEEPROM_ucOffsetDrawerPayload ) )
            {
               if ( VEEPROM_bWrite( ucCounter,
                            (const uint8_t *)(void *)pucCache,
                            ucDataLen               ) != false )
               {
                  UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)ucCounter, false );
               }
               else
               {
                  /* assert */
                  /* write not successful */
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
                  bRetVal = false;
               }
            }
            else
            {
               /* not enough space */
               bRetVal = false;
            }
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
bool NVMEM_bFlushBuffer(void)
{
   uint8_t ucCounter;
   bool bRetVal = true;
   uint8_t ucDataLen;
   uint32_t ulAddressEEPROM;
   uint8_t *pucCache;

   for (ucCounter = 0; ucCounter < CACHE_ucNumberOfDrawersEEPROM; ucCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)ucCounter) )
      {
         /* native EEPROM write */
         /* finish possible other process */
         while ( EXTEEP_tGetStatus() != EEP_READY )
         {
            (void)EXTEEP_ucHandleTask();
            _NVMEM_vResetWatchdogTimer();
         }

         ucDataLen = CACHE_ucGetDrawerSizeEEPROM( ucCounter );
         pucCache = CACHE_pucGetCacheAddressEEPROM( ucCounter );
         ulAddressEEPROM = CACHE_ulGetVirtualAddressEEPROM(ucCounter);

         if ( EXTEEP_bWrite(ulAddressEEPROM, pucCache, (uint16_t) ucDataLen) != false )
         {
            do
            {
               (void)EXTEEP_ucHandleTask();
               _NVMEM_vResetWatchdogTimer();
            }
            while (EXTEEP_tGetStatus() != EEP_READY);
            UTI_vSetBit( NVMEM_aucDirtyFlags, (uint16_t)ucCounter, (bool)OFF );
         }
         else
         {
            /* assert */
            /* write not started */
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(const void *)(__FILE__), __LINE__);
#endif
            bRetVal = false;
         }

      }
      else
      {
         /* no flag is set */
      }
   }
   return(bRetVal);
}
#endif
#endif
#endif
#endif

/* NVMEM_uiGetUnsavedCount() */
#if defined(VEEPROM_LARGE)
#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM large, external EEPROM large version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   tDrawerID tCounter;
   uint16_t uiUnsaved = 0;
   for (tCounter = 0; tCounter < (CACHE_tNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM); tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != 0 )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#else
/* only virtual EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   tDrawerID tCounter;
   uint16_t uiUnsaved = 0;
   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersVEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, tCounter) != 0 )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   tDrawerID tCounter;
   uint16_t uiUnsaved = 0;
   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, tCounter) != 0 )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#endif

#else
/* Virtual EEPROM large, external EEPROM short version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   tDrawerID tCounter;
   uint16_t uiUnsaved = 0;
   for (tCounter = 0; tCounter < (CACHE_tNumberOfDrawersVEEPROM + (tDrawerID)CACHE_ucNumberOfDrawersEEPROM); tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16_t)tCounter) != 0 )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#else
/* only virtual EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   tDrawerID tCounter;
   uint16_t uiUnsaved = 0;
   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersVEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, tCounter) != false )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   uint16_t uiCounter;
   uint16_t uiUnsaved = 0;
   for (uiCounter = 0; uiCounter < (uint16_t)CACHE_ucNumberOfDrawersEEPROM; uiCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, uiCounter) != 0 )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#endif

#endif
#else
#if defined(EXTERNAL_LARGE)
/* Virtual EEPROM short, external EEPROM large version */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   tDrawerID tCounter;
   uint16_t uiUnsaved = 0;
   for (tCounter = 0; tCounter < ((tDrawerID)CACHE_ucNumberOfDrawersVEEPROM + CACHE_tNumberOfDrawersEEPROM); tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, tCounter) != 0 )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#else
/* only virtual EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   uint16_t uiCounter;
   uint16_t uiUnsaved = 0;
   for (uiCounter = 0; uiCounter < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM; uiCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, uiCounter) != 0 )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_ucPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   tDrawerID tCounter;
   uint16_t uiUnsaved = 0;
   for (tCounter = 0; tCounter < CACHE_tNumberOfDrawersEEPROM; tCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, tCounter) != false )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_tPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#endif
#else
/* Both short versions */
#ifdef CACHE_INCLUDE_VEEPROM
#ifdef CACHE_INCLUDE_EEPROM
/* both virtual and native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   uint16_t uiCounter;
   uint16_t uiUnsaved = 0;
   for (uiCounter = 0; uiCounter < ((uint16_t)CACHE_ucNumberOfDrawersVEEPROM + (uint16_t)CACHE_ucNumberOfDrawersEEPROM); uiCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, uiCounter) != 0U )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_ucPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#else
/* only virtual EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   uint16_t uiCounter;
   uint16_t uiUnsaved = 0;
   for (uiCounter = 0; uiCounter < (uint16_t)CACHE_ucNumberOfDrawersVEEPROM; uiCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, uiCounter) != false )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_ucPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#else
#ifdef CACHE_INCLUDE_EEPROM
/* only native EEPROM defined */
uint16_t NVMEM_uiGetUnsavedCount(void)
{
   uint16_t uiCounter;
   uint16_t uiUnsaved = 0;
   for (uiCounter = 0; uiCounter < (uint16_t)CACHE_ucNumberOfDrawersEEPROM; uiCounter++)
   {
      if ( UTI_bGetBit(NVMEM_aucDirtyFlags, uiCounter) )
      {
         uiUnsaved++;
      }
      else
      {
         /* no flag is set */
      }
   }

   if (NVMEM_ucPendingID != CACHE_INVALID_DRAWER_IDENTIFIER)
   {
      uiUnsaved++;
   }
   return(uiUnsaved);
}
#endif
#endif
#endif
#endif

/* NVMEM_tGetStatus() */
eNVMEMtaskStateType NVMEM_tGetStatus(void)
{
   return(NVMEM_tTaskState);
}
/* NVMEM_bIsSavePending() */
bool NVMEM_bIsSavePending( tCacheLocationIdentifier tCachePlace )
{
   return(UTI_bGetBit(NVMEM_aucDirtyFlags, (uint16)tCachePlace));
}
#ifdef CACHE_INCLUDE_VEEPROM
/* NVMEM_vFlushDefragEnabled() */
void NVMEM_vFlushDefragEnabled( bool bEnabled )
{
   NVMEM_bFlushDefragEnabled = bEnabled;

}
#endif
#endif
/*lint -restore -e954 */

