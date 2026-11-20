/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 * PROJECT          Generic SW
 * COMP_ABBREV      PHYP
 ******************************************************************************/


#ifndef PHYSPAN_H
#define PHYSPAN_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  Physpan
 *
 *  \brief    Public definitions and declarations for unit Physpan.
 *
 *  \details  A detailed file description that may span several lines
 *            and that has to be terminated by an empty line.
 *
 */
 
 /**
\if LIB_DOC
\file 
 Public declarations and definitions of physical panel library
\endif
*/
/**
\if LIB_DOC
\mainpage Documentation of Library Physical Panel
\else
\file 
 Public declarations and definitions of physical panel library
\endif

\section phypComIntro Introduction

The Physical Panel Library handles different kind of panels.\n
On the one hand it needs a unique interface (independent of panel technology).
On the other hand it must be possible to join some capacitive keys with some mechanical keys.
The application (overlaying modules) shall not know anything about panel technology (design!).
So it is required to handle all kind of panels (mechanical, infrared, capacitive) in one library.\n
The flexibility and the complexity make the integration of this library not so easy.
This Introduction may help to handle the configuration of physical panel.\n
Please use the library configuration templates:
\n
\n
<TABLE>
<TR><TD><b>File</TD>  <TD>Description</b></TD></TR>
<TR><TD>PhyXS.h       </TD><TD>Application specific definition to be published. </TD></TR>
<TR><TD>PhyComXS.c    </TD><TD>Common declarations for physical panel (used by all kinds of panel)</TD></TR>
<TR><TD>PhyDigXS.c    </TD><TD>Declarations for digital (mechanical) panel</TD></TR>
<TR><TD>PhyCapXS.c    </TD><TD>Declarations for capacitive panel</TD></TR>
<TR><TD>PhyIrXS.c     </TD><TD>Declarations for capacitive panel</TD></TR>
</TABLE>


Never the less the touch technology is not so easy, especially if touch is mixed with additional
mechanical keys. Please don't forget, that touch technology not only depends on HW,
it depends on construction too. So for touch technology this library never will be a plug and play library!\n
\n
Although high complexity the library can be used for tiny applications with some mechanical keys.
Using a HC08 these applications will need 110 to 150 bytes (75 bytes for library) for reading keys.

\section phypDigiIntro Introduction Digital Panel
\b TBD

\section phypInfraIntro Introduction Infrared Panel
\b TBD

\section phypCapaIntro Introduction Capacitive Panel

The following charts will explain different aspects of capacitive panel.\n
\n
The first chart explains the relation between minimum and maximum as well as the role of the filter.\n
The measured AD values must be saved in the array #PHYP_aucCapKeyValues[].
This has to be done be overwriting the function _PHYP_bCapReadAndSetNextChannel()
which will be called from capacitive task handler PHYP_ucHandleCapacitive().
The first chart shows an input signal with some interference.\n
These interferences does not influence the filtered signal #PHYP_auiCapFilteredValues[] in a significant way.\n
\n
Important for further calculations are the dynamic minimum and maximum. For each key there is a separate minimum.
The minimum #PHYP_auiCapMinimum[] is tracking minimums of the input signal #PHYP_aucCapKeyValues[].\n
As tracking of sensitivity (range) is not required (and not so easy ->you need someone pressing the bottom),
the value #PHYP_ucCapRange[] must be programmed once for each key -- if it is required individual for each control.\n
The sum of High Byte(PHYP_auiCapMinimum[]) and PHYP_ucCapRange[] is the maximum.\n
Minimum and maximum are used to get relative values. A value equal or below minimum is equivalent to 0\%.
A value equal or above maximum is equivalent to 100%.\n
The advantage evaluating measurement relative to maximum and minimum demonstrates Picture 2.

\b Note: \n
The high byte of PHYP_auiCapFilteredValues[] and PHYP_auiCapMinimum[] is the output of the filter.\n
The low byte is needed by the IIR-Filter for truncation.
\n
\n

\image html CapaChart1.gif "Picture 1: Filtered disturbed input signal of capacitive panel"

\n

Picture 2 explains the usage of the relative thresholds #PHYP_ucCapOffThreshold and #PHYP_ucCapOnThreshold.\n
There is one relative On-threshold for all keys. If the filtered value increases above this level, the (pre sampled) key
will be set to "pressed".
The (pre-sampled) key will be set to "released" again when the filtered value falls below the relative Off-threshold.\n
The absolute thresholds to sense pressed/released are certainly different for each key (and time variant).
So the current absolute thresholds are calculated internally from the fixed relative ones.\n
Additional to the sampling filter there is a post sampling of the pre-sampled key state.
The returned key state is changed when the key state is stable for some sampling periods: #PHYP_ucCapNumberOfPostSamples.

\n
\image html CapaChart2.gif "Picture 2: Detecting key pressed/released using relative thresholds"
\n
\n
Picture 3 illustrates the tracking of minimum with a minimum of the input signal.\n
To track the dynamic minimum a track-and-hold filter is used.
The filter enters the track mode when the filtered input signal falls below the Min-Threshold #PHYP_ucCapMinThreshold.
To get a stable minimum an IIR-low-pass-filter with low corner frequency is used.
The dynamic minimum needs some seconds to reach the value of input singnal.
If the input signal rises above the Min-Threshold, the value of dynamic minimum will hold.\n
\n
The critical point of this method is the start value of the minimum.
The minimum never can track the input signal when the start value is tool low.
So it is better to select a start value of minimum which is a little bit higher
than the sensed minimum than a value below this minimum, especially when minimum will not be written back
to get the last minimum after reset.

\n
\image html CapaChart3.gif "Picture 3: Tracking of Minimum and Maximum with input signal"

\n


\section configu Configuration


\subsection confDigi Configuration of Digital Panel

\subsection confIr Configuration of Infrared Panel

\subsection confCapa Configuration of Capacitive Panel

To adapt/configurate the digital panel the following files must be created and compiled:
\n
\n
<TABLE>
<TR><TD><b>File</TD>  <TD>Description</b></TD></TR>
<TR><TD>PhyXS.h       </TD><TD>Application specific definition to be published. </TD></TR>
<TR><TD>PhyComXS.c    </TD><TD>Common declarations for physical panel (used by all kinds of panel)</TD></TR>
<TR><TD>PhyCapXS.c    </TD><TD>Declarations for capacitive panel</TD></TR>
</TABLE>

Please use the detailed description in these files. This chapter just will give you an overview. 

*/



/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>
#include "physpan/phyxs.h"



/******************************************************************************/
/*   DEPENDENCIES                                                             */
/******************************************************************************/
#define DEPLIB_PHYP MOD_TIM    ///< dependencies of physical panel libraries


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/


/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
typedef uint32_t Tpanel;///< Type for interface PHYP_tGetKeys()



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/
//@{\name Application interface for all kind of panels

/** \brief Returns state of panel
 * 
 *  This function returns the current state of the keys.
 *  All keys (digital and capacitive) will be returned.
 * 
 * \return State of the panel (bit coded)\n
 * 
 */
extern Tpanel PHYP_tGetKeys(void);
//@}



//@{\name Application interface for Digital Panel

/** Task handler of digital panel
 *
 * The task is initialised when TASK_INITIALISED is returned.
 *
 * \return Current state of digital panel: [#TASK_NOT_INITIALISED, #TASK_INITIALISED]
 */
extern uint8_t PHYP_ucHandleDigital(void);


/** Restarts the the sampling sequence for digital panel
 *
 * This interface may be called when undebounced key state has changed to get stable key reaction time.
 * If two keys are pressed/released within the debounce time, they will be set/release at the same time.
 * 
 * \note It is not mandatory to call this interface.
 */
extern void PHYP_vRestartDigiSampleSequence(void);
//@}



//@{\name Application interface for Capacitive Panel
/** Task handler of capacitive panel
 *
 *  The Task has to be called frequently until TASK_TERMINATE will be returned.\n
 *  The task is initialised when TASK_INITIALISED or TASK_TERMINATE will be returned.
 *  
 *  \return Current state of capacitive panel (info/command for scheduler)\n
 *    \b Range: [#TASK_NOT_INITIALISED, #TASK_INITIALISED, #TASK_TERMINATE]
 *  
 *  \image html statechart.gif "This is the state chart of PHYP_ucHandleCapacitive()"
 */
extern uint8_t PHYP_ucHandleCapacitive(void);


/** May be used to block and release a clamping capacitive key

 *  This function blocks a clamping capacitive key until they will be released again.
 *  This is done by setting the high byte of minimum to maximum value (0xFF).
 *
 *  \note The key will not be locked permanently. If the incoming AD-Values fall again
 *        the minimum will follow. So this function does not help for floating inputs which may be drift up and down!
 *  
 *  \param ucLogKey: Key ID of the key to be released\n
 *      \b Range: 0 255 (last key id), range is not checked!!!
 *  
 */
extern void PHYP_vCapResetKey(uint8_t ucKeyId);
extern void PHYP_vCapBlockKey(uint8_t ucKeyId); //Same as PHYP_vCapResetKey do not use in new applications


/** Returns whether capacitive keyboard is plausible or not
 * 
 *  This function may be implemented application specific.\n
 *  E.g. the result of plausibility check may be returned.\n
 *  In applications which have touch power key where switch off is critical
 *  but not switch on. This function may be used to ignore switch on with implausible keyboard.
 *
 *  \return status of capacitive keyboard\n
 *
 *  \note This interface may be overwritten, the default function just returns true!
 */
extern bool PHYP_bCapIsKeyboardPlausible(void);


/** Sets the calibration mode for capacitive
 * 
 *  After calling the function the calibration mode is enabled.
 *  For each new filtered dataset the function _PHYP_vCapDoCalibration() will be called.
 *  Minimum trackinh and plausibility checks will be stopped.
 * 
 *  \note In this mode no keys will be set or released.
 */
extern void PHYP_vCapSetCalibrationMode(void);
//@}


#ifdef __cplusplus
}
#endif

#endif // PHYSPAN_H
