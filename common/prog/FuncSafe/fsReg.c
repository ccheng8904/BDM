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
 *  COMP_ABBREV      FSF
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file fsReg.c
 *
 *  \ingroup  <DOXY_GROUP_FSF_REG>
 *
 *  \brief    Implementation of package fsReg of library functional safety.
 *
 *  \details  Work registers and the condition codes (flags) of a
specific controller are checked by this task. This task is thought to be so-called one-time check at boot time and
continuous check while operational work.
\n
\n
<b>Functional Safety Register Check Component - Top-Level Design</b>
\n
\n
\li<b>Package Model </b>\n
\image html Package_Register.jpg
\n
<b>Description </b>\n
The fsReg package provides the parts outlined in the model. By compilation switch, the Reg task may run either in
the usual operating mode or in test harness mode. When running in the usual operating mode, the test harness part
will not be linked to the package. When running in test harness mode, stub services are provided, so the real
Register checking interface is separated from being called and only the test server will be able to call it.
\n
<b>Rationale </b>\n
Checking the program counter register and the stack pointer register at normal operational work is not considered
as these registers get checked during program execution and calling functions and passing parameters. Moreover,
checking work registers depends on the type of controller, so the real task is implemented using assembler language.
This high level language part is only meant to be a portable wrapper function on its top. So, its import to keep in
mind that names of entry points can't be changed freely, except they get normalized for all types of controllers.
\n
\n
\n
\li<b>Input Document Reference(s) </b>\n
\n
<TABLE style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>Reference</TD>
<TD>Ducument Title</TD>
<TD>Document Reference Name/Number</TD>
<TD>Version</TD>
<TD>Status</TD>
<TD>Release Date</TD>
</TR>
<TR>
<TD> [1] </TD>
<TD> Functional Safety Software Design document </TD>
<TD> FuncSafeSWSpec.pdf / 5560 0000003770 </TD>
<TD> Version 2.0 </TD>
<TD> Proposed </TD>
<TD> 08.05.2008 </TD>
</TR>
<TR>
<TD> [2] </TD>
<TD> Generic Functional Safety Software Specification</TD>
<TD> GenericFuncSafeSpec.pdf / 5560 0000007751 </TD>
<TD> Version 2.0 </TD>
<TD> Proposed </TD>
<TD> 08.05.2008 </TD>
</TR>
</TABLE>
\n
*/

#include "bsh_stdinc.h"

#include "LibTypes.h"
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fspriv.h"
#include "hsup.h"

/* Local macros ***************************************************************************************/

/* Local type definitions *****************************************************************************/

/* Local prototypes ********************************************************************************/

/* RAM/ROM Variable definitions ***********************************************************************/

/* static function definitions ************************************************************************/

// The Register One-time check handler
void FSF_vRegister_OneTimeCheck (void)
{
#if !defined(TH_FSCOM)
   FSF_vRegistercheck();
#else
   TH_FSFP_PushOutPar(TP_FSF_vRegister_OneTimeCheck);
#endif
}

//The Register Fatal Exception
void FSF_vReg_FatalException(void)
{
   FSF_vReportFatal();
}

/** The Register Channel Task.
    CPU work registers are saved and checked against stuck-at faults and comparable faults.
    After this check, they get restored again. So, this check does not change the content
    of the work registers from the callers point of view. In case of a fault, there is no
    return here!

    \return uint8_t   state of register channel task

    \pre    None
    \post   Conditions for the CPU registers are fulfilled
*/
uint8_t FSF_ucRegHandleTask(void)
{
/*======================================================================
    TP_FSF_ISR_INT_Disabled
======================================================================*/
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Disabled);
#else
   HSUP_vDisableInt();
#endif

/*======================================================================
    TP_FSF_vReg_OperCheck
======================================================================*/
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_vReg_OperCheck);
#endif

   FSF_vRegOpercheck();

/*======================================================================
    TP_FSF_ISR_INT_Enabled
======================================================================*/
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_ISR_INT_Enabled);
#else
   HSUP_vEnableInt();
#endif

   FSF_vReportSuccess((uint8_t)FSF_REG_TEST);

   return (uint8_t)TASK_INITIALISED;
}
