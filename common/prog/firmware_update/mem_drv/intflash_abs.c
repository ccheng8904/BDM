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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    implementation for internal flash memory abstraction.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "intflash_drv.h"
#include "intflash_abs.h"
#include "hsup.h"

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
uint32_t ABSIF_write(uint32_t addr, uint32_t size, const uint8_t* buffer)
{
    uint32_t writtenSize;
    HSUP_vDisableInt();
    writtenSize = DRVIF_write(addr, size, buffer);/* write to flash           */
    while(DRVIF_isBusy()){}                       /* wait for operation finish*/
    if(DRVIF_isError())
    {   /* set written size to zero in case of error */
        writtenSize = 0;
    }
    DRVIF_resetFlash();
    HSUP_vEnableInt();
    return writtenSize;
}
