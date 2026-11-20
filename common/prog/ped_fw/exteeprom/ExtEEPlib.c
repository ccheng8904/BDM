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

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \brief      Common module of external EEPROM library .
 */

#ifdef __DOXYGEN__
#ifdef GSW_USE_SINGLE_MEMORY_CHIP
/**
 * \mainpage
 *
 * \section secIntor Introduction
 *
 * Main goal of this library is to have a possibility to read and write into external EEPROM chips \n
 * without concern about 
 * - how many integrated circuits are used,
 * - how are they connected in electrical circuit
 * - how are they addressed,
 * - which memory capacities do they have,
 * - which communication protocol do they use
 *
 *
 * \section secHow How to use
 *
 * To be able to use this library first you have to call the function EXTEEP_ucHandleTask() to initialize whole library. \n
 * To use library interface functions you can use possible addresses :\n
 * BOTTOM_ADDRESS = 0 \n
 * TOP_ADDRESS = CAPACITY_EEPROM - 1
 *
 *
 * Here is an example how to use write function:
 \code
 *   bool bWriteResult
 *   bool bInitResult
 *
 *   bInitResult = EXTEEP_ucHandleTask();
 *   bWriteResult = EXTEEP_bWrite(START_ADDRESS, MAI_ucWriteData, (uint16_t)(sizeof(MAI_ucWriteData)));
 *   if ( bWriteResult != false )
 *    {
 *     do
 *      {
 *       (void)EXTEEP_ucHandleTask();
 *      }
 *     while (EXTEEP_tGetStatus() != EEP_READY);
 *    }\endcode
 *
 * Here is an example how to use read function:
 * \code
 *   bool bReadResult;
 *   bool bInitResult
 *
 *   
 *   bInitResult = EXTEEP_ucHandleTask();
 *   bReadResult = EXTEEP_bRead(START_ADDRESS, MAI_ucReadData,(uint16_t)(sizeof(MAI_ucReadData)));
 *   if ( bReadResult != false )
 *    {
 *       do
 *       {
 *          (void)EXTEEP_ucHandleTask();
 *       }
 *       while (EXTEEP_tGetStatus() != EEP_READY);
 *    }
 * \endcode
 *
 * You should not to call read/write function if library status is not "EEP_READY". Result of such calling
 * will be false .
 *\n

 *\n
 *\n
 */
#else
/**
 * \mainpage
 *
 * \section secIntor Introduction
 *
 * Main goal of this library is to have a possibility to read and write into external EEPROM chips \n
 * without concern about 
 * - how many integrated circuits are used,
 * - how are they connected in electrical circuit
 * - how are they addressed,
 * - which memory capacities do they have,
 * - which communication protocol do they use
 *
 *
 * \section secHow How to use
 *
 * To be able to use this library first you have to call the function EXTEEP_ucHandleTask() to initialize whole library. \n
 * To use library interface functions you can use possible addresses :\n
 * BOTTOM_ADDRESS = 0 \n
 * TOP_ADDRESS = (CAPACITY_EEPROM_1 + CAPACITY_EEPROM_2 + CAPACITY_EEPROM_3 + ... + CAPACITY_EEPROM_n) - 1
 *
 *\image html ExtEEPROM.JPG "Memory organisation example for 4 chips"
 *
 * Here is an example how to use write function:
 \code
 *   bool bWriteResult
 *   bool bInitResult
 *
 *   bInitResult = EXTEEP_ucHandleTask();
 *   EXTEEP_vVerify(false);  true if necessary
 *   bWriteResult = EXTEEP_bWrite(START_ADDRESS, MAI_ucWriteData, (uint16_t)(sizeof(MAI_ucWriteData)));
 *   if ( bWriteResult != false )
 *    {
 *     do
 *      {
 *       (void)EXTEEP_ucHandleTask();
 *      }
 *     while (EXTEEP_tGetStatus() != EEP_READY);
 *    }\endcode
 *
 * Here is an example how to use read function:
 * \code
 *   bool bReadResult;
 *   bool bInitResult
 *
 *   
 *   bInitResult = EXTEEP_ucHandleTask();
 *   EXTEEP_vVerify(false);  true if necessary
 *   bReadResult = EXTEEP_bRead(START_ADDRESS, MAI_ucReadData,(uint16_t)(sizeof(MAI_ucReadData)));
 *   if ( bReadResult != false )
 *    {
 *       do
 *       {
 *          (void)EXTEEP_ucHandleTask();
 *       }
 *       while (EXTEEP_tGetStatus() != EEP_READY);
 *    }
 * \endcode
 *
 * You should not to call read/write function if library status is not "EEP_READY". Result of such calling
 * will be false .
 *\n

 *\n
 *\n
 */
#endif
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "ExtEEPlib.h"
#if defined (DEBUG) && (DEBUG != 0u)
#include "Assert.h"
#endif

/* Switch off: Note 923: cast from unsigned int to pointer [MISRA 2012 Rule 11.1, required], [MISRA 2012 Rule 11.4, required], [MISRA 2012 Rule 11.6, required]
 * It was checked and it works well */
/*lint -save -e923 */

/******************************************************************************/
/* TYPES                                                                      */
/******************************************************************************/
#ifndef __DOXYGEN__
typedef enum enum_WriteResult
{
   WRITE_FAILURE   ,
   WRITE_SUCCESSFUL,
   WRITE_ONGOING
} eWriteResultType;
#ifdef GSW_USE_SINGLE_MEMORY_CHIP

#else
/* type to describe return parameters of function EXTEEP_tGetChipInfo()*/
typedef struct struct_chip_params
{
   uint8_t ucIndex;         //index of field, which describes all memory chips
   uint16_t  uiAddressOnChip; //real address on given chip
} realChipParamsType;
#endif
#endif

/******************************************************************************/
/* DEFINES                                                                    */
/******************************************************************************/
#ifndef __DOXYGEN__

#ifndef GSW_USE_SINGLE_MEMORY_CHIP
#define CHIP_UNKNOWN                  0xFFU
#define ADDRESS_UNKNOWN             0xFFFFU
#define STEP_VALIDATE_FIRST_BYTES        2u
#define STEP_VALIDATE_FULL_PAGE          4u
#define VERIFY_FIELD_SIZE                8u
#else
#define STEP_WAIT_UNTIL_READY            7u
#endif

#define MAX_READ_ERROR_NUMBER           10u
#define MAX_WRITE_ERROR_NUMBER          10u
#define IS_READY_MAX_TEST_COUNT        100u
#define STEP_CALCULATE_PARAMETERS        0u
#define STEP_FIRST_BYTES_WRITE           1u
#define STEP_FULL_PAGE_WRITE             3u
#define STEP_LAST_BYTES_WRITE            5u
#define STEP_VALIDATE_LAST_BYTES         6u

/******************************************************************************/
/* STATIC FUNCTIONS                                                           */
/******************************************************************************/
static bool EXTEEP_bCalculateTaskParameters(uint32_t ulAddress, uint8_t * const pucData, uint16_t uiLength);
#ifdef GSW_USE_SINGLE_MEMORY_CHIP
static eWriteResultType EXTEEP_tWriteSinglePages(void);
static uint8_t EXTEEP_ucGetNextStep(uint16_t uiPageCounter, uint16_t uiNumberOfPages);
#else
static realChipParamsType EXTEEP_tGetChipInfo(uint32_t ulAddress);
static eWriteResultType EXTEEP_tWriteSinglePages( uint8_t ucChipInfoCounter );
static bool EXTEEP_bVerifyLastData(uint16_t uiVerifyAddress, const uint8_t *pucVerifyData, uint8_t ucVerifyBytesCount, uint8_t ucChipInfoCounter);
#endif

/******************************************************************************/
/* VARIABLES                                                                  */
/******************************************************************************/
static eTaskStateType EXTEEP_tTaskState   = EEP_UNINITIALIZED;
static uint8_t          EXTEEP_ucCurentStep = STEP_CALCULATE_PARAMETERS;
#ifdef GSW_USE_SINGLE_MEMORY_CHIP
static uint8_t EXTEEP_ucStepAfterReady = STEP_CALCULATE_PARAMETERS;
#else
/* used for verifying */
static bool  EXTEEP_bVerifyWrittenData = false;
#endif
#endif

/******************************************************************************/
/* STATIC FUNCTIONS                                                           */
/******************************************************************************/
#ifndef __DOXYGEN__
#ifdef GSW_USE_SINGLE_MEMORY_CHIP
/* static EXTEEP_bCalculateTaskParameters()*/
static bool EXTEEP_bCalculateTaskParameters(uint32_t ulAddress, uint8_t * const pucData, uint16_t uiLength)
{
   bool bRetVal = false;
   if ( uiLength > 0u )
   {
      if ( pucData != NULL )
      {
         if ((((uint16_t)ulAddress + uiLength) - 1u) <= EXTEEP_tChipInfo[0].uiMaxAddress)
         {
            if ( EXTEEP_tChipInfo[0].bIsChipCorrupt == false )
            {
               EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress = (uint16_t)ulAddress;
               EXTEEP_tChipInfo[0].tTaskParam.pucData = pucData;
               EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount = uiLength;
               bRetVal = true;
            }
            else
            {
               /* chip is not readable*/
#if defined (DEBUG) && (DEBUG != 0u)
               _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
            }
         }
         else
         {
            /* out of range*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
         }
      }
      else
      {
         /* pointer to pucData is NULL*/
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
      }
   }
   else
   {
      /* length must be greater*/
#if defined (DEBUG) && (DEBUG != 0u)
      _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
   }
   return(bRetVal);
}
/* static EXTEEP_tWriteSinglePages()*/
static eWriteResultType EXTEEP_tWriteSinglePages(void)
{
   /* local variables*/
   eWriteResultType tRetVal = WRITE_ONGOING;
   static uint8_t ucFirstBytesCnt ; //number of first bytes
   static uint8_t ucLastBytesCnt  ; //number of last bytes
   static uint16_t  uiNumberOfPages ; //page contains ucPageSize bytes
   static uint16_t  uiPageCounter   ;
   static uint16_t  uiAddress       ;
   static uint8_t *pucData        ;
   static uint8_t ucIsReadyTestNumber = 0;
   uint8_t ucRemainder;
   bool bFuctResult;
   switch (EXTEEP_ucCurentStep)
   {
      case STEP_CALCULATE_PARAMETERS:
      {
         uiPageCounter = 0;
         uiAddress  = EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress;
         pucData    = EXTEEP_tChipInfo[0].tTaskParam.pucData;
         ucRemainder = (uint8_t)(uiAddress % EXTEEP_tChipInfo[0].ucPageSize);
         if ( ucRemainder > 0u ) // is address divisible by ucPageSize?
         {
            ucFirstBytesCnt = EXTEEP_tChipInfo[0].ucPageSize - ucRemainder;
         }
         else
         {
            ucFirstBytesCnt = 0;
         }
         if ( EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount > (uint16_t)ucFirstBytesCnt )
         {
            uiNumberOfPages = (uint16_t)((EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount - (uint16_t)ucFirstBytesCnt) / EXTEEP_tChipInfo[0].ucPageSize);
            ucLastBytesCnt  = (uint8_t)((EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount - (uint16_t)ucFirstBytesCnt) % EXTEEP_tChipInfo[0].ucPageSize);
         }
         else
         {
            ucFirstBytesCnt = (uint8_t) EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount;
            uiNumberOfPages = 0;
            ucLastBytesCnt  = 0;
         }
         EXTEEP_ucCurentStep     = STEP_WAIT_UNTIL_READY ;
         if ( ucFirstBytesCnt > 0u )
         {
            EXTEEP_ucStepAfterReady = STEP_FIRST_BYTES_WRITE;
         }
         else
         {
            EXTEEP_ucStepAfterReady = EXTEEP_ucGetNextStep(uiPageCounter,uiNumberOfPages);
         }
         break;
      }
      case STEP_FIRST_BYTES_WRITE:
      {
         bFuctResult = EXTEEP_tChipInfo[0].ptCommunicator->pfWrite(
                       uiAddress,
                       pucData,
                 (uint16_t)ucFirstBytesCnt,
                       EXTEEP_tChipInfo[0].ucBaseAddress);
         if ( bFuctResult != false )
         {
            uiAddress += (uint16_t)ucFirstBytesCnt;
            pucData   += ucFirstBytesCnt;
            EXTEEP_ucCurentStep     = STEP_WAIT_UNTIL_READY;
            EXTEEP_ucStepAfterReady = EXTEEP_ucGetNextStep(uiPageCounter,uiNumberOfPages);
         }
         else
         {
            tRetVal = WRITE_FAILURE;
         }
         break;
      }
      case STEP_FULL_PAGE_WRITE:
      {
         bFuctResult = EXTEEP_tChipInfo[0].ptCommunicator->pfWrite(
                       uiAddress,
                       pucData,
                 (uint16_t)EXTEEP_tChipInfo[0].ucPageSize,
                       EXTEEP_tChipInfo[0].ucBaseAddress);
         if ( bFuctResult != false )
         {
            uiAddress += (uint16_t)EXTEEP_tChipInfo[0].ucPageSize;
            pucData   += EXTEEP_tChipInfo[0].ucPageSize;
            uiPageCounter++;
            EXTEEP_ucCurentStep = STEP_WAIT_UNTIL_READY;
            EXTEEP_ucStepAfterReady = EXTEEP_ucGetNextStep(uiPageCounter,uiNumberOfPages);
         }
         else
         {
            tRetVal = WRITE_FAILURE;
         }
         break;
      }
      case STEP_LAST_BYTES_WRITE:
      {
         if ( ucLastBytesCnt > 0u )
         {
            bFuctResult = EXTEEP_tChipInfo[0].ptCommunicator->pfWrite(
                          uiAddress,
                          pucData,
                    (uint16_t)ucLastBytesCnt,
                          EXTEEP_tChipInfo[0].ucBaseAddress);
            if ( bFuctResult != false )
            {
               EXTEEP_ucCurentStep     = STEP_WAIT_UNTIL_READY   ;
               EXTEEP_ucStepAfterReady = STEP_VALIDATE_LAST_BYTES;
            }
            else
            {
               tRetVal = WRITE_FAILURE;
            }
         }
         else
         {
            tRetVal = WRITE_SUCCESSFUL;
            EXTEEP_ucCurentStep = STEP_CALCULATE_PARAMETERS;
         }
         break;
      }
      case STEP_VALIDATE_LAST_BYTES:
      {
         tRetVal = WRITE_SUCCESSFUL;
         EXTEEP_ucCurentStep = STEP_CALCULATE_PARAMETERS;
         break;
      }
      case STEP_WAIT_UNTIL_READY:
      {
         bFuctResult = EXTEEP_tChipInfo[0].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[0].ucBaseAddress);
         if ( bFuctResult != false )
         {
            ucIsReadyTestNumber = 0;
            EXTEEP_ucCurentStep = EXTEEP_ucStepAfterReady;
         }
         else
         {
            ucIsReadyTestNumber++;
            if ( ucIsReadyTestNumber >= IS_READY_MAX_TEST_COUNT )
            {
               tRetVal = WRITE_FAILURE;
               ucIsReadyTestNumber = 0;
            }
         }
         break;
      }
      default:
      {
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(tRetVal);

}
/* static uint8_t EXTEEP_ucGetNextStep(uint16_t uiPageCounter, uint16_t uiNumberOfPages)*/
static uint8_t EXTEEP_ucGetNextStep(uint16_t uiPageCounter, uint16_t uiNumberOfPages)
{
   uint8_t ucResult;
   if ( (uiNumberOfPages > 0u) && (uiPageCounter < uiNumberOfPages) )
   {
      ucResult = STEP_FULL_PAGE_WRITE;
   }
   else
   {
      ucResult = STEP_LAST_BYTES_WRITE;
   }
   return(ucResult);
}
#else
/* static EXTEEP_bGetChipInfo()*/
/* Gets chip number and real address on this chip */
static realChipParamsType EXTEEP_tGetChipInfo( uint32_t ulAddress )
{
   uint8_t ucCounter;
   uint32_t ulBottomAddress;
   uint32_t ulTopAddress;
   realChipParamsType tResult;
   ulBottomAddress = 0UL;
   tResult.ucIndex = CHIP_UNKNOWN;
   tResult.uiAddressOnChip = ADDRESS_UNKNOWN;
   for (ucCounter=0; ucCounter < EXTEEP_ucChipCount; ucCounter++)
   {
      ulTopAddress = ulBottomAddress + EXTEEP_tChipInfo[ucCounter].uiMaxAddress;
      if (( ulAddress >= ulBottomAddress) && ( ulAddress <= ulTopAddress ))
      {
         tResult.ucIndex = ucCounter;
         tResult.uiAddressOnChip = (uint16_t)(ulAddress - ulBottomAddress);
         break;
      }
      else
      {
         ulBottomAddress += (uint32_t)EXTEEP_tChipInfo[ucCounter].uiMaxAddress + 1u;
      }
   }
   return(tResult);
}
/* static EXTEEP_bCalculateTaskParameters()*/
static bool EXTEEP_bCalculateTaskParameters(uint32_t ulAddress, uint8_t * const pucData, uint16_t uiLength)
{
   realChipParamsType tChipParams;
   bool bRetVal = false;
   uint16_t uiChipBytesAvailable;
   uint16_t uiRestOfBytes;
   uint8_t ucChipCounter;
   uint16_t uiStartAddress;
   uint16_t uiPosition;
   if ( uiLength > 0u )
   {
      if ( pucData != NULL )
      {
         if (((ulAddress + uiLength) - 1u) <= EXTEEP_ulMaxAddress)
         {
            uiRestOfBytes = uiLength;
            uiPosition = 0;
            tChipParams = EXTEEP_tGetChipInfo(ulAddress);
            uiStartAddress = tChipParams.uiAddressOnChip;
            for (ucChipCounter = tChipParams.ucIndex; ucChipCounter < EXTEEP_ucChipCount; ucChipCounter++)
            {
               if ( EXTEEP_tChipInfo[ucChipCounter].bIsChipCorrupt == false )
               {
                  EXTEEP_tChipInfo[ucChipCounter].tTaskParam.uiStartAddress = uiStartAddress;
                  EXTEEP_tChipInfo[ucChipCounter].tTaskParam.pucData = &pucData[uiPosition];
                  uiChipBytesAvailable = (EXTEEP_tChipInfo[ucChipCounter].uiMaxAddress - uiStartAddress) + 1u;
                  if ( uiRestOfBytes <= uiChipBytesAvailable )
                  {
                     /* data will be read from/write to this chip*/
                     EXTEEP_tChipInfo[ucChipCounter].tTaskParam.uiBytesCount = uiRestOfBytes;
                     uiRestOfBytes = 0;
                     break;
                  }
                  else
                  {
                     /* we need to read data from/write data into next chip too*/
                     EXTEEP_tChipInfo[ucChipCounter].tTaskParam.uiBytesCount = uiChipBytesAvailable;
                     uiStartAddress = 0;
                     uiPosition += uiChipBytesAvailable;
                     uiRestOfBytes -= uiChipBytesAvailable;
                  }
               }
               else
               {
                  /* chip is not readable*/
#if defined (DEBUG) && (DEBUG != 0u)
                  _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
                  break;
               }
            }
            if ( uiRestOfBytes == 0u )
            {
               bRetVal = true;
            }
         }
         else
         {
            /* out of range*/
#if defined (DEBUG) && (DEBUG != 0u)
            _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
         }
      }
      else
      {
         /* pointer to pucData is NULL*/
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
      }
   }
   else
   {
      /* length must be greater*/
#if defined (DEBUG) && (DEBUG != 0u)
      _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
   }
   return(bRetVal);
}
/* static EXTEEP_tWriteSinglePages()*/
static eWriteResultType EXTEEP_tWriteSinglePages( uint8_t ucChipInfoCounter )
{
   /* local variables*/
   eWriteResultType tRetVal = WRITE_ONGOING;
   static uint8_t ucFirstBytesCnt ; //number of first bytes
   static uint8_t ucLastBytesCnt  ; //number of last bytes
   static uint16_t  uiNumberOfPages ; //page contains ucPageSize bytes
   static uint16_t  uiPageCounter   ;
   static uint16_t  uiAddress;
   static const uint8_t *pucData;
   static uint8_t ucPageSize;
   static uint16_t  uiLength;
   static uint8_t ucIsReadyTestNumber = 0;
   uint8_t ucRemainder;
   bool bFuctResult;
   switch (EXTEEP_ucCurentStep)
   {
      case STEP_CALCULATE_PARAMETERS:
      {
         uiPageCounter = 0;
         uiAddress  = EXTEEP_tChipInfo[ucChipInfoCounter].tTaskParam.uiStartAddress;
         pucData    = EXTEEP_tChipInfo[ucChipInfoCounter].tTaskParam.pucData;
         ucPageSize = EXTEEP_tChipInfo[ucChipInfoCounter].ucPageSize;
         uiLength   = EXTEEP_tChipInfo[ucChipInfoCounter].tTaskParam.uiBytesCount;
         ucRemainder = (uint8_t)(uiAddress % ucPageSize);
         if ( ucRemainder > 0u ) // is address divisible by ucPageSize?
         {
            ucFirstBytesCnt = ucPageSize - ucRemainder;
         }
         else
         {
            ucFirstBytesCnt = 0;
         }
         if ( uiLength > (uint16_t)ucFirstBytesCnt )
         {
            uiNumberOfPages = (uint16_t)((uiLength - (uint16_t)ucFirstBytesCnt) / ucPageSize);
            ucLastBytesCnt  = (uint8_t)((uiLength - (uint16_t)ucFirstBytesCnt) % ucPageSize);
         }
         else
         {
            ucFirstBytesCnt = (uint8_t) uiLength;
            uiNumberOfPages = 0;
            ucLastBytesCnt  = 0;
         }
         EXTEEP_ucCurentStep = STEP_FIRST_BYTES_WRITE;
         /* fall through into next case */
      }  /* lint -e616 control flows into case/default */
      case STEP_FIRST_BYTES_WRITE:
      {
         if ( ucFirstBytesCnt > 0u )
         {
            bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
            if ( bFuctResult != false )
            {
               ucIsReadyTestNumber = 0;
               bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfWrite(
                             uiAddress,
                             pucData,
                       (uint16_t)ucFirstBytesCnt,
                             EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
               if ( bFuctResult != false )
               {
                  tRetVal = WRITE_ONGOING;
                  EXTEEP_ucCurentStep = STEP_VALIDATE_FIRST_BYTES;
               }
               else
               {
                  tRetVal = WRITE_FAILURE;
               }
            }
            else
            {
               ucIsReadyTestNumber++;
               if ( ucIsReadyTestNumber >= IS_READY_MAX_TEST_COUNT )
               {
                  tRetVal = WRITE_FAILURE;
                  ucIsReadyTestNumber = 0;
               }
            }
         }
         else
         {
            EXTEEP_ucCurentStep = STEP_FULL_PAGE_WRITE;
         }
         break;
      }
      case STEP_VALIDATE_FIRST_BYTES:
      {
         /* test after write process start*/
         bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
         if ( bFuctResult != false )
         {
            ucIsReadyTestNumber = 0;
            if ( EXTEEP_bVerifyWrittenData != false )
            {
               bFuctResult = EXTEEP_bVerifyLastData(uiAddress,pucData,ucFirstBytesCnt,ucChipInfoCounter);
               if ( bFuctResult != false )
               {
                  uiAddress += (uint16_t)ucFirstBytesCnt;
                  pucData   += ucFirstBytesCnt;
                  EXTEEP_ucCurentStep = STEP_FULL_PAGE_WRITE;
               }
               else
               {
                  /* data are not equal*/
                  EXTEEP_ucCurentStep = STEP_FIRST_BYTES_WRITE;
                  tRetVal = WRITE_FAILURE;
               }
            }
            else
            {
               uiAddress += (uint16_t)ucFirstBytesCnt;
               pucData   += ucFirstBytesCnt;
               EXTEEP_ucCurentStep = STEP_FULL_PAGE_WRITE;
            }
         }
         else
         {
            ucIsReadyTestNumber++;
            if ( ucIsReadyTestNumber >= IS_READY_MAX_TEST_COUNT )
            {
               tRetVal = WRITE_FAILURE;
               ucIsReadyTestNumber = 0;
            }
         }
         break;
      }
      case STEP_FULL_PAGE_WRITE:
      {
         if ((uiNumberOfPages > 0u) && (uiPageCounter < uiNumberOfPages))
         {
            bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
            if ( bFuctResult != false )
            {
               ucIsReadyTestNumber = 0;
               bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfWrite(
                             uiAddress,
                             pucData,
                       (uint16_t)ucPageSize,
                             EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
               if ( bFuctResult != false )
               {
                  tRetVal = WRITE_ONGOING;
                  EXTEEP_ucCurentStep = STEP_VALIDATE_FULL_PAGE;
               }
               else
               {
                  tRetVal = WRITE_FAILURE;
               }
            }
            else
            {
               ucIsReadyTestNumber++;
               if ( ucIsReadyTestNumber >= IS_READY_MAX_TEST_COUNT )
               {
                  tRetVal = WRITE_FAILURE;
                  ucIsReadyTestNumber = 0;
               }
            }
         }
         else
         {
            EXTEEP_ucCurentStep = STEP_LAST_BYTES_WRITE;
         }
         break;
      }
      case STEP_VALIDATE_FULL_PAGE:
      {
         /* test after write process start*/
         bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
         if ( bFuctResult != false )
         {
            ucIsReadyTestNumber = 0;
            if ( EXTEEP_bVerifyWrittenData != false )
            {
               bFuctResult = EXTEEP_bVerifyLastData(uiAddress,pucData,ucPageSize,ucChipInfoCounter);
               if ( bFuctResult != false )
               {
                  uiAddress += (uint16_t)ucPageSize;
                  pucData   += ucPageSize;
                  uiPageCounter++;
                  EXTEEP_ucCurentStep = STEP_FULL_PAGE_WRITE;
               }
               else
               {
                  /* data are not equal*/
                  EXTEEP_ucCurentStep = STEP_FULL_PAGE_WRITE;
                  tRetVal = WRITE_FAILURE;
               }
            }
            else
            {
               uiAddress += (uint16_t)ucPageSize;
               pucData   += ucPageSize;
               uiPageCounter++;
               EXTEEP_ucCurentStep = STEP_FULL_PAGE_WRITE;
            }
         }
         else
         {
            ucIsReadyTestNumber++;
            if ( ucIsReadyTestNumber >= IS_READY_MAX_TEST_COUNT )
            {
               tRetVal = WRITE_FAILURE;
               ucIsReadyTestNumber = 0;
            }
         }
         break;
      }
      case STEP_LAST_BYTES_WRITE:
      {
         if ( ucLastBytesCnt > 0u )
         {
            bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
            if ( bFuctResult != false )
            {
               ucIsReadyTestNumber = 0;
               bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfWrite(
                             uiAddress,
                             pucData,
                       (uint16_t)ucLastBytesCnt,
                             EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
               if ( bFuctResult != false )
               {
                  tRetVal = WRITE_ONGOING;
                  EXTEEP_ucCurentStep = STEP_VALIDATE_LAST_BYTES;
               }
               else
               {
                  tRetVal = WRITE_FAILURE;
               }
            }
            else
            {
               ucIsReadyTestNumber++;
               if ( ucIsReadyTestNumber >= IS_READY_MAX_TEST_COUNT )
               {
                  tRetVal = WRITE_FAILURE;
                  ucIsReadyTestNumber = 0;
               }
            }
         }
         else
         {
            tRetVal = WRITE_SUCCESSFUL;
            EXTEEP_ucCurentStep = STEP_CALCULATE_PARAMETERS;
         }
         break;
      }
      case STEP_VALIDATE_LAST_BYTES:
      {
         /* test after write process start*/
         bFuctResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
         if ( bFuctResult != false )
         {
            ucIsReadyTestNumber = 0;
            if ( EXTEEP_bVerifyWrittenData != false )
            {
               bFuctResult = EXTEEP_bVerifyLastData(uiAddress,pucData,ucLastBytesCnt,ucChipInfoCounter);
               if ( bFuctResult != false )
               {
                  uiAddress += (uint16_t)ucLastBytesCnt;
                  pucData   += ucLastBytesCnt;
                  tRetVal = WRITE_SUCCESSFUL;
                  EXTEEP_ucCurentStep = STEP_CALCULATE_PARAMETERS;
               }
               else
               {
                  /* data are not equal*/
                  EXTEEP_ucCurentStep = STEP_LAST_BYTES_WRITE;
                  tRetVal = WRITE_FAILURE;
               }
            }
            else
            {
               uiAddress += (uint16_t)ucLastBytesCnt;
               pucData   += ucLastBytesCnt;
               tRetVal = WRITE_SUCCESSFUL;
               EXTEEP_ucCurentStep = STEP_CALCULATE_PARAMETERS;
            }
         }
         else
         {
            ucIsReadyTestNumber++;
            if ( ucIsReadyTestNumber >= IS_READY_MAX_TEST_COUNT )
            {
               tRetVal = WRITE_FAILURE;
               ucIsReadyTestNumber = 0;
            }
         }
         break;
      }
      default:
      {
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
         ;
      }
   }
   return(tRetVal);

}
/* static EXTEEP_bVerifyLastData()*/
static bool EXTEEP_bVerifyLastData(uint16_t uiVerifyAddress, const uint8_t *pucVerifyData, uint8_t ucVerifyBytesCount, uint8_t ucChipInfoCounter)
{
   static uint8_t EXTEEP_ucVerifyField[VERIFY_FIELD_SIZE];
   uint8_t ucReadBytesCount;
   bool  bReadResult;
   bool  bRetVal;
   uint8_t ucCounter;
   bRetVal = true;
   do
   {
      if ( ucVerifyBytesCount > VERIFY_FIELD_SIZE )
      {
         ucReadBytesCount = VERIFY_FIELD_SIZE;
      }
      else
      {
         ucReadBytesCount = ucVerifyBytesCount;
      }
      bReadResult = EXTEEP_tChipInfo[ucChipInfoCounter].ptCommunicator->pfRead( uiVerifyAddress      ,
                                                                                EXTEEP_ucVerifyField ,
                                                                          (uint16_t)ucReadBytesCount     ,
                                                                                EXTEEP_tChipInfo[ucChipInfoCounter].ucBaseAddress);
      if ( bReadResult != false )
      {
         for (ucCounter = 0; ucCounter < ucReadBytesCount; ucCounter++)
         {
            if ( EXTEEP_ucVerifyField[ucCounter] != pucVerifyData[ucCounter] )
            {
               bRetVal = false;
               ucVerifyBytesCount = 0;
               break; //exit cyclus for
            }
         }
         /* QAC Message 3382: evaluation of this type operation will sometimes result in wraparound.*/
         /*~+:In this case can never occur*/
         ucVerifyBytesCount -= ucReadBytesCount;
         uiVerifyAddress    += ucReadBytesCount;
         pucVerifyData      += ucReadBytesCount;
      }
      else
      {
         bRetVal = false;
         break; //exit do/while cyclus
      }
   }
   while ( ucVerifyBytesCount > 0u );
   return(bRetVal);
}
#endif
#endif
/* Public functions*/
/* EXTEEP_bRead()*/
bool EXTEEP_bRead(uint32_t ulAddress, uint8_t * const pucData, uint16_t uiLength)
{
   bool bRetVal = false;
   bool bFuncResult;
   if ( EXTEEP_tTaskState == EEP_READY)
   {
      bFuncResult = EXTEEP_bCalculateTaskParameters(ulAddress,pucData,uiLength);
      if ( bFuncResult != false )
      {
         EXTEEP_tTaskState = EEP_READING;
         bRetVal = true;
      }
      else
      {
         /* debug*/
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
      }
   }
   else
   {
      /* debug*/
      /* not initialized or another task pending*/
#if defined (DEBUG) && (DEBUG != 0u)
      _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
   }
   return(bRetVal);
}
/* EXTEEP_bWrite()*/
bool EXTEEP_bWrite(uint32_t ulAddress, const uint8_t* const pucData, uint16_t uiLength)
{
   bool bRetVal = false;
   bool bFuncResult;
   uint8_t *data = (uint8_t *)((uintptr_t)pucData);

   if ( EXTEEP_tTaskState == EEP_READY)
   {
      bFuncResult = EXTEEP_bCalculateTaskParameters(ulAddress,data,uiLength);
      if ( bFuncResult != false )
      {
         EXTEEP_tTaskState = EEP_WRITING;
         bRetVal = true;
      }
      else
      {
         /* debug*/
#if defined (DEBUG) && (DEBUG != 0u)
         _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
      }
   }
   else
   {
      /* debug*/
      /* not initialized or another task pending*/
#if defined (DEBUG) && (DEBUG != 0u)
      _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
   }
   return(bRetVal);
}
/* EXTEEP_bWriteByte()*/
bool EXTEEP_bWriteByte(uint32_t ulAddress, uint8_t ucData)
{
   return( EXTEEP_bWrite(ulAddress,  &ucData, 1) );
}
/* EXTEEP_tGetStatus()*/
eTaskStateType EXTEEP_tGetStatus(void)
{
   return(EXTEEP_tTaskState);
}
#ifdef GSW_USE_SINGLE_MEMORY_CHIP
/* EXTEEP_ucHandleTask()*/
uint8_t EXTEEP_ucHandleTask(void)
{
   bool  bFuncResult               ;
   eWriteResultType tWriteResult   ;
   static uint8_t EXTEEP_ucErrors = 0;
   switch (EXTEEP_tTaskState)
   {
      case EEP_UNINITIALIZED:
      {
         bFuncResult = EXTEEP_atCommunicators[0].pfInit();
         if ( bFuncResult != false )
         {
            EXTEEP_tTaskState = EEP_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case EEP_READY:
      {
         break;
      }
      case EEP_READING:
      {
         /* reading part*/
         if ( EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount > 0u )
         {
            /* Test if chip is ready to communicate */
            bFuncResult = EXTEEP_tChipInfo[0].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[0].ucBaseAddress);
            if ( bFuncResult != false)
            {
               bFuncResult = EXTEEP_tChipInfo[0].ptCommunicator->pfRead( EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress,
               EXTEEP_tChipInfo[0].tTaskParam.pucData       ,
               EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount  ,
               EXTEEP_tChipInfo[0].ucBaseAddress);
               if ( bFuncResult != false )
               {
                  EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress = 0   ;
                  EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount   = 0   ;
                  EXTEEP_tChipInfo[0].tTaskParam.pucData        = NULL;
                  EXTEEP_ucErrors = 0;
               }
               else
               {
                  ++EXTEEP_ucErrors;
                  if ( EXTEEP_ucErrors >= MAX_READ_ERROR_NUMBER )
                  {
                     EXTEEP_tChipInfo[0].bIsChipCorrupt = true;
                     EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress = 0   ;
                     EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount   = 0   ;
                     EXTEEP_tChipInfo[0].tTaskParam.pucData        = NULL;
                     EXTEEP_ucErrors = 0;
                  }
               }
            }
            else
            {
               ++EXTEEP_ucErrors;
               if ( EXTEEP_ucErrors >= IS_READY_MAX_TEST_COUNT )
               {
                  EXTEEP_tChipInfo[0].bIsChipCorrupt = true;
                  EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress = 0   ;
                  EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount   = 0   ;
                  EXTEEP_tChipInfo[0].tTaskParam.pucData        = NULL;
                  EXTEEP_ucErrors = 0;
               }
            }
         }
         else
         {
            EXTEEP_tTaskState = EEP_READY;
         }
         break;
      }
      case EEP_WRITING:
      {
         /* writing part*/
         if ( EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount > 0u )
         {
            tWriteResult = EXTEEP_tWriteSinglePages();
            if ( tWriteResult == WRITE_SUCCESSFUL )
            {
               EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress = 0   ;
               EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount   = 0   ;
               EXTEEP_tChipInfo[0].tTaskParam.pucData        = NULL;
               EXTEEP_ucErrors = 0;
            }
            else
            {
               if ( tWriteResult == WRITE_FAILURE )
               {
                  ++EXTEEP_ucErrors;
                  if ( EXTEEP_ucErrors >= MAX_WRITE_ERROR_NUMBER )
                  {
                     EXTEEP_tChipInfo[0].bIsChipCorrupt = true;
                     EXTEEP_tChipInfo[0].tTaskParam.uiStartAddress = 0   ;
                     EXTEEP_tChipInfo[0].tTaskParam.uiBytesCount   = 0   ;
                     EXTEEP_tChipInfo[0].tTaskParam.pucData        = NULL;
                     EXTEEP_ucErrors = 0;
                     EXTEEP_ucCurentStep     = STEP_CALCULATE_PARAMETERS;
                     EXTEEP_ucStepAfterReady = STEP_CALCULATE_PARAMETERS;
                  }
               }
               else
               {
                  /* process ongoing*/
               }
            }
         }
         else
         {
            EXTEEP_tTaskState = EEP_READY;
         }
         break;
      }
      default:
      {
         /* default can never occur because switch variable is enum type*/
      }
   }
   return(TASK_INITIALISED);
}
#else
/* EXTEEP_ucHandleTask()*/
uint8_t EXTEEP_ucHandleTask(void)
{
   uint8_t ucCounter                 ;
   bool  bFuncResult               ;
   eWriteResultType tWriteResult   ;
   bool  bJobFound                 ;
   static uint8_t EXTEEP_ucErrors = 0;
   switch (EXTEEP_tTaskState)
   {
      case EEP_UNINITIALIZED:
      {
         bFuncResult = true;
         for (ucCounter = 0; ucCounter < EXTEEP_ucCommunicatorsCount; ucCounter++)
         {
            bFuncResult = bFuncResult && EXTEEP_atCommunicators[ucCounter].pfInit();
         }
         if ( bFuncResult != false )
         {
            EXTEEP_tTaskState = EEP_READY;
         }
         else
         {
            return(TASK_NOT_INITIALISED);
         }
         break;
      }
      case EEP_READY:
      {
         break;
      }
      case EEP_READING:
      {
         /* reading part*/
         bJobFound = false;
         for (ucCounter = 0; ucCounter < EXTEEP_ucChipCount; ucCounter++)
         {
            if ( EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount > 0u )
            {
               bJobFound = true;
               /* Test if chip is ready to communicate */
               bFuncResult = EXTEEP_tChipInfo[ucCounter].ptCommunicator->pfIsReady(EXTEEP_tChipInfo[ucCounter].ucBaseAddress);
               if ( bFuncResult != false)
               {
                  bFuncResult = EXTEEP_tChipInfo[ucCounter].ptCommunicator->pfRead( EXTEEP_tChipInfo[ucCounter].tTaskParam.uiStartAddress,
                  EXTEEP_tChipInfo[ucCounter].tTaskParam.pucData       ,
                  EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount  ,
                  EXTEEP_tChipInfo[ucCounter].ucBaseAddress);
                  if ( bFuncResult != false )
                  {
                     EXTEEP_tChipInfo[ucCounter].tTaskParam.uiStartAddress = 0   ;
                     EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount   = 0   ;
                     EXTEEP_tChipInfo[ucCounter].tTaskParam.pucData        = NULL;
                     EXTEEP_ucErrors = 0;
                  }
                  else
                  {
                     ++EXTEEP_ucErrors;
                     if ( EXTEEP_ucErrors >= MAX_READ_ERROR_NUMBER )
                     {
                        EXTEEP_tChipInfo[ucCounter].bIsChipCorrupt = true;
                        EXTEEP_tChipInfo[ucCounter].tTaskParam.uiStartAddress = 0   ;
                        EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount   = 0   ;
                        EXTEEP_tChipInfo[ucCounter].tTaskParam.pucData        = NULL;
                        EXTEEP_ucErrors = 0;
                     }
                  }
               }
               else
               {
                  ++EXTEEP_ucErrors;
                  if ( EXTEEP_ucErrors >= IS_READY_MAX_TEST_COUNT )
                  {
                     EXTEEP_tChipInfo[ucCounter].bIsChipCorrupt = true;
                     EXTEEP_tChipInfo[ucCounter].tTaskParam.uiStartAddress = 0   ;
                     EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount   = 0   ;
                     EXTEEP_tChipInfo[ucCounter].tTaskParam.pucData        = NULL;
                     EXTEEP_ucErrors = 0;
                  }
               }
               break; //from cyclus for
            }
         }
         if ( bJobFound == false )
         {
            EXTEEP_tTaskState = EEP_READY;
         }
         break;
      }
      case EEP_WRITING:
      {
         /* writing part*/
         bJobFound = false;
         for (ucCounter = 0; ucCounter < EXTEEP_ucChipCount; ucCounter++)
         {
            if ( EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount > 0u )
            {
               bJobFound = true;
               tWriteResult = EXTEEP_tWriteSinglePages(ucCounter);
               if ( tWriteResult == WRITE_SUCCESSFUL )
               {
                  EXTEEP_tChipInfo[ucCounter].tTaskParam.uiStartAddress = 0   ;
                  EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount   = 0   ;
                  EXTEEP_tChipInfo[ucCounter].tTaskParam.pucData        = NULL;
                  EXTEEP_ucErrors = 0;
               }
               else
               {
                  if ( tWriteResult == WRITE_FAILURE )
                  {
                     ++EXTEEP_ucErrors;
                     if ( EXTEEP_ucErrors >= MAX_WRITE_ERROR_NUMBER )
                     {
                        EXTEEP_tChipInfo[ucCounter].bIsChipCorrupt = true;
                        EXTEEP_tChipInfo[ucCounter].tTaskParam.uiStartAddress = 0   ;
                        EXTEEP_tChipInfo[ucCounter].tTaskParam.uiBytesCount   = 0   ;
                        EXTEEP_tChipInfo[ucCounter].tTaskParam.pucData        = NULL;
                        EXTEEP_ucErrors = 0;
                        EXTEEP_ucCurentStep     = STEP_CALCULATE_PARAMETERS;
                     }
                  }
                  else
                  {
                     /* process ongoing*/
                  }
               }
               break; //from cyclus for
            }
         }
         if ( bJobFound == false )
         {
            EXTEEP_tTaskState = EEP_READY;
         }
         break;
      }
      default:
      {
         /* default can never occur because switch variable is enum type*/
      }
   }
   return(TASK_INITIALISED);
}
/* EXTEEP_vVerify()*/
void EXTEEP_vVerify(bool bStatus)
{
   EXTEEP_bVerifyWrittenData = bStatus;
}
#endif
/*lint -restore */
