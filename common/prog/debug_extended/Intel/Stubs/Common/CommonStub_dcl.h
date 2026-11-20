/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PTEng
*   COMP_ABBREV      Common
*******************************************************************************/


#ifndef COMMONSTUB_DCL_STUB_H
#define COMMONSTUB_DCL_STUB_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
*
*   \brief    Common Stub function declaration for dynamic SW unit test.
*
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/* (necessary  if this file is provided as stand alone translation unit       */
/* in an application project)                                                 */
/******************************************************************************/
#include "LibTypes.h"
#include "SegmentDef.h"
#include "debug/debug_api.h"
#include "debug/debug_api_config.h"
#include "debug_extended/api.h"
#include "debug_extended/error.h"
#include "debug_extended/interface.h"
#include "debug_extended/protocol.h"
#include "hsup.h"
#include "hsupMc.h"
#include "mem_utility/ring_buffer.h"
#include "ped_fw/stdcrc/CrcCom.h"
#include "sts_sim_test_uty.h"

/* PUBLIC STUB FUNCTION DECLARATIONS                                          */
/******************************************************************************/
extern void DBG_writeChar_Stub(char ch);
extern void DBG_writeStringLen_Stub(const char* str, uint16_t len);
extern char DBG_readChar_Stub(void);
extern void HSUP_vDisableInt_Stub(void);
extern void HSUP_vEnableInt_Stub(void);
extern void HSUP_vGenerateReset_Stub(void);
extern uint32 TIM_u32GetCircleMicroSeconds_Stub(void);
extern uint8_t MUART_init_Stub(struct MUART_Handle *handle, const struct MUART_Channel *channel, const struct MUART_Config *cfg);
extern uint8_t MUART_enable_Stub(struct MUART_Handle *handle);
extern uint8_t MUART_enableEvent_Stub(struct MUART_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event);
extern enum MCAL_Error MUART_write_Stub(struct MUART_Handle *handle, const uint8_t *buffer, uint16_t length);
extern uint8_t MUART_writeStr_Stub(struct MUART_Handle *handle, const char *str);
extern enum MCAL_Error MUART_read_Stub(struct MUART_Handle *handle, uint8_t *buffer, uint16_t length);
extern void MCAL_initCallback_Stub(MCAL_Callback_t *callback, MCAL_CallbackFunction_t callbackFunction, void *objPtr);

#ifdef __cplusplus
}
#endif

#endif // COMMONSTUB_DCL_STUB_H
