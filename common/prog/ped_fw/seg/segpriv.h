/*******************************************************************************
*   Copyright (c) 2016 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          PP PED-Framework
*   COMP_ABBREV      SEG
*******************************************************************************/

#ifndef SEGPRIV_H
#define SEGPRIV_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \brief Private definitions and declarations of segment library
 *         (used for configuration)
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>


/* Definitions and Declarations */

/* DEFINITIONS ****************************************************************************************/
#define SEG_TEXT_READY   0xFFu      ///< special value of #SEG_ucTextIndex to indicte that text mirror was written to output
#define SEG_TEXT_START_WRITE 0x00u  ///< special value of #SEG_ucTextIndex to indicte that text mirror has to be written
#define SEG_END             ((TsegmentId)(-1))///< Endmark for segment groups
#define SEG_BUFFER_LENGTH   (uint8_t)((SEG_NUMBER_OF_SEGMENTS+7u)/8u)///< Size of buffer to save all physical segments or blink bits


#ifdef UNIT_TEST
#define SEG_COUNT_FOR_DIGIT 24U
#endif

/* TYPE DEFINITIONS ***********************************************************************************/

#ifdef UNIT_TEST
typedef uint32_t TDigits;
#endif

/* DECLARATIONS (to be (re)defined in library template)***********************************************/
extern const uint8_t SEG_ucNumberOfBlinkBytes;    ///< Number of bytes handled in blink buffer, must defined outside library!
extern const uint16_t  SEG_uiNumberOfSegments;      ///< Number of physical segments, must defined outside library!
//extern const uint8_t SEG_ucStartOfConversionTable;///< Code of first character in table SEG_atDigitsSegmentConversionTable, must defined outside library.
extern const uint8_t SEG_ucNumberOfDigits;        ///< Number of digits, must be defined by application
extern const uint8_t SEG_ucNumberOfBlinkRhythm;   ///< Number of supported blink rhythms
extern uint8_t  SEG_ucTextIndex;  				///< Current index in text buffer, #SEG_TEXT_READY means nothing to do
extern uint8_t  SEG_ucSegmentCountForDigits;		///< Store number of segments count for digits use in WriteText function

/*lint -save -e9059 "C comment contains C++ comment"
 * Codeblock with comments for doxygen needed. Tested with various parsers to work correctly
 */
 
/** Routing table for 7-Segment display

The routing table must be defined individual for each project (with 7-segment display).\n
Each line in this table represents the "routing" of one digit.
A line contains 7 bytes for the 7 segments A to G.\n

\image html SevenSegExample.gif "Example of a 3+1 digit display"
\n
\b Example: (routing of display shown above)

\code

#define xxx   INVALID

const uint8_t SEG_atDigitsSegmentRoutingTable[SEG_NUMBER_OF_DIGITS][SEG_COUNT_FOR_DIGIT]=
{//  A     B     C     D      E     F     G
 {  23,   26,   27,   25,    22,   21,   24}, //Segment IDs digit 0
 { xxx,   19,   20,  xxx,   xxx,  xxx,  xxx}, //Segment IDs digit 1 just ':'
 {  30,   33,   34,   32,    29,   28,   31}, //Segment IDs digit 2
 {  37,   40,   41,   39,    36,   35,   38}, //Segment IDs digit 3
};

\endcode

\note The Segment ID of this table are the physical ones, not logical!

\sa _SEG_uiGetRoutingMask() (with example for 16bit segments)

*/
//extern const TDigits SEG_atDigitsSegmentRoutingTable[][SEG_COUNT_FOR_DIGIT];


/** Routing table for 7-Segment display

The following image shows a digit of a 7-segment display.\n

\image html sevenSeg.gif "Segment A to G are equivalent to Bit-0 to Bit-6"
\n
Each byte of %SEG_atDigitsSegmentConversionTable represents one character. Each bit in each byte represents a segment
assigned as shown in the image above.\n
The default table is medium ASCII compatible and starts with character code 0x20 (ASCII-blank).
To start with another character #SEG_ucStartOfConversionTable must overwritten.\n


 \note It is recommended to define at least characters from ' ' to ":"\n
       So ' ' may be used to clear the display (You cannot save a lot of memory with short table!).

\b Example:

\code

const uint8_t SEG_ucStartOfConversionTable = ' ';

const uint8_t SEG_atDigitsSegmentConversionTable[] =  //ASCII table for 7-segment display
{
// ' '   '!'   '"'   '#'   '$'   '%'   '&'   '''
  0x00, 0x06, 0x22, 0x36, 0x6D, 0x24, 0x7F, 0x20,
// '('   ')'   '*'   '+'   ','   '-'   '.'   '/'
  0x39, 0x0F, 0x42, 0x46, 0x80, 0x40, 0x02, 0x52,
// '0'   '1'   '2'   '3'   '4'   '5'   '6'   '7'  
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 
// '8'   '9'   ':'  
  0x7F, 0x6F, 0x06
};

\endcode
*/
//extern const TDigits SEG_atDigitsSegmentConversionTable[];

extern uint8_t SEG_pucBlinkField[];///< Buffer saves blinking segments, one bit for each segment, must be defined outside library!
extern uint8_t SEG_pucTextBuffer[];///< Buffer saves the text for 7-Segment display, length must be number of digits
extern uint8_t SEG_pucTextBlinkBuffer[];///<Buffer for blinking digits, length must be number of digits

/* GLOBAL FUNCTIONS (to be (re)defined in library template)******************************************/

// functions to overwrite

//@{\name Functions to Overwrite: Common

/** Runtime configuration & intialisation of segment library

 This function will be called frequently from SEG task handler until  true will be returned. 

   \return  State of configuration\n
    \b Type: bool\n
    \b Range: FALSE -> configuration in progress, all other -> configuration done

   \note <em><b>This function may be overwritten.!!!</b></em>\n
         E.g. variant of HW may be read by this function from eeprom to select variants of routing table


\code
bool _SEG_bGetInitConfig(void)
{
  if (EEP_bIsReady()) //eeprom ready
  {
    SEG_ucVariant  = VAR_ucGetVariant();
    return TRUE;
  }
  return FALSE;
}
\endcode
\n

\note Overwriting _SEG_bGetInitConfig() may change the dependencies of segment library.
      The example above possibly requires to adapt library dependencies #DEP_SEG in segXs.h

*/
extern bool _SEG_bGetInitConfig(void);

/** Interface to lower layer, sets or resets segment with ID \<uiSegdId\>.


 This function should call the interface from lower layered modules (DHWI, PTC, LED, MPX...).\n
 
 
 \param uiSegId: ID of segment\n
        \b Type: uint16_t\n
        \b Range: 0..(number of Segments-1)\n
\n
   \param ucSegStatus: Status of segment \n
        \b Type: uint8_t\n
        \b Range: #SEG_enmStatus

   \return  None

   \note <em><b>This function must overwritten !!!</b></em>.\n
                Use the library template.\n
                Function will be called by library itself.


\b Example:

\code

uint8_t SEG_pucBlinkField[(SEG_NUMBER_OF_SEGMENTS+7)/8]; // blink buffer

void _SEG_vSetSegment(uint16_t uiSegId, uint8_t ucSegStatus)
{
   MPX_vSetSegment((uint8_t) uiSegId,ucSegStatus);
}
\endcode

\sa _SEG_vSetBlinkBuffer()

\n 
\n
*/
extern void _SEG_vSetSegment(uint16_t uiSegId, uint8_t ucSegStatus);
//@}

//@{\name Functions to Overwrite: 7-Segment

/**  Calculates the segment ID of segment \<ucSegment\> in digit \<ucDigit\> (For 7-Segment displays)

   \param ucDigit: Id of digit\n
    \b Type: uint8_t\n
    \b Range: 0.. (number of digits)-1\n
\n
   \param ucSegment: segment in digit
    \b Type: uint8_t\n
    \b Range: [0..7]\n
\n
   \return  Physical Id of segment\n
    \b Type: uint16_t\n
    \b Range: 0..(number of physical segments)-1


   \note <em><b>This function may be overwritten !!!</b></em>\n
      The default function access the array #SEG_atDigitsSegmentRoutingTable and supportes up to 255 segments.
      To use more segments ovewrite _SEG_uiGetRoutingMask and access table of uint16_t.


\b Example: (If you have really more than 255 Segments)

\code
uint16_t SEG_puiSegmentRoutingTable[NUMBER_OF_DIGITS][SEG_COUNT_FOR_DIGIT]
{//  A        B        C        D        E        F        G
 {UI_INVALID,       4,       3, UI_INVALID, UI_INVALID, UI_INVALID, UI_INVALID}, //Segment IDs digit 0 (half segment)
 {       280,     283,     322,        323,        333,         25,          1}, //Segment IDs digit 1
 {       260,     212,      28,         27,         20,         18,         19}  //Segment IDs digit 2
};

uint16_t _SEG_uiGetRoutingMask(uint8_t ucDigit, uint8_t ucSegment)
{
 return (SEG_puiSegmentRoutingTable[ucDigit][ucSegment]);
}
\endcode

<b>If less than 256 segments are used just define routing table accessed in default _SEG_uiGetRoutingMask()</b>

\code
uint8_t SEG_atDigitsSegmentRoutingTable[NUMBER_OF_DIGITS][SEG_COUNT_FOR_DIGIT]
{//  A        B        C        D        E        F        G   
 {INVALID,       4,       3, INVALID, INVALID, INVALID, INVALID}, //Segment IDs digit 0 (half segment)
 {     12,      11,      10,       9,       2,       0,       1}, //Segment IDs digit 1
 {     17,      16,      28,      27,      20,      18,      19}  //Segment IDs digit 2
};
\endcode

*/
extern uint16_t _SEG_uiGetRoutingMask(uint8_t ucDigit, uint8_t ucSegment);

/** Handles text

\note Function may be overwritten with empty function if text display is unused
*/
extern void _SEG_vHandleText(void);
//@}

//@{\name Functions to Overwrite: Logical segments and segment groups
/** Assigns a logical segment with a physical one

 This function takes a logical segment ID and rturns the ID of the corresponding physical segment.\n
 This function is called by library itself, the default function just returns \<uiLogSeg\>.\n

 \param uiLogSeg: logical ID of segment\n
   \b Type: uint16_t\n
   \b Range: 0x0000..0xFFFF

   \note <em><b>This function may be overwritten !!!</b></em> \n
      The default function just returns \<uiLogSeg\>. \n


\b Example: \n

\code

TsegmentId SEG_ptLogicalSegAssignment[]={1,3,5,7,9,0,2,4,6,8};

_SEG_uiGetPhysicalSegment(uint16_t uiLogSeg)
{
 return SEG_ptLogicalSegAssignment[uiLogSeg];
}
\endcode


\sa _SEG_pGetPhysicalSegmentGroup()

*/
extern uint16_t _SEG_uiGetPhysicalSegment(uint16_t uiLogSeg);

/** Returns address of a table of physical segment IDs assigned with \<ucGroup\>

 This Function may be overwritten to define groups of segments.
 It assigns a logical segment ID with a number of physical segments.\n
 The table of physical segments which is returned must be terminated with SEG_END.\n

 \param ucGroup: logical ID of segment\n
   \b Type: uint8_t\n
   \b Range: 0x00..0xFF

  \return Pointer to table of physical segments, terminated by SEG_END ( (TsegmentId)(-1) ).\n
           SEG_vSetSegment16() interprets the pointer as uint16_t*, SEG_vSetSegment() as uint8_t*\n
   \b Type: void*


   \note <em><b>This function may be overwritten!!!</b></em>\n
                This Function works together with _SEG_uiGetPhysicalSegment() like shown in example

\b Example: \n
\code 

#define SEG_GROUP(group)  (group + SEG_NUMBER_OF_SEGMENTS)
#define SEG_NUMBER_OF_GROUPS 2

TsegmentId SEG_ptSegGroup0[] = {4,5,6,SEG_END};
TsegmentId SEG_ptSegGroup1[] = {8,12,9,7,SEG_END};

TsegmentId SEG_ptLogicalSegAssignment[]={1,3,5,SEG_GROUP(0),9,0,2,4,6,SEG_GROUP(1)};

TsegmentId* SEG_pptSegGroups[] = {SEG_pucSegGroup0, SEG_pucSegGroup1};

const void* _SEG_pGetPhysicalSegmentGroup(uint8_t ucGroup)
{
 if(ucGroup <= SEG_NUMBER_OF_GROUPS)
    return (const void*)(SEG_pptSegGroups[ucGroup]);

 else
    return ((const void*)0);
}

\endcode


\sa _SEG_uiGetPhysicalSegment()

*/
extern const void* _SEG_pGetPhysicalSegmentGroup(uint8_t ucGroup);
//@}

//@{\name Functions to Overwrite: Blinking behaviour

/** Function handles blink behaviour. 

\note Function may be overwritten with empty function if blinking is unused

*/
extern void _SEG_vHandleBlinkingSeg(void);

/** Handles blinking text

\note Function may be overwritten with empty function if blinking or text display is unused
*/
extern void _SEG_vWriteBlinkText(uint8_t ucDigitStart, const uint8_t* pucText, uint8_t ucBlinkState);

/** Returns true if the blink state has changed.

  This function returns true if blink state has changed.\n
  If there are different blink rhythms this function returns true if any of them has changed.\n
  The Beat library may be used to implement this function (and synchronice SEG blinking with e.g. text-blinking)

 \return True if any rhythm has changed sice last call\n
 \b Type: uint8_t\n
 \b Range: FALSE ->no changed, all other ->blink state has changed

 \note <em><b>This function may be overwritten if blinking is used!!!</b></em>\n
              The following example shows the default implementation.


\b Example:

\code
bool _SEG_bIsBlinkStateChanged(void)
{ 
  if (BEAT_ucGetBeat() == SEG_ucLastBeatValue)

    return FALSE;//same state as last call

  else

    SEG_ucLastBeatValue = BEAT_ucGetBeat();
    return TRUE;//new beat!
}
\endcode

*/
extern bool _SEG_bIsBlinkStateChanged(void);

/** Returns the mask of blinking segments to switch on

 This Function returns byte number \<ucBlinkByte\> of blink-ON mask.
 Each bit in this mask represents one segment. 
 In the overwritten function just bits of \b blinking segments which are switched \b ON may be set.

 \param ucBlinkByte: Index in blinkbuffer\n
 \b Type: uint8_t\n
 \b Range: 0.. (number of bytes in blinkbuffer)-1\n
\n
 \return Mask of bits to switch ON\n
   \b Type: uint8_t\n
   \b Range: 0..255

   \note <em><b>This function may be overwritten if blinking is used!!!</b></em>\n

\b Example: (Default implementation, with single rhythm using beat library)

\code

uint8_t _SEG_ucGetBlinkOnSegment(uint8_t ucBlinkByte)
{
  if (BEAT_ucGetBeat() & (1<<0))

     return SEG_pucBlinkField[ucBlinkByte];

  else

     return 0;
}
\endcode

*/
extern uint8_t _SEG_ucGetBlinkOnSegment(uint8_t ucBlinkByte);

/** Returns the mask of blinking segments to switch off

 This Function returns byte number \<ucBlinkByte\> of blink-Off mask.
 Each bit in this mask represents one segment. 
 In the overwritten function just bits of \b blinking segments which are switched \b OFF may be set.

 \param ucBlinkByte: Index in blinkbuffer\n
 \b Type: uint8_t\n
 \b Range: 0.. (number of bytes in blinkbuffer)-1\n
\n
 \return Mask of bits to switch OFF\n
   \b Type: uint8_t\n
   \b Range: 0..255

   \note <em><b>This function may be overwritten if blinking is used!!!</b></em>\n

\b Example: (Default implementation, with single rhythm using beat library)

\code

uint8_t _SEG_ucGetBlinkOnSegment(uint8_t ucBlinkByte)
{
  if (BEAT_ucGetBeat() & (1<<0))

     return 0;

  else

     return SEG_pucBlinkField[ucBlinkByte];
}
\endcode

*/
extern uint8_t _SEG_ucGetBlinkOffSegment(uint8_t ucBlinkByte);

/** Interface to lower layer, sets or resets blink status of segment with ID \<uiSegdId\>.

The default implementation of this function supports a single blink rhythm.
If multi blink rhythm are used this function shall be overwritten.

 \param uiSegId: ID of segment\n
        \b Type: uint16_t\n
        \b Range: 0..(number of Segments-1)\n
\n
   \param ucSegStatus: Status of segment \n
        \b Type: uint8_t\n
        \b Range: #SEG_enmStatus

   \return  None

   \note <em><b>This function may be overwritten</b></em>.\n
                Function will be called by library itself.


\b Example(multi blink Rhythm):

\code
uint8_t SEG_ppucBlinkBuffer[NUMBER_OF_BLINK_RHYTHM][(SEG_NUMBER_OF_SEGMENTS+7)/8]; // multi blink buffer


void _SEG_vSetBlinkBuffer(uint16_t uiSegId, uint8_t ucSegStatus)
{ 
  uint8_t i = NUMBER_OF_BLINK_RHYTHM;
  
  while(i)
     UTI_vSetBit(SEG_ppucBlinkBuffer[--i],uiSegId,0); // reset blink bit

  if (ucSegStatus >= SEG_BLINK)
     UTI_vSetBit(SEG_ppucBlinkBuffer[ucSegStatus-1],uiSegId,1); // reset blink bit
}
\endcode
\n 
\n 
\n
*/
extern void _SEG_vSetBlinkBuffer(uint16_t uiSegId, uint8_t ucSegStatus);
//@}

//@{\name Functions for internal use

/** Function handles blink behaviour. 

\note Internal function, do not call do not overwrite

*/
extern void vHandleBlinkingSeg(void);

/** Handles blinking text

\note Internal function, do not call do not overwrite
*/
extern void vWriteBlinkText(uint8_t ucDigitStart, const uint8_t* pucText, uint8_t ucBlinkState);

/** Handles text

\note Internal function, do not call do not overwrite
*/
extern void vHandleText(void);

/** Interface to lower layer, gets Segment Code  with ID \<Index\> from Digits Segment Conversion Table.

The default implementation of this function supports read from SEG_atDigitsSegmentConversionTable.\n
User can dynamicaly change Conversion Table.


 \param Index: ID of character\n
        \b Type: TDigits\n
        \b Range: 0..(size of Digits Segment Conversion Table)\n
\n

   \return  None

   \note <em><b>This function may be overwritten</b></em>.\n
                Function will be called by library itself.
*/
//extern TDigits _SEG_tGetDigitsSegmentCode(TDigits Index);
extern uint32_t _SEG_tGetDigitsSegmentCode(uint8_t Index);
//@}

/*lint -restore -e9059 */

#ifdef __cplusplus
}
#endif

#endif
