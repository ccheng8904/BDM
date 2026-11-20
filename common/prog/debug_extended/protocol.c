/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBGP
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  debug_extended
 *
 *  \brief    Definitions and declarations of generic part of the debug_extended
 *            component: Protocol layer
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "bsh_stdinc.h"

#include "debug_extended/protocol.h"
#include "debug_extended/interface.h"
#include "ped_fw/stdcrc/CrcCom.h"
#include "sts_sim_test_uty.h"

/*lint -e866 Is approved: sizeof((char)'x') delivers 1 as intended; 'x' is only for naming the exact character */
//Axivion Disable Style MisraC2012-18.4: Pointer = Pointer + Integer is tested as OK


/******************************************************************************/
/* PRIVATE PREPROCESSOR DEFINITIONS                                            */
/******************************************************************************/

#if (! defined STS_SIM_TEST_CI)
    #define STATIC_DBGX    static
#else
    #define STATIC_DBGX
#endif

#define DBGP_BIN_TO_ASCII_SIZE_MULTIPLIER  ((uint32_t)3)
#define DBGP_NUMBER_BYTES_OF_LEN           ((uint32_t)1)
#define DBGP_NUMBER_BYTES_OF_ID            ((uint32_t)1)
#define DBGP_NUMBER_BYTES_OF_CRC           ((uint32_t)2)
#define DBGP_NUMBER_BYTES_OF_LEN_ID_CRC    (DBGP_NUMBER_BYTES_OF_LEN + DBGP_NUMBER_BYTES_OF_ID + DBGP_NUMBER_BYTES_OF_CRC)

#define DBGP_FUNCTION_MASK   ((uint8_t)0x0F)
#define DBGP_MODULE_SHIFT    ((uint8_t)4)
#define DBGP_BITS_PER_BYTE   ((uint8_t)8)

#define DBGP_NIBBLE_VAL_0X9  ((uint8_t)0x09)
#define DBGP_NIBBLE_VAL_0XF  ((uint8_t)0x0F)


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/* Receive data function for application protocol (e.g. touch) */
STATIC_DBGX DBGP_ReceiveDataFunctionOfApi_t DBGP_ReceiveDataFunctionOfProt_CB = NULL; //lint !e9003 not yet used outside block scope @@


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                              */
/******************************************************************************/
static int32_t DBGP_build_TouchDerA_TextP_Frame(const char *DbgAttributes, uint16_t Module, uint32_t Function,
                                                const uint8_t *InBytesArray, uint16_t InArrayLen, uint8_t *OutBytesArray);
static int32_t DBGP_build_TouchDerA_TouchDerP_Frame(uint16_t Module, uint32_t Function,
                                                    const uint8_t *InBytesArray, uint16_t InArrayLen, uint8_t *OutBytesArray);
static int32_t DBGP_build_TextA_TextP_Frame(const char *DbgAttributes, const char *InCharArray,
                                            uint16_t InArrayLen, uint8_t *OutBytesArray);
static uint8_t DBGP_sendBytesOfProtocolToInterfaces(uint8_t Protocol, const uint8_t *Bytes, uint16_t Len);
static void DBGP_buildBinTouchFrame(uint16_t Module, uint32_t Function, 
                                    const uint8_t *InBytesArray, uint16_t InArrayLen, uint8_t *OutBytesArray);
static uint16_t DBGP_calcTouchCrc(const uint8_t *InBytesArray, uint16_t InArrayLen);
static uint8_t *DBGP_copyAttributeStringWithSeparatorToOutString(const char *DbgAttributes, uint8_t *OutBytesArray);
static void DBGP_convBinArrayToAsciiString(const uint8_t *InBytesArray, char *AsciiString, uint16_t InArrayLen);
static void DBGP_convBinByteToAsciiString(uint8_t BinByte, char *AsciiString);
static char DBGP_convBinNibbleToAsciiChar(uint8_t BinNibble);


/******************************************************************************/
/* PRIVATE DATA DECLARATIONS                                                  */
/******************************************************************************/

static struct DBGP_ProtocolConfig_s DBGP_ProtocolConfig = {NULL, 0U, false};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

int32_t DBGP_sendProtData(const char *DbgAttributes, uint16_t Module_ID, 
                          uint16_t Function, const uint8_t *Data, uint16_t DataLen)
{
    int32_t StatusLen;

    uint8_t ProtFrameArray[DBGP_LEN_OF_PROT_FRAME_ARRAY];
                     /* @@ Supports reentrace ability, can move from stack */
                     /* to heap and can have dynamic length as soon as we */
                     /* have memory block pools available in the framework */

    /* Frame from touch derived application in text protocol */
    StatusLen = DBGP_build_TouchDerA_TextP_Frame(DbgAttributes, Module_ID, Function, Data, DataLen, ProtFrameArray);
    if (StatusLen >= DBGP_STAT_NO_ERROR)
    {
        StatusLen = (int32_t)DBGP_sendBytesOfProtocolToInterfaces(DBGP_PROTOCOL_TEXT, ProtFrameArray, (uint16_t)StatusLen);
    }
    else
    {
        ProtFrameArray[0] = (uint8_t)'!';
        (void)DBGP_sendBytesOfProtocolToInterfaces(DBGP_PROTOCOL_TEXT, ProtFrameArray, (uint16_t)sizeof((char)'!'));
    }

    /* Frame from touch derived application in touch derived protocol */
    StatusLen = DBGP_build_TouchDerA_TouchDerP_Frame(Module_ID, Function, Data, DataLen, ProtFrameArray);
    if (StatusLen >= DBGP_STAT_NO_ERROR)
    {
        StatusLen = (int32_t)DBGP_sendBytesOfProtocolToInterfaces(DBGP_PROTOCOL_TCH_DER, ProtFrameArray, (uint16_t)StatusLen);
    }

    return StatusLen;
}

int32_t DBGP_notifyProtReceiveData_CB(DBGP_ReceiveDataFunctionOfApi_t ReceiveDataFunctionOfProt_CB)
{
    int32_t ErrorReturnCode;

    if (DBGP_ReceiveDataFunctionOfProt_CB == NULL)
    {
        DBGP_ReceiveDataFunctionOfProt_CB = ReceiveDataFunctionOfProt_CB;
        ErrorReturnCode = DBGP_STAT_NO_ERROR;
    }
    else
    {
        ErrorReturnCode = DBGP_STAT_NOTIFY_PROBLEM;
    }

    return ErrorReturnCode;
}

int32_t DBGP_writeProtCharArray(const char *DbgAttributes, const char *CharArray, uint16_t NumberOfArrayElements)
{
    const char StringTooLong[25] = {"!~Output string too long"};
    int32_t  StatusLen;
    uint8_t  ProtFrameArray[DBGP_LEN_OF_PROT_FRAME_ARRAY];
                     /* @@ Supports reentrace ability, can move from stack */
                     /* to heap and can have dynamic length as soon as we */
                     /* have memory block pools available in the framework */

    STS_logStringAndStringWithLen("--- DBGP_writeProtCharArray, CharArray: ", CharArray, NumberOfArrayElements); 
    STS_logStringAndStringWithLen("--- DBGP_writeProtCharArray, DbgAttributes: ", DbgAttributes, DBGP_LEN_ATTRIBUTES_STRING); 

    if(DBGP_ProtocolConfig.DbgAttributesActive)
    {
        StatusLen = DBGP_build_TextA_TextP_Frame(DbgAttributes, CharArray, NumberOfArrayElements, ProtFrameArray);
    }
    else
    {
        (void)memcpy((void *)ProtFrameArray, (const void *)CharArray, NumberOfArrayElements); //lint !e920 cast from pointer to void is OK
        StatusLen = (int32_t)NumberOfArrayElements;
    }

    if (StatusLen >= DBGP_STAT_NO_ERROR)
    {
        StatusLen = (int32_t)DBGP_sendBytesOfProtocolToInterfaces(DBGP_PROTOCOL_TEXT, ProtFrameArray, (uint16_t)StatusLen);
    }
    else
    {
        (void)memcpy((void *)ProtFrameArray, (const void *)StringTooLong, sizeof(StringTooLong)); //lint !e920 cast from pointer to void is OK: return value is not used
        (void)DBGP_sendBytesOfProtocolToInterfaces(DBGP_PROTOCOL_TEXT, ProtFrameArray, (uint16_t)sizeof(StringTooLong));
    }

    return StatusLen;
}

int32_t DBGP_getProtString(char *String, uint16_t MaxLen)
{
    return DBGI_GetInterfaceBytes(String, MaxLen);
}

void DBGP_getProtString_CB(uint8_t *Data, uint16_t DataLen)
{
    DBGX_getString_CB(Data, DataLen);
}

void DBGP_setProtocolConfiguration(const uint8_t *Protocols, uint8_t NumberOfProtocols, bool DbgAttributesActive)
{
    DBGP_ProtocolConfig.Protocols = Protocols;
    DBGP_ProtocolConfig.NumberOfProtocols = NumberOfProtocols;
    DBGP_ProtocolConfig.DbgAttributesActive = DbgAttributesActive;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/** \brief  Builds the debug_extended protocol frame for touch derived application and text protocol
 *
 *  \param  DbgAttributes  Pointer to the string of indexes of level, scenario
 *                         module ID and other attributes needed for putting out
 *                         onto the debug_extended interface besides the debug_extended data
 *  \param  Module         ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  Function       Function of the debug_extended output data, needed for the PC GUI tool
 *  \param  InBytesArray   Pointer to the array of payload input bytes
 *  \param  InArrayLen     Number of payload input bytes
 *  \param  OutBytesArray  Pointer to the array of built frame inclusive payload
 *                         (Is also used for temporary values for saving of RAM)
 *  \return Number of bytes of built frame inclusive payload or, if negative, error code
 */
static int32_t DBGP_build_TouchDerA_TextP_Frame(const char *DbgAttributes, uint16_t Module, uint32_t Function,
                    const uint8_t *InBytesArray, uint16_t InArrayLen, uint8_t *OutBytesArray)
{
    int32_t  Status;
    uint8_t  *ArrayStringPtr;
    uint16_t AddrOffsetForTmpBinFrame;
    uint16_t BinArrayLen;

    Status = ((int32_t)DBGP_NUMBER_BYTES_OF_LEN_ID_CRC * (int32_t)DBGP_BIN_TO_ASCII_SIZE_MULTIPLIER)
             + ((int32_t)InArrayLen * (int32_t)DBGP_BIN_TO_ASCII_SIZE_MULTIPLIER);

    if(DBGP_ProtocolConfig.DbgAttributesActive)
    {
        Status += (int32_t)DBGP_LEN_ATTRIBUTES_STRING + (int32_t)DBGP_LEN_ATTRIBUTES_STRING_POSTFIX;
    }

    if (Status <= (int32_t)DBGP_LEN_OF_PROT_FRAME_ARRAY)
    {
        if(DBGP_ProtocolConfig.DbgAttributesActive)
        {
            ArrayStringPtr = DBGP_copyAttributeStringWithSeparatorToOutString(DbgAttributes, OutBytesArray);
        }

        AddrOffsetForTmpBinFrame = (uint16_t)DBGP_LEN_OF_PROT_FRAME_ARRAY 
                                    - ((uint16_t)DBGP_NUMBER_BYTES_OF_LEN_ID_CRC + InArrayLen);
        DBGP_buildBinTouchFrame(Module, Function, InBytesArray, InArrayLen, (OutBytesArray + AddrOffsetForTmpBinFrame));

        BinArrayLen = InArrayLen + (uint16_t)DBGP_NUMBER_BYTES_OF_LEN_ID_CRC;
        DBGP_convBinArrayToAsciiString(OutBytesArray + AddrOffsetForTmpBinFrame, (char *)ArrayStringPtr, BinArrayLen);  /*lint !e926 Type is OK here: char has 1 byte and uint8_t has 1 byte */
    }
    else
    {
        Status = DBGP_STAT_ARRAY_LEN_PROBLEM;
    }

    return Status;
}

/** \brief  Builds the debug_extended protocol frame for touch derived application and touch derived protocol
 *
 *  \param  Module         ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  Function       Function of the debug_extended output data, needed for the PC GUI tool
 *  \param  InBytesArray   Pointer to the array of payload input bytes
 *  \param  InArrayLen     Number of payload input bytes
 *  \param  OutBytesArray  Pointer to the array of built frame inclusive payload
 *  \return Number of bytes of built frame inclusive payload or, if negative, error code
 */
static int32_t DBGP_build_TouchDerA_TouchDerP_Frame( uint16_t Module, uint32_t Function, const uint8_t *InBytesArray,
                                                     uint16_t InArrayLen, uint8_t *OutBytesArray)
{
    int32_t Status;

    Status = (int32_t)DBGP_NUMBER_BYTES_OF_LEN_ID_CRC + (int32_t)InArrayLen;

    if (Status <= (int32_t)DBGP_LEN_OF_PROT_FRAME_ARRAY)
    {
        DBGP_buildBinTouchFrame(Module, Function, InBytesArray, InArrayLen, OutBytesArray);
    }
    else
    {
        Status = DBGP_STAT_ARRAY_LEN_PROBLEM;
    }

    return Status;
}

/** \brief  Builds the debug_extended protocol frame for text application and text protocol
 *
 *  \param  DbgAttributes  Pointer to the string of indexes of level, scenario module ID and other attributes
 *                         needed for putting out onto the debug_extended interface besides the debug_extended data
 *  \param  InCharArray    Pointer to the array of payload input characters
 *  \param  InArrayLen     Number of payload input characters
 *  \param  OutBytesArray  Pointer to the array of built frame inclusive payload
 *  \return Number of characters of built frame inclusive payload or, if negative, error code
 */
static int32_t DBGP_build_TextA_TextP_Frame(const char *DbgAttributes, const char *InCharArray,
                                            uint16_t InArrayLen, uint8_t *OutBytesArray)
{
    int32_t Status;
    uint8_t *ArrayStringPtr;

    Status = ((int32_t)InArrayLen + (int32_t)DBGP_LEN_ATTRIBUTES_STRING_POSTFIX + (int32_t)DBGP_LEN_ATTRIBUTES_STRING);

    if (Status <= (int32_t)DBGP_LEN_OF_PROT_FRAME_ARRAY)
    {
        ArrayStringPtr = DBGP_copyAttributeStringWithSeparatorToOutString(DbgAttributes, OutBytesArray);
        (void)memcpy((void *)ArrayStringPtr, (const void *)InCharArray, InArrayLen); //lint !e920 cast from pointer to void is OK
    }
    else
    {
        Status = DBGP_STAT_ARRAY_LEN_PROBLEM;
    }

    return Status;
}

/** \brief Passes the data from the protocol layer to the configured interfaces
 *
 *  \param  Protocol  Protocol ID of the debug_extended data
 *  \param  Bytes     Data string to be sent via the protocol
 *  \param  Len       Length of the data string to be sent
 *  \return DBGI_STAT_NO_ERROR --> Data has been given to the interface layer
 */
static uint8_t DBGP_sendBytesOfProtocolToInterfaces(uint8_t Protocol, const uint8_t *Bytes, uint16_t Len)
{
    for (uint8_t Index = 0U; Index < DBGP_ProtocolConfig.NumberOfProtocols; Index++)
    {
        if (DBGP_ProtocolConfig.Protocols[Index] == Protocol)
        {
            (void)DBGI_SendInterfaceBytes(Index, Bytes, Len);
        }
    }
    return DBGI_STAT_NO_ERROR;
}

/** \brief  Builds the binary touch frame
 *
 *  \param  Module         ID of the module (e.g touch), needed for the PC GUI tool
 *  \param  Function       Function of the debug_extended output data, needed for the PC GUI tool
 *  \param  InBytesArray   Pointer to the array of payload input bytes (PAYLOAD)
 *  \param  InArrayLen     Number of payload input bytes
 *  \param  OutBytesArray  Pointer to the array of built frame inclusive payload
 *                         (LEN, ID, PAYLOAD, CRC_HIGH, CRC_LOW)
 */
static void DBGP_buildBinTouchFrame(uint16_t Module, uint32_t Function, const uint8_t *InBytesArray, uint16_t InArrayLen, uint8_t *OutBytesArray)
{
    uint8_t *WritePtr;
    uint8_t FrameID;
    uint16_t Crc;

    WritePtr = OutBytesArray;
    *WritePtr = (uint8_t)InArrayLen;  /* LEN */
    WritePtr++;

    FrameID = (uint8_t)Function & (uint8_t)DBGP_FUNCTION_MASK;
    FrameID = FrameID | ((uint8_t)Module << DBGP_MODULE_SHIFT); /*lint !e734 Shift is OK here, even if loss of precision */
    *WritePtr = FrameID;              /* ID */
    WritePtr++;
    (void)memcpy(WritePtr, InBytesArray, InArrayLen);
                                      /* PAYLOAD */
    WritePtr += InArrayLen;

    Crc = DBGP_calcTouchCrc(OutBytesArray, InArrayLen + (uint16_t)DBGP_NUMBER_BYTES_OF_LEN + (uint16_t)DBGP_NUMBER_BYTES_OF_ID);
    *WritePtr = (uint8_t)(Crc >> DBGP_BITS_PER_BYTE);
    WritePtr++;
    *WritePtr = (uint8_t)Crc;         /* CRC */
}

/** \brief  Calculates the CRC of touch frame
 *
 *  \param  InBytesArray  Pointer to the array of bytes the CRC has to be be calculated
 *  \param  InArrayLen    Number of bytes the CRC has to be calculated
 *  \return CRC
 */
static uint16_t DBGP_calcTouchCrc(const uint8_t *InBytesArray, uint16_t InArrayLen)
{
    uint16_t Crc;

    Crc = 0U;
    for (uint16_t Idx = 0U; Idx < InArrayLen; Idx++)
    {
        Crc = CRC_CcittCalcByteTbl(Crc, InBytesArray[Idx]);
    }

    return Crc;
}

/** \brief  Copies the string of attributes with separation characters pending into an output string
 *
 *  \param  DbgAttributes  Pointer to the string of indexes of level, scenario
 *                         module ID and other attributes needed for putting out
 *                         onto the debug_extended interface besides the debug_extended data
 *  \param  OutBytesArray  Pointer to the array of output string
 *  \return Pointer to the next write position in the output string
 */
static uint8_t *DBGP_copyAttributeStringWithSeparatorToOutString(const char *DbgAttributes, uint8_t *OutBytesArray)
{
    uint8_t *ArrayStringPtr;

    (void)memcpy((void *)OutBytesArray, (const void *)DbgAttributes, (size_t)DBGP_LEN_ATTRIBUTES_STRING); //lint !e920 cast from pointer to void is OK
    ArrayStringPtr = OutBytesArray + DBGP_LEN_ATTRIBUTES_STRING;

    if(DBGP_ProtocolConfig.DbgAttributesActive)
    {
        *ArrayStringPtr = (uint8_t)':';
        ArrayStringPtr = ArrayStringPtr + sizeof((char)':');
        *ArrayStringPtr = (uint8_t)' ';
        ArrayStringPtr = ArrayStringPtr + sizeof((char)' ');
    }

    return ArrayStringPtr;
}

/** \brief  Converts a binary byte array to a non-zero terminated ASCII string
 *
 *  \param  InBytesArray  Binary byte array
 *  \param  AsciiString   2-byte ASCII string
 *  \param  InArrayLen    Number of binary bytes in the array
 */
static void DBGP_convBinArrayToAsciiString(const uint8_t *InBytesArray, char *AsciiString, uint16_t InArrayLen)
{

    const uint8_t *InBytePtr;
    char *AsciiStringPtr;

    InBytePtr      = InBytesArray;
    AsciiStringPtr = AsciiString;
    for (uint16_t Cnt = 0U; Cnt < InArrayLen; Cnt++)
    {
        DBGP_convBinByteToAsciiString(*InBytePtr, AsciiStringPtr);
        InBytePtr++;
        AsciiStringPtr++;
        AsciiStringPtr++;
        *AsciiStringPtr = (char)' ';
        AsciiStringPtr++;
    }
}

/** \brief  Converts a binary byte to a non-zero terminated 2-character ASCII string
 *
 *  \param  BinByte      Binary byte
 *  \param  AsciiString  2-byte ASCII string
 */
static void DBGP_convBinByteToAsciiString(uint8_t BinByte, char *AsciiString)
{
    char Char;

    Char = DBGP_convBinNibbleToAsciiChar(BinByte >> 4);
    AsciiString[0] = Char;
    Char = DBGP_convBinNibbleToAsciiChar(BinByte & (uint8_t)0x0F);
    AsciiString[1] = Char;
}

/** \brief  Converts a binary nibble to an ASCII character
 *
 *  \param  BinNibble  Binary nibble
 *  \return ASCII character
 */
static char DBGP_convBinNibbleToAsciiChar(uint8_t BinNibble)
{
    uint8_t ReturnVal;

    if (BinNibble <= DBGP_NIBBLE_VAL_0X9)
    {
        ReturnVal = BinNibble + (uint8_t)'0';
    }
    else if (BinNibble <= DBGP_NIBBLE_VAL_0XF)
    {
        ReturnVal = BinNibble + (uint8_t)'7';
    }
    else
    {
        ReturnVal = (uint8_t)'?';
    }

    return (char)ReturnVal;
}

/*lint +e866 */
//Axivion Enable Style MisraC2012-18.4

