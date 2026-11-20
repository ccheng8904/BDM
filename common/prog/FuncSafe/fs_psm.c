/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Functional Safety
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file fs_psm.c
 *
 *  \ingroup  FunctionalSafety
 *
 *  \brief    Application specific implementation and configuration
 *            for library functional safety.
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"

#include "fs_psm.h"
#include "fSafe.h"

/******************************************************************************/
/* RAM/ROM VARIABLE PROTOTYPES                                                */
/******************************************************************************/
#ifdef VARIANT_BOOTMANAGER
/** \brief    Static variables used for Program Counter Monitoring when
 *            BootManager is active
 *
 *  \details  Two variables (normal and inverted) are used for Program Counter
 *            Monitoring and has values from enum <FSF_PrgSeqMonitorPatterns>.
 */
static uint32_t FSF_ui32BMPrgSeqMonVal;
static uint32_t FSF_ui32BMPrgSeqMonValInv;
#else
/** \brief    Static variables used for Program Counter Monitoring when
 *            Application is active
 *
 *  \details  Two variables (normal and inverted) are used for Program Counter
 *            Monitoring and has values from application specific enum.
 */
static uint32_t FSF_ui32AppPrgSeqMonVal;
static uint32_t FSF_ui32AppPrgSeqMonValInv;
#endif

/******************************************************************************/
/* FUNCTION PROTOTYPES                                                        */
/******************************************************************************/

/** \brief    Function for checking of consistency valuse used for Program
 *            Counter Monitoring.
 *
 *  \details  For Program Counter Monitoring are used two variables (normal
 *             and inverted). In this function is checked if this static
 *             variable are consinstent
 *
 *  \param    none: void
 *
 *  \return   bool: true, false
 */
static bool FSF_isPrgSeqMonitorValuesConsistent(void);

/** \brief    Function returns Program Counter Monitoring value.
 *
 *  \details  Returns only normal (not inverted) value of variable used
 *            for checking.
 *
 *  \param    none:      void
 *
 *  \return   <FSF_PrgSeqMonitorPatterns>
 */
static enum FSF_PrgSeqMonitorPatterns FSF_getPrgSeqMonitorValues(void);


/******************************************************************************/
/* STATIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/
static bool FSF_isPrgSeqMonitorValuesConsistent(void)
{
#ifdef VARIANT_BOOTMANAGER
  return (((FSF_ui32BMPrgSeqMonVal ^ FSF_ui32BMPrgSeqMonValInv) == UINT32_MAX) ? true : false);
#else
  return (((FSF_ui32AppPrgSeqMonVal ^ FSF_ui32AppPrgSeqMonValInv) == UINT32_MAX) ? true : false);
#endif
}

static enum FSF_PrgSeqMonitorPatterns FSF_getPrgSeqMonitorValues(void)
{
/* Note 9034: Expression assigned to a narrower or different essential type
 * Note 9030: Note 9030: cannot cast an unsigned value to an enum type [MISRA 2012 Rule 10.5, advisory] */

#ifdef VARIANT_BOOTMANAGER
  return ((enum FSF_PrgSeqMonitorPatterns) FSF_ui32BMPrgSeqMonVal);  /*lint !e9034 !e9030 */
#else
  return ((enum FSF_PrgSeqMonitorPatterns) FSF_ui32AppPrgSeqMonVal); /*lint !e9034 !e9030 */
#endif
}



void FSF_DoProgramCounterMonitoring(enum FSF_PrgSeqMonitorPatterns CheckPattern,
                                    enum FSF_PrgSeqMonitorPatterns NewPattern)
{
    /* Verify 1st PrgSequCnt integrity */
    if ((FSF_isPrgSeqMonitorValuesConsistent() == true) && (FSF_getPrgSeqMonitorValues() == CheckPattern))
    {
        FSF_setProgramCounterMonitoringVal(NewPattern);
    }
    else
    {
        FSF_vReportFatal();
    }
}

void FSF_setProgramCounterMonitoringVal(enum FSF_PrgSeqMonitorPatterns SetPattern)
{
#ifdef VARIANT_BOOTMANAGER
    FSF_ui32BMPrgSeqMonVal = (uint32_t)SetPattern;
    FSF_ui32BMPrgSeqMonValInv = ((uint32_t)SetPattern ^ UINT32_MAX);
#else
    FSF_ui32AppPrgSeqMonVal = (uint32_t)SetPattern;
    FSF_ui32AppPrgSeqMonValInv = ((uint32_t)SetPattern ^ UINT32_MAX);
#endif
}
