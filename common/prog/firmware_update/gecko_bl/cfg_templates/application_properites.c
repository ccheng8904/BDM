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
 *  \ingroup  gecko_bootloader
 *
 *  \brief    Implementation of application properties.
 *  \details  This is a configuration file, never the less usually
 *            no changes are required.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "gecko_bl/api/application_properties.h"
#include "std_lib/std_symbol.h"
#include "IntTbl.h"

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/


static const ApplicationCertificate_t APPP_certificate =
{
    .structVersion = (uint8_t)APPLICATION_CERTIFICATE_VERSION,
    .flags     =  {0},
    .key       =  {0},
    .version   =   0u,
    .signature =  {0},
};

/* Hint: as Silabs provides a 32Bit value for version (only), we need to take
 *       the lower 8-Bit portion of version attributes only.
 *       This is required as rollback protection is enabled!
 */
static const uint32_t APPP_major8    = ((uint32_t)((uint8_t)VERSION_MAJOR)) << (uint32_t)16u;
static const uint32_t APPP_minor8    = ((uint32_t)((uint8_t)VERSION_MINOR)) << (uint32_t)8u;
static const uint32_t APPP_revision8 = ((uint32_t)((uint8_t)VERSION_REVISION));


SYMBOL_USED const ApplicationProperties_t ITBL_slAppProperties =
{
    .magic = APPLICATION_PROPERTIES_MAGIC,
    .structVersion = APPLICATION_PROPERTIES_VERSION,
    .signatureType = APPLICATION_SIGNATURE_NONE,
    .signatureLocation = ROM1_END + 1u,
    .app =
    {
        .type = APPLICATION_TYPE_BLUETOOTH_APP,
        .version = APPP_major8 + APPP_minor8 + APPP_revision8,
        .capabilities = 0UL,
        .productId = { 0U },
    },
    .cert = &APPP_certificate,
    .longTokenSectionAddress = NULL,
    .decryptKey = {0}
};

