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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Implementation of Volatile EEPROM library for Stm32f0xx.
*
*   \details  This module contain routines for maintenance of data packs on
*             at least two flash pages. Only one data page contains valid data
*             another pages should be empty. At the write time a new data pack is saved
*             to new free place on the page. If page is full, last current data packs are
*             transfered to another page and previous full one is erased.
*
*/
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"

#include "LibTypes.h"
#include "VEEPROMpriv.h"
#include "VEEPROM.h"
#include "VEEPROMXS.h"
#if defined(DEBUG) && (DEBUG != 0u)
#include "ped_fw\veeprom\pedAssert.h"
#endif
#include "flash/flash.h"
#include "utility.h"
#include "crc.h"
#include "VEEPROMmcXS.h"
#include "nvmem/cache.h"
#include "hsup.h"
//lint -e923 cast from pointer to unsigned int
/*lint -save -e717*/
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
//#define TIME_MEASUREMENT
#ifdef TIME_MEASUREMENT
#include "timerlib.h"
#endif
/**
 * \brief content of unwritten flash-cells
 *
 * unwritten (or erased) flash-cells contain the byte 0xFF
 */
#define VEEPROM_UNWRITTEN_FLASH_CELL     (uint8_t) 0xFF
#if defined(VEEPROM_LARGE)
#define VEEPROM_DEFAULT_UNIT 0 //!< default unit that is assigned when all units are still free (initial start) 
#else
#define VEEPROM_DEFAULT_PAGE 0 //!< default page that is assigned when all pages are still free (initial start) 
#endif
/**
 * length of page transfer counter in bytes
 *
 * these are the first n bytes of page that do not have any header.
 */
#define VEEPROM_PAGE_TRANSFER_COUNTER_LEN 2U
/*  when using this library without NVMEM-library,*/
/*             _     _   _                         _ _       _               */
/*    ___  ___| |_  | |_| |__   ___   _____      _(_) |_ ___| |__  _         */
/*    __|/ _ \ __| | __| '_ \ / _ \ / __\ \ /\ / / | __/ __| '_ \(_)         */
/*   \__ \  __/ |_  | |_| | | |  __/ \__ \\ V  V /| | || (__| | | |_         */
/*   |___/\___|\__|  \__|_| |_|\___| |___/ \_/\_/ |_|\__\___|_| |_(_)        */
/*   */
/* VEEPROM_USE_LIB_WITHOUT_NVMEM*/
/*   */
/*   - to disable using cached values for process of defragmentation and*/
/*   - to enable check if data to write is different from actually data stored.*/
#ifdef VEEPROM_USE_LIB_WITHOUT_NVMEM
#define VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
#define VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
#endif
/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/
/**
 * possible returned values of functions VEEPROM_tDoWriteStep() and VEEPROM_tDoDefragStep()
 */
typedef enum enum_ProcessResult
{
   VEEP_PROCESS_FAILURE    ,
   VEEP_PROCESS_SUCCESSFUL ,
   VEEP_PROCESS_ONGOING    ,
   VEEP_PROCESS_DEFRAG_NEED 
}eProcessResultType;
/**
 * internal work cases of function VEEPROM_tDoWriteStep()
 */
typedef enum enum_WriteSteps
{
   VEEP_WRITE_LABEL            ,
   VEEP_VERIFY_LABEL           ,
   VEEP_WRITE_DATA             ,
   VEEP_VERIFY_DATA            ,
   VEEP_WRITE_CRC              ,
   VEEP_VERIFY_CRC             ,
   VEEP_FIND_NEXT_POSITION     ,
   VEEP_DRAWER_FINISHED
}eWriteStepsType;
/**
 * internal work cases of function VEEPROM_tDoDefragStep()
 */
typedef enum enum_DefragSteps
{
   VEEP_CHECK_PAGE_EMPTY       ,
   VEEP_READ_ERASE_COUNTER     ,
   VEEP_DRAWER_WRITE_START     ,
   VEEP_DRAWER_WRITE_ONGOING   ,
   VEEP_WRITE_ERASE_COUNTER    ,
   VEEP_PAGE_ERASE             ,
   VEEP_WRITE_ERASE_MARK       ,
   VEEP_DEFRAG_FINISHED
}eDefragStepsType;

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#ifdef TIME_MEASUREMENT
static uint16_t VEEPROM_u16TimeBefore;
static uint16_t VEEPROM_u16TimeAfter;
static uint16_t VEEPROM_u16TimeDifference;
#endif
#if defined(VEEPROM_LARGE)
/**
 * active page for storing data
 */
static uint8_t VEEPROM_u8ActiveUnit;
/**
 * \brief content of unwritten flash-cells
 *
 * unwritten (or erased) flash-cells contain the byte 0xFF
 */
const tDrawerID VEEPROM_tInvalidDrawer = 0xFFFFU;
/* variable for Handle task controlled process */
static tDrawerID VEEPROM_tLabel;
#else
/**
 * active page for storing data
 */
static uint8_t VEEPROM_u8ActivePage;
static uint8_t VEEPROM_u8Label; ///< drawer label
#endif
/**
 * actual write-position in active page
 */
static uint8_t *VEEPROM_pu8WriteAddress;
/* variables for Handle task controlled process */
static uint8_t VEEPROM_u8DataLen                         ; ///< valid data length in data array
static uint8_t VEEPROM_au8WriteData[VEEPROM_MAX_DATA_LEN]; ///< drawer data array
/* Variable holds current task state */
static eVEEPROMtaskStateType VEEPROM_tTaskState = VEEP_UNINITIALIZED;
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
#if defined(VEEPROM_LARGE)
static uint8_t VEEPROM_u8GetNextUnit(uint8_t u8Unit);
static uint16_t VEEPROM_u16GetUsedSpace(uint8_t u8Unit);
static bool VEEPROM_bIsUnitErased(uint8_t u8UnitNumber);
static uint8_t *VEEPROM_pu8ResetWriteAddress(uint8_t u8UnitNumber);
static bool VEEPROM_bWriteWithoutCheck(tDrawerID tLabel, const uint8_t *pu8Data, uint8_t u8DataLen );
#else
static uint8_t VEEPROM_ucGetNextPage(uint8_t u8Page);
static uint16_t VEEPROM_u16GetUsedSpace(uint8_t u8Page);
static bool VEEPROM_bIsPageErased(uint8_t u8FlashPageNumber);
static uint8_t* VEEPROM_pu8ResetWriteAddress(uint8_t u8FlashPage);
static bool  VEEPROM_bWriteWithoutCheck(uint8_t u8Label, const uint8_t* pu8Data, uint8_t u8DataLen );
#endif
static bool   VEEPROM_bDefrag( uint8_t u8Source, uint8_t u8Destination );
static eProcessResultType VEEPROM_tDoWriteStep(uint8_t u8PageNumber);
static eProcessResultType VEEPROM_tDoDefragStep(void);
#if defined(DEBUG) && (DEBUG != 0u)
static void VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bool bValue);
#endif
/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
#if defined(VEEPROM_LARGE)
/* VEEPROM_u8GetNextUnit()*/
/**
 * \brief get page-number of next memory unit
 * 
 * 
 * \param u8Unit memory unit number
 * 
 * \return page number of next unit
 */
static uint8_t VEEPROM_u8GetNextUnit ( uint8_t u8Unit )
{
   return (uint8_t)( ( u8Unit + 1U) % VEEPROM_ucNumberOfUnits );
}

/*   VEEPROM_u16GetUsedSpace()*/
/**
 * \brief evaluate bytes already used for given page
 *
 * \note page must not be actually in use. This is why VEEPROM_pu8WriteAddress must not be correct for this page.
 * \param u8Page pagenumber
 * 
 * \return number of bytes (including header and page erase counter) already used of page
 */
static uint16_t VEEPROM_u16GetUsedSpace ( uint8_t u8Unit )
{
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked:
      (a) VEEPROM_pu8ResetWriteAddress(..) returns next free write-address in given Page
      (b) VEEPROM_apicFlashPage contains the start address of this page.
      thus (a) - (b) results in actually used space of given flash-page.
   */
   return (uint16_t)( (uint32_t)VEEPROM_pu8ResetWriteAddress( u8Unit ) - (uint32_t)VEEPROM_apucUnitStart[ u8Unit ] );
}

/*   VEEPROM_pu8ResetWriteAddress()*/
/**
 * \brief retrieve first free drawer-position of desired memory unit
 *
 * if memory unit is full, the next theoretical position after end of flash page is returned.
 * This is not a valid write address. VEEPROM_bWrite() will handle this and initiate a defragmentation
 *
 * \param u8UnitNumber number of memory unit that shall be checked
 *
 * \return position of next free drawer within memory unit
 * \image html VEEPROM_pu8ResetWriteAddress.emf "flowchart of VEEPROM_pu8ResetWriteAddress()"
 * \image latex VEEPROM_pu8ResetWriteAddress.eps "flowchart of VEEPROM_pu8ResetWriteAddress()"

 */
static uint8_t *VEEPROM_pu8ResetWriteAddress( uint8_t u8UnitNumber )
{
   uint8_t       *pu8CurrentAddress;
   uint8_t       *pu8EmptyAreaBase;
   tDrawerID    tDrawerIdentification;
   uint8_t        u8OffsetNextDrawer = 0U;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bReadRetVal;
#endif
   /**
    * \internal
    * \section secSf4 Outline of function
    */
   uint16_t u16TempOffset = VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
   pu8CurrentAddress = VEEPROM_apucUnitStart[ u8UnitNumber ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
   pu8EmptyAreaBase = VEEPROM_pucGetFreeAreaBase(u8UnitNumber, pu8CurrentAddress);
   /**
    * \internal
    * \subsection subSf4_2 Finding End
    * step ahead over all drawers, until field containing DrawerIdentifier contains a blank, unwritten 0xFF.
    *
    */
   do
   {
      /*
         Msg(4:0488) Performing pointer arithmetic. 
         MISRA-C:2004 Rule 17.4 <next> 
         Pointer arithmetic is checked.
         (a) VEEPROM_apucFlashPage contains start address of given flash page
         (b) u16TempOffset contains offset to next drawer
         (c) VEEPROM_ucOffsetDrawerId is the offset of the drawer-ID to start of drawer.
         thus (a) + (b) + c  is position of next drawer-ID for next iteration
      */
      pu8CurrentAddress = VEEPROM_apucUnitStart[ u8UnitNumber ] + u16TempOffset;
      if ( (u16TempOffset + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1u)) <= VEEPROM_uiUnitSize)
      {
#if defined(DEBUG) && (DEBUG != 0u)
         bReadRetVal = FLASH_bRead(pu8CurrentAddress + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tDrawerIdentification, (uint16_t)sizeof(tDrawerIdentification));
         ASSERT_vEXPR ( bReadRetVal != false );
         VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bReadRetVal);
#else
         (void) FLASH_bRead(pu8CurrentAddress + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tDrawerIdentification, (uint16_t)sizeof(tDrawerIdentification));
#endif
         u8OffsetNextDrawer = VEEPROM_ucOffsetDrawerPayload
                            + VEEPROM_ucObserverGetBruttoPayloadLength( tDrawerIdentification );
         ASSERT_vEXPR( u16TempOffset < ( (uint16_t)UINT_MAX - (uint16_t)u8OffsetNextDrawer ) );
         u16TempOffset = u16TempOffset + (uint16_t)u8OffsetNextDrawer;
      }
      else
      {
         /* magic number achieved*/
         break;
      }
   }
   while ( (( tDrawerIdentification != VEEPROM_tInvalidDrawer ) || ( (uint32_t)pu8CurrentAddress < (uint32_t)pu8EmptyAreaBase ))
        && (u16TempOffset <= VEEPROM_uiUnitSize ) );
   /**
    * \internal
    * actually position is after last drawer. This is the location for the next drawer to store.
    * End has been found. Return its address.
    */
   return pu8CurrentAddress;
}

/*   VEEPROM_bDefrag()*/
/**
 * \brief defrag data from full flash page
 * 
 * \pre Destination Page is assumed to be clear. 
 *
 * Data of defragmented u8Source FlashPage is defragmented by copying 
 * cached data of all valid drawers to destination page. 
 * Validity of cached drawers is stored in #CACHE_aucDrawerCacheValidVeeprom.
 * When copying process has finished, fragmented old page is cleared.
 * New defragmented page is set to be VEEPROM_u8ActivePage.
 *
 * \note if VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG is defined, then values used
 *       for defragmentation are read out directly from full flash page. This takes
 *       more time, but results in smaller code for small systems.
 * 
 *
 * \param u8Source unit number to copy data from
 * \param u8Destination unit number to copy data to
 * 
 * \return status of defragmentation:
 *          - false, if error while writing or flash page full
 *          - true,  if defragmentation succeeded with no errors.
 * \image html VEEPROM_bDefrag.emf "flowchart of VEEPROM_bDefrag()"
 * \image latex VEEPROM_bDefrag.eps "flowchart of VEEPROM_bDefrag()"
 */
static bool          VEEPROM_bDefrag                  ( uint8_t u8Source,
                                                        uint8_t u8Destination )
{
//lint -e954 "Pointer variable could be declared as pointing to const"
   tDrawerID  tActualDrawer = 0;
   uint8_t      u8DrawerPayloadLen;
   bool bRetVal = false;
   tDrawerID tNumberOfDrawers = VEEPROM_tGetNumberOfDrawers();
   uint8_t u8PageCounter;
   uint8_t *pu8CacheAddress;
   uint8_t au8NumberPageTransfers[ 2 ];
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
   uint8_t *pu8Data;
#endif
   /**
    * \internal
    * \section secSf6 Outline of function
    * \subsection subSf6_1 Preparations
    * first enable Flash programming. Then
    * set WritePointer to start of erased page and set active page
    */
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start of destination page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is first valid write address on destination flash-page.
   */
   VEEPROM_pu8WriteAddress = VEEPROM_apucUnitStart[ u8Destination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
   if ( VEEPROM_bIsUnitErased(u8Destination) == false )
   {
      /*   error state - destination unit must be erased*/
      _VEEPROM_vResetWatchdogTimer();
      VEEPROM_vBeforePageErase();
      bRetVal = FLASH_bEnableAccess();
      if ( false != bRetVal )
      {
         //erase page has not been finished before. Do erase all pages in unit once more
         for (u8PageCounter = 0; u8PageCounter < VEEPROM_ucFlashPagesPerUnit; u8PageCounter++)
         {
            bRetVal = FLASH_bErase(  VEEPROM_apucUnitStart[ u8Destination ] + (uint16_t)(u8PageCounter * VEEPROM_uiTotalPageSize) );
            _VEEPROM_vResetWatchdogTimer();
         }
         (void) FLASH_bDisableAccess();
         _VEEPROM_vResetWatchdogTimer();
         if ( bRetVal != false )
         {
            VEEPROM_vSetUnitEraseMark( u8Destination );
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
      }
      else
      {
         ASSERT_vEXPR( bRetVal != false);
      }
   }
   /*   comment - increment page transfer counter*/
   /**
    * \internal
    * \subsection subSf6_2 PageTransferCounter
    *  - read out PageTransferCounter high and low byte
    *  - increment PageTransferCounter: check if high byte has to be incremented too
    *  - write incremented PageTransferCounter
    */
   /*   read out page erase counter of full page and increment it*/
#if defined(DEBUG) && (DEBUG != 0u)
   bRetVal = FLASH_bRead( (uint8_t *)VEEPROM_apucUnitStart[ u8Source ], &au8NumberPageTransfers[ 0 ], VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
   ASSERT_vEXPR( false != bRetVal );
#else
   (void) FLASH_bRead( VEEPROM_apucUnitStart[ u8Source ], &au8NumberPageTransfers[ 0 ], VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
#endif
   if ( au8NumberPageTransfers[ 1 ] == 0xFFU )
   {
      au8NumberPageTransfers[ 0 ]++;
      if ( au8NumberPageTransfers[ 0 ] == 0x00U )
      {
         /**
          * for checking reset that occured while process of erasing page is progress, 
          * we check first two bytes of page erase counter.
          *
          * if these two bytes are 0x0000, and there are still two pages containing data, we take the following assumptions>
          * - it was necessary to defragment active page, because data to write was larger than remaining space on active page
          * - transfering data to transfer-page has succeeded, though process of erasing active (old) page was started.
          * - process of erasing page did not finnish due to reset ( if it had it would have erased all cells with value 0xFF
          *
          * ==> as a result of this pageTransferCounter with value 0x0000 is not allowed, so we increment twice and count in the
          *     following way: 0xFFFF, 0x0001, 0x0002, 0x0003, ....
          */
         au8NumberPageTransfers[ 1 ]++;
      }
   }
   au8NumberPageTransfers[ 1 ]++;
    /**
    * \subsection subSf6_3 Defragmenting data
    * for every drawer, ...
    */
   while ( tActualDrawer < tNumberOfDrawers )
   {
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
      /*   comment - defrag data using values from full page*/
      /**
       * ... try to get latest data of actual drawer from fully fragmented page
       * ... if no error while getting latest data of actual drawer, 
       *     copy to new page
       * ... go on with next drawer.
       */
      pu8Data = VEEPROM_pucGetDataPayloadFromPage( u8Source, tActualDrawer );
      if ( pu8Data != 0)
      {
         u8DrawerPayloadLen = VEEPROM_ucGetDrawerSize( tActualDrawer );
         ASSERT_vEXPR(   u8DrawerPayloadLen + VEEPROM_pu8WriteAddress
                     <= VEEPROM_apucUnitStart[ u8Destination ] + VEEPROM_uiUnitSize );
         bRetVal =  VEEPROM_bWriteWithoutCheck( tActualDrawer, pu8Data, u8DrawerPayloadLen );
         if (false == bRetVal )
         {
            ASSERT_vEXPR( false != bRetVal );
            break;
         }
      }
      else
      {
         /* do nothing*/
         /**
          * ... if data 
          * does not exist, 
          * do nothing
          */
      }
#else
      _VEEPROM_vResetWatchdogTimer();
      /* comment - defrag data using cached values*/
      /**
       * ... if cached value is valid, copy this value to new page
       * \note if cached value is invalid, no value has been stored up to now so do nothing.
       */
      if ( CACHE_bIsCacheValidVeeprom( tActualDrawer) != 0U)
      {
         pu8CacheAddress =  CACHE_pucGetCacheAddressVEEPROM( tActualDrawer );
         u8DrawerPayloadLen = VEEPROM_ucGetDrawerSize( tActualDrawer );
         ASSERT_vEXPR ( VEEPROM_pu8WriteAddress < (VEEPROM_apucUnitStart[ u8Destination ] + VEEPROM_uiUnitSize) );
         bRetVal = VEEPROM_bWriteWithoutCheck( tActualDrawer, pu8CacheAddress, u8DrawerPayloadLen );
         if ( false == bRetVal )
         {
            ASSERT_vEXPR( bRetVal != false);
            break;
         }
      }
      else
      {
         /* do nothing*/
         /**
          * \internal
          * if value in cache 
          * not valid, go on  
          * with next drawer.
          */
      }
#endif
      tActualDrawer++;
   }
   /* finally write page erase counter of new page and erase old (full) page if everything ok*/
   /* finally write updated page erase counter*/
   if (false != bRetVal)
   {
      bRetVal = FLASH_bEnableAccess();
      if (false != bRetVal)
      {
         bRetVal = FLASH_bWrite( VEEPROM_apucUnitStart[ u8Destination ], &au8NumberPageTransfers[ 0 ], VEEPROM_PAGE_TRANSFER_COUNTER_LEN );
         (void) FLASH_bDisableAccess();
         if ( false == bRetVal )
         {
            ASSERT_vEXPR( bRetVal != false);
         }
      }
      else
      {
         ASSERT_vEXPR( bRetVal != false);
      }
      /* if everything part of defragmenting data successfully ==> erase full page*/
      _VEEPROM_vResetWatchdogTimer();
      VEEPROM_vBeforePageErase();
      bRetVal = FLASH_bEnableAccess();
      if ( false != bRetVal )
      {
         //erase page has not been finished before. Do erase all pages in unit once more
         for (u8PageCounter = 0; u8PageCounter < VEEPROM_ucFlashPagesPerUnit; u8PageCounter++)
         {
            bRetVal = FLASH_bErase(  VEEPROM_apucUnitStart[ u8Source ] + (uint16_t)(u8PageCounter * VEEPROM_uiTotalPageSize) );
            _VEEPROM_vResetWatchdogTimer();
         }
         (void) FLASH_bDisableAccess();
         _VEEPROM_vResetWatchdogTimer();
         if ( bRetVal != false )
         {
            VEEPROM_vSetUnitEraseMark( u8Source );
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
      }
      else
      {
         ASSERT_vEXPR( bRetVal != false);
      }
   }
   VEEPROM_u8ActiveUnit = u8Destination;
   return bRetVal;
}
/* VEEPROM_bWriteWithoutCheck()*/
/**
 * \brief writing data to VEEPROM without checking if remaining space left in Flash is large enough
 *
 * \pre remaining space in Flash is big enough to hold new data
 * 
 * \param tLabel DrawerID to write to
 * \param pu8Data Buffer containing data
 * \param u8DataLen length of Buffer to write
 */
static bool  VEEPROM_bWriteWithoutCheck     ( tDrawerID   tLabel,
                                              const uint8_t *pu8Data,
                                              uint8_t        u8DataLen )
{
   uint8_t     *pu8DrawerStartAddress =  VEEPROM_pu8WriteAddress;
   uint8_t     ucPosition;
   bool      bRetVal;
   uint8_t     u8Index;
   uint8_t     u8Crc = 0;
   // to get real payload len including unused cells for cellspace != 0
   uint8_t     u8PayloadLen;
   tDrawerID tReadOutDrawerId; 
#if defined(DEBUG) && (DEBUG != 0u)
   bool bDbgRetVal;
#endif
   /* comment - enable FLASH-Write and write DrawerIdentifier. Then write all data of payload */
   /**
    * \internal
    * \subsection sub1_3 Write
    * Data to write is new or different from latest valid data stored.
    * First enable Flash operations ...
    * ... set Write-Pointer to Address of Identifier of Drawer and <b>write Identifier</b>
    * ... set Write-Pointer to start of Payload of Drawer and <b>write data</b>
    */
   bRetVal = FLASH_bEnableAccess();
   if ( false != bRetVal )
   {
      bRetVal = FLASH_bWrite( pu8DrawerStartAddress + VEEPROM_ucOffsetDrawerId, (tWriteData *)(void *)&tLabel, (uint16_t)sizeof(tDrawerID));
      ASSERT_vEXPR( bRetVal != false);
      /* remaining consistency of data stored in FLASH :*/
      /* If there was an interference while writing drawer ID it can be that the wrong drawer ID was written to FLASH. - If this happens, actually (wrong) written drawer ID is taken as input to get its drawer length to set new valid WriteAddress. */
      /* - In this case the data to write is not written and the return value of the function is false.  */
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead ( pu8DrawerStartAddress + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8DrawerStartAddress + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
#endif
      u8PayloadLen = VEEPROM_ucObserverGetBruttoPayloadLength( tReadOutDrawerId );
      if (tReadOutDrawerId == tLabel )
      {
         /* start calculation of crc */
         CRC_vUpdateCrc( &u8Crc, HSUP_ucGetHighByte(tLabel));
         CRC_vUpdateCrc( &u8Crc, HSUP_ucGetLowByte(tLabel));
         VEEPROM_pu8WriteAddress = &pu8DrawerStartAddress[ VEEPROM_ucOffsetDrawerPayload ];
         bRetVal = FLASH_bWrite(VEEPROM_pu8WriteAddress, pu8Data, u8DataLen);
         (void)bRetVal;
         ASSERT_vEXPR( bRetVal != false);
         /* continue calculation of crc */
         for ( ucPosition = 0; ucPosition < u8DataLen; ucPosition++ )
         {
            CRC_vUpdateCrc( &u8Crc, pu8Data[ ucPosition ] );
         }
         /* comment - finally write CRC or validity field */
         /**
          * \internal
          * ... set Write-Pointer to Address of consistency-Field and <b>write consistency-Field</b>
          * ... finally update Write-Pointer and disable Flash operations 
          */
         bRetVal = FLASH_bWriteByte( &pu8DrawerStartAddress[ 0 ], u8Crc );
         ASSERT_vEXPR( bRetVal != false);
      }
      else
      {
         ASSERT_vEXPR( tReadOutDrawerId == tLabel);
         bRetVal = false;
      }
      u8Index =  u8PayloadLen + VEEPROM_ucOffsetDrawerPayload;
      VEEPROM_pu8WriteAddress = &pu8DrawerStartAddress[ u8Index ];
   }
   else
   {
      ASSERT_vEXPR( bRetVal != false);
   }
   (void) FLASH_bDisableAccess();
   return bRetVal;
}
/* VEEPROM_bIsUnitErased() */
/**
 * \brief checks if desired memory unit is empty, except place for page erase mark
 *
 * \param u8UnitNumber Number of memory unit
 * \return 
 *          - true: flash page is empty
 *          - false: at least one cell of flash page is not empty
 */
static bool VEEPROM_bIsUnitErased ( uint8_t u8UnitNumber )
{
   bool bIsEmpty = true;
   uint8_t *pu8ReadAddress;
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      start of page + length of page == end of page
   */
   uint8_t *pu8EndAddress = VEEPROM_apucUnitStart[ u8UnitNumber ] + VEEPROM_uiUnitSize;
   /* test memory contents */
   pu8ReadAddress = VEEPROM_apucUnitStart[ u8UnitNumber ];
   do
   {
      if ( *pu8ReadAddress != VEEPROM_UNWRITTEN_FLASH_CELL )
      {
         bIsEmpty = false;
      }
      ++pu8ReadAddress;
   }
   while ( (bIsEmpty == true) && ((uint32_t)pu8ReadAddress < (uint32_t)pu8EndAddress) );
   return bIsEmpty;
}
/* VEEPROM_bIsPageErased() */
#if 0
/**
 * \brief checks if desired flash page is completely erased
 *
 * \param u8FlashPageNumber Number of Flash Page
 * \return 
 *          - true: flash page is erased completely.
 *          - false: at least one cell of flash page is not erased.
 */
static bool VEEPROM_bIsPageErased ( uint8_t u8FlashPageNumber )
{
   bool bIsEmpty = true;
   uint8_t u8Value;
   uint8_t *pu8ReadAddress;
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      start of page + length of page == end of page
   */
   uint8_t *pu8EndAddress = VEEPROM_apucUnitStart[0] + (VEEPROM_uiTotalPageSize * (u8FlashPageNumber + 1));
   /* test memory contents */
   pu8ReadAddress = VEEPROM_apucUnitStart[0] + (VEEPROM_uiTotalPageSize * u8FlashPageNumber);
   do
   {
      (void) FLASH_bRead(pu8ReadAddress, &u8Value, 1 );
      if ( *pu8ReadAddress != VEEPROM_UNWRITTEN_FLASH_CELL )
      {
         bIsEmpty = false;
      }
      ++pu8ReadAddress;
   }
   while ( (bIsEmpty == true) && ((uint32_t)pu8ReadAddress < (uint32_t)pu8EndAddress) );
   return bIsEmpty;
}
#endif
/* VEEPROM_tDoWriteStep() */
static eProcessResultType VEEPROM_tDoWriteStep( uint8_t u8UnitNumber )
{
   eProcessResultType tRetVal = VEEP_PROCESS_ONGOING;
   tDrawerID          tReadOutDrawerId;
   uint16_t               u16FreeSpace;
   static eWriteStepsType  VEEPROM_tWriteStep = VEEP_WRITE_LABEL;
   static uint8_t           *VEEPROM_pu8DrawerStart;
   static uint8_t            VEEPROM_u8PayloadLen;
   static uint8_t            VEEPROM_u8Crc;
   static uint8_t            VEEPROM_u8Position;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bResult;
#endif
   switch (VEEPROM_tWriteStep)
   {
      case VEEP_WRITE_LABEL:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bEnableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_pu8DrawerStart = VEEPROM_pu8WriteAddress;
         bResult = FLASH_bWrite(VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, (tWriteData *)&VEEPROM_tLabel, (uint16_t)sizeof(VEEPROM_tLabel));
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bDisableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_tWriteStep     = VEEP_VERIFY_LABEL;
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         VEEPROM_pu8DrawerStart = VEEPROM_pu8WriteAddress;
         (void)FLASH_bWrite(VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, (tWriteData *)(void *)&VEEPROM_tLabel,(uint16_t)sizeof(VEEPROM_tLabel));
         (void) FLASH_bDisableAccess();
         VEEPROM_tWriteStep     = VEEP_VERIFY_LABEL;
#endif
         break;
      }
      case VEEP_VERIFY_LABEL:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bRead ( VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tReadOutDrawerId) );
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
#else
         /* normal version */
         (void) FLASH_bRead ( VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tReadOutDrawerId) );
#endif
         VEEPROM_u8PayloadLen = VEEPROM_ucObserverGetBruttoPayloadLength( tReadOutDrawerId );
         if ( tReadOutDrawerId == VEEPROM_tLabel )
         {
            /* start calculation crc*/
            VEEPROM_u8Crc = 0;
            VEEPROM_u8Position = 0;
            CRC_vUpdateCrc( &VEEPROM_u8Crc, HSUP_ucGetHighByte(VEEPROM_tLabel));
            CRC_vUpdateCrc( &VEEPROM_u8Crc, HSUP_ucGetLowByte(VEEPROM_tLabel));
            VEEPROM_tWriteStep = VEEP_WRITE_DATA;
         }
         else
         {
            VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
         }
         break;
      }
      case VEEP_WRITE_DATA:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         if ( VEEPROM_u8Position < VEEPROM_u8DataLen)
         {
            bResult = FLASH_bEnableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            bResult = FLASH_bWrite(&VEEPROM_pu8DrawerStart[ VEEPROM_ucOffsetDrawerPayload + VEEPROM_u8Position], (tWriteData *)&VEEPROM_au8WriteData[VEEPROM_u8Position], VEEPROM_ucCellSize);
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            bResult = FLASH_bDisableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            if ( (VEEPROM_u8Position + VEEPROM_ucCellSize) < VEEPROM_u8DataLen )
            {
               VEEPROM_u8Position += VEEPROM_ucCellSize;
            }
            else
            {
               VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
            }
         }
         else
         {
            if ( VEEPROM_u8DataLen == 0 )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
         }
#else
         /* normal version */
         if ( VEEPROM_u8Position < VEEPROM_u8DataLen)
         {
            (void)FLASH_bEnableAccess();
            (void)FLASH_bWrite(&VEEPROM_pu8DrawerStart[VEEPROM_ucOffsetDrawerPayload + VEEPROM_u8Position], (tWriteData *)(void *)&VEEPROM_au8WriteData[VEEPROM_u8Position], VEEPROM_ucCellSize);
            (void) FLASH_bDisableAccess();
            if ( (VEEPROM_u8Position + VEEPROM_ucCellSize) < VEEPROM_u8DataLen )
            {
               VEEPROM_u8Position += VEEPROM_ucCellSize;
            }
            else
            {
               VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
            }
         }
         else
         {
            VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
         }
#endif
         break;
      }
      case VEEP_VERIFY_DATA:
      {
         VEEPROM_tWriteStep = VEEP_WRITE_CRC;
         for ( VEEPROM_u8Position = 0; VEEPROM_u8Position < VEEPROM_u8DataLen; VEEPROM_u8Position++ )
         {
            /* continue calculation of crc */
            CRC_vUpdateCrc( &VEEPROM_u8Crc, VEEPROM_au8WriteData[VEEPROM_u8Position] );
            if ( VEEPROM_pu8DrawerStart[ VEEPROM_ucOffsetDrawerPayload + VEEPROM_u8Position] != VEEPROM_au8WriteData[VEEPROM_u8Position] )
            {
               VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
               break; //end of cyclus for
            }
         }
         break;
      }
      case VEEP_WRITE_CRC:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bEnableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bWriteByte( VEEPROM_pu8DrawerStart, VEEPROM_u8Crc );
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bDisableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_tWriteStep     = VEEP_VERIFY_CRC;
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         (void)FLASH_bWriteByte( VEEPROM_pu8DrawerStart, VEEPROM_u8Crc );
         (void) FLASH_bDisableAccess();
         VEEPROM_tWriteStep     = VEEP_VERIFY_CRC;
#endif
         break;
      }
      case VEEP_VERIFY_CRC:
      {
         if ( VEEPROM_pu8DrawerStart[0] != VEEPROM_u8Crc )
         {
            VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
         }
         else
         {
            VEEPROM_tWriteStep = VEEP_DRAWER_FINISHED;
         }
         break;
      }
      case VEEP_DRAWER_FINISHED:
      {
         VEEPROM_pu8WriteAddress = &VEEPROM_pu8DrawerStart[ VEEPROM_u8PayloadLen + VEEPROM_ucOffsetDrawerPayload ];
         VEEPROM_tWriteStep = VEEP_WRITE_LABEL;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
         break;
      }
      case VEEP_FIND_NEXT_POSITION:
      {
         /* some data byte not correctly written, */
         /* move pointer to next position and try again */
         u16FreeSpace = (uint16_t)(((uint32_t)VEEPROM_apucUnitStart[ u8UnitNumber ] + VEEPROM_uiUnitSize) - (uint32_t)(&VEEPROM_pu8DrawerStart[ VEEPROM_u8PayloadLen + VEEPROM_ucOffsetDrawerPayload ]) );
         if ( u16FreeSpace > ((uint16_t)VEEPROM_u8PayloadLen + (uint16_t)VEEPROM_ucOffsetDrawerPayload) )
         {
            /* move pointer to next position and try again */
            /* Debug */
#if defined(DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
#endif
            VEEPROM_pu8WriteAddress = &VEEPROM_pu8DrawerStart[ VEEPROM_u8PayloadLen + VEEPROM_ucOffsetDrawerPayload ];
         }
         else
         {
            /* not enougt memory space on page */
            /* Debug */
#if defined(DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
#endif
            /* read out another drawer ID */
            tRetVal = VEEP_PROCESS_DEFRAG_NEED;
         }
         VEEPROM_tWriteStep = VEEP_WRITE_LABEL;
         break;
      }
      default:
      {
         ;
      }
   }
   return(tRetVal);
}
/* VEEPROM_tDoDefragStep() */
//#define TEST_EXIT_AFTER_READ_ERASE_COUNTER
//#define TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN
//#define TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN
//#define TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN
//#define TEST_EXIT_AFTER_OLD_PAGE_ERASE
#if defined(TEST_EXIT_AFTER_READ_ERASE_COUNTER)  || defined(TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN)  ||     defined(TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN) || defined(TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN) ||     defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
static uint8_t *VEEPROM_pu8WriteAddress_OLD = NULL;
#endif
static eProcessResultType VEEPROM_tDoDefragStep(void)
{
   eProcessResultType tRetVal         = VEEP_PROCESS_ONGOING;
   eProcessResultType tWriteResult                          ;
   uint8_t             *pu8CacheAddress                     ;
   uint8_t              u8Counter                           ;
#if !defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
   uint8_t             *pu8MagicNumberPosition = NULL       ;
   uint8_t              u8MagicNumber                       ;
#endif
#if defined(DEBUG) && (DEBUG != 0u)
   bool bResult;
#endif
   static eDefragStepsType  VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
   static uint8_t     VEEPROM_u8Source                   ;
   static uint8_t     VEEPROM_u8Destination              ;
   static uint8_t     VEEPROM_u8CurUnitNr                ; //holds page number to be erased, source or destination
   static uint8_t     VEEPROM_au8NumberPageTransfers[ 2 ];
   static tDrawerID VEEPROM_tActDefragDrawer             ;
   static uint8_t     VEEPROM_u8PageCounter              ;
   switch (VEEPROM_tDefragStep)
   {
      case VEEP_CHECK_PAGE_EMPTY:
      {
         VEEPROM_u8Source          = VEEPROM_u8ActiveUnit;
         VEEPROM_u8Destination     = VEEPROM_u8GetNextUnit( VEEPROM_u8ActiveUnit );
         VEEPROM_tActDefragDrawer = 0;
#ifdef TIME_MEASUREMENT
         VEEPROM_u16TimeBefore = TIM_uiGetCircleMicroSeconds();
#endif
         if ( VEEPROM_bIsUnitErased( VEEPROM_u8Destination ) != false )
         {
#ifdef TIME_MEASUREMENT
            VEEPROM_u16TimeAfter = TIM_uiGetCircleMicroSeconds();
            VEEPROM_u16TimeDifference = VEEPROM_u16TimeAfter - VEEPROM_u16TimeBefore;
#endif
            VEEPROM_tDefragStep = VEEP_READ_ERASE_COUNTER;
         }
         else
         {
#ifdef TIME_MEASUREMENT
            VEEPROM_u16TimeAfter = TIM_uiGetCircleMicroSeconds();
            VEEPROM_u16TimeDifference = VEEPROM_u16TimeAfter - VEEPROM_u16TimeBefore;
#endif
            VEEPROM_u8CurUnitNr = VEEPROM_u8Destination;
            VEEPROM_u8PageCounter = 0;
            VEEPROM_tDefragStep   = VEEP_PAGE_ERASE;
         }
         break;
      }
      case VEEP_READ_ERASE_COUNTER:
      {
         VEEPROM_u8CurUnitNr = VEEPROM_u8Source;
#if defined(TEST_EXIT_AFTER_READ_ERASE_COUNTER)  || defined(TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN)  ||     defined(TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN) || defined(TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN) ||     defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
         VEEPROM_pu8WriteAddress_OLD = VEEPROM_pu8WriteAddress;
#endif
         VEEPROM_pu8WriteAddress = VEEPROM_apucUnitStart[ VEEPROM_u8Destination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
         /* comment - increment page transfer counter */
         /**
          * \internal
          * \subsection subSf6_2 PageTransferCounter
          *  - read out PageTransferCounter high and low byte
          *  - increment PageTransferCounter: check if high byte has to be incremented too
          *  - write incremented PageTransferCounter
          */
         /* read out page erase counter of full page and increment it */
#if defined(DEBUG) && (DEBUG != 0u)
         bResult = FLASH_bRead( VEEPROM_apucUnitStart[ VEEPROM_u8Source ], (tReadData *)(void *)&VEEPROM_au8NumberPageTransfers[ 0 ], 2);
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
#else
         (void) FLASH_bRead(VEEPROM_apucUnitStart[ VEEPROM_u8Source ],(tReadData *)(void *)&VEEPROM_au8NumberPageTransfers[ 0 ], 2);
#endif
         if ( VEEPROM_au8NumberPageTransfers[ 1 ] == 0xFFU )
         {
            VEEPROM_au8NumberPageTransfers[ 0 ]++;
            if ( VEEPROM_au8NumberPageTransfers[ 0 ] == 0x00U )
            {
               /**
                * for checking reset that occured while process of erasing page is progress, 
                * we check first two bytes of page erase counter.
                *
                * if these two bytes are 0x0000, and there are still two pages containing data, we take the following assumptions>
                * - it was necessary to defragment active page, because data to write was larger than remaining space on active page
                * - transfering data to transfer-page has succeeded, though process of erasing active (old) page was started.
                * - process of erasing page did not finnish due to reset ( if it had it would have erased all cells with value 0xFF
                *
                * ==> as a result of this pageTransferCounter with value 0x0000 is not allowed, so we increment twice and count in the
                *     following way: 0xFFFF, 0x0001, 0x0002, 0x0003, ....
                */
               VEEPROM_au8NumberPageTransfers[ 1 ]++;
            }
         }
         VEEPROM_au8NumberPageTransfers[ 1 ]++;
#if !defined(TEST_EXIT_AFTER_READ_ERASE_COUNTER)
         VEEPROM_tDefragStep = VEEP_DRAWER_WRITE_START;
#else
         /* only for test  */
         VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         break;
      }
      case VEEP_DRAWER_WRITE_START:
      {
         /* comment - defrag data using cached values */
         /**
          * ... if cached value is valid, copy this value to new page
          * \note if cached value is invalid, no value has been stored up to now so do nothing.
          */
         while ( VEEPROM_tActDefragDrawer < VEEPROM_tGetNumberOfDrawers() )
         {
            if ( CACHE_bIsCacheValidVeeprom( VEEPROM_tActDefragDrawer) != 0U )
            {
               pu8CacheAddress =  CACHE_pucGetCacheAddressVEEPROM( VEEPROM_tActDefragDrawer );
               VEEPROM_u8DataLen   = VEEPROM_ucGetDrawerSize( VEEPROM_tActDefragDrawer );
               VEEPROM_tLabel     = VEEPROM_tActDefragDrawer  ;
               /* copy data to its own field */
               for (u8Counter = 0; u8Counter < VEEPROM_u8DataLen; u8Counter++)
               {
                  VEEPROM_au8WriteData[u8Counter] = pu8CacheAddress[u8Counter];
               }
               /* fill rest of field with 0xFF => there must be empty info */
               for (u8Counter = VEEPROM_u8DataLen; u8Counter < VEEPROM_MAX_DATA_LEN; u8Counter++)
               {
                  VEEPROM_au8WriteData[u8Counter] = VEEPROM_UNWRITTEN_FLASH_CELL;
               }
               VEEPROM_tDefragStep = VEEP_DRAWER_WRITE_ONGOING;
               VEEPROM_tActDefragDrawer++; //set next drawer for future
               break; //from while cyclus
            }
            else
            {
               VEEPROM_tActDefragDrawer++;
            }
         }
         if (VEEPROM_tDefragStep == VEEP_DRAWER_WRITE_START )
         {
            /* nothing to write */
#if !defined(TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN)
            VEEPROM_tDefragStep = VEEP_WRITE_ERASE_COUNTER;
#else
            /* only for test  */
            VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         }
         break;
      }
      case VEEP_DRAWER_WRITE_ONGOING:
      {
         tWriteResult = VEEPROM_tDoWriteStep(VEEPROM_u8Destination);
         if ( tWriteResult == VEEP_PROCESS_SUCCESSFUL )
         {
#if !defined(TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN)
            VEEPROM_tDefragStep = VEEP_DRAWER_WRITE_START;
#else
            /* only for test  */
            VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         }
         else
         {
            if ( (tWriteResult == VEEP_PROCESS_FAILURE) || (tWriteResult == VEEP_PROCESS_DEFRAG_NEED) )
            {
               /* Debug */
#if defined(DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
#endif
               VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
               tRetVal = VEEP_PROCESS_FAILURE;
            }
            else
            {
               /* process ongoing */
            }
         }
         break;
      }
      case VEEP_WRITE_ERASE_COUNTER:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bEnableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bWrite(VEEPROM_apucUnitStart[ VEEPROM_u8Destination ], (tWriteData *) &VEEPROM_au8NumberPageTransfers[ 0 ], 2);
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bDisableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_u8PageCounter = 0;
         VEEPROM_tDefragStep   = VEEP_PAGE_ERASE;
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         (void)FLASH_bWrite(VEEPROM_apucUnitStart[ VEEPROM_u8Destination ], &VEEPROM_au8NumberPageTransfers[ 0 ], 2);
         (void)FLASH_bDisableAccess();
         VEEPROM_u8PageCounter = 0;
         VEEPROM_tDefragStep   = VEEP_PAGE_ERASE;

#endif
         break;
      }
      case VEEP_PAGE_ERASE:
      {
#if !defined(TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN)
         if ( VEEPROM_u8PageCounter == 0u )
         {
            VEEPROM_vBeforePageErase();
         }
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         if ( VEEPROM_u8PageCounter < VEEPROM_ucFlashPagesPerUnit )
         {
            bResult = FLASH_bEnableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            /* erase only one page of whole flash unit */
            bResult = FLASH_bErase(VEEPROM_apucUnitStart[ VEEPROM_u8CurUnitNr ] + (uint16_t)(VEEPROM_u8PageCounter * VEEPROM_uiTotalPageSize));
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            bResult = FLASH_bDisableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            VEEPROM_u8PageCounter++;
         }
         else
         {
            VEEPROM_u8PageCounter = 0;
            VEEPROM_tDefragStep     = VEEP_WRITE_ERASE_MARK;
         }
#else
         /* normal version */
         if ( VEEPROM_u8PageCounter < VEEPROM_ucFlashPagesPerUnit )
         {
            (void)FLASH_bEnableAccess();
            (void)FLASH_bErase(VEEPROM_apucUnitStart[ VEEPROM_u8CurUnitNr ] + (uint16_t)(VEEPROM_u8PageCounter * VEEPROM_uiTotalPageSize));
            (void)FLASH_bDisableAccess();
            VEEPROM_u8PageCounter++;
         }
         else
         {
            VEEPROM_u8PageCounter = 0;
            VEEPROM_tDefragStep     = VEEP_WRITE_ERASE_MARK;
         }
#endif
#else
         /* only for test  */
         VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         break;
      }
      case VEEP_WRITE_ERASE_MARK:
      {
#if !defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
         u8MagicNumber = 0xA5;
         VEEPROM_vSetEraseMarkData( VEEPROM_u8CurUnitNr, &u8MagicNumber,&pu8MagicNumberPosition);
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         if ( pu8MagicNumberPosition != NULL )
         {
            ASSERT_vEXPR( false != VEEPROM_bIsUnitErased( VEEPROM_u8CurUnitNr ) );
            bResult = FLASH_bEnableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            bResult = FLASH_bWrite(pu8MagicNumberPosition, (tWriteData *) &u8MagicNumber, (uint16_t)sizeof(u8MagicNumber));
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            bResult = FLASH_bDisableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            if ( VEEPROM_u8CurUnitNr == VEEPROM_u8Source )
            {
               VEEPROM_tDefragStep     = VEEP_DEFRAG_FINISHED;
            }
            else
            {
               VEEPROM_tDefragStep     = VEEP_READ_ERASE_COUNTER;
            }
         }
         else
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            /* user access disabled or setup CPU problem */
            tRetVal = VEEP_PROCESS_FAILURE;
         }
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         (void)FLASH_bWrite(pu8MagicNumberPosition, &u8MagicNumber, (uint16_t)sizeof(u8MagicNumber));
         (void)FLASH_bDisableAccess();
         if ( VEEPROM_u8CurUnitNr == VEEPROM_u8Source )
         {
            VEEPROM_tDefragStep     = VEEP_DEFRAG_FINISHED;
         }
         else
         {
            VEEPROM_tDefragStep     = VEEP_READ_ERASE_COUNTER;
         }
#endif
         VEEPROM_vDefragEnded();
#else
         /* only for test  */
         VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         break;
      }
      case VEEP_DEFRAG_FINISHED:
      {
         VEEPROM_u8ActiveUnit = VEEPROM_u8Destination;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
         break;
      }
      default:
      {
         ;
      }
   }
   return(tRetVal);
}
#else
/* VEEPROM_ucGetNextPage() */
/**
 * \brief get page-number of next page
 * 
 * 
 * \param u8Page page number
 * 
 * \return page number of next page
 */
static uint8_t VEEPROM_ucGetNextPage ( uint8_t u8Page )
{
   return (uint8_t)( ( u8Page + 1U) % VEEPROM_ucNumberOfPages );
}
/* VEEPROM_u16GetUsedSpace() */
/**
 * \brief evaluate bytes already used for given page
 *
 * \note page must not be actually in use. This is why VEEPROM_pu8WriteAddress must not be correct for this page.
 * \param u8Page pagenumber
 * 
 * \return number of bytes (including header and page erase counter) already used of page
 */
static uint16_t VEEPROM_u16GetUsedSpace ( uint8_t u8Page )
{
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked:
      (a) VEEPROM_pu8ResetWriteAddress(..) returns next free write-address in given Page
      (b) VEEPROM_apicFlashPage contains the start address of this page.
      thus (a) - (b) results in actually used space of given flash-page.
   */
   return (uint16_t)( (uint32_t)VEEPROM_pu8ResetWriteAddress( u8Page ) - (uint32_t)VEEPROM_apucFlashPage[ u8Page ] );
}
/* VEEPROM_pu8ResetWriteAddress() */
/**
 * \brief retrieve first free drawer-position of desired FlashPage
 *
 * if FlashPage is full, the next theoretical position after end of flash page is returned.
 * This is not a valid write address. VEEPROM_bWrite() will handle this and initiate a defragmentation
 *
 * \param u8FlashPage number of FlashPage that shall be 
 *
 * \return position of next free drawer within FlashPage
 * \image html VEEPROM_pu8ResetWriteAddress.emf "flowchart of VEEPROM_pu8ResetWriteAddress()"
 * \image latex VEEPROM_pu8ResetWriteAddress.eps "flowchart of VEEPROM_pu8ResetWriteAddress()"

 */
static uint8_t* VEEPROM_pu8ResetWriteAddress   ( uint8_t u8FlashPage )
{
   uint8_t *pu8CurrentAddress;
   uint8_t *pu8EmptyAreaBase;
   uint8_t  u8DrawerIdentification;
   uint8_t  u8OffsetNextDrawer = 0U;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bReadRetVal;
#endif
   /**
    * \internal
    * \section secSf4 Outline of function
    */
   uint16_t u16TempOffset = VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
   pu8CurrentAddress = VEEPROM_apucFlashPage[ u8FlashPage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
   pu8EmptyAreaBase = VEEPROM_pucGetFreeAreaBase(u8FlashPage, pu8CurrentAddress);
   /**
    * \internal
    * \subsection subSf4_2 Finding End
    * step ahead over all drawers, until field containing DrawerIdentifier contains a blank, unwritten 0xFF.
    *
    */
   do
   {
      /*
         Msg(4:0488) Performing pointer arithmetic. 
         MISRA-C:2004 Rule 17.4 <next> 
         Pointer arithmetic is checked.
         (a) VEEPROM_apucFlashPage contains start address of given flash page
         (b) u16TempOffset contains offset to next drawer
         (c) VEEPROM_ucOffsetDrawerId is the offset of the drawer-ID to start of drawer.
         thus (a) + (b) + c  is position of next drawer-ID for next iteration
      */
      pu8CurrentAddress = VEEPROM_apucFlashPage[ u8FlashPage ] + u16TempOffset;
      if ((u16TempOffset + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1U)) <= VEEPROM_uiFlashPageSize )
      {
#if defined(DEBUG) && (DEBUG != 0u)
         bReadRetVal = (bool)FLASH_bRead(pu8CurrentAddress + VEEPROM_ucOffsetDrawerId, &u8DrawerIdentification, 1U);
         ASSERT_vEXPR ( bReadRetVal != false );
         VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bReadRetVal);
#else
         (void) FLASH_bRead(pu8CurrentAddress + VEEPROM_ucOffsetDrawerId, &u8DrawerIdentification, 1U);
#endif
         u8OffsetNextDrawer = VEEPROM_ucOffsetDrawerPayload 
                            + VEEPROM_ucObserverGetBruttoPayloadLength( u8DrawerIdentification );
      ASSERT_vEXPR( u16TempOffset < ( (uint16_t)UINT_MAX - (uint16_t)u8OffsetNextDrawer ));
      u16TempOffset = ( u16TempOffset + (uint16_t)u8OffsetNextDrawer);
      }
      else
      {
         /* magic number achieved*/
         break;
      }
   }
   while ( (( u8DrawerIdentification != VEEPROM_UNWRITTEN_FLASH_CELL ) || ( (uint32_t)pu8CurrentAddress < (uint32_t)pu8EmptyAreaBase ))
        && ( u16TempOffset <=  VEEPROM_uiFlashPageSize ) );
   /**
    * \internal
    * actually position is after last drawer. This is the location for the next drawer to store.
    * End has been found. Return its address.
    */
   return pu8CurrentAddress;
}
/* VEEPROM_bDefrag() */
/**
 * \brief defrag data from full flash page
 * 
 * \pre Destination Page is assumed to be clear. 
 *
 * Data of defragmented u8Source FlashPage is defragmented by copying 
 * cached data of all valid drawers to destination page. 
 * Validity of cached drawers is stored in CACHE_aucDrawerCacheValidVeeprom.
 * When copying process has finished, fragmented old page is cleared.
 * New defragmented page is set to be VEEPROM_u8ActivePage.
 *
 * \note if VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG is defined, then values used
 *       for defragmentation are read out directly from full flash page. This takes
 *       more time, but results in smaller code for small systems.
 * 
 *
 * \param u8Source page number to copy data from
 * \param u8Destination page number to copy data to
 * 
 * \return status of defragmentation:
 *          - false, if error while writing or flash page full
 *          - true,  if defragmentation succeeded with no errors.
 * \image html VEEPROM_bDefrag.emf "flowchart of VEEPROM_bDefrag()"
 * \image latex VEEPROM_bDefrag.eps "flowchart of VEEPROM_bDefrag()"
 */
static bool          VEEPROM_bDefrag                  ( uint8_t u8Source,
                                                        uint8_t u8Destination )
{
   uint8_t  u8ActualDrawer = 0U;
   uint8_t  u8DrawerPayloadLen;

   bool bRetVal = false;
   uint8_t u8NumberOfDrawers = VEEPROM_ucGetNumberOfDrawers();
   const uint8_t* pu8CacheAddress;
   uint8_t au8NumberPageTransfers[ 2 ];
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
   uint8_t* pu8Data;
#endif
   /**
    * \internal
    * \section secSf6 Outline of function
    * \subsection subSf6_1 Preparations
    * first enable Flash programming. Then
    * set WritePointer to start of erased page and set active page
    */
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start of destination page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is first valid write address on destination flash-page.
   */
   VEEPROM_pu8WriteAddress = VEEPROM_apucFlashPage[ u8Destination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
   if ( VEEPROM_bIsPageErased(u8Destination) == false )
   {
      /* error state - destination must be erased */
      _VEEPROM_vResetWatchdogTimer();
      VEEPROM_vBeforePageErase();
      bRetVal = FLASH_bEnableAccess();
      if ( false != bRetVal )
      {
         bRetVal = FLASH_bErase(  VEEPROM_apucFlashPage[ u8Destination ] );
         (void) FLASH_bDisableAccess();
         _VEEPROM_vResetWatchdogTimer();
         if ( bRetVal != false )
         {
            VEEPROM_vSetPageEraseMark( u8Destination );
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
      }
      else
      {
         ASSERT_vEXPR( bRetVal != false);
      }
   }
   /* comment - increment page transfer counter */
   /**
    * \internal
    * \subsection subSf6_2 PageTransferCounter
    *  - read out PageTransferCounter high and low byte
    *  - increment PageTransferCounter: check if high byte has to be incremented too
    *  - write incremented PageTransferCounter
    */
   /* read out page erase counter of full page and increment it */
#if defined(DEBUG) && (DEBUG != 0u)
   bRetVal = (bool)FLASH_bRead( VEEPROM_apucFlashPage[ u8Source ], &au8NumberPageTransfers[ 0 ], 2);
   ASSERT_vEXPR( false != bRetVal );
#else
   (void) FLASH_bRead( VEEPROM_apucFlashPage[ u8Source ], &au8NumberPageTransfers[ 0 ], 2);
#endif
   if ( au8NumberPageTransfers[ 1 ] == 0xFFU )
   {
      au8NumberPageTransfers[ 0 ]++;
      if ( au8NumberPageTransfers[ 0 ] == 0x00U )
      {
         /**
          * for checking reset that occured while process of erasing page is progress, 
          * we check first two bytes of page erase counter.
          *
          * if these two bytes are 0x0000, and there are still two pages containing data, we take the following assumptions>
          * - it was necessary to defragment active page, because data to write was larger than remaining space on active page
          * - transfering data to transfer-page has succeeded, though process of erasing active (old) page was started.
          * - process of erasing page did not finnish due to reset ( if it had it would have erased all cells with value 0xFF
          *
          * ==> as a result of this pageTransferCounter with value 0x0000 is not allowed, so we increment twice and count in the
          *     following way: 0xFFFF, 0x0001, 0x0002, 0x0003, ....
          */
         au8NumberPageTransfers[ 1 ]++;
      }
   }
   au8NumberPageTransfers[ 1 ]++;
   /**
    * \subsection subSf6_3 Defragmenting data
    * for every drawer, ...
    */
   while ( u8ActualDrawer < u8NumberOfDrawers )
   {
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
      /* comment - defrag data using values from full page */
      /**
       * ... try to get latest data of actual drawer from fully fragmented page
       * ... if no error while getting latest data of actual drawer, 
       *     copy to new page
       * ... go on with next drawer.
       */
      pu8Data = VEEPROM_pucGetDataPayloadFromPage( u8Source, u8ActualDrawer );
      if ( pu8Data != 0)
      {
         u8DrawerPayloadLen = VEEPROM_ucGetDrawerSize( u8ActualDrawer );
         ASSERT_vEXPR(   u8DrawerPayloadLen + VEEPROM_pu8WriteAddress
                     <= VEEPROM_apucFlashPage[ u8Destination ] + VEEPROM_uiFlashPageSize );
         bRetVal =  VEEPROM_bWriteWithoutCheck( u8ActualDrawer, pu8Data, u8DrawerPayloadLen );
         if (false == bRetVal )
         {
            ASSERT_vEXPR( false != bRetVal );
            break;
         }
      }
      else
      {
         /* do nothing */
         /**
          * ... if data 
          * does not exist, 
          * do nothing
          */
      }
#else
      _VEEPROM_vResetWatchdogTimer();
      /* comment - defrag data using cached values */
      /**
       * ... if cached value is valid, copy this value to new page
       * \note if cached value is invalid, no value has been stored up to now so do nothing.
       */
      if ( CACHE_bIsCacheValidVeeprom( u8ActualDrawer) != 0U )
      {
         pu8CacheAddress =  CACHE_pucGetCacheAddressVEEPROM( u8ActualDrawer );
         u8DrawerPayloadLen = VEEPROM_ucGetDrawerSize( u8ActualDrawer );
         ASSERT_vEXPR ( VEEPROM_pu8WriteAddress < (VEEPROM_apucFlashPage[ u8Destination ] + VEEPROM_uiFlashPageSize) );
         bRetVal = VEEPROM_bWriteWithoutCheck( u8ActualDrawer, pu8CacheAddress, u8DrawerPayloadLen );
         if ( false == bRetVal )
         {
            ASSERT_vEXPR( bRetVal != false);
            break;
         }
      }
      else
      {
         /* do nothing */
         /**
          * \internal
          * if value in cache 
          * not valid, go on  
          * with next drawer.
          */
      }
#endif
      u8ActualDrawer++;
   }
   /* finally write page erase counter of new page and erase old (full) page if everything ok */
   /* finally write updated page erase counter */
   if (false != bRetVal)
   {
      bRetVal = FLASH_bEnableAccess();
      if (false != bRetVal)
      {
         bRetVal = FLASH_bWrite( VEEPROM_apucFlashPage[ u8Destination ], &au8NumberPageTransfers[ 0 ], 2 );
         (void) FLASH_bDisableAccess();
         if ( false == bRetVal )
         {
            ASSERT_vEXPR( bRetVal != false);
         }
      }
      else
      {
         ASSERT_vEXPR( bRetVal != false);
      }
      /* if everything part of defragmenting data successfully  ==> erase full page*/
      _VEEPROM_vResetWatchdogTimer();
      VEEPROM_vBeforePageErase();
      bRetVal = FLASH_bEnableAccess();
      if ( false != bRetVal )
      {
         bRetVal = FLASH_bErase(  VEEPROM_apucFlashPage[ u8Source ] );
         (void) FLASH_bDisableAccess();
         _VEEPROM_vResetWatchdogTimer();
         if ( bRetVal != false )
         {
            VEEPROM_vSetPageEraseMark( u8Source );
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
      }
      else
      {
         ASSERT_vEXPR( bRetVal != false);
      }
   }
   VEEPROM_u8ActivePage = u8Destination;
   return bRetVal;
}
/* VEEPROM_bWriteWithoutCheck() */
/**
 * \brief writing data to VEEPROM without checking if remaining space left in Flash is large enough
 *
 * \pre remaining space in Flash is big enough to hold new data
 * 
 * \param u8Label DrawerID to write to
 * \param pu8Data Buffer containing data
 * \param u8DataLen length of Buffer to write
 */
static bool  VEEPROM_bWriteWithoutCheck     ( uint8_t        u8Label,
                                              const uint8_t* pu8Data,
                                              uint8_t        u8DataLen )
{
   uint8_t* pu8DrawerStartAddress =  VEEPROM_pu8WriteAddress;
   uint8_t  u8Position;
   bool   bRetVal;
   uint8_t  u8Index;
   uint8_t  u8Crc = 0U;
   // to get real payload len including unused cells for cellspace != 0
   uint8_t  u8PayloadLen;
   uint8_t u8ReadOutDrawerId; 
#if defined(DEBUG) && (DEBUG != 0u)
   bool bDbgRetVal;
#endif
   /* comment - enable FLASH-Write and write DrawerIdentifier. Then write all data of payload */
   /**
    * \internal
    * \subsection sub1_3 Write
    * Data to write is new or different from latest valid data stored.
    * First enable Flash operations ...
    * ... set Write-Pointer to Address of Identifier of Drawer and <b>write Identifier</b>
    * ... set Write-Pointer to start of Payload of Drawer and <b>write data</b>
    */
   bRetVal = (bool)FLASH_bEnableAccess();
   if ( false != bRetVal )
   {
      bRetVal = (bool)FLASH_bWriteByte( pu8DrawerStartAddress + VEEPROM_ucOffsetDrawerId, u8Label );
      ASSERT_vEXPR( bRetVal != false);
      (void) FLASH_bDisableAccess();
      /* remaining consistency of data stored in FLASH :*/
      /* If there was an interference while writing drawer ID it can be that the wrong drawer ID was written to FLASH. - If this happens, actually (wrong) written drawer ID is taken as input to get its drawer length to set new valid WriteAddres.  */
      /* - In this case the data to write is not written and the return value of the function is false.  */
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = (bool)FLASH_bRead ( pu8DrawerStartAddress + VEEPROM_ucOffsetDrawerId, (uint8_t*)&u8ReadOutDrawerId, 1 );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8DrawerStartAddress + VEEPROM_ucOffsetDrawerId, &u8ReadOutDrawerId, 1U );
#endif
      u8PayloadLen = VEEPROM_ucObserverGetBruttoPayloadLength( u8ReadOutDrawerId );
      if (u8ReadOutDrawerId == u8Label )
      {
         /* start calculation of crc */
         CRC_vUpdateCrc( &u8Crc, u8Label);//CRC_vUpdateCrc( pucCrc, u8Label);
         VEEPROM_pu8WriteAddress =  &pu8DrawerStartAddress[ VEEPROM_ucOffsetDrawerPayload ];
         bRetVal = (bool)FLASH_bEnableAccess();
         if ( false != bRetVal )
         {
            bRetVal = (bool)FLASH_bWrite(VEEPROM_pu8WriteAddress, pu8Data, u8DataLen);
            ASSERT_vEXPR( bRetVal != false);
            (void)bRetVal; //Previously assigned value to variable 'bRetVal' has not been used [MISRA 2012 Rule 2.2, required]
            (void) FLASH_bDisableAccess();
            /* continue calculation of crc */
            for ( u8Position = 0; u8Position < u8DataLen; u8Position++ )
            {
               CRC_vUpdateCrc( &u8Crc, pu8Data[ u8Position ] );
            }
            /* comment - finally write CRC or validity field */
            /**
             * \internal
             * ... set Write-Pointer to Address of consistency-Field and <b>write consistency-Field</b>
             * ... finally update Write-Pointer and disable Flash operations 
             */
            bRetVal = (bool)FLASH_bEnableAccess();
            if ( false != bRetVal )
            {
               bRetVal = (bool)FLASH_bWriteByte( &pu8DrawerStartAddress[ 0 ], u8Crc );
               (void) FLASH_bDisableAccess();
               ASSERT_vEXPR( bRetVal != false);
            }
            else
            {
               ASSERT_vEXPR( bRetVal != false);
            }
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
      }
      else
      {
         ASSERT_vEXPR( u8ReadOutDrawerId == u8Label);
         bRetVal = false;
      }
      u8Index =  u8PayloadLen + VEEPROM_ucOffsetDrawerPayload;
      VEEPROM_pu8WriteAddress = &pu8DrawerStartAddress[ u8Index ];
   }
   else
   {
      ASSERT_vEXPR( bRetVal != false);
   }
   return bRetVal;
}
/* VEEPROM_bIsPageErased() */
/**
 * \brief checks if desired flash page is completely erased
 *
 * \param u8FlashPageNumber Number of Flash Page
 * \return 
 *          - true: flash page is erased completely.
 *          - false: at least one cell of flash page is not erased.
 */
static bool VEEPROM_bIsPageErased ( uint8_t u8FlashPageNumber )
{
   bool bIsEmpty = true;
   uint8_t* pu8ReadAddress;
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      start of page + length of page == end of page
   */
   uint8_t* pu8EndAddress = VEEPROM_apucFlashPage[ u8FlashPageNumber ] + VEEPROM_uiFlashPageSize;
   /* test memory contents */
   pu8ReadAddress = VEEPROM_apucFlashPage[ u8FlashPageNumber ];
   do
   {
      if ( *pu8ReadAddress != VEEPROM_UNWRITTEN_FLASH_CELL )
      {
         bIsEmpty = false;
      }
      ++pu8ReadAddress;
   }
   while ( (bIsEmpty == true) && ((uint32_t)pu8ReadAddress < (uint32_t)pu8EndAddress) );
   return bIsEmpty;
}
/* VEEPROM_tDoWriteStep() */
/**
 * \brief Does one write step called in VEEPROM_ucHandleTask()
 *
 * \param u8PageNumber Number of Flash Page
 * \return 
 *          - VEEP_PROCESS_FAILURE     : process failure
 *          - VEEP_PROCESS_SUCCESSFUL  : process was successful
 *          - VEEP_PROCESS_ONGOING     : process still ongoing
 */
static eProcessResultType VEEPROM_tDoWriteStep( uint8_t u8PageNumber )
{
   eProcessResultType tRetVal = VEEP_PROCESS_ONGOING;
   uint8_t            u8ReadOutDrawerId             ;
   uint16_t           u16FreeSpace                  ;
   static eWriteStepsType  VEEPROM_tWriteStep = VEEP_WRITE_LABEL;
   static uint8_t           *VEEPROM_pu8DrawerStart               ;
   static uint8_t            VEEPROM_u8PayloadLen                 ;
   static uint8_t            VEEPROM_u8Crc                        ;
   static uint8_t            VEEPROM_u8Position                   ;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bResult;
#endif
   switch (VEEPROM_tWriteStep)
   {
      case VEEP_WRITE_LABEL:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bEnableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_pu8DrawerStart = VEEPROM_pu8WriteAddress;
         bResult = FLASH_bWriteByte(VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, VEEPROM_u8Label);
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_tWriteStep     = VEEP_VERIFY_LABEL;
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         VEEPROM_pu8DrawerStart = VEEPROM_pu8WriteAddress;
         (void)FLASH_bWriteByte(VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, VEEPROM_u8Label);
         (void) FLASH_bDisableAccess();
         VEEPROM_tWriteStep     = VEEP_VERIFY_LABEL;
#endif
         break;
      }
      case VEEP_VERIFY_LABEL:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         bResult = FLASH_bRead ( VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, &u8ReadOutDrawerId, 1U );
         ASSERT_vEXPR ( bResult != false );
#else
         (void) FLASH_bRead ( VEEPROM_pu8DrawerStart + VEEPROM_ucOffsetDrawerId, &u8ReadOutDrawerId, 1U );
#endif
         VEEPROM_u8PayloadLen = VEEPROM_ucObserverGetBruttoPayloadLength( u8ReadOutDrawerId );
         if ( u8ReadOutDrawerId == VEEPROM_u8Label )
         {
            /* start calculation crc*/
            VEEPROM_u8Crc = 0;
            VEEPROM_u8Position = 0;
            CRC_vUpdateCrc( &VEEPROM_u8Crc, VEEPROM_u8Label);
            VEEPROM_tWriteStep = VEEP_WRITE_DATA;
         }
         else
         {
            VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
         }
         break;
      }
      case VEEP_WRITE_DATA:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         if ( VEEPROM_u8Position < VEEPROM_u8DataLen)
         {
            bResult = FLASH_bEnableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            bResult = FLASH_bWrite(&VEEPROM_pu8DrawerStart[ VEEPROM_ucOffsetDrawerPayload + VEEPROM_u8Position], &VEEPROM_au8WriteData[VEEPROM_u8Position], VEEPROM_ucCellSize);
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            (void) FLASH_bDisableAccess();
            if ( (VEEPROM_u8Position + VEEPROM_ucCellSize) < VEEPROM_u8DataLen )
            {
               VEEPROM_u8Position += VEEPROM_ucCellSize;
            }
            else
            {
               VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
            }
         }
         else
         {
            if ( VEEPROM_u8DataLen == 0 )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
         }
#else
         /* normal version */
         if ( VEEPROM_u8Position < VEEPROM_u8DataLen)
         {
            (void)FLASH_bEnableAccess();
            (void)FLASH_bWrite(&VEEPROM_pu8DrawerStart[VEEPROM_ucOffsetDrawerPayload + VEEPROM_u8Position], &VEEPROM_au8WriteData[VEEPROM_u8Position],VEEPROM_ucCellSize);
            (void) FLASH_bDisableAccess();
            if ( (VEEPROM_u8Position + VEEPROM_ucCellSize) < VEEPROM_u8DataLen )
            {
               VEEPROM_u8Position += VEEPROM_ucCellSize;
            }
            else
            {
               VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
            }
         }
         else
         {
            VEEPROM_tWriteStep = VEEP_VERIFY_DATA;
         }
#endif
         break;
      }
      case VEEP_VERIFY_DATA:
      {
         VEEPROM_tWriteStep = VEEP_WRITE_CRC;
         for ( VEEPROM_u8Position = 0; VEEPROM_u8Position < VEEPROM_u8DataLen; VEEPROM_u8Position++ )
         {
            /* continue calculation of crc */
            CRC_vUpdateCrc( &VEEPROM_u8Crc, VEEPROM_au8WriteData[VEEPROM_u8Position] );
            if ( VEEPROM_pu8DrawerStart[ VEEPROM_ucOffsetDrawerPayload + VEEPROM_u8Position] != VEEPROM_au8WriteData[VEEPROM_u8Position] )
            {
               VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
               break;
            }
         }
         break;
      }
      case VEEP_WRITE_CRC:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bEnableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bWriteByte( VEEPROM_pu8DrawerStart, VEEPROM_u8Crc );
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_tWriteStep     = VEEP_VERIFY_CRC;
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         (void)FLASH_bWriteByte( VEEPROM_pu8DrawerStart, VEEPROM_u8Crc );
         (void) FLASH_bDisableAccess();
         VEEPROM_tWriteStep     = VEEP_VERIFY_CRC;
#endif
         break;
      }
      case VEEP_VERIFY_CRC:
      {
         if ( VEEPROM_pu8DrawerStart[0] != VEEPROM_u8Crc )
         {
            VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
         }
         else
         {
            VEEPROM_tWriteStep = VEEP_DRAWER_FINISHED;
         }
         break;
      }
      case VEEP_DRAWER_FINISHED:
      {
         VEEPROM_pu8WriteAddress = &VEEPROM_pu8DrawerStart[ VEEPROM_u8PayloadLen + VEEPROM_ucOffsetDrawerPayload ];
         VEEPROM_tWriteStep = VEEP_WRITE_LABEL;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
         break;
      }
      case VEEP_FIND_NEXT_POSITION:
      {
         /* some data byte not correctly written, */
         /* move pointer to next position and try again */
         u16FreeSpace = (uint16_t)(((uint32_t)VEEPROM_apucFlashPage[ u8PageNumber ] + (uint32_t)VEEPROM_uiFlashPageSize ) - (uint32_t)&VEEPROM_pu8DrawerStart[ VEEPROM_u8PayloadLen + VEEPROM_ucOffsetDrawerPayload ] );
         if ( u16FreeSpace > ((uint16_t)VEEPROM_u8PayloadLen + (uint16_t)VEEPROM_ucOffsetDrawerPayload) )
         {
            /* move pointer to next position and try again */
            /* Debug */
#if defined(DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
#endif
            VEEPROM_pu8WriteAddress = &VEEPROM_pu8DrawerStart[ VEEPROM_u8PayloadLen + VEEPROM_ucOffsetDrawerPayload ];
         }
         else
         {
            /* not enougt memory space on page */
            /* Debug */
#if defined(DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
#endif
            /* read out another drawer ID */
            tRetVal = VEEP_PROCESS_DEFRAG_NEED;
         }
         VEEPROM_tWriteStep = VEEP_WRITE_LABEL;
         break;
      }
      default:
      {
         ;
      }
   }
   return(tRetVal);
}
/* VEEPROM_tDoDefragStep() */
//#define TEST_EXIT_AFTER_READ_ERASE_COUNTER
//#define TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN
//#define TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN
//#define TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN
//#define TEST_EXIT_AFTER_OLD_PAGE_ERASE
#if defined(TEST_EXIT_AFTER_READ_ERASE_COUNTER)  || defined(TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN)  ||     defined(TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN) || defined(TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN) ||     defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
static uint8_t *VEEPROM_pu8WriteAddress_OLD = NULL;
#endif
static eProcessResultType VEEPROM_tDoDefragStep(void)
{
   eProcessResultType tRetVal         = VEEP_PROCESS_ONGOING;
   eProcessResultType tWriteResult                          ;
   const uint8_t     *pu8CacheAddress                       ;
   uint8_t            u8Counter                             ;
#if !defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
   uint8_t           *pu8MagicNumberPosition = NULL         ;
   uint8_t            u8MagicNumber                         ;
#endif
#if defined(DEBUG) && (DEBUG != 0u)
   bool bResult;
#endif
   static eDefragStepsType  VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
   static uint8_t  VEEPROM_u8Source                   ;
   static uint8_t  VEEPROM_u8Destination              ;
   static uint8_t  VEEPROM_u8CurPageNr                ; //holds page number to be erased, source or destination
   static uint8_t  VEEPROM_au8NumberPageTransfers[ 2 ];
   static uint8_t  VEEPROM_u8ActDefragDrawer          ;
   switch (VEEPROM_tDefragStep)
   {
      case VEEP_CHECK_PAGE_EMPTY:
      {
         VEEPROM_u8Source          = VEEPROM_u8ActivePage;
         VEEPROM_u8Destination     = VEEPROM_ucGetNextPage( VEEPROM_u8ActivePage );
         VEEPROM_u8ActDefragDrawer = 0;
#ifdef TIME_MEASUREMENT
         VEEPROM_u16TimeBefore = TIM_uiGetCircleMicroSeconds();
#endif
         if ( VEEPROM_bIsPageErased( VEEPROM_u8Destination ) != false )
         {
#ifdef TIME_MEASUREMENT
            VEEPROM_u16TimeAfter = TIM_uiGetCircleMicroSeconds();
            VEEPROM_u16TimeDifference = VEEPROM_u16TimeAfter - VEEPROM_u16TimeBefore;
#endif
            VEEPROM_tDefragStep = VEEP_READ_ERASE_COUNTER;
         }
         else
         {
#ifdef TIME_MEASUREMENT
            VEEPROM_u16TimeAfter = TIM_uiGetCircleMicroSeconds();
            VEEPROM_u16TimeDifference = VEEPROM_u16TimeAfter - VEEPROM_u16TimeBefore;
#endif
            VEEPROM_u8CurPageNr = VEEPROM_u8Destination;
            VEEPROM_tDefragStep = VEEP_PAGE_ERASE;
         }
         break;
      }
      case VEEP_READ_ERASE_COUNTER:
      {
         VEEPROM_u8CurPageNr = VEEPROM_u8Source;
#if defined(TEST_EXIT_AFTER_READ_ERASE_COUNTER)  || defined(TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN)  ||     defined(TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN) || defined(TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN) ||     defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
         VEEPROM_pu8WriteAddress_OLD = VEEPROM_pu8WriteAddress;
#endif
         VEEPROM_pu8WriteAddress = VEEPROM_apucFlashPage[ VEEPROM_u8Destination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
         /* comment - increment page transfer counter */
         /**
          * \internal
          * \subsection subSf6_2 PageTransferCounter
          *  - read out PageTransferCounter high and low byte
          *  - increment PageTransferCounter: check if high byte has to be incremented too
          *  - write incremented PageTransferCounter
          */
         /* read out page erase counter of full page and increment it */
#if defined(DEBUG) && (DEBUG != 0u)
         bResult = FLASH_bRead( VEEPROM_apucFlashPage[ VEEPROM_u8Source ], &VEEPROM_au8NumberPageTransfers[ 0 ], 2U);
         ASSERT_vEXPR( false != bResult );
#else
         (void) FLASH_bRead( VEEPROM_apucFlashPage[ VEEPROM_u8Source ], &VEEPROM_au8NumberPageTransfers[ 0 ], 2U);
#endif
         if ( VEEPROM_au8NumberPageTransfers[ 1 ] == 0xFFU )
         {
            VEEPROM_au8NumberPageTransfers[ 0 ]++;
            if ( VEEPROM_au8NumberPageTransfers[ 0 ] == 0x00U )
            {
               /**
                * for checking reset that occured while process of erasing page is progress, 
                * we check first two bytes of page erase counter.
                *
                * if these two bytes are 0x0000, and there are still two pages containing data, we take the following assumptions>
                * - it was necessary to defragment active page, because data to write was larger than remaining space on active page
                * - transfering data to transfer-page has succeeded, though process of erasing active (old) page was started.
                * - process of erasing page did not finnish due to reset ( if it had it would have erased all cells with value 0xFF
                *
                * ==> as a result of this pageTransferCounter with value 0x0000 is not allowed, so we increment twice and count in the
                *     following way: 0xFFFF, 0x0001, 0x0002, 0x0003, ....
                */
               VEEPROM_au8NumberPageTransfers[ 1 ]++;
            }
         }
         VEEPROM_au8NumberPageTransfers[ 1 ]++;
#if !defined(TEST_EXIT_AFTER_READ_ERASE_COUNTER)
         VEEPROM_tDefragStep = VEEP_DRAWER_WRITE_START;
#else
         /* only for test  */
         VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         break;
      }
      case VEEP_DRAWER_WRITE_START:
      {
         /* comment - defrag data using cached values */
         /**
          * ... if cached value is valid, copy this value to new page
          * \note if cached value is invalid, no value has been stored up to now so do nothing.
          */
         while ( VEEPROM_u8ActDefragDrawer < VEEPROM_ucGetNumberOfDrawers() )
         {
            if ( CACHE_bIsCacheValidVeeprom( VEEPROM_u8ActDefragDrawer) != 0U )
            {
               pu8CacheAddress =  CACHE_pucGetCacheAddressVEEPROM( VEEPROM_u8ActDefragDrawer );
               VEEPROM_u8DataLen   = VEEPROM_ucGetDrawerSize( VEEPROM_u8ActDefragDrawer );
               VEEPROM_u8Label     = VEEPROM_u8ActDefragDrawer  ;
               /* copy data to its own field */
               for (u8Counter = 0; u8Counter < VEEPROM_u8DataLen; u8Counter++)
               {
                  VEEPROM_au8WriteData[u8Counter] = pu8CacheAddress[u8Counter];
               }
               /* fill rest of field with 0xFF => there must be empty info */
               for (u8Counter = VEEPROM_u8DataLen; u8Counter < VEEPROM_MAX_DATA_LEN; u8Counter++)
               {
                  VEEPROM_au8WriteData[u8Counter] = VEEPROM_UNWRITTEN_FLASH_CELL;
               }
               VEEPROM_tDefragStep = VEEP_DRAWER_WRITE_ONGOING;
               VEEPROM_u8ActDefragDrawer++; //set next drawer for future
               break; //from while cyclus
            }
            else
            {
               VEEPROM_u8ActDefragDrawer++;
            }
         }
         if (VEEPROM_tDefragStep == VEEP_DRAWER_WRITE_START )
         {
            /* nothing to write */
#if !defined(TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN)
            VEEPROM_tDefragStep = VEEP_WRITE_ERASE_COUNTER;
#else
            /* only for test  */
            VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         }
         break;
      }
      case VEEP_DRAWER_WRITE_ONGOING:
      {
         tWriteResult = VEEPROM_tDoWriteStep(VEEPROM_u8Destination);
         if ( tWriteResult == VEEP_PROCESS_SUCCESSFUL )
         {
#if !defined(TEST_EXIT_AFTER_FIRST_DRAWER_WRITTEN)
            VEEPROM_tDefragStep = VEEP_DRAWER_WRITE_START;
#else
            /* only for test  */
            VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         }
         else
         {
            if ( (tWriteResult == VEEP_PROCESS_FAILURE) || (tWriteResult == VEEP_PROCESS_DEFRAG_NEED) )
            {
               /* Debug */
#if defined(DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
#endif
               VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
               tRetVal = VEEP_PROCESS_FAILURE;
            }
            else
            {
               /* process ongoing */
            }
         }
         break;
      }
      case VEEP_WRITE_ERASE_COUNTER:
      {
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bEnableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bWrite(VEEPROM_apucFlashPage[ VEEPROM_u8Destination ], (const uint8_t *) &VEEPROM_au8NumberPageTransfers[ 0 ], 2);
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_tDefragStep     = VEEP_PAGE_ERASE;
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         (void)FLASH_bWrite(VEEPROM_apucFlashPage[ VEEPROM_u8Destination ], &VEEPROM_au8NumberPageTransfers[ 0 ], 2U);
         VEEPROM_tDefragStep     = VEEP_PAGE_ERASE;

#endif
         break;
      }
      case VEEP_PAGE_ERASE:
      {
#if !defined(TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN)
         VEEPROM_vBeforePageErase();
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         bResult = FLASH_bEnableAccess();
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         bResult = FLASH_bErase(VEEPROM_apucFlashPage[ VEEPROM_u8CurPageNr ]);
         if ( bResult == false )
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
         VEEPROM_tDefragStep     = VEEP_WRITE_ERASE_MARK;
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         (void)FLASH_bErase(VEEPROM_apucFlashPage[ VEEPROM_u8CurPageNr ]);
         VEEPROM_tDefragStep     = VEEP_WRITE_ERASE_MARK;
#endif
#else
         /* only for test  */
         VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         break;
      }
      case VEEP_WRITE_ERASE_MARK:
      {
#if !defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
         u8MagicNumber = 0xA5;
         VEEPROM_vSetEraseMarkData( VEEPROM_u8CurPageNr, &u8MagicNumber,&pu8MagicNumberPosition);
#if defined(DEBUG) && (DEBUG != 0u)
         /* debug version */
         if ( pu8MagicNumberPosition != NULL )
         {
            ASSERT_vEXPR( false != VEEPROM_bIsPageErased( VEEPROM_u8CurPageNr ) );
            bResult = FLASH_bEnableAccess();
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            bResult = FLASH_bWrite(pu8MagicNumberPosition, (const uint8_t *) &u8MagicNumber, (uint16_t)sizeof(u8MagicNumber));
            if ( bResult == false )
            {
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
            if ( VEEPROM_u8CurPageNr == VEEPROM_u8Source )
            {
               VEEPROM_tDefragStep     = VEEP_DEFRAG_FINISHED;
            }
            else
            {
               VEEPROM_tDefragStep     = VEEP_READ_ERASE_COUNTER;

            }
         }
         else
         {
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            /* user access disabled or setup CPU problem */
            tRetVal = VEEP_PROCESS_FAILURE;
         }
#else
         /* normal version */
         (void)FLASH_bEnableAccess();
         (void)FLASH_bWrite(pu8MagicNumberPosition, &u8MagicNumber, (uint16_t)sizeof(u8MagicNumber));
         if ( VEEPROM_u8CurPageNr == VEEPROM_u8Source )
         {
            VEEPROM_tDefragStep     = VEEP_DEFRAG_FINISHED;
         }
         else
         {
            VEEPROM_tDefragStep     = VEEP_READ_ERASE_COUNTER;

         }
#endif
         VEEPROM_vDefragEnded();
#else
         /* only for test  */
         VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
         break;
      }
      case VEEP_DEFRAG_FINISHED:
      {
         VEEPROM_u8ActivePage = VEEPROM_u8Destination;
         VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
         tRetVal = VEEP_PROCESS_SUCCESSFUL;
         break;
      }
      default:
      {
         ;
      }
   }
   return(tRetVal);
}
#endif
#if defined(DEBUG) && (DEBUG != 0u)
/* VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified() */
/** \brief under construction */
static void VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bool bValue)
{

}
#endif
/* Global Functions */
/* VEEPROM_bDefragStart() */
bool VEEPROM_bDefragStart(void)
{
   bool bRetVal = false;
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
      bRetVal = true;
   }
   else
   {
      /* Debug */
      /* previous process not finished */
#if defined(DEBUG) && (DEBUG != 0u)
      _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
#endif
   }
   return(bRetVal);
}
/* VEEPROM_tGetStatus() */
eVEEPROMtaskStateType VEEPROM_tGetStatus(void)
{
   return(VEEPROM_tTaskState);
}
#if defined(VEEPROM_LARGE)
/* VEEPROM_bInit() */
bool  VEEPROM_bInit                         ( void )
{
   //! \callgraph
   uint8_t u8NumberOfNonEmptyUnits = 0U;
   uint8_t u8OtherUnit            = 0U;
   uint8_t u8InterruptedUnit;
   uint8_t u8CheckedUnit;
   uint8_t u8PageCounter;
   uint16_t u16UsedSpace_Active;
   uint16_t u16UsedSpace_Other;
   bool  bRetVal;
   VEEPROM_tTaskState = VEEP_UNINITIALIZED;
   /* check for interrupted page-transfer */
   /** 
    * \internal
    * \section sec2 Outline of function
    * 
    * \subsection sub2_1 Are flash pages o.k.?
    * First <b>check out</b> how many <b>pages</b> are currently <b>in use</b>: 
    * If first possible position for a cabinet has a entry in the Drawer-Identifier-Field, then
    * this page is in use. Its location is evaluated in pucAddressFirstDrawer.
    */
   for ( u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_ucNumberOfUnits; u8CheckedUnit++ )
   {
      //DrawerIdentification is written at first. if first DrawerIdentification is still unwritten, page is unwritten.
      /*
         Msg(4:0488) Performing pointer arithmetic. 
         MISRA-C:2004 Rule 17.4
         Pointer arithmetic is checked.
         (a) VEEPROM_apucFlashPage contains start address of checked page
         (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
         (c) VEEPROM_ucOffsetDrawerId is offset of drawer-ID
         thus (a) + (b) + (c) is address of first drawer-ID of page being checked.
      */
      /* pucAddressFirstDrawerId =   VEEPROM_apucFlashPage[ ucCheckedPage ]
                                + VEEPROM_PAGE_TRANSFER_COUNTER_LEN
                                + VEEPROM_ucOffsetDrawerId; */
      if ( (VEEPROM_bIsUnitErased (u8CheckedUnit) == false) && (VEEPROM_bUnitDataValid(u8CheckedUnit) != false) )
      {
         u8NumberOfNonEmptyUnits++;
         u8OtherUnit = VEEPROM_u8ActiveUnit;
         VEEPROM_u8ActiveUnit = u8CheckedUnit;
      }
   }
   switch ( u8NumberOfNonEmptyUnits )
   {
      /**
       * \callgraph
       *
       * \internal
       * \subsection sub2_2 First Run
       *
       * Only emty pages exist.
       * <b>If no page has been written yet</b>, select FLASH-page 0 to be active page.
       * And set write-pointer to start of page.
       *
       * Ensure for all flash pages that they are erased. Therefor a magic number is checked
       * that is set when page is erased. If this magic number is not set, page erase did not 
       * succeed without interruption.
       */
      case 0:
      {
         VEEPROM_u8ActiveUnit = VEEPROM_DEFAULT_UNIT;
         for (u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_ucNumberOfUnits; ++u8CheckedUnit)
         {
            VEEPROM_vEnsureUnitIsErased( u8CheckedUnit );
         }
         /*
            Msg(4:0488) Performing pointer arithmetic. 
            MISRA-C:2004 Rule 17.4
            Pointer arithmetic is checked.
            (a) VEEPROM_apucFlashPage contains start address of default page
            (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
            thus (a) + (b) is address of first drawer of default page
         */
         VEEPROM_pu8WriteAddress = VEEPROM_apucUnitStart[ VEEPROM_DEFAULT_UNIT ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
         VEEPROM_tTaskState = VEEP_READY;
         bRetVal = true;
         break;
      }
      /**
       * \internal
       * \subsection sub2_3 No reset while defragmenting pages
       *
       * exact one non-empty page
       * <b>If exactly one page contains data</b>, set this FLASH-page to be active.
       * And refresh the RamCache - Values.
       * ... Set write-pointer to position after end of last drawer.
       *
       * Ensure for all flash pages except active page that they are erased. Therefor a magic 
       * number is checked that is set when page is erased. If this magic number is not set, 
       * page erase did not succeed without interruption.
       */
      case 1:
      {
         VEEPROM_pu8WriteAddress = VEEPROM_pu8ResetWriteAddress( VEEPROM_u8ActiveUnit );
         for (u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_ucNumberOfUnits; ++u8CheckedUnit)
         {
            if ( u8CheckedUnit != VEEPROM_u8ActiveUnit )
            {
               VEEPROM_vEnsureUnitIsErased( u8CheckedUnit );
            }
         }
         VEEPROM_tTaskState = VEEP_READY;
         bRetVal = true;
         break;
      }
      /**
       * \internal
       * \subsection sub2_4 Reset while defragmenting pages
       *
       * Two non-empty pages: There must have been a reset while defragmenting data.
       *
       * <b>If prior Defragmentation has been interrupted </b>, 
       * First erase page and then defrag again.  Check if to copy from page 1 to 0, or from 0 to 1.
       * At end of defragmenting, WritePointer shows to next free position.
       *
       * Refresh the RamCache - Values.
       * 
       * \todo check for interrupted page erase with writing 
       */
      case 2:
      {
         /* set smaller page as interrupted page */
         /*
            Msg(4:0488) Performing pointer arithmetic. 
            MISRA-C:2004 Rule 17.4
            Pointer arithmetic is checked.
            VEEPROM_u16GetUsedSpace (..) returns used space of given page
         */
         u16UsedSpace_Active = VEEPROM_u16GetUsedSpace( VEEPROM_u8ActiveUnit );
         u16UsedSpace_Other = VEEPROM_u16GetUsedSpace( u8OtherUnit );
         if ( u16UsedSpace_Active > u16UsedSpace_Other )
         {
            u8InterruptedUnit = u8OtherUnit;
         }
         else
         {
            u8InterruptedUnit = VEEPROM_u8ActiveUnit;
            VEEPROM_u8ActiveUnit = u8OtherUnit;
         }
         /* check larger page for interrupted page erase (00 00 00 00 00 00 .. .. fe 1f 2a .. .. .. ff ff) */
         /* if larger page has interrupted page transfer */
         /* -  do not erase smaller page, that is actually set as interrupted page. */
         /* -  erase larger page */
         /* -  set smaller page as active page. */
         if ( VEEPROM_bCheckForInterruptedUnitErase( VEEPROM_u8ActiveUnit ) != false )
         {
            /* page erase of full page was interrupted due to reset. In this case the smaller page */
            /* contains already all latest data, because erase of full page is first started when transfer of latest data has been finished. */
            /* erase fuller page, that no longer contains valid data. */
            /* swap active and interrupted pages. */
            u8OtherUnit = u8InterruptedUnit;
            u8InterruptedUnit = VEEPROM_u8ActiveUnit;
            VEEPROM_u8ActiveUnit = u8OtherUnit;
         }
         else
         {
            /* there was no interrupted unit erase. In this case a reset occured while transfering data from active(full) page to transfer-page. */
            /* In this case the smaller page probably does not contain all latest data, but the fuller page does. */
            if ( VEEPROM_bCheckForDefragmentedAndNotErasedUnit( VEEPROM_u8ActiveUnit, u8InterruptedUnit ) != false)
            {
               /* In this case full page was defragmented, new valid drawer values are in the new page, but erase process of full page did not start yet. If this happened, begin of new page contains page erase counter incremented by 1 compared with page erase counter of previous page */
               /* erase fuller page. */
               /* swap active and interrupted pages. */
               u8OtherUnit = u8InterruptedUnit;
               u8InterruptedUnit = VEEPROM_u8ActiveUnit;
               VEEPROM_u8ActiveUnit = u8OtherUnit;
            }
            else
            {
               /* erase smaller page, that is already set as interrupted page. */
            }
         }
         // delete interrupted page
         bRetVal = FLASH_bEnableAccess();
         if ( false != bRetVal )
         {
            //erase page has not been finished before. Do erase all pages in unit once more
            for (u8PageCounter = 0; u8PageCounter < VEEPROM_ucFlashPagesPerUnit; u8PageCounter++)
            {
               bRetVal = FLASH_bErase(  VEEPROM_apucUnitStart[ u8InterruptedUnit ] + (uint16_t)(u8PageCounter * VEEPROM_uiTotalPageSize) );
               ASSERT_vEXPR( bRetVal != false);
               _VEEPROM_vResetWatchdogTimer();
            }
            (void) FLASH_bDisableAccess();
            VEEPROM_vSetUnitEraseMark( u8InterruptedUnit );
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
         VEEPROM_pu8WriteAddress = VEEPROM_pu8ResetWriteAddress( VEEPROM_u8ActiveUnit );
         for (u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_ucNumberOfUnits; ++u8CheckedUnit)
         {
            if ( u8CheckedUnit != VEEPROM_u8ActiveUnit )
            {
               VEEPROM_vEnsureUnitIsErased( u8CheckedUnit );
            }
         }
         if ( bRetVal != false )
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         break;
      }
      default:
      {
         // if there is more than one non-free page, something must have gone wrong before - this should never happen.
         ASSERT_vEXPR (-1);
         bRetVal = false;
      }
   }
   return bRetVal;
}
/* VEEPROM_bDoDefragNow() */
bool VEEPROM_bDoDefragNow( void )
{
   bool bRetVal;
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      bRetVal = VEEPROM_bDefrag ( (uint8_t)VEEPROM_u8ActiveUnit, VEEPROM_u8GetNextUnit( VEEPROM_u8ActiveUnit ) );
      ASSERT_vEXPR( bRetVal==true );
      VEEPROM_vDefragEnded();
      return bRetVal;
   }
   else
   {
      return false;
   }
}
/* VEEPROM_bWrite() */
bool  VEEPROM_bWrite                        ( tDrawerID           tLabel,
                                              const uint8_t       *pu8Data,
                                              uint8_t              u8DataLen )
{
   bool   bRetVal;
   if ( VEEPROM_tTaskState != VEEP_READY )
   {
      return false;
   }
   ASSERT_vEXPR( tLabel < VEEPROM_tGetNumberOfDrawers() );
   /* some assertions */
   ASSERT_vEXPR( u8DataLen == VEEPROM_ucGetDrawerSize( tLabel ) );
   ASSERT_vEXPR( u8DataLen <= VEEPROM_MAX_DATA_LEN );
   ASSERT_vEXPR( u8DataLen > 0 );
   /* check if value to write is different from latest one stored in FLASH. */
   /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do write new data if it is different from latest one stored. */
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
   uint8_t au8Buffer[VEEPROM_MAX_DATA_LEN];
   uint8_t u8Index;
   bool bDifferentData = false;
   ASSERT_vEXPR (VEEPROM_MAX_DATA_LEN >= u8DataLen);
   VEEPROM_bRead( tLabel, au8Buffer, u8DataLen);
   for (u8Index = 0; u8Index < u8DataLen; u8Index++)
   {
      if (au8Buffer[u8Index] != pu8Data[u8Index])
      {
         bDifferentData == true;
         break;
      }
   }
   /* - if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do if data to write is different from latest one stored. */
   if (false != bDifferentData)
     {
#endif
   /* comment - check if we have to defrag first */
   /**
    * \internal
    * \subsection sub1_2 Check flash space
    * If too less space is left for writing new data, ...
    * ... defragmenting before must be done.
    * There shall never happen any errors 
    * while defragmenting data. 
    * But if there are some errors, return with error. 
    */
#ifdef VEEPROM_CELLSIZE_ONE_BYTE
   if ( VEEPROM_uiGetFreeSpace() < ((uint16_t)u8DataLen + (uint16_t)VEEPROM_ucOffsetDrawerPayload) )
#else
   ASSERT_vEXPR ( VEEPROM_ucCellSize > 0U );
   if ( VEEPROM_uiGetFreeSpace() < (((((uint16_t)u8DataLen + ((uint16_t)VEEPROM_ucCellSize - 1U) )/(uint16_t)VEEPROM_ucCellSize ) * (uint16_t)VEEPROM_ucCellSize ) + (uint16_t)VEEPROM_ucOffsetDrawerPayload))
#endif

   {
      /**
       * only start process of defragmentation if allowed from application
       */
      if ( false != VEEPROM_bApprobateDefrag() )
      {
         bRetVal = VEEPROM_bDoDefragNow();
         /* if cached values are not used for defragmentation, we have to write new values after process of defragmentation has finished. */
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
         if ( false != bRetVal )
         {
            bRetVal = VEEPROM_bWriteWithoutCheck (u8Label, pu8Data, u8DataLen);
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
#endif
      }
      else
      {
         bRetVal = false;
         ASSERT_vEXPR( bRetVal != false);
      }
   }
   else
   {
      bRetVal = VEEPROM_bWriteWithoutCheck (tLabel, pu8Data, u8DataLen);
      ASSERT_vEXPR( bRetVal != false);
   }
   return bRetVal;
   /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, do not write data if data to write equals latest one stored. */
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
     }
   /* otherwise return true (data is already up to date) */
   else
     {
       return true;
     }
#endif
}
/* VEEPROM_ucRead() */
uint8_t VEEPROM_ucRead                        ( tDrawerID    tLabel,
                                              uint8_t       *pu8Data,
                                              uint8_t        u8DataLen )
{
   uint8_t        u8Crc  = 0U    ;
   uint8_t       *pucCrc = &u8Crc;
   uint8_t        u8Index        ;
   bool         bEmptyAreaFound;
   uint8_t     *pu8CheckedDrawerStartAddress;
   uint8_t     *pu8CurrentAddress           ;
   uint8_t     *pu8FoundPayload = NULL      ;
   tDrawerID    tReadOutDrawerId            ;
   uint8_t      u8ReadOutCrc                ;
   /**
    * \internal
    * \section secSf5 Outline of function 
    * \subsection subSf5_1 Preparation    
    * retrieve start address where to search for the desired Drawer
    */
   uint16_t u16TempOffset = 0U;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bDbgRetVal;
#endif
   if ( VEEPROM_tTaskState != VEEP_READY )
   {
      return 0;
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start address of active page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is position of first drawer on active page.
   */
   pu8CheckedDrawerStartAddress = VEEPROM_apucUnitStart[ VEEPROM_u8ActiveUnit ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
   (void)FLASH_bEnableAccess();
   do
   {
      /**
       * read out CRC-field
       */
      pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ u16TempOffset ];
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, &u8ReadOutCrc, 1U );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8CurrentAddress, &u8ReadOutCrc, 1U );
#endif
      /**
       * read out drawerID field
       */
      pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerId + u16TempOffset ];
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
#endif
      /** 
       * if not at end of written segment of flash-page
       */
      if ( (tReadOutDrawerId != VEEPROM_tInvalidDrawer) || (VEEPROM_bIsUnitRemainderEmpty(VEEPROM_u8ActiveUnit, pu8CurrentAddress) == false) )
      {
         bEmptyAreaFound = false;
         /**
          * if we have the correct drawer ID
          */
         if ( tLabel == tReadOutDrawerId )
         {
            /**
             * read out payload of drawer
             */
            ASSERT_vEXPR (  VEEPROM_ucGetDrawerSize( tReadOutDrawerId ) == u8DataLen );
#if defined(DEBUG) && (DEBUG != 0u)
            bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ], pu8Data, u8DataLen );
            ASSERT_vEXPR ( false != bDbgRetVal );
            VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
            (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ], pu8Data, u8DataLen );
#endif
            /**
             * calc crc over drawerID and payload
             */
            {
               u8Crc = 0U;
               CRC_vUpdateCrc( pucCrc, HSUP_ucGetHighByte(tLabel));
               CRC_vUpdateCrc( pucCrc, HSUP_ucGetLowByte(tLabel));
               for (u8Index = 0 ; u8Index < u8DataLen; ++u8Index ) 
               {
                  CRC_vUpdateCrc( pucCrc, pu8Data[ u8Index ] );
               }
            }
            /**
             * if calculated crc matches stored one, we have found a valid and correct drawer
             */
            if ( u8Crc == u8ReadOutCrc )
            {
               pu8FoundPayload = pu8CheckedDrawerStartAddress + u16TempOffset;
            }
         }
         // go on with next drawer ...
         u16TempOffset +=  (uint16_t)VEEPROM_ucOffsetDrawerPayload + (uint16_t)VEEPROM_ucObserverGetBruttoPayloadLength(tReadOutDrawerId);
      }
      else
      {
         bEmptyAreaFound = true;
      }
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      try to run not over end of active page:
      (a) pu8CheckedDrawerStartAddress is address to check for next iteration
      (b) VEEPROM_apucFlashPage contains start address of active page
      (c) VEEPROM_uiFlashPageSize is size of flash-page
      thus (a) - (b) must be smaller than (c) if regarded address is in range of active page.
   */
   while (   ( bEmptyAreaFound == false )
          && (((u16TempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1U))   <  VEEPROM_uiUnitSize )
         );
   if ( pu8FoundPayload != NULL)
   {
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead  ( &pu8FoundPayload[ VEEPROM_ucOffsetDrawerPayload ], pu8Data, u8DataLen );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead  ( &pu8FoundPayload[ VEEPROM_ucOffsetDrawerPayload ], pu8Data, u8DataLen );
#endif
   }
   else
   {
      u8DataLen = 0U;
   }
   (void)FLASH_bDisableAccess();
   return u8DataLen;
}
/* VEEPROM_uiGetFreeSpace() */
uint16_t VEEPROM_uiGetFreeSpace( void )
{
    uint16_t u16FreeSpace;
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      /*
         Msg(4:0488) Performing pointer arithmetic. 
         MISRA-C:2004 Rule 17.4 <next> 
         Pointer arithmetic is checked.
         (a) VEEPROM_apucFlashPage contains start address of active page
         (b) VEEPROM_uiFlashPageSize is length of page
         (c) VEEPROM_pu8WriteAddress is write address on active page
         thus (a) + (b) - (c) contains free space in bytes left on active page
      */
       u16FreeSpace = (uint16_t)(((uint32_t)VEEPROM_apucUnitStart[ VEEPROM_u8ActiveUnit ] + VEEPROM_uiUnitSize ) - (uint32_t)VEEPROM_pu8WriteAddress );
      /* if page is full,  VEEPROM_pu8WriteAddress is set after active page in use. This results in a "negative" pucFreeSpace. But because of unsigned variables, we have to check, if it out of range. In this case we do not have any space left and return "0". */
      if (u16FreeSpace > VEEPROM_uiUnitSize)
      {
         return 0;
      }
      else
      {
         return u16FreeSpace;
      }
   }
   else
   {
      return 0;
   }
}
/* VEEPROM_bReadWholeActivePage() */
bool VEEPROM_bReadWholeActivePage(void)
{
   uint8_t        u8Crc  = 0     ;
   uint8_t       *pucCrc = &u8Crc;
   uint8_t        u8Index        ;
   bool           bEmptyAreaFound;
   bool           bRetVal = true ;
   uint8_t       *pu8CheckedDrawerStartAddress;
   uint8_t       *pu8CurrentAddress;
   tDrawerID      tReadOutDrawerId;
   uint8_t        u8ReadOutCrc;
   uint8_t        u8DrawerLength;
   uint8_t       *pucDrawerCacheAddress;
   uint8_t       *pu8EmptyAreaBase = NULL;
   uint8_t au8Buffer[VEEPROM_MAX_DATA_LEN];
   /**
    * \internal
    * \section secSf5 Outline of function 
    * \subsection subSf5_1 Preparation    
    * retrieve start address where to search for the desired Drawer
    */
   uint16_t u16TempOffset = 0;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bDbgRetVal;
#endif
   if ( VEEPROM_tTaskState != VEEP_READY )
   {
#if defined(DEBUG) && (DEBUG != 0u)
      /* assert */
      /* VEEPROM_tTaskState is not ready */
      _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);

#endif
      return false;
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start address of active page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is position of first drawer on active page.
   */
   pu8CheckedDrawerStartAddress = VEEPROM_apucUnitStart[ VEEPROM_u8ActiveUnit ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
   do
   {
      /*
       * read out drawerID field
       */
      pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ u16TempOffset + (uint16_t)VEEPROM_ucOffsetDrawerId ];
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
#endif
      /* 
       * if not at end of written segment of flash-page
       */
      if ( tReadOutDrawerId != VEEPROM_tInvalidDrawer )
      {
         bEmptyAreaFound = false;
      }
      else
      {
         if ( pu8EmptyAreaBase == NULL )
         {
            pu8EmptyAreaBase = VEEPROM_pucGetFreeAreaBase(VEEPROM_u8ActiveUnit, pu8CurrentAddress);
         }
         if ( (uint32_t)pu8CurrentAddress < (uint32_t)pu8EmptyAreaBase )
         {
            bEmptyAreaFound = false;
         }
         else
         {
            bEmptyAreaFound = true;
         }
      }
      if ( bEmptyAreaFound == false )
      {
         if ( tReadOutDrawerId < VEEPROM_tGetNumberOfDrawers() )
         {
            /*
             * read out payload of drawer
             */
            u8DrawerLength = VEEPROM_ucGetDrawerSize( tReadOutDrawerId );
            if ( u8DrawerLength > 0U )
            {
               /*
                * read out CRC-field
                */
#if defined(DEBUG) && (DEBUG != 0u)
               bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset ], &u8ReadOutCrc, 1 );
               ASSERT_vEXPR ( false != bDbgRetVal );
               VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
               (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset ], &u8ReadOutCrc, 1 );
#endif
#if defined(DEBUG) && (DEBUG != 0u)
               bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ], &au8Buffer[0], u8DrawerLength );
               ASSERT_vEXPR ( false != bDbgRetVal );
               VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
               (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ],&au8Buffer[0], u8DrawerLength );
#endif
               /*
                * calc crc over drawerID and payload
                */
               {
                  u8Crc = 0;
                  CRC_vUpdateCrc( pucCrc, HSUP_ucGetHighByte(tReadOutDrawerId));
                  CRC_vUpdateCrc( pucCrc, HSUP_ucGetLowByte(tReadOutDrawerId));
                  for (u8Index = 0 ; u8Index < u8DrawerLength; ++u8Index ) 
                  {
                     CRC_vUpdateCrc( pucCrc, au8Buffer[ u8Index ] );
                  }
               }
               /*
                * if calculated crc matches stored one, we have found a valid and correct drawer
                */
               if ( u8Crc == u8ReadOutCrc )
               {
                  pucDrawerCacheAddress = VEEPROM_pucGetDrawerAddress( tReadOutDrawerId );
                  if ( pucDrawerCacheAddress != NULL )
                  {
                     for (u8Index = 0 ; u8Index < u8DrawerLength; ++u8Index ) 
                     {
                        if ( pucDrawerCacheAddress[u8Index] != au8Buffer[u8Index] )
                        {
                           pucDrawerCacheAddress[u8Index] = au8Buffer[u8Index];
                        }
                     }
                     VEEPROM_vAfterCacheVariableSet( tReadOutDrawerId);
                  }
                  else
                  {
                     /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
                     ASSERT_vEXPR ( pucDrawerCacheAddress != NULL );
#endif
                     bRetVal = false;
                  }
               }
               else
               {
                  /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
                  ASSERT_vEXPR ( u8Crc == u8ReadOutCrc );
#endif
                  bRetVal = false;
               }
            }
            else
            {
               /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
               ASSERT_vEXPR ( u8DrawerLength > 0 );
#endif
               bRetVal = false;
            }
         }
         else
         {
            /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
            ASSERT_vEXPR ( tReadOutDrawerId < VEEPROM_tGetNumberOfDrawers() );
#endif
            bRetVal = false;
         }
         // go on with next drawer ...
         u16TempOffset +=  ((uint16_t)VEEPROM_ucOffsetDrawerPayload + (uint16_t)VEEPROM_ucObserverGetBruttoPayloadLength(tReadOutDrawerId));
      }
   }
   while (   ( bEmptyAreaFound == false )
          && (((u16TempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1U))  <=  VEEPROM_uiUnitSize )
         );
   return bRetVal;
}
/* VEEPROM_bWriteStart() */
bool  VEEPROM_bWriteStart( uint16 tLabel, const uint8_t *pu8Data, uint8_t u8DataLen )
{
   bool bRetVal = false;
   uint8_t u8Counter;
#if defined(DEBUG) && (DEBUG != 0u)
   /* debug version */
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      if ( tLabel < VEEPROM_tGetNumberOfDrawers() )
      {
         if ( u8DataLen == VEEPROM_ucGetDrawerSize(tLabel) )
         {
            if ( u8DataLen <= VEEPROM_MAX_DATA_LEN )
            {
               if ( u8DataLen > 0U )
               {
                  if ( VEEPROM_uiGetFreeSpace() < (((( u8DataLen + (VEEPROM_ucCellSize - 1U)) / VEEPROM_ucCellSize ) * VEEPROM_ucCellSize ) + VEEPROM_ucOffsetDrawerPayload) )
                  {
                     if ( VEEPROM_bApprobateDefrag() != false )
                     {
                        VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
                        bRetVal = true;
                     }
                     else
                     {
                        /* assert */
                        /* defragmentation not alowed */
                        _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
                     }
                  }
                  else
                  {
                     /* copy data to its own field */
                     for (u8Counter = 0; u8Counter < u8DataLen; u8Counter++)
                     {
                        VEEPROM_au8WriteData[u8Counter] = pu8Data[u8Counter];
                     }
                     /* fill rest of field with 0xFF => there must be empty info */
                     for (u8Counter = u8DataLen; u8Counter < VEEPROM_MAX_DATA_LEN; u8Counter++)
                     {
                        VEEPROM_au8WriteData[u8Counter] = VEEPROM_UNWRITTEN_FLASH_CELL;
                     }
                     VEEPROM_u8DataLen   = u8DataLen;
                     VEEPROM_tLabel     = tLabel  ;
                     VEEPROM_tTaskState = VEEP_WRITING;
                     bRetVal = true;
                  }
               }
               else
               {
                  /* assert */
                  /* u8DataLen is 0 */
                  _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
               }
            }
            else
            {
               /* assert */
               /* u8DataLen is too long */
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
         }
         else
         {
            /* assert */
            /* u8DataLen not valid */
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
      }
      else
      {
         /* assert */
         /* u8Label not valid */
         _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
      }
   }
   else
   {
      /* assert */
      /* previous process not finished */
      _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
   }
#else
   /* normal version */
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      if ( VEEPROM_uiGetFreeSpace() < (((( (uint16_t)u8DataLen + ((uint16_t)VEEPROM_ucCellSize - 1U)) / (uint16_t)VEEPROM_ucCellSize ) * (uint16_t)VEEPROM_ucCellSize ) + (uint16_t)VEEPROM_ucOffsetDrawerPayload) )
      {
         if ( VEEPROM_bApprobateDefrag() != false )
         {
            VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
            bRetVal = true;
         }
         else
         {
            /* defragmentation not alowed */
         }
      }
      else
      {
         /* copy data to its own field */
         for (u8Counter = 0; u8Counter < u8DataLen; u8Counter++)
         {
            VEEPROM_au8WriteData[u8Counter] = pu8Data[u8Counter];
         }
         /* fill rest of field with 0xFF => there must be empty info */
         for (u8Counter = u8DataLen; u8Counter < VEEPROM_MAX_DATA_LEN; u8Counter++)
         {
            VEEPROM_au8WriteData[u8Counter] = VEEPROM_UNWRITTEN_FLASH_CELL;
         }
         VEEPROM_u8DataLen   = u8DataLen;
         VEEPROM_tLabel      = tLabel  ;
         VEEPROM_tTaskState  = VEEP_WRITING;
         bRetVal = true;
      }
   }
   else
   {
      /* previous process not finished */
   }
#endif
   return(bRetVal);
}
/* VEEPROM_ucHandleTask() */
uint8_t VEEPROM_ucHandleTask(void)
{
   eProcessResultType tResult;
   switch (VEEPROM_tTaskState)
   {
      case VEEP_UNINITIALIZED:
      {
         if ( VEEPROM_bInit() != false )
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case VEEP_READY:
      {
         break;
      }
      case VEEP_WRITING:
      {
         if ( VEEPROM_u8DataLen > 0U )
         {
            tResult = VEEPROM_tDoWriteStep(VEEPROM_u8ActiveUnit);
            if ( (tResult == VEEP_PROCESS_SUCCESSFUL) || (tResult == VEEP_PROCESS_FAILURE) )
            {
               VEEPROM_u8DataLen = 0U;
               VEEPROM_tTaskState = VEEP_READY;
            }
            else
            {
               if (tResult == VEEP_PROCESS_DEFRAG_NEED)
               {
                  VEEPROM_u8DataLen = 0;
                  VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
               }
               else
               {
                  /* process ongoing */
               }
            }
         }
         else
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         break;
      }
      case VEEP_DEFRAGMENTING:
      {
         tResult = VEEPROM_tDoDefragStep();
         if ( (tResult == VEEP_PROCESS_SUCCESSFUL) || (tResult == VEEP_PROCESS_FAILURE) )
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         else
         {
            /* process ongoing */
         }
         break;
      }
      default:
      {
         /* default can never occur because switch variable is enum type */
      }
   }
   return(TASK_INITIALISED);
}
#else
/* VEEPROM_bInit() */
bool VEEPROM_bInit( void )
{
   //! \callgraph
   uint8_t u8NumberOfNonEmptyPages = 0U;
   uint8_t u8OtherPage            = 0U;
   uint8_t u8InterruptedPage;
   uint8_t u8CheckedPage;
   uint16_t u16UsedSpace_Active;
   uint16_t u16UsedSpace_Other;
   //uint8_t *pucAddressFirstDrawerId;
   bool  bRetVal;
   VEEPROM_tTaskState = VEEP_UNINITIALIZED;
   /* check for interrupted page-transfer */
   /** 
    * \internal
    * \section sec2 Outline of function
    * 
    * \subsection sub2_1 Are flash pages o.k.?
    * First <b>check out</b> how many <b>pages</b> are currently <b>in use</b>: 
    * If first possible position for a cabinet has a entry in the Drawer-Identifier-Field, then
    * this page is in use. Its location is evaluated in pucAddressFirstDrawer.
    */
   for ( u8CheckedPage = 0; u8CheckedPage < VEEPROM_ucNumberOfPages; u8CheckedPage++ )
   {
      //DrawerIdentification is written at first. if first DrawerIdentification is still unwritten, page is unwritten.
      /*
         Msg(4:0488) Performing pointer arithmetic. 
         MISRA-C:2004 Rule 17.4
         Pointer arithmetic is checked.
         (a) VEEPROM_apucFlashPage contains start address of checked page
         (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
         (c) VEEPROM_ucOffsetDrawerId is offset of drawer-ID
         thus (a) + (b) + (c) is address of first drawer-ID of page being checked.
      */
      /* pucAddressFirstDrawerId =   VEEPROM_apucFlashPage[ u8CheckedPage ]
                                + VEEPROM_PAGE_TRANSFER_COUNTER_LEN
                                + VEEPROM_ucOffsetDrawerId; */
      if ( (VEEPROM_bIsPageRemainderEmpty(u8CheckedPage, VEEPROM_apucFlashPage[ u8CheckedPage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) == false) && (VEEPROM_bPageDataValid(u8CheckedPage) != false) )
      {
         u8NumberOfNonEmptyPages++;
         u8OtherPage = VEEPROM_u8ActivePage;
         VEEPROM_u8ActivePage = u8CheckedPage;
      }
   }
   switch ( u8NumberOfNonEmptyPages )
   {
      /**
       * \callgraph
       *
       * \internal
       * \subsection sub2_2 First Run
       *
       * Only emty pages exist.
       * <b>If no page has been written yet</b>, select FLASH-page 0 to be active page.
       * And set write-pointer to start of page.
       *
       * Ensure for all flash pages that they are erased. Therefor a magic number is checked
       * that is set when page is erased. If this magic number is not set, page erase did not 
       * succeed without interruption.
       */
      case 0:
      {
         VEEPROM_u8ActivePage = VEEPROM_DEFAULT_PAGE;
         for (u8CheckedPage = 0; u8CheckedPage < VEEPROM_ucNumberOfPages; ++u8CheckedPage)
         {
            VEEPROM_vEnsurePageIsErased( u8CheckedPage );
         }
         /*
            Msg(4:0488) Performing pointer arithmetic. 
            MISRA-C:2004 Rule 17.4
            Pointer arithmetic is checked.
            (a) VEEPROM_apucFlashPage contains start address of default page
            (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
            thus (a) + (b) is address of first drawer of default page
         */
         VEEPROM_pu8WriteAddress = VEEPROM_apucFlashPage[ VEEPROM_DEFAULT_PAGE ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
         bRetVal = true;
         VEEPROM_tTaskState = VEEP_READY;
         break;
      }
      /**
       * \internal
       * \subsection sub2_3 No reset while defragmenting pages
       *
       * exact one non-empty page
       * <b>If exactly one page contains data</b>, set this FLASH-page to be active.
       * And refresh the RamCache - Values.
       * ... Set write-pointer to position after end of last drawer.
       *
       * Ensure for all flash pages except active page that they are erased. Therefor a magic 
       * number is checked that is set when page is erased. If this magic number is not set, 
       * page erase did not succeed without interruption.
       */
      case 1:
      {
         VEEPROM_pu8WriteAddress = VEEPROM_pu8ResetWriteAddress( VEEPROM_u8ActivePage );
         for (u8CheckedPage = 0; u8CheckedPage < VEEPROM_ucNumberOfPages; ++u8CheckedPage)
         {
            if ( u8CheckedPage != VEEPROM_u8ActivePage )
            {
               VEEPROM_vEnsurePageIsErased( u8CheckedPage );
            }
         }
         bRetVal = true;
         VEEPROM_tTaskState = VEEP_READY;
         break;
      }
      /**
       * \internal
       * \subsection sub2_4 Reset while defragmenting pages
       *
       * Two non-empty pages: There must have been a reset while defragmenting data.
       *
       * <b>If prior Defragmentation has been interrupted </b>, 
       * First erase page and then defrag again.  Check if to copy from page 1 to 0, or from 0 to 1.
       * At end of defragmenting, WritePointer shows to next free position.
       *
       * Refresh the RamCache - Values.
       * 
       * \todo check for interrupted page erase with writing 
       */
      case 2:
      {
         //u8OtherPage = (uint8_t) VEEPROM_ucGetPriorPage( VEEPROM_u8ActivePage );
         /* set smaller page as interrupted page */
         /*
            Msg(4:0488) Performing pointer arithmetic. 
            MISRA-C:2004 Rule 17.4
            Pointer arithmetic is checked.
            VEEPROM_u16GetUsedSpace (..) returns used space of given page
         */
        u16UsedSpace_Active = VEEPROM_u16GetUsedSpace( VEEPROM_u8ActivePage );
        u16UsedSpace_Other = VEEPROM_u16GetUsedSpace( u8OtherPage );
        if ( u16UsedSpace_Active > u16UsedSpace_Other )
         {
            u8InterruptedPage = u8OtherPage;
         }
         else
         {
            u8InterruptedPage = VEEPROM_u8ActivePage;
            VEEPROM_u8ActivePage = u8OtherPage;
         }
         /* check larger page for interrupted page erase (00 00 00 00 00 00 .. .. fe 1f 2a .. .. .. ff ff) */
         /* if larger page has interrupted page transfer */
         /* -  do not erase smaller page, that is actually set as interrupted page. */
         /* -  erase larger page */
         /* -  set smaller page as active page. */
         if ( VEEPROM_bCheckForInterruptedPageErase( VEEPROM_u8ActivePage ) != false )
         {
            /* page erase of full page was interrupted due to reset. In this case the smaller page */
            /* contains already all latest data, because erase of full page is first started when transfer of latest data has been finished. */
            /* erase fuller page, that no longer contains valid data. */
            /* swap active and interrupted pages. */
            u8OtherPage = u8InterruptedPage;
            u8InterruptedPage = VEEPROM_u8ActivePage;
            VEEPROM_u8ActivePage = u8OtherPage;
         }
         else
         {
            /* there was no interrupted page erase. In this case a reset occured while transfering data from active(full) page to transfer-page. */
            /* In this case the smaller page probably does not contain all latest data, but the fuller page does. */
            if ( VEEPROM_bCheckForDefragmentedAndNotErasedPage( VEEPROM_u8ActivePage, u8InterruptedPage ) != false)
            {
               /* In this case full page was defragmented, new valid drawer values are in the new page, but erase process of full page did not start yet. If this happened, begin of new page contains page erase counter incremented by 1 compared with page erase counter of previous page */
               /* erase fuller page. */
               /* swap active and interrupted pages. */
               u8OtherPage = u8InterruptedPage;
               u8InterruptedPage = VEEPROM_u8ActivePage;
               VEEPROM_u8ActivePage = u8OtherPage;
            }
            else
            {
               /* erase smaller page, that is already set as interrupted page. */
            }
         }
         // delete interrupted page
         bRetVal = FLASH_bEnableAccess();
         if ( false != bRetVal )
         {
            bRetVal = FLASH_bErase(  VEEPROM_apucFlashPage[ u8InterruptedPage ] );
            ASSERT_vEXPR( bRetVal != false);
            (void) FLASH_bDisableAccess();
            VEEPROM_vSetPageEraseMark( u8InterruptedPage );
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
         VEEPROM_pu8WriteAddress = VEEPROM_pu8ResetWriteAddress( VEEPROM_u8ActivePage );
         for (u8CheckedPage = 0; u8CheckedPage < VEEPROM_ucNumberOfPages; ++u8CheckedPage)
         {
            if ( u8CheckedPage != VEEPROM_u8ActivePage )
            {
               VEEPROM_vEnsurePageIsErased( u8CheckedPage );
            }
         }
         if ( bRetVal != false )
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         break;
      }
      default:
      {
         // if there is more than one non-free page, something must have gone wrong before - this should never happen.
         ASSERT_vEXPR (-1);
         bRetVal = false;
      }
   }
   return bRetVal;
}
/* VEEPROM_bDoDefragNow() */
bool VEEPROM_bDoDefragNow( void )
{
   bool bRetVal;
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      bRetVal = VEEPROM_bDefrag ( (uint8_t)VEEPROM_u8ActivePage, VEEPROM_ucGetNextPage( VEEPROM_u8ActivePage ) );
      ASSERT_vEXPR( bRetVal==true );
      VEEPROM_vDefragEnded();
      return bRetVal;
   }
   else
   {
      return false;
   }
}
/* VEEPROM_bWrite() */
bool  VEEPROM_bWrite                        ( uint8_t        u8Label,
                                              const uint8_t* pu8Data,
                                              uint8_t        u8DataLen )
{
   bool   bRetVal;
   if ( VEEPROM_tTaskState != VEEP_READY )
   {
      return false;
   }
   ASSERT_vEXPR( u8Label < VEEPROM_ucGetNumberOfDrawers() );
   /* some assertions */
   ASSERT_vEXPR( u8DataLen == VEEPROM_ucGetDrawerSize( u8Label ) );
   ASSERT_vEXPR( u8DataLen <= VEEPROM_MAX_DATA_LEN );
   ASSERT_vEXPR( u8DataLen > 0 );
   /* check if value to write is different from latest one stored in FLASH. */
   /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do write new data if it is different from latest one stored. */
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
   uint8_t au8Buffer[VEEPROM_MAX_DATA_LEN];
   uint8_t u8Index;
   bool bDifferentData = false;
   ASSERT_vEXPR (VEEPROM_MAX_DATA_LEN >= u8DataLen);
   VEEPROM_bRead( u8Label, au8Buffer, u8DataLen);
   for (u8Index = 0; u8Index < u8DataLen; u8Index++)
   {
      if (au8Buffer[u8Index] != pu8Data[u8Index])
      {
         bDifferentData == true;
         break;
      }
   }
   /* - if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do if data to write is different from latest one stored. */
   if (false != bDifferentData)
     {
#endif
   /* comment - check if we have to defrag first */
   /**
    * \internal
    * \subsection sub1_2 Check flash space
    * If too less space is left for writing new data, ...
    * ... defragmenting before must be done.
    * There shall never happen any errors 
    * while defragmenting data. 
    * But if there are some errors, return with error. 
    */
   ASSERT_vEXPR ( VEEPROM_ucCellSize > 0U );
   if (   VEEPROM_uiGetFreeSpace() < (uint16_t)( ( (  ( u8DataLen + ((uint16_t)VEEPROM_ucCellSize - 1U) )// \    length of Payload
                                            / (uint16_t)VEEPROM_ucCellSize                      //  +=> quantisized to 
                                         ) * (uint16_t)VEEPROM_ucCellSize )                     // /    cellsize
                                         + (uint16_t)VEEPROM_ucOffsetDrawerPayload              // ===> length of Header
                                     )
      )
   {
      /**
       * only start process of defragmentation if allowed from application
       */
      if ( false != VEEPROM_bApprobateDefrag() )
      {
         bRetVal = VEEPROM_bDoDefragNow();
         /* if cached values are not used for defragmentation, we have to write new values after process of defragmentation has finished. */
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
         if ( false != bRetVal )
         {
            bRetVal = VEEPROM_bWriteWithoutCheck (u8Label, pu8Data, u8DataLen);
         }
         else
         {
            ASSERT_vEXPR( bRetVal != false);
         }
#endif
      }
      else
      {
         bRetVal = false;
         ASSERT_vEXPR( bRetVal != false);
      }
   }
   else
   {
      bRetVal = VEEPROM_bWriteWithoutCheck (u8Label, pu8Data, u8DataLen);
      ASSERT_vEXPR( bRetVal != false);
   }
   return bRetVal;
   /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, do not write data if data to write equals latest one stored. */
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
     }
   /* otherwise return true (data is already up to date) */
   else
     {
       return true;
     }
#endif
}
/* VEEPROM_ucRead() */
uint8_t VEEPROM_ucRead                      ( uint8_t        u8Label,
                                              uint8_t*       pu8Data,
                                              uint8_t        u8DataLen )
{
   uint8_t  u8Crc  = 0U    ;
   uint8_t *pucCrc = &u8Crc;
   uint8_t  u8Index        ;
   bool   bEmptyAreaFound;
   uint8_t *pu8CheckedDrawerStartAddress;
   uint8_t *pu8CurrentAddress           ;
   uint8_t *pu8FoundPayload = NULL      ;
   uint8_t u8ReadOutDrawerId            ;
   uint8_t u8ReadOutCrc                 ;
   /**
    * \internal
    * \section secSf5 Outline of function 
    * \subsection subSf5_1 Preparation    
    * retrieve start address where to search for the desired Drawer
    */
   uint16_t u16TempOffset = 0U;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bDbgRetVal;
#endif
   if ( VEEPROM_tTaskState != VEEP_READY )
   {
      return 0;
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start address of active page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is position of first drawer on active page.
   */
   pu8CheckedDrawerStartAddress = VEEPROM_apucFlashPage[ VEEPROM_u8ActivePage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
   do
   {
      /**
       * read out CRC-field
       */
      pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ u16TempOffset ];
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, (uint8_t*)&u8ReadOutCrc, 1U );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8CurrentAddress, &u8ReadOutCrc, 1U );
#endif
      /**
       * read out drawerID field
       */
      pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerId + u16TempOffset ];
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, &u8ReadOutDrawerId, 1U );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8CurrentAddress, &u8ReadOutDrawerId, 1U );
#endif
      /** 
       * if not at end of written segment of flash-page
       */
      if ( (u8ReadOutDrawerId != VEEPROM_UNWRITTEN_FLASH_CELL) || (VEEPROM_bIsPageRemainderEmpty(VEEPROM_u8ActivePage, pu8CurrentAddress) == false) )
      {
         bEmptyAreaFound = false;
         /**
          * if we have the correct drawer ID
          */
         if ( u8Label == u8ReadOutDrawerId )
         {
            /**
             * read out payload of drawer
             */
            ASSERT_vEXPR (  VEEPROM_ucGetDrawerSize( u8ReadOutDrawerId ) == u8DataLen );
#if defined(DEBUG) && (DEBUG != 0u)
            bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ], pu8Data, u8DataLen );
            ASSERT_vEXPR ( false != bDbgRetVal );
            VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
            (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ], pu8Data, u8DataLen );
#endif
            /**
             * calc crc over drawerID and payload
             */
            {
               u8Crc = 0;
               CRC_vUpdateCrc( pucCrc, u8Label );
               for (u8Index = 0 ; u8Index < u8DataLen; ++u8Index ) 
               {
                  CRC_vUpdateCrc( pucCrc, pu8Data[ u8Index ] );
               }
            }
            /**
             * if calculated crc matches stored one, we have found a valid and correct drawer
             */
            if ( u8Crc == u8ReadOutCrc )
            {
               pu8FoundPayload = pu8CheckedDrawerStartAddress + u16TempOffset;
            }
         }
         // go on with next drawer ...
         u16TempOffset += (uint16_t)VEEPROM_ucOffsetDrawerPayload + (uint16_t)VEEPROM_ucObserverGetBruttoPayloadLength(u8ReadOutDrawerId);
      }
      else
      {
         bEmptyAreaFound = true;
      }
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      try to run not over end of active page:
      (a) pu8CheckedDrawerStartAddress is address to check for next iteration
      (b) VEEPROM_apucFlashPage contains start address of active page
      (c) VEEPROM_uiFlashPageSize is size of flash-page
      thus (a) - (b) must be smaller than (c) if regarded address is in range of active page.
   */
   while (   ( bEmptyAreaFound == false )
          && ( ((u16TempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1U)) < VEEPROM_uiFlashPageSize )
         );
   if ( pu8FoundPayload != NULL)
   {
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead  ( &pu8FoundPayload[ VEEPROM_ucOffsetDrawerPayload ],
                     pu8Data,
                     u8DataLen );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead  ( &pu8FoundPayload[ VEEPROM_ucOffsetDrawerPayload ], pu8Data, u8DataLen );
#endif
      return u8DataLen;
   }
   else
   {
      return 0;
   }
}
/* VEEPROM_uiGetFreeSpace() */
uint16_t VEEPROM_uiGetFreeSpace( void )
{
   uint32_t u32FreeSpace;
   if ( VEEPROM_tTaskState != VEEP_READY )
   {
      return 0;
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start address of active page
      (b) VEEPROM_uiFlashPageSize is length of page
      (c) VEEPROM_pu8WriteAddress is write address on active page
      thus (a) + (b) - (c) contains free space in bytes left on active page
   */
   u32FreeSpace = (((uint32_t)VEEPROM_apucFlashPage[ VEEPROM_u8ActivePage ] + (uint32_t)VEEPROM_uiFlashPageSize ) - (uint32_t)VEEPROM_pu8WriteAddress );
   /* if page is full,  VEEPROM_pu8WriteAddress is set after active page in use. This results in a "negative" pucFreeSpace. But because of unsigned variables, we have to check, if it out of range. In this case we do not have any space left and return "0". */
   if (u32FreeSpace > (uint32_t)VEEPROM_uiFlashPageSize)
   {
      return 0U;
   }
   else
   {
      return (uint16_t)u32FreeSpace;
   }
}
/* VEEPROM_bReadWholeActivePage() */
bool VEEPROM_bReadWholeActivePage(void)
{
   uint8_t  u8Crc  = 0U;
   uint8_t *pucCrc = &u8Crc;
   uint8_t  u8Index;
   bool     bEmptyAreaFound;
   bool     bRetVal = true;
   uint8_t *pu8CheckedDrawerStartAddress;
   uint8_t *pu8CurrentAddress;
   uint8_t  u8ReadOutDrawerId;
   uint8_t  u8ReadOutCrc;
   uint8_t  u8DrawerLength;
   uint8_t *pu8DrawerCacheAddress;
   uint8_t *pu8EmptyAreaBase = NULL;
   uint8_t  au8Buffer[VEEPROM_MAX_DATA_LEN];
   /**
    * \internal
    * \section secSf5 Outline of function 
    * \subsection subSf5_1 Preparation    
    * retrieve start address where to search for the desired Drawer
    */
   uint16_t u16TempOffset = 0U;
#if defined(DEBUG) && (DEBUG != 0u)
   bool bDbgRetVal;
#endif
   if ( VEEPROM_tTaskState != VEEP_READY )
   {
#if defined(DEBUG) && (DEBUG != 0u)
      /* assert */
      /* VEEPROM_tTaskState is not ready */
      _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);

#endif
      return false;
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start address of active page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is position of first drawer on active page.
   */
   pu8CheckedDrawerStartAddress = VEEPROM_apucFlashPage[ VEEPROM_u8ActivePage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
   do
   {
      /*
       * read out drawerID field
       */
      pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ u16TempOffset + (uint16_t)VEEPROM_ucOffsetDrawerId ];
#if defined(DEBUG) && (DEBUG != 0u)
      bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, &u8ReadOutDrawerId, 1U );
      ASSERT_vEXPR ( false != bDbgRetVal );
      VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
      (void) FLASH_bRead ( pu8CurrentAddress, &u8ReadOutDrawerId, 1U );
#endif
      /* 
       * if not at end of written segment of flash-page
       */
      if ( u8ReadOutDrawerId != VEEPROM_UNWRITTEN_FLASH_CELL )
      {
         bEmptyAreaFound = false;
      }
      else
      {
         if ( pu8EmptyAreaBase == NULL )
         {
            pu8EmptyAreaBase = VEEPROM_pucGetFreeAreaBase(VEEPROM_u8ActivePage, pu8CurrentAddress);
         }
         if ( (uint32_t)pu8CurrentAddress < (uint32_t)pu8EmptyAreaBase )
         {
            bEmptyAreaFound = false;
         }
         else
         {
            bEmptyAreaFound = true;
         }
      }
      if ( bEmptyAreaFound == false )
      {
         if ( u8ReadOutDrawerId < VEEPROM_ucGetNumberOfDrawers() )
         {
            /*
             * read out payload of drawer
             */
            u8DrawerLength = VEEPROM_ucGetDrawerSize( u8ReadOutDrawerId );
            if ( u8DrawerLength > 0U )
            {
               /*
                * read out CRC-field
                */
#if defined(DEBUG) && (DEBUG != 0u)
               bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset ], &u8ReadOutCrc, 1U );
               ASSERT_vEXPR ( false != bDbgRetVal );
               VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
               (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset ], &u8ReadOutCrc, 1U );
#endif
#if defined(DEBUG) && (DEBUG != 0u)
               bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ], (uint8_t*)&au8Buffer[0], u8DrawerLength );
               ASSERT_vEXPR ( false != bDbgRetVal );
               VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
               (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + u16TempOffset ],&au8Buffer[0], u8DrawerLength );
#endif
               /*
                * calc crc over drawerID and payload
                */
               {
                  u8Crc = 0U;
                  CRC_vUpdateCrc( pucCrc, u8ReadOutDrawerId );
                  for (u8Index = 0 ; u8Index < u8DrawerLength; ++u8Index ) 
                  {
                     CRC_vUpdateCrc( pucCrc, au8Buffer[ u8Index ] );
                  }
               }
               /*
                * if calculated crc matches stored one, we have found a valid and correct drawer
                */
               if ( u8Crc == u8ReadOutCrc )
               {
                  pu8DrawerCacheAddress = VEEPROM_pucGetDrawerAddress( u8ReadOutDrawerId );
                  if ( pu8DrawerCacheAddress != NULL )
                  {
                     for (u8Index = 0 ; u8Index < u8DrawerLength; ++u8Index ) 
                     {
                        if ( pu8DrawerCacheAddress[u8Index] != au8Buffer[u8Index] )
                        {
                           pu8DrawerCacheAddress[u8Index] = au8Buffer[u8Index];
                        }
                     }
                     VEEPROM_vAfterCacheVariableSet( u8ReadOutDrawerId);
                  }
                  else
                  {
                     /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
                     ASSERT_vEXPR ( pu8DrawerCacheAddress != NULL );
#endif
                     bRetVal = false;
                  }
               }
               else
               {
                  /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
                  ASSERT_vEXPR ( u8Crc == u8ReadOutCrc );
#endif
                  bRetVal = false;
               }
            }
            else
            {
               /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
               ASSERT_vEXPR ( u8DrawerLength > 0U );
#endif
               bRetVal = false;
            }
         }
         else
         {
            /* DEBUG */
#if defined(DEBUG) && (DEBUG != 0u)
            ASSERT_vEXPR ( u8ReadOutDrawerId < VEEPROM_ucGetNumberOfDrawers() );
#endif
            bRetVal = false;
         }
         // go on with next drawer ...
         u16TempOffset += (uint16_t)VEEPROM_ucOffsetDrawerPayload + (uint16_t)VEEPROM_ucObserverGetBruttoPayloadLength(u8ReadOutDrawerId);
      }
   }
   /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      try to run not over end of active page:
      (a) pu8CheckedDrawerStartAddress is address to check for next iteration
      (b) VEEPROM_apucFlashPage contains start address of active page
      (c) VEEPROM_uiFlashPageSize is size of flash-page
      thus (a) - (b) must be smaller than (c) if regarded address is in range of active page.
   */
   while (   ( bEmptyAreaFound == false )
          && (((u16TempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1U)) <  VEEPROM_uiFlashPageSize )
         );
   return bRetVal;
}
/* VEEPROM_bWriteStart() */
bool VEEPROM_bWriteStart( uint8_t u8Label, const uint8_t *pu8Data, uint8_t u8DataLen )
{
   bool bRetVal = false;
   uint8_t u8Counter;
#if defined(DEBUG) && (DEBUG != 0u)
   /* debug version */
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      if ( u8Label < VEEPROM_ucGetNumberOfDrawers() )
      {
         if ( u8DataLen == VEEPROM_ucGetDrawerSize(u8Label) )
         {
            if ( u8DataLen <= VEEPROM_MAX_DATA_LEN )
            {
               if ( u8DataLen > 0U )
               {
                  if ( VEEPROM_uiGetFreeSpace() < (((( u8DataLen + (VEEPROM_ucCellSize - 1)) / VEEPROM_ucCellSize ) * VEEPROM_ucCellSize ) + VEEPROM_ucOffsetDrawerPayload ) )
                  {
                     if ( VEEPROM_bApprobateDefrag() != false )
                     {
                        VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
                        bRetVal = true;
                     }
                     else
                     {
                        /* assert */
                        /* defragmentation not alowed */
                        _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
                     }
                  }
                  else
                  {
                     /* copy data to its own field */
                     for (u8Counter = 0; u8Counter < u8DataLen; u8Counter++)
                     {
                        VEEPROM_au8WriteData[u8Counter] = pu8Data[u8Counter];
                     }
                     /* fill rest of field with 0xFF => there must be empty info */
                     for (u8Counter = u8DataLen; u8Counter < VEEPROM_MAX_DATA_LEN; u8Counter++)
                     {
                        VEEPROM_au8WriteData[u8Counter] = VEEPROM_UNWRITTEN_FLASH_CELL;
                     }
                     VEEPROM_u8DataLen   = u8DataLen;
                     VEEPROM_u8Label     = u8Label  ;
                     VEEPROM_tTaskState = VEEP_WRITING;
                     bRetVal = true;
                  }
               }
               else
               {
                  /* assert */
                  /* u8DataLen is 0 */
                  _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
               }
            }
            else
            {
               /* assert */
               /* u8DataLen is too long */
               _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
            }
         }
         else
         {
            /* assert */
            /* u8DataLen not valid */
            _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
         }
      }
      else
      {
         /* assert */
         /* u8Label not valid */
         _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
      }
   }
   else
   {
      /* assert */
      /* previous process not finished */
      _ASSERT_vOnFailure ((const uint8_t*)((void *)__FILE__), __LINE__);
   }
#else
   /* normal version */
   if ( VEEPROM_tTaskState == VEEP_READY )
   {
      if (   VEEPROM_uiGetFreeSpace() < (((( (uint16_t)u8DataLen + ((uint16_t)VEEPROM_ucCellSize - 1U)) / (uint16_t)VEEPROM_ucCellSize ) * (uint16_t)VEEPROM_ucCellSize ) + (uint16_t)VEEPROM_ucOffsetDrawerPayload ) )
      {
         if ( VEEPROM_bApprobateDefrag() != false )
         {
            VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
            bRetVal = true;
         }
         else
         {
            /* defragmentation not alowed */
         }
      }
      else
      {
         /* copy data to its own field */
         for (u8Counter = 0; u8Counter < u8DataLen; u8Counter++)
         {
            VEEPROM_au8WriteData[u8Counter] = pu8Data[u8Counter];
         }
         /* fill rest of field with 0xFF => there must be empty info */
         for (u8Counter = u8DataLen; u8Counter < VEEPROM_MAX_DATA_LEN; u8Counter++)
         {
            VEEPROM_au8WriteData[u8Counter] = VEEPROM_UNWRITTEN_FLASH_CELL;
         }
         VEEPROM_u8DataLen   = u8DataLen;
         VEEPROM_u8Label     = u8Label  ;
         VEEPROM_tTaskState  = VEEP_WRITING;
         bRetVal = true;
      }
   }
   else
   {
      /* previous process not finished */
   }
#endif
   return(bRetVal);
}
/* VEEPROM_ucHandleTask() */
uint8_t VEEPROM_ucHandleTask(void)
{
   eProcessResultType tResult;
   switch (VEEPROM_tTaskState)
   {
      case VEEP_UNINITIALIZED:
      {
         if ( VEEPROM_bInit() != false )
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case VEEP_READY:
      {
         break;
      }
      case VEEP_WRITING:
      {
         if ( VEEPROM_u8DataLen > 0U )
         {
            tResult = VEEPROM_tDoWriteStep(VEEPROM_u8ActivePage);
            if ( (tResult == VEEP_PROCESS_SUCCESSFUL) || (tResult == VEEP_PROCESS_FAILURE) )
            {
               VEEPROM_u8DataLen = 0U;
               VEEPROM_tTaskState = VEEP_READY;
            }
            else
            {
               if (tResult == VEEP_PROCESS_DEFRAG_NEED)
               {
                  VEEPROM_u8DataLen = 0U;
                  VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
               }
               else
               {
                  /* process ongoing */
               }
            }
         }
         else
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         break;
      }
      case VEEP_DEFRAGMENTING:
      {
         tResult = VEEPROM_tDoDefragStep();
         if ( (tResult == VEEP_PROCESS_SUCCESSFUL) || (tResult == VEEP_PROCESS_FAILURE) )
         {
            VEEPROM_tTaskState = VEEP_READY;
         }
         else
         {
            /* process ongoing */
         }
         break;
      }
      default:
      {
         /* default can never occur because switch variable is enum type */
      }
   }
   return(TASK_INITIALISED);
}
/*lint -restore -e717*/
#endif
