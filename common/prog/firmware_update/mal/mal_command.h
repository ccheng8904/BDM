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
*   COMP_ABBREV      MAL
*******************************************************************************/


#ifndef MAL_COMMAND_H
#define MAL_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*  \ingroup  firmware_update
 *
 *  \brief    Public definitions for unit mal_command.
 *
 *  \details  This unit implements the mal commands provided for MAP.
 */




/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include <stdint.h>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Do the mal initialization */
extern void MAL_CMD_init(void);

/** \brief Gets information about one module for device info.
 *  \param[in] index Current offset in area, where memory modules are defined
 *  \param[out] newIndex New offset in area, where memory modules are defined
 *  \param[out] modNum Number of module
 *  \param[out] size Size of module
 *  \param[out] nameLen Length of name of module
 *  \param[in] maxNameLen Maximum supported length of name of module
 *  \param[out] name Name of module
 *  \return status
 *  \retval Zero means that everything well, any other value indicates errors.
 */
extern uint8_t MAL_CMD_DevInfo(uint8_t index,
                               uint8_t  *const newIndex, uint16_t *const modNum,
                               uint32_t *const size,     uint8_t  *const nameLen,
                               uint8_t  maxNameLen,      uint8_t *const name);


/** \brief Opens a memory module
 *  \param[in] modNum Number of module
 *  \param[out] timeOutPwr2 2^timeOutPwr2 representing max timeout in seconds.
 *  \return status
 *  \retval Zero means that everything well, any other value indicates errors.
 */
extern uint8_t MAL_CMD_Open(uint16_t modNum, uint8_t *const timeOutPwr2);


/** \brief Closes a memory module.
 *  \param[in] modNum Number of module
 *  \return status
 *  \retval Zero means that everything well, any other value indicates errors.
 */
extern uint8_t MAL_CMD_Close(uint16_t modNum);


/** \brief Reads from a memory module.
 *  \param[in] modNum Number of module
 *  \param[in] offset Offset within module
 *  \param[in] dataLen Length of data to be read
 *  \param[out] data Data that has been read
 *  \return status
 *  \retval Zero means that everything well, any other value indicates errors.
 */
extern uint8_t MAL_CMD_Read(uint16_t modNum, uint32_t offset, uint16_t dataLen,
                     uint8_t *const data);


/** \brief Start a write operation in a module.
 *  \param[in] modNum Number of module
 *  \param[in] overallDataLen Overall length of all data, that are to be written,
               i.e. data contained in this and succeeding write append requests
 *  \param[in] dataLen Length of data to be written
 *  \param[in] data Data to be written
 *  \return status
 *  \retval Zero means that everything well, any other value indicates errors.
 */
extern uint8_t MAL_CMD_WriteStart(uint16_t modNum, uint32_t overallDataLen,
                           uint16_t dataLen, const uint8_t *const data);


/** \brief Appends data in a write operation
 *  \param[in] modNum Number of module
 *  \param[in] dataLen Length of data to be written
 *  \param[in] data Data to be written
 *  \return status
 *  \retval Zero means that everything well, any other value indicates errors.
 */
extern uint8_t MAL_CMD_WriteAppend(uint16_t modNum, uint16_t dataLen,
                            const uint8_t *const data);


/** \brief Finishes a write operation in a module after smaller amount of data, than previously declared.
 *  \param[in] modNum Number of module.
 *  \param[in] overallDataWritten Overall length of all data, that must have been received to write.
 *  \return status
 *  \retval Zero means that everything well, any other value indicates errors.
 */
extern uint8_t MAL_CMD_WritePrematurelyEnd(uint16_t modNum,
                                           uint32_t overallDataWritten);
#ifdef __cplusplus
}
#endif

#endif // MAL_COMMAND_H
