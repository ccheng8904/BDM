/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          VEEPROM
 ******************************************************************************/

#ifndef VEEPROM_MC_XS_H
#define VEEPROM_MC_XS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file VEEPROMmcXS.h
 *  \brief Published observer function definitions and declarations for Volatile EEPROM.
 *         This file is for SH30F9X71 derivatives.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/**
 * \brief content of unwritten flash-cells
 *
 * unwritten (or erased) flash-cells contain the byte 0x00
 */
#define VEEPROM_UNWRITTEN_FLASH_CELL      0x00u

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/** \brief    Type used for drawer ID variable
 *
 */
#if defined(VEEPROM_LARGE)
typedef uint16_t tDrawerID;
#else
typedef uint8_t tDrawerID;
#endif //defined(VEEPROM_LARGE)

/** \brief    Offset of drawerID from begin of drawer e.g. R8C 1Byte, M16C 2Byte
 *            has to be filled in VEEPROM_bInit-Routine
 */
//VEEPROM_ucObserverGetOffsetDrawerId()
extern const uint8_t VEEPROM_ucOffsetDrawerId;

/** \brief    Offset of drawerPayload from begin of drawer e.g. R8C 2Byte, M16C 4Byte
 *            has to be filled in VEEPROM_bInit-Routine
 */
//VEEPROM_ucObserverGetOffsetDrawerPayload()
extern const uint8_t VEEPROM_ucOffsetDrawerPayload;

/** \brief    Cell size of lower FLASH; e.g. R8C 1Byte, M16C 2Byte
 *            has to be filled in VEEPROM_bInit-Routine
 */
extern const uint8_t VEEPROM_ucCellSize;

/** \brief    External, defined in VEEPROMmcXS::VEEPROM_ucWrittenAtOnce
 */
extern const uint8_t VEEPROM_ucWrittenAtOnce;

#if defined(VEEPROM_LARGE)
/** \brief    External, defined in VEEPROMmcXS::VEEPROM_apucUnitStart
 */
extern uint8_t *VEEPROM_apucUnitStart[  ];

/** \brief    External, defined in VEEPROMmcXS::VEEPROM_uiUnitSize
 */
extern const uint16_t VEEPROM_uiUnitSize;

/** \brief    External, defined in VEEPROMmcXS::VEEPROM_ucNumberOfUnits
 */
extern uint8_t VEEPROM_ucNumberOfUnits;

/** \brief    External, defined in VEEPROMmcXS::VEEPROM_ucFlashPagesPerUnit
 */
extern const uint8_t VEEPROM_ucFlashPagesPerUnit;

/** \brief    External, defined in VEEPROMmcXS::VEEPROM_uiTotalPageSize
 */
extern const uint16_t VEEPROM_uiTotalPageSize;

#else //if not defined(VEEPROM_LARGE)
/** \brief    External, defined in VEEPROMmcXS::VEEPROM_apucFlashPage
 */
extern uint8_t *VEEPROM_apucFlashPage[  ];

/** \brief    External, defined in VEEPROMmcXS::VEEPROM_uiFlashPageSize
 */
extern uint16_t VEEPROM_uiFlashPageSize;

/** \brief    External, defined in VEEPROMmcXS::VEEPROM_ucNumberOfPages
 */
extern uint8_t VEEPROM_ucNumberOfPages;
#endif //defined(VEEPROM_LARGE)

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#if defined(VEEPROM_LARGE)
/**
 * \brief    Observer function: get the length of payload in bytes including
 *           dead payloadCells for cellspaces != 1
 *
 *           If drawer does not exist it returns the largest drawer brutto
 *           payload including dead payloadCells.
 *
 * \return   length payload in bytes
 * \note     if cellspace is 4 bytes and payload length is 1 this function returns 4 (bytes) 
 */
extern uint8_t VEEPROM_ucObserverGetBruttoPayloadLength ( tDrawerID ucDrawerIdentification );

/**
 * \brief    Observer function: check given page for interrupted page erase.
 *
 *           In this case erasing page was interrupted by a reset.
 *           If this happened, begin of page contains "00" entries and
 *           flushing page with "FF" was not reached.
 *
 * \param    ucPageNumber number of page that is checked
 *
 * \return   true, if page erase was interrupted. Otherwise false.
 */
extern bool VEEPROM_bCheckForInterruptedUnitErase ( uint8_t ucUnitNumber );

/**
 * \brief    Observer function: check both pages for finished defragmentation
 *           and not erased full page.
 *
 *           In this case full page was defragmented, new valid drawer values 
 *           are in the new page, but erase process of full page did not start yet.
 *           If this happened, begin of new page contains page erase counter incremented
 *           by 1 compared with page erase counter of previous page.
 *
 * \param    ucActivePage number of page, which is set as active at the moment
 * \param    ucPageToDefragment number of page, which ist dedicated to erase at the moment
 *
 * \return   true, if page erase was interrupted. Otherwise false.
 */
extern bool VEEPROM_bCheckForDefragmentedAndNotErasedUnit( uint8_t ucActiveUnit, uint8_t ucUnitToDefragment );

/**
 * \brief    Observer function: set mark for given page that process
 *           of erasing page succeeded fully
 * 
 * \param    ucPageNumber Number of page that shall be marked
 */
extern void VEEPROM_vSetUnitEraseMark( uint8_t ucUnitNumber );

/**
 * \brief    Observer function: Check if pages in one data unit are erased fully
 *           including magic number written at end of last page
 * 
 *           if page erase magic number does not exist we expect that there was a reset
 *           while erasing page. Thus we erase all pages again.
 */
extern void VEEPROM_vEnsureUnitIsErased( uint8_t ucUnitNumber );

/**
 * \brief    Observer function: Check if page data are valid
 * 
 *           if page erase magic number does not exist we expect that data cannot be valid.
 *           Thus we erase the page again.
 */
extern bool VEEPROM_bUnitDataValid( uint8_t ucUnitNumber );

/**
 * \brief    Observer function: Check if memory range is empty
 * 
 * \param    ucPageNumber Number of page
 * \param    pucStartAddress first address of area to be checked
 * 
 * \return   true, if memory area is empty, otherwise false
 * 
 */
extern bool VEEPROM_bIsUnitRemainderEmpty(uint8_t ucUnitNumber, uint8_t *pucStartAddress);

/**
 * \brief    Observer function: Gets position of last non-empty byte on memory page area
 * 
 * \param    ucPageNumber Number of page
 * \param    pucStartAddress first address of area to be checked
 * 
 * \return   pointer to address
 * 
 */
extern uint8_t *VEEPROM_pucGetFreeAreaBase(uint8_t ucUnitNumber, uint8_t *pucStartAddress);

/**
 * \brief    Observer function: set magic number and its address
 */
extern void VEEPROM_vSetEraseMarkData( uint8_t ucUnitNumber, uint8_t *pucMagicNumber, uint8_t **pucMagicNumberAddress );
#else //if not defined(VEEPROM_LARGE)
/**
 * \brief    Observer function: get the length of payload in bytes including
 *           dead payloadCells for cellspaces != 1
 *
 *           If drawer does not exist it returns the largest drawer brutto payload including
 *           dead payloadCells.
 *
 * \return   length payload in bytes
 * \note     if cellspace is 4 bytes and payload length is 1 this function returns 4 (bytes)
 */
extern uint8_t VEEPROM_ucObserverGetBruttoPayloadLength ( uint8_t ucDrawerIdentification );

/**
 * \brief    Observer function: check given page for interrupted page erase.
 *
 *           In this case erasing page was interrupted by a reset. If this happened,
 *           there's no guarantee what value would be inside the flash
 *
 * \param    ucPageNumber number of page that is checked
 *
 * \return   true, if page erase was interrupted. Otherwise false.
 */
extern bool VEEPROM_bCheckForInterruptedPageErase ( uint8_t ucPageNumber );

/**
 * \brief    Observer function: check both pages for finished defragmentation
 *           and not erased full page.
 *
 *           In this case full page was defragmented, new valid drawer values
 *           are in the new page, but erase process of full page did not start yet.
 *           If this happened, begin of new page contains page erase counter incremented 
 *           by 1 compared with page erase counter of previous page.
 *
 * \param    ucActivePage number of page, which is set as active at the moment
 * \param    ucPageToDefragment number of page, which is dedicated to erase at the moment
 *
 * \return true, if page erase was interrupted. Otherwise false.
 */
extern bool VEEPROM_bCheckForDefragmentedAndNotErasedPage( uint8_t ucActivePage, uint8_t ucPageToDefragment );

/**
 * \brief    Observer function: set mark for given page that process of erasing page succeeded fully
 * 
 * \param    ucPageNumber Number of page that shall be marked
 */
extern void VEEPROM_vSetPageEraseMark( uint8_t ucPageNumber );

/**
 * \brief    Observer function: Check if page is erased fully including magic number
 *           written at end of page erase
 * 
 *           if page erase magic number does not exist we expect that there was a reset
 *           while erasing page. Thus we erase the page again.
 */
extern void VEEPROM_vEnsurePageIsErased( uint8_t ucPageNumber );

/**
 * \brief    Observer function: Check if page data are valid
 * 
 *           if page erase magic number does not exist we expect that data cannot be valid.
 *           Thus we erase the page again.
 */
extern bool VEEPROM_bPageDataValid( uint8_t ucPageNumber );

/**
 * \brief    Observer function: Check if memory range is empty
 * 
 * \param    ucPageNumber Number of page
 * \param    pucStartAddress first address of area to be checked
 * 
 * \return   true, if memory area is empty, otherwise false
 * 
 */
extern bool VEEPROM_bIsPageRemainderEmpty(uint8_t ucPageNumber, uint8_t *pucStartAddress);

/**
 * \brief    Observer function: Gets position of last non-empty byte
 *           on memory page area
 * 
 * \param    ucPageNumber Number of page
 * \param    pucStartAddress first address of area to be checked
 * 
 * \return   pointer to address
 * 
 */
extern uint8_t *VEEPROM_pucGetFreeAreaBase(uint8_t ucPageNumber, uint8_t *pucStartAddress);

/**
 * \brief Observer function: set magic number and its address
 */
extern void VEEPROM_vSetEraseMarkData(uint8_t ucPageNumber, uint8_t* pucMagicNumber, uint8_t** pucMagicNumberAddress );
#endif //defined(VEEPROM_LARGE)

#ifdef __cplusplus
}
#endif

#endif //VEEPROM_MC_XS_H
