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


#ifndef BLCOMMANDS_H
#define BLCOMMANDS_H

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
 *  \brief    public boot loader command definitions.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define BL_INVALID_CMD         0x00u       ///< Invalid command
#define BL_HAND_SHAKE          0x54u       ///< Start-up notification, boot loader sends this after reset
#define BL_FRAME_LEGTH         0x4u        ///< Frame without checksum


/* COMMANDS USUALLY USED BY ROM-LOADER ****************************************************************/
#define BL_STORE_DATA          0x0Au       ///< Stores received data block at the specified address in RAM.
#define BL_JUMP_LOADER         0x1Au       ///< Does a jump to the specified address, usually to RAM-Loader
#define BL_SELF_IDENT          0x5Bu       ///< Requests the electronic to identify itself, a ID-String will be transmitted      
#define BL_SET_BAUDRATE        0x04u       ///< Change the baud rate
#define BL_SET_PAGE            0x77u       ///< Sets the upper 16-bit of a long address

/* COMMANDS USUALLY USED BY RAM-LOADER ****************************************************************/
#define BL_JUMP_APPLICATION    0x02u       ///< Jump about the reset to the standard application
#define BL_PAGE_ERASE          0x08u       ///< Erase Flash-ROM page wise
#define BL_MASS_ERASE          0x09u       ///< Erase complete flash 
#define BL_PROGRAMME_DATA      0x18u       ///< Programme date in Flash-ROM page wise
#define BL_READ_DATA           0x38u       ///< Read data from a memory block
#define BL_CALC_CRC            0x39u       ///< Calculate CRC
#define BL_BREAK               0x33u       ///< Preliminary: Breaks a current erase or programming command




/* ACKNOWLEDGE DEFINITIONS ****************************************************************************/
#define BL_ACKNOWLEDGE_OK      0xA0       ///< Positive Acknowledge
#define BL_FLASH_CMD_OK        0xB0       ///< Positive acknowledge for erasing and programming flash
#define BL_CRC_OK              0xC0       ///< Positive acknowledge for successfully finished CRC

#define BL_UNKNOWN_COMMAND     0x01       ///< Notify that received command is unknown
#define BL_WRONG_CHECKSUM      0x02       ///< Notify that there was a checksum failure
#define BL_PROGRAMMING_FAILED  0x04       ///< Notify that programming was failed
#define BL_FLASH_CMD_FAILED    0x04       ///< Notify that any flash error happened (may be used in case of minimal application)
#define BL_ERASE_FAILED        0x05       ///< Notify that erasing was failed
#define BL_TRANSMISSION_ERROR  0x06       ///< Notify that there was an transmission error (e.g. parity, overflow, framing)
#define BL_RECEIVER_TIMEOUT    0x07       ///< Notify that there was a receive time-out
#define BL_CRC_FAILED          0x08       ///< Notify that CRC check has been failed
#define BL_READ_FAILED         0x09       ///< Notify that read has been failed

#ifdef __cplusplus
}
#endif

#endif // BLCOMMANDS_H
