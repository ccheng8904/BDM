/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      EXTEEP
 ******************************************************************************/
#ifndef __EXTEEPLIB_H
#define __EXTEEPLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/  
/** \file ExtEEPlib.h
    Interface of external EEPROM library .
*/
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

/** pointer to initialisation function type */
typedef BOOL (*type_InitFunction)(void);
/** pointer to read function type */
typedef BOOL (*type_ReadFunction)(uint uiAddress, uchar *pucData, uint uiLength, uchar ucChipAddress);
/** pointer to write function type */
typedef BOOL (*type_WriteFunction)(uint uiAddress      ,
                                   const uchar *pucData,
                                   uint uiLength       ,
                                   uchar ucChipAddress  );
/** pointer to readiness test function type */
typedef BOOL (*type_IsReady)(uchar ucChipAddress);

/** type to describe parameters for read/write process from EEPROM*/
typedef struct struct_Task
{
   uint  uiStartAddress;  ///< start address for reading/writing
   uint  uiBytesCount  ;  ///< count of bytes for reading/writing
   uchar *pucData      ;  ///< pointer to memory either where come data for writing from or where are read out data saved
}TaskType;

/** communicator type */
typedef struct struct_Communicator
{
   const type_InitFunction      pfInit     ; ///< pointer to initialisation function
   const type_ReadFunction      pfRead     ; ///< pointer to read function
   const type_WriteFunction     pfWrite    ; ///< pointer to write function
   const type_IsReady           pfIsReady  ; ///< pointer to function for readiness test
}type_Communicator;

/** type to describe properties of memory chip */
typedef struct struct_chipInfo
{
   const uchar             ucNumber       ; ///< agreed number of memory chip project dependent
   const uchar             ucBaseAddress  ; ///< base chip address(for I2C) or chip ID (for SPI)
   const uint              uiMaxAddress   ; ///< real top address for data on this chip
   const uchar             ucPageSize     ; ///< size of page in bytes for this chip
   const type_Communicator *ptCommunicator; ///< pointer to communicator variable (with function pointers)
   BOOL                    bIsChipCorrupt ; ///< holds functionality status for this chip
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

/*Variables*/
/** external EEPROM chip count used in the project */
extern const uchar EXTEEP_ucChipCount;
/** Field with parameters of all used EEPROM chips */
extern chipInfoType EXTEEP_tChipInfo[];
/** Field with all communicators used in the project */
extern const type_Communicator EXTEEP_atCommunicators[];
/** Number of communicators used in the project */
extern const uchar EXTEEP_ucCommunicatorsCount;
/** gets top usable address for write/read */
extern const ulong EXTEEP_ulMaxAddress;
/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
/* ---------------------------------- */

/** Accesses external EEPROM for read
\param   ulAddress - start address for read
\param   pucData - byte buffer for read data
\param   uiLength  - number of bytes to read
\return  
   #TRUE    - operation successful \n
   #FALSE   - operation failed
\remarks 
   - ulAddress  : Address value must be within valid addresses interval.
   - pucData != NULL  : Read buffer must be valid.
   - uiLength  > 0    : At least one data must be read.
   - This function only prepares data for read.\n Then is to be call the function EXTEEP_ucHandleTask()
so many times until the function EXTEEP_tGetStatus() returns EEP_READY (1).
*/
bool EXTEEP_bRead(uint32_t ulAddress,const uint8_t *pucData, uint16_t uiLength);

/** Accesses external EEPROM for writing
\param   ulAddress - start address for writing
\param   pucData - byte buffer containing data to write, read only
\param   uiLength  - number of bytes to write
\return  
   #TRUE    - operation successful \n
   #FALSE   - operation failed
\remarks 
   This function only prepares data for writing. Then is to be call the function EXTEEP_ucHandleTask()
   so many times \n until the function EXTEEP_tGetStatus() returns EEP_READY (1).
   - ulAddress  : Address value must be within valid addresses interval.
   - pucData != NULL  : Write buffer must be valid.
   - uiLength  > 0    : At least one data must be written. \n
*/
bool EXTEEP_bWrite(uint32_t ulAddress, const uint8_t *pucData, uint16_t uiLength);

/** Accesses external EEPROM for writing a single data byte
\param   ulAddress - start address for writing
\param   ucData - byte data to write
\return  
   #TRUE    - operation successful \n
   #FALSE   - operation failed
\remarks 
   This function only prepares data for writing. Then is to be call the function EXTEEP_ucHandleTask()
   so many times \n until the function EXTEEP_tGetStatus() returns EEP_READY (1).
   - ulAddress  : Address value must be within valid addresses interval.
*/
bool EXTEEP_bWriteByte(uint32_t ulAddress, uint8_t ucData);

/** Returns status of external EEPROM library
\param   void
\return  
   - #EEP_UNINITIALIZED
   - #EEP_READY
   - #EEP_READING
   - #EEP_WRITING
*/
eTaskStateType EXTEEP_tGetStatus(void);

/** Handles read/write tasks
\param   void
\return  
       #TASK_NOT_INITIALISED \n
       #TASK_INITIALISED \n
       #TASK_TERMINATE \n
*/
uchar EXTEEP_ucHandleTask(void);
#ifdef GSW_USE_SINGLE_MEMORY_CHIP
#else
/** Set status of memory write process verification
\param   bStatus - boolean value TRUE/FALSE
\return  none
\remarks 
   If this function is called with \n
   - TRUE  - next calling of EXTEEP_bWrite() or EXTEEP_bWriteByte() will read every written data and compare each other \n
   - FALSE - next calling of EXTEEP_bWrite() or EXTEEP_bWriteByte() only writes data. \n
*/
void EXTEEP_vVerify(BOOL bStatus);
#endif

#ifdef __cplusplus
}
#endif

#endif
