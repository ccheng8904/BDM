<div align="center">

<h1>Software Unit Test Strategy</h1>
</div>

# 1. Aim

This documentation delivers a software unit test architecture and patterns for common problems.
Since most source code for MCUs is currently written in C, the focus of this document is on testing C code.

# 2. Preface

Early unit testing of C and C++ source code is a crucial practice that offers numerous benefits to developers and the overall software development process.
1. **Bug Detection and Prevention:**
Unit testing allows developers to identify and address bugs and issues in the codebase at an early stage. By testing individual units of code in isolation, it becomes easier to pinpoint and fix defects before they propagate to other parts of the system. This proactive approach significantly reduces the likelihood of encountering critical bugs later in the development cycle.
2. **Code Quality Improvement:**
Writing unit tests requires developers to think critically about the design and implementation of their code. This process encourages cleaner and more modular code architectures, leading to improved code quality. By adhering to best practices and writing testable code, developers can enhance maintainability, readability, and reusability.
3. **Faster Debugging and Troubleshooting:** Unit tests act as a safety net when making changes to the codebase. They provide a quick and automated way to verify that existing functionality remains intact after modifications. When a test fails, it immediately highlights the specific unit or functionality that is affected, enabling developers to isolate and fix the issue more efficiently. This accelerates the debugging and troubleshooting process, saving valuable time and effort.
4. **Continuous Integration and Deployment:** Early unit testing is a fundamental component of continuous integration and deployment (CI/CD) pipelines. By automating the execution of unit tests, developers can ensure that changes introduced to the codebase do not break existing functionality. This practice fosters a culture of frequent and incremental code integration, leading to faster and more reliable software delivery.
5. **Documentation and Code Understanding:** Well-written unit tests serve as living documentation that describes the expected behavior of the code. They provide valuable insights into the functionality and usage of various units, making it easier for developers to understand and work with unfamiliar codebases. This documentation aspect of unit testing promotes collaboration among team members and enables knowledge sharing.
6. **Maintainability:** 
One of the main advantages having unit test form the beginning is, that software developers (not only the author itself) easily can maintain, refactor and improve the code without the fear to deploy new bugs to the code.

In conclusion, early unit testing of C and C++ source code is a powerful practice that enhances code quality, reduces bugs, improves debugging efficiency, enables continuous integration, and instills confidence in refactoring. By embracing unit testing from the outset, developers can lay a solid foundation for building robust and maintainable software systems.


# 3. Stubbing vs. Mocking

Stubbing and mocking are techniques used in unit testing to simulate dependencies and control the behavior of external components. These techniques are essential for isolating the code under test and ensuring it functions correctly in different scenarios. Stubbing involves creating a simplified version of an object or component that provides predefined responses. It is used to isolate the code under test from external dependencies. <br>Mocking, on the other hand, involves creating objects that simulate the behavior of real objects and verify interactions between components. It helps test complex scenarios and ensure the correct interactions occur. In summary, stubbing replaces real objects with simplified versions, while mocking creates objects to simulate behavior and verify interactions.

One of the significant advantages of using a mocking framework is that the expected interactions with other software components are described in the test cases themselves. This is why the unit test strategy implements mock technology only. Each software should provide the unit tests and the mock class for the unit tests of other software components. Providing mocks does not make much sense for some software units. Instead, the original interfaces will be used. For example, in the case of simple calculations, it is easier not to mock the interfaces and take the original ones.

## 3.1 Mocking internal interfaces

Mocking internal interfaces is generally not recommended as it can make future refactoring more difficult. Mocking internal functions would expose the internal behavior of software units in their test cases, creating dependencies that may hinder code changes. However, there are cases where mocking internal interfaces can help reduce the complexity of test cases. In such situations, carefully considering the trade-offs and potential impacts on maintainability is essential.

An appropriate instrumentation of the source code under test is required to mock internal functions.

You can use the `MOCKI(function-name)(paramA, paramB,...)` macro inside the code under test to mock internal interfaces. This macro allows you to simulate the behavior of internal functions during testing. However, it is relevant to note that mocking internal interfaces may limit the flexibility to refactor the code in the future, as any changes to these interfaces would also require updating the corresponding mock implementations.

Consider mocking internal interfaces sparingly and only when necessary to simplify test cases. Prioritize designing code that is easily testable without relying heavily on internal mocking. This approach will help maintain the flexibility to refactor and improve the codebase in the long run.

## 3.2 Mocking own interface functions

It is sometimes beneficial to call the original interface instead of mocking external interfaces to reduce the complexity of test cases. However, this approach partially reveals the internal behavior of software units in their test cases. When mocking its interface functions, the source code under test needs to be instrumented accordingly using the `MOCKE(function-name)(paramA, paramB,...)` macro. Further details will be described in the following sections.

## 3.3 'Un-Mock' the call of external interfaces

In general, it is common practice to mock external interfaces in unit testing. However, there are situations where calling the original interface instead of mocking it can be beneficial. This approach may simplify test cases or provide better testing of the interaction between software units.

# 4. Test architecture

## 4.1 folder structure and files

The following folder structure should be used to have a unified test setup. In this case, you can use a script to create mock files. Test dummies for the interface functions have also been created. SW units often need private headers. Those headers usually contain interfaces whose implementation is platform-specific, e.g., in MCAL. Other setups are possible, but more manual adjustments are required in this case.
```
<unit>
├──── <unit>.c                     implementation of <unit>
├──── <unit>.h                     public definitions and declarations of <unit>
├──── <unit>_priv.h                private definitions and declarations (optional)
│
├──── test
│    ├──── <unit>.dtest.cpp        test case implementation, dummy test cases can be generated for all interfaces
|    ├──── <unit>.mock.cpp         mock implementation, provides mocks for other units (code generator)
│    ├──── <unit>.mock.h           mock header (code generator)
|    ├──── <unit>_priv.mock.cpp    mock implementation, (generated, optional)
│    └──── <unit>_priv.mock.h      mock header, (generated, optional)
```
### 4.1.1 Implementation of <unit>.mock.h

The following introduces the implementation of <unit>.mock.h in the example of the unit madc. When this is included instead of madc.h the mocks will be used instead of the original functions. The mocks are located in the namespace <unit>_m. 
``` CPP
#ifndef MADC_MOCK_H
#define MADC_MOCK_H

#include <doctest/doctest.h>                        // include doctest must be before trompeloeil.hpp !!
#include <trompeloeil/doctest/trompeloeil.hpp>      // include the mocking framework

namespace madc_m                                    // mocking namespace for madc
{
    #include "mcal/madc.h"                          // include madc.h into the mocking namespace 

    class madc_mock_class                           // mocking class for madc
    {
    public:
        /* mocks "void MADC_enable(MADC_Handle *handle)", MAKE_MOCK1 means 1 params */
        MAKE_MOCK1(MADC_enable         , void          ( MADC_Handle *handle ));
        /* mocks "uint16_t MADC_readSample(MADC_Handle *handle, const MADC_Channel *channel)", MAKE_MOCK2 means 2 params */
        MAKE_MOCK2(MADC_readSample     , uint16_t      ( MADC_Handle *handle, const MADC_Channel *channel ));
        ...
        ...
    };

    extern madc_mock_class madc_mock; // external declaration of madc mocking instance
}

/* normally 'using' is quite ugly in an header. Here it helps to use the mocks as simple as possible */
using namespace madc_m;

#endif  // MADC_MOCK_H
```
### 4.1.2 Implementation of <unit>.mock.cpp

``` CPP
#include "madc.mock.h"  // include the mock declarations (and interface declarations in madc_m namespace)

namespace madc_m        // continue with mocking namespace for madc
{
    madc_mock_class madc_mock; // definition of madc mocking instance
    // mock wrapper for MADC_enable
    void MADC_enable(struct MADC_Handle *handle)
    {   // call the mock
        madc_mock.MADC_enable(handle);
    }

    // mock wrapper for MADC_readSample
    uint16_t MADC_readSample(struct MADC_Handle *handle, const struct MADC_Channel *channel)
    {   // call the mock
        return madc_mock.MADC_readSample(handle, channel);
    }
    ...
    ...
}
```

### 4.1.3 Implementation of <unit>.dtest.cpp

The following code demonstrates a typical unit test implementation for a unit written in C using the doctest framework. The namespace <unit>_test is used for the location of the code under test and the test cases. It is possible to create one test suite for each software unit or split a unit into several test suites.
In this example, each interface has one test case. A test case may have sub-cases to test different interface aspects, and even a sub-case may have further sub-cases. Creating the test cases independently is fundamental, as they may be executed arbitrarily.
Please note that this example does not include the use of mocks. The use of mocks will be explained in another chapter.


```CPP
#include <doctest/doctest.h>
// Note: The order of mock includes is important, the mock of lowest layer has to be included first 
#include "mcal.mock.h"          // include the mocks of external mcal interfaces
#include "madc_priv.mock.h"     // include the mocks of private madc interfaces
#include "msup.mock.h"          // include the mocks of external msup interfaces
#include "madc_mc.h"            // include platform specific definitions and types for madc   

using trompeloeil::_;

namespace madc_test // the code under test and the test cases are in the namespace <unit>_test
{   
    /*---- START: mocking own internal (static) and external interface functions (optional!) -----*/
    /* Hint: Own functions can be mocked to reduce the complexity of the test cases.
     *       Also for instance endless loops might be avoided by mocking own functions.
     *       If there is not a really good reason mocking of own functions should be avoided.
     *       Mocking internal functions makes refactoring more difficult, as the test cases have
     *       to be adapted when the internal functions are changed.
     */
    #include "mcal/madc.h"
    /* To mock internal functions the following macros are used in the code under test:
     * MOCKI(function-name)(paramA, paramB,...)
     * The parameters are the same as in the original function.
     * For own external functions the macro MOCKE(function-name)(paramA, paramB,...) is used.
     */
    class
    {/* 1 param    function name                   return type   parameter list */
        MAKE_MOCK1(MADC_isConversionCorrupted    , bool        ( const MADC_Handle *handle ));
    }int_mock; // the instance int_mock contains the mocks of internal static functions to be mocked, by default they are not mocked

    class
    {/* 2 params   function name                   return type   parameter list */
        MAKE_MOCK2(MADC_readSample               , uint16_t    ( MADC_Handle *handle, const MADC_Channel *channel ));
    }ext_mock;

    /**** END: mocking own functions **********************************************/



    #include "mcal/madc.c" // include the source file to be tested

    void test_setup(void)
    {
        /* Optional interface 
           Do here here everything to prepare the tests.
           E.g. common variable initialization.
           Note: The final test result shall be independent of the test order
                 This is why you should prepare each test case.
        */
    }   

    TEST_SUITE("madc") // the test suite for the unit madc
    {
        // here you can declare variables and constants you need in all test cases within this test suite.

        TEST_CASE("MADC_init") // test case for interface MADC_init
        {
            // you can declare here variables and constants you need in this test case (including sub-cases)
            test_setup(); // call the test setup function, it is called before each sub case

            SUBCASE("check error handling")
            {   /* here all checks for error handling are done */
                SUBCASE("init error") // SUBCASE() can be used hierarchically
                {   
                    MCAL_Error ret = MADC_init(...); // call the function under test
                    CHECK(ret == MCAL_ERROR);    
                }
                SUBCASE("config error")
                {   
                    MCAL_Error ret = MADC_init(...); // call the function under test
                    CHECK(ret == MCAL_ERROR);    
                }
            }

        } // test case MADC_init
        TEST_CASE("MADC_getStatus") // the test suite for interface MADC_getStatus
        {
            test_setup(); // call the test setup function, it is called before each sub case
            ...
        }
    } // test suite madc
} // namespace
     
```

## 5 Instrumentation of code under test

## 5.1 Header instrumentation

The test architecture requires a slight modification to the header files, which allows the headers to be included in different namespaces.
```C
#if defined(__cplusplus) && !defined(MOCK_IT)
extern "C" {
#endif
...
...
#if defined(__cplusplus) && !defined(MOCK_IT)
}
#endif
```
The `extern "C" { ... }` is only required for normal build. For the test build the namespace is defined by the including file.

## 5.2 Source instrumentation

In many cases no instrumentation of source code is required. As mentioned in some cases the test cases are more simple when internal functions are mocked. In this case the instrumentation of the source code is required. The instrumentation is done by the following macro: `MOCKI(function-name)(paramA, paramB,...)`. The following example shows the instrumentation:
```C
do
{
    ...
    ...
} while(MOCKI(MADC_isConversionCorrupted)(handle));
```
Without instrumentation the test case would run into an endless loop or the loop will be left after first iteration.
Own external interfaces can be mocked by the macro `MOCKE(function-name)(paramA, paramB,...)`. The following example shows the instrumentation:
```C
enum MCAL_Error MADC_readSampleWithValidation(struct MADC_Handle *handle, const struct MADC_Channel *channel, uint16_t *result)
{
    *result = MOCKE(MADC_readSample)(handle, channel);

    return handle->sampleValidity;
}
```
This instrumentation simplifies the test cases of interface MADC_readSampleWithValidation. Without this instrumentation the test cases for MADC_readSample need to be repeated in the test cases of MADC_readSampleWithValidation.

Note: The instrumentation of source code is not recommended. It makes refactoring more difficult, as the test cases have to be adapted when the internal functions are changed.

## 6 Code generator

Especially the creation of mock files is quite mechanically and can be done by a code generator. The code generator is a python script which reads the header files and creates the mock files.

The code generator is triggered by the target **gen_doc_test**. To generate the mock files and the test dummy the for e.g. madc the following command is used:
```bash
make gen_doc_test header=MSP/mcal/madc.h
```
If you also need mocks for static inline interfaces the following command is used:
```bash
make gen_doc_test header=MSP/mcal/madc.h inline_mock=true
```
The build target gen_doc_test will generate the files madc.mock.h, madc.mock.cpp and madc.dtest.cpp in the folder MSP/mcal/test. By default existing files will be overwritten, but for each generation a new backup folder with the date and time of the generation is created. The backup folder is located in the same folder as the generated files.

**Note**: The code generator does not include a real C/C++ parser. It is a simple text parser. The code generator can handle the most common cases. In the case of complex header files, the code generator might fail. In this case, the mock files must be created or adapted manually. 


## 7 MCU Framework Build process

## 7.1 Build setup for simple units

In many cases the adaptation of build process is quite simple. The following steps are required. Instead of using the make variable *lib_src* the make variable *ut_lib_src* is used for common code to specify the objects. For external and application code use the make variable *ut_src* instead of *src*. 
```makefile
# file: common/prog/math/build.mak
# in case of unit test math/test/mathDiv.dtest.cpp will be compiled
ut_lib_src += math/mathDiv.c
```
## 7.2 Build setup for components including mocking support

In more complex situations the make variable **std_variant** is used, it is defined with **true** when the unit test in not active:
```makefile
# file: common/prog/MSP/mcal/build.mak
ifeq ($(unit_test_variant), true)
...
...
else
    # build objects for unit tests
    lib_src    += MSP/mcal/msup.c
    lib_src    += MSP/mcal/test/msup.mock.cpp
    lib_src    += MSP/mcal/test/madc.mock.cpp
    lib_src    += MSP/mcal/test/madc_priv.mock.cpp
    lib_src    += MSP/mcal/test/mcal.mock.cpp
    lib_src    += MSP/mcal/test/mcom.mock.cpp
    ut_lib_src += MSP/mcal/madc.c
    ut_lib_src += MSP/mcal/mcom.c
    ut_lib_src += MSP/mcal/mcal.c
endif
```	
This approach is applicable to most sw units and components.

## 7.3 Build setup in detail

In some cases it is required to adapt the build process in detail.
The CRC-Unit of the MCU Framework has a unusual structure, for this unit the settings need to do manually. The following code snippet shows the adaptation of the build process for the CRC-Unit:
```makefile
# file: common/prog/ped_fw/stdcrc/build.mak
# Note: comments after variable definitions are not allowed in makefiles, here it is used for inline explanation 
ifeq ($(unit_test_variant), true)                                            # use this switch to identify a unit test variant
    ut_test_src += ped_fw/stdcrc/test/StdCrc$(ut_test_file_extension).cpp    # add StdCrc.dtest.cpp to the list of test sources
                                                                             # Hint: the test file does not include the source file 
    ifeq ($(platform),Intel)                                                 # Hint: there are also tests for target system
        ut_cov_filter += $(sdk_prog_path)/ped_fw/stdcrc                      # tells the coverage tool to include the source into coverage report
        StdCrc_copt    =--coverage                                           # sets the coverage compiler option for StdCrc.c
        CrcCom_copt    =--coverage                                           # sets the coverage compiler option for StdCom.c
    endif
    lib_src += $(addprefix ped_fw/stdcrc/,CrcCom.c $(stdcrc_folder)StdCrc.c) # add StdCrc.c and CrcCom.c to the list of sources
endif
```
When a software unit has the simple structure (<unit>.c + <unit>.h) this all is not required you simply need to add your unit to the list `ut_lib_src`. The compiler option for coverage and the option for coverage tool are set automatically. By default the file `test/<unit>.dtest.cpp` is compiled only, it is expected that the test file includes the source file <unit>.c. If the source file is not included in the test file, the source file has to be added to the list `lib_src`.


## 8 Quick start for mocking

As already mentioned the mocking is a powerful tool. It is used to simulate the behavior of external components and to verify the interactions between components. A big advantage is, that the expected interactions are described in the test cases itself. The code generator is used to create the mock files, so the you can concentrate on the test cases.

In the following the quick start for mocking is described. The example is based on the unit abs_spi_flash.
THe following mocking implementation maps the interfaces to the mocking class. To reduce the complexity of the test cases the c-arrays are mapped to vectors. 
```CPP
/* file: common/prog/devices/spi_flash/test/abs_spi_flash.mock.cpp */
#include "spi_flash.mock.h"
#include <vector>
#include <algorithm>


SpiFlashCfg flashCfgMock;  // create an instance of the mock class
 
 /*  here it is enough to have the mocks in global namespace, as this interfaces does not need to be tested,
  *  it is the interface to the hardware.
  */
extern "C" {
    void ABSSF_initInterface()
    {
        flashCfgMock.ABSSF_initInterface();
    }
    
    void ABSSF_readData(uint32_t size, uint8_t* buffer)
    {
        vector<uint8_t> vbuf(size);
        flashCfgMock.ABSSF_readData(size, vbuf);    // vector used in case of c-array, to simplify the test cases
        std::copy_n(vbuf.begin(),size, buffer);     // copy the vector to the c-array
    }

    void ABSSF_writeData(uint32_t size, const uint8_t* buffer)
    {
        vector<uint8_t> vbuf;
        vbuf.assign(buffer, buffer + size);         // copy the c-array to the vector
        flashCfgMock.ABSSF_writeData(size, vbuf);   // vector used in case of c-array, to simplify the test cases
    }

    void ABSSF_selectChip(void)
    {
        flashCfgMock.ABSSF_selectChip();
    }
    
    void ABSSF_unselectChip(void)
    {
        flashCfgMock.ABSSF_unselectChip();
    }

}
```
The former defined interfaces are used in the test cases. The following test case shows the usage of the mocks. The test case is part of the test file `abs_spi_flash.dtest.cpp`.

```CPP
/* file: common/prog/devices/spi_flash/test/abs_spi_flash.dtest.cpp */
#include <doctest/doctest.h>
#include "spi_flash.mock.h"     // include the mocks for HW access interfaces
#include "../abs_spi_flash.c"   // include the source file to be tested
#include <vector>
#include <algorithm>

using trompeloeil::_;  // for matching any value
using trompeloeil::eq; // matching equal values
using std::vector;


TEST_SUITE("Spi-Flash")
{
    trompeloeil::sequence seq;    // needed to define the (in order) sequence of expected calls
    /* the following variables are used in different test cases */
    vector<uint8_t> cmd(1);
    vector<uint8_t> cmdWrEnable{ABSSF_cmdWrEnable};

    TEST_CASE("ABSSF_init: Do the initialization, and read ID form device") // testing interface ABSSF_init()
    {
        const uint8_t Memory_Type    = 0x20;
        const uint8_t Memory_density = 0x15;  //16MBit
        vector<uint8_t> rdIdent{ABSSF_Macronix, Memory_Type, Memory_density}; // this is the ident response of the device
        cmd[0]= ABSSF_cmdRdId;                                                // the command to read the ident from the device

    /* following expected calls in this order (sequence:seq)
     * The sequence simulates the behavior of the SPI-Flash including hardware interface during initialization
     */
        REQUIRE_CALL(flashCfgMock, ABSSF_initInterface())            // 1st expected call: init the interface    
            .IN_SEQUENCE(seq);                                    
        REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())               // 2nd expected call: select the chip
            .IN_SEQUENCE(seq);                                    
        REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmd.size(),cmd))  // 3rd expected call: write the command to the device
            .IN_SEQUENCE(seq);                              
        REQUIRE_CALL(flashCfgMock, ABSSF_readData(rdIdent.size(),_)) // 4th expected call: read the ident from the device
            .LR_SIDE_EFFECT(_2 = rdIdent)                            // copy the ident structure to the buffer (_2 = second parameter)
                                                                     // thanks to vector this is pretty simple
            .IN_SEQUENCE(seq);                              
        REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())             // 5th expected call: unselect the chip
            .IN_SEQUENCE(seq);
    /* sequence end */

        ABSSF_init();                                          // call the function under test
        CHECK(ABSSF_jedecID.manufacturer == ABSSF_Macronix);   // check the fist member in the jedecID structure
        CHECK(ABSSF_jedecID.type         == Memory_Type);      // check the fist member in the jedecID structure
        CHECK(ABSSF_jedecID.capacity     == Memory_density);   // check the fist member in the jedecID structure
    }   // Note: with the end of the test case the expected calls are verified!

    TEST_CASE("ABSSF_isBusy") // testing interface ABSSF_isBusy()
    {
        const uint8_t busy_mask = 0x01;
        vector<uint8_t> rdStatus{ABSSF_cmdRdStatusReg};
        vector<uint8_t> status{busy_mask};

        SUBCASE("ABSSF_isBusy: busy mask set")
        {
        /* following expected calls in this order (sequence:seq) */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                        // 1st expected call: select the chip
                .IN_SEQUENCE(seq);                                    
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(rdStatus.size(),rdStatus)) // 2nd expected call: write the rdStatus command to the device
                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_readData(status.size(),_))           // 3rd expected call: read the status from the device
                .LR_SIDE_EFFECT(_2=status)    /* copy busy_mask to buffer */
                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                      // 4th expected call: unselect the chip
                .IN_SEQUENCE(seq);
        /* sequence end */
            CHECK(ABSSF_isBusy() == true); // check the return value, true as the busy mask is set
        }
        // other sub-cases might be added here e.g. busy mask not set
        ...
    } // test case ABSSF_isBusy
...
}
```

The code snippet demonstrates the use of mocking close to hardware. Typically, drivers like these are tested in integration tests, but it can be challenging to test exceptional situations such as writing and erasing errors. Additionally, it may not be possible to test devices from different vendors as they may not be readily available. Mocking allows the simulation of such scenarios.

The examples illustrate the seamless integration of the trompeloeil mocking framework into doctest. For more information on trompeloeil, please refer to the [trompeloeil-manual](../../../trompeloeil/README.md). You can also find several examples within the MCU Framework, such as for the MCAL in the folder `common/prog/MSP/mcal/test` or the spi_flash in the folder `common/prog/devices/spi_flash/test`.





