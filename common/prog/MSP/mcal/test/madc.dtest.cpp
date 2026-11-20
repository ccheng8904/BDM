/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include <doctest/doctest.h>
#include "mcal.mock.h"
#include "madc_priv.mock.h"
#include "msup.mock.h"
#include "madc_mc.h"

using trompeloeil::_;

namespace madc_test
{   
    #include "mcal/madc.h"

    class
    {
        MAKE_MOCK1(MADC_isConversionCorrupted    , bool        ( const MADC_Handle *handle ));
    }int_mock;

    class
    {
        MAKE_MOCK2(MADC_readSample               , uint16_t    ( MADC_Handle *handle, const MADC_Channel *channel ));
    }ext_mock;



    #include "mcal/madc.c"

    void initHandle(MADC_Handle *h)
    {   // initialize with some random values
        h->channelCallback   = (STDCB_Callback*)0x1234;
        h->adcCallback       = (STDCB_Callback*)0x1234;
        h->status            =  MCAL_STATUS_RESET;
        h->downscale         =  0xff;
        h->regularConversion =  true;
        h->nextHandle        =  (MADC_Handle*)(0x1234);
        h->reentrancyCounter =  42;
        h->scanChannels      =  (const MADC_Channel**)(0x1234);
        h->scanResults       =   (uint16_t*)(0x1234);
        h->currentResult     =   12;
        h->scanChannelCount  =   55;
        h->sampleTimeout     =   55;
        h->sampleValidity    =   (MCAL_Error)55;
        h->regularConversion =   true;
    }   

    TEST_SUITE("madc")
    {
        trompeloeil::sequence seq;
        MADC_Handle handle{0};
        const MADC_Channel* channel    = (const MADC_Channel*)(0x54321);
        const MADC_Periph* peripheral  = (const MADC_Periph*)(0x12345);
        const MADC_Periph* peripheral2 = (const MADC_Periph*)(0x54321);
        const MADC_Periph* peripheral3 = (const MADC_Periph*)(0x55AA);        
        const MADC_Config cfg = 
        {
            MADC_TRIGGER_SOFTWARE,    // Software trigger selection
            MADC_RESOLUTION_16_BITS,  // Resolution width
            4711                      // Without a time limit for a single channel conversion
        };
        const uint8_t irq_index = 1;
        const uint16_t conv_result     = 0x0815u;


        TEST_CASE("MADC_init")
        {   //enum MCAL_Error MADC_init(struct MADC_Handle *handle, const struct MADC_Periph *periph, const struct MADC_Config *cfg)
            initHandle(&handle);
            MADC_IRQ_HandleTable[irq_index] = nullptr;
            SUBCASE("configuration error")
            {   /* when configuration as first action returns a value different form MCAL_OK
                * init immediately leave with the given error message */
                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))
                .WITH(_1 == peripheral) 
                .WITH(_2 == &cfg) 
                .RETURN(MCAL_ERROR);
                MCAL_Error ret = MADC_init(&handle, peripheral, &cfg);
                CHECK(ret == MCAL_ERROR);
            }
            SUBCASE("successful configuration")
            {
                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                MCAL_Error ret = MADC_init(&handle, peripheral, &cfg);
                CHECK(ret == MCAL_OK);
                CHECK(handle.periph == peripheral);
                CHECK(handle.adcCallback == nullptr);
                CHECK(handle.channelCallback == nullptr);
                CHECK(handle.downscale == MADC_RESOLUTION_16_BITS);
                CHECK(handle.reentrancyCounter == 0u);
                CHECK(handle.scanChannels == nullptr);
                CHECK(handle.scanResults == nullptr);
                CHECK(handle.currentResult == 0u);
                CHECK(handle.scanChannelCount == 0u);
                CHECK(handle.sampleTimeout == 4711);
                CHECK(handle.sampleValidity == MCAL_OK);
                CHECK(handle.status == MCAL_STATUS_READY);
                CHECK(handle.nextHandle == nullptr);
                CHECK(handle.regularConversion == false);
                CHECK(MADC_IRQ_HandleTable[irq_index] == &handle);
            }
            SUBCASE("register 3 handler")
            {   //for coverage 3 handlers are required
                MADC_Handle handle2{0};
                MADC_Handle handle3{0};
                initHandle(&handle2);
                initHandle(&handle3);

                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                (void)MADC_init(&handle, peripheral, &cfg);
                (void)MADC_init(&handle2, peripheral, &cfg);
                (void)MADC_init(&handle3, peripheral, &cfg);                
                CHECK(handle3.nextHandle == nullptr);
                CHECK(handle2.nextHandle == &handle3);
                CHECK(handle.nextHandle  == &handle2);
                CHECK(MADC_IRQ_HandleTable[irq_index] == &handle);
            }
        }

        TEST_CASE("MADC_enable")
        {   //void MADC_enable(struct MADC_Handle *handle)
            initHandle(&handle);
            MADC_IRQ_HandleTable[irq_index] = nullptr;
            REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
            REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
            (void)MADC_init(&handle, peripheral, &cfg);

            SUBCASE("handle not initialized")
            {
                handle.status = MCAL_STATUS_RESET; // deinitialize the handler
                MADC_enable(&handle);
                CHECK(handle.status == MCAL_STATUS_RESET);
            }
            SUBCASE("handle initialized")
            {
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                handle.status = MCAL_STATUS_DISABLED;
                MADC_enable(&handle);
                CHECK(handle.status == MCAL_STATUS_READY);
            }

        }

        TEST_CASE("MADC_disable")
        {   //void MADC_disable(struct MADC_Handle *handle)
            initHandle(&handle);
            MADC_IRQ_HandleTable[irq_index] = nullptr;
            REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
            REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
            (void)MADC_init(&handle, peripheral, &cfg);
            REQUIRE_CALL(madc_priv_mock, MADC_disablePeriph(_)) .WITH(_1 == peripheral);
            MADC_disable(&handle);
            CHECK(handle.status == MCAL_STATUS_DISABLED);
        }

        TEST_CASE("MADC_getStatus")
        {   //enum MCAL_Status MADC_getStatus(const struct MADC_Handle *handle)
            handle.status = MCAL_STATUS_READY;
            CHECK(MADC_getStatus(&handle) == MCAL_STATUS_READY);
            handle.status = MCAL_STATUS_BUSY;
            CHECK(MADC_getStatus(&handle) == MCAL_STATUS_BUSY);

        }

        TEST_CASE("MADC_IRQ_Handler")
        {   //void MADC_IRQ_Handler(enum MADC_IRQ_Index index)
            uint16_t results[2] = {0};
            initHandle(&handle);
            MADC_IRQ_HandleTable[irq_index] = nullptr;
            REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
            REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
            (void)MADC_init(&handle, peripheral, &cfg);
            // precondition for scan event
            handle.scanResults  = results;
            handle.currentResult    = 1;
            handle.scanChannelCount = 2;


            SUBCASE("IRQ entry for given index is a null pointer")
            {
                MADC_IRQ_HandleTable[0] = nullptr;
                // no mocks shall be called, handler shall be left emmediatly 
                MADC_IRQ_Handler((MADC_IRQ_Index)0);
            }

            SUBCASE("regular conversion is ongoing")
            {
                handle.regularConversion = true;
                /* no mocks shall be called, handler shall be left after recognise that there is
                   a regular conversion ongoing */
                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
            }
            SUBCASE("no conversion / scan complete for given handle")
            {
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))  .RETURN(false);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))  .TIMES(2)  .RETURN(false);
                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
            }
            SUBCASE("Conversion complete event, scan sequence finished")
            {
                handle.currentResult     = 2; // scan sequence finished
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))  .RETURN(true)              .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .RETURN(false)  .IN_SEQUENCE(seq);
                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
            }
            SUBCASE("Scan complete event, scan sequence finished")
            {
                handle.currentResult     = 2; // scan sequence finished
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))             .RETURN(false)  .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .TIMES(2)  .RETURN(true)   .IN_SEQUENCE(seq);
                REQUIRE_CALL(mcal_mock, MCAL_invokeCallbacks(_,_,_,_))                          .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_clearScanEventFlag(_))                        .IN_SEQUENCE(seq);
                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
                CHECK(handle.currentResult == 0u);
            }
            SUBCASE("Scan complete event, result index corrupted")
            {  //Test intention: robustness of the code, invalid index will be corrected
                handle.currentResult     = 3; // scan sequence finished
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))             .RETURN(false)  .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .TIMES(2)  .RETURN(true)   .IN_SEQUENCE(seq);
                REQUIRE_CALL(mcal_mock, MCAL_invokeCallbacks(_,_,_,_))                          .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_clearScanEventFlag(_))                        .IN_SEQUENCE(seq);
                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
                CHECK(handle.currentResult == 0u);
            }
            SUBCASE("Both Conversion and Scan complete event (just to have all combinations), last scan index reached")
            {   // test case not that important, here both events are pending
                handle.currentResult     = 2; // scan sequence finished
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))             .RETURN(true) .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))               .RETURN(true) .IN_SEQUENCE(seq);
                REQUIRE_CALL(mcal_mock, MCAL_invokeCallbacks(_,_,_,_))                        .IN_SEQUENCE(seq); 
                REQUIRE_CALL(madc_priv_mock, MADC_clearScanEventFlag(_))                      .IN_SEQUENCE(seq);
                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
                CHECK(handle.currentResult == 0u);
            }
            SUBCASE("Conversion complete, scan not finished yet, no channelCallback")
            {
                handle.currentResult = 0;
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(true)         .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))          .RETURN(conv_result)  .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_clearConversionEventFlag(_))                       .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))              .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))              .RETURN(false)        .IN_SEQUENCE(seq); // condition after while loop

                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
                CHECK(handle.currentResult == 1u);
                CHECK(results[0] == conv_result);
            }

            SUBCASE("Conversion complete, scan not finished yet, no scan result buffer")
            {

                handle.currentResult = 0;
                handle.scanResults = nullptr;
                handle.channelCallback   = (STDCB_Callback*)0x1234;
                const struct MADC_Channel* scanChannels[2];
                handle.scanChannels = scanChannels;
                uint8_t theChannel = 0x55;
                results[0] = 0x4711u;

                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(true)         .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))          .RETURN(conv_result)  .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_getChannelNum(_))            .RETURN(theChannel)   .IN_SEQUENCE(seq);
                REQUIRE_CALL(mcal_mock, MCAL_invokeChannelCallbacks(_,_,_,_))
                    .WITH(_4->dataType == MCAL_DATA_TYPE_UINT16)
                    .WITH(_4->data.uint16 == conv_result)                                            .IN_SEQUENCE(seq);

                REQUIRE_CALL(madc_priv_mock, MADC_clearConversionEventFlag(_))                       .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))              .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))              .RETURN(false)        .IN_SEQUENCE(seq); // condition after while loop

                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
                CHECK(handle.currentResult == 1u);
                CHECK(results[0] == 0x4711u);
            }

            SUBCASE("2 conversions pending")
            {

                handle.currentResult = 0;

                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(true)          .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))          .RETURN(conv_result)   .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_clearConversionEventFlag(_))                        .IN_SEQUENCE(seq);
                // second loop
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(true)          .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))          .RETURN(conv_result+1) .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_clearConversionEventFlag(_))                        .IN_SEQUENCE(seq);
                // try third loop
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(false)          .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))              .RETURN(false)          .IN_SEQUENCE(seq);

                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))              .RETURN(false)         .IN_SEQUENCE(seq); // condition after while loop

                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);
                CHECK(handle.currentResult == 2u);
                CHECK(results[0] == conv_result);
                CHECK(results[1] == conv_result+1);
            }
            SUBCASE("3 chained handles, no conversions")
            {
                MADC_Handle handle2{0};
                MADC_Handle handle3{0};
                initHandle(&handle2);
                initHandle(&handle3);

                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
                REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
                (void)MADC_init(&handle, peripheral, &cfg);
                (void)MADC_init(&handle2, peripheral2, &cfg);
                (void)MADC_init(&handle3, peripheral3, &cfg);

                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))  .WITH(_1 == peripheral)   .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .WITH(_1 == peripheral)   .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .WITH(_1 == peripheral)   .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))  .WITH(_1 == peripheral2)  .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .WITH(_1 == peripheral2)  .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .WITH(_1 == peripheral2)  .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))  .WITH(_1 == peripheral3)  .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .WITH(_1 == peripheral3)  .RETURN(false)     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isScanEvent(_))    .WITH(_1 == peripheral3)  .RETURN(false)     .IN_SEQUENCE(seq);

                MADC_IRQ_Handler((MADC_IRQ_Index)irq_index);


            }



        }

        TEST_CASE("MADC_readSample")
        {   //uint16_t MADC_readSample(struct MADC_Handle *handle, const struct MADC_Channel *channel)
            initHandle(&handle);
            MADC_IRQ_HandleTable[irq_index] = nullptr;            
            REQUIRE_CALL(madc_priv_mock, MADC_configure(_,_))  .RETURN(MCAL_OK);
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_getIndex(_)) .RETURN(MADC_IRQ_Index(irq_index));
            REQUIRE_CALL(madc_priv_mock, MADC_IRQ_enable(_));
            REQUIRE_CALL(madc_priv_mock, MADC_enablePeriph(_));
            (void)MADC_init(&handle, peripheral, &cfg);

            handle.scanChannels      =  nullptr; // no event conversion

            SUBCASE("simple normal conversion, no corruption, no timeout ")
            {
                const MADC_Channel** channels = (const MADC_Channel**)(0x1234);
                handle.scanChannels = channels;
                REQUIRE_CALL(madc_priv_mock, MADC_disablePeriphEvent(_))    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_startPeriphScanConversion(_,_,_))
                    .RETURN(MCAL_OK)
                    .WITH(_1 = peripheral)
                    .WITH(_2 = &channel)
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(msup_mock, MSUP_setTimeOutExt(_,_)) 
                    .WITH(_2 == handle.sampleTimeout)
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))
                    .WITH(_1 == peripheral)
                    .RETURN(false)                                      // no conversion available in first try
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(int_mock, MADC_isConversionCorrupted(_))
                    .RETURN(false)                                      // no corrupted adc conversion
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(msup_mock, MSUP_isTimeOutExt(_)) 
                    .RETURN(false)                                     // no timeout
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))
                    .RETURN(true)                                      // conversion finished!
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))
                    .RETURN(conv_result)                               // here comes the conversion result
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(int_mock, MADC_isConversionCorrupted(_))
                    .RETURN(false)                                     // no corrupted adc conversion
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_clearScanEventFlag(_))
                    .WITH(_1 == peripheral)
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriphEvent(_))
                    .WITH(_1 == peripheral)
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_enableScanEvent(_))
                    .WITH(_1 == peripheral)
                    .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_startPeriphScanConversion(_,_,_))
                    .RETURN(MCAL_OK)
                    .IN_SEQUENCE(seq);



                CHECK(MADC_readSample(&handle,channel) == conv_result);
                CHECK(handle.reentrancyCounter == 0u);
                CHECK(handle.regularConversion == false);
                CHECK(MADC_isConversionResultValid(&handle) == true);
            }

            SUBCASE("no corruption, but timeout ")
            {
                REQUIRE_CALL(madc_priv_mock, MADC_disablePeriphEvent(_))                             .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_startPeriphScanConversion(_,_,_)) .RETURN(MCAL_OK) .IN_SEQUENCE(seq);
                REQUIRE_CALL(msup_mock, MSUP_setTimeOutExt(_,_))                                     .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(int_mock, MADC_isConversionCorrupted(_))          .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(msup_mock, MSUP_isTimeOutExt(_))                  .RETURN(true)         .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))          .RETURN(conv_result)  .IN_SEQUENCE(seq);
                REQUIRE_CALL(int_mock, MADC_isConversionCorrupted(_))          .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_clearScanEventFlag(_))                             .IN_SEQUENCE(seq);
                CHECK(MADC_readSample(&handle,channel) == conv_result);
                CHECK(handle.reentrancyCounter == 0u);
                CHECK(handle.regularConversion == false);
                CHECK(MADC_isConversionResultValid(&handle) == false);  // as timeout happened
            }

            SUBCASE("no corruption, timeout disabled, first conversion corrupted")
            {
                handle.sampleTimeout =  0; // disable timeout
                REQUIRE_CALL(madc_priv_mock, MADC_disablePeriphEvent(_))                             .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_startPeriphScanConversion(_,_,_)) .RETURN(MCAL_OK) .IN_SEQUENCE(seq);
                ALLOW_CALL(msup_mock, MSUP_setTimeOutExt(_,_))                                       .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(false)        .IN_SEQUENCE(seq);
                REQUIRE_CALL(int_mock, MADC_isConversionCorrupted(_))          .RETURN(true)         .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))          .RETURN(conv_result)  .IN_SEQUENCE(seq); // read data anyway
                REQUIRE_CALL(int_mock, MADC_isConversionCorrupted(_))          .RETURN(true)         .IN_SEQUENCE(seq); // corrupted, outer loop

                // restart conversion, outer loop
                REQUIRE_CALL(madc_priv_mock, MADC_disablePeriphEvent(_))                             .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_stopPeriphConversion(_))                           .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_startPeriphScanConversion(_,_,_)) .RETURN(MCAL_OK) .IN_SEQUENCE(seq);
                ALLOW_CALL(msup_mock, MSUP_setTimeOutExt(_,_))                                       .IN_SEQUENCE(seq); 
                REQUIRE_CALL(madc_priv_mock, MADC_isPeriphEvent(_))            .RETURN(true)         .IN_SEQUENCE(seq); // now data is available
                REQUIRE_CALL(madc_priv_mock, MADC_readResultReg(_,_))          .RETURN(conv_result)  .IN_SEQUENCE(seq);
                REQUIRE_CALL(int_mock, MADC_isConversionCorrupted(_))          .RETURN(false)        .IN_SEQUENCE(seq); // now data is valid
                REQUIRE_CALL(madc_priv_mock, MADC_clearScanEventFlag(_))                             .IN_SEQUENCE(seq);
                CHECK(MADC_readSample(&handle,channel) == conv_result);
                CHECK(handle.reentrancyCounter == 0u);
                CHECK(handle.regularConversion == false);
            }

        }

        TEST_CASE("MADC_readSampleWithValidation")
        {
            uint16_t adc_result = 0u;
            const uint16_t adc_value = 0x0815;
            const MADC_Channel** channels = (const MADC_Channel**)(0x1234);

            SUBCASE("valid conversion")
            {
                handle.sampleValidity = MCAL_OK;
                REQUIRE_CALL(ext_mock, MADC_readSample(_,_)) .RETURN(adc_value);
                CHECK(MADC_readSampleWithValidation(&handle, channel, &adc_result) == MCAL_OK);
                CHECK(adc_result == adc_value);

            }
            SUBCASE("timeout")
            {
                handle.sampleValidity = MCAL_ERROR_TIMEOUT;
                REQUIRE_CALL(ext_mock, MADC_readSample(_,_)) .RETURN(adc_value);
                CHECK(MADC_readSampleWithValidation(&handle, channel, &adc_result) == MCAL_ERROR_TIMEOUT);
                CHECK(adc_result == adc_value);

            }

        }

        TEST_CASE("MADC_isConversionResultValid")
        {   //bool MADC_isConversionResultValid(const struct MADC_Handle *handle)
            SUBCASE("timeout --> conversion invalid")
            {
                handle.sampleValidity = MCAL_ERROR_TIMEOUT;
                CHECK(MADC_isConversionResultValid(&handle) == false);
            }
            SUBCASE("conversion valid")
            {
                handle.sampleValidity = MCAL_OK;
                CHECK(MADC_isConversionResultValid(&handle) == true);
            }

        }

        TEST_CASE("MADC_enableChannelEvent")
        {   //enum MCAL_Error MADC_enableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback)
            STDCB_Callback* dummyCBchannel =  (STDCB_Callback*)0x1234;
            const  uint8_t channelNum = 42;
            MCAL_Callback_t*  dummyCB = (MCAL_Callback_t*)0x54321;
            handle.channelCallback = dummyCBchannel;
            REQUIRE_CALL(madc_priv_mock, MADC_getChannelNum(_))      .RETURN(channelNum)              .IN_SEQUENCE(seq);
            REQUIRE_CALL(mcal_mock, MCAL_enableChannelEvent(_,_,_))
                .WITH(_1 == &handle.channelCallback)
                .WITH(_2 == dummyCB)
                .WITH(_3 = channelNum)
                .IN_SEQUENCE(seq);
            CHECK(MADC_enableChannelEvent(&handle, channel, dummyCB) == MCAL_OK);


        }


        TEST_CASE("MADC_enablePeriphScanEvent: Check...")
        {   //enum MCAL_Error MADC_enablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback)
            MCAL_Callback_t*  dummyCB = (MCAL_Callback_t*)0x54321;
            initHandle(&handle);
            REQUIRE_CALL(mcal_mock, MCAL_enableEvent(_,_,_))
                .WITH(_1 == &handle.adcCallback)
                .WITH(_2 == dummyCB)
                .WITH(_3 == MCAL_EVENT_END_OF_SEQUENCE);
            CHECK(MADC_enablePeriphScanEvent(&handle, dummyCB) == MCAL_OK);
        }



        TEST_CASE("MADC_startScanConversion")
        {   //enum MCAL_Error MADC_startScanConversion(struct MADC_Handle *handle, const struct MADC_Channel *channels[], uint8_t channelCount, uint16_t *results)
            const MADC_Channel** channels = (const MADC_Channel**)(0x1234);
            uint16_t results[7];
            initHandle(&handle);

            SUBCASE("successful start")
            {
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriphEvent(_))                                .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_enableScanEvent(_))                                  .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_startPeriphScanConversion(_,_,_)) .RETURN(MCAL_OK)   .IN_SEQUENCE(seq);

                CHECK(MADC_startScanConversion(&handle, channels, 7, results) == MCAL_OK);
                CHECK(handle.scanChannels     == channels);
                CHECK(handle.scanResults      == results);
                CHECK(handle.scanChannelCount == 7);
                CHECK(handle.currentResult    == 0u);
                CHECK(handle.status == MCAL_STATUS_BUSY);
            }

            SUBCASE("peripheral start")
            {
                REQUIRE_CALL(madc_priv_mock, MADC_enablePeriphEvent(_))                                 .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_enableScanEvent(_))                                   .IN_SEQUENCE(seq);
                REQUIRE_CALL(madc_priv_mock, MADC_startPeriphScanConversion(_,_,_)) .RETURN(MCAL_ERROR) .IN_SEQUENCE(seq);

                CHECK(MADC_startScanConversion(&handle, channels, 7, results) == MCAL_ERROR);
                /*TODO: should the handle changed, even when scan cannot be started ?*/
                CHECK(handle.scanChannels     == channels);
                CHECK(handle.scanResults      == results);
                CHECK(handle.scanChannelCount == 7);
                CHECK(handle.currentResult    == 0u);
                CHECK(handle.status == MCAL_STATUS_BUSY);
            }

        }

        TEST_CASE("MADC_isConversionCorrupted")
        {   //static bool MADC_isConversionCorrupted(const struct MADC_Handle *handle);
            SUBCASE("not corrupted")
            {
                handle.regularConversion = true;
                CHECK(MADC_isConversionCorrupted(&handle) == false);
            }
            SUBCASE("corrupted")
            {
                handle.regularConversion = false;
                CHECK(MADC_isConversionCorrupted(&handle) == true);
            }

        }

        TEST_CASE("MADC_disablePeriphScanEvent: Check...")
        {   //enum MCAL_Error MADC_disablePeriphScanEvent(struct MADC_Handle *handle, MCAL_Callback_t *callback)
            /****** Interface not implemented *****/
            CHECK(true);
        }

        TEST_CASE("MADC_disableChannelEvent")
        {   //enum MCAL_Error MADC_disableChannelEvent(struct MADC_Handle *handle, const struct MADC_Channel *channel, MCAL_Callback_t *callback)
            /****** Interface not implemented *****/
            CHECK(true);
        }


    }
}

