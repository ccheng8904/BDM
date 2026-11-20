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
 *  COMP_ABBREV      FSF_ROM
 ******************************************************************************/

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file fsRom.c
 *
 *  \ingroup  <DOXY_GROUP_FSF_ROM>
 *
 *  \brief    Implementation of package fsRom of library functional safety. This module continuous watches for CRC errors in ROM.
 *
 *  \details  \n
<b>Functional Safety ROM CRC-16 Supervision Component - Top-Level Design </b>
\n
\n
\li<b>Package Model </b>\n
\image html Package_Rom.jpg
\n
<b>Description </b>\n
The fsRom package provides the parts outlined in the model. In order to save space, there are different configurations,
depending what Rom modules exist and if some the modules are connected by means of I2C, or comparable.
By compilation switch, the Rom task handler may run either in the usual operating mode or in test harness mode. When
running in the usual operating mode, the test harness part will not be linked to the package. When running in test
harness mode, separate data parts are provided, so the real Rom area is separated from being manipulated and only the
test server will be able to change it.
\n
<b>Rationale </b>\n
Test generation requires some ROM (flash memory) parts to be changed and restored again. This is  not so easy to realize. When in test harness mode, certain test patterns will be defined and located into RAM.
\n
\n
\n
\li<b>State Model </b>\n
\image html StateModel_Rom.jpg
\n
<b>Description </b>\n
Before checking the Rom, the Rom task handlers work area must be initialised correctly. This happens the first time the
Rom task handler gets called and every time a full Rom check has been completed. After this initialisation, the Rom task
handler transitions into the state "FSF_ROM_CHECK_ONGOING" and starts walking through the Rom space. On completion, it
transitions back into state "FSF_ROM_CHECK_RESTART" and the process restarts again.
\n
<b>Rationale </b>\n
None
\n
\n
\n
\li<b>Scenario: Rom Supervision </b>\n
\image html SequenceChart_RomSupervision.jpg
\n
<b>Description </b>\n
There is not that much to explain on this scenario.
\n
<b>Rationale </b>\n
None
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
#include "fSafe.h"
#include "FuncSafe/fSafeXS.h"
#include "FuncSafe/fspriv.h"
#include "utility/utility.h"
#include "hsup.h"
#include "FuncSafe/fsramcon.h"
#include "processor.h"

/* Local macros ***************************************************************************************/

/* Local type definitions *****************************************************************************/

/* Local prototypes ********************************************************************************/

STATIC  bool    bIsROMAtOnce        (void);
STATIC  void    vFinalizeROMCheck   (void);
STATIC  void    vROMCheckSuccessfullyCompleted (void);

#ifdef FSF_CRC_32_BIT
STATIC  uint32_t  ui32InitializeCRCValue (void);
#else
STATIC  uint16_t  ui16InitializeCRCValue (void);
#endif

STATIC  void    vROMCheckOngoing     (void);

#ifndef FSF_CRC_32_BIT
/*STATIC*/  uint16_t  uiCalculateCRC  (uint16_t CRC16Value, FSTH_CONST uint8_t *pData, uint16_t ui16length);
#endif
uint32_t  uiSetupNextROMAddress   (uint16_t ui16length);
STATIC  uint8_t   ucInitialiseROMChannel  (void);
STATIC  uint8_t   ucInternalROMTask       (void);
STATIC  bool      bExtIsROMReady          (uint8_t *pucData, uint16_t ui16length);
STATIC  uint8_t*  pucExtROMRead           (uint8_t *pucData, uint16_t ui16length);
STATIC  bool      bIsROMAtOnceAllowed     (void);
STATIC  bool      bCheckINTduringROMCheck (void);

/* RAM/ROM Variable definitions ***********************************************************************/

/*=====================================================================================
    ROM Channel Work Area
=====================================================================================*/

#if defined(TH_FSCOM)

//  Configuration array:
//  ROMConfig[0]    ROMConfig[1]            Meaning
//  0               0                       Small ROM with Indirect ROM Access -> not used
//  0               1                       Small ROM with Direct ROM Access -> not used
//  1               0                       Large ROM with Indirect ROM Access
//  1               1                       Large ROM with Direct ROM Access -> not used

const uint8_t ROMConfig[2] =
{
(uint8_t)1,
(uint8_t)0,
};
#endif

/**
ROM Channel State and work area
*/

/*======================================================================
    Large ROM Model
======================================================================*/
// definition of the memory block for RAM Consistency check
typedef struct _TFS_ROM_RamConBlock {
    uint8_t  ucROMTState;               //!< State of ROM ch.
    uint8_t  ucROMIdx;                  //!< ROM Mod. Index
    uint8_t  ucSegIdx;                  //!< ROM Seg. Index
    uint8_t *pucRomPtr;                 //!< ROM Data
    uint32_t uiROMDLen;                 //!< ROM Data length
    T_ROM_Module  FSTH_CONST *patRom;   //!< ROM Module ref.
    T_ROM_Segment FSTH_CONST *patSeg;   //!< ROM Segment ref.
#ifdef FSF_CRC_32_BIT
    uint32_t ui32Crc;                   //!< CRC Value
#else
    uint16_t uiCrc;                     //!< CRC Value
#endif
}TFS_ROM_RamConBlock;

__ALIGNED(4) STATIC TFS_ROM_RamConBlock FS_ROM_RamConBlock;
__ALIGNED(4) STATIC TFS_ROM_RamConBlock FS_ROM_RamConBlockMirror;
__ALIGNED(4) STATIC uint8_t FS_ROM_ucCheckValueRamConBlock; ///< Holds current consistency check value
STATIC bool FS_ROM_bIsRamConInitDone = false; ///< Flag to check if mirror area is already initialized

//  This is in order to monitor correct work of the ROM channel.
//lint -e551
STATIC  uint8_t   ucROMCheckCycleCount  = (uint8_t)0;

/**
ROM Channel Test Harness
*/
/*======================================================================
    Large ROM Model
======================================================================*/
#if defined(TH_FSCOM)
#ifdef FSF_CRC_32_BIT
    /**  CRC-32 Test Patterns:
    0-Pattern, 1-Pattern, Text-Pattern, each in one separate segment
    */

    __ALIGNED(4) FSTH_CONST   uint8_t FSF_CRC_P0[]            = "000000000000000000000000000000000000";     //size 36
    __ALIGNED(4) FSTH_CONST   uint8_t FSF_CRC_P1[]            = "111111111111111111111111111111111111111111111"; //size 44
    __ALIGNED(4) FSTH_CONST   uint8_t FSF_CRC_P2[]            = "The quick brown fox jumps over the lazy ";//size 40
    //FSTH_CONST   uint32_t FSF_uiCRCTable[2]       = {0xE2F481B8,0x41B8C5CA};
    FSTH_CONST   uint32_t FSF_uiCRCTable[2]       = {0x41B8C5CAu,0xD81E0E94u};
    FSTH_CONST   uint8_t  FSF_ucRomModuleCount    = (uint8_t)2;


    /** Test Segment Tables
    */
    FSTH_CONST   T_ROM_Segment   FSF_atRomSeg_0 []  = { {FSF_CRC_P0,(FSF_CRC_P0 + (sizeof(FSF_CRC_P0)-2))},
                                                   {FSF_CRC_P1,(FSF_CRC_P1 + (sizeof(FSF_CRC_P1)-2))}};

    FSTH_CONST   T_ROM_Segment   FSF_atRomSeg_1 []  = { {FSF_CRC_P2,(FSF_CRC_P2 + (sizeof(FSF_CRC_P2)-2))}};

    FSTH_CONST   T_ROM_Module    FSF_atRomModules[]  = { {FSF_atRomSeg_0,
                                                   (uint8_t)2, (uint16_t)3,
                                                   _FSF_pucReadROMData,
                                                   _FSF_bIsROMReady, (uint32_t*)&FSF_uiCRCTable[0]},

                                                   {FSF_atRomSeg_1,
                                                   (uint8_t)1, (uint16_t)2,
                                                   _FSF_pucReadROMData,
                                                   _FSF_bIsROMReady, (uint32_t*)&FSF_uiCRCTable[1]}};

    STATIC bool TH_FSCOM_bIsROMReady            = true;   //!< Grant access to ROM
    STATIC bool TH_FSCOM_bIsROMAtOnceAllowed    = true;   //!< Enable ROM check at once
    STATIC bool TH_FSF_bCheckINTduringROMCheck  = true;   //!< Maintain INT's while checking

#else
    /**  CRC-16 Test Patterns:
    0-Pattern, 1-Pattern, Text-Pattern, each in one separate segment
    */
        FSTH_CONST   uint8_t   FSF_CRC_P0[]            = "000000000000000000000000000000000000";
        FSTH_CONST   uint8_t   FSF_CRC_P1[]            = "111111111111111111111111111111111111111111111";
        FSTH_CONST   uint8_t   FSF_CRC_P2[]            = "The quick brown fox jumps over the lazy dog";
        FSTH_CONST   uint16_t  FSF_uiCRCTable[2]       = {0x5686,0xF0C8};
        FSTH_CONST   uint8_t   FSF_ucRomModuleCount    = (uint8_t)2;


        /** Test Segment Tables
        */
        FSTH_CONST   T_ROM_Segment   FSF_atRomSeg_0 []  = { {FSF_CRC_P0,(FSF_CRC_P0 + (sizeof(FSF_CRC_P0)-2))},
                                                       {FSF_CRC_P1,(FSF_CRC_P1 + (sizeof(FSF_CRC_P1)-2))}};

        FSTH_CONST   T_ROM_Segment   FSF_atRomSeg_1 []  = { {FSF_CRC_P2,(FSF_CRC_P2 + (sizeof(FSF_CRC_P2)-2))}};

        FSTH_CONST   T_ROM_Module    FSF_atRomModules[]  = { {FSF_atRomSeg_0,
                                                       (uint8_t)2, (uint16_t)13,
                                                       _FSF_pucReadROMData,
                                                       _FSF_bIsROMReady, &FSF_uiCRCTable[0]},

                                                       {FSF_atRomSeg_1,
                                                       (uint8_t)1, (uint16_t)9,
                                                       _FSF_pucReadROMData,
                                                       _FSF_bIsROMReady, &FSF_uiCRCTable[1]}};

        STATIC bool TH_FSCOM_bIsROMReady            = true;   //!< Grant access to ROM
        STATIC bool TH_FSCOM_bIsROMAtOnceAllowed    = true;   //!< Enable ROM check at once
        STATIC bool TH_FSF_bCheckINTduringROMCheck  = true;   //!< Maintain INT's while checking
#endif //#ifdef FSF_CRC_32_BIT
#endif //#if defined(TH_FSCOM)

/* static function definitions ************************************************************************/

/*=====================================================================================
    Large ROM Model
=====================================================================================*/

        /** Setup the next ROM address.
    Move on to the next ROM address. This handler directly changes the ROM channels work area. \n
    Note:   the parameter passed describes the space of ROM already checked!! In order to make \n
            sure the segment boundaries will be matched exactly, on return this procedure will \n
            return the space left to be checked. Returning 0 means, no space left to be checked. \n

    \param      uint16_t  length of data actually calculated \n\n

    \return     uint16_t  space left to check inside actual segment. Some cases to distinguish:
                        return value == length:     end of the ROM has not yet been reached. \n
                        0 < return value < length:  end of ROM has been reached. \n
                        return value == 0:          no more data to be checked \n

    \pre        Called only at operational time. \n
    \post       ROM channel work area updated, if necessary \n
*/
uint32_t  uiSetupNextROMAddress   (uint16_t ui16length)
{
   bool    bSegChanged     = false;
#ifdef FSF_CRC_32_BIT
    uint32_t ui32StoredCrc;
#else
    uint16_t uiStoredCrc;
#endif

   //  Add the size of data already checked and determine the space left inside the actual segment
#ifdef FSF_CRC_32_BIT
   FS_ROM_RamConBlock.pucRomPtr  += ui16length*sizeof(uint32_t);/*lint !e9016 "Pointer arithmetic not allowed MisraC201 18.4 [advisory], required here */
#else
   FS_ROM_RamConBlock.pucRomPtr  += ui16length;
#endif
   //  Msg(4:0488) Performing pointer arithmetic.
   //  MISRA-C:2004 Rule 17.4 <next>
   //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.

   FS_ROM_RamConBlock.uiROMDLen  -= ui16length;
   //  Has this ROM segment been checked?
   if (FS_ROM_RamConBlock.uiROMDLen == 0u)
   {
      FS_ROM_RamConBlock.ucSegIdx++; // Next ROM Segment

      if (FS_ROM_RamConBlock.ucSegIdx >= FS_ROM_RamConBlock.patRom->ucSegmentCount)
      {
         //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
         //  REFERENCE - ISO-6.3.3.2 Semantics
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.

         //  Before move on to the next ROM module, the CRC-16 value from
         //  the current ROM module need to be checked
#ifdef FSF_CRC_32_BIT
          FS_ROM_RamConBlock.ui32Crc = _FSF_ui32FinalizeCrcCalculation(FS_ROM_RamConBlock.ui32Crc);
          ui32StoredCrc = *(FS_ROM_RamConBlock.patRom->pui32CRCValue);
         if (FS_ROM_RamConBlock.ui32Crc != ui32StoredCrc)
#else//#ifdef FSF_CRC_32_BIT
         uiStoredCrc = *(FS_ROM_RamConBlock.patRom->pui16CRCValue);
         if (FS_ROM_RamConBlock.uiCrc != uiStoredCrc)
#endif//#ifdef FSF_CRC_32_BIT

         {
            // Rom check is always success (Error is ignored)
             if(!FWU_isIntegCheckDisabledCalc() && !FWU_isIntegCheckErrorIgnored())
             {
                 FSF_vReportError((uint8_t)FSF_ROM_TEST, FSF_ROM_CRC_ERROR,(uint8_t)FS_ROM_RamConBlock.ucROMIdx);
             }
         }

         FS_ROM_RamConBlock.ucROMIdx++; // Next ROM Module
         if (FS_ROM_RamConBlock.ucROMIdx < FSF_ucRomModuleCount)
         {
            FS_ROM_RamConBlock.patRom++;
            //  Msg(2:0510) Arithmetic operation on pointer value that is possibly NULL.
            //  Msg(4:0489) Increment or decrement operation performed on pointer.
            //  MISRA-C:2004 Rule 17.4 <next>
            //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.

            FS_ROM_RamConBlock.patSeg          = FS_ROM_RamConBlock.patRom->patSegment;
            //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
            //  REFERENCE - ISO-6.3.3.2 Semantics
            //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
#ifdef FSF_CRC_32_BIT
            FS_ROM_RamConBlock.ui32Crc           = (uint32_t)FSF_CRC_INIT;
            // Reset CRC calculation was moved into Xs file, because lib user now can select crc
            // calculation type (SW/HW/...)
            (void)_FSF_ui32ResetCrcCalculation();
#else
            FS_ROM_RamConBlock.uiCrc           = (uint16_t)FSF_CRC_INIT;
#endif
            bSegChanged     = true;
            FS_ROM_RamConBlock.ucSegIdx        = (uint8_t)0;

//  Testpoint "ROM Module changed"
#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ROM_Module_Changed);
#endif
         }
      }
      else
      {
         // Next ROM segment
         FS_ROM_RamConBlock.patSeg++;
         //  Msg(2:0510) Arithmetic operation on pointer value that is possibly NULL.
         //  Msg(4:0489) Increment or decrement operation performed on pointer.
         //  MISRA-C:2004 Rule 17.4
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.

         bSegChanged = true;
//  Testpoint "ROM Segment changed"
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ROM_Segment_Changed);
#endif
      }
      if (bSegChanged != false)
      {
         // Segment has changed
         FS_ROM_RamConBlock.pucRomPtr      = FS_ROM_RamConBlock.patSeg->pucStartSeg;
         //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
         //  REFERENCE - ISO-6.3.3.2 Semantics
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
         /*lint -save -e946 -e947 -e9029 tested to work correctly */
#ifdef FSF_CRC_32_BIT
         FS_ROM_RamConBlock.uiROMDLen      = (((FS_ROM_RamConBlock.patSeg->pucEndSeg - FS_ROM_RamConBlock.pucRomPtr) + 1u) / sizeof(uint32_t));
#else
         FS_ROM_RamConBlock.uiROMDLen      = (((FS_ROM_RamConBlock.patSeg->pucEndSeg) - FS_ROM_RamConBlock.pucRomPtr)) + 1u;
         //  Msg(4:0488) Performing pointer arithmetic.
         //  MISRA-C:2004 Rule 17.4 <next>
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
         //  Computing distances of locations assumed to be legal pointer operation.
#endif
         /*lint -restore -e946 -e947 -e9029 */
      }
   }

   return FS_ROM_RamConBlock.uiROMDLen;
}

/** Initialise the ROM channels work area.

    \return     uint8_t   FSF_ROM_CHECK_ONGOING, if ROM exists. FSF_ROM_CHECK_RESTART otherwise.

    \pre        Called only at operational time. \n
    \post       ROM channel work area updated \n
*/
STATIC  uint8_t   ucInitialiseROMChannel  (void)
{
   if(FSF_ucRomModuleCount != (uint8_t)0)
   {
      FS_ROM_RamConBlock.ucROMIdx      = (uint8_t)0;                         //!< First ROM Mod. Index
      FS_ROM_RamConBlock.ucSegIdx      = (uint8_t)0;                         //!< First ROM Seg. Index
      FS_ROM_RamConBlock.patRom        = (T_ROM_Module FSTH_CONST *)(&FSF_atRomModules[0]); /*lint !e927 !e929 cast tested to work correctly */
      FS_ROM_RamConBlock.patSeg        = FS_ROM_RamConBlock.patRom->patSegment;               //!< First ROM Segment
      FS_ROM_RamConBlock.pucRomPtr     = FS_ROM_RamConBlock.patSeg->pucStartSeg;              //!< First Data Position in ROM

      /*lint -save -e946 -e947 -e9029 tested to work correctly */
#ifdef FSF_CRC_32_BIT
      FS_ROM_RamConBlock.uiROMDLen     = (((FS_ROM_RamConBlock.patSeg->pucEndSeg - FS_ROM_RamConBlock.pucRomPtr) + 1u) / sizeof(uint32_t)); //!< Partial CRC Data length
      FS_ROM_RamConBlock.ui32Crc        = ui32InitializeCRCValue();         //!< CRC Initial value
      /* Reset CRC generator */
      // Reset CRC calculation was moved into Xs file, because lib user now can select crc
      // calculation type (SW/HW/...)
      (void)_FSF_ui32ResetCrcCalculation();
#else
      FS_ROM_RamConBlock.uiROMDLen     = (((FS_ROM_RamConBlock.patSeg->pucEndSeg) - FS_ROM_RamConBlock.pucRomPtr)) + 1u; //!< Partial CRC Data length
      //  Msg(4:0488) Performing pointer arithmetic.
      //  MISRA-C:2004 Rule 17.4 <next>
      //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
      //  Computing distances of locations assumed to be legal pointer operation.

      FS_ROM_RamConBlock.uiCrc         = ui16InitializeCRCValue();         //!< CRC Initial value
#endif
      /*lint -restore -e946 -e947 -e9029 */

      return (uint8_t)FSF_ROM_CHECK_ONGOING;
   }
   else
   {
      return (uint8_t)FSF_ROM_CHECK_RESTART;
   }
}


//  ROM Indirect Access
STATIC  uint8_t   ucInternalROMTask   (void)
{
   if (FSF_ucRomModuleCount == 0u)
   {
//  Testpoint "No ROM Configuration"
#ifdef TH_FSCOM
      TH_FSFP_PushOutPar(TP_FSF_ROM_No_Configuration);
#endif
      //  Return this value in order not to run into an indefinite loop.
      return  (uint8_t)TASK_NOT_INITIALISED;
   }
   switch (FS_ROM_RamConBlock.ucROMTState)
   {
      case (uint8_t)FSF_ROM_CHECK_RESTART:
      {
/*======================================================================
    ROM Check Restart
======================================================================*/
         //  Report ongoing. The ROM channel is about to start
         vROMCheckOngoing();
         //  (Re-)Initialise the ROM channel (again)
         FS_ROM_RamConBlock.ucROMTState = ucInitialiseROMChannel();
         //  Restart the ROM check again and again and ..

//  Testpoint "ROM check restart"
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ROM_Check_Restart);
#endif
         break;
      }
      case (uint8_t)FSF_ROM_CHECK_ONGOING:
      {
/*======================================================================
    ROM Check Ongoing
======================================================================*/
         uint16_t ui16Partlen = FS_ROM_RamConBlock.patRom->ui16CycleBytesCount;
         //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
         //  REFERENCE - ISO-6.3.3.2 Semantics
         //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.

         //  Make sure, the rest of data does not exceed the segment boundary
         if (ui16Partlen > FS_ROM_RamConBlock.uiROMDLen)
         {
            ui16Partlen = (uint16_t)FS_ROM_RamConBlock.uiROMDLen;
         }

         //  Report ongoing, even for ROM parts that are not ready for
         //  access. If such type of ROM remains unaccessable, the ROM
         //  channel will fail as soon as the check cycle time has elapsed.
         vROMCheckOngoing();
         //  ROM parts must be accessible before checking. Otherwise,
         //  suspend the actual check cycle. Do not move on to the next
         //  ROM data.
         if (bExtIsROMReady(FS_ROM_RamConBlock.pucRomPtr,ui16Partlen) != false)
         {
            //  Calculate a partial data area
#ifdef FSF_CRC_32_BIT
            FS_ROM_RamConBlock.ui32Crc = uiCalculateCRC32  (FS_ROM_RamConBlock.ui32Crc, (FSTH_CONST uint32_t*)pucExtROMRead((uint8_t*)FS_ROM_RamConBlock.pucRomPtr,ui16Partlen),ui16Partlen);/*lint !e927 !e926 cast tested to work correctly */
#else
            FS_ROM_RamConBlock.uiCrc = uiCalculateCRC  (FS_ROM_RamConBlock.uiCrc, (FSTH_CONST uint8_t*)pucExtROMRead(FS_ROM_RamConBlock.pucRomPtr,ui16Partlen),ui16Partlen);/*lint !e927 !e926 cast tested to work correctly */
#endif

            //  Calculate the next ROM data area
            FS_ROM_RamConBlock.uiROMDLen = uiSetupNextROMAddress (ui16Partlen);
            //  The space left is returned. If 0 (zero), all ROM modules have
            //  been checked. So, restart the check again.
            if (FS_ROM_RamConBlock.uiROMDLen == 0u)
            {
               //  The ROM channel has completed one full ROM check.
               vFinalizeROMCheck();
               //  Some configurations want to check the ROM space only once.
               if (bIsROMAtOnce() != false)
               {
                  //  ROM check has finished
                  FS_ROM_RamConBlock.ucROMTState = (uint8_t)FSF_ROM_CHECK_FINISH;
               }
               else
               {
                  // Re-Initialise the work area (again)
                  FS_ROM_RamConBlock.ucROMTState = (uint8_t)FSF_ROM_CHECK_RESTART;
               }
            }
         }
         else
         {

//  Testpoint "ROM not ready"
#ifdef TH_FSCOM
            TH_FSFP_PushOutPar(TP_FSF_ROM_Not_Ready);
#endif
         }
         break;
      }
      case (uint8_t)FSF_ROM_CHECK_FINISH:
      {
/*======================================================================
    ROM Check Finished
======================================================================*/
         //  Report success. The ROM channel has completed one full ROM check.
         vROMCheckSuccessfullyCompleted ();
         break;
      }
      default:
      {
/*======================================================================
    Report Error due to invalid state
======================================================================*/
         FSF_vReportError((uint8_t)FSF_ROM_TEST,(uint8_t)FSF_ERR_INVALID_STATE,(uint8_t)0);
         break;
      }
   }

   return  (uint8_t)TASK_INITIALISED;
}

/*======================================================================
    Stub Interface Implementations
======================================================================*/
/** This wrapper function separate the test harness from overloaded application.

    \pre       patROM must be valid!\n
*/
STATIC  bool    bExtIsROMReady (uint8_t *pucData, uint16_t ui16length)
{
//  Testpoint "ROM Ready"
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_ROM_Is_Ready);
   return TH_FSCOM_bIsROMReady;
#else
   return  FS_ROM_RamConBlock.patRom->pfROMReady(pucData,ui16length);
   //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
   //  REFERENCE - ISO-6.3.3.2 Semantics
   //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
#endif
}

/** This wrapper function separate the test harness from overloaded application.

    \pre       patROM must be valid!\n
*/

STATIC uint8_t*  pucExtROMRead (uint8_t *pucData, uint16_t ui16length)
{
//  Testpoint "ROM Data read"
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_ROM_Data_Read);
#endif
   return  FS_ROM_RamConBlock.patRom->pfROMRead(pucData,ui16length);
   //  Msg(2:0506) Dereferencing pointer value that is possibly NULL.
   //  REFERENCE - ISO-6.3.3.2 Semantics
   //  PCA comment: code tested and reviewed to work correctly. Thus, left unchanged.
}

/** Check, if ROM at once is possible to start.
    Default implementation for checking the ROM check ready state. \n

    \param      none

    \return     bool        true, if the ROM check may start, false otherwise.\n\n

    \pre        Called only at operational time. Called each time BEFORE start of check\n
    \post       If false is returned, the currect check cycle is supended and resumed again \n
                when dispatched the next time.\n\n

    \note       Overloading is not designed here, as this is up to an application. \n
*/

STATIC  bool    bIsROMAtOnceAllowed     (void)
{
#ifdef TH_FSCOM
//  Testpoint "TP_FSF_ROM_Check_Allowed"
   TH_FSFP_PushOutPar(TP_FSF_ROM_Check_Allowed);

   return TH_FSCOM_bIsROMAtOnceAllowed;
#else
   return _FSF_bIsROMAtOnceAllowed();
#endif
}

/** Check, if ROM get checked at once or piece by piece.
    Default implementation for getting the type of ROM check. \n

    \param      none

    \return     bool        true, if the ROM get checked at once, false otherwise.\n\n

    \pre        Called only at operational time. \n

    \note       Overloading is not designed here, as this is up to an application. \n
*/

STATIC  bool    bIsROMAtOnce     (void)
{
#ifdef TH_FSCOM
   return TH_FSF_bROM_At_Once;
#else
   return _FSF_bIsROMAtOnce();
#endif
}

/** Finalize the actual ROM check cycle
    This function is called once a ROM check cycle has completed successfully. \n

    \param      none

    \pre        The state of the ROM channel changes into state success. \n
*/
STATIC  void    vFinalizeROMCheck     (void)
{
   //  Report success. The ROM channel has completed one full ROM check.
   vROMCheckSuccessfullyCompleted ();
   //  Notify the application on completion of one ROM check cycle
   _FSF_vNotifyROMCheckComplete();
   //  Increase the check cycle counter
   ucROMCheckCycleCount++;

//  Testpoint "TP_FSF_ROM_Check_Finished"
#ifdef TH_FSCOM
   TH_FSFP_PushOutPar(TP_FSF_ROM_Check_Finished);
#endif
}

/** ROM Check successfully completed
    This function is called if a ROM check cycle has completed successfully. \n

    \param      none

    \pre        none. \n
*/
STATIC  void    vROMCheckSuccessfullyCompleted     (void)
{
#ifdef SYSTEMTEST_F15
   //  System Test Case F15: The ROM channel remains in state idle (no ongoing, no success)
   return;
#endif
#ifdef SYSTEMTEST_F19
   //  System Test Case F19: The ROM channel does not complete a check (no success)
   FSF_vReportOnGoing((uint8_t)FSF_ROM_TEST);

   return;
#endif
#ifdef SYSTEMTEST_F16
   //  System Test Case F16: Use wrong channel identifier.
   FSF_vReportSuccess((uint8_t)255);

   return;
#endif

   //  Report success. The ROM channel has completed one full ROM check.
   FSF_vReportSuccess((uint8_t)FSF_ROM_TEST);
}

/** ROM Check Ongoing
    This function is called if a ROM check cycle is ongoing. \n

    \param      none

    \pre        none. \n
*/
STATIC  void    vROMCheckOngoing     (void)
{
#ifndef SYSTEMTEST_F15
   //  Report ongoing ROM check.
   FSF_vReportOnGoing((uint8_t)FSF_ROM_TEST);
#else
   //  System Test Case F15
   //  The ROM channel is not ongoin
#endif
}

/** Return the CRC initialisation value
    This function return the CRC initialization value. \n

    \return     the 16Bit-CRC value. \n

    \pre        none. \n
*/
#ifdef FSF_CRC_32_BIT
STATIC  uint32_t    ui32InitializeCRCValue (void)
{
#ifdef SYSTEMTEST_F20
   return (uint32_t)(FSF_CRC_INIT + 1);
#else

   return (uint32_t)FSF_CRC_INIT;
#endif
}
#else
STATIC  uint16_t ui16InitializeCRCValue (void)
{
#ifdef SYSTEMTEST_F20
   return (uint16_t)(FSF_CRC_INIT + 1);
#else

   return (uint16_t)FSF_CRC_INIT;
#endif
}
#endif


/** While checking the ROM at once, this interface decides if interrupts get maintained or not.
    This is mainly intended due to performance reasons. While checking the ROM without maintaining
    the interrupts, some overflow conditions may occurre. Otherwise, maintaining a huge number of
    interrupt sources is time consuming, hence it will lengthen the whole ROM check time. So, by
    this interface, the user has the ability to decide whether to maintain the interrupts or not.
    The ROM channel internally provide this interface and it returns true by default. This interface
    may be overloaded, if necessary. As an example: if interrupts need to be  maintained at least
    once every second, and checking the whole ROM space at once takes only half a second, in that
    case there is no need to maintain the interrupt sources while checking.\n

    \param      none\n
    \return     true, if interrupts have to maintain, false otherwise.

    \pre        Called only at operational time.\n

    \note       Overloading is not designed here, as this is up to an application. \n
*/

STATIC  bool    bCheckINTduringROMCheck     (void)
{
#ifdef TH_FSCOM
   return   TH_FSF_bCheckINTduringROMCheck;
#else
   return  _FSF_bCheckINTduringROMCheck();
#endif
}

/* exported function definitions ***********************************************************************/

/** Schedule ROM Channel.
   By this handler, the ROM channel is scheduled on a regular base.

   \return     uint8_t  TASK_NOT_INITIALISED:   ROM channel check not started yet or restarted\n
                        TASK_INITIALISED:       ROM channel check cycle completed\n

    \pre        FSCOM runs in its operational state\n
    \post       Partial ROM data checked.
*/
uint8_t FSF_ucRomHandleTask(void)
{
   uint8_t   ucRetVal = (uint8_t)TASK_INITIALISED;

   if(FWU_isIntegCheckDisabledCalc())
   {
       vROMCheckSuccessfullyCompleted();
       return ucRetVal;
   }
   _FSF_RAM_CON_vDoCheckInRomModule();

   //  Ask the application, if the ROM check is allowed to start. Do it only once per full check!
   //  Thus, the application is asked in state FSF_ROM_CHECK_RESTART. In case of refusal, do not
   //  start so the state of the channel remains unchanged. Instaed ask the application again.
   if (FS_ROM_RamConBlock.ucROMTState == (uint8_t)FSF_ROM_CHECK_RESTART)
   {
      if (bIsROMAtOnceAllowed() == false)
      {
         //  Notify to the generic that the ROM channel is keeping alive. Otherwise, an idle error
         //  get created by the channel itself.
         //  PCA, 04/07/2006: do not signal ongoing work, if the ROM check is not allowed actually, as
         //  this would produce a check completion overdue error. Instaed, signal succes.
         vROMCheckSuccessfullyCompleted();

//  Testpoint "TP_FSF_ROM_Check_Refused"
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ROM_Check_Refused);
#endif
         //no secure data touched --> no calculation of checksum
         return (uint8_t)TASK_INITIALISED;
      }
   }
   if (bIsROMAtOnce() == false)
   {
/*======================================================================
    Check the ROM piece by piece
======================================================================*/
      /** If the ROM space is checked piece by piece, the internal ROM task handler get called continuously
      */

//  Testpoint "TP_FSF_ROM_Check_Fragmented"
#ifdef TH_FSCOM
      //  Does the ROM check restart (again)?
      if (FS_ROM_RamConBlock.ucROMTState != (uint8_t)FSF_ROM_CHECK_FINISH)
      {
         TH_FSFP_PushOutPar(TP_FSF_ROM_Check_Fragmented);
      }
#endif
      ucRetVal = ucInternalROMTask ();
   }
   else
   {
/*======================================================================
    Check the ROM at once
======================================================================*/
      /** If the ROM space has to be checked at once, the internal ROM task handler get called only once.
          If this check completes successfully, this result is notified to FSCOM.
      */
      //  ROM check completed?
      if (FS_ROM_RamConBlock.ucROMTState == (uint8_t)FSF_ROM_CHECK_FINISH)
      {
         ucRetVal = ucInternalROMTask ();
      }
      else
      {
//  Testpoint "TP_FSF_ROM_Check_At_Once"
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ROM_Check_At_Once);
#endif
         if (bCheckINTduringROMCheck() == false)
         {
            //  Interrupt maintenance is not wanted while checking the ROM, So, do it only
            //  once before start
            (void)FSF_ucIsrHandleTask ();
         }
         //  Loop until the whole ROM space is checked.
         while (FS_ROM_RamConBlock.ucROMTState != (uint8_t)FSF_ROM_CHECK_FINISH)
         {
            ucRetVal = ucInternalROMTask ();

            //  Avoid problems concerning the internal watchdog
            vExtTriggerInternalWatchdog();

            //  DO NOT TRIGGER THE EXTERNAL WATCHDOG AT THIS MOMENT IN TIME AS THIS WOULD CHANGE THE STATE OF IT !!!
            if (ucRetVal != (uint8_t)TASK_NOT_INITIALISED)
            {
               if (bCheckINTduringROMCheck() == true)
               {
                  //  Call the Isr Handle Task. This is in order to avoid event overflow!
                  ucRetVal = FSF_ucIsrHandleTask ();
               }
            }
            if (ucRetVal == (uint8_t)TASK_NOT_INITIALISED)
            {
               break;
               //  Msg(2:0769) This break statement terminates an iteration statement.
               //  PCA comment: desired state and better readable in X32, code tested and reviewed to work correctly. Thus, left unchanged.
            }
         }
      }
   }

   /* calc new checksum at exit of task handler */
   _FSF_RAM_CON_vDoUpdateCheckInRomModule();

   return ucRetVal;
}

uint8_t FSF_ucRomTinyHandleTask(void)
{
   uint8_t   ucRetVal = (uint8_t)TASK_INITIALISED;
   //  Ask the application, if the ROM check is allowed to start. Do it only once per full check!
   //  Thus, the application is asked in state FSF_ROM_CHECK_RESTART. In case of refusal, do not
   //  start so the state of the channel remains unchanged. Instaed ask the application again.
   if (FS_ROM_RamConBlock.ucROMTState == (uint8_t)FSF_ROM_CHECK_RESTART)
   {
      if (bIsROMAtOnceAllowed() == false)
      {
         //  Notify to the generic that the ROM channel is keeping alive. Otherwise, an idle error
         //  get created by the channel itself.
         //  PCA, 04/07/2006: do not signal ongoing work, if the ROM check is not allowed actually, as
         //  this would produce a check completion overdue error. Instaed, signal succes.
         vROMCheckSuccessfullyCompleted();
//  Testpoint "TP_FSF_ROM_Check_Refused"
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ROM_Check_Refused);
#endif
         //no secure data touched --> no calculation of checksum
         return (uint8_t)TASK_INITIALISED;
      }
   }
   if (bIsROMAtOnce() == false)
   {
/*======================================================================
    Check the ROM piece by piece
======================================================================*/
      /** If the ROM space is checked piece by piece, the internal ROM task handler get called continuously
      */

//  Testpoint "TP_FSF_ROM_Check_Fragmented"
#ifdef TH_FSCOM
      //  Does the ROM check restart (again)?
      if (FS_ROM_RamConBlock.ucROMTState != (uint8_t)FSF_ROM_CHECK_FINISH)
      {
         TH_FSFP_PushOutPar(TP_FSF_ROM_Check_Fragmented);
      }
#endif
      ucRetVal = ucInternalROMTask ();
   }
   else
   {
/*======================================================================
    Check the ROM at once
======================================================================*/
      /** If the ROM space has to be checked at once, the internal ROM task handler get called only once.
          If this check completes successfully, this result is notified to FSCOM.
      */
      //  ROM check completed?
      if (FS_ROM_RamConBlock.ucROMTState == (uint8_t)FSF_ROM_CHECK_FINISH)
      {
         ucRetVal = ucInternalROMTask ();
      }
      else
      {
//  Testpoint "TP_FSF_ROM_Check_At_Once"
#ifdef TH_FSCOM
         TH_FSFP_PushOutPar(TP_FSF_ROM_Check_At_Once);
#endif
         //  Loop until the whole ROM space is checked.
         while (FS_ROM_RamConBlock.ucROMTState != (uint8_t)FSF_ROM_CHECK_FINISH)
         {
            ucRetVal = ucInternalROMTask ();

            //  Avoid problems concerning the internal watchdog
            vExtTriggerInternalWatchdog();

            //  DO NOT TRIGGER THE EXTERNAL WATCHDOG AT THIS MOMENT IN TIME AS THIS WOULD CHANGE THE STATE OF IT !!!
            if (ucRetVal != (uint8_t)TASK_NOT_INITIALISED)
            {

            }
            if (ucRetVal == (uint8_t)TASK_NOT_INITIALISED)
            {
               break;
               //  Msg(2:0769) This break statement terminates an iteration statement.
               //  PCA comment: desired state and better readable in X32, code tested and reviewed to work correctly. Thus, left unchanged.
            }
         }
      }
   }

   return ucRetVal;
}


void _FSF_RAM_CON_vRomModuleCheckBySum(void)
{
     _FSF_RAM_CON_vCheckBySum(FS_ROM_ucCheckValueRamConBlock,
        (uint8_t)sizeof(FS_ROM_RamConBlock),
        (const uint8_t*)(&FS_ROM_RamConBlock), /*lint !e927 !e928 cast tested to work correctly */
        (uint8_t)(FSF_ERR_RAM_CON_IN_ROM));
}

void _FSF_RAM_CON_vRomModuleCheckByMirror(void)
{
    if (FS_ROM_bIsRamConInitDone == true) {
        _FSF_RAM_CON_vCheckByMirror((uint8_t) sizeof(FS_ROM_RamConBlock),
                (const uint8_t*)(&(FS_ROM_RamConBlock)), /*lint !e927 !e928 cast tested to work correctly */
                (const uint8_t*)(&(FS_ROM_RamConBlockMirror)), /*lint !e927 !e928 cast tested to work correctly */
                (uint8_t) (FSF_ERR_RAM_CON_IN_ROM));
    } else {
        FS_ROM_bIsRamConInitDone = true;
    }
}

/*Axivion Disable MisraC2012-1.1 : Number of significant initial characters for external identifier not a problem with our tools */
void _FSF_RAM_CON_vRomModuleUpdateCheckBySum(void)
{
    _FSF_RAM_CON_vUpdateCheckBySum((uint8_t*)(&(FS_ROM_ucCheckValueRamConBlock)), /*lint !e927 !e926 cast tested to work correctly */
        (uint8_t)sizeof(FS_ROM_RamConBlock),
        (const uint8_t*)(&FS_ROM_RamConBlock)); /*lint !e927 !e928 cast tested to work correctly */
}

void _FSF_RAM_CON_vRomModuleUpdateCheckByMirror(void)
{
    _FSF_RAM_CON_vUpdateCheckByMirror((uint8_t)sizeof(FS_ROM_RamConBlock),
        (const uint8_t*)(&(FS_ROM_RamConBlock)), /*lint !e927 !e928 cast tested to work correctly */
        (uint8_t*)(&(FS_ROM_RamConBlockMirror)));/*lint !e927 !e928 cast tested to work correctly */
}
/*Axivion Enable MisraC2012-1.1 */
