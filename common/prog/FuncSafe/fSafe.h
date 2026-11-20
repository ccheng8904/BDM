/*******************************************************************************
 *  Copyright (c) 2019 BSH Hausgeraete GmbH,
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
/*lint -save -e9059 C comment contains C++ comment*/
/**
\if funcsafe
\file
 Public declarations and definitions of library functional safety
\endif
*/

/**
\if funcsafe
\mainpage FSF - Functional Safety
\else
\file
 Public declarations and definitions of library functional safety
\endif

\section intro FSF Functional SaFety
   The <i> Functional SaFety</i> \b FSF Library consists of several modules:\n

  The generic management part:
  \li fscom.c provides boot time and operational services\n

  Pre-defined (micro-controller related) channels:
  \li fsIsr.c for checking interrupt service routines
  \li fsRam.c for checking RAM
  \li fsRom.c for checking ROM
  \li fsStack.c for checking the stack
  \li fsReg.c for checking the registers

  \subsection subsection1 Library
 For applications that do not make use of the dispatching component, the library itself provides a dispatching function FSF_ucComHandleTask(), that calls
 all relevant library modules mentioned in the section above.

 \subsection subsection2 Headers
 fSafe.h contains public (global) information, which is needed when using functional safety library modules.\n
 fspriv.h contains private information and declarations needed for the functional safety library modules.

 \subsection subsection3 Excess templates (source files and headers)
 The templates are application dependent, i.e. a template may be changed according to project needs.\n

  \li fSafeXS.c for application specific implementations
  \li fSafeXS.h for application specific definitions (and possibly application specific function prototypes, however functions, which are needed by the library may not be added in the excess template, as the functions will not be known by compilation of the library).

 \subsection subsection4 Reference to input documents
This design is strongly based on its input document and make use of references to it.\n
Table of Input documents:\n
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

 \subsection subsection5 Generic SW Module
The generic module fscom.c provides all the functions, while fSafe.h provides exported parts. fspriv.h consists of parts locally to the generic module. \n\n

 \subsection subsection6 Step-by-Step Procedures on how to create the static configuration
<TABLE width=900 align=center style="text-align: center"> <TR style="background-color:rgb(255,255,206)">
<TD> <b> Functional Safety Channel Configuration  </b> </TD> </TR> </TABLE>

 \subsubsection subsubsection1 Add more channels
This example is based on chapter 8.7.1 of [1]. It has to be noticed that adding a new channel does not need a recompilation of the library. Instead, extensions have to be made at the top-level header file fSafe.h and the application-dependent implementation file fsApplication.c\n

\li <b> Step 1: Extend the Channel Addressing Enumeration (in fSafe.h). </b>\n
At the end of the already existing list of channels add two new channels, the Washing Programme Monitor Channel and the Switching Relay Channel.\n\n
<pre>
    add FSF_WASH_PROG_MON
    and FSF_SWITCH_RELAY
</pre>\n

\li <b> Step 2: Make a task handler known to FSCOM, if needed (in fSafe.h). </b>\n
Provide a task handler for the scheduler. Add it to the list of already existing scheduler functions.\n\n
<pre>
    extern  uint8_t   FSF_ucWashProgMonHandleTask(void);
    extern  uint8_t   FSF_ucSwitchRelayHandleTask(void);
</pre>\n

\li <b> Step 3: Provide a Pre-condition handler for a channel, if needed (in fSafeXS.c). </b>\n
Provide a pre-condition for a channel and make it known to FSCOM. Add it to the external function definitions part.\n\n
<pre>
    extern  Bool    FSF_bGetWashProgMonCondition(void);
</pre>\n
In the example, the washing programme monitor asks the Switching Relay channel about its success status, which is performed by the pre-condition above.\n

\li <b> Step 4: Create a data structure that describes a channel (in fSafeXS.c). </b>\n
At application level, this step requires some questions to be clarified:\n that is\n
Q1: does the channel use the check cycle supervision ?\n
Q2 does the channel provide a one-time check to be executed once while boot-time ?\n
Q3 does the channel provide a pre-condition to be fulfilled before a transition takes place into state Ongoing?\n
Q4 does the channel provide a task handler to be called on a regular base\n\n
If a channel make use of the check cycle supervision provided by FSCOM, then the data structure member CheckCycleTime holds the check time maximum measured in pieces of 100 milliseconds, otherwise a zero value has to be used.\n\n
For the channels mentioned in the example:\n
It is assumed, that the Switching Realy channel completes a check cycle within less then 100 milliseconds, so this value is set to 1. The multiplier value is also set to 1 as this check does not require a bigger span than the base cycle. The switching relay has to be checked on a regular base, so the flag for this purpose is set true. There is no special pre-condition provided and there is no one-time check to be executed at boot-time. Then, the data structure looks like:\n\n
<pre>
const   struct  T_FSCOM_Channel     Switching_Relay_Channel =
        {(uint16_t)1,(uint8_t)1,
        true,
        _FSF_bDefaultPreconditionTRUE,
        _FSF_vDefaultOneTimeCheck};
</pre>\n
It is assumed, that a Washing Programme completes in a certain time, hence check cycle supervision is configured. In order to recognize an ongoing washing programme, this channel also becomes checked on a regular base. A special pre-condition provides the success status of the Switching Relay channel. There is no one-time check to be executed at boot-time. Then, the data structure looks like:\n\n
<pre>
const   struct  T_FSCOM_Channel     Wash_Prog_Monitor_Channel =
            {(uint16_t)MAX_WASHING_PROGRAM_CYCLE,(uint8_t)1,
            true,
            FSF_bGetWashProgMonCondition,
            _FSF_vDefaultOneTimeCheck};
</pre>\n
\li <b> Step 5: Complete the application data structure (in fSafeXS.c). </b>\n
Since FSCOM doesn't know what channels are to service, the number of channels has to be provided together with channel data structures. Correct channel addressing provides the count of channels automatically (FSF_NR_OF_CHANNELS).\n\n
<pre>
const   struct  T_FSCOM_Configuration   _FSCOM_Configuration = {
            (uint16_t)FSF_NR_OF_CHANNELS,
            ....parts not of interrest have been omitted
            &Switching_Relay_Channel,       // added the Switching Relay channel
            &Wash_Prog_Monitor_Channel,     // added the Washing Programme Monitor channel
            ....rest of initialisation
            };
</pre>

<TABLE width=900 align=center style="text-align: center"> <TR style="background-color:rgb(255,255,206)">
<TD> <b> RAM Module Configuration  </b> </TD> </TR> </TABLE>
 \subsubsection subsubsection2 Create RAM module descriptor and segments to be checked
\li Controls that describe the RAM configuration \n
The RAM configuration will be built at compile-time (make file and its included parts). In the table below, the available set of controls is summarized. \n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>Control </TD> <TD>Meaning </TD> </TR>
<TR> <TD>    OPERATIONAL_RAM_CHECK     </TD> <TD>   If this control exists, when operational the RAM get checked continuously.  </TD> </TR>
<TR> <TD>    MARCH_BUFFER_LENGTH       </TD> <TD>   While checking the RAM, the actual RAM content need to be saved and restored again. For this, a buffer
of fixed length will be used. This control determines the buffer size which is equal to the length of the piece of RAM that is checked per cycle.  </TD> </TR>
</TABLE>
\n
\li RAM Data structure overview \n
Below the basic RAM configurations are shown.
\n
\image html funsafe_RAMConfiguration.jpg
\n
\n
\li <b> Step 1: Analyse the RAM structure and what segments need to be checked (fSafeXS.c). </b> \n
Most of the micro controller do not provide the RAM as a contiguous area. Special function registers (SFR) are placed in between. In order to solve this
problem, a number of segments may be defined in a table. The number of available segments must be placed inside the RAM module descriptor.
\n
\n
\li <b> Step 2: Provide the set of MARCH Patterns to be used (fSafeXS.c). </b> \n
Cluster structures of RAM cells differ between types of controllers. Moreover, the RAM get checked byte wise. One commen approach will be usage of four
patterns. Zero-entries (0, the not-inverted pattern) in the left side of the pattern table are: 0x00, 0xF0, 0xCC and 0xAA. One-entries (1, the inverted
pattern) in the right side of the pattern table are: 0xFF, 0x0F, 0x33 and 0x55. Those set of patterns seem to be sufficient for an 8 Bit word size.
The number of available patterns must be placed inside the RAM module descriptor.
\n
\n
\li <b> Step 3: Provide a data exchange buffer and its length (fSafeXS.c). </b> \n
The data buffer arMarchDataBuffer will be used for data saving and restore. It is recommended to set its size to 8 byte length. This size is sufficient
for most of the micro controllers actually in use.
\n
\n
\li <b> Step 4: Provide the RAM channel data structure with limited processing time (fSafeXS.c). </b> \n
The RAM shall be checked completely within 5 seconds. This limitation holds for only one pattern. Then, following computation is valid: if the RAM channel
get control every 10 milliseconds, then within 5 seconds it is scheduled 500 times, multiplied by a buffer length of 8 byte (see explanation of control
MARCH_BUFFER_LENGTH) results into 4KB every 5 seconds. The time consumed for one check cycle would be 20 microseconds. While checking, interrupts must
be disabled. But, this small amount of processing time does not create problems for most of the applications. So, the RAM channel has to configure its
cycle supervision:
\n
<pre>
        typedef struct  FSTH_CONST T_FSCOM_Channel FSF_sRAMChannel

        Initial value:

        {(uint16_t)RAM_CYCLE_CHECK_TIME,\
         (uint8_t)1,\
         true,\
         .....
         }

        The RAM channel.
</pre>
\n
\n
<TABLE width=900 align=center style="text-align: center"> <TR style="background-color:rgb(255,255,206)">
<TD> <b> ROM Module Configuration  </b> </TD> </TR> </TABLE>
 \subsubsection subsubsection3 Create ROM module descriptors and segments to be checked or excluded
<b> Supported ROM Memory Models and other things to consider </b> \n
\li The ROM SMALL or TINY Model: \n
It supports <b> only one ROM module </b> and this module must be located completely into one bank (bank \n
number 0 or any other). Usage of 24-bit addresses is possible. Multiple segments are possible, so the \n address ranges need not to be contigues. \n
\n
\li The ROM LARGE Model: \n
It supports <b> a number of ROM modules </b> located into diffent banks. So, 24-bit addresses \n
are required. Each ROM module may have multiple segments, so the address ranges need not to be \n
contigues. In this model, different physical ROM modules (ROM directly addressable by CPU and EEPROM \n
accesses by I2C bus) can be mixed. \n
\n
\li ROM Channel Task Handlers \n
The ROM channel provides two task handlers, one for each ROM model.\n
\n
\li Handling of CRC Errors \n
<TABLE style="text-align: center">
<TR style="background-color:rgb(255,255,206)" style="color:rgb(193,0,0)">
<TD> <b> SPECIAL NOTICE on how the ROM channel handles CRC errors \n
CRC checking is per ROM module. If this check failes for one module, the ROM channel will notify an application about the problem. After that, if the application returns control, the ROM channel does NOT STOP WORKING, instead it will continue checking with the next ROM module, if any. Otherwise, the ROM channel restarts checking with the first ROM module again. </b> </TD> </TR> </TABLE>
\n
\li Handling externally connected ROM Modules \n
In case of externally connected ROM, it can't be accessed directly and, before accessed, it has to be checked against it's ready state. The data structure and the ROM channel code support this. \n
\n
\li Controls that describe the ROM configuration \n
The ROM configuration will be built at compile-time (make file and its included parts). In the table below, the available set of controls is summarized. Changes of any of the controls below require to re-compile the library itself. \n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>Control </TD> <TD>Meaning </TD> </TR>
<TR> <TD> ROM_LARGE         </TD> <TD> A number of different ROM modules will be supported. This kind of
configuration make use of the far addressing, if necessary. Since each ROM module has its own CRC value, which
might be located anywhere, the ROM data structure holds one additional CRC value reference.   </TD> </TR>
<TR> <TD> ROM_SMALL         </TD> <TD> Only one ROM module will be supported. This kind of configuration basically make use of the near addressing. The CRC value reference is absent, so the CRC value will be placed on a fixed location.  </TD> </TR>
<TR> <TD> ROM_DIRECT_ACCESS </TD> <TD> The library assumes the ROM data to be accessed directly \n
without a ready check before. So, externally connected ROM modules are not supported when this \n
configuration has been choosen </TD> </TR>
</TABLE>
\n
\li ROM Data structure overview \n
Below the basic ROM configurations are shown. Due to the lack of an automated analysis, the ROM configuration need to checked and corrected again AFTER EACH BUILD! This is due to the fact, the linker is free to place segements in order to get the code packed on a maximum level. \n
\n
\image html funsafe_ROMConfiguration.jpg
\n
\n
\li <b> Step 1: Analyse the ROM structure and what modules need to be checked or excluded. </b>\n
\n
Lmitations and other things to consider: \n
\n
<b> The generator tool already features 24-Bit addresses (8-Bit bank number,16-Bit bank address) </b> \n
<b>Consequences:</b> In non-banking ROM models (SMALL or TINY), 16-Bit bank adddresses will be used and \n
the bank number is left 0 (zero). This is only proposed solution. \n
\n
<b> The generator tool does NOT allow to cross the bank border. </b> \n
<b>Consequences:</b> as there is no mechanism to solve this problem automatically, crossing segments \n
have to be partitioned manually into smaller segments. So, for ROM modules, in the segment \n
table additonal segments of smaller size have to be created and filled in. For each segment, two pointers \n
have to be provided, the start pointer directs the first byte of the segment, while the end pointer \n
directs the LAST BYTE INSIDE THE SEGMENT. The start pointer must always be equal or less than the end \n
pointer, unpredictable results will occure otherwise. \n
\n
<b> The generator tool does NOT recognise a common address range when banking has been used. </b> \n
<b>Consequences:</b> basically, two approaches would exit to solve that problem. First, there is a \n
ROM access procedure in order to return the data read from any bank (preferred solution). Second, \n
the ROM segments to be checked will be placed basically into the common addressing range. \n
\n
<b> The generator tool does NOT consider parts to be excluded. </b> \n
<B>Consequences:</b> this is important for parts that need to be patched again after build, i.e. the \n
address range(s) where CRC values will be written (confer the ROM configuratiuon example below). \n
\n
\li <b> Step 2: Provide a CRC-16 value, one for each ROM module (in fSafeXS.c). </b>\n
Only one reference is used to access all CRC data structured in a table. This simplifies placing the \n
position where to patch the ROM image. But, it has to be noted, as this table cannot be part of \n
the CRC-checked data itself, actually, the ROM space reserved for that has to be placed near at the \n
end of the ROM, just before the interrupt vector table starts (proposed solution). \n
\n
\li <b> Step 3: Define the number of bytes checked for one scheduling cycle (in fSafeXS.c). </b>\n
By the scheduler, the ROM channel gets scheduled regularly. When scheduled, the number of \n
bytes actually checked is limited by the member "Cycle Byte Count". Here, providing the correct \n
configuration value is crucial in order to fulfil the required check time limitation. \n
\n
\li <b> Step 4: Provide a ROM data reader and a "ROM ready" checking routine (in fSafeXS.c). </b>\n
The current concept provides the flexibility of checking ROM modules of different attributes, \n
like I2C-coupled EEPROM or comparable. For that reason, the ROM channel provides a reference \n
member, pointing to an application-depnedent "data read" function. There, its possible for an \n
application to define its own accessing mechanism.\n
Before reading, the area the ROM channel is about to access, must be ready for reading. Otherwise, \n
the check gets suspended. On the next scheduling cycle,it resumes from the actual position. Since \n
this "ready check" is application dependent, an additional reference is provied for that. \n
\n
\li <b> Step 5: Finalization (in fSafeXS.c). </b>\n
If all parts have been created correctly, provide the number of segments per ROM module and \n
the number of ROM modules. The reference of the ROM module descriptior table will be determined at \n
compile time. Provide all CRC-16[0..N-1] values structured in a table. Its address will be part of \n
an applications make file. \n
\n
 \subsubsection subsubsection4 A concrete ROM (SMALL) Configuration Example
It assumes the ROM memory layout(s) to be described within an applications make file and its included parts. In the table below the ROM segment attribute \n
describes the system area, PT the process technique area, VT the vector table area. Then, there must be space reserved for only one CRC-16 value (2-byte) \n
and this is considered to be excluded from CRC checking. The configuration would look like below, and the ROM module count will be one. \n
\n
\li <b> How the ROM configuration table looks like </b>\n
Controls set for this type of configuration are: ROM_SMALL, ROM_DIRECT_ACCESS. The resulting library does not supports indirect ROM data \n access and supports only one ROM module. This is the default configuration. \n
\n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>Segment</TD> <TD>Start Address</TD> <TD>End Address</TD> </TR>
<TR> <TD> ROM  </TD> <TD> 0x0860 </TD> <TD> 0xE3FF </TD> </TR>
<TR> <TD> PT   </TD> <TD> 0xE400 </TD> <TD> 0xFBEF </TD> </TR>
<TR> <TD> VT   </TD> <TD> 0xFFD0 </TD> <TD> 0xFFFF </TD> </TR>
</TABLE>
\n
The CRC value will be located at 0xFBFE (and 0xFBFF) and this space is excluded from checking. \n
\n
\li <b> How the CRC values will be created and the ROM image gets patched </b>\n
Note: this is a non-banked ROM example, so \n
the most-significant byte of the 24-bit addresses is left zero. It has to be noted, the content of all ROM parts need to be known at build-time \n (S-Record), even in case of an external EEPROM, otherwise CRC checking will not be possible. \n
\n
<TABLE align=center style="text-align: center" style="background-color:rgb(255,255,206)">
<TR> <TD> xcrc.exe <S-Record file> /S /C000860-00E3FF /C00E400-00FBEF /C00FFD0-00FFFF  /D00FBFE </TD> </TR> </TABLE>
\n

 \subsubsection subsubsection5 A concrete ROM (LARGE) Configuration Example
It assumes the ROM memory layout(s) to be described within an applications make file and its included parts. In the table below the ROM segment attribute \n
describes the system area (ROM code 0 and ROM code 1), PT the process technique area, VT the vector table area, BL the Bootloader and DELTA_x a set of exchange data. Then, there must be space reserved for three separate CRC-16 values (2-byte each). At the end of the ROM code 1 area, there is space reserved for up to 8 CRC values (CRC_0 until CRC_7, located from 0xF7F0 until 0xF7FF) and this is considered to be excluded from CRC checking. Then, the configuration would look like below, and the ROM module count will be three. \n
Moreover, an application may use some RAM area to be used for data exchange (Read/Write into flash). Then, the CRC value changes dynamically and is not known at compile time. In the configuration table, those parts are named as DELTA_0 until DELTA_3. Here, for all parts the resulting CRC value get computed at once and changed dynamically, so it is placed into RAM.  But, this CRC value must be guaranteed to be valid as soon the ROM channel starts checking. \n
\n
\li <b> How the ROM configuration table looks like </b>\n
Controls set for this type of configuration are: ROM_LARGE, ROM_DIRECT_ACCESS. The resulting library does not support indirect ROM data \n access, but it supports all the ROM modules as defined. As shown below, this is the default configuration, which might be extended as needed. \n
\n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>ROM Module 0 Segments</TD> <TD>Start Address</TD> <TD>End Address</TD> </TR>
<TR> <TD> ROM Code 0 </TD> <TD> 0x4000   </TD> <TD> 0x7FFF   </TD> </TR>
<TR> <TD> ROM Code 1 </TD> <TD> 0xC000   </TD> <TD> 0xF7EF (16 byte reserved)   </TD> </TR>
<TR> <TD> BL         </TD> <TD> 0xFC00   </TD> <TD> 0xFDFF   </TD> </TR>
<TR> <TD> VT         </TD> <TD> 0xFF80   </TD> <TD> 0xFFFF   </TD> </TR>
</TABLE>
\n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>ROM Module 1 Segments</TD> <TD>Start Address</TD> <TD>End Address</TD> </TR>
<TR> <TD> DELTA_0    </TD> <TD> 0x3A8000 </TD> <TD> 0x3A83FF </TD> </TR>
<TR> <TD> DELTA_1    </TD> <TD> 0x3A8400 </TD> <TD> 0x3A87FF </TD> </TR>
<TR> <TD> DELTA_2    </TD> <TD> 0x3A8800 </TD> <TD> 0x3A8BFF </TD> </TR>
<TR> <TD> DELTA_3    </TD> <TD> 0x3A8C00 </TD> <TD> 0x3A8FFF </TD> </TR>
</TABLE>
\n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>ROM Module 2 Segments</TD> <TD>Start Address</TD> <TD>End Address</TD> </TR>
<TR> <TD> PT         </TD> <TD> 0x3AB000 </TD> <TD> 0x3ABFFF </TD> </TR>
</TABLE>
\n
For the process technique (PT), the CRC value will be placed within the 24-bit address range. Other CRC values will be allocated within the 16-bit address range, namely CRC_0 for all parts located in the 16-bit address range. Those get excluded from checking.
The CRC value for the DELTA buffers will be placed in RAM, thus excluded automatically.
\n
\n
\li <b> How the CRC values will be created and the ROM image gets patched </b>\n
For parts located in the 16-bit addressing range, creation is comparable to the small configuration. For parts located in the 24-bit addressing range, the resulting CRC value has to be located within the data structure as shown below. This data structure is placed at 0x3AB000 (it's PT location). The CRC generation tool does not allow to use expressions. Moreover, it does not allow synonyms to be used, which may be used in order to solve differences between configurations. Thus, the location of the Checksum must be computed manually and it will be: 0x3AB000 + SIZE_OF_VERSION_INFO (36).\n
\n
<pre>
    typedef struct
    {
        uint8_t VersionInfo[SIZE_OF_VERSION_INFO];
        uint16_t Checksum;
        uint8_t NumberOfTables;
        void  *VectorAddresses[CNT_OF_VECTORS];
    } StructVectorTab;
</pre>
For the DELTA areas, there is one variable defined where the CRC value has to be stored.\n
\n
<TABLE align=center style="text-align: left" style="background-color:rgb(255,255,206)">
<TR> <TD> xcrc.exe <S-Record file> /S /C004000-007FFF /C00C000-00F7EF /C00FC00-00FDFF /C00FF80-00FFFF  /D00F7F0 </TD> </TR>
<TR> <TD> xcrc.exe <S-Record file> /S /C3AB000-3AB023 /C3AB026-3ABFFF /D3AB024 </TD> </TR>
</TABLE>
\n

<TABLE width=900 align=center style="text-align: center"> <TR style="background-color:rgb(255,255,206)">
<TD> <b> Stack Configuration  </b> </TD> </TR> </TABLE>
 \subsubsection subsubsection6 Define the stack, its location and magic entries
<TABLE style="text-align: center">
<TR style="background-color:rgb(255,255,206)" style="color:rgb(193,0,0)">
<TD> <b> First of all, there is one limitation to be noticed: Actually, the stack channel assumes that local variables be placed on the hardware stack. Its location depends on the type of controller in use. So-called "compiled stacks" are not supported currently. If necessary, the concept has to be extended for that. </b> </TD> </TR> </TABLE>
\n
Below, an overview is given on how the stack is structured and where the magic number will be written.
\n
\image html funsafe_StackConfiguration.jpg
\n
In order to describe the data structure, basically three parameters have to be provided by an
application , that is:
\n
<pre>

extern  FSTH_CONST T_Stack   _FSF_Stack[1];
extern  FSTH_CONST uint16_t  _FSF_ui16StackMagic;
FSTH_CONST uint16_t  _FSF_ui16StackMagic   = (uint16_t)STACKMAGIC;

</pre>
Initialisation values will be taken from application specific build process (make file). Magic numbers will be written into the first and the last stack location and they get checked continousely while the system runs in its operational state. \n
\li Portability Issue \n
Depending on the architecture of the actual controllers , while operation the stack may grow in any direction. In order to keep the code portable, it has to be kept in mind, the first address of the stack area must be greater than the last address of the stack area.\n
\n
<TABLE width=900 align=center style="text-align: center"> <TR style="background-color:rgb(255,255,206)">
<TD> <b> Interrupt Configuration  </b> </TD> </TR> </TABLE>
 \subsubsection subsubsection7 Define the interrupt configuration, its types and limitations
<TABLE style="text-align: center">
<TR style="background-color:rgb(255,255,206)" style="color:rgb(193,0,0)">
<TD> <b> There are some assumptions to be noticed: \n
<ul>
<li> Interrupts are not allowed to occur in a nested manner. Otherwise, they can't be checked and measured as required. Confer chapter "Interrupts" of [2]. Existing application designs that do not follow this rule have to be changed respectively. </li> \n
<li> The interrupt supervision interval cannot exceed 16,77 seconds. </li>
</ul>
</b> </TD> </TR> </TABLE> \n
Interrupts will be classified by \n
\li Short running interrupt service \n
Event counters are typical example for this kind of service. They get incremented or decremented per occurrence. So, for short running services, it does not make sense to supervise its execution time.
\n
\li Long running interrupt service \n
There is some functionality assigned to this type of interrupt, so its execution time need to be supervised.
\n
\li Spurious interrupt service \n
Unused interrrupts have to be assigned to one or more interrupt handling routine(s). These kind of interrupts have to be supervised too, as they may block the system from operational service (i.e. constantly received NMI). This class of interrupts need no special treatment, Instead, they may be handled like the short interrupt class (simple event counter).\n
\n
<b>Things to be supervised </b>\n
It has to be guaranteed, <b> the number of received interrupts </b> lies between predefined limts. That means, for <b> a given supervision (time) interval </b>, they cannot occur too often or few. For example, spurious interrupts are not allowed to occur, so the upper limit (max.) has to be configured to one. This means, an exception will be raised if it happened once or more times. It has also to be guaranteed, the <b> run length of an interrupt service routine </b> does not exceed a prefined limit. In order to gather the necessary data,
the functional safety generic provides three interfaces to be placed within the application-specific interrupt service routines, that is \n
\n
<b> Notify entering a long running interrupt service routine </b>
<pre>
void FSF_vIsrDoEntry(uint8_t ucIsrId)
</pre>
<b> Notify exit of an long running interrupt service routine </b>
<pre>
void FSF_vIsrDoExit(uint8_t ucIsrId)
</pre>
<b> Notify entering a short running interrupt service routine </b>
<pre>
void FSF_vIsrDoShortEntry(uint8_t ucIsrShortId)
</pre>
\n
An overview of the interrupt data structure is given below. Basically, the number of interrupts to be checked is not limited by the functional safety generic.
\n
\n
\image html funsafe_ISRConfiguration.jpg
\n
\n
\n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>Configuration Element </TD> <TD>Meaning </TD> </TR>
<TR> <TD> ISR Run length </TD> <TD> Measured in microseconds </TD> </TR>
<TR> <TD> At least </TD> <TD> Minimum number of interrupt expected within the supervision interval </TD> </TR>
<TR> <TD> No more </TD> <TD> Maximum number of interrupt allowed within the supervision interval\n <b>Reaching this limit forces an exception!</b> </TD> </TR>
<TR> <TD> Supervision interval </TD> <TD> Measured in pieces of 256 microseconds </TD> </TR>
<TR> <TD> Control </TD> <TD> Used to configure what has to be checked: maximum, minimum and/or latency </TD> </TR>
</TABLE>
\n
Note: for the short running interrupt service, the "ISR Run length" row will be absent. The first four places (in the graphics Interrupt 0 until Interrupt 3) can't be configured freely. They are reserved for communication interrupts. The rest is free to be occupied by applications.
\n
\n
\li Controls that describe the interrupt configuration \n
The interrupt configuration will be built at compile-time (make file and its included parts). In the table below, the available set of controls is summarized. Changes of any of the controls below require to re-compile the library itself. \n
<TABLE align=center style="text-align: center">
<TR style="background-color:rgb(255,255,206)">
<TD>Control </TD> <TD>Meaning </TD> </TR>
<TR> <TD> FS_USE_SHORT_ISR_CONTROL </TD> <TD> Provide support for short running interrupt service </TD> </TR>
<TR> <TD> FSF_ISRCONFIG </TD> <TD> Provide support for interrupt configuration </TD> </TR>
</TABLE>
\n
<TABLE width=900 align=center style="text-align: center"> <TR style="background-color:rgb(255,255,206)">
<TD> <b> Task Scheduling Configuration  </b> </TD> </TR> </TABLE>
 \subsection subsection7 Step-by-Step Procedure on how to configure the dynamic aspects
This part concerns channels that are scheduled on a regular base, which is not necessarily the case (confer the RAM channel). As outlined in [1], a separate component schedules registered components according the
configured execution priority. \n
There is one overall limitation that has been taken into consideration: while operational work, the run length  of the generic and its assigned channel do not exceed the 1 milliseconds limitation. Bigger tasks have been split into pieces of smaller tasks. While boot time, this limitation does not hold.
\n
\subsubsection subsubsection8 How the pseudo scheduler has to be configured
Unfortunately, some applications do not make use of the general scheduling feature (the so-called <b> pseudo scheduler </b>). For those, the functional safety generic provides this feature, with the limitations that
\n
\li <b> Registered components cannot change their priority dynamically at runtime </b> \n
\li <b> Dynamic task termination doesn't work </b> \n
\li <b> Dependecies between components will not be taken into consideration </b> \n

There is one exception from the last statement above: the pseudo scheduler executes components call by call,
starting with the first component in the table until the last onne. If one of the components return control with the return value TASK_NOT_INITIALISED,
the pseudo scheduler immediately stops execution and, on next activation, it restarts execution again with the first component in the given table. So,
in case of a problem an application is able to change the actual path of execution at runtime, simply by a change of the actual table of components.
In order to achieve this behaviour, the pseudo scheduler still provides the necessary interface and it follows the same signature and semantics as
the real scheduler does. That is \n
\n
<pre>
    <b> Change the current path of execution to an alternative one </b>
        void SCH_vSetMainStateTable(const TschMainTaskTable* ptMainTbl)

    <b> Do one scheduling cycle for all registered components </b>
        bool SCH_bHandleTask(void)
</pre>
\n
Below, an overview is given about the data structure(s) the <b> pseudo scheduler </b> is working on.
\n
\image html funsafe_scheduler.jpg
\n
\li <b> Step 1: Make components known to the pseudo scheduler (in fSafeXS.h and fSafeXS.c). </b>\n
Usually, for each system state there will be one table of components to be executed. Before entering the main
processing loop, calling SCH_vSetMainStateTable has to be called in order to pre-define the currently executed system state.
\li <b> Step 2: In the main processing loop, place one call to SCH_bHandleTask. </b>\n
The functional safety generic does not know when to activate the pseudo scheduler. This decision is completely up to an application. But, make sure the pseudo scheduler gets called only once per main processing loop cycle.
\n
 \subsubsection subsubsection9 How the real scheduler has to be configured
\li <b> Step 1: Make the module(s) known to the scheduler (in schedDef.h). </b>\n
As functionality is allocated to channels, one task has to be reserved for one channel of dedicated functionality. If functionality is the same, but for different objects, this is the case for measurement of interrupt latency and run-length, there only one channel may exist providing configurable attributes.
\n
<pre>
enum {
    ...
    MOD_<last existing one>,
    MOD_WMCH,           // the Washing Monitoring channel
    MOD_SRCH,           // the Switching Relay channel
    ...
    NUMBER_OF_MODULES    //last entry, do not change
    },
</pre>
\n
Make the scheduling table known were the channels task handlers are linked in. In case of a washing machine, the both channels mentioned above will be part of the main washing table.
\n
\n
Think about the task dependencies. For functional safety and the channels managed, at least the timer task must be up-and-running, hence all modules will have at least this dependency. For each channel,
its dependecy table has to be exported to the task scheduler.
\n
<pre>
extern const TtaskId FSF_ptWMCH_Dep[];
extern const TtaskId FSF_ptSRCH_Dep[];
</pre>
\n
\li <b> Step 2: Create a task dependency table (in schedDef.c). </b>\n
Functional safety itself depends on the timer task. Here, its sufficient that both channels depend only on the functional safety task.
<pre>
const TtaskId FSF_ptWMCH_Dep[] = {DEP_FSF SCH_END_MARK};
const TtaskId FSF_ptSRCH_Dep[] = {DEP_FSF SCH_END_MARK};
</pre>
\n
Link it into the global task scheduler's dependency table (Note: take care of its position!).
\n
<pre>
const TtaskId* const SCH_pptDependencyListPtrTable[ NUMBER_OF_MODULES ]={
    ...
    FSF_ptWMCH_Dep,     // placed on position of MOD_WMCH
    FSF_ptSRCH_Dep,     // placed on position of MOD_SRCH
    ....
    };
</pre>
\n
\li <b> Step 3: Add task handlers to the global task entry table (in schedDef.c). </b>\n
The scheduler excecutes code that is linked in into this table (Note: take care of its position!).
\n
<pre>
const TpfTaskHandler SCH_ppfTaskHandlerTable[ NUMBER_OF_MODULES ] ={
    ...
    WMCH_bHandleTask,     // placed on position of MOD_WMCH
    SRCH_bHandleTask,     // placed on position of MOD_SRCH
    ...
    };
</pre>
\n
\li <b> Step 4: Add task priorities to the global task priority table (in schedDef.c). </b>\n
For a channel its not allowed to remain in the state idle. If so, the system immediately changes into
safe state. Hence, all channels have to be executed at least once BEFORE scheduling of the functional safety task. This is guaranteed, if channels in general get the high priority assigned, while the functional safety task get the low priotity assigned (Note: take care of its position!).
\n
<pre>
const TschMainTaskTable SCH_tMainDoWashingTable[ ]={
    ...
    MOD_WMCH, SCH_PRIOR_HI,     // placed on position of MOD_WMCH
    MOD_SRCH, SCH_PRIOR_HI,     // placed on position of MOD_SRCH
    ...
    };
</pre>
\n
 \subsection subsection8 The Functional Safety Boot process
Controller resources need to be checked at boot time. The boot procedure is outlined here.\n\n

\image html funsafe_BootScenario.jpg
\n
It starts with the register check, as the RAM check makes use of a pointer register and at least one work register. So, at the end of the register check the pre-condition for the RAM check is fulfilled. Dynamic data (allocated variables and the stack) require the RAM, so the RAM check comes next. After completion of the RAM check, the pre-condition to continue the start up is fulfilled. The stack can't be checked this early in time as the stack is defined after the boot phase.

 \subsection subsection9 Structured Description of Library Functional Safety
*/

#ifndef __FSAFE_H

#define __FSAFE_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"

#include "utility/utility.h"

#include "timerlib.h"

/*====================================================================================================
    FSCOM global definitions
====================================================================================================*/

/*===================================================================================================================
    Definitions and Declarations 
===================================================================================================================*/

/* DEFINITIONS ****************************************************************************************/

/* Channel Addressing Identifiers
*/

/*
Since FSCOM does not know details on a particular application, the channels an application
provides will be addressed by its pre-defined identifier as listed below. Applications are
NOT completely free in allocating identifiers. Points to consider:
1.) Applications have to add their new channels by allocating channel identifiers which have to start from FSF_APP_CHANNEL_0
2.) The order of the FSF_REG_TEST (must always be located at position zero) and the FSF_RAM_TEST  (must always be located at position 1) is fixed and not allowed to change
3.) Channels related to a typical controller (Register, RAM, ROM, Interrupt and Stack)
cannot be used by applications and cannot be replaced freely.
*/

enum    FSF_CHANNEL_ID {


        FSF_REG_TEST,                       //!< ID for reg. checking
        FSF_RAM_TEST,                       //!< ID for RAM checking
        FSF_FIRST_APPL_TEST,                //!< ID of first oper. test
        FSF_ROM_TEST = FSF_FIRST_APPL_TEST, //!< ID for ROM checking
        FSF_STACK_TEST,                  //!< ID for stack checking
        FSF_ISR_CHECK,                   //!< ID for functional safety interrupt service routine task handler
        FSF_ISR_LATENCY_CHECK,           //!< ID for interrupt service routine latency monitoring
        FSF_ISR_MIN_FREQUENCY_CHECK,     //!< ID for interrupt service routine minimum frequency check
        FSF_ISR_MAX_FREQUENCY_CHECK,     //!< ID for interrupt service routine maximum frequency check
        FSF_RAM_CONSISTENCY_CHECK,       //!< ID for RAM variables consistency check
        FSF_APP_CHANNEL_0,               //!< ID for application channel 0
        FSF_APP_CHANNEL_1,               //!< ID for application channel 1
        FSF_APP_CHANNEL_2,               //!< ID for application channel 2
        FSF_APP_CHANNEL_3,               //!< ID for application channel 3
        FSF_APP_CHANNEL_4,               //!< ID for application channel 4
        FSF_APP_CHANNEL_5,               //!< ID for application channel 5
        FSF_APP_CHANNEL_6,               //!< ID for application channel 6
        FSF_APP_CHANNEL_7,               //!< ID for application channel 7
        FSF_APP_CHANNEL_8,               //!< ID for application channel 8
        FSF_APP_CHANNEL_9,               //!< ID for application channel 9
        FSF_APP_CHANNEL_10,              //!< ID for application channel 10
        FSF_APP_CHANNEL_11,              //!< ID for application channel 11
        FSF_APP_CHANNEL_12,              //!< ID for application channel 12
        FSF_APP_CHANNEL_13,              //!< ID for application channel 13
        FSF_APP_CHANNEL_14,              //!< ID for application channel 14
        FSF_APP_CHANNEL_15,              //!< ID for application channel 15
        FSF_LASTCHANNEL,                 //!< ID for the last available channel
        FSF_NOCHANNEL,                   //!< ID to be used for not available channel
        FSF_NOT_SPECIFIED_CHANNEL = FSF_NOCHANNEL, //FSF_NOCHANNEL should not be used...
        FSF_EXT_WDOG,                    //!< ID used for external watchdog error (not a specific channel)
        FSF_LASTID                       //!< Number of channel ID's
};


/** The first channel available for an application will be starting from FSF_APP_CHANNEL_0.
*/

/*=====================================================================================
    Error codes
=====================================================================================*/
enum    FSF_ERROR_CODES {                   //!< FSCOM generated error codes
        FSF_ERROR_OK,                       //!< Service completed successfully
        FSF_ERR_BOOT_FAILURE,               //!< Cycle failure while boot
        FSF_ERR_FATAL_EXCEPTION,            //!< Cycle fatal exception
        FSF_ERR_PRECONDITION_FALSE,         //!< Precondition returned false for ongoing cycle
        FSF_ERR_CYCLE_OVERDUE,              //!< Check cycle overdue
        FSF_ERR_WD_TIMING_VIOLATION,        //!< The watchdog timing has been violated
        FSF_ERR_INVALID_RANGE,              //!< Channel addressing violation
        FSF_ERR_INVALID_STATE,              //!< FSCOM invalid state
        FSF_ERR_CHANNEL_IDLE,               //!< Channel remains idle
        FSF_ERR_CHANNEL_ERROR,              //!< Channel has notified an error
        FSF_ERR_WD_STATE_ERROR,             //!< Incorrect state read from external watchdog
        FSF_ERR_LATENCY_ERROR,              //!< Interrupt service routine run time overflow
        FSF_ERR_MAX_FREQ_ERROR,             //!< Interrupts occurred too often
        FSF_ERR_MIN_FREQ_ERROR,             //!< Interrupts occurred too little
        FSF_ERR_RAM_CON_IN_ROM,             //!< RAM variables consistency error in ROM module
        FSF_ERR_RAM_CON_IN_FSCOM            //!< RAM variables consistency error in FSCOM module
        };

/*=====================================================================================
    External Watchdog Definitions
=====================================================================================*/
/** External Watchdog Time Slot Definitions.

When the external watchdog is defined to be serviced (for a minimum period of time\n
corresponding to T3) in a time slot called T2 and not allowed to be serviced in a\n
time slot called T1, a violation will occur in the following cases:\n
- The external watchdog is serviced within T1\n
- The external watchdog is NOT serviced within T2\n
- Service of the external watchdog is too short or too long compared to T3.\n
*/

enum    WD_CYCLES {         //!< External watchdog configuration settings
    WDC_T1,                 //!< Window watchdog cycle 0
    WDC_T2,                 //!< Window watchdog cycle 1
    WDC_T3,                 //!< Window watchdog cycle 2
    NR_OF_CYCLES            //!< Number of Window watchdog cycles
};

/*=====================================================================================
    Dynamic Data Area
=====================================================================================*/
/** FSCOM dynamic data area.

FSCOM currently manages three flags per channel:\n\n

- the ongoing flag\n
set when a channel notifies ongoing check to FSCOM\n
- the success flag\n
set when a channel notifies successfully channel check completion to FSCOM\n
- the success in past flag\n
set when a channel has notified success at least once in the past\n

For the set of channels an application defines, one flag field is reserved per state in order to keeps track\n
of the channel state. These flag fields are grouped by bytes, so up to eight channels are possible per byte.\n
The required size of the bit field measured in bytes would be:\n

flag_field_size = (FSF_LASTCHANNEL  + 7) diveded by 8.

As mentioned before, three flags are required per channel. Then, the size of the channel state description\n requires would be:\n

channel_state_size = FSF_NR_OF_FLAGS multiplied by the flag_field_size.

Functionality to access bit fields is provided by a standard library that is already in use, stable and\n
tested within different projects (UTI bitfield routines). For example, setting the bit when the ROM channel\n
notifies ongoing would look like:\n\n
UTI_vSetBit( (dereferenced flag field)[FSF_ONGOING_FIELD],ROM_CHANNEL_ID)\n
\n
*/
enum    FSF_BIT_FIELDS {
    FSF_ONGOING_FIELD,          //!< set when a channel notifies ongoing check to FSCOM
    FSF_SUCCESS_FIELD,          //!< set when a channel notifies successful check to FSCOM
    FSF_SUCC_IN_PAST_FIELD,     //!< set when a channel notifies at least one success in the past to FSCOM
    FSF_NR_OF_FLAGS             //!< number of flags currenty in use
    };

#define FSCOM_FLAG_FIELD        (((uint8_t)FSF_LASTCHANNEL + 7u) / 8u)

/*=====================================================================================
    Interrupt Control Definitions
=====================================================================================*/
/** Mask definitions used to switch what has to be used at run time
*/
#define    FS_ISR_CONTROL_NONE      0x00u    //!< Use this control if you don't want to check ISRs
#define    FS_ISR_LATENCY_CHECK     0x01u    //!< Use this control if you want to check ISR Latency
#define    FS_ISR_MAXIMUM_CHECK     0x02u    //!< Use this control if you want to check ISR Maximum
#define    FS_ISR_MINIMUM_CHECK     0x04u    //!< Use this control if you want to check ISR Minimum
#define    FS_ISR_CONTROL_ALL       (FS_ISR_LATENCY_CHECK|FS_ISR_MAXIMUM_CHECK|FS_ISR_MINIMUM_CHECK)    //!< Use this control if you want to check all
#define    FS_ISR_CONTROL_SHORT     (FS_ISR_MAXIMUM_CHECK|FS_ISR_MINIMUM_CHECK)    //!< Use this control if you want to check all whats relevant for short ISR


/*====================================================================================================
    TYPE DEFINITIONS
====================================================================================================*/

/*=====================================================================================
    Channel Interface Types
=====================================================================================*/
/* Interfaces provided by channels to FSCOM
*/

/** Observe a channels precondition.
    In order to determine if all conditions are fulfilled for the actual check to be performed,
    FSCOM will use the precondition service each time a particular check is ongoing. As pre-
    conditions may be composed internally by a large number of single conditions, these are
    assigned as responsibility of the channel. A precondition simply returns a Boolean value.
    If the precondition is false, then FSCOM assumes there is an error on this channel and stops
    servicing the external watchdog. Not all channels have preconditions. In this case, FSCOM
    assumes all preconditions to be fulfilled at start of the check - because the channel returns
    true for the preconditions. A detailed example is provided in chapter 8.7 of [1].

    \return     Bool    true, if the precondition allows continuation\n

    \pre        FSCOM runs in its operational state\n
    \post       If false is returned by a channel, FSCOM internally calls FSF_vReportError with
                error code FSF_ERR_PRECONDITION_FALSE and a transition happens into the safe state.
                As this takes place within the processing of the ongoing report for a certain
                channel, parameters required in order to report the error are present at FSCOM.

    \note       Overloading is not designed here, as this is up to the real channel implementation
                and to be overloaded there. Then, from FSCOM point of view the mechanism will always
                be the same.
*/
typedef bool   (*T_FSF_Precondition)(void);

/** Perform a one-time check.
    If a one-time check is required on a particular channel (e.g. during booting phase), a one-time
    check service has to be provided on that channel.

    \pre        Called only at boot time.\n
    \post       If false is returned by a channel, FSCOM internally calls FSF_vReportError with
                error code FSF_ERR_PRECONDITION_FALSE and a transition happens into the safe state.
                As this takes place within the processing of the ongoing report for a certain
                channel, parameters required in order to report the error are present at FSCOM.

    \note       Overloading is not designed here, as this is up to a real channels implementation
                and to be overloaded there. Then, from FSCOM point of view the mechanism will always
                be the same.
*/
typedef void   (*T_FSF_OneTimeCheck)(void);


/*=====================================================================================
    Channel Configuration
=====================================================================================*/
/** <b>Channel configuration data</b>

Note that this data structure does not reflect completely the model outlined in chapter 7.1 of [1] as the
regular check must be present basically, but FSCOM does not schedule regular checks on particular channels.

<b>How FSCOM manages timing constraints</b>

First of all, an overview is provided that shows the timing relation.

\image html funsafe_TimingConstraints.jpg

\li <b>Parts involved in time management</b>

FSCOM does not reserve its own timer, instead the system clock is used. When FSCOM is scheduled, member FSF_i16LastWDTrigger holds the current value (read from TIM_uiGetCircleMicroSeconds). Hence,
FSF_ui16LastWDTrigger allows to recognize timing violations at microseconds accuracy, of course which is
not needed. Instead, the timing of the external watchdog is measured at 1 milliseconds accuracy. The
timing of the channel cycle time is measured at the accuracy that is possible using the interface
mentioned above.

\li <b>How FSCOM behaves in case of a timing violation</b>

The FSCOM timing is managed on both, at service level and at task scheduling level.\n
At service level means, that timing constraints are checked while processing either the Report Ongoing
service or the Report Success service. At task scheduing level means, that timing constraints are checked when the FSCOM task handler has been scheduled. On violation, FSCOM will immediately transition into the safe state with error code FSF_ERR_CYCLE_OVERDUE or FSF_ERR_WD_TIMING_VIOLATION, depending on the type of the timing violation.

\li <b>Watchdog Timing</b>

The watchdog timing is measured at a one milliseconds accuracy (hard data members and FSF_ui16WDC1_3 and FSF_ui16WDC1_2_3). By the overloading mechanism, an application has to provide these values to FSCOM. The watchdog timing constraints are checked at task scheduling level.

\li <b>Channel Cycle Timing</b>

Channel cycle timing is managed per channel. In order to save space, accuracy is at 100 milliseconds.
This means, a check cycle time that is below this base time has to be configured by 1. When scheduled,
FSCOM computes the time elapsed from last scheduling cycle. If this span is greater than 100 millisecond,
a timing violation is recognized and a transition into the safe state occurres. In order to support a
bigger span (up to 8 hours, for example) a multiplier is added that extends the base time by this value.
For example, a multiplier of 4 would extend the channel cycle base time up to 400 milliseconds. Since a multiplier value of 1 is sufficient for up to approximately 1 hour, a multipler value of 4 would then be sufficient for up to 4 hours (refer to chapter 8.2 of [1]).

<b>How FSCOM manages One-time checks</b>

\li <b>One-time Check Execution Order</b>

Parts of a typical controller need to be checked BEFORE use. Thus, a special order of execution of the
one-time checks has to be considered. FSCOM does not know what one-time checks are to be performed for
a particular application. The FSCOM configuration data structure has a table of pointers to channel data structures. The one-time check execution order is determined by the channel addressing scheme and how channels are ordered within the channel table.

\li <b>Channel Responsibility</b>

If a channel provide a one-time check, the member FSF_pvOneTimeCheck must be valid according to a particular project needs. For channels that do not provide one-time cheks, a generic one-time check handler has to be linked in. At boot time, FSCOM will walk over all channels and calls each one-time check as a normal C-function.


\li <b>One-time check calling Convention</b>

One-time checks have to follow the calling convention for a particular type of controller. At the end of the one-time check, control is returned to FSCOM simply like a C-function.


*/
/*lint -e{9045} 
  complete definition of '(anonymous)' is unnecessary in this translation unit [MISRA 2012 Directive 4.8, advisory] */
typedef struct    {
       uint16_t              ui16CheckCycleTime;   //!< Configured in pieces of 100 Milliseconds
       uint8_t               ucCycleMultiplier;    //!< Extends the base cycle by CheckCycleTime*(CycleMultiplier+1)
} T_FSCOM_CheckCycle;

typedef struct     {
        T_FSCOM_CheckCycle  sCheckCycle;
        bool                bIsRegularCheck;     //!< true, if this channel becomes scheduled regularely
        T_FSF_Precondition  FSF_pbPrecondition;  //!< Function pointer to a channels precondition
        T_FSF_OneTimeCheck  FSF_pvOneTimeCheck;  //!< Function pointer to a channels one-time check
} T_FSCOM_Channel;

/*====================================================================================================
    DECLARATIONS
====================================================================================================*/

/*=====================================================================================
    Exported boot services
=====================================================================================*/

//@{\name FSCOM provided interfaces (Boot-time related)

/** Start of the FSCOM boot phase.

    This 1st activation of FSCOM is reserved in order to perform all one-time checks,
    which are destructive. At the end, FSCOM performs initialisation of its internal
    data structures. This service is NOT allowed to use during regular scheduling by
    the scheduler component, as RAM is a resource, which is to be checked for the
    whole system. Using this service as soon as the system has reached its operational
    state would be too late. This service may NOT be called, as this would require the
    stack that can't be assumed to be functional before completion of the RAM check.

    \pre    The stack pointer MUST be set on a valid range\n
    \post   All one-time checks have been performed successfully, that is the one-time
            Register check and the one-time RAM check.\n
    \note   This is added mainly for completeness. FSF_vBoot is only allowed to be called by
            immediate transfer of control, that is by GOTO or JUMP instruction as the stack
            can't be assumed to be functional before check. On completion, the start process
            has to be continued by usual function call. The return address gets lost. This
            doesn't matter as there is no return allowed back into the boot process.\n
*/
extern  void    FSF_vBoot(void);

/** End of the FSCOM boot phase.
    At the end of the FSCOM boot phase, control is transferred directly to this routine.
    Within this routine, start up has to continue depending on a project needs.

    \pre    FSCOM boot phase completed successfully\n
    \post   All one-time checks have been performed successfully.
    \note   By this routine, completion of the FSCOM process is clearly defined. For the
            application-specific start up continuation, please refer to the interfaces
            required by FSCOM.
*/
extern  void    FSF_vContinueStartUp(void);

/** Report a boot error.
    This service is provided in order to move the system into the safe state.

    \pre    None\n
    \post   No return from this call.\n
*/
extern  void    FSF_vBootError(void);

//@}

/*=====================================================================================
    Exported operational services
=====================================================================================*/

//@{\name FSCOM provided interfaces (Channel related)

/** Report ongoing check to FSCOM.
    When a channel starts checking this event is immediately reported to FSCOM. Each
    time the channel is processing it will report ongoing or success to FSCOM. If a
    channel does not report ongoing (or success), it is assumed not to work correctly.
    FSCOM keeps a flag for each channel. For ease of implementation it is recommended
    to set the ongoing flag also when reporting success (done directly by the report
    success service).
    \param  ucCheckId  identifier of the the addressed channel\n

    \pre    FSCOM initialised correctly. Channel identifier within the correct range.\n
    \post   For valid channels, internal state transitions, if any, have been completed.
            Invalid channel addressing immediately results into the safe state as this
            is not allowed to occur.\n
*/
extern  void    FSF_vReportOnGoing(uint8_t ucCheckId);

/** Report successfully completed check to FSCOM.
    If a channel has completed checking (partially or finally) successfully, this
    event is reported to FSCOM.
    \param  ucCheckId  identifier of the the addressed channel\n

    \pre    FSCOM initialised correctly. Channel identifier within the correct range.
    \post   For valid channels, internal state transitions, if any, have been completed.\n
            Invalid channel addressing immediately results into the safe state as this
            is not allowed to occur.\n
*/
extern  void    FSF_vReportSuccess(uint8_t ucCheckId);

/** Report a channel error to FSCOM.
    If a channel has stopped monitoring due to an error, this event is immediately
    reported to FSCOM.
    \param      ucCheckId      identifier of the the addressed channel
    \param      ucSubCheckId   FSF_ERROR_CODE that describes the problem - please note that this parameter is not used, any value input via this parameter will be replaced by an error code according to rules of this function!\n
    \param      ucUserPar      defined by user\n

    \pre        FSCOM initialised correctly. Channel identifier within the correct range.\n
    \post       Invalid channel addressing immediately results into the safe state as this
                is not allowed to occur.\n
*/
extern  void    FSF_vReportError(uint8_t ucCheckId, uint8_t ucSubCheckId, uint8_t ucUserPar);

/** Report a fatal error to FSCOM.
    This service is provided for channels that require IMMEDIATE transition into the
    safe state. For example, if the register channel has recognised stuck-at fault on
    the carry flag, then control immediately has to move to the safe state (Jump instruction).

    \pre        None\n
    \post       No return from this call.\n
*/
extern  void    FSF_vReportFatal(void);

/** Observe successful channel check in the past.
    A Boolean value is returned by this service. When at least one check cycle has
    been completed successfully true is returned, otherwise false is returned. See
    chapter 8.7 (Functional Safety Software Specification document) for an example of use.
    \param          ucCheckId  identifier of the the addressed channel\n

    \return         bool       true/false, see description.\n\n
    \pre            FSCOM initialised correctly. Channel identifier within the correct range.\n
    \post           Invalid channel addressing immediately results into the safe state as this
                    is not allowed to occur.\n
*/
extern  bool    FSF_bSuccesInPast(uint8_t ucCheckId);

//@}

/*=====================================================================================
    Exported scheduling services
=====================================================================================*/

//@{\name FSCOM provided scheduling interfaces

/** Schedule FSCOM.
   By this handler, FSCOM is scheduled on a regular base.

   \return     uint8_t\n   TASK_NOT_INITIALISED:   channel monitoring not started yet\n
                       TASK_INITIALISED:       channel monitoring cycle completed\n
                       TASK_TERMINATE:         channel exception occurred\n

    \pre        FSCOM runs in its operational state\n
    \post       All configured channels checked. The external watchdog serviced correctly.
*/
extern uint8_t FSF_ucComHandleTask(void);

//@}

/*=====================================================================================
    Exported interrupt services
=====================================================================================*/

//@{\name FSCOM provided interrupt count and latency measurement interfaces

/**
  Entry function for short interrupts
  This function has to be called at the beginning of simple short interrupts.
  See explanation FSF_vIsrDoEntry for classification of interrupt.

 \param ucShortIsrId :  Id of calling interrupt\n
   Type  : uint8_t\n
   Range : [0..FS_ISR_NUMBER_OF_LARGE_ISR]\n
   Use enumeration in fSafeXS.h\n
  \return None
*/
extern  void FSF_vIsrDoShortEntry(uint8_t ucIsrShortId);

/**
  Entry function for Interrupts
  This function has to be called at the beginning of interrupts.
  For simple interrupts (simple structure no loops) use function FSF_vIsrDoShortEntry()
  Use function FSF_vIsrDoExit() as exit function.

 \param ucIsrId :  Id of calling interrupt\n
   Type  : uint8_t\n
   Range : [0..FS_ISR_NUMBER_OF_LARGE_ISR]\n
   Use enumeration in fSafeXS.h\n
  \return None
*/
extern  void FSF_vIsrDoEntry(uint8_t ucIsrId);

/**
  Exit function for Interrupts.

  This function has to be called at the end of interrupts, whenever FSF_vIsrDoEntry() was called upon entering.
  Do not call this function when FSF_vIsrDoShortEntry() was called as entry.
  In this case no exit function is required.

 \param ucIsrId :  Id of calling interrupt\n
   Type  : uint8_t\n
   Range : [0..FS_ISR_NUMBER_OF_SMALL_ISR]\n
   Use enumeration in fSafeXS.h\n
  \return None
*/
extern  void FSF_vIsrDoExit(uint8_t ucIsrId);

//@}

/*====================================================================================================
    GLOBAL FUNCTIONS
====================================================================================================*/
/*=====================================================================================
    Imported scheduler functions
=====================================================================================*/

//@{\name FSCOM Task Function

/**
* Task handler of FSF for interrupt statistics.
* Handler has to be called by scheduler.
* Runtime and frequency of interrupts will be checked and compared with upper and lower limits.
* \return
* TASK_NOT_INITIALISED: module is not initialised
* TASK_INITIALISED:     module is initialised
*/
extern uint8_t FSF_ucIsrHandleTask(void);

/**
* Task handler of FSF for ROM check.
* Handler has to be called by scheduler.
* This task watch for CRC errors in ROM. FSF_ucRomTinyHandleTask is the tiny variant of FSF_ucRomHandleTask.
* FSF_atRomModules just have one entry for controller ROM. Other Rom modules are not supported.
* \return
* TASK_NOT_INITIALISED: module is not initialised
* TASK_INITIALISED:     module is initialised
*/
extern  uint8_t FSF_ucRomTinyHandleTask(void);

/**
* Task handler of FSF for ROM check.
* Handler has to be called by scheduler.
* This task watch for CRC errors in ROM.
* The test can be configurated by defining the array of structures FSF_atRomModules (one entry for each ROM module)
* FSF_ucRomHandleTask can manage different ROM modules e.g. output texts, controller ROM, washing table...
* \return
* TASK_NOT_INITIALISED: module is not initialised
* TASK_INITIALISED:     module is initialised
*/
extern uint8_t FSF_ucRomHandleTask(void);

/**
* Task handler of FSF for register check.
* Handler has to be called by scheduler.
* Work registers of controller are checked by this task.
* \return
* TASK_NOT_INITIALISED: module is not initialised
* TASK_INITIALISED:     module is initialised
*/
extern uint8_t FSF_ucRegHandleTask(void);

/** Task handler of FSF for stack check.

Handler has to be called by scheduler.
This task is monitoring the stack. Stack overflow will be sense.
\return
TASK_NOT_INITIALISED: module is not initialised
TASK_INITIALISED:     module is initialised
*/
extern uint8_t FSF_ucStackHandleTask(void);

/** Task handler of FSF for RAM check.

Handler has to be called by scheduler.
This task is checking the RAM against cell faults and some kind of address decoder faults.
\return
TASK_NOT_INITIALISED: module is not initialised
TASK_INITIALISED:     module is initialised
*/
extern uint8_t FSF_ucRamHandleTask(void);

//@}

/*=====================================================================================
    Exported one-time check functions (mainly RAM and Register related)
=====================================================================================*/
extern  void FSF_vRam_FatalException(void);
extern  void FSF_vRam_OperationalException(void);
extern  void FSF_vReg_FatalException(void);

/*lint -restore -e9059 */

#ifdef __cplusplus
}
/*lint -restore -e9059 */
#endif

#endif
