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
#ifndef VEEPROM_H
#define VEEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/ 
 /** \file VEEPROM.h
    Interfaces for the handling of Virtual EEPROM.
*/

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#if defined(VEEPROM_LARGE)
#include "VEEPROMmcXS.h"
#endif
/*lint -save -e9026 Function-like macro defined */

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/** Type to describe possible library cases */
typedef enum enum_VEEPROMTaskState
{
   VEEP_UNINITIALIZED, ///< not initialized, the funktion VEEPROM_bInit() is to be call
   VEEP_READY        , ///< basic status, ready to accept read or write commands
   VEEP_WRITING      , ///< write operation is pending
   VEEP_DEFRAGMENTING  ///< erase operation is pending
}eVEEPROMtaskStateType;

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/* features*/
/**
 * \def VEEPROM_FEATURE_PAGE_ERASE_COUNTER
 * \brief Feature: if defined, VEEPROM_PAGE_ERASE_COUNTER_LEN bytes are reserved for counting number of performed flashPage switches
 *
 * This makes it easy to evaluate how much data is stored in a specific period of time. But
 * remember that the first two bytes of the flash-pages are incrementet at every defragmentation.
 * \note If we have a controller with two flash-pages, and the counter counts 10,000 this means that
 * endurance has not yet been reached, because we only performed 5,000 page-erases per page.
 */
#define VEEPROM_FEATURE_PAGE_ERASE_COUNTER
/* comment for compile switch when feature is not defined*/
#ifndef VEEPROM_FEATURE_PAGE_ERASE_COUNTER
/**
 * \addtogroup features
This line of comment is not seen!
  @{
   <hr>
  VEEPROM_FEATURE_PAGE_ERASE_COUNTER<br>

  Feature: if you want to evaluate how much data is written to VEEPROM in a specific period of
          time (with regard to endurance of FLASH!), you can define this compile switch.<br>
          As an effect of this the first two bytes of FLASH page will contain a page switch-counter.
          As a result of this two bytes of flash for storing data will be lost, so this feature can
          be disabled for release-code.<br>
          By default this feature <b>is set</b>! 
  <hr> 
  @}
 */
#endif
   
/**
 * \addtogroup features
</dl>
 */
   
/**
 * \def VEEPROM_MAX_DATA_LEN
 * \brief max data length of drawer
 *
 * \todo define optimal value for this.
 */
#if defined(STM32G0) || defined(STM32G4) || defined(STM32L4) || defined(MKV5X) || defined(MSPM0)
#define VEEPROM_MAX_DATA_LEN (36U)
#else
#define VEEPROM_MAX_DATA_LEN (31U)
#endif
    
/**
 * \def VEEPROM_CRC_VALIDATION_USED
 * \brief Feature: if defined, verification field contains calculated crc-8 over drawer-label and payload
 * 
 * Pro: 
 *      - This increases reliabilty of stored data enormeously, 
 * Cons: 
 *      - but decreases access time 
 *      - enlages the lib (+64 bytes)
 */
#define VEEPROM_CRC_VALIDATION_USED
#ifdef ASSERT_INCLUDED 
/**
 * to get rid of:  Msg(5:6013) Name of function-like macro 'ASSERT_EXPR' is not correct. 
 */
#define ASSERT_vEXPR(param) ASSERT_EXPR((param))
#else
//! \todo remove the following definition and find out why ASSERT_EXPR is not expanded to ";"
/** \brief we don't have assertions in Release Built*/
#define ASSERT_vEXPR(param) do{}while(0)
#endif
   
/* Public declarations*/
/* bool  VEEPROM_bInit                         ( void )*/

/* doxygen note*/
/**
 * \brief VEEPROM Initialization API
 *
 * \return
 *    -# true  : Initialization successful
 *    -# false : Initialization failed
 *
 * \remarks
 *    This API has following tasks:
 *    -# Set active flash page at system startup if not yet done
 *    -# Check integrity of active flash page if already set
 *    -# Set write pointer to valid cabinet location
 *    -# Check and do data recovery if needed
 *    -# Update the data-cache for speeding up access is done in upper layer NVMEM_bInit()
 * 
 *    This interface must be called at system startup before any other interface is called.\n
 *    In case the startup is caused by a power failure, this interface checks the active flash
 *    page and sets up a valid flash environment.
 * \image html VEEPROM_bInit.emf "flowchart of VEEPROM_bInit()"
 * \image latex VEEPROM_bInit.eps "flowchart of VEEPROM_bInit()"
 */
extern bool  VEEPROM_bInit                         ( void );
   
/* bool  VEEPROM_bWrite                        ( uint8_t        ucLabel,*/
/*                                               const uint8_t *pucData,*/
/*                                               uint8_t        ucDataLen )*/

/* doxygen comment*/
/**
 * \brief VEEPROM Write API
 * 
 * \param ucLabel   - Label of data set
 * \param pucData   - Pointer to data set to write
 * \param ucDataLen - Byte size of data to write
 * \return
 *    -# true  : Write successful
 *    -# false : Write failed. 
 * 
 * \note false can be returned when e.g. full flash-page was not allowed to be defragmented because 
 *   VEEPROM_bApprobateDefrag() returned false and thus did not allow to start defragmentation. In this case
 *   <B>cached value</B> is <B>updated</B>. <B>But value in flash</B>  is not up to date. To synchronize data of  
 *   cache and flash defragment flash manually or write an other data and allow to defragment flash.
 *
 * \remarks
 *    This API has following tasks:
 *    -# Check if data is new in case there is no upper layer
 *    -# Write data in valid drawer in case data is new or modified
 *    -# Set crc as data valid flag at the end of a successful write
 *    -# Transfer data to transfer page when active page is full (start process of defragmentation)
 *    -# Set valid active page at run time.
 * 
 * 
 *    This interface must be called ONLY after call of initialization API.\n
 *    This interface is blocking and returns only on write completion or failure.
 * 
 *   \callgraph
 * 
 * \image html VEEPROM_bWrite.emf "flowchart of VEEPROM_bWrite()"
 * \image latex VEEPROM_bWrite.eps "flowchart of VEEPROM_bWrite()"
 */
   
#if defined(VEEPROM_LARGE)
extern bool VEEPROM_bWrite ( tDrawerID tLabel, const uint8_t *pucData, uint8_t ucDataLen );
#else
extern bool  VEEPROM_bWrite( uint8_t ucLabel, const uint8_t* pucData, uint8_t ucDataLen );
#endif //defined(VEEPROM_LARGE)

   
/* uint8_t VEEPROM_ucRead                        ( uint8_t  ucLabel,*/
/*                                                 uint8_t* pucData,*/
/*                                                 uint8_t  ucDataLen )*/
/**
 * \brief VEEPROM Read API
 * 
 * \param ucLabel   - Label of data set
 * \param pucData   - Pointer to data set to read
 * \param ucDataLen - Byte size of data to read
 * \return
 *    -# !0  : Number of data bytes read
 *    -# 0   : Error while reading
 * \remarks
 *    This API has following tasks:
 *    -# Retrieve lates set of data stored with a specific label into VEEPROM
 *    -# Refresh cache buffer at startup if application reads data from cache
 * 
 *    This interface must be called ONLY after call of initialization API.\n
 *    This interface is blocking and returns only on read completion or failure.
 * 
 *   \callgraph
 * 
 * \image html VEEPROM_ucGetDataPayloadFromPage.emf "flowchart of VEEPROM_ucRead()"
 * \image latex VEEPROM_ucGetDataPayloadFromPage.eps "flowchart of VEEPROM_ucRead()"
 */
#if defined(VEEPROM_LARGE)
extern uint8_t VEEPROM_ucRead ( tDrawerID tLabel, uint8_t* pucData, uint8_t ucDataLen );
#else
extern uint8_t VEEPROM_ucRead ( uint8_t ucLabel, uint8_t *pucData, uint8_t ucDataLen );
#endif //defined(VEEPROM_LARGE)
   
/* uint16_t  VEEPROM_uiGetFreeSpace                ( void )*/

/* doxygen comment*/
/**
 * \brief return free available space of active FLASH-Page in use
 *
 * This function can be called from application to check (before time-critical
 * passages in code) if enough free space is available to perform write operations.
 *
 * \return number of bytes free in active page
 * \image html VEEPROM_uiGetFreeSpace.emf "flowchart of VEEPROM_uiGetFreeSpace()"
 * \image latex VEEPROM_uiGetFreeSpace.eps "flowchart of VEEPROM_uiGetFreeSpace()"
 */
   
extern uint16_t  VEEPROM_uiGetFreeSpace                ( void );
   
/* bool  VEEPROM_bDoDefragNow                  ( void )*/

/* doxygen comment*/
/**
 * \brief start process of defragmentation
 * 
 * This process is performed automatically from application when Write-operation
 * tries to write to active page and page has too less free cells.
 * 
 * Application can start process of defragmentation e.g. if free space left on
 * active page is very small ( checked by calling function VEEPROM_uiGetFreeSpace() )
 * and it will ensure that a following write-process will not start a process of
 * defragmentation. This can be wished, if the write-process is started in a time-critical
 * section of application where Interrupts shall not be locked by process of defragmentation.
 *
 * \return if defragmentation succeeded: true / false
 * \image html VEEPROM_bDoDefragNow.emf "flowchart of VEEPROM_bDoDefragNow()"
 * \image latex VEEPROM_bDoDefragNow.eps "flowchart of VEEPROM_bDoDefragNow()"
 */

extern bool VEEPROM_bDoDefragNow                  ( void );
   
/* bool  VEEPROM_bReadWholeActivePage          ( void )*/
/**
 * \brief fill cache variables values
 *
 * This function can be called from application which uses cache variables
 * to quick reading of their values from active flash page.
 *
 * \return
 *    -# true  : Read successful
 *    -# false : Read failed. 
 */
extern bool VEEPROM_bReadWholeActivePage( void );
   
#if defined(RX100) || defined(RX200) || defined(STM32F401) || defined(CORTEX_M4)
#if defined(VEEPROM_LARGE)
/* bool VEEPROM_bWriteStart()*/
/* doxygen comment*/
/**
 * \brief Starts write process
 * 
 * \param tLabel    - Label of data set
 * \param pucData   - Pointer to data set to write
 * \param ucDataLen - Byte size of data to write
 * \return
 *    -# true  : Write start successful
 *    -# false : Write start failed
 * 
 * \note false can be returned when :
 *   - another write/defrag process is pending
 *   - full flash-page was not allowed to be defragmented because VEEPROM_bApprobateDefrag() returned false
 *     and thus did not allow to start defragmentation. In this case <B>cached value</B> is <B>updated</B>.
 *     <B>But value in flash</B>  is not up to date. To synchronize data of cache and flash defragment flash
 *     manually or write an other data and allow to defragment flash.
 *
 * \remarks
 *    This API only starts write process. Rest of task does its task handler.
 * 
 *    This interface must be called ONLY after call of initialization API.\n
 *    This interface is not blocking.
 * 
 */
   
bool  VEEPROM_bWriteStart( uint16 tLabel, const uint8_t *pucData, uint8_t ucDataLen );
   
#else // not defined(VEEPROM_LARGE)
/* bool  VEEPROM_bWriteStart()*/
/* doxygen comment*/
/**
 * \brief Starts write process
 * 
 * \param ucLabel   - Label of data set
 * \param pucData   - Pointer to data set to write
 * \param ucDataLen - Byte size of data to write
 * \return
 *    -# true  : Write start successful
 *    -# false : Write start failed
 * 
 * \note false can be returned when :
 *   - another write/defrag process is pending
 *   - full flash-page was not allowed to be defragmented because VEEPROM_bApprobateDefrag() returned false
 *     and thus did not allow to start defragmentation. In this case <B>cached value</B> is <B>updated</B>.
 *     <B>But value in flash</B>  is not up to date. To synchronize data of cache and flash defragment flash
 *     manually or write an other data and allow to defragment flash.
 *
 * \remarks
 *    This API only starts write process. Rest of task does its task handler.
 * 
 *    This interface must be called ONLY after call of initialization API.\n
 *    This interface is not blocking.
 * 
 */
   
bool  VEEPROM_bWriteStart( uint8_t ucLabel, const uint8_t *pucData, uint8_t ucDataLen );
   
#endif //defined(VEEPROM_LARGE)
/* bool  VEEPROM_bDefragStart                  ( void )*/
/**
 * \brief start process of defragmentation
 * 
 * This process is performed automatically from application when WriteStart-operation
 * tries to write to active page and page has too less free cells.
 * 
 * Application can start process of defragmentation e.g. if free space left on
 * active page is very small ( checked by calling function VEEPROM_uiGetFreeSpace() )
 * and it will ensure that a following write-process will not start a process of
 * defragmentation. This can be wished, if the write-process is started in a time-critical
 * section of application where Interrupts shall not be locked by process of defragmentation.
 *
 * \return if defragmentation start succeeded: true / false
 */
    
bool VEEPROM_bDefragStart(void);
   
/* uint8_t VEEPROM_ucHandleTask                  ( void )*/
/**
 * \brief VEEPROM task handler API
 *
 * \return
 *    -# TASK_INITIALISED
 *    -# TASK_NOT_INITIALISED
 *
 * \remarks
 *    This API has following tasks:
 *    -# Set active flash page at system startup if not yet done
 *    -# Check integrity of active flash page if already set
 *    -# Set write pointer to valid cabinet location
 *    -# Check and do data recovery if needed
 *    -# Update the data-cache for speeding up access is done in upper layer NVMEM_bInit()
 * 
 *    This interface must be called at system startup before any other interface is called.\n
 *    In case the startup is caused by a power failure, this interface checks the active flash
 *    page and sets up a valid flash environment.
 */
    
uint8_t VEEPROM_ucHandleTask(void);
   
/* eVEEPROMtaskStateType VEEPROM_tGetStatus    ( void )*/
/**
 * \brief Gets library current status
 *
 * \return
 *    -# VEEP_UNINITIALIZED
 *    -# VEEP_READY
 *    -# VEEP_WRITING
 *    -# VEEP_DEFRAGMENTING
 * 
 *    You can use this interface to determine if other API functions can be called.\n
 */
    
eVEEPROMtaskStateType VEEPROM_tGetStatus(void);
   
#endif //defined(RX100) || defined(RX200) || defined(STM32F401) || defined(CORTEX_M4)

#ifdef __cplusplus
}
#endif
/*lint -restore -e9026 */
#endif //VEEPROM_H
