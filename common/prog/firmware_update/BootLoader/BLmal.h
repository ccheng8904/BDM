/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


#ifndef BLMAL_H
#define BLMAL_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Public definitions of unit BLmal (Boot-Loader Memory Abstraction).
 *
 *  \details  Provides interfaces for memory abstraction required for RAM-Loader.
 *            The correct driver will be selected depending on specified address
 */




/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/** Interface need to be called before any other */
extern void BLMAL_init(void);


/** \brief writes a buffer of bytes to flash,
 *  
 *  \param addr: start address of block to write
 *  \param size: length of data to write
 *  \param buffer: data buffer to write
 *  \return true if writing succeed
 */
extern bool BLMAL_writeBuffer(uint32_t addr, uint32_t size, const uint8_t* buffer);


/** \brief erases a single flash block for given start address.
 *  
 *  \param addr: start address of block to erase
 *  \return true if erasing succeed
 */
extern bool BLMAL_eraseBlock(uint32_t addr);


/** \brief read a number of bytes into given buffer.
 *  
 *  \param addr: start address of block to read
 *  \param size: length of data to read
 *  \param buffer: data buffer to read
 *  \return true if reading succeed
 */
extern bool BLMAL_readBuffer(uint32_t addr, uint32_t size, uint8_t* buffer);


/** \brief delivers the number of entries in \sa BLMAL_driverTable
 *  \note this function need to be implemented in BLmal_config.c
 *  \return number of entries
 */
extern uint8_t BLMAL_getDriverTableSize(void);


#ifdef __cplusplus
}
#endif

#endif
