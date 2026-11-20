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
/** \file fsRam.c
 *
 *  \ingroup  <DOXY_GROUP_FSF_RAM>
 *
 *  \brief    Implementation of package fsRam of library functional safety.
 *
 *  \details  The RAM area of a controller is checked by this task.
 *Its responsibility is separated into one task that is thought to be so-called one-time check, which is only active at
 *startup of a controller, and the task handler, which is thought to check the RAM at normal operational work.
 *\n
 *\n
 *<b>Functional Safety RAM Check Component - Top-Level Design</b>
 *\n
 *\n
 *\li<b>Package Model </b>\n
 *\image html Package_RAM.jpg
 *\n
 *<b>Description </b>\n
 *The fsRam package provides the parts outlined in the model. By compilation switch, the Ram task may run either in
 *the usual operating mode or in test harness mode. When running in the usual operating mode, the test harness part
 *will not be linked to the package. When running in test harness mode, stub services are provided, so the real
 *RAM checking interface is separated from being called and only the test server will be able to call it.
 *\n
 *<b>Rationale </b>\n
 *At normal operational work, checking the whole RAM at once is not possible, at least a very difficult task. For that
 *reason, it was decided to perform a full RAM check at startup only. Below an overview is provided what test algorithm
 *is used comparable to some other public algorithms. Fault models can be found at the respective internet locations.
 *\n
 *\image html RAM_Test_Algorithm.jpg
 *\n
 *As shown the quality of the implemented algorithm is comparable to the March Y algorithm extended by some elements of
 *the March-LR algorithm. The comparison table shows the implemented algorithm to be able to cover DC faults within
 *data cells and address lines as well. The RAM check task is implemented using assembler language with a portable
 *C-wrapper function on its top, thus it depends on the type of controller.
 *\n
 *During normal operational work, the March-X algorithm is used to continue checking the RAM. As this algorithm is very easily and efficiently to handle, for a given check pattern (actually we use 4 different) checking the whole RAM is possible to perform within 5 seconds for a 4KB RAM space. Moreover, RAM portions get checked by overlapping the last byte of the actual buffer, hence recognition of cross-talk between portions will also be possible.
 *\n
 *\n
 *\li<b>Input Document Reference(s) </b>\n
 *\n
 *<TABLE style="text-align: center">
 *<TR style="background-color:rgb(255,255,206)">
 *<TD>Reference</TD>
 *<TD>Ducument Title</TD>
 *<TD>Document Reference Name/Number</TD>
 *<TD>Version</TD>
 *<TD>Status</TD>
 *<TD>Release Date</TD>
 *</TR>
 *<TR>
 *<TD> [1] </TD>
 *<TD> Functional Safety Software Design document </TD>
 *<TD> FuncSafeSWSpec.pdf / 5560 0000003770 </TD>
 *<TD> Version 2.0 </TD>
 *<TD> Proposed </TD>
 *<TD> 08.05.2008 </TD>
 *</TR>
 *<TR>
 *<TD> [2] </TD>
 *<TD> Generic Functional Safety Software Specification</TD>
 *<TD> GenericFuncSafeSpec.pdf / 5560 0000007751 </TD>
 *<TD> Version 2.0 </TD>
 *<TD> Proposed </TD>
 *<TD> 08.05.2008 </TD>
 *</TR>
 *</TABLE>
 *\n
 */

#include "bsh_stdinc.h"

#ifdef TH_FSCOM
#include "LibTypes.h"
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fspriv.h"
#else
#include "FuncSafe/fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "hsup.h"
#include "FuncSafe/fspriv.h"
#endif

/* Local macros ***************************************************************************************/

/* ((RAM_BLOCKSIZE * sizeof(uint32)) + 4 bytes) => + 4 bytes which are omitted in Scramble (0, 2, 3, 5, 4, 6) */
#define COUNT_OF_TESTED_BYTES    ((uint32_t)RAM_BLOCKSIZE * (uint32_t)sizeof(uint32_t))

/* Local type definitions *****************************************************************************/

/* Local prototypes ********************************************************************************/
#ifdef OPERATIONAL_RAM_CHECK
STATIC  void    vRamOpercheck (uint8_t * pucRAM, uint8_t * pucDEXBuffer, uint8_t ucPatNoInv, uint8_t ucPatInv);
STATIC  void    vRAMCheckOngoing     (void);
STATIC  void    vRAMCheckSuccess     (void);
#endif

/* RAM/ROM Variable definitions ***********************************************************************/
#ifdef OPERATIONAL_RAM_CHECK
STATIC  uint8_t       FSF_ucRamTaskState = (uint8_t)FSF_RAM_CHECK_RESTART;  //!<    The RAM task handler state
STATIC  uint8_t *     FSF_pucActualRAMAddress;                              //!<    The actual RAM check address
STATIC  FSTH_CONST uint8_t *     FSF_pucEndAdrRamSeg;
STATIC  uint8_t       FSF_ucActualRAMIdx  = (uint8_t)0;                     //!<    The actual RAM segment index
STATIC  uint8_t       FSF_ucPatternIdx    = (uint8_t)0;                     //!<    The actual check pattern index
#endif
#ifdef  TH_FSCOM
//  Actual RAM Configuration:
#ifdef  OPERATIONAL_RAM_CHECK

//  Operational RAM check
const   uint8_t       RAMConfig[2]    = {(uint8_t)true,(uint8_t)MARCHBUFLENGTH};


//  Reserve three and two elements for test
STATIC  char        RAMTestBuffer0[3 * MARCHBUFLENGTH]          = {0};
STATIC  char        RAMTestBuffer1[2 * (MARCHBUFLENGTH - 3)]    = {0};


//  RAM Segment Table.
FSTH_CONST   T_RAM_Segment   FSF_atRamSegments[] = {
   {(uint8_t *)RAMTestBuffer0,(uint8_t *)&RAMTestBuffer0[(3 * MARCHBUFLENGTH) - 1]},
   {(uint8_t *)RAMTestBuffer1,(uint8_t *)&RAMTestBuffer1[(2 * (MARCHBUFLENGTH - 3)) - 1]}
};


//  Check Pattern Table
FSTH_CONST   T_RAM_Pattern   FSF_asPatternTable[] = {    {MARCH_PATTERN_0_0,MARCH_PATTERN_1_0},
                                                    {MARCH_PATTERN_0_1,MARCH_PATTERN_1_1},
                                                    {MARCH_PATTERN_0_2,MARCH_PATTERN_1_2},
                                                    {MARCH_PATTERN_0_3,MARCH_PATTERN_1_3}};


//  RAM Module descriptor
FSTH_CONST   T_RAM_Module    FSF_atRamModule   = {   (uint8_t)MARCHBUFLENGTH,
                                                (uint8_t)(sizeof(FSF_atRamSegments)/sizeof(T_RAM_Segment)),
                                                (uint8_t)(sizeof(FSF_asPatternTable)/sizeof(T_RAM_Pattern))};
#else

//  No operational RAM check
const   uint8_t   RAMConfig[2] = {(uint8_t)false,(uint8_t)0};
#endif
#endif

/* static function definitions ************************************************************************/
#ifdef OPERATIONAL_RAM_CHECK

/*=====================================================================================
    Check a piece of RAM
=====================================================================================*/

/** Do one operational check cycle
This handler basically provide three different means:
1.) On call, it checks the addressed area against cell errors.
2.) It maintains the actual RAM segment length
*/
STATIC  void    vRamOpercheck (uint8_t * pucRAM, uint8_t * pucDEXBuffer, uint8_t ucPatNoInv, uint8_t ucPatInv)
{

/*======================================================================
    Preserve some loop variables for test
======================================================================*/
#ifdef TH_FSCOM
   //  Some Loop variables
   uint8_t   i;
   uint8_t * p;
   uint8_t * q;
#endif

/*======================================================================
    Disable all interrupts while checking
======================================================================*/
#ifdef TH_FSCOM
   //  Only keep track of interrupt disabling
   TH_FSFP_PushOutPar(TP_FSF_RAM_INT_Disabled);
#else
   //  Do it real!
   HSUP_vDisableInt();
   _FSF_vDoWhenPieceRamCheckStarted();
#endif

/*======================================================================
    Test Harness code
======================================================================*/
#ifdef TH_FSCOM
   //  Notify this piece of code has passed
   TH_FSFP_PushOutPar(TP_FSF_FSF_vRamOpercheck);
   TH_FSFP_PushOutPar(ucPatNoInv);
   TH_FSFP_PushOutPar(ucPatInv);
   //  For test purposes:
   //  - save the original data into the exchange buffer
   for (i=(uint8_t)0,q = pucDEXBuffer, p = pucRAM; i < (uint8_t)MARCHBUFLENGTH; i++)
   {
      *q++ = *p++;
   }
   //  - then, fill the original data with the check pattern
   for (i=(uint8_t)0,q = pucRAM; i < (uint8_t)MARCHBUFLENGTH; i++)
   {
      *q++ = ucPatNoInv;
   }
   //  - finally restore the original data part
   for (i=(uint8_t)0,q = pucDEXBuffer, p = pucRAM; i < (uint8_t)MARCHBUFLENGTH; i++)
   {
      *p++ = *q++;
   }
#endif

/*======================================================================
    Production code
======================================================================*/
#ifndef TH_FSCOM
/*=======================================================
    Interface description R8c
=======================================================*/

   /**
    Since the interface below is required to run as fast as possible, it is implemented in pure ASM language.
    For R8C, IAR, this interface convention holds:
    ucPatInv            R0H     as unsigned byte
    ucPatNo Inv         R0L     as unsigned byte
    pucDEXBuffer        R2      as unsigned word (pointer to near data)
    pucRAM              A0      as unsigned word (pointer to near data)
    */

/*=======================================================
    Interface description Hcs12
=======================================================*/

   /** For HCS12, METROWERKS, this interface convention holds:
    ucPatInv            B       as unsigned byte
    ucPatNo Inv         [SP]    as unsigned byte
    pucDEXBuffer        [SP+1]  as unsigned word (pointer to near data)
    pucRAM
   */

   FSF_vRamOpercheck(pucRAM,pucDEXBuffer,ucPatNoInv,ucPatInv);
#endif

/*======================================================================
    Re-Enable all interrupts
======================================================================*/
#ifdef TH_FSCOM
   //  Only keep track of enebling
   TH_FSFP_PushOutPar(TP_FSF_RAM_INT_Enabled);
#else
   _FSF_vDoWhenPieceRamCheckFinished();
   // Do it real!
   HSUP_vEnableInt();
#endif
}

/*=====================================================================================
    Notify RAM check ongoing
=====================================================================================*/
/** RAM Check Ongoing
    This function is called if a RAM check cycle is ongoing. \n

    \param      none
    \pre        none. \n
*/
STATIC  void    vRAMCheckOngoing     (void)
{
#ifdef SYSTEMTEST_F15b
   //  System Test Case F15b
   //  The RAM channel is not ongoing, it remains idle
#else
   //  Report ongoing RAM check.
   FSF_vReportOnGoing((uint8_t)FSF_RAM_TEST);
#endif
}

/*=====================================================================================
    Notify RAM check successfully completed
=====================================================================================*/
/** RAM Check Success
    This function is called if a RAM check cycle is completed. \n

    \param      none
    \pre        none. \n
*/
STATIC  void    vRAMCheckSuccess     (void)
{
#ifdef SYSTEMTEST_F19b
   //  System Test Case F19b
   //  The RAM channel does not complete in time
#else
   //  Report successfully completed RAM check.
   FSF_vReportSuccess((uint8_t)FSF_RAM_TEST);
#endif
}
#endif

/*====================================================================================================
    external function definitions
====================================================================================================*/
#if (!defined(WIN32) && defined(MC9S12GC96))
#pragma CODE_SEG NON_BANKED
#endif

// The RAM One-time check handler
void FSF_vRam_OneTimeCheck(void)
{
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_vRam_OneTimeCheck);
#else
   FSF_vRamcheck(FSF_atRamSegments, FSF_u32GetNumberOfRamSegments(), 0u);
#endif
}

// The RAM operational check handler
uint8_t FSF_ucRamHandleTask(void)
{
/*======================================================================
    The operational RAM check
======================================================================*/
#ifdef OPERATIONAL_RAM_CHECK
   switch (FSF_ucRamTaskState)
   {
      case (uint8_t)FSF_RAM_CHECK_RESTART:
      {
/*=======================================================
    RAM check initialisation
=======================================================*/
          /** Restart the RAM check again. But, do not change the actual check pattern at this point.
          */
#ifdef FSF_RAM_4_BYTES_AT_ONCE
          //  Before start of the RAM check, the data exchange buffer has to be checked. The physical
          //  location of the data exchange buffer is just behind RAMTOP. Since this area has to be
          //  excluded from operational checking, it is checked separately at this point.

          // **************************************************************************
          //             Check the data exchange buffer. (Data is not saved)
          // **************************************************************************
          vRamOpercheck( FSF_arMarchDataBuffer,
                                  &FSF_arMarchDataBuffer[MARCHBUFLENGTH/2],
          //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
          //  REFERENCE - ISO-6.3.3.2 Semantics
          //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
                                  FSF_asPatternTable[FSF_ucPatternIdx].ucPatNotInv,
                                  FSF_asPatternTable[FSF_ucPatternIdx].ucPatInvers);

          //  Start with the first RAM address
          FSF_pucActualRAMAddress = FSF_atRamSegments[0].pucStartSeg;
#else

         //  Before start of the RAM check, the data exchange buffer has to be checked. The physical
         //  location of the data exchange buffer is just behind RAMTOP. Since this area has to be
         //  excluded from operational checking, it is checked separately at this point.

         // **************************************************************************
         //             Check the data exchange buffer. Data get saved behind
         // **************************************************************************
         vRamOpercheck( FSF_arMarchDataBuffer,
                                 &FSF_arMarchDataBuffer[MARCHBUFLENGTH],
         //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
         //  REFERENCE - ISO-6.3.3.2 Semantics
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
                                 FSF_asPatternTable[FSF_ucPatternIdx].ucPatNotInv,
                                 FSF_asPatternTable[FSF_ucPatternIdx].ucPatInvers);


         // **************************************************************************
         //             Check the data behind.  Data get saved in the exchange buffer.
         // **************************************************************************
         vRamOpercheck( &FSF_arMarchDataBuffer[MARCHBUFLENGTH],
                                  FSF_arMarchDataBuffer,
                                  FSF_asPatternTable[FSF_ucPatternIdx].ucPatNotInv,
                                  FSF_asPatternTable[FSF_ucPatternIdx].ucPatInvers);

         //  Start with the first RAM address
         FSF_pucActualRAMAddress = FSF_atRamSegments[0].pucStartSeg;
#endif
         //  Start with the first segment from the segment table.
         FSF_ucActualRAMIdx      = (uint8_t)0;
         /*Note 9016 Pointer arithmetic not allowed MisraC2012 18.4 [advisory] : required here */
         /*lint -save -e9016 */
         // We must exclude RAMCONT_EXCLUDED_DATA for RAM1
         if(FSF_ucActualRAMIdx == 0u) // RAM1
         {
            FSF_pucEndAdrRamSeg = FSF_atRamSegments[FSF_ucActualRAMIdx].pucEndSeg + 1 - (COUNT_OF_TESTED_BYTES + RAMCONT_EXCLUDED_DATA_SIZE);
         }
         else // RAM2, RAM3
         {
            FSF_pucEndAdrRamSeg = FSF_atRamSegments[FSF_ucActualRAMIdx].pucEndSeg + 1 - COUNT_OF_TESTED_BYTES;
         }
         //  Msg(4:0488) Performing pointer arithmetic.
         //  MISRA-C:2004 Rule 17.4 <next>
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
          //  Change to next state
         FSF_ucRamTaskState      = (uint8_t)FSF_RAM_CHECK_ONGOING;
         break;
      }
      case (uint8_t)FSF_RAM_CHECK_ONGOING:
      {
/*=======================================================
    Do one operational check cycle
=======================================================*/
          //  RAM check inside the middle of the RAM segment OR does it have reached the final portion?
          if ( FSF_pucActualRAMAddress > FSF_pucEndAdrRamSeg )           /*lint !e946 Relational or subtract operator applied to pointers */
          {
                //  Finally change to the next RAM segment (if any)
                FSF_ucRamTaskState = (uint8_t)FSF_RAM_CHECK_CHANGE_SEGMENT;
          }
          else
          {
                // Do the check for the actual RAM protion
                vRamOpercheck( FSF_pucActualRAMAddress, FSF_arMarchDataBuffer,
                               FSF_asPatternTable[FSF_ucPatternIdx].ucPatNotInv,
                               FSF_asPatternTable[FSF_ucPatternIdx].ucPatInvers);

                //  Set up next address area
                FSF_pucActualRAMAddress += (int16_t)FSF_atRamModule.ucBufferLength - 1;
          }
         break;
      }
      case (uint8_t)FSF_RAM_CHECK_CHANGE_SEGMENT:
      {
/*=======================================================
    Compute the next RAM address
=======================================================*/
         /** Compute the next RAM address
         Check the next RAM segment WITHOUT changing the check pattern. The requirements says, do full
         check using one pattern. When finished, change the pattern and start the check again.
         */

         // Switch to the next segment, if any
         FSF_ucActualRAMIdx++;

         // If all segments have been checked, finalise the actual check session
         if (FSF_ucActualRAMIdx >= FSF_atRamModule.ucSegmentCount)
         {
            //  Enter the finish state
            FSF_ucRamTaskState = (uint8_t)FSF_RAM_CHECK_FINISH;
         }
         else
         {
            //  Set up the next RAM address
            FSF_pucActualRAMAddress = FSF_atRamSegments[FSF_ucActualRAMIdx].pucStartSeg;
            FSF_pucEndAdrRamSeg      = FSF_atRamSegments[FSF_ucActualRAMIdx].pucEndSeg + 1 - COUNT_OF_TESTED_BYTES;
            /*lint -restore -e9016 */
            //  Msg(4:0488) Performing pointer arithmetic.
            //  MISRA-C:2004 Rule 17.4
            //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_RAM_Segment_changed);
            TH_FSFP_PushOutPar(FSF_ucActualRAMIdx);
#endif
            FSF_ucRamTaskState = (uint8_t)FSF_RAM_CHECK_ONGOING;
         }
         break;
      }
      case (uint8_t)FSF_RAM_CHECK_FINISH:
      {
/*=======================================================
    RAM check complete
=======================================================*/
         /** Change to the next check pattern and restart the RAM check
         */

         //  Switch to the next pattern
         FSF_ucPatternIdx++;

         if (FSF_ucPatternIdx >= FSF_atRamModule.ucPatternCount)
         {
            FSF_ucPatternIdx = (uint8_t)0;
         }

         //  Restart the RAM check
         FSF_ucRamTaskState = (uint8_t)FSF_RAM_CHECK_RESTART;
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_RAM_Pattern_changed);
         TH_FSFP_PushOutPar(FSF_ucPatternIdx);
#endif
         break;
      }
      default:
      {
/*=======================================================
    Report Error due to invalid state
=======================================================*/
         FSF_vReportError((uint8_t)FSF_RAM_TEST,(uint8_t)FSF_ERR_INVALID_STATE,(uint8_t)0);
         break;
      }
   }
   //  Signal the channel state to the FSCOM
   if (FSF_ucRamTaskState == (uint8_t)FSF_RAM_CHECK_RESTART)
   {
      //  Signal finished check
      vRAMCheckSuccess();
   }
   else
   {
      //  Signal ongoing work
      vRAMCheckOngoing();
   }
#endif
   return  (uint8_t)TASK_INITIALISED;
}

/*=====================================================================================
    Operational Exception
=====================================================================================*/
#ifdef OPERATIONAL_RAM_CHECK
// The RAM Operational Exception
void FSF_vRam_OperationalException(void)
{
   FSF_vReportError((uint8_t)FSF_RAM_TEST,(uint8_t)FSF_ERR_FATAL_EXCEPTION,(uint8_t)0);
}
#endif

/*=====================================================================================
    Fatal Exception
=====================================================================================*/
// The RAM Fatal Exception
void FSF_vRam_FatalException(void)
{
   FSF_vReportFatal();
}

#if (!defined(WIN32) && defined(MC9S12GC96))
#pragma CODE_SEG DEFAULT
#endif
