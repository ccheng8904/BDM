# Framework NG: Debug Component: How-To-Do Requirements of the Protocol  Layer
    
In this document main points can be seen, how to implement the protocol layer  
of the debug component, according to the what-to-do requirements in [1].
    
For further explanations with protocol layer mentioned:

- An overview over the functions of the debug component (interfacing functions  
  of the protocol layer included) can be found in [3] and [4].
- Explanations about the API layer (layer above the protocol layer) are given  
  in [2].
    
As an example detailled points for text debug data output via the DBus can be  
seen in [A2].
    
## General Points about the Protocol Layer of the Debug Component
    
### Subject of the Protocol Layer
    
The protocols are:

- Text terminal protocol                     (TextP)
- Touch derived application protocol         (TouchDerP)
- Other protocols according to [1] (not yet to be implemented)
    
The protocol layer provides the following subjects:

- For debug data to be sent:
  * Converting the data to be sent, coming from the API layer of debug  
    component, to data of the configured protocol
  * Passing the converted data to be sent to the interface layer
- For received debug data:
  * Synchronizing of Rx-data on the protocol frames
  * Converting the received data, coming from the interface layer,  
    to the data for the API layer
  * Giving over the converted received data to the application layer
    
The origin functions of the API layer are:

- Text application functions                  (TextA)
  * Application is text application
- Touch derived application functions         (TouchDerA)
  * Application can be e.g. touch, temperature regulation, motor  
    control
- See [A1] for the usage of the expressions "module" and "application"
    
<.pn>
### Overview and Mapping of Protocol Layer into the Software Structure of  Debug Component
    
A layer structure, for protocol and interface processing outlined by MiKo,  
and the what-to-do requirements, written down in [1], are assumed for the  
software structure here.
    
The layers of the debug component are:

- API layer
- Protocol layer (subject of this document here)
- Interface layer
    
The layers have to be implemented as C-modules for easy modifying, with  
connected configuration modules.
    
    +-------------------------------------|---------------------------------------------+
    | Debug Component                     |                                             |
    | (Directory debug                    |             This layer is described         |
    |  for C files)                       |             in this document here.          |
    |                                     |                 .                           |
    |                                     +----------------.--------------+             |
    |                                     |               .               |             |
    |                                     V              .                V             |
    | +-------------+     +----------------------+      .      +----------------------+ |
    | | DBGE:       |     | DBG:  API Layer      |     .       | DBG:                 | |
    | |  Exceptional|<----|                      |<---.--------|  Configuration Area  | |
    | |  Error      |     |       api.*          |   .         |  api_cfg.*           | |
    | |  Memory     |     +----------------------+  .          |  api_error_cfg.*     | |
    | |  Processing |                     |        .           |  filters_aut.h       | |
    | |  with its   |                     |       .            +----------------------+ |
    | |  Config.    |                     |      .                              |       |
    | |  Area       |  ...................|.....................................|.......|
    | |             |  .                                                        |      .|
    | |             |  .                  +-------------------------------+     |      .|
    | |             |  .                  |                               |     |      .|
    | |             |  .                  |                               |     |      .|
    | |             |  .                  V                               V     V      .|
    | |             |  .  +-----------------------+            +----------------------+.|
    | |             |<----| DBGP: Protocol Layer  |<-----------| DBGP:                |.|
    | |             |  .  |                       |            |  Configuration Area  |.|
    | |             |  .  |       protocol.*      |            |  protocol_cfg.*      |.|
    | |             |  .  +-----------------------+            +----------------------+.|
    | |             |  .                  |                                     |      .|
    | |             |  .                  |                                     |      .|
    | |             |  ...................|.....................................|.......|
    | |             |                     |                                     |       |
    | |             |                     |                                     |       |
    | |             |                     +-------------------------------+     |       |
    | |             |                     |                               |     |       |
    | |             |                     V                               V     V       |
    | |  dbge_      |     +-----------------------+            +----------------------+ |
    | |    debug_   |<----| DBGI: Interface Layer |<-----------| DBGI:                | |
    | |      err.*  |     |                       |            |  Configuration Area  | |
    | |             |     |       interface.*     |            |  interface_cfg.*     | |
    | +-------------+     +-----------------------+            +----------------------+ |
    |                                     |                                             |
    +-------------------------------------|---------------------------------------------+
                                          V                 The arrows above tell "uses".
    
<.pn>
## Protocols
    
Only at a first glance the protocol conception with its numerous cases of usage  
seems to be quite much for the user, how to understand it.
    
With a user's manual or support explanations involved, starting with exact examples,  
the protocols should be easily understandable, with the teached knowledge, how to  
configure the protocol needed.
    
For many users it is going to be sufficient, only to know, how special cases of  
usage have to be handled, most likely just the usage of text application with  
text terminal protocol outputs.
    
And for specialized or very sophisticated users we have many kinds of protocol  
usage, for example the usage of touch derived application with touch derived  
protocol outputs and inputs doing the EMC tests.
    
Input or output are meant from the point of view of the controller here.
    
### TextP:    Text Terminal Protocol
    
    - Format on the Interface:             As ASCII Characters, useable in standard text terminal
                                           programs
    - Frame Contents for Output String:    <Module><Level><Scenario>:<Data>
                                           * See [A3] for further explanations
    - Frame Contents for Input String:     <User_Defineable_String>
                                           * E.g. "DBGX_EVT_COOLING_DOWN(POWER_BRIDGING)"
    
    - The ASCII characters of the string are written directly to the terminal
    - The characters 0x0D 0x0A are written implicitely at the end of the frame by the 
      protocol layer for output
    - For input 0x0D 0x0A or 0x0A only are recognized as string termination in the 
      controller
    - For output by preprocessor conditioning the implicite 0x0D 0x0A can be suppressed
    - For output by preprocessor conditioning the level and scenario can be suppressed
    
<.pn>
### TouchDerP:  Touch Derived Application Protocol
    
    - Format on the Interface:  Binary
    - General Frame Contents:   <LEN><ID><PAY_LOAD><CRC>
    
                                Number of Bytes         Element
                                - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                                1                       LEN
                                1                       ID
                                LEN-4                   PAY_LOAD (user data; 16-bit sample values)
                                2                       CRC (over LEN, ID, PAY_LOAD)
    
                                Endianess is the same as on the target controller.
    
                                Special case: Extended touch sample frames:
                                - Contain sub-framed functional data (events)
                                - Are special debug frames with ID 0x45
    
                                See docu for further explanations (contents valid on 25.1.2019):
                                http://wiki.bshg.com/pages/viewpage.action?pageId=190088500
    
                                See docu from Valeriy Yatsenko for further explanations (contents valid on
                                25.1.2019):
                                http://wiki.bshg.com/display/operatingtechnologies/Generic+serial+touch+protocol
    
                                IDs Already in Use (Requirements from the Touch Library)
                                --------------------------------------------------------
    
                                - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                                ID     Module (Target or Sender   | Function
                                - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                                0x00   Touch and PC-GUI           | Framed test data stream
                                0x52   Touch and PC-GUI           | Read  request, read  response
                                0x57   Touch and PC-GUI           | Write request, write response
                                                                  |
                                0x45   System master              | Touch events 
    
                                Touch variables for the read requests and write requests:
                                - Variables for debug pre-settings:
                                  TCH_DebugConfig.DebugMasks.DebugDataMask[0]
                                  TCH_DebugConfig.DebugMasks.DebugDataMask[1]
                                  TCH_DebugConfig.DebugMasks.ElementEnableMask
                                - Variable for reading out of touch debug data:
                                  TCH_DebugSignals[]   (e.g. for raw data, filtered data and so on, according to the pre-settings)
    
                                ID: 1st half byte: Target or sender (is the module, e.g. touch)
                                    2nd          : Function
    
                                IDs Already in Use and Future IDs
                                ---------------------------------
    
                                * stands for {0,1,2,...,E,F}
    
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
                                0x4*   System master
                                -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
                                0x6*   ...
                                -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
                                ...    ...
                                -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
                                0xD*   ...
                                -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
                                0xE*   Text application (for Variable Values and Infos as ASCII Characters)
                                - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                                0x*F   Reserved for future unlimited extensions with function in pay load
                                0xF*   Reserved for future unlimited extensions with module   in pay load
    
    - For Text Application Functions:
                                * ID 0xE0 is used
                                * The ASCII codes of the string are the pay load, null-terminated
    
    - For Touch Derived Application Functions:
                                * IDs 0x0*, 0x5*, 0x1*, 0x2*, 0x3*, 0x6*, ..., 0xD*, 0x4*, 0xF* are used
                                * The application (e.g. touch library) determines the frame IDs and the pay load
                                  structurizing for sending and receiving. The PC-GUI must work in the way, as the
                                  IDs and pay load structurizing are defined
    
<.pn>
### Considering the Protocol Outputs via the DBus Interface
    
To save hardware efforts it is useful (for not very frequently occuring debug  
data) to have the DBus interface as debug output. Also input of debug data are  
useful via DBus.
    
    - Format on the Interface:  Packed as pay load (user data) into binary DBus message frames with DBus message ID 0x,,,,
    - DBus message IDs are:     MSGID_MSG_SEND_DBGX_DATA_TO_PC = 0xB9FE and MSGID_MSG_RECEIVE_DBGX_DATA_FROM_PC = 0xB9FF
    - General Frame Contents:   MSG_SEND_DBGX_DATA_TO_PC      (<Text_Frame_or_Touch_Derived_Binary_Frame>)
                                MSG_RECEIVE_DBGX_DATA_FROM_PC (   ~                                      )
    
    - For Text Application Functions:
      * General Frame Contents: MSG_SEND_DBGX_DATA_TO_PC      (<Text_Frame>)
                                MSG_RECEIVE_DBGX_DATA_FROM_PC (<Text_String>)
    
      * Frame Contents for Output:
                                MSG_SEND_DBGX_DATA_TO_PC      (<Module><Level><Scenario>:<Data>)
                                + See [A3] for further explanations
    
      * Frame Contents for Input:
                                MSG_RECEIVE_DBGX_DATA_FROM_PC (<User_Defineable_String>)
                                + E.g. MSG_RECEIVE_DBGX_DATA_FROM_PC ("DBGX_EVT_COOLING_DOWN(POWER_BRIDGING)")
    
      * The ASCII characters of the string are in the message pay load
      * String in the DBus message pay load is terminated with the bytes 0x0D, 0x0A
    
      * For the string: see the paragraph "TextP:    Text Terminal Protocol" above here
    
    - For Touch Application Functions, Currently Used
      * Is useable independently from the debug component
      * Usage of the generic DBus messages for reading and for writing of variables
        (read_request, read_response, write_request, write_response)
      * Variables for debug pre-settings:
        TCH_DebugConfig.DebugMasks.DebugDataMask[0]
        TCH_DebugConfig.DebugMasks.DebugDataMask[1]
        TCH_DebugConfig.DebugMasks.ElementEnableMask
      * Variable for reading out of touch debug data:
        TCH_DebugSignals[]   (e.g. for raw data, filtered data and so on, according to the pre-settings)
    
    - For Touch Derived Application Functions, Currently Not Yet to Be Implemented:
      * Is debug component supported
      * General Frame Contents: MSG_SEND_DBGX_DATA_TO_PC      (<Touch_Derived_Binary_Frame>), is the same as:
                                MSG_SEND_DBGX_DATA_TO_PC      (<ID><PAY_LOAD>)
                                MSG_RECEIVE_DBGX_DATA_FROM_PC (<Touch_Derived_Binary_Frame>), is the same as:
                                MSG_RECEIVE_DBGX_DATA_FROM_PC (<ID><PAY_LOAD>)
      * For <ID><PAY_LOAD> see the paragraph "TouchDerP:  Touch Derived Application Protocol" above here
      * MSG_SEND_DBGX_DATA_TO_PC        mainly for configuring the touch debug behaviour of the touch library
      * MSG_RECEIVE_DBGX_DATA_FROM_PC   mainly for transmission of touch debug data (e.g. raw data)
    
### Other Protocols
    
In [1] are mentioned other protocols. Those are not yet planned to be implemented.
    
<.pn>
## Functions for Sending and Functions for Receiving of Debug Data
    
To have an introduction as an overview see [3] and [4]. Functions maps of the debug component can be seen there.  
More detailed explanations about the functions can be seen in this chapter here.
    
### Functions for Sending of Debug Data
    
                       * Debug Output Function
                       +-----------------------------------------------------------------------------------------------------------
                       * TextA                                                TouchDerA
                       *
                       * Text Application Functions                           Touch Derived Application Function
                       *
           | DBG (API) * DBGX_log...:                                        DBGX_sendData:
           |           *
           | DBGP      * int32_t Status                                       int32_t Status
           |           *  DBGP_writeProtCharArray                              DBGP_sendProtData
           |           *   (const char *DbgAttributes,                          (const char *DbgAttributes,
           |           *    const char *CharArray, uint16_t ArrayLen)            uint16_t Module, uint16_t Function,
           \           *                                                         const uint8_t *Data, uint16_t DataLen)
            \----------+---
    *****+*************+***********************************************************************************************************
         |             *
    Pro- | TextP       * TextA_TextP                                          TouchDerA_TextP
    to-  |             *
    col  | Text        * int32_t Status                                       int32_t Status
         | Terminal    *  DBGP_build_TextA_TextP_Frame                         DBGP_build_TouchDerA_TextP_Frame
         | Protocol    *   (const char *DbgAttributes,                          (const char *DbgAttributes,
         | Data        *    const char *InCharArray, uint16_t InArrayLen,        uint16_t Module, uint16_t Function,
         |             *    uint8_t *OutBytesArray)                              const uint8_t *InBytesArray, uint16_t InArrayLen, 
         |             *                                                         uint8_t *OutBytesArray)
         | ------------+---
         | TouchDerP   * TextA_TouchDerP                                      TouchDerA_TouchDerP
         |             *
         | Touch       * int32_t Status                                       int32_t Status
         | Derived     *  DBGP_build_TextA_TouchDerP_Frame                    DBGP_build_TouchDerA_TouchDerP_Frame
         | Application *   (const char *DbgAttributes,                          (uint16_t Module, uint16_t Function,
         | Protocol    *    const char *InCharArray, uint16_t InArrayLen,        const uint8_t *InBytesArray, uint16_t InArrayLen,
         | Data        *    uint8_t *OutBytesArray)                              uint8_t *OutBytesArray)
         |             *
         | ------------+---
    
    
### Functions for Receiving of Debug Data
    
                       * Debug Input Function
                       +-----------------------------------------------------------------------------------------------------------
                       * TextA                                                TouchDerA
                       *
                       * Text Application Functions                           Touch Derived Application Function
                       *
           | APP       * APP_getString_CB:                                    APP_receiveData_CB:
           |           *
           | DBG (API) * DBGX_ApiCallbacks.GetStringFunctionOfApi_CB #FPtr     DBGX_ApiCallbacks.ReceiveDataFunctionsOfApi_CB[] #FPtr
           |           *
           |           * void DBGX_getProtString_CB                            void DBGX_receiveProtData_CB
           |           *       (uint16_t MaxLen, char *String)                      (uint16_t Module, uint16_t Function,
           |           *                                                             uint8_t *Data, uint16_t DataLen);
           |           *
           | DBGP      * DBGP_GetStringFunctionOfProt_CB            #FPtr     DBGP_ReceiveDataFunctionOfProt_CB               #FPtr
           |           *
           |           * void DBGP_receiveInterfaceData_CB                    void DBGP_receiveInterfaceData_CB
           |           *       (uint8_t *Bytes, uint16_t Len)                       (uint8_t *Bytes, uint16_t Len)
           \           *
            \----------+---
    *****+*************+***********************************************************************************************************
         |             *
    Pro- | TextP       * TextP_TextA                                          TextP_TouchDerA
    to-  |             *
    col  | Text        * void DBGP_ana_TextP_TextA_Frame                      uint32_t DataLen
         | Terminal    *       (uint8_t *InBytesArray, uint16_t InArrayLen,    DBGP_ana_TextP_TouchDerA_Frame
         | Protocol    *        char *OutString)                               (uint8_t *InBytesArray, uint16_t InArrayLen,
         | Data        *                                                        uint8_t *OutBytesArray)
         |             *
         | ------------+---
         | TouchDerP   * TouchDerP_TextA                                      TouchDerP_TouchDerA
         |             *
         | Touch       * void DBGP_ana_TouchDerP_TextA_Frame                  uint32_t DataLen
         | Derived     *       (uint8_t *InBytesArray, uint16_t InArrayLen,     DBGP_ana_TouchDerP_TouchDerA_Frame
         | Application *        char *OutString)                                (uint8_t *InBytesArray, uint16_t InArrayLen,
         | Protocol    *                                                         uint8_t *OutBytesArray)
         | Data        *
         |             *
         | ------------+---
    
<.pn>
## Configuration
    
In [A2.2] is described, how the protocols are configured.
    
(There is also shown, how each protocol is mapped to an interface in the
configuration.)
    
## Formal Points
    
### Protocol Layer Specific Abbreviations
    
    TextP      Text terminal protocol
    TouchDerP  Touch derived application protocol
    
    TextA      Text application functions
    TouchDerA  Touch derived application functions
    
### Companion Documents
    
[1] Framework NG: Debug Data: What-To-Do Requirements

- [https://github-bshg.boschdevcloud.com/Generic/Framework](https://github-bshg.boschdevcloud.com/Generic/Framework)  
  \\common\\prog\\debug_extended\\doc\\FNG_DebugDataWhatToDoReq.md
- Printed document: See [https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Generated+Documents+from+Repo](https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Generated+Documents+from+Repo)
    
[2] Framework NG: Debug Component: How-To-Do Requirements of the Application  
                                   Interface (API) Layer

- [https://github-bshg.boschdevcloud.com/Generic/Framework](https://github-bshg.boschdevcloud.com/Generic/Framework)  
  \\common\\prog\\debug_extended\\doc\\FNG_DebugDataAPIHowToDoReq.md
- Printed document: See [https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Generated+Documents+from+Repo](https://wiki.bsh-sdd.com/display/PEDF/Debug+Component%3A+Generated+Documents+from+Repo)

[3] Framework NG: Send Function Map of Debug Component: Essential Elements

- [https://github-bshg.boschdevcloud.com/Generic/Framework](https://github-bshg.boschdevcloud.com/Generic/Framework)  
  \\common\\prog\\debug_extended\\doc\\Function_map_DBG_Tx.pdf
    
[4] Framework NG: Receive Function Map of Debug Component: Essential Elements

- [https://github-bshg.boschdevcloud.com/Generic/Framework](https://github-bshg.boschdevcloud.com/Generic/Framework)  
  \\common\\prog\\debug_extended\\doc\\Function_map_DBG_Rx.pdf
    
### History of Versions
    
The document is versioned in Git.
    
<.pn>
## Annotations
    
### [A1]   Usage of the Expressions for the Debug Component: Application and  Module are Sysnonyms Here
    
Applications of the debug component are:

- Text  application   (send function DBGX_log...)
- Touch application   (send function DBGX_sendData)
    
Originally the expression "module" has been introduced for the application of  
the debug component.
    
So for the usage of the debug component "module" and "application" can be seen  
as synonyms.
    
(Special case for text application: As text module it must be seen as a sub-set  
of application software spread over one or more object modules, so the  
expression "module" must be seen in a sense being not defined too narrow.)
    
<.pn>
### [A2]   Points for the Text Terminal Protocols with DBus as Debug Output  Interface
    
#### [A2.1] Operation
    
Building of the protocol frames happens in the protocol layer using the functions  
DBGP_..., as shown in the overview below.
    
In the left part of the overview the text application processing, derived from  
the API functions DBGX_log_..., e.g. DBGX_logStr, is shown. The right part  
contains the processing derived from the touch derived API function DBGX_sendData.
    
The debug output data are FIFO-buffered inside the interface layer of the debug  
component (layer below the protocol layer).
    
Reading out the FIFO buffer, the interface layer fills in the debug output  
data as pay load of the DBus message MSG_SEND_DBGX_DATA_TO_PC = 0xB9FF.
    
So the data of text terminal protocol (TextP) or of touch derived application  
protocol (TouchDerP) are just packed into the DBus frames in the interface layer.
    
One interface has one protocol. Each interface has its own FIFO in the  
interface layer. So we have a FIFO for the DBus interface (another one for the  
Debug UART interface). Exception from having a FIFO is the cyclic RAM buffer as  
an interface, that is no bottle neck, so it does not need a FIFO.
    
    
    int32_t Status                                  int32_t Status
     DBGP_writeProtCharArray                         DBGP_sendProtData
      (const char *DbgAttributes,                     (const char *DbgAttributes,
       const char *CharArray, uint16_t ArrayLen)       uint16_t Module, uint16_t Function,
                                                       const uint8_t *Data, uint16_t DataLen)
    =========================================================================================
                                             |
                                             | call
                                             V
    +--------------------------------------------------------------------------------------+
    | int32_t Status                                int32_t Status                         |
    |  DBGP_build_TextA_TextP_Frame                  DBGP_build_TouchDerA_TextP_Frame      |
    |   (const char *DbgAttributes,                   (const char *DbgAttributes,          |
    |    char *InCharArray, uint16_t InArrayLen,       uint16_t Module, uint16_t Function, |
    |    const char *OutBytesArray)                    const uint8_t *InBytesArray,        |
    |                                                                 uint16_t InArrayLen, |
    |                                                  uint8_t *OutBytesArray)             |
    +--------------------------------------------------------------------------------------+
                                             |
                                             | call
                                             V
                 +------------------------------------------------------+
                 | int32_t Status DBGI_SendInterfaceBytes               |
                 |                 (uint8_t InterfaceIndex,             |
                 |                  const uint8_t *Bytes, uint16_t Len) |
                 |                                                      |
                 |   *Bytes   +------+                                  |
                 |   -------->| FIFO |--+                               |
                 |            +------+  |                               |
                 |                      |                               |
                 +----------------------|-------------------------------+
                                        |    |
                                        |    | call
                                        |    |
                                        |    V
                                        |  return
                                        |
                                  *Bytes|
                                        |
                                        |   +-------------+
                                        |   | Transfering |  DBusMessage, DBusMessage, ...
                                        +-->| to the      |-------------------------------> 
                                            | interface   |   MSG_SEND_DBGX_DATA_TO_PC
                                            +-------------+
    
    ==================================================================================
    
<.pn>
#### [A2.2] Configuration
    
**Constants with Their Variables:**
    
    protocol.c                                  interface.c
    - - - - - -                                 - - - - - -
    DBGP_Protocols //Ptr                        DBGI_Interfaces //Ptr
    
    protocol_cfg.c:                             interface_cfg.c:
    - - - - - - - -                             - - - - - - - - -
    DBGP_Protocols_Cfg[] = {DBGP_PROTOCOLS}
    
    protocol_cfg.h:                             interface_cfg.h:
    - - - - - - - -                             - - - - - - - - -
    #define DBGP_PROTOCOLS {     \              #define DBGI_INTERFACES {      \
      DBGP_PROTOCOL_TEXT,        \                DBGI_INTERFACE_DBGX_UART,    \
      DBGP_PROTOCOL_TCH_DER      \                DBGI_INTERFACE_DBUS,         \
                                 \                DBGI_INTERFACE_RAM,          \
                                 \                DBGI_SIMPLE_DEBUG            \
                                 
    }                                           }
    
      Can be choosen from:                        Can be choosen from:
        DBGP_PROTOCOL_TEXT,                         DBGI_INTERFACE_DBGX_UART,
        DBGP_PROTOCOL_TCH_DER,                      DBGI_INTERFACE_DBUS,
        DBGP_PROTOCOL_NONE                          DBGI_INTERFACE_RAM,
                                                    DBGI_SIMPLE_DEBUG,
                                                    DBGI_INTERFACE_NONE
    
      DBGX_USR_PROTOCOLS                           DBGX_USR_INTERFACES
        wraps DBGP_PROTOCOLS                        wraps DBGI_INTERFACES
        centrally for the user                      centrally for the user
    
    ----------------------------------------------------------------------------------
    
**Calling and Data Flow:**
    
    GEN                                             CFG
    ---                                             ---
    
                                                    DBGX_init()
                                                         :
                                                        3:call
    DBGP_Protocols //Pointer                             :
                                                         :
             ^                                           :
             |                                           :
            6|dat                                        :
             |                                           V
                                           4                                  6
    DBGP_setProtocolConfiguration(&...) <.......... DBGP_initProtocols_Cfg() .......
                                           call                               call :
                         ^                                                         :
                         |                                                         :
                         |                 5                                       :
                         +------------------------- DBGP_Protocols_Cfg[]           :
                                           dat                                     :
                                                               ^                   :
                                                               |                   :
                                                              2|dat, init by dcl   :
                                                               |                   :
                                                                                   :
                                                        DBGP_PROTOCOLS             :
                                                                                   :
                                                                                   :
    DBGI_Interfaces //Pointer                                                      :
                                                                                   :
             ^                                                                     :
             |                                                                     :
            9|dat                                                                  :
             |                                                                     :
             
      DBGI_INTERFACES
    
    ----------------------------------------------------------------------------------
    
<.pn>
**Layer Structure and User's C-Preprocessor Definitions Centralized in ONE file:**
    
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Allowed Feeding with Ressources (via Include Files) inside the Layer Structure 
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
                    ................         ....................
                    <applications>.c         <applications_cfg>.c
                    ................         ....................
                      ^                          ^     ^
                      |feeds                     |feeds|
                      |   +----------------------+     |
                      |   |                            |
                    ..........               ..............
                    api.c                    api_cfg.c
                    ..........               ..............
                      ^                          ^     ^
                      |feeds                     |feeds|
                      |   +----------------------+     |
                      |   |                            |
                    ..........               ..............
                    protocol.c               protocol_cfg.c
                    ..........               ..............
                      ^                          ^     ^
                      |feeds                     |feeds|
                      |   +----------------------+     |
                      |   |                            |
                    ...........              ...............
                    interface.c              interface_cfg.c
                    ...........              ...............
    
<.pn>
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Central User Changeable Definitions Connected to the Layer Structure
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
                        .................
                        . user_cfg.h:  // User changeable definitions in ONE file
                        .................
                        . #define DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED             -->+ 
                        . //#define DBGX_USR_BIN_DAT_FUNCTIONS_ENABLED            -->+ 
                        . #define DBGX_USR_RECEIVE_FUNCTIONS_OF_MODULES_INIT  {      | 
                        .   ,,,                                                      | 
                        . }                                                          | 
                        .                                                            | 
                        . #define DBGX_USR_PROTOCOLS  {                  \    ------>+ 
                        .   DBGP_PROTOCOL_NONE,       // On interface 1 \     <------+ 
                        .   DBGP_PROTOCOL_NONE,       // On interface 2 \     <------+ 
                        .   DBGP_PROTOCOL_NONE,       // On interface 3 \     <------+ 
                        .   DBGP_PROTOCOL_NONE,       // On interface 4 \     <------+ 
                        .   DBGP_PROTOCOL_NONE,       // On interface 5 \     <------+ 
                        . }                                                          | 
                        .      // Can be respectively choosen from:                  | 
                        .      //  DBGP_PROTOCOL_TEXT,                               | 
                        .      //  DBGP_PROTOCOL_TCH_DER,                            | 
                        .      //  DBGP_PROTOCOL_NONE                                | 
                        .................                                            | 
                                                                                     | 
                                                                                feeds| 
                       feeds                                                         | 
             .........  |  ...................                                       | 
             api_cfg.c <-- . api_cfg.h                                               | 
             .........     ...................                                       | 
                 ^         . #ifdef DBGX_USR_TEXT_DAT_FUNCTIONS_ENABLED              |
                 |         .   #define DBGX_TEXT_DAT_FUNCTIONS_ENABLED            <---+ 
                 |         . #endif                                                  |
                 |         . #ifdef DBGX_USR_BIN_DAT_FUNCTIONS_ENABLED               |
                 |         .   #define DBGX_BIN_DAT_FUNCTIONS_ENABLED             <---+ 
                 |         . #endif                                                  |
                 |         .                                                         |
                 |         . #define DBGX_RECEIVE_FUNCTIONS_OF_MODULES_INIT \     <---+ 
                 |         .           DBGX_USR_RECEIVE_FUNCTIONS_OF_MODULES_INIT    |
                 |         .                                                         | 
                 |         . Function Prototypes                                     | 
                 |         ...................                                       | 
                 |                                                                   | 
    .....        |                                                                   | 
    api.h -------+feeds                                                              | 
    .....                                                                            | 
                                                                                     | 
                       feeds                                                         | 
        ..............  |  ...................                                       | 
        protocol_cfg.c <-- . protocol_cfg.h:                                         | 
        ..............     ...................                                       | 
                 ^         . #define DBGP_PROTOCOLS   DBGX_USR_PROTOCOLS     <-------+ 
                 |         .                                                         | 
    ..........   |         .                                                         | 
    protocol.h --+feeds    . #define DBGP_PROTOCOL_TEXT     1     ------------------>+ 
    ..........             . #define DBGP_PROTOCOL_TCH_DER  2     ------------------>+ 
                           . #define DBGP_PROTOCOL_NONE     0     ------------------>+ 
                           .                                                         | 
                           . Function Prototypes                                     | 
                           ...................                                       | 
                                                                                     | 
                       feeds                                                         | 
       ...............  |  ...................                                       | 
       interface_cfg.c <-- . interface_cfg.h:                                        | 
       ...............     ...................                                       | 
                 ^         .                                                         | 
    ...........  |         .                                                         | 
    interface.h -+feeds    . #define DBGI_INTERFACE_DBGX_UART  1  ------------------>+ 
    ...........            . #define DBGI_INTERFACE_DBUS       2  ------------------>+ 
                           . #define DBGI_INTERFACE_RAM        3  ------------------>+ 
                           . #define DBGI_SIMPLE_DEBUG         4  ------------------>+ 
                           . #define DBGI_INTERFACE_NONE       0  ------------------>+
                           .
                           . Function Prototypes
                           ...................
    
<.pn>
#### [A3]   Text Terminal Protocol Output Contents
    
    Example: TTI000302:MyString96
             | ||   |  |
             | ||   |  +- String, Put out by: DBGX_logStr_INFO_SCN_APP2("MyString96");
             | ||   |
             | ||   +---- Scenario Bit Index:  02 (hex)
             | ||
             | |+-------- Scenario Word Index: 0003 (hex)
             | |
             | +--------- Level:               Info
             |
             +----------- Application Module:  Text application
    
             (Please consider, that we have to avoid too much formatting info, because
             of limited memory and limited debug interface channel capacity.)
    
    General: mmlwwwwbb:p
             | ||   |  |
             | ||   |  +- User String, Put out by DBGX_log ... or by DBGX_sendData
             | ||   |
             | ||   +---- Scenario Bit Index  (hex):   01 ...   20, | identifies
             | ||         No Scenario:                 00           | the scenario flag
             | ||
             | |+-------- Scenario Word Index (hex): 0001 ... FFFF, | 
             | |          No Scenario:               0000
             | |
             | +--------- Level: E - Error
             |                   W - Warning
             |                   I - Info
             |                   0 - No level
             |
             +----------- Application Module:
    
                                 Debug   Module Name                                 Module
                                 Output                                              ID 
                                 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                                 TC      Touch                  and PC-GUI           0, 5
                                 TE      Temperature regulation and PC-GUI           1
                                 ML      Motor control          and PC-GUI           2
                                 M3      Module 3                                    3
                                 SM      System master                               4
                                 M6      Module 6                                    6
                                 M7      Module 7                                    7
                                 ...
                                 M9      Module 9                                    9
                                 MA      Module A                                    A
                                 ...
                                 MC      Module C                                    C
                                 MD      Module D                                    D
                                 TT      Text application                            E
                                 FU      Reserved for future unlimited extensions    F
                                         with module values in pay load
    
    If non-filtered debug output is used, then we will not have level and scenario info:
    
    General: mm:p
             |  |
             |  +- User String, Put out by DBGX_log_..._NonFiltered
             +---- Application Module

