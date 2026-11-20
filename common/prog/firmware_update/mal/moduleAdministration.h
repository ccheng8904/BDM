/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Firmware Update Memory Abstraction Layer
 ******************************************************************************/


#ifndef MODULEADMINISTRATION_H
#define MODULEADMINISTRATION_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     moduleAdministration.h
 *
 *  \ingroup  firmware_update
 *
 *  \brief    Public definitions and declarations for unit moduleAdministration.
 *
 *  \details  Provides interfaces and definition for module administration
 *            e.g. module partitioning and versioning.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "std_lib/std_symbol.h"
#include "firmware_update/version/fwu_version.h"
#include "firmware_update/mal/module_administration_cfg.h"
#include "firmware_update/mal/mal_driver.h"
#include "stdcrc/StdCrc.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/* maximum Firmware IDs to be written to OTP memory */
#define FWU_FW_ID_MAX_COUNT           10u

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/** \brief  Union of integer and pointer to enable clean access without cast */
union MAL_addressPtr//lint !e9018 "union type/object declared MisraC2012 9.3 advisory"
{
    uint32_t addr; ///< enable access as integer
    void *   ptr;  ///< enable access as pointer
};


/** \brief    partition entry structure definition */
struct MAL_partition_s
{
    union MAL_addressPtr  physical_start; ///< physical start address
    union MAL_addressPtr  physical_end;   ///< physical end address
    #if defined (BM_BACKWARD_COMPATIBILITY)
    uint32_t signature;               ///< only used in BM_BACWARD_COMPATIBILITY, it is null
    #endif //defined (BM_BACKWARD_COMPATIBILITY)
    enum  MAL_ModuleEnum  mod_number;     ///< module number
    enum  MAL_DriverID_e  driver_id;      ///< driver identifier
    uint8_t               mod_attributes; ///< module attributes
    uint8_t               mod_name[16];   ///< module name
    enum  MAL_ModuleEnum  dep_module;     ///< dependency module number
};


/** \brief constant data memory map structure definition */
struct MAL_DataAnchor_s
{
    #if defined (BM_BACKWARD_COMPATIBILITY)
    const struct FWU_hw_version_s *hw_version;        ///< points to hw version(s)
    #endif //defined (BM_BACKWARD_COMPATIBILITY)
    const struct MAL_partition_s *partitionTblFix;  ///< points to fixed partition table
    const struct MAL_partition_s *partitionTblFlex; ///< points to flexible partition table
};


typedef void (*MAL_isrPtr_t)(void); ///< function pointer to interrupt service routine


/** \brief    structure for array of IDs */
struct MAL_id_array_s
{
    uint8_t id[STDV_ID_LENGTH];
};


/** \brief    structure for storing one entry of firmware ID linked to specific module
 *
 *  \details  At startup the BM check if all firmware IDs in firmware ID list that are stored
 *            in OTP memory are are compatible with firmware ID in individual module headers.
 */
struct MAL_fw_id_entry
{
    enum    MAL_ModuleEnum  mod_number;
    struct  MAL_id_array_s  firmware_id;
};


/** \brief    structure for storing an array of firmware IDs linked to specific modules in OTP memory
 *
 *  \details  At startup the BM check if all firmware IDs in firmware ID list that are stored
 *            in OTP memory are are compatible with firmware ID in individual module headers.
 */
struct MAL_fw_id_array
{
    CRC_crc_t CRC;
    struct MAL_fw_id_entry entries[FWU_FW_ID_MAX_COUNT];
}; /*lint -esym(9045,*MAL_fw_id_array) "non-hidden definition of type 'struct MAL_fw_id_array'" */


/** \brief    software module header structure definition
 *
 *  \details  All sw modules shall start with this structure
 */
struct MAL_ModuleHeader_s
{
    uint32_t       crc_expected;     ///< the crc value of the present module
    const MAL_isrPtr_t *ISRtbl_ptr;       ///< pointer to start of vector table
    uint32_t       size;             ///< number of bytes used by memory module incl. module header
    uint32_t       executionAddress; /** \brief execution address for the module
                                         usually the physical start address of the module,
                                         in all other cases module will be copied to its
                                         execution address before starting
                                      */
    struct FWU_sw_version_s sw_version; ///< present sw-version
    uint16_t reserved; ///< unused, currently used to enforce 4 byte alignment
    /** pointer to array of hardware identifiers (for which SW has been build) */
    const struct MAL_id_array_s *hw_id_array;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
/*  The following data are declared externally, as it will be accessed by
 *  different applications.
 *  \todo move external data definition to private header
 */


#ifdef FWU_FLEX_PARTITION_USED
/** \brief flexible project specific part of partition table
 *  \details This table must be specified by application and
 *           may be updated by firmware update process.
 */
extern const struct MAL_partition_s MAL_partitionFlex[MAL_FLEX_PARTITION_ENTRIES];
#endif

#ifdef VARIANT_BOOTMANAGER
/** \brief fixed part of partition table */
extern const struct MAL_partition_s MAL_partitionFix[MAL_FIX_PARTITION_ENTRIES];

/** \brief hardware version of present product */
extern const struct FWU_hw_version_s MAL_hw_version[MAL_HW_VERSION_ENTRIES];
#endif


/** \brief    data map points to shared const data
 *
 *  \details  The following data is placed in a special segment and is used
 *            to share constant data between the boot manager and other applications.
 *  \note structure will be compiled for all application using mal interfaces,
 *        but binaries output of boot manager is used only for this structure
 */
extern const struct MAL_DataAnchor_s MAL_DataAnchor;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief This Interface initialize the partition table
 *
 *  \details it must be called to initialize this unit
 */
void MAL_initPartition(void);


/** \brief    This interface returns the pointer the the requested partition
 *            entry.
 *
 *  \param    module:   enumerator of requested module
 *
 *  \note     Interface \sa MAL_initPartition must be called first
 *  \return   pointer to requested partition in case of success, else NULL
 */
extern const struct MAL_partition_s * MAL_GetMemModule(enum MAL_ModuleEnum module);



/** \brief delivers the number of memory modules
 *
 *  \return number of memory modules provided
 */
extern uint8_t MAL_GetMemModuleCount(void);


/** \brief    This interface returns the pointer the the requested partition
 *            entry form index.
 *
 *  \param    index:   index of requested memory module
 *
 *  \note     Interface \sa MAL_initPartition must be called first
 *  \return   pointer to requested partition in case of success, else NULL
 */
extern const struct MAL_partition_s * MAL_GetMemModuleByIndex(uint8_t index);


/** \brief    This interface returns the size of the partition
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \note     Interface \sa MAL_initPartition must be called first
 *  \return   size of the partition in bytes
 */
extern uint32_t MAL_getPartitionSize(const struct MAL_partition_s * partition);


/** \brief    This interface returns the header of a partition
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \return   pointer to header
 */
extern const struct MAL_ModuleHeader_s * MAL_PartitionToHeader(
       const struct MAL_partition_s *partition
       );


/** \brief    This interface validates the integrity of a module inside partition
 *
 *  \param    partitionToStart:   pointer to partition table entry
 *
 *  \note     Interface \sa MAL_initPartition must be called first
 *  \return   true if module integrity is valid, otherwise false
 */
extern bool MAL_isModuleIntegrityOk(const struct MAL_partition_s * partitionToStart);


/** \brief    Checks if a given partition contains a build-in application
 *
 *  \param    partition:   pointer to partition table entry
 *
 *  \return   true if partition contains a build-in application
 */
extern bool MAL_isBuildInApplication(const struct MAL_partition_s * partition);


/** \brief    Function for copying Firmware ID numbers from module headers to OTP memory
 *
 *  \note     This interface is enabled in applicaiton as BM does not have drivers for writing
 *  \return   true write has been successful;
 */
#if defined(APP_VARIANT)
extern bool MAL_writeFirmwareIDs(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // MODULEADMINISTRATION_H
