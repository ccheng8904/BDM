/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB_FRAMEWORK_NG
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Implementation of Volatile EEPROM library for microcontrolers with write granularity 8 bytes( STM32G0/STM32G4/STM32L4 ).
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
#if defined(DEBUG) && (DEBUG != 0x00u)
#include <limits.h>
#include "ped_fw/veeprom/pedAssert.h"
#endif
#include "flash/flash.h"
#include "utility.h"
#include "nvmem/cache.h"
#include "hsup.h"

//lint -e923  cast from pointer to unsigned int
//lint -e9021 use of '#undef' is discouraged
//lint -e954  Pointer variable could be declared as pointing to const
//lint -e539  unexpected positive indentation
/* Definitions and Declarations*/
/* Definitions*/

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
#define VEEPROM_EMPTY_FLASH_BYTE           (0xFFu)
#define VEEPROM_EMPTY_CELL_CONTENT         (0xFFFFFFFFFFFFFFFFu)
#if defined(VEEPROM_LARGE)
#define VEEPROM_DEFAULT_UNIT               (0u) //!< default unit that is assigned when all units are still free (initial start)
#else
#warning "Short version of VEEPROM package not supported for this derivative ! Use large version."
#endif
/**
 * length of page transfer counter in bytes
 *
 * these are the first n bytes of page that do not have any header.
 */
#define VEEPROM_PAGE_TRANSFER_COUNTER_LEN  (8u)
#define VEEPROM_CRC_SIZE                   (2u)
#if defined(VEEPROM_MAX_DATA_LEN)
#undef  VEEPROM_MAX_DATA_LEN
#define VEEPROM_MAX_DATA_LEN               (36u)
#endif
/* when using this library without NVMEM-library,*/
/*~+:          _     _   _                         _ _       _       */
/*~+: ___  ___| |_  | |_| |__   ___   _____      _(_) |_ ___| |__  _ */
/*~+:  __|/ _ \ __| | __| '_ \ / _ \ / __\ \ /\ / / | __/ __| '_ \(_)*/
/*~+:\__ \  __/ |_  | |_| | | |  __/ \__ \\ V  V /| | || (__| | | |_ */
/*~+:|___/\___|\__|  \__|_| |_|\___| |___/ \_/\_/ |_|\__\___|_| |_(_)*/
/*~+:*/
/*~+:VEEPROM_USE_LIB_WITHOUT_NVMEM*/
/*~+:*/
/*~+:- to disable using cached values for process of defragmentation and*/
/*~+:- to enable check if data to write is different from actually data stored.*/
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
    VEEP_WRITE_CRC_ID_DATA_PART ,
    VEEP_VERIFY_CRC_ID_DATA_PART,
    VEEP_WRITE_NEXT_DATA_PART   ,
    VEEP_VERIFY_NEXT_DATA_PART  ,
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

/*typedef struct structDataRecord
{
    uint16_t CRC16;
    uint16_t DrawerId;
    uint8_t  Data[4];
}tDataRecord;  */

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
const tDrawerID VEEPROM_tInvalidDrawer = 0xFFFF;
/* variable for Handle task controlled process */
//static tDrawerID VEEPROM_tLabel; obsolete
#else
    #warning "Short version of VEEPROM package not supported for this derivative ! Use large version."
#endif
/**
 * actual write-position in active page
 */
static uint8_t *VEEPROM_pu8WriteAddress;
/* variables for Handle task controlled process */
static uint8_t VEEPROM_u8DataLen   ; ///< valid data length in data array
static uint8_t VEEPROM_u8WriteSteps; ///< number of write steps
static uint8_t VEEPROM_au8DataRecord[VEEPROM_MAX_DATA_LEN + (VEEPROM_FLASH_CELL_SIZE/2u)] __attribute__((aligned(2))); ///< drawer data array
/** Variable holds current task state */
static eVEEPROMtaskStateType VEEPROM_tTaskState = VEEP_UNINITIALIZED;

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
static uint32_t VEEPROM_u32AlignToCellSize(uint32_t u32Input);
#if defined(VEEPROM_LARGE)
static uint8_t VEEPROM_u8GetNextUnit(uint8_t u8Unit);
static uint16_t VEEPROM_u16GetUsedSpace(uint8_t u8Unit);
static bool VEEPROM_bIsUnitErased(uint8_t u8UnitNumber);
static uint8_t *VEEPROM_pu8ResetWriteAddress(uint8_t u8UnitNumber);
static bool VEEPROM_bWriteWithoutCheck(tDrawerID tLabel, const uint8_t *pu8Data, uint8_t u8DataLen );
#else
     #warning "Short version of VEEPROM package not supported for this derivative ! Use large version."
#endif
static bool VEEPROM_bDefrag( uint8_t u8Source, uint8_t u8Destination );
static eProcessResultType VEEPROM_tDoWriteStep(uint8_t u8PageNumber);
static eProcessResultType VEEPROM_tDoDefragStep(void);

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
/* VEEPROM_u32AlignToCellSize() */
/**
 * \brief Alignes input number to nearest value divisible by write cell size (8)
 *
 * \param u32Input number to check
 *
 * \return new value divisible by 8
 */
static uint32_t VEEPROM_u32AlignToCellSize(uint32_t u32Input)
{
    uint8_t u8ModuloResult;

    u8ModuloResult = (uint8_t)(u32Input % (uint32_t)VEEPROM_u8CellSize);
    if ( u8ModuloResult != 0u )
    { /* start place is not divisible by 8 */
        u32Input = u32Input + ((uint32_t)VEEPROM_u8CellSize - (uint32_t)u8ModuloResult);
    }
    return u32Input;
}
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
    return (uint8_t)( ( u8Unit + 1U) % VEEPROM_u8NumberOfUnits );
}
/* VEEPROM_u16GetUsedSpace()*/
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
    return (uint16_t)( (uint32_t)VEEPROM_pu8ResetWriteAddress( u8Unit ) - (uint32_t)VEEPROM_apu8UnitStart[ u8Unit ] );
}
/* VEEPROM_pu8ResetWriteAddress()*/
/**
 * \brief retrieve first free drawer-position of desired memory unit
 *
 * if memory unit is full, the next theoretical position after end of flash page is returned.
 * This is not a valid write address. VEEPROM_bWrite() will handle this and initiate a defragmentation
 *
 * \param u8UnitNumber number of memory unit that shall be checked
 *
 * \return position of next free drawer within memory unit
 */
static uint8_t *VEEPROM_pu8ResetWriteAddress( uint8_t u8UnitNumber )
{
    uint8_t *pu8CurrentAddress;
    uint8_t *pu8StartAddress;

    pu8StartAddress = VEEPROM_apu8UnitStart[ u8UnitNumber ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
    pu8CurrentAddress = VEEPROM_pu8GetFreeAreaBase(u8UnitNumber, pu8StartAddress);
    pu8CurrentAddress = (uint8_t *)VEEPROM_u32AlignToCellSize((uint32_t)pu8CurrentAddress);
    return pu8CurrentAddress;
}
/* VEEPROM_bDefrag()*/
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
static bool VEEPROM_bDefrag ( uint8_t u8Source, uint8_t u8Destination )
{
    tDrawerID tActualDrawer = 0;
    uint8_t   u8DrawerPayloadLen;
    bool      bRetVal = false;
    tDrawerID tNumberOfDrawers = VEEPROM_tGetNumberOfDrawers();
    uint8_t   u8PageCounter;
    const uint8_t  *pu8CacheAddress;
    uint64_t  u64PageTransfers;
#ifdef VEEPROM_NO_USE_OF_CACHED_VALUES_FOR_DEFRAG
    uint8_t  *pu8Data;
#endif
    /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apu8UnitStart contains start of destination unit
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is first valid write address on destination flash-page.
     */
    VEEPROM_pu8WriteAddress = VEEPROM_apu8UnitStart[ u8Destination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
    if ( VEEPROM_bIsUnitErased(u8Destination) == false )
    {
        /* error state - destination unit must be erased*/
        _VEEPROM_vResetWatchdogTimer();
        VEEPROM_vBeforePageErase();
        bRetVal = (bool)FLASH_bEnableAccess();
        if ( bRetVal != false )
        {
            //erase page has not been finished before. Do erase all pages in unit once more
            for (u8PageCounter = 0; u8PageCounter < VEEPROM_u8FlashPagesPerUnit; u8PageCounter++)
            {
                bRetVal = (bool)FLASH_bErase(  VEEPROM_apu8UnitStart[ u8Destination ] + (uint16_t)(u8PageCounter * VEEPROM_uiTotalPageSize) );
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR(bRetVal != false);
#endif
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
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR( bRetVal != false);
#endif
            }
        }
        else
        {
#if defined(DEBUG) && (DEBUG != 0x00u)
            ASSERT_vEXPR( bRetVal != false);
#endif
        }
    }
    /* comment - increment page transfer counter*/
    /**
     *  - read out PageTransferCounter
     *  - increment PageTransferCounter
     *  - write incremented PageTransferCounter
     */
    /* read out page erase counter of full page and increment it*/
#if defined(DEBUG) && (DEBUG != 0x00u)
    bRetVal = FLASH_bRead( VEEPROM_apu8UnitStart[ u8Source ], (tReadData *)(void *)&u64PageTransfers, VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
    ASSERT_vEXPR( false != bRetVal );
#else
    (void) FLASH_bRead( VEEPROM_apu8UnitStart[ u8Source ], (tReadData *)(void *)&u64PageTransfers, VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
#endif
    u64PageTransfers = (u64PageTransfers == 0xFFFFFFFFFFFFFFFFu) ? 1u : (u64PageTransfers + 1u) ;
    /**
     * Defragmenting data
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
        pu8Data = VEEPROM_pu8GetDataPayloadFromPage( u8Source, tActualDrawer );
        if ( pu8Data != 0)
        {
            u8DrawerPayloadLen = VEEPROM_ucGetDrawerSize( tActualDrawer );
            ASSERT_vEXPR(   u8DrawerPayloadLen + VEEPROM_pu8WriteAddress
                    <= VEEPROM_apu8UnitStart[ u8Destination ] + VEEPROM_uiUnitSize );
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
        if ( 0u != CACHE_bIsCacheValidVeeprom( tActualDrawer) )
        {
            pu8CacheAddress =  CACHE_pucGetCacheAddressVEEPROM( tActualDrawer );
            u8DrawerPayloadLen = VEEPROM_ucGetDrawerSize( tActualDrawer );
#if defined(DEBUG) && (DEBUG != 0x00u)
            ASSERT_vEXPR ( (uint32_t)VEEPROM_pu8WriteAddress < ((uint32_t)VEEPROM_apu8UnitStart[ u8Destination ] + (uint32_t)VEEPROM_uiUnitSize) );
#endif
            bRetVal = VEEPROM_bWriteWithoutCheck( tActualDrawer, pu8CacheAddress, u8DrawerPayloadLen );
            if ( false == bRetVal )
            {
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR(bRetVal != false);
#endif
                break;
            }
        }
        else
        {
            /* do nothing*/
            /**
             * if value in cache
             * not valid, go on
             * with next drawer.
             */
        }
#endif
        tActualDrawer++;
    }
    /* finally write page erase counter of new page and erase old (full) page if everything ok */
    /* finally write updated page erase counter */
    if (false != bRetVal)
    {
        bRetVal = (bool)FLASH_bEnableAccess();
        if (false != bRetVal)
        {
            bRetVal = (bool)FLASH_bWrite( VEEPROM_apu8UnitStart[ u8Destination ], (tWriteData *)(void *)&u64PageTransfers, VEEPROM_PAGE_TRANSFER_COUNTER_LEN );
            (void) FLASH_bDisableAccess();
            if ( false == bRetVal )
            {
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR(bRetVal != false);
#endif
            }
        }
        else
        {
#if defined(DEBUG) && (DEBUG != 0x00u)
            ASSERT_vEXPR(bRetVal != false);
#endif
        }
        /* if everything part of defragmenting data successfully ==> erase full page*/
        _VEEPROM_vResetWatchdogTimer();
        VEEPROM_vBeforePageErase();
        bRetVal = (bool)FLASH_bEnableAccess();
        if ( false != bRetVal )
        {
            //erase page has not been finished before. Do erase all pages in unit once more
            for (u8PageCounter = 0; u8PageCounter < VEEPROM_u8FlashPagesPerUnit; u8PageCounter++)
            {
                bRetVal = (bool)FLASH_bErase(  VEEPROM_apu8UnitStart[ u8Source ] + (uint16_t)(u8PageCounter * VEEPROM_uiTotalPageSize) );
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR(bRetVal != false);
#endif
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
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR( bRetVal != false);
#endif
            }
        }
        else
        {
#if defined(DEBUG) && (DEBUG != 0x00u)
            ASSERT_vEXPR(bRetVal != false);
#endif
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
static bool VEEPROM_bWriteWithoutCheck( tDrawerID tLabel, const uint8_t *pu8Data, uint8_t u8DataLen )
{
    uint8_t  *pu8DrawerStartAddress = VEEPROM_pu8WriteAddress;
    bool      bRetVal;
    uint8_t   u8Index;
    uint16_t  u16Crc = 0;
    // to get real payload len including unused cells for cellspace != 0
    uint8_t   u8PayloadLen;
    uint8_t   u8WriteSteps;
    tDrawerID tReadOutDrawerId;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    /* comment - enable FLASH-Write and write DrawerIdentifier. Then write all data of payload*/
    /**
     * Data to write is new or different from latest valid data stored.
     * First enable Flash operations ...
     * ... set Write-Pointer to Address of Identifier of Drawer and <b>write Identifier</b>
     * ... set Write-Pointer to start of Payload of Drawer and <b>write data</b>
     */
    bRetVal = (bool)FLASH_bEnableAccess();
    if ( false != bRetVal )
    {
        /* set label to its position */
        *((tDrawerID *)(void *)(VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId)) = tLabel;
        /* set data to its position */
        UTI_vMemCopy( pu8Data, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerPayload, (uint16_t)u8DataLen);
        /* fill rest of array with empty content */
        UTI_vMemFill(VEEPROM_au8DataRecord + (VEEPROM_u8OffsetDrawerPayload + u8DataLen), VEEPROM_EMPTY_FLASH_BYTE, (uint16_t) UTI_NELEMENTS(VEEPROM_au8DataRecord) - ((uint16_t)VEEPROM_u8OffsetDrawerPayload + (uint16_t)u8DataLen));
        /* calculate Crc */
        u16Crc = VEEPROM_vUpdateCrc( (uint32_t)sizeof(tDrawerID) + (uint32_t)u8DataLen, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId );
        /* set Crc to its position */
        *((uint16_t *)(void *)VEEPROM_au8DataRecord) = u16Crc;
        /* calculate number of write steps */
        u8WriteSteps = ((uint8_t)VEEPROM_u32AlignToCellSize( (uint32_t)VEEPROM_u8OffsetDrawerPayload + (uint32_t)u8DataLen)) / VEEPROM_u8CellSize;
        /* write first group of 8 */
        bRetVal = (bool)FLASH_bWrite( pu8DrawerStartAddress, (tWriteData *)(void *)VEEPROM_au8DataRecord, (uint16_t)VEEPROM_u8CellSize);
        if (bRetVal != false)
        {
            /* remaining consistency of data stored in FLASH:*/
            /* If there was an interference while writing drawer ID it can be that the wrong drawer ID was written to FLASH. - If this happens, actually (wrong) written drawer ID is taken as input to get its drawer length to set new valid WriteAddress.*/
            /* - In this case the data to write is not written and the return value of the function is false. */
#if defined(DEBUG) && (DEBUG != 0x00u)
            bDbgRetVal = (bool)FLASH_bRead ( pu8DrawerStartAddress + VEEPROM_u8OffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
            ASSERT_vEXPR ( false != bDbgRetVal );
#else
            (void) FLASH_bRead ( pu8DrawerStartAddress + VEEPROM_u8OffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
#endif
            u8PayloadLen = VEEPROM_u8ObserverGetBruttoPayloadLength( tReadOutDrawerId );
            if (tReadOutDrawerId == tLabel )
            {
                /* write rest groups of 8 */
                for(uint8_t counter=1u; counter < u8WriteSteps; counter++)
                {
                    bRetVal = (bool)FLASH_bWrite( pu8DrawerStartAddress + (counter * VEEPROM_u8CellSize), (tWriteData *)(void *)(VEEPROM_au8DataRecord + (counter * VEEPROM_u8CellSize)), (uint16_t)VEEPROM_u8CellSize);
                    if(bRetVal == false)
                    {
#if defined(DEBUG) && (DEBUG != 0x00u)
                        ASSERT_vEXPR( bRetVal != false);
#endif
                        break;
                    }
                }
            }
            else
            {
#if defined(DEBUG) && (DEBUG != 0x00u)
                ASSERT_vEXPR( tReadOutDrawerId == tLabel);
#endif
                bRetVal = false;
            }
            u8Index = (uint8_t) VEEPROM_u32AlignToCellSize((uint32_t)u8PayloadLen + (uint32_t)VEEPROM_u8OffsetDrawerPayload);
            VEEPROM_pu8WriteAddress = &pu8DrawerStartAddress[ u8Index ];
        }
        else
        {
#if defined(DEBUG) && (DEBUG != 0x00u)
            ASSERT_vEXPR( bRetVal != false);
#endif
        }
    }
    else
    {
        ASSERT_vEXPR( bRetVal != false);
    }
    (void) FLASH_bDisableAccess();
    return bRetVal;
}
/* VEEPROM_bIsUnitErased()*/
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
    uint64_t *p64ReadAddress;
    /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      start of page + length of page == end of page
     */
    uint64_t *p64EndAddress = (uint64_t *)(void *)(VEEPROM_apu8UnitStart[ u8UnitNumber ] + VEEPROM_uiUnitSize);
    /* test memory contents*/
    p64ReadAddress = (uint64_t *)(void *)VEEPROM_apu8UnitStart[ u8UnitNumber ];
    do
    {
        if ( *p64ReadAddress != VEEPROM_EMPTY_CELL_CONTENT )
        {
            bIsEmpty = false;
        }
        ++p64ReadAddress;
    }
    while ( (bIsEmpty == true) && ((uint32_t)p64ReadAddress < (uint32_t)p64EndAddress) );
    return bIsEmpty;
}
/* VEEPROM_tDoWriteStep()*/
static eProcessResultType VEEPROM_tDoWriteStep( uint8_t u8UnitNumber )
{
    eProcessResultType     tRetVal            = VEEP_PROCESS_ONGOING       ;
    static eWriteStepsType VEEPROM_tWriteStep = VEEP_WRITE_CRC_ID_DATA_PART;
    tDrawerID              tReadOutDrawerId      ;
    uint16_t               u16FreeSpace          ;
    static uint8_t        *VEEPROM_pu8DrawerStart;
    static uint8_t         VEEPROM_u8PayloadLen  ;
    static uint8_t         VEEPROM_u8Step        ;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bResult;
#endif
    switch (VEEPROM_tWriteStep)
    {
    case VEEP_WRITE_CRC_ID_DATA_PART:
    {
        VEEPROM_pu8DrawerStart = VEEPROM_pu8WriteAddress;
#if defined(DEBUG) && (DEBUG != 0x00u)
        /* debug version*/
        bResult = (bool)FLASH_bEnableAccess();
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
        bResult = (bool)FLASH_bWrite( VEEPROM_pu8DrawerStart, (tWriteData *)(void *)VEEPROM_au8DataRecord, (uint16_t)VEEPROM_u8CellSize);
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
        bResult = (bool)FLASH_bDisableAccess();
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
#else
        /* normal version*/
        (void)FLASH_bEnableAccess();
        (void)FLASH_bWrite( VEEPROM_pu8DrawerStart, (tWriteData *)(void *)VEEPROM_au8DataRecord, (uint16_t)VEEPROM_u8CellSize);
        (void)FLASH_bDisableAccess();
#endif
        VEEPROM_tWriteStep     = VEEP_VERIFY_CRC_ID_DATA_PART;
        break;
    }
    case VEEP_VERIFY_CRC_ID_DATA_PART:
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        /* debug version*/
        bResult = (bool)FLASH_bRead ( VEEPROM_pu8DrawerStart + VEEPROM_u8OffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tReadOutDrawerId) );
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
#else
        /* normal version*/
        (void) FLASH_bRead ( VEEPROM_pu8DrawerStart + VEEPROM_u8OffsetDrawerId, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tReadOutDrawerId) );
#endif
        VEEPROM_u8PayloadLen = VEEPROM_u8ObserverGetBruttoPayloadLength( tReadOutDrawerId );
        /* test if value was written correctly */
        if( false != UTI_bIsMemEqual(VEEPROM_pu8DrawerStart, VEEPROM_au8DataRecord, (uint16_t)(VEEPROM_u8CellSize)) )
        {
            /* good value */
            VEEPROM_u8Step = 1u;
            if( VEEPROM_u8Step < VEEPROM_u8WriteSteps )
            {
                VEEPROM_tWriteStep = VEEP_WRITE_NEXT_DATA_PART;
            }
            else
            {
                VEEPROM_tWriteStep = VEEP_DRAWER_FINISHED;
            }
        }
        else
        {
            /* wrong written value */
            VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
        }
        break;
    }
    case VEEP_WRITE_NEXT_DATA_PART:
    {
        if( VEEPROM_u8Step < VEEPROM_u8WriteSteps )
        {
#if defined(DEBUG) && (DEBUG != 0x00u)
            bResult = (bool)FLASH_bWrite( VEEPROM_pu8DrawerStart + (VEEPROM_u8Step * VEEPROM_u8CellSize), (tWriteData *)(void *)VEEPROM_au8DataRecord + (VEEPROM_u8Step * VEEPROM_u8CellSize), (uint16_t)VEEPROM_u8CellSize);
            if ( bResult == false )
            {
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            }
#else
            (void)FLASH_bEnableAccess();
            (void)FLASH_bWrite( VEEPROM_pu8DrawerStart + (VEEPROM_u8Step * VEEPROM_u8CellSize), (tWriteData *)(void *)(VEEPROM_au8DataRecord + (VEEPROM_u8Step * VEEPROM_u8CellSize)), (uint16_t)VEEPROM_u8CellSize);
            (void)FLASH_bDisableAccess();
            if ( (VEEPROM_u8Step + 1u) < VEEPROM_u8WriteSteps )
            {
                VEEPROM_u8Step++;
            }
            else
            {
                VEEPROM_tWriteStep = VEEP_VERIFY_NEXT_DATA_PART;
            }
#endif
        }
        else
        {
            VEEPROM_tWriteStep = VEEP_VERIFY_NEXT_DATA_PART;
        }
        break;
    }
    case VEEP_VERIFY_NEXT_DATA_PART:
    {
        if( false != UTI_bIsMemEqual(VEEPROM_pu8DrawerStart + VEEPROM_u8CellSize, VEEPROM_au8DataRecord + VEEPROM_u8CellSize, (((uint16_t)VEEPROM_u8WriteSteps - 1u) * (uint16_t)VEEPROM_u8CellSize)) )
        {
            VEEPROM_tWriteStep = VEEP_DRAWER_FINISHED;
        }
        else
        {
            VEEPROM_tWriteStep = VEEP_FIND_NEXT_POSITION;
        }
        break;
    }
    case VEEP_DRAWER_FINISHED:
    {
        VEEPROM_pu8WriteAddress = (uint8_t *)VEEPROM_u32AlignToCellSize((uint32_t)&VEEPROM_pu8DrawerStart[VEEPROM_u8OffsetDrawerPayload + VEEPROM_u8PayloadLen]);
        VEEPROM_tWriteStep = VEEP_WRITE_CRC_ID_DATA_PART;
        tRetVal = VEEP_PROCESS_SUCCESSFUL;
        break;
    }
    case VEEP_FIND_NEXT_POSITION:
    {
        /* some data byte not correctly written, */
        /* move pointer to next position and try again */
        u16FreeSpace = (uint16_t)(((uint32_t)VEEPROM_apu8UnitStart[ u8UnitNumber ] + (uint32_t)VEEPROM_uiUnitSize) - VEEPROM_u32AlignToCellSize((uint32_t)&VEEPROM_pu8DrawerStart[ VEEPROM_u8OffsetDrawerPayload + VEEPROM_u8PayloadLen]) );
        if ( u16FreeSpace >= VEEPROM_u32AlignToCellSize(((uint32_t)VEEPROM_u8OffsetDrawerPayload + (uint32_t)VEEPROM_u8PayloadLen)) )
        {
            /* move pointer to next position and try again*/
            /* Debug*/
#if defined(DEBUG) && (DEBUG != 0x00u)
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
#endif
            VEEPROM_pu8WriteAddress = (uint8_t *)VEEPROM_u32AlignToCellSize((uint32_t)&VEEPROM_pu8DrawerStart[VEEPROM_u8OffsetDrawerPayload + VEEPROM_u8PayloadLen]);
        }
        else
        {
            /* not enougt memory space on page*/
            /* Debug*/
#if defined(DEBUG) && (DEBUG != 0x00u)
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
#endif
            /* read out another drawer ID */
            tRetVal = VEEP_PROCESS_DEFRAG_NEED;
        }
        VEEPROM_tWriteStep = VEEP_WRITE_CRC_ID_DATA_PART;
        break;
    }
    default:
    {
        break;
    }
    }
    return(tRetVal);
}
/* VEEPROM_tDoDefragStep()*/
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
    uint16_t           u16Crc                                ;
#if !defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
    uint64_t          *p64MagicNumberPosition = NULL         ;
    uint64_t           u64MagicNumber                        ;
#endif
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bResult;
#endif
    static eDefragStepsType  VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
    static uint8_t   VEEPROM_u8Source                   ;
    static uint8_t   VEEPROM_u8Destination              ;
    static uint8_t   VEEPROM_u8CurUnitNr                ; //holds page number to be erased, source or destination
    static uint64_t  u64PageTransfers                   ;
    static tDrawerID VEEPROM_tActDefragDrawer           ;
    static uint8_t   VEEPROM_u8PageCounter              ;
    switch (VEEPROM_tDefragStep)
    {
    case VEEP_CHECK_PAGE_EMPTY:
    {
        VEEPROM_u8Source          = VEEPROM_u8ActiveUnit;
        VEEPROM_u8Destination     = VEEPROM_u8GetNextUnit( VEEPROM_u8ActiveUnit );
        VEEPROM_tActDefragDrawer = 0;
#ifdef TIME_MEASUREMENT
        VEEPROM_u16TimeBefore = (uint16_t)TIM_uiGetCircleMicroSeconds();
#endif
        if ( VEEPROM_bIsUnitErased( VEEPROM_u8Destination ) != false )
        {
#ifdef TIME_MEASUREMENT
            VEEPROM_u16TimeAfter = (uint16_t)TIM_uiGetCircleMicroSeconds();
            VEEPROM_u16TimeDifference = VEEPROM_u16TimeAfter - VEEPROM_u16TimeBefore;
#endif
            VEEPROM_tDefragStep = VEEP_READ_ERASE_COUNTER;
        }
        else
        {
#ifdef TIME_MEASUREMENT
            VEEPROM_u16TimeAfter = (uint16_t)TIM_uiGetCircleMicroSeconds();
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
        VEEPROM_pu8WriteAddress = VEEPROM_apu8UnitStart[ VEEPROM_u8Destination ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
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
        bResult = (bool)FLASH_bRead( (tAddress *)(void *)VEEPROM_apu8UnitStart[ VEEPROM_u8Source ], (tReadData *)(void *)&u64PageTransfers, VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
#else
        (void) FLASH_bRead((tAddress *)(void *)VEEPROM_apu8UnitStart[ VEEPROM_u8Source ],(tReadData *)(void *)&u64PageTransfers, VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
#endif
        u64PageTransfers = (u64PageTransfers == 0xFFFFFFFFFFFFFFFFu) ? 1u : (u64PageTransfers + 1u) ;
#if !defined(TEST_EXIT_AFTER_READ_ERASE_COUNTER)
        VEEPROM_tDefragStep = VEEP_DRAWER_WRITE_START;
#else
        /* only for test */
        VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
        VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
        tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
        break;
    }
    case VEEP_DRAWER_WRITE_START:
    {
        /* comment - defrag data using cached values*/
        /**
         * ... if cached value is valid, copy this value to new page
         * \note if cached value is invalid, no value has been stored up to now so do nothing.
         */
        while ( VEEPROM_tActDefragDrawer < VEEPROM_tGetNumberOfDrawers() )
        {
            if ( CACHE_bIsCacheValidVeeprom( VEEPROM_tActDefragDrawer) != 0u )
            {
                pu8CacheAddress =  CACHE_pucGetCacheAddressVEEPROM( VEEPROM_tActDefragDrawer );
                VEEPROM_u8DataLen   = VEEPROM_ucGetDrawerSize( VEEPROM_tActDefragDrawer );
                /* set label to its position */
                *((tDrawerID *)(void *)(VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId)) = VEEPROM_tActDefragDrawer;
                /* set data to its position */
                UTI_vMemCopy( pu8CacheAddress, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerPayload, (uint16_t)VEEPROM_u8DataLen);
                /* fill rest of array with empty content */
                UTI_vMemFill(VEEPROM_au8DataRecord + (VEEPROM_u8OffsetDrawerPayload + VEEPROM_u8DataLen), VEEPROM_EMPTY_FLASH_BYTE, (uint16_t) UTI_NELEMENTS(VEEPROM_au8DataRecord) - ((uint16_t)VEEPROM_u8OffsetDrawerPayload + (uint16_t)VEEPROM_u8DataLen));
                /* calculate Crc */
                u16Crc = VEEPROM_vUpdateCrc( (uint32_t)sizeof(tDrawerID) + (uint32_t)VEEPROM_u8DataLen, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId );
                /* set Crc to its position */
                *((uint16_t *)(void *)VEEPROM_au8DataRecord) = u16Crc;
                /* calculate number of write steps */
                VEEPROM_u8WriteSteps = ((uint8_t)VEEPROM_u32AlignToCellSize((uint32_t)VEEPROM_u8OffsetDrawerPayload + (uint32_t)VEEPROM_u8DataLen)) / VEEPROM_u8CellSize;
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
            /* nothing to write*/
#if !defined(TEST_EXIT_AFTER_ALL_DRIWERS_WRITTEN)
            VEEPROM_tDefragStep = VEEP_WRITE_ERASE_COUNTER;
#else
            /* only for test */
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
            /* only for test */
            VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
        }
        else
        {
            if ( (tWriteResult == VEEP_PROCESS_FAILURE) || (tWriteResult == VEEP_PROCESS_DEFRAG_NEED) )
            {
                /* Debug*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
#endif
                VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
                tRetVal = VEEP_PROCESS_FAILURE;
            }
            else
            {
                /* process ongoing*/
            }
        }
        break;
    }
    case VEEP_WRITE_ERASE_COUNTER:
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        /* debug version*/
        bResult = (bool)FLASH_bEnableAccess();
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
        bResult = (bool)FLASH_bWrite(VEEPROM_apu8UnitStart[ VEEPROM_u8Destination ], (tWriteData *)(void *)&u64PageTransfers, VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
        bResult = (bool)FLASH_bDisableAccess();
        if ( bResult == false )
        {
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
        VEEPROM_u8PageCounter = 0;
        VEEPROM_tDefragStep   = VEEP_PAGE_ERASE;
#else
        /* normal version*/
        (void)FLASH_bEnableAccess();
        (void)FLASH_bWrite(VEEPROM_apu8UnitStart[ VEEPROM_u8Destination ], (tWriteData *)(void *)&u64PageTransfers, VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
        (void)FLASH_bDisableAccess();
        VEEPROM_u8PageCounter = 0;
        VEEPROM_tDefragStep   = VEEP_PAGE_ERASE;

#endif
        break;
    }
    case VEEP_PAGE_ERASE:
    {
#if !defined(TEST_EXIT_AFTER_ERASE_COUNTER_WRITTEN)
        if ( VEEPROM_u8PageCounter == 0U )
        {
            VEEPROM_vBeforePageErase();
        }
#if defined(DEBUG) && (DEBUG != 0x00u)
        /* debug version*/
        if ( VEEPROM_u8PageCounter < VEEPROM_u8FlashPagesPerUnit )
        {
            bResult = (bool)FLASH_bEnableAccess();
            if ( bResult == false )
            {
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            }
            /* erase only one page of whole flash unit*/
            bResult = (bool)FLASH_bErase(VEEPROM_apu8UnitStart[ VEEPROM_u8CurUnitNr ] + ((uint16_t)VEEPROM_u8PageCounter * VEEPROM_uiTotalPageSize));
            if ( bResult == false )
            {
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            }
            bResult = (bool)FLASH_bDisableAccess();
            if ( bResult == false )
            {
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            }
            VEEPROM_u8PageCounter++;
        }
        else
        {
            VEEPROM_u8PageCounter = 0;
            VEEPROM_tDefragStep     = VEEP_WRITE_ERASE_MARK;
        }
#else
        /* normal version*/
        if ( VEEPROM_u8PageCounter < VEEPROM_u8FlashPagesPerUnit )
        {
            (void)FLASH_bEnableAccess();
            (void)FLASH_bErase(VEEPROM_apu8UnitStart[ VEEPROM_u8CurUnitNr ] + ((uint16_t)VEEPROM_u8PageCounter * VEEPROM_uiTotalPageSize));
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
        /* only for test */
        VEEPROM_pu8WriteAddress = VEEPROM_pu8WriteAddress_OLD;
        VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
        tRetVal = VEEP_PROCESS_SUCCESSFUL;
#endif
        break;
    }
    case VEEP_WRITE_ERASE_MARK:
    {
#if !defined(TEST_EXIT_AFTER_OLD_PAGE_ERASE)
        VEEPROM_vSetEraseMarkData( VEEPROM_u8CurUnitNr, &u64MagicNumber,&p64MagicNumberPosition);
#if defined(DEBUG) && (DEBUG != 0x00u)
        /* debug version*/
        if ( p64MagicNumberPosition != NULL )
        {
            ASSERT_vEXPR( false != VEEPROM_bIsUnitErased( VEEPROM_u8CurUnitNr ) );
            bResult = (bool)FLASH_bEnableAccess();
            if ( bResult == false )
            {
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            }
            bResult = (bool)FLASH_bWrite((tAddress *)(void *)p64MagicNumberPosition, (tWriteData *)(void *)&u64MagicNumber, (uint16_t)sizeof(u64MagicNumber));
            if ( bResult == false )
            {
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            }
            bResult = (bool)FLASH_bDisableAccess();
            if ( bResult == false )
            {
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
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
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            VEEPROM_tDefragStep = VEEP_CHECK_PAGE_EMPTY;
            /* user access disabled or setup CPU problem */
            tRetVal = VEEP_PROCESS_FAILURE;
        }
#else
        /* normal version*/
        (void)FLASH_bEnableAccess();
        (void)FLASH_bWrite((tAddress *)(void *)p64MagicNumberPosition, (tWriteData *)(void *)&u64MagicNumber, (uint16_t)sizeof(u64MagicNumber));
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
        /* only for test */
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
        break;
    }
    }
    return(tRetVal);
}
#else
    #warning "Short version of VEEPROM package not supported for this derivative ! Use large version."
#endif
/* Global Functions*/
/* VEEPROM_bDefragStart()*/
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
        /* Debug*/
        /* previous process not finished*/
#if defined(DEBUG) && (DEBUG != 0x00u)
        _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
#endif
    }
    return(bRetVal);
}
/* VEEPROM_tGetStatus()*/
eVEEPROMtaskStateType VEEPROM_tGetStatus(void)
{
    return(VEEPROM_tTaskState);
}
#if defined(VEEPROM_LARGE)
/* VEEPROM_bInit()*/
bool  VEEPROM_bInit( void )
{
    //! \callgraph
    uint8_t  u8NumberOfNonEmptyUnits = 0U;
    uint8_t  u8OtherUnit             = 0U;
    uint8_t  u8InterruptedUnit;
    uint8_t  u8CheckedUnit;
    uint8_t  u8PageCounter;
    uint16_t u16UsedSpace_Active;
    uint16_t u16UsedSpace_Other;
    bool     bRetVal;
    VEEPROM_tTaskState = VEEP_UNINITIALIZED;
    /* check for interrupted page-transfer*/
    /**
     * \internal
     * \section sec2 Outline of function
     *
     * \subsection sub2_1 Are flash pages o.k.?
     * First <b>check out</b> how many <b>pages</b> are currently <b>in use</b>:
     * If first possible position for a cabinet has a entry in the Drawer-Identifier-Field, then
     * this page is in use. Its location is evaluated in pu8AddressFirstDrawer.
     */
    VEEPROM_vInitStart();
    for ( u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_u8NumberOfUnits; u8CheckedUnit++ )
    {
        //DrawerIdentification is written at first. if first DrawerIdentification is still unwritten, page is unwritten.
        /*
         Msg(4:0488) Performing pointer arithmetic. 
         MISRA-C:2004 Rule 17.4
         Pointer arithmetic is checked.
         (a) VEEPROM_apu8FlashPage contains start address of checked page
         (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
         (c) VEEPROM_u8OffsetDrawerId is offset of drawer-ID
         thus (a) + (b) + (c) is address of first drawer-ID of page being checked.
         */
        /* pu8AddressFirstDrawerId =   VEEPROM_apu8FlashPage[ u8CheckedPage ]
                                + VEEPROM_PAGE_TRANSFER_COUNTER_LEN
                                + VEEPROM_u8OffsetDrawerId; */
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
        for (u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_u8NumberOfUnits; ++u8CheckedUnit)
        {
            VEEPROM_vEnsureUnitIsErased( u8CheckedUnit );
        }
        /*
            Msg(4:0488) Performing pointer arithmetic. 
            MISRA-C:2004 Rule 17.4
            Pointer arithmetic is checked.
            (a) VEEPROM_apu8FlashPage contains start address of default page
            (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page erase-counter
            thus (a) + (b) is address of first drawer of default page
         */
        VEEPROM_pu8WriteAddress = VEEPROM_apu8UnitStart[ VEEPROM_DEFAULT_UNIT ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN;
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
        for (u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_u8NumberOfUnits; ++u8CheckedUnit)
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
        /* set smaller page as interrupted page*/
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
        /* check larger page for interrupted page erase (00 00 00 00 00 00 .. .. fe 1f 2a .. .. .. ff ff)*/
        /* if larger page has interrupted page transfer*/
        /* -  do not erase smaller page, that is actually set as interrupted page.*/
        /* -  erase larger page*/
        /* -  set smaller page as active page.*/
        if ( VEEPROM_bCheckForInterruptedUnitErase( VEEPROM_u8ActiveUnit ) != false )
        {
            /* page erase of full page was interrupted due to reset. In this case the smaller page*/
            /* contains already all latest data, because erase of full page is first started when transfer of latest data has been finished.*/
            /* erase fuller page, that no longer contains valid data.*/
            /* swap active and interrupted pages.*/
            u8OtherUnit = u8InterruptedUnit;
            u8InterruptedUnit = VEEPROM_u8ActiveUnit;
            VEEPROM_u8ActiveUnit = u8OtherUnit;
        }
        else
        {
            /* there was no interrupted unit erase. In this case a reset occured while transfering data from active(full) page to transfer-page.*/
            /* In this case the smaller page probably does not contain all latest data, but the fuller page does.*/
            if ( VEEPROM_bCheckForDefragmentedAndNotErasedUnit( VEEPROM_u8ActiveUnit, u8InterruptedUnit ) != false)
            {
                /* In this case full page was defragmented, new valid drawer values are in the new page, but erase process of full page did not start yet. If this happened, begin of new page contains page erase counter incremented by 1 compared with page erase counter of previous page*/
                /* erase fuller page.*/
                /* swap active and interrupted pages.*/
                u8OtherUnit = u8InterruptedUnit;
                u8InterruptedUnit = VEEPROM_u8ActiveUnit;
                VEEPROM_u8ActiveUnit = u8OtherUnit;
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
            for (u8PageCounter = 0; u8PageCounter < VEEPROM_u8FlashPagesPerUnit; u8PageCounter++)
            {
                bRetVal = FLASH_bErase(  VEEPROM_apu8UnitStart[ u8InterruptedUnit ] + ((uint16_t)u8PageCounter * VEEPROM_uiTotalPageSize) );
                (void)bRetVal;
                ASSERT_vEXPR( bRetVal != false);
                _VEEPROM_vResetWatchdogTimer();
            }
            bRetVal = FLASH_bDisableAccess();
            ASSERT_vEXPR( bRetVal != false);
            VEEPROM_vSetUnitEraseMark( u8InterruptedUnit );
        }
        else
        {
            ASSERT_vEXPR( bRetVal != false);
        }
        VEEPROM_pu8WriteAddress = VEEPROM_pu8ResetWriteAddress( VEEPROM_u8ActiveUnit );
        for (u8CheckedUnit = 0; u8CheckedUnit < VEEPROM_u8NumberOfUnits; ++u8CheckedUnit)
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
#if defined(DEBUG) && (DEBUG != 0x00u)
        // if there is more than one non-free page, something must have gone wrong before - this should never happen.
        _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
#endif
        bRetVal = false;
        break;
    }
    }
    return bRetVal;
}
/* VEEPROM_bDoDefragNow()*/
bool VEEPROM_bDoDefragNow( void )
{
    bool bRetVal;
    if ( VEEPROM_tTaskState == VEEP_READY )
    {
        bRetVal = VEEPROM_bDefrag ( VEEPROM_u8ActiveUnit, VEEPROM_u8GetNextUnit( VEEPROM_u8ActiveUnit ) );
#if defined(DEBUG) && (DEBUG != 0x00u)
        ASSERT_vEXPR( bRetVal != false );
#endif
        VEEPROM_vDefragEnded();
        return (bool)bRetVal;
    }
    else
    {
        return false;
    }
}
/* VEEPROM_bWrite()*/
bool  VEEPROM_bWrite( tDrawerID tLabel, const uint8_t *pu8Data, uint8_t u8DataLen )
{
    bool bRetVal;
    if ( VEEPROM_tTaskState != VEEP_READY )
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        ASSERT_vEXPR(VEEPROM_tTaskState != VEEP_READY);
#endif
        return false;
    }
#if defined(DEBUG) && (DEBUG != 0x00u)
    /* some assertions*/
    ASSERT_vEXPR( tLabel < VEEPROM_tGetNumberOfDrawers() );
    ASSERT_vEXPR( u8DataLen == VEEPROM_ucGetDrawerSize( tLabel ) );
    ASSERT_vEXPR( u8DataLen <= VEEPROM_MAX_DATA_LEN );
    ASSERT_vEXPR( u8DataLen > 0U );
#endif
    /* check if value to write is different from latest one stored in FLASH.*/
    /* if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do write new data if it is different from latest one stored.*/
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
    /* - if VEEPROM_CHECK_VALUE_TO_WRITE_BEFORE_WRITE is set, only do if data to write is different from latest one stored.*/
    if (false != bDifferentData)
    {
#endif
        /* comment - check if we have to defrag first*/
        /**
         * If too less space is left for writing new data, ...
         * ... defragmenting before must be done.
         * There shall never happen any errors
         * while defragmenting data.
         * But if there are some errors, return with error.
         */
        if ( VEEPROM_uiGetFreeSpace() < ( (uint16_t)u8DataLen + (uint16_t)VEEPROM_u8OffsetDrawerPayload ) )
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
                    bRetVal = VEEPROM_bWriteWithoutCheck (tLabel, pu8Data, u8DataLen);
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
            bRetVal = (bool)VEEPROM_bWriteWithoutCheck (tLabel, pu8Data, u8DataLen);
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
uint8_t VEEPROM_ucRead( tDrawerID tLabel, uint8_t *pu8Data, uint8_t u8DataLen )
{
    uint16_t   u16Crc = 0u                 ;
    uint16_t   u16ReadOutCrc               ;
    bool       bEmptyAreaFound             ;
    uint8_t   *pu8CheckedDrawerStartAddress;
    uint8_t   *pu8CurrentAddress           ;
    uint8_t   *pu8FoundDataRecord = NULL   ;
    tDrawerID  tReadOutDrawerId            ;
    uint8_t   *pu8EmptyAreaBase = NULL     ;
    uint8_t    au8CrcSource[VEEPROM_MAX_DATA_LEN + sizeof(tLabel)];
    tDrawerID *ptCrcPosition = (tDrawerID *)(void *)au8CrcSource;
    /**
     * \internal
     * \section secSf5 Outline of function
     * \subsection subSf5_1 Preparation
     * retrieve start address where to search for the desired Drawer
     */
    uint16_t u16TempOffset = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    if ( VEEPROM_tTaskState != VEEP_READY )
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        /* assert*/
        /* VEEPROM_tTaskState is not ready*/
        _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);

#endif
        return 0u;
    }
    /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apucFlashPage contains start address of active page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is position of first drawer on active page.
     */
    pu8CheckedDrawerStartAddress = VEEPROM_apu8UnitStart[ VEEPROM_u8ActiveUnit ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    (void)FLASH_bEnableAccess();
    do
    {
        /**
         * read out CRC-field
         */
        pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ u16TempOffset ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&u16ReadOutCrc, VEEPROM_CRC_SIZE );
        ASSERT_vEXPR ( false != bDbgRetVal );
#else
        (void) FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&u16ReadOutCrc, VEEPROM_CRC_SIZE );
#endif
        /**
         * read out drawerID field
         */
        pu8CurrentAddress = &pu8CheckedDrawerStartAddress[u16TempOffset + (uint16_t)VEEPROM_u8OffsetDrawerId];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
        ASSERT_vEXPR ( false != bDbgRetVal );
#else
        (void) FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
#endif
        /**
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
                pu8EmptyAreaBase = VEEPROM_pu8GetFreeAreaBase(VEEPROM_u8ActiveUnit, pu8CurrentAddress);
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
        if(bEmptyAreaFound == false)
        {
            {
                /**
                 * if we have the correct drawer ID
                 */
                if ( tLabel == tReadOutDrawerId )
                {
                    /**
                     * read out payload of drawer
                     */
#if defined(DEBUG) && (DEBUG != 0x00u)
                    ASSERT_vEXPR (  VEEPROM_ucGetDrawerSize( tReadOutDrawerId ) == u8DataLen );
                    bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset + (uint16_t)VEEPROM_u8OffsetDrawerPayload ], (tReadData *)(void *)(au8CrcSource + sizeof(tLabel)), (uint16_t)u8DataLen );
                    ASSERT_vEXPR ( false != bDbgRetVal );
#else
                    (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset + (uint16_t)VEEPROM_u8OffsetDrawerPayload ], (tReadData *)(void *)(au8CrcSource + sizeof(tLabel)), (uint16_t)u8DataLen );
#endif
                    /**
                     * calc crc over drawerID and payload
                     */
                    *ptCrcPosition = tLabel;
                    u16Crc = VEEPROM_vUpdateCrc( (uint32_t)sizeof(tDrawerID) + (uint32_t)u8DataLen, au8CrcSource );
                    /**
                     * if calculated crc matches stored one, we have found a valid and correct drawer
                     */
                    if ( u16Crc == u16ReadOutCrc )
                    {
                        pu8FoundDataRecord = pu8CheckedDrawerStartAddress + u16TempOffset;
                    }
                }
                // go on with next drawer ...
                u16TempOffset += (uint16_t)VEEPROM_u32AlignToCellSize( (uint32_t)VEEPROM_u8OffsetDrawerPayload + (uint32_t)VEEPROM_u8ObserverGetBruttoPayloadLength(tReadOutDrawerId) );
            }
        }
    }
    while (   ( bEmptyAreaFound == false )
            && (((u16TempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_u8OffsetDrawerPayload - 1U))   <  VEEPROM_uiUnitSize )
    );
    if ( pu8FoundDataRecord != NULL)
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead  ( &pu8FoundDataRecord[ VEEPROM_u8OffsetDrawerPayload ], (tReadData *)(void *)pu8Data, (uint16_t)u8DataLen );
        ASSERT_vEXPR ( false != bDbgRetVal );
#else
        (void) FLASH_bRead  ( &pu8FoundDataRecord[ VEEPROM_u8OffsetDrawerPayload ], (tReadData *)(void *)pu8Data, (uint16_t)u8DataLen );
#endif
    }
    else
    {
        u8DataLen = 0u;
    }
    (void)FLASH_bDisableAccess();
    return u8DataLen;
}
/* VEEPROM_uiGetFreeSpace()*/
uint16_t  VEEPROM_uiGetFreeSpace( void )
{
    uint32_t u32FreeSpace;
    if ( VEEPROM_tTaskState == VEEP_READY )
    {
        u32FreeSpace = (((uint32_t)VEEPROM_apu8UnitStart[ VEEPROM_u8ActiveUnit ] + (uint32_t)VEEPROM_uiUnitSize ) - (uint32_t)VEEPROM_pu8WriteAddress );
        /* if page is full,  VEEPROM_pu8WriteAddress is set after active page in use. This results in a "negative" pu8FreeSpace. But because of unsigned variables, we have to check, if it out of range. In this case we do not have any space left and return "0". */
        if (u32FreeSpace > (uint32_t)VEEPROM_uiUnitSize)
        {
            return 0;
        }
        else
        {
            return (uint16_t)u32FreeSpace;
        }
    }
    else
    {
        return 0;
    }
}
/* VEEPROM_bReadWholeActivePage()*/
bool VEEPROM_bReadWholeActivePage(void)
{
    uint16_t     u16Crc  = 0    ;
    uint16_t     u16ReadOutCrc  ;
    uint8_t      u8Index        ;
    bool         bEmptyAreaFound;
    bool         bRetVal = true ;
    uint8_t     *pu8CheckedDrawerStartAddress;
    uint8_t     *pu8CurrentAddress;
    tDrawerID    tReadOutDrawerId;
    uint8_t      u8DrawerLength;
    uint8_t     *pu8DrawerCacheAddress;
    uint8_t     *pu8EmptyAreaBase = NULL;
    /**
     * \internal
     * \section secSf5 Outline of function
     * \subsection subSf5_1 Preparation
     * retrieve start address where to search for the desired Drawer
     */
    uint16_t u16TempOffset = 0;
#if defined(DEBUG) && (DEBUG != 0x00u)
    bool bDbgRetVal;
#endif
    if ( VEEPROM_tTaskState != VEEP_READY )
    {
#if defined(DEBUG) && (DEBUG != 0x00u)
        /* assert*/
        /* VEEPROM_tTaskState is not ready*/
        _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);

#endif
        return false;
    }
    /*
      Msg(4:0488) Performing pointer arithmetic. 
      MISRA-C:2004 Rule 17.4 <next> 
      Pointer arithmetic is checked.
      (a) VEEPROM_apu8FlashPage contains start address of active page
      (b) VEEPROM_PAGE_TRANSFER_COUNTER_LEN is length of page-erase-counter
      thus (a) + (b) is position of first drawer on active page.
     */
    pu8CheckedDrawerStartAddress = VEEPROM_apu8UnitStart[ VEEPROM_u8ActiveUnit ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN ;
    do
    {
        /*
         * read out drawerID field
         */
        pu8CurrentAddress = &pu8CheckedDrawerStartAddress[ u16TempOffset + (uint16_t)VEEPROM_u8OffsetDrawerId ];
#if defined(DEBUG) && (DEBUG != 0x00u)
        bDbgRetVal = FLASH_bRead ( pu8CurrentAddress, (tReadData *)(void *)&tReadOutDrawerId, (uint16_t)sizeof(tDrawerID) );
        ASSERT_vEXPR ( false != bDbgRetVal );
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
                pu8EmptyAreaBase = VEEPROM_pu8GetFreeAreaBase(VEEPROM_u8ActiveUnit, pu8CurrentAddress);
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
#if defined(DEBUG) && (DEBUG != 0x00u)
                    bDbgRetVal = FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset ], (tReadData *)(void *)&u16ReadOutCrc, VEEPROM_CRC_SIZE );
                    ASSERT_vEXPR ( false != bDbgRetVal );
#else
                    (void) FLASH_bRead ( &pu8CheckedDrawerStartAddress[ u16TempOffset ], (tReadData *)(void *)&u16ReadOutCrc, VEEPROM_CRC_SIZE );
#endif
                    /*
                     * calc crc over drawerID and payload
                     */
                    u16Crc = VEEPROM_vUpdateCrc((uint32_t)u8DrawerLength + (uint32_t)sizeof(tDrawerID), &pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_u8OffsetDrawerId + u16TempOffset ]);
                    /*
                     * if calculated crc matches stored one, we have found a valid and correct drawer
                     */
                    if ( u16Crc == u16ReadOutCrc )
                    {
                        pu8DrawerCacheAddress = VEEPROM_pucGetDrawerAddress( tReadOutDrawerId );
                        if ( pu8DrawerCacheAddress != NULL )
                        {
                            for (u8Index = 0u ; u8Index < u8DrawerLength; ++u8Index )
                            {
                                if ( pu8DrawerCacheAddress[u8Index] != pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_u8OffsetDrawerPayload + u16TempOffset + (uint16_t)u8Index] )
                                {
                                    pu8DrawerCacheAddress[u8Index] = pu8CheckedDrawerStartAddress[ (uint16_t)VEEPROM_u8OffsetDrawerPayload + u16TempOffset + (uint16_t)u8Index];
                                }
                            }
                            VEEPROM_vAfterCacheVariableSet( tReadOutDrawerId);
                        }
                        else
                        {
                            /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                            ASSERT_vEXPR ( pu8DrawerCacheAddress != NULL );
#endif
                            bRetVal = false;
                        }
                    }
                    else
                    {
                        /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                        ASSERT_vEXPR ( u16Crc == u16ReadOutCrc );
#endif
                        bRetVal = false;
                    }
                }
                else
                {
                    /* DEBUG*/
#if defined(DEBUG) && (DEBUG != 0x00u)
                    ASSERT_vEXPR ( u8DrawerLength > 0u );
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
            u16TempOffset += (uint16_t)VEEPROM_u32AlignToCellSize( (uint32_t)VEEPROM_u8OffsetDrawerPayload + (uint32_t)VEEPROM_u8ObserverGetBruttoPayloadLength(tReadOutDrawerId) );
        }
    }
    while (   ( bEmptyAreaFound == false )
            && (((u16TempOffset + VEEPROM_PAGE_TRANSFER_COUNTER_LEN) + ((uint16_t)VEEPROM_u8OffsetDrawerPayload - 1U))  <=  VEEPROM_uiUnitSize )
    );
    return (bool)bRetVal;
}
/* VEEPROM_bWriteStart()*/
bool  VEEPROM_bWriteStart( uint16 tLabel, const uint8_t *pu8Data, uint8_t u8DataLen )
{
    bool bRetVal = false;
    uint16_t u16Crc;
#if defined(DEBUG) && (DEBUG != 0x00u)
    /* debug version*/
    if ( VEEPROM_tTaskState == VEEP_READY )
    {
        if ( tLabel < VEEPROM_tGetNumberOfDrawers() )
        {
            if ( u8DataLen == VEEPROM_ucGetDrawerSize(tLabel) )
            {
                if ( u8DataLen <= VEEPROM_MAX_DATA_LEN )
                {
                    if ( u8DataLen > 0u )
                    {
                        if ( VEEPROM_uiGetFreeSpace() < (uint16_t)( u8DataLen + VEEPROM_u8OffsetDrawerPayload ) )
                        {
                            if ( VEEPROM_bApprobateDefrag() != false )
                            {
                               VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
                               bRetVal = true;
                            }
                            else
                            {
                                 /* assert*/
                                 /* defragmentation not alowed*/
                                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
                            }
                        }
                        else
                        {
                            VEEPROM_u8DataLen   = u8DataLen;
                            /* set label to its position */
                            *((tDrawerID *)(void *)(VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId)) = tLabel;
                            /* set data to its position */
                            UTI_vMemCopy( pu8Data, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerPayload, (uint16_t)u8DataLen);
                            /* fill rest of array with empty content */
                            UTI_vMemFill(VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerPayload + u8DataLen, VEEPROM_EMPTY_FLASH_BYTE, (uint16_t) UTI_NELEMENTS(VEEPROM_au8DataRecord) - (uint16_t)(VEEPROM_u8OffsetDrawerPayload + u8DataLen));
                            /* calculate Crc */
                            u16Crc = VEEPROM_vUpdateCrc( (uint32_t)sizeof(tDrawerID) + (uint32_t)u8DataLen, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId );
                            /* set Crc to its position */
                            *((uint16_t *)(void *)VEEPROM_au8DataRecord) = u16Crc;
                            /* calculate number of write steps */
                            VEEPROM_u8WriteSteps = ((uint8_t)VEEPROM_u32AlignToCellSize((uint32_t) (VEEPROM_u8OffsetDrawerPayload + u8DataLen))) / VEEPROM_u8CellSize;
                            VEEPROM_tTaskState = VEEP_WRITING;
                            bRetVal = true;
                        }
                    }
                    else
                    {
                        /* assert*/
                        /* u8DataLen is 0*/
                        _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
                    }
                }
                else
                {
                    /* assert*/
                    /* u8DataLen is too long*/
                    _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
                }
            }
            else
            {
                /* assert*/
                /* u8DataLen not valid*/
                _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
            }
        }
        else
        {
            /* assert*/
            /* u8Label not valid*/
            _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
        }
    }
    else
    {
        /* assert*/
        /* previous process not finished*/
        _ASSERT_vOnFailure ((const uint8_t*)((const void *)__FILE__), __LINE__);
    }
#else
    /* normal version*/
    if ( VEEPROM_tTaskState == VEEP_READY )
    {
        if ( VEEPROM_uiGetFreeSpace() < ( (uint16_t)u8DataLen + (uint16_t)VEEPROM_u8OffsetDrawerPayload ) )
        {
            if ( VEEPROM_bApprobateDefrag() != false )
            {
                VEEPROM_tTaskState = VEEP_DEFRAGMENTING;
                bRetVal = true;
            }
            else
            {
                /* defragmentation not alowed*/
            }
        }
        else
        {
            VEEPROM_u8DataLen   = u8DataLen;
            /* set label to its position */
            *((tDrawerID *)(void *)(VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId)) = tLabel;
            /* set data to its position */
            UTI_vMemCopy( pu8Data, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerPayload, (uint16_t)u8DataLen);
            /* fill rest of array with empty content */
            UTI_vMemFill(VEEPROM_au8DataRecord + (VEEPROM_u8OffsetDrawerPayload + u8DataLen), VEEPROM_EMPTY_FLASH_BYTE, (uint16_t) UTI_NELEMENTS(VEEPROM_au8DataRecord) - ((uint16_t)VEEPROM_u8OffsetDrawerPayload + (uint16_t)u8DataLen));
            /* calculate Crc */
            u16Crc = VEEPROM_vUpdateCrc( (uint32_t)sizeof(tDrawerID) + (uint32_t)u8DataLen, VEEPROM_au8DataRecord + VEEPROM_u8OffsetDrawerId );
            /* set Crc to its position */
            *((uint16_t *)(void *)VEEPROM_au8DataRecord) = u16Crc;
            /* calculate number of write steps */
            VEEPROM_u8WriteSteps = ((uint8_t)VEEPROM_u32AlignToCellSize( (uint32_t)VEEPROM_u8OffsetDrawerPayload + (uint32_t)u8DataLen)) / VEEPROM_u8CellSize;
            VEEPROM_tTaskState = VEEP_WRITING;
            bRetVal = true;
        }
    }
    else
    {
        /* previous process not finished*/
    }
#endif
    return(bRetVal);
}
/* VEEPROM_ucHandleTask()*/
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
                    VEEPROM_u8DataLen = 0;
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
                    /* process ongoing*/
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
            /* process ongoing*/
            }
            break;
        }
        default:
        {
        /* default can never occur because switch variable is enum type*/
            break;
        }
    }
    return(TASK_INITIALISED);
}
#else
    #warning "Short version of VEEPROM package not supported for this derivative ! Use large version."
#endif
