# Debug_Extended Component - User Manual


The debug_extended component provides data logging, processing of incoming debug data and (very related to 
dealing with bugs and data logging) the handling of exceptional errors.

Subject of this document is how to use the debug_extended component from the point of view of the application.

<.pn>


## First steps - Use the debug_extended component

To use the debug_extended component, the following Framework components have to be part of your project:  
  - MCAL
  - debug  
  - debug_extended  
  - mem_utility  

Note:  
Currently, not all platforms are supported by the MCAL.  

Note:  
All functions below are non-blocking and reentrant.


### First steps for logging debug data (variables as text terminal data)
    
**Include the following header file:**

    #include "debug_extended/api_cfg.h"
    
**Call the initialization function:**

    DBGX_init();

**Opt. 1: Call non-filtered text application functions "DBGX_log..._NonFiltered":**

    DBGX_logStr_NonFiltered("Here we go.");
        //Puts out "Here we go."

    uint32_t MyDetailedInteger = 24;
    DBGX_logStrIntLocatFull_NonFiltered("Int [hex] ", &MyDetailedInteger, DBGX_UNSIGNED_HEXADECIMAL);
        //Puts out "Int [hex] 24 /debug_extended.c 189 DBGX_testNonFilteredComposedFunctions"
    
    ...

**Opt. 2: Call filtered text application functions "DBGX_log...":**

*Setup your filters:*  
(1) Create your file "filter".json according to the template of the debug_extended component: "/debug_extended/dbgx_filters.json"
    and add it to your makefile, like:  
    
    code_gen_cfg += $(app_prog_path)/debug_extended/dbgx_filters.json

(2) Fill in a valid domain name and add it to your makefile:
    
    dbgx_activated_filters = <domain_name>

(3) Define scenarios and activate / deactivate your filters.


*Call the filtered text application functions "DBGX_log..." (for variable values and infos as ASCII characters):*

    DBGX_logStr_SCN_POWER(MyString);          // POWER is scenario filter
        // Will put out the contents of MyString, if scenario POWER is activated
    
    DBGX_logStr_INFO("Mode is pre-heating");  // INFO is level filter
        // Will put out "Mode is pre-heating", if level INFO is activated
    
    ...


### First steps for printing out debug data: Segger RTT interface**

The fastest way to make the debug output visible, is to use the Segger RTT interface. 
This interface requires a Segger J-Link programming adapter and works with all Cortex-M devices.

The Segger RTT interface is already activated per default, so there is no need for any adaption.

The Segger RTT interface will be accessed by using a telnet connection (e.g. via Putty, J-Link RTT Logger / Viewer, ...) with the following characteristics:

    - Host/IP-Address:  localhost
    - Port              19021


### First steps for handling of exceptional errors
    
The handling of exceptional errors means logging the error and calling a user-defined error solving function. 
The error can also be catched and inspected using a break point at a central program location in the debug component.
    
**Include the following header file in your application file:**

    #include "debug_extended/api_cfg.h"

**Call the initialization function:**

    DBGX_init();

**Define your unique exceptional error code**

    // In your according header file
    #define DBGX_ERR_IDX_HEATING        (0x00000002 | DBGX_ERR_SOLVING_RESET)
        // Example with reset for error solving

**Call the macros for handling of exceptional errors:**

    DBGX_handleErr(DBGX_ERR_IDX_HEATING);
        // DBGX_ERR_IDX_HEATING is an application defined unique exceptional error code
        // Calling e.g. in a default branch of a switch-case-instruction
    
    ...

**Set a break point for working in debugger:**

Inside DBGX_handleErrAfterErrOutput(), see the comments there. 
Using the break point you can inspect easily the error code and, if used, the error parameter.


### First steps for using data input

The data input should be used for sending data from the user PC to the target HW.
While the simple debug interface acts non-cooperative (~busy waiting), the DBus and UART interface act cooperative.

**Include the following header file in your application file:**

    #include "debug_extended/api_cfg.h"

**Call the initialization function:**

    DBGX_init();

**Call the macros for handling of exceptional errors:**

    (void)DBGX_registerInputCallbackFunction(&DBGXTA_testStringInput_CB);
        // Register your user defined callback function for the input data

    int32_t status = DBGX_getString(array, (sizeof(array) / sizeof(array[0])));
        // returns the length of the input string, that was read.
        // -1 in case of interface error
        // -2 in case of a cooperative input interface. (Callback mechanism will return signal for read data)
        
        // Param1: First parameter: User initialized array for saving the interface data
        // Param2: Maximum length of the input data
    
<.pn>


## Usage of the API layer
    
### Overview over the API functions

There are two types of text protocol functions.
While the "non-filtered" functions will return their output unconditionally, 
the “filtered” api functions allow a further selection of the calling situation.

      DBGX_sendData         DBGX_logChar_NonFiltered
                            DBGX_logStr_NonFiltered
                            DBGX_logInt_NonFiltered                 (*1)
                            DBGX_logIntArr_NonFiltered              (*1)
                            DBGX_logTime_NonFiltered
                            DBGX_logLocat_NonFiltered
                            DBGX_logStrStr_NonFiltered
                            DBGX_logStrInt_NonFiltered              (*1)
                            DBGX_logStrIntArr_NonFiltered           (*1)
                            DBGX_logStrStrLocat_NonFiltered
                            DBGX_logStrStrLocatFull_NonFiltered
                            DBGX_logStrIntLocat_NonFiltered         (*1)
                            DBGX_logStrIntLocatFull_NonFiltered     (*1)

                            DBGX_logChar_<Filters>
                            DBGX_logStr_<Filters>
                            DBGX_logInt_<Filters>                   (*1)
                            DBGX_logIntArr_<Filters>                (*1)
                            DBGX_logTime_<Filters>
                            DBGX_logLocat_<Filters>
                            DBGX_logStrStr_<Filters>
                            DBGX_logStrInt_<Filters>                (*1)
                            DBGX_logStrIntArr_<Filters>             (*1)
                            DBGX_logStrStrLocat_<Filters>
                            DBGX_logStrStrLocatFull_<Filters>
                            DBGX_logStrIntLocat_<Filters>           (*1)
                            DBGX_logStrIntLocatFull_<Filters>       (*1)

According to the exceptional error handling and its callback mechanism, we provide following api functions:

      DBGX_handleError      DBGX_registerErrorSolvingFunction
                            DBGX_handleErr
                            DBGX_handleErrWithParam
                            DBGX_handleErrIf
                            DBGX_handleErrWithParamIf
                            DBGX_readFirstError
                            DBGX_readLastError

At least, the data input will be handled with the following functions:

      DBGX_receiveData      DBGX_registerInputCallbackFunction
                            DBGX_getString


(*1) This function can be called with and without a format parameter.
     In case no format parameter is used, the default format parameter DBGX_DEFAULT_FORMAT, defined in api_cfg.h, is used for the output formatting.
     In case you use the explicit format parameter, please have a look at the corresponding chapter.
     
     Note: While calling the function without explicit format parameter is more intuitive and more memory efficient,
     calling the function with the explicit format parameter gives better possibilities in configuring the output.


### Runtime of the text protocol related api functions

The execution time of the text protocol related api functions was measured on the following derivates:  
  - STM32G071 @64MHz
  - STM32G473 @170MHz  

The implementation is part of the dbgx_test_app.

The following times are rounded up to the nearest increment of 5 and will vary with the length of the generated debug output.
As the DBus and UART interface correlate to the baudrate and amount of data, there are no measurements for these two interfaces.


    STM32G071 @64MHz          |           Interface SeggerRTT         |              Interface RAM            |
                              |                                       |                                       |
    DbgAttributes             |      enabled      |     disabled      |      enabled      |     disabled      |
                              |                   |                   |                   |                   |
    Filters                   |   off   /   on    |   off   /   on    |   off   /   on    |   off   /   on    |
                              |                   |                   |                   |                   |
    [time in microseconds]    |                   |                   |                   |                   |
    --------------------------|-------------------|-------------------|-------------------|--------------------
    DBGX_logChar              |   60    /   80    |   15    /    17   |   55    /   70    |   15    /   15    |
    DBGX_logStr               |   60    /   75    |   20    /    25   |   60    /   70    |   15    /   20    |
    DBGX_logInt               |   65    /   80    |   25    /    25   |   60    /   75    |   20    /   25    |
    DBGX_logIntArr            |   100   /   115   |   60    /    60   |   95    /   110   |   55    /   60    |
    DBGX_logTime              |   95    /   105   |   55    /    55   |   90    /   105   |   60    /   65    |
    DBGX_logLocat             |   100   /   115   |   55    /    60   |   95    /   105   |   50    /   55    |
    DBGX_logStrStr            |   75    /   90    |   35    /    40   |   70    /   85    |   30    /   35    |
    DBGX_logStrInt            |   90    /   100   |   55    /    50   |   85    /   95    |   45    /   45    |
    DBGX_logStrIntArr         |   125   /   135   |   85    /    85   |   120   /   135   |   80    /   80    |
    DBGX_logStrStrLocat       |   100   /   115   |   60    /    60   |   95    /   110   |   55    /   55    |
    DBGX_logStrStrLocatFull   |   160   /   170   |   115   /   115   |   150   /   165   |   110   /   110   |
    DBGX_logStrIntLocat       |   125   /   150   |   85    /    85   |   120   /   135   |   80    /   80    |
    DBGX_logStrIntLocatFull   |   175   /   190   |   135   /   165   |   170   /   185   |   125   /   130   |

-

    STM32G473 @170MHz         |           Interface SeggerRTT         |              Interface RAM            |
                              |                                       |                                       |
    DbgAttributes             |      enabled      |     disabled      |      enabled      |     disabled      |
                              |                   |                   |                   |                   |
    Filters                   |   off   /   on    |   off   /   on    |   off   /   on    |   off   /   on    |
                              |                   |                   |                   |                   |
    [time in microseconds]    |                   |                   |                   |                   |
    --------------------------|-------------------|-------------------|-------------------|--------------------
    DBGX_logChar              |   10    /   15    |   5     /   5     |   10    /   10    |   5     /   5     |
    DBGX_logStr               |   10    /   15    |   5     /   10    |   10    /   15    |   5     /   5     |
    DBGX_logInt               |   10    /   15    |   10    /   10    |   10    /   15    |   5     /   10    |
    DBGX_logIntArr            |   20    /   20    |   15    /   15    |   15    /   20    |   15    /   15    |
    DBGX_logTime              |   15    /   15    |   10    /   10    |   15    /   15    |   10    /   10    |
    DBGX_logLocat             |   20    /   20    |   15    /   15    |   20    /   20    |   15    /   15    |
    DBGX_logStrStr            |   15    /   15    |   10    /   10    |   15    /   15    |   10    /   10    |
    DBGX_logStrInt            |   20    /   20    |   15    /   15    |   20    /   20    |   15    /   15    |
    DBGX_logStrIntArr         |   25    /   25    |   20    /   20    |   25    /   25    |   20    /   20    |
    DBGX_logStrStrLocat       |   20    /   25    |   15    /   15    |   20    /   25    |   15    /   15    |
    DBGX_logStrStrLocatFull   |   35    /   35    |   30    /   30    |   35    /   35    |   30    /   30    |
    DBGX_logStrIntLocat       |   30    /   30    |   25    /   25    |   30    /   30    |   25    /   25    |
    DBGX_logStrIntLocatFull   |   40    /   45    |   35    /   35    |   40    /   40    |   35    /   35    |


Note:  
Be aware of the time consumption of these api functions.
Especially in a fast threaded rtos environment, these api functions may lead to execution timeouts, if used too generously.

### Unfiltered api functions

The unfiltered text protocol functions are a very simple way to generate debug data. 

#### Examples: Unfiltered api functions

    DBGX_logChar_NonFiltered('a');
        // Puts out "a"
    
    DBGX_logStr_NonFiltered("Here we go.");
        // Puts out "Here we go."
    
    uint16_t MyVariable = 15;
    DBGX_logInt_NonFiltered(MyVariable);                            // more intuitive and more memory optimized
    DBGX_logInt_NonFiltered(MyVariable, DBGX_UNSIGNED_HEXADECIMAL); // more specializable
        // Puts out "F"
    
    int8_t MyArray[] = {-1, 0, 1, 2, 3};
    DBGX_logIntArr_NonFiltered(MyArray, 5);                         // more intuitive and more memory optimized
    DBGX_logIntArr_NonFiltered(MyArray, 5, DBGX_SIGNED_DECIMAL);    // more specializable
        // Puts out "-1 0 1 2 3"
    
    DBGX_logTime_NonFiltered();
        // Puts out the time in microseconds, e.g. "98368932"
    
    DBGX_logLocat_NonFiltered();
        // Puts out file and line number, e.g. "TEMP_Temperature.c, 292"
    
    DBGX_logStrStr_NonFiltered("Hello ", "world !!!");
        // Puts out "Hello world !!!"
    
    uint16_t MyInteger = 24;
    DBGX_logStrInt_NonFiltered("My integer in HEX = ", MyInteger, DBGX_UNSIGNED_HEXADECIMAL);
        // Puts out "My variable in HEX = 18"
        
    int8_t MyIntArray[] = {-1, 0, 1, 2, 3};
    DBGX_logStrIntArr_NonFiltered("My integer array in HEX: ", MyIntArray, 5, DBGX_SIGNED_DECIMAL);
        // Puts out "My integer array in DEC: -1 0 1 2 3 "
        
    DBGX_logStrStrLocat_NonFiltered("Source ", "function: ");
        // Puts out "Source function: DBGX_testNonFilteredComposedFunctions"

    DBGX_logStrStrLocatFull_NonFiltered("Example ", "path: ");
        // Puts out "Example path: /debug_extended.c 189 DBGX_testNonFilteredComposedFunctions"

    uint16_t MyBasicInteger = 24;
    DBGX_logStrIntLocat_NonFiltered("Integer in hex ", MyBasicInteger, DBGX_UNSIGNED_HEXADECIMAL);
        // Puts out "Integer in hex 18 DBGX_testNonFilteredComposedFunctions"

    uint16_t MyDetailedInteger = 24;
    DBGX_logStrIntLocatFull_NonFiltered("Int [hex] ", MyDetailedInteger, DBGX_UNSIGNED_HEXADECIMAL);
        // Puts out "Int [hex] 18 /debug_extended.c 189 DBGX_testNonFilteredComposedFunctions"


### Filtered api functions

Besides the unconditional logging of variables using non-filtered text application functions, 
the debug_extended component provides a powerful filter mechanism for the debug output. 
Filtering means the individual (static or dynamic) activation or deactivation of logging for the debug output calling locations


#### Structure of the filter mechanism

The filter mechanism of the debug_extended component works as follows:

     +---------------------------------+       +---------------------------------+
     | "filter".json (of your module)  |       | "filter".json (of module xy)    |
     |                                 |       |                                 |       ...
     | Contains module specific filters|       | Contains module specific filters|
     +---------------------------------+       +---------------------------------+
              |                                               |
              +-----+-----------------------------------------+------------------------- ...
                    |
                    | - - - - - - - make with
                    |               generic code generator
                    V                       |
     +---------------------------------+    |      +------------------------------------+
     | dictionary: dbgx_filters        |    |      | filters_aut.h                      |
     |                                 |    |      |                                    |
     | Summarizes all filters (level,  |  -------> | c header file with function macros |
     | scenario, static, dynamic)      |           | according to filter configuration  |
     +---------------------------------+           +------------------------------------+
                                                         |
                                                         |
                                                         |
     +--------------------------------------+            |
     |  Macro calling at source code        |           v/
     |  locations                           |           /
     |                                      |          /      Debug output
     |  DBGX_log...(CallingFilter, ...);    |-------- + ------------------------->
     +--------------------------------------+


#### Filter categories

The filtering covers two different filter categories:

   - The **level filter** is a predefined and unchangeable classification of the api functions.
     There are 3 levels, which can be used and be centrally activated / deactivated:  
       –> WARN  (warnings)  
       –> ERROR (error)  
       –> INFO  (information)  

   - The **scenario filter** is a user definable filter mechanism, which allows a classification of the api function usage. 
     Here, the user classifies his functions into a kind of chapters and can globally activate or deactivate the filter condition. 
     In the example from below, we created the following scenarios: 
       –> DBGX_CLOCK  
       –> DBGX_TOUCH  
       -> ...

#### Filter configuration file

Each module hold its own filter configuration file, which defines all filters and its filter flags for that specific module.
Follow these instructions to set up the configuration file:

**Create configuration file**  
Create the file "filter".json, according to the template of the debug_extended component: "/debug_extended/dbgx_filters.json".
Give it a resonable "domain"-name and add it to your makefile:  
     code_gen_cfg += $(app_prog_path)/debug_extended/dbgx_filters.json

Note:  
Push the file "filter".json and the make configuration into your git repository, so it is included in your release version.
It will be necessary for the software layers above you, to compile the software project without error.

**Activate configuration file**  
In order to activate your filter configuration domain to be actively used, add it to the following list:
    dbgx_activated_filters = <domain_name>

Note:  
This activation of the filter domain should **not be included** in any of the release versions of your software.
Therefore, we strongly recommend **not to add this configuration into a versioned .mak file**.
In case you need this filter domain activated in the ci test, just add your filter module via the build commands.

#### Multiple filter configuration files - Hierarchical order  

There should be many module specific "filter".json files in your software project.
All these files are composed by make and are processed by the generic code generator.

Make automatic generates the file "filter_aut.h", which holds all function macros for the debug_extended component.
This file will be automatic (re)created in case it was deleted or any filter flag within the filter configuration files was changes.
Both targets "make clean" and "make clean_all" will delete the file "filters_aut.h"

The automatic code generator will take all filter configuration files and compose all corresponding function macros.
Per default, all scenario filters are interpreted as deactivated (static: undef, dynamic: off), no matter how the filter flags are set.
This prevents the users to be overwhelmed with all activated filters over the whole software.

Therefore, the list "dbgx_activated_filters" was created.
Only the filter flags of the listed filter domain names in this list, are taken into account for the activated filters and macro replacement in "filter_aut.h".
In case you want to choose multiple filter domains to be activated, just list them one after the other.

Each "filter".json file holds the level-filters.
As there would be configuration conflicts, only the level filter configuration of the first filter domain in the list dbgx_activated_filters will be taken into account.
All other level filter configurations are ignored.


#### Filter types

There are two filter types in the debug_extended component - the static and dynamic filter.  
   - static: before generating and loading the target software  
   - dynamic: after generating and loading the target software  
Both of them are configured in each "filter".json file:

**Static filters**

def / undef at a filter flag below means static activation / deactivation of debug_extended output.

The debug_extended output calling macros with the corresponding filter flags are empty for "undef" (no code in the macro). 
So "undef" means no code consumption at all for the corresponding macro calls. 
You can activate the output actions just by writing a "def" at the switches and generating the firmware again by make.

Nearly an unlimited number of such filters can be defined.
Hundreds or thousands of the macro calls can stay in the code, for debugging later repeatedly.

**Dynamic filters**

on / off at a filter flag below means dynamic activation / deactivation of debug_extended output

The debug_extended output calling macros with the corresponding filter flags cause no output for "off" or 
cause output for "on" just because of a switching flag in the RAM. The switching flags are initialized in
DBGX_CentralConfigFilter_Cfg.FilterFlagWords[] with the values according to "on" / "off" and can be overwritten 
in this array without generating a new firmware.


#### Filter combination (static and dynamic)

The static and dynamic filters are designed to be used in parallel.
Following table shows the meaning of the combinations:

    static         dynamic       Output macro     Output          Remark
    filter         filter        content          activated
    ------------------------------------------------------------------------------
    def             on           not empty        yes
    def             off          not empty        no
    undef           on           empty            no              Avoid to use it.
    undef           off          empty            no

The debug_extended output calling macros and the initialization values in
DBGX_CentralConfigFilter_Cfg.FilterFlagWords[] are generated automatically
in filters_aut.h according to the filter flags defined in the files "filter".json:


#### Configure the level filter flags

The level flags have unchangeable, predefined names and can be used alone at the calling locations, examples:
    DBGX_logChar_ERR('!');
    DBGX_logStr_INFO("Critical code passed.");
    ...

The level flags can also be combined with the scenario flags at the calling locations. 
For activated output the level flag OR the scenario flag must be "def" and "on", examples:
    DBGX_logChar_ERR_SCN_POWER(MyChar);
    DBGX_logStr_INFO_SCN_TOUCH("TCH: Critical code passed");

Those 3 levels with the predefined names ERR, WARN, INFO can be used.
If you do not use levels, then list them with "undef, off".

    "level":[
        ["ERR"  , "def"   , "on", "Some comment"],
        ["WARN" , "def"   , "on"],
        ["INFO" , "undef" , "off", "Some comment"]
    ],

Each level with "def" consumes only one bit in the RAM and the code
of the calling location for debug_extended output.
Each level with "undef" consumes nothing, even if the macro with the
corresponding (alone used) level flag is called.

Note:  
Please keep the hierarchical order of the filter configuration files in mind. (Chapter above)


#### Configure the scenario filter flags

The scenario flags have changeable user defined names and can be used alone at the calling locations, examples:
    DBGX_logChar_SCN_STEAM('1');
    DBGX_logStr_SCN_CLOCK(MyString);

The scenario flags can also be combined with the level flags at the calling
locations. For activated output the level flag OR the scenario flag must be
"def" and "on", examples:
    DBGX_logChar_ERR_SCN_POWER('!');
    DBGX_logStr_INFO_SCN_TOUCH("TCH: Noise occured");

Many more scenarios can be defined. 
The scenario names can be chosen application specific.

    "scenario":[
        ["DBGX_TOUCH" , "def"   , "on" , "This field is optional for comments"],
        ["DBGX_POWER" , "undef" , "off", "This field is optional for comments"]
    ]

Each scenario with "def" consumes only one bit in the RAM and the code
of the calling location for debug_extended output.
Each scenario with "undef" consumes nothing, even if the macro with the
corresponding (alone used) scenario flag is called.

The scenario filters can be named user specific, as long as no problem of macro identifier length is detected by Lint or the preprocessor.
Nearly an unlimited number of the scenario filters can be defined.
Hundreds or thousands of the macro calls can stay in the code, and be repeatedly be activated in your file "filter".json, if necessary

You can use as many scenarios as you want, as long as you have enough RAM; one scenario with "def" consumes one bit.
By default the macro calls are deactivated with "undef" for each filter, so consuming no code at the calling locations.


#### Example: Filter configuration file:

A example for the module specific filter configuration file is located here: common/prog/debug_extended/cfg_templates/dbgx_filters.json

The "def / undef" is for static activation / deactivation (means: macro at calling location not empty / empty).
The sub-ordered "on / off" is for dynamic activation / deactivation by a RAM flag (initial value of this flag).
    
Have always the level filters ERR, WARN and INFO and none, one, some or many free nameable scenario filters in your file "filter".json.


#### Use the dynamic filter flags

As described in the chapters above, the debug output can be activated / deactivated with dynamic filter flags.
These can be modified during runtime with the following array:

    DBGX_CentralConfigFilter_Cfg.FilterFlagWords[]

This array is initialized according to the content of *filters_aut.h* (from *"filter".json*).
While the array cell [0] holds the level flags, the remaining array cells hold the scenario flags.
The array is handled like a bit field, where each bit stands for one dynamic filter.
Therefore, you can activate / deactivate dynamic filters by modifying the numeric value according to the bit logic.

The mapping from the dynamic filter to the corresponding bit, can be found at *filters_aut.h* --> "Dynamic filter definitions".
While the "index of filter word" defines the array cell of DBGX_CentralConfigFilter_Cfg.FilterFlagWords[],
the "index of filter bit" defines the place of the correpsonding filter bit.


#### Naming of filtered api functions

The filtered api functions for the text output are automatically composed by a code generator (according to the *"filter".json*).
The names are composed as followed:
    
    - function-name  +  filter-level  +  scenario-level
    
The code generator creates the following file, where you can easily read out every available (generated) filtered api function:

    app/<MyProject>/prog/debug_extended/filters_aut.h


#### Examples: Filtered functions
    
    DBGX_logChar_WARN_SCN_CLOCK('!');            // Combined: WARN is level filter,
                                                   // CLOCK is scenario filter
        // Will put out "!", if level WARN is activated 
        // or if scenario CLOCK is activated
    
    DBGX_logChar_...('A');
        // Will put out "A", if the filter(s) ... is (are) activated
    
    DBGX_logStr_...("BC");
        // Will put out "BC", if the filter(s) ... is (are) activated
    
    uint32_t MyVariable = 0x89ABCDEF;
    DBGX_logInt_...(MyVariable);                                      // more intuitive and more memory optimized
    DBGX_logInt_...(MyVariable, DBGX_SIGNED_HEXADECIMAL);             // more specializable
        // Will put out "89ABCDEF", , if the filter(s) ... is (are) activated
        // The (second) parameter "Format" is explained in a later chapter
    
    int16_t MyArray[3] = {-1, 0, 1};
    DBGX_logIntArr_...(MyArray, 3);                                   // more intuitive and more memory optimized
    DBGX_logIntArr_...(MyArray, 3, DBGX_SIGNED_DECIMAL);              // more specializable
        // Will put out "-1 0 1", if the filter(s) ... is (are) activated
    
    DBGX_logTime_...();
        // Will put out for example 192281369", if the filter(s) ... is (are) activated
        // Is the time in microseconds
    
    DBGX_logLocat_...();
        // Will put out for example "HEAT_Heating.c, 93" , if the filter(s) ... is (are) activated
        //                           file          , line
    
                      *         *         *


### Processing exceptional errors

The debug_extended component also provides a module for handling and logging exceptional errors.  
The macros for exceptional errors will write the ErrorCode and ErrorParameter into the error memory and may execute user defined solving actions.

Just activate the following switch: (*api_cfg.h*)

    #define DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED

If not defined, the macros will have no contents.

The list with the available error handling functions can be found above, in the api functions overview.
Further, there are calling examples in the chapter below: *Examples: Handle error functions*.

Note:
For additional information, feel free to have a look at the test application: dbgx_test_app_error

#### Unique error code

Each exceptional error handling function takes at least an ErrorCode as input parameter.
As the ErrorCode should be unique for each error case, make sure not to double define an error code.
Unique in this context means: The source code location, where the error is detected, is findeable by the error code value.

To create error codes, please first reserve yourself an error code domain and add it to this overview here:  
https://wiki.bsh-sdd.com/display/PEDF/Error+Code+Domains  

Further, it will be best to create your own list, to keep an overview of the already defined error codes.
Again, you can refer to the table from the same wiki page.

**Meaning of the ErrorCode**

The error code is packed into an unsigned 32 bit variable and is coded as follows:

    0x12345678
      ||||||||
      ||||||||
      ||||||||
      ||||++++---Error codes within the domain
      ||||
      |+++-------Domain
      |
      +----------Error handling (0: None, 1: Reset, 2: Callback)

All in all, this structure allows 4095 different domains which are divided for the applications and the Framework:

    0x*000**** - 0x*7FF****   Framework   --> 2048 domains
    0x*800**** - 0x*FFF****   Application --> 2048 domains

The remaining (lower) 16 bits can be filled with error codes, which allows 65536 unique error codes in each domain.  
Here we recommend, using the last significant bits to be used to identify instances.

**Action in case of the error**

The error codes should be defined in the header file of your application (like in the test app: dbgx_test_app_error).
Here, you need to apply a bitwise "OR" operation with the type of the error solving (see below: reset, callback, none).
The error code definition should look like this:

    // Name of the error                  error code | error solving type  
    #define DBGXTE_ERR_CALLBACK_TEST     (0x0000F000 | DBGX_ERR_SOLVING_CALLBACK)
    #define DBGXTE_ERR_RESET_TEST        (0x0000F010 | DBGX_ERR_SOLVING_RESET)

#### Error parameter

Some exceptional error functions also take the ErrorParameter as input.
It is an additional parameter for the user, to better categorize and handle the errors.

Note:
All error handling functions, which are not receiving the error parameter as input, will use the error parameter 0.

#### Error solving types

All "DBGX_handleErr..."-functions provide an optional error solving mechanism for the user.
It will be executed right after logging the exceptional error.
It can be chosen between the following 3 options:

    Way of Error Handling      Element of Error Code
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    SW Reset                   DBGX_ERR_SOLVING_RESET
    Callback Function          DBGX_ERR_SOLVING_CALLBACK
    None                       DBGX_ERR_SOLVING_NONE (is 0; need not be written)

**SW Reset**

Once the error solving function is called with SW reset, the HSUP_vGenerateReset() function will be called.

**Callback Function**

After logging the error data, a user defined callback function / error mechanism can be triggered.
For the configuration and further information, have a look at the next chapter.

**None**

Nothing will be done additionally. This is the default configuration.

#### Error solving: callback functions

After logging the error data, a user defined callback function can be triggered.
In order to trigger this error solving mechanism, the DBGX_ERR_SOLVING_CALLBACK has to be ORed to your unique error code.

Once the DBGX_handleErr... function with the callback option is called, a central callback function within the debug_extended component is called.
From there, all user defined error solving functions are called (like callback functions).
This error solving function should be in the module/application, the error code is defined.

Just follow these steps, to add your error solving function to the callback mechanism:

   - Implement your user solving function
   - Call the function "DBGX_registerErrorSolvingFunction" in the init phase of your module

Your callback-like error solving function should follow this signature:

    void <ABBR>_<solveExceptionalErrorFunctionName>( uint32_t errorCode, uint32_t errorParameter )

Because the errorCode and the errorParameter from your function call "DBGX_handleErr..." will be given as an input,
it will be best to implement for example a switch-case statement to distinguish between the error cases.

Note:
The test application (dbgx_test_app_error) implements the error solving mechanism as described here.

#### Set a Break Point for working on debugger

Inside DBGX_handleErrAfterErrOutput, see the comments there.  
By this you can inspect easily the error code and, if used, the error parameter.

#### Calling locations

The handling of exceptional errors can also return the calling location of the error.
Therefore, activate the following switch: (*api_cfg.h*)

    #define DBGX_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED

#### Examples: Handle error functions:

    (void)DBGX_registerErrorSolvingFunction(&DBGXTE_solveExceptionalErrorFunctions);
        // Let the debug_extended component know your application specific error solving 
        // function, in case of DBGX_ERR_SOLVING_CALLBACK

    DBGX_handleErr(TEMP_ERR_UNDEFINED_TEMPERATURE);
        // Parameter: Unique error code defined in api_error_cfg.h
        // Save error code and return text output (if activated)

    uint32_t LIB_ReturnCode = ...;
    DBGX_handleErrWithParam(HEAT_MODE_NOT_SENT, LIB_ReturnCode);
        // Parameters: Unique error code and an error parameter for that code
        //             Error parameter can be e.g. an error return value of a
        //             called function or forbidden index value
        // Save error code with return code and return text output (if activated)

    uint8_t Index = ...
    DBGX_handleErrIf          ((Index > 10U), KNB_TOO_MANY_POSITIONS);
        // Parameters: Condition (must be true for the error) and unique error code
        // Save error code and return text output (if activated)

    uint32_t Status = ...;
    DBGX_handleErrWithParamIf ((Status != 0), TCH_UNDEFINED_KEY_PATTERN, (uint32_t)Status);
        // Parameters: Condition (must be true for the error), unique error
        //             code and an error parameter for that code
        //             Error parameter can be e.g. an error return value of a
        //             called function or forbidden index value
        // Save error code with return code and return text output (if activated)

The following function can be called by an application to read out the data of the exceptional error memory 
(e.g. for transfering those data via DBus to the PC or to the system master):

    NumberOfErrors = DBGX_readFirstError(&ErrorCode, &ErrorParameter);
        // Returns the number of saved errors: 0 for no error, 1 for 1 error (first one is the last one), 
                                                               2 for 2 errors (first one and last one)
    NumberOfErrors = DBGX_readLastError(&ErrorCode, &ErrorParameter);
        // Returns the number of saved errors: 0 for no error, 1 for 1 error (first one is the last one), 
                                                               2 for 2 errors (first one and last one)


#### General note - error solving functions

Feel free to use the error solving macros at many (hundreds of) program locations, where surprising irregular passing of branches or 
irregular values could occur. Mostly a lot of hidden errors will be found by this.

- E.g. default branches of switch-instructions, else branches of if-instructions, ...
- E.g. return values telling "error", forbidden index values, forbidden calculation results, ...


### User input functions

The input api functions of the debug_extended component allow the user to send a byte string from the PC to the Microcontroller.

#### Examples: Input api functions

    (void)DBGX_registerInputCallbackFunction(&DBGXTA_testStringInput_CB);
        // Register your user defined callback function for the input data

    int32_t status = DBGX_getString(array, (sizeof(array) / sizeof(array[0])));
        // returns the length of the input string, that was read.
        // -1 in case of interface error
        // -2 in case of a cooperative input interface. (Callback mechanism will return signal for read data)
        
        // Param1: First parameter: User initialized array for saving the interface data
        // Param2: Maximum length of the input data

#### Supported interfaces
The following interfaces are supported for the input data:

   - MUART
   - DBus
   - Simple Debug

For the configuration of the input interface and further specifics, have a look at the corresponding chapter below.

#### String input
The input data, nonetheless which interface is used, will be stored in a char array.
This array has to be initialized by the application itself.

Each reading sequence of the input string ends automatically after receiving a null character, but at least after the MaxLen, that has been given as second input character to the function DBGX_getString.

#### Access the string input

In case the simple debug component is used, the data array will be filled using busy waiting.
After the DBGX_getString function ends, the received data is accessible immediately.

In case of a cooperative input interface, the data will be read beside the normal software execution.
After the whole string was received, a callback mechanism will be triggered.

Note:  
Examples for both interface categories can be found in the dbgx_test_app.  
A more detailed, graphic documentation of the data reading can be found in the wiki:
https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Standalone+Documents

#### Using the callback mechanism / cooperative interface

All cooperative interfaces work with a callback mechanism.  
Similiar to the error solving functions, the callback mechanism is triggered after ending the read sequence.
After that, a central callback function within the generic area is called first.
From there, all user defined callback functions called.

Therefore, the user defined callback functions has to be registered to the generic area.
This has to be done with the function: DBGX_registerInputCallbackFunction

All in all, you can add 5 functions to the callback mechanism per default.
This number can be modified with DBGX_LIMIT_OF_INPUT_CALLBACK_FUNCTIONS (*api_cfg.h*).

### General information

#### Maximum frame length of the api functions debug output
Each api function is limited to a maximum output length of 96 bytes. Due to the DbgAttributes the available length gets minimized to 79 Bytes.
In order to use the full 96 Bytes payload, you can deactivate the DbgAttributes.

If the concatenation of the debug information within a message is too long, the following error string will be put out via the interface - instead of the debug data:
    
    "!~Concatenated string too long"

#### Configuration of the function parameter: Format 

All API functions to log integer values, take the parameter "Format" as optional input argument.
In case it is not explicitely given, the default parameter "DBGX_DEFAULT_FORMAT", defined within "api_cfg.h", will be used.
While calling the function without explicit format parameter is more intuitive and more memory efficient,
calling the function with the explicit format parameter gives better possibilities in configuring the output.

The format parameter generally describes the numeric interpretation for the input and output.
In the following, all possibilities for the format parameter are listed. (also in *api_cfg.h*)

    Format parameter              |                  Description
    ------------------------------|---------------------------------------------------------------------
                                  |
    DBGX_UNSIGNED_DUAL            |      unsigned input     and     output representation with base 2
    DBGX_UNSIGNED_OCTAL           |      unsigned input     and     output representation with base 8
    DBGX_UNSIGNED_DECIMAL         |      unsigned input     and     output representation with base 10
    DBGX_UNSIGNED_HEXADECIMAL     |      unsigned input     and     output representation with base 16
                                  |
    DBGX_SIGNED_DUAL              |        signed input     and     output representation with base 2
    DBGX_SIGNED_OCTAL             |        signed input     and     output representation with base 8
    DBGX_SIGNED_DECIMAL           |        signed input     and     output representation with base 10
    DBGX_SIGNED_HEXADECIMAL       |        signed input     and     output representation with base 16


### Configuration of the API Layer / debug_extended features

The main configuration of the debug_extended component is mostly done in the file *user_cfg.h*.

**If filtering of output data (text application functions or touch derived application function) is needed:**

Define the Output Data Filters in "filter".json. See [A1].

**If text application functions (for variable values and infos as ASCII characters) are needed:**

  *Enable the Text Application Functions in user_cfg.h (is for DBGX_log... and DBGX_getString):*

    #define DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED

**If Touch Derived Application Functions are Needed:**

  This feature is not yet implemented !!!

**If exceptional error handling is needed:**
    
  *Activate the Exceptional Error Processing Functions in user_cfg.h:*

    #define DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED
    
  *Define the unique exceptional Error Codes in your application, like:*

    // Example with reset for error solving
    #define DBGX_ERR_IDX_HEATING        ((uint32_t)0x00000002 | DBGX_ERR_SOLVING_RESET)
             // your error name                    your error | your solving method

The definitions of the exceptional error codes of the applications is used in the handleError functions.
Each error code indentifies the programm location of the error.

**If source locations for exceptional errors are needed for the outputs of exceptional errors:**
    
*Activate the outputs of source locations for exceptional errors: (user_cfg.h)*

    #define DBGX_EXCEPTIONAL_ERROR_SOURCE_LOCATION_ACTIVATED

#### Input text application function

  *Activate input text application function: (user_cfg.h)*

    #define DBGX_INPUT_FUNCTIONS_ENABLED

#### Output touch derived application function

  This feature is not yet implemented !!!

#### Input touch derived application function

  This feature is not yet implemented !!!

<.pn>

## Usage of the protocol layer
    
The debug_extended component provides several output protocols, which relate to different types of data representation.

### Overview of the protocols
    
The following list shows the different protocols:

    #define DBGP_PROTOCOL_NONE             ((uint8_t)0x00)
    #define DBGP_PROTOCOL_TEXT             ((uint8_t)0x01)
    #define DBGP_PROTOCOL_TCH_DER          ((uint8_t)0x02)

Note:  
At this moment, only the protocols DBGP_PROTOCOL_NONE and DBGP_PROTOCOL_TEXT are implemented and tested.

#### Protocol: DBGP_PROTOCOL_NONE

If the DBGP_PROTOCOL_TEXT is selected, the debug data output will not be put out via the selected interface type.


#### Protocol: DBGP_PROTOCOL_TEXT

If the DBGP_PROTOCOL_TEXT is selected, the debug data output will be put out / saved in a ASCII format via the selected interface type.

### DbgAttributes

The debug attribute string (DbgAttributes) is an additional source of information, which is leading each api function output.
It is activated by default and contains additional debug information like the system timestamp, filters, ... .

#### Activate / Deactivate the DbgAttributes

The debug attribute string is activated by default and is leading the output of each api function.
It can be deactivated by removing the preprocessor switch "DBGX_USR_DBG_ATTRIBUTES_ENABLED" from the file *user_cfg.h*.
Activating /Deactivation the DbgAttributes will have impact on the maximum payload of the api function. (See: DbgAttributes: Message length and payload)

#### Interpret the DbgAttributes

With the activated DbgAttributes, the output is a concatenation of a general attribute string (DbgAttributes) and the requested information from the api function:

     <DbgAttributes>: <api-function-output> 

A example output may look like this:
 
     TT0.101...4389B: Hello world !!!
    
The variable DbgAttributes consists of 15 characters, which all represent specific information.
The character '.' is used as a placeholder and has no further meaning for the interpretation.

The DbgAttributes are coded as followed:

     Example               TTE001001234567

     Module abbrevation    TT*************   - Module abbreviation from DBGX_ModuleAbbreviations_Cfg[]

     Level abbrevation     **0************   - Neutral (neither error nor warning nor info)
                           **E************   - Error
                           **W************   - Warning
                           **I************   - Info

     Scenario abbrevation  ***00**********   - Index of filter word (according to *filter_aut.h*)
                           *****10********   - Index of filter bit (according to *filter_aut.h*)

     Timestamp             *******01234567   - Timestamp in hexadecimal

#### DbgAttributes: Message length and payload

The maximum frame length of each output is limited to 96 Bytes.
Because the DbgAttributes are part of the output string, they affect the payload for the api function output.
If the user configuration activates the DbgAttributes (default configuration), the maximum payload shrinks.
Due to the 15 Bytes information string and the 2 Bytes seperator, the maximum payload will be 79 Bytes.

Once the DbgAttributes has been disabled by the user configuration, the DbgAttributes are neither be calculated nor be part of the output string.
The maximum payload will stay the original 96 Bytes and the runtime will be improved.

Tipp:
For time sensitive debug data, we recommend the deactivation of the DbgAttributes, due to runtime effects.

<.pn>

## Usage of the interface layer
    
### Overview of the interfaces

There are several debug interfaces for the debug data output.
Following structure shows all available interfaces:

    - MUART interface (via the debug_extended component)
    - DBUS interface
    - RAM interface
    - Simple debug component as interface:
          -> Serial interface (UART via the simple debug component)
          -> ITM interface (Instrumentation Trace Macrocell)
          -> STDIO interface (stdio library (intel))
          -> Segger RTT interface (Segger J-Link)

It is possible to use the 4 upper interfaces simultaneously.
This feature is not available within the simple debug component.
There you can just use one out of the four interfaces at the same time.

### Selection of the output interfaces

As indicated in the listing above, the choosing of the debug interface may run in 2 steps.
First choose the interface of the debug_extended component (1) and if necessary, choose the interface of the simple debug component (2).  
The following code sections show the interface configuration for the Segger RTT as an example.

Note:
The simple debug interface is selected as default interface.

**(1) Choose the interface mapping in the debug_extended component**

The interface(s) for the debug output are defined in the file *user_cfg.h*.
In the following example, the text-debug-output will be put out via the simple debug component.

    #define DBGX_USR_PROTOCOLS {   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_NONE            0 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_MUART           1 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_DBUS            2 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_RAM             3 */   \
        DBGP_PROTOCOL_TEXT    /* via DBGI_INTERFACE_SIMPLE_DEBUG    4 */   \
    }

Corresponding to the first steps and a fast setup, the interface type simple debug component is predefined.
Further, it is also possible to use several interfaces for the debug output simultaneously.

**(2) Choose the interface type within the simple debug component**

This configuration is only relevant, if you have chosen the simple debug component as an interface for the debug data!

If you use the (simple) debug component for output data, you have several interfaces to choose from.
Just define your interface type in the file *debug/debug_api_config.h*.

In this example, we set the Segger RTT interface for the debug data:

    //#define DEBUG_INTERFACE_SERIAL  ///< UART serial port
    //#define DEBUG_INTERFACE_ITM     ///< ITM (Instrumentation Trace Macrocell)
    //#define DEBUG_INTERFACE_STDIO   ///< stdio library (intel)
    #define DEBUG_INTERFACE_RTT     ///< SEGGER RTT
    
Note:  
Only one of the simple debug interfaces can be used at the same time.  
Further, be aware of the limitations of the simple debug component (See user manual of simple debug component).

**Multiple interfaces**

Within the debug_extended component it is even possible to use more than one interface simultaneously for the debug data output.
Just define the DBGX_USR_PROTOCOLS as in this example:

    #define DBGX_USR_PROTOCOLS {   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_NONE            0 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_MUART           1 */   \
        DBGP_PROTOCOL_NONE,   /* via DBGI_INTERFACE_DBUS            2 */   \
        DBGP_PROTOCOL_TEXT,   /* via DBGI_INTERFACE_RAM             3 */   \
        DBGP_PROTOCOL_TEXT    /* via DBGI_INTERFACE_SIMPLE_DEBUG    4 */   \
    }

Note:  
The "simple debug component"-interfaces, can __not__ be used simultaneously to each other.


#### Use the MUART interface
The debug_extended MUART interface sends the debug data via the MCAL/MUART component of the Framework.
With an FTDI it is possible to establish a communication connection to your development PC.

The UART pins of your controller are only initialized, if the UART is selected for output data.
The pin configuration is set by default and can be adapted for your project. (See later chapter).

Note:
Per default, the simple debug interface (with serial connection -> UART) is selected.
If you activate the MUART interface, make sure, the simple debug interface is NOT defined with the serial (UART) interface.
This may cause inconsistent and strange behavior.

##### MCAL / MUART

The MUART interface of the debug_extended component builds on the MCAL / MUART component of the Framework.
Therefore, your controller has to be supported by the MCAL.

There are different ways to include the MUART component to your project:  
  - Add the whole MCAL component, e.g. with the setup.bat  
  - Just add the MUART component: by setting "mcal_modules" make variable in variant build file (mcal_modules = muart).  

For a more detailed information and especially to limitations of the MUART, have a look at the mcal readme file (common/prog/MSP/mcal/).

##### MUART and FIFO buffer (mem_utility)

The MUART speed is limited to the configured baudrate and works relatively slow, compared to other interfaces.
In order to avoid data loss, the MUART is built on a ring buffer of the mem_utility component of the Framework.
It works like a FIFO queue and contains the data to be sent via the MUART interface.

The writing of debug data to the FIFO is very fast, while the MUART connection is limited in its speed.
As a consequence, the ring buffer may be written full, before the data can be send via the UART.
Once the ring buffer is full, because the MUART did not send fast enough, there will be problems with data continuity.

In case of a full ring buffer, the MUART will be stopped and the following error message will be printed out: "!~ MUART TOO SLOW".
The read index of the MUART interface on the ring buffer will be reset to the beginning and the MUART will start the debug output from here.
As a result, you can expect a debug data gap in the MUART output.

##### Configure the MUART connection

The MUART interface has to be configured in the file: *interface_cfg.h*.
There you have to set the UART pins, the UART baudrate and the UART FIFO buffer size.

A default configuration for the UART may look like this: (with STM32G0):

    // Interface parameters - UART
    #define DBGI_SIZE_OF_UART_RING_BUFFER        5120
    #define DBGI_UART_BAUDRATE                   19200
    #define DBGI_UART_RX_TX_PIN_USR              1 // replace with your PIN, e.g.: &MUART3_RX_PB11_TX_PB10 (STM32G0)


**Buffer size**  
The FIFO buffer size should be considered according to the debug output frequency and the MUART speed.
In our tests we used a buffer size of about 5120 bytes (having a high debug output frequency).

**UART pin**  
The UART pin (DBGI_UART_RX_TX_PIN_USR) has to be set according to your hardware (see example behind).
Per default (defined with value "1"), it is defined for some controllers, according to the list below.
For those default settings, the pins are mapped to the USB COM interface of the DBus-reference board.

    STM32G0 || STS_SIM_TEST_CI             &MUART3_RX_PB11_TX_PB10
    STM32G4                                &MUART3_RX_PB11_TX_PB10
    RA4                                    &MUART2_RX_P301_TX_P302
    MKV5X                                  &MUART2_RX_PD2_TX_PD3

If you do not define the UART pin and none of the default pin mappings holds true, the preprocessor will print out the following warning:

    #warning Please configure the MUART for your controller

Tipp:  
If you often get the output "!~ MUART TOO SLOW", it may help to increase the baudrate or the FIFO buffer size.

##### Establish the serial connection to the MUART

There are several possibilities to access the debug data via the MUART interface (Putty, Keil uVision, ...) (over an FTDI).
A detailed documentation to some tools can be found in the documentation of the simple debug component, which uses the UART via the HAL.
The basic configuration data is always the same, therefore we will only focus on the basic tool "Putty" and its configuration here.

**COM line**  
When the FTDI is connected to your development PC, there will be a new entry in the device manager of your operating system.
This new entry "COM ports" gives you the information of the serial communication line, which is used by our PC.

Hint: The serial line may change with each new USB connection.

**Baudrate**  
The MUART speed within the Putty has to be the same as you have configured it in the file: *interface_cfg.h*.  
By default, the DBGI_UART_BAUDRATE is set to 19200u.

**Terminal options**  
Activate the option "Implicit CR in every LF" within the tab "Terminal".

##### UART-conflicts between simple debug and debug_extended component

The simple debug and debug_extended component may both configure the UART interface as output.
To point out possible UART conflicts, the following compiler warning will be created in that case:

    Debug and debug extended component both define the UART

In order to avoid strange behavior, make sure only one component actively uses the UART.
The easiest way to bypass the warning is to swap one of the components away from the UART interface.
Just follow one of the following steps:

   - Change the interface of the simple debug component:
     Navigate to the file *app/<MyProject>/prog/debug/debug_api_config.h*.
     Deactivate the interface "DEBUG_INTERFACE_SERIAL" and set another one.  

   - Use another interface in the debug_extended component.
     See the chapter "Usage of the interface layer" for further information.

Note:
The simple debug component defines the UART as default interface.


#### Use the DBus interface

The DBus interface of the debug_extended component stores the debug data output in a FIFO buffer first and then sends it via DBus messages to the PC.
To make the reading of the output easier, there will be one DBus message for one debug data output message.

Because the maximum message length of each single debug output data is limited to 98 bytes, the dbus dbus_transmit_buffer_size should at least be set to that value or higher.
In case the dbus_transmit_buffer_size is too small, the following message will be printed out with a compiler warning:

    Please increase your dbus_transmit_buffer_size to at least 98 bytes. See the debug_extended user manual for further information.

The dbus_transmit_buffer_size can be defined in your variants .mak file.

** DBus output message **

The message for the output data is defined as followed:

    #define DBGX_MSGID_MSG_SEND_DBGX_DATA_TO_PC            0xB9FF

** DBus input message **

The message for the input data is defined as followed:

    #define DBGX_MSGID_MSG_SEND_DBGX_DATA_TO_CONTROLLER    0xB9FE

Note:  
For the DBus interface, the DBus has to be activated and configured in your project (which is usually done with the initial setup).
Further, be advised, that the DBus interface is the slowest interface of the debug_extended component. 
Therefore, we do not recommend sending much data with that interface.

##### Configure the DBus interface

In order to use the DBus interface, you have to set the DBus FIFO buffer first.
Just define the buffer size (in bytes) in the file *interface_cfg.h*

    #define DBGI_SIZE_OF_DBUS_RING_BUFFER    10840

Note: Because the DBus is usually the slowest interface here, we recommend a big buffer. Here for example 10840 bytes.

##### Error message: DBus too slow

If the DBus is too slow for the debug data transmission and the FIFO buffer will be affected by an overflow with the incoming debug data,
the FIFO buffer gets resetted and the following error message appears at the interface:

    !~ DBUS TOO SLOW

Due to the FIFO reset you will have a debug data loss.  
Of course, the debug interface will go on with its works, right after the reset.

Following approaches may help in case of regular FIFO resets:
   - Increase the DBus baudrate
   - Increase the FIFO buffer size
   - Print out less debug data
   - Change to another interface

##### Interpret the DBus data output

The output data of the DBus interface will be a hexadecimal string with variable length.
That gives you several possibilities of interpreting the output.
Just know, that there is no zero-termination at the end of each messages.

The most comfortable way, is using the communication monitor and its automatically translation into an ASCII string.
Just create a new message type with the following attributes:
 - ID:    0xB9FF
 - Name:  \MessageName\
 - Variable:
   - Name: Output data
   - Type: n x char


#### Use the RAM interface
If the RAM interface is selected for the debug output, the data gets stored in the global static array (DBGI_RamRingBuffer).
It is used like a ring buffer and gets initialized within the *interface_cfg.c*.

The static array gets initialized with a defined length of SIZE_OF_RAM_RING_BUFFER (*interface_cfg.c*).
In order to use the RAM interface, you have to set the length of the RAM memory to a useful size.
In our test implementations we usually defined a size of 1024 Bytes:
    
    #define DBGI_SIZE_OF_RAM_RING_BUFFER    1024
    
Note:
The default size of the RAM ring buffer is 1 Byte, which is the minimum length of the debug array.
At least the size of the maximum frame length of a single message (96 Bytes) is recommended.

**MemUtil and beeing reentrant**  

The RAM interface is based on the mem_util component of the Framework and is handled as a ring buffer.
Because the RAM interface is implemented reentrant, it dis- /enables the interrupts with each output string.

The mem_utility, which initializes the static array as Ring Buffer, offers explicitely the feature to be reentrant.
For the implementation of the RAM interface we do NOT activate this feature, because then it will work in a reentrant way in 
each module. This may cause major impact on other modules.
Instead, all interrupts get disabled, during sending a single debug output string via the interface.
If any other module defines the mem_util as reentrant, there will be no side effects here.

Through the structure DBGI_RamRingBuff (from common/prog/debug_extended/interface.c) you can access the write index in the RAM, 
where you can find the break in the ring buffered data. Just use the index:

    DBGI_RamRingBuffer.write
    
Note:
The data type of this index is RBUF_size_t, which is defined as uint_least16_t.
New data will be written starting at this index position (upcounting).
Therefore, the recent written data is stored till this index.

**Read the RAM input**  

As described above, the debug data is stored in the ring buffer DBGI_RamRingBuffer, while DBGI_RamRingBuff.write holds its index.
Both data should be manually interpretable with your usual debug tools.

In order to make the interpretation of the RAM data even more comfortable, we provide mostly automated solutions for the Keil uVision and the Ozone.
You can find them in the next chapters.
Examples of all needed and generated files, which are described in this chapter are stored in the wiki:  
https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Interpret+output+from+RAM+interface

##### Keil uVision

The Keil uVision environment allows us during a debug session to save memory data in hex-format into a output file.   
Just add the following code to the .ini file of your Keil uVision project.
It will create a button in the top bar, which saves the debug data in .hex format (HEX386).

    FUNC void displayvalues(void) {
        exec("SAVE \"./common/tools/debug_extended/interpret_ram_output/DebugData_RingBuffer.hex\" &DBGI_RamBuffer, (&DBGI_RamBuffer+0x1)");
        exec("SAVE \"./common/tools/debug_extended/interpret_ram_output/DebugData_RingBufferIndex.hex\" &DBGI_RamRingBuffer.write, (&DBGI_RamRingBuffer.write+0x1)");
    }
    DEFINE BUTTON "Save RAM interface data to file" "displayvalues()"

Depending on your folder structure, you may have to adapt the the file path in both of the exec-commands.
The both .hex files need to be stored in the tools directory. (common/tools/debug_extended/interpret_ram_output/)

After pushing the button, two .hex files with the corresponding data were created.
In order to interpret this data, we call the python script *interpreter.py* (common/tools/debug_extende/interpret_ram_output/)
This script converts the hex data into a ASCII formatted and ordered string (due to ring buffer).
This data will be stored in the file *output.log*.

##### Ozone

The Ozone environment allows us during a debug session to save memory data in binary-format into a output file.
Just add the following code to the end of your project file (\<Project\>.jdebug) of your Ozone project.
It allows you to save all needed data to a .bin file, using a single command in the Ozone command line.

    /*********************************************************************
    *
    *       DBGX_SaveRamToFile
    *
    * Function description
    *   Save the RAM data of the debug extended component to files. Optional.
    *   Call "Script.Exec("DBGX_SaveRamToFile")" in the command line
    *
    **********************************************************************
    */
    void DBGX_SaveRamToFile ( void ) {
        Target.SaveMemory("common\tools\debug_extended\interpret_ram_output\DebugData_RingBuffer.bin", Elf.GetExprValue ("DBGI_RamBuffer")-1, Elf.GetExprValue ("sizeof(DBGI_RamBuffer)")+1);
        Target.SaveMemory("common\tools\debug_extended\interpret_ram_output\DebugData_RingBufferIndex.bin", Elf.GetExprValue ("&DBGI_RamRingBuffer.write"), Elf.GetExprValue ("sizeof(DBGI_RamRingBuffer.write)")+1);
    }

Depending on your folder structure, you may have to adapt the the file path in both of the commands.
The both .bin files need to be stored in the tools directory. (common/tools/debug_extended/interpret_ram_output/)

As you can see in the related description, you call this function by typing to the Ozone command line:

    Script.Exec("DBGX_SaveRamToFile")

After calling this, two binary files with the corresponding data were created.

In order to interpret this data, we call the python script "interpreterOzone.py" (common/tools/debug_extende/interpret_ram_output).
This script converts the binary data first into the corresponding hex data and then into a ASCII formatted and ordered string (due to ring buffer).
The string output will be stored in the file "output.log".

#### Use the Segger RTT interface

For the Segger RTT interface, you have to use the Segger J-Link programming adapter!  

Usually, the Segger RTT interface is selected per default, so there will be no adaptions needed.
This default configuration can also be read here: "Choose the interface of your choice".

In the following section you find the basic informations to the Segger RTT connection.
For a more detailed description and different connection types, have a look at the user manual of the simple debug component.

**Establish telnet connection to the Segger RTT**  

The Segger RTT interface will be accessed with a telnet connection (e.g. via Putty, J-Link RTT Logger / Viewer, ...).
In case of using Putty, the debbuging session of your development environment has to be activated.
Tools like J-Linke RTT Viewer can be used without an active debugging session, by activating "attach to running system".

The telnet connection to the Segger RTT interface uses following characteristics:

    - Host/IP-Address:  localhost
    - Port              19021

If necessary, the translation type of your console has to be set for a good reading output.

If needed, here is a link, how to use the telnet connection with Eclipse:
https://mcuoneclipse.com/2015/07/07/using-segger-real-time-terminal-rtt-with-eclipse/
Also the Ozone debugger has a build in terminal, which works without any further configuration.

### Selection of the input interfaces

Not all interfaces from above are suited for data input.
Therefore only the interfaces described below are supported.

Note:  
The usage of the interfaces, stays the same, as it was described in the chapter to the interface output from above.  
Further, the interfaces can be used as input and output simultaneously.

#### Specifics of the input interfaces

The following interfaces are available for data input:

    | Interface        | cooperative / non cooperative | receive input data    |
    | ---------------- |:-----------------------------:|:---------------------:|
    | MUART            | cooperative                   | callback mechanism *1 |
    | DBUS             | cooperative                   | callback mechanism    |
    | Simple Debug     | non-cooperative *2            | busy waiting          |
    |  -  UART         |                               |                       |
    |  -  ITM          |                               |                       |
    |  -  STDIO        |                               |                       |
    |  -  Segger RTT   |                               |                       |
   
*1 The callback mechanism is described in the chapter below.  
*2 Non-cooperative means the interface to use busy waiting, until the whole input string has been received.

#### Activate data input interface

The functionality of the debug data input is enabled per default.
In order to disable the text input data, remove/comment the following switch (user_cfg.h):

    DBGX_INPUT_FUNCTIONS_ENABLED

#### Select data input interface

The selection of the input interface is also done in the file *user_cfg.h*.
Per default, the input interface is not selected, by setting:

    #define DBGP_PROTOCOL_INPUT_DATA  DBGI_INTERFACE_NONE

In order to define your input interface, just modify this definition.
According to the interface names, you can choose between:

    DBGI_INTERFACE_NONE
    DBGI_INTERFACE_DBGX_UART
    DBGI_INTERFACE_DBUS
    DBGI_INTERFACE_SIMPLE_DEBUG

#### Callback mechanism

The callback mechanism for the data input is used with cooperative inputs only.
After the whole input string was received by the cooperative interface, a callback mechanism up to the application layer gets triggered.

Similar to the error solving function callback, a central callback within the generic area is triggered first.
From there, all user defined callback functions for the input data are called.
These functions have to be registered in the init phase by the following function: DBGX_registerInputCallbackFunction.

See the corresponding api-chapter for the usage of the api functions.

<.pn>

## Use the test_application as an example

The test applications of the debug_extended component can be used as a reference for your implementations. 
There are 3 test application, one for general write api function, one for isr stress test and one for error solving functions.
The according files are located here:  
    - common/prog/debug_extended/test_app
    - common/prog/debug_extended/test_app_isr
    - common/prog/debug_extended/test_app_error

Each c-file contains a handle Task which has to be added to the scheduler. 
In order to establish a fast test run, the test applications are prepared for the auto scheduler. 
Therefore, just add the following line to the *common_config.mak*
    
    scheduler_config = auto
    
Further, the compilation of the test_application is protected by a predefined switch. It has to be set,
in order to add all corresponding source files. A easy way to add this switch as parameter is via 
the command line, according to the test application:

    make all -j dbgx_test_app=active
    make all -j dbgx_test_app_isr=active
    make all -j dbgx_test_app_error=active

The last step is to establish the connection to the debug interfaces. The corresponding characteristics 
can be found in the chapters above. 

<.pn>

## Formal points
    
### Abbreviations
    
    API:          Application programming interface
    DbgAttributes Debug attributes
    DBGX:         Debug extended

<.pn>

## Attachment

### [A1.1] Example: Filtering api functions by scenario and / or by level

**Macro calling, abstractly:**
    
    DBGX_log<element>_SCN_<scenario>(<parameters>);
    DBGX_log<element>_<level>(<parameters>);
    DBGX_log<element>_<level>_SCN_<scenario>(<parameters>);
    
    <element>:        <level>:              <scenario>:
      Char              ERR                   TOUCH
      String            WARN                  POWER
      Int               INFO                  CLOCK
      IntArray            Exactely those 3    RINSING
      TimeStamp           with those names    HEATING_MODE
      SourceLocation                          PYROLYSE
                                              ... As many as you,
                                                  want, names are
                                                  free
    
    <parameters>: 
      For Char:           char Char
          String:         char *String            //Usage: e.g. "Tst", MyString, ...
          Int:            const void *IntegerAddr, uint32_t Format
          IntArray:       const void *IntegerArrayAddr,
                                                   uint16_t NumberOfArrayElements,
                                                                     uint32_t Format
          TimeStamp:      None
          SourceLocation: None
    
    Format: Composed of those logical ORed constants:
      DBGX_FORMAT_INTLEN_8       | DBGX_FORMAT_UNSIGNED    | DBGX_FORMAT_BASE_2 
      DBGX_FORMAT_INTLEN_16      | DBGX_FORMAT_SIGNED      | DBGX_FORMAT_BASE_8 
      DBGX_FORMAT_INTLEN_32                                | DBGX_FORMAT_BASE_10
                                                           | DBGX_FORMAT_BASE_16


#### [A1.1.1] Filtering api functions by scenario
    
You can name the scenarios (means: scenario filters) as you want, as long as no problem of macro identifier length name 
is detected by Lint or even by the C-preprocessor.
    
You can use as many scenarios as you want, as long as you have enough RAM; one scenario with "def" consumes one bit.
    
**Example:**
    
    Example entry in "filter".json for a scenario filter:
      RINSING:   def,   off
    
    Calling example: DBGX_logStr_SCN_RINSING("TCH: Critical code passed");
                                         RINSING is the scenario filter
    - It is the calling of a macro.
    - Even more such calls with the same scenario filter can be used.
    - The entry for scenario name must appear in "filter".json.
    - The macro contents is empty for scenario "undef" in "filter".json, it
      is not empty for "def". In this example it is not empty.
    - Macro has a dynamically activated output for scenario "on", for "off" the
      output is dynamically deactivated.
      In this example the output is dynamically deactivated.
    - In this example the output is deactivated (calling macro defined, but
      dynamically deactivated).
    
**Effects of "def" / "undef" together with "on" / "off":**
    
      def / undef     on / off     Output Macro     Output          Remark
                                   Contents         activated
                                                    dynamically
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      def             on           not empty        yes
      def             off          not empty        no
      undef           on           empty            no              1*)
      undef           off          empty            no
    
      1*) The irregular entry "undef, on" is automatically interpreted as "undef, off",
          because it does not make sense to activate dynamically a scenario or level,
          that is not defined (that is not in the binary program code). To avoid 
          worrying, just avoid "undef, on". (So the "on / off" is sub-ordered to the
          "def / undef".)

#### [A1.1.2] Filtering api functions by level
    
**Filtering by level is the same as filtering by scenario, unless:**

- You cannot change the pre-defined names of the levels (ERR, WARN, INFO).
- You cannot add more levels.
    
**Example:**
    
    Calling example: DBGX_logStr_INFO("Critical code of power module passed");
                                      INFO is the level filter
    
    Example entry in "filter".json for the level filter:
      INFO:    def,   on
    
**Hint for usage of the level filter typically as level:**
    
*To have a level, use the filters this way:*
    
      No level           Only errors        Errors and         Errors,
      debug outputs:     as level debug     warnings           warnings and
                         outputs:           as level debug     infos
                                            outputs:           as level debug
                                                               outputs:
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      ERR:  undef, off   ERR:  def,   on    ERR:  def,   on    ERR:  def, on
      WARN: undef, off   WARN: undef, off   WARN: def,   on    WARN: def, on
      INFO: undef, off   INFO: undef, off   INFO: undef, off   INFO: def, on
    
*Nevertheless, you can even activate e.g. warnings only, 2*):*
    
      ERR:  undef, off
      WARN: def,   on
      INFO: undef, off
    
      2*) This special example is (strongly interpreting) no instance of a verbose
          level. This possible way of definition is resulting from not having a
          singular logical system besides the activation flags of the scenario 
          filters. So the level flags are just corresponding to the corresponding 
          level filters, same as having scenario flags corresponding to the
          scenario filters. Especially the combination rules stay understandable:
          - For "undef / def" combined with "on / off" (as described in the previous
            item)
          - Moreover, for level filters combined with scenario filters (as described
            in the next item)


#### [A1.1.3] Combined Filtering
    
Combined filtering by level and scenario can also be used for the debug output calls.
    
**Example:**
    
    Example entry in "filter".json for a level filter and a scenario filter:
      ...
      INFO:          undef, off
      ...
      HEATING_MODE   def,   on
      ...
    
    Calling example: DBGX_logStr_INFO_SCN_HEATING_MODE("Critical code passed");
                                      INFO is the level filter
                                              HEATING_MODE is the scenario filter
    - It is the calling of a macro.
    - Entry for scenario name must appear in "filter".json.
    - Macro contents is empty for scenario "undef" and level "undef".
      It is not empty for scenario "def" or level "def".
      In this example it is not empty because of "HEATING_MODE def".
    - Macro has a dynamically deactivated output for scenario "off" and level "off".
      It is dynamically activated for scenario "on" or level "on".
      In this example the output is dynamically activated because of "HEATING_MODE on".
    - In this example the output is activated (calling macro defined and dynamically
      activated) because of "HEATING_MODE def" with having sub-ordered 
      "HEATING_MODE on".
    
**Effects of a level combined with a scenario:**
    
      Scenario            Level
    
      def/undef  on/off   def/undef  on/off   Output Macro   Output          Remark
                                              Contents       activated
                                                             dynamically
      - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
      undef      off      undef      off      empty          no
      undef      off      undef      on       empty          no              3*)
      undef      off      def        off      not empty      no
      undef      off      def        on       not empty      yes
    
      undef      on       undef      off      empty          no              3*)
      undef      on       undef      on       empty          no              3*)
      undef      on       def        off      not empty      no              3*)
      undef      on       def        on       not empty      yes             3*)
    
      def        off      undef      off      not empty      no
      def        off      undef      on       not empty      no              3*)
      def        off      def        off      not empty      no
      def        off      def        on       not empty      yes
    
      def        on       undef      off      not empty      yes
      def        on       undef      on       not empty      yes             3*)
      def        on       def        off      not empty      yes
      def        on       def        on       not empty      yes
    
      3*) The irregular entry "undef, on" is automatically interpreted as "undef, off",
          because it does not make sense to activate dynamically a scenario or level,
          that is not defined (that is not in the binary program code). To avoid 
          worrying, just avoid "undef, on".
    
For the calling location containing level filter and scenario filter:  
Level "def, on" or scenario "def, on" activates the debug output. Else the debug output is deactivated.

