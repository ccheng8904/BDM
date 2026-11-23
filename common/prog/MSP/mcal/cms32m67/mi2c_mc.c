/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MI2C
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mi2c_mc.c
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MI2C.
 *
 *  \details  The CMS32M67xx I2C unit (IICA0) is controlled by the vendor
 *            driver (I2CA_...). The MCAL register style interface is mapped
 *            onto this driver, the events are derived from the status of the
 *            IICA shift register (ACKD, STD/SPD flags).
 *
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "math/mathDiv.h"
#include "mi2c.h"
#include "hsup.h"
#include "utility.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif

#ifdef MCAL_MI2C_NDEBUG
    #define NDEBUG
#endif
#include "mcal_assert.h"

/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/

#define BAUDRATE_STANDARD_HZ          100000U
#define BAUDRATE_FAST_HZ              400000U

#define I2CCLK_MIN                    1600000U          /*!< i2cclk minimum value */
#define I2CCLK_MAX                    24000000U         /*!< i2cclk maximum value */

/* registers are 8 bit wide in the CMS32M67xx IICA block */
#define MI2C_ACKNOWLEDGE_ENABLE       0x04U             /*!< IICCTL00.ACKE */

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void MI2C_enableClock(const struct MI2C_Channel *channel);
static void MI2C_clockConfigure(const struct MI2C_Channel *channel, uint32_t clkSpeedHz);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

#if defined(MI2C0_PRESENT)
const struct MI2C_Periph MI2C0 =
{
    I2C0,
    MI2C0_IRQ_INDEX,
    I2C0_IRQn
};
#endif

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static volatile uint16_t MI2C_DataLength = 0U;
static volatile bool     MI2C_isReading = false;
static volatile bool     MI2C_lastByteNack = false;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void MI2C_configure(const struct MI2C_Channel *channel, const struct MI2C_Config *cfg)
{
    uint32_t clkSpeedHz;

    MI2C_enableClock(channel);
    MI2C_disableChannel(channel);

    if(cfg->baudrate == MI2C_BAUDRATE_FAST)
    {
        clkSpeedHz = BAUDRATE_FAST_HZ;
    }
    else
    {
        /* fast plus is not supported, standard is used */
        clkSpeedHz = BAUDRATE_STANDARD_HZ;
    }

    MI2C_clockConfigure(channel, clkSpeedHz);
    MI2C_initPort(channel, true);
}




void MI2C_setBaudrate(const struct MI2C_Channel *channel, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode)
{
    uint32_t clkSpeedHz;

    (void)mode;

    MI2C_disableChannel(channel);

    if(baudRate == MI2C_BAUDRATE_FAST)
    {
        clkSpeedHz = BAUDRATE_FAST_HZ;
    }
    else
    {
        clkSpeedHz = BAUDRATE_STANDARD_HZ;
    }

    MI2C_clockConfigure(channel, clkSpeedHz);
    MI2C_enableChannel(channel);
}


void MI2C_enableChannel(const struct MI2C_Channel *channel)
{
    I2CA_Cmd(ENABLE);
}


void MI2C_disableChannel(const struct MI2C_Channel *channel)
{
    I2CA_Cmd(DISABLE);
}


/******************************************************************************/
/* Functions for hardware created I2C */
/******************************************************************************/

enum MCAL_Error MI2C_makeStartCondition(const struct MI2C_Channel *channel)
{
    I2CA_GenerateSTART();

    return MCAL_OK;
}


enum MCAL_Error MI2C_sendStop(const struct MI2C_Channel *channel)
{
    MI2C_sendStopReg(channel);

    return MCAL_OK;
}


void MI2C_beforeCommunication(const struct MI2C_Channel *channel, uint16_t baseAddress, bool isReading, enum MI2C_BaseAddressLength addressLength, uint16_t dataLength)
{
    (void)baseAddress;
    (void)addressLength;

    MI2C_DataLength   = dataLength;
    MI2C_isReading    = isReading;
    MI2C_lastByteNack = false;

    /* the acknowledge of the last received byte is handled by the ISR */
    I2CA_Acknowledge_Cmd(ENABLE);
}


void MI2C_afterCommunication(const struct MI2C_Channel *channel)
{
    MI2C_DataLength = 0U;
    MI2C_isReading  = false;
}


void MI2C_sendStopReg(const struct MI2C_Channel *channel)
{
    (void)*channel;

    I2CA_GenerateSTOP();
}


void MI2C_sendAckReg(const struct MI2C_Channel *channel)
{
    bool ack = true;

    (void)*channel;

    if((MI2C_DataLength <= 1U) && MI2C_isReading)
    {
        /* no acknowledge before the last received byte */
        ack = false;
        MI2C_lastByteNack = true;
    }

    I2CA_Acknowledge_Cmd((ack) ? ENABLE : DISABLE);
}


void MI2C_sendNackReg(const struct MI2C_Channel *channel)
{
    (void)*channel;

    I2CA_Acknowledge_Cmd(DISABLE);
    MI2C_lastByteNack = true;
}


void MI2C_clearAckFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;
}


void MI2C_clearNackFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;

    MI2C_lastByteNack = false;
}


void MI2C_clearStopFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;
}


void MI2C_clearRxFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;
}


void MI2C_setReceiveMode(const struct MI2C_Channel *channel)
{
    (void)*channel;
}


void MI2C_beforeReadByte(const struct MI2C_Channel *channel)
{
    bool ack = true;

    (void)*channel;

    if(MI2C_DataLength <= 1U)
    {
        /* the last byte is followed by a NACK and a STOP condition */
        ack = false;
    }

    I2CA_Acknowledge_Cmd((ack) ? ENABLE : DISABLE);
}


uint8_t MI2C_readDataReg(const struct MI2C_Channel *channel)
{
    uint8_t data;

    (void)*channel;

    data = I2CA_ReadByte();

    if(MI2C_DataLength > 0U)
    {
        MI2C_DataLength--;
    }

    return data;
}


void MI2C_writeDataReg(const struct MI2C_Channel *channel, uint32_t data)
{
    (void)*channel;

    I2CA_WriteByte((uint8_t)data);
}


uint32_t MI2C_getStatusReg(const struct MI2C_Channel *channel)
{
    uint32_t status = 0u;

    (void)*channel;

    if((I2C->IICA0 & 0x01U) != 0u)  /* ACKD flag */
    {
        if(MI2C_lastByteNack)
        {
            status |= MI2C_STATUS_NACK;
        }
        else
        {
            status |= MI2C_STATUS_ACK;
        }
    }

    if((I2C->IICS & I2C_IICS0_STD_Msk) != 0u)
    {
        status |= MI2C_STATUS_START;
    }

    if((I2C->IICS & I2C_IICS0_SPD_Msk) != 0u)
    {
        status |= MI2C_STATUS_STOP;
    }

    if((I2C->IICS & I2C_IICS0_ACKD_Msk) != 0u)
    {
        status |= MI2C_STATUS_RX;
    }

    return status;
}


uint32_t MI2C_getStatusRegMaskForEvent(const struct MI2C_Channel *channel, enum MCAL_Event event)
{
    (void)channel;

    if(event == MCAL_EVENT_ACKNOWLEDGE)
    {
        return MI2C_STATUS_ACK;
    }
    else if(event == MCAL_EVENT_NOT_ACKNOWLEDGE)
    {
        return MI2C_STATUS_NACK;
    }
    else if(event == MCAL_EVENT_STOP)
    {
        return MI2C_STATUS_STOP;
    }
    else if(event == MCAL_EVENT_READ_BYTE)
    {
        return MI2C_STATUS_RX;
    }
    else if(event == MCAL_EVENT_ERROR)
    {
        return MI2C_STATUS_ERROR;
    }
    else
    {
        return 0u;
    }
}


enum MCAL_Error MI2C_getChannelError(const struct MI2C_Channel *channel)
{
    (void)*channel;

    return MCAL_OK;
}


void MI2C_clearErrorReg(const struct MI2C_Channel *channel)
{
    (void)*channel;
}


enum MI2C_unitIndex MI2C_IRQ_getIndex(const struct MI2C_Channel *channel)
{
    return channel->mi2c->index;
}


void MI2C_IRQ_enable(const struct MI2C_Channel *channel)
{
    NVIC_EnableIRQ(channel->mi2c->IRQn);
}


void MI2C_IRQ_enableEvent(const struct MI2C_Channel *channel, enum MCAL_Event event)
{
    (void)*channel;
    (void)event;

    /* the IICA0 interrupt is a single interrupt source */
    I2CA_InterruptPosition_Config(0x01U);
}


void MI2C_IRQ_disableEvent(const struct MI2C_Channel *channel, enum MCAL_Event event)
{
    (void)*channel;
    (void)event;
}


void MI2C_output(const struct MDIO_Channel *channel)
{
    (void)MDIO_init(channel, &MDIO_OUTPUT_OPEN_DRAIN);
}


void MI2C_input(const struct MDIO_Channel *channel)
{
    (void)MDIO_init(channel, &MDIO_INPUT_PULL_UP);
}


void MI2C_initPort(const struct MI2C_Channel *channel, bool internalPullUps)
{
    const struct MDIO_Config cfgWithPullUp = MDIO_CFG_ALTERNATE_FUNCTION_PULL_UP;
    const struct MDIO_Config cfgWithoutPullUp = MDIO_CFG_ALTERNATE_FUNCTION;
    const struct MDIO_Config *cfg = (internalPullUps) ? &cfgWithPullUp : &cfgWithoutPullUp;

    if(channel->sdaPin != NULL)
    {
        (void)MDIO_init(channel->sdaPin, cfg);
    }
    if(channel->sclPin != NULL)
    {
        (void)MDIO_init(channel->sclPin, cfg);
    }
}


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void MI2C_enableClock(const struct MI2C_Channel *channel)
{
    (void)*channel;

    CGC_PER0PeriphClockCmd(CGC_PER0Periph_IIC, ENABLE);
}


static void MI2C_clockConfigure(const struct MI2C_Channel *channel, uint32_t clkSpeedHz)
{
    I2CA_InitTypeDef i2cInit;

    (void)*channel;

    CGC_PER0PeriphClockCmd(CGC_PER0Periph_IIC, ENABLE);


    /* the vendor driver derives the IICA clock dividers from the IICA clock
     * and the requested SCL frequency */
    i2cInit.I2C_ClockSpeed  = clkSpeedHz;
    i2cInit.I2C_Mode        = I2CA_Mode_SMBusMaster;
    i2cInit.I2C_DutyCycle   = 0u;
    i2cInit.I2C_OwnAddress  = 0u;
    i2cInit.I2C_Ack         = I2CA_Ack_Enable;

    I2CA_Init(&i2cInit);
}



/******************************************************************************/
/* VENDOR DRIVER EXTENSIONS                                                   */
/******************************************************************************/
/* The address byte of the transfer is written to the IICA shift register by the
 * generic MI2C implementation, the vendor driver provides only the byte write
 * function. */
enum MCAL_Error MI2C_sendAddress(const struct MI2C_Channel *channel, uint16_t baseAddress, enum MI2C_BaseAddressLength addressLength)
{
    enum MCAL_Error retVal = MCAL_OK;

    (void)*channel;

    if(addressLength == MI2C_ADDRESS_LENGTH_7_BITS)
    {
        I2CA_WriteByte((uint8_t)baseAddress);
    }
    else
    {
        /* 10 bit addressing is not supported by the CMS32M67xx IICA unit */
        retVal = MCAL_ERROR_PRECISION;
    }

    return retVal;
}