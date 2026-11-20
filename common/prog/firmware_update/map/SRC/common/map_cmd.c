/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          FWU
*   COMP_ABBREV      MAP
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  Implementation of MAP commands.
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "map_cmd.h"
#include "map_util.h"
#include "map_mem_access_interface.h"
/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define MAP_CMD_DEV_INFO_MAX_NAME_LEN 50U
#define MAP_CMD_MAX_EXPONENT_OF_TIMEOUT 10U
#define MAP_CMD_INVALID_MODULE_NUMBER 0XFFFFU

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

enum MAP_CMD_ID
{
    MAP_CMD_ID_DEV_INFO_REQ=1U,
    MAP_CMD_ID_DEV_INFO_RESP=2U,
    MAP_CMD_ID_SETUP_MODULE_REQ=3U,
    MAP_CMD_ID_SETUP_MODULE_RESP=4U,
    MAP_CMD_ID_READ_REQ=5U,
    MAP_CMD_ID_READ_RESP=6U,
    MAP_CMD_ID_WRITE_START_REQ=7U,
    MAP_CMD_ID_WRITE_APPEND_REQ=8U,
    MAP_CMD_ID_WRITE_RESP=9U,
    MAP_CMD_ID_WRITE_PREMATURELY_END_REQ=10U,
    MAP_CMD_ID_UNKNOWN_CMD_RESP=0xFFU
};

enum MAP_Status
{
    MAP_ST_OK=0U,
    MAP_ST_WRONG_PARAM=1U,
    MAP_ST_MISMATCH_PARAM=2U,
    MAP_ST_UNKNOWN_REQ=3U,
    MAP_ST_SEQUENCE_CNT_ERROR=4U,
    MAP_ST_PAYLOAD_SIZE_EXCEEDED=5U
};


struct MAP_ModuleInfo
{
    struct BIGE_Short moduleNumber;
    struct BIGE_Long moduleSize;
    uint8_t nameLength;
    uint8_t name[];/*lint !e9038 for axivion*/
};/*lint !e9038 Arrays with flexible length not liked by Lint, but they reflect reality. It is always being checked, whether we have enough space.*/

union MAP_ModuleInfoPtrUnion /*lint !e9018 The union keyword should not be used. MisraC2012 19.2 advisory */
{
    struct MAP_ModuleInfo *modulePtr;
    uint8_t *bytePtr;
};

struct MAP_DeviceInfoReq
{
    struct MAP_ReqHeader header;
    uint8_t index;
};

struct MAP_DeviceInfoResp
{
    struct MAP_RespHeader header;
    uint8_t nextIndex;
    uint8_t moduleInfos[];/*lint !e9038 for axivion*/
};/*lint !e9038 Arrays with flexible length not liked by Lint, but they reflect reality. It is always being checked, whether we have enough space.*/

struct MAP_SetUpModuleReq
{
    struct MAP_ReqHeader header;
    struct BIGE_Short moduleNumber;
};

struct MAP_SetUpModuleResp
{
    struct MAP_RespHeader header;
    struct BIGE_Short maxRxPayload;
    uint8_t maxTimeoutExponentOfPowerOfTwoOfSeconds;
};

struct MAP_ReadReq
{
    struct MAP_ReqHeader header;
    struct BIGE_Short moduleNumber;
    struct BIGE_Short numOfBytes;
    struct BIGE_Long offset;
};

struct MAP_ReadResp
{
    struct MAP_RespHeader header;
    uint8_t data[];/*lint !e9038 for axivion*/
};/*lint !e9038 Arrays with flexible length not liked by Lint, but they reflect reality. It is always being checked, whether we have enough space.*/

struct MAP_WriteStartReq
{
    struct MAP_ReqHeader header;
    struct BIGE_Short moduleNumber;
    struct BIGE_Long overallNumberOfBytes;
    uint8_t data[];/*lint !e9038 for axivion*/
};/*lint !e9038 Arrays with flexible length not liked by Lint, but they reflect reality. It is always being checked, whether we have enough space.*/

struct MAP_WriteAppendReq
{
    struct MAP_ReqHeader header;
    struct BIGE_Short moduleNumber;
    uint8_t data[];/*lint !e9038 for axivion*/
};/*lint !e9038 Arrays with flexible length not liked by Lint, but they reflect reality. It is always being checked, whether we have enough space.*/

struct MAP_WriteprematurelyEndReq
{
    struct MAP_ReqHeader header;
    struct BIGE_Short moduleNumber;
    struct BIGE_Long overallBytesWritten;
};

struct MAP_WriteResp
{
    struct MAP_RespHeader header;
};

struct MAP_UnknowCmdResp
{
    struct MAP_RespHeader header;
};

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void MAP_CMD_GetDeviceInfo(struct MAP_Arguments args);
static void MAP_CMD_SetUpModule(struct MAP_Arguments args);
static void MAP_CMD_Read(struct MAP_Arguments args);
static void MAP_CMD_WriteStart(struct MAP_Arguments args);
static void MAP_CMD_WriteAppend(struct MAP_Arguments args);
static void MAP_CMD_WritePrematurelyEnd(struct MAP_Arguments args);
static void MAP_CMD_HandleUnknownCmd(struct MAP_Arguments args);
/* Warning 522: highest operation, function 'MAP_CMD_SetResponseHeader', lacks side effects [MISRA 2012 Rule 2.2, advisory] */
/*lint -esym(522,MAP_CMD_SetResponseHeader) false positive */
static void MAP_CMD_SetResponseHeader(struct MAP_RespHeader *const*header, enum MAP_CMD_ID id, uint8_t requestSeqCnt, uint8_t status);
static uint8_t MAP_CMD_GetResponseSeqCnt(uint8_t requestSeqCnt);
static void MAP_CMD_SetCorrectWriteRequestSeqCnt(uint8_t value);
static uint8_t MAP_CMD_GetLastCorrectWriteRequestSeqCnt(void);
static uint8_t MAP_CMD_GetPresentCorrectWriteRequestSeqCnt(void);

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static uint8_t MAP_CMD_seqCntOfLastCorrectWriteMsg=0U;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/*lint -save -e934 "Taking address of near auto variable 'xx' [MISRA 2012 Rule 1.3, required]" does not make sense on MCU platforms */

MAP_CMD_Func_t MAP_CMD_GetCmdExecFunc(uint8_t id)
{
    MAP_CMD_Func_t retVal=NULL;

    switch(id)
    {
        default:
            retVal=MAP_CMD_HandleUnknownCmd;
            break;
        case (uint8_t)MAP_CMD_ID_DEV_INFO_REQ:
            retVal=MAP_CMD_GetDeviceInfo;
            break;
        case (uint8_t)MAP_CMD_ID_SETUP_MODULE_REQ:
            retVal=MAP_CMD_SetUpModule;
            break;
        case (uint8_t)MAP_CMD_ID_READ_REQ:
            retVal=MAP_CMD_Read;
            break;
        case (uint8_t)MAP_CMD_ID_WRITE_START_REQ:
            retVal=MAP_CMD_WriteStart;
            break;
        case (uint8_t)MAP_CMD_ID_WRITE_APPEND_REQ:
            retVal=MAP_CMD_WriteAppend;
            break;
        case (uint8_t)MAP_CMD_ID_WRITE_PREMATURELY_END_REQ:
            retVal=MAP_CMD_WritePrematurelyEnd;
            break;
    }

    return retVal;
}

/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

/* Axivion Disable Style MisraC2012-11.5: "Conversion between object pointer and void pointer" 
   Tested to work correctly for all platforms*/
static void MAP_CMD_GetDeviceInfo(struct MAP_Arguments args)
{
    uint8_t status=(uint8_t)MAP_ST_OK;
    // AXIVION Next Line MisraC2012-11.5 tested to work correctly on all platforms
    const struct MAP_DeviceInfoReq *req=(const struct MAP_DeviceInfoReq*)args.rxData;
    struct MAP_DeviceInfoResp *resp=(struct MAP_DeviceInfoResp*)args.txData;
    struct MAP_RespHeader *respHeader=&resp->header;
    union MAP_ModuleInfoPtrUnion moduleInfo;/*lint !e9018 union of pointers not liked by lint, but needed, to put structures into byte array.*/

    uint8_t oldIndex;
    uint8_t newIndex;
    uint16_t modNum;
    uint32_t modSize;
    uint8_t nameLen;
    uint8_t name[MAP_CMD_DEV_INFO_MAX_NAME_LEN];

    bool getOneMoreModuleInfo=true;

    *args.txLen=(uint16_t)sizeof(struct MAP_DeviceInfoResp);

    if(args.rxLen!=(uint16_t)sizeof(struct MAP_DeviceInfoReq))
    {/*Wrong length of request*/
        resp->nextIndex=0U;
        status=(uint8_t)MAP_ST_MISMATCH_PARAM;
    }
    else if(args.maxTxPayload<(uint16_t)(sizeof(struct MAP_DeviceInfoResp)+sizeof(struct MAP_ModuleInfo)))
    {/*Too little tx buffer to send even one module info in response*/
        *args.txLen=(uint16_t)sizeof(struct MAP_RespHeader);
        status=(uint8_t)MAP_ST_PAYLOAD_SIZE_EXCEEDED;
    }
    else
    {
        moduleInfo.bytePtr=resp->moduleInfos;
        oldIndex=req->index;

        while(getOneMoreModuleInfo)
        {
            status=MAP_MemAccess.devInfo(oldIndex, &newIndex, &modNum, &modSize, &nameLen, MAP_CMD_DEV_INFO_MAX_NAME_LEN, name);

            if(status==(uint8_t)MAP_ST_OK)
            {
                if(args.maxTxPayload<(*args.txLen+(uint16_t)sizeof(struct MAP_ModuleInfo)+nameLen))
                {
                    /*Last module info does not fit into tx buffer*/
                    if(req->index==(newIndex-1U))
                    {
                        /*Not even one module info fits into tx buffer*/
                        status=(uint8_t)MAP_ST_PAYLOAD_SIZE_EXCEEDED;
                    }

                    newIndex=oldIndex;

                    getOneMoreModuleInfo=false;
                }
                else
                {
                    /*Module info fits -> Increase size of data to be sent*/
                    (*args.txLen)+=(uint16_t)sizeof(struct MAP_ModuleInfo)+nameLen;
                    /*Fill parameters of module info with values*/
                    BIGE_SetShortBytes(&moduleInfo.modulePtr->moduleNumber, modNum);
                    BIGE_SetLongBytes(&moduleInfo.modulePtr->moduleSize, modSize);
                    moduleInfo.modulePtr->nameLength=nameLen;

                    for(uint8_t i=0;i<nameLen;i++)
                    {
                        moduleInfo.modulePtr->name[i]=name[i];
                    }
                    /*Shift pointer in tx buffer to write next module info*/
                    /*lint -e{9016} "pointer arithmetic not allowed MisraC2012 18.4 advisory" */
                    moduleInfo.bytePtr+=(uint8_t)sizeof(struct MAP_ModuleInfo)+nameLen;
                }
            }
            else
            {
                /*If getting module info from lower layer ended in error, stop getting additional infos and return error.*/
                getOneMoreModuleInfo=false;
            }

            resp->nextIndex=newIndex;
            oldIndex=newIndex;

            if(newIndex==0U)
            {
                /*Got all module infos, so lower layer returned index zero, with which we started*/
                getOneMoreModuleInfo=false;
            }
        }
    }

    MAP_CMD_SetResponseHeader(&respHeader, MAP_CMD_ID_DEV_INFO_RESP, req->header.seqCnt, status);
}


static void MAP_CMD_SetUpModule(struct MAP_Arguments args)
{
    static uint16_t MAP_CMD_lastModuleOpened=MAP_CMD_INVALID_MODULE_NUMBER;

    uint8_t status=(uint8_t)MAP_ST_OK;
    const struct MAP_SetUpModuleReq *req=(const struct MAP_SetUpModuleReq*)args.rxData;
    struct MAP_SetUpModuleResp *resp=(struct MAP_SetUpModuleResp*)args.txData;
    struct MAP_RespHeader *respHeader=&resp->header;

    uint16_t modNum;

    *args.txLen=(uint16_t)sizeof(struct MAP_SetUpModuleResp);

    if(args.rxLen!=(uint16_t)sizeof(struct MAP_SetUpModuleReq))
    {/*Wrong length of request*/
        BIGE_SetShortBytes(&resp->maxRxPayload, args.maxRxPayload);
        resp->maxTimeoutExponentOfPowerOfTwoOfSeconds=0U;
        status=(uint8_t)MAP_ST_MISMATCH_PARAM;
    }
    else if(args.maxTxPayload<(uint16_t)sizeof(struct MAP_SetUpModuleResp))
    {/*Too little tx buffer for response*/
        *args.txLen=(uint16_t)sizeof(struct MAP_RespHeader);
        status=(uint8_t)MAP_ST_PAYLOAD_SIZE_EXCEEDED;
    }
    else
    {
        modNum=BIGE_ToShort(&req->moduleNumber);
        BIGE_SetShortBytes(&resp->maxRxPayload, args.maxRxPayload);

        if(modNum==MAP_CMD_INVALID_MODULE_NUMBER)
        {
            /*Module Number 0xFFFF not allowed*/
            resp->maxTimeoutExponentOfPowerOfTwoOfSeconds=0U;
            status=(uint8_t)MAP_ST_WRONG_PARAM;
        }
        else
        {
            if(MAP_CMD_lastModuleOpened!=MAP_CMD_INVALID_MODULE_NUMBER)
            {
                /*If module open, close it*/
                status=MAP_MemAccess.close(MAP_CMD_lastModuleOpened);
            }

           if(status==(uint8_t)MAP_ST_OK)
           {
                status=MAP_MemAccess.open(modNum, &resp->maxTimeoutExponentOfPowerOfTwoOfSeconds);

                if(status==(uint8_t)MAP_ST_OK)
                {
                    if(resp->maxTimeoutExponentOfPowerOfTwoOfSeconds>MAP_CMD_MAX_EXPONENT_OF_TIMEOUT)
                    {
                        /*Module wrong, because we opened a module, for which timeout is bigger, than the specified maximum.*/
                        status=(uint8_t)MAP_ST_WRONG_PARAM;
                    }
                    MAP_CMD_lastModuleOpened=modNum;
                }
            }
        }
    }

    MAP_CMD_SetResponseHeader(&respHeader, MAP_CMD_ID_SETUP_MODULE_RESP, req->header.seqCnt, status);
}


static void MAP_CMD_Read(struct MAP_Arguments args)
{
    uint8_t status=(uint8_t)MAP_ST_OK;
    const struct MAP_ReadReq *req=(const struct MAP_ReadReq*)args.rxData;
    struct MAP_ReadResp *resp=(struct MAP_ReadResp*)args.txData;
    struct MAP_RespHeader *respHeader=&resp->header;

    uint16_t modNum;
    uint16_t dataLen;
    uint32_t offset;

    *args.txLen=(uint16_t)sizeof(struct MAP_ReadResp);

    if(args.rxLen!=(uint16_t)sizeof(struct MAP_ReadReq))
    {/*Wrong length of request*/
        status=(uint8_t)MAP_ST_MISMATCH_PARAM;
    }
    else if(args.maxTxPayload==(uint16_t)sizeof(struct MAP_ReadResp))
    {/*Too little tx buffer to read at least one byte*/
        status=(uint8_t)MAP_ST_PAYLOAD_SIZE_EXCEEDED;
    }
    else
    {
        modNum=BIGE_ToShort(&req->moduleNumber);
        dataLen=BIGE_ToShort(&req->numOfBytes);
        offset=BIGE_ToLong(&req->offset);

        if(dataLen>(args.maxTxPayload-(uint16_t)sizeof(struct MAP_RespHeader)))
        {
            /*Request to read more data, than tx buffer available -> Read as much, as fits*/
            dataLen=args.maxTxPayload-(uint16_t)sizeof(struct MAP_RespHeader);
        }

        status=MAP_MemAccess.read(modNum, offset, dataLen, resp->data);

        if(status==(uint8_t)MAP_ST_OK)
        {
            *args.txLen+=dataLen;
        }
    }

    MAP_CMD_SetResponseHeader(&respHeader, MAP_CMD_ID_READ_RESP, req->header.seqCnt, status);
}


static void MAP_CMD_WriteStart(struct MAP_Arguments args)
{
    uint8_t status=(uint8_t)MAP_ST_OK;
    const struct MAP_WriteStartReq *req=(const struct MAP_WriteStartReq*)args.rxData;
    struct MAP_WriteResp *resp=(struct MAP_WriteResp*)args.txData;
    struct MAP_RespHeader *respHeader=&resp->header;

    uint16_t modNum;
    uint32_t overallNumOfBytes;
    uint16_t bytesToWriteInThisMessage;

    *args.txLen=(uint16_t)sizeof(struct MAP_WriteResp);

    if(args.rxLen<(uint16_t)sizeof(struct MAP_WriteStartReq))
    {/*Wrong length of request*/
        status=(uint8_t)MAP_ST_MISMATCH_PARAM;
    }
    else
    {
        modNum=BIGE_ToShort(&req->moduleNumber);
        overallNumOfBytes=BIGE_ToLong(&req->overallNumberOfBytes);
        bytesToWriteInThisMessage=args.rxLen-(uint16_t)sizeof(struct MAP_WriteStartReq);

        if(overallNumOfBytes<bytesToWriteInThisMessage)
        {
            /*For example, overall you want to program 100 bytes and this message already provides at least 101*/
            status=(uint8_t)MAP_ST_WRONG_PARAM;
        }
        else
        {
            status=MAP_MemAccess.writeStart(modNum, overallNumOfBytes, bytesToWriteInThisMessage, req->data);
            /*With each succeeding write request, sequence counter is to be checked*/
            MAP_CMD_SetCorrectWriteRequestSeqCnt(req->header.seqCnt);
        }
    }

    MAP_CMD_SetResponseHeader(&respHeader, MAP_CMD_ID_WRITE_RESP, req->header.seqCnt, status);
}


static void MAP_CMD_WriteAppend(struct MAP_Arguments args)
{
    static uint8_t status=(uint8_t)MAP_ST_OK;
    const struct MAP_WriteAppendReq *req=(const struct MAP_WriteAppendReq*)args.rxData;
    struct MAP_WriteResp *resp=(struct MAP_WriteResp*)args.txData;
    struct MAP_RespHeader *respHeader=&resp->header;

    uint16_t modNum;
    uint16_t bytesToWriteInThisMessage;

    *args.txLen=(uint16_t)sizeof(struct MAP_WriteResp);

    if(args.rxLen<(uint16_t)sizeof(struct MAP_WriteAppendReq))
    {/*Wrong length of request*/
        status=(uint8_t)MAP_ST_MISMATCH_PARAM;
    }
    else if(req->header.seqCnt==MAP_CMD_GetLastCorrectWriteRequestSeqCnt())
    {
        /*Old sequence counter of previous write command: Nothing to do, except for sending answer with same status, as previous.*/
    }
    else if(req->header.seqCnt!=MAP_CMD_GetPresentCorrectWriteRequestSeqCnt())
    {/*Completely wrong sequence counter*/
        status=(uint8_t)MAP_ST_SEQUENCE_CNT_ERROR;
    }
    else
    {
        modNum=BIGE_ToShort(&req->moduleNumber);
        bytesToWriteInThisMessage=args.rxLen-(uint16_t)sizeof(struct MAP_WriteAppendReq);

        status=MAP_MemAccess.writeAppend(modNum, bytesToWriteInThisMessage, req->data);

        MAP_CMD_SetCorrectWriteRequestSeqCnt(req->header.seqCnt);
    }

    MAP_CMD_SetResponseHeader(&respHeader, MAP_CMD_ID_WRITE_RESP, req->header.seqCnt, status);
}


static void MAP_CMD_WritePrematurelyEnd(struct MAP_Arguments args)
{
    static uint8_t status=(uint8_t)MAP_ST_OK;
    const struct MAP_WriteprematurelyEndReq *req=(const struct MAP_WriteprematurelyEndReq*)args.rxData;
    struct MAP_WriteResp *resp=(struct MAP_WriteResp*)args.txData;
    struct MAP_RespHeader *respHeader=&resp->header;

    uint16_t modNum;
    uint32_t overallBytesWritten;

    *args.txLen=(uint16_t)sizeof(struct MAP_WriteResp);

    if(args.rxLen!=(uint16_t)sizeof(struct MAP_WriteprematurelyEndReq))
    {/*Wrong length of request*/
        status=(uint8_t)MAP_ST_MISMATCH_PARAM;
    }
    else if(req->header.seqCnt==MAP_CMD_GetLastCorrectWriteRequestSeqCnt())
    {
        /*Old sequence counter of previous write command: Nothing to do, except for sending answer with same status, as previous.*/
    }
    else if(req->header.seqCnt!=MAP_CMD_GetPresentCorrectWriteRequestSeqCnt())
    {/*Wrong sequence counter*/
        status=(uint8_t)MAP_ST_SEQUENCE_CNT_ERROR;
    }
    else
    {
        modNum=BIGE_ToShort(&req->moduleNumber);
        overallBytesWritten=BIGE_ToLong(&req->overallBytesWritten);

        status=MAP_MemAccess.writePrematurelyEnd(modNum, overallBytesWritten);

        MAP_CMD_SetCorrectWriteRequestSeqCnt(req->header.seqCnt);
    }

    MAP_CMD_SetResponseHeader(&respHeader, MAP_CMD_ID_WRITE_RESP, req->header.seqCnt, status);
}


static void MAP_CMD_HandleUnknownCmd(struct MAP_Arguments args)
{
    const struct MAP_ReqHeader *req=(const struct MAP_ReqHeader*)args.rxData;
    struct MAP_UnknowCmdResp *resp=(struct MAP_UnknowCmdResp*)args.txData;
    struct MAP_RespHeader *respHeader=&resp->header;

    *args.txLen=(uint16_t)sizeof(struct MAP_UnknowCmdResp);

    MAP_CMD_SetResponseHeader(&respHeader, MAP_CMD_ID_UNKNOWN_CMD_RESP, req->seqCnt, (uint8_t)MAP_ST_UNKNOWN_REQ);
}
/* Axivion Enable Style MisraC2012-11.5 */

static void MAP_CMD_SetResponseHeader(struct MAP_RespHeader *const*header, enum MAP_CMD_ID id, uint8_t requestSeqCnt, uint8_t status)
{
    (*header)->id=(uint8_t)id;
    (*header)->seqCnt=MAP_CMD_GetResponseSeqCnt(requestSeqCnt);
    (*header)->status=status;
}


static uint8_t MAP_CMD_GetResponseSeqCnt(uint8_t requestSeqCnt)
{
    return (requestSeqCnt+1U);
}


static void MAP_CMD_SetCorrectWriteRequestSeqCnt(uint8_t value)
{
    MAP_CMD_seqCntOfLastCorrectWriteMsg=value;
}


static uint8_t MAP_CMD_GetLastCorrectWriteRequestSeqCnt(void)
{
    return MAP_CMD_seqCntOfLastCorrectWriteMsg;
}


static uint8_t MAP_CMD_GetPresentCorrectWriteRequestSeqCnt(void)
{
    return (MAP_CMD_seqCntOfLastCorrectWriteMsg+2U);
}
/*lint -restore -e934 */
