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


#ifndef PHYPPRIV_H
#define PHYPPRIV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *  \ingroup  Physpan
 *  \brief    internal definitions and declarations for unit physpan.
 *
 *  The definitions and declarations of this file are used by the library
 *  configuration:
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define PHYP_INVALID   ((uint8_t)(0xFFU))       ///< used to set a key value to invalid
#define PHYP_MAX_VALUE ((uint8_t)(0xFEU))       ///< maximal used valid value
#define PHYP_INVALID_U16   ((uint16_t)(0xFFFFU))    ///< used to set a key value to invalid
#define PHYP_MAX_VALUE_U16 ((uint16_t)(0xFFFEU))    ///< maximal used valid value



#if ( (PHYP_DIGI_KEY_COUNT > 0u)  && (PHYP_C_KEY_COUNT < 0u) )
#define PHYP_MIXED_PANEL ///< if defined a mixed panel is in use e.g some capacitive and some mechanical keys
#endif

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/* CONSTANTS TO BE DEFINED EXTERNAL */

//@{\name Constants for Digital Panel
/** \brief Digital panel: Number of equal samples to accept a new key state
 *
 *  This are the number of samples for digital panel. 
 *  Each time when PHYP_ucDigiSampleCount samples are done the key-state will
 *  be refreshed.\n
 *  If the result for a key is not equal for all samples, the old state of this
 *  key is the result.
 *
 *  \note It is recommended not to use a value <4 for this constant.
 */
extern const uint8_t PHYP_ucDigiSampleCount;

/** \brief Digital panel: Time between two samples
 *
 *  This is the distance between 2 samples in ticks for digital panel.
 *  For each sample all keys will be read.
 *  Please note that this is the minimum time. The resulting sampling rate
 *  depend on scheduling.
 *
 *  \note This is a minimum time, the time in application depends on scheduling.\n
 *  Beware to select any time which results in sampling frequency which has easy relation to 50 or 60HZ.\n
 *  The refresh time for keys can be calculated with PHYP_ucDigiSampleCount x PHYP_uo8DigiSampleTime *256µs.\n
*/
extern const uint16_t PHYP_uiDigiSampleTime;
//@}


//@{\name Constants for Capacitive Panel
#ifndef PHYP_NO_TYPE_CHECK 

/** \brief Threshold to switch Capacitive-Key ON
 *
 * This is the threshold where key is accepted as ON.
 * This threshold will be interpreted relative between upper and lower dynamic thresholds.\n
 * This constant must be defined in library template.\n
 */
extern const uint8_t PHYP_ucCapOnThreshold;


/** \brief Threshold to switch Capacitive-Key OFF
 *
 * This is the threshold where key is accepted as OFF.
 * This threshold will be interpreted relative between upper and lower dynamic thresholds.\n
 * This constant must be defined in library template.\n
 */
extern const uint8_t PHYP_ucCapOffThreshold;


/** \brief Number of capacitive keys
 *
 * Number of supported capacitive keys.\n
 * This constant must be defined in library template.
 */
extern const uint8_t PHYP_ucCapNumberOfKeys;
//@}

#endif

//Macros for calculating of filter coefficients and number of post samples (capacitive panel)

/*lint -save -e9026 "Function-like macro defined..." it is intation to let this do by preprocessor */


/** \brief Calculates the IIR filter coefficient for capacitive sampling filter
 *
 *
 * \param Tsample: sampling time in us
 * \param Tau: time to settle to 63% in ms
 */
#define PHYP_CAP_CALC_SAMPLING_FILTER_63(Tsample,Tau)   ((((uint32_t)(Tau))*1000)/((Tsample)))


/** \brief Calculates the IIR filter coefficient for capacitive minimum filter
 *  
 *  A settling to 95% needs the time 3xtau.
 *  The factor 3 is caused by under sampling of minimum.
 *  
 * \param Tsample: sampling time in us
 * \param Tau: time to settle to 95% in ms
 *  
 */
#define PHYP_CAP_CALC_MINIMUM_FILTER_95(Tsample,Tau3)    ((((uint32_t)(Tau3))*1000)/(((uint32_t)(Tsample))*3*3))


/** \brief Calculates the number of required post samples (capacitive panel)
 * 
 * \note the factor 3 is caused by under sampling
 * 
 * \param Tsample: sampling time in us\n
 * \param Tr: Key reaction time in ms\n
 */
#define PHYP_CAP_CALC_POST_SAMPLES(Tsample,Tr)    ((uint8_t)(((uint32_t)(Tr)*1000u)/((((uint32_t)(Tsample))*3u))))

/*lint -restore -e9026 */

/* VARIABLES TO BE DEFINED EXTERNAL */


/** Buffer for hole panel
    This is the panel buffer, each bit represents a key. The buffer is used for digital IR and capacitive keys.\n
*/
extern Tpanel PHYP_tKeyState;

//@{\name Variables for Capacitive Panel

/** \brief Capacitive Panel: Dynamic lower threshold (one value for each Capacitive-Key)
 * 
 *  This is the filtered lower threshold (Minimum). For each Key there is one value.
 *  The Low Byte of this value is just used by IIR Filter
 *  This array has to be defined by application.
 *
 * 
 * \note The upper byte of the uint16_t represents the filtered value. The lower byte is for truncation.\n
 *       To reconstruct the threshold after reset, save the upper byte in non-volatile memory.
 * 
 */
extern uint16_t PHYP_auiCapMinimum[];
//@}

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/* functions to be defined in library template*********************************/

/** \brief Notify application that key state has changed
 * 
 * This function is called by library if key state has changed.\n
 * This function may implement by the application.\n
 * The default function in library is empty.
 * 
 */
extern void _PHYP_vNotifyKeyStateChanged(void);


/** Configuration handler of digital panel
 *  
 *  This function is called by PHYP_ucHandleDigital() until it returns true.
 *  This Function must be defined by application. If something (e.g. range) has to be load from non volatile memory, 
 *  it has to be return FALSE until read is done.\n
 *  
 *  \return true if configuration is ready, false if not
 *
 *  \note This function must be defined by application.
 */
extern bool _PHYP_bIsDigiConfigurationReady(void);


/** Reads the current key state
 *  
 *  Must be defined, if digital panel is used!!
 *  This function reads the keys of current channel from HW. Please look in library template for more information and example.
 *  Do not call this function, it will be called from library itself!
 *  
 *  \note This function must be defined by application.
 */
extern void _PHYP_vGetDigiKeyState(void);


/** Initilise a new sampling sequence
 * 
 *  This function must be defined by application.\n
 *  It initialises a new sampling sequence and copy the result of last sampling to the
 *  panel output buffer. For further information please see example in library template.\n
 *  Do not call this function, it will be called from library itself!
 * 
 *  \note This function must be defined by application.
 */
extern void _PHYP_vInitDigiSampleSequence(void);


/** Switch to next channel of key multiplexer
 * 
 *  This selects the next channel of multiplexer. When first channel is reached again, the function must return
 *  true, to signalize library that all keys has been scaned. In all other cases the function returns false.
 *  Please look in library template for more information and example.\n
 *  If multiplexing is not used this function returns true.
 *  Do not call this function, it will be called from library itself!
 * 
 *  \return true if all columns done, false if not
 *
 *  \note This function must be defined by application.
 * 
*/
extern bool _PHYP_bDigiSetNextChannel(void);


/******************************************************************************************************/
/* CAPACITIVE PANEL                                                                                   */
/******************************************************************************************************/

#if defined(PHYSPAN32)
/** \brief Returns the range (sensitivity) for key \<keyId\>
 *
 *  May be used to implement e.g. variant handling.
 *  In a simple implementation a constant value may be returned.\n
 * 
 * \b Example: Common implementation
 *  \code
 *  uint16_t _PHYP_uiCapGetRange(uint8_t keyId)
 *   {
 *     return PHYP_auiCapRange[keyId];
 *   }
 *  \endcode
 *
 *  \param keyId: The ID of capacitive key
 *  \return range of key \<keyId\>
 *  \note This function must be defined by application.
 */
extern uint16_t _PHYP_uiCapGetRange(uint8_t keyId);


/** \brief Restarts the key status including filter \<keyId\>
 *
 *  \param keyId: The ID of capacitive key
 */
extern void _PHYP_vCapRestartKey(uint8_t keyId);
#endif


/** \brief Calculates the relative key value
 *
 *  This Function calculates from the absolute value \<value\> the relative one.\n
 *  A relative value of 50 means that \<value\> is in the middle of upper and lower threshold.\n
 *  A relative value of 100 means that \<value\> is equal or larger than upper threshold.\n
 *  For value smaller than lower threshold the result is zero.
 *  
 *  \param keyId: The ID of capacitive key
 *  \param value: Absolute value for key \<keyId\>\n
 *  
 *  \return Relative value for Key \<keyId\> with absolute value \<value\>
 *  
 *  \note This function is used to implement an application specific plausibility check.\n
 *        Plausibility may be checked before wrong values enter the filters.
 */
extern uint8_t PHYP_ucCapCalcRelativeKeyValue(uint8_t ucKeyId, uint16_t uiValue);



/******************************************************************************************************/
/* FUNCTION TO BE DEFINED BY APPLICATION                                                              */
/******************************************************************************************************/
/** \brief _PHYP_uiCapGetMinimum returns the minimum for a key \<keyId\>
 *
 *  \b Example: Common implementation
 *  \code
 *  uint16_t _PHYP_uiCapGetMinimum(uint8_t keyId)
 *  {
 *     return (uint16_t)PHYP_ulCapMinimum[keyId];
 *  }
 *  \endcode
 *
 *  \param keyId: The ID of capacitive key
 *
 *  \return minimum fo the key \<keyId\>\n
 *  \note This function must be defined by application.
 */
extern uint16_t _PHYP_uiCapGetMinimum(uint8_t keyId);



/** \brief Configuration handler of capacitive panel
 *
 *  This function is called by PHYP_ucHandleCapacitive() until it returns true.
 *  This Function must be defined by application. If something (e.g. range) has to be load from 
 *  non volatile memory, it has to be return FALSE until read is done.\n
 *  Additional this function may be used to initialise a PWM used as clock for the capacitive keys.
 *
 *  \return true if configuration finished, false if not
 *  \note This function must be defined by application.
 */
extern bool  _PHYP_bIsCapConfigurationReady(void);


/** \brief Returns whether current (unfiltered) dataset is valid or not
 *
 *  This function must be defined by application.\n
 *  It returns true if a new dataset is present.
 *  Additional the validity of a new dataset may be checked before entering in the pre-filter.
 *  If range check of data is not required just return TRUE if new dataset is present.
 *
 *  \return true if dataset is valid, false if not
 *  \note This function must be defined by application and is called by library itself.
 */
extern bool  _PHYP_bIsCapDataSetValid(void);


/** \brief Locks all keys which are not allowed to press together with \<keyId\>
 * 
 *  This Function  must be defined by application if more than one key may be pressed at the same time.\n
 *  If interlocking is not required (e.g. #PHYP_ucCapMaxSimultaneousKeys=1) this function should be implemented
 *  as empty function.\n
 *  This function works together with function _PHYP_bCapIsKeyLocked()\
 *  For further information please look example implementation in phycapxs.c
 *  
 *  \param keyId: The ID of capacitive key
 *  \note This function must be defined by application and is called by library itself.
 */
extern void _PHYP_vCapHandleInterlocking(uint8_t keyId);


/** \brief returns lock state of  key \<keyId\>
 *
 *  If the key \<keyId\> is not locked \b false shall be returned.
 *  If \b true is returned the key shall not be set.\n
 *  This Function  must be defined by application if more than one key may be pressed at the same time.\n
 *  If interlocking is not required (e.g. #PHYP_ucCapMaxSimultaneousKeys=1) this function should return \b false.\n
 *  This function works together with function _PHYP_vCapHandleInterlocking().\n
 *  For further information please look example implementation in phycapxs.c
 *
 *  \param keyId: The ID of capacitive key
 *  \return true if key is locked, false if nat
 *  \note   This function must be defined by application and is called by library itself.
 */
extern bool  _PHYP_bCapIsKeyLocked(uint8_t keyId);


/** \brief Returns the range (sensitivity) for key \<keyId\>
 *  
 *  This function returns the range (sensitivity) for key \<keyId\>.\n
 *  It may be used to implement e.g. variant handling.
 *  In a simple implementation a constant value may be returned.\n
 *
 *  \b Example: Common implementation
 *  \code
 *  uint8_t _PHYP_ucCapGetRange(uint8_t keyId)
 *   {
 *     return PHYP_aucCapRange[keyId];
 *   }
 *  \endcode
 *  
 *  
 *  \param keyId: The ID of capacitive key\n
 *  \return range of key \<keyId\>\n
 *  \note This function must be defined by application.
 */
extern uint8_t _PHYP_ucCapGetRange(uint8_t keyId);


/** \brief Release the bit for the key \<keyId\>
 *  
 *  \b Example:
 *  \code
 *  void _PHYP_vCapReleaseKey(uint8_t keyId)
 *  { 
 *    PHYP_tKeyState &= ~((Tpanel)(1u << keyId));
 *  }
 *  \endcode
 *  
 *  \sa _PHYP_vCapSetKey().
 *  
 *  \param keyId: The ID of capacitive key\n
 *  \note This function must be defined by application and is called by library itself.
 */
extern void _PHYP_vCapReleaseKey(uint8_t keyId);


/** \brief Sets the bit for the key \<keyId\>
 *  
 *  \b Example:
 *  \code
 *  bool _PHYP_vCapSetKey(uint8_t keyId)
 *  { 
 *      PHYP_tKeyState |= ((Tpanel)(1u << keyId));
 *  }
 *  \endcode
 *  
 *  \sa _PHYP_vCapReleaseKey().
 *  
 *  \param keyId: The ID of capacitive key\n
 *  \note This function must be defined by application and is called by library itself.
 */
extern void _PHYP_vCapSetKey(uint8_t keyId);


/** \brief This is the post handler for key scanning.
 *  
 *  It is responsible for final key debouncing.
 *  For further information please look example implementation in phycapxs.c
 *  
 *  
 *  \note This function must be defined by application and is called by library itself.
 *  
 */
extern void  _PHYP_vCapDoPostKeyScan(void);


/** \brief Filters the incoming ADC-Values for key \<keyId\>
 *  
 *  \b Example:
 *  \code
 *  #define PHYP_CAP_SAMPLING_COEFF 8
 *  
 *  uint16_t _PHYP_uiCapFilterData(uint8_t keyId)
 *  { 
 *    if (PHYP_auiADbuffer[keyId] != (uint16_t)UI_INVALID)
 *     {
 *           PHYP_auiCapFilteredValues[keyId]-= PHYP_auiCapFilteredValues[keyId]/PHYP_CAP_SAMPLING_COEFF;
 *           PHYP_auiCapFilteredValues[keyId]+= PHYP_auiADbuffer[keyId]/PHYP_CAP_SAMPLING_COEFF;
 *     }
 *     return PHYP_auiCapFilteredValues[keyId];
 *  }
 *  \endcode
 *  
 *  
 *  \param keyId: The ID of capacitive key\n
 *  \return Filtered data value\n
 *    \b Range: Bit15: MSB of ADC (8 Bit ADC: Bit8 = LSB, 10 Bit ADC: Bit6 = LSB)
 +
 *  \note This function must be defined by application and is called by library itself.
 */
extern uint16_t _PHYP_uiCapFilterData(uint8_t keyId);

/*lint -save -e9059 "C comment contains C++ comment"
 * Codeblock with comments for doxygen needed. Tested with various parsers to work correctly
 */
 
/** \brief Tracks the minimum for key \<keyId\>
 *  
 *  This function is responsible for minimum tracking.\n
 *  As rising and falling time of this filter should be different 2 different filters must be implemented:
 *  
 *  \b Example:
 *  \code
 *  #define PHYP_CAP_MIN_COEFF_FALL   (uint16_t)16u
 *  #define PHYP_CAP_MIN_COEFF_RISE   (uint16_t)32u
 *  #define PHYP_MIN_LOCK_THRESHOLD   (uint8_t)40u
 *  
 *  void _PHYP_vCapFilterMinimum(uint8_t keyId, uint8_t percent)
 *  { 
 *    const uint16_t minimum = PHYP_auiCapMinimum[keyId];
 *    const uint16_t value   = PHYP_auiCapFilteredValues[keyId];
 *  
 *    if (percent <= PHYP_MIN_LOCK_THRESHOLD)
 *    {
 *      if (minimum <= value)
 *      {
 *           minimum-= (uint16_t)(minimum/PHYP_CAP_MIN_COEFF_RISE);
 *           minimum+= (uint16_t)(value/PHYP_CAP_MIN_COEFF_RISE);
 *      }
 *      else
 *      {
 *           minimum-= (uint16_t)(minimum/PHYP_CAP_MIN_COEFF_FALL);
 *           minimum+= (uint16_t)(value/PHYP_CAP_MIN_COEFF_FALL);
 *      }
 *     PHYP_auiCapMinimum[keyId] = minimum; //save new minimum
 *    }
 *  }
 *  \endcode
 *  
 *  \param keyId: The ID of capacitive key\n
 *
 *  \note This function must be defined by application and is called by library itself.
 */
extern void _PHYP_vCapFilterMinimum(uint8_t keyId,uint8_t percent);


/** \brief Checks the plausibility of filtered data
 *  
 *  This function should contain a peak filter for each key.\n
 *  The filtered relative key values should be cumulated.
 *  In function _PHYP_vInitNewDataset() the cumulated value should be check against a limit.
 *  If the cumulated value is above a limit current key values a not plausible.
 *  
 *  \b Example:
 *  \code
 *  
 *  #define PHYP_CAP_PLAUSI_COEFF   (uint16_t)8u
 *  
 *  void _PHYP_vCapCheckPlausibility(uint8_t keyId, uint8_t percent)
 *  {
 *     uint16_t relativePeakValue;
 *     uint16_t uiPercent;
 *  
 *  
 *     PHYP_ucCapCalcRelativeKeyValue[keyId] = percent;
 *  
 *     if (percent > 100)
 *     {
 *        percent = 100;//limit relative value to 100%
 *     }
 *  
 *     uiPercent = (uint16_t)((uint16_t)percent << (uint16_t)8u)
 *  
 *     if (relativePeakValue > uiPercent)
 *     {
 *        relativePeakValue -= (uint16_t)(relativePeakValue/PHYP_CAP_PLAUSI_COEFF);
 *        relativePeakValue += (uint16_t)(uiPercent/PHYP_CAP_PLAUSI_COEFF);
 *     }
 *     else
 *     {
 *        relativePeakValue = uiPercent;
 *     }
 *  
 *     PHYP_auiPeakKeyValues[keyId] = relativePeakValue;
 *     PHYP_uiPercentSum += relativePeakValue;
 *  
 *  }\endcode
 *  
 *  
 *  \param keyId: The ID of Capacitive Key\n
 *
 *  \note This function must be defined by application and is called by library itself.
 */
extern void _PHYP_vCapCheckPlausibility(uint8_t keyId,uint8_t percent);


/** \brief This is the post handler for plausibility check.
 *  
 *  This function is called after plausibility loop is done.
 *  The function should clear the cumulated percent sum, calculated in _PHYP_vCapCheckPlausibility.
 *  For further information please look example implementation in phycapxs.c
 *  
 *  \b Example:
 *  \code
 *  
 *  #define PHYP_MAX_PERCENT_SUM       250
 *  #define PHYP_IMPLAUSIBLE_LOCK_TIME 120
 *  
 *  void _PHYP_vCapDoPostPlausibility(void)
 *  {
 *     if (PHYP_uiPercentSum > PHYP_MAX_PERCENT_SUM)
 *     {
 *          PHYP_tim8ImplausibleLockTimer = PHYP_IMPLAUSIBLE_LOCK_TIME;
 *     }
 *     PHYP_uiPercentSum = 0;
 *  }\endcode
 *  
 *  \note This function must be defined by application and is called by library itself.
 */
extern void _PHYP_vCapDoPostPlausibility(void);


/** \brief Returns whether key \<keyId\> is plausible or not
 *  
 *  The following example shows an easy implementation without individual plausibility.
 *  
 *  \b Example:
 *  \code
 *  bool _PHYP_bCapIsPlausible(uint8_t keyId)
 *  { 
 *      return ((PHYP_tim8ImplausibleLockTimer == 0u) ? true : false);
 *  }
 *  \endcode
 *  
 *  \param keyId: The ID of Capacitive Key\n
 *
 *  \note This function must be defined by application and is called by library itself.
 */
extern bool _PHYP_bCapIsPlausible(uint8_t keyId);


/** \brief Factory calibration
 * 
 * \note This function must be defined by application and is called by library itself.
 */
extern void _PHYP_vCapDoCalibration(void);

/*lint -restore -e9059 */

#ifdef __cplusplus
}
#endif

#endif // PHYPPRIV_H
