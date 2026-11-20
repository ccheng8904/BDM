/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          NVMEM
 ******************************************************************************/
#ifndef __NVMEM_H
#define __NVMEM_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
#if defined(CACHE_INCLUDE_VEEPROM) && defined(CACHE_INCLUDE_EEPROM)
/**
\if NVMEM
\file 
 Interfaces for the handling of <b>N</b>on <B>V</B>olatile <B>MEM</B>mory.
\endif
*/
/**
\if NVMEM
\mainpage NVMEM is the EDS internal abbreviation for Non Volatile MEMmory.
\else
\file 
 Interfaces for the handling of <B>N</B>on <B>V</B>olatile <B>MEM</B>mory.
\endif


 * \section secIntro Introduction
 *
 * The NVMEM library calls functions of lower level
 * libraries providing functions for reading and writing data. Libraries that are supported up to now
 * are:
 * - VEEPROM library.
 * - ExtEEPROM library
 *
 * The lower libraries are hidden for the user of this library. In the partition table
 * CACHE_tPartitionTableVeeprom in file cacheXS.c it  has to be specified where data shall be cached and the size of each cache-location.
 *
 *
 * The interface of this library is kept very simple. For details see the page \ref pageinter1.
 *
 *
 * Every portion of data is stored in a container. A container has a unique identifier and a defined size. The size of
 * the container, its identifier and cache location are set in the partition tables defined 
 * in the file cacheXS.c:
 * - CACHE_tPartitionTableVeeprom defines all drawers that are stored in VEEPROM
 * 
 * These tables have two entries per index: Cache Address and Size of Drawer. The index is taken as Drawer Identifier. 
 * <B>All entries must have different cache locations</B>. It is not allowed to share one cache location for two technologies!
 *
 * \section VEEPROM
 * When using VEEPROM as underlying library we have to pay attention on how to partition data. This, because due to
 * the technology FLASH we have a very limited program/erase endurance.
 *
 * Application has control when defragmenting data can be performed, therefore it has to implement a callbackfunction: VEEPROMXS::VEEPROM_bApprobateDefrag(). This function is called everytime when the
active flash-page is full and it has to be defragmented and transfered to the other flash-page. After
process of defragmentation has finished, callback-function VEEPROMXS::VEEPROM_bDefragEnded() is called.
 *
 * \attention Take care on how to partition the partition table, because of limited program/erase endurance of different technologies!!! Therefore see page \ref pagecookbook to get some ideas how to partition data.
 *
 * To check out how much data is stored in a specific period of time, check page \ref pagehowmuchdata.
 *
 *
 * \section EEPROM
 * To use this feature, you need to add entries to the partition table, CACHE_tPartitionTableEeprom.
 * There are two types of external EEPROM libraries:
 * - ExtEEPROMsingle.lib is used for boards with one external memory chip connected by bus (I2C, SPI, ...) to processor
 * - ExtEEPROM.lib is used for boards with more external memory chips connected by common bus (I2C, SPI, ...) to processor
 *
 * \attention Take care that external EEPROM memories are paged and save process into a page takes some millisecond depending on HW producer.
 */
#else
#if !defined(CACHE_INCLUDE_VEEPROM) && defined(CACHE_INCLUDE_EEPROM)
/**
\if NVMEMext
\file 
 Interfaces for the handling of <b>N</b>on <B>V</B>olatile <B>MEM</B>mory.
\endif
*/
/**
\if NVMEMext
\mainpage NVMEM is the EDS internal abbreviation for Non Volatile MEMmory.
\else
\file 
 Interfaces for the handling of <B>N</B>on <B>V</B>olatile <B>MEM</B>mory.
\endif


 * \section secIntro Introduction
 *
 * The NVMEMext library calls functions of lower level
 * library providing functions for reading and writing data. Library that is supported :
 * - ExtEEPROM library
 *
 * The lower library is hidden for the user of this NVMEMext library. In the partition table
 * CACHE_tPartitionTableEeprom in file cacheXS.c it has to be specified where data shall be cached, the size of each cache-location and 
 * absolute address in the EEPROM chip.
 *
 * The interface of this library is kept very simple. For details see the page \ref pageinter1.
 *
 *
 * Every portion of data is stored in a container. A container has a unique identifier and a defined size. The size of
 * the container, its identifier and cache location are set in the partition tables defined 
 * in the file cacheXS.c:
 *
 *
 * \section EEPROM
 * To use this feature, you need to add entries to the partition table, CACHE_tPartitionTableEeprom.
 * There are two types of external EEPROM libraries:
 * - ExtEEPROMsingle.lib is used for boards with one external memory chip connected by bus (I2C, SPI, ...) to processor
 * - ExtEEPROM.lib is used for boards with more external memory chips connected by common bus (I2C, SPI, ...) to processor
 *
 * \attention Take care that external EEPROM memories are paged and save process into a page takes some millisecond depending on HW producer.
 */
#endif
#endif
#if defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM)
/**
 * \page pageinter1 Interface
 * The interface of this library is kept very simple. 
 * *
 * Following API is provided by this library:
 * - NVMEM_bInit() for initializing this library. Must be
 *   called once before other functions are called.
 * - NVMEM_bWrite() for writing data to technology. The 
 *   data is routed to the correct technology acordingly 
 *   to its cache location. 
 * - NVMEM_bWriteStart() starts write process. Rest of work must be done by
 * call of function NVMEM_ucHandleTask().
 * - NVMEM_ucHandleTask() task handler API
 * - NVMEM_bFlushBuffer() writes all values which were already changed in cache places
 * - NVMEM_uiGetUnsavedCount() gets number of unsaved cache variables.
 * - NVMEM_tGetStatus() gets library current status
 * - NVMEM_bIsSavePending() save process pending check function
 * - NVMEM_vFlushDefragEnabled() sets data flush behavior
 * - NVMEM_bRead() for reading out data from application.It
 *   can be done also directly by reading out cache locations.
 */
#else
/**
 * \page pageinter1 Interface
 * The interface of this library is kept very simple. 
 * *
 * Following API is provided by this library:
 * - NVMEM_bInit() for initializing this library. Must be
 *   called once before other functions are called.
 * - NVMEM_bWrite() for writing data to technology. The 
 *   data is routed to the correct technology acordingly 
 *   to its cache location. 
 * - NVMEM_bRead() for reading out data from application.It
 *   can be done also directly by reading out cache locations.
 */
#endif
#if defined(CACHE_INCLUDE_VEEPROM)
#if defined(VEEPROM_LARGE)
/**
 * \page pagecookbook VEEPROM Cookbook - or: How to partition data using VEEPROM
 *
 * How to partition data depends on the technology you use. 
 *
 * \section secVEEPROM VEEPROM using FLASH
 * When you use VEEPROM to emulate EEPROM in FLASH, it is a good idea to think twice on how to 
 * store data. Because on this technology we have a special limited program/erase endurance it is even
 * more important to have the partitionened data best to extend lifespan of FLASH.
 *
 *
 *
 *   This section tries to give a recipe on how to cook the partition-table of flash.
 *
 *   \subsection subCookIntro 1. Optimal drawer capacity
 *   Defining size of drawer shall be done with regard to limited endurance of flash.
 *   Thus you should set size of drawers to the "optimal" value.
 *   
 *   Partitioning data to be stored will be different from project to project. Do not
 *   forget that flash can be erased only about 10,000 times!
 * 
 *   \subsection subCookProCons 2. Pros and Cons about large/small drawers
 *   Some points to think about of:
 *   - the larger a drawer is, the more data it can keep with only one time of drawer-overhead.
 *     \dot
        digraph structs {
         node [shape=plaintext, fontsize=14];
         structBigDrawer [shape=record,label="{{<v>CRC|<i>ID1|ID1|B0|B1|B2|B3|B4|...|Bn}}"];
         "Verification Field" -> structBigDrawer:v;
         "unique Identifier" -> structBigDrawer:i;
 *     \enddot
 *     \dot
        digraph structs {
         node [shape=plaintext, fontsize=14];
         structSmallDrawer1 [shape=record,label="{{<v>CRC|<i>ID2|ID2|B0}}"];
         structSmallDrawer2 [shape=record,label="{{<v>CRC|<i>ID3|ID3|B0}}"];
         structSmallDrawer3 [shape=record,label="{{<v>CRC|<i>IDN|IDN|B0}}"];

         "Verification Field" -> structSmallDrawer1:v;
         "Verification Field" -> structSmallDrawer2:v;
         "Verification Field" -> structSmallDrawer3:v;

         "unique Identifier" -> structSmallDrawer1:i
         "unique Identifier" -> structSmallDrawer2:i
         "unique Identifier" -> structSmallDrawer3:i
        }
       \enddot


 *   - <b>but:</b> if only one value within this drawer has to be updated, the whole drawer has
 *     to be rewritten at the next free position of flash-page. Thus only try to pack more than one
 *     data in one drawer that changes together.
 *
 *   \subsection subCookRecipe 3. Now the recipe
 *   So a recipe to partition data can be:
 *   - If you can, store "write-once-data" in a separate page (not using this lib) e.g. in program-data.
 *   - Try to have as much "single-data" drawer, as possible. e.g. Integer-Value would have 
 *     drawer-length of 2 (2 * 8Bit = 16 byte = sizeof(int)). 
 *   - If you cannot store "write-one-data in a extra page, put it together in one drawer.
 *   - If there are too less drawers, try to put "write not so often"-data in one drawer.
 *   
 *   \subsection subWarning 4. Warning
 *   And remember: If a drawer contains 10 separated data and you have to update because only one single
 *   datum has changed, the whole drawer must be written once again at the next free position of flash.
 *   This is ineffective, because 9 bytes are written, that must not have been written, if every data
 *   had its own drawer.
 *
 *   But if there are data changing together, you should put them in one drawer acumulated. If you can do
 *   so, you save the additional headers.
 * 
 */

/** 
 * \page pagehowmuchdata How much data is written in a specific period of time?
 *
 *  It is not too easy to estimate how much data has to be written in a specific period of time.
 *  To have an instrument for this, you can have a look at the first two bytes of the active flashPage.
 *  Each time a full flashPage is full and defragmented to next free page, the Page-Erase-Counter is 
 *  incremented by one. 
 * 
 *  But therefore this library must be compiled with option #VEEPROM_FEATURE_PAGE_ERASE_COUNTER defined (which is by default).
 *   \dot
       digraph structs {
         node [shape=plaintext, fontsize=16];
         "Page Erase Counter";
         node [shape=record];  
         
         structFlashPage [shape=record,label="{{<PC1> 00 |<PC2> 05 | now drawers follow ...}}"];
         "Begin of Flash-page" -> structFlashPage:PC1;
         "Page Erase Counter" -> structFlashPage:PC1;
         "Page Erase Counter" -> structFlashPage:PC2;
       }
     \enddot 
 * In the figure above we see an extract of a flash-page. In this example it is the sixth time that a page has 
 * been erased (0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05). If this situation has been reached after n cycles you can
 * estimate if write/erase endurance of flash is not exceeded.
 */
#else
#if defined(STM32F0)
/**
 * \page pagecookbook VEEPROM Cookbook - or: How to partition data using VEEPROM
 *
 * How to partition data depends on the technology you use. 
 *
 * \section secVEEPROM VEEPROM using FLASH
 * When you use VEEPROM to emulate EEPROM in FLASH, it is a good idea to think twice on how to 
 * store data. Because on this technology we have a special limited program/erase endurance it is even
 * more important to have the partitionened data best to extend lifespan of FLASH.
 *
 *
 *
 *   This section tries to give a recipe on how to cook the partition-table of flash.
 *
 *   \subsection subCookIntro 1. Optimal drawer capacity
 *   Defining size of drawer shall be done with regard to limited endurance of flash.
 *   Thus you should set size of drawers to the "optimal" value.
 *   
 *   Partitioning data to be stored will be different from project to project. Do not
 *   forget that flash can be erased only about 10,000 times!
 * 
 *   \subsection subCookProCons 2. Pros and Cons about large/small drawers
 *   Some points to think about of:
 *   - the larger a drawer is, the more data it can keep with only one time of drawer-overhead.
 *     \dot
        digraph structs {
         node [shape=plaintext, fontsize=14];
         structBigDrawer [shape=record,label="{{<v>CRC|<b>FF|<i>ID1|<c>FF|B0|B1|B2|B3|B4|...|Bn}}"];
         "Verification Field" -> structBigDrawer:v;
         "unique Identifier" -> structBigDrawer:i;
         "unused Bytes" -> structBigDrawer:b;
         "unused Bytes" -> structBigDrawer:c;
 *     \enddot
 *     \dot
        digraph structs {
         node [shape=plaintext, fontsize=14];
         structSmallDrawer1 [shape=record,label="{{<v>CRC|<a>FF|<i>ID2|<b>FF|B0|<c>FF}}"];
         structSmallDrawer2 [shape=record,label="{{<v>CRC|<d>FF|<i>ID3|<e>FF|B0|<f>FF}}"];
         structSmallDrawer3 [shape=record,label="{{<v>CRC|<g>FF|<i>IDN|<h>FF|B0|<j>FF}}"];

         "Verification Field" -> structSmallDrawer1:v;
         "Verification Field" -> structSmallDrawer2:v;
         "Verification Field" -> structSmallDrawer3:v;

         "unique Identifier" -> structSmallDrawer1:i
         "unique Identifier" -> structSmallDrawer2:i
         "unique Identifier" -> structSmallDrawer3:i

         "unused Bytes" -> structSmallDrawer1:a;
         "unused Bytes" -> structSmallDrawer1:b;
         "unused Bytes" -> structSmallDrawer1:c;
         "unused Bytes" -> structSmallDrawer2:d;
         "unused Bytes" -> structSmallDrawer2:e;
         "unused Bytes" -> structSmallDrawer2:f;
         "unused Bytes" -> structSmallDrawer3:g;
         "unused Bytes" -> structSmallDrawer3:h;
         "unused Bytes" -> structSmallDrawer3:j;
        }
       \enddot


 *   - <b>but:</b> if only one value within this drawer has to be updated, the whole drawer has
 *     to be rewritten at the next free position of flash-page. Thus only try to pack more than one
 *     data in one drawer that changes together.
 *   - <b>Remember:</b> This processor hardware saves allways two bytes at once. Hence some bytes stay there unused.


 *
 *   \subsection subCookRecipe 3. Now the recipe
 *   So a recipe to partition data can be:
 *   - If you can, store "write-once-data" in a separate page (not using this lib) e.g. in program-data.
 *   - Try to have as much "single-data" drawer, as possible. e.g. Integer-Value would have 
 *     drawer-length of 2 (2 * 8Bit = 16 byte = sizeof(int)). 
 *   - If you cannot store "write-one-data in a extra page, put it together in one drawer.
 *   - If there are too less drawers, try to put "write not so often"-data in one drawer.
 *   
 *   \subsection subWarning 4. Warning
 *   And remember: If a drawer contains 10 separated data and you have to update because only one single
 *   datum has changed, the whole drawer must be written once again at the next free position of flash.
 *   This is ineffective, because 9 bytes are written, that must not have been written, if every data
 *   had its own drawer.
 *
 *   But if there are data changing together, you should put them in one drawer acumulated. If you can do
 *   so, you save the additional headers.
 * 
 */

/** 
 * \page pagehowmuchdata How much data is written in a specific period of time?
 *
 *  It is not too easy to estimate how much data has to be written in a specific period of time.
 *  To have an instrument for this, you can have a look at the first two bytes of the active flashPage.
 *  Each time a full flashPage is full and defragmented to next free page, the Page-Erase-Counter is 
 *  incremented by one. 
 * 
 *  But therefore this library must be compiled with option #VEEPROM_FEATURE_PAGE_ERASE_COUNTER defined (which is by default).
 *   \dot
       digraph structs {
         node [shape=plaintext, fontsize=16];
         "Page Erase Counter";
         node [shape=record];  
         
         structFlashPage [shape=record,label="{{<PC1> 00 |<PC2> 05 | now drawers follow ...}}"];
         "Begin of Flash-page" -> structFlashPage:PC1;
         "Page Erase Counter" -> structFlashPage:PC1;
         "Page Erase Counter" -> structFlashPage:PC2;
       }
     \enddot 
 * In the figure above we see an extract of a flash-page. In this example it is the sixth time that a page has 
 * been erased (0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05). If this situation has been reached after n cycles you can
 * estimate if write/erase endurance of flash is not exceeded.
 */
#else
/**
 * \page pagecookbook VEEPROM Cookbook - or: How to partition data using VEEPROM
 *
 * How to partition data depends on the technology you use. 
 *
 * \section secVEEPROM VEEPROM using FLASH
 * When you use VEEPROM to emulate EEPROM in FLASH, it is a good idea to think twice on how to 
 * store data. Because on this technology we have a special limited program/erase endurance it is even
 * more important to have the partitionened data best to extend lifespan of FLASH.
 *
 *
 *
 *   This section tries to give a recipe on how to cook the partition-table of flash.
 *
 *   \subsection subCookIntro 1. Optimal drawer capacity
 *   Defining size of drawer shall be done with regard to limited endurance of flash.
 *   Thus you should set size of drawers to the "optimal" value.
 *   
 *   Partitioning data to be stored will be different from project to project. Do not
 *   forget that flash can be erased only about 10,000 times!
 * 
 *   \subsection subCookProCons 2. Pros and Cons about large/small drawers
 *   Some points to think about of:
 *   - the larger a drawer is, the more data it can keep with only one time of drawer-overhead.
 *     \dot
        digraph structs {
         node [shape=plaintext, fontsize=14];
         structBigDrawer [shape=record,label="{{<v>CRC|<i>ID1|B0|B1|B2|B3|B4|...|Bn}}"];
         "Verification Field" -> structBigDrawer:v;
         "unique Identifier" -> structBigDrawer:i;
 *     \enddot
 *     \dot
        digraph structs {
         node [shape=plaintext, fontsize=14];
         structSmallDrawer1 [shape=record,label="{{<v>CRC|<i>ID2|B0}}"];
         structSmallDrawer2 [shape=record,label="{{<v>CRC|<i>ID3|B0}}"];
         structSmallDrawer3 [shape=record,label="{{<v>CRC|<i>IDN|B0}}"];

         "Verification Field" -> structSmallDrawer1:v;
         "Verification Field" -> structSmallDrawer2:v;
         "Verification Field" -> structSmallDrawer3:v;

         "unique Identifier" -> structSmallDrawer1:i
         "unique Identifier" -> structSmallDrawer2:i
         "unique Identifier" -> structSmallDrawer3:i
        }
       \enddot


 *   - <b>but:</b> if only one value within this drawer has to be updated, the whole drawer has
 *     to be rewritten at the next free position of flash-page. Thus only try to pack more than one
 *     data in one drawer that changes together.
 *
 *   \subsection subCookRecipe 3. Now the recipe
 *   So a recipe to partition data can be:
 *   - If you can, store "write-once-data" in a separate page (not using this lib) e.g. in program-data.
 *   - Try to have as much "single-data" drawer, as possible. e.g. Integer-Value would have 
 *     drawer-length of 2 (2 * 8Bit = 16 byte = sizeof(int)). 
 *   - If you cannot store "write-one-data in a extra page, put it together in one drawer.
 *   - If there are too less drawers, try to put "write not so often"-data in one drawer.
 *   
 *   \subsection subWarning 4. Warning
 *   And remember: If a drawer contains 10 separated data and you have to update because only one single
 *   datum has changed, the whole drawer must be written once again at the next free position of flash.
 *   This is ineffective, because 9 bytes are written, that must not have been written, if every data
 *   had its own drawer.
 *
 *   But if there are data changing together, you should put them in one drawer acumulated. If you can do
 *   so, you save the additional headers.
 * 
 */

/** 
 * \page pagehowmuchdata How much data is written in a specific period of time?
 *
 *  It is not too easy to estimate how much data has to be written in a specific period of time.
 *  To have an instrument for this, you can have a look at the first two bytes of the active flashPage.
 *  Each time a full flashPage is full and defragmented to next free page, the Page-Erase-Counter is 
 *  incremented by one. 
 * 
 *  But therefore this library must be compiled with option #VEEPROM_FEATURE_PAGE_ERASE_COUNTER defined (which is by default).
 *   \dot
       digraph structs {
         node [shape=plaintext, fontsize=16];
         "Page Erase Counter";
         node [shape=record];  
         
         structFlashPage [shape=record,label="{{<PC1> 00 |<PC2> 05 | now drawers follow ...}}"];
         "Begin of Flash-page" -> structFlashPage:PC1;
         "Page Erase Counter" -> structFlashPage:PC1;
         "Page Erase Counter" -> structFlashPage:PC2;
       }
     \enddot 
 * In the figure above we see an extract of a flash-page. In this example it is the sixth time that a page has 
 * been erased (0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05). If this situation has been reached after n cycles you can
 * estimate if write/erase endurance of flash is not exceeded.
 */
#endif
#endif
#endif
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "nvmem/cacheXS.h"

/** Type to describe possible library cases */
typedef enum enum_NVMEMTaskState
{
   NVMEM_UNINITIALIZED    , ///< not initialized, the funktion VEEPROM_bInit() is to be call
   NVMEM_READY            , ///< basic status, ready to accept read or write commands
   NVMEM_VEEPROM_WRITING  , ///< virtual EEPROM write operation is pending
   NVMEM_EXTEEPROM_WRITING, ///< external EEPROM write operation is pending
   NVMEM_DEFRAGMENTING      ///< erase operation is pending
}eNVMEMtaskStateType;
#ifdef TM_TESTPOINTS_INCLUDED
/** NVMEM Test Points*/
typedef enum _T_NVMEM_TESTPOINTS
{
   /** 'end of init reached' test point */
    NVMEM_TP__INITEND = 1,
   /** 'VEEPROM-Procedures are included with compile switch' test point*/
    NVMEM_TP__VEEPROM_PROC_INCLUDED_IN_INIT,
   /** 'EEPROM-Procedures are included with compile switch' test point*/
    NVMEM_TP__EEPROM_PROC_INCLUDED_IN_INIT,

   /** 'data had to be written to VEEPROM, because new data' test point*/
    NVMEM_TP__VEEPROM_WAS_WRITTEN,
   /** 'data had not to be written to VEEPROM, because equals data in cache' test point*/
    NVMEM_TP__VEEPROM_WAS_UP_TO_DATE,
   /** 'end of write' test point*/
    NVMEM_TP__WRITEEND,

   /** 'end of refresh ram cache' test point*/
    NVMEM_TP__REFRESH_RAM_CACHE_END,
   /** ''chacehd value found' test point*/
    NVMEM_TP__REFRESH_RAM_CACHE__CACHED_VALUE_FOUND,
   /** 'cached value not found' test point*/
    NVMEM_TP__REFRESH_RAM_CACHE__CACHED_VALUE_FOUND_NOT
}tTestPoints
;
#endif

/* ---------------------------------- */
/**
 * \brief Initialization API
 *
 * This function has to be called once before any other function can be called. 
 * It calls lower functions for initializing their states
 *
 * \return
 *   -# TRUE  : Initialization succesfull
 *   -# FALSE : Initialization failed
 * \remarks
 *  This API has following tasks:
 *   -# call appropriate low level path for initialization
 *   -# refresh the RAM cache after each power failure
 *   -# repair interrupted page-transfer if reset while process of defragmentation
 *
 * \test TestCase 1.3.A: initialisation of data cache locations
 * \test TestCase 1.3.B: repair interrupted page transfer
 *
 * \bug Status :\n
 *      - tested

 */
bool NVMEM_bInit( void );
/**
 * \brief Read API
 *
 * function for reading data from the non volatile memory assigned to given cache address in 
 * #CACHE_tPartitionTableVeeprom or #CACHE_tPartitionTableEeprom
 *
 * \param tCachePlace  Index to partition table item
 * \param pucData      Pointer to read data buffer
 * 
 * \return
 *    -# TRUE  : Read succesfull
 *    -# FALSE : Read failed
 *
 * \remarks
 *    This API has following tasks:
 *    -# Fetch size of given data set to read according to PartitionTable
 *    -# Fetch cache place address of given data set
 *    -# Copy content of appropriate cache place into memory given by pucData
 *
 *
 * \bug Status :\n
 *      - tested
 */

bool  NVMEM_bRead( tCacheLocationIdentifier tCachePlace, uint8_t *pucData );
/**
 * \brief Write API
 *
 * function for writing data to the non volatile memory assigned to given cache address in 
 * #CACHE_tPartitionTableVeeprom or #CACHE_tPartitionTableEeprom
 *
 * \param tCachePlace  Enumeration item to specify cache place
 * \param pucData      Pointer to data set to write, read only
 *
 * \return
 *    -# TRUE  : Write successful
 *    -# FALSE : Write failed
 *
 * \remarks
 *    This API has following tasks:
 *    -# Fetch size of given data set to write according to PartitionTable
 *    -# Find out which low level path should do the write operation
 *    -# Call appropriate Low Level routine for the write operation
 * 
 *
 * \test Test Case 1.1:   Writing defined values to all drawers
 * \test Test Case 1.4:   Writing random data to flash-page results in defragmentation of page
 *
 * \bug Status :\n
 *      - tested
 */
bool  NVMEM_bWrite( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData );

#if defined(RX100) || defined(RX200) || defined(CORTEX_M4) || (defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM))
/**
 * \brief Start write API
 *
 * function for start writing data to the non volatile memory assigned to given cache address in 
 * #CACHE_tPartitionTableVeeprom or #CACHE_tPartitionTableEeprom
 *
 * \param tCachePlace  Enumeration item to specify cache place
 * \param pucData      Pointer to data set to write, read only
 *
 * \return
 *    -# TRUE  : Start write successful
 *    -# FALSE : Start write failed
 *
 * \remarks
 *    This API has following tasks:
 *    -# Copy variable bytes into internal cache place
 *    -# Set appropriate flag in flag field
 *    -# All other tasks will be done in task handler
 * 
 */
bool  NVMEM_bWriteStart( tCacheLocationIdentifier tCachePlace, const uint8_t *pucData );

/**
 * \brief NVMEM task handler API
 *
 * \return
 *    -# TASK_INITIALISED
 *    -# TASK_NOT_INITIALISED
 *
 * \remarks
 *    This API has following tasks:
 *    -# Calls library initialisation routine if it is not done yet
 *    -# Check change flags of cache places and starts write processes
 *    -# Check free memory space and starts defragmentation process
 * 
 *    This interface must be called only if NVMEM_bWriteStart() function is used.\n
 *    Other API functions do not need to call task handler.
 *    
 */
uint8_t NVMEM_ucHandleTask(void);
/**
 * \brief Write API
 *
 * Function writes all values which were already changed in cache places into flash page or external EEPROM.\n
 * So called 'dirty flags' are observed and only such variables are saved of which flag were set.
 *
 * \return
 *    -# TRUE  : function successful
 *    -# FALSE : function failed
 *
 * \remarks
 *    At function successful end all cached variables are saved.
 *
 */
bool NVMEM_bFlushBuffer(void);
/**
 * \brief Gets number of unsaved cache variables
 *
 *
 * \return  count of unsaved cache variables
 *
 *
 */
uint16_t NVMEM_uiGetUnsavedCount(void);
/**
 * \brief Gets library current status
 *
 *
 * \return  
 *   NVMEM_UNINITIALIZED
 *   NVMEM_READY
 *   NVMEM_VEEPROM_WRITING
 *   NVMEM_EXTEEPROM_WRITING
 *   NVMEM_DEFRAGMENTING
 *
 *
 */
eNVMEMtaskStateType NVMEM_tGetStatus(void);
/**
 * \brief Save process pending check function
 *
 * function checks if variable in given cache place has new value and is waiting for save
 *
 *
 * \param tCachePlace  Enumeration item to specify cache place
 *
 * \return
 *    -# Other than FALSE  : Save process pending
 *    -# FALSE             : No save process pending
 *
 * \remarks
 *     This function is usable only after NVMEM_bWriteStart() was used
 * 
 */
bool NVMEM_bIsSavePending( tCacheLocationIdentifier tCachePlace );
/**
 * \brief Sets data flush behavior
 *
 * This function controls behavior of NVMEM_bFlushBuffer() function :
 * -# bEnabled = TRUE   : causes that NVMEM_bFlushBuffer() does data defragmentation if necessary 
 * -# bEnabled = FALSE  : NVMEM_bFlushBuffer() returns FALSE if there are not enough space on the memory page.\n
 *                      Some of cache variables can stay unsaved.
 * \param bEnabled  new status to set
 *    none
 * \return  none
 *
 * \remarks
 *    This function has no influence on external EEPROM, it does not use defragmentation.
 *
 */
void NVMEM_vFlushDefragEnabled( bool bEnabled );
#endif

#ifdef __cplusplus
}
#endif

#endif
