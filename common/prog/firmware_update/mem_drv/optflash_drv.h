/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 ******************************************************************************/
#ifndef OPTFLASH_DRV_H
#define OPTFLASH_DRV_H

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
 *  \brief    public definitions for option byte flash driver.
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
extern const struct MEMDRV_driver_s DRVOF_driver;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief initializes the flash controller.
 */
extern void DRVOF_initFlash(void);


/** \brief dinitializes the flash controller.
 */
extern void DRVOF_deinitFlash(void);


/** \brief atomic write to option byte flash.
 *
 *  \details This interface performs an atomic write to option byte flash.
 *           The flash controller decides how many bytes are written in a single
 *           step.
 * 
 *  \param addr: start address of block to write
 *  \param size: length of data to write
 *  \param buffer: data buffer to write
 *  \return number of written bytes, zero in case of invalid address
 */
extern uint32_t DRVOF_write(uint32_t addr, uint32_t size, const uint8_t* buffer);


/** \brief erases a single flash block for given start address.
 *  
 *  \param addr: start address of block to erase
 *  \param size: number of bytes to be erased
 *  \return number of erased bytes, zero in case of invalid address
 */
extern uint32_t DRVOF_erase(uint32_t addr, uint32_t size);


/** \brief checks if a given area of memory is erased a single flash block for given start address.
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
extern uint32_t DRVOF_checkErase(uint32_t addr, uint32_t size);


/** \brief    Checks if start address and range is valid.
 *
 *  \param    addr:  address to check
 *  \param    range: range to check
 *  \return   true if address and range is valid, false otherwise
 */
extern bool DRVOF_isAddrRangeValid(uint32_t addr, uint32_t range);


/** returns true if flash is busy, false otherwise.
 */
extern bool DRVOF_isBusy(void);


/** returns true if any error occurred, false otherwise
 */
extern bool DRVOF_isError(void);


/** \brief Resets all flash operations and errors.
 */
extern void DRVOF_resetFlash(void);


/** \brief    reads from memory.
 *
 *  \param    addr:  address to read from
 *  \param    size:  number of bytes to read
 *  \param    buffer: pointer to data buffer
 *  \return   true if reading was successful, otherwise false
 */
extern bool DRVOF_read(uint32_t addr, uint32_t size, uint8_t* buffer);


#ifdef __cplusplus
}
#endif

#endif
