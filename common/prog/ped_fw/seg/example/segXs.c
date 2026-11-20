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
 *  COMP_ABBREV      SEG
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
* \ingroup  seglib.c
* \brief    Implemantation of segxs.c
*/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

#include "utility/utility.h"
#include "seg.h"
#include "segpriv.h"
#include "beat.h"
#include "mpx.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define SEG_GROUP_PROGRAM_POSITION_0            0         ///<Position of program 0 leds
#define SEG_GROUP_PROGRAM_POSITION_1            1         ///<Position of program 1 leds
#define SEG_GROUP_PROGRAM_POSITION_5            2         ///<Position of program 5 leds
#define SEG_GROUP_PROGRAM_POSITION_6            3         ///<Position of program 6 leds
#define SEG_GROUP_PROGRAM_POSITION_7            4         ///<Position of program 7 leds
#define SEG_GROUP_PROGRAM_POSITION_8            5         ///<Position of program 8 leds
#define SEG_GROUP_PROGRAM_POSITION_12           6         ///<Position of program 12 leds
#define SEG_GROUP_PROGRAM_POSITION_13           7         ///<Position of program 13 leds
#define SEG_GROUP_JOGDIAL_BOSCH                 8         ///<Position of leds around jogdial
#define SEG_GROUP_JOGDIAL_SIEMENS               9         ///<Position of leds around jogdial variant Siemens
#define SEG_GROUP_ALL_PROGRAM_POSITIONS_BOSCH   10        ///<Position of program leds
#define SEG_GROUP_ALL_PROGRAM_POSITIONS_SIEMENS 11        ///<Position of program leds variant Siemens
#define SEG_START_OF_CONVERSION_TAB             (' ')       ///< Start of Conversion Character table

///Calculates physical ID for segment groups
#define SEG_GROUP(group)  ((group) + SEG_NUMBER_OF_SEGMENTS)

#define SEG_NUMBER_OF_GROUPS          12          ///<Number of segment groups

#define BLINKING_ACTIVE                          ///<Comment out if blinking is unused
#ifdef BLINKING_ACTIVE
uint8_t SEG_pucBlinkField[SEG_BUFFER_LENGTH];  //buffer of blinking sement
#endif
uint8_t SEG_pucTextBuffer[SEG_NUMBER_OF_DIGITS];

static uint8_t SEG_ucLastBeatValue;

#define xxx                     INVALID          ///<Used in SEG_pTDigitsSegmentRoutingTable to mark not implemented segment

#define SEG_BEAT_COUNT                8          ///<Periodicity of beat counter. Need to be the same as BEAT_COUNT.


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
///Definition of position in the group array SEG_pptSegGroups

/*Enumeration of all physical segments for F24*/
///Enumeration of all physical segments
enum
{
  SEG_00,       ///<Column 1, Row 1
  SEG_01,       ///<Column 1, Row 2
  SEG_02,       ///<Column 1, Row 3
  SEG_03,       ///<Column 1, Row 4
  SEG_04,       ///<Column 1, Row 5
  SEG_05,       ///<Column 1, Row 6
  SEG_06,       ///<Column 1, Row 7

  SEG_07,       ///<Column 2, Row 1
  SEG_08,       ///<Column 2, Row 2
  SEG_09,       ///<Column 2, Row 3
  SEG_10,       ///<Column 2, Row 4
  SEG_11,       ///<Column 2, Row 5
  SEG_12,       ///<Column 2, Row 6
  SEG_13,       ///<Column 2, Row 7

  SEG_14,       ///<Column 3, Row 1
  SEG_15,       ///<Column 3, Row 2
  SEG_16,       ///<Column 3, Row 3
  SEG_17,       ///<Column 3, Row 4
  SEG_18,       ///<Column 3, Row 5
  SEG_19,       ///<Column 3, Row 6
  SEG_20,       ///<Column 3, Row 7

  SEG_21,       ///<Column 4, Row 1
  SEG_22,       ///<Column 4, Row 2
  SEG_23,       ///<Column 4, Row 3
  SEG_24,       ///<Column 4, Row 4
  SEG_25,       ///<Column 4, Row 5
  SEG_26,       ///<Column 4, Row 6
  SEG_27,       ///<Column 4, Row 7

  SEG_28,       ///<Column 5, Row 1
  SEG_29,       ///<Column 5, Row 2
  SEG_30,       ///<Column 5, Row 3
  SEG_31,       ///<Column 5, Row 4
  SEG_32,       ///<Column 5, Row 5
  SEG_33,       ///<Column 5, Row 6
  SEG_34        ///<Column 5, Row 7
};

/*Const variables*/
const uint8_t SEG_ucNumberOfDigits = SEG_NUMBER_OF_DIGITS;
//const uint8_t SEG_ucStartOfConversionTable = ' ';
const uint8_t SEG_ucNumberOfBlinkBytes     = SEG_BUFFER_LENGTH;
const uint16_t  SEG_uiNumberOfSegments       = SEG_NUMBER_OF_SEGMENTS;
#ifdef UNIT_TEST
uint8_t  SEG_ucTextIndex=0x00;
uint8_t SEG_ucSegmentCountForDigits=0x18;
#endif

/*const TDigits SEG_atDigitsSegmentConversionTable[]*/

const TDigits SEG_atDigitsSegmentConversionTable[] =  // ASCII table for 7-segment display
{
//  ' '     '!'     '"'     '#'    'ڿ'     '%'     '&'     '''     '('     ')'
  0x0000, 0x0006, 0x0022, 0x0036, 0x0054, 0x0024, 0x007F, 0x0020, 0x0039, 0x000F,
//  '�'     '+'     ','     '-'    '.'      '/'     '0'     '1'     '2'     '3'
  0x0050, 0x0046, 0x0080, 0x0040, 0x0002, 0x0052, 0x003F, 0x0406, 0x00DB, 0x00CF,
//  '4'     '5'     '6'     '7'    '8'      '9'     ':'     ';'     '<'     '='
  0x00E6, 0x00ED, 0x00FD, 0x1401, 0x00FF, 0x00EF, 0x0006, 0x0006, 0x0060, 0x0048,
//'>'   '?'   'h'   'A'   'B'   'C'   'D'   'E'   'F'   'G'
  0x0042, 0x00D3, 0x0074, 0x0077, 0x007C, 0x0058, 0x005E, 0x0079, 0x0071, 0x003D,
//'H'   'I'   'J'   'K'   'L'   'M'   'N'   'O'   'P'   'Q'
  0x0076, 0x0030, 0x001F, 0x0070, 0x0038, 0x0037, 0x0037, 0x005C, 0x0073, 0x003F,
//'R'   'S'   'T'   'U'   'V'   'W'   'X'   'Y'   'Z'
  0x0050, 0x006D, 0x0078, 0x001C, 0x001E, 0x007E, 0x0049, 0x006E, 0x005B
};

/*Constant variables of routing table for SEG_COUNT_FOR_DIGIT-Segment display of F24_OU_Y Siemens*/
#ifdef seg14
//definition of routing table for 14-Segment display of F24_OU_Y Siemens
const TDigits SEG_atDigitsSegmentRoutingTable[][SEG_COUNT_FOR_DIGIT]=
{// a    b    c    d    e    f    g1   g2   h    i    j    k    l    m
  {114, 122, 121, 107, 105, 106, 117, 125, 118, 113, 126, 123, 115, 119},//Segment IDs digit 0
  {142, 150, 149, 135, 133, 134, 129, 137, 130, 141, 138, 151, 143, 131},//Segment IDs digit 1
  {154, 162, 161, 147, 145, 146, 157, 165, 158, 153, 166, 163, 155, 159},//Segment IDs digit 2
  {182, 190, 189, 175, 173, 174, 169, 177, 170, 181, 178, 191, 183, 171},//Segment IDs digit 3
  {194, 202, 201, 187, 185, 186, 197, 205, 198, 193, 206, 203, 195, 199},//Segment IDs digit 4
  {222, 230, 229, 215, 213, 214, 209, 217, 210, 221, 218, 231, 223, 211},//Segment IDs digit 5
  {234,  98,  99, 227, 225, 226, 237, 109, 238, 233, 110, 111, 235, 239},//Segment IDs digit 6
  { 94,  86,  85,  87, 103, 102,  89,  81,  90,  93,  82,  83,  95,  91},//Segment IDs digit 7
  { 66,  58,  57,  59,  75,  74,  77,  69,  78,  65,  70,  71,  67,  79},//Segment IDs digit 8
  { 54,  46,  45,  47,  63,  62,  49,  41,  50,  53,  42,  43,  55,  51},//Segment IDs digit 9
  { 26,  18,  17,  19,  35,  34,  37,  29,  38,  25,  30,  31,  27,  39},//Segment IDs digit 10
  { 14,   6,   5,   7,  23,  22,   9,   1,  10,  13,   2,   3,  15,  11}//Segment IDs digit 11
};
#else
//definition of routing table for 14-Segment display of F24_OU_Y Siemens
const TDigits SEG_atDigitsSegmentRoutingTable[][SEG_COUNT_FOR_DIGIT] =
{// a    b    c    d    e    f    g1   g2   h    i    j    k    l    m
  {114, 122, 121, 107, 105, 106, 117},//Segment IDs digit 0
  {142, 150, 149, 135, 133, 134, 129},//Segment IDs digit 1
  {154, 162, 161, 147, 145, 146, 157},//Segment IDs digit 2
  {182, 190, 189, 175, 173, 174, 169},//Segment IDs digit 3
  {194, 202, 201, 187, 185, 186, 197},//Segment IDs digit 4
  {222, 230, 229, 215, 213, 214, 209},//Segment IDs digit 5
  {234,  98,  99, 227, 225, 226, 237},//Segment IDs digit 6
  { 94,  86,  85,  87, 103, 102,  89},//Segment IDs digit 7
  { 66,  58,  57,  59,  75,  74,  77},//Segment IDs digit 8
  { 54,  46,  45,  47,  63,  62,  49},//Segment IDs digit 9
  { 26,  18,  17,  19,  35,  34,  37},//Segment IDs digit 10
  { 14,   6,   5,   7,  23,  22,   9}//Segment IDs digit 11
};
#endif
/*Constant variables of segment groups*/
/*const TsegmentId SEG_pucSegGroupPosition0LEDs[]*/
///Segment group of LEDs Program Position 0
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition0LEDs[] =
{
  SEG_23,                              ///<0:  Position 0 inside  (4/3)
  SEG_16,                              ///<1:  Position 0 outside (3/3)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupPosition1LEDs[]*/
///Segment group of LEDs Program Position 1
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition1LEDs[] =
{
  SEG_09,                              ///<0:  Position 1 inside  (2/3)
  SEG_02,                              ///<1:  Position 1 outside (1/3)
  SEG_END
};

/*const TsegmentId SEG_pucSegGroupPosition5LEDs[]*/
///Segment group of LEDs Program Position 5
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition5LEDs[] =
{
  SEG_18,                              ///<0:  Position 5 inside  (3/5)
  SEG_32,                              ///<1:  Position 5 outside (5/5)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupPosition6LEDs[]*/
///Segment group of LEDs Program Position 6
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition6LEDs[] =
{
  SEG_04,                              ///<0:  Position 6 inside  (1/5)
  SEG_25,                              ///<1:  Position 6 outside (4/5)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupPosition7LEDs[]*/
///Segment group of LEDs Program Position 7
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition7LEDs[] =
{
  SEG_15,                              ///<0:  Position 7 inside  (3/2)
  SEG_22,                              ///<1:  Position 7 outside (4/2)
  SEG_END
};

/*const TsegmentId SEG_pucSegGroupPosition8LEDs[]*/
///Segment group of LEDs Program Position 8
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition8LEDs[] =
{
  SEG_29,                              ///<0:  Position 8 inside  (5/2)
  SEG_08,                              ///<1:  Position 8 outside (2/2)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupPosition12LEDs[]*/
///Segment group of LEDs Program Position 12
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition12LEDs[] =
{
  SEG_07,                              ///<0:  Position 12 inside  (2/1)
  SEG_28,                              ///<1:  Position 12 outside (5/1)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupPosition13LEDs[]*/
///Segment group of LEDs Program Position 13
///Variants: Bosch F24
static const TsegmentId SEG_pucSegGroupPosition13LEDs[] =
{
  SEG_21,                              ///<0:  Position 13 inside  (4/1)
  SEG_00,                              ///<1:  Position 13 outside (1/1)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupJogLEDsBosch[]*/
///Segment group of Jogdial LEDs
///Variants: Bosch F24
static TsegmentId SEG_pucSegGroupJogLEDsBosch[] =
{
  SEG_10,                           ///<22: Jogdial 0  (2/4)
  SEG_17,                           ///<23: Jogdial 1  (3/4)
  SEG_31,                           ///<24: Jogdial 2  (5/4)
  SEG_03,                           ///<25: Jogdial 3  (1/4)
  SEG_26,                           ///<26: Jogdial 4  (4/6)
  SEG_19,                           ///<27: Jogdial 5  (3/6)
  SEG_12,                           ///<28: Jogdial 6  (2/6)
  SEG_33,                           ///<29: Jogdial 7  (5/6)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupJogLEDsSiemens[]*/
///Segment group of Jogdial LEDs
///Variants: Siemens F24
static TsegmentId SEG_pucSegGroupJogLEDsSiemens[] =
{
  SEG_21,                           ///<22: Jogdial 0  (4/1)
  SEG_14,                           ///<23: Jogdial 1  (3/1)
  SEG_15,                           ///<24: Jogdial 2  (3/2)
  SEG_16,                           ///<25: Jogdial 3  (3/3)
  SEG_17,                           ///<26: Jogdial 4  (3/4)
  SEG_24,                           ///<27: Jogdial 5  (4/4)
  SEG_23,                           ///<28: Jogdial 6  (4/3)
  SEG_22,                           ///<29: Jogdial 7  (4/2)
  SEG_END
};
/*const TsegmentId SEG_pucSegGroupProgramPositionsLEDsBosch[]*/
///Segment group of Jogdial LEDs
///Variants: Bosch F24
static TsegmentId SEG_pucSegGroupProgramPositionsLEDsBosch[] =
{
  SEG_00,       ///<Column 1, Row 1
  SEG_01,       ///<Column 1, Row 2
  SEG_02,       ///<Column 1, Row 3
  SEG_04,       ///<Column 1, Row 5
  SEG_05,       ///<Column 1, Row 6
  SEG_06,       ///<Column 1, Row 7

  SEG_07,       ///<Column 2, Row 1
  SEG_08,       ///<Column 2, Row 2
  SEG_09,       ///<Column 2, Row 3
  SEG_11,       ///<Column 2, Row 5
  SEG_13,       ///<Column 2, Row 7

  SEG_14,       ///<Column 3, Row 1
  SEG_15,       ///<Column 3, Row 2
  SEG_16,       ///<Column 3, Row 3
  SEG_18,       ///<Column 3, Row 5
  SEG_20,       ///<Column 3, Row 7

  SEG_21,       ///<Column 4, Row 1
  SEG_22,       ///<Column 4, Row 2
  SEG_23,       ///<Column 4, Row 3
  SEG_24,       ///<Column 4, Row 4
  SEG_25,       ///<Column 4, Row 5
  SEG_27,       ///<Column 4, Row 7

  SEG_28,       ///<Column 5, Row 1
  SEG_29,       ///<Column 5, Row 2
  SEG_30,       ///<Column 5, Row 3
  SEG_32,       ///<Column 5, Row 5
  SEG_34,       ///<Column 5, Row 7
  SEG_END
};

/*const TsegmentId SEG_pucSegGroupProgramPositionsLEDsSiemens[]*/
///Segment group of Jogdial LEDs
///Variants: Siemens F24
static TsegmentId SEG_pucSegGroupProgramPositionsLEDsSiemens[] = // PRQA S 0778
{
  SEG_07,             ///<0:  Position 0    (2/1)
  SEG_08,             ///<1:  Position 1    (2/2)
  SEG_09,             ///<2:  Position 2    (2/3)
  SEG_10,             ///<3:  Position 3    (2/4)
  SEG_11,             ///<4:  Position 4    (2/5)
  SEG_12,             ///<5:  Position 5    (2/6)
  SEG_18,             ///<6:  Position 6    (3/5)
  SEG_25,             ///<7:  Position 7    (4/5)
  SEG_05,             ///<8:  Position 8    (1/6)
  SEG_04,             ///<9:  Position 9    (1/5)
  SEG_03,             ///<10: Position 10   (1/4)
  SEG_02,             ///<11: Position 11   (1/3)
  SEG_01,             ///<12: Position 12   (1/2)
  SEG_00,             ///<13: Position 13   (1/1)
  SEG_END
};
/*TsegmentId* SEG_pptSegGroups[]*/
///Definition of segment group table
static const TsegmentId* SEG_pptSegGroups[] =
{
  SEG_pucSegGroupPosition0LEDs,
  SEG_pucSegGroupPosition1LEDs,
  SEG_pucSegGroupPosition5LEDs,
  SEG_pucSegGroupPosition6LEDs,
  SEG_pucSegGroupPosition7LEDs,
  SEG_pucSegGroupPosition8LEDs,
  SEG_pucSegGroupPosition12LEDs,
  SEG_pucSegGroupPosition13LEDs,
  SEG_pucSegGroupJogLEDsBosch,
  SEG_pucSegGroupJogLEDsSiemens,
  SEG_pucSegGroupProgramPositionsLEDsBosch,
  SEG_pucSegGroupProgramPositionsLEDsSiemens
};

/*Constant variables of convertion logical segments to physical*/
/* Variant 0: LED IDs for Siemens F24 */
static const  TsegmentId SEG_ptLogicalSegAssignment_Siemens_F24[] =
{
  SEG_07,             ///<0:  Position 0    (2/1)
  SEG_08,             ///<1:  Position 1    (2/2)
  SEG_09,             ///<2:  Position 2    (2/3)
  SEG_10,             ///<3:  Position 3    (2/4)
  SEG_11,             ///<4:  Position 4    (2/5)
  SEG_12,             ///<5:  Position 5    (2/6)
  SEG_18,             ///<6:  Position 6    (3/5)
  SEG_25,             ///<7:  Position 7    (4/5)
  SEG_05,             ///<8:  Position 8    (1/6)
  SEG_04,             ///<9:  Position 9    (1/5)
  SEG_03,             ///<10: Position 10   (1/4)
  SEG_02,             ///<11: Position 11   (1/3)
  SEG_01,             ///<12: Position 12   (1/2)
  SEG_00,             ///<13: Position 13   (1/1)

  SEG_GROUP(SEG_GROUP_JOGDIAL_SIEMENS),
  SEG_GROUP(SEG_GROUP_ALL_PROGRAM_POSITIONS_SIEMENS),

  SEG_26,             ///<15: ECO_WASH      (4/6)
  SEG_19              ///<10: SPEED_WASH    (3/6)
};


/*Variant 1: LED IDs for Bosch F24*/
static const TsegmentId SEG_ptLogicalSegAssignment_Bosch_F24[] =
{
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_0),
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_1),
  SEG_30,                                            ///<2:  Position 2    (5/3)
  SEG_24,                                            ///<3:  Position 3    (4/4)
  SEG_11,                                            ///<4:  Position 4    (2/5)
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_5),
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_6),
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_7),
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_8),
  SEG_01,                                            ///<9:  Position 9    (1/2)
  SEG_05,                                            ///<10: Position 10   (1/6)
  SEG_14,                                            ///<11: Position 11   (3/1)
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_12),
  SEG_GROUP(SEG_GROUP_PROGRAM_POSITION_13),

  SEG_GROUP(SEG_GROUP_JOGDIAL_BOSCH),
  SEG_GROUP(SEG_GROUP_ALL_PROGRAM_POSITIONS_BOSCH),

  SEG_27,                                            ///<15: ECO_WASH      (4/7)
  SEG_06                                             ///<10: SPEED_WASH    (1/7)
};

/* Variant 2: Display IDs for F24Y */
static const TsegmentId SEG_ptLogicalSegAssignment_Display_F24[] =
{
// digit 0
//  SEG_114,             ///<0:  Position a
//  SEG_122,             ///<1:  Position b
//  SEG_121,             ///<2:  Position c
//  SEG_107,             ///<3:  Position d
//  SEG_105,             ///<4:  Position e
//  SEG_106,             ///<5:  Position f
// SEG_117,             ///<6:  Position g1
//  SEG_125,             ///<7:  Position g2
//  SEG_118,             ///<8:  Position h
//  SEG_113,             ///<9:  Position i
//  SEG_126,             ///<10: Position j
//  SEG_123,             ///<11: Position k
//  SEG_115,             ///<12: Position l
//  SEG_119,             ///<13: Position m

// digit 1
//  SEG_142,             ///<14: Position a
//  SEG_150,             ///<15: Position b
//  SEG_149,             ///<16: Position c
//  SEG_135,             ///<17: Position d
//  SEG_133,             ///<18: Position e
//  SEG_134,             ///<19: Position f
//  SEG_129,             ///<20: Position g1
//  SEG_137,             ///<21: Position g2
//  SEG_130,             ///<22: Position h
//  SEG_141,             ///<23: Position i
//  SEG_138,             ///<24: Position j
//  SEG_151,             ///<25: Position k
//  SEG_143,             ///<26: Position l
//  SEG_131,             ///<27: Position m
SEG_00
};
/*TsegmentId *SEG_ptLogicalSegAssignment*/
///definition of table with logical segment ID

static TsegmentId *SEG_ptLogicalSegAssignment = (TsegmentId *)SEG_ptLogicalSegAssignment_Siemens_F24; // PRQA S 0311


#ifdef BLINKING_ACTIVE
static uint8_t SEG_ppucBlinkField[SEG_BEAT_COUNT][SEG_BUFFER_LENGTH];  //buffer of blinking segments for each blinking rhythm
#endif
/*static volatile uint8_t  SEG_aucSegmentStates[SEG_NUMBER_OF_SEGMENTS]*/
/**
Array for storing states of all available segments:
Each element contains the blinking state defined in led.h:

  LED_STATE_OFF,                       ///<Segment off
  LED_STATE_ON,                        ///<Segment on
  LED_STATE_BLINK_1,                   ///<Segment blink rhythm 1
  LED_STATE_BLINK_2,                   ///<Segment blink rhythm 2 (optional)
  LED_STATE_BLINK_3,                   ///<Segment blink rhythm 3 (optional)
  LED_STATE_FADING_1,                  ///<Segment fading with rhythm 1 (optional)
  LED_STATE_FADING_2                   ///<Segment fading with rhythm 2 (optional)

\b Type: array of uint8_t
\b Range: [ 0 .. 255 ]
*/
static volatile uint8_t SEG_aucSegmentStates[SEG_NUMBER_OF_SEGMENTS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};



#ifndef UNIT_TEST

extern BOOL LCD_bGetInitStatus(void);
BOOL _SEG_bGetInitConfig(void)
{
   if (VAR_ucGetVariant(VAR_BRAND) == VAR_SIEMENS)
   {
      SEG_ptLogicalSegAssignment = (TsegmentId *)SEG_ptLogicalSegAssignment_Siemens_F24; // PRQA S 0311
   }
   else
   {
      SEG_ptLogicalSegAssignment = (TsegmentId *)SEG_ptLogicalSegAssignment_Bosch_F24; // PRQA S 0311
   }
   if (TRUE == LCD_bGetInitStatus())
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


extern void LCD_vSetSegment16(uint16_t uiLogSegId, uint8_t ucSegStatus);
void _SEG_vSetSegment(uint16_t uiSegId, uint8_t ucSegStatus)
{
   LCD_vSetSegment16(uiSegId,ucSegStatus);
   if (uiSegId < SEG_NUMBER_OF_SEGMENTS)
   {
      //store status of segments for test automation
      //SEG_aucSegmentStates[(uint8_t)uiSegId] = ucSegStatus;
      if (ucSegStatus < SEG_BLINK)
      {
         // set multiplexer
         //MPX_vSet((uint8_t) uiSegId,ucSegStatus);
      }
   }
}



void SEG_vClearAll(void)
{
   uint8_t i = SEG_BUFFER_LENGTH;
   /*~T*/
   MPX_vClearMpx();//clear multiplexer
   while (i != 0)
   {
      i--;
      SEG_pucBlinkField[i]=0;//delete blink buffer
   }
}



void SEG_vSetAll(void)
{
   uint8_t i;
   for (i=0; i<SEG_NUMBER_OF_SEGMENTS; i++)
   {
      SEG_vSetSegment(i, SEG_ON);
   }
}


uint16_t _SEG_uiGetPhysicalSegment(uint16_t uiLogSeg)
{
   if (uiLogSeg < (uint8_t)(UTI_NELEMENTS(SEG_ptLogicalSegAssignment_Siemens_F24)))
   {
      return SEG_ptLogicalSegAssignment[uiLogSeg]; // PRQA S 0506
   }
   else
   {
      return INVALID;
   }
}


const void* _SEG_pGetPhysicalSegmentGroup(uint8_t ucGroup)
{
   if (ucGroup <= (uint8_t)(UTI_NELEMENTS(SEG_pptSegGroups)))
   {
      return (const void*)(SEG_pptSegGroups[ucGroup]);
   }
   else
   {
      return ((const void*)0);
   }
}

#endif
/*~E*/
#define LSW_BLINKING_SEGMENTS_USED
#define LSW_TEXT_DISPLAY_USED  // not used
#define LSW_BLINKING_TEXT_USED // not used

#ifndef UNIT_TEST
void _SEG_vHandleText(void)
{
#ifdef LSW_TEXT_DISPLAY_USED
   vHandleText();
#else
   //empty, no text used
#endif
}


uint _SEG_uiGetRoutingMask(uint8_t ucDigit, uint8_t ucSegment)
{
#ifdef LSW_TEXT_DISPLAY_USED
   return ((uint) SEG_atDigitsSegmentRoutingTable[ucDigit][ucSegment]);
#else
   return 0;
   //empty, no text used
#endif
}
#endif


#ifdef LSW_BLINKING_TEXT_USED

uint8_t SEG_pucTextBlinkBuffer[SEG_NUMBER_OF_DIGITS];
#ifndef UNIT_TEST
void _SEG_vWriteBlinkText(uint8_t ucDigitStart, const uint8_t* pucText, uint8_t ucBlinkState)
{
   vWriteBlinkText(ucDigitStart, pucText, ucBlinkState);
   vHandleText();
}
#endif
#else
uint8_t SEG_pucTextBlinkBuffer[SEG_NUMBER_OF_DIGITS] = {1,1,1,1,1,1,1,1};


void _SEG_vWriteBlinkText(uint8_t ucDigitStart, const uint8_t* pucText, uint8_t ucBlinkState)
{

}
#endif

#ifndef UNIT_TEST

void _SEG_vHandleBlinkingSeg(void)
{
#ifdef LSW_BLINKING_SEGMENTS_USED
   vHandleBlinkingSeg();
#else
   //empty, no blinking segments
#endif
}



BOOL _SEG_bIsBlinkStateChanged(void)
{
#if defined(LSW_BLINKING_TEXT_USED)||defined(LSW_BLINKING_SEGMENTS_USED)
   if (BEAT_ucGetBeat() == SEG_ucLastBeatValue)
   {
      return FALSE;//same state as last call
   }
   else
   {
      SEG_ucLastBeatValue = BEAT_ucGetBeat();
      return TRUE;//new beat!
   }
#else
   //no blinking used
#endif
}

void _SEG_vSetBlinkBuffer(uint uiSegId, uint8_t ucSegStatus)
{
#ifdef LSW_BLINKING_SEGMENTS_USED
   //todo: taken from F20, consider more effektive solution 

   uint8_t i;
   uint8_t ucBlink;
   if (uiSegId < SEG_NUMBER_OF_SEGMENTS)
   {
      if (ucSegStatus < SEG_BLINK)
      {
         //No blink rhythm
         if (ucSegStatus == SEG_OFF)
         {
            //Always off
            ucBlink = 0;
         }
         else
         {
            //Always on
            ucBlink = 0xFF;
         }
      }
      else
      {
         //Blink rhythm SEG_BLINK ... SEG_BLINK3
         ucBlink = SEG_pucBasicBlinkRhythm[ucSegStatus-SEG_BLINK];
      }
      //Set or reset blink bit of all rhythm
      for (i=0; i<SEG_BEAT_COUNT; i++)
      {
         if ((ucBlink & (uint8_t)(1<<i)) != 0)
         {
            UTI_vSetBit(SEG_ppucBlinkField[i], uiSegId, 1);
         }
         else
         {
            UTI_vSetBit(SEG_ppucBlinkField[i], uiSegId, 0);
         }
      }
   }
#endif
}

uint8_t _SEG_ucGetBlinkOnSegment(uint8_t ucBlinkByte)
{
#ifdef LSW_BLINKING_SEGMENTS_USED
   return SEG_ppucBlinkField[BEAT_ucGetBeat()][ucBlinkByte];
#endif
}


uint8_t _SEG_ucGetBlinkOffSegment(uint8_t ucBlinkByte)
{
#ifdef LSW_BLINKING_SEGMENTS_USED
   return ~SEG_ppucBlinkField[BEAT_ucGetBeat()][ucBlinkByte];
#endif
}
TDigits _SEG_tGetDigitsSegmentCode(TDigits Index)
{
    return (SEG_atDigitsSegmentConversionTable[SEG_pucTextBuffer[Index] - SEG_START_OF_CONVERSION_TAB]);
}

#endif
