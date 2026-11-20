/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PTEng
*   UNIT-PREFIX      Common
*******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Default stub implementation of undefined functions.
*
*/



/******************************************************************************/
/* INCLUDES                                                                   */
/* (necessary  if this file is provided as stand alone translation unit       */
/* in an application project)                                                 */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>

#include "LibTypes.h"
#include "fSafe.h"
#include "fSafeXS.h"
#include "fspriv.h"
#include "hsup.h"
#include "utility/utility.h"
#include "fsramcon.h"
#include "fs_psm.h"
#include "fSafe.h"
#include "SegmentDef.h"
#include "hsupMc.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile bool _FSF_bIsROMAtOnce_RET;

volatile uint32_t uiCalculateCRC32_CRC32Value;
volatile  uint32_t *volatile uiCalculateCRC32_pData;
volatile  uint32_t   uiCalculateCRC32_pData_argument;
volatile uint16_t uiCalculateCRC32_ui16length;
volatile uint32_t uiCalculateCRC32_RET;

volatile uint32_t _FSF_ui32FinalizeCrcCalculation_crc;
volatile uint32_t _FSF_ui32FinalizeCrcCalculation_RET;

volatile bool _FSF_bCheckINTduringROMCheck_RET;

volatile uint32_t _FSF_ui32ResetCrcCalculation_RET;

volatile uint16 TIM_uiGetCircleMicroSeconds_RET;

volatile uint8_t *volatile UTI_vSetBit_pucBitField;
volatile uint8_t   UTI_vSetBit_pucBitField_argument;
volatile uint16_t UTI_vSetBit_uiBitId;
volatile bool UTI_vSetBit_bOnOff;

volatile uint8_t *volatile _FSF_pucGetStackPointer_RET;

volatile bool _FSF_bIsROMAtOnceAllowed_RET;

volatile  uint8_t *volatile UTI_bGetBit_pucBitField;
volatile  uint8_t   UTI_bGetBit_pucBitField_argument;
volatile uint16_t UTI_bGetBit_uiBitId;
volatile bool UTI_bGetBit_RET;

volatile bool _FSF_bReadExtWatchdog_RET;

volatile uint8_t *volatile FSF_vRamOpercheck_pucRAM;
volatile uint8_t   FSF_vRamOpercheck_pucRAM_argument;
volatile uint8_t *volatile FSF_vRamOpercheck_pucDEXBuffer;
volatile uint8_t   FSF_vRamOpercheck_pucDEXBuffer_argument;
volatile uint8_t FSF_vRamOpercheck_ucPatNoInv;
volatile uint8_t FSF_vRamOpercheck_ucPatInv;

volatile uint8_t _FSF_vApplicationErrorNotify_ucSubCheckId;
volatile uint8_t _FSF_vApplicationErrorNotify_ucChannelId;
volatile uint8_t _FSF_vApplicationErrorNotify_ucUserPar;

uint16_t FSF_auiIsrTime[STUBVAR_ARRAY_SIZE]={};
uint16_t FSF_auiIsrCount[STUBVAR_ARRAY_SIZE]={};
uint16_t FSF_auiShortIsrCount[STUBVAR_ARRAY_SIZE]={};
const TisrControl FSF_atIsrCntrTbl[STUBVAR_ARRAY_SIZE]={};
const TisrShortControl FSF_atIsrShortCntrTbl[STUBVAR_ARRAY_SIZE]={};
const uint8_t FSF_ucNumberOfInterrupts;
const uint8_t FSF_ucNumberOfShortInterrupts;
const uint16_t FSF_uiIsrClockCycle;
const T_ROM_Segment FSF_atRomSegments_0[STUBVAR_ARRAY_SIZE]={};
const T_ROM_Module FSF_atRomModules[STUBVAR_ARRAY_SIZE]={};
const uint8_t FSF_ucRomModuleCount;
const  uint16_t * const FSF_puiCRC_Table;
const T_FSCOM_Channel FSF_sROMChannel;
uint8_t FSF_arMarchDataBuffer[STUBVAR_ARRAY_SIZE]={};
const T_RAM_Segment FSF_atOtRamSegments[STUBVAR_ARRAY_SIZE]={};
const uint8_t FSF_ucOtRamSegCnt;
const T_RAM_Segment FSF_atRamSegments[STUBVAR_ARRAY_SIZE]={};
const T_RAM_Pattern FSF_asPatternTable[STUBVAR_ARRAY_SIZE]={};
const T_RAM_Module FSF_atRamModule;
const T_FSCOM_Channel FSF_sRAMChannel;
const T_Stack _FSF_Stack[1];
const uint16_t _FSF_ui16StackMagic;
uint8_t FSF_ucTaskState;
uint16_t FSF_ui16LastWDTrigger;
uint8_t FSF_ucCH_states[3][4];
T_FSCOM_CheckCycle FSF_sCH_CheckCycleTime[STUBVAR_ARRAY_SIZE]={};
uint16_t FSF_ui16CycLastTime;
uint32_t FSF_ulCheckCycleTimer;
uint16_t FSF_ui16Current;
uint16_t FSF_ui16delta_t;
const  T_FSCOM_Channel * const FSF_psChannelTab[STUBVAR_ARRAY_SIZE]={};
const uint8_t FSF_ucNrOfChannels = 10U;
const uint16_t FSF_ui16WDC1_2_3;
const uint16_t FSF_ui16WDC1_3;
const T_FSCOM_Channel FSF_sDefaultChannel;
const uint8_t FSF_ucPreSchedCycles = 10U;
uint16_t HSUP_ucGetHighByte_Stub_1;
uint16_t HSUP_ucGetHighByte_Stub_RET;

uint16_t HSUP_ucGetLowByte_Stub_1;
uint16_t HSUP_ucGetLowByte_Stub_RET;



/******************************************************************************/
/* FUNCTION POINTER LIST to reference functions under test and                */
/* pointer argument variables use function to prevent linker optimization     */
/******************************************************************************/
static int COMMONSTUB_use_of_pointer_argument_vars(void);

const T_TM_Anchor COMMONSTUB_atTestFuncPtr[] = 
{
    (T_TM_Anchor) COMMONSTUB_use_of_pointer_argument_vars
};


static int COMMONSTUB_use_of_pointer_argument_vars (void)
{
int help = 0;
help += (int) &uiCalculateCRC32_pData_argument;
	help += (int) &UTI_vSetBit_pucBitField_argument;
	help += (int) &UTI_bGetBit_pucBitField_argument;
	help += (int) &FSF_vRamOpercheck_pucRAM_argument;
	help += (int) &FSF_vRamOpercheck_pucDEXBuffer_argument;

return help;
}




/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
#ifndef _FSF_VDOWHENPIECERAMCHECKSTARTED_STUB
void _FSF_vDoWhenPieceRamCheckStarted_Stub(void)
{
}
#endif


#ifndef _FSF_BISROMATONCE_STUB
bool _FSF_bIsROMAtOnce_Stub(void)
{
    return (bool) _FSF_bIsROMAtOnce_RET;
}
#endif


#ifndef FSF_VREGISTERCHECK_STUB
void FSF_vRegistercheck_Stub(void)
{
}
#endif


#ifndef _FSF_RAM_CON_VDOCHECKINFSCOMMODULE_STUB
void _FSF_RAM_CON_vDoCheckInFsComModule_Stub(void)
{
}
#endif


#ifndef _FSF_VREPORTFATAL_STUB
void _FSF_vReportFatal_Stub(void)
{
}
#endif


#ifndef UICALCULATECRC32_STUB
uint32_t uiCalculateCRC32_Stub(uint32_t CRC32Value, const  uint32_t * pData, uint16_t ui16length)
{
    uiCalculateCRC32_CRC32Value = CRC32Value;
    uiCalculateCRC32_pData = (volatile  uint32_t *volatile ) pData;
    uiCalculateCRC32_ui16length = ui16length;
    return (uint32_t) uiCalculateCRC32_RET;
}
#endif


#ifndef _FSF_RAM_CON_VDOCHECKINROMMODULE_STUB
void _FSF_RAM_CON_vDoCheckInRomModule_Stub(void)
{
}
#endif


#ifndef _FSF_RAM_CON_VDOUPDATECHECKINFSCOMMODULE_STUB
void _FSF_RAM_CON_vDoUpdateCheckInFsComModule_Stub(void)
{
}
#endif


#ifndef _FSF_UI32FINALIZECRCCALCULATION_STUB
uint32_t _FSF_ui32FinalizeCrcCalculation_Stub(uint32_t crc)
{
    _FSF_ui32FinalizeCrcCalculation_crc = crc;
    return (uint32_t) _FSF_ui32FinalizeCrcCalculation_RET;
}
#endif


#ifndef _FSF_VTRIGGEREXTWATCHDOG_STUB
void _FSF_vTriggerExtWatchdog_Stub(void)
{
}
#endif


#ifndef _FSF_BCHECKINTDURINGROMCHECK_STUB
bool _FSF_bCheckINTduringROMCheck_Stub(void)
{
    return (bool) _FSF_bCheckINTduringROMCheck_RET;
}
#endif


#ifndef _FSF_UI32RESETCRCCALCULATION_STUB
uint32_t _FSF_ui32ResetCrcCalculation_Stub(void)
{
    return (uint32_t) _FSF_ui32ResetCrcCalculation_RET;
}
#endif


#ifndef TIM_UIGETCIRCLEMICROSECONDS_STUB
uint16 TIM_uiGetCircleMicroSeconds_Stub(void)
{
    return (uint16) TIM_uiGetCircleMicroSeconds_RET;
}
#endif


#ifndef UTI_VSETBIT_STUB
void UTI_vSetBit_Stub(uint8_t * pucBitField, uint16_t uiBitId, bool bOnOff)
{
    UTI_vSetBit_pucBitField = (volatile uint8_t *)pucBitField;
    UTI_vSetBit_uiBitId = uiBitId;
    UTI_vSetBit_bOnOff = bOnOff;
}
#endif


#ifndef _FSF_PUCGETSTACKPOINTER_STUB
uint8_t *  _FSF_pucGetStackPointer_Stub(void)
{
    return (uint8_t * ) _FSF_pucGetStackPointer_RET;
}
#endif


#ifndef _FSF_VNOTIFYROMCHECKCOMPLETE_STUB
void _FSF_vNotifyROMCheckComplete_Stub(void)
{
}
#endif


#ifndef _FSF_VTRIGGERINTERNALWATCHDOG_STUB
void _FSF_vTriggerInternalWatchdog_Stub(void)
{
}
#endif


#ifndef FSF_VREGOPERCHECK_STUB
void FSF_vRegOpercheck_Stub(void)
{
}
#endif


#ifndef HSUP_VENABLEINT_STUB
void HSUP_vEnableInt_Stub(void)
{
}
#endif


#ifndef _FSF_RAM_CON_VDOUPDATECHECKINROMMODULE_STUB
void _FSF_RAM_CON_vDoUpdateCheckInRomModule_Stub(void)
{
}
#endif


#ifndef _FSF_BISROMATONCEALLOWED_STUB
bool _FSF_bIsROMAtOnceAllowed_Stub(void)
{
    return (bool) _FSF_bIsROMAtOnceAllowed_RET;
}
#endif


#ifndef UTI_BGETBIT_STUB
bool UTI_bGetBit_Stub(const  uint8_t * pucBitField, uint16_t uiBitId)
{
    UTI_bGetBit_pucBitField = (volatile  uint8_t *volatile ) pucBitField;
    UTI_bGetBit_uiBitId = uiBitId;
    return (bool) UTI_bGetBit_RET;
}
#endif


#ifndef FSF_VRAMCHECK_STUB
void FSF_vRamcheck_Stub(void)
{
}
#endif


#ifndef _FSF_BREADEXTWATCHDOG_STUB
bool _FSF_bReadExtWatchdog_Stub(void)
{
    return (bool) _FSF_bReadExtWatchdog_RET;
}
#endif


#ifndef FSF_VRAMOPERCHECK_STUB
void FSF_vRamOpercheck_Stub(uint8_t * pucRAM, uint8_t * pucDEXBuffer, uint8_t ucPatNoInv, uint8_t ucPatInv)
{
    FSF_vRamOpercheck_pucRAM = (volatile uint8_t *)pucRAM;
    FSF_vRamOpercheck_pucDEXBuffer = (volatile uint8_t *)pucDEXBuffer;
    FSF_vRamOpercheck_ucPatNoInv = ucPatNoInv;
    FSF_vRamOpercheck_ucPatInv = ucPatInv;
}
#endif


#ifndef _FSF_RAM_CON_VSETERRCOUNTER_STUB
void _FSF_RAM_CON_vSetErrCounter_Stub(void)
{
}
#endif


#ifndef _FSF_VAPPLICATIONERRORNOTIFY_STUB
void _FSF_vApplicationErrorNotify_Stub(uint8_t ucSubCheckId, uint8_t ucChannelId, uint8_t ucUserPar)
{
    _FSF_vApplicationErrorNotify_ucSubCheckId = ucSubCheckId;
    _FSF_vApplicationErrorNotify_ucChannelId = ucChannelId;
    _FSF_vApplicationErrorNotify_ucUserPar = ucUserPar;
}
#endif


#ifndef _FSF_VAPPLICATIONSTARTUP_STUB
void _FSF_vApplicationStartUp_Stub(void)
{
}
#endif


#ifndef HSUP_VDISABLEINT_STUB
void HSUP_vDisableInt_Stub(void)
{
}
#endif


#ifndef _FSF_VDOWHENPIECERAMCHECKFINISHED_STUB
void _FSF_vDoWhenPieceRamCheckFinished_Stub(void)
{
}
#endif


#ifndef HSUP_UCGETHIGHBYTE_STUB
uint HSUP_ucGetHighByte_Stub(uint stub_param_1)
{
HSUP_ucGetHighByte_Stub_1 = stub_param_1;
return HSUP_ucGetHighByte_Stub_RET;

}
#endif

#ifndef HSUP_UCGETLOWBYTE_STUB
uint HSUP_ucGetLowByte_Stub(uint stub_param_1)
{
HSUP_ucGetLowByte_Stub_1 = stub_param_1;
return HSUP_ucGetLowByte_Stub_RET;

}
#endif


