/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          FWU
*   COMP_ABBREV      MAP
*******************************************************************************/

#ifndef MAP_MEM_ACCESS_INTERFACE_H_
#define MAP_MEM_ACCESS_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>

/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/**
*   \brief Gets information about one module for device info.
*   \param[in] index Current offset in area, where memory modules are defined
*   \param[out] newIndex New offset in area, where memory modules are defined
*   \param[out] modNum Number of module
*   \param[out] size Size of module
*   \param[out] nameLen Length of name of module
*   \param[in] maxNameLen Maximum supported length of name of module
*   \param[out] name Name of module
*   \return status
*   \retval Zero means that everything went well; Any other specified value indicates errors.
*/
typedef uint8_t (*DevInfo)(uint8_t index, uint8_t *const newIndex, uint16_t *const modNum, uint32_t *const size, uint8_t *const nameLen, uint8_t maxNameLen, uint8_t *const name);

/**
*   \brief Opens a memory module
*   \param[in] modNum Number of module
*   \param[out] maxTimeoutExponentOfPowerOfTwoOfSeconds Exponent of expression 2^x representing max timeout in seconds
*   \return status
*   \retval Zero means that everything went well; Any other specified value indicates errors.
*/
typedef uint8_t (*Open)(uint16_t modNum, uint8_t *const maxTimeoutExponentOfPowerOfTwoOfSeconds);

/**
*   \brief Closes a memory module.
*   \param[in] modNum Number of module
*   \return status
*   \retval Zero means that everything went well; Any other specified value indicates errors.
*/
typedef uint8_t (*Close)(uint16_t modNum);

/**
*   \brief Reads from a memory module.
*   \param[in] modNum Number of module
*   \param[in] offset Offset within module
*   \param[in] dataLen Length of data to be read
*   \param[out] data Data that has been read
*   \return status
*   \retval Zero means that everything went well; Any other specified value indicates errors.
*/
typedef uint8_t (*Read)(uint16_t modNum, uint32_t offset, uint16_t dataLen, uint8_t *const data);

/**
*   \brief Start a write operation in a module.
*   \param[in] modNum Number of module
*   \param[in] overallDataLen Overall length of all data, that are to be written, i.e. data contained in this and succeeding write append requests
*   \param[in] dataLen Length of data to be written
*   \param[in] data Data to be written
*   \return status
*   \retval Zero means that everything went well; Any other specified value indicates errors.
*/
typedef uint8_t (*WriteStart)(uint16_t modNum, uint32_t overallDataLen, uint16_t dataLen, const uint8_t *const data);

/**
*   \brief Appends data in a write operation
*   \param[in] modNum Number of module
*   \param[in] dataLen Length of data to be written
*   \param[in] data Data to be written
*   \return status
*   \retval Zero means that everything went well; Any other specified value indicates errors.
*/
typedef uint8_t (*WriteAppend)(uint16_t modNum, uint16_t dataLen, const uint8_t *const data);

/**
*   \brief Finishes a write operation in a module after smaller amount of data, than previously declared.
*   \param[in] modNum Number of module.
*   \param[in] overallDataWritten Overall length of all data, that must have been received to write.
*   \return status
*   \retval Zero means that everything went well; Any other value from MAL_Status_t indicates errors.
*/
typedef uint8_t (*WritePrematurelyEnd)(uint16_t modNum, uint32_t overallDataWritten);


/**
*   \brief Type of function pointer interface for map_cmd to access memory drivers.
*/
struct MAP_MemAccessInterface
{
    DevInfo devInfo;
    Open open;
    Close close;
    Read read;
    WriteStart writeStart;
    WriteAppend writeAppend;
    WritePrematurelyEnd writePrematurelyEnd;
};

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**
*   \brief Interface for map_cmd to access memory drivers.
*/
extern struct MAP_MemAccessInterface MAP_MemAccess;
/**
*   \brief Initializes MAP_MemAccess.
*/
void MAP_MemAccess_Init(void);

#ifdef __cplusplus
}
#endif

#endif
