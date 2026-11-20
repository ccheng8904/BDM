/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          FLASH
 ******************************************************************************/
#ifndef FLASHMCXS_H
#define FLASHMCXS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

/** \file     flashMcXs.h
 *
 *  \brief    Public declarations and definitions of flash types and defines for SH30F9X71
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
typedef       uint8_t tAddress  ;  ///< type for address
typedef       uint8_t tReadData ;  ///< type for data to be read
typedef const uint8_t tWriteData;  ///< type for data to be written
/** Data size (16-bit data) */
typedef uint16_t TFlashData;
/** \brief Flash operation states */
typedef enum _T_FLASH_OPSTATE
{
   FLASH_STATE_PROGRAM   ,   ///< write operation called
   FLASH_STATE_READ      ,   ///< read operation called
   FLASH_STATE_ERASE     ,   ///< erase operation called
   FLASH_STATE_OPTIONS   ,   ///< write option bytes operation called
   FLASH_STATE_LOCK_CHECK    ///< lock check operation called
}T_FLASH_OPSTATE;

/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
extern const uint16_t FLASH_blockCount          ;
extern const uint16_t FLASH_blockSize           ;
extern const uint16_t FLASH_protectedPagesPerPin;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief    Restarts the Watchdog timer
 *
 *  \details  This function is used when polling the flash ready bit to avoid
 *            watchdog timer expiration and therefore a reset during flash operation.
 *  \return   void
*/
void _FLASH_resetWatchdogTimer(void);

/* _FLASH_vOperationStart() */
/** \brief    Perform all actions needed immediately before the FLASH access
 *
 *  \details  This function is called immediately before the FLASH operation is started,
 *            i.e. before writing commands to the flash controller. The function might be
 *            used to conditionally enable the suspend mode of the flash controller.
 *
 *  \param    operation Actually performed flash operation
 *
 *  \return   void
 *
 */
void _FLASH_vOperationStart(T_FLASH_OPSTATE operation);

/* _FLASH_vOperationFinished() */
/** \brief    Perform all actions needed immediately after the FLASH access
 *
 *  \details  This function is called immediately after the FLASH operation is finished,
 *            i.e. after the command execution by the flash controller. The function
 *            might be used to conditionally restore all settings that were valid before
 *            the FLASH operation (e.g. erase suspend).
 *
 *  \param    operation Actually performed flash operation
 *
 *  \return   void
 *
 */
void _FLASH_vOperationFinished(T_FLASH_OPSTATE operation);

/* FLASH_eraseAll() */
/** \brief    Erases all blocks.
 *
 *  \param    none
 *  \return
 *            #true    - successful
 *            #false   - failed
 *  \remarks
 *            - This function erases all blocks.
 *            - This function takes longer time so do not use watchdog
 *              timer for program flow control.
 */
bool FLASH_eraseAll(void);

/* FLASH_readLockStatus() */
/** \brief    Read the lock bit status of given ROM block
 *
 *  \param    address - an address from desired memory block
 *  \param   *isLocked  - pointer to result status
 *  \return
 *           #true    - test successful \n
 *           #false   - test failed
 *  \remarks
 *           - address must be an address from desired memory block.
 *           - if function returns true, lock result can be read as second parameter
 *             otherwise is result invalid.
 */
bool FLASH_readLockStatus(tAddress *address, bool *isLocked);

/* FLASH_getStartAddress() */
/** \brief    Gets the start address of given ROM block number
 *
 *  \param    blockNumber - number of desired memory block
 *  \return
 *            - Start address for given memory block number in 4 bytes format.
 *            - value 0 if block number is invalid
 *  \remarks
 *            - blockNumber must be a valid value for given derivative.
 */
uint32_t FLASH_getStartAddress(uint16_t blockNumber);

/* FLASH_getEndAddress() */
/** \brief    Gets the last address of given ROM block number
 *
 *  \param    blockNumber - number of desired memory block
 *  \return
 *            - End address for given memory block number in 4 bytes format.
 *            - value 0 if block number is invalid
 *  \remarks
 *            - blockNumber must be a valid value for given derivative.
 */
uint32_t FLASH_getEndAddress(uint16_t blockNumber);

/* FLASH_getPageNumber()*/
/** \brief    Gets the number of flash page for requested memory address
 *
 *  \param    address - address of flash memory
 *  \return
 *            - page number. \n
 *  \remarks
 *            - address must be a valid value for given derivative.
 */
uint8_t FLASH_getPageNumber(tAddress *address);

#ifdef __cplusplus
}
#endif

#endif //FLASHMCXS_H
