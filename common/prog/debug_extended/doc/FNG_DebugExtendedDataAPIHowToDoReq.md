# Framework NG: Debug Component: How-To-Do Requirements of the Application  Interface (API) Layer
    
In this document main points can be seen, how to implement the API layer of the  
debug component.
    
## Important What-To-Do Requirements about the Debug Component
    
In [1] the what-to-do requirements can be seen. Two important points of them  
are mentioned here again:
    
**Special Requirements about the Debug Interface and about the Debug Protocol:**
    
Both, debug interface and debug protocol depending on the debug interface,  
are assumed in the configuration area and do not appear at the API function  
calls.
    
The API layer provides neither the debug interface nor the debug protocol  
in its parameters of interface functions.
    
Special protocols to be provided are text terminal protocol data and touch  
derived application protocol data.
    
For the frames for touch derived application protocol data see [A2] for IDs  
already in use and future ID definitions in respect to the target of data  
from PC.
    
**Special Requirements about Re-Entrance:**
    
The sending functions of debug data to the PC have to be re-entrance able for  
example because of interrupt service routines also sending debug data next to  
the interrupted program parts.
    
<.pn>
## General Points about the API Layer of the Debug Component
    
### Subject of the API Layer
    
The API layer provides the following subjects:

- Application interfacing to controller debug data (output and input data)
- Handling of the API callback functions of the controller input debug data  
  (callback functions defined in the application)
  * Notifying of the callback functions by an API function
  * Calling of the callback function by the debug component
- Filtering by level and / or scenario flags of the controller output debug data
- Interfacing to the exceptional error handling
- Interfacing to the protocol layer below the API layer
    
See [A4] for the layers and their interface parameters.
    
The contents in this chapter here is described interface based, see  
[A8] especially for the data contents in the generic and in the configuration  
area.
    
### Overview and Mapping of API Layer into the Software Structure of  Debug Component
    
A layer structure, for protocol and interface processing outlined by MiKo,  
and the what-to-do requirements, written down in [1], are assumed for the  
software structure here.
    
An API layer is above the protocol layer, that provides a defined application  
interface (API).
    
So the layers of the debug component are:

- API related processing
- Protocol processing
- Interface processing
    
The layers have to be implemented as C-modules for easy modifying, with  
connected configuration modules.
    
An exceptional error memory processing module completes the debug component for  
catching the numerous exceptional run-time errors during the debug sessions.
    
    +-------------------------------------|---------------------------------------------+
    | Debug Component                     |                                             |
    | (Directory debug                    | Those modules with their                    |
    |  for C files)                       | interfaces are described                    |
    |                                     | in this document here.                      |
    |                                     |      .                                      |
    |                                     |     .                                       |
    |                                     |    .                                        |
    |       1*)         ..................|.............................................|
    |       Exceptional .                 |                                            .|
    |       Error Mem.  .                 +-------------------------------+            .|
    |       Interface   . Application     |                               |            .|
    |       (EEMI)  \   .       Interface |        Configu-               |            .|
    |                \  .       (API)     V        ration                 V            .|
    | +-------------+ \ . +----------------------+ Interface   +----------------------+.|
    | | DBGE:       |1*). | DBG:  API Related    | (CFGI)      | DBG:                 |.|
    | |  Exceptional|<----|       Processing     |<------------|  Configuration Area  |.|
    | |  Error      |   . |       api.*          |             |  api_cfg.*           |.|
    | |  Memory     |   . +----------------------+             |  api_error_cfg.*     |.|
    | |  Processing |   .   Protocol      |                    |  filters_aut.h; 1*)  |.|
    | |  with its   |   .      Processing |                    +----------------------+.|
    | |  Config.    |   .      Client                                           |      .|
    | |  Area       |   .      Interface  +-------------------------------+     |      .|
    | |             |   .      (CLI)      |                               |     |      .|
    | |  error.h    |   ..................|...............................|.....|.......|
    | |             |                     V                               V     V       |
    | |             |     +-----------------------+            +----------------------+ |
    | |             |<----| DBGP: Protocol        |<-----------| DBGP:                | |
    | |             |     |       Processing      |            |  Configuration Area  | |
    | |             |     |       protocol.*      |            |  protocol_cfg.*      | |
    | |             |     +-----------------------+            +----------------------+ |
    | |             |                     |                                     |       |
    | |             |                     +-------------------------------+     |       |
    | |             |                     |                               |     |       |
    | |             |                     V                               V     V       |
    | |             |     +-----------------------+            +----------------------+ |
    | |             |<----| DBGI: Interface       |<-----------| DBGI:                | |
    | |             |     |       Processing      |            |  Configuration Area  | |
    | |             |     |       interface.*     |            |  interface_cfg.*     | |
    | +-------------+     +-----------------------+            +----------------------+ |
    |                                     |                                             |
    +-------------------------------------|---------------------------------------------+
                                          V                 The arrows above tell "uses".
    
    1*) Automatically generated from filters.spc;
        filtering of debug output
    
<.pn>
    Interfacing of the API Related Processing Layer of the Debug Component:                
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            ............................................................................
                                            (API)
    
            DBGX_sendData                            DBGX_logChar
                                                     DBGX_logStr
                                                     DBGX_logInt
                                                     DBGX_logIntArr
                                                     DBGX_logTime
                                                     DBGX_logLocat
                                                     DBGX_logStrStr
                                                     DBGX_logStrInt
                                                     DBGX_logStrIntArr
                                                     DBGX_logStrStrLocat
                                                     DBGX_logStrStrLocatFull
                                                     DBGX_logStrIntLocat
                                                     DBGX_logStrIntLocatFull
    
                                                     DBGX_logChar_NonFiltered
                                                     DBGX_logStr_NonFiltered
                                                     DBGX_logInt_NonFiltered
                                                     DBGX_logIntArr_NonFiltered
                                                     DBGX_logTime_NonFiltered
                                                     DBGX_logLocat_NonFiltered
                                                     DBGX_logStrStr_NonFiltered
                                                     DBGX_logStrInt_NonFiltered
                                                     DBGX_logStrIntArr_NonFiltered
                                                     DBGX_logStrStrLocat_NonFiltered
                                                     DBGX_logStrStrLocatFull_NonFiltered
                                                     DBGX_logStrIntLocat_NonFiltered
                                                     DBGX_logStrIntLocatFull_NonFiltered
    
            DBGX_notifyReceiveData_CB(..,            DBGX_getString
                                Module_ID)
               .. : APP_receiveDebugData_CB
    
                                  DBGX_handleErr
                                  DBGX_handleErrWithParam
                                  DBGX_handleErrIf
                                  DBGX_handleErrWithParamIf
                                  DBGX_readFirstError
                                  DBGX_readLastError
            ............................................................................
                                        *
                                        *****************************************
                                        *                                       *
      ........................          V         ...................           V
            (EEMI)               +------------+              (CFGI)     +--------------+
                              <--| API Related|<**                   <--| Configuration|
      DBGE_saveError             | Processing |   DBGX_initDebugAPI...  | Area         |
      DBGE_saveErrorWithParam    +------------+   and kernel func-      +--------------+
      DBGE_readFirstError               |         tions for API wraps
      DBGE_readLastError                |         ...................
      ........................          |
                                        V
            ................................................................
                                            (CLI)
            DBGP_sendProtData                        DBGP_writeProtCharArray
    
            DBGP_notifyProtReceiveData_CB(..)        DBGP_getProtString
               (..): DBGX_protReceiveDebugData_CB
            ................................................................
    
    <***    "is contained in", "is defined in"
    <---    "calls"
    
    See [A9] for details of exceptional error memory processing.
    
<.pn>
## API Layer of the Debug Component: Interface Elements
    
### Application Interface (API)
    
#### API for Controller Output Data Transfer of Touch Derived Application
    
    int32_t DBGX_sendData(uint16_t Module_ID, uint32_t CallingFilter,
                          uint16_t Function, uint8_t *Data, uint16_t DataLen)
    - ModuleID is the sender (e.g. touch or temperature regulation)
    
    See [A5] for the attributes of CallingFilter.
    
#### API for Controller Input Data Transfer of Touch Derived Application
    
    int32_t DBGX_notifyReceiveData_CB  (DBGX_ReceiveDataFunctionOfApi_t
                                                   ReceiveDataFunctionOfAPI_CB,
                                       uint16_t ExpectedModuleID)
    ReceiveDataFunctionOfAPI_CB:
        void APP_receiveDebugData_CB  (uint16_t Function, uint8_t *Data,
                                                              uint16_t DataLen)
    
#### API for Controller Output Data Transfer of Text Application  (for Variable Values and Infos as ASCII Characters)
    
    void DBGX_log...
    
    DBGX_logChar                           (uint32_t CallingFilter, char Char)
    DBGX_logChar_NonFiltered               (                        char Char)
    
    DBGX_logStr                            (uint32_t CallingFilter, char *String)
    DBGX_logStr_NonFiltered                (                        char *String)
    
    DBGX_logInt                            (uint32_t CallingFilter, void *Int, uint32_t Format)
    DBGX_logInt_NonFiltered                (                        void *Int, uint32_t Format)
                                            * Format: 0x00ssllbb
                                              ss: Signed / unsigned: 1 / 0
                                              ll: Integer length: 1, 2, 4
                                              bb: Base: 2 for binary, 10 for decimal,
                                                        8 for octal, 16 for hexadecimal
    
    DBGX_logIntArr                         (uint32_t CallingFilter, void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
    DBGX_logIntArr_NonFiltered             (                        void *IntArray, uint16_t NumberOfArrayElements, uint32_t Format)
    
    DBGX_logTime                           (uint32_t CallingFilter)
    DBGX_logTime_NonFiltered               (                      ) 
    
    DBGX_logLocat                          (uint32_t CallingFilter)
    DBGX_logLocat_NonFiltered              (                      )
    
    DBGX_logStrStr                         (uint32_t CallingFilter, const char *StringA, const char *StringB)
    DBGX_logStrStr_NonFiltered             (                        const char *StringA, const char *StringB)
    
    DBGX_logStrInt                         (uint32_t CallingFilter, 
                                                   const char *String, const void *IntegerAddress,
                                                      uint32_t Format)
    DBGX_logStrInt_NonFiltered             (      const char *String, const void *IntegerAddress, 
                                                     uint32_t Format)
                                               
    DBGX_logStrIntArr                      (uint32_t CallingFilter, 
                                                  const char *String, const void *IntArray, 
                                                      uint16_t NumberOfArrayElements, uint32_t Format)
    DBGX_logStrIntArr_NonFiltered          (      const char *String, const void *IntArray, 
                                                      uint16_t NumberOfArrayElements, uint32_t Format)
                                                 
    DBGX_logStrStrLocat                    (uint32_t CallingFilter, 
                                                const char *StringA, const char *StringB)
    DBGX_logStrStrLocat_NonFiltered        (    const char *StringA, const char *StringB)
                                                  
    DBGX_logStrStrLocatFull                (uint32_t CallingFilter, 
                                                const char *StringA, const char *StringB)
    DBGX_logStrStrLocatFull_NonFiltered    (    const char *StringA, const char *StringB)

    DBGX_logStrIntLocat                    (uint32_t CallingFilter, 
                                                const char *String, const void *IntegerAddress, 
                                                    uint32_t Format)
    DBGX_logStrIntLocat_NonFiltered        (    const char *String, const void *IntegerAddress,
                                                    uint32_t Format)
                                                                        
    DBGX_logStrIntLocatFull                (uint32_t CallingFilter, 
                                                const char *String, const void *IntegerAddress, 
                                                    uint32_t Format)
    DBGX_logStrIntLocatFull_NonFiltered    (    const char *String, const void *IntegerAddress,
                                                    uint32_t Format)
                                      
    See [A5] for CallingFilter.
    
#### API for Controller Input Data Transfer of Text Application
    
    int32_t DBGX_getString(uint16_t MaxLen, char *String)
    - Returns the number of characters
    - Waits (blocking) for text string, terminated by <CR> == 0x1C
    
<.pn>
#### API Interface for Writing of Exceptional Errors
    
**The API macros for writing of exceptional errors do the following steps:**
    
    - Saving the first occuring exceptional error code and error parameter
      after reset in RAM cells using the exceptional error memory (EEMI)
      * Error code is a unique identifier of the exceptional error
      * Error parameter is a 4-byte value (e.g. a return code of library function)
    
    - Saving the currently after first occuring exceptional error code and error 
      parameter after reset in a cyclic buffer using the exceptional error memory
      interface (EEMI)
    
    - Calling ASSERT for debug interface controller output of:
      * File name
      * Numeral of line
    
    - Calling debug interface controller output for file name, numeral of line,
      error code and error parameter: Output function of text application (for
      variable values and infos as ASCII Characters:
      DBGX_logInt and DBGX_logStr
    
**The API functions for writing exceptional errors are the following ones:**
    
    void DBGX_handleErr               (uint32_t ErrorCode)
    void DBGX_handleErrWithParam      (uint32_t ErrorCode, uint32_t Parameter)
    void DBGX_handleErrIf             (bool Condition, uint32_t ErrorCode)
    void DBGX_handleErrWithParamIf    (bool Condition, uint32_t ErrorCode, 
                                                                uint32_t ErrorParameter)
    uint8_t DBGX_readFirstError       (uint32_t *ErrorCode, uint32_t *ErrorParameter)
    - Returns the number of saved errors: 0 for no error, 1 for 1 error (first one is the last one), 
                                                          2 for 2 errors (first one and last one)
    uint8_t DBGX_readLastError        (uint32_t *ErrorCode, uint32_t *ErrorParameter)
    - Returns the number of saved errors: 0 for no error, 1 for 1 error (first one is the last one), 
                                                          2 for 2 errors (first one and last one)
    
<.pn>
### Protocol Processing Client Interface (CLI)
    
#### Protocol Processing Client Interface for Controller Output Data Transfer  of Touch Derived Application
    
    int32_t DBGP_sendProtData(const char *DbgAttributes,
                              uint16_t Module_ID, uint16_t Function,
                              const uint8_t *Data, uint32_t DataLen)
    
#### Protocol Processing Client Interface for Controller Input Data Transfer  of Touch Derived Application
    
    int32_t DBGP_notifyProtReceiveData_CB(DBGP_ReceiveDataFunctionOfApi_t 
                                                         ReceiveDataFunctionOfProt_CB)
    ReceiveDataFunctionOfProt_CB:
           void DBGX_receiveProtData_CB(uint16_t Module_ID, uint16_t Function,
                                                      uint8_t *Data, uint16_t DataLen)
             - Calls the right application callback function (from the right sender)
               using the notified sender related function pointers
    
    See [A7] for the callback calling chain up to the API.
    
#### Protocol Processing Client Interface for Controller Output Data Transfer  of Text Application (for Variable Values and Infos as ASCII Characters)
    
    int32_t DBGP_writeProtCharArray(const char *DbgAttributes, const char *CharArray,
                                    uint32_t NumberOfArrayElements)
    
#### Protocol Processing Client Interface for Controller Input Data Transfer  of Text Application
    
    int32_t DBGP_getProtString(uint32_t MaxLen, char *String)
    - Returns the length of the string
    - Waits (blocking) for text string, terminated by <CR> == 0x1C
    
### Configuration Interface (CFGI)
    
    DBGX_initDebugAPI...:
    void DBGX_initDebugAPI_ForCallbacks (DBGX_ApiCallbacksGen_s *ApiCallbacks)
    void DBGX_initDebugAPI_ForFilters   (DBGX_Filter_s *FilterRef)
    - Are set functions defined inside the configured area, called inside the
      configuration area
    - Parameter structure of ApiCallbacks contains those elements:
      * MaxNumberModules_Rx
        + Number of modules (is number of receive data functions)
      * ReceiveDataFcts_CB
        + Pointer on array of receive data functions
      * Modules
        + Pointer on array of modules
    
### Exceptional Error Memory Interface (EEMI)
    
    void DBGE_saveError          (uint32_t ErrorCode)
    void DBGE_saveErrorWithParam (uint32_t ErrorCode, uint32_t ParameterOfError)
    
    uint8_t DBGE_readFirstError  (uint32_t *ErrorCode, uint32_t *ErrorParameter)
    - Returns the number of saved errors: 0 for no error, 1 for 1 error (first one is the last one), 
                                                          2 for 2 errors (first one and last one)
    
    uint8_t DBGE_readLastError   (uint32_t *ErrorCode, uint32_t *ErrorParameter)
    - Returns the number of saved errors: 0 for no error, 1 for 1 error (first one is the last one), 
                                                          2 for 2 errors (first one and last one)
    
### Exceptional Error Configuration Interface (ECFGI)
    
    void DBGE_initErrorsMem(DBGE_ERROR_MEMORY_t *ErrorsMemory)
    - Is a set function defined inside the configured area, called inside the
      configuration area
    - Parameter structure contains those elements:
      ErrorCodes, ErrorParameters
    
<.pn>
## Formal Points
    
### Abbreviations
    
    API:                 Application Interface
    MiKo:                Michal Kopcik
    SvJo:                Sven Joppien
    
### Companion Documents
    
[1] Framework NG: Debug Data: What-To-Do Requirements

- [https://github-bshg.boschdevcloud.com/Generic/Framework](https://github-bshg.boschdevcloud.com/Generic/Framework)  
  \\common\\prog\\debug\\doc\\FNG_DebugDataWhatToDoReq.md
- Printed document: See [https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Generated+Documents+from+Repository](https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Generated+Documents+from+Repository)
    
### History of Versions
    
    Date        Author   Version   Contents
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    02.04.2019  SvJo     190402    NEW: First release
    03.04.2019  SvJo     190403    MOD: Wording more understandable
                                   NEW: More exact description of the API callback
                                        function
    04.04.2019  SvJo     190404    NEW: [A7] about the callback calling chain
    05.04.2019  SvJo     190405    MOD: Results from the code review (MiKo, SvJo)
                                        on 4.4.2019 (from version 190404),
                                        see review protocol
                                        review_FNG_DebugDataHowToDoReq_190404.txt
                                   MOD: Current link on the what-to-do reqirements
    11.04.2019  SvJo     190411    MOD: dbgx_debug_api_*cfg.* instead of 
                                        dbgx_debug_api_cfg.*
    12.04.2019  SvJo     190412    MOD: Client interface functions with "Prot" in the
                                        name
                                   MOD: Configuration interface: init. function
    15.05.2019  SvJo     190415    NEW: Annotaion [A8] about the data elements in
                                        CFG and APP
    16.04.2019  SvJo     190416    MOD: Results from the code review (MiKo, SvJo)
                                        on 16.4.2019 (from version 190415),
                                        see review protocol
                                        review_FNG_DebugDataHowToDoReq_190415.txt
    17.04.2019  SvJo     190417    FIX: Doubled DBGX_vInitDebugAPI in the function 
                                        name fixed
                                   FIX: Version 190416 instead of 190405 in the
                                        history here
                                   MOD: DBGP_*GetProtString returns the length of the
                                        string (not as parameter in the list)
    18.04.2019  SvJo     190418    NEW/MOD: 
                                        Some details resulting from the simulation
    21.05.2019  SvJo     190521    MOD: Indentifiers now without Hungarian Notation
    Versioning is going on in Git, since checked in there
    
<.pn>
## Annotations
    
### [A2]  IDs Already in Use and Future ID Definitions in Respect to the  Target of Data from PC for the Touch Derived Application Protocol  Data
    
    IDs Already in Use
    ------------------
    
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ID     Module (Target or Sender   | Function
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    0x00   Touch and PC-GUI           | Framed test data stream
    0x52   Touch and PC-GUI           | Read  request, read  response
    0x57   Touch and PC-GUI           | Write request, write response
                                      |
    0x45   System master              | Touch events 
    
    The asterisk * below stands for {0,1,2,...,E,F}
    
    ID: 1st half byte: Target or sender (is the module, e.g. touch)
        2nd          : Function
    
    IDs Already in Use and Future IDs
    ---------------------------------
    
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ID     Module (Target or Sender)
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    0x0*   Touch                  and PC-GUI (log data)
    0x5*   Touch                  and PC-GUI (configuration and test messages)
    -       -       -       -       -       -       -       -       -       -
    0x1*   Temperature regulation and PC-GUI
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    0x2*   Motor control          and PC-GUI
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    0x3*   ...
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    0x6*   ...
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    ...    ...
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    0xD*   ...
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    0xE*   Text Application (for Variable Values and Infos as ASCII Characters)
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    0x4*   System master
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    0x*F   Reserved for future unlimited extensions with function in PL
    0xF*   Reserved for future unlimited extensions with module in pay load
    
### [A4]  Layers and Their Used Interface Parameters in API and Configuration
    
    -   Parameter not relevant
    W   Writing debug data from controller to PC at API write function
    R   Reading debug data from PC to controller at API read callback function
    r   Notifying Reading debug data from PC to controller at API notify read 
        callback function
    
                  API Parameters                  
    ----------------------------------------------
                  CallingFilter             
                        Module (Target or Sender)
                              Function
                                    Data / DataLen
    ==============================================
                  -W    rW    RW    RW            
    ----------------------------------------------
    DBG:
     API
     Related
     Processing
    ----------------------------------------------
                  --    RW    RW    RW            
    ----------------------------------------------
    DBGP:
     Protocol
     Processing
    ----------------------------------------------
    
<.pn>
### [A5]  Attributes of the Calling Filter
    
#### [A5a] Attributes of Usage
    
**The user calls the debug output macros of text application at the  
calling locations, for instance:**
    
    //Hundreds or thousands of such callings can be used and stay in the
    //numerous source files for re-usage in later stages of development.
    
    DBGX_logStr_SCN_COOKING1("Start"); //Filter is a scenario (COOKING1)
                        --------
    
    DBGX_logStr_SCN_POWER(MyStringA);  //Filter is a scenario (POWER)
                        -----
    
    DBGX_logChar_SCN_COOKING2(MyString);  //Filter is a scenario (COOKING2)
                      --------
    
    DBGX_logChar_SCN_COOKING2('!');       //Filter is a scenario (COOKING2)
                      --------
    
    DBGX_logStr_SCN_CLOCK(MyString9);  //Filter is a scenario (CLOCK)
                        -----
    
    DBGX_logChar_SCN_CLOCK(Char1);        //Filter is a scenario (CLOCK)
                      -----
    
    DBGX_logStr_SCN_STEAM(MyInfo);     //Filter is a scenario (STEAM)
                        -----
    
    DBGX_logChar_INFO('?');               //Filter is a level    (ERR, WARN, INFO)
                  ----
    
    DBGX_logStr_ERR_SCN_COOKING1(MyStringX);
                    ---     --------       //Filter is combined by
                                           //          a level    (ERR, WARN, INFO) and
                                           //          a scenario (COOKING1)
    
**Or the user calls the debug output function of touch derived  
application, for instance:**
    
    //Can also be called additionally to the callings of debug output macros of text
    //application.
    //Is usually called only at one source code location, in tch_com_XS.c.
    
    For touch stream output:
    - - - - - - - - - - - - -
    Status = 
      DBGX_sendData(DBGX_MODULE_TOUCH_STREAM_OUT, DBGX_SCN_TOUCH, 
                    DBGX_FUNCTION_TOUCH_STREAM_OUT_1,
                    MyArray, MyArrayLen);
                                           //Filter is the scenario TOUCH
    
    For read request and read response of variables:
    - - - - - - - - - - - - - - - - - - - - - - - - -
    Status = 
      DBGX_sendData(DBGX_MODULE_TOUCH_RW_MSG, DBGX_SCN_TOUCH, 
                    DBGX_FUNCTION_TOUCH_R_REQ_RESP,
                    MyArray, MyArrayLen);
    
    For write request and write response of variables:
    - - - - - - - - - - - - - - - - - - - - - - - - -
    Status = 
      DBGX_sendData(DBGX_MODULE_TOUCH_RW_MSG, DBGX_SCN_TOUCH, 
                    DBGX_FUNCTION_TOUCH_W_REQ_RESP,
                    MyArray, MyArrayLen);
    
**The user defines his filters in file "filters.spc" and generates C-Code  
definitions by using of "gen_filters.bat" (reacheable by calling  
"make gen_dbgx_filters"):**
    
    #Filters at Calling Locations: Static def/undef, Dynamic on/off:
    #- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ERR:      def,   on  #ERR, WARN, INFO reserved for verbose level
    WARN:     def,   on
    INFO:     undef, off
    
    TOUCH:    def,   on  #For the touch derived application
    
    POWER:    def,   on  #From here on text application scenarioes
    CLOCK:    undef, off
    STEAM:    undef, off
    ...
    
    ...
    COOKING1: def,   on
    COOKING2: undef, off
    ...
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
<.pn>
                         /
                        / //Values of the calling filters with index of Word,
                        | //of Level Bit and of Scenario Bit in
                        | //DBGX_CentralConfigFilter_Cfg.FilterFlagWords[]:
                        | //------------------------------------------------
                        |
                        | #define DBGX_ERR_BITIDX          ((uint32_t)1 )
                        | #define DBGX_WARN_BITIDX         ((uint32_t)2 )
                        | #define DBGX_INFO_BITIDX         ((uint32_t)3 )
                        |
                        | #define DBGX_SCN_TOUCH_BITIDX    ((uint32_t)1 )
                        | #define DBGX_SCN_POWER_BITIDX    ((uint32_t)2 )
                        | #define DBGX_SCN_CLOCK_BITIDX    ((uint32_t)3 )
                        | #define DBGX_SCN_STEAM_BITIDX    ((uint32_t)4 )
                        | ...
                        | #define DBGX_SCN_COOKING1_BITIDX ((uint32_t)28 )
                        | #define DBGX_SCN_COOKING2_BITIDX ((uint32_t)29 )
                        | ...
                        |
                        | #define DBGX_ERR          ((DBGX_ERR_BITIDX           << DBGX_LEV_BIT_IDX_SHIFT))
                        | #define DBGX_WARN         ((DBGX_WARN_BITIDX          << DBGX_LEV_BIT_IDX_SHIFT))
                        | #define DBGX_INFO         ((DBGX_INFO_BITIDX          << DBGX_LEV_BIT_IDX_SHIFT))
                        |
                        | #define DBGX_SCN_TOUCH    ((DBGX_SCN_TOUCH_BITIDX     << DBGX_SCN_BIT_IDX_SHIFT) | (uint32_t)1 )
                        | #define DBGX_SCN_POWER    ((DBGX_SCN_POWER_BITIDX     << DBGX_SCN_BIT_IDX_SHIFT) | (uint32_t)1 )
                        | #define DBGX_SCN_CLOCK    ((DBGX_SCN_CLOCK_BITIDX     << DBGX_SCN_BIT_IDX_SHIFT) | (uint32_t)1 )
                        | #define DBGX_SCN_STEAM    ((DBGX_SCN_STEAM_BITIDX     << DBGX_SCN_BIT_IDX_SHIFT) | (uint32_t)1 )
                        | ...
                        | #define DBGX_SCN_COOKING1 ((DBGX_SCN_COOKING1_BITIDX  << DBGX_SCN_BIT_IDX_SHIFT) | (uint32_t)9 )
                        | #define DBGX_SCN_COOKING2 ((DBGX_SCN_COOKING2_BITIDX  << DBGX_SCN_BIT_IDX_SHIFT) | (uint32_t)9 )
                        | ...
                        |
                        | //Initialization of the filter flag words in
                        | //DBGX_CentralConfigFilter_Cfg.FilterFlagWords[]:
                        | //----------------------------------------------
                        |
                        | #define DBGX_FILTER_FLAG_WORDS_CENTRAL_CONFIG {                                                    \ 
                        |                                                                            /* Filter Flag Word */ \ 
                        |   (                                                                                               \ 
    filters_aut.h,      |         (uint32_t)((uint32_t)1 << (DBGX_ERR_BITIDX          - (uint32_t)1)) /* 0                */ \ 
    automatically     <-+       | (uint32_t)((uint32_t)1 << (DBGX_WARN_BITIDX         - (uint32_t)1)) /* 0                */ \ 
    generated by        |       | (uint32_t)((uint32_t)0 << (DBGX_INFO_BITIDX         - (uint32_t)1)) /* 0                */ \ 
    gen_filters.bat     |   ),                                                                                              \ 
                        |   (                                                                                               \ 
                        |         (uint32_t)((uint32_t)1 << (DBGX_SCN_TOUCH_BITIDX    - (uint32_t)1)) /* 1                */ \ 
                        |       | (uint32_t)((uint32_t)0 << (DBGX_SCN_CLOCK_BITIDX    - (uint32_t)1)) /* 1                */ \ 
                        |   ),                                                                                              \ 
                        |   ...
                        |   (                                                                                               \ 
                        |       ...
                        |       | (uint32_t)((uint32_t)1 << (DBGX_SCN_COOKING1_BITIDX - (uint32_t)1)) /* 9                */ \ 
                        |       | (uint32_t)((uint32_t)0 << (DBGX_SCN_COOKING2_BITIDX - (uint32_t)1)) /* 9                */ \ 
                        |       ...
                        |   ),                                                                                              \ 
                        |   ...
                        | }
                        |
                        | //Macros DBGX_log*(...) are empty or they call DBGX_log*(DBGX_SCN_*, ...):
                        | //-------------------------------------------------------------------------
                        |
                        | #define DBGX_logChar_SCN_POWER(char)        DBGX_logChar(DBGX_SCN_POWER, char)
                        | #define DBGX_logStr_SCN_POWER(string)       DBGX_logStr(DBGX_SCN_POWER, string)
                        | ...
                        | #define DBGX_logChar_SCN_COOKING1(char)     DBGX_logChar(DBGX_SCN_COOKING1, char)
                        | #define DBGX_logStr_SCN_COOKING1(string)    DBGX_logStr(DBGX_SCN_COOKING1, string)
                        | ...
                        | #define DBGX_logChar_SCN_CLOCK(char)
                        | #define DBGX_logStr_SCN_CLOCK(string)
                        | ...
                        | #define DBGX_logChar_SCN_STEAM(char)
                        | #define DBGX_logStr_SCN_STEAM(string)
                        | ...
                        | #define DBGX_logChar_SCN_COOKING2(char)
                        | #define DBGX_logStr_SCN_COOKING2(string)
                        | ...
                        | And the additional definitions for ERR, WARN, INFO, e.g.:
                        | #define DBGX_logChar_INFO
                        \ #define DBGX_logStr_ERR_SCN_COOKING1(string) DBGX_logStr(DBGX_ERR | DBGX_SCN_COOKING1, string)
                         \
    
<.pn>
                         +----------------------+
        filters.spc      | make            via  |     filters_aut.h
    -------------------->|  dbgx_gen_filters.awk |------------------------>
                         +----------------------+
    
**The macro / function with filter parameter calls conditionally an unfiltered  
debug output function, for instance:**
    
      DBGX_logStr_SCN_POWER(string)
      |                   -----
      | calls, if defined
      +------------------> DBGX_logStr(DBGX_SCN_POWER, string)
                           |                       -----
                           | calls, if on
                           +-------------> DBGP_writeProtCharArray(level_and_scenario, 
                                                                     string, length)
    
      Status =
      DBGX_sendData(DBGX_MODULE_TOUCH_STREAM_OUT, DBGX_SCN_TOUCH, DBGX_FUNCTION_TOUCH_STREAM_OUT,
      |                                                 ----- 
      |                                                        MyArray, MyArrayLen)
      | calls, if on
      +------------------> DBGP_sendProtData(level_and_scenario,
                                             Module_ID, Function, Data, DataLen)
    
**Notations of "defined", "undefined", "on", "off" in filters.spc explained by  
the filter example POWER (is the same for other filter scenarioes and for the  
filter levels ERR, WARN, INFO):**
    
      POWER:  undef, off #POWER filtered scenario is not defined;
                         #cannot be (dynamically) switched on by variable
    
      POWER:  undef, on  #Makes no sense; do not use it, because undefined filtered 
                         #scenario cannot be (dynamically) switched on by variable
    
      POWER:  def, off   #POWER filtered scenario is defined and switched off;
                         #can be (dynamically) switched on by variable
    
      POWER:  def, on    #POWER filtered scenario is defined and switched off;
                         #is (dynamically) switched on by variable
    
**Variable for dynamically switching on / off for a filter:**
    
    - Variable is: DBGX_CentralConfigFilter_Cfg.FilterFlagWords[]
    
    - Variable can be set with values using tools like Edith or Labview
    
    - The values of the variable can be generated using a script:
    
                                +---------------------+
        filters_dyn.spc         | gen_filters_dyn.bat |    filters_dyn_aut.txt
    --------------------------->|                     |---------------------------->
                 |              +---------------------+             |
                 |                                                  |
      Is a working copy of                                    Contains the values of
      filters.spc                        DBGX_CentralConfigFilter_Cfg.FilterFlagWords
    
#### [A5b] White-Box Attributes
    
**Contents of Calling Filter Parameter:**
    
    Example:
    #defineDBGX_logStr_SCN_COOKING1(string)     DBGX_logStr(DBGX_SCN_COOKING1, string)
    
    The calling filter is here the value DBGX_SCN_COOKING1.
    
    The calling filter parameter getting the value has the following contents, bitwise:
    
    Index       Value    Mask                                     Encode  Decode
    of                                                            Bit     Bit
                                                                  Shift   Shift
    - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Level Word  0        -                                        -       -
    Scen. Word  1,2, ..  0000.0000.0000.0000.WWWW.WWWW.WWWW.WWWW  0       0
    Lev.  Bit   1 .. 32  bbbb.bb00.0000.0000.0000.0000.0000.0000  <<26    >>26
    Scen. Bit   1 .. 32  0000.00BB.BBBB.0000.0000.0000.0000.0000  <<20    >>20
    
    In generic area: #define DBGX_LEV_BIT_IDX_SHIFT   26
                     #define DBGX_SCN_BIT_IDX_SHIFT   20
    
    
**Level and Scenario Can Be Used Separately Or Combined in a Calling Filter  
Variable:**
    
    Examples for separate usage:
    - - - - - - - - - - - - - - -
    
   DBGX_logStr_SCN_COOKING1(string) => DBGX_logStr(DBGX_SCN_COOKING1, string)
    
    Example for combined usage:
    - - - - - - - - - - - - - -
    
   DBGX_logStr_ERR_SCN_COOKING1(string) => DBGX_logStr(DBGX_ERR | DBGX_SCN_COOKING1, string)
    
    
<.pn>
### [A7]  Application Callback Function and Callback Function of the Protocol  Layer for Controller Input Debug Data
    
**Callback Functions and Notify Functions:**
    
        DBGX_notifyReceiveData_CB         (.., Module_ID)
           (..): APP_ReceiveDebugData_CB (Function, Data, DataLen)
    
                          ^
                          | Function pointer
                          | DBGX_ApiCallbacks.ReceiveDataFunctionsOfAPI_CB[]
                          | calls the right application
                          | callback function
                          |
                          | These function pointers are module related:
                          | Array of function pointers with module as index
                          |
    +---------------------------------------------------------------------------------+
    |DBGX_ApiCallbacks.ReceiveDataFunctionsOfAPI_CB[Module_Index]                      |
    |                                                     = &APP_ReceiveDebugData_CB  |
    +---------------------------------------------------------------------------------+
                          ^
                          | Function pointer DBGP_ReceiveDataFunctionOfProt_CB
                          | calls the right application
                          | callback function pointer (from the
                          | right module [target / sender]):
                          |
        DBGP_notifyProtReceiveData_CB       (..)
            (..): DBGX_receiveProtData_CB (Module_ID, Function, Data, DataLen)
    
**Callback Functions and Function Pointers:**
    
    APP_ReceiveDebugData_CB           (Function, Data, DataLen)                1*)
                 ^
                 | call
                 |
    DBGX_ApiCallbacks.ReceiveDataFunctionsOfAPI_CB[Module_Index]
                                      (Function, Data, DataLen)
    - Is a function pointer
                 ^
                 | call
                 |
    DBGX_receiveProtData_CB            (ModuleID, Function, Data, DataLen)
                 ^
                 | call
                 |
    
    DBGP_ReceiveDataFunctionOfProt_CB (ModuleID, Function, Data, DataLen)      2*)
    - Is a function pointer
    
    1*) Function name can be freely defined in the application, example name here.
    
    2*) Function pointer name of the protocol layer could also be another one;
        name of a test pointer is written down here.
    
<.pn>
### [A8]  Data in the Generic Part and in the Configuration Part of the Software
    
#### [A8a] Structure Type Names and Structure Variables
    
**For the Callback Functions:**
    
    Generic Part                      Configuration Part
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    DBGX_ApiCallbacksGen_s                  DBGX_ApiCallbacksCfg_s
          DBGX_API_Callbacks;               DBGX_API_Callbacks_Cfg;
    
**For the Error Memory:**
    
    Generic Part                      Configuration Part
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    DBGE_ERRORS_MEMORY_GEN_t               DBGE_ERRORS_MEMORY_CFG_t
          DBGEs_ErrorsMemory;               DBGEs_ErrorsMemory_Cfg;
    
#### [A8b] Structure Components for the Callback Functions
    
    Generic Part                      Configuration Part
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    DBGX_ApiCallbacksGen_s                  DBGX_ApiCallbacksCfg_s
          DBGX_API_Callbacks;               DBGX_API_Callbacks_Cfg;
    -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
    
    MaxNumberModules_Rx               -
    
    ReceiveDataFunctionsOfApi_CB      ReceiveDataFunctionsOfApi_CB
                                          [DBGX_MAX_NUMBER_MODULES_RX]
    - Function pointer                - Array of function pointers
    
    ModuleIDs                         ModuleIDs
                                          [DBGX_MAX_NUMBER_MODULES_RX]
    - Pointer                         - Array of function pointers
    
    - The configuration part contains the newly compileable data elements:
      * Changeable C-preprocessor constants
      * Changeable for length statically allocated memory arrays
    
    - The generic part contains the once compiled and setteable data elements,
      setteable by initialization function:
      * Pointers to memory areas
      * Scalar variables with few memory volume
    
### [A9]  Exceptional Error Memory Processing with its Configuration Area
    
                                          |
    +-------------------------------------|---------------------------------------------+
    |                                     |                                             |
    |                                     |                                             |
    |                                     |                                             |
    |                                     V                                             |
    |                     +-----------------------+                                     |
    |                     | DBGE: Exceptional     |                                     |
    |                     |       Error Memory    |                                     |
    |                     |       Processing      |                                     |
    |                     |       error.*         |                                     |
    |                     +-----------------------+                                     |
    +-----------------------------------------------------------------------------------+
    
                          ........................
                                   (EEMI)
                          DBGE_saveError
                          DBGE_saveErrorWithParam
                          DBGE_readFirstError
                          DBGE_readLastError
                          ........................
                                 |
                                 |             .....................
                          +--------------+                (ECFGI)
                          | Exceptional  |-----
                          | Error Memory |     DBGE_initErrorsMem
                          | Processing   |     .....................
                          +--------------+
                                 |


