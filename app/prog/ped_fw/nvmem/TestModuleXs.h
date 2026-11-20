/*~-*/
/*~XSF_LANGUAGE: C/C++*/
/*~A*/
/*~+:Module Header*/
/*~T*/
/*****************************************************************************************************************
* Copyright by BSH Bosch und Siemens Hausgeraete GmbH - EDS
*
*-----------------------------------------------------------------------------------------------------------------
*
* Die Nutzung, die Vervielfaeltigung und die Weitergabe dieser Software oder ihrer Teile ist nur mit
* ausdruecklicher schriftlicher Zustimmung der BSH EDS gestattet. Die Software darf vom Verwender
* ausschliesslich fuer die Entwicklung von Elektroniken fuer BSH genutzt werden, eine Nutzung fuer
* sonstige Zwecke (z.B. Entwicklung von Elektroniken fuer Dritte) ist untersagt. Saemtliche Rechte,
* einschliesslich aller Urheber-, Patent- und Gebrauchsmusterrechte, an der Software bleiben BSH
* vorbehalten. Zuwiderhandlungen werden straf- und zivilrechtlich verfolgt.
*
* Die Software wurde nach dem Stand der Technik erstellt und geprueft. Aufgrund der Komplexitaet der
* Implementierung von Embedded Controller Software sind  vom Verwender fuer jede Implementierung eigene
* Tests durchzufuehren, um die Funktionssicherheit in seiner Entwicklung sicherzustellen. Die Software
* wurde fuer den Einsatz als Bibliothek entwickelt. Jede anderweitige Verwendung erfolgt in alleiniger
* Verantwortung des Nutzers. Der Nutzer ist berechtigt, die Software zur Anpassung an andere
* Microcontroller und andere Compiler in eigener Verantwortung zu veraendern  und anzupassen. 
* BSH leistet keinen Support fuer derartige Anpassungen, ist jedoch
* berechtigt, von Verwender vorgenommene Anpassungen fuer eigene Zwecke kostenfrei zu nutzen.
*
* Die Haftung der BSH fuer die Funktionsfaehigkeit und -sicherheit der Software, auch beim  Einsatz in
* einer konkreten Anwendung, und moegliche, durch sie verursachte Folgeschaeden ist, ausser im Falle von
* Vorsatz oder grober Fahrlaessigkeit, ausgeschlossen. In jedem Fall ist die Haftung der BSH jedoch auf
* den typischerweise auftretenden, vorhersehbaren Schaden begrenzt.
*
* Technische Aenderungen sind vorbehalten.
*
*-----------------------------------------------------------------------------------------------------------------
*
* Use, reproduction and dissemination of this software or parts of it, is not permitted without express
* written authority of BSH EDS. The user is allowed to use this software exclusively for the development
* of electronic boards for BSH. Usage for other purposes is strictly prohibited (e.g. development of
* electronic boards for third parties). All rights, including copyright, rights created by patent grant
* or registration, and rights by protection of utility patents, are reserved. Violations will be
* prosecuted by civil and criminal law.
*
* The software was created and approved by acknowledged rules of technology. Because of the complexity
* of embedded controller software the user of this software has to perform his own tests to ensure
* proper functionality in his environment. The software was developed for usage as a software library.
* The user is sole responsible for every other usage.  The user may modify the software for adaptations
* needed for other microcontrollers, as well as to another compilers Metrowerks at own risk. 
* BSH will not support any adaptations or changes. BSH is authorised to use all adaptations for 
* own purposes free of charge.
*
* BSH assumes no liability for the functionality or reliability of the software even in concrete
* applications. BSH assumes no liability for consequential damages, except in case of intention or gross
* negligence. In any case the liability is limited to the typical and predictable damage.
*
* Technical changes are reserved.
*
******************************************************************************************************************
   PROJECT          TEST_MODULE
   MODULE-PREFIX    TM_
   FILE             %PM%
   ARCHIVE          %PP%:%PI%
   PROCESSOR        independent
   COMPILER         independent
 *****************************************************************************************************************
   AUTHOR           emka
   CREATED          2006-05-05
   LAST CHANGE      %PRT%
   LAST REVISION    %PR%
   STATUS           %PS%
 *****************************************************************************************************************
   DESCRIPTION                                                                                                   */
 /** \file TestModuleXS.h
   published application specific definitions and declarations for Test Module                                 
 */
 /****************************************************************************************************************
   CHANGES
   %PL%
 *****************************************************************************************************************/

/*~E*/
/*~I*/
#ifndef __TESTMODULEXS_H
/*~T*/
#define __TESTMODULEXS_H
/*~A*/
/*~+:Includes*/
/*~T*/

/*~E*/
/*~A*/
/*~+:Definitions and Declarations */
/*~A*/
/*~+:DEFINITIONS */
/*~T*/
/* DEFINITIONS ****************************************************************************************/
/*~T*/
/** Maximal number of test points, user defined. */
#define TM_MAX_TESTPOINTS        (30)

/** Maximal entries of runtime measurements values, user defined. */
#define TM_MAX_RTIME_ENTRIES     (15)

/*~T*/
/**
 * \brief size of debug array (for use with Tcl Test Scripts)
 */
#define TM_DEBUG_ARRAY_SIZE 20
/*~E*/
/*~A*/
/*~+:TYPE DEFINITIONS*/
/*~T*/
/* TYPE DEFINITIONS ***********************************************************************************/
/*~T*/

/*~E*/
/*~A*/
/*~+:DECLARATIONS*/
/*~T*/
/* DECLARATIONS **************************************************************************************/
/*~T*/
extern uchar TM_aucDebugArray[];

extern BOOL TM_bCriticalSection;
/*~E*/
/*~A*/
/*~+:GLOBAL FUNCTIONS*/
/*~T*/
/* GLOBAL FUNCTIONS **********************************************************************************/
/*~A*/
/*~+:Test Preset*/
/*~T*/
/**
   Do neccessary presets before calling the test server.
\param none
\return
   none
\remarks
   This function is useful only for library tests and has following task :
   - Configure platform if necesary : IO, UART, ADC etc.
   - Load test symbols to be accessed from script (by calling them in a safe way)
   - Do necessary resets to set software in a correct initial state
   - enable system interrupts (mandatory for DBUS)
*/
void TM_vTestPreset (void);
/*~E*/
/*~A*/
/*~+:Trigger Functions*/
/*~I*/
#ifdef TM_TRIGGER_INCLUDED 
/*~T*/
/**
   Set a trigger pin ON or OFF.
\param ucTrgPin - Trigger pin in range 1 to 255
\param btrgOn   - TRUE=ON/FALSE=OFF
\return
   none
\remarks
   ucTrgPin = 0 clears all triggers
*/
extern void TM_vSetTrg (uchar ucTrgPin, BOOL btrgOn);
/*~-*/
#endif
/*~E*/
/*~T*/

/*~E*/
/*~T*/

/*~E*/
/*~E*/
/*~-*/
#endif
/*~E*/
/*~T*/

