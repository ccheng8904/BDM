/*~A:Module Header*/
/*****************************************************************************************************************
* Copyright by BSH Bosch und Siemens Hausgeraete GmbH - EDS
*
*-----------------------------------------------------------------------------------------------------------------
*
* Die Nutzung, die Vervielfaeltigung und die Weitergabe dieser Software oder ihrer Teile ist nur mit
* ausdruecklicher schriftlicher Zustimmung der BSH EDS gestattet. Die Software darf vom Verwender
* ausschliesslich fuer die Entwicklung von Elektroniken fuer BSH genutzt werden, eine Nutzung fuer
* sonstige Zwecke (z.B. Entwicklung von Elektroniken fuer Dritte) ist untersagt. Saemtliche Rechte,
* einschliesslich aller Urheber-, Patent- und Gebrauchsmusterrechte, an der Software bleiben BSH
* vorbehalten. Zuwiderhandlungen werden straf- und zivilrechtlich verfolgt.
*
* Die Software wurde nach dem Stand der Technik erstellt und geprueft. Aufgrund der Komplexitaet der
* Implementierung von Embedded Controller Software sind  vom Verwender fuer jede Implementierung eigene
* Tests durchzufuehren, um die Funktionssicherheit in seiner Entwicklung sicherzustellen. Die Software
* wurde fuer den Einsatz als Bibliothek entwickelt. Jede anderweitige Verwendung erfolgt in alleiniger
* Verantwortung des Nutzers. Der Nutzer ist berechtigt, die Software zur Anpassung an andere
* Microcontroller und andere Compiler in eigener Verantwortung zu veraendern  und anzupassen. 
* BSH leistet keinen Support fuer derartige Anpassungen, ist jedoch
* berechtigt, von Verwender vorgenommene Anpassungen fuer eigene Zwecke kostenfrei zu nutzen.
*
* Die Haftung der BSH fuer die Funktionsfaehigkeit und -sicherheit der Software, auch beim  Einsatz in
* einer konkreten Anwendung, und moegliche, durch sie verursachte Folgeschaeden ist, ausser im Falle von
* Vorsatz oder grober Fahrlaessigkeit, ausgeschlossen. In jedem Fall ist die Haftung der BSH jedoch auf
* den typischerweise auftretenden, vorhersehbaren Schaden begrenzt.
*
* Technische Aenderungen sind vorbehalten.
*
*-----------------------------------------------------------------------------------------------------------------
*
* Use, reproduction and dissemination of this software or parts of it, is not permitted without express
* written authority of BSH EDS. The user is allowed to use this software exclusively for the development
* of electronic boards for BSH. Usage for other purposes is strictly prohibited (e.g. development of
* electronic boards for third parties). All rights, including copyright, rights created by patent grant
* or registration, and rights by protection of utility patents, are reserved. Violations will be
* prosecuted by civil and criminal law.
*
* The software was created and approved by acknowledged rules of technology. Because of the complexity
* of embedded controller software the user of this software has to perform his own tests to ensure
* proper functionality in his environment. The software was developed for usage as a software library.
* The user is sole responsible for every other usage.  The user may modify the software for adaptations
* needed for other microcontrollers, as well as to another compilers Metrowerks at own risk. 
* BSH will not support any adaptations or changes. BSH is authorised to use all adaptations for 
* own purposes free of charge.
*
* BSH assumes no liability for the functionality or reliability of the software even in concrete
* applications. BSH assumes no liability for consequential damages, except in case of intention or gross
* negligence. In any case the liability is limited to the typical and predictable damage.
*
* Technical changes are reserved.
*
******************************************************************************************************************
   PROJECT          NVMEN
   MODULE-PREFIX    NVMEN_
   FILE             %PM%
   ARCHIVE          %PP%:%PI%
   PROCESSOR        independent
   COMPILER         independent
 *****************************************************************************************************************
   AUTHOR           emka
   CREATED          2006-16-11
   LAST CHANGE      %PRT%
   LAST REVISION    %PR%
   STATUS           %PS%
 *****************************************************************************************************************
   DESCRIPTION                                                                                                   */
/** \file NVMEMXS.c
  Application specific implementation / configuration for Non Volatile Memory library                           */
/****************************************************************************************************************
  CHANGES
  %PL%
*****************************************************************************************************************/
/*~E*/
/*~A:Includes*/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "nvmem.h"
#include "NVMEMXS.h"
#ifdef TRIGGER_WDO
#include "hwdt.h"
#endif

/*~E*/
/*~A:Definitions and Declarations*/
/**
 * NVMEM_CACHE_BUFFER_SIZE must be adapted to size of largestDrawer in CACHE_tPartitionTableVeeprom
 * This is defined in NVMEMXS.c
 */
uchar NVMEM_aucCacheBuffer[ NVMEM_CACHE_BUFFER_SIZE ];
/*~T*/
/*
 * Application must adapt this buffer to the number of drawers divided by 8 ((CACHE_tNumberOfDrawersVEEPROM + CACHE_ucNumberOfDrawersEEPROM + 7)/ 8)
 * Needed only if NVMEM_ucHandleTask() is used in application
 */
uchar NVMEM_aucDirtyFlags[ NVMEM_DIRTY_FLAGS_MAX_BUFFER_SIZE ];
#if defined(TEST_ACTIVE)
#define TEST_TASK_INITIALIZE                         0
#define TEST_TASK_WRITE_INIT_VALUES                  1
#define TEST_TASK_READY                              2
#define TEST_TASK_WRITE                              3
#define TEST_TASK_READ                               4
static volatile uchar NVMEM_ucTaskState = TEST_TASK_INITIALIZE;
static const uchar NVMEM_aucInit[] = {1,2,3,4,5,6,7,8,9,10};
static uchar NVMEM_aucWrite[]      = {'A','B','C','D','E','F','G','H','I','J','K'};
static uchar NVMEM_aucRead[]       = {0,0,0,0,0,0,0,0,0,0 };
static volatile tCacheLocationIdentifier NVMEM_tCurrentItem = _1_CACHE_PLACE_1BYTE;
#endif
/*~K:Functions*/
/*~A:_NVMEM_vResetWatchdogTimer()*/
void _NVMEM_vResetWatchdogTimer( void )
{
#ifdef TRIGGER_WDO
   HWDT_vTrigger();
#endif
}
/*~E*/
#if defined(TEST_ACTIVE)
/*~A:NVMEM_vTest()*/
uchar NVMEM_ucTest(void)
{
   static BOOL bResult = FALSE;
   uint uiCounter;
   switch (NVMEM_ucTaskState)
   {
      case TEST_TASK_INITIALIZE:
      {
         bResult = NVMEM_bInit();
         if (bResult != FALSE)
         {
            NVMEM_ucTaskState = TEST_TASK_WRITE_INIT_VALUES;
         }
         break;
      }
      case TEST_TASK_WRITE_INIT_VALUES:
      {
         bResult = TRUE;
         for (uiCounter = 0; uiCounter < (uint)CACHE_PLACE_COUNT; uiCounter++)
         {
            bResult = NVMEM_bWrite((tCacheLocationIdentifier)uiCounter, NVMEM_aucInit) && bResult;
         }
         if (bResult != FALSE)
         {
            NVMEM_ucTaskState = TEST_TASK_READY;
         }
         break;
      }
      case TEST_TASK_READY:
      {
          break;
      }
      case TEST_TASK_WRITE:
      {
         if (NVMEM_tCurrentItem < CACHE_PLACE_COUNT)
         {
            bResult = NVMEM_bWrite(NVMEM_tCurrentItem, NVMEM_aucWrite);
         }
         else
         {
            bResult = FALSE;
         }
         NVMEM_ucTaskState = TEST_TASK_READY;
         /*~T*/
         break;
      }
      case TEST_TASK_READ:
      {
         if (NVMEM_tCurrentItem < CACHE_PLACE_COUNT)
         {
             for (uint16_t u16Counter = 0u; u16Counter < (uint16_t)sizeof(NVMEM_aucRead); u16Counter++)
             {
                 NVMEM_aucRead[u16Counter] = 0;
             }
            bResult = NVMEM_bRead(NVMEM_tCurrentItem, NVMEM_aucRead);
         }
         else
         {
            bResult = FALSE;
         }
         NVMEM_ucTaskState = TEST_TASK_READY;
         /*~T*/
         break;
      }
   }
   return NVMEM_ucTaskState;
}
/*~E*/
#endif
/*~E*/
