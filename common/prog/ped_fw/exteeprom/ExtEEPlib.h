/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          EXTEEPROM
 ******************************************************************************/

#ifndef __EXTEEPLIB_H
#define __EXTEEPLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief      Interface of external EEPROM library .
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/

/* function type*/
/** pointer to initialisation function type */
typedef bool (*type_InitFunction)(void);
/** pointer to read function type */
typedef bool (*type_ReadFunction)(uint16_t uiAddress, uint8_t *pucData, uint16_t uiLength, uint8_t ucChipAddress);
/** pointer to write function type */
typedef bool (*type_WriteFunction)(uint16_t uiAddress      ,
                                   const uint8_t *pucData,
                                   uint16_t uiLength       ,
                                   uint8_t ucChipAddress  );
/** pointer to readiness test function type */
typedef bool (*type_IsReady)(uint8_t ucChipAddress);
/* TaskType*/
/** type to describe parameters for read/write process from EEPROM*/
typedef struct struct_Task
{
   uint16_t  uiStartAddress;  ///< start address for reading/writing
   uint16_t  uiBytesCount  ;  ///< count of bytes for reading/writing
   uint8_t *pucData      ;  ///< pointer to memory either where come data for writing from or where are read out data saved
}TaskType;
/* type_Communicator*/
/** communicator type */
typedef struct struct_Communicator
{
   const type_InitFunction      pfInit     ; ///< pointer to initialisation function
   const type_ReadFunction      pfRead     ; ///< pointer to read function
   const type_WriteFunction     pfWrite    ; ///< pointer to write function
   const type_IsReady           pfIsReady  ; ///< pointer to function for readiness test
}type_Communicator;
/* chipInfoType*/
/** type to describe properties of memory chip */
typedef struct struct_chipInfo
{
   const uint8_t             ucNumber       ; ///< agreed number of memory chip project dependent
   const uint8_t             ucBaseAddress  ; ///< base chip address(for I2C) or chip ID (for SPI)
   const uint16_t              uiMaxAddress   ; ///< real top address for data on this chip
   const uint8_t             ucPageSize     ; ///< size of page in bytes for this chip
   const type_Communicator *ptCommunicator; ///< pointer to communicator variable (with function pointers)
   bool                    bIsChipCorrupt ; ///< holds functionality status for this chip
   TaskType                tTaskParam     ; ///< parameters for read/write task
}chipInfoType;

/** Type to describe possible library cases */
typedef enum enum_TaskState
{
   EEP_UNINITIALIZED, ///< not initialized, the funktion EXTEEP_bHandleTask() is to be call
   EEP_READY        , ///< basic status, ready to accept read or write commands
   EEP_READING      , ///< read operation is pending
   EEP_WRITING        ///< write operation is pending
}eTaskStateType;

/******************************************************************************/
/* VARIABLES                                                                  */
/******************************************************************************/
/** external EEPROM chip count used in the project */
extern const uint8_t EXTEEP_ucChipCount;
/** Field with parameters of all used EEPROM chips */
extern chipInfoType EXTEEP_tChipInfo[];
/** Field with all communicators used in the project */
extern const type_Communicator EXTEEP_atCommunicators[];
/** Number of communicators used in the project */
extern const uint8_t EXTEEP_ucCommunicatorsCount;
/** gets top usable address for write/read */
extern const uint32_t EXTEEP_ulMaxAddress;

/* Basic Interfaces*/
/* ------ Test instrumentation ------ */
#if defined (MODULE_TEST)
#include "ExtEEPlib_Stub.h"
#endif
/* ---------------------------------- */

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
/* EXTEEP_bRead()*/
/** Accesses external EEPROM for read

\param   ulAddress - start address for read
\param   pucData - byte buffer for read data
\param   uiLength  - number of bytes to read
\return  
   #true    - operation successful \n
   #false   - operation failed
\remarks 
   - ulAddress  : Address value must be within valid addresses interval.
   - pucData != NULL  : Read buffer must be valid.
   - uiLength  > 0    : At least one data must be read.
   - This function only prepares data for read.\n Then is to be call the function EXTEEP_ucHandleTask()
so many times until the function EXTEEP_tGetStatus() returns EEP_READY (1).
*/
bool EXTEEP_bRead(uint32_t ulAddress, uint8_t * const pucData, uint16_t uiLength);

/* EXTEEP_bWrite()*/
/** Accesses external EEPROM for writing

\param   ulAddress - start address for writing
\param   pucData - byte buffer containing data to write, read only
\param   uiLength  - number of bytes to write
\return  
   #true    - operation successful \n
   #false   - operation failed
\remarks 
   This function only prepares data for writing. Then is to be call the function EXTEEP_ucHandleTask()
   so many times \n until the function EXTEEP_tGetStatus() returns EEP_READY (1).
   - ulAddress  : Address value must be within valid addresses interval.
   - pucData != NULL  : Write buffer must be valid.
   - uiLength  > 0    : At least one data must be written. \n
*/
bool EXTEEP_bWrite(uint32_t ulAddress, const uint8_t * const pucData, uint16_t uiLength);

/* EXTEEP_bWriteByte()*/
/** Accesses external EEPROM for writing a single data byte

\param   ulAddress - start address for writing
\param   ucData - byte data to write
\return  
   #true    - operation successful \n
   #false   - operation failed
\remarks 
   This function only prepares data for writing. Then is to be call the function EXTEEP_ucHandleTask()
   so many times \n until the function EXTEEP_tGetStatus() returns EEP_READY (1).
   - ulAddress  : Address value must be within valid addresses interval.
*/
bool EXTEEP_bWriteByte(uint32_t ulAddress, uint8_t ucData);
/* EXTEEP_tGetStatus()*/
/** Returns status of external EEPROM library

\param   void

\return  
   - #EEP_UNINITIALIZED
   - #EEP_READY
   - #EEP_READING
   - #EEP_WRITING
*/
eTaskStateType EXTEEP_tGetStatus(void);
/* EXTEEP_ucHandleTask()*/
/** Handles read/write tasks

\param   void
\return  
       #TASK_NOT_INITIALISED \n
       #TASK_INITIALISED \n
       #TASK_TERMINATE \n
*/
uint8_t EXTEEP_ucHandleTask(void);
#ifdef GSW_USE_SINGLE_MEMORY_CHIP

#else
/* EXTEEP_vVerify()*/
/** Set status of memory write process verification

\param   bStatus - boolean value true/false
\return  none
\remarks 
   If this function is called with \n
   - true  - next calling of EXTEEP_bWrite() or EXTEEP_bWriteByte() will read every written data and compare each other \n
   - false - next calling of EXTEEP_bWrite() or EXTEEP_bWriteByte() only writes data. \n
*/
void EXTEEP_vVerify(bool bStatus);
#endif

#ifdef __cplusplus
}
#endif

#endif
