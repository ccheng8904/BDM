/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          SW_LIB
 *  COMP_ABBREV      FSF_RAM_CON
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file fsramcon.c
 *
 *  \ingroup  <DOXY_GROUP_FSF_RAMCON>
 *
 *  \brief    This file contains the implementation of the Functional Safety Module RAM Consistency Test - FSF RAM CON.
 *
 *  \details  The detailed description can be found in the section \ref FSF_RAM_CON_DESCR.
 */



#include "bsh_stdinc.h"


/*~+:Includes*/
#include "hsup.h"

#include "FuncSafe/fSafe.h"  // needed for fspriv.h (FSF_NR_OF_FLAGS,....)
#include "FuncSafe/fSafeXS.h"// needed for fspriv.h (T_Stack, TisrControl, TisrShortControl)
#include "FuncSafe/fspriv.h" // needed for _FSF_vApplicationErrorNotify()
#include "FuncSafe/fsramcon.h"
#include <assert.h>

/*~+:Definitions and Declarations*/

/*~+:Private Definitions*/

/*~+:Private Variables*/

/*~+:Private Functions*/

/*~+:Global Functions */
#ifdef __QAC_RELEASE__
/* Switch off QAC-Warning for 489:
"Msg(4:0489) Increment or decrement operation performed on pointer.
MISRA-C:2004 Rule 17.4"
*/
#pragma PRQA_MESSAGES_OFF 489
#endif

#ifdef __QAC_RELEASE__
/* Switch off QAC-Warning for 506:
"Msg(2:0506) Dereferencing pointer value that is possibly NULL.
REFERENCE - ISO-6.3.3.2 Semantics"
*/
#pragma PRQA_MESSAGES_OFF 506
#endif

#define SIZE_OF_CHECKED_BYTES  4uL
/*lint -save -e9005 attempt to cast away const/volatile from a pointer or reference */

/*~+:void _FSF_RAM_CON_vCheckBySum(uint8_t ucCheckValue, uint8_t ucSizeInBytes, const uint8_t* pucStartAddress, uint8_t ucId)*/
void _FSF_RAM_CON_vCheckBySum(uint8_t ucCheckValue, uint8_t ucSizeInBytes, const uint8_t* pucStartAddress, uint8_t ucId)
{
   uint8_t ucNewCheckValue;
   uint8_t m;

   ucNewCheckValue = 0;

   //HSUP_vDisableInt();//moved into fsramcon.h (macro FSF_RAM_CON_vCheck)-> is too late ucCheckValue is already on stack
   for(m = 0; m < ucSizeInBytes; m++)
   {
      ucNewCheckValue += *pucStartAddress;

      pucStartAddress++;
   }

   if (ucNewCheckValue != ucCheckValue)
   {
       _FSF_vApplicationErrorNotify((uint8_t)FSF_ERR_CHANNEL_ERROR, (uint8_t)FSF_RAM_CONSISTENCY_CHECK, ucId);
   }

   //HSUP_vEnableInt();//moved itno fsramcon.h
}

/*~+:void _FSF_RAM_CON_vCheckByMirror(uint8_t ucSizeInBytes, const uint8_t* pucStartAddress, const uint8_t* pucMIrrorStartAddress, uint8_t ucId)*/
void _FSF_RAM_CON_vCheckByMirror(uint8_t ucSizeInBytes, const uint8_t* pucStartAddress, const uint8_t* pucMIrrorStartAddress, uint8_t ucId)
{
   uint32_t u32BytesCounter = 0u;
   uint32_t u32CountOfAlignedBytes;

   u32CountOfAlignedBytes = (ucSizeInBytes/SIZE_OF_CHECKED_BYTES) * SIZE_OF_CHECKED_BYTES;

   HSUP_vDisableInt();
   for(; u32BytesCounter < u32CountOfAlignedBytes; u32BytesCounter += SIZE_OF_CHECKED_BYTES)
   {
       if(*(uint32_t*)pucStartAddress != ((*(uint32_t*)pucMIrrorStartAddress) ^ 0xFFFFFFFFuL)) /*lint !e927 cast tested to work correctly, upper layer responsible for 4 bytes alignment!! */
       {
           _FSF_vApplicationErrorNotify((uint8_t)FSF_ERR_CHANNEL_ERROR, (uint8_t)FSF_RAM_CONSISTENCY_CHECK, ucId);
       }
       pucStartAddress += SIZE_OF_CHECKED_BYTES;       /*lint !e9016 "Pointer arithmetic not allowed MisraC201 18.4 [advisory], required here */
       pucMIrrorStartAddress += SIZE_OF_CHECKED_BYTES; /*lint !e9016 "Pointer arithmetic not allowed MisraC201 18.4 [advisory], required here */
   }

   for(; u32BytesCounter < ucSizeInBytes; u32BytesCounter++)
   {
       if((*pucStartAddress) != ((*pucMIrrorStartAddress) ^ 0xFFu))
       {
           _FSF_vApplicationErrorNotify((uint8_t)FSF_ERR_CHANNEL_ERROR, (uint8_t)FSF_RAM_CONSISTENCY_CHECK, ucId);
       }
       pucStartAddress++;
       pucMIrrorStartAddress++;
   }
   HSUP_vEnableInt();
}

/*~+:void _FSF_RAM_CON_vUpdateCheckBySum(uint8_t *pucCheckValue, uint8_t ucSizeInBytes, const uint8_t *pucStartAddress)*/
void _FSF_RAM_CON_vUpdateCheckBySum(uint8_t *pucCheckValue, uint8_t ucSizeInBytes, const uint8_t *pucStartAddress)
{
   uint8_t ucNewCheckValue;
   uint8_t m;

   ucNewCheckValue = 0;

   HSUP_vDisableInt();
   for(m = 0; m < ucSizeInBytes; m++)
   {
      ucNewCheckValue += *pucStartAddress;

      pucStartAddress++;
   }

   *pucCheckValue = ucNewCheckValue;

   HSUP_vEnableInt();
}

/*~+:void _FSF_RAM_CON_vUpdateCheckByMirror(uint8_t ucSizeInBytes, const uint8_t *pucStartAddress, uint8_t *pucMirrorStartAddress)*/
void _FSF_RAM_CON_vUpdateCheckByMirror(uint8_t ucSizeInBytes, const uint8_t *pucStartAddress, uint8_t *pucMirrorStartAddress)
{
    uint32_t u32BytesCounter = 0u;
    uint32_t u32CountOfAlignedBytes;

    u32CountOfAlignedBytes = (ucSizeInBytes/SIZE_OF_CHECKED_BYTES) * SIZE_OF_CHECKED_BYTES;

   HSUP_vDisableInt();
   for(; u32BytesCounter < u32CountOfAlignedBytes; u32BytesCounter += SIZE_OF_CHECKED_BYTES)
   {
       *(uint32_t*)pucMirrorStartAddress = (*(uint32_t*)pucStartAddress) ^ 0xFFFFFFFFuL; /*lint !e927 cast tested to work correctly, upper layer responsible for 4 bytes alignment!! */
       pucStartAddress += SIZE_OF_CHECKED_BYTES;        /*lint !e9016 "Pointer arithmetic not allowed MisraC201 18.4 [advisory], required here */
       pucMirrorStartAddress += SIZE_OF_CHECKED_BYTES;  /*lint !e9016 "Pointer arithmetic not allowed MisraC201 18.4 [advisory], required here */
   }

   for(; u32BytesCounter < ucSizeInBytes; u32BytesCounter++)
   {
       *pucMirrorStartAddress = (*pucStartAddress) ^ 0xFFu;
       pucStartAddress++;
       pucMirrorStartAddress++;
   }

   HSUP_vEnableInt();
}

#ifdef __QAC_RELEASE__
/* Switch on QAC-Warning for 489:
"Msg(4:0489) Increment or decrement operation performed on pointer.
MISRA-C:2004 Rule 17.4"
*/
#pragma PRQA_MESSAGES_ON 489
#endif


/*~+:void _FSF_RAM_CON_vSetUcharBySum(uint8_t *pucVariable, uint8_t *pucCheckValue, uint8_t ucNewVal)*/
void _FSF_RAM_CON_vSetUcharBySum(uint8_t *pucVariable, uint8_t *pucCheckValue, uint8_t ucNewVal)
{
   HSUP_vDisableInt();

   (*(pucCheckValue)) = ((*pucCheckValue) - (*pucVariable)) + (ucNewVal);

   *pucVariable = ucNewVal;

   HSUP_vEnableInt();
}

/*~+:void _FSF_RAM_CON_vSetUintBySum(uint16_t *puiVariable, uint8_t *pucCheckValue, uint16_t uiNewVal)*/
void _FSF_RAM_CON_vSetUintBySum(uint16_t *puiVariable, uint8_t *pucCheckValue, uint16_t uiNewVal)
{
   HSUP_vDisableInt();

   (*(pucCheckValue)) = ((*pucCheckValue) - HSUP_ucGetHighByte(*puiVariable)) + HSUP_ucGetHighByte(uiNewVal);
   (*(pucCheckValue)) = ((*pucCheckValue) - HSUP_ucGetLowByte(*puiVariable)) + HSUP_ucGetLowByte(uiNewVal);
   *puiVariable = uiNewVal;

   HSUP_vEnableInt();
}

/*~+:void _FSF_RAM_CON_vSetBitUintBySum(uint16_t *puiBitField, uint8_t *pucCheckValue, uint16_t uiBitMask)*/
void _FSF_RAM_CON_vSetBitUintBySum(uint16_t *puiBitField, uint8_t *pucCheckValue, uint16_t uiBitMask)
{
   uint16_t uiTemp;

   HSUP_vDisableInt();

   uiTemp = *puiBitField;

   *pucCheckValue = (*pucCheckValue) - HSUP_ucGetHighByte(uiTemp);
   *pucCheckValue = (*pucCheckValue) - HSUP_ucGetLowByte(uiTemp);

   uiTemp |= uiBitMask;

   *pucCheckValue = (*pucCheckValue) + HSUP_ucGetHighByte(uiTemp);
   *pucCheckValue = (*pucCheckValue) + HSUP_ucGetLowByte(uiTemp);

   *puiBitField = uiTemp;

   HSUP_vEnableInt();
}

/*~+:void _FSF_RAM_CON_vResBitUintBySum(uint16_t *puiBitField, uint8_t *pucCheckValue, uint16_t uiBitMask)*/
void _FSF_RAM_CON_vResBitUintBySum(uint16_t *puiBitField, uint8_t *pucCheckValue, uint16_t uiBitMask)
{
   uint16_t uiTemp;

   HSUP_vDisableInt();

   uiTemp = *puiBitField;

   *pucCheckValue = (*pucCheckValue) - HSUP_ucGetHighByte(uiTemp);
   *pucCheckValue = (*pucCheckValue) - HSUP_ucGetLowByte(uiTemp);

   uiTemp &= ~uiBitMask;

   *pucCheckValue = (*pucCheckValue) + HSUP_ucGetHighByte(uiTemp);
   *pucCheckValue = (*pucCheckValue) + HSUP_ucGetLowByte(uiTemp);

   *puiBitField = uiTemp;

   HSUP_vEnableInt();

}

/*lint -restore -e9005*/

#ifdef __QAC_RELEASE__
/* Switch on QAC-Warning for 506:
"Msg(2:0506) Dereferencing pointer value that is possibly NULL.
REFERENCE - ISO-6.3.3.2 Semantics"
*/
#pragma PRQA_MESSAGES_ON 506
#endif
