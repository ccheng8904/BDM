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
#include "CommonStub_dcl.h"

#include "firmware_update/mal/mal_statuses.h"
#include "stdcrc/StdCrc.h"
#include "firmware_update/mal/mal_driver.h"
#include "mem_drv.h"
#include "mem_utility/ring_buffer.h"
#include "utility.h"
#include "hwdt.h"
#include "hsup.h"
#include "firmware_update/mem_drv/com_mal.h"
#include "firmware_update/mem_drv/mem_drv.h"
#include "firmware_update/mem_drv/intflash_drv.h"
#include "LibTypes.h"
#include "hsupMc.h"
#include "SegmentDef.h"


/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/
#define STUBVAR_ARRAY_SIZE  10U

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS to be used by TEng                                 */
/******************************************************************************/
volatile struct RBUF_ringBuffer *volatile RBUF_writeBuff_rb;
volatile struct RBUF_ringBuffer   RBUF_writeBuff_rb_argument;
volatile  uint8_t *volatile RBUF_writeBuff_data;
volatile  uint8_t   RBUF_writeBuff_data_argument;
volatile RBUF_size_t RBUF_writeBuff_len;
volatile bool RBUF_writeBuff_RET;

volatile struct RBUF_ringBuffer *volatile RBUF_writeBuffN_rb;
volatile struct RBUF_ringBuffer   RBUF_writeBuffN_rb_argument;
volatile  uint8_t *volatile RBUF_writeBuffN_data;
volatile  uint8_t   RBUF_writeBuffN_data_argument;
volatile RBUF_size_t RBUF_writeBuffN_len;
volatile RBUF_size_t RBUF_writeBuffN_RET;

volatile uint32_t CRC_Calc_len;
volatile  uint8_t *volatile CRC_Calc_data;
volatile  uint8_t   CRC_Calc_data_argument;
volatile CRC_crc_t CRC_Calc_RET;

volatile CRC_crc_t CRC_Finish_crc;
volatile CRC_crc_t CRC_Finish_RET;

volatile struct RBUF_ringBuffer *volatile RBUF_init_rb;
volatile struct RBUF_ringBuffer   RBUF_init_rb_argument;
volatile uint8_t *volatile RBUF_init_buffer;
volatile uint8_t   RBUF_init_buffer_argument;
volatile RBUF_size_t RBUF_init_size;

volatile  struct RBUF_ringBuffer *volatile RBUF_getBytesToRead_rb;
volatile  struct RBUF_ringBuffer   RBUF_getBytesToRead_rb_argument;
volatile RBUF_size_t RBUF_getBytesToRead_RET;

volatile uint32_t COMA_read_addr;
volatile uint32_t COMA_read_size;
volatile uint8_t *volatile COMA_read_buffer;
volatile uint8_t   COMA_read_buffer_argument;
volatile bool COMA_read_RET;

volatile struct RBUF_ringBuffer *volatile RBUF_readBuffN_rb;
volatile struct RBUF_ringBuffer   RBUF_readBuffN_rb_argument;
volatile uint8_t *volatile RBUF_readBuffN_buff;
volatile uint8_t   RBUF_readBuffN_buff_argument;
volatile RBUF_size_t RBUF_readBuffN_len;
volatile RBUF_size_t RBUF_readBuffN_RET;

volatile struct RBUF_ringBuffer *volatile RBUF_reset_rb;
volatile struct RBUF_ringBuffer   RBUF_reset_rb_argument;

volatile CRC_crc_t CRC_Update_crc;
volatile uint32_t CRC_Update_len;
volatile  uint8_t *volatile CRC_Update_data;
volatile  uint8_t   CRC_Update_data_argument;
volatile CRC_crc_t CRC_Update_RET;

volatile CRC_crc_t CRC_Init_RET;


uint32_t DRVIF_checkErase_Stub_1;
uint32_t DRVIF_checkErase_Stub_2;
uint32_t DRVIF_checkErase_Stub_RET;

uint32_t DRVIF_erase_Stub_1;
uint32_t DRVIF_erase_Stub_2;
uint32_t DRVIF_erase_Stub_RET;

uint32_t DRVIF_isAddrRangeValid_Stub_1;
uint32_t DRVIF_isAddrRangeValid_Stub_2;
bool DRVIF_isAddrRangeValid_Stub_RET;

bool DRVIF_isBusy_Stub_RET;

bool DRVIF_isError_Stub_RET;

uint32_t DRVIF_write_Stub_1;
uint32_t DRVIF_write_Stub_2;
const uint8_t* DRVIF_write_Stub_3;
uint8_t DRVIF_write_Stub_3_argument0;
uint32_t DRVIF_write_Stub_RET;

uint32_t DRVIF_read_Stub_1;
uint32_t DRVIF_read_Stub_2;
uint8_t* DRVIF_read_Stub_3;
uint8_t DRVIF_read_Stub_3_argument0;
bool DRVIF_read_Stub_RET;



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
help += (int) &RBUF_writeBuff_rb_argument;
	help += (int) &RBUF_writeBuff_data_argument;
	help += (int) &RBUF_writeBuffN_rb_argument;
	help += (int) &RBUF_writeBuffN_data_argument;
	help += (int) &CRC_Calc_data_argument;
	help += (int) &RBUF_init_rb_argument;
	help += (int) &RBUF_init_buffer_argument;
	help += (int) &RBUF_getBytesToRead_rb_argument;
	help += (int) &COMA_read_buffer_argument;
	help += (int) &RBUF_readBuffN_rb_argument;
	help += (int) &RBUF_readBuffN_buff_argument;
	help += (int) &RBUF_reset_rb_argument;
	help += (int) &CRC_Update_data_argument;

return help;
}


struct MEMDRV_driver_s XXX_driver =
{
    .allowInterrupts     = false,
    .timeOutPwr2         = MEMDRV_TMO_32S,
    .writeBlockSize      = sizeof(uint32_t),
    .init                = DRVIF_initFlash_Stub,
    .deinit              = DRVIF_deinitFlash_Stub,
    .isAddrRangeValid    = DRVIF_isAddrRangeValid_Stub,
    .reset               = DRVIF_resetFlash_Stub,
    .isBusy              = DRVIF_isBusy_Stub,
    .isError             = DRVIF_isError_Stub,
    .write               = DRVIF_write_Stub,
    .erase               = DRVIF_erase_Stub,
    .read                = DRVIF_read_Stub,
    .checkErase          = DRVIF_checkErase_Stub
};


const struct MEMDRV_driver_s DRVIF_driver =
{
    .allowInterrupts     = true,
    .timeOutPwr2         = MEMDRV_TMO_32S,
    .writeBlockSize      = sizeof(uint32_t),
    .init                = DRVIF_initFlash_Stub,
    .deinit              = DRVIF_deinitFlash_Stub,
    .isAddrRangeValid    = DRVIF_isAddrRangeValid_Stub,
    .reset               = DRVIF_resetFlash_Stub,
    .isBusy              = DRVIF_isBusy_Stub,
    .isError             = DRVIF_isError_Stub,
    .write               = DRVIF_write_Stub,
    .erase               = DRVIF_erase_Stub,
    .read                = DRVIF_read_Stub,
    .checkErase          = DRVIF_checkErase_Stub
};


/******************************************************************************/
/* PUBLIC STUB FUNCTION DEFINITIONS                                           */
/******************************************************************************/
#ifndef RBUF_WRITEBUFF_STUB
bool RBUF_writeBuff_Stub(struct RBUF_ringBuffer * rb, const  uint8_t * data, RBUF_size_t len)
{
    RBUF_writeBuff_rb = (volatile struct RBUF_ringBuffer *)rb;
    RBUF_writeBuff_data = (volatile  uint8_t *volatile ) data;
    RBUF_writeBuff_len = len;
    return (bool) RBUF_writeBuff_RET;
}
#endif


#ifndef HSUP_VDISABLEINT_STUB
void HSUP_vDisableInt_Stub(void)
{
}
#endif


#ifndef HSUP_VENABLEINT_STUB
void HSUP_vEnableInt_Stub(void)
{
}
#endif


#ifndef RBUF_WRITEBUFFN_STUB
RBUF_size_t RBUF_writeBuffN_Stub(struct RBUF_ringBuffer * rb, const  uint8_t * data, RBUF_size_t len)
{
    RBUF_writeBuffN_rb = (volatile struct RBUF_ringBuffer *)rb;
    RBUF_writeBuffN_data = (volatile  uint8_t *volatile ) data;
    RBUF_writeBuffN_len = len;
    return (RBUF_size_t) RBUF_writeBuffN_RET;
}
#endif


#ifndef CRC_CALC_STUB
CRC_crc_t CRC_Calc_Stub(uint32_t len, const  uint8_t * data)
{
    CRC_Calc_len = len;
    CRC_Calc_data = (volatile  uint8_t *volatile ) data;
    return (CRC_crc_t) CRC_Calc_RET;
}
#endif


#ifndef HWDT_VTRIGGER_STUB
void HWDT_vTrigger_Stub(void)
{
}
#endif


#ifndef CRC_FINISH_STUB
CRC_crc_t CRC_Finish_Stub(CRC_crc_t crc)
{
    CRC_Finish_crc = crc;
    return (CRC_crc_t) CRC_Finish_RET;
}
#endif


#ifndef RBUF_INIT_STUB
void RBUF_init_Stub(struct RBUF_ringBuffer * rb, uint8_t * buffer, RBUF_size_t size)
{
    RBUF_init_rb = (volatile struct RBUF_ringBuffer *)rb;
    RBUF_init_buffer = (volatile uint8_t *)buffer;
    RBUF_init_size = size;
}
#endif


#ifndef RBUF_GETBYTESTOREAD_STUB
RBUF_size_t RBUF_getBytesToRead_Stub(const  struct RBUF_ringBuffer * rb)
{
    RBUF_getBytesToRead_rb = (volatile  struct RBUF_ringBuffer *volatile ) rb;
    return (RBUF_size_t) RBUF_getBytesToRead_RET;
}
#endif


#ifndef COMA_READ_STUB
bool COMA_read_Stub(uint32_t addr, uint32_t size, uint8_t * buffer)
{
    COMA_read_addr = addr;
    COMA_read_size = size;
    COMA_read_buffer = (volatile uint8_t *)buffer;
    return (bool) COMA_read_RET;
}
#endif


#ifndef RBUF_READBUFFN_STUB
RBUF_size_t RBUF_readBuffN_Stub(struct RBUF_ringBuffer * rb, uint8_t * buff, RBUF_size_t len)
{
    RBUF_readBuffN_rb = (volatile struct RBUF_ringBuffer *)rb;
    RBUF_readBuffN_buff = (volatile uint8_t *)buff;
    RBUF_readBuffN_len = len;
    return (RBUF_size_t) RBUF_readBuffN_RET;
}
#endif


#ifndef RBUF_RESET_STUB
void RBUF_reset_Stub(struct RBUF_ringBuffer * rb)
{
    RBUF_reset_rb = (volatile struct RBUF_ringBuffer *)rb;
}
#endif


#ifndef CRC_UPDATE_STUB
CRC_crc_t CRC_Update_Stub(CRC_crc_t crc, uint32_t len, const  uint8_t * data)
{
    CRC_Update_crc = crc;
    CRC_Update_len = len;
    CRC_Update_data = (volatile  uint8_t *volatile ) data;
    return (CRC_crc_t) CRC_Update_RET;
}
#endif


#ifndef CRC_INIT_STUB
CRC_crc_t CRC_Init_Stub(void)
{
    return (CRC_crc_t) CRC_Init_RET;
}
#endif


#ifndef DRVIF_CHECKERASE_STUB
uint32_t DRVIF_checkErase_Stub(uint32_t stub_param_1, uint32_t stub_param_2)
{
DRVIF_checkErase_Stub_1 = stub_param_1;
DRVIF_checkErase_Stub_2 = stub_param_2;
return DRVIF_checkErase_Stub_RET;

}
#endif

#ifndef DRVIF_ERASE_STUB
uint32_t DRVIF_erase_Stub(uint32_t stub_param_1, uint32_t stub_param_2)
{
DRVIF_erase_Stub_1 = stub_param_1;
DRVIF_erase_Stub_2 = stub_param_2;
return DRVIF_erase_Stub_RET;

}
#endif

#ifndef DRVIF_DEINITFLASH_STUB
void DRVIF_deinitFlash_Stub(void)
{

}
#endif

#ifndef DRVIF_INITFLASH_STUB
void DRVIF_initFlash_Stub(void)
{

}
#endif

#ifndef DRVIF_ISADDRRANGEVALID_STUB
bool DRVIF_isAddrRangeValid_Stub(uint32_t stub_param_1, uint32_t stub_param_2)
{
DRVIF_isAddrRangeValid_Stub_1 = stub_param_1;
DRVIF_isAddrRangeValid_Stub_2 = stub_param_2;
return DRVIF_isAddrRangeValid_Stub_RET;

}
#endif

#ifndef DRVIF_ISBUSY_STUB
volatile uint8_t DRVIF_isBusyCounter;
bool DRVIF_isBusy_Stub(void)
{
    return --DRVIF_isBusyCounter != 0u;
}
#endif

#ifndef DRVIF_ISERROR_STUB
bool DRVIF_isError_Stub(void)
{
return DRVIF_isError_Stub_RET;

}
#endif

#ifndef DRVIF_RESETFLASH_STUB
void DRVIF_resetFlash_Stub(void)
{

}
#endif

#ifndef DRVIF_WRITE_STUB
uint32_t DRVIF_write_Stub(uint32_t stub_param_1, uint32_t stub_param_2, const uint8_t* stub_param_3)
{
DRVIF_write_Stub_1 = stub_param_1;
DRVIF_write_Stub_2 = stub_param_2;
DRVIF_write_Stub_3 = stub_param_3;
return DRVIF_write_Stub_RET;

}
#endif

#ifndef DRVIF_READ_STUB
bool DRVIF_read_Stub(uint32_t stub_param_1, uint32_t stub_param_2, uint8_t* stub_param_3)
{
DRVIF_read_Stub_1 = stub_param_1;
DRVIF_read_Stub_2 = stub_param_2;
DRVIF_read_Stub_3 = stub_param_3;
return DRVIF_read_Stub_RET;

}
#endif


