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


#ifndef COMMON_PROG_FUNCSAFE_FSPRGSEQMON_H_
#define COMMON_PROG_FUNCSAFE_FSPRGSEQMON_H_

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"

/*****************************************************************************/
/* TYPE DEFINITIONS                                                          */
/*****************************************************************************/

/** \brief    A enum with possible valuse for Program Counter
 *            Monitoring.
 *
 *  \details  A possible values used when Boot Mannager is
 *            active or can be changed by application specific
 *            values (if it is implemented).
 */
enum FSF_PrgSeqMonitorPatterns
{
#ifdef VARIANT_BOOTMANAGER
    FSF_BM_RAM_TEST_COMPLETED        = 0x00000000UL,
    FSF_BM_PLATFORM_INITIALIZED      = 0x00000100UL,
    FSF_BM_SHARED_DATA_INITIALIZED   = 0x00000200UL,
    FSF_BM_MEMORY_MODULE_INITIALIZED = 0x00000300UL,
    FSF_BM_IS_MODULE_STARTABLE       = 0x00000400UL,
    FSF_BM_IS_MODULE_INTEGRITY_OK    = 0x00000500UL,
    FSF_BM_IS_MODULE_FW_ID_VALID     = 0x00000600UL,
    FSF_BM_START_MODULE              = 0x00000700UL,
#else
    FSF_APP_RAM_CHECK_MODULE         = 0x00000900UL,
#endif
};


/*****************************************************************************/
/* DECLARATIONS                                                              */
/*****************************************************************************/

/**
   Check Program Sequence Monitor Value and Set new

    \param    CheckPattern - expected value from previous step\n

    \param    NewPattern - new value which should be set for next step\n

    \return   none\n

    \remark   If Check Patterns are not consistent or current value is not expected
              -> go To fatal state (FSF_vReportFatal() function is called).

*/
#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
extern void FSF_DoProgramCounterMonitoring(enum FSF_PrgSeqMonitorPatterns CheckPattern,
                                    enum FSF_PrgSeqMonitorPatterns NewPattern);
#else
/* Warning 522: highest operation, function 'FSF_DoProgramCounterMonitoring', lacks side effects [MISRA 2012 Rule 2.2, advisory] */
/*lint -esym(522,FSF_DoProgramCounterMonitoring) */
static inline void FSF_DoProgramCounterMonitoring(enum FSF_PrgSeqMonitorPatterns CheckPattern,
                                    enum FSF_PrgSeqMonitorPatterns NewPattern)
{
    (void)CheckPattern;
    (void)NewPattern;
}
#endif
/**
   Set new value for Program Sequence Monitor for next step

    \param    NewPattern - new value which should be set for next step\n

    \return   none\n

*/

#ifdef FEATURE_FUNCTIONAL_SAFETY_ENABLED
extern void FSF_setProgramCounterMonitoringVal(enum FSF_PrgSeqMonitorPatterns SetPattern);
#else
static inline void FSF_setProgramCounterMonitoringVal(enum FSF_PrgSeqMonitorPatterns SetPattern)
{
    (void)SetPattern;
}
#endif

#endif /* COMMON_PROG_FUNCSAFE_FSPRGSEQMON_H_ */
