# Framework NG: Debug Data: What-To-Do Requirements
    
For the debug data the essential requirements and abstraction requirements,  
derived from the dealing with debug data in the current projects, from new ideas  
and from hardly predictable debug data features, are written down here as  
what-to-do, not yet as how-to-do requirements.
    
                Essential              API
                User Req.              Abstraction Req.
                        .             .
                         .           .
                          .         .
                           .       .
                           .       .
                           V       V
                       +---------------+
                       | Framework NG: |
                       | Debug Data    |
                       +---------------+
                               ^
                               .
                               .
                           Hardware
                       Abstraction Req.
    
            Fig. 1: Requirements for the Framework
    
<.pn>
## Listed What-To-Do Requirements
    
The following main overview is assumed for the what-to-do requirements.
    
    +----------------------------------------------------------------------------------+
    | Application Components Dealing with Debug Data                                   |
    |                                                                                  |
    | +------+     +-------+            +-------------+      +---------+    +-----+    |
    | | Any  |     | Touch |            | Temperature |      | Motor   |    | Any |    |
    | | via  |     +-------+            | Regulation  |      | Control |    +-----+    |
    | | Text |1*)   |                   +-------------+      +---------+     |         |
    | +------+      |                    |                    |              |         |
    |  | Text       | Touch Derived      | Frames for Touch   | Data         | May Be  |
    |  | Terminal   | Application        | GUI and Derived    | for          | Future, |
    |  | Protocol   | Protocol Data      | GUIs               | FreeMASTER   | not yet |
    |  | Data       |                    |                    |              | Known   |
    |  |            |                    |                    |              | Data    |
    +--|------------|--------------------|--------------------|--------------|---------+
       |            |                    |                    |              |
       |<<use>>     |<<use>>             |<<use>>             |<<use>>       |<<use>>
       |            +--------+    +------+                    |              |
       |                     |    |                           |              |
       +----------------+    |    |    +----------------------+              |
                        |    |    |    |                                     |
                        |    |    |    |     +-------------------------------+
                    API |    |    |    |     |
                        V    V    V    V     V
                    +----------------------------+
                    |    Framework or Other      |
                    |    Components Supporting   |
                    |    Debug Data Frame and    |
                    |    Protocol Handling       |
                    +----------------------------+
                                   |
                                   |<<use>>
                                   V
       +----------------------------------------------------------------+
       |                                                                |
       |    +-------+    +------+    +------+    +----------------+     |
       |    | Debug |    | DBus |    | JTAG,|    | May Be Future, |     |
       |    | UART  |    +------+    | ICE  |    | not yet Known  |     |
       |    +-------+                | (via |    | Interfaces     |     |
       |                             | ITM) |    +----------------+     |
       |                             +------+                           |
       | Debug Interfaces on Hardware for the PC                        |
       +----------------------------------------------------------------+
                                   | 
                                   |<<h/w connection>> 
    +-------------------------------------------------------------------------+
    | PC   +----------+ +-------+           +------------+ +----------------+ |
    |      | Text     | | Touch |           | FreeMASTER | | May Be Future, | |
    |      | Terminal | | GUI   |           +------------+ | not yet Known  | |
    |      +----------+ +-------+                          | Programs       | |
    |                                                      +----------------+ |
    |                   +-----------+                                         |
    |                   | Derived   |                                         |
    |                   | Programs  |                                         |
    |                   | from      |                                         |
    |                   | Touch GUI |                                         |
    |                   +-----------+                                         |
    +-------------------------------------------------------------------------+
    
    1*) Any via Text means: Any application can use text application functions
                            for variable values and infos as ASCII characters
                            (similar to printf, assert, getchar and the like)
    
             Fig. 2: Applications, Protocols and Interfaces of Debug Data 
    
### Essential User Requirements
    
Essential user requirements are the requirements from the current point of  
view and knowledge of the user.
    
For may be future requirements and for having as less features as possible  
at the API function calls see the abstraction requirements, that complete  
the what-to-do requirements in addition to the essential user requirements.
    
<.pn>
#### UR190307a: Applications, protocols and interfaces of debug data
    
A big part of essential requirements about applications, protocols and interfaces  
of debug data is contained in this table (e.g. 01-50-80 and so forth).
    
Those are the user-defineable constellations of applications, protocols and  
interfaces. (Further constellations are shown in the next item - item of  
abstraction requirements.)
    
    Reading of table: Application x sends / receives via protocol y on interface z.
    
    +--------------------------------------------------------------------------------+
    |           Application                     |                Interface           |
    |--------------------------------------------------------------------------------|
    | 01   . 02    . 03     . 04           .    |       . 80    . 81   . 82          |
    | Any  . Touch . Tempe- . Motor        .    |       . Debug . DBus . JTAG,       |
    | via  .       . rature . Con-         .    |       . UART  .      . ICE         |
    | Text .       . Regu-  . trol         .    |       .       .      . (via        |
    |      .       . lation .              .    |       .       .      . ITM)        |
    |---------------------------------------------------.----------------------------|
    |      .       .        .              .    |       .       .      .             |
    |      .       .        .              . 50         .       .      .             |
    | RWF  .       .        . RWF          . Text       . RWF   .      . RWF         |
    |      .       .        . 4*), 5*)     . Terminal   .       .      .             |
    |      .       .        .              . Protocol   .       .      .             |
    |      .       .        .              . Data       .       .      .             |
    |...........................................|....................................|
    |      .       .        .              . 51         .       .      .             |
    |      . RWF   . RWF    .              . Touch Deri-. RWF   . RWF  .             |
    |      .       .        .              . ved Appli- .       .      .             |
    |      .       .        .              . cation Pro-.       .      .             |
    |      .       .        .              . tocol Data . 2*)   . 3*)  .             |
    |...........................................|....................................|
    |      .       .        .              . 52         .       .      .             |
    |      .       .        . RW3          . Frames     . RW3   .      .             |
    |      .       .        . 5*)          . for        .       .      .             |
    |      .       .        .              . FreeMASTER .       .      .             |
    |      .       .        .              .            .       .      .             |
    +---------------------------------------+----------+-----------------------------+
                                            | Protocol |
                                            +----------+
    
    R    -   reading   from interface,   needed for sure
     W   -   writing   to   interface,   needed for sure
      F  -   framework components    supporting debug data frames and protocol handling
      3  -   3rd party components    supporting debug data frames and protocol handling
    
    Only  one         from some protocols  for one interface is  configurable, 1*).
    Zero, one or more from some interfaces                   are configurable, 1*).
    
    1*) For using at the same time. Especially needed for hardware tests e.g. of touch.
    2*) See [A2] for the frames.
    3*) See [A3] for the frames.
    4*) No text can be processed by FreeMASTER PC GUI
    5*) Either unframed text (text terminal protocol data) or framed data for the 
        motor control application (not both together at the same time)
    
            Table 1: Coincidence of Application, Protocol and Interface of Debug Data,
                     Taking Care of the Essential Requirements
    
#### UR190220a: The text terminal protocol data on the debug UART has to be  useable by text standard terminal tools (e.g. PUTTY or Teraterm)
- Reasons:
  * Reuseable powerful costless text standard terminals are available.
  * Many users are used to deal with such standard tools.
    
#### UR190218b: For the touch derived application protocol data on the debug UART:  pre-defined debug UART frame structure
- Reasons:
  * The powerful PC GUIs are based on the pre-defined debug frame  
    structure and developed, tested and reuseable (e.g. touch GUI).
  * For many users "TOUCH GUI" and similar PC programs are  
    very helpful and cannot be avoided.
  * The frame structure is a base of many components (e.g. PC GUIs,  
    touch library, other software components).
- The frame structure is already kind of standard in the BSH.
- UART frame strcture has been pre-defined for:
  * Internal usage in the touch library.
  * Compatibility with the tools (e.g. labview tool "Touch GUI").
    
<.pn>
#### UR190218c: For the framed test data on DBus:  pre-defined DBus frame structure
- Reasons:
  * The powerful PC GUIs are based on the pre-defined DBus frame  
    structure and developed, tested and reuseable (e.g. touch GUI).
  * For many users "TOUCH GUI" and similar PC programs are  
    very helpful and cannot be avoided.
  * The frame structure is a base of many components (e.g. PC GUIs,  
    PC DBus tools, other software components).
- The frame structure is already kind of standard in the BSH.
    
#### UR200309a: For the DBus as interface for output data (besides debug UART):  text protocol data or touch derived application protocol data as  payload
- Reasons:
  * The hadware efforts of debug UART (especially for the CI-test  
    equipment: e.g. Trump-Tower, Putin-Tower, S{oe}der-Tower) are too  
    big in some cases.
  * The DBus can be used as well in case of not having too many debug  
    data.
- The DBus protocol stack can be used as is also for debug data,  
  together with the debug API functions and debug component supported  
  FIFO buffering of the output data.
    
#### UR190226d: Filtering according to the sender of data for the outputs  from the controller
- Reason:
  * In some debug contexts it is disturbing to have too many data  
    on the debug interface.
- The data of data senders has to be enableable or disableable at a  
  central element of controller software to avoid disturbing outputs  
  to the PC.
- Senders of data: e.g. touch, temperature regulation, ...
    
#### UR190301b: Filtering of output data according to user defineable flags,  enableable by the user, for the outputs from the controller
- Reasons:
  * Such filtering has shown as very helpfully in the applications  
    of exiisting projects for analyzing special contexts in the  
    software.
  * One or more scenarioes and / or levels are selecteable by flags.  
    => Data of those scenarioes and / or levels appear on the debug  
       interface.
- By  user configurable flags the debug outputs of a scenario  
  and or level can be switched on or off (scenario flags, level  
  flags).
- Especially verbose leveling for error, warning, info can be done  
  by filtering using level flags.
    
#### UR190307c: FIFO buffering for the inputs and outputs of the controller  to avoid blocking of the software during the transmission
- Reason:
  * During the transmission of debug data the sofware must not  
    be blocked for keeping the liveliness of the whole software  
    system.
    
#### UR190313a: Processing from Start of Output API Function until the End  of Transmitted Data Must Be Able for Reentrance
- Reasons:
  * Ouput API function can be called from different levels of  
    priority with interruption of code sequences by events  
    (e.g. from ISR and from main loop).
  * Output API function can be called from same level of  
    priority with interruption of code sequences by scheduler  
    (e.g. using time slicing in an RTOS).
- Reentrance means new start of the output API before all data  
  from the start before are sent.
- This protection here means: Context of output data on the  
  interface must not get insertions or desructions because of  
  reentrance.
- At the same time blocking code sequences until the end of  
  transmission are not allowed (UR190307c).
    
<.pn>
### Abstraction Requirements
    
For may be future requirements and for having as less features as possible  
at the API function calls the abstraction requirements complete the what-to-do  
requirements in addition to the essential user requirements.
    
#### AR190307a: Applications, protocols and interfaces of debug data,  completed by the abstraction requirements
    
A big part of essential and abstraction requirements about applications,   
protocols and interfaces of debug data is contained in this table   
(e.g. 05-51-80 and so forth).
    
Those are the user-defineable constellations of applications, protocols and  
interfaces.
    
    Reading of table: Application x sends / receives via protocol y on interface z.
    
    +--------------------------------------------------------------------------------+
    |           Application                     |                Interface           |
    |--------------------------------------------------------------------------------|
    | 01   . 02    . 03     . 04    . 05   .    |       . 80    . 81   . 82    . 84  |
    | Any  . Touch . Tempe- . Motor . Any  .    |       . Debug . DBus . JTAG, . Any |
    | via  .       . rature . Con-  . Fu-  .    |       . UART  .      . ICE   . Fu- |
    | Text .       . Regu-  . trol  . tu-  .    |       .       .      . (via  . tu- |
    |      .       . lation .       . re   .    |       .       .      . ITM)  . re  |
    |---------------------------------------------------.----------------------------|
    |      .       .        .       .      .    |       .       .      .       .     |
    |      .       .        .       .      . 50         .       .      .       .     |
    | RWF  . rwF   . rwF    . RWF   . rwF  . Text       . RWF   . rwF  . RWF   . rwF |
    |      .       .        . 4*)   .      . Terminal   .       .      .       .     |
    |      .       .        . 5*)   .      . Protocol   .       .      .       .     |
    |      .       .        .       .      . Data       .       .      .       .     |
    |...........................................|....................................|
    |      .       .        .       .      . 51         .       .      .       .     |
    | rwF  . RWF   . RWF    . rwF   . rwF  . Touch Deri-. RWF   . RWF  . rwF   . rwF |
    |      .       .        .       .      . ved Appli- .       .      .       .     |
    |      .       .        .       .      . cation Pro-.       .      .       .     |
    |      .       .        .       .      . tocol Data . 2*)   . 3*)  .       .     |
    |...........................................|....................................|
    |      .       .        .       .      . 52         .       .      .       .     |
    | rw3  . rw3   . rw3    . RW3   . rw3  . Frames     . RW3   . ---  . rw3   . rw3 |
    |      .       .        . 5*)   .      . for        .       .      .       .     |
    |      .       .        .       .      . FreeMASTER .       .      .       .     |
    |...........................................|....................................|
    |      .       .        .       .      . 53         .       .      .       .     |
    | rw?  . rw?   . rw?    . rw?   . rw?  . Any        . rw?   . rw?  . rw?   . rw? |
    |      .       .        .       .      . Future     .       .      .       .     |
    |      .       .        .       .      .    |       .       .      .       .     |
    +---------------------------------------+----------+-----------------------------+
                                            | Protocol |
                                            +----------+
    
    R    -   reading   from interface,   needed for sure
    r    -   reading   from interface,   may be needed in future
     W   -   writing   to   interface,   needed for sure
     w   -   writing   to   interface,   may be needed in future
      F  -   framework components    supporting debug data frames and protocol handling
      3  -   3rd party components    supporting debug data frames and protocol handling
      ?  -   not yet specified comp. supporting debug data frames and protocol handling
    ---  -   not applied
    
    Only  one         from some protocols  for one interface is  configurable, 1*).
    Zero, one or more from some interfaces                   are configurable, 1*).
    
    1*) For using at the same time. Especially needed for hardware tests, e.g. of touch.
    2*) See [A2] for the debug UART frames.
    3*) See [A3] for the DBus       frames.
    4*) No text can be processed by FreeMASTER PC GUI
    5*) Either unframed text (text terminal protocol data) or framed data for the motor
        control application (not both together at the same time)
    
            Table 2: Coincidence of Application, Protocol and Interface of Debug Data,
                     Taking Care of the Essential and Abstraction Requirements
    
#### AR190226a: The API function calling locations should deal with as less  features as possible
- Reason:
  * Changing one feature (e.g. the debug data transmission interface)  
    should not have to be needed at each location of calling of the  
    API function.
- Each feature not be seen at the calling locations have to be  
  configurable by a central element of the controller software.  
- Especially not defined at the calling locations:
  * Debug interfaces (defined by centralized configuration).
  * Protocol for each debug interface.
    
<.pn>
#### AR190224a: The data outputs of called text application functions (e.g. like  printf) will have to be converted to texts inside frames, if touch  derived application protocol data are configured as protocol
- Reasons:
  * It is fine to unify software structures in the framework (no  
    separated handling of unframed text and framed binary data in  
    the main structure).
  * Especially critical text outputs from assert would be available  
    in frames. Could be needed in the future, especially for error  
    outputs.
- Helpful idea from MiKo as a key for abstraction here.
    
#### AR190301a: The data outputs of called touch derived application functions (for  frames for touch GUI and derived GUIs) will have to be converted to  unframed text with hexadecimal numerals, if text terminal protocol  data is configured as protocol
- Reasons:
  * Symmetrie to AR190224a.
  * Could be needed in future.
- Converted frames are texts, e.g.:
  04 45 11 22 AA BB 48 A6
- Helpful idea from MiKo as a key for abstraction here.
    
#### AR190220a: The text terminal protocol data on may be future debug data interfaces  have to be useable by text standard terminal tools
- Reason:
  * Reuseable powerful costless text standard terminals are available.
- Many users could be used to deal with such standard tools.
    
#### AR190218b: For the may be future kinds of frames (protocols) the frame  structure has to be pre-defined
- Reasons:
  * The future PC GUIs will be based on the pre-defined frame  
    structure.
  * The frame structure will be a base of many controller  
    software components (e.g. PC GUIs, libraries, other software  
    components).
- The frame structure should get kind of standard in the BSH.
    
#### AR190301b: Filtering of output according to user defineable debug scenario  flags: number of filter scenario flags defineable by the  user
- Reasons:
  * Some application projects need less, other need more debug  
    scenarios.
  * Scaling of the maximum number of flags according to the  
    microcontroller memory ressources and to the current needs can  
    be done.
    
<.pn>
## Background Especially for Touch Derived Application Protocol Data and for  Text Terminal Protocol Data
    
### Text Terminal Protocol Data and Touch Derived Application Protocol Data in  a Common View
    
                                                    +------------------+
                                                    | Debug PC         |
          |                  |           +---+  USB |                  |
          O------------------+-----<->---|UDA|------|                  |
          |                  |           +---+      |   Text Terminal  |
      DBus|                  |                      |   Programs       |
          |                  |                      |                  |
          |                  |                      |                  |
          |             Debug|         +-----+      |   Touch GUI      |
          |             UART O------->-|UART/|  USB |   (if microcon-  |
          |                  |         |USB  |------|   troller with   |
          |             +----+-------<-|     |      |   touch) or      |
          |             |    |         +-----+      |   similar tools  |
          |             |    |                      +------------------+
          |             |    |
          |             |    |             ****************************
          |             |    |             * Debug UART Data, See also [A1]:
          |             |    |             * - - - - - - - - - - - - - - - -
    +---------------------------+          * Framed data on TxD and RxD
    |    Dat     |     RxD  TxD |          * - Touch configuration frames from PC
    |---------------------------|          * - Basic touch sample frames to PC
    | Microcontroller           |          * - Extended touch sample frames to PC
    | e.g. with Touch           |          * - Other frames than touch data
    |                           |          * - For touch GUI and similar tools
    +---------------------------+          * Unframed text data on TxD and RxD
                                           * - Text commands from PC
                                           * - Text responses to PC
                                           * - Text log data to PC
                                           * - Text control data from PC
                                           * - For text terminal programs
                                           *****************************
                                           * Touch debug data also via DBus
                                           *****************************
    
      Fig. 3: Microcontroller and Debug PC with UART and DBus as Debug Interfaces
              (Optional ITM Interface for Text Terminal Protocol Data Is not
              Shown Here)
    
    For the future still unknown interfaces and protocols could be
    introduced.
    
    -------------------------------------------------------------------------------
     TEXT TERMINAL PROTOCOL DATA            TOUCH DERIVED APPLICATION PROTOCOL DATA
       Text terminal usage is                    Additionally there are special
       comfortable for the user                  framework use cases, based on
       and must be implemented                   the general hardware variants
       on the debug UART.                        with framed touch data on the
                                                 debug UART. An implementation
                                                 for these will have to be done
                                                 as well. The framed test data
                                                 are designed as binary data
                                                 frames, compatible with the
                                                 touch library and with the
                                                 touch PC tool chain.
                          \                       /
                           \                     /
    
                       FRAMED AND UNFRAMED DEBUG DATA
               Merged User Requirements can be derived from
               the special framework use cases:
               - With miscellanous protocols of debug UART data
                 (text terminal protocol data and touch derived
                 application protocol data).
               - As a base of a contradiction-free implementation.
    ------------------------------------------------------------------------------
    
            Demanded on the TxD and RxD line of the microcontroller, [A1]:
    
                                                          +------------------------+
                                                          | Touch Derived Applica- |
                                                          |   tion Protocol Data   |
                     +---------------------+              |   +----------------+   |
         EITHER      | Text Terminal       |      OR      |   | Optionally:    |   |
                     | Protocol Data       |              |   | Sub-Framed     |   |
                     |                     |              |   | Functional     |   |
                     +---------------------+              |   |    Data        |   |
                                                          |   |  (Events), 1*) |   |
                                                          |   +----------------+   |
                                                          |                        |
                                                          +------------------------+
    
    1*) Only on the TxD line of the microcontroller.
    
<.pn>
### Text Terminal Protocol Data in a Separate View
    
                                                                +------------------+
                                                                | Debug PC         |
                                                                |                  |
                                                                |                  |
                                                                |   Text Terminal  |
                                                                |   Program        |
                                                                |                  |
                                    Debug          +-----+      |                  |
                                    UART +------->-|UART/|  USB |                  |
                                         |         |USB  |------|                  |
                                    +----+-------<-|     |      |                  |
                                    |    |         +-----+      |                  |
                                    |    |                      |                  |
                                    |    |                      +------------------+
                                    |    |
                                    |    |          ****************************
    +---------------------------------------+       * Debug UART data:
    |    Dat     |                 RxD  TxD |       *   Text terminal protocol data
    |---------------------------------------|       *   - Text commands from PC to
    | Microcontroller                       |       *     controller
    |                                       |       *   - Text Responses from
    |                                       |       *     controller to PC
    +---------------------------------------+       *   - Text log data to PC
                                                    *   - Text control data from PC
    
      Fig. 4: Microcontroller and Debug PC with UART as Debug Interface for
              Text Terminal Protocol Data
              (Optional ITM Interface for Text Terminal Protocol Data is not Shown 
              Here)
    
**Structure of Text Terminal Protocol Data on the Debug UART**
    
The text data are not filled into frames.  
The structure is given by:

- CR characters at the end of commands from the PC to the controller
- CR characters inside and at the end of commend responses or log data from  
  the controller to the PC
- Text formatting by spaces
- Explaining strings and convinent value formating for human readability
    
    
### Touch Derived Application Protocol Data in a Separate View,  No Functional Events on the Debug UART
    
    Example Project: EOX6020_21
    
    +------------------------------------+
    | System Master                      |
    |                                    |
    +------------------------------------+                   +------------------+
          |               |                                  | Debug PC         |
          |               |                       +---+  USB |                  |
          O---------------|-----------------<->---|UDA|------|                  |
          |               |                       +---+      |                  |
      DBus|          Event|                                  |                  |
          |          UART |                                  |                  |
          |               |      Debug          +-----+      |                  |
          |               |      UART +------->-|UART/|  USB |   Touch GUI      |
          |               |           |         |USB  |------|     or similar   |
          |               |      +----+-------<-|     |      |           Tools  |
          |               |      |    |         +-----+      |                  |
          |               |      |    |                      |                  |
          |               |      |    |                      +------------------+
          |               |      |    |
          |               |      |    |         ****************************
    +------------------------------------+      * Debug UART Data, See also [A1]:
    |    Dat     |     TxD2  |  RxD  TxD |      * - - - - - - - - - - - - - - - -
    |------------------------------------|      * Framed data on TxD and RxD
    | Microcontroller                    |      * - Touch configuration frames from PC
    | with Touch                         |      * - Basic touch sample frames to PC
    |                                    |      * - Other frames than touch data
    +------------------------------------+      *****************************
                                                * Touch debug data also via DBus
                                                *****************************
    
      Fig. 5: Microcontroller and Debug PC with separated UART as Debug Interface
              for Touch Derived Application Protocol Data
    
**Structure and Contents of Touch Derived Application Protocol Data on the Debug  
UART (No Functional Events on the Debug UART)**
    
See [A2].
    
<.pn>
### Touch Derived Application Protocol Data in a Separate View,  Shared UART for Functional Events and Debug
    
    Example Project: PSOC5 Touch Development Board
    
    +------------------------------+
    | System Master                |
    |                              |
    +------------------------------+                   +------------------+
          |                     |                      | Debug PC         |
          |                     |           +---+  USB |                  |
          O---------------------+-----<->---|UDA|------|                  |
          |                     |           +---+      |                  |
      DBus|                Event|                      |                  |
          |                UART |                      |                  |
          |                and  |                      |                  |
          |                Debug|         +-----+      |   Touch GUI      |
          |                UART O------->-|UART/|  USB |     or similar   |
          |                     |         |USB  |------|           Tools  |
          |                +----+-------<-|     |      |                  |
          |                |    |         +-----+      |                  |
          |                |    |                      +------------------+
          |                |    |
          |                |    |
          |                |    |              ****************************
    +------------------------------+           * Debug UART Data, See also [A1]:
    |    Dat     |        RxD  TxD |           * - - - - - - - - - - - - - - - -
    |------------------------------|           * Framed data on TxD and RxD
    | Microcontroller              |           * - Touch configuration frames from PC
    | with Touch                   |           * - Extended touch sample frames to PC
    |                              |           * - Other frames than touch data
    +------------------------------+           *****************************
                                               * Touch debug data also via DBus
                                               *****************************
    
      Fig. 6: Microcontroller and Debug PC with Shared Event and Debug UART as 
              Debug Interface for Touch Derived Application Protocol Data
    
**Structure and Contents of Touch Derived Application Protocol Data on the Debug  
UART (Shared UART for Functional Events and Debug)**
    
See [A2]. There are not only usual touch debug UART frames.  
Additionally we have touch event frames with the same structure and with  
the special ID 0x45.
    
<.pn>
## Formal Points
    
### Abbreviations
    
    MiKo: Michal Kopcik
    SvJo: Sven Joppien
    
### History of Versions
    
    Data        Author   Version   Contents
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    28.01.2019  SvJo     190128    NEW: First contents for merging the requirements
    31.01.2019  SvJo     190130    NEW: Chapter about DBus for debug data
    01.02.2019  SvJo     190201    NEW: List of numerated reqirements
                                   NEW: List of ASCII text commands for controlling
                                        log data
                                   MOD: Avoiding some redundancies of contents
                                   NEW: Some special additions
    04.02.2019  SvJo     190204    MOD: List of Enumerated User Requirements brought
                                        to the foreground
                                   MOD: Better filling in of signal names in the 
                                        images
                                   NEW: In [A1] some statements of CPM point of view
    06.02.2019  SvJo     190206    MOD: Explanation about THDO
    07.02.2019  SvJo     190207    MOD: Dynamic allowing or disabling of framed data
                                        not yet to be implemented, see [A3] 
    11.02.2019  SvJo     190211    NEW: Requirement R190211a
    18.02.2019  SvJo     190218    NEW: Also RxD of controller for touch debug data
                                        (because of new touch library with many
                                        inclusive touch commands)
                                   MOD: Text commands for configuring of touch data
                                        removed (because of new touch library with
                                        inclusive framed commands for that)
                                   MOD: DBus messages for configuring of touch data
                                        removed (because new touch library with many
                                        inclusive DBus messages for that)
                                   MOD: Example terminal commands removed
                                   MOD: Commands for choosing the kind of data
                                        removed
                                   MOD: Interface extension of debug module removed
                                   NEW: R190216a, R190218a, R190218b
                                   NEW: Underlines of the figures (for better 
                                        referencing)
                                   MOD: Figures are more specialized for the exact
                                        cases of usage
                                   NEW: Head lines of some items mor specialized to
                                        avoid misunderstandings by mix-ups
    18.02.2019  SvJo     190218a   MOD: More unified expressions to avoid 
                                        misunderstanding
                                   NEW: Some detailled information
    19.02.2019  SvJo     190219    MOD: On RxD of the debug UART only the commands to
                                        set and read variables
    20.02.2019  SvJo     190220    NEW: R190220a
                                   MOD: Target element in the UART frames could be 
                                        not necessary
    22.02.2019  SvJo     190222    NEW: Reasons of the requirements
    24.02.2019  SvJo     190224a   NEW: R190224a
    26.02.2019  SvJo     190226    NEW/MOD: R190226*, according to the meeting with
                                        MiKo
                                   NEW: Item: Debug Data Interfaces and Debug Data
                                        Protocols: Combinations
    27.02.2019  SvJo     190227    MOD: Distinguishing between essential user req.
                                        and abstration req.
                                   NEW: Abstraction requirements
    01.03.2019  SvJo     190301    MOD: Conditions for mapping text and binary data
                                        into frames
                                   NEW: Also converting binary data to text
                                   MOD: Filtering (now by sender and by category)
                                   NEW: Category of output data for selecting by the
                                        user
    06.03.2019  SvJo     190306    MOD: Better listings of the reqirements in the
                                        items "Essential Reqirements" and 
                                        "... Abstraction Requirements"
                                   MOD: Better wording and understandeability in
                                        the requirements
    07.03.2019  SvJo     190307    MOD: More overview in the listed requirements
                                        (Figures / Tables)
                                   MOD: DBus debug commands
                                   MOD: Background chapter updated and shortened
                                   MOD: Subject line of document shortened
    08.03.2019  SvJo     190308    MOD: Better understandeable wording at some
                                        points, further explanations 
    11.03.2019  SvJo     190311    MOD: Results of review of V190308
                                        (MiKo, SvJo)
                                        - Framework componets of hardware acces not
                                          explicitely mentioned here
                                        - Additional cases in UR190307a for
                                          FreeMASTER
                                        - No text can be processed by GUI of
                                          FreeMASTER
                                        - Space as separation character for hexa-
                                          decimal numerals as text outputs not
                                          specified here
                                        - Link fixed in [A3]
                                        - Separation to touch library specified in [A3]
    12.03.2019  SvJo     190312    MOD: Results of review of V190311
                                        (MiKo, SvJo)
                                        - Either unframed text or framed data for the
                                          motor control application (not both together
                                          at the same time)
                                        - The protocol stack of motor control data is
                                          handled as 3rd-party software taken over
                                          without big modifications
    13.03.2019  SvJo     190313    NEW: - UR190313a: Reentrance Protection
    05.04.2019  SvJo     190405    NEW: - This comment here only: 
                                          V190313 has been reviewed in March 2019: OK
                                          (MiKo, SvJo)
    Versioning is going on in Git, since checked in there
    
<.pn>
## Annotations
    
### [A1] TxD and RxD of Controller are Needed for the Touch Library

**Is reading / writing of variables via touch debug UART for configuring the touch  
debug data stream via Debug UART necessary?**

- Question to the former (partly still current) users of framed touch data streams

`

                          H/W                          S/W
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Cooking               Oskar Weinzierl:  no         Sven Joppien:      no
                          Dominik Stadler:  no
    
    Washing (Clothing)    Manfred Bergner:  no         Christian Lippert: no
                          Rudi Feldmeier:   no
                          Maxim Munt:       no
    
    Cooling               Dirk Rosenwald:   no         Thomas Arens:      See H/W
    
    Coffee                Dirk Rosenwald:   no         Thorsten Piehler:  no
                          Dominik Stadler:  no
    
    Dishwasher            Dirk Rosenwald:   no         Andreas B{ae}uml:     no
    
    Touch                               Carsten Zei: no
    
Reading / writing of variables via touch debug UART would be a comfort  
function, but it is no must-be.  
So the special reading / writing frames for variables could be avoided.
    
We need from that point of view for the PC program only:

- Frames of CPM debug data stream on the uC Tx line
    
    

**Is reading / writing of variables via touch debug UART for configuring the CPM  
debug data stream via Debug UART necessary?**

- Question to the former (partly still current) users of framed temperature  
  regulation data streams

`

                                                       S/W
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Coffee                                             Michael Obermaier:  no
                                                       J{ue}rgen D{oe}ffinger:   no
    
    Cooling                                            Ralf Dauberschmidt: yes 1*)
    
    
    1*) In the QP framework (company Quantum Libs), in Gingen
    
We need from that point of view for the PC program only:

- Frames of CPM debug data stream on the uC Tx line
    
    

**Is reading / writing of variables via touch debug UART for configuring the touch  
debug data stream and requesting of touch debug data of one sample via Debug UART  
necessary?**

- Question to the touch library developers
    
Yes, because it is helpful to handle these functions via debug UART directly.  
(Peter Winter, Arthur Stanov). We need from hat point of view:

- Frames of CPM debug data stream on the uC Tx line
- Frames of CPM debug data stream on the uC Rx line
    
<.pn>
### [A2] Structure and Contents of Frames of Touch Derived Application Protocol  Data on the Debug UART
    
Elements of the Debug UART frames for program Touch GUI:

    Number of Bytes         Element
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    1                       LEN
    1                       ID
    LEN-4                   PAYLOAD (user data; 16-bit sample values)
    2                       CRC (over LEN, ID, PAYLOAD)
    
Endianess is the same as on the target controller.
    
Special case: Extended touch sample frames:

- Contain sub-framed functional data (events)
- Are special debug frames with ID 0x45
    
See docu for further explanations (contents valid on 25.1.2019):  
[http://wiki.bshg.com/pages/viewpage.action?pageId=190088500](http://wiki.bshg.com/pages/viewpage.action?pageId=190088500)
    
See docu from Valeriy Yatsenko for further explanations (contents valid on  
25.1.2019):  
[http://wiki.bshg.com/display/operatingtechnologies/Generic+serial+touch+protocol](http://wiki.bshg.com/display/operatingtechnologies/Generic+serial+touch+protocol)
    
Touch library data on the debug UART:

- Debug UART protocol frames for writing of variables
- Debug UART protocol frames for reading of variables
- Debug UART protocol frames for requesting touch data of one sample
- Debug UART protocol frames for touch log data stream
    
The processing of UART protocol frames is inclusive in the touch library.
    
### [A3] Structure and Contents of Debug Frames on the DBus Respective Frames  Defined in the Touch Library and Defineable outside that Framework
    
**DBus Messages Implemented inside of the Touch Library**
    
See Docu (contents valid on 7.3.2019):  
[https://wiki.bsh-sdd.com/download/attachments/1018007071/TouchLib_DBUS_Messages.xlsx?api=v2](https://wiki.bsh-sdd.com/download/attachments/1018007071/TouchLib_DBUS_Messages.xlsx?api=v2)
    
For those messages anything will have to be done neither inside the debug  
part of framework nor in the application software.
    
**DBus Messages Implementeable outside of the Touch Library**
    
Outside of the touch library DBus debug messages can be defined as well.  
Applications for that are:

- DBus message for putting out the touch debug data stream from the controller
- DBus message for controlling the touch debug data stream to the controller
- A versatile useable DBus message for data from the controller
- A versatile useable DBus message for data to   the controller
    
The exact frame structure will be defined in the how-to-do requirements.


