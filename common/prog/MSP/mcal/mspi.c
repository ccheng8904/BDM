/*******************************************************************************
 *  Copyright (c) 2020 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MSPI
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  MCAL
 *
 *  \brief    Implementation of MSPI
 */


/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "BSH_stdinc.h"
#include "mspi.h"
#include "mspi_mc.h"
#include "mspi_priv.h"
#include "mdio.h"
#include "msup.h"

#ifdef MCAL_MPCM_INCLUDED
    #include "mpcm.h"
#endif /* MCAL_MPCM_INCLUDED */

#ifdef MCAL_MSPI_NDEBUG
    #define NDEBUG
#endif 
#include "mcal_assert.h"


/******************************************************************************/
/* PRIVATE DEFINITIONS                                                        */
/******************************************************************************/


/******************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS AND PRIVATE MACRO FUNCTION DEFINITIONS       */
/******************************************************************************/

/** Function for registering handle instance to IQR handle table.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 */
static void MSPI_IRQ_registerHandle(struct MSPI_Handle *handle);


/** Function configuring SCLK, MOSI and MISO pins to alternate function. Pin chip select is configuring according the settings.
 *  \param sclkPin: pointer MDIO_Channel instance of clock pin
 *  \param mosiPin: pointer MDIO_Channel instance of MOSI (master output, slave input) pin
 *  \param misoPin: pointer MDIO_Channel instance of MISO (master input, slave output) pin
 *  \param csPin  : pointer MSPI_ChipSelect instance of chip select configuration
 */
static enum MCAL_Error MSPI_configurePins(const struct MDIO_Channel *sclkPin, const struct MDIO_Channel *mosiPin, const struct MDIO_Channel *misoPin, const struct MDIO_Channel *csPin, const struct MSPI_Config *config);


/** Function for checking if event is pending.
 *  \note Pending event is, when event is enabled using MXXX_enableEvent and
 *        respective status flag in peripheral status register is set.
 *  \param event: event that is checked
 *  \param status: status register value
 *  \return true if event is pending
 */
static bool MSPI_isEventPending(enum MCAL_Event event, uint32_t status);


/** Function setting chip select pin to active state.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 */
static void MSPI_activateChipSelect(const struct MSPI_Handle *handle);


/** Function setting chip select pin to deactive state.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 */
static void MSPI_deactivateChipSelect(const struct MSPI_Handle *handle);


/** Function for checking if clock generation has been finished
 *  \param handle: pointer to MSPI_Handle instance
 */
static bool MSPI_isClockFinished(const struct MSPI_Handle *handle);


/** Function for starting of communication
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 *  \param txLength: number of bytes to write
 *  \param rxLength: number of bytes to read
 */
static enum MCAL_Error MSPI_startCommunication(struct MSPI_Handle *handle, uint16_t *txLength, uint16_t *rxLength);

#if defined(RA2)

#else
#ifdef MCAL_MDMA_INCLUDED
/** Function for starting DMA communication
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 *  \param dmaDir: enum element describing DMA direction
 */
static enum MCAL_Error MSPI_startDmaCommunication(struct MSPI_Handle *handle, enum MSPI_DmaDirection dmaDir);


/** Callback function from MDMA module.
 *  \param obj: pointer to MDMA_Handle instance
 *  \param flags: pending MDMA_STATUS_XXX event flags
 *  \param eventResponse: response from event sender
 */
/*lint -e818 Pointer parameter 'obj' could be declared as pointing to const */
static void MSPI_MDMA_TxCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);


/** Callback function from MDMA module.
 *  \param obj: pointer to MDMA_Handle instance
 *  \param flags: pending MDMA_STATUS_XXX event flags
 *  \param eventResponse: response from event sender
 */
/*lint -e818 Pointer parameter 'obj' could be declared as pointing to const */
static void MSPI_MDMA_RxCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);


/** Function for setting Tx channel configuration .
 *  \param handle: pointer to MSPI_Handle instance
 *  \param txBuffer: pointer to buffer containing the data to send. This buffer
 *                   needs to be allocated during complete write operation (e.g.
 *                   static variable). Content of buffer is not changed.
 *  \param txLength: number of data to be written
 */
static enum MCAL_Error MSPI_MDMA_setTxChannelConfig(struct MSPI_Handle *handle, struct MDMA_ChannelConfig *dmaConfigTx, const uint8_t *txBuffer, uint16_t txLength);

/** Function for setting Rx channel configuration.
 *  \param handle: pointer to MSPI_Handle instance
 *  \param rxBuffer: pointer to buffer where data will be placed. This buffer needs to
 *                   be allocated during complete read operation (e.g. static variable).
 *  \param rxLength: number of data to read
 */
static enum MCAL_Error MSPI_MDMA_setRxChannelConfig(struct MSPI_Handle *handle, struct MDMA_ChannelConfig *dmaConfigRx, const uint8_t *rxBuffer, uint16_t rxLength);
#endif/* MCAL_MDMA_INCLUDED */
#endif/* defined(RA2) */


#ifdef MCAL_MPCM_INCLUDED
/** Callback function from MPCM module.
 *  \param obj: pointer to MSPI_Handle instance
 *  \param flags: pending MPCM_STATUS_XXX event flags
 *  \param eventResponse: response from event sender
 */
/*lint -e818 Pointer parameter 'obj' could be declared as pointing to const */
static void MSPI_MPCM_CallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse);
#endif /* MCAL_MPCM_INCLUDED */


/** Function handling write event inside interrupt.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 */
static void MSPI_handleWriteEvent(struct MSPI_Handle *handle);


/** Function handling read event inside interrupt.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 */
static void MSPI_handleReadEvent(struct MSPI_Handle *handle);


static enum MCAL_Error MSPI_handleErrorEvent(struct MSPI_Handle *handle);


/** Function handling transfer functionality.
 *  \param handle: pointer to MSPI_Handle instance, stored in static memory
 *  \param status: value of status register of SPI peripheral
 */
static enum MCAL_Error MSPI_handleTransfer(struct MSPI_Handle *handle, uint32_t status);


/******************************************************************************/
/* PRIVATE DATA DEFINTIONS                                                    */
/******************************************************************************/
static struct MSPI_Handle *MSPI_IRQ_HandleTable[MSPI_IRQ_CNT];

#if !defined(RA2)
#ifdef MCAL_MDMA_INCLUDED
static const uint8_t       MSPI_MDMA_DummyVal = 0xFF;
static uint8_t             MSPI_MDMA_DummyBuf;
#endif
#endif /* !defined(RA2) */

/******************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                */
/******************************************************************************/

enum MCAL_Error MSPI_init(struct MSPI_Handle *handle, const struct MSPI_Channel *channel, const struct MSPI_Config *cfg)
{
    handle->channel = channel;

    MSPI_disable(handle);
    MCAL_RETURN_ON_ERROR(MSPI_configurePins(channel->sclk, channel->mosi, channel->miso, channel->cs, cfg));
    
    if(cfg->chipSelMode != MSPI_CHIP_SELECT_MANUAL)
    {
        handle->chipSelectPin = channel->cs;
    }

    MCAL_RETURN_ON_ERROR(MSPI_configure(handle->channel, cfg));

    MSPI_IRQ_enable(handle->channel);
    MSPI_IRQ_registerHandle(handle);

    handle->defaultBaudrate = cfg->clockFreq;
    handle->cbList = NULL;
    handle->status = MCAL_STATUS_DISABLED;
    handle->dataBits = cfg->dataBits;
    handle->clockPolarity = cfg->clockPolarity;

  #ifdef MCAL_MPCM_INCLUDED
    MCAL_initCallback(&handle->mpcmCallback, MSPI_MPCM_CallbackFunction, handle);
    MCAL_RETURN_ON_ERROR(MPCM_enableEvent(&handle->mpcmCallback, MCAL_EVENT_CLOCK_CHANGE_ANY));
  #endif /* MCAL_MPCM_INCLUDED */

    MSPI_enable(handle);

    return MCAL_OK;
}


void MSPI_enable(struct MSPI_Handle *handle)
{
    if(MCAL_isInitialized(handle->status))
    {
        MSPI_enableChannel(handle->channel);
        handle->status = MCAL_STATUS_READY;
        MSPI_clearError(handle);
    }
}


void MSPI_disable(struct MSPI_Handle *handle)
{
    if(MCAL_isInitialized(handle->status))
    {
        MSPI_disableChannel(handle->channel);
        handle->status = MCAL_STATUS_DISABLED;
    }
}


void MSPI_endCommunication(struct MSPI_Handle *handle)
{
    MSPI_deactivateChipSelect(handle);
    (void)MSPI_disableEvent(handle, NULL, MCAL_EVENT_WRITE_BYTE);
    (void)MSPI_disableEvent(handle, NULL, MCAL_EVENT_READ_BYTE);
    MCOM_setReadReady(&handle->transfer);
    MCOM_setWriteReady(&handle->transfer);

    handle->status = MCAL_STATUS_READY;
    MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_TRANSFER_COMPLETE, NULL);
}


enum MCAL_Error MSPI_read(struct MSPI_Handle *handle, uint8_t buffer[], uint16_t len)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(buffer != NULL);

    return MSPI_transfer(handle, NULL, 0u, buffer, len);
}


enum MCAL_Error MSPI_write(struct MSPI_Handle *handle, const uint8_t buffer[], uint16_t len)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(buffer != NULL);

    return MSPI_transfer(handle, buffer, len, NULL, 0u);
}


enum MCAL_Error MSPI_transfer(struct MSPI_Handle *handle, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
{
    MCAL_assert(handle != NULL);

    MCAL_RETURN_ON_ERROR(MSPI_startCommunication(handle, &txLength, &rxLength));

    if(txBuffer != NULL)
    {
        MCOM_setWriteBuffer(&handle->transfer, txBuffer, txLength);
    }

    if(rxBuffer != NULL)
    {
        MCOM_setReadBuffer(&handle->transfer, rxBuffer, rxLength);
    }
    MSPI_clearRxFlag(handle->channel);

    (void)MSPI_enableEvent(handle, NULL, MCAL_EVENT_READ_BYTE);
    (void)MSPI_enableEvent(handle, NULL, MCAL_EVENT_WRITE_BYTE);
    
    return MCAL_OK;
}


enum MCAL_Error MSPI_readBlocking(struct MSPI_Handle *handle, uint8_t *buffer, uint16_t len)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(buffer != NULL);

    return MSPI_transferBlocking(handle, NULL, 0u, buffer, len);
}


enum MCAL_Error MSPI_writeBlocking(struct MSPI_Handle *handle, const uint8_t buffer[], uint16_t len)
{
    MCAL_assert(handle != NULL);
    MCAL_assert(buffer != NULL);

    return MSPI_transferBlocking(handle, buffer, len, NULL, 0u);
}


enum MCAL_Error MSPI_transferBlocking(struct MSPI_Handle *handle, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
{
    MCAL_assert(handle != NULL);

    MCAL_RETURN_ON_ERROR(MSPI_startCommunication(handle, &txLength, &rxLength));

    if(txBuffer != NULL)
    {
        MCOM_setWriteBuffer(&handle->transfer, txBuffer, txLength);
    }

    if(rxBuffer != NULL)
    {
        MCOM_setReadBuffer(&handle->transfer, rxBuffer, rxLength);
    }
    MSPI_clearRxFlag(handle->channel);

    MSUP_setTimeOut(MSPI_TIMEOUT_VALUE);
    while((handle->readCounter != 0u) || (handle->writeCounter != 0u))
    {
        uint32_t status = MSPI_getStatusReg(handle->channel);
        MCAL_RETURN_ON_ERROR(MSPI_handleTransfer(handle, status));
        
        if(MSUP_isTimeOut())
        {
            MSPI_endCommunication(handle);
            return MCAL_ERROR_TIMEOUT;
        }
    }

    return MCAL_OK;
}


enum MCAL_Status MSPI_getStatus(const struct MSPI_Handle *handle)
{
    return handle->status;
}


void MSPI_clearError(const struct MSPI_Handle *handle)
{
    MCAL_assert(handle != NULL);
    if(handle != NULL)
    {
        MSPI_clearErrorReg(handle->channel);
    }
}


enum MCAL_Error MSPI_enableEvent(struct MSPI_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
{
    MCAL_assert(handle != NULL);

    if(handle != NULL)
    {
        if(callback != NULL)
        {
            MCAL_enableEvent(&handle->cbList, callback, event);
        }
        MSPI_IRQ_enableEvent(handle->channel, event);
        return MCAL_OK;
    }
    return MCAL_ERROR_NOT_READY;
}


enum MCAL_Error MSPI_disableEvent(const struct MSPI_Handle *handle, MCAL_Callback_t *callback, enum MCAL_Event event)
{
    MCAL_assert(handle != NULL);

    if(handle != NULL)
    {
        MSPI_IRQ_disableEvent(handle->channel, event);
        if(callback != NULL)
        {
            MCAL_disableEvent(callback, event);
        }
        return MCAL_OK;
    }
    return MCAL_ERROR_NOT_READY;
}


bool MSPI_isEvent(const struct MSPI_Channel *channel, enum MCAL_Event event)
{
    uint32_t status = MSPI_getStatusReg(channel);
    uint32_t eventStatusMask = MSPI_getStatusRegMaskForEvent(event);
    return ((status & eventStatusMask) != 0u) ? true : false;
}


struct MSPI_Handle* MSPI_getHandle(const void *handlePtr)
{
    MCAL_assert(handlePtr != NULL);
    
  #ifndef NDEBUG
    struct MSPI_Handle *handle;
    for(uint8_t n = 0; n < (uint8_t)MSPI_IRQ_CNT; n++)
    {
        handle = MSPI_IRQ_HandleTable[n];
        
        while(handle != NULL)
        {
            if((void *)handle == handlePtr)
            {
                return handle;
            }
          #ifdef MSPI_MULTIPLE_HANDLES_PER_IRQ
            handle = handle->nextHandle;
          #else
            break;
          #endif
        }
    }
    return NULL;
  #else
    /* Note 926:  Conversion between object pointer and void pointer
       Note 9005: Cast removes const qualification
       Uncritical in this case */
    return (struct MSPI_Handle*)handlePtr;/*lint !e9005 !e926 */
  #endif
}

#if defined(RA2)
#else
#ifdef MCAL_MDMA_INCLUDED
void MSPI_initDma(struct MSPI_Handle *handle, struct MDMA_Handle *txHandle, struct MDMA_Handle *rxHandle)
{
    MCAL_assert(txHandle != NULL);
    MCAL_assert(rxHandle != NULL);
    handle->mdmaTxHandle = txHandle;
    handle->mdmaRxHandle = rxHandle;
    handle->mdmaConfigValid = false;
    MCAL_initCallback(&handle->mdmaRxCallback, MSPI_MDMA_RxCallbackFunction, handle);
    MCAL_initCallback(&handle->mdmaTxCallback, MSPI_MDMA_TxCallbackFunction, handle);
}


enum MCAL_Error MSPI_transferDma(struct MSPI_Handle *handle, const uint8_t txBuffer[], uint16_t txLength, uint8_t rxBuffer[], uint16_t rxLength)
{
    uint16_t bytesToTransfer = (txLength >= rxLength) ? txLength : rxLength;
    handle->writeCounter = txLength;
    handle->readCounter = rxLength;
    handle->transfer.readBufferHandle.length = rxLength;
    handle->transfer.writeBufferHandle.length = txLength;
    
    MCAL_assert(handle != NULL);
    handle->status = MCAL_STATUS_BUSY;
    
    if((txBuffer != NULL) && (rxBuffer != NULL) && (txLength > 0U) && (rxLength > 0U))
    {
        MCAL_RETURN_ON_ERROR(MSPI_MDMA_setRxChannelConfig(handle, &handle->mdmaConfigRx, rxBuffer, rxLength));
        MCAL_RETURN_ON_ERROR(MDMA_initChannel(handle->mdmaRxHandle, &handle->mdmaConfigRx));
        MCAL_RETURN_ON_ERROR(MSPI_MDMA_setTxChannelConfig(handle, &handle->mdmaConfigTx, txBuffer, txLength));
        MCAL_RETURN_ON_ERROR(MDMA_initChannel(handle->mdmaTxHandle, &handle->mdmaConfigTx));
    }
    else if((txBuffer != NULL) && (txLength > 0U))
    {
        MCAL_RETURN_ON_ERROR(MSPI_MDMA_setRxChannelConfig(handle, &handle->mdmaConfigRx, &MSPI_MDMA_DummyBuf, bytesToTransfer));
        MCAL_RETURN_ON_ERROR(MDMA_initChannel(handle->mdmaRxHandle, &handle->mdmaConfigRx));
        MCAL_RETURN_ON_ERROR(MSPI_MDMA_setTxChannelConfig(handle, &handle->mdmaConfigTx, txBuffer, bytesToTransfer));
        MCAL_RETURN_ON_ERROR(MDMA_initChannel(handle->mdmaTxHandle, &handle->mdmaConfigTx));
    }
    else if((rxBuffer != NULL) && (rxLength > 0U))
    {
        MCAL_RETURN_ON_ERROR(MSPI_MDMA_setRxChannelConfig(handle, &handle->mdmaConfigRx, rxBuffer, bytesToTransfer));
        MCAL_RETURN_ON_ERROR(MDMA_initChannel(handle->mdmaRxHandle, &handle->mdmaConfigRx));
        MCAL_RETURN_ON_ERROR(MSPI_MDMA_setTxChannelConfig(handle, &handle->mdmaConfigTx, &MSPI_MDMA_DummyVal, bytesToTransfer));
        MCAL_RETURN_ON_ERROR(MDMA_initChannel(handle->mdmaTxHandle, &handle->mdmaConfigTx));
    }
    else
    {   
        handle->status = MCAL_STATUS_ERROR;
        return MCAL_ERROR_PARAM;
    }
    handle->mdmaConfigValid = true;

    MCAL_RETURN_ON_ERROR(MDMA_enableChannelEvent(handle->mdmaRxHandle, &handle->mdmaRxCallback));
    MCAL_RETURN_ON_ERROR(MDMA_enableChannelEvent(handle->mdmaTxHandle, &handle->mdmaTxCallback));
    
    MCAL_RETURN_ON_ERROR(MSPI_startDmaCommunication(handle, MSPI_DMA_DIRECTION_TX_RX));//lint !e774 condition always false

    return MCAL_OK;
}

enum MCAL_Error MSPI_transferDmaContinue(struct MSPI_Handle *handle)
{

    if(handle->mdmaConfigValid == false)
    {
        handle->status = MCAL_STATUS_ERROR;
        return MCAL_ERROR_PARAM;
    }
    handle->writeCounter = handle->mdmaConfigTx.dataLen;
    handle->readCounter  = handle->mdmaConfigRx.dataLen;
    handle->transfer.writeBufferHandle.length = handle->mdmaConfigTx.dataLen;
    handle->transfer.readBufferHandle.length  = handle->mdmaConfigRx.dataLen;

    handle->status = MCAL_STATUS_BUSY;

    MCAL_RETURN_ON_ERROR(MDMA_configureChannelVolatileParams(handle->mdmaRxHandle->channel, &handle->mdmaConfigRx));
    MCAL_RETURN_ON_ERROR(MDMA_configureChannelVolatileParams(handle->mdmaTxHandle->channel, &handle->mdmaConfigTx));

    MCAL_RETURN_ON_ERROR(MDMA_enableChannelEvent(handle->mdmaRxHandle, &handle->mdmaRxCallback));
    MCAL_RETURN_ON_ERROR(MDMA_enableChannelEvent(handle->mdmaTxHandle, &handle->mdmaTxCallback));

    MCAL_RETURN_ON_ERROR(MSPI_startDmaCommunication(handle, MSPI_DMA_DIRECTION_TX_RX));

    return MCAL_OK;
}

#endif/* MCAL_MDMA_INCLUDED */
#endif/* defined(RA2) */


/******************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                               */
/******************************************************************************/

static void MSPI_IRQ_registerHandle(struct MSPI_Handle *handle)
{
    enum MSPI_IRQ_Index index = MSPI_IRQ_getIndex(handle->channel);
    
    if(MSPI_IRQ_HandleTable[index] == NULL)
    {
        MSPI_IRQ_HandleTable[index] = handle;
    }
  #ifdef MSPI_MULTIPLE_HANDLES_PER_IRQ
    else
    {
        struct MSPI_Handle *handleEntry = MSPI_IRQ_HandleTable[index];
        while(handleEntry->nextHandle != NULL)
        {
            handleEntry = handleEntry->nextHandle;
        }
        handleEntry->nextHandle = handle;
    }
    handle->nextHandle = NULL;
  #endif //MSPI_MULTIPLE_HANDLES_PER_IRQ
}


static enum MCAL_Error MSPI_configurePins(const struct MDIO_Channel *sclkPin, const struct MDIO_Channel *mosiPin,
                               const struct MDIO_Channel *misoPin, const struct MDIO_Channel *csPin, const struct MSPI_Config *config)
{
    struct MDIO_Config cfg = MDIO_CFG_ALTERNATE_FUNCTION;

    if(csPin != NULL)
    {
        struct MDIO_Config cfgCS = MDIO_CFG_OUTPUT;
        cfgCS.outputState = (config->chipSelMode == MSPI_CHIP_SELECT_AUTO_HIGH) ? MDIO_OUTPUT_STATE_LOW : MDIO_OUTPUT_STATE_HIGH;
        MCAL_RETURN_ON_ERROR(MDIO_init(csPin, &cfgCS));
    }
    cfg.outputState = (config->clockPolarity == MSPI_CLOCK_POLARITY_1) ? MDIO_OUTPUT_STATE_HIGH : MDIO_OUTPUT_STATE_LOW;
    if(sclkPin != NULL)
    {
        MCAL_RETURN_ON_ERROR(MDIO_init(sclkPin, &cfg));
    }
    cfg.outputState = MDIO_OUTPUT_STATE_NO_CHANGE;
    if(mosiPin != NULL)
    {
        MCAL_RETURN_ON_ERROR(MDIO_init(mosiPin, &cfg));
    }
    if(misoPin != NULL)
    {
        cfg.pullType = config->misoPullResistor;
        MCAL_RETURN_ON_ERROR(MDIO_init(misoPin, &cfg));
    }
    
    return MCAL_OK;
}


static bool MSPI_isEventPending(enum MCAL_Event event, uint32_t status)
{
    uint32_t eventStatusMask = MSPI_getStatusRegMaskForEvent(event);
    return ((status & eventStatusMask) != 0u) ? true : false;
}


static void MSPI_activateChipSelect(const struct MSPI_Handle *handle)
{
    if(handle->chipSelectPin != NULL)
    {
        MDIO_toggle(handle->chipSelectPin);
    }
}


static void MSPI_deactivateChipSelect(const struct MSPI_Handle *handle)
{
    while(false == MSPI_isClockFinished(handle)) {}
    if(handle->chipSelectPin != NULL)
    {
        MDIO_toggle(handle->chipSelectPin);
    }
}


static bool MSPI_isClockFinished(const struct MSPI_Handle *handle)
{
    bool result = MDIO_read(handle->channel->sclk);
    return (MSPI_CLOCK_POLARITY_0 == handle->clockPolarity) ? !result : result;
}


static enum MCAL_Error MSPI_startCommunication(struct MSPI_Handle *handle, uint16_t *txLength, uint16_t *rxLength)
{
    uint16_t bytesToTransfer;
    
    MCAL_RETURN_ON_ERROR(MCAL_checkReady(handle->status));

    handle->status = MCAL_STATUS_BUSY;

    if(handle->dataBits == MSPI_DATA_BITS_16)
    {
        if((*txLength % 2u) != 0u)
        {
            *txLength += 1u;
        }
        if((*rxLength % 2u) != 0u)
        {
            *rxLength += 1u;
        }
    }
    bytesToTransfer = (*txLength >= *rxLength) ? *txLength : *rxLength;
    handle->readCounter = bytesToTransfer;
    handle->writeCounter = bytesToTransfer;

    MSPI_activateChipSelect(handle);

    return MCAL_OK;
}

#if defined(RA2)

#else
#ifdef MCAL_MDMA_INCLUDED
static enum MCAL_Error MSPI_startDmaCommunication(struct MSPI_Handle *handle, enum MSPI_DmaDirection dmaDir)
{
    MSPI_activateChipSelect(handle);
    MSPI_enableDMA(handle->channel->mspi, dmaDir);
    if((dmaDir == MSPI_DMA_DIRECTION_RX) || (dmaDir == MSPI_DMA_DIRECTION_TX_RX))
    {
        MDMA_enableChannel(handle->mdmaRxHandle);
    }
    if((dmaDir == MSPI_DMA_DIRECTION_TX) || (dmaDir == MSPI_DMA_DIRECTION_TX_RX))
    {
        MDMA_enableChannel(handle->mdmaTxHandle);
    }
    
    return MCAL_OK;
}


static void MSPI_MDMA_TxCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void)flags;
    (void)*eventResponse;
    struct MSPI_Handle *handle = MSPI_getHandle(obj);
    
    if(handle->writeCounter == 0U)
    {
        (void)MDMA_disableChannelEvent(handle->mdmaTxHandle, &handle->mdmaTxCallback);
        MDMA_disableChannel(handle->mdmaTxHandle);
    }
    else if(handle->transfer.writeBufferHandle.length >= handle->transfer.readBufferHandle.length)
    {
        (void)MDMA_disableChannelEvent(handle->mdmaTxHandle, &handle->mdmaTxCallback);
        MDMA_disableChannel(handle->mdmaTxHandle);
        handle->writeCounter = 0U;
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_WRITE_COMPLETE, NULL);
    }
    else
    {
        uint16_t remainder = handle->transfer.readBufferHandle.length - handle->transfer.writeBufferHandle.length;
        struct MDMA_ChannelConfig dmaConfigTx;
        MDMA_disableChannel(handle->mdmaTxHandle);
        handle->writeCounter = 0U;
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_WRITE_COMPLETE, NULL);
        (void)MSPI_MDMA_setTxChannelConfig(handle, &dmaConfigTx, &MSPI_MDMA_DummyVal, remainder);
        (void)MDMA_initChannel(handle->mdmaTxHandle, &dmaConfigTx);
        MDMA_enableChannel(handle->mdmaTxHandle);
        return;
    }
}


static void MSPI_MDMA_RxCallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    (void)flags;
    (void)*eventResponse;
    struct MSPI_Handle *handle = MSPI_getHandle(obj);
    
    if(handle->readCounter == 0U)
    {
        (void)MDMA_disableChannelEvent(handle->mdmaRxHandle, &handle->mdmaRxCallback);
        MDMA_disableChannel(handle->mdmaRxHandle);
    }
    else if(handle->transfer.readBufferHandle.length >= handle->transfer.writeBufferHandle.length)
    {
        (void)MDMA_disableChannelEvent(handle->mdmaRxHandle, &handle->mdmaRxCallback);
        MDMA_disableChannel(handle->mdmaRxHandle);
        handle->readCounter = 0U;
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_READ_COMPLETE, NULL);
    }
    else
    {
        uint16_t remainder = handle->transfer.writeBufferHandle.length - handle->transfer.readBufferHandle.length;
        struct MDMA_ChannelConfig dmaConfigRx;
        MDMA_disableChannel(handle->mdmaRxHandle);
        handle->readCounter = 0U;
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_READ_COMPLETE, NULL);
        (void)MSPI_MDMA_setRxChannelConfig(handle, &dmaConfigRx, &MSPI_MDMA_DummyBuf, remainder);
        (void)MDMA_initChannel(handle->mdmaRxHandle, &dmaConfigRx);
        MDMA_enableChannel(handle->mdmaRxHandle);
        return;
    }

    if(MDMA_getStatus(handle->mdmaTxHandle) == MCAL_STATUS_READY)
    {
        MSPI_deactivateChipSelect(handle);
        MSPI_disableDMA(handle->channel->mspi);
        handle->status = MCAL_STATUS_READY;
        MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_TRANSFER_COMPLETE, NULL);
    }
}


static enum MCAL_Error MSPI_MDMA_setTxChannelConfig(struct MSPI_Handle *handle, struct MDMA_ChannelConfig *dmaConfigTx, const uint8_t *txBuffer, uint16_t txLength)
{
    const struct MDMA_Link *periphLink = MSPI_MDMA_getPeriphLink_TX(handle->channel->mspi);
    bool incAddr = (txBuffer == &MSPI_MDMA_DummyVal) ? false : true;

    dmaConfigTx->direction     = MDMA_DIRECTION_MEM_TO_PERIPH;
    dmaConfigTx->mode          = MDMA_MODE_SINGLE;
    dmaConfigTx->dataLen       = txLength;
    dmaConfigTx->src.addr      = (uint32_t)txBuffer; //lint !e923 "Cast form pointer to integer" tested to work correctly 
    dmaConfigTx->src.dataWidth = MDMA_WIDTH_8_BIT;
    dmaConfigTx->src.increment = incAddr;
    dmaConfigTx->dst.addr      = (uint32_t)periphLink->addr;
    dmaConfigTx->dst.dataWidth = periphLink->dataWidth;
    dmaConfigTx->dst.increment = periphLink->increment;

    return MCAL_OK;
}


static enum MCAL_Error MSPI_MDMA_setRxChannelConfig(struct MSPI_Handle *handle, struct MDMA_ChannelConfig *dmaConfigRx, const uint8_t *rxBuffer, uint16_t rxLength)
{
    const struct MDMA_Link *periphLink = MSPI_MDMA_getPeriphLink_RX(handle->channel->mspi);
    bool incAddr = (rxBuffer == &MSPI_MDMA_DummyBuf) ? false : true;

    dmaConfigRx->direction     = MDMA_DIRECTION_PERIPH_TO_MEM;
    dmaConfigRx->mode          = MDMA_MODE_SINGLE;
    dmaConfigRx->dataLen       = rxLength;
    dmaConfigRx->src.addr      = (uint32_t)periphLink->addr;
    dmaConfigRx->src.dataWidth = periphLink->dataWidth;
    dmaConfigRx->src.increment = periphLink->increment;
    dmaConfigRx->dst.addr      = (uint32_t)rxBuffer; //lint !e923 "Cast form pointer to integer" tested to work correctly
    dmaConfigRx->dst.dataWidth = MDMA_WIDTH_8_BIT;
    dmaConfigRx->dst.increment = incAddr;

    return MCAL_OK;
}

#endif/* MCAL_MDMA_INCLUDED */
#endif/* defined(RA2) */


#ifdef MCAL_MPCM_INCLUDED
static void MSPI_MPCM_CallbackFunction(void *obj, uint32_t flags, const struct MCAL_EventResponse *eventResponse)
{
    const struct MSPI_Handle *handle = MSPI_getHandle(obj);
    enum MCAL_Status status = MSPI_getStatus(handle);
    
    (void)flags;
    
    if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_REQ))
    {
        /* if MSPI is busy, abort request to change pmode */
        if(status == MCAL_STATUS_BUSY)
        {
            MPCM_abortPowerModeChange();
        }
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_PRE))
    {
        /* if MSPI is enabled, disable it until pmode is changed */
        if(status != MCAL_STATUS_DISABLED)
        {
            MSPI_disableChannel(handle->channel);
        }
    }
    else if(MCAL_isEvent(eventResponse, MCAL_EVENT_CLOCK_CHANGE_POST))
    {
        /* configure and reenable MSPI if not disabled by user */
        if(status != MCAL_STATUS_DISABLED)
        {
            (void)MSPI_configureBaudrate(handle->channel, handle->defaultBaudrate);
            MSPI_enableChannel(handle->channel);
        }
    }
    else
    {
        /* Nothing to do */
    }
}
#endif /* MCAL_MPCM_INCLUDED */


static void MSPI_handleWriteEvent(struct MSPI_Handle *handle)
{
    if(handle->writeCounter > 0u)
    {
        uint16_t txData = 0xFFFFu;

        if(handle->dataBits == MSPI_DATA_BITS_16)
        {
            handle->writeCounter -= 2u;
            if(MCOM_isWriteBusy(&handle->transfer))
            {
                txData  = MCOM_popByte(&handle->transfer);
                txData |= (uint16_t)MCOM_popByte(&handle->transfer) << 8u;
            }
        }
        else
        {
            handle->writeCounter -=1u; /* -- creates axivion issue due to volatile declaration */
            if(MCOM_isWriteBusy(&handle->transfer))
            {
                txData = MCOM_popByte(&handle->transfer);
            }
        }

        MSPI_writeDataReg(handle->channel, txData);

        if(MCOM_isWriteComplete(&handle->transfer))
        {
            MCOM_setWriteReady(&handle->transfer);
            MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_WRITE_COMPLETE, NULL);
        }

        if(handle->writeCounter == 0u)
        {
            (void)MSPI_disableEvent(handle, NULL, MCAL_EVENT_WRITE_BYTE);
        }
    }
}


static void MSPI_handleReadEvent(struct MSPI_Handle *handle)
{
    uint16_t rxData = MSPI_readDataReg(handle->channel);

    if(handle->readCounter > 0u)
    {
        if(handle->dataBits == MSPI_DATA_BITS_16)
        {
            handle->readCounter -= 2u;
            if(MCOM_isReadBusy(&handle->transfer))
            {
                MCOM_pushByte(&handle->transfer, (uint8_t)rxData);
                MCOM_pushByte(&handle->transfer, (uint8_t)(rxData >> 8u));
            }
        }
        else
        {
            handle->readCounter -=1u; /* -- creates axivion issue due to volatile declaration */
            if(MCOM_isReadBusy(&handle->transfer))
            {
                MCOM_pushByte(&handle->transfer, (uint8_t)rxData);
            }
        }

        if(MCOM_isReadComplete(&handle->transfer))
        {
            struct MCAL_EventData eventData;
            uint16_t readCount = MCOM_getNumOfReadData(&handle->transfer);

            MCAL_setEventData_uint16(&eventData, readCount);
            MCOM_setReadReady(&handle->transfer);
            MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_READ_COMPLETE, &eventData);
        }
    }

    if(handle->readCounter == 0u)
    {
        MSPI_endCommunication(handle);
    }
}


static enum MCAL_Error MSPI_handleErrorEvent(struct MSPI_Handle *handle)
{
    /* TODO add error value for event data */
    MCAL_invokeCallbacks(handle->cbList, MCAL_MODULE_MSPI, MCAL_EVENT_ERROR, NULL);
    MSPI_clearErrorReg(handle->channel);
    MSPI_endCommunication(handle);
    return MCAL_ERROR;
}


static enum MCAL_Error MSPI_handleTransfer(struct MSPI_Handle *handle, uint32_t status)
{
    if(MSPI_isEventPending(MCAL_EVENT_ERROR, status))
    {
        return MSPI_handleErrorEvent(handle);
    }
    else
    {
        if(MSPI_isEventPending(MCAL_EVENT_READ_BYTE, status))
        {
            MSPI_handleReadEvent(handle);
        }
        
        if(MSPI_isEventPending(MCAL_EVENT_WRITE_BYTE, status))
        {
            MSPI_handleWriteEvent(handle);
        }
    }
    return MCAL_OK;
}


/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES                                                 */
/******************************************************************************/
void MSPI_IRQ_Handler(enum MSPI_IRQ_Index index)
{
    struct MSPI_Handle *handle = MSPI_IRQ_HandleTable[index];

    while(handle != NULL)
    {
        if(handle->status == MCAL_STATUS_BUSY)
        {
            uint32_t status = MSPI_getStatusReg(handle->channel);
            (void)MSPI_handleTransfer(handle, status);
        }
      #ifdef MSPI_MULTIPLE_HANDLES_PER_IRQ
        // Get next handle if multiple handles share the same (channel) IRQ
        handle = handle->nextHandle;
      #else
        break;
      #endif
    }
}
