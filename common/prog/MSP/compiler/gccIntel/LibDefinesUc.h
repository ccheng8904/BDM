/*****************************************************************************************************************
* Copyright by BSH Bosch und Siemens Hausgeraete GmbH - EDS                                                       
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
* needed for other microcontrollers, as well as to another compilers at own risk.                                 
* BSH will not support any adaptations or changes. BSH is authorised to use all adaptations for own purposes      
* free of charge.                                                                                                 
*                                                                                                                 
* BSH assumes no liability for the functionality or reliability of the software even in concrete                  
* applications. BSH assumes no liability for consequential damages, except in case of intention or gross          
* negligence. In any case the liability is limited to the typical and predictable damage.                         
*                                                                                                                 
* Technical changes are reserved.                                                                                 
*                                                                                                                 
******************************************************************************************************************
 *   MODULE-PREFIX    ---
 *   FILE             %PM
 *   ARCHIVE          %PP%:%PI%
 *   PROCESSOR        independant
 *   COMPILER         independant
 *****************************************************************************************************************
 *   AUTHOR           Jens Lehmann
 *   CREATED          19.03.2012
 *
 *   LAST CHANGE      %PRT%
 *   LAST REVISION    %PR%
 *   STATUS           %PS%
 *****************************************************************************************************************
   DESCRIPTION                                                                                                   */
/** \file
    This header makes some generic definitions, must be included if there is no platform specific variant available
*/
 /*****************************************************************************************************************
 *   CHANGES
 *   %PL%
 *
 *****************************************************************************************************************/


#ifndef __LIBDEFINESUC_H

#define __LIBDEFINESUC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int TbitField; ///< uses this definition for bitfield definitions instead of int!


#ifdef __cplusplus
}
#endif

#endif
