/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          your project
 ******************************************************************************/
#ifndef MY_MEMORY_DRV_H
#define MY_MEMORY_DRV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  msp
 *
 *  \brief    public definitions for flash driver.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "firmware_update/mem_drv/mem_drv.h"

/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
/** driver structure, containing pointers to memory access functions */
extern const struct MEMDRV_driver_s DRVMD_driver;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief initializes the flash controller.
 */
extern void DRVMD_initFlash(void);


/** \brief dinitializes the flash controller.
 */
extern void DRVMD_deinitFlash(void);


/** \brief atomic write to flash.
 *
 *  \details This interface performs an atomic write to flash.
 *           The flash controller decides how many bytes are written in a single
 *           step.
 *  \note After calling this interface you need to wait until DRVMD_isBusy()
 *        returns 'false'. Afterwards the success must be checked by calling
 *        DRVMD_isError()
 *  \note Do not call this interface while DRVMD_isBusy() returns 'true'.
 * 
 *  \param addr: start address of block to write
 *  \param size: length of data to write
 *  \param buffer: data buffer to write
 *  \return number of written bytes, zero in case of invalid address
 */
extern uint32_t DRVMD_write(uint32_t addr, uint32_t size, const uint8_t* buffer);


/** \brief erases a single flash block for given start address.
 *
 *  \note See notes for interface DRVMD_write().
 *
 *  \param addr: start address of block to erase
 *  \param size: number of bytes to be erased
 *  \return number of erased bytes, zero in case of invalid address
 */
extern uint32_t DRVMD_erase(uint32_t addr, uint32_t size);


/** \brief checks if a given area of memory is erased.
 *  
 *  \note  This driver decided how many bytes will be checked in a single call.
 *         Driver for internal flash shall check complete memory in a single step.
 *  \note  Do not call this for checking if a preceding erase has been successful,
 *         as some platforms may not require an erase.
 *  \note  A driver may return 0 without checking (e.g. to force an erase)
 *  \param addr: start address of block for erase check
 *  \param size: number of bytes to be checked
 *  \return number of checked erased bytes, 0 means not erased
 */
extern uint32_t DRVMD_checkErase(uint32_t addr, uint32_t size);


/** \brief    Checks if start address and range is valid.
 *
 *  \param    addr:  address to check
 *  \param    range: range to check
 *  \return   true if address and range is valid, false otherwise
 */
extern bool DRVMD_isAddrRangeValid(uint32_t addr, uint32_t range);


/** returns true if flash is busy, false otherwise.
 */
extern bool DRVMD_isBusy(void);


/** returns true if any error occurred, false otherwise
 */
extern bool DRVMD_isError(void);


/** \brief Resets all flash operations and errors.
 */
extern void DRVMD_resetFlash(void);


/** \brief read some bytes from according memory
 *  
 *  \param addr: start address of block to read
 *  \param size: number of bytes to read
 *  \param buffer: buffer to copy the read bytes to
 *  \return true if operation succeed, otherwise false
 */
extern bool DRVMD_read(uint32_t addr, uint32_t size, uint8_t* buffer);


#ifdef __cplusplus
}
#endif

#endif
