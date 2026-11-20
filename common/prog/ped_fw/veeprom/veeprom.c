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
 *  \brief      Implementation of Volatile EEPROM library
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <limits.h>
#include "LibTypes.h"
#include "VEEPROMpriv.h"
#include "VEEPROM.h"
#include "VEEPROMXS.h"
#include "flash/Flash.h"
#include "utility/utility.h"
#include "crc.h"
#include "VEEPROMmcXS.h"
#include "nvmem/cache.h"
#include "hsup.h"
/*lint -save -e717*/
/*lint -save -e553 Undefined preprocessor variable 'DEBUG', assumed 0 */
/*lint -save -e731 Boolean argument to equal/not equal */
/*lint -save -e923 cast from/to pointer */
/*lint -save -e946 Relational or subtract operator applied to pointers */
/*lint -save -e947 Subtract operator applied to pointers */
/*lint -save -e9033 Impermissible cast of composite expression */
/*lint -save -e9005 attempt to cast away const/volatile from a pointer or reference */
/*lint -save -e954  Pointer variable could be declared as pointing to const */
/*lint -save -e661 potential out of bounds pointer access */
#if defined(DEBUG) && (DEBUG != 0x00u)
#include "pedAssert.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
//#define TIME_MEASUREMENT
#ifdef TIME_MEASUREMENT
#include "timer/timerlib.h"
#endif

/**
 * \brief content of unwritten flash-cells
 *
 * unwritten (or erased) flash-cells contain in most cases the byte 0xFF
 */
#if !defined(VEEPROM_UNWRITTEN_FLASH_CELL)
#define VEEPROM_UNWRITTEN_FLASH_CELL     ( 0xFFu )
#endif
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
#define VEEPROM_PAGE_TRANSFER_COUNTER_LEN 2u

/* when using this library without NVMEM-library,*/
/*           _     _   _                         _ _       _      */
/*  ___  ___| |_  | |_| |__   ___   _____      _(_) |_ ___| |__ _ */
/*/ __|/ _ \ __| | __| '_ \ / _ \ / __\ \ /\ / / | __/ __| '_ \(_)*/
/*\__ \  __/ |_  | |_| | | |  __/ \__ \\ V  V /| | || (__| | | |_ */
/*\___/\___|\__|  \__|_| |_|\___| |___/ \_/\_/ |_|\__\___|_| |_(_)*/
/*                                                                */
/* VEEPROM_USE_LIB_WITHOUT_NVMEM*/
/* - to disable using cached values for process of defragmentation and*/
/* - to enable check if data to write is different from actually data stored.*/
#ifdef VEEPROM_USE_LIB_WITHOUT_NVMEM
#define VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
#define VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
#endif

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#ifdef TIME_MEASUREMENT
static uint16_t VEEPROM_uiTimeBefore;
static uint16_t VEEPROM_uiTimeAfter;
static uint16_t VEEPROM_uiTimeDifference;
#endif
#if defined(VEEPROM_LARGE)
/**
 * active page for storing data
 */
static uint8_t VEEPROM_ucActiveUnit;
/**
 * \brief content of unwritten flash-cells
 *
 * unwritten (or erased) flash-cells contain the byte 0xFF
 */
const tDrawerID VEEPROM_tInvalidDrawer = 0xFFFF;
#else
/**
 * active page for storing data
 */
static uint8_t VEEPROM_ucActivePage;
#endif
#if (!defined(STM32_ARM) && !defined(R2_ARM) && !defined(SH30_ARM)) || defined(VEEPROM_LARGE)
static eVEEPROMtaskStateType VEEPROM_tTaskState = VEEP_UNINITIALIZED;
#endif
/**
 * actual write-position in active page
 */
static uint8_t *VEEPROM_pucWriteAddress;
/* variables*/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
#if defined(VEEPROM_LARGE)
static uint8_t VEEPROM_ucGetNextUnit(uint8_t ucUnit);
static uint16_t VEEPROM_uiGetUsedSpace(uint8_t ucUnit);
static bool VEEPROM_bIsUnitErased ( uint8_t ucUnitNumber );
static uint8_t *VEEPROM_pucResetWriteAddress( uint8_t ucUnitNumber );
static bool VEEPROM_bWriteWithoutCheck( tDrawerID tLabel, const uint8_t *pucData, uint8_t ucDataLen );
#else
static uint8_t VEEPROM_ucGetNextPage(uint8_t ucPage);
static uint16_t VEEPROM_uiGetUsedSpace(uint8_t ucPage);
static uint8_t* VEEPROM_pucResetWriteAddress       ( uint8_t ucFlashPage );
static bool VEEPROM_bIsPageErased( uint8_t ucFlashPageNumber );
static bool VEEPROM_bWriteWithoutCheck( uint8_t ucLabel, const uint8_t* pucData, uint8_t ucDataLen );
#endif
static bool VEEPROM_bDefrag( uint8_t ucSource, uint8_t ucDestination );

#if defined(DEBUG) && (DEBUG != 0x00u)
static void VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bool bValue);
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
#if defined(VEEPROM_LARGE)
/* VEEPROM_bDefrag()*/
/**
 * \brief defrag data from full flash page
 * 
 * \pre Destination Page is assumed to be clear. 
 *
 * Data of defragmented ucSource FlashPage is defragmented by copying 
 * cached data of all valid drawers to destination page. 
 * Validity of cached drawers is stored in #CACHE_aucDrawerCacheValidVeeprom.
 * When copying process has finished, fragmented old page is cleared.
 * New defragmented page is set to be VEEPROM_ucActivePage.
 *
 * \note if VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG is defined, then values used
 *       for defragmentation are read out directly from full flash page. This takes
 *       more time, but results in smaller code for small systems.
 * 
 *
 * \param ucSource unit number to copy data from
 * \param ucDestination unit number to copy data to
 * 
 * \return status of defragmentation:
 *          - false, if error while writing or flash page full
 *          - true,  if defragmentation succeeded with no errors.
 * \image html VEEPROM_bDefrag.emf "flowchart of VEEPROM_bDefrag()"
 * \image latex VEEPROM_bDefrag.eps "flowchart of VEEPROM_bDefrag()"
 */
static bool VEEPROM_bDefrag ( uint8_t ucSource, uint8_t ucDestination )
{
    tDrawerID  tActualDrawer = 0;
    uint8_t      ucDrawerPayloadLen;
    bool bRetVal = false;
    tDrawerID tNumberOfDrawers = VEEPROM_tGetNumberOfDrawers();
    uint8_t ucPageCounter;
    uint8_t *pucCacheAddress;
    static uint8_t aucNumberPageTransfers[ 2 ];
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
    uint8_t *pucData;
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
    VEEPROM_pucWriteAddress = VEEPROM_apucUnitStart[ ucDestination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
    if ( VEEPROM_bIsUnitErased(ucDestination) == false )
    {
        /* error state - destination unit must be erased*/
        _VEEPROM_vResetWatchdogTimer();
        VEEPROM_vBeforePageErase();
        bRetVal = FLASH_bEnableAccess();
        if ( false != bRetVal )
        {
            //erase page has not been finished before. Do erase all pages in unit once more
            for (ucPageCounter = 0; ucPageCounter < VEEPROM_ucFlashPagesPerUnit; ucPageCounter++)
            {
                bRetVal = FLASH_bErase(  VEEPROM_apucUnitStart[ ucDestination ] + (uint16_t)(ucPageCounter * VEEPROM_uiTotalPageSize) );
                _VEEPROM_vResetWatchdogTimer();
            }
            (void) FLASH_bDisableAccess();
            _VEEPROM_vResetWatchdogTimer();
            if ( bRetVal != false )
            {
                VEEPROM_vSetUnitEraseMark( ucDestination );
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
    /* comment - increment page transfer counter*/
    /**
     * \internal
     * \subsection subSf6_2 PageTransferCounter
     *  - read out PageTransferCounter high and low byte
     *  - increment PageTransferCounter: check if high byte has to be incremented too
     *  - write incremented PageTransferCounter
     */
    /* read out page erase counter of full page and increment it*/
    if ( FLASH_bEnableAccess() != false )
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        bRetVal = FLASH_bRead( VEEPROM_apucUnitStart[ ucSource ], &aucNumberPageTransfers[ 0 ], VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
        ASSERT_vEXPR( false != bRetVal );
#else
        (void) FLASH_bRead( VEEPROM_apucUnitStart[ ucSource ], &aucNumberPageTransfers[ 0 ], VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
#endif
        if ( aucNumberPageTransfers[ 1 ] == 0xFFU )
        {
            aucNumberPageTransfers[ 0 ]++;
            if ( aucNumberPageTransfers[ 0 ] == 0x00U )
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
                aucNumberPageTransfers[ 1 ]++;
            }
        }
        aucNumberPageTransfers[ 1 ]++;
    }
    else
    {
        ASSERT_vEXPR( bRetVal != false);
    }
    (void)FLASH_bDisableAccess();
    /**
     * \subsection subSf6_3 Defragmenting data
     * for every drawer, ...
     */
    while ( tActualDrawer < tNumberOfDrawers )
    {
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
        /* comment - defrag data using values from full page*/
        /**
         * ... try to get latest data of actual drawer from fully fragmented page
         * ... if no error while getting latest data of actual drawer, 
         *     copy to new page
         * ... go on with next drawer.
         */
        pucData = VEEPROM_pucGetDataPayloadFromPage( ucSource, tActualDrawer );
        if ( pucData != 0)
        {
            ucDrawerPayloadLen = VEEPROM_ucGetDrawerSize( tActualDrawer );
            ASSERT_vEXPR(   ucDrawerPayloadLen + VEEPROM_pucWriteAddress
                            <= VEEPROM_apucUnitStart[ ucDestination ] + VEEPROM_uiUnitSize );
            bRetVal =  VEEPROM_bWriteWithoutCheck( tActualDrawer, pucData, ucDrawerPayloadLen );
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
        if ( 0U != CACHE_bIsCacheValidVeeprom( tActualDrawer) )
        {
            pucCacheAddress =  CACHE_pucGetCacheAddressVEEPROM( tActualDrawer );
            ucDrawerPayloadLen = VEEPROM_ucGetDrawerSize( tActualDrawer );
            ASSERT_vEXPR ( VEEPROM_pucWriteAddress < (VEEPROM_apucUnitStart[ ucDestination ] + VEEPROM_uiUnitSize) );
            bRetVal = VEEPROM_bWriteWithoutCheck( tActualDrawer, pucCacheAddress, ucDrawerPayloadLen );
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
            bRetVal = FLASH_bWrite( VEEPROM_apucUnitStart[ ucDestination ], &aucNumberPageTransfers[ 0 ], VEEPROM_PAGE_TRANSFER_COUNTER_LEN );
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
            for (ucPageCounter = 0; ucPageCounter < VEEPROM_ucFlashPagesPerUnit; ucPageCounter++)
            {
                bRetVal = FLASH_bErase(  VEEPROM_apucUnitStart[ ucSource ] + (uint16_t)(ucPageCounter * VEEPROM_uiTotalPageSize) );
                _VEEPROM_vResetWatchdogTimer();
            }
            (void) FLASH_bDisableAccess();
            _VEEPROM_vResetWatchdogTimer();
            if ( bRetVal != false )
            {
                VEEPROM_vSetUnitEraseMark( ucSource );
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
    VEEPROM_ucActiveUnit = ucDestination;
    return bRetVal;
}
/* VEEPROM_bIsUnitErased()*/
/**
 * \brief checks if desired memory unit is empty, except place for page erase mark
 *
 * \param ucUnitNumber Number of memory unit
 * \return 
 *          - true: flash page is empty
 *          - false: at least one cell of flash page is not empty
 */
static bool VEEPROM_bIsUnitErased ( uint8_t ucUnitNumber )
{
    bool bIsEmpty = true;
    uint8_t *pucReadAddress;
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        start of page + length of page == end of page
    */
    uint8_t *pucEndAddress = VEEPROM_apucUnitStart[ ucUnitNumber ] + VEEPROM_uiUnitSize;
    /* test memory contents*/
    pucReadAddress = VEEPROM_apucUnitStart[ ucUnitNumber ];
    if ( FLASH_bEnableAccess() != false )
    {
        do
        {
            if ( *pucReadAddress != VEEPROM_UNWRITTEN_FLASH_CELL )
            {
                bIsEmpty = false;
            }
            ++pucReadAddress;
        }
        while ( (bIsEmpty == true) && ((uint32_t)pucReadAddress < (uint32_t)pucEndAddress) );
    }
    else
    {
        bIsEmpty = false;
    }
    (void) FLASH_bDisableAccess();
    return bIsEmpty;
}
/* VEEPROM_ucGetNextUnit()*/
/**
 * \brief get page-number of next memory unit
 * 
 * 
 * \param ucUnit memory unit number
 * 
 * \return page number of next unit
 */
static uint8_t VEEPROM_ucGetNextUnit ( uint8_t ucUnit )
{
    return (uint8_t)( ( ucUnit + 1U) % VEEPROM_ucNumberOfUnits );
}
/* VEEPROM_uiGetUsedSpace()*/
/**
 * \brief evaluate bytes already used for given page
 *
 * \note page must not be actually in use. This is why VEEPROM_pucWriteAddress must not be correct for this page.
 * \param ucPage pagenumber
 * 
 * \return number of bytes (including header and page erase counter) already used of page
 */
static uint16_t VEEPROM_uiGetUsedSpace ( uint8_t ucUnit )
{
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked:
        (a) VEEPROM_pucResetWriteAddress(..) returns next free write-address in given Page
        (b) VEEPROM_apicFlashPage contains the start address of this page.
        thus (a) - (b) results in actually used space of given flash-page.
    */
    return (uint16_t)( VEEPROM_pucResetWriteAddress( ucUnit ) - VEEPROM_apucUnitStart[ ucUnit ] );
}
/* VEEPROM_pucResetWriteAddress()*/
/**
 * \brief retrieve first free drawer-position of desired memory unit
 *
 * if memory unit is full, the next theoretical position after end of flash page is returned.
 * This is not a valid write address. VEEPROM_bWrite() will handle this and initiate a defragmentation
 *
 * \param ucUnitNumber number of memory unit that shall be checked
 *
 * \return position of next free drawer within memory unit
 * \image html VEEPROM_pucResetWriteAddress.emf "flowchart of VEEPROM_pucResetWriteAddress()"
 * \image latex VEEPROM_pucResetWriteAddress.eps "flowchart of VEEPROM_pucResetWriteAddress()"

 */
static uint8_t *VEEPROM_pucResetWriteAddress ( uint8_t ucUnitNumber )
{
    uint8_t       *pucCurrentAddress;
    uint8_t       *pucEmptyAreaBase;
    tDrawerID    tDrawerIdentification;
    uint8_t        ucOffsetNextDrawer = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bReadRetVal;
#endif
    /**
     * \internal
     * \section secSf4 Outline of function
     */
    uint16_t uiTempOffset = VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    pucCurrentAddress = (VEEPROM_apucUnitStart[ ucUnitNumber ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN );
    pucEmptyAreaBase = VEEPROM_pucGetFreeAreaBase(ucUnitNumber, pucCurrentAddress);
    /**
     * \internal
     * \subsection subSf4_2 Finding End
     * step ahead over all drawers, until field containing DrawerIdentifier contains a blank, unwritten 0xFF.
     *
     */
    if ( FLASH_bEnableAccess() != false )
    {
        do
        {
            /*
                Msg(4:0488) Performing pointer arithmetic. 
                MISRA-C:2004 Rule 17.4 <next> 
                Pointer arithmetic is checked.
                (a) VEEPROM_apucFlashPage contains start address of given flash page
                (b) uiTempOffset contains offset to next drawer
                (c) VEEPROM_ucOffsetDrawerId is the offset of the drawer-ID to start of drawer.
                thus (a) + (b) + c  is position of next drawer-ID for next iteration
            */
            pucCurrentAddress = (VEEPROM_apucUnitStart[ ucUnitNumber ] + uiTempOffset);
            if ( (uiTempOffset + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1U)) <= VEEPROM_uiUnitSize)
            {
#if defined(DEBUG) && (DEBUG != 0x00u)
                bReadRetVal = FLASH_bRead(pucCurrentAddress + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tDrawerIdentification, (uint16_t)sizeof(tDrawerIdentification));
                ASSERT_vEXPR ( bReadRetVal != false );
                VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bReadRetVal);
#else
                (void) FLASH_bRead(pucCurrentAddress + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&tDrawerIdentification, (uint16_t)sizeof(tDrawerIdentification));
#endif
                ucOffsetNextDrawer = VEEPROM_ucOffsetDrawerPayload
                                         + VEEPROM_ucObserverGetBruttoPayloadLength( tDrawerIdentification );
                ASSERT_vEXPR( uiTempOffset < ( UINT_MAX - ucOffsetNextDrawer ) );
                uiTempOffset = uiTempOffset + (uint16_t)ucOffsetNextDrawer;
            }
            else
            {
                /* magic number achieved*/
                break;
            }
        }
        while ( (( tDrawerIdentification != VEEPROM_tInvalidDrawer ) || ( pucCurrentAddress < pucEmptyAreaBase ))
              && (uiTempOffset <= VEEPROM_uiUnitSize ) );
        /**
         * \internal
         * actually position is after last drawer. This is the location for the next drawer to store.
         * End has been found. Return its address.
         */
    }
    else
    {
        pucCurrentAddress = (uint8_t  *)NULL;
    }
    (void)FLASH_bDisableAccess();
    return pucCurrentAddress;
}
/* VEEPROM_bWriteWithoutCheck()*/
/**
 * \brief writing data to VEEPROM without checking if remaining space left in Flash is large enough
 *
 * \pre remaining space in Flash is big enough to hold new data
 * 
 * \param tLabel DrawerID to write to
 * \param pucData Buffer containing data
 * \param ucDataLen length of Buffer to write
 */
static bool VEEPROM_bWriteWithoutCheck ( tDrawerID tLabel, const uint8_t *pucData, uint8_t ucDataLen )
{
    uint8_t    *pucDrawerStartAddress =  VEEPROM_pucWriteAddress;
    uint8_t     ucPosition;
    bool      bRetVal;
    uint8_t     ucIndex;
    uint8_t     ucCrc = 0;
    // to get real payload len including unused cells for cellspace != 0
    uint8_t     ucPayloadLen;
    tDrawerID tReadOutDrawerId; 
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    /* comment - enable FLASH-Write and write DrawerIdentifier. Then write all data of payload*/
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
        bRetVal = FLASH_bWrite( pucDrawerStartAddress + VEEPROM_ucOffsetDrawerId, (tWriteData *)(void *)&tLabel, (uint16_t)sizeof(tDrawerID));
        ASSERT_vEXPR( bRetVal != false);
        /* remaining consistency of data stored in FLASH:*/
        /* If there was an interference while writing drawer ID it can be that the wrong drawer ID was written to FLASH. - If this happens, actually (wrong) written drawer ID is taken as input to get its drawer length to set new valid WriteAddress.*/
        /* - In this case the data to write is not written and the return value of the function is false. */
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucDrawerStartAddress + VEEPROM_ucOffsetDrawerId, (uint8_t *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucDrawerStartAddress + VEEPROM_ucOffsetDrawerId, (uint8_t *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
#endif
        ucPayloadLen = VEEPROM_ucObserverGetBruttoPayloadLength( tReadOutDrawerId );
        if (tReadOutDrawerId == tLabel )
        {
            /* start calculation of crc*/
            CRC_vUpdateCrc( &ucCrc, HSUP_ucGetHighByte(tLabel));
            CRC_vUpdateCrc( &ucCrc, HSUP_ucGetLowByte(tLabel));
            VEEPROM_pucWriteAddress = &pucDrawerStartAddress[ VEEPROM_ucOffsetDrawerPayload ];
            (void)FLASH_bWrite(VEEPROM_pucWriteAddress, (const uint8_t *)(void *) pucData, ucDataLen);
            ASSERT_vEXPR( bRetVal != false);
            /* continue calculation of crc*/
            for ( ucPosition = 0; ucPosition < ucDataLen; ucPosition++ )
            {
                CRC_vUpdateCrc( &ucCrc, pucData[ ucPosition ] );
            }
            /* comment - finally write CRC or validity field*/
            /**
             * \internal
             * ... set Write-Pointer to Address of consistency-Field and <b>write consistency-Field</b>
             * ... finally update Write-Pointer and disable Flash operations 
             */
            bRetVal = FLASH_bWriteByte( &pucDrawerStartAddress[ 0 ], ucCrc );
            ASSERT_vEXPR( bRetVal != false);
        }
        else
        {
            ASSERT_vEXPR( tReadOutDrawerId == tLabel);
            bRetVal = false;
        }
        ucIndex =  ucPayloadLen + VEEPROM_ucOffsetDrawerPayload;
        VEEPROM_pucWriteAddress = &pucDrawerStartAddress[ ucIndex ];
    }
    else
    {
        ASSERT_vEXPR( bRetVal != false);
    }
    (void) FLASH_bDisableAccess();
    return bRetVal;
}
#else
/* VEEPROM_bDefrag()*/
/**
 * \brief defrag data from full flash page
 * 
 * \pre Destination Page is assumed to be clear. 
 *
 * Data of defragmented ucSource FlashPage is defragmented by copying 
 * cached data of all valid drawers to destination page. 
 * Validity of cached drawers is stored in #CACHE_aucDrawerCacheValidVeeprom.
 * When copying process has finished, fragmented old page is cleared.
 * New defragmented page is set to be VEEPROM_ucActivePage.
 *
 * \note if VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG is defined, then values used
 *       for defragmentation are read out directly from full flash page. This takes
 *       more time, but results in smaller code for small systems.
 * 
 *
 * \param ucSource page number to copy data from
 * \param ucDestination page number to copy data to
 * 
 * \return status of defragmentation:
 *          - false, if error while writing or flash page full
 *          - true,  if defragmentation succeeded with no errors.
 * \image html VEEPROM_bDefrag.emf "flowchart of VEEPROM_bDefrag()"
 * \image latex VEEPROM_bDefrag.eps "flowchart of VEEPROM_bDefrag()"
 */
static bool VEEPROM_bDefrag ( uint8_t ucSource, uint8_t ucDestination )
{
    uint8_t  ucActualDrawer = 0;
    uint8_t  ucDrawerPayloadLen;

    bool bRetVal = false;
    uint8_t ucNumberOfDrawers = VEEPROM_ucGetNumberOfDrawers();
    uint8_t* pucCacheAddress;
    uint8_t aucNumberPageTransfers[ 2 ];
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
    uint8_t* pucData;
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
    VEEPROM_pucWriteAddress = VEEPROM_apucFlashPage[ ucDestination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
    if ( VEEPROM_bIsPageErased(ucDestination) == false )
    {
        /* error state - destination must be erased*/
        _VEEPROM_vResetWatchdogTimer();
        VEEPROM_vBeforePageErase();
        bRetVal = FLASH_bEnableAccess();
        if ( false != bRetVal )
        {
            bRetVal = FLASH_bErase(  VEEPROM_apucFlashPage[ ucDestination ] );
            (void) FLASH_bDisableAccess();
            _VEEPROM_vResetWatchdogTimer();
            if ( bRetVal != false )
            {
                VEEPROM_vSetPageEraseMark( ucDestination );
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
    /* comment - increment page transfer counter*/
    /**
     * \internal
     * \subsection subSf6_2 PageTransferCounter
     *  - read out PageTransferCounter high and low byte
     *  - increment PageTransferCounter: check if high byte has to be incremented too
     *  - write incremented PageTransferCounter
     */
    /* read out page erase counter of full page and increment it*/
#if defined(DEBUG) && (DEBUG != 0x00u)
    bRetVal = FLASH_bRead( (tAddress *)(void *)VEEPROM_apucFlashPage[ ucSource ], &aucNumberPageTransfers[ 0 ], 2);
    ASSERT_vEXPR( false != bRetVal );
#else
    (void) FLASH_bRead( (tAddress *)(void *)VEEPROM_apucFlashPage[ ucSource ], &aucNumberPageTransfers[ 0 ], 2);
#endif
    if ( aucNumberPageTransfers[ 1 ] == 0xFFu )
    {
        aucNumberPageTransfers[ 0 ]++;
        if ( aucNumberPageTransfers[ 0 ] == 0x00u )
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
            aucNumberPageTransfers[ 1 ]++;
        }
    }
    aucNumberPageTransfers[ 1 ]++;
    /**
     * \subsection subSf6_3 Defragmenting data
     * for every drawer, ...
     */
    while ( ucActualDrawer < ucNumberOfDrawers )
    {
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
        /* comment - defrag data using values from full page*/
        /**
         * ... try to get latest data of actual drawer from fully fragmented page
         * ... if no error while getting latest data of actual drawer, 
         *     copy to new page
         * ... go on with next drawer.
         */
        pucData = VEEPROM_pucGetDataPayloadFromPage( ucSource, ucActualDrawer );
        if ( pucData != 0)
        {
            ucDrawerPayloadLen = VEEPROM_ucGetDrawerSize( ucActualDrawer );
            ASSERT_vEXPR(   ucDrawerPayloadLen + VEEPROM_pucWriteAddress
                            <= VEEPROM_apucFlashPage[ ucDestination ] + VEEPROM_uiFlashPageSize );
            bRetVal =  VEEPROM_bWriteWithoutCheck( ucActualDrawer, pucData, ucDrawerPayloadLen );
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
        if ( 0u != CACHE_bIsCacheValidVeeprom( ucActualDrawer) )
        {
            pucCacheAddress =  CACHE_pucGetCacheAddressVEEPROM( ucActualDrawer );
            ucDrawerPayloadLen = VEEPROM_ucGetDrawerSize( ucActualDrawer );
            ASSERT_vEXPR ( VEEPROM_pucWriteAddress < (VEEPROM_apucFlashPage[ ucDestination ] + VEEPROM_uiFlashPageSize) );
            bRetVal = VEEPROM_bWriteWithoutCheck( ucActualDrawer, pucCacheAddress, ucDrawerPayloadLen );
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
        ucActualDrawer++;
    }
    /* finally write page erase counter of new page and erase old (full) page if everything ok*/
    /* finally write updated page erase counter*/
    if (false != bRetVal)
    {
        bRetVal = FLASH_bEnableAccess();
        if (false != bRetVal)
        {
            bRetVal = FLASH_bWrite( VEEPROM_apucFlashPage[ ucDestination ], &aucNumberPageTransfers[ 0 ], 2 );
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
            bRetVal = FLASH_bErase(  VEEPROM_apucFlashPage[ ucSource ] );
            (void) FLASH_bDisableAccess();
            _VEEPROM_vResetWatchdogTimer();
            if ( bRetVal != false )
            {
                VEEPROM_vSetPageEraseMark( ucSource );
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
    VEEPROM_ucActivePage = ucDestination;
    return bRetVal;
}
/* VEEPROM_bIsPageErased()*/
/**
 * \brief checks if desired flash page is completely erased
 *
 * \param ucFlashPageNumber Number of Flash Page
 * \return 
 *          - true: flash page is erased completely.
 *          - false: at least one cell of flash page is not erased.
 */
static bool VEEPROM_bIsPageErased ( uint8_t ucFlashPageNumber )
{
    bool bIsEmpty = true;
    uint8_t* pucReadAddress;
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        start of page + length of page == end of page
    */
    uint8_t* pucEndAddress = VEEPROM_apucFlashPage[ ucFlashPageNumber ] + VEEPROM_uiFlashPageSize;
    /* test memory contents*/
    pucReadAddress = VEEPROM_apucFlashPage[ ucFlashPageNumber ];
    do
    {
        if ( *pucReadAddress != VEEPROM_UNWRITTEN_FLASH_CELL )
        {
            bIsEmpty = false;
        }
        ++pucReadAddress;
    }
    while ( (bIsEmpty == true) && ((uint32_t)pucReadAddress < (uint32_t)pucEndAddress) );
    return bIsEmpty;
}
/* VEEPROM_ucGetNextPage()*/
/**
 * \brief get page-number of next page
 * 
 * 
 * \param ucPage page number
 * 
 * \return page number of next page
 */
static uint8_t VEEPROM_ucGetNextPage ( uint8_t ucPage )
{
    return (uint8_t)( ( ucPage + 1u) % VEEPROM_ucNumberOfPages );
}
/* VEEPROM_uiGetUsedSpace()*/
/**
 * \brief evaluate bytes already used for given page
 *
 * \note page must not be actually in use. This is why VEEPROM_pucWriteAddress must not be correct for this page.
 * \param ucPage pagenumber
 * 
 * \return number of bytes (including header and page erase counter) already used of page
 */
static uint16_t VEEPROM_uiGetUsedSpace ( uint8_t ucPage )
{
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked:
        (a) VEEPROM_pucResetWriteAddress(..) returns next free write-address in given Page
        (b) VEEPROM_apicFlashPage contains the start address of this page.
        thus (a) - (b) results in actually used space of given flash-page.
    */
    return (uint16_t)( (uint32_t)VEEPROM_pucResetWriteAddress( ucPage ) - (uint32_t)VEEPROM_apucFlashPage[ ucPage ] );
}
/* VEEPROM_pucResetWriteAddress()*/
/**
 * \brief retrieve first free drawer-position of desired FlashPage
 *
 * if FlashPage is full, the next theoretical position after end of flash page is returned.
 * This is not a valid write address. VEEPROM_bWrite() will handle this and initiate a defragmentation
 *
 * \param ucFlashPage number of FlashPage that shall be 
 *
 * \return position of next free drawer within FlashPage
 * \image html VEEPROM_pucResetWriteAddress.emf "flowchart of VEEPROM_pucResetWriteAddress()"
 * \image latex VEEPROM_pucResetWriteAddress.eps "flowchart of VEEPROM_pucResetWriteAddress()"

 */
static uint8_t* VEEPROM_pucResetWriteAddress ( uint8_t ucFlashPage )
{
    uint8_t *pucCurrentAddress;
    uint8_t *pucEmptyAreaBase;
    uint8_t  ucDrawerIdentification;
    uint8_t  ucOffsetNextDrawer = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bReadRetVal;
#endif
    /**
     * \internal
     * \section secSf4 Outline of function
     */
    uint16_t uiTempOffset = VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    pucCurrentAddress = (uint8_t *)(void *)(VEEPROM_apucFlashPage[ ucFlashPage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN );
    pucEmptyAreaBase = VEEPROM_pucGetFreeAreaBase(ucFlashPage, pucCurrentAddress);
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
            (b) uiTempOffset contains offset to next drawer
            (c) VEEPROM_ucOffsetDrawerId is the offset of the drawer-ID to start of drawer.
            thus (a) + (b) + c  is position of next drawer-ID for next iteration
        */
        pucCurrentAddress = (uint8_t *)(void *)(VEEPROM_apucFlashPage[ ucFlashPage ] + uiTempOffset);
        if ( (uiTempOffset + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1u)) <= VEEPROM_uiFlashPageSize )
        {
#if defined(DEBUG) && (DEBUG != 0x00u)
            bReadRetVal = FLASH_bRead(pucCurrentAddress + VEEPROM_ucOffsetDrawerId, &ucDrawerIdentification, 1);
            ASSERT_vEXPR ( bReadRetVal != false );
            VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bReadRetVal);
#else
            (void) FLASH_bRead(pucCurrentAddress + VEEPROM_ucOffsetDrawerId, &ucDrawerIdentification, 1);
#endif
            ucOffsetNextDrawer = VEEPROM_ucOffsetDrawerPayload
                                     + VEEPROM_ucObserverGetBruttoPayloadLength( ucDrawerIdentification );
            ASSERT_vEXPR( uiTempOffset < ( UINT_MAX - ucOffsetNextDrawer ) );
            uiTempOffset = uiTempOffset + (uint16_t)ucOffsetNextDrawer;
        }
        else
        {
            /* magic number achieved*/
            break;
        }
    }
    while ( (( ucDrawerIdentification != VEEPROM_UNWRITTEN_FLASH_CELL ) || ( pucCurrentAddress < pucEmptyAreaBase ))
          && ( uiTempOffset <=  VEEPROM_uiFlashPageSize )
            );
    /**
     * \internal
     * actually position is after last drawer. This is the location for the next drawer to store.
     * End has been found. Return its address.
     */
    return pucCurrentAddress;
}
/* VEEPROM_bWriteWithoutCheck()*/
/**
 * \brief writing data to VEEPROM without checking if remaining space left in Flash is large enough
 *
 * \pre remaining space in Flash is big enough to hold new data
 * 
 * \param ucLabel DrawerID to write to
 * \param pucData Buffer containing data
 * \param ucDataLen length of Buffer to write
 */
static bool VEEPROM_bWriteWithoutCheck ( uint8_t ucLabel, const uint8_t* pucData, uint8_t ucDataLen )
{
    uint8_t* pucDrawerStartAddress =  VEEPROM_pucWriteAddress;
    uint8_t  ucPosition;
    bool   bRetVal;
    uint8_t  ucIndex;
    uint8_t  ucCrc = 0;
    // to get real payload len including unused cells for cellspace != 0
    uint8_t  ucPayloadLen;
    uint8_t ucReadOutDrawerId; 
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    /* comment - enable FLASH-Write and write DrawerIdentifier. Then write all data of payload*/
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
        bRetVal = FLASH_bWriteByte( pucDrawerStartAddress + VEEPROM_ucOffsetDrawerId, ucLabel );
        ASSERT_vEXPR( bRetVal != false);
        (void) FLASH_bDisableAccess();
        /* remaining consistency of data stored in FLASH:*/
        /* If there was an interference while writing drawer ID it can be that the wrong drawer ID was written to FLASH. - If this happens, actually (wrong) written drawer ID is taken as input to get its drawer length to set new valid WriteAddres. */
        /* - In this case the data to write is not written and the return value of the function is false. */
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucDrawerStartAddress + VEEPROM_ucOffsetDrawerId, (uint8_t*)&ucReadOutDrawerId, 1 );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucDrawerStartAddress + VEEPROM_ucOffsetDrawerId, (tReadData *)(void *)&ucReadOutDrawerId, 1 );
#endif
        ucPayloadLen = VEEPROM_ucObserverGetBruttoPayloadLength( ucReadOutDrawerId );
        if (ucReadOutDrawerId == ucLabel )
        {
            /* start calculation of crc*/
            CRC_vUpdateCrc( &ucCrc, ucLabel);//CRC_vUpdateCrc( pucCrc, ucLabel);
            VEEPROM_pucWriteAddress =  &pucDrawerStartAddress[ VEEPROM_ucOffsetDrawerPayload ];
            bRetVal = FLASH_bEnableAccess();
            if ( false != bRetVal )
            {
                (void)FLASH_bWrite(VEEPROM_pucWriteAddress, (tWriteData*)(void *)pucData, (uint16_t)ucDataLen);
                ASSERT_vEXPR( bRetVal != false);
                (void) FLASH_bDisableAccess();
                /* continue calculation of crc*/
                for ( ucPosition = 0; ucPosition < ucDataLen; ucPosition++ )
                {
                    CRC_vUpdateCrc( &ucCrc, pucData[ ucPosition ] );
                }
                /* comment - finally write CRC or validity field*/
                /**
                 * \internal
                 * ... set Write-Pointer to Address of consistency-Field and <b>write consistency-Field</b>
                 * ... finally update Write-Pointer and disable Flash operations 
                 */
                bRetVal = FLASH_bEnableAccess();
                if ( false != bRetVal )
                {
                    bRetVal = FLASH_bWriteByte( &pucDrawerStartAddress[ 0 ], ucCrc );
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
            ASSERT_vEXPR( ucReadOutDrawerId == ucLabel);
            bRetVal = false;
        }
        ucIndex =  ucPayloadLen + VEEPROM_ucOffsetDrawerPayload;
        VEEPROM_pucWriteAddress = &pucDrawerStartAddress[ ucIndex ];
    }
    else
    {
        ASSERT_vEXPR( bRetVal != false);
    }
    return bRetVal;
}
#endif //defined(VEEPROM_LARGE)

#if defined(DEBUG) && (DEBUG != 0x00u)
/* VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified()*/
/** \brief under construction */
static void VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bool bValue)
{

}
#endif
/* Global Functions*/

#if defined(VEEPROM_LARGE)
/* bool  VEEPROM_bDoDefragNow()*/
bool VEEPROM_bDoDefragNow ( void )
{
    bool bRetVal;
    if ( VEEPROM_tTaskState == VEEP_READY )
    {
        bRetVal = VEEPROM_bDefrag ( (uint8_t)VEEPROM_ucActiveUnit, VEEPROM_ucGetNextUnit( VEEPROM_ucActiveUnit ) );
        ASSERT_vEXPR( bRetVal==true );
        VEEPROM_vDefragEnded();
        return bRetVal;
    }
    else
    {
        return false;
    }
}
/* VEEPROM_bInit()*/
bool  VEEPROM_bInit ( void )
{
    //! \callgraph
    uint8_t ucNumberOfNonEmtyUnits = 0;
    uint8_t ucOtherUnit            = 0;
    uint8_t ucInterruptedUnit;
    uint8_t ucCheckedUnit;
    uint8_t ucPageCounter;
    bool  bRetVal;
    VEEPROM_tTaskState = VEEP_UNINITIALIZED;
    /* check for interrupted page-transfer*/
    /** 
     * \internal
     * \section sec2 Outline of function
     * 
     * \subsection sub2_1 Are flash pages o.k.?
     * First <b>check out</b> how many <b>pages</b> are currently <b>in use</b>: 
     * If first possible position for a cabinet has a entry in the Drawer-Identifier-Field, then
     * this page is in use. Its location is evaluated in pucAddressFirstDrawer.
     */
    (void)FLASH_bEnableAccess();
    for ( ucCheckedUnit = 0; ucCheckedUnit < VEEPROM_ucNumberOfUnits; ucCheckedUnit++ )
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
        if ( (VEEPROM_bIsUnitErased (ucCheckedUnit) == false) && (VEEPROM_bUnitDataValid(ucCheckedUnit) != false) )
        {
            ucNumberOfNonEmtyUnits++;
            ucOtherUnit = VEEPROM_ucActiveUnit;
            VEEPROM_ucActiveUnit = ucCheckedUnit;
        }
    }
    (void)FLASH_bDisableAccess();

    switch ( ucNumberOfNonEmtyUnits )
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
            VEEPROM_ucActiveUnit = VEEPROM_DEFAULT_UNIT;
            for (ucCheckedUnit = 0; ucCheckedUnit < VEEPROM_ucNumberOfUnits; ++ucCheckedUnit)
            {
                VEEPROM_vEnsureUnitIsErased( ucCheckedUnit );
            }
            /*
                Msg(4:0488) Performing pointer arithmetic. 
                MISRA-C:2004 Rule 17.4
                Pointer arithmetic is checked.
                (a) VEEPROM_apucFlashPage contains start address of default page
                (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
                thus (a) + (b) is address of first drawer of default page
            */
            VEEPROM_pucWriteAddress = VEEPROM_apucUnitStart[ VEEPROM_DEFAULT_UNIT ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
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
            VEEPROM_pucWriteAddress = VEEPROM_pucResetWriteAddress( VEEPROM_ucActiveUnit );
            for (ucCheckedUnit = 0; ucCheckedUnit < VEEPROM_ucNumberOfUnits; ++ucCheckedUnit)
            {
                if ( ucCheckedUnit != VEEPROM_ucActiveUnit )
                {
                    VEEPROM_vEnsureUnitIsErased( ucCheckedUnit );
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
            uint16_t ActiveUnit_UsedSpace = VEEPROM_uiGetUsedSpace( VEEPROM_ucActiveUnit );
            uint16_t OtherUnit_UsedSpace  = VEEPROM_uiGetUsedSpace( ucOtherUnit );
            /* set smaller page as interrupted page*/
            /*
                Msg(4:0488) Performing pointer arithmetic. 
                MISRA-C:2004 Rule 17.4
                Pointer arithmetic is checked.
                VEEPROM_uiGetUsedSpace (..) returns used space of given page
            */
            if ( ActiveUnit_UsedSpace > OtherUnit_UsedSpace )
            {
                ucInterruptedUnit = ucOtherUnit;
            }
            else
            {
                ucInterruptedUnit = VEEPROM_ucActiveUnit;
                VEEPROM_ucActiveUnit = ucOtherUnit;
            }
            /* check larger page for interrupted page erase (00 00 00 00 00 00 .. .. fe 1f 2a .. .. .. ff ff)*/
            /* if larger page has interrupted page transfer*/
            /* -  do not erase smaller page, that is actually set as interrupted page.*/
            /* -  erase larger page*/
            /* -  set smaller page as active page.*/
            if ( VEEPROM_bCheckForInterruptedUnitErase( VEEPROM_ucActiveUnit ) != false )
            {
                /* page erase of full page was interrupted due to reset. In this case the smaller page*/
                /* contains already all latest data, because erase of full page is first started when transfer of latest data has been finished.*/
                /* erase fuller page, that no longer contains valid data.*/
                /* swap active and interrupted pages.*/
                ucOtherUnit = ucInterruptedUnit;
                ucInterruptedUnit = VEEPROM_ucActiveUnit;
                VEEPROM_ucActiveUnit = ucOtherUnit;
            }
            else
            {
                /* there was no interrupted unit erase. In this case a reset occured while transfering data from active(full) page to transfer-page.*/
                /* In this case the smaller page probably does not contain all latest data, but the fuller page does.*/
                if ( VEEPROM_bCheckForDefragmentedAndNotErasedUnit( VEEPROM_ucActiveUnit, ucInterruptedUnit ) != false)
                {
                    /* In this case full page was defragmented, new valid drawer values are in the new page, but erase process of full page did not start yet. If this happened, begin of new page contains page erase counter incremented by 1 compared with page erase counter of previous page*/
                    /* erase fuller page.*/
                    /* swap active and interrupted pages.*/
                    ucOtherUnit = ucInterruptedUnit;
                    ucInterruptedUnit = VEEPROM_ucActiveUnit;
                    VEEPROM_ucActiveUnit = ucOtherUnit;
                }
                else
                {
                    /* erase smaller page, that is already set as interrupted page.*/
                }
            }
            // delete interrupted page
            bRetVal = FLASH_bEnableAccess();
            if ( false != bRetVal )
            {
                //erase page has not been finished before. Do erase all pages in unit once more
                for (ucPageCounter = 0; ucPageCounter < VEEPROM_ucFlashPagesPerUnit; ucPageCounter++)
                {
                    bRetVal = FLASH_bErase(  VEEPROM_apucUnitStart[ ucInterruptedUnit ] + (uint16_t)(ucPageCounter * VEEPROM_uiTotalPageSize) );
                    ASSERT_vEXPR( bRetVal != false);
                    _VEEPROM_vResetWatchdogTimer();
                }
                (void) FLASH_bDisableAccess();
                VEEPROM_vSetUnitEraseMark( ucInterruptedUnit );
            }
            else
            {
                ASSERT_vEXPR( bRetVal != false);
            }
            VEEPROM_pucWriteAddress = VEEPROM_pucResetWriteAddress( VEEPROM_ucActiveUnit );
            for (ucCheckedUnit = 0; ucCheckedUnit < VEEPROM_ucNumberOfUnits; ++ucCheckedUnit)
            {
                if ( ucCheckedUnit != VEEPROM_ucActiveUnit )
                {
                    VEEPROM_vEnsureUnitIsErased( ucCheckedUnit );
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
/* VEEPROM_bWrite()*/
bool  VEEPROM_bWrite ( tDrawerID tLabel, const uint8_t *pucData, uint8_t ucDataLen )
{
    bool   bRetVal;
    if ( VEEPROM_tTaskState != VEEP_READY )
    {
        return false;
    }
    ASSERT_vEXPR( tLabel < VEEPROM_tGetNumberOfDrawers() );
    /* some assertions*/
    ASSERT_vEXPR( ucDataLen == VEEPROM_ucGetDrawerSize( tLabel ) );
    ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
    ASSERT_vEXPR( ucDataLen > 0 );
    /* check if value to write is different from latest one stored in FLASH.*/
    /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do write new data if it is different from latest one stored.*/
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
    uint8_t aucBuffer[VEEPROM_MAX_DATA_LEN];
    uint8_t ucIndex;
    bool bDifferentData = false;
    ASSERT_vEXPR (VEEPROM_MAX_DATA_LEN >= ucDataLen);
    VEEPROM_bRead( tLabel, aucBuffer, ucDataLen);
    for (ucIndex = 0; ucIndex < ucDataLen; ucIndex++)
    {
        if (aucBuffer[ucIndex] != pucData[ucIndex])
        {
            bDifferentData == true;
            break;
        }
    }
    /* - if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do if data to write is different from latest one stored.*/
    if (false != bDifferentData)
      {
#endif
    /* comment - check if we have to defrag first*/
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
    if (   VEEPROM_uiGetFreeSpace() < (uint16_t)( ucDataLen + VEEPROM_ucOffsetDrawerPayload ) )
#else
    ASSERT_vEXPR ( VEEPROM_ucCellSize > 0 );
    if ( VEEPROM_uiGetFreeSpace() < (((((uint16_t)ucDataLen + (uint16_t)(VEEPROM_ucCellSize - 1U) ) / (uint16_t)VEEPROM_ucCellSize ) * (uint16_t)VEEPROM_ucCellSize ) + (uint16_t)VEEPROM_ucOffsetDrawerPayload ) )
#endif

    {
        /**
         * only start process of defragmentation if allowed from application
         */
        if ( false != VEEPROM_bApprobateDefrag() )
        {
            bRetVal = VEEPROM_bDoDefragNow();
            /* if cached values are not used for defragmentation, we have to write new values after process of defragmentation has finished.*/
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
            if ( false != bRetVal )
            {
                bRetVal = VEEPROM_bWriteWithoutCheck (ucLabel, pucData, ucDataLen);
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
        bRetVal = VEEPROM_bWriteWithoutCheck (tLabel, pucData, ucDataLen);
        ASSERT_vEXPR( bRetVal != false);
    }
    return bRetVal;
    /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, do not write data if data to write equals latest one stored.*/
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
      }
    /* otherwise return true (data is already up to date)*/
    else
      {
         return true;
      }
#endif
}
/* VEEPROM_ucRead()*/
uint8_t VEEPROM_ucRead ( tDrawerID tLabel, uint8_t *pucData, uint8_t ucDataLen )
{
    uint8_t  ucCrc  = 0     ;
    uint8_t *pucCrc = &ucCrc;
    uint8_t  ucIndex        ;
    bool   bEmptyAreaFound;
    uint8_t    *pucCheckedDrawerStartAddress;
    uint8_t    *pucCurrentAddress           ;
    uint8_t    *pucFoundPayload = NULL      ;
    tDrawerID tReadOutDrawerId            ;
    uint8_t     ucReadOutCrc                ;
    /**
     * \internal
     * \section secSf5 Outline of function 
     * \subsection subSf5_1 Preparation    
     * retrieve start address where to search for the desired Drawer
     */
    uint16_t uiTempOffset = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    if ( VEEPROM_tTaskState != VEEP_READY )
    {
        return 0U;
    }
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        (a) VEEPROM_apucFlashPage contains start address of active page
        (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
        thus (a) + (b) is position of first drawer on active page.
    */
    pucCheckedDrawerStartAddress = VEEPROM_apucUnitStart[ VEEPROM_ucActiveUnit ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    (void)FLASH_bEnableAccess();
    do
    {
        /**
         * read out CRC-field
         */
        pucCurrentAddress = &pucCheckedDrawerStartAddress[ uiTempOffset ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&ucReadOutCrc, 1 );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&ucReadOutCrc, 1 );
#endif
        /**
         * read out drawerID field
         */
        pucCurrentAddress = &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerId + uiTempOffset ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
#endif
        /** 
         * if not at end of written segment of flash-page
         */
        if ( (tReadOutDrawerId != VEEPROM_tInvalidDrawer) || (VEEPROM_bIsUnitRemainderEmpty(VEEPROM_ucActiveUnit, pucCurrentAddress) == false) )
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
                ASSERT_vEXPR (  VEEPROM_ucGetDrawerSize( tReadOutDrawerId ) == ucDataLen );
#if defined(DEBUG) && (DEBUG != 0x00u)
                bDbgRetVal = FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ], (tReadData *)(void *)pucData, ucDataLen );
                ASSERT_vEXPR ( false != bDbgRetVal );
                VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
                (void) FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ], (tReadData *)(void *)pucData, ucDataLen );
#endif
                /**
                 * calc crc over drawerID and payload
                 */
                {
                    ucCrc = 0;
                    CRC_vUpdateCrc( pucCrc, HSUP_ucGetHighByte(tLabel));
                    CRC_vUpdateCrc( pucCrc, HSUP_ucGetLowByte(tLabel));
                    for (ucIndex = 0 ; ucIndex < ucDataLen; ++ucIndex ) 
                    {
                        CRC_vUpdateCrc( pucCrc, pucData[ ucIndex ] );
                    }
                }
                /**
                 * if calculated crc matches stored one, we have found a valid and correct drawer
                 */
                if ( ucCrc == ucReadOutCrc )
                {
                    pucFoundPayload = (pucCheckedDrawerStartAddress + uiTempOffset);
                }
            }
            // go on with next drawer ...
            uiTempOffset +=  (uint16_t)VEEPROM_ucOffsetDrawerPayload + (uint16_t)VEEPROM_ucObserverGetBruttoPayloadLength(tReadOutDrawerId);
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
        (a) pucCheckedDrawerStartAddress is address to check for next iteration
        (b) VEEPROM_apucFlashPage contains start address of active page
        (c) VEEPROM_uiFlashPageSize is size of flash-page
        thus (a) - (b) must be smaller than (c) if regarded address is in range of active page.
    */
    while (   ( bEmptyAreaFound == false )
             && (((uiTempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + (uint16_t)(VEEPROM_ucOffsetDrawerPayload - 1U))   <  VEEPROM_uiUnitSize )
            );
    if ( pucFoundPayload != NULL)
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead  ( &pucFoundPayload[ VEEPROM_ucOffsetDrawerPayload ], (tReadData *)(void *)pucData, ucDataLen );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead  ( &pucFoundPayload[ VEEPROM_ucOffsetDrawerPayload ], (tReadData *)(void *)pucData, ucDataLen );
#endif
    }
    else
    {
        ucDataLen = 0;
    }
    (void)FLASH_bDisableAccess();
    return ucDataLen;
}
/* VEEPROM_uiGetFreeSpace()*/
uint16_t  VEEPROM_uiGetFreeSpace ( void )
{
    uint8_t *pucFreeSpace;
    if ( VEEPROM_tTaskState == VEEP_READY )
    {
        /*
            Msg(4:0488) Performing pointer arithmetic. 
            MISRA-C:2004 Rule 17.4 <next> 
            Pointer arithmetic is checked.
            (a) VEEPROM_apucFlashPage contains start address of active page
            (b) VEEPROM_uiFlashPageSize is length of page
            (c) VEEPROM_pucWriteAddress is write address on active page
            thus (a) + (b) - (c) contains free space in bytes left on active page
        */
        pucFreeSpace = (uint8_t *)((VEEPROM_apucUnitStart[ VEEPROM_ucActiveUnit ] + VEEPROM_uiUnitSize ) - VEEPROM_pucWriteAddress );
        /* if page is full,  VEEPROM_pucWriteAddress is set after active page in use. This results in a "negative" pucFreeSpace. But because of unsigned variables, we have to check, if it out of range. In this case we do not have any space left and return "0". */
        if (pucFreeSpace > (uint8_t *)(uint32_t)VEEPROM_uiUnitSize)
        {
            return 0;
        }
        else
        {
            return (uint16_t)((uint32_t)pucFreeSpace);
        }
    }
    else
    {
        return 0;
    }
}
/* VEEPROM_bReadWholeActivePage()*/
bool VEEPROM_bReadWholeActivePage (void)
{
    uint8_t  ucCrc  = 0     ;
    uint8_t *pucCrc = &ucCrc;
    uint8_t  ucIndex        ;
    bool   bEmptyAreaFound;
    bool bRetVal = true;
    uint8_t     *pucCheckedDrawerStartAddress;
    uint8_t     *pucCurrentAddress;
    tDrawerID  tReadOutDrawerId;
    uint8_t      ucReadOutCrc;
    uint8_t      ucDrawerLength;
    uint8_t     *pucDrawerCacheAddress;
    uint8_t     *pucEmptyAreaBase = NULL;
    uint8_t aucBuffer[VEEPROM_MAX_DATA_LEN];
    /**
     * \internal
     * \section secSf5 Outline of function 
     * \subsection subSf5_1 Preparation    
     * retrieve start address where to search for the desired Drawer
     */
    uint16_t uiTempOffset = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        (a) VEEPROM_apucFlashPage contains start address of active page
        (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
        thus (a) + (b) is position of first drawer on active page.
    */
    pucCheckedDrawerStartAddress = VEEPROM_apucUnitStart[ VEEPROM_ucActiveUnit ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    do
    {
        /*
         * read out drawerID field
         */
        pucCurrentAddress = &pucCheckedDrawerStartAddress[ uiTempOffset + (uint16_t)VEEPROM_ucOffsetDrawerId ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
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
            if ( pucEmptyAreaBase == NULL )
            {
                pucEmptyAreaBase = VEEPROM_pucGetFreeAreaBase(VEEPROM_ucActiveUnit, pucCurrentAddress);
            }
            if ( pucCurrentAddress < pucEmptyAreaBase )
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
                ucDrawerLength = VEEPROM_ucGetDrawerSize( tReadOutDrawerId );
                if ( ucDrawerLength > 0U )
                {
                    /*
                     * read out CRC-field
                     */
#if defined(DEBUG) && (DEBUG != 0x00u)
                    bDbgRetVal = FLASH_bRead ( &pucCheckedDrawerStartAddress[ uiTempOffset ], (tReadData *)(void *)&ucReadOutCrc, 1 );
                    ASSERT_vEXPR ( false != bDbgRetVal );
                    VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
                    (void) FLASH_bRead ( &pucCheckedDrawerStartAddress[ uiTempOffset ], (tReadData *)(void *)&ucReadOutCrc, 1 );
#endif
#if defined(DEBUG) && (DEBUG != 0x00u)
                    bDbgRetVal = FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ], (tReadData *)(void *)&aucBuffer[0], ucDrawerLength );
                    ASSERT_vEXPR ( false != bDbgRetVal );
                    VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
                    (void) FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ],(tReadData *)(void *)&aucBuffer[0], ucDrawerLength );
#endif
                    /*
                     * calc crc over drawerID and payload
                     */
                    {
                        ucCrc = 0;
                        CRC_vUpdateCrc( pucCrc, HSUP_ucGetHighByte(tReadOutDrawerId));
                        CRC_vUpdateCrc( pucCrc, HSUP_ucGetLowByte(tReadOutDrawerId));
                        for (ucIndex = 0 ; ucIndex < ucDrawerLength; ++ucIndex ) 
                        {
                            CRC_vUpdateCrc( pucCrc, aucBuffer[ ucIndex ] );
                        }
                    }
                    /*
                     * if calculated crc matches stored one, we have found a valid and correct drawer
                     */
                    if ( ucCrc == ucReadOutCrc )
                    {
                        pucDrawerCacheAddress = VEEPROM_pucGetDrawerAddress( tReadOutDrawerId );
                        if ( pucDrawerCacheAddress != NULL )
                        {
                            for (ucIndex = 0 ; ucIndex < ucDrawerLength; ++ucIndex ) 
                            {
                                if ( pucDrawerCacheAddress[ucIndex] != aucBuffer[ucIndex] )
                                {
                                    pucDrawerCacheAddress[ucIndex] = aucBuffer[ucIndex];
                                }
                            }
                            VEEPROM_vAfterCacheVariableSet( tReadOutDrawerId);
                        }
                        else
                        {
                            /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                            ASSERT_vEXPR ( pucDrawerCacheAddress != NULL );
#endif
                            bRetVal = false;
                        }
                    }
                    else
                    {
                        /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                        ASSERT_vEXPR ( ucCrc == ucReadOutCrc );
#endif
                        bRetVal = false;
                    }
                }
                else
                {
                    /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                    ASSERT_vEXPR ( ucDrawerLength > 0 );
#endif
                    bRetVal = false;
                }
            }
            else
            {
                /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR ( tReadOutDrawerId < VEEPROM_tGetNumberOfDrawers() );
#endif
                bRetVal = false;
            }
            // go on with next drawer ...
            uiTempOffset +=  (uint16_t)(VEEPROM_ucOffsetDrawerPayload + VEEPROM_ucObserverGetBruttoPayloadLength(tReadOutDrawerId));
        }
    }
    while (   ( bEmptyAreaFound == false )
             && (((uiTempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + (uint16_t)(VEEPROM_ucOffsetDrawerPayload - 1U))  <=  VEEPROM_uiUnitSize )
            );
    return bRetVal;
}
#else
/* bool VEEPROM_bDoDefragNow()*/
bool VEEPROM_bDoDefragNow ( void )
{
    bool bRetVal;
    bRetVal = VEEPROM_bDefrag ( (uint8_t)VEEPROM_ucActivePage, VEEPROM_ucGetNextPage( VEEPROM_ucActivePage ) );
    ASSERT_vEXPR( bRetVal==true );
    VEEPROM_vDefragEnded();
    return bRetVal;
}
/* VEEPROM_bInit()*/
bool VEEPROM_bInit ( void )
{
    //! \callgraph
    uint8_t ucNumberOfNonEmtyPages = 0;
    uint8_t ucOtherPage            = 0;
    uint8_t ucInterruptedPage;
    uint8_t ucCheckedPage;

    //uint8_t *pucAddressFirstDrawerId;
    bool  bRetVal;
    /* check for interrupted page-transfer*/
    /** 
     * \internal
     * \section sec2 Outline of function
     * 
     * \subsection sub2_1 Are flash pages o.k.?
     * First <b>check out</b> how many <b>pages</b> are currently <b>in use</b>: 
     * If first possible position for a cabinet has a entry in the Drawer-Identifier-Field, then
     * this page is in use. Its location is evaluated in pucAddressFirstDrawer.
     */
    for ( ucCheckedPage = 0; ucCheckedPage < VEEPROM_ucNumberOfPages; ucCheckedPage++ )
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
        if ( (VEEPROM_bIsPageRemainderEmpty(ucCheckedPage, VEEPROM_apucFlashPage[ ucCheckedPage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) == false) && (VEEPROM_bPageDataValid(ucCheckedPage) != false) )
        {
            ucNumberOfNonEmtyPages++;
            ucOtherPage = VEEPROM_ucActivePage;
            VEEPROM_ucActivePage = ucCheckedPage;
        }
    }
    switch ( ucNumberOfNonEmtyPages )
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
            VEEPROM_ucActivePage = VEEPROM_DEFAULT_PAGE;
            for (ucCheckedPage = 0; ucCheckedPage < VEEPROM_ucNumberOfPages; ++ucCheckedPage)
            {
                VEEPROM_vEnsurePageIsErased( ucCheckedPage );
            }
            /*
                Msg(4:0488) Performing pointer arithmetic. 
                MISRA-C:2004 Rule 17.4
                Pointer arithmetic is checked.
                (a) VEEPROM_apucFlashPage contains start address of default page
                (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
                thus (a) + (b) is address of first drawer of default page
            */
            VEEPROM_pucWriteAddress = VEEPROM_apucFlashPage[ VEEPROM_DEFAULT_PAGE ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
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
            VEEPROM_pucWriteAddress = VEEPROM_pucResetWriteAddress( VEEPROM_ucActivePage );
            for (ucCheckedPage = 0; ucCheckedPage < VEEPROM_ucNumberOfPages; ++ucCheckedPage)
            {
                if ( ucCheckedPage != VEEPROM_ucActivePage )
                {
                    VEEPROM_vEnsurePageIsErased( ucCheckedPage );
                }
            }
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
            uint16_t ActivePage_UsedSpace = VEEPROM_uiGetUsedSpace(VEEPROM_ucActivePage);
            uint16_t OtherPage_UsedSpace  = VEEPROM_uiGetUsedSpace(ucOtherPage);
            /* set smaller page as interrupted page*/
            /*
                Msg(4:0488) Performing pointer arithmetic. 
                MISRA-C:2004 Rule 17.4
                Pointer arithmetic is checked.
                VEEPROM_uiGetUsedSpace (..) returns used space of given page
            */
            if ( ActivePage_UsedSpace > OtherPage_UsedSpace )
            {
                ucInterruptedPage = ucOtherPage;
            }
            else
            {
                ucInterruptedPage = VEEPROM_ucActivePage;
                VEEPROM_ucActivePage = ucOtherPage;
            }
            /* check larger page for interrupted page erase (00 00 00 00 00 00 .. .. fe 1f 2a .. .. .. ff ff)*/
            /* if larger page has interrupted page transfer*/
            /* -  do not erase smaller page, that is actually set as interrupted page.*/
            /* -  erase larger page*/
            /* -  set smaller page as active page.*/
            if ( VEEPROM_bCheckForInterruptedPageErase( VEEPROM_ucActivePage ) != false )
            {
                /* page erase of full page was interrupted due to reset. In this case the smaller page*/
                /* contains already all latest data, because erase of full page is first started when transfer of latest data has been finished.*/
                /* erase fuller page, that no longer contains valid data.*/
                /* swap active and interrupted pages.*/
                ucOtherPage = ucInterruptedPage;
                ucInterruptedPage = VEEPROM_ucActivePage;
                VEEPROM_ucActivePage = ucOtherPage;
            }
            else
            {
                /* there was no interrupted page erase. In this case a reset occured while transfering data from active(full) page to transfer-page.*/
                /* In this case the smaller page probably does not contain all latest data, but the fuller page does.*/
                if ( VEEPROM_bCheckForDefragmentedAndNotErasedPage( VEEPROM_ucActivePage, ucInterruptedPage ) != false)
                {
                    /* In this case full page was defragmented, new valid drawer values are in the new page, but erase process of full page did not start yet. If this happened, begin of new page contains page erase counter incremented by 1 compared with page erase counter of previous page*/
                    /* erase fuller page.*/
                    /* swap active and interrupted pages.*/
                    ucOtherPage = ucInterruptedPage;
                    ucInterruptedPage = VEEPROM_ucActivePage;
                    VEEPROM_ucActivePage = ucOtherPage;
                }
                else
                {
                    /* erase smaller page, that is already set as interrupted page.*/
                }
            }
            // delete interrupted page
            bRetVal = FLASH_bEnableAccess();
            if ( false != bRetVal )
            {
                bRetVal = FLASH_bErase(  VEEPROM_apucFlashPage[ ucInterruptedPage ] );
                ASSERT_vEXPR( bRetVal != false);
                (void) FLASH_bDisableAccess();
                VEEPROM_vSetPageEraseMark( ucInterruptedPage );
            }
            else
            {
                ASSERT_vEXPR( bRetVal != false);
            }
            VEEPROM_pucWriteAddress = VEEPROM_pucResetWriteAddress( VEEPROM_ucActivePage );
            for (ucCheckedPage = 0; ucCheckedPage < VEEPROM_ucNumberOfPages; ++ucCheckedPage)
            {
                if ( ucCheckedPage != VEEPROM_ucActivePage )
                {
                    VEEPROM_vEnsurePageIsErased( ucCheckedPage );
                }
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
/* VEEPROM_bWrite()*/
bool VEEPROM_bWrite ( uint8_t ucLabel, const uint8_t* pucData, uint8_t ucDataLen )
{
    bool   bRetVal;
    ASSERT_vEXPR( ucLabel < VEEPROM_ucGetNumberOfDrawers() );
    /* some assertions*/
    ASSERT_vEXPR( ucDataLen == VEEPROM_ucGetDrawerSize( ucLabel ) );
    ASSERT_vEXPR( ucDataLen <= VEEPROM_MAX_DATA_LEN );
    ASSERT_vEXPR( ucDataLen > 0 );
    /* check if value to write is different from latest one stored in FLASH.*/
    /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do write new data if it is different from latest one stored.*/
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
    uint8_t aucBuffer[VEEPROM_MAX_DATA_LEN];
    uint8_t ucIndex;
    bool bDifferentData = false;
    ASSERT_vEXPR (VEEPROM_MAX_DATA_LEN >= ucDataLen);
    VEEPROM_bRead( ucLabel, aucBuffer, ucDataLen);
    for (ucIndex = 0; ucIndex < ucDataLen; ucIndex++)
    {
        if (aucBuffer[ucIndex] != pucData[ucIndex])
        {
            bDifferentData == true;
            break;
        }
    }
    /* - if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do if data to write is different from latest one stored.*/
    if (false != bDifferentData)
      {
#endif
    /* comment - check if we have to defrag first*/
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
    if (   VEEPROM_uiGetFreeSpace() < (uint16_t)( ucDataLen + VEEPROM_ucOffsetDrawerPayload ) )
#else
    ASSERT_vEXPR ( VEEPROM_ucCellSize > 0 );
    if (   VEEPROM_uiGetFreeSpace() < ( ( (  ( (uint16_t)ucDataLen + ((uint16_t)VEEPROM_ucCellSize - 1u) ) // \    length of Payload
                                                          / (uint16_t)VEEPROM_ucCellSize                   //  +=> quantisized to
                                                      ) * (uint16_t)VEEPROM_ucCellSize )                   // /    cellsize
                                                      + (uint16_t)VEEPROM_ucOffsetDrawerPayload            // ===> length of Header
                                                 )
        )
#endif

    {
        /**
         * only start process of defragmentation if allowed from application
         */
        if ( false != VEEPROM_bApprobateDefrag() )
        {
            bRetVal = VEEPROM_bDoDefragNow();
            /* if cached values are not used for defragmentation, we have to write new values after process of defragmentation has finished.*/
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
            if ( false != bRetVal )
            {
                bRetVal = VEEPROM_bWriteWithoutCheck (ucLabel, pucData, ucDataLen);
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
        bRetVal = VEEPROM_bWriteWithoutCheck (ucLabel, pucData, ucDataLen);
        ASSERT_vEXPR( bRetVal != false);
    }
    return bRetVal;
    /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, do not write data if data to write equals latest one stored.*/
#ifdef VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE
      }
    /* otherwise return true (data is already up to date)*/
    else
      {
         return true;
      }
#endif
}
/* VEEPROM_ucRead()*/
uint8_t VEEPROM_ucRead ( uint8_t ucLabel, uint8_t* pucData, uint8_t ucDataLen )
{
    uint8_t  ucCrc  = 0     ;
    uint8_t *pucCrc = &ucCrc;
    uint8_t  ucIndex        ;
    bool   bEmptyAreaFound;
    uint8_t *pucCheckedDrawerStartAddress;
    uint8_t *pucCurrentAddress           ;
    uint8_t *pucFoundPayload = NULL      ;
    uint8_t ucReadOutDrawerId            ;
    uint8_t ucReadOutCrc                 ;
    /**
     * \internal
     * \section secSf5 Outline of function 
     * \subsection subSf5_1 Preparation    
     * retrieve start address where to search for the desired Drawer
     */
    uint16_t uiTempOffset = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        (a) VEEPROM_apucFlashPage contains start address of active page
        (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
        thus (a) + (b) is position of first drawer on active page.
    */
    pucCheckedDrawerStartAddress = VEEPROM_apucFlashPage[ VEEPROM_ucActivePage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    do
    {
        /**
         * read out CRC-field
         */
        pucCurrentAddress = &pucCheckedDrawerStartAddress[ uiTempOffset ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucCurrentAddress, (uint8_t*)&ucReadOutCrc, 1 );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&ucReadOutCrc, 1 );
#endif
        /**
         * read out drawerID field
         */
        pucCurrentAddress = &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerId + uiTempOffset ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucCurrentAddress, (uint8_t*)&ucReadOutDrawerId, 1 );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&ucReadOutDrawerId, 1 );
#endif
        /** 
         * if not at end of written segment of flash-page
         */
        if ( (ucReadOutDrawerId != VEEPROM_UNWRITTEN_FLASH_CELL) || (VEEPROM_bIsPageRemainderEmpty(VEEPROM_ucActivePage, pucCurrentAddress) == false) )
        {
            bEmptyAreaFound = false;
            /**
             * if we have the correct drawer ID
             */
            if ( ucLabel == ucReadOutDrawerId )
            {
                /**
                 * read out payload of drawer
                 */
                ASSERT_vEXPR (  VEEPROM_ucGetDrawerSize( ucReadOutDrawerId ) == ucDataLen );
#if defined(DEBUG) && (DEBUG != 0x00u)
                bDbgRetVal = FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ], pucData, ucDataLen );
                ASSERT_vEXPR ( false != bDbgRetVal );
                VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
                (void) FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ], pucData, ucDataLen );
#endif
                /**
                 * calc crc over drawerID and payload
                 */
                {
                    ucCrc = 0;
                    CRC_vUpdateCrc( pucCrc, ucLabel );
                    for (ucIndex = 0 ; ucIndex < ucDataLen; ++ucIndex ) 
                    {
                        CRC_vUpdateCrc( pucCrc, pucData[ ucIndex ] );
                    }
                }
                /**
                 * if calculated crc matches stored one, we have found a valid and correct drawer
                 */
                if ( ucCrc == ucReadOutCrc )
                {
                    pucFoundPayload = (uint8_t*)(void *)(pucCheckedDrawerStartAddress + uiTempOffset);
                }
            }
            // go on with next drawer ...
            uiTempOffset +=  (uint16_t)VEEPROM_ucOffsetDrawerPayload + (uint16_t)VEEPROM_ucObserverGetBruttoPayloadLength(ucReadOutDrawerId);
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
        (a) pucCheckedDrawerStartAddress is address to check for next iteration
        (b) VEEPROM_apucFlashPage contains start address of active page
        (c) VEEPROM_uiFlashPageSize is size of flash-page
        thus (a) - (b) must be smaller than (c) if regarded address is in range of active page.
    */
    while (   ( bEmptyAreaFound == false )
             && ( ((uiTempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1u)) < VEEPROM_uiFlashPageSize )
            );
    if ( pucFoundPayload != NULL)
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead  ( &pucFoundPayload[ VEEPROM_ucOffsetDrawerPayload ],
                            pucData,
                            ucDataLen );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void)FLASH_bRead( &pucFoundPayload[ VEEPROM_ucOffsetDrawerPayload ], pucData, ucDataLen );
#endif
        return ucDataLen;
    }
    else
    {
        return 0;
    }
}
/* VEEPROM_uiGetFreeSpace()*/
uint16_t VEEPROM_uiGetFreeSpace ( void )
{
    uint8_t *pucFreeSpace;
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        (a) VEEPROM_apucFlashPage contains start address of active page
        (b) VEEPROM_uiFlashPageSize is length of page
        (c) VEEPROM_pucWriteAddress is write address on active page
        thus (a) + (b) - (c) contains free space in bytes left on active page
    */
    pucFreeSpace = (uint8_t*)((VEEPROM_apucFlashPage[ VEEPROM_ucActivePage ] + VEEPROM_uiFlashPageSize ) - VEEPROM_pucWriteAddress );
    /* if page is full,  VEEPROM_pucWriteAddress is set after active page in use. This results in a "negative" pucFreeSpace. But because of unsigned variables, we have to check, if it out of range. In this case we do not have any space left and return "0". */
    if (pucFreeSpace > (uint8_t*)(uint32_t)VEEPROM_uiFlashPageSize)
    {
        return 0;
    }
    else
    {
        return (uint16_t)((uint32_t)pucFreeSpace);
    }
}
/* VEEPROM_bReadWholeActivePage()*/
bool VEEPROM_bReadWholeActivePage(void)
{
    uint8_t  ucCrc  = 0;
    uint8_t *pucCrc = &ucCrc;
    uint8_t  ucIndex;
    bool   bEmptyAreaFound;
    bool bRetVal = true;
    uint8_t *pucCheckedDrawerStartAddress;
    uint8_t *pucCurrentAddress;
    uint8_t  ucReadOutDrawerId;
    uint8_t  ucReadOutCrc;
    uint8_t  ucDrawerLength;
    uint8_t *pucDrawerCacheAddress;
    uint8_t *pucEmptyAreaBase = NULL;
    uint8_t aucBuffer[VEEPROM_MAX_DATA_LEN];
    /**
     * \internal
     * \section secSf5 Outline of function 
     * \subsection subSf5_1 Preparation    
     * retrieve start address where to search for the desired Drawer
     */
    uint16_t uiTempOffset = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        (a) VEEPROM_apucFlashPage contains start address of active page
        (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
        thus (a) + (b) is position of first drawer on active page.
    */
    pucCheckedDrawerStartAddress = VEEPROM_apucFlashPage[ VEEPROM_ucActivePage ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    do
    {
        /*
         * read out drawerID field
         */
        pucCurrentAddress = &pucCheckedDrawerStartAddress[ uiTempOffset + (uint16_t)VEEPROM_ucOffsetDrawerId ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&ucReadOutDrawerId, 1 );
        ASSERT_vEXPR ( false != bDbgRetVal );
        VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
        (void) FLASH_bRead ( pucCurrentAddress, (tReadData *)(void *)&ucReadOutDrawerId, 1 );
#endif
        /* 
         * if not at end of written segment of flash-page
         */
        if ( ucReadOutDrawerId != VEEPROM_UNWRITTEN_FLASH_CELL )
        {
            bEmptyAreaFound = false;
        }
        else
        {
            if ( pucEmptyAreaBase == NULL )
            {
                pucEmptyAreaBase = VEEPROM_pucGetFreeAreaBase(VEEPROM_ucActivePage, pucCurrentAddress);
            }
            if ( pucCurrentAddress < pucEmptyAreaBase )
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
            if ( ucReadOutDrawerId < VEEPROM_ucGetNumberOfDrawers() )
            {
                /*
                 * read out payload of drawer
                 */
                ucDrawerLength = VEEPROM_ucGetDrawerSize( ucReadOutDrawerId );
                if ( ucDrawerLength > 0u )
                {
                    /*
                     * read out CRC-field
                     */
#if defined(DEBUG) && (DEBUG != 0x00u)
                    bDbgRetVal = FLASH_bRead ( &pucCheckedDrawerStartAddress[ uiTempOffset ], (tReadData *)(void *)&ucReadOutCrc, 1 );
                    ASSERT_vEXPR ( false != bDbgRetVal );
                    VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
                    (void) FLASH_bRead ( &pucCheckedDrawerStartAddress[ uiTempOffset ], (tReadData *)(void *)&ucReadOutCrc, 1 );
#endif
#if defined(DEBUG) && (DEBUG != 0x00u)
                    bDbgRetVal = FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ], (tReadData *)(void *)&aucBuffer[0], ucDrawerLength );
                    ASSERT_vEXPR ( false != bDbgRetVal );
                    VEEPROM_vDbgGetRidOfMsgNotUsedAndAlwaysModified(bDbgRetVal);
#else
                    (void) FLASH_bRead ( &pucCheckedDrawerStartAddress[ (uint16_t)VEEPROM_ucOffsetDrawerPayload + uiTempOffset ],(tReadData *)(void *)&aucBuffer[0], ucDrawerLength );
#endif
                    /*
                     * calc crc over drawerID and payload
                     */
                    {
                        ucCrc = 0;
                        CRC_vUpdateCrc( pucCrc, ucReadOutDrawerId );
                        for (ucIndex = 0 ; ucIndex < ucDrawerLength; ++ucIndex ) 
                        {
                            CRC_vUpdateCrc( pucCrc, aucBuffer[ ucIndex ] );
                        }
                    }
                    /*
                     * if calculated crc matches stored one, we have found a valid and correct drawer
                     */
                    if ( ucCrc == ucReadOutCrc )
                    {
                        pucDrawerCacheAddress = VEEPROM_pucGetDrawerAddress( ucReadOutDrawerId );
                        if ( pucDrawerCacheAddress != NULL )
                        {
                            for (ucIndex = 0 ; ucIndex < ucDrawerLength; ++ucIndex ) 
                            {
                                if ( pucDrawerCacheAddress[ucIndex] != aucBuffer[ucIndex] )
                                {
                                    pucDrawerCacheAddress[ucIndex] = aucBuffer[ucIndex];
                                }
                            }
                            VEEPROM_vAfterCacheVariableSet( ucReadOutDrawerId);
                        }
                        else
                        {
                            /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                            ASSERT_vEXPR ( pucDrawerCacheAddress != NULL );
#endif
                            bRetVal = false;
                        }
                    }
                    else
                    {
                        /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                        ASSERT_vEXPR ( ucCrc == ucReadOutCrc );
#endif
                        bRetVal = false;
                    }
                }
                else
                {
                    /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                    ASSERT_vEXPR ( ucDrawerLength > 0 );
#endif
                    bRetVal = false;
                }
            }
            else
            {
                /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR ( ucReadOutDrawerId < VEEPROM_ucGetNumberOfDrawers() );
#endif
                bRetVal = false;
            }
            // go on with next drawer ...
            uiTempOffset +=  (uint16_t)VEEPROM_ucOffsetDrawerPayload + (uint16_t)VEEPROM_ucObserverGetBruttoPayloadLength(ucReadOutDrawerId);
        }
    }
    /*
        Msg(4:0488) Performing pointer arithmetic. 
        MISRA-C:2004 Rule 17.4 <next> 
        Pointer arithmetic is checked.
        try to run not over end of active page:
        (a) pucCheckedDrawerStartAddress is address to check for next iteration
        (b) VEEPROM_apucFlashPage contains start address of active page
        (c) VEEPROM_uiFlashPageSize is size of flash-page
        thus (a) - (b) must be smaller than (c) if regarded address is in range of active page.
    */
    while (   ( bEmptyAreaFound == false )
             && (((uiTempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_ucOffsetDrawerPayload - 1u))  <  VEEPROM_uiFlashPageSize )
            );
    return bRetVal;
}
#endif

/*lint -restore -e717*/
/*lint -restore -e553*/
/*lint -restore -e731*/
/*lint -restore -e923*/
/*lint -restore -e946*/
/*lint -restore -e947*/
/*lint -restore -e9033*/
/*lint -restore -e9005*/
/*lint -restore -e954 */
