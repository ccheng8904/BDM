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
#ifndef INTFLASH_ABS_H
#define INTFLASH_ABS_H

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
 *  \brief    public definitions for internal flash memory abstraction.
 *  \details  This uinit provides platform independent interface implementation
 *            for internal flash. The resulting code must be placed into RAM,
 *            Therfore this is optimized to get a less as possible code.
 *            Code which might be run into flash must be implemented in an
 *            upper layer.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief atomic write to flash.
 *
 *  \details This interface performs an atomic write to flash.
 *           The flash controller decides how many bytes are written in a single
 *           step. The interface blocks until writing is finished.
 *  \note For security reasons this interface does not delock the flash,
 *        DRVIF_initFlash() need to be called before and DRVIF_dinitFlash after.
 *
 *  \note Interrupts must be deactivated before calling this interface.
 *  \note Depending on pltform, usually ths interface must be allocated inside RAM 
 *
 *  \param addr: start address of block to write
 *  \param size: length of data to write
 *  \param buffer: data buffer to write
 *  \return number of written bytes, zero in case of any kind of error
 */
extern uint32_t ABSIF_write(uint32_t addr, uint32_t size, const uint8_t* buffer);



#ifdef __cplusplus
}
#endif

#endif
