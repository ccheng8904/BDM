/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update
 ******************************************************************************/


#ifndef FWU_OTP_MEMORY_H
#define FWU_OTP_MEMORY_H

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
 *  \brief    Implementation of one-time programable area.
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

#include "dbuspresentation_production_types.h"
#include "firmware_update/mal/moduleAdministration.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/

#if defined (PSOC4) || defined(CMS32M67XX)
    #define OTP_UNFLASHED_BYTE      0x00u
#else
    #define OTP_UNFLASHED_BYTE      0xFFu
#endif

/*lint -save -e9026 Function-like macro */
// Macro for calculation of aligned size based on minimum write block size
#define OTP_ALIGNED_SIZE(itemSize)  (((((itemSize) - 1u) / MIN_WRITEBLOCK_SIZE) + 1u) * MIN_WRITEBLOCK_SIZE)


/* Axivion Disable Style MisraC2012-20.7: "Macro parameter not enclosed in parentheses" - It works correctly */
// Macro for creating declaration of OTP compatible type
#define OTP_TYPEDEF_ITEM(itemType, otpType)                     \
typedef union                                                   \
{                                                               \
    itemType value;                                             \
    const uint8_t raw_data[OTP_ALIGNED_SIZE(sizeof(itemType))]; \
} otpType;


// Macro for creating declaration of OTP compatible array type
#define OTP_TYPEDEF_ARRAY(itemType, otpType, arrayLength)                       \
typedef union                                                                   \
{                                                                               \
    itemType value[arrayLength];                                                \
    const uint8_t raw_data[OTP_ALIGNED_SIZE(sizeof(itemType) * (arrayLength))]; \
} otpType;
/* Axivion Enable Style MisraC2012-20.7 */
/*lint -restore -e9026 */

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/


extern bool OTP_writeFlash(const uint8_t* addr, uint32_t size, const uint8_t* const data);

extern bool OTP_isEmpty(const uint8_t* addr, uint32_t size);


extern bool OTP_writeProductionTime(const struct DBPL_ProductionTime* const productionTime);

extern const struct DBPL_ProductionTime* OTP_getProductionTime(void);


extern bool OTP_writeTracingID(const struct DBPL_TracingId* const tracingID);

extern const struct DBPL_TracingId* OTP_getTracingID(void);


extern bool OTP_writeHardwareVersion(const struct FWU_hw_version_s* const hwVersion);

extern const struct FWU_hw_version_s* OTP_getHardwareVersion(void);


#ifdef REMOTE_FIRMWARE_UPDATE
extern bool OTP_writeFirmwareIDs(const struct MAL_fw_id_array* const firmwareIDs);

extern void OTP_initializeFirmwareIDArray(struct MAL_fw_id_array* const firmwareIDArray);

extern const struct MAL_fw_id_array* OTP_getFirmwareIDs(void);
#endif /* REMOTE_FIRMWARE_UPDATE */


#ifdef DBUS2_APPLIANCE_MSG_IN_APP
extern const struct DBPL_ApplianceData* OTP_getApplianceData(void);
#endif /* DBUS2_APPLIANCE_MSG_IN_APP */


#ifdef __cplusplus
}
#endif

#endif /* FWU_OTP_MEMORY_H */
