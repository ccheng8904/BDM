/*******************************************************************************
* Copyright (C) 2019 China Micro Semiconductor Limited Company. All Rights Reserved.
*
* This software is owned and published by:
* CMS LLC, No 2609-10, Taurus Plaza, TaoyuanRoad, NanshanDistrict, Shenzhen, China.
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with CMS
* components. This software is licensed by CMS to be adapted only
* for use in systems utilizing CMS components. CMS shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. CMS is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/

/*****************************************************************************/
/** \file wdt.h
**
** History:
** 2019/12/24  initial version
*****************************************************************************/
#ifndef __WDT_H__
#define __WDT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/* Include files */
/*****************************************************************************/
#include "common.h"
#include "cgc.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/
/*---------------------------------------------------------------------------
 ** WDT clock prescaler (CGC option byte WDTCFG3 / WDTPRE)
---------------------------------------------------------------------------*/
#define		WDT_CLK_DIV_1		(0x00UL)
#define		WDT_CLK_DIV_16		(0x01UL)
#define		WDT_CLK_DIV_256		(0x02UL)

/*---------------------------------------------------------------------------
 ** WDT debug behaviour (CGC WDTCFG1)
---------------------------------------------------------------------------*/
#define     WDT_DEBUG_STOP_MASK     (0x01UL)

/*---------------------------------------------------------------------------
 ** WDT service key: writing WDTE restarts the watchdog timer
---------------------------------------------------------------------------*/
#define     WDT_RESTART_KEY         (0xACU)

/*****************************************************************************
 ** \brief	WDT_ENABLE_DEBUG
 **			WDT在DEBUG模式中使能
 ** \param [in] none
 ** \return  none
 ** \note	Watchdog runs also when the core is halted by the debugger
*****************************************************************************/
#define		WDT_ENABLE_DEBUG()	do{\
									CGC->WDTCFG1 &= (uint8_t)(~WDT_DEBUG_STOP_MASK);\
								}while(0)

/*****************************************************************************
 ** \brief	WDT_DISABLE_DEBUG
 **			WDT在DEBUG模式中关闭
 ** \param [in] none
 ** \return  none
 ** \note	Watchdog stops when the core is halted by the debugger
*****************************************************************************/
#define		WDT_DISABLE_DEBUG()	do{\
									CGC->WDTCFG1 |= (uint8_t)WDT_DEBUG_STOP_MASK;\
								}while(0)

/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/


/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/


/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/

/*****************************************************************************
 ** \brief	 WDT_Restart
 **			 喂WDT (service the watchdog timer)
 ** \param [in] none
 ** \return  none
 ** \note	The watchdog itself (enable, prescaler, overflow time, window) is
 **         defined by the user option byte 0xC0 (WDTON, WDSTBYON, WDCS2..0,
 **         WINDOW1..0, WDTINT), see system_CMS32M67xx.c
*****************************************************************************/
void WDT_Restart(void);

#ifdef __cplusplus
}
#endif

#endif /* __WDT_H__ */
