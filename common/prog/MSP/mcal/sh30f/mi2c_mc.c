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
 *  PROCESSOR        SH30F
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

#define CLOCK_1MHZ                    1000000U //lint !e750  macro not used
#define I2CCLK_MIN                    1600000U          /*!< i2cclk minimum value */
#define I2CCLK_MAX                    24000000U         /*!< i2cclk maximum value */

/* TWI transmit event type */
#define REMAPPING_STATE_08H           ((uint32_t)0x1U << 0)       /*!< TWI_MASTER_START_SEND*/
#define REMAPPING_STATE_10H           ((uint32_t)0x1U << 1)       /*!< TWI_MASTER_RESTART_SEND*/
#define REMAPPING_STATE_18H           ((uint32_t)0x1U << 2)       /*!< TWI_MASTER_SLA_W_ACK*/
#define REMAPPING_STATE_20H           ((uint32_t)0x1U << 3)       /*!< TWI_MASTER_SLA_W_NACK*/
#define REMAPPING_STATE_28H           ((uint32_t)0x1U << 4)       /*!< TWI_MASTER_DATA_SEND_ACK*/
#define REMAPPING_STATE_30H           ((uint32_t)0x1U << 5)       /*!< TWI_MASTER_DATA_SEND_NACK*/
#define REMAPPING_STATE_38H           ((uint32_t)0x1U << 6)       /*!< TWI_MASTER_LOST_ARBITRATION*/
#define REMAPPING_STATE_40H           ((uint32_t)0x1U << 7)       /*!< TWI_MASTER_SLA_R_ACK*/
#define REMAPPING_STATE_48H           ((uint32_t)0x1U << 8)       /*!< TWI_MASTER_SLA_R_NACK*/
#define REMAPPING_STATE_50H           ((uint32_t)0x1U << 9)       /*!< TWI_MASTER_DATA_RECEIVE_ACK*/
#define REMAPPING_STATE_58H           ((uint32_t)0x1U << 10)      /*!< TWI_MASTER_DATA_RECEIVE_NACK*/
#define REMAPPING_STATE_F8H           ((uint32_t)0x1U << 11)      /*!< TWI_NO_VALID_EVENT*/

/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

static void MI2C_enableClock(const struct MI2C_Channel *channel);
static void MI2C_clockConfigure(I2C_TypeDef *i2c_periph, uint32_t clkSpeed);
static void MI2C_ackConfigure(I2C_TypeDef *i2c_periph, TWI_AA_Type newAck);
static uint8_t MI2C_sendWaitDone(I2C_TypeDef *i2c_periph);

/******************************************************************************/
/* PUBLIC DATA DEFINITIONS                                                    */
/******************************************************************************/

#if defined(MI2C0_PRESENT)
const struct MI2C_Periph MI2C0 =
{
    I2C0,
    MI2C0_IRQ_INDEX,
    TWI0_IRQn,
    {
        &RCC->APB1ENR.V32,
        RCC_APB1ENR_TWI0EN_Msk
    }
};
#endif

/******************************************************************************/
/* PRIVATE DATA DEFINITIONS                                                   */
/******************************************************************************/

static __IO uint16_t MI2C_DataLength = 0U;

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

void MI2C_configure(const struct MI2C_Channel *channel, const struct MI2C_Config *cfg)
{
    uint32_t clkSpeedHz;
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* enable peripheral clock to I2C */
    MI2C_enableClock(channel);

    /* disable peripheral */
    MI2C_disableChannel(channel);

    if(cfg->baudrate == MI2C_BAUDRATE_STANDARD)
    {
        clkSpeedHz = BAUDRATE_STANDARD_HZ;
    }
    else if(cfg->baudrate == MI2C_BAUDRATE_FAST)
    {
        clkSpeedHz = BAUDRATE_FAST_HZ;
    }
    else
    {
        /* Not support MI2C_BAUDRATE_FAST_PLUS, use MI2C_BAUDRATE_STANDARD */
        clkSpeedHz = BAUDRATE_STANDARD_HZ;
    }

    /* set timing parameters */
    MI2C_clockConfigure(mi2cReg, clkSpeedHz);

    /* enable peripheral */
    MI2C_enableChannel(channel);
}


void MI2C_setBaudrate(const struct MI2C_Channel *channel, enum MI2C_Baudrate baudRate, enum MI2C_OperationMode mode)
{
    (void)mode;
    uint32_t clkSpeedHz;
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* disable peripheral */
    MI2C_disableChannel(channel);

    if(baudRate == MI2C_BAUDRATE_STANDARD)
    {
        clkSpeedHz = BAUDRATE_STANDARD_HZ;
    }
    else if(baudRate == MI2C_BAUDRATE_FAST)
    {
        clkSpeedHz = BAUDRATE_FAST_HZ;
    }
    else
    {
        /* Not support MI2C_BAUDRATE_FAST_PLUS, use MI2C_BAUDRATE_STANDARD */
        clkSpeedHz = BAUDRATE_STANDARD_HZ;
    }

    /* set timing parameters */
    MI2C_clockConfigure(mi2cReg, clkSpeedHz);

    /* enable peripheral */
    MI2C_enableChannel(channel);
}


void MI2C_enableChannel(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* enable peripheral */
    mi2cReg->CR.BIT.TWIEN = (uint32_t)SH_ON;
}


void MI2C_disableChannel(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* disable peripheral */
    mi2cReg->CR.BIT.TWIEN = (uint32_t)SH_OFF;
}


/******************************************************************************/
/* Functions for hardware created I2C */
/******************************************************************************/

enum MCAL_Error MI2C_makeStartCondition(const struct MI2C_Channel *channel)
{
    uint8_t event;
    enum MCAL_Error retVal = MCAL_OK;
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;
    
    /* Generate start condition */
    mi2cReg->CR.BIT.STA = (uint32_t)SH_ENABLE;

    event = MI2C_sendWaitDone(mi2cReg);
    if ((event != (uint8_t)TWI_MASTER_START_SEND) && (event != (uint8_t)TWI_MASTER_RESTART_SEND))
    {
        retVal = MCAL_BUS_ERROR;
    }

    return retVal;
}


enum MCAL_Error MI2C_sendAddress(const struct MI2C_Channel *channel, uint16_t baseAddress, enum MI2C_BaseAddressLength addressLength)
{
    uint8_t event;
    enum MCAL_Error retVal = MCAL_OK;    
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;    

    mi2cReg->CR.BIT.STO = (uint32_t)SH_DISABLE; //do not send end signal

    if(addressLength == MI2C_ADDRESS_LENGTH_7_BITS)
    {
        mi2cReg->DR = (uint8_t)baseAddress;

        event = MI2C_sendWaitDone(mi2cReg);
        if ((event != (uint8_t)TWI_MASTER_SLA_W_ACK) && (event != (uint8_t)TWI_MASTER_SLA_R_ACK))
        {
            retVal = MCAL_BUS_ERROR;
        }
    }
    else
    {
        /* Nothing to do, not support MI2C_ADDRESS_LENGTH_10_BITS*/
        retVal = MCAL_ERROR_PRECISION;
    }    

    return retVal;
}


enum MCAL_Error MI2C_sendDummyData(const struct MI2C_Channel *channel)
{
    uint8_t event;
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;
    enum MCAL_Error retVal = MCAL_OK;

    mi2cReg->DR = 0x000000FFu;  /* sent 8 clocks on SCL and dummy data to SDA */

    event = MI2C_sendWaitDone(mi2cReg);
    if ((event != (uint8_t)TWI_MASTER_SLA_R_ACK) && (event != (uint8_t)TWI_MASTER_SLA_R_NACK))
    {
        retVal = MCAL_BUS_ERROR;
    }

    return retVal;
}


enum MCAL_Error MI2C_sendStop(const struct MI2C_Channel *channel)
{
    MI2C_sendStopReg(channel);

    return MCAL_OK;
}


enum MCAL_Error MI2C_quickCommand(const struct MI2C_Channel *channel, uint16_t baseAddress, enum MI2C_BaseAddressLength addressLength)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;
    enum MCAL_Error retVal;

    /* disable necessary interrupts */
    mi2cReg->CR.BIT.EFREE  = (uint32_t)SH_DISABLE;
    mi2cReg->CR.BIT.ETOT   = (uint32_t)SH_DISABLE;
    mi2cReg->CR.BIT.TWINTE = (uint32_t)SH_DISABLE;

    retVal = MI2C_makeStartCondition(channel);
    if (retVal == MCAL_OK)
    {
        retVal = MI2C_sendAddress(channel, baseAddress, addressLength);
        if (retVal == MCAL_OK)
        {
            retVal = MI2C_sendStop(channel);
        }
        else
        {
            (void)MI2C_sendStop(channel);
        }
    }
    return retVal;
}


/******************************************************************************/
/* End of functions for hardware created I2C */
/******************************************************************************/

void MI2C_sendStopReg(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* Generate stop condition */
    mi2cReg->CR.BIT.STO = (uint32_t)SH_ENABLE;

    /* clear Interrupt Flag to start transmit */
    TWI_ClearFlag(mi2cReg, TWI_FR_TWINT_Msk | TWI_FR_TFREE_Msk | TWI_FR_TOUT_Msk);
}


void MI2C_sendAckReg(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    if(MI2C_DataLength == 1U)
    {
        /* set no acknowledge before receive last byte */
        MI2C_ackConfigure(mi2cReg, TWI_AA_NACK);
    }
    else
    {
        /* set acknowledge */
        MI2C_ackConfigure(mi2cReg, TWI_AA_ACK);
    }
    
    /* clear Interrupt Flag to start transmit */
    TWI_ClearFlag(mi2cReg, TWI_FR_TWINT_Msk | TWI_FR_TFREE_Msk | TWI_FR_TOUT_Msk);
}


void MI2C_sendNackReg(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* set no acknowledge */
    MI2C_ackConfigure(mi2cReg, TWI_AA_NACK);

    /* clear Interrupt Flag to start transmit */
    TWI_ClearFlag(mi2cReg, TWI_FR_TWINT_Msk | TWI_FR_TFREE_Msk | TWI_FR_TOUT_Msk);
}


void MI2C_beforeCommunication(const struct MI2C_Channel *channel, uint16_t baseAddress, bool isReading, enum MI2C_BaseAddressLength addressLength, uint16_t dataLength)
{
    (void)baseAddress;
    (void)isReading;
    (void)addressLength;

    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* disable necessary interrupts */
    mi2cReg->CR.BIT.EFREE  = (uint32_t)SH_DISABLE;
    mi2cReg->CR.BIT.ETOT   = (uint32_t)SH_DISABLE;
    mi2cReg->CR.BIT.TWINTE = (uint32_t)SH_DISABLE;

    MI2C_DataLength = dataLength;
    if(MI2C_DataLength <= 1U)
    {
        /* set not acknowledge before receive last byte */
        MI2C_ackConfigure(mi2cReg, TWI_AA_NACK);
    }
    else
    {
        /* set acknowledge */
        MI2C_ackConfigure(mi2cReg, TWI_AA_ACK);
    }
}


void MI2C_afterCommunication(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    /* enable necessary interrupts */
    mi2cReg->CR.BIT.TWINTE = (uint32_t)SH_ENABLE;
    mi2cReg->CR.BIT.EFREE  = (uint32_t)SH_ENABLE;
    mi2cReg->CR.BIT.ETOT   = (uint32_t)SH_ENABLE;
}


void MI2C_writeDataReg(const struct MI2C_Channel *channel, uint32_t data)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    mi2cReg->DR = (uint8_t)data;

    /* clear Interrupt Flag to start transmit */
    TWI_ClearFlag(mi2cReg, TWI_FR_TWINT_Msk | TWI_FR_TFREE_Msk | TWI_FR_TOUT_Msk);
}


uint8_t MI2C_readDataReg(const struct MI2C_Channel *channel)
{
    uint8_t retVal;

    MI2C_DataLength = MI2C_DataLength - (uint16_t)1;

    retVal = (uint8_t)channel->mi2c->I2Cx->DR;

    return retVal;
}


void MI2C_beforeReadByte(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    mi2cReg->CR.BIT.STA = (uint32_t)SH_DISABLE;     //donnot send start signal
    mi2cReg->CR.BIT.STO = (uint32_t)SH_DISABLE;     //donnot send end signal

    if(MI2C_DataLength == 1U)
    {
        /* set stop condition before receive last byte */
        MI2C_sendStopReg(channel);
    }
}


uint32_t MI2C_getStatusReg(const struct MI2C_Channel *channel)
{
    __IO uint8_t event;
    uint32_t retStatus = 0;

    event = (uint8_t)channel->mi2c->I2Cx->STAT.V32;
    /* Axivion Disable Style MisraC2012-10.5 : Explicit conversion to inappropriate essential type */
    /* Axivion Disable Style MisraC2012-10.3 : Case label of essential type TWI_EVENT_Type does not match switch controlling expression of type uint8_t */
    switch(event)
    {
    case TWI_MASTER_START_SEND:
        retStatus = REMAPPING_STATE_08H;
        break;
    case TWI_MASTER_RESTART_SEND:
        retStatus = REMAPPING_STATE_10H;
        break;
    case TWI_MASTER_SLA_W_ACK:
        retStatus = REMAPPING_STATE_18H;
        break;
    case TWI_MASTER_SLA_W_NACK:
        retStatus = REMAPPING_STATE_20H;
        break;
    case TWI_MASTER_DATA_SEND_ACK:
        retStatus = REMAPPING_STATE_28H;
        break;
    case TWI_MASTER_DATA_SEND_NACK:
        retStatus = REMAPPING_STATE_30H;
        break;
    case TWI_MASTER_LOST_ARBITRATION:
        retStatus = REMAPPING_STATE_38H;
        break;
    case TWI_MASTER_SLA_R_ACK:
        retStatus = REMAPPING_STATE_40H;
        break;
    case TWI_MASTER_SLA_R_NACK:
        retStatus = REMAPPING_STATE_48H;
        break;
    case TWI_MASTER_DATA_RECEIVE_ACK:
        retStatus = REMAPPING_STATE_50H;
        break;
    case TWI_MASTER_DATA_RECEIVE_NACK:
        retStatus = REMAPPING_STATE_58H;
        break;
    case TWI_NO_VALID_EVENT:
        retStatus = REMAPPING_STATE_F8H;
        break;
    default:
        retStatus = 0U;
        break;
    }
    /* Axivion Enable Style MisraC2012-10.5 */ 
    /* Axivion Enable Style MisraC2012-10.3 */ 
    return retStatus;
}


void MI2C_clearErrorReg(const struct MI2C_Channel *channel)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    mi2cReg->FR.V32 = (uint32_t)(TWI_FR_TFREEC_Msk | TWI_FR_TOUTC_Msk);
}


void MI2C_clearAckFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;
    uint32_t retVal;
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    retVal = MI2C_getStatusReg(channel);
    if(retVal == REMAPPING_STATE_40H)
    {
        /* clear Interrupt Flag to start transmit */
        TWI_ClearFlag(mi2cReg, TWI_FR_TWINT_Msk | TWI_FR_TFREE_Msk | TWI_FR_TOUT_Msk);
    }
}


void MI2C_clearNackFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;
    /* nothing to do for this derivative */
}


void MI2C_clearStopFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;
    /* nothing to do for this derivative */
}


void MI2C_clearRxFlag(const struct MI2C_Channel *channel)
{
    (void)*channel;
    /* nothing to do for this derivative */
}


void MI2C_setReceiveMode(const struct MI2C_Channel *channel)
{
    (void)*channel;
    /* nothing to do for this derivative */
}


uint32_t MI2C_getStatusRegMaskForEvent(const struct MI2C_Channel *channel, enum MCAL_Event event)
{
    (void)*channel;
    uint32_t retVal = 0U;

    switch(event)
    {
    case MCAL_EVENT_READ_BYTE:
        retVal = REMAPPING_STATE_50H | REMAPPING_STATE_58H;
        break;
    case MCAL_EVENT_ACKNOWLEDGE:
        retVal = REMAPPING_STATE_18H | REMAPPING_STATE_28H | REMAPPING_STATE_40H;
        break;
    case MCAL_EVENT_NOT_ACKNOWLEDGE:
        retVal = REMAPPING_STATE_20H | REMAPPING_STATE_30H | REMAPPING_STATE_48H;
        break;
    case MCAL_EVENT_ERROR:
        retVal = REMAPPING_STATE_38H;
        break;
    case MCAL_EVENT_START:
        retVal = REMAPPING_STATE_08H;
        break;
    case MCAL_EVENT_WRITE_BYTE:        
    case MCAL_EVENT_STOP:
    default:
        retVal = 0U;
        break;
    }
    return retVal;
}


enum MCAL_Error MI2C_getChannelError(const struct MI2C_Channel *channel)
{
    bool temp1, temp2;
    uint8_t staValue = 0;   
    enum MCAL_Error retVal = MCAL_OK;
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;  

    temp1 = (TWI_GetFlagStatus(mi2cReg, TWI_FLAG_BUS_TIMEOUT) == SH_SET) ? true : false;
    temp2 = (TWI_GetFlagStatus(mi2cReg, TWI_FLAG_SCL_TIMEOUT) == SH_SET) ? true : false;
    if (temp1 || temp2)
    {
        retVal = MCAL_BUS_ERROR;
    }
    else
    {
        staValue = (uint8_t)(mi2cReg->STAT.V32);
        if (staValue == (uint8_t)TWI_MASTER_LOST_ARBITRATION)    //38H
        {
            retVal = MCAL_ERROR_ARLO;
        }
    }

    return retVal;
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
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    switch(event)
    {  
        case MCAL_EVENT_ERROR:
            mi2cReg->CR.BIT.TWINTE = (uint32_t)SH_ENABLE;
            mi2cReg->CR.BIT.EFREE  = (uint32_t)SH_ENABLE;
            mi2cReg->CR.BIT.ETOT   = (uint32_t)SH_ENABLE;         
            break; 
        case MCAL_EVENT_READ_BYTE:
        case MCAL_EVENT_WRITE_BYTE:
        case MCAL_EVENT_ACKNOWLEDGE:
        case MCAL_EVENT_NOT_ACKNOWLEDGE:
        case MCAL_EVENT_START:
        case MCAL_EVENT_STOP: 
        case MCAL_EVENT_READ_COMPLETE :
        case MCAL_EVENT_WRITE_COMPLETE:    
        default:
            /* not a valid event */
            break;
    }
}


void MI2C_IRQ_disableEvent(const struct MI2C_Channel *channel, enum MCAL_Event event)
{
    I2C_TypeDef *mi2cReg = channel->mi2c->I2Cx;

    switch(event)
    {
        case MCAL_EVENT_ERROR:
            mi2cReg->CR.BIT.EFREE  = (uint32_t)SH_DISABLE;
            mi2cReg->CR.BIT.ETOT   = (uint32_t)SH_DISABLE; 
            mi2cReg->CR.BIT.TWINTE = (uint32_t)SH_DISABLE;   
            break;
        case MCAL_EVENT_READ_BYTE:
        case MCAL_EVENT_WRITE_BYTE:
        case MCAL_EVENT_ACKNOWLEDGE:
        case MCAL_EVENT_NOT_ACKNOWLEDGE:
        case MCAL_EVENT_START:
        case MCAL_EVENT_STOP: 
        case MCAL_EVENT_READ_COMPLETE :
        case MCAL_EVENT_WRITE_COMPLETE:
        default:
            /* not a valid event */
            break;
    }
}


/* functions for software created I2C */
void MI2C_initPort(const struct MI2C_Channel *channel, bool internalPullUps)
{
    struct MDIO_Config mi2cGpioCfg = {MDIO_MODE_DIGITAL_INPUT, (internalPullUps != false) ? MDIO_PULL_UP : MDIO_PULL_DISABLED, MDIO_OUTPUT_TYPE_OPEN_DRAIN, MDIO_OUTPUT_STATE_HIGH};

    MCAL_ASSERT_ON_ERROR(MDIO_init(channel->sdaPin, &mi2cGpioCfg));
    MCAL_ASSERT_ON_ERROR(MDIO_init(channel->sclPin, &mi2cGpioCfg));
}


void MI2C_input(const struct MDIO_Channel *channel)
{
    //standard GPIO input without pull-up
    MDIO_configureMode(channel->port, channel->pin, MDIO_MODE_DIGITAL_INPUT);
}


void MI2C_output(const struct MDIO_Channel *channel)
{
    MDIO_configureMode(channel->port, channel->pin, MDIO_MODE_DIGITAL_OUTPUT);
}

/* end of functions for software created I2C */


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void MI2C_enableClock(const struct MI2C_Channel *channel)
{
    /* Unlock RCC registers */
    RCC_REGS_UNLOCK();

    //RCC->APB1ENR.V32 |= RCC_APB1_TWI0;
    MSUP_setRegBitField32(&channel->mi2c->clockCfg);   

    /* Lock RCC registers */
    RCC_REGS_LOCK();
}


static void MI2C_clockConfigure(I2C_TypeDef *i2c_periph, uint32_t clkSpeed)
{
    /* SCL frequency (clkspeed) : Ftwi(pclk) / (16 + 2 * PresClock(1,4,16,64) * BaudRate(brt:0-255)) */
    /* PresClock fixed as 1, brt = (pclk / clkspeed - 16) / 2 */
    uint8_t brtValue;
    uint32_t pclkValue, temp;

#ifdef MCAL_MPCM_INCLUDED
    pclkValue = MPCM_getClockFreq(MPCM_CLOCK_PCLK1);
#else
    pclkValue = PCLOCK1 * CLOCK_1MHZ;
#endif
    
    /* I2C peripheral clock frequency */
    if(pclkValue < I2CCLK_MIN)
    {
        brtValue = 0;        
    } 
    else if(pclkValue >= I2CCLK_MAX)
    {
        pclkValue = I2CCLK_MAX;
        temp = pclkValue / clkSpeed;
        brtValue = (uint8_t)((temp - 16U) / 2U);
    }
    else
    {
        temp = pclkValue / clkSpeed;
        if (temp < (uint8_t)16)
        {
            brtValue = 0;
        }
        else
        {
            brtValue = (uint8_t)((temp - 16U) / 2U);
        }        
    }
    i2c_periph->BRT       = brtValue;
    i2c_periph->CR.BIT.CR = (uint32_t)TWI_CR_DIV_1;
}


static void MI2C_ackConfigure(I2C_TypeDef *i2c_periph, TWI_AA_Type newAck)
{
    i2c_periph->CR.BIT.AA = (uint8_t)newAck;
}


static uint8_t MI2C_sendWaitDone(I2C_TypeDef *i2c_periph)
{
    bool temp1, temp2;
    __IO uint8_t event;
    __IO uint16_t timeOut = 0xFFFFU;

    /* STA bit if be enabled, STAT register possiblly automatic transition from F8H to 08H */
    event = (uint8_t)i2c_periph->STAT.V32;
    if(event != (uint8_t)TWI_NO_VALID_EVENT)  
    {
        /* clear Interrupt Flag to start transmit */
        i2c_periph->FR.V32 = (TWI_FR_TOUTC_Msk | TWI_FR_TFREEC_Msk | TWI_FR_TWINTC_Msk);
    }

    do{
        event = (uint8_t)TWI_GetFlagStatus(i2c_periph, TWI_FLAG_INT);
        timeOut = timeOut - (uint16_t)1;
        temp1 = (event == (uint8_t)0) ? true : false;
        temp2 = (timeOut > 0U) ? true : false;
    } while(temp1 && temp2);

    if(timeOut > 0U)
    {
        /* Clear TWI0->CR.STA / TWI0->CR.STO */
        i2c_periph->CR.BIT.STA = (uint32_t)SH_RESET;
        i2c_periph->CR.BIT.STO = (uint32_t)SH_RESET;
        event = (uint8_t)i2c_periph->STAT.V32;
    }
    else
    {
        event = (uint8_t)TWI_INVALID_EVENT;
    }
    return event;
}
