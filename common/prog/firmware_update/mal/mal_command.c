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
 *  \brief    Implementation of mal_command.
 *
 *  \details  This unit implements the mal commands provided for MAP.
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "mal_command.h"
#include "firmware_update/mal/mal_statuses.h"
#include "firmware_update/mal/moduleAdministration.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include <string.h>
/*lint -e934 "Taking address of near auto variable [MISRA 2012 Rule 1.3, required]", relevant for DLLs only. Inhibit msg for the whole module*/
/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static void MAL_copyDevInfo(uint8_t index, uint16_t *const modNum, uint32_t *const size, uint8_t *const name);
static void MAL_processNextIndex(uint8_t *const index);
static const struct MAL_partition_s * MAL_getPartition(uint8_t index);
static uint8_t MAL_getPartitionNameLen(const struct MAL_partition_s *partition);
static enum MAL_Status MAL_checkReadOperation(uint16_t modNum);
static enum MAL_Status MAL_checkWriteOperation(uint16_t modNum);
static enum MAL_Status MAL_checkAddressRange(uint32_t address, uint32_t size);
static enum MAL_Status MAL_eraseModule(void);
static enum MAL_Status MAL_handleWriteBlock(uint16_t dataLen, const uint8_t *const data);
static enum MAL_Status MAL_finishWriting(void);
static bool MAL_isReadingAllowed(void);
static bool MAL_isWriteEraseAllowed(void);
static bool MAL_isModuleOpened(uint16_t modNum);
static uint32_t MAL_getPhyAddr(uint32_t offset);
static bool MAL_IsOk(enum MAL_Status result);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/** partition of currently oppened module */
static const struct MAL_partition_s *MAL_OpenPartition;

/** id of currntly oppened module */
static enum MAL_ModuleEnum  MAL_OpenModule = MAL_NO_MODULE;

static uint32_t  MAL_currentAddr;  ///< next physical address to write
static uint32_t  MAL_sizeLeft;     ///< number of bytes left to finish writing
static uint32_t  MAL_dataLen;      ///< overall data length of ongoing write job
static CRC_crc_t MAL_crc;          ///< hold crc, calculated on the fly
/** flag indicates a correct start of write process */
static bool      MAL_writeStarted = false;
static enum MAL_Status MAL_result;
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void MAL_CMD_init(void)
{
    MAL_initPartition();
}



uint8_t MAL_CMD_DevInfo(uint8_t index,
                        uint8_t  *const newIndex, uint16_t *const modNum,
                        uint32_t *const size,     uint8_t  *const nameLen,
                        uint8_t  maxNameLen,      uint8_t *const name)
{
    MAL_result = MAL_ST_ERROR;
    *newIndex  = index;
    const struct MAL_partition_s *partition = MAL_GetMemModuleByIndex(index);
    if (partition != NULL)
    {
        *nameLen=MAL_getPartitionNameLen(partition);
        if (*nameLen <= maxNameLen)
        {   
            MAL_copyDevInfo(index, modNum, size, name);
            MAL_processNextIndex(newIndex);
            MAL_result = MAL_ST_OK;
        }
        else
        {   /* not enough space in buffer, no device info copied */
            *modNum   = 0u;
            *size     = 0u;
            *nameLen  = 0u;
        }
    }
    return (uint8_t)MAL_result;
}


uint8_t MAL_CMD_Open(uint16_t modNum, uint8_t *const timeOutPwr2)
{
    MAL_result = MAL_ST_SETUP_MODULE_ERROR;
    /*lint -e{9030,9034} [MISRA 2012 Rule 10.5, advisory], [MISRA 2012 Rule 10.3, required]
                          validated by MAL_GetMemModule() */
    MAL_OpenModule    = (enum MAL_ModuleEnum)modNum;
    MAL_OpenPartition = MAL_GetMemModule(MAL_OpenModule);
    if(MAL_OpenPartition!=NULL)
    {
        MAL_result = MEMDRV_open(MAL_OpenPartition->driver_id);
    }
    if(MAL_result == MAL_ST_OK)
    {
        *timeOutPwr2 = (uint8_t)MEMDRV_getTimeout();
    }
    else
    {
        MAL_OpenModule = MAL_NO_MODULE;
        MAL_OpenPartition = NULL;
    }
    return (uint8_t)MAL_result;
}


uint8_t MAL_CMD_Close(uint16_t modNum)
{
    (void)modNum;
    MAL_OpenModule = MAL_NO_MODULE;
    MAL_OpenPartition = NULL;
    return (uint8_t)MEMDRV_close();
}


uint8_t MAL_CMD_Read(uint16_t modNum, uint32_t offset, uint16_t dataLen, uint8_t *const data)
{
    if(MAL_IsOk(MAL_checkReadOperation(modNum)))
    {
        MAL_result = MEMDRV_read(MAL_getPhyAddr(offset), dataLen, data);
    }
    return (uint8_t)MAL_result;
}


uint8_t MAL_CMD_WriteStart(uint16_t modNum, uint32_t overallDataLen, uint16_t dataLen, const uint8_t *const data)
{
    if( MAL_IsOk(MAL_checkWriteOperation(modNum)) &&
        MAL_IsOk(MAL_checkAddressRange(0u, overallDataLen)) &&
        MAL_IsOk(MAL_eraseModule()))
    {   // all checks passed
        MAL_crc = CRC_Init();
        MAL_writeStarted = true;
        MAL_currentAddr = 0u;
        MAL_sizeLeft    = overallDataLen;
        MAL_dataLen     = overallDataLen;
        MAL_result = MAL_handleWriteBlock(dataLen, data); // write data
        if((MAL_sizeLeft == 0u) && (MAL_result == MAL_ST_OK))
        {
            MAL_result = MAL_finishWriting();
        }
    }
    return (uint8_t)MAL_result;
}


uint8_t MAL_CMD_WriteAppend(uint16_t modNum, uint16_t dataLen, const uint8_t *const data)
{
    if(!MAL_writeStarted) {return (uint8_t)MAL_ST_WRITE_ERROR;} // missing start
    if( MAL_IsOk(MAL_checkWriteOperation(modNum)) &&
        MAL_IsOk(MAL_handleWriteBlock(dataLen, data))) // write data
    {   // writing successfully
        if(MAL_sizeLeft == 0u)
        {   // all data written
            MAL_result = MAL_finishWriting(); // flush and check crc
        }
    }
    return (uint8_t)MAL_result;
}


uint8_t MAL_CMD_WritePrematurelyEnd(uint16_t modNum, uint32_t overallDataWritten)
{
    if(!MAL_writeStarted) {return (uint8_t)MAL_ST_WRITE_ERROR;} // missing start
    if(MAL_IsOk(MAL_checkWriteOperation(modNum)))
    {
        if((uint32_t)(MAL_dataLen - MAL_sizeLeft) != overallDataWritten)
        {   // wrong data length
            MAL_result = MAL_ST_WRITE_DATA_LEN_MISMATCH;
        }
        else
        {
            MAL_dataLen = overallDataWritten;  // new overall length
            MAL_result  = MAL_finishWriting(); // flush and check crc
        }
    }
    return (uint8_t)MAL_result;
}



/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** collect and copy the device info of partition given by index
 *  \param[in]  index:  index of requested partition
 *  \param[out] modNum: number of module inside requested partition
 *  \param[out] size:   size of partition
 *  \param[out] name:   name of module inside requested partition
 */
static void MAL_copyDevInfo(uint8_t index, uint16_t *const modNum,
                            uint32_t *const size, uint8_t *const name)
{
    const struct MAL_partition_s *partition = MAL_getPartition(index);
    uint8_t nameLen  = MAL_getPartitionNameLen(partition);
    
    *modNum = (uint16_t)partition->mod_number;
    *size   = MAL_getPartitionSize(partition);
    memcpy(name, partition->mod_name, nameLen);
}


/** Processes the next partition index
 *  \param[in,out] index
 */
static void MAL_processNextIndex(uint8_t *const index)
{
    (*index)++;
    if (*index >= MAL_GetMemModuleCount()) {*index = 0u;}
}


/** delivers a pointer to partition entry for a given index
 *  \param  index: partition index
 *  \return pointer partition entry, NULL for invalid index
 */
static const struct MAL_partition_s * MAL_getPartition(uint8_t index)
{
    MAL_initPartition();
    return MAL_GetMemModuleByIndex(index);
}


/** computes the length of module name for a given partition
 *  \param[in] partition: pointer partition entry
 *  \return  length of the partition name
 */
static uint8_t MAL_getPartitionNameLen(const struct MAL_partition_s *partition)
{
    return (uint8_t)strlen((const char*)partition->mod_name);//lint !e926 cast from pointer to pointer
}


/** Checks if reading is allowed for a given module
 *  \param modNum: module number
 *  \return MAL_ST_OK if check is OK
 */
static enum MAL_Status MAL_checkReadOperation(uint16_t modNum)
{
    if(!MAL_isModuleOpened(modNum))  {return MAL_ST_SETUP_MODULE_ERROR;     }
    if(!MAL_isReadingAllowed())      {return MAL_ST_ACTION_PROHIBITED_ERROR;}
    return MAL_ST_OK;
}


/** Checks if writing is allowed for a given module
 *  \param modNum: module number
 *  \return MAL_ST_OK if check is OK
 */
static enum MAL_Status MAL_checkWriteOperation(uint16_t modNum)
{
    if(!MAL_isModuleOpened(modNum))  {return MAL_ST_SETUP_MODULE_ERROR;     }
    if(!MAL_isWriteEraseAllowed())   {return MAL_ST_ACTION_PROHIBITED_ERROR;}
    return MAL_ST_OK;
}


/** \brief checks if the address range is valid.
 *  \details For security reasons it is important to check the 
 *           The CRC is compared with the ongoing calculated CRC of
 *           incoming data.
 *  \return error status: MAL_ST_OK in case of success or MAL_ST_OUT_OF_RANGE_ERROR
 */
static enum MAL_Status MAL_checkAddressRange(uint32_t address, uint32_t size)
{
    address += size;
    if((address >= size) &&  // check for overflow
       (MAL_getPartitionSize(MAL_OpenPartition) >= address)) // check range
    {
        return MAL_ST_OK;
    }
    return MAL_ST_OUT_OF_RANGE_ERROR;
}


/** Erases the complete partition of a given module
 *  \return MAL_ST_OK if successful
 */
static enum MAL_Status MAL_eraseModule(void)
{
    uint32_t addr = MAL_getPhyAddr(0u);
    uint32_t size = MAL_getPartitionSize(MAL_OpenPartition);

    return MEMDRV_eraseRange(addr, size);
}


/** check if reading is allowed for opened module */
static bool MAL_isReadingAllowed(void)
{
    return ((MAL_OpenPartition->mod_attributes & MAL_CTX_MF_READ_BLOCK) != 0u);
}


/** check if wriet/erase is allowed for opened module */
static bool MAL_isWriteEraseAllowed(void)
{
    return ((MAL_OpenPartition->mod_attributes & MAL_CTX_MF_WE) == MAL_CTX_MF_WE);
}


/** check if a given module is correctly opened */
static bool MAL_isModuleOpened(uint16_t modNum)
{
    return ((uint16_t)MAL_OpenModule == modNum);
}


/** computes the physical address of a given virtual address for the opened module
 *  \param offset logical/virtual address
 *  \return physical address
 */
static uint32_t MAL_getPhyAddr(uint32_t offset)
{
    return (uint32_t)(MAL_OpenPartition->physical_start.addr + offset);
}


/** \brief Handles a received data block and writes it to memory.
 *  \details The interface also checks the plausibility of data length and
 *           updates CRC as well as the addresses and size left.
 *  \param dataLen: length of received data block
 *  \param data:    pointer to received data
 *  \return error status: MAL_ST_OK in case of success
 */
static enum MAL_Status MAL_handleWriteBlock(uint16_t dataLen, const uint8_t *const data)
{
    if(MAL_sizeLeft < dataLen){return MAL_ST_WRITE_DATA_LEN_MISMATCH;}
    if(MAL_IsOk(MAL_checkAddressRange(MAL_currentAddr, dataLen)))
    {
        MAL_crc = CRC_Update(MAL_crc, dataLen, data);
        MAL_result = MEMDRV_write(MAL_getPhyAddr(MAL_currentAddr), dataLen, data);
        MAL_currentAddr += dataLen;
        MAL_sizeLeft    -= dataLen;
    }
    return MAL_result;
}


/** \brief finishes a started write operation.
 *  \details data buffer is flushed and CRC of written data is calculated
 *           The CRC is compared with the ongoing calculated CRC of
 *           incoming data.
 *  \return error status: MAL_ST_OK in case of success
 */
static enum MAL_Status MAL_finishWriting(void)
{
    CRC_crc_t checkCrc;

    MAL_crc = CRC_Finish(MAL_crc);
    MAL_writeStarted = false;
    /*lint -e{934} "Taking address of near auto variable [MISRA 2012 Rule 1.3, required]", relevant for DLLs only */
    if (MAL_IsOk(MEMDRV_flush())  &&
        MAL_IsOk(MEMDRV_calcCrc(MAL_getPhyAddr(0),MAL_dataLen, &checkCrc)))
    {
        if (checkCrc != MAL_crc)
        {
            MAL_result = MAL_ST_WRITE_ERROR;
        }
    }
    return MAL_result;
}


/** \brief sets result and checks if it is OK
 *  \param result: The result of an operation
 *  \return true if result is MAL_ST_OK, otherwise false
 */
static bool MAL_IsOk(enum MAL_Status result)
{
    MAL_result = result;
    return result == MAL_ST_OK;
}
