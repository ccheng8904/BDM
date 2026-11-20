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


#ifndef MEM_DRV_H
#define MEM_DRV_H

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
 *  \brief    Public definitions for memory driver.
 *
 */



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "firmware_update/mal/mal_statuses.h"
#include "stdcrc/StdCrc.h"
#include "firmware_update/mal/mal_driver.h"

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
enum MEMDRV_timeout_e
{
    MEMDRV_TMO_2S    =  1u,
    MEMDRV_TMO_4S    =  2u,
    MEMDRV_TMO_8S    =  3u,
    MEMDRV_TMO_16S   =  4u,
    MEMDRV_TMO_32S   =  5u,
    MEMDRV_TMO_64S   =  6u,
    MEMDRV_TMO_128S  =  7u,
    MEMDRV_TMO_250S  =  8u,
    MEMDRV_TMO_500S  =  9u,
    MEMDRV_TMO_1000S = 10u
};

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

#ifndef FWU_MEM_DRV_READ_ONLY
/** \brief Structure for memory abstraction interfaces.
 *  \details All addresses in this driver are physical addresses.
 *           The parameter size gives the number of bytes.
 */
struct MEMDRV_driver_s
{
    /** false if driver expects that interrupts are masked during flash
      * operations. When interrupt vector table is located into ROM, this shall be false
      * \note During write/erase no read access to flash (even code fetch) is allowed .
      */
    bool allowInterrupts;

    /**  \brief erase/write timeout
     *   timeout = 2^timeOutPwr2 (e.g. timeOutPwr2=3 -> timeout=8s)
     */
    enum  MEMDRV_timeout_e timeOutPwr2;

    /**  \brief smallest write block in bytes */
    size_t writeBlockSize;

    /** interface for initialization, to be called before any other operation
     */
    void (*init)(void);

    /** interface for de-initialization, to be called to close final operation
     */
    void (*deinit)(void);

    /** need to be called to check if driver supports a given address range*/
    bool (*isAddrRangeValid)(uint32_t addr, uint32_t size);

    /** \brief resets command and error flags.
     *  This interface need to be called after any error happened
     */
    void (*reset)(void);

    bool (*isBusy)(void);  ///< checks if any operation is pending

    bool (*isError)(void); ///< checks if any error happened after operation finished

    /** interface for writing data, to flash */
    uint32_t (*write)(uint32_t addr, uint32_t size, const uint8_t* buffer);

    /** interface to erase flash memory */
    uint32_t (*erase)(uint32_t addr, uint32_t size);

    /** Interface to read a certain number of bytes.
     *  For internal memory it should be NULL (null pointer)
     *  As internal memory is mapped in the address space memcpy may be used
     *  instead.
     *  Also CRC does not need any read operation for internal memory.
     */
    bool (*read)(uint32_t addr, uint32_t size, uint8_t* buffer);

    /** interface to check if memory area is erased */
    uint32_t (*checkErase)(uint32_t addr, uint32_t size);
};

#else
/** \brief Structure for read only memory abstraction interfaces.
 *  \details All addresses in this driver are physical addresses.
 *           The parameter size gives the number of bytes.
 */
struct MEMDRV_driver_s
{
    /** interface for initialization, to be called before any other operation
     */
    void (*init)(void);

    /** interface for de-initialization, to be called to close final operation
    */
    void (*deinit)(void);

    /** need to be called to check if driver supports a given address range*/
    bool (*isAddrRangeValid)(uint32_t addr, uint32_t size);

    /** Interface to read a certain number of bytes.
     *  For internal memory it should be NULL (null pointer)
     *  As internal memory is mapped in the address space memcpy may be used
     *  instead.
     *  Also CRC does not need any read operation for internal memory.
     */
    bool (*read)(uint32_t addr, uint32_t size, uint8_t* buffer);
};
#endif

/** data structure for memory driver entry */
struct MEMDRV_driver_entry_s
{
    const struct MEMDRV_driver_s* driver;   //< pointer to memory driver
    enum         MAL_DriverID_e   driverID; //< identifier of memory driver
    uint32_t bp2_offset_addr; //< virtual offset address for bp2 protocol
};
/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
extern const struct MEMDRV_driver_entry_s MEMDRV_driverTable[];


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Initializes and opens the current memory driver for operation
 *  \note No operation allowed before calling this interface.
 *  \note Only one driver may be opened
 *  \param driverId identifier for memory driver
 *  \return  MAL_ST_OK or MAL_ST_SETUP_MODULE_ERROR
 */
extern enum MAL_Status MEMDRV_open(enum MAL_DriverID_e driverId);


/** Closes the current driver
 *  \details This interface also calls the MEMDRV_flush interface to finish
 *           possible ongoing operations.
 *  \return  MAL_ST_OK if operation has completed successfully,
 *           else MAL_ST_WRITE_BUFFER_DATA_LEFT or MAL_ST_WRITE_ERROR
 */
extern enum MAL_Status MEMDRV_close(void);


/** returns the timeout of the opened memory driver */
extern enum MEMDRV_timeout_e MEMDRV_getTimeout(void);


/** Schedules an ongoing write operation if there is data in write buffer.
 *  \details If any error occurs during schedule, the next operation will
 *           return this result as the last occurred error.
 */
extern void MEMDRV_schedule(void);


/** \brief Erases a defined memory range
 *
 *  \param addr:   physical start address of the memory range to erase
 *  \param size:   number of of the memory range
 *  \return MAL_ST_OK if operation has completed successfully or one of
 *          [MAL_ST_OUT_OF_RANGE_ERROR, MAL_ST_ERASE_ERROR].
 */
extern enum MAL_Status MEMDRV_eraseRange(uint32_t addr, uint32_t size);


/** Flushes all data form ring buffer to according memory
 *  \return MAL_ST_OK if operation has completed successfully
 */
extern enum MAL_Status MEMDRV_flush(void);


/** \brief Writes data to erased memory
 *
 *  \param addr:   physical start address of the memory to write
 *  \param size:   number of bytes of the memory to write
 *  \return MAL_ST_OK if operation has completed successfully or one of
 *          [MAL_ST_OUT_OF_RANGE_ERROR, MAL_ST_WRITE_ERROR].
 */
extern enum MAL_Status MEMDRV_write(uint32_t addr, uint32_t size, const uint8_t *data);


/** \brief Reads data from memory
 *
 *  \param driver: pointer to the memory driver
 *  \param addr:   physical start address of the memory to read from
 *  \param size:   number of bytes of the memory to read
 *  \return MAL_ST_OK if operation has completed successfully or one of
 *          [MAL_ST_OUT_OF_RANGE_ERROR, MAL_ST_READ_ERROR].
 */
extern enum MAL_Status MEMDRV_read(uint32_t addr, uint32_t size, uint8_t *data);


/** \brief calculates a CRC for a defined memory range
 *
 *  \note The CRC is platform specific
 *
 *  \param addr:   physical start address of the memory
 *  \param size:   number of bytes of the memory to perform CRC
 *  \return MAL_ST_OK if operation has completed successfully or one of
 *          [MAL_ST_OUT_OF_RANGE_ERROR, MAL_ST_READ_ERROR].
 */
extern enum MAL_Status MEMDRV_calcCrc(uint32_t addr, uint32_t size, CRC_crc_t *crc);

#ifdef __cplusplus
}
#endif

#endif
