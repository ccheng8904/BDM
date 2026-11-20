/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCU framework
 ******************************************************************************/

#ifndef __ExtEEPlibMc_H
#define __ExtEEPlibMc_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief      External EEPROM library for STM32F4XX derivative - excess header file.
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "ExtEEPlib.h"

/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/
/** pointer to read function type */
typedef bool (*T_Read)(void *ptUnit, uint32_t u32Address, const uint8_t *pu8Data, uint16_t u16Length);
/** pointer to write function type */
typedef bool (*T_Write)(void *ptUnit, uint32_t u32Address, const uint8_t *pu8Data, uint16_t u16Length);
/** pointer to write byte function type */
typedef bool (*T_WriteByte)(void *ptUnit, uint32_t u32Address, uint8_t u8Data);
/** pointer to get status function type */
typedef eTaskStateType (*T_GetStatus)(void *ptUnit);
/** pointer to task handler function type */
typedef uint8_t (*T_HandleTask)(void *ptUnit);
/** pointer to function that tests if command impulses were sent */
typedef bool (*T_CommandSent)(void);
/** pointer to function that informs data were written */
typedef void (*T_DataWritten)(void);
/** Type to describe possible library cases */
typedef enum enum_InternalState
{
   STEP_NOT_INITIALIZED = 0,     ///< not initialized
   STEP_READY              ,     ///< basic status, ready to accept read or write commands
   STEP_FIRST_BYTES_WRITTEN,     ///< first bytes were written
   STEP_FULL_PAGE_WRITTEN  ,     ///< some of page bytes were written
   STEP_LAST_BYTES_WRITTEN ,     ///< last bytes werer written
   STEP_READ_STARTED             ///< read impulses were generated
}eInternalStateType;

typedef struct _T_I2C_Unit
{
   const T_Read                pfRead             ; ///< pointer to read function
   const T_Write               pfWrite            ; ///< pointer to write function
   const T_WriteByte           pfWriteByte        ; ///< pointer to write byte function
   const T_GetStatus           pfGetStatus        ; ///< pointer to get status function
   const T_HandleTask          pfHandleTask       ; ///< pointer to task handler function
   const T_CommandSent         pfCommandSent      ; ///< pointer to command sent function
   const T_DataWritten         pfDataWritten      ; ///< pointer to data written function
         uint8_t               ucFirstBytesCnt    ;
         uint8_t               ucLastBytesCnt     ;
         eInternalStateType    tInternalState     ;
         eTaskStateType        tTaskState         ;
         uint16_t              uiPageCounter      ;
         uint16_t              uiNumberOfPages    ;
         uint16_t              uiStatusTestCounter;
         chipInfoType         *tChipInfo          ;
}T_I2C_Unit;

/******************************************************************************/
/* VARIABLES                                                                  */
/******************************************************************************/

/******************************************************************************/
/* FUNCTIONS                                                                   */
/******************************************************************************/
bool EXTEEP_bReadPrivate(void *ptUnit, uint32_t u32Address, const uint8_t *pu8Data, uint16_t u16Length) ;
bool EXTEEP_bWritePrivate(void *ptUnit, uint32_t u32Address, const uint8_t *pu8Data, uint16_t u16Length);
bool EXTEEP_bWriteBytePrivate(void *ptUnit, uint32_t u32Address, uint8_t u8Data)                        ;
eTaskStateType EXTEEP_tGetStatusPrivate(void *ptUnit)                                                   ;
uint8_t EXTEEP_ucHandleTaskPrivate(void *ptUnit)                                                        ;

#ifdef __cplusplus
}
#endif

#endif
