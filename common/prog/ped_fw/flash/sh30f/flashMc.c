/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          FLASH_LIB
 ******************************************************************************/

/** \file     flashMc.c
 *
 *  \brief    Implementation of processor specific flash routines for SH30F9X71.
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "libtypes.h"
#include "segmentdef.h"
#include "utility/utility.h"
#include "processor.h"
#include "FlashMcXS.h"
#include "Flash.h"
#include "FlashMc.h"
#include "hsup.h"
#if defined(DEBUG) && (DEBUG != 0U)
#include "pedAssert.h"
#endif

//lint -e818  Pointer parameter could be declared as pointing to const
//lint -e661 potential out of bounds pointer access

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC
#define FLASH_INVALID_VALUE             (0xFFFFu)
#define BYTES_WRITTEN_AT_ONCE           (2u)
#define FLASH_DATA_BUFFER_SIZE          BYTES_WRITTEN_AT_ONCE
#define UNLOCK_PARAMS_COUNT             (3u)
#define ACR_UNLOCK_KEY                  ((uint32_t)0x5AA5u)
#define CLEAR_BITS_POSITION             (16u)
#define FLASH_ERROR_FLAGS               (FLASH_FLAG_OPERR | FLASH_FLAG_FLSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGWERR | FLASH_FLAG_STAERR)
#define INVALID_ADDRESS                 (0xFFFFFFFFuL)
#define EEPROM_LIKE_MEMORY_END          (0x0FFC0FFFuL)
#define PROTECT_MEMORY_END              (0x0FFF83FFuL)
#define CUSTOMER_MEMORY_END             (0x0FFFA3FFuL)
#define OTP_MEMORY_END                  (0x0FFFE3FFuL)
#define MAIN_AREA_INDEX                 (0u)
#define EEPROM_LIKE_AREA_INDEX          (1u)
#define SPECIAL_AREA_INDEX              (2u)
#define UNKNOWN_AREA_INDEX              (255u)

#define FLASH_MEM_E2_KEY                ((uint32_t)0x9BDF1357u)
#define FLASH_MEM_INFO_KEY              ((uint32_t)0xABCD5678u)
#endif

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

struct unlockParams
{
    volatile uint32_t *unlockReg;
    uint32_t unlockKey;
    uint32_t mask;
};
/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                    */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC
static bool lowVoltageResetEnabled = false;

static const struct unlockParams FLASH_unlockParams[UNLOCK_PARAMS_COUNT] =
{
    { .unlockReg = &FLASH->MKYR,  .unlockKey = FLASH_MAIN_KEY,     .mask = FLASH_CR_MNLCK_Msk  },
    { .unlockReg = &FLASH->E2KYR, .unlockKey = FLASH_MEM_E2_KEY,   .mask = FLASH_CR_E2LCK_Msk  },
    { .unlockReg = &FLASH->IKYR,  .unlockKey = FLASH_MEM_INFO_KEY, .mask = FLASH_CR_INFLCK_Msk }
};

/* PROTOTYPES */
static bool FLASH_isMainMemory(const tAddress *address,uint16_t size);
static bool FLASH_isEEPROMlikeMemory(const tAddress *address,uint16_t size);
static bool FLASH_isProtectMemory(const tAddress *address,uint16_t size);
static bool FLASH_isCustomerMemory(const tAddress *address,uint16_t size);
static bool FLASH_isOTPmemory(const tAddress *address,uint16_t size);
static uint16_t FLASH_getWriteProtectInfo(tAddress *address);
static bool FLASH_checkStatus(void);
static bool FLASH_write16bits(uint16_t *address, uint16_t *data);
static bool FLASH_writeKernel(tAddress *address, tWriteData *data, uint16_t size);
static void FLASH_cacheReset(void);
static void FLASH_setPGMWindow(uint32_t CounterInitValue, uint32_t UpperCounterValue);
static void FLASH_clearPGMWindow(void);
static bool FLASH_unlock(uint8_t areaIndex);
#endif
/* PROTOTYPES */

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC

static bool FLASH_isMainMemory(const tAddress *address,uint16_t size)
{
    /* there is a various derivatives depending count of memory blocks */
    return ( ((uint32_t)address >= FLASH_getStartAddress(0)) && ((((uint32_t)address + (uint32_t)size) - 1UL) <= FLASH_getEndAddress(FLASH_blockCount - 1U)) );
}

static bool FLASH_isEEPROMlikeMemory(const tAddress *address,uint16_t size)
{

    return ( ((uint32_t)address >= (uint32_t)E2_BLOCK_ADDRESS) && ((((uint32_t)address + (uint32_t)size) - 1UL) <= EEPROM_LIKE_MEMORY_END) );
}

static bool FLASH_isProtectMemory(const tAddress *address,uint16_t size)
{

    return ( ((uint32_t)address >= (uint32_t)PROTECT_BLOCK_ADDRESS) && ((((uint32_t)address + (uint32_t)size) - 1UL) <= PROTECT_MEMORY_END) );
}

static bool FLASH_isCustomerMemory(const tAddress *address,uint16_t size)
{

    return ( ((uint32_t)address >= (uint32_t)CUSTOMER_BLOCK_ADDRESS) && ((((uint32_t)address + (uint32_t)size) - 1UL) <= CUSTOMER_MEMORY_END) );
}

static bool FLASH_isOTPmemory(const tAddress *address,uint16_t size)
{

    return ( ((uint32_t)address >= (uint32_t)OTP_BLOCK_ADDRESS) && ((((uint32_t)address + (uint32_t)size) - 1UL) <= OTP_MEMORY_END) );
}

static uint16_t FLASH_getWriteProtectInfo(tAddress *address)
{
    uint16_t counter;
    uint16_t bitNr = FLASH_INVALID_VALUE;

    for ( counter = 0; counter < FLASH_blockCount; counter++ )
    {
        if ( (FLASH_getStartAddress(counter) <= (uint32_t)address) && ((uint32_t)address <= FLASH_getEndAddress(counter)) )
        {
            bitNr = counter / FLASH_protectedPagesPerPin;
            break; //from cycle for
        }
    }
    return(bitNr);
}

/** \brief    FLASH_checkStatus()
 *
 *  \details  Polls the flash ready bit within given time.
 *            In case timeout is reached, error is issued.
 *  \remarks
 *            The timeout values are calibrated with respect to flash
 *            specification (write/erase times)
 */
SDEF_SetSegmentCode(FLASH_ROUTINES)
static bool FLASH_checkStatus(void)
{
    uint32_t timeOut = 0x00FFFFFFuL;
    bool retVal = false;
    while ( (UTI_TEST(FLASH->SR.V32,FLASH_FLAG_BSY) != 0uL) && (timeOut > 0uL) )
    {
        --timeOut;
    }
    if ((timeOut == 0UL)  || ( UTI_TEST(FLASH->SR.V32,FLASH_FLAG_FLSERR) != 0uL ))
    {
        /* reset flag */
        UTI_SET(FLASH->SR.V32,(FLASH_FLAG_FLSERR << CLEAR_BITS_POSITION));
        FLASH_tFunctionStatus = MEMORY_PROCESS_TIMEOUT;
        /* timeout */
#if defined(DEBUG) && (DEBUG != 0U)
        _ASSERT_vOnFailure ((const uint8_t*)((void *)(__FILE__)), __LINE__);
#endif
    }
    else if( UTI_TEST(FLASH->SR.V32,FLASH_FLAG_OPERR) != 0uL )
    {
        /* common error flag is set */
        if(UTI_TEST(FLASH->SR.V32,FLASH_FLAG_WRPERR) != 0uL)
        {
            /* reset flag */
            UTI_SET(FLASH->SR.V32,(FLASH_FLAG_WRPERR << CLEAR_BITS_POSITION));
            FLASH_tFunctionStatus = PROTECTION_ERROR;
#if defined(DEBUG) && (DEBUG != 0U)
        _ASSERT_vOnFailure ((const uint8_t*)((void *)(__FILE__)), __LINE__);
#endif
        }
        if(UTI_TEST(FLASH->SR.V32,FLASH_FLAG_PGPERR) != 0uL)
        {
            /* reset flag */
            UTI_SET(FLASH->SR.V32,(FLASH_FLAG_PGPERR << CLEAR_BITS_POSITION));
            FLASH_tFunctionStatus = WRITE_ERROR;
#if defined(DEBUG) && (DEBUG != 0U)
        _ASSERT_vOnFailure ((const uint8_t*)((void *)(__FILE__)), __LINE__);
#endif
        }
        if(UTI_TEST(FLASH->SR.V32,FLASH_FLAG_PGWERR) != 0uL)
        {
            /* reset flag */
            UTI_SET(FLASH->SR.V32,(FLASH_FLAG_PGWERR << CLEAR_BITS_POSITION));
            FLASH_tFunctionStatus = TIME_WINDOW_ERROR;
#if defined(DEBUG) && (DEBUG != 0U)
        _ASSERT_vOnFailure ((const uint8_t*)((void *)(__FILE__)), __LINE__);
#endif
        }
        if(UTI_TEST(FLASH->SR.V32,FLASH_FLAG_STAERR) != 0uL)
        {
            /* reset flag */
            UTI_SET(FLASH->SR.V32,(FLASH_FLAG_STAERR << CLEAR_BITS_POSITION));
            FLASH_tFunctionStatus = FLASH_STATUS_ERROR;
#if defined(DEBUG) && (DEBUG != 0U)
        _ASSERT_vOnFailure ((const uint8_t*)((void *)(__FILE__)), __LINE__);
#endif
        }
        /* reset error flags */
        UTI_SET(FLASH->SR.V32,(uint32_t)(FLASH_ERROR_FLAGS << CLEAR_BITS_POSITION));
    }
    else
    {
        /* process finished successfully */
        retVal = true;
    }
    return(retVal);
}
SDEF_SetSegmentCode_Default()

/** \brief Part of FLASH_bWrite() function
\param   address - flash start address for writing
\param   data - byte buffer containing data to write, read only
\param   size - number of bytes to write
\return
   #true    - operation successful \n
   #false   - operation failed
 *
 */
static bool FLASH_writeKernel(tAddress *address, tWriteData *data, uint16_t size)
{
    static uint8_t FLASH_dataBuffer[FLASH_DATA_BUFFER_SIZE] __attribute__((aligned (BYTES_WRITTEN_AT_ONCE)));
    static const uint8_t FLASH_unwritten = 0x00;
    uint8_t  firstBytesCnt    ; //number of first bytes, which must be filled up with default bytes, quad word (64 bit) must be created
    uint8_t  lastBytesCnt     ; //number of last bytes, which must be filled up with default bytes, quad word (64 bit) must be created
    uint16_t numberOfRecords  ; //record contains 8 bytes
    uint8_t  remainder        ;
    uint16_t recordCounter    ;
    uint8_t  byteCounter      ;

    remainder = (uint8_t)((uint32_t)(address) % BYTES_WRITTEN_AT_ONCE);
    if ( remainder > 0U ) // is address divisible by 8?
    {
        firstBytesCnt = BYTES_WRITTEN_AT_ONCE - remainder;
    }
    else
    {
        firstBytesCnt = 0;
    }
    if ( size > (uint16_t)firstBytesCnt )
    {
        numberOfRecords = (uint16_t)((uint16_t)(size - (uint16_t)firstBytesCnt) / BYTES_WRITTEN_AT_ONCE);
        lastBytesCnt  = (uint8_t)((uint16_t)(size - (uint16_t)firstBytesCnt) % BYTES_WRITTEN_AT_ONCE);
    }
    else
    {
        firstBytesCnt = (uint8_t)size;
        numberOfRecords = 0;
        lastBytesCnt  = 0;
    }
    if ( firstBytesCnt > 0U )
    {
        for (byteCounter = 0; byteCounter < BYTES_WRITTEN_AT_ONCE; byteCounter++)
        {
            if ( (byteCounter < remainder) || (firstBytesCnt == 0U) )
            {
                FLASH_dataBuffer[byteCounter] = FLASH_unwritten;
            }
            else
            {
                FLASH_dataBuffer[byteCounter] = *data;
                ++data;
                --firstBytesCnt;
            }
        }
        address -= remainder;
        if ( FLASH_write16bits((uint16_t *)(void *)address, (uint16_t *)(void *)FLASH_dataBuffer) != false )
        {
            address += BYTES_WRITTEN_AT_ONCE;
        }
        else
        {
            return(false);
        }
    }
    if ( numberOfRecords > 0U )
    {
        for ( recordCounter = 0; recordCounter < numberOfRecords; recordCounter++ )
        {
            for (byteCounter = 0; byteCounter < BYTES_WRITTEN_AT_ONCE; byteCounter++)
            {
                FLASH_dataBuffer[byteCounter] = *data;
                data++;
            }
            if ( FLASH_write16bits((uint16_t *)(void *)address, (uint16_t *)(void *)FLASH_dataBuffer) != false )
            {
                address += BYTES_WRITTEN_AT_ONCE;
            }
            else
            {
                return(false);
            }
        }
    }
    if ( lastBytesCnt > 0U )
    {
        for (byteCounter = 0; byteCounter < BYTES_WRITTEN_AT_ONCE; byteCounter++)
        {
            if ( byteCounter < lastBytesCnt )
            {
                FLASH_dataBuffer[byteCounter] = *data;
                data++;
            }
            else
            {
                FLASH_dataBuffer[byteCounter] = FLASH_unwritten;
            }
        }
        if ( FLASH_write16bits((uint16_t *)(void *)address, (uint16_t *)(void *)FLASH_dataBuffer) == false )
        {
            return(false);
        }
    }
    return(true);
}

static bool FLASH_write16bits(uint16_t *address, uint16_t *data)
{
    bool retVal= false;
    if ( ((uint32_t)address % BYTES_WRITTEN_AT_ONCE) == 0U )
    {
        FLASH_setPGMWindow(SE_ERS_TIME, SE_ERS_TIME);
        /* set start flag */
        UTI_SET(FLASH->CR.V32,FLASH_CR_STRT_Msk);

        *(address) = *(data);
        retVal = FLASH_checkStatus();
        FLASH_clearPGMWindow();
    }
    else
    {
        /* assert */
        /* address not aligned */
#if (DEBUG != 0U)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
        FLASH_tFunctionStatus = ALIGNMENT_ERROR;
    }
    return retVal;
}

static void FLASH_cacheReset(void)
{
    uint32_t tmpreg = FLASH->ACR.V32;
    /* Set CRST bit to Reset Cache  */
    FLASH->ACR.V32 |= ((ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos) | ((uint32_t)0x01U << FLASH_ACR_CRST_Pos));
    FLASH->ACR.V32 = (tmpreg | (ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos));
}

SDEF_SetSegmentCode(FLASH_ROUTINES)
static void FLASH_setPGMWindow(uint32_t CounterInitValue, uint32_t UpperCounterValue)
{
    /* Sets the initial value of counter */
    FLASH->CNTR = CounterInitValue;
    /* Sets the upper limit value of counter */
    FLASH->UPCNTR = UpperCounterValue;
    /* Enable the downcounter */
    FLASH->CNTCR = 0x01U;
}
SDEF_SetSegmentCode_Default()

SDEF_SetSegmentCode(FLASH_ROUTINES)
static void FLASH_clearPGMWindow(void)
{
    /* Clears the window counters */
    FLASH->CNTR = 0x0U;
    FLASH->UPCNTR = 0x0U;
    FLASH->CNTCR = 0x0U;
}
SDEF_SetSegmentCode_Default()

static bool FLASH_unlock(uint8_t areaIndex)
{
    uint32_t timeOut = 0x00000FFFu;
    bool retVal = true;

    /* Test if current area is locked */
    if (UTI_TEST(FLASH->CR.V32, FLASH_unlockParams[areaIndex].mask) != 0uL)
    {
        /* Authorize the access */
        *FLASH_unlockParams[areaIndex].unlockReg = FLASH_unlockParams[areaIndex].unlockKey;
        /* The multiple unlock key unlocks the flash interface for write or erase operations until the block is locked. */
        *FLASH_unlockParams[areaIndex].unlockReg = FLASH_MULTI_OP_KEY;
        while ( (timeOut > 0uL) && (UTI_TEST(FLASH->CR.V32, FLASH_unlockParams[areaIndex].mask) != 0uL) )
        {
            --timeOut;
        }
        if ( UTI_TEST(FLASH->CR.V32, FLASH_unlockParams[areaIndex].mask) != 0uL )
        {
            /* flash memory is still locked */
            return false;
        }
    }
    return retVal;
}

#endif

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC
/* functions called from flash.c */

bool FLASH_bEnableAccessMc(void)
{
    bool retVal = true;

    lowVoltageResetEnabled = (UTI_TEST(SYSCFG->PWRCR.V32,SYSCFG_PWRCR_LVREN_Msk) != 0u);
    /* disable low voltage reset function */
    UTI_RES(SYSCFG->PWRCR.V32,SYSCFG_PWRCR_LVREN_Msk);

    /* Clear all possible errors flags */
    FLASH->SR.V32 |= ((uint32_t)(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_FLSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGWERR | FLASH_FLAG_STAERR) << CLEAR_BITS_POSITION);

    /* all control register are set to 0 */
    FLASH->CR.V32 = 0U;

    return retVal;
}

bool FLASH_bDisableAccessMc(void)
{
    if (lowVoltageResetEnabled)
    {
        /* enable low voltage reset function */
        SYSCFG->PWRCR.V32 |= SYSCFG_PWRCR_LVREN_Msk;
    }
    return true;
}

bool FLASH_bWriteMc(tAddress *address, tWriteData *data, uint16_t size)
{
    bool retVal             = false;
    uint32_t flashCommand;
    uint8_t areaIndex;

    if(FLASH_isMainMemory(address, size) != false)
    {
        flashCommand = FLASH_CR_CMD_MPG;
        areaIndex = MAIN_AREA_INDEX;
    }
    else if(FLASH_isEEPROMlikeMemory(address, size) != false)
    {
        flashCommand = FLASH_CR_CMD_E2PG;
        areaIndex = EEPROM_LIKE_AREA_INDEX;
    }
    else if(FLASH_isCustomerMemory(address, size) != false)
    {
        flashCommand = FLASH_CR_CMD_CBPG;
        areaIndex = SPECIAL_AREA_INDEX;
    }
    else if(FLASH_isProtectMemory(address, size) != false)
    {
        flashCommand = FLASH_CR_CMD_SPG;
        areaIndex = SPECIAL_AREA_INDEX;
    }
    else if(FLASH_isOTPmemory(address, size) != false)
    {
        flashCommand = FLASH_CR_CMD_OPG;
        areaIndex = SPECIAL_AREA_INDEX;
    }
    else
    {
        flashCommand = 0uL;
        areaIndex = UNKNOWN_AREA_INDEX;
    }
    if(areaIndex != UNKNOWN_AREA_INDEX)
    {
        _FLASH_resetWatchdogTimer();
        HSUP_vDisableInt();
        if(FLASH_unlock(areaIndex) == false)
        {
            /* unlock failed */
            FLASH_tFunctionStatus = MEMORY_WRITE_LOCKED;
            HSUP_vEnableInt();
            return false;
        }

        if ( UTI_TEST(FLASH->SR.V32,FLASH_SR_BSY_Msk) == 0uL )
        {
            uint32_t tmpVal = FLASH->ACR.V32;
            //disable the pre read
            FLASH->ACR.V32 = ((ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos) | (tmpVal & FLASH_ACR_LATENCY_Msk));

            /* set write mode, programming command type */
            FLASH->CR.V32 = (uint32_t)((uint32_t)(FLASH_CR_PSIZE_HALFWD << FLASH_CR_PSIZE_Pos) | (uint32_t)(flashCommand << FLASH_CR_CMD_Pos));

            retVal = FLASH_writeKernel(address, data, size);

            //enable the pre read
            tmpVal = ((ACR_UNLOCK_KEY << FLASH_ACR_LOCK_Pos) | tmpVal);
            FLASH->ACR.V32 = tmpVal;

            FLASH_cacheReset();
        }
        else
        {
            /* some flash operation is in progress */
            FLASH_tFunctionStatus = PREVIOUS_PROCESS_INCOMPLETE;
        }

        /* Set the LOCK bit to lock the FLASH register access */
        UTI_SET(FLASH->CR.V32, FLASH_unlockParams[areaIndex].mask);
        HSUP_vEnableInt();
    }
    else
    {
        /* address out of range */
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }

    return retVal;
}

bool FLASH_bWriteByteMc(tAddress *address, uint8_t data)
{
    uint8_t au8Data[2] = {0x00, 0x00};
    bool retVal;

    if ( ((uint32_t)address % 2U) == 0UL )
    {
        au8Data[0] =  data;
    }
    else
    {
        address--;
        au8Data[1] =  data;
    }

    retVal = FLASH_bWriteMc(address, au8Data, (uint16_t) sizeof(au8Data));
    return retVal;
}

bool FLASH_bReadMc(tAddress *address, tReadData *data, uint16_t size)
{
    uint16_t counter;
    bool retVal = false;

    if ( (FLASH_isMainMemory(address, size) != false) || (FLASH_isEEPROMlikeMemory(address, size) != false) )
    {
        for (counter = 0; counter < size; counter++)
        {
            data[counter] = address[counter];
        }
        retVal = true;
    }
    else
    {
        /* address out of range */
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }
    return retVal;
}

bool FLASH_bEraseMc(tAddress *address)
{
    bool retVal = false;
    uint32_t flashCommand;
    uint8_t areaIndex;

    if (FLASH_isMainMemory(address, 1u) != false)
    {
        flashCommand = FLASH_CR_CMD_MSE;
        areaIndex    = MAIN_AREA_INDEX;
    }
    else if (FLASH_isEEPROMlikeMemory(address, 1u) != false)
    {
        flashCommand = FLASH_CR_CMD_E2SE;
        areaIndex = EEPROM_LIKE_AREA_INDEX;
    }
    else if(FLASH_isCustomerMemory(address, 1u) != false)
    {
        flashCommand = FLASH_CR_CMD_CBSE;
        areaIndex = SPECIAL_AREA_INDEX;
    }
    else
    {
        flashCommand = 0uL;
        areaIndex = UNKNOWN_AREA_INDEX;
    }

    if(areaIndex != UNKNOWN_AREA_INDEX)
    {
        _FLASH_resetWatchdogTimer();
        HSUP_vDisableInt();
        if(FLASH_unlock(areaIndex) == false)
        {
            /* unlock failed */
            FLASH_tFunctionStatus = MEMORY_WRITE_LOCKED;
            HSUP_vEnableInt();
            return false;
        }

        if ( UTI_TEST(FLASH->SR.V32,FLASH_SR_BSY_Msk) == 0uL )
        {
            FLASH->CR1 = (uint32_t)FLASH_getPageNumber(address);
            FLASH_setPGMWindow(SE_ERS_TIME, SE_ERS_TIME);
            /* sector erase start */
            FLASH->CR.V32 = (uint32_t)((flashCommand << FLASH_CR_CMD_Pos) | ((uint32_t)1u << FLASH_CR_STRT_Pos));
            retVal = FLASH_checkStatus();
            FLASH_clearPGMWindow();
            FLASH_cacheReset();
        }
        else
        {
            /* some flash operation is in progress */
            FLASH_tFunctionStatus = PREVIOUS_PROCESS_INCOMPLETE;
        }

        /* Set the LOCK bit to lock the FLASH register access */
        UTI_SET(FLASH->CR.V32, FLASH_unlockParams[areaIndex].mask);
        HSUP_vEnableInt();
    }
    else
    {
        /* address out of range */
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }
    return retVal;
}

/* microcontroller specific functions called directly */

/** \brief    FLASH_eraseAll()
 *
 *  \details  Erases all blocks.
 *
 *  \return
 *            true    - successful \n
 *            false   - failed
 *  \remarks
 *            - This function erases all blocks.
 *            - This function takes longer time so do not use watchdog timer for program flow control.
 */
SDEF_SetSegmentCode(FLASH_ROUTINES)
bool FLASH_eraseAll(void)
{
//    uint32_t timeOut = UINT32_MAX;
    bool retVal = false;
    FLASH_tFunctionStatus = NO_ERRORS;

    if(FLASH_bIsAccessEnabled())
    {
        _FLASH_resetWatchdogTimer();
        HSUP_vDisableInt();
        if(FLASH_unlock(MAIN_AREA_INDEX) == false)
        {
            /* unlock failed */
            FLASH_tFunctionStatus = MEMORY_WRITE_LOCKED;
            HSUP_vEnableInt();
            return false;
        }
        if ( UTI_TEST(FLASH->SR.V32,FLASH_SR_BSY_Msk) == 0uL )
        {
            FLASH_setPGMWindow(SE_ERS_TIME, SE_ERS_TIME);
            /* total main area erase start */
            FLASH->CR.V32 = (uint32_t)((FLASH_CR_CMD_ME0 << FLASH_CR_CMD_Pos) | ((uint32_t)1u << FLASH_CR_STRT_Pos));
            retVal = FLASH_checkStatus();
            FLASH_clearPGMWindow();

            /* Set the LOCK bit to lock the FLASH register access */
            UTI_SET(FLASH->CR.V32, FLASH_unlockParams[MAIN_AREA_INDEX].mask);
        }
        else
        {
            /* some flash operation is in progress */
            FLASH_tFunctionStatus = PREVIOUS_PROCESS_INCOMPLETE;
        }
    }
    else
    {
        /* user access is disabled */
        FLASH_tFunctionStatus = USER_ACCESS_DISABLED;
    }
    return retVal;
}
SDEF_SetSegmentCode_Default()

#endif //DOXY_DEVELOPERS_DOC

/** \brief    FLASH_readLockStatus()
 *
 *  \details  Read the lock bit status of given ROM block
 *
 *  \param    address   - an address from desired memory block
 *  \param    *isLocked  - pointer to result status
 *  \return
 *            true    - test successful
 *            false   - test failed
 *\remarks
 *            - address must be an address from desired memory block.
 *            - if function returns true, lock result can be read as second parameter
 *              otherwise is result invalid.
 */
bool FLASH_readLockStatus(tAddress *address, bool *isLocked)
{
    uint32_t mask;
    uint16_t bitNr;
    bool retVal = false;
    FLASH_tFunctionStatus = NO_ERRORS;

    /* is flash access enabled by user application ? */
    if (FLASH_bIsAccessEnabled())
    {
        if (isLocked != (bool *)NULL)
        {
            bitNr = FLASH_getWriteProtectInfo(address);
            if (bitNr != FLASH_INVALID_VALUE)
            {
                _FLASH_vOperationStart(FLASH_STATE_LOCK_CHECK); //flashMcXs.c
                mask = (uint32_t)(1uL << (uint32_t)bitNr);
                if (UTI_TEST(FLASH->WRPR, mask) == 0UL)
                {
                    *isLocked = false;
                }
                else
                {
                    *isLocked = true;
                }
                _FLASH_vOperationFinished(FLASH_STATE_LOCK_CHECK); //flashMcXs.c
                retVal = true;
            }
            else
            {
                /* address out of range */
                FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
            }
        }
        else
        {
            /* some input pointer is invalid */
            FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
        }
    }
    else
    {
        FLASH_tFunctionStatus = USER_ACCESS_DISABLED;
    }
    return retVal;
}

/** \brief    FLASH_getStartAddress()
 *
 *  \details  Gets the start address of given ROM block number
 *
 *  \param    blockNumber - number of desired memory block
 *  \return
 *            - Start address for given memory block number in 4 bytes format.
 *            - value 0 if block number is invalid
 *  \remarks
 *            - blockNumber must be a valid value for given derivative.
 */
uint32_t FLASH_getStartAddress(uint16_t blockNumber)
{
    if ( blockNumber < FLASH_blockCount )
    {
        return(ROM1_PHY_START + ((uint32_t)blockNumber * (uint32_t)FLASH_blockSize));
    }
    else
    {
        return INVALID_ADDRESS;
    }
}

/** \brief    FLASH_getEndAddress()
 *
 *  \details  Gets the last address of given ROM block number
 *
 *  \param    blockNumber - number of desired memory block
 *  \return
 *            - End address for given memory block number in 4 bytes format.
 *            - value 0 if block number is invalid
 *  \remarks
 *            - blockNumber must be a valid value for given derivative.
 */
uint32_t FLASH_getEndAddress(uint16_t blockNumber)
{
    if ( blockNumber < FLASH_blockCount )
    {
        return(ROM1_PHY_START + ((uint32_t)FLASH_blockSize - 1UL) + ((uint32_t)blockNumber * (uint32_t)FLASH_blockSize));
    }
    else
    {
        return INVALID_ADDRESS;
    }
}

/** \brief    Gets the number of flash page for requested memory address
 *
 *  \param    address - address of flash memory
 *  \return
 *            - page number. \n
 *  \remarks
 *            - address must be a valid value for given derivative.
 */
uint8_t FLASH_getPageNumber(tAddress *address)
{
    uint8_t u8Result = UINT8_MAX;
    uint32_t block_addr;

    if (FLASH_isMainMemory(address, 1u))
    {

        block_addr = (uint32_t)address - (uint32_t)MAIN_BLOCK_ADDRESS;
        u8Result = (uint8_t)(block_addr / (uint32_t)FLASH_blockSize);
    }
    else if (FLASH_isEEPROMlikeMemory(address, 1u))
    {
        block_addr = (uint32_t)address - (uint32_t)E2_BLOCK_ADDRESS;
        u8Result = (uint8_t)(block_addr / (uint32_t)FLASH_blockSize);
    }
    else
    {
        /* out of memory*/
#if defined(DEBUG) && (DEBUG != 0x00u)
        _ASSERT_vOnFailure ((const uint8_t*)(__FILE__), __LINE__);
#endif
    }
    return u8Result;
}
