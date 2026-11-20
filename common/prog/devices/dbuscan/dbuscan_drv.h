/*******************************************************************************
 *  Copyright (c) 2022 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      DBCDRV
 ******************************************************************************/


#ifndef DBUSCAN_DRV_H
#define DBUSCAN_DRV_H

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Public definitions and declarations for unit dbuscan
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "mcal_channels.h"
#include "dbuscan.h"
#include "dbuscan_types.h"

/******************************************************************************/
/* DEPENDENCIES                                                               */
/******************************************************************************/
#define DEP_DBC    MOD_DLL, ///< dependencies of module


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
#define DBCDRV_REG_SIZE                          (4u) ///< The number of bytes of DBusCAN chip registers
#define DBCDRV_DBUS_TXS_FIFO_SIZE                64u  ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Tx Status FIFO buffer. 64 bytes is maximum, must be a multiple of 8 bytes.
#define DBCDRV_DBUS_RX_FIFO_SIZE                512u  ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Rx FIFO buffer. Must be a multiple of 4 bytes.
#define DBCDRV_DBUS_TX_FIFO_SIZE                512u  ///< The number of bytes to be allocated in DBusCAN's RAM for the DBus Tx FIFO buffer. Must be a multiple of 4 bytes.
#define DBCDRV_DBUS_BASE_RAM_ADDR                 0u  ///< Start (offset) address of DBusCAN's RAM space allocated for DBus buffers


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/
extern bool DBCDRV_eepromWriteEnable; ///< Enables or disables write to the chip EEPROM via DBCDRV_HandleTask()

/** \brief   Configuration of SPI peripheral unit and pins to be used for communication with the DBusCAN chip
 */
extern const struct MSPI_Channel DBCDRV_mspiChannel;

/** \brief   MSPI Handle for SPI communication with DBusCAN
 */
extern struct MSPI_Handle DBCDRV_mspiHandle;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/** \brief   Gets GPIO pin to be used for the external interrupt from interrupt pin (INT) of the DBusCAN chip.
 *
 *  \return  pointer to structure MEXTI_Channel
 */
const struct MEXTI_Channel* DBCDRV_getMextiChannel(void);


/** \brief   Gets clock frequency value used for SPI communication with the DBusCAN chip.
 *
 *  \return  uint32_t
 */
uint32_t DBCDRV_getMspiFrequency(void);


/** \brief   Gets structure of values used for DBusCAN chip configuration (to be) stored in chip EEPROM.
 *
 *  \return  configuration structure DBC_Cfg_t
 */
extern DBC_Cfg_t DBCDRV_getConfig(void);


/** \brief   Initializes MSPI, MEXTI and MDIO channels to be used for communication with the DBusCAN chip.
 *
 *  \param   irqHandleCbFunc: callback function to be executed when external interrupt from the DBusCAN chip occurs
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc);


/** \brief   Initializes the DBusCAN chip for communication.
 *
 *  \param   irqHandleCbFunc: callback function for handling interrupts from chip
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_init(MCAL_CallbackFunction_t irqHandleCbFunc);


#ifdef APP_VARIANT
/** \brief Disables detection of interrupts from DBusCAN chip
 */
extern void DBCDRV_disableIrq(void);
#endif


/** \brief   Handles write to chip's EEPROM. It is called from scheduler.
 *
 *  \return  uint8_t
 */
extern uint8_t DBCDRV_HandleTask(void);


/** \brief  Returns information whether power on reset of the device has occurred.
 *
 *  \return true or false
 */
extern bool DBCDRV_isPowerOnReset(void);


/** \brief  Sets notification that power on reset of the device has occurred.
 *
 *  \note   This interface is only for internal use, not to be called in application SW.
 */
extern void DBCDRV_notifyPowerOnReset(void);


/** \brief   Resets the DBusCAN chip.
 *
 *  \details Reset is triggered by the SPI frame sent to the DBusCAN chip to set a reset flag in a register.
 *
 *  \note    If full reset is executed, @DBCDRV_isResetFinished() can be used
 *           to see when DBusCAN chip is ready after reset.
 *
 *  \param   rstType: SOFT / HARD / FULL (enum DBC_Reset)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_reset(enum DBC_Reset rstType);


/** \brief   Resets the DBusCAN chip by special toggling of the SPI pins.
 *
 *  \note    @DBCDRV_isResetFinished() can be used to see when DBusCAN chip is ready after reset.
 */
extern void DBCDRV_spiReset(void);


/** \brief   Returns whether the DBusCAN chip is ready after full reset
 *           triggered by either DBCDRV_reset(DBC_RST_FULL) or DBCDRV_spiReset().
 *
 *  \return  bool
 */
extern bool DBCDRV_isResetFinished(void);


/** \brief   Sets the power mode of the DBusCAN chip.
 *
 *  \param   mode: power mode to be set (enum DBC_PowerMode)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_setPowerMode(enum DBC_PowerMode mode);


/** \brief   Writes data to the DBusCAN chip.
 *
 *  \param   addr:  start address where to write data (enum DBC_RegAddr)
 *  \param   writeBuf: pointer to the buffer with data to be written
 *  \param   len:   length of the data to be written
 *  \param   cmd:   write operation command (enum DBC_WriteCmd)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_write(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_WriteCmd cmd);


/** \brief   Writes data to the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   addr:        start address where to write data (enum DBC_RegAddr)
 *  \param   writeBuf:    pointer to the buffer with data to be written
 *  \param   len:         length of the data to be written
 *  \param   cmd:         write operation command (enum DBC_WriteCmd)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_writeNbl(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_WriteCmd cmd);


/** \brief   Reads data from the DBusCAN chip.
 *
 *  \param   addr:    start address from where to read data (enum DBC_RegAddr)
 *  \param   readBuf: pointer to the buffer where read data should be stored
 *  \param   len:     length of the data to be read
 *  \param   cmd:     read operation command (enum DBC_ReadCmd)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_read(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_ReadCmd cmd);


/** \brief   Reads data from the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   addr:        start address from where to read data (enum DBC_RegAddr)
 *  \param   readBuf:     pointer to the buffer where read data should be stored
 *  \param   len:         length of the data to be read
 *  \param   cmd:         read operation command (enum DBC_ReadCmd)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_readNbl(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_ReadCmd cmd);


/** \brief   Writes 4-byte register of the DBusCAN chip.
 *
 *  \param   addr:  address of the register to be written (enum DBC_RegAddr)
 *  \param   data:  data to be written (4 bytes)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data);


/** \brief   Writes 4-byte register of the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   addr:  address of the register to be written (enum DBC_RegAddr)
 *  \param   data:  data to be written (4 bytes)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_writeReg32Nbl(enum DBC_RegAddr addr, uint32_t data);


/** \brief   Reads data from the DBusCAN chip's register.
 *
 *  \param   addr:  address of the register to be read (enum DBC_RegAddr)
 *
 *  \return  4 bytes of data
 */
extern uint32_t DBCDRV_readReg32(enum DBC_RegAddr addr);


/** \brief   Reads data from the DBusCAN chip's register with non-blocking SPI communication.
 *
 *  \note    The read data can be accessed using the function DBCDRV_getReadReg32Nbl
 *
 *  \param   addr:        address of the register to be read (enum DBC_RegAddr)
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_readReg32Nbl(enum DBC_RegAddr addr);


/** \brief   Returns the last data read by the function DBCDRV_readReg32Nbl
 *
 *  \return  uint32_t
 */
extern uint32_t DBCDRV_getReadReg32Nbl(void);


/** \brief   Writes the protected IPEC register (0x0814) of the DBusCAN chip.
 *
 *  \note    To successfully write the IPEC register, the DBusCAN chip must be in STANDBY power mode.
 *
 *  \param   bitVal:  value to be set
 *  \param   bitPos:  position of the bit/bitfield to be set
 *  \param   bitMask: mask of the bit/bitfield to be set
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask);


/** \brief   Returns information whether some interrupt event has occurred in the DBusCAN chip.
 *
 *  \return  bool
 */
extern bool DBCDRV_isIrqEvent(void);


/** \brief   Reads interrupt flags from the DBusCAN chip.
 *
 *  \param   irqBuf: pointer to the buffer in which read interrupt flags will be stored
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_readIrq(union DBC_IrqBuf *irqBuf);


/** \brief   Unlocks DBus configuration registers of DBusCAN chip.
 */
extern enum DBC_Error DBCDRV_enableCfgDbus(void);


/** \brief   Locks DBus configuration registers of DBusCAN chip.
 */
extern enum DBC_Error DBCDRV_disableCfgDbus(void);


/** \brief   Converts DBus decimal baudrate value (in hectobauds) to the baudrate value code used in the DBusCAN chip registers.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  baudrate value code (see enum DBC_DbusBaudrate)
 */
extern uint16_t DBCDRV_convertDbusBaudValue(uint16_t baudrate);


/** \brief   Sets DBus baudrate in the DBusCAN chip.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  enum DBC_Error
 */
extern enum DBC_Error DBCDRV_setDbusBaudrate(uint16_t baudrate);


#ifdef __cplusplus
}
#endif

#endif // DBUSCAN_DRV_H
