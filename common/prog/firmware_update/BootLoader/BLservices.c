/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW / Boot-Loader
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  firmware_update
 *
 *  \brief    implementation of Boot-Loader service functions.
 */


 

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "BLservices.h"
#include "BLcommands.h"
#include "BLconfig.h"
#include "BLcom.h"
#if defined(VARIANT_RAMLOADER)  || defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
  #ifdef MCAL_INCLUDED
    #include "BLcomMcalMap.h"
  #else
    #include "BLcomMap.h"
  #endif
    #include "stdcrc/StdCrc.h"
    #include "BLmal.h"
#endif
#include "utility/utility.h"
#if defined (BTM_FIRMWARE_UPDATE)
    #include "firmware_update/gecko_bl/gecko_bl_api.h"
#endif

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define BLSRV_FRAME_CMD_POS   0u
#define BLSRV_FRAME_LEN_POS   1u
#define BLSRV_FRAME_ADDRH_POS 2u



#if defined(VARIANT_RAMLOADER) || defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
    #ifndef BLSRV_BUFFER_SIZE
        #define BLSRV_BUFFER_SIZE 256u
    #endif
#endif

#if defined(VARIANT_ROMLOADER) || defined(VARIANT_GECKO_BL) || defined(VARIANT_LROMLOADER) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
    #define BLSRV_HIGH_BAUD_ID 0u
#endif
/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/
#if defined(VARIANT_ROMLOADER) || defined(VARIANT_LROMLOADER) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
    typedef void (*vvFunction)(void); ///< void void function pointer
#endif

/** service function pointer */
typedef enum BLCOM_rxResult (*ServiceFunc)(uint16_t address, uint16_t length);

/** structure for command service table */
struct BLSRV_cmdService
{
    uint8_t     cmd;     ///< id of command
    ServiceFunc service; ///< function pointer to command service
};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/

/****************** Common service commands ***********************************/
static enum BLCOM_rxResult BLSRV_setPage(uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_invalidCommand(uint16_t address, uint16_t length);

#if defined(VARIANT_ROMLOADER) || defined(VARIANT_GECKO_BL) || defined(VARIANT_LROMLOADER) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
/*************** ROM-Loader service commands *********************************/
static enum BLCOM_rxResult BLSRV_setBaudRate(uint16_t address, uint16_t length);
#if !defined(SELF_IDENT_DISABLED) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
    static enum BLCOM_rxResult BLSRV_selfIdent  (uint16_t address, uint16_t length);
#endif
static enum BLCOM_rxResult BLSRV_jumpLoader (uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_storeData  (uint16_t address, uint16_t length);
#endif

#if defined(VARIANT_RAMLOADER) || defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
/*************** RAM-Loader service commands *********************************/
static enum BLCOM_rxResult BLSRV_jumpApplication(uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_pageErase      (uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_massErase      (uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_programmeData  (uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_readData       (uint16_t address, uint16_t length);
static enum BLCOM_rxResult BLSRV_calcCrc        (uint16_t address, uint16_t length);
static uint32_t BLSRV_u32FromBuffer(const uint8_t* buffer);
static bool BLSRV_isCrcCorrect(uint32_t address, uint32_t length, uint32_t crcExpected);
#endif

static uint32_t BLSRV_getTargetAddress(uint16_t address);
static uint16_t BLSRV_getAdressFromFrame(const uint8_t *buffer);
static uint16_t BLSRV_u16FromBuffer(const uint8_t* buffer);



/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
/*lint -esym(9003,BLSRV_*) could define variable at block scope [MISRA 2012 Rule 8.9, advisory]
 *                         It is not wished to define this variables in a block scope
 */

static const uint16_t BLSRV_dataTimeout  = 300u;     // 300ms data time-out


#if defined(VARIANT_ROMLOADER)  || defined(VARIANT_GECKO_BL) || defined(VARIANT_LROMLOADER) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
    static const uint16_t BLSRV_highBaud = 1250u; ///< standard high baud rate 125k
    #if !defined(SELF_IDENT_DISABLED) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
    typedef char BLSRV_char;
    static const BLSRV_char BLSRV_idString[] = BLCFG_company "_" BLCFG_procVendor "_" BLCFG_procFamily "_2.0_" BLCFG_projectName "\x55\xaa";
    #endif
#endif

#if defined(VARIANT_ROMLOADER) || defined(VARIANT_LROMLOADER)
/** Command service table */
static const struct BLSRV_cmdService BLSRV_cmdServiceTbl[] =
{
    {BL_STORE_DATA,       BLSRV_storeData      },
    {BL_JUMP_LOADER,      BLSRV_jumpLoader     },
#if !defined(SELF_IDENT_DISABLED)
    {BL_SELF_IDENT,       BLSRV_selfIdent      },
#endif
    {BL_SET_BAUDRATE,     BLSRV_setBaudRate    },
    {BL_SET_PAGE,         BLSRV_setPage        },
    {BL_INVALID_CMD,      BLSRV_invalidCommand } // must be last entry !!
};
#elif defined(VARIANT_RAMLOADER)

static uint8_t  BLSRV_dataBuffer[BLSRV_BUFFER_SIZE]; // message buffer
/** Command service table */
static const struct BLSRV_cmdService BLSRV_cmdServiceTbl[] =
{
    {BL_JUMP_APPLICATION, BLSRV_jumpApplication},
    {BL_PAGE_ERASE,       BLSRV_pageErase      },
    {BL_MASS_ERASE,       BLSRV_massErase      },
    {BL_PROGRAMME_DATA,   BLSRV_programmeData  },
    {BL_READ_DATA,        BLSRV_readData       }, 
    {BL_CALC_CRC,         BLSRV_calcCrc        },
    {BL_SET_PAGE,         BLSRV_setPage        },
    {BL_INVALID_CMD,      BLSRV_invalidCommand } // must be last entry !!
};

#elif defined(VARIANT_GECKO_BL)
static uint8_t  BLSRV_dataBuffer[BLSRV_BUFFER_SIZE]; // message buffer
/** Command service table */
static const struct BLSRV_cmdService BLSRV_cmdServiceTbl[] =
{
    {BL_STORE_DATA,       BLSRV_storeData       },
    {BL_JUMP_LOADER,      BLSRV_jumpLoader      },
    {BL_SELF_IDENT,       BLSRV_selfIdent       },
    {BL_SET_BAUDRATE,     BLSRV_setBaudRate     },
    {BL_SET_PAGE,         BLSRV_setPage         },
    {BL_JUMP_APPLICATION, BLSRV_jumpApplication },
    {BL_PAGE_ERASE,       BLSRV_pageErase       },
    {BL_MASS_ERASE,       BLSRV_massErase       },
    {BL_PROGRAMME_DATA,   BLSRV_programmeData   },
    {BL_READ_DATA,        BLSRV_readData        }, 
    {BL_CALC_CRC,         BLSRV_calcCrc         },
    {BL_INVALID_CMD,      BLSRV_invalidCommand  } // must be last entry !!
};

#elif defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
static uint8_t  BLSRV_dataBuffer[BLSRV_BUFFER_SIZE]; // message buffer
/** Command service table */
static const struct BLSRV_cmdService BLSRV_cmdServiceTbl[] =
{
    {BL_STORE_DATA,       BLSRV_storeData_Stub  },
    {BL_JUMP_LOADER,      BLSRV_jumpLoader_Stub },
    {BL_SELF_IDENT,       BLSRV_selfIdent       },
    {BL_SET_BAUDRATE,     BLSRV_setBaudRate     },
    {BL_SET_PAGE,         BLSRV_setPage         },
    {BL_JUMP_APPLICATION, BLSRV_jumpApplication },
    {BL_PAGE_ERASE,       BLSRV_pageErase       },
    {BL_MASS_ERASE,       BLSRV_massErase       },
    {BL_PROGRAMME_DATA,   BLSRV_programmeData   },
    {BL_READ_DATA,        BLSRV_readData        }, 
    {BL_CALC_CRC,         BLSRV_calcCrc         },
    {BL_INVALID_CMD,      BLSRV_invalidCommand  } // must be last entry !!
};
#else
    #error "Either ROMLOADER, LROMLOADER, GECKO_BL or RAMLOADER shell be defined"
#endif

static uint16_t BLSRV_pageAddress;   ///< holds the 16 bit page address
/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
void BLSRV_init(void)
{
#if defined(VARIANT_RAMLOADER)|| defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
    BLMAL_init();
  #if defined(VARIANT_GECKO_BL)
    BLSRV_pageAddress = 0u;
  #endif
#else
    #if defined(RX200) || defined(RX100)
        BLSRV_pageAddress = 0u;
    #endif
#endif
}

enum BLCOM_rxResult BLSRV_doCommandService(const uint8_t *buffer)
{
    uint16_t index;
    uint16_t address = BLSRV_getAdressFromFrame(buffer);
    uint16_t length  = buffer[BLSRV_FRAME_LEN_POS];
    uint8_t  command = buffer[BLSRV_FRAME_CMD_POS];
    if (length == 0u) {length = 256u;}
    for (index = 0u; BLSRV_cmdServiceTbl[index].cmd !=BL_INVALID_CMD ; index++)
    {
        if (BLSRV_cmdServiceTbl[index].cmd  == command) {break;}
    }
    return BLSRV_cmdServiceTbl[index].service(address,length);

}
/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static enum BLCOM_rxResult BLSRV_setPage(uint16_t address, uint16_t length)
{
    (void)length;
    
    BLSRV_pageAddress = address;
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
    return (BLCOM_rxSuccess);
}

static enum BLCOM_rxResult BLSRV_invalidCommand(uint16_t address, uint16_t length)
{
    (void)length;
    (void)address;
    
    BLCOM_sendByte(BL_UNKNOWN_COMMAND);
    return (BLCOM_rxSuccess);

}

#if defined(VARIANT_ROMLOADER) || defined(VARIANT_LROMLOADER) || defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
/*****************************************************************************/
/*************** ROM-Loader service commands *********************************/
/*****************************************************************************/
static enum BLCOM_rxResult BLSRV_setBaudRate(uint16_t address, uint16_t length)
{
    uint16_t baud = address; // address 
    
    (void)length;
    if (baud == BLSRV_HIGH_BAUD_ID)
    {
        baud = BLSRV_highBaud;
    }
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK); // send acknowledge in former baud rate
    BLCOM_setBaudrate(baud);
    return (BLCOM_rxSuccess);
}

#if !defined(SELF_IDENT_DISABLED) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
static enum BLCOM_rxResult BLSRV_selfIdent(uint16_t address, uint16_t length)
{
    (void)address;
    (void)length;

    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
    /* cast uncritical as casted to less stricter alignment */
    BLCOM_sendBuffer((const void *)BLSRV_idString, (uint16_t)sizeof(BLSRV_idString)); /*lint !e928 */
    return (BLCOM_rxSuccess);

}
#endif

static enum BLCOM_rxResult BLSRV_jumpLoader(uint16_t address, uint16_t length)
{
    (void)length;
    (void)address;
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
  #if !defined(UNIT_UNDER_TEST) && !defined(UNIT_TEST_INST) && !defined(VARIANT_GECKO_BL)
    vvFunction fptr = (vvFunction)BLSRV_getTargetAddress(address); //lint !e923 tested to work correctly  
    fptr(); // call RAM-Loader (never come back)
  #endif
    /* following line will never be reached !! */
    return (BLCOM_rxSuccess);
}


static enum BLCOM_rxResult BLSRV_storeData(uint16_t address, uint16_t length)
{
    enum BLCOM_rxResult rxResult;
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
  #if defined(VARIANT_GECKO_BL)
    (void)address;
    /* receive data stream, received data will be ignored
     * The command just can write to the buffer, no other RAM can be written (security!) */
    rxResult = BLCOM_receiveFrame(BLSRV_dataBuffer, length, BLSRV_dataTimeout);
  #else
    void * ptr = (void*)BLSRV_getTargetAddress(address); //lint !e923 tested to work correctly
    // receive data stream and return result
    /* cast uncritical as casted to less stricter alignment */
    rxResult = BLCOM_receiveFrame(ptr, length, BLSRV_dataTimeout);  /*lint !e928 */
  #endif
    if (rxResult == BLCOM_rxSuccess)
    {   // if data stream was received correctly send ack
        BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
    }
    return rxResult;
    
}
#endif

#if defined(VARIANT_RAMLOADER) || defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
/*****************************************************************************/
/*************** RAM-Loader service commands *********************************/
/*****************************************************************************/
static enum BLCOM_rxResult BLSRV_jumpApplication(uint16_t address, uint16_t length)
{
    (void)length;
    (void)address;
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
    BLCOM_generateReset();
    return (BLCOM_rxSuccess);

}

static enum BLCOM_rxResult BLSRV_pageErase(uint16_t address, uint16_t length)
{
    (void)length;
    uint32_t phy_address = BLSRV_getTargetAddress(address);
    
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
    if (BLMAL_eraseBlock(phy_address))
    {   // success
        BLCOM_sendByte(BL_FLASH_CMD_OK);
    }
    else
    {
        BLCOM_sendByte(BL_FLASH_CMD_FAILED);
    }
    return (BLCOM_rxSuccess);
}


static enum BLCOM_rxResult BLSRV_massErase(uint16_t address, uint16_t length)
{
    (void)length;
    (void)address;
    /* not supported because it is dangerous */
    BLCOM_sendByte(BL_UNKNOWN_COMMAND);
    return (BLCOM_rxSuccess);

}

static enum BLCOM_rxResult BLSRV_programmeData(uint16_t address, uint16_t length)
{
    enum BLCOM_rxResult rxResult;
    uint32_t phy_address = BLSRV_getTargetAddress(address);

    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);  // command received successfully
    rxResult = BLCOM_receiveFrame(BLSRV_dataBuffer, length, BLSRV_dataTimeout);
    if (rxResult == BLCOM_rxSuccess)
    {   
        BLCOM_sendByte(BL_ACKNOWLEDGE_OK); // data received successfully
        
        if (BLMAL_writeBuffer(phy_address, length, BLSRV_dataBuffer))
        {   // success
            BLCOM_sendByte(BL_FLASH_CMD_OK);
        }
        else
        {   // programming/flashing error
            BLCOM_sendByte(BL_FLASH_CMD_FAILED);
        }
    }
    return rxResult;
}


static enum BLCOM_rxResult BLSRV_readData(uint16_t address, uint16_t length)
{
    uint32_t phy_address = BLSRV_getTargetAddress(address);
    
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
    if(BLMAL_readBuffer(phy_address, length, BLSRV_dataBuffer))
    {
        BLCOM_sendBuffer(BLSRV_dataBuffer, length);
    }
    else
    {   // reading failed
        BLCOM_sendByte(BL_READ_FAILED);
    }
    return (BLCOM_rxSuccess);
}


static enum BLCOM_rxResult BLSRV_calcCrc(uint16_t address, uint16_t length)
{
    enum BLCOM_rxResult rxResult;
    const uint8_t crcStartAddrPos  = 0u;
    const uint8_t crcEndAddrPos    = 4u;
    const uint8_t crcExpectedPos   = 8u;
    const uint8_t crcCmdDataLength = 0xC;
    
    (void)address;
    (void)length;// use crcCmdDataLength instead (robustness)
    BLCOM_sendByte(BL_ACKNOWLEDGE_OK);
    rxResult = BLCOM_receiveFrame(BLSRV_dataBuffer, crcCmdDataLength, BLSRV_dataTimeout);
    if (rxResult == BLCOM_rxSuccess)
    {   
        uint32_t crcStartAddr = BLSRV_u32FromBuffer(&BLSRV_dataBuffer[crcStartAddrPos]);
        uint32_t crcEndAddr   = BLSRV_u32FromBuffer(&BLSRV_dataBuffer[crcEndAddrPos]);
        uint32_t crcExpected  = BLSRV_u32FromBuffer(&BLSRV_dataBuffer[crcExpectedPos]);
        uint32_t crcLength    =  (uint32_t)(crcEndAddr-crcStartAddr) + 1u;

        BLCOM_sendByte(BL_ACKNOWLEDGE_OK); // CRC data reception OK
        if(TESTUB(BLSRV_isCrcCorrect)(crcStartAddr, crcLength, crcExpected))
        {
          #if defined (BTM_FIRMWARE_UPDATE)
            /* Inform gecko boot loader to install the image to internal flash after reset.
               The reset will be managed by command BL_JUMP_APPLICATION */
            GBTL_installAfterReset();
          #endif
            BLCOM_sendByte(BL_CRC_OK);
        }
        else
        {
            BLCOM_sendByte(BL_CRC_FAILED);
        }
    }
    return rxResult;
}


static bool BLSRV_isCrcCorrect(uint32_t address, uint32_t length, uint32_t crcExpected)
{
    CRC_crc_t crc = CRC_Init();
    while(length > 0u)
    {
        uint32_t readSize = UTI_MIN(BLSRV_BUFFER_SIZE,length);
        if(!BLMAL_readBuffer(address, readSize, BLSRV_dataBuffer))
        {   // no crc result due to read error!
            return false;
        }
        crc = CRC_Update(crc, readSize, BLSRV_dataBuffer);
        address += readSize;
        length  -= readSize;
    }
    crc = CRC_Finish(crc);
    return ((uint32_t)crc == crcExpected);
}
#endif  // defined(VARIANT_RAMLOADER) || defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)


static uint32_t BLSRV_getTargetAddress(uint16_t address)
{
    uint32_t targetAddress = (uint32_t)BLSRV_pageAddress << 16u;
    targetAddress += address;
    return targetAddress;
}

static uint16_t BLSRV_getAdressFromFrame(const uint8_t *buffer)
{
    return BLSRV_u16FromBuffer(&buffer[BLSRV_FRAME_ADDRH_POS]);
}


static uint16_t BLSRV_u16FromBuffer(const uint8_t* buffer)
{
    uint16_t result = (uint16_t)buffer[0u] << 8u;
    result += buffer[1u];
    return result;
}


#if defined(VARIANT_RAMLOADER) || defined(VARIANT_GECKO_BL) || defined(UNIT_UNDER_TEST) || defined(UNIT_TEST_INST)
static uint32_t BLSRV_u32FromBuffer(const uint8_t* buffer)
{
    uint32_t result = 0u;
    
    for (uint16_t loop = 0u; loop < sizeof(uint32_t); loop++)
    {
        result <<= 8u;
        result  += buffer[loop];
    }
    return result;
}
#endif

