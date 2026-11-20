/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          PP PED Framework
 *  COMP_ABBREV      UTI
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief    Implementation of utility library (common functions)
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "utility.h"


void UTI_vMemFill(uint8_t * const pucDestination, const uint8_t ucFillValue, uint16_t uiLength)
{
   while (uiLength > 0U)
   {
      uiLength--;
      pucDestination[uiLength] = ucFillValue;
   }
}


void UTI_vMemCopy(const uint8_t * const pucSource, uint8_t * const pucDestination, uint16_t uiLength)
{
   while (uiLength > 0U)
   {
      uiLength--;
      pucDestination[uiLength] = pucSource[uiLength];
   }
}


bool UTI_bIsMemEqual(const uint8_t * const pucSource1, const uint8_t * const pucSource2, uint16_t uiLength)
{
   const uint8_t* puc1 = pucSource1;
   const uint8_t* puc2 = pucSource2;

   while (uiLength > 0U)
   {
      uiLength--;
      if (*puc1 != *puc2)
      {
         return false;
      }
      puc1++;
      /* Msg(4:0489) Increment or decrement operation performed on pointer.
         Code is tested, not dangerous
      */
      puc2++;
      /* Msg(4:0489) Increment or decrement operation performed on pointer.
         Code is tested, not dangerous
      */
   }
   return true;
}


void UTI_vSetBit(uint8_t* pucBitField, uint16_t uiBitId, bool bOnOff)
{
   uint8_t ucByte;
   uint8_t ucBitMask;
      ucByte = (uint8_t)(uiBitId / (uint16_t)8U);
   pucBitField +=ucByte; //lint !e9016  Pointer arithmetic MisraC2012 18.4 advisory
   ucBitMask = UTI_ucShiftLeft(1u, (uint8_t)(uiBitId % (uint16_t)8U));
   if (bOnOff)
   {
      *pucBitField |= ucBitMask;
   }
   else
   {
      *pucBitField &= (uint8_t)(~ucBitMask);
   }
}


bool UTI_bGetBit(const uint8_t* pucBitField, uint16_t uiBitId)
{
   uint8_t ucByte;
   uint8_t ucBit;
   uint8_t ucBitMask;
   ucByte = (uint8_t)(uiBitId / (uint16_t)8U);
   ucBit  = (uint8_t)(uiBitId % (uint16_t)8U);
   pucBitField += ucByte; //lint !e9016  Pointer arithmetic MisraC2012 18.4 advisory
   ucBitMask = (uint8_t)(1U << ucBit);
   return ( ((*pucBitField & ucBitMask) == 0u) ? false : true );
}


void UTI_vSetBitField(uint8_t* pucBitField, uint16_t uiBitFieldId, uint8_t ucBitCount, uint8_t ucValue)
{
   uint8_t ucBitPos;
   uint8_t* pucPtr;
   
   ucBitPos = (uint8_t)(uiBitFieldId * ucBitCount); //calculate absolute bit position
   //lint -e{9016}  Pointer arithmetic MisraC2012 18.4 advisory
   pucPtr = pucBitField + (uint8_t)(ucBitPos / (uint8_t)8U);  //calculate pointer in bitfield
   ucBitPos  = ucBitPos % (uint8_t)8U;                      //calculate start bit position in ucBytePos
   *pucPtr |=          UTI_ucShiftLeft( ucValue,ucBitPos);  //sets bits
   *pucPtr &= (uint8_t)(~UTI_ucShiftLeft((uint8_t)~ucValue, ucBitPos)); //resets bits
   pucPtr++;
   *pucPtr |=          UTI_ucShiftRight( ucValue,(uint8_t)(8U-ucBitPos)); //sets bits
   *pucPtr &= (uint8_t)(~UTI_ucShiftRight((uint8_t)~ucValue,(uint8_t)(8U-ucBitPos)));//resets bits
}


uint8_t UTI_ucGetBitField(const uint8_t* pucBitField, uint16_t uiBitFieldId, uint8_t ucBitCount)
{
   uint8_t ucData;
   uint8_t ucBitPos;
   const uint8_t* pucPtr;
   
   ucBitPos = (uint8_t)(uiBitFieldId * ucBitCount); //calculate absolute bit position
   //lint -e{9016}  Pointer arithmetic MisraC2012 18.4 advisory
   pucPtr = pucBitField + (uint8_t)(ucBitPos / (uint8_t)8U);  //calculate pointer in bitfield
   ucBitPos  = ucBitPos % (uint8_t)8U;                        //calculate start bit position in ucBytePos


   ucData = UTI_ucShiftRight(*pucPtr,ucBitPos);

   pucPtr++;

   ucData |=UTI_ucShiftLeft(*pucPtr,(uint8_t)(8U-ucBitPos));
   ucData &=(uint8_t)(~UTI_ucShiftLeft(0xFFU,ucBitCount));

   return ucData;
}


void UTI_vIIRFilter(uint16_t *puiAccu, uint8_t ucValue, uint8_t ucDampPlus, uint8_t ucDampMinus)
{
   uint16_t uiAccu = *puiAccu;

   uint16_t uiValue=(uint16_t)(ucValue) << 8U;
   if ( uiValue >= uiAccu )
   {
      if (ucDampPlus != 0U)
      {
         uiAccu += (uint16_t)(uiValue-uiAccu) / ucDampPlus;
      }
   }
   else
   {
      if (ucDampMinus != 0U)
      {
         uiAccu -= (uint16_t)(uiAccu - uiValue) / ucDampMinus;
      }
   }
   *puiAccu = uiAccu;
}

void UTI_vIIRFilter16Bit(uint32_t *pulAccu, uint16_t uiValue, uint8_t ucDampPlus, uint8_t ucDampMinus)
{
  uint32_t accu;
  uint32_t value;

  accu = *pulAccu;
  value = (uint32_t)(uiValue) << (uint32_t)16U;

  if ( value >= accu )
  {
    if (ucDampPlus != 0U)
    {
      accu += (value - accu) / ucDampPlus;
    }
  }
  else
  {
    if (ucDampMinus != 0U)
    {
      accu -= (accu - value) / ucDampMinus;
    }
  }
  *pulAccu = accu;
}


