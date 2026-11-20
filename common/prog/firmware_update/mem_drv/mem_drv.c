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
 *  \brief    Implementation of memory driver
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "mem_drv.h"
#include "mem_utility/ring_buffer.h"
#include "firmware_update/mal/mal_statuses.h"
#include "utility.h"
#include "hwdt.h"
#include "hsup.h"
#define COMA_UNIT
#include "firmware_update/mem_drv/com_mal.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifndef MEMDRV_READ_BUFFER_SIZE
    #define MEMDRV_READ_BUFFER_SIZE  0x80u ///< size of read buffer
#endif

#ifndef FWU_MEM_DRV_READ_ONLY

#ifndef MEMDRV_RBUF_SIZE
    #define MEMDRV_RBUF_SIZE 400u ///< size of ring data buffer
#endif
#ifndef MEMDRV_WRITE_BUFFER_SIZE
    #define MEMDRV_WRITE_BUFFER_SIZE 0x80u ///< size of write buffer
#endif

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static enum MAL_Status MEMDRV_writeBuffered(uint32_t *size, const uint8_t **data);
static enum MAL_Status MEMDRV_writeBlock(void);
static enum MAL_Status MEMDRV_writeAtomic(uint32_t *addr, uint32_t *size, const uint8_t **data);
static enum MAL_Status MEMDRV_eraseAtomic(uint32_t *addr, uint32_t *size);
static bool MEMDRV_isEvenDataLeft(void);
static void MEMDRV_waitForOperationComplete(void);
static void MEMDRV_disableInt(void);
static void MEMDRV_enableInt(void);
#endif
static const struct MEMDRV_driver_s* MEMDRV_getDriver(enum MAL_DriverID_e driverId);
static bool MEMDRV_isInternalMemory(void);
static enum MAL_Status MEMDRV_entryCheck(uint32_t addr, uint32_t size);
static enum MAL_Status MEMDRV_calcCrcAtomic(uint32_t *addr, uint32_t *size, CRC_crc_t *crc);
static bool MEMDRV_readBlock(uint32_t addr, uint32_t size, uint8_t *data);
static uint32_t MEMDRV_min(uint32_t val1, uint32_t val2);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#ifndef FWU_MEM_DRV_READ_ONLY
    static struct RBUF_ringBuffer MEMDRV_rb; ///< ring buffer context
    static uint32_t MEMDRV_writeAddr;        ///< next address to write
#endif
    static enum MAL_Status MEMDRV_lastError;     ///< latest occurred error

/** pointer to active driver */
static const struct MEMDRV_driver_s * MEMDRV_driver = NULL;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*lint -e934 "Taking address of near auto variable [MISRA 2012 Rule 1.3, required]", relevant for DLLs only */

enum MAL_Status MEMDRV_open(enum MAL_DriverID_e driverId)
{
  #ifndef FWU_MEM_DRV_READ_ONLY
    static uint8_t  MEMDRV_ringData[MEMDRV_RBUF_SIZE]; ///< ring data buffer
    RBUF_init(&MEMDRV_rb, MEMDRV_ringData, MEMDRV_RBUF_SIZE);
  #endif
    MEMDRV_driver = MEMDRV_getDriver(driverId);
    if(MEMDRV_driver != NULL)
    {
#ifdef FWU_MEM_DRV_READ_ONLY
        if(!MEMDRV_isInternalMemory())
#endif
        {
            MEMDRV_driver->init();
        }

        MEMDRV_lastError = MAL_ST_OK;
        return MAL_ST_OK;
    }
    return MAL_ST_SETUP_MODULE_ERROR;
}


enum MAL_Status MEMDRV_read(uint32_t addr, uint32_t size, uint8_t *data)
{
    enum MAL_Status status = MEMDRV_entryCheck(addr,size);
    if(status == MAL_ST_OK)
    {
        if(!MEMDRV_readBlock(addr,size,data))
        {
            status =  MAL_ST_READ_ERROR;
        }
    }
    return status;
}


enum MAL_Status MEMDRV_calcCrc(uint32_t addr, uint32_t size, CRC_crc_t *crc)
{
    enum MAL_Status status = MEMDRV_entryCheck(addr,size);
    if(status == MAL_ST_OK)
    {
        *crc = CRC_Init();
        while((size > 0u) && (status == MAL_ST_OK))
        {
            status = MEMDRV_calcCrcAtomic(&addr, &size, crc);
        }
        *crc = CRC_Finish(*crc);
    }
    return status;
}


enum MAL_Status MEMDRV_close(void)
{
    enum MAL_Status status;
#ifndef FWU_MEM_DRV_READ_ONLY
    status = MEMDRV_flush();
#else
    status = MAL_ST_OK;
#endif
    MEMDRV_driver->deinit();
    MEMDRV_driver = NULL;
    return status;
}

#ifndef FWU_MEM_DRV_READ_ONLY
enum MEMDRV_timeout_e MEMDRV_getTimeout(void)
{
    return MEMDRV_driver->timeOutPwr2;
}


void MEMDRV_schedule(void)
{
    if((MEMDRV_driver != NULL) && (MEMDRV_driver->allowInterrupts))
    {
        enum MAL_Status status = MEMDRV_writeBlock();
        if(status != MAL_ST_OK)
        {
            MEMDRV_lastError = status;
        }
    }
}


enum MAL_Status MEMDRV_eraseRange(uint32_t addr, uint32_t size)
{
    enum MAL_Status status = MEMDRV_entryCheck(addr, size);
    if(status == MAL_ST_OK)
    {
        MEMDRV_disableInt();
        while((size > 0u) && (status == MAL_ST_OK))
        {
            status = MEMDRV_eraseAtomic(&addr, &size);
        }
        MEMDRV_driver->reset();
        MEMDRV_enableInt();
    }
    return status;
}


enum MAL_Status MEMDRV_write(uint32_t addr, uint32_t size, const uint8_t *data)
{
    enum MAL_Status status = MEMDRV_entryCheck(addr, size);
    if(status == MAL_ST_OK)
    {   // correct write address with possible content in ring buffer
        MEMDRV_writeAddr = addr - RBUF_getBytesToRead(&MEMDRV_rb);
        while((size > 0u) && (status == MAL_ST_OK))
        {
            status = MEMDRV_writeBuffered(&size, &data);
        }
    }
    return status;
}


enum MAL_Status MEMDRV_flush(void)
{
    enum MAL_Status status = MAL_ST_OK;
    uint16_t bytesToWrite = (uint16_t)RBUF_getBytesToRead(&MEMDRV_rb);
    
    if(bytesToWrite > 0u)
    {
        status = MEMDRV_entryCheck(MEMDRV_writeAddr, bytesToWrite);
        while((status == MAL_ST_OK) && (RBUF_getBytesToRead(&MEMDRV_rb) > 0u))
        {
            if(MEMDRV_isEvenDataLeft())
            {
                status = MEMDRV_writeBlock();
            }
            else
            {
                status = MAL_ST_WRITE_BUFFER_DATA_LEFT;
            }
        }
    }

    MEMDRV_driver->reset();
    RBUF_reset(&MEMDRV_rb);
    return status;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief Writes a block of data to the ring buffer.
 *  \details If there is not enough space inside ring buffer the a data block
 *           will be written to according memory
 *  \param size[in,out] in: number of bytes to write, out: number of bytes left
 *  \param data[in,out] in: pointer to data to place in ring buffer, out: next byte
 *  \return result of operation [MAL_ST_WRITE_ERROR, MAL_ST_OK]
 */
static enum MAL_Status MEMDRV_writeBuffered(uint32_t *size, const uint8_t **data)
{
    enum MAL_Status  status = MAL_ST_OK;
    // write as much as possible to ring buffer
    uint32_t wrSize = RBUF_writeBuffN(&MEMDRV_rb, *data, (RBUF_size_t)*size);

    *data += wrSize; /*lint !e9016 "Pointer arithmetic MisraC2012 18.4 advisory" tested to work correctly */
    *size -= wrSize;
    if(*size > 0u)
    {   /* ring buffer couldn't take all data, so write a block to flash */
        status = MEMDRV_writeBlock(); // write a block to flash
    }
    return status;
}


/** \brief Writes a block of data from ring buffer to according memory.
 *  \details This interface writes at most MEMDRV_WRITE_BUFFER_SIZE bytes
 *           to the according memory. rest smaller than minimal write block
 *           is ignored.
 *  \return result of operation [MAL_ST_WRITE_ERROR, MAL_ST_OK]
 */
static enum MAL_Status MEMDRV_writeBlock(void)
{
    enum MAL_Status  status = MAL_ST_OK;
    uint8_t  buf[MEMDRV_WRITE_BUFFER_SIZE];
    const uint8_t  *data  = buf;

    // copy data form ring to linear buffer
    uint32_t size = RBUF_readBuffN(&MEMDRV_rb, buf, (RBUF_size_t)MEMDRV_WRITE_BUFFER_SIZE);
    if(size > 0u)
    {
        MEMDRV_disableInt();
        while((size >= MEMDRV_driver->writeBlockSize) && (status == MAL_ST_OK))
        {
            status = MEMDRV_writeAtomic(&MEMDRV_writeAddr, &size, &data);
        }
        // copy unwritten data back to ring buffer
        (void)RBUF_writeBuff(&MEMDRV_rb, data, (RBUF_size_t)size);
        MEMDRV_driver->reset();
        MEMDRV_enableInt();
    }
    return status;
}


/** \brief Writes an atomic block of data from ring buffer to according memory.
 *  \details The memory driver decides how much bytes will be written.
 *  \param addr[in,out] in: start addr to write, out: next addr
 *  \param size[in,out] in: number of bytes to write, out: number of bytes left
 *  \param data[in,out] in: pointer to data to place in ring buffer, out: next byte
 *  \return result of operation [MAL_ST_WRITE_ERROR, MAL_ST_OK]
 */
static enum MAL_Status MEMDRV_writeAtomic(uint32_t *addr, uint32_t *size, const uint8_t **data)
{
    uint32_t writtenSize = MEMDRV_driver->write(*addr, *size, *data);
    MEMDRV_waitForOperationComplete();
    if(MEMDRV_driver->isError() || (writtenSize == 0u))
    {
        return MAL_ST_WRITE_ERROR;
    }
    *size  = ((*size > writtenSize) ? (uint32_t)(*size-writtenSize) : 0u);
    *addr += writtenSize;
    *data += writtenSize; /*lint !e9016 "Pointer arithmetic not allowed", MIRA advisory 18.4 */
    return MAL_ST_OK;
}


/** \brief Erases an atomic block inside according memory.
 *  \details The memory driver decides how much bytes will be erased.
 *  \param addr[in,out] in: start addr to erase, out: next erase addr
 *  \param size[in,out] in: number of bytes to erase, out: number of bytes left
 *  \return result of operation [MAL_ST_ERASE_ERROR, MAL_ST_OK]
 */
static enum MAL_Status MEMDRV_eraseAtomic(uint32_t *addr, uint32_t *size)
{
    uint32_t erasedSize = MEMDRV_driver->erase(*addr, *size);
    MEMDRV_waitForOperationComplete();
    if(MEMDRV_driver->isError() || (erasedSize == 0u))
    {
        return MAL_ST_ERASE_ERROR;
    }
    *size  = ((*size > erasedSize) ? (uint32_t)(*size-erasedSize) : 0u);
    *addr += erasedSize;
    return MAL_ST_OK;
}


/** Checks if even data is left in buffer.
 *  \details Underlying memory may have a minimal write block size,
 *           smaller blocks cannot be written to the memory.
 *  \return true if enough data available for a write, otherwise false
 */
static bool MEMDRV_isEvenDataLeft(void)
{
    return (RBUF_getBytesToRead(&MEMDRV_rb) >= MEMDRV_driver->writeBlockSize);
}



/** Waits until on going operation has been finished
 *  \note This interface triggers the watchdog as
 *        e.g. flash operation may take some time.
 */
static void MEMDRV_waitForOperationComplete(void)
{
    while(MEMDRV_driver->isBusy())
    {
        HWDT_vTrigger();
    }
}


/** disable all interrupts if needed by selected memory driver */
static void MEMDRV_disableInt(void)
{
    if(!MEMDRV_driver->allowInterrupts)
    {
        HSUP_vDisableInt();
    }
}

/** enables all interrupts if needed by selected memory driver */
static void MEMDRV_enableInt(void)
{
    if(!MEMDRV_driver->allowInterrupts)
    {
        HSUP_vEnableInt();
    }
}

#endif // FWU_MEM_DRV_READ_ONLY

/** \brief    Search for a valid driver for given driverID.
 *
 *  \param    driverID: identifier of requested memory driver
 *
 *  \return   pointer to the responsible memory driver,
              or NULL when no driver found.
 */
static const struct MEMDRV_driver_s* MEMDRV_getDriver(enum MAL_DriverID_e driverId)
{   // search in MEMDRV_driverTable until end mark (NULL) is reached
    for(uint8_t i = 0; MEMDRV_driverTable[i].driver != NULL; i++)
    {
        if (MEMDRV_driverTable[i].driverID == driverId)
        {
            return MEMDRV_driverTable[i].driver;
        }
    }
    return NULL; // no driver found for specified memory
}



/** \brief checks common error conditions
 *  \param addr physical address given by public interface
 *  \param size size in bytes given by public interface
 *  \return MAL_ST_OK if there is no problem
 */
static enum MAL_Status MEMDRV_entryCheck(uint32_t addr, uint32_t size)
{
    enum MAL_Status status = MEMDRV_lastError;

    // reset last error after once reported
    MEMDRV_lastError = MAL_ST_OK;

    if (MEMDRV_driver == NULL)
    {
        return MAL_ST_SETUP_MODULE_ERROR;
    }
    if(!MEMDRV_driver->isAddrRangeValid(addr,size))
    {
        return MAL_ST_OUT_OF_RANGE_ERROR;
    }

    return status;
}


/** \brief Calculates the CRC for a block inside according memory.
 *  \details The memory driver decides how much bytes will be erased.
 *  \param addr[in,out] in: start addr to calculate CRC, out: next addr
 *  \param size[in,out] in: number of bytes to calculate, out: number of bytes left
 *  \return result of operation [MAL_ST_READ_ERROR, MAL_ST_OK]
 */
static enum MAL_Status MEMDRV_calcCrcAtomic(uint32_t *addr, uint32_t *size, CRC_crc_t *crc)
{
    uint8_t readBuffer[MEMDRV_READ_BUFFER_SIZE];
    uint32_t readSize = MEMDRV_min(MEMDRV_READ_BUFFER_SIZE, *size);
    if(!MEMDRV_readBlock(*addr, readSize, readBuffer))
    {
          return MAL_ST_READ_ERROR;
    }
    *crc   = CRC_Update(*crc, readSize, readBuffer);
    *size -= readSize;
    *addr += readSize;
    return MAL_ST_OK;
}


/** \brief reads a number of bytes
 *  \param addr physical address
 *  \param size size in bytes to be read
 *  \param data pointer to data buffer to copy the read data
 *  \return true if reading successfully, false otherwise
 */
static bool MEMDRV_readBlock(uint32_t addr, uint32_t size, uint8_t *data)
{
    if(MEMDRV_isInternalMemory())
    {   // just copy the data
        return COMA_read(addr,size,data);
    }
    else
    {   // read from external memory
        return MEMDRV_driver->read(addr,size,data);
    }
}


/** returns true if open memory driver */
static bool MEMDRV_isInternalMemory(void)
{
    return (MEMDRV_driver->read == NULL);
}


/** returns the minimum of the parameters */
static uint32_t MEMDRV_min(uint32_t val1, uint32_t val2)
{
    return ((val1>val2) ? val2 : val1);
}
