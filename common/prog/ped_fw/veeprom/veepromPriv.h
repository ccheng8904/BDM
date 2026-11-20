/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          VEEPROM
 ******************************************************************************/

#ifndef VEEPROM_PRIV_H
#define VEEPROM_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file VEEPROMpriv.h
 * Private definitions and declarations of Volatile EEPROM library
 *
 * This definitions are used in configuration file VEEPROMXs.c
 */
/**
 * \mainpage
 *
 * \section secVEEPROM Introduction: VEEPROM - Virtual EEPROM
 *   This library implements all you need for emulation of EEPROM using FLASH.
 *   Because EEPROM is emulated by using FLASH, you have to pay <b> attention</b> 
 *   when using this library not to exceed the limits of endurance of flash. A typically
 *   FLASH has an endurance of about 10,000 erase-cycles. So write only those portions
 *   of data to VEEPROM that are really needed to be stored there. 
 *
 * If you can, try to store
 *   write-once data, to a separate page of flash not using this library.
 * 
 * On this mainpage following sections are placed:
 *  - \ref secInterface, where the API is described in short
 *  - \ref secHowToUse
 *  - \ref secOrgData
 *  - \ref secCookbook
 *    - containing a very important subsection \ref subWarning
 *  - \ref secHowMuchData
 * 
 * \section secInterface 1. The interface
 *   This section shows the interface that is provided by this library. It contains functions
 *   to initialize the VEEPROM, to write data to VEEPROM and to read out data from VEEPROM:
 *   
 *    - VEEPROM_bInit() to initialize the library after a reset or at startup
 *    - VEEPROM_bWrite() to write data and
 *    - VEEPROM_ucRead() to read data.
 *    - VEEPROM_uiGetFreeSpace() to get number of free bytes in active flash-page
 *    - VEEPROM_bDoDefragNow() to start manually the process of defragmentation
 *    - VEEPROM_bApprobateDefrag() has to be implemented in VEEPROMXS.c. This function is called each time
        when process of defragmentation must be started. If VEEPROM_bApprobateDefrag() return true, it is
        allowed for library to start process of defragmentation. Otherwise it will not defrag Flash.
 *    - VEEPROM_vDefragEnded() has to be implemented in VEEPROMXS.c. It is called each time when process of defragmentation has been finished as a callback.
 *    - VEEPROM_vBeforePageErase() callback function called from VEEPROM_bDoDefragNow().The function is called each time when current data were copied into new page but erase process was not performed yet.Application can adapt this function for its needs.
 *
 *
 * \section secHowToUse 2. How to use this Library
 *  To use this library you simply have to include the VEEPROM.h and link to the VEEPROM.lib.
 *  The application also must call the FLASH_vEnableAccess() function to permit general access to flash. 
 * \note FlashLibXS.c gives hints on register settings which may be applied for platform in use
 in regard of clock, operating mode, wait states a.s.o. It is the responsibility to the developer 
 to enable the correct settings.
 *  
 *  In VEEPROMXS.c/h you have to  specify the start-addresses of flash-pages to be used to and  their size.
 *  You must implement a function called <B>VEEPROM_bApprobateDefrag()</B>. This function is called each time when
 *  process of defragmentation is started automatically because too less space was left to perform a 
 *  VEEPROM_bWrite() operation. If VEEPROM_bApprobateDefrag() returns true, it is allowed for library to 
 *  defrag flash. Otherwise it is not allowed. This can be wanted when performing critical section in 
 *  Application where it is not wanted that process of defragmentation locks interrupts for some 
 *  microseconds.
 *
 *  Finally specify in the table  <B>CACHE_tPartitionTable</B> the drawers: 
 *   - pointer, where data shall be cached.
 *   - size of each drawer, (have a look at \ref secCookbook)
 *
 *  To have an impression how much data is written in specific  period of time and thus how often flash
 *  has to be erased, have a look at \ref secHowMuchData .
 *
 *
 *
 *
 *  VEEPROM_bWrite() also checks, if data to write is different from latest data already stored. It
 *  only writes a new drawer if the content to write is new.
 *
 * 
 * \section secOrgData 3. How data is organized
 *   Data is organized in drawers. Each drawer can have its own specific size. If content of
 *   a drawer changed, it is rewritten at the next free position of FLASH-page. 
 *   Each drawer has the following design:
 *   \dot
       digraph structs {
         node [shape=record];
         structDrawer [shape=record,label="<CF> CF |<L1> IL |<D0> D0 |<D1> D1 |<D2> D2 |<D3> D3 | ... |<Dn> Dn "];
         node [shape=plaintext, fontsize=16];
         structDrawer:CF -> "Header of drawer";
         structDrawer:L1 -> "Header of drawer";
         structDrawer:D0 -> "contents of the drawer";
         structDrawer:D1 -> "contents of the drawer";
         structDrawer:D2 -> "contents of the drawer";
         structDrawer:D3 -> "contents of the drawer";
         structDrawer:Dn -> "contents of the drawer";
     \enddot
 *   where: CF=<b>Consistency Field</b>, IL=<b>Identification Label</b>, D0..Dn= bytes of data.
 *
 *   \note if #VEEPROM_CRC_VALIDATION_USED is defined in this file, then the Consistency Field is used as the CRC 
 *         over Identification Label and Payload.
 *   
 *   An example on how data is stored in FLASH, is shown in the next picture:
 *   \dot
       digraph structs {
         node [shape=plaintext, fontsize=16];
         "Page Erase Counter"
         "start of drawer 00"
         "start of drawer 0x15"
         "start of drawer 0x17"
         "first free position"
         node [shape=record];
         
         structFlashPage [shape=record,label=\
"{{<EC1> 00 |<EC2> 05 |<CF1>[C1 |<IL1> 00 |<D1_1> FF |<D1_2> 01]|<CF2>[A3 |<IL2> 17 |\
<D2_1> FF]|<CF3>[B5 |<IL3> 17 |<D3_1> F2]|<CF4>[69 |<IL4> 15 |<D4_1> 32 |<D4_2> 54 }|\
{<D4_3> 1A |<D4_4> F2]|<CF5>[DE |<IL5> 17 |<D5_1> 01]|<firstFreePos> FF | FF | FF | \
FF | FF | FF | FF | FF | FF | FF | FF }|\
\
{ FF | FF | FF | FF | FF | FF | FF | FF | FF | FF | FF | FF | FF | FF | FF | FF }| {...}| {...}| {...}| {...}}"];
         
         "Page Erase Counter" -> structFlashPage:EC1;
         "Page Erase Counter" -> structFlashPage:EC2;
         structFlashPage:CF1 -> "start of drawer 00";
         structFlashPage:CF4 -> "start of drawer 0x15" ;
         structFlashPage:CF2 -> "start of drawer 0x17" ;
         structFlashPage:CF3 -> "start of drawer 0x17" ;
         structFlashPage:CF5 -> "start of drawer 0x17";
         "first free position" -> structFlashPage:firstFreePos;
       }
     \enddot 
 *   In this example, DataLen of drawer 00 is 2Bytes, drawer 0x15 is 4 Bytes long and drawer 0x17 is 1 Bytes long.
 *   As you can see on the first two bytes of the FLASH-page, PageEraseCounter counts 5. This means that before this
 *   page has been written, 5 pages have been erased before.
 *   Drawer with identifier 00 contains valid data, consistency field contains value that is calculated internally. Its payload is: [0xFF 0x01].
 *   Drawer with identifier 0x15 contains also valid data: [0x32,0x54,0x1A,0xF2]. Data with identifier 0x17 contains actually the data: [01].
 *
 *
 * \section secCookbook 4. A cookbook for setting size of drawers
 *   This section tries to give a recipe on how to cook the partition-table of flash.
 *
 *   \subsection subCookIntro 4.1. Optimal drawer capacity
 *   Defining size of drawer shall be done with regard to limited endurance of flash.
 *   Thus you should set size of drawers to the "optimal" value.
 *   
 *   Partitioning data to be stored will be different from project to project. Do not
 *   forget that data flash can be erased only about 10,000 times!
 * 
 *   \subsection subCookProCons 4.2. Pros and Cons about large/small drawers
 *   Some points to think about of:
 *   - the larger a drawer is, the more data it can keep with only one time of drawer-overhead.
 *   - <b>but:</b> if only one value within this drawer has to be updated, the whole drawer has
 *     to be rewritten at the next free position of flash-page. Thus only try to merge data that
 *     changes at all.
 *
 *   \subsection subCookRecipe 4.3. Now the recipe
 *   So a recipe to partition data can be:
 *   - If you can, store "write-once-data" in a separate page (not using this lib) e.g. in program-data.
 *   - Try to have as much "single-data" drawer, as possible. e.g. Integer-Value would have 
 *     drawer-length of 2 (2 * 8Bit = 16 byte = sizeof(int)). 
 *   - If you cannot store "write-one-data in a extra page, put it together in one drawer.
 *   - If there are too less drawers, try to put "write not so often"-data in one drawer.
 *   
 *   \subsection subWarning 4.4. Warning
 *   And remember: If a drawer contains 10 separated data and you have to update because only one single
 *   datum has changed, the whole drawer must be written once again at the next free position of flash.
 *   This is ineffective, because 9 bytes are written, that must not have been written, if every data
 *   had its own drawer.
 *
 *   But if there are data changing together, you should put them in one drawer acumulated. If you can do
 *   so, you save the additional headers.
 * 
 * 
 * 
 * 
 * \section secHowMuchData 5. How much data is written in a specific period of time?
 *  It is not too easy to estimate how much data has to be written in a specific period of time.
 *  To have an instrument for this, you can have a look at the first two bytes of the active flashPage.
 *  Each time a full flashPage is full and defragmented to next free page, the Page-Erase-Counter is 
 *  incremented by one. But therefor this library must be compiled with option #VEEPROM_FEATURE_PAGE_ERASE_COUNTER defined.
 * 
 *
 * \note if you need more detailed comments on functions, generate this docu again with 
 *       \code INTERNAL_DOCS = YES \endcode set in file doxy.inc
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "LibTypes.h"
#include "timer/timerlib.h"
#include "VEEPROM.h"
/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
