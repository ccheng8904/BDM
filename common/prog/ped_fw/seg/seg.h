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

#ifndef SEG_H
#define SEG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/*lint -save -e9059 "C comment contains C++ comment"
 * Codeblock with comments for doxygen needed. Tested with various parsers to work correctly
 */
/** \file
 *
 *  \brief    Public declarations and definitions of segment library
 *
 *  \details  \section SegIntro 1. Introduction
 *  The Segment library handles all kind of segments:
 *  
 *  - LEDs,
 *  - LCD segments of alphanumric display controller,
 *  - LCD segments of segment LCD controller (e.g. Princeton),
 *  - LCD graphics (fixed placed like segments).\n
 *  \n
 *  
 *  
 *  This different segments may be accessed with a unique application interface.
 *  So the overlaying application just need to know the ID of a segment but not the kind.
 *  The resulting advantage is a better design, it makes it easier ti handle variants.\n
 *  \n
 *  
 *  The following features are implemented or may be activated:
 *  
 *  
 *  
 *  
 *  
 *  \section SegInterface   2. Library Interface
 *  
 *  The Segment library has a quite marrow application interface
 *  
 *  <TABLE>
 *  <TR><TD><b>Function</TD>  <TD>Description</b></TD></TR>
 *  <TR><TD>   SEG_ucHandleTask() </TD><TD> Task handler of segment library </TD></TR>
 *  <TR><TD>   SEG_vWriteText() </TD><TD>   Writes a text to the segment display </TD></TR>
 *  <TR><TD>   SEG_vSetSegment() </TD><TD>  Sets/reset/blink  a logical segment or segment group </TD></TR>
 *  <TR><TD>   SEG_vClearAll() </TD><TD>    Clears all segments</TD></TR>
 *  
 *  </TABLE>
 *  
 *  
 *  \section SegConf 3. Configuration
 *  \subsection SegComConf 3.1 Common Configuration (getting started)
 *  
 *  This part of configuration has to be done always.
 *  
 *  \b I) In file segXs.h the number of (physical) segments has to be defined
 *  
 *  \code
 *  #define SEG_NUMBER_OF_SEGMENTS     47
 *  \endcode
 *  
 *  \b II) In file segXs.c at least the following configurations shall be done:\n
 *  
 *  \code
 *  
 *  #define SEG_BUFFER_LENGTH ((SEG_NUMBER_OF_SEGMENTS+7)/8)
 *  
 *  const uint8_t SEG_ucNumberOfBlinkBytes   = SEG_BUFFER_LENGTH;
 *  const uint16_t  SEG_uiNumberOfSegments     = SEG_NUMBER_OF_SEGMENTS;
 *  
 *  uint8_t SEG_pucBlinkField[SEG_BUFFER_LENGTH];
 *  
 *  
 *  _SEG_vSetSegment(uint16_t uiSegId, uint8_t ucSegStatus)//definition of segment connection to underlaying layer
 *  {
 *   if (ucSegStatus <= SEG_ON)
 *    
 *     MPX_vSetSegment((uint8_t) uiSegmentId,ucSegStatus);//Copy segment to multiplexer (connection to HW)
 *  }
 *  \endcode
 *  \n
 *  
 *  \subsection SegComAdv 3.1 Advanced Configuration
 *  
 *  
 *  <TABLE>
 *  <TR><TD><b>Feature</TD>  <TD>Description</b></TD></TR>
 *  <TR><TD>Run-Time-Configuration  </TD><TD> By overwriting _SEG_bGetInitConfig() you may implement a configuration at runtime.
 *                                     </TD></TR>
 *  <TR><TD>16-Bit segments  </TD><TD> Up to 65535 segments are supported. If you really need this,
 *                                     overwrite _SEG_uiGetRoutingMask().\n
 *                                     \n
 *                                     \b Default: 8-Bit Interface -> up to 255 segments</TD></TR>
 *  
 *  <TR><TD>Logical Segments </TD><TD> By overwriting _SEG_uiGetPhysicalSegment() you can assign a physical Segment ID with
 *                                     a logical one.\n
 *                                     Logical segment IDs make overlaying application more independent of variants.</TD></TR>
 *  
 *  <TR><TD>Segment grouping </TD><TD> Sometimes it is required to switch some segments together.\n 
 *                                     By overwriting _SEG_pGetPhysicalSegmentGroup() you may create groups of segments.\n
 *                                     After connecting the groups with logical segments (see obove)\n
 *                                     groups can switched with the same Interface like single segments.
 *                                     SEG_vSetSegment()</TD></TR>
 *  
 *  <TR><TD>Blinking         </TD><TD> By default there is a blinking behaviour (using beat library) implemented.
 *                                     If you need more than one blink rhythm some functions must be overwritten.\n
 *                                     _SEG_bIsBlinkStateChanged()\n
 *                                     _SEG_ucGetBlinkOnSegment()\n
 *                                     _SEG_ucGetBlinkOffSegment()\n
 *                                     _SEG_vSetBlinkBuffer()\n
 *                                     To deactivate blinking (and save some code) overwrite vHandleBlinkingSeg() with empty function.
 *  
 *  </TD></TR>
 *  
 *  <TR><TD>7-Segment        </TD><TD> A seven segment display is supported. To use a seven segment display just
 *                                     define #SEG_pucSegmentRoutingTable in SegXs.c </TD></TR>
 *  </TABLE>
 */
/*lint -restore -e9059 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>

#include "seg/segXs.h"

/* Definitions and Declarations */

/* DEFINITIONS ****************************************************************************************/
#define DEPLIB_SEG      ///< dependencies of segment library (SEG has no dependencies)

enum SEG_enmStatus{
     SEG_OFF,    ///< segment off
     SEG_ON,     ///< segment on
     SEG_BLINK,  ///< segment blink rhythm 1
     SEG_BLINK2, ///< segment blink rhythm 2 (optional)
     SEG_BLINK3, ///< segment blink rhythm 3 (optional)
     SEG_BLINK4  ///< segment blink rhythm 4 (optional)

};

/* TYPE DEFINITIONS ***********************************************************************************/

#ifdef SEG_16BIT
typedef uint16_t TsegmentId;///< Type for SegmentId (16 Bit)
#else
typedef uint8_t TsegmentId;///< Type for SegmentId (8 Bit)
#endif


/* PUBLIC FUNCTIONS */


//@{\name Application Interface of Segment Library

/** Task handler of segment library. 

The Task has to be called until TASK_TERMINATE is returned.\n
The library is initialised when function returns TASK_INITIALISED or TASK_TERMINATE.

\return Current state of segment library (info/command for scheduler)\n
  \b Type: uint8_t\n
  \b Range: [#TASK_NOT_INITIALISED, #TASK_INITIALISED, #TASK_TERMINATE]
*/
extern uint8_t SEG_ucHandleTask(void);

/** Writes text on the segment display.

 This function write some digits to display, for example <tt>SEG_vWriteText(2,"12:54",SEG_ON)</tt>
 writes "12:54" on the display which starts with 3rd digit (see below)\n
 
\code
        ...   ...         ---     ---         ...   ... 
       .   . .   .     |     | . |     |   | .   . .   .
       .   . .   .     |     |   |     |   | .   . .   .
        ...   ...         ---     ---   ---   ...   ... 
       .   . .   .     | |     .     |     | .   . .   .
       .   . .   .     | |           |     | .   . .   .
        ...   ...         ---     ---         ...   ... 
       0.digit                                     7.digit
\endcode

   \param ucDigitStart Start digit for writing\n
    \b Type: uint8_t\n
    \b Range: 0 .. (number of digits -1)\n
\n
   \param pucText Test sring to write, zero terminated\n
    \b Type: uint8_t*\n
\n
   \param ucBlinkState Blink status of text to write\n
    \b Type: uint8_t\n
    \b Range: [SEG_ON, SEG_BLINK1, SEG_BLINK2...]\n
\n
   \return  None

\note The state SEG_OFF is not supported

*/
extern void SEG_vWriteText(uint8_t ucDigitStart, const uint8_t* pucText, uint8_t ucBlinkState);


/** Sets the state of a Segment or group of segments.

 This interface sets the state (on, off, blinking) of a segment or a group of segments.\n

   \param segId: ID of segment\n
       \b Type: uint8_t\n
       \b Range: 0..(number of logical segments)\n
\n
   \param ucSegStatus: New status for segment \<segId\>\n
    \b Type: uint8_t\n
    \b Range: [#SEG_OFF, #SEG_ON, #SEG_BLINK, #SEG_BLINK2...]\n

 \return  None

\note Do not call this Interface, better use SEG_vSetSegment().

*/
extern void SEG_vSetSegment8(uint8_t segId, uint8_t ucSegStatus);

/** Sets the state of a Segment or group of segments.

 This interface sets the state (on, off, blinking) of a segment or a group of segments.\n

   \param segId: ID of segment\n
       \b Type: uint16_t\n
       \b Range: 0..(number of logical segments)\n
\n
   \param ucSegStatus: New status for segment \<segId\>\n
    \b Type: uint8_t\n
    \b Range: [#SEG_OFF, #SEG_ON, #SEG_BLINK, #SEG_BLINK2...]\n

 \return  None

\note Do not call this Interface, better use SEG_vSetSegment().

*/
extern void SEG_vSetSegment16(uint16_t segId, uint8_t ucSegStatus);

/** Sets the state of a Segment or group of segments.

 This interface sets the state (on, off, blinking) of a segment or a group of segments.\n
 The function macro calls the 8 or 16 bit interface to set segment, depending on number of segments.

   \param tSegId: ID of segment\n
       \b Type: Tsegment\n
       \b Range: 0..(number of logical segments)\n
\n
   \param ucSegStatus: New status for segment \<tSegId\>\n
    \b Type: uint8_t\n
    \b Range: [#SEG_OFF, #SEG_ON, #SEG_BLINK, #SEG_BLINK2...]\n

 \return  None

\note Overwrite function _SEG_uiGetPhysicalSegment() and _SEG_pucGetPhysicalSegGroup()
                         to define logical segments and/or groups Ids.\n

*/
#ifdef SEG_16BIT
static inline void SEG_vSetSegment(uint16_t segId, uint8_t ucSegStatus) {SEG_vSetSegment16(segId,ucSegStatus);}
#else
static inline void SEG_vSetSegment(uint8_t  segId, uint8_t ucSegStatus) {SEG_vSetSegment8(segId,ucSegStatus);}
#endif



/** Clears the hole display

This clears the hole display segment for segment.\n
Also the blink buffer will be cleared.\n

\note It is recommended to overwrite this function, if there is a more powerful solution like clear segment bitwise.

\b Example: (using multiplexer)
\code
void SEG_vClearAll(void)
{
   uint8_t i = SEG_BUFFER_LENGTH;
   
   MPX_vClearMpx();

   while (i)
   {
      SEG_pucBlinkField[--i]=0;
   }
}
\endcode
*/
extern void SEG_vClearAll(void);


//@}

extern void SEG_vSetAll(void);///<Switches on all LEDs

#ifdef __cplusplus
}
#endif

#endif
