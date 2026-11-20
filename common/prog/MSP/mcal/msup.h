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


#ifndef MSUP_H
#define MSUP_H

#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif //__cplusplus

 /******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MSUP.
 *            HSUP library is used because of interrupts enable/disable. It is too 
 *            complicated to implement interrupts enable/disable because it uses
 *            other functionality and it has to be able to measure disable time,
 *            etc. It will be resolved in future update.
 **/


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "hsup.h"
#include "mcal.h"


/******************************************************************************/
/* PUBLIC TYPE DECLARATIONS                                                   */
/******************************************************************************/

/** Structure for specifying register 32-bit bitfield */
struct MSUP_RegBitField32
{
    volatile uint32_t   *regPtr;
    uint32_t            bitFieldMask;
};


/** Structure for specifying register 16-bit bitfield */
struct MSUP_RegBitField16
{
    volatile uint16_t   *regPtr;
    uint16_t            bitFieldMask;
};


struct MSUP_TimeoutHandle
{
    /** Variable for storing the current timer value */
    uint32_t  timer;
    
    /** Variable for storing required time for timeout in microseconds */
    uint32_t  timeout;
    
    /** Variable for storing the last timestamp */
    uint16_t  timeStamp;
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** Function for updating local time measurement variables.
 *  \brief  To handle internal timer functionality, function MSUP_handleTimerExt,
 *          MSUP_getMeasuredTimeExt or MSUP_isTimeoutExt needs to be
 *          called periodically at least once in 30 ms.
 *  \param handle: pointer to MSUP_TimeoutHandle instance
 *  \return none
 */
extern void MSUP_handleTimerExt(struct MSUP_TimeoutHandle *handle);


/** Function for starting of microsecond time measurement with local time 
 *  measurement variables.
 *  \brief  See related MSUP_handleTimerExt function.
 *  \param handle: pointer to MSUP_TimeoutHandle instance
 *  \return none
 */
extern void MSUP_startTimeMeasureExt(struct MSUP_TimeoutHandle *handle);


/** Sets timeout for local time measurement variables.
 *  \brief This function sets required time for timeout in microseconds.
 *  \param handle: pointer to MSUP_TimeoutHandle instance
 *  \param timeout: The number of microseconds after which MSUP_isTimeOutExt returns
 *         true(0x1 .. 0xFFFF FFFF). See related MSUP_handleTimerExt function.
 *  \return none
 */
extern void MSUP_setTimeOutExt(struct MSUP_TimeoutHandle *handle, uint32_t timeout);


/** Function for getting actual measured time in microseconds from local 
 *  time measurement variables.
 *  \brief  For starting of time measurement use MSUP_startTimeMeasureExt
 *          or MSUP_setTimeOutExt function. See related MSUP_handleTimerExt function.
 *  \param handle: pointer to MSUP_TimeoutHandle instance
 *  \return measured time
 */
extern uint32_t MSUP_getMeasuredTimeExt(struct MSUP_TimeoutHandle *handle);


/** Performs a non busy wait for an arbitrary time in microseconds.
 *  \brief This function performs a non busy wait for an arbitrary time
 *         in microseconds set by MSUP_setTimeOutExt.
 *         See related MSUP_handleTimerExt function. 
 *  \param handle: pointer to MSUP_TimeoutHandle instance
 *  \return true if timer is down, false else
 */
extern bool MSUP_isTimeOutExt(struct MSUP_TimeoutHandle *handle);


/** Performs a non busy wait for an arbitrary time in microseconds.
 *  \brief  Same as MSUP_isTimeOutExt but with different return type.
 *          This allows it to be used together with MCAL_RETURN_ON_ERROR or
 *          MCAL_BREAK_ON_ERROR macros. See related MSUP_handleTimerExt function.
 *  \param handle: pointer to MSUP_TimeoutHandle instance
 *  \return MCAL_ERROR_TIMEOUT if time has been reached, MCAL_OK otherwise
 */
static inline enum MCAL_Error MSUP_checkTimeOutExt(struct MSUP_TimeoutHandle *handle)
{
    return MSUP_isTimeOutExt(handle) ? MCAL_ERROR_TIMEOUT : MCAL_OK;
}


/** Performs a busy wait for an arbitrary time in microseconds.
 *  \brief This function performs a busy wait for an arbitrary time.
 *  \param delay: The function waits at least delay microseconds (0x1 .. 0xFFFF).
 *  \return none
 */
extern void MSUP_delay(uint16_t delay);


/** Function for updating internal time measurement variables.
 *  \brief  To handle internal timer functionality, function MSUP_handleTimer,
 *          MSUP_getMeasuredTime or MSUP_isTimeout needs to be
 *          called periodically at least once in 30 ms.
 *  \return none
 */
extern void MSUP_handleTimer(void);


/** Function for starting of microsecond time measurement with internal time 
 *  measurement variables.
 *  \brief  See related MSUP_handleTimer function.
 *  \return none
 */
extern void MSUP_startTimeMeasure(void);


/** Sets timeout for internal time measurement variables.
 *  \brief This function sets required time for timeout in microseconds.
 *  \param timeout: The number of microseconds after which MSUP_isTimeOut returns
 *         true(0x1 .. 0xFFFF FFFF). See related MSUP_handleTimer function.
 *  \return none
 */
extern void MSUP_setTimeOut(uint32_t timeout);


/** Function for getting actual measured time in microseconds from internal 
 *  time measurement variables.
 *  \brief  For starting of time measurement use MSUP_startTimeMeasure
 *          or MSUP_setTimeOut function. See related MSUP_handleTimer function.
 *  \return measured time
 */
extern uint32_t MSUP_getMeasuredTime(void);


/** Performs a non busy wait for an arbitrary time in microseconds.
 *  \brief This function performs a non busy wait for an arbitrary time
 *         in microseconds set by MSUP_setTimeOut.
 *         See related MSUP_handleTimer function. 
 *  \param none
 *  \return true if timer is down, false else
 */
extern bool MSUP_isTimeOut(void);


/** Performs a non busy wait for an arbitrary time in microseconds.
 *  \brief  Same as MSUP_isTimeOut but with different return type.
 *          This allows it to be used together with MCAL_RETURN_ON_ERROR or
 *          MCAL_BREAK_ON_ERROR macros. See related MSUP_handleTimer function.
 *  \return MCAL_ERROR_TIMEOUT if time has been reached, MCAL_OK otherwise
 */
static inline enum MCAL_Error MSUP_checkTimeOut(void)
{
    return MSUP_isTimeOut() ? MCAL_ERROR_TIMEOUT : MCAL_OK;
}


#if !defined(NO_ISR_SUPPORT) || defined(MOCK_IT)
/** Disables all interrupts.
 *  \brief This function disables all interrupts.
 *  \param none
 *  \return none
 */
extern void HSUP_disableInt(void);


/** Enables all interrupts.
 *  \brief This function enables all interrupts.
 *  \param none
 *  \return none
 */
extern void HSUP_enableInt(void);
#else
#define HSUP_DisableInt()
#define HSUP_enableInt()
#endif // NO_ISR_SUPPORT

/** Generates a CPU reset.
 *  \brief This function generates a CPU reset.
 *  \param none
 *  \return none
 */
extern void MSUP_generateReset(void);

#if defined(__cplusplus) && defined(MOCK_IT)
extern "C" { // in global namespace --> no mocking
#endif

/** Returns the high byte of the passed integer value.
 *  \brief This function returns the high byte of the passed integer value.
 *  \param value: The value where the high byte shall be returned.
 *  \return High byte of value.
 */
extern uint8_t MSUP_getHighByte(uint16_t value);


/** Returns the low byte of the passed integer value.
 *  \brief This function returns the low byte of the passed integer value.
 *  \param value: The value where the low byte shall be returned.
 *  \return Low byte of value.
 */
extern uint8_t MSUP_getLowByte(uint16_t value);


/** Sets the high byte of 16bit value.
 *  \brief This function sets the high byte of *value1 with the passed value value2.
 *  \param value1: The pointer to the value where the high byte will be set.
 *  \param value2: The new value of the high byte.
 *  \return none
 */
extern void MSUP_setHighByte(uint16_t *value1, uint8_t value2);


/** Sets the low byte of 16bit value.
 *  \brief This function sets the low byte of value1 with the passed value value2.
 *  \param value1: The pointer to the value where the low byte will be set.
 *  \param value2: The new value of the low byte.
 *  \return none
 */
extern void MSUP_setLowByte(uint16_t *value1, uint8_t value2);


 /** Returns the high word of the passed long value.
 *  \brief This function returns the high word of the passed long value.
 *  \param value: The value where the high word will be returned.
 *  \return The high word of the value val.
 */
extern uint32_t MSUP_getHighWord(uint32_t value);


 /** Returns the low word of the passed long value.
 *  \brief This function returns the low word of the passed long value.
 *  \param value: The value where the low word will be returned.
 *  \return The low word of the value val.
 */
extern uint32_t MSUP_getLowWord(uint32_t value);


 /** Sets the high word of uint32_t value.
 *  \brief This function sets the high word of *value1 with the passed value value2.
 *  \param value1: The pointer to the value where the high word will be set.
 *  \param value2: The new value of the high word.
 *  \return none
 */
extern void MSUP_setHighWord(uint32_t *value1, uint16_t value2);


 /** Sets the low word of uint32_t value.
 *  \brief This function sets the low word of *value1 with the passed value value2.
 *  \param value1: The pointer to the value where the low word will be set.
 *  \param value2: The new value of the low word.
 *  \return none
 */
extern void MSUP_setLowWord(uint32_t *value1, uint16_t value2);


/** Clears section of 32-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be cleared
 *  \return none
 */
static inline void MSUP_clearReg32(volatile uint32_t *reg, uint32_t mask)
{
    *reg &= ~mask;
}


/** Sets section of 32-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be set
 *  \return none
 */
static inline void MSUP_setReg32(volatile uint32_t *reg, uint32_t mask)
{
    *reg |= mask;
}


/** Writes section of 32-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be overwritten
 *  \param value new value for section
 *  \return none
 */
static inline void MSUP_writeReg32(volatile uint32_t *reg, uint32_t mask, uint32_t value)
{
    uint32_t tmpReg = *reg & ~mask;
    tmpReg |= value & mask;
    *reg = tmpReg;
}


/** Clears section of 16-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be cleared
 *  \return none
 */
static inline void MSUP_clearReg16(volatile uint16_t *reg, uint16_t mask)
{
    *reg &= ~mask;
}


/** Sets section of 16-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be set
 *  \return none
 */
static inline void MSUP_setReg16(volatile uint16_t *reg, uint16_t mask)
{
    *reg |= mask;
}


/** Writes section of 16-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be overwritten
 *  \param value new value for section
 *  \return none
 */
static inline void MSUP_writeReg16(volatile uint16_t *reg, uint16_t mask, uint16_t value)
{
    uint16_t tmpReg = *reg & ~mask;
    tmpReg |= value & mask;
    *reg = tmpReg;
}


/** Clears section of 8-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be cleared
 *  \return none
 */
static inline void MSUP_clearReg8(volatile uint8_t *reg, uint8_t mask)
{
    *reg &= ~mask;
}


/** Sets section of 8-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be set
 *  \return none
 */
static inline void MSUP_setReg8(volatile uint8_t *reg, uint8_t mask)
{
    *reg |= mask;
}


/** Writes section of 8-bit register given by mask.
 *  \param reg pointer to the register
 *  \param mask define bits to be overwritten
 *  \param value new value for section
 *  \return none
 */
static inline void MSUP_writeReg8(volatile uint8_t *reg, uint8_t mask, uint8_t value)
{
    uint8_t tmpReg = *reg & ~mask;
    tmpReg |= value & mask;
    *reg = tmpReg;
}


/** Clears bitfield of 16-bit register.
 *  \param regBitField structure containing register pointer and bitfield mask
 *  \return none
 */
static inline void MSUP_clearRegBitField16(const struct MSUP_RegBitField16 *regBitField)
{
    MSUP_clearReg16(regBitField->regPtr, regBitField->bitFieldMask);
}


/** Sets bitfield of 16-bit register.
 *  \param regBitField structure containing register pointer and bitfield mask
 *  \return none
 */
static inline void MSUP_setRegBitField16(const struct MSUP_RegBitField16 *regBitField)
{
    MSUP_setReg16(regBitField->regPtr, regBitField->bitFieldMask);
}


/** Clears bitfield of 32-bit register.
 *  \param regBitField structure containing register pointer and bitfield mask
 *  \return none
 */
static inline void MSUP_clearRegBitField32(const struct MSUP_RegBitField32 *regBitField)
{
    MSUP_clearReg32(regBitField->regPtr, regBitField->bitFieldMask);
}


/** Sets bitfield of 32-bit register.
 *  \param regBitField structure containing register pointer and bitfield mask
 *  \return none
 */
static inline void MSUP_setRegBitField32(const struct MSUP_RegBitField32 *regBitField)
{
    MSUP_setReg32(regBitField->regPtr, regBitField->bitFieldMask);
}


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MSUP_H
