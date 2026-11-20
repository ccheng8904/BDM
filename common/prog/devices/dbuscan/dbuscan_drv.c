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


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup dbuscan
 *
 *  \brief   Implementation of generic drivers for the DBusCAN chip
 *
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include <stdint.h>
#include "dbuscan_drv.h"
#include "mcal/mcal_includes.h"
#include "math/mathDiv.h"
#include "utility.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/
#define DBCDRV_RESET_TIME_US                    700u ///< The time (in microseconds) after a reset event before the device is ready


/******************************************************************************/
/* PRIVATE TYPE DEFINITIONS                                                   */
/******************************************************************************/

/** \brief   Possible states of the write process to DBusCAN chip's EEPROM.
 */
enum DBCDRV_EepWriteState
{
    DBCDRV_EEP_WRITE_START,
    DBCDRV_EEP_WRITE_ONGOING,
    DBCDRV_EEP_WRITE_END
};


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/
static void DBCDRV_setSpiFrameHdr(uint16_t addr, uint16_t len, uint8_t cmd, uint8_t *writeBuf);
static void DBCDRV_uint32ToBuffer(uint32_t value, uint8_t *buff);
static uint32_t DBCDRV_bufferToUint32(const uint8_t *buff);
static enum DBC_Error DBCDRV_configure(DBC_Cfg_t cfg);
static enum DBC_Error DBCDRV_writeEeprom(void);
static bool DBCDRV_isSupplyForEepromWrite(void);
static enum DBC_Error DBCDRV_setTableDbusBaudrate(uint16_t baudrate);
#ifdef DBC_ANY_DBUS_BAUDRATE_ENABLED
static enum DBC_Error DBCDRV_setAnyDbusBaudrate(uint16_t baudrate);
static uint32_t DBCDRV_getClockInputInHz(uint32_t clkinBitfield);
#endif //DBC_ANY_DBUS_BAUDRATE_ENABLED
static enum DBC_Error DBCDRV_sendSpiFrame(const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen);


/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/
static bool DBCDRV_isPwrOnReset = false; // Indicates that power on reset of the device has occurred
static struct MSUP_TimeoutHandle DBCDRV_resetTimeHandle;
#ifdef APP_VARIANT
static struct MEXTI_Handle DBCDRV_mextiHandle;
static MCAL_Callback_t DBCDRV_cbIrqHandle;
#endif
static uint8_t DBCDRV_readRegBuf[DBC_SPI_HDR_SIZE + DBCDRV_REG_SIZE];   ///< Buffer for reading DBusCAN registers via non-blocking SPI communication


/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/
struct MSPI_Handle DBCDRV_mspiHandle;
bool DBCDRV_eepromWriteEnable = false;


/******************************************************************************/
/* PRIVATE INLINE FUNCTION DEFINITIONS                                        */
/******************************************************************************/
/** \brief   Sends SPI frame to the DBusCAN chip with non-blocking SPI communication.
 *
 *  \param   writeBuf:    pointer to buffer with data to be written
 *  \param   writeLen:    number of bytes to be written
 *  \param   readBuf:     pointer to buffer for storing read data
 *  \param   readLen:     number of bytes to be read
 *
 *  \return  enum DBC_Error
 */
static inline enum DBC_Error DBCDRV_sendSpiFrameNbl(const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen)
{
    return (MCAL_OK != MSPI_transfer(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen)) ? DBC_ERROR : DBC_OK;
}


/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc)
{
    const struct MSPI_Config mspiCfg =
    {
        .dataBits         = MSPI_DATA_BITS_8,
        .clockPolarity    = MSPI_CLOCK_POLARITY_0,
        .clockPhase       = MSPI_CLOCK_PHASE_0,
        .dataInvert       = MSPI_DATA_INVERT_DISABLED,
        .frameFormat      = MSPI_FRAME_FORMAT_MSB_FIRST,
        .chipSelMode      = MSPI_CHIP_SELECT_AUTO_LOW,
        .clockFreq        = DBCDRV_getMspiFrequency(),
        .misoPullResistor = MDIO_PULL_UP
    };
    /* Configuration of IO pin used for IRQ sensing from DBusCAN */
    const struct MDIO_Channel *intPin = MEXTI_getPin(DBCDRV_getMextiChannel());

    if(MCAL_OK != MDIO_init(intPin, &MDIO_INPUT_PULL_UP))
    {
        return DBC_ERROR;
    }

#ifdef APP_VARIANT
    const struct MEXTI_Config mextiCfg = {.trigger = MEXTI_TRIGGER_FALLING};
    if(MCAL_OK == MEXTI_init(&DBCDRV_mextiHandle, DBCDRV_getMextiChannel(), &mextiCfg))
    {
        MCAL_initCallback(&DBCDRV_cbIrqHandle, irqHandleCbFunc, (void*)&DBCDRV_mextiHandle);
    }
    else
    {
        return DBC_ERROR;
    }
#else
    (void)irqHandleCbFunc; //lint !e920 Cast from pointer to void - callback function is not used in bootloader variants
    DBCDRV_mspiHandle.status = MCAL_STATUS_RESET; // set reset status explicitly (necessary for bootloader variants due to the uninitialized RAM)
#endif //APP_VARIANT

    if(MCAL_OK != MSPI_init(&DBCDRV_mspiHandle, &DBCDRV_mspiChannel, &mspiCfg))
    {
        return DBC_ERROR;
    }

    return DBC_OK;
}

enum DBC_Error DBCDRV_init(MCAL_CallbackFunction_t irqHandleCbFunc)
{
    uint16_t dataOffset = DBCDRV_REG_SIZE;
    uint32_t regVal;
    bool isAdvPwrMgmt;
    bool isPwrOnReset;
    DBC_WriteBuf_t buff;
    DBC_Cfg_t cfg = DBCDRV_getConfig();

    DBC_RETURN_ON_ERROR(DBCDRV_initComChannels(irqHandleCbFunc));
    DBC_RETURN_ON_ERROR(DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY)); // Initialization of chip can only be done when in STANDBY power mode

    isAdvPwrMgmt = (DBC_DBUS_DPC_ADV_PWR_MGMT_MASK == (DBCDRV_readReg32(DBC_DBUS_DPC_ADDR) & DBC_DBUS_DPC_ADV_PWR_MGMT_MASK)) ? true : false;
    isPwrOnReset = (DBC_IF_PWRON_MASK == (DBCDRV_readReg32(DBC_IF_ADDR) & DBC_IF_PWRON_MASK)) ? true : false;
    if (isPwrOnReset && !isAdvPwrMgmt)
    {   // full reset as a workaround for the issue of DBusCAN chip (not signalling interrupts when booting in simple power management)
        struct MSUP_TimeoutHandle resetTimeHandle;
        DBC_RETURN_ON_ERROR(DBCDRV_reset(DBC_RST_FULL));
        MSUP_setTimeOutExt(&resetTimeHandle, DBCDRV_RESET_TIME_US);
        do
        {   // wait until device is ready
            regVal = DBCDRV_readReg32(DBC_IF_ADDR);
        } while ((DBC_IF_PWRON_MASK != (regVal & DBC_IF_PWRON_MASK)) && !MSUP_isTimeOutExt(&resetTimeHandle));
    }
    else
    {
        DBC_RETURN_ON_ERROR(DBCDRV_reset(DBC_RST_DBUS_HARD));
    }

    DBC_RETURN_ON_ERROR(DBCDRV_configure(cfg));
    DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());

    // Configuration of interrupts
    regVal = DBCDRV_readReg32(DBC_IE_ADDR);
    regVal &= ~DBC_IF_UVCC_MASK; // disable interrupt on UVCC event (as this flag cannot be cleared by SW)
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IE_ADDR, regVal));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_IE_ADDR, (DBC_DBUS_IE_ALL_BIT_MASK & ~DBC_DBUS_IE_DBUSSLNT_EN_MASK))); //enable all DBus interrupts except DBus silent flag
    // Clear global interrupt and SPI status flags
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IF_ADDR, DBCDRV_readReg32(DBC_IF_ADDR)));
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_STATUS_ADDR, DBCDRV_readReg32(DBC_STATUS_ADDR)));

    // Configuration of DBus buffers in chip's RAM
    DBCDRV_uint32ToBuffer(DBCDRV_DBUS_BASE_RAM_ADDR, buff.payload);              // Dbus Base Address reg (0x4060) => set start address of DBusCAN's RAM allocated for DBus buffers
    regVal = ((uint32_t)DBCDRV_DBUS_RX_FIFO_SIZE << DBC_DBUS_BSC0_RX_BUF_SIZE_POS) + DBCDRV_DBUS_TX_FIFO_SIZE;
    DBCDRV_uint32ToBuffer(regVal, &buff.payload[dataOffset]);                    // DBus Buffer Size Control 0 reg (0x4064) => set DBus Rx & Tx FIFO size
    dataOffset += DBCDRV_REG_SIZE;
    DBCDRV_uint32ToBuffer(DBCDRV_DBUS_TXS_FIFO_SIZE, &buff.payload[dataOffset]); // Dbus Buffer Size Control 1 reg (0x4068) => set DBus Tx Status FIFO size
    dataOffset += DBCDRV_REG_SIZE;
    DBC_RETURN_ON_ERROR(DBCDRV_write(DBC_DBUS_BSA_ADDR, &buff, dataOffset, DBC_WRITE_H)); // write to registers BSA, BSC0 and BSC1

#ifdef BOOTLOADER_VARIANT
    // Set RAW communication mode over DBus
    regVal  = DBCDRV_readReg32(DBC_DBUS_RXC_ADDR);
    regVal |= DBC_DBUS_RXC_RX_MODE_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_RXC_ADDR, regVal));
#else
    // Allow maximum number of DBus Node ID filters
    regVal = DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR) | DBC_DBUS_SIDFC_LSS_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
    // Enable message reception for all subsystems by using node filter
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_NF14_ADDR, DBC_DBUS_NF_ALL_SUBSYS_EN_MASK | ((uint32_t)cfg.NODE_ID << DBC_DBUS_NF_PID_POS)));
#endif //BOOTLOADER_VARIANT

    DBC_RETURN_ON_ERROR(DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL));

#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
    DBCDRV_isPwrOnReset = false; // reset notification that power on reset of the device has occurred
#endif
#ifdef APP_VARIANT
    // Enable detection of interrupts from DBusCAN chip
    MEXTI_enableEvent(&DBCDRV_mextiHandle, &DBCDRV_cbIrqHandle);
#endif

    return DBC_OK;
}

#ifdef APP_VARIANT
void DBCDRV_disableIrq(void)
{
    MEXTI_disableEvent(&DBCDRV_mextiHandle, &DBCDRV_cbIrqHandle);
}
#endif

uint8_t DBCDRV_HandleTask(void)
{
    if (true == DBCDRV_eepromWriteEnable)
    {
        if (DBC_ERROR != DBCDRV_writeEeprom())
        {
            DBCDRV_eepromWriteEnable = false;
        }
    }
    return TASK_INITIALISED;
}

bool DBCDRV_isPowerOnReset(void)
{
    return DBCDRV_isPwrOnReset;
}

void DBCDRV_notifyPowerOnReset(void)
{
    DBCDRV_isPwrOnReset = true;
}

enum DBC_Error DBCDRV_reset(enum DBC_Reset rstType)
{
    if (rstType == DBC_RST_FULL)
    {
        const uint32_t resetKey = DBC_MOPC_DEVICE_RESET_MASK | ((uint32_t)DBC_POWER_MODE_STANDBY << DBC_MOPC_MODE_SEL_POS);
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_MOPC_ADDR, resetKey));
        #ifdef APP_VARIANT
        MSUP_setTimeOutExt(&DBCDRV_resetTimeHandle, DBCDRV_RESET_TIME_US);
        #endif
    }
    else if((rstType == DBC_RST_DBUS_SOFT) || (rstType == DBC_RST_DBUS_HARD))
    {
        uint32_t regVal;
        struct MSUP_TimeoutHandle resetTimeHandle;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_BCC_ADDR, (uint32_t)rstType));
        MSUP_setTimeOutExt(&resetTimeHandle, DBCDRV_RESET_TIME_US);
        do
        {
            regVal = DBCDRV_readReg32(DBC_DBUS_BCC_ADDR);
            regVal &= (uint32_t)rstType;
        } while (((uint32_t)rstType == regVal) && !MSUP_isTimeOutExt(&resetTimeHandle));
    }
    else
    {
        // invalid reset mode
        return DBC_ERROR;
    }
    return DBC_OK;
}

void DBCDRV_spiReset(void)
{
    MDIO_write(DBCDRV_mspiChannel.sclk, (bool)true);
    MDIO_write(DBCDRV_mspiChannel.cs,   (bool)true);
    (void)MDIO_init(DBCDRV_mspiChannel.sclk, &MDIO_OUTPUT);
    (void)MDIO_init(DBCDRV_mspiChannel.mosi, &MDIO_OUTPUT);
    MSUP_delay(10u);

    MDIO_write(DBCDRV_mspiChannel.sclk, (bool)false);
    MSUP_delay(10u);
    MDIO_write(DBCDRV_mspiChannel.cs, (bool)false);
    MSUP_delay(10u);

    MDIO_toggle(DBCDRV_mspiChannel.mosi);
    MSUP_delay(10u);
    MDIO_toggle(DBCDRV_mspiChannel.mosi);
    MSUP_delay(10u);
    MDIO_toggle(DBCDRV_mspiChannel.mosi);
    MSUP_delay(10u);

    MDIO_write(DBCDRV_mspiChannel.cs, (bool)true);
#ifdef APP_VARIANT
    MSUP_setTimeOutExt(&DBCDRV_resetTimeHandle, DBCDRV_RESET_TIME_US);
#endif

    (void)MDIO_init(DBCDRV_mspiChannel.sclk, &MDIO_ALTERNATE_FUNCTION);
    (void)MDIO_init(DBCDRV_mspiChannel.mosi, &MDIO_ALTERNATE_FUNCTION);
}

bool DBCDRV_isResetFinished(void)
{
    return MSUP_isTimeOutExt(&DBCDRV_resetTimeHandle);
}

enum DBC_Error DBCDRV_setPowerMode(enum DBC_PowerMode mode)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_MOPC_ADDR);
    uint32_t chipMode = (regVal & DBC_MOPC_MODE_SEL_MASK) >> DBC_MOPC_MODE_SEL_POS;
    if (chipMode != (uint32_t)mode)
    {
        if ((DBC_POWER_MODE_NORMAL == mode) && (chipMode != (uint32_t)DBC_POWER_MODE_STANDBY))
        {   // changing to NORMAL mode is only possible from STANDBY mode
            regVal &= ~DBC_MOPC_MODE_SEL_MASK;
            regVal |= (uint32_t)DBC_POWER_MODE_STANDBY << DBC_MOPC_MODE_SEL_POS;
            (void)DBCDRV_writeReg32(DBC_MOPC_ADDR, regVal);
        }
        regVal &= ~DBC_MOPC_MODE_SEL_MASK;
        regVal |= (uint32_t)mode << DBC_MOPC_MODE_SEL_POS;
        (void)DBCDRV_writeReg32(DBC_MOPC_ADDR, regVal);
        regVal &= DBC_MOPC_MODE_SEL_MASK;
        return ((DBCDRV_readReg32(DBC_MOPC_ADDR) & DBC_MOPC_MODE_SEL_MASK) != regVal) ? DBC_ERROR : DBC_OK;
    }
    else
    {
        return DBC_OK;
    }
}

enum DBC_Error DBCDRV_write(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_WriteCmd cmd)
{
    DBCDRV_setSpiFrameHdr((uint16_t)addr, len, (uint8_t)cmd, writeBuf->spiHdr);
    return DBCDRV_sendSpiFrame((uint8_t*)(void*)writeBuf, (len + DBC_SPI_HDR_SIZE), NULL, 0u);
}

enum DBC_Error DBCDRV_writeNbl(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_WriteCmd cmd)
{
    DBCDRV_setSpiFrameHdr((uint16_t)addr, len, (uint8_t)cmd, writeBuf->spiHdr);
    return DBCDRV_sendSpiFrameNbl((uint8_t*)(void*)writeBuf, (len + DBC_SPI_HDR_SIZE), NULL, 0u);
}

enum DBC_Error DBCDRV_read(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_ReadCmd cmd)
{
    uint8_t writeBuf[DBC_SPI_HDR_SIZE];
    DBCDRV_setSpiFrameHdr((uint16_t)addr, len, (uint8_t)cmd, writeBuf);
    return DBCDRV_sendSpiFrame(writeBuf, DBC_SPI_HDR_SIZE, readBuf, (len + DBC_SPI_HDR_SIZE));
}

enum DBC_Error DBCDRV_readNbl(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_ReadCmd cmd)
{
    static uint8_t writeBuf[DBC_SPI_HDR_SIZE];
    DBCDRV_setSpiFrameHdr((uint16_t)addr, len, (uint8_t)cmd, writeBuf);
    return DBCDRV_sendSpiFrameNbl(writeBuf, DBC_SPI_HDR_SIZE, readBuf, (len + DBC_SPI_HDR_SIZE));
}

enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data)
{
    uint8_t buff[DBC_SPI_HDR_SIZE + DBCDRV_REG_SIZE];
    DBCDRV_uint32ToBuffer(data, &buff[DBC_SPI_HDR_SIZE]); // prepare data into a buffer (after the SPI header)
    DBCDRV_setSpiFrameHdr((uint16_t)addr, DBCDRV_REG_SIZE, (uint8_t)DBC_WRITE_H, buff);
    return DBCDRV_sendSpiFrame(buff, (uint16_t)(DBC_SPI_HDR_SIZE + DBCDRV_REG_SIZE), NULL, 0u);
}

enum DBC_Error DBCDRV_writeReg32Nbl(enum DBC_RegAddr addr, uint32_t data)
{
    static uint8_t buff[DBC_SPI_HDR_SIZE + DBCDRV_REG_SIZE];
    DBCDRV_uint32ToBuffer(data, &buff[DBC_SPI_HDR_SIZE]); // prepare data into a buffer (after the SPI header)
    DBCDRV_setSpiFrameHdr((uint16_t)addr, DBCDRV_REG_SIZE, (uint8_t)DBC_WRITE_H, buff);
    return DBCDRV_sendSpiFrameNbl(buff, (uint16_t)(DBC_SPI_HDR_SIZE + DBCDRV_REG_SIZE), NULL, 0u);
}

uint32_t DBCDRV_readReg32(enum DBC_RegAddr addr)
{
    uint8_t buff[DBC_SPI_HDR_SIZE + DBCDRV_REG_SIZE];
    if (DBC_ERROR != DBCDRV_read(addr, buff, DBCDRV_REG_SIZE, DBC_READ_H))
    {
        return DBCDRV_bufferToUint32(&buff[DBC_SPI_HDR_SIZE]);
    }
    else
    {
        return 0uL;
    }
}

enum DBC_Error DBCDRV_readReg32Nbl(enum DBC_RegAddr addr)
{
    static uint8_t writeBuffer[DBC_SPI_HDR_SIZE];
    DBCDRV_setSpiFrameHdr((uint16_t)addr, DBCDRV_REG_SIZE, (uint8_t)DBC_READ_H, writeBuffer);
    return DBCDRV_sendSpiFrameNbl(writeBuffer, DBC_SPI_HDR_SIZE, DBCDRV_readRegBuf, (DBCDRV_REG_SIZE + DBC_SPI_HDR_SIZE));
}

uint32_t DBCDRV_getReadReg32Nbl(void)
{
    return DBCDRV_bufferToUint32(&DBCDRV_readRegBuf[DBC_SPI_HDR_SIZE]);
}

enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_IPEC_ADDR);
    if (((regVal & bitMask) >> bitPos) == bitVal)
    {
        return DBC_OK;
    }

    // enable write access to IPEC reg
    regVal |= DBC_IPEC_CCE_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IPEC_ADDR, regVal));

    // configure desired bit/bitfield value
    regVal &= ~bitMask;
    regVal |= (bitVal << bitPos);

    // disable write access to IPEC reg
    regVal &= ~DBC_IPEC_CCE_MASK;

    // apply new IPEC reg settings
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_IPEC_ADDR, regVal));

    regVal &= bitMask;
    regVal &= ~DBC_IPEC_EP_CC_MASK; // do not include EP_CC bitfield as it always reads zeros
    return ((DBCDRV_readReg32(DBC_IPEC_ADDR) & bitMask) != regVal) ? DBC_ERROR : DBC_OK;
}

bool DBCDRV_isIrqEvent(void)
{
    const struct MDIO_Channel *intPin = MEXTI_getPin(DBCDRV_getMextiChannel());
    return !MDIO_read(intPin);
}

enum DBC_Error DBCDRV_readIrq(union DBC_IrqBuf *irqBuf)
{
    uint16_t irqBufSize = (uint16_t)sizeof(*irqBuf);
    DBCDRV_setSpiFrameHdr((uint16_t)DBC_IF_ADDR, (irqBufSize - DBC_SPI_HDR_SIZE), (uint8_t)DBC_READ_L, irqBuf->irqArray); // exclude SPI header data size - it is not counted in data length byte of SPI frame
#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
    return DBCDRV_sendSpiFrameNbl(irqBuf->irqArray, DBC_SPI_HDR_SIZE, irqBuf->irqArray, irqBufSize);
#else
    // Bootloader variants do not use interrupts
    return DBCDRV_sendSpiFrame(irqBuf->irqArray, DBC_SPI_HDR_SIZE, irqBuf->irqArray, irqBufSize);
#endif // VARIANT_PROGRAMMER || APP_VARIANT
}

enum DBC_Error DBCDRV_enableCfgDbus(void)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_DBUS_CCCR_ADDR);

    if ((regVal & DBC_DBUS_CCCR_INIT_MASK) != DBC_DBUS_CCCR_INIT_MASK)
    {
        regVal |= DBC_DBUS_CCCR_INIT_MASK;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal));
    }
    regVal |= DBC_DBUS_CCCR_CCE_MASK;
    DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal));

    return DBC_OK;
}

enum DBC_Error DBCDRV_disableCfgDbus(void)
{
    uint32_t regVal = DBCDRV_readReg32(DBC_DBUS_CCCR_ADDR);
    regVal &= ~(DBC_DBUS_CCCR_CCE_MASK | DBC_DBUS_CCCR_INIT_MASK);
    return DBCDRV_writeReg32(DBC_DBUS_CCCR_ADDR, regVal);
}

uint16_t DBCDRV_convertDbusBaudValue(uint16_t baudrate)
{
    switch (baudrate)
    {
        case 96u:    baudrate = (uint16_t)DBC_DBUS_BAUD_9600;  break;
        case 192u:   baudrate = (uint16_t)DBC_DBUS_BAUD_19200; break;
        case 384u:   baudrate = (uint16_t)DBC_DBUS_BAUD_38400; break;
        case 576u:   baudrate = (uint16_t)DBC_DBUS_BAUD_57600; break;
        case 1250u:  baudrate = (uint16_t)DBC_DBUS_BAUD_125K;  break;
        case 2500u:  baudrate = (uint16_t)DBC_DBUS_BAUD_250K;  break;
        case 5000u:  baudrate = (uint16_t)DBC_DBUS_BAUD_500K;  break;
        case 10000u: baudrate = (uint16_t)DBC_DBUS_BAUD_1M;    break;
        default:     baudrate = (uint16_t)DBC_DBUS_BAUD_9600;  break;
    }
    return baudrate;
}

enum DBC_Error DBCDRV_setDbusBaudrate(uint16_t baudrate)
{
#ifdef DBC_ANY_DBUS_BAUDRATE_ENABLED
    if (DBCDRV_readReg32(DBC_REVISION_ADDR) >= DBC_REVISION_WITH_ALL_FEATURES)
    {
        return DBCDRV_setAnyDbusBaudrate(baudrate);
    }
    else
    {
        return DBCDRV_setTableDbusBaudrate(baudrate);
    }
#else
    return DBCDRV_setTableDbusBaudrate(baudrate);
#endif //DBC_ANY_DBUS_BAUDRATE_ENABLED
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void DBCDRV_setSpiFrameHdr(uint16_t addr, uint16_t len, uint8_t cmd, uint8_t *writeBuf)
{
    writeBuf[DBC_SPI_HDR_BYTE_CMD]       = cmd;
    writeBuf[DBC_SPI_HDR_BYTE_ADDR_HIGH] = (uint8_t)(addr >> 8u);
    writeBuf[DBC_SPI_HDR_BYTE_ADDR_LOW]  = (uint8_t)addr;
    writeBuf[DBC_SPI_HDR_BYTE_DATA_LEN]  = (uint8_t)(len >> 2u); // data length must be given in number of words (1 word = 4 bytes)
}

static void DBCDRV_uint32ToBuffer(uint32_t value, uint8_t *buff)
{
    buff[3u] = (uint8_t)value;
    buff[2u] = (uint8_t)((value >>  8u) & 0xFFu);
    buff[1u] = (uint8_t)((value >> 16u) & 0xFFu);
    buff[0u] = (uint8_t)((value >> 24u) & 0xFFu);
}

static uint32_t DBCDRV_bufferToUint32(const uint8_t *buff)
{
    uint32_t temp = 0u;
    temp |= (uint32_t)buff[3u];
    temp |= (uint32_t)buff[2u] << 8u;
    temp |= (uint32_t)buff[1u] << 16u;
    temp |= (uint32_t)buff[0u] << 24u;
    return temp;
}

/** \brief   Configures the DBusCAN chip according to the configuration structure settings.
 *
 *  \param   cfg: configuration structure
 *
 *  \note    DBusCAN chip must be in STANDBY power mode when using this function.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_configure(DBC_Cfg_t cfg)
{
    uint32_t bitMask = 0u;
    uint32_t regVal;
    DBC_Cfg_t currCfg;

    currCfg.word = DBCDRV_readReg32(DBC_EEPP_ADDR); // get current configuration of the chip

    if (currCfg.word != cfg.word)
    {
        // configuration of general purpose memory bits
        if (currCfg.GP_MEM != cfg.GP_MEM)
        {
            regVal = currCfg.word;
            UTI_RES(regVal, DBC_EEP_GP_MEM_MASK);
            UTI_SET(regVal, (cfg.word & DBC_EEP_GP_MEM_MASK));
            DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_EEPP_ADDR, regVal));
        }

        // configuration of DBus register bits
        if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_MASK))
        {
            DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(1uL, DBC_IPEC_DBUS_EN_POS, DBC_IPEC_DBUS_EN_MASK)); // Enable DBus part
            DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus()); // unlock DBus regs for write access
            // configuration of DBUS_DPC register bits
            if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_DPC_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_DPC_MASK))
            {
                regVal = DBCDRV_readReg32(DBC_DBUS_DPC_ADDR);
                UTI_RES(regVal, DBC_DBUS_DPC_ADV_PWR_MGMT_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.ADV_PWR_MGMT << DBC_DBUS_DPC_ADV_PWR_MGMT_POS));
                UTI_RES(regVal, DBC_DBUS_DPC_BVD_WAIT_EN_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.BVD_WAIT_EN << DBC_DBUS_DPC_BVD_WAIT_EN_POS));
                UTI_RES(regVal, DBC_DBUS_DPC_BVD_THLD_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.BVD_THLD << DBC_DBUS_DPC_BVD_THLD_POS));
                UTI_RES(regVal, DBC_DBUS_DPC_BVD_TO_NWKRQ_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.BVD_TO_NWKRQ << DBC_DBUS_DPC_BVD_TO_NWKRQ_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DPC_ADDR, regVal));
            }
            // configuration of DBUS_SIDFC register bits
            if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_SIDFC_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_SIDFC_MASK))
            {
                regVal = DBCDRV_readReg32(DBC_DBUS_SIDFC_ADDR);
                UTI_RES(regVal, DBC_DBUS_SIDFC_SID_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.SUBNODE_ID));
                UTI_RES(regVal, DBC_DBUS_SIDFC_PID_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.NODE_ID << DBC_DBUS_SIDFC_PID_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_SIDFC_ADDR, regVal));
            }
            // configuration of DBUS_DBR register bits
            if ((currCfg.word & DBC_EEP_BITS_REG_DBUS_DBR_MASK) != (cfg.word & DBC_EEP_BITS_REG_DBUS_DBR_MASK))
            {
                regVal = DBCDRV_readReg32(DBC_DBUS_DBR_ADDR);
                UTI_RES(regVal, DBC_DBUS_DBR_DBR_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.DBR));
                UTI_RES(regVal, DBC_DBUS_DBR_CLKIN_MASK);
                UTI_SET(regVal, ((uint32_t)cfg.CLKIN << DBC_DBUS_DBR_CLKIN_POS));
                DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DBR_ADDR, regVal));
            }
            DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus()); // lock DBus regs for write access
        }

        // configuration of CAN register bits
        if ((currCfg.word & DBC_EEP_BITS_REG_CAN_MASK) != (cfg.word & DBC_EEP_BITS_REG_CAN_MASK))
        {
            //(void)DBCDRV_writeRegIpec(1uL, DBC_IPEC_MCAN_EN_POS, DBC_IPEC_MCAN_EN_MASK); // Enable CAN part
            // todo
        }

        // configuration of IPEC register bits
        if ((currCfg.word & DBC_EEP_BITS_REG_IPEC_MASK) != (cfg.word & DBC_EEP_BITS_REG_IPEC_MASK))
        {
            regVal = 0u;
            bitMask |= DBC_IPEC_MCAN_EN_MASK;
            regVal  |= (uint32_t)cfg.MCAN_EN << DBC_IPEC_MCAN_EN_POS;
            bitMask |= DBC_IPEC_DBUS_EN_MASK;
            regVal  |= (uint32_t)cfg.DBUS_EN << DBC_IPEC_DBUS_EN_POS;
            bitMask |= DBC_IPEC_DBUS2CAN_MASK;
            regVal  |= (uint32_t)cfg.DBUS2CAN << DBC_IPEC_DBUS2CAN_POS;
            bitMask |= DBC_IPEC_CAN_BIAS_MASK;
            regVal  |= (uint32_t)cfg.CAN_BIAS << DBC_IPEC_CAN_BIAS_POS;
            DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(regVal, 0uL, bitMask));
        }

        if (DBCDRV_readReg32(DBC_EEPP_ADDR) != cfg.word)
        {
            return DBC_ERROR;
        }
    }
    return DBC_OK;
}


/** \brief   Performs non-blocking write to the EEPROM memory of the DBusCAN chip.
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_writeEeprom(void)
{
    uint32_t regVal;
    uint32_t eepPending;
    const uint32_t eepWriteKey = DBC_IPEC_EP_WR_MASK | ((uint32_t)DBC_IPEC_EP_CC_VAL << DBC_IPEC_EP_CC_POS);
    static enum DBCDRV_EepWriteState writeState = DBCDRV_EEP_WRITE_START;

    switch (writeState)
    {
        case DBCDRV_EEP_WRITE_START:
        {
            if (DBCDRV_isPwrOnReset)
            {
                return DBC_ERROR;
            }
            eepPending = DBCDRV_readReg32(DBC_EEPP_ADDR);
            if (DBCDRV_readReg32(DBC_EEPC_ADDR) == eepPending)
            {
                return DBC_OK; // nothing to do, all EEPROM bits are set as expected
            }
            if (true != DBCDRV_isSupplyForEepromWrite())
            {   // insufficient supply- not possible to flash EEPROM
                DBCDRV_eepromWriteEnable = false;
                return DBC_ERROR;
            }
            regVal = DBCDRV_readReg32(DBC_IPEC_ADDR);
            if (DBC_IPEC_EP_WR_MASK == (regVal & DBC_IPEC_EP_WR_MASK)) // check if there isn't ongoing write process to EEPROM
            {
                return DBC_ERROR;
            }
            // start EEPROM write process
            DBC_RETURN_ON_ERROR(DBCDRV_writeRegIpec(eepWriteKey, 0uL, DBC_IPEC_EP_CC_MASK));
            writeState = DBCDRV_EEP_WRITE_ONGOING;
            break;
        }
        case DBCDRV_EEP_WRITE_ONGOING:
        {
            regVal  = DBCDRV_readReg32(DBC_IPEC_ADDR);
            regVal &= DBC_IPEC_EP_WR_MASK;
            writeState = (DBC_IPEC_EP_WR_MASK == regVal) ? DBCDRV_EEP_WRITE_ONGOING : DBCDRV_EEP_WRITE_END; // wait until EEPROM write process is finished
            break;
        }
        case DBCDRV_EEP_WRITE_END:
        {
            eepPending = DBCDRV_readReg32(DBC_EEPP_ADDR);
            if (DBCDRV_readReg32(DBC_EEPC_ADDR) != eepPending)
            {   // try to write EEPROM again
                writeState = DBCDRV_EEP_WRITE_START;
            }
            else
            {   // write to EEPROM has been successful
                return DBC_OK;
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return DBC_ERROR;
}


/** \brief   Checks if there is enough voltage level (on VCC pin) to flash EEPROM of the DBusCAN chip.
 *
 *  \return  bool
 */
static bool DBCDRV_isSupplyForEepromWrite(void)
{
    bool isVcc;
    uint32_t regVal = DBCDRV_readReg32(DBC_IPEC_ADDR);
    bool isCanEnabled = (DBC_IPEC_MCAN_EN_MASK == (regVal & DBC_IPEC_MCAN_EN_MASK)) ? true : false;
    bool isCanBiasSet = (DBC_IPEC_CAN_BIAS_MASK == (regVal & DBC_IPEC_CAN_BIAS_MASK)) ? true : false;

    if (!isCanEnabled || !isCanBiasSet)
    {
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY);
        if (!isCanEnabled)
        {   // enable CAN to see if UVCC flag is set
            (void)DBCDRV_writeRegIpec(1uL, DBC_IPEC_MCAN_EN_POS, DBC_IPEC_MCAN_EN_MASK);
        }
        if (!isCanBiasSet)
        {   // set CAN_BIAS (which lowers VCC undervoltage level) to see if UVCC flag is still set
            (void)DBCDRV_writeRegIpec(1uL, DBC_IPEC_CAN_BIAS_POS, DBC_IPEC_CAN_BIAS_MASK);
        }
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL);
    }

    (void)DBCDRV_writeReg32(DBC_IF_ADDR, DBC_IF_UVCC_MASK);
    regVal = DBCDRV_readReg32(DBC_IF_ADDR) & DBC_IF_UVCC_MASK;
    isVcc = (DBC_IF_UVCC_MASK == regVal) ? false : true;

    // set back CAN_BIAS and MCAN_EN bits
    if (!isCanEnabled || !isCanBiasSet)
    {
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_STANDBY);
        if (!isCanEnabled)
        {   // disable CAN
            (void)DBCDRV_writeRegIpec(0uL, DBC_IPEC_MCAN_EN_POS, DBC_IPEC_MCAN_EN_MASK);
        }
        if (!isCanBiasSet)
        {   // reset CAN_BIAS bit
            (void)DBCDRV_writeRegIpec(0uL, DBC_IPEC_CAN_BIAS_POS, DBC_IPEC_CAN_BIAS_MASK);
        }
        (void)DBCDRV_setPowerMode(DBC_POWER_MODE_NORMAL);
    }

    return isVcc;
}

/** \brief   Sets DBus baudrate value from the enum DBC_DbusBaudrate in the DBusCAN chip.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_setTableDbusBaudrate(uint16_t baudrate)
{
    uint32_t regVal;

    baudrate = DBCDRV_convertDbusBaudValue(baudrate);

    if (baudrate <= (uint16_t)DBC_DBUS_BAUD_1M)
    {
        regVal = DBCDRV_readReg32(DBC_DBUS_DBR_ADDR);
        if ((regVal & DBC_DBUS_DBR_DBR_MASK) != (uint32_t)baudrate)
        {
            DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());
            // change baudrate value
            regVal &= ~DBC_DBUS_DBR_DBR_MASK;
            regVal |= (uint32_t)baudrate;
            DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DBR_ADDR, regVal));
            DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());
        }
        return DBC_OK;
    }
    else
    {
        return DBC_ERROR;
    }
}


#ifdef DBC_ANY_DBUS_BAUDRATE_ENABLED
/** \brief   Sets any (possible) DBus baudrate value in the DBusCAN chip.
 *
 *  \param   baudrate: DBus baudrate value in hectobauds
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_setAnyDbusBaudrate(uint16_t baudrate)
{
    const uint8_t OVERSAMPLING = 16u;
    const uint32_t INT_DIV_MAX = (DBC_DBUS_DBR_CB_INT_MASK >> DBC_DBUS_DBR_CB_INT_POS);
    const uint32_t FRAC_DIV_RESOLUTION = (DBC_DBUS_DBR_CB_FRAC_MASK >> DBC_DBUS_DBR_CB_FRAC_POS) + 1u;
    uint32_t regVal = DBCDRV_readReg32(DBC_DBUS_DBR_ADDR);
    uint32_t clkin  = DBCDRV_getClockInputInHz((regVal & DBC_DBUS_DBR_CLKIN_MASK) >> DBC_DBUS_DBR_CLKIN_POS);
    struct MTDIV_Result divResult = MTDIV_div_mod(clkin, (INT_DIV_MAX * OVERSAMPLING));
    uint32_t minBaudrate = (0u != divResult.remainder) ? (divResult.quotient + 1u) : divResult.quotient;
    uint32_t maxBaudrate = MTDIV_div_32_32(clkin, (uint32_t)OVERSAMPLING);
    uint32_t neededBaudrate = 100uL * baudrate; // convert baudrate from hectobauds to Hz

    if ((neededBaudrate >= minBaudrate) && (neededBaudrate <= maxBaudrate))
    {
        divResult = MTDIV_div_mod(clkin, (neededBaudrate * OVERSAMPLING));
        uint32_t intDiv = divResult.quotient;
        uint32_t fracDiv = 0u;
        if (0u != divResult.remainder)
        {
            // formula: fracDiv = (FRAC_DIV_RESOLUTION * clkin / OVERSAMPLING / neededBaudrate) - (intDiv * FRAC_DIV_RESOLUTION)
            fracDiv = MTDIV_div_32_32(clkin, neededBaudrate) * FRAC_DIV_RESOLUTION; // partial division before multiplication to avoid overflowing uint32_t
            fracDiv = MTDIV_div_32_32(fracDiv, (uint32_t)OVERSAMPLING) - (intDiv * FRAC_DIV_RESOLUTION);
        }
        DBC_RETURN_ON_ERROR(DBCDRV_enableCfgDbus());
        // set custom baudrate value
        regVal &= ~(DBC_DBUS_DBR_CB_INT_MASK | DBC_DBUS_DBR_CB_FRAC_MASK);
        regVal |= intDiv << DBC_DBUS_DBR_CB_INT_POS;
        regVal |= fracDiv << DBC_DBUS_DBR_CB_FRAC_POS;
        regVal |= DBC_DBUS_DBR_CB_EN_MASK;
        DBC_RETURN_ON_ERROR(DBCDRV_writeReg32(DBC_DBUS_DBR_ADDR, regVal));
        DBC_RETURN_ON_ERROR(DBCDRV_disableCfgDbus());
        return DBC_OK;
    }
    else
    {
        return DBC_ERROR;
    }
}


static uint32_t DBCDRV_getClockInputInHz(uint32_t clkinBitfield)
{
    uint32_t clkinHz = 0u;
    switch (clkinBitfield)
    {
        case (uint32_t)DBC_CLKIN_20M: clkinHz = 20000000u;  break;
        case (uint32_t)DBC_CLKIN_40M: clkinHz = 40000000u;  break;
        case (uint32_t)DBC_CLKIN_80M: clkinHz = 80000000u;  break;
        default:                                            break;
    }
    return clkinHz;
}
#endif //DBC_ANY_DBUS_BAUDRATE_ENABLED


/** \brief   Sends SPI frame to the DBusCAN chip.
 *
 *  \param   writeBuf:   pointer to buffer with data to be written
 *  \param   writeLen:   number of bytes to be written
 *  \param   readBuf:    pointer to buffer for storing read data
 *  \param   readLen:    number of bytes to be read
 *
 *  \return  enum DBC_Error
 */
static enum DBC_Error DBCDRV_sendSpiFrame(const uint8_t *writeBuf, uint16_t writeLen, uint8_t *readBuf, uint16_t readLen)
{
#if defined(VARIANT_PROGRAMMER) || defined(APP_VARIANT)
    struct MSUP_TimeoutHandle DBCDRV_spiTimeoutHandle;
    if(MCAL_OK == MSPI_transfer(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen))
    {
        bool isTimeout;
        MSUP_setTimeOutExt(&DBCDRV_spiTimeoutHandle, MSPI_TIMEOUT_VALUE);
        do
        {
            isTimeout = MSUP_isTimeOutExt(&DBCDRV_spiTimeoutHandle);
        } while((DBCDRV_mspiHandle.status != MCAL_STATUS_READY) && !isTimeout);
        if(!isTimeout)
        {
            return DBC_OK;
        }
        MSPI_endCommunication(&DBCDRV_mspiHandle);
    }
    return DBC_ERROR;
#else
      // bootloader variants do not use interrupts
    return (MCAL_OK != MSPI_transferBlocking(&DBCDRV_mspiHandle, writeBuf, writeLen, readBuf, readLen)) ? DBC_ERROR : DBC_OK;
#endif // VARIANT_PROGRAMMER || APP_VARIANT
}
