/*******************************************************************************
*   Copyright (c) 2023 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          SW_LIB_FRAMEWORK_NG
*   COMP_ABBREV      VEEPROM
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/**
 * \file  VeepromMcXs.c
 * \brief Application specific implementation/configuration for Volatile EEPROM
 *        component - observer functions for SH30F9X71 derivatives
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "nvmem/cache.h"
#include "VeepromXs.h"
#include "flash/Flash.h"
#include "hsup.h"

/*lint -save -e923 "cast from unsigned int to pointer"*/
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
/**
 * \brief cell size of flash
 */
#define VEEPROM_FLASH_CELL_SIZE 2u

/**
 * \brief length of magic number entry for page is erased
 */
#define VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH 2u

/**
 * \brief First address of flash memory
 */
#if !defined(ROM1_PHY_START)
#define ROM1_PHY_START 0x00000000uL
#endif

#define EEPROM_LIKE_DATA_START    0x0FFC0000uL
#define EEPROM_LIKE_BLOCK_SIZE    1024u

/**
 * \brief magic number used as mark for successfully erased page
 */
#define VEEPROM_MAGIC_NUMBER 0xA5u

/**
 * length of page transfer counter in bytes
 *
 * these are the first n bytes of page that do not have any header.
 */
#define VEEPROM_PAGE_TRANSFER_COUNTER_LEN 2

#if defined(VEEPROM_LARGE) && !defined(FLASH_PAGES_PER_UNIT)
/**
* \brief count of flash pages per unit
*/
#define FLASH_PAGES_PER_UNIT  2u
#endif

#if !defined(VEEPROM_UNIT1_START_ADDRESS) || !defined(VEEPROM_UNIT2_START_ADDRESS) || !defined(VEEPROM_FLASH_BLOCK_SIZE)
#define VEEPROM_FLASH_BLOCK_SIZE        1024u   ///< Number of bytes in flash memory block
#endif
/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#if defined(VEEPROM_LARGE)
/**
 * \brief Start addresses of the flash units used for VEEPROM. One flash unit can contain more flash pages. Must be set accordingly to used Target.
 *
 * \note AT LEAST 2 flash units must be defined !
 * \note All flash pages used in one unit must have the same length !
 */
uint8_t *VEEPROM_apucUnitStart[ ] =
{
#if defined(VEEPROM_UNIT1_START_ADDRESS) && defined(VEEPROM_UNIT2_START_ADDRESS)
    (uint8_t *)VEEPROM_UNIT1_START_ADDRESS,
    (uint8_t *)VEEPROM_UNIT2_START_ADDRESS
#else /* all 4 pages of EEPROM like area used as default */
    (uint8_t *)(EEPROM_LIKE_DATA_START),
    (uint8_t *)(EEPROM_LIKE_DATA_START + (EEPROM_LIKE_BLOCK_SIZE * FLASH_PAGES_PER_UNIT))
#endif
}
;

/**
 * \brief Defining the count of flash pages which belong to one memory unit
 *
 * Change this value by redefining #FLASH_PAGES_PER_UNIT
 *
 */
const uint8_t VEEPROM_ucFlashPagesPerUnit = FLASH_PAGES_PER_UNIT;

const uint16_t VEEPROM_uiTotalPageSize = VEEPROM_FLASH_BLOCK_SIZE;

/**
 * \brief Defining the size of each flash unit decreased by page erase magic number field
 *
 * Change this value by redefining #VEEPROM_FLASH_BLOCK_SIZE
 *
 */
const uint16_t VEEPROM_uiUnitSize = (uint16_t)((VEEPROM_FLASH_BLOCK_SIZE * FLASH_PAGES_PER_UNIT) - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH);

/**
 * \brief Number of flash units reserved for using this library
 *
 * \note DO NOT CHANGE.
 */
uint8_t VEEPROM_ucNumberOfUnits = (uint8_t)UTI_NELEMENTS(VEEPROM_apucUnitStart);
#else //if not defined(VEEPROM_LARGE)
/**
 * \brief Start addresses of the flash-pages used for VEEPROM. Must be set accordingly to used Target.
 *
 * \note AT LEAST 2 FlashPages of equal sizes must be defined!
 */
uint8_t *VEEPROM_apucFlashPage[ ] =
{
#if defined(VEEPROM_UNIT1_START_ADDRESS) && defined(VEEPROM_UNIT2_START_ADDRESS)
    (uint8_t *)VEEPROM_UNIT1_START_ADDRESS,
    (uint8_t *)VEEPROM_UNIT2_START_ADDRESS
#else /* first two pages of EEPROM like area used as default */
    (uint8_t *)(EEPROM_LIKE_DATA_START),
    (uint8_t *)(EEPROM_LIKE_DATA_START + EEPROM_LIKE_BLOCK_SIZE)
#endif
};

/**
 * \brief Defining the size of each FlashPage decreased by page erase magic number field
 *
 * Change this value by redefining #VEEPROM_FLASH_BLOCK_SIZE
 */
uint16_t VEEPROM_uiFlashPageSize = (uint16_t)(VEEPROM_FLASH_BLOCK_SIZE - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH);
/**
 * \brief Number of FlashPages reserved for using this library
 *
 * \note DO NOT CHANGE.
 */
uint8_t VEEPROM_ucNumberOfPages = (uint8_t)UTI_NELEMENTS(VEEPROM_apucFlashPage);
#endif //defined(VEEPROM_LARGE)

const uint8_t VEEPROM_ucCellSize            = VEEPROM_FLASH_CELL_SIZE;
const uint8_t VEEPROM_ucOffsetDrawerId      = VEEPROM_FLASH_CELL_SIZE;
const uint8_t VEEPROM_ucOffsetDrawerPayload = (uint8_t)(2u * VEEPROM_FLASH_CELL_SIZE);

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/* Observerfunctions */
#if defined(VEEPROM_LARGE)
uint8_t VEEPROM_ucObserverGetBruttoPayloadLength ( tDrawerID drawerIdentification )
{
    tDrawerID numberOfDrawers = VEEPROM_tGetNumberOfDrawers();
    uint8_t drawerSize;
    if ( drawerIdentification < numberOfDrawers )
    {
        drawerSize = VEEPROM_ucGetDrawerSize( drawerIdentification);
    }
    else
    {
        tDrawerID checkedDrawer;
        uint8_t actualDrawerSize;
        drawerSize = 0;
        for ( checkedDrawer = 0;
                checkedDrawer < numberOfDrawers;
                checkedDrawer++)
        {
            actualDrawerSize = VEEPROM_ucGetDrawerSize(checkedDrawer);
            if ( actualDrawerSize > drawerSize )
            {
                drawerSize = actualDrawerSize;
            }
        }
    }
    return (uint8_t)((( drawerSize + (VEEPROM_FLASH_CELL_SIZE - 1u) ) / VEEPROM_FLASH_CELL_SIZE ) * VEEPROM_FLASH_CELL_SIZE);
}

bool VEEPROM_bCheckForInterruptedUnitErase ( uint8_t unitNumber )
{
    uint8_t pageEraseCounter[2];
    bool retVal = false;

    (void)FLASH_bRead(VEEPROM_apucUnitStart[unitNumber], &pageEraseCounter[0], 2);
    if ( ( pageEraseCounter[0] == 0x00u ) && ( pageEraseCounter[1] == 0x00u ) )
    {
        retVal = true;
    }
    return retVal;
}

bool VEEPROM_bCheckForDefragmentedAndNotErasedUnit( uint8_t activeUnit, uint8_t unitToDefragment )
{
    uint8_t unitEraseCounterOld[2];
    uint8_t unitEraseCounterNew[2];
    uint16_t eraseCounterOld;
    uint16_t eraseCounterNew;
    bool resultOfOld;
    bool resultOfNew;
    bool retVal = false;

    resultOfOld = FLASH_bRead(VEEPROM_apucUnitStart[activeUnit], &unitEraseCounterOld[0], 2);
    resultOfNew = FLASH_bRead(VEEPROM_apucUnitStart[unitToDefragment], &unitEraseCounterNew[0], 2);
    if ( (resultOfOld != false) && (resultOfNew != false) )
    {
        /* conversion from big endian to little endian */
        HSUP_vSetHighByte(&eraseCounterOld, unitEraseCounterOld[0]);
        HSUP_vSetLowByte (&eraseCounterOld, unitEraseCounterOld[1]);

        HSUP_vSetHighByte(&eraseCounterNew, unitEraseCounterNew[0]);
        HSUP_vSetLowByte (&eraseCounterNew, unitEraseCounterNew[1]);
        if ( eraseCounterNew == (eraseCounterOld + 1u) )
        {
            retVal = true;
        }
    }
    return(retVal);
}

void VEEPROM_vSetUnitEraseMark( uint8_t unitNumber )
{
    bool retVal;

    uint8_t magicNumber = VEEPROM_MAGIC_NUMBER;
    uint8_t *magicNumberPosition = (VEEPROM_apucUnitStart[ unitNumber ]
                                    + (VEEPROM_FLASH_BLOCK_SIZE * FLASH_PAGES_PER_UNIT))
                                    - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH;

    retVal = FLASH_bEnableAccess();
    if (false != retVal)
    {
        retVal = FLASH_bWrite( magicNumberPosition, &magicNumber, (uint16_t)sizeof(magicNumber) );
        (void)retVal;
    }
    (void) FLASH_bDisableAccess();
}

void VEEPROM_vEnsureUnitIsErased( uint8_t unitNumber )
{
    bool retVal;
    bool isEmpty;
    uint8_t magicNumber;
    uint8_t *testAddress;
    uint8_t byte;
    uint8_t pageCounter;

    uint8_t *magicNumberPosition = (VEEPROM_apucUnitStart[ unitNumber ]
                                    + (VEEPROM_FLASH_BLOCK_SIZE * FLASH_PAGES_PER_UNIT))
                                    - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH;

    isEmpty = true;

    //read out page erase magic number at end of flash page
    (void) FLASH_bRead ( magicNumberPosition, &magicNumber, (uint16_t)sizeof(magicNumber) );
    if ( magicNumber != VEEPROM_MAGIC_NUMBER )
    {
        isEmpty = false;
    }
    else
    {
        /* test memory contents */
        testAddress = (VEEPROM_apucUnitStart[ unitNumber ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
        do
        {
            (void) FLASH_bRead ( testAddress, &byte, (uint16_t)sizeof(byte) );
            if ( byte != VEEPROM_UNWRITTEN_FLASH_CELL )
            {
                isEmpty = false;
            }
            ++testAddress;
        }
        while ( (isEmpty == true) && ((uint32_t)testAddress < (uint32_t)magicNumberPosition) );
    }
    if ( isEmpty == false )
    {
        retVal = FLASH_bEnableAccess();
        if ( retVal != false )
        {
            //erase page has not been finished before. Do erase all pages in unit once more
            for (pageCounter = 0; pageCounter < FLASH_PAGES_PER_UNIT; pageCounter++)
            {
                retVal = FLASH_bErase(  VEEPROM_apucUnitStart[ unitNumber ] + (uint16_t)(pageCounter * VEEPROM_FLASH_BLOCK_SIZE) );
                (void)retVal;
                _VEEPROM_vResetWatchdogTimer();
            }
            // finally mark erased page to be fully erased.
            /* set page erase mark */
            magicNumber = VEEPROM_MAGIC_NUMBER;
            retVal = FLASH_bWrite( magicNumberPosition, &magicNumber, (uint16_t)sizeof(magicNumber) );
            (void)retVal;
        }
        (void) FLASH_bDisableAccess();
    }
}

uint8_t *VEEPROM_pucGetFreeAreaBase(uint8_t unitNumber, uint8_t *startAddress)
{
    uint8_t *lastNotFree;
    uint8_t *testAddress;
    uint8_t byte;

    const uint8_t *magicNumberPosition = (VEEPROM_apucUnitStart[ unitNumber ]
                                          + (VEEPROM_FLASH_BLOCK_SIZE * FLASH_PAGES_PER_UNIT))
                                          - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH;

    testAddress = startAddress;
    lastNotFree = startAddress - 1;
    while ( (uint32_t)testAddress < (uint32_t)magicNumberPosition )
    {
        (void)FLASH_bRead ( testAddress, &byte, (uint16_t)sizeof(byte) );
        if ( byte != VEEPROM_UNWRITTEN_FLASH_CELL )
        {
            lastNotFree = testAddress;
        }
        ++testAddress;
    }
    /* next address is free or contains magic number */
    lastNotFree++;

    _VEEPROM_vResetWatchdogTimer();

    return(lastNotFree);
}

bool VEEPROM_bUnitDataValid( uint8_t unitNumber )
{
    uint8_t magicNumber = 0;
    uint8_t *magicNumberPosition = (VEEPROM_apucUnitStart[ unitNumber ]
                                    + (VEEPROM_FLASH_BLOCK_SIZE * FLASH_PAGES_PER_UNIT))
                                    - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH;

    (void) FLASH_bRead ( magicNumberPosition, &magicNumber, (uint16_t)sizeof(magicNumber) );

    return(magicNumber == VEEPROM_MAGIC_NUMBER);
}

bool VEEPROM_bIsUnitRemainderEmpty(uint8_t unitNumber, uint8_t *startAddress)
{
    bool isEmpty;
    uint8_t *testAddress;
    uint8_t byte;

    const uint8_t *magicNumberPosition = (VEEPROM_apucUnitStart[ unitNumber ]
                                          + (VEEPROM_FLASH_BLOCK_SIZE * FLASH_PAGES_PER_UNIT))
                                          - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH;

    testAddress = startAddress;

    isEmpty = true;
    while ( (isEmpty == true) && ((uint32_t)testAddress < (uint32_t)magicNumberPosition) )
    {
        (void)FLASH_bRead ( testAddress, &byte, (uint16_t)sizeof(byte) );
        if ( byte != VEEPROM_UNWRITTEN_FLASH_CELL )
        {
            isEmpty = false;
        }
        else
        {
            ++testAddress;
        }
    }
    _VEEPROM_vResetWatchdogTimer();

    return(isEmpty);
}

void VEEPROM_vSetEraseMarkData( uint8_t unitNumber, uint8_t *magicNumber, uint8_t **magicNumberAddress )
{
   *magicNumber = VEEPROM_MAGIC_NUMBER;
   *magicNumberAddress = VEEPROM_apucUnitStart[unitNumber] + ((VEEPROM_FLASH_BLOCK_SIZE * FLASH_PAGES_PER_UNIT) - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH);

}

#else //if not defined(VEEPROM_LARGE)
uint8_t VEEPROM_ucObserverGetBruttoPayloadLength ( uint8_t drawerIdentification )
{
    uint8_t drawerSize;
    uint8_t numberOfDrawers = VEEPROM_ucGetNumberOfDrawers();

    if (drawerIdentification < numberOfDrawers )
    {
        drawerSize = VEEPROM_ucGetDrawerSize( drawerIdentification );
    }
    else
    {
        uint8_t actualDrawerSize;
        uint8_t checkedDrawer;
        drawerSize = 0;
        for ( checkedDrawer = 0;
                checkedDrawer < numberOfDrawers;
                checkedDrawer++)
        {
            actualDrawerSize = VEEPROM_ucGetDrawerSize(checkedDrawer);
            if ( actualDrawerSize > drawerSize )
            {
                drawerSize = actualDrawerSize;
            }
        }
    }
    return (uint8_t)((( drawerSize + (VEEPROM_FLASH_CELL_SIZE - 1u) ) / VEEPROM_FLASH_CELL_SIZE ) * VEEPROM_FLASH_CELL_SIZE);
}

bool VEEPROM_bCheckForInterruptedPageErase ( uint8_t pageNumber )
{
    /* If page erasing was interrupted by a reset, there's no guarantee what value would be inside the flash */
    (void)pageNumber;
    return false;
}

bool VEEPROM_bCheckForDefragmentedAndNotErasedPage( uint8_t activePage, uint8_t pageToDefragment )
{
    uint8_t pageEraseCounterOld[2];
    uint8_t pageEraseCounterNew[2];
    uint16_t eraseCounterOld;
    uint16_t eraseCounterNew;
    bool resultOfOld;
    bool resultOfNew;
    bool retVal;

    retVal = false;
    resultOfOld = FLASH_bRead(VEEPROM_apucFlashPage[activePage], &pageEraseCounterOld[0], 2);
    resultOfNew = FLASH_bRead(VEEPROM_apucFlashPage[pageToDefragment], &pageEraseCounterNew[0], 2);
    if ( (resultOfOld != false) && (resultOfNew != false) )
    {
        /* conversion from big endian to little endian */
        HSUP_vSetHighByte(&eraseCounterOld, pageEraseCounterOld[0]);
        HSUP_vSetLowByte (&eraseCounterOld, pageEraseCounterOld[1]);

        HSUP_vSetHighByte(&eraseCounterNew, pageEraseCounterNew[0]);
        HSUP_vSetLowByte (&eraseCounterNew, pageEraseCounterNew[1]);
        if ( eraseCounterNew == (eraseCounterOld + 1u) )
        {
            retVal = true;
        }
    }
    return(retVal);
}

void VEEPROM_vSetPageEraseMark( uint8_t pageNumber )
{
    bool retVal;

    uint8_t magicNumber = VEEPROM_MAGIC_NUMBER;

    uint8_t *magicNumberPosition = VEEPROM_apucFlashPage[ pageNumber ] + VEEPROM_uiFlashPageSize;

    retVal = FLASH_bEnableAccess();
    if (false != retVal)
    {
        retVal = FLASH_bWrite( magicNumberPosition, &magicNumber, 1 );
        (void)retVal;
        (void) FLASH_bDisableAccess();
    }
}

void VEEPROM_vEnsurePageIsErased( uint8_t pageNumber )
{
    bool retVal;
    bool isEmpty;
    uint8_t magicNumber;
    const uint8_t *testAddress;

    uint8_t *magicNumberPosition = VEEPROM_apucFlashPage[ pageNumber ] + VEEPROM_uiFlashPageSize;

    isEmpty = true;

    //read out page erase magic number at end of flash page
    (void) FLASH_bRead ( magicNumberPosition, &magicNumber, 1 );
    if ( magicNumber != VEEPROM_MAGIC_NUMBER )
    {
        isEmpty = false;
    }
    else
    {
        /* test memory contents */
        testAddress = (VEEPROM_apucFlashPage[ pageNumber ] + VEEPROM_PAGE_TRANSFER_COUNTER_LEN);
        do
        {
            if ( *testAddress != VEEPROM_UNWRITTEN_FLASH_CELL )
            {
                isEmpty = false;
            }
            ++testAddress;
        }
        while ( (isEmpty == true) && ((uint32_t)testAddress < (uint32_t)magicNumberPosition) );
    }
    if ( isEmpty == false )
    {
        //erase page has not been finished before. Do erase page once more
        retVal = FLASH_bEnableAccess();
        if ( false != retVal )
        {
            retVal = FLASH_bErase(  VEEPROM_apucFlashPage[ pageNumber ] );
            (void)retVal;
        }
        (void) FLASH_bDisableAccess();
        // finally mark erased page to be fully erased.
        VEEPROM_vSetPageEraseMark( pageNumber );
    }
}

uint8_t *VEEPROM_pucGetFreeAreaBase(uint8_t pageNumber, uint8_t *startAddress)
{
    uint8_t *lastNotFree;
    uint8_t *testAddress;
    uint8_t byte;

    const uint8_t *magicNumberPosition = (VEEPROM_apucFlashPage[ pageNumber ]
                                          + VEEPROM_FLASH_BLOCK_SIZE)
                                          - VEEPROM_PAGE_ERASE_MAGIC_NUMBER_LENGTH;

    testAddress = startAddress;
    lastNotFree = startAddress - 1;
    while ( (uint32_t)testAddress < (uint32_t)magicNumberPosition )
    {
        (void)FLASH_bRead ( testAddress, &byte, (uint16_t)sizeof(byte) );
        if ( byte != VEEPROM_UNWRITTEN_FLASH_CELL )
        {
            lastNotFree = testAddress;
        }
        ++testAddress;
    }
    /* next address is free or contains magic number */
    lastNotFree++;

    _VEEPROM_vResetWatchdogTimer();

    return(lastNotFree);
}

bool VEEPROM_bPageDataValid( uint8_t pageNumber )
{
    uint8_t magicNumber;

    uint8_t *magicNumberPosition = VEEPROM_apucFlashPage[ pageNumber ] + VEEPROM_uiFlashPageSize;

    (void) FLASH_bRead ( magicNumberPosition, &magicNumber, 1 );

    return(magicNumber == VEEPROM_MAGIC_NUMBER);
}

bool VEEPROM_bIsPageRemainderEmpty(uint8_t pageNumber, uint8_t *startAddress)
{
    bool isEmpty;
    uint8_t *testAddress; //lint !e954 'testAddress' could be pointer to const

    const uint8_t *magicNumberPosition = VEEPROM_apucFlashPage[ pageNumber ] + VEEPROM_uiFlashPageSize;

    isEmpty = true;

    testAddress = startAddress;
    while ( (isEmpty == true) && ((uint32_t)testAddress < (uint32_t)magicNumberPosition) )
    {
        if ( *testAddress != VEEPROM_UNWRITTEN_FLASH_CELL )
        {
            isEmpty = false;
        }
        else
        {
            ++testAddress;
        }
    }
    _VEEPROM_vResetWatchdogTimer();

    return(isEmpty);
}

void VEEPROM_vSetEraseMarkData( uint8_t pageNumber, uint8_t* magicNumber, uint8_t** magicNumberAddress )
{
   *magicNumber = VEEPROM_MAGIC_NUMBER;
   *magicNumberAddress =  VEEPROM_apucFlashPage[pageNumber] + VEEPROM_uiFlashPageSize;

}

#endif //defined(VEEPROM_LARGE)
/*lint -restore -e923 */
