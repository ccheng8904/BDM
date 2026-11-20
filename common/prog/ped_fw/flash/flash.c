/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          FLASH_LIB
 ******************************************************************************/
/** \file     Flash.c
 *
 *  \brief    Implementation of generic Flash Routines library.
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "utility/utility.h"
#include "FlashMcXS.h"
#include "FlashMc.h"
#include "FlashXS.h"
#include "hsup.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC
/** \brief    Magic number to be checked before deciding
 *            about flash access enabled or disabled.
 *  \remarks
 *            To avoid check of corrupted data leading to wrong program behaviour, 
 *            this special token is supposed to be enough accurate to evaluate user action
 *            to enable flash access.
 */
#define FLASH_ACCESS_ENABLED     0xA5U
#define FLASH_ACCESS_DISABLED    0x00U
#endif //DOXY_DEVELOPERS_DOC

/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
#ifndef DOXY_DEVELOPERS_DOC
static uint8_t FLASH_ucEnableAccess = FLASH_ACCESS_DISABLED;
tStatusCode FLASH_tFunctionStatus = NO_ERRORS;
#endif //DOXY_DEVELOPERS_DOC

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
/* FLASH_bEnableAccess() */
bool FLASH_bEnableAccess(void)
{
    bool bResult;
    FLASH_tFunctionStatus = NO_ERRORS;
    bResult = FLASH_bEnableAccessMc();
    if ( bResult != false )
    {
        FLASH_ucEnableAccess = FLASH_ACCESS_ENABLED;
    }
    return(bResult);
}

/* FLASH_bDisableAccess() */
bool FLASH_bDisableAccess(void)
{
    bool bResult;
    FLASH_tFunctionStatus = NO_ERRORS;
    bResult = FLASH_bDisableAccessMc();
    if ( bResult != false )
    {
        FLASH_ucEnableAccess = FLASH_ACCESS_DISABLED;
    }
    return(bResult);
}

/* FLASH_bIsAccessEnabled() */
bool FLASH_bIsAccessEnabled(void)
{
    return ((bool)(FLASH_ucEnableAccess == FLASH_ACCESS_ENABLED));
}

/* FLASH_ucGetCellSize() */
uint8_t FLASH_ucGetCellSize(void)
{
    return((uint8_t)sizeof(TFlashData));
}

/* FLASH_bWrite() */
bool FLASH_bWrite(tAddress *ptAddr, tWriteData *ptData, uint16_t uiSize)
{
    bool bWriteResult = false;
    FLASH_tFunctionStatus = NO_ERRORS;
    /* is flash access enabled by user application ? */
    if ( FLASH_ucEnableAccess == FLASH_ACCESS_ENABLED )
    {
        if ( (ptAddr != (tAddress *)NULL) && (ptData != (tWriteData *)NULL) )
        {
            if ( uiSize > 0U )
            {
                _FLASH_vOperationStart(FLASH_STATE_PROGRAM); //FlashMcXS.c
                bWriteResult = FLASH_bWriteMc(ptAddr,ptData,uiSize); //FlashMc.c
                _FLASH_vOperationFinished(FLASH_STATE_PROGRAM); //FlashMcXS.c
            }
            else
            {
                FLASH_tFunctionStatus = PARAMETER_ERROR;
            }
        }
        else
        {
            FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
        }
    }
    else
    {
        FLASH_tFunctionStatus = USER_ACCESS_DISABLED;
    }
    return(bWriteResult);

}

/* FLASH_bWriteByte() */
bool FLASH_bWriteByte(tAddress *ptAddr, uint8_t ucData)
{
    bool bWriteResult = false;
    FLASH_tFunctionStatus = NO_ERRORS;
    /* is flash access enabled by user application ? */
    if ( FLASH_ucEnableAccess == FLASH_ACCESS_ENABLED )
    {
        if ( ptAddr != (tAddress *)NULL )
        {
            _FLASH_vOperationStart(FLASH_STATE_PROGRAM); //FlashMcXS.c
            bWriteResult = FLASH_bWriteByteMc(ptAddr,ucData); //FlashMc.c
            _FLASH_vOperationFinished(FLASH_STATE_PROGRAM); //FlashMcXS.c
        }
        else
        {
            FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
        }
    }
    else
    {
        FLASH_tFunctionStatus = USER_ACCESS_DISABLED;
    }
    return(bWriteResult);

}

/* FLASH_bRead() */
bool FLASH_bRead(tAddress *ptAddr, tReadData *ptData, uint16_t uiSize)
{
    bool bResult = false;
    FLASH_tFunctionStatus = NO_ERRORS;
    if ( (ptAddr != (tAddress *)NULL) && (ptData != (tReadData *)NULL) )
    {
        if ( uiSize > 0U )
        {
            _FLASH_vOperationStart(FLASH_STATE_READ);
            bResult = FLASH_bReadMc(ptAddr,ptData,uiSize); //FlashMc.c
            _FLASH_vOperationFinished(FLASH_STATE_READ);
        }
        else
        {
            FLASH_tFunctionStatus = PARAMETER_ERROR;
        }
    }
    else
    {
        FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
    }
    return( bResult ); 

}

/* FLASH_bErase() */
bool FLASH_bErase(tAddress *ptAddress)
{
    bool bResult = false;
    FLASH_tFunctionStatus = NO_ERRORS;
    /* is flash access enabled by user application ? */
    if ( FLASH_ucEnableAccess == FLASH_ACCESS_ENABLED )
    {
        if ( ptAddress != (tAddress *)NULL )
        {
            _FLASH_vOperationStart(FLASH_STATE_ERASE);
            bResult = FLASH_bEraseMc(ptAddress);
            _FLASH_vOperationFinished(FLASH_STATE_ERASE);
        }
        else
        {
            FLASH_tFunctionStatus = ADDRESS_OUT_OF_RANGE;
        }
    }
    else
    {
        FLASH_tFunctionStatus = USER_ACCESS_DISABLED;
    }
    return(bResult);

}

/* FLASH_tGetFunctionStatus() */
tStatusCode FLASH_tGetFunctionStatus(void)
{
    return(FLASH_tFunctionStatus);
}
