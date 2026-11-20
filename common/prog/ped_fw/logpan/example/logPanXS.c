/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          SW_LIB
*   COMP_ABBREV      LPAN
*******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/

 /** \file logPanXS.c
   Application specific implementation / configuration for logpan library
                    */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "logpan.h"
#include "logpPriv.h"
#include "SegmentDef.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

typedef uint8_t Tpanel;

typedef struct TlpanKeyEventConfig
{
  Tpanel tKeyMask;        ///< Bit-Mask of Key or grip, if just one key creates the event one bit is set
  uint8_t  aucTiming[3];    ///< Timing for Keyevent [DelayTime, RepeatStartTime, RepeatTime]
  uint8_t  aucEventMapping[LPAN_NUMBER_OF_EV];

}TlpanKeyEventConfig;     ///< Type for configuration of key events


#define CHILLER_GRIP_TIME      30 // 3s
#define TEST_GRIP_TIME        100 //10s
#define SELF_TEST_GRIP_TIME    50 // 5s
#define SALES_ROOM_GRIP_TIME  100 //10s
#define AUTO_REPEAT_START_TIME 10 // 1s
#define AUTO_REPEAT_TIME       10 // 1s

#define LPAN_NUMBER_OF_GRIPS  6


SDEF_SetSegmentRW(TIMER8_100MS)
tTimer8 LPAN_atim8TGripTimer[LPAN_NUMBER_OF_GRIPS];
SDEF_SetSegmentRW_Default()


uint8_t LPAN_aucGripState[LPAN_NUMBER_OF_GRIPS];
const uint8_t LPAN_ucNumberOfGrips = LPAN_NUMBER_OF_GRIPS;

#ifndef UNIT_TEST
const TlpanKeyEventConfig  LPAN_atEventConfigTable[LPAN_NUMBER_OF_GRIPS]=
{
//--------------- Test Program Grip (KEY_COOLER_SUPER 10s) ----------------------- 
 {
   PHYP_SUPER_COOLER_KEY + PHYP_SUPER_FREEZER_KEY,
   {TEST_GRIP_TIME, INVALID, INVALID},
 {
  /* LPAN_RELEASED_EV      */ LPAN_NO_EVENT,
  /* LPAN_PRESSED_EV       */ LPAN_NO_EVENT,
  /* LPAN_DELAY_EV         */ EV_TEST_GRIP,
  /* LPAN_REPEAT_EV        */ LPAN_NO_EVENT,
  /* LPAN_REPEAT_REL_EV    */ EV_TEST_GRIP_RELEASE
 }
},

//--------------- Self Test Grip (KEY_COOLER_SUPER 5s) ----------------------- 
 {
   PHYP_SUPER_COOLER_KEY + PHYP_SUPER_FREEZER_KEY,
   {SELF_TEST_GRIP_TIME, INVALID, INVALID},
 {
  /* LPAN_RELEASED_EV      */ LPAN_NO_EVENT,
  /* LPAN_PRESSED_EV       */ LPAN_NO_EVENT,
  /* LPAN_DELAY_EV         */ EV_SELF_TEST_GRIP,
  /* LPAN_REPEAT_EV        */ LPAN_NO_EVENT,
  /* LPAN_REPEAT_REL_EV    */ EV_SELF_TEST_GRIP_RELEASE
 }

},

//--------------- Chiller Grip (KEY_COOLER_SUPER 3s) ----------------------- 
 {
   PHYP_SUPER_COOLER_KEY,
   {CHILLER_GRIP_TIME, INVALID, INVALID},
   {
  /* LPAN_RELEASED_EV      */ LPAN_NO_EVENT,
  /* LPAN_PRESSED_EV       */ EV_KEY_SUPER_COOLER,
  /* LPAN_DELAY_EV         */ EV_CHILLER_GRIP,
  /* LPAN_REPEAT_EV        */ LPAN_NO_EVENT,
  /* LPAN_REPEAT_REL_EV    */ EV_CHILLER_GRIP_RELEASE
   }
 },


//--------------- Alarm + SalesRoom (KEY_ALARM , KEY_ALARM 10s) ----------------------- 
 {
   PHYP_ALARM_KEY,
   {SALES_ROOM_GRIP_TIME, INVALID,INVALID},
   {
  /* LPAN_RELEASED_EV      */ LPAN_NO_EVENT,
  /* LPAN_PRESSED_EV       */ EV_KEY_ALARM,
  /* LPAN_DELAY_EV         */ EV_SALES_ROOM_GRIP,
  /* LPAN_REPEAT_EV        */ LPAN_NO_EVENT,
  /* LPAN_REPEAT_REL_EV    */ LPAN_NO_EVENT
   }
 },

//--------------- KEY_FREEZER with autorepeat----------------------------------- 
 {
   PHYP_FREEZER_KEY,
   {0, AUTO_REPEAT_START_TIME, AUTO_REPEAT_TIME},
 {
  /* LPAN_RELEASED_EV      */ LPAN_NO_EVENT,
  /* LPAN_PRESSED_EV       */ EV_KEY_FREEZER,
  /* LPAN_DELAY_EV         */ EV_KEY_FREEZER,
  /* LPAN_REPEAT_EV        */ EV_KEY_FREEZER,
  /* LPAN_REPEAT_REL_EV    */ LPAN_NO_EVENT
 }
},

///--------------- Key: KEY_COOLER with autorepeat
 {
   PHYP_COOLER_KEY,
   {0, AUTO_REPEAT_START_TIME, AUTO_REPEAT_TIME},
   {
  /* LPAN_RELEASED_EV      */ LPAN_NO_EVENT,
  /* LPAN_PRESSED_EV       */ EV_KEY_COOLER,
  /* LPAN_DELAY_EV         */ EV_KEY_COOLER,
  /* LPAN_REPEAT_EV        */ EV_KEY_COOLER,
  /* LPAN_REPEAT_REL_EV    */ LPAN_NO_EVENT
   }
 }

};


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
bool _LPAN_bIsKeyMask(uint8_t ucGripId)
{
    return ((LPAN_atEventConfigTable[ucGripId].tKeyMask & PHYP_tGetKeys()) != 0u);
}

bool _LPAN_bIsTimerDown(uint8_t ucGripId)
{
    return (LPAN_atim8TGripTimer[ucGripId] == 0u);
}


void _LPAN_vLoadTimer(uint8_t ucGripId, uint8_t ucTimeId)
{
    LPAN_atim8TGripTimer[ucGripId] = LPAN_atEventConfigTable[ucGripId].aucTiming[ucTimeId];
}


uint8_t _LPAN_ucMapGripEvent(uint8_t ucGripId, uint8_t ucPhysEvent)

{
    return LPAN_atEventConfigTable[ucGripId].aucEventMapping[ucPhysEvent];
}

#endif
