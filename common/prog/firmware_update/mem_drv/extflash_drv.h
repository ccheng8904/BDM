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
#ifndef EXTFLASH_DRV_H
#define EXTFLASH_DRV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  mal
 *
 *  \brief    public definitions for external serial flash driver.
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
extern const struct MEMDRV_driver_s DRVSF_driver;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief initializes the flash.
 */
extern void DRVSF_initFlash(void);


/** \brief de-initializes the flash.
 */
extern void DRVSF_deinitFlash(void);


/** \brief atomic write to flash.
 *
 *  \details This interface performs an atomic write data from buffer to flash.
 *           The flash controller decides how many bytes are written in a single
 *           step.
 *  \note After calling this interface you need to wait until DRVSF_isBusy()
 *        returns 'false'. Afterwards the success must be checked by calling
 *        DRVSF_isError()
 *  \note Do not call this interface while DRVSF_isBusy() returns 'true'.
 * 
 *  \param addr: start address of block to write
 *  \param size: length of data to write (number of bytes)
 *  \param buffer: data buffer to write
 *  \return number of written bytes, zero in case of invalid address
 */
extern uint32_t DRVSF_write(uint32_t addr, uint32_t size, const uint8_t* buffer);


/** \brief erases a single flash block for given start address.
 *
 *  \note See notes for interface DRVSF_write().
 *
 *  \param addr: start address of block to erase
 *  \param size: number of bytes to be erased
 *  \return number of erased bytes, zero in case of invalid address
 */
extern uint32_t DRVSF_erase(uint32_t addr, uint32_t size);


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
extern uint32_t DRVSF_checkErase(uint32_t addr, uint32_t size);


/** \brief    Checks if start address and range is valid.
 *
 *  \param    addr:  address to check
 *  \param    range: range to check
 *  \return   true if address and range is valid, false otherwise
 */
extern bool DRVSF_isAddrRangeValid(uint32_t addr, uint32_t range);


/** returns true if flash is busy, false otherwise.
 */
extern bool DRVSF_isBusy(void);


/** returns true if any error occurred, false otherwise
 */
extern bool DRVSF_isError(void);


/** \brief Resets all flash operations and errors.
 */
extern void DRVSF_resetFlash(void);


/** \brief Reads data from the external flash memory to the buffer.
 *
 *  \param addr address to read from
 *  \param size number of bytes to read
 *  \param buffer read-buffer to hold the read data with sizeof(buffer)>= size
 *
 */
extern bool DRVSF_read(uint32_t addr, uint32_t size, uint8_t* buffer);


#ifdef __cplusplus
}
#endif

#endif
