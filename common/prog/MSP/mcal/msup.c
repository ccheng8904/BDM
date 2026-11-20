/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MSUP
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MSUP
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "msup.h"
#include "timerlib.h"
#include "std_lib/std_symbol.h"


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static struct MSUP_TimeoutHandle MSUP_LocalTimeoutHandle;

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
#ifndef HIGH_ADR_OFFSET
    #ifdef __BIG_ENDIAN_ADAPTATION
        #define HIGH_ADR_OFFSET  0 ///< Address offset for high byte/word (big endian)
        #define LOW_ADR_OFFSET   1 ///< Address offset for low byte/word (big endian)
    #elif defined(__LITTLE_ENDIAN_ADAPTATION)
        #define HIGH_ADR_OFFSET  1 ///< Address offset for high byte/word (little endian)
        #define LOW_ADR_OFFSET   0 ///< Address offset for low byte/word (little endian)
    #else
        #error "Either '__LITTLE_ENDIAN_ADAPTATION' or '__BIG_ENDIAN_ADAPTATION' must be defined in build process!"
    #endif
#endif

static uint8_t* MSUP_getBytePtr(uint16_t *value, uint32_t offset);
static uint16_t* MSUP_getWordPtr(uint32_t *value, uint32_t offset);


/** Accesses high byte of parameter word.
 *  \brief This function accesses the  high byte of parameter word.
 *  \param word: high byte of word will be accessed, type of word shall be int16_t or uint16_t
 *  \return none
 */
#define MSUP_HighByte(word) (*MSUP_getBytePtr(word,HIGH_ADR_OFFSET))

/** Accesses high word of parameter ms_long.
 * \brief This function accesses the high word of parameter ms_long
 * \param ms_long: high word of word will be accessed, type of word shall be int32_t or uint32_t
 * \return none
 */
#define MSUP_HighWord(ms_long) (*MSUP_getWordPtr(ms_long,HIGH_ADR_OFFSET))

/** Accesses low byte of parameter word.
 *  \brief This function accesses the  low byte of parameter word.
 *  \param word: low byte of word will be accessed, type of word shall be int16_t or uint16_t
 *  \return none
 */
#define MSUP_LowByte(word) (*MSUP_getBytePtr(word,LOW_ADR_OFFSET))
 
/** Accesses low word of parameter ms_long.
 * \brief This function accesses the low word of parameter ms_long
 * \param ms_long: low word of word will be accessed, type of word shall be int32_t or uint32_t
 * \return none
 */
#define MSUP_LowWord(ms_long) (*MSUP_getWordPtr(ms_long,LOW_ADR_OFFSET))


#if (!defined(__cplusplus) && !defined(MOCK_IT)) || (defined(__cplusplus) && defined(MOCK_IT))
// compiled if no unit test active or if included for unit test

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void MSUP_handleTimerExt(struct MSUP_TimeoutHandle *handle)
{
    uint16_t actualTime = TIM_uiGetCircleMicroSeconds();
    uint16_t deltaTime = (uint16_t)(actualTime - handle->timeStamp);
    handle->timeStamp = actualTime;
    handle->timer += deltaTime;
}


void MSUP_startTimeMeasureExt(struct MSUP_TimeoutHandle *handle)
{
    handle->timeStamp = TIM_uiGetCircleMicroSeconds();
    handle->timer = 0uL;
}


uint32_t MSUP_getMeasuredTimeExt(struct MSUP_TimeoutHandle *handle)
{
    MSUP_handleTimerExt(handle);
    return handle->timer;
}


void MSUP_setTimeOutExt(struct MSUP_TimeoutHandle *handle, uint32_t timeout)
{
    handle->timeout = timeout;
    MSUP_startTimeMeasureExt(handle);
}


bool MSUP_isTimeOutExt(struct MSUP_TimeoutHandle *handle)
{
    MSUP_handleTimerExt(handle);
    return (handle->timer >= handle->timeout) ? true : false;
}


void MSUP_handleTimer(void)
{
    MSUP_handleTimerExt(&MSUP_LocalTimeoutHandle);
}


void MSUP_startTimeMeasure(void)
{
    MSUP_startTimeMeasureExt(&MSUP_LocalTimeoutHandle);
}


uint32_t MSUP_getMeasuredTime(void)
{
    return MSUP_getMeasuredTimeExt(&MSUP_LocalTimeoutHandle);
}


void MSUP_delay(uint16_t delay)
{
    uint16_t currentTime = TIM_uiGetCircleMicroSeconds();

    if(delay == UINT16_MAX)
    {
        delay--;
    }
    while ((uint16_t)(TIM_uiGetCircleMicroSeconds() - currentTime) <= delay )
    {
        ;
    }
}


void MSUP_setTimeOut(uint32_t timeout)
{
    MSUP_setTimeOutExt(&MSUP_LocalTimeoutHandle, timeout);
}


bool MSUP_isTimeOut(void)
{
    return MSUP_isTimeOutExt(&MSUP_LocalTimeoutHandle);
}


SYMBOL_WEAK SYMBOL_NO_RETURN void MSUP_generateReset(void)
{
    NVIC_SystemReset();  //Reset the system provided by CMSIS
}
#endif // (!defined(__cplusplus) && !defined(MOCK_IT)) || (defined ...

#if !(defined(__cplusplus) && defined(MOCK_IT)) // do not include in test

uint8_t MSUP_getHighByte(uint16_t value)
{
    return (uint8_t)((value) >> 8);
}


uint8_t MSUP_getLowByte(uint16_t value)
{
    return (uint8_t)(value);
}

/* Conversion between object pointer and void pointer MisraC2012-11.5, tested to work correctly */
/*lint -save -e926 */ 
void MSUP_setHighByte(uint16_t *value1, uint8_t value2)
{
    MSUP_HighByte(value1) = value2;
}


void MSUP_setLowByte(uint16_t *value1, uint8_t value2)
{
    MSUP_LowByte(value1) = value2;
}


uint32_t MSUP_getHighWord(uint32_t value)
{
    return ((uint16_t)((value) >> 16));  //lint !e9031 tested to work correctly
}


uint32_t MSUP_getLowWord(uint32_t value)
{
    return ((uint16_t)(value));
}


void MSUP_setHighWord(uint32_t *value1, uint16_t value2)
{
    MSUP_HighWord(value1) = value2;
}


void MSUP_setLowWord(uint32_t *value1, uint16_t value2)
{
    MSUP_LowWord(value1) = value2;
}
/*lint -restore -e926 */ 


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/
/* Pointer arithmetic not allowed [+] MisraC2012 18.4 */
/*lint -e{9016} */
/*Axivion Disable MisraC2012-18.4 : no problem here */

static uint8_t* MSUP_getBytePtr(uint16_t *value, uint32_t offset)
{   // explicit cast from uint16_t* to uint8_t* [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]
    return ((uint8_t*)value)+offset; /*lint !e928 */ 
}

static uint16_t* MSUP_getWordPtr(uint32_t *value, uint32_t offset)
{   // explicit cast from uint32_t* to uint16_t* [MISRA 2012 Rule 11.3, required], [MISRA 2012 Rule 11.5, required]
    return ((uint16_t*)value)+offset; /*lint !e929 */ 
}
#endif //!(defined(__cplusplus) && defined(MOCK_IT))
