/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          ASSERT_LIB
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file AssertXS.c
 *  Application specific implementation / configuration for Assert library   
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"
#include "AssertXS.h"
#include "pedAssert.h"

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/
#ifdef ASSERT_INCLUDED 
/* Variables Definition*/
uint8_t  ASSERT_ucLogFileName[ASSERT_LOG_NUM][ASSERT_MAXLEN_FILENAME];
uint16_t ASSERT_u16LogLineNo[ASSERT_LOG_NUM];

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/* void _ASSERT_vOnFailure (const uint8_t *pucFileName, uint16 u16LineNo)*/
void _ASSERT_vOnFailure (const uint8_t *pucFileName, uint16_t u16LineNo)
{
   static uint8_t  ASSERT_ucEntryNum = 0;
          uint8_t  ucSizeLeft = ASSERT_MAXLEN_FILENAME;
          uint8_t  ucNameLen  = 0;

   /* Entries number must be less than the given threshold */
   ASSERT_ucEntryNum %= ASSERT_LOG_NUM;
   /* copy line number */
   ASSERT_u16LogLineNo [ASSERT_ucEntryNum] = u16LineNo;
   /* go to the end of file name character buffer, as we want to copy
      from the last character to get more precision. 
      Note : Due to the same module prefix at the beginning of all files, it will
             not be helpfull to start from the beginning of the file name, as 
             this will lead to confusion in case the name is cut after the prefix.*/
   while (*pucFileName != 0u)
   {
      pucFileName ++;
      /* To avoid buffer overflows, we can only 
         consider <available size in buffer> - 1 */
      if (ucNameLen == (ucSizeLeft - 1u))
      {
         continue;
      }
      ucNameLen ++;
   }
   while (ucSizeLeft != 0u)
   {
      ucSizeLeft--;
      while (ucNameLen != 0u)
      {
          --pucFileName;
          ASSERT_ucLogFileName [ASSERT_ucEntryNum][ucSizeLeft] = *(pucFileName);
         ucNameLen --;
         ucSizeLeft --;
      }
      /* Reset untouched entries for more clarity in order to avoid confusing file name */
      ASSERT_ucLogFileName [ASSERT_ucEntryNum][ucSizeLeft] = 32;
   }
   ASSERT_ucEntryNum ++;
}
#endif
