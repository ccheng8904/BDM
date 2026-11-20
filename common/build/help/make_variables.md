#         List of make variables and definitions for application
---

This page is meant to describe variables of MCU Framework make process.

### Content

1. [Application build variables](#ApplicationBuildVariables)
2. [General configuration variables](#GeneralConfigurationVariables)
3. [Microcontroller configuration variables](#MicrocontrollerConfigurationVariables)
4. [Communication configuration variables](#CommunicationConfigurationVariables)
5. [FirmwareUpdate configuration variables](#FirmwareUpdateConfigurationVariables)
6. [Memory organization variables](#MemoryOrganizationVariables)
7. [Component management variables](#ComponentManagementVariables)
8. [Source code management](#SourceCodeManagement)
9. [Compilation make variables](#CompilationMakeVariables)
10. [Static analysis](#StaticAnalysis)
11. [Debug management](#DebugManagement)
12. [Library generation](#LibraryGeneration)
13. [Binary generation](#BinaryGeneration)
14. [PSoC specific make variables](#PSoCspecificMakeVariables)
15. [pp_flash target specific make variables](#pp_flashTargetSpecificMakeVariables)
16. [JLink target specific make variables](#JLinkTargetSpecificMakeVariables)
17. [doxy target specific make variables](#doxyTargetSpecificMakeVariables)
18. [Others make variables](#OthersMakeVariables)


**Symbols and parameters used in examples**

|**Symbol**|**Explanation**|
|---|---|
|<>|required parameter or variable|
|[ \]|optional parameter or variable|
|**Parameter name**|**Explanation**|
|component|Code package consisting of source files, header files and make file containing build information In examples below the component is "led_control"|
|project|One repository may contain multiple applications (projects) in app folder, e.g.: app/projectA and app/projectB. When the project is not specified, application is assumed to be directly in app folder (this is the most common case). In examples below the project is "test_project"|
|variant|One application contains of multiple variants, whether if it is build in variant (BootManager, romLoader, programmer, ...) or one or more application variants. In examples below the application variant is "test_variant"|
|source_file|Base name of source file without extension. In examples the source file is "led_blink"|
|c cpp asm s |file extensions of languages C, C++ and assembler|
|library_name|Base name of the library. In examples below the library name is "led_control_lib"|

**Make prog path variables**

|Variable name|Default value|Description|
|-|-|-|
|app_prog_path|*app/\[project\]/prog*|The variable containing the path to the application prog folder|
|| app/test_project/prog||
|out_prog_path|*out/\[project\]/&lt;variant&gt;/prog*|The variable containing the path to output prog folder for the current variant|
||out/test_project/test_variant/prog||
|ext_prog_path|ext|The variable containing the path to external components|
|sdk_prog_path|common/prog|The variable containing the path to the common framework prog folder - for internal use only|
|make_search_path|Combination of all sdk, app, cfg and ext paths|The variable for setting paths where the make searches for prerequisites|

---

[Back to content](#content)

## **Application build variables** <a name="ApplicationBuildVariables"></a>

app/[project]/build/common_config.mak<br>app/[project]/build/<variant_name>.mak

|Variable name|Example|Description|Version|
|-|-|-|-|
|build_type|build_type = DEVELOP|This variable is used to specify build type for application variant(s).<br><br>`build_type ?= DEVELOP` - default setting used mostly for development proposes.<br>If make variable `build_type` is set to `DEVELOP` then output binary file will contain FWU OTP memory area and there will be dummy FWID set for development purposes.<br><br>`build_type ?= RELEASE`<br>If make variable `build_type` is set to `RELEASE` then application variants are always completely rebuilt and both macros `DEBUG=0` and `NDEBUG` are defined.<br><br><span style="color: rgb(255,0,0);">Furthermore, to get release binary files, make target `release` must be executed.</span><br>As a result, folder `out/<app_variant>/bin/release` will be created. This folder will contain `<app_variant>.hex`, `<app_variant>.mot` and binary file `Application`. <br><br>**NOTE:** make variable `build_type` is automatically set to `RELEASE` when `release` target is used. The FWU OTP memory area is excluded from those binaries.<br><br>**NOTE:** Unlike in older Framework versions (up to v1.8.x), HWID compatibility check is no more performed after boot in FWU3. Instead, new Firmware Compatibility Check (FCC) is performed. Now, Programmer is no more entered when OTP area is empty.<br>For more details about FCC, please, refer to file [Firmware_Compatibility_Check](./../../prog/firmware_update/doc/Firmware_Compatibility_Check.md).|>= v1.3.0|
|project|project = MyProject|Variable for specifying current project whose variant will be built, mainly used in command line|>= v1.0.0
|variant|variant = MyVariant|Variable for specifying current variant which will be built. It is mainly used for building different variants using command line, e.g.: <br>"make variant=programmer". <br><span style="color: rgb(255,0,0);">This variable should not be changed in build.mak!</span>|>= v1.0.0|
|app_variants|app_variants = MyVariant|List of application variants. First variant in list is built when variant is not specified explicitly|>= v1.0.0|
|cc_build_path|cc_build_path ?= keilArm|Folder name of desired compiler.<br> Currently supported for ARM core platforms:<br>-   *keilArm*<br>-   *gccArm*<br>-   *armclang*<br>Currently supported for RX platforms:<br>-   *iarRx*<br>|>= v1.0.0|

[Back to content](#content)

## **General configuration variables** <a name="GeneralConfigurationVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|auto_dep|auto_dep = false|enables/disables implicit generation of dependency files|>= v1.0.0|
|auto_lint|auto_lint = false|if `false`. excludes lint target from default_targets|>= v1.4.0|
|feature_functional_safety|feature_functional_safety = ENABLED|If this variable is set true, functional safety library is included [Note 1](#Notes)|>= v1.0.0|
|scheduler|scheduler = schedule|Variable for selecting a scheduler|>= v1.5.0|
|SyStHostedMaster|SyStHostedMaster=SyStMasterNone|Variable for selecting system state for microcontroller on DBus, default is SystemMasterNone which means system state Slave|
|noOptionBytes|noOptionBytes = true <br> in common_config.mak| if the variable is present and defined then the build process disable patching of option bytes in the final hex/mot binary file|>= v1.19.0|

[Back to content](#content)

## **Microcontroller configuration variables** <a name="MicrocontrollerConfigurationVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|platform| platform = stm32g0| Variable for selecting of the platform (lowercase) | >= v1.0.0|
|derivative| derivative = STM32G071CB| Selected derivative of microcontroller | >= v1.0.0|
|core_clock| core_clock := 56| Desired core clock in MHz used for clock setup in basicMcXs.c file | >= v1.0.0|
|pclock / pclock1 | pclock := 8 | Desired peripheral clock in MHz used for clock setup in basicMcXs.c file | >= v1.0.0|
|ext_osc| ext_osc := 8|External oscillator clock frequency (use )| >= v1.0.0|

[Back to content](#content)

## **Communication configuration variables** <a name="CommunicationConfigurationVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|UDA| UDA ?= UDA-01|This variable holds name of UDA1 which will be used for flashing using make (see list of make targets)| >= v1.0.0|
|dbus_uart_channel| dbus_uart_channel = 0| Selected UART channel for DBus|<span style="color: rgb(165,173,186);"><= v1.3.x: UART_CHANNEL</span>|
|dbus_node_address| dbus_node_address = 1|Variable for selecting DBus node address|<span style="color: rgb(165,173,186);"><= v1.3.x: NODE_ADDRESS</span>|
|dbus_default_baudrate|dbus_default_baudrate = 96|Variable for selecting default baudrate of the DBus, in hecto units |<span style="color: rgb(165,173,186);"><= v1.3.x: DEFAULT_BAUDRATE</span>|
|dbus_tx_pin| dbus_tx_pin = A2| Variable for selecting DBus Tx pin of node|<span style="color: rgb(165,173,186);"><= v1.3.x: DBUS_TX_PIN</span>|
|dbus_rx_pin| dbus_rx_pin = A3| Variable for selecting DBus Rx pin of node|<span style="color: rgb(165,173,186);"><= v1.3.x: DBUS_RX_PIN</span>|
|dbus_break_pin| dbus_break_pin = A2|Selected DBus break pin of node (by default, for most platforms dbus_tx_pin value is used)|>= v1.4.0<br><span style="color: rgb(165,173,186);"><= v1.3.x: WAKEUP_PIN</span>|
|dbus_idle_pin| dbus_idle_pin = A3| Selected DBus idle pin of node (by default, for most platforms dbus_rx_pin value is used)|>= v1.4.0<br><span style="color: rgb(165,173,186);">== v1.3.x: DBUS_IDLE_PIN (STM32F4 only)</span>|
|dbus_receive_buffer_size|dbus_receive_buffer_size = 16|Sets size of DBus receive buffer, also used for the definition of DBus macro `__MESSAGE_INPUT_BUFFER_SIZE`|>= v1.4.0<br><span style="color: rgb(193,199,208);">>= v1.3.2</span>|
|dbus_transmit_buffer_size|dbus_transmit_buffer_size = 50|Sets size of DBus transmit buffer, also used for the definition of Dbus macro `DLL_TRANSMIT_BUFFER_DATA_LENGTH`|>= v1.4.0<br><span style="color: rgb(193,199,208);">>= v1.3.2</span>|
|TESTMSGSRV_SUBNODE_ADDRESS|TESTMSGSRV_SUBNODE_ADDRESS = 10|Variable for selecting DBus node address for Teng message server|>= v1.0.0|
|dbus_mapping|dbus_mapping = dbuscan|Selects whether DBusCAN chip or MCAL (MUART) or HAL (HUART) channel of microcontroller is used for DBus communication. Valid options are "dbuscan", "mcal" (default for the platform ra4) or "hal" (default for all other platforms). Also used for generating the definition DBM_<dbus_mapping>, e.g. DBM_HAL|>= v1.12.0|

[Back to content](#content)

## **FirmwareUpdate configuration variables** <a name="FirmwareUpdateConfigurationVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|FW_update_type| FW_update_type = FWU3| Variable for selecting firmware update type. Possible values are FWU1 or FWU3 [1]|>= v1.0.0|
|fwu_flex_partition_used| fwu_flex_partition_used = ENABLED|  Variable for selecting flexible partition table for memory mapping to use [Note 1](#Note1)|>= v1.0.0|

[Back to content](#content)

## **Memory organization variables** <a name="MemoryOrganizationVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|UNINIT_DATA_SIZE| UNINIT_DATA_SIZE = 40| Variable for specifying the size of the uninitialized data segment in RAM (in hexadecimal). This segment will not be initialized / cleared in case of a soft-reset. Additionally, it will be excluded from FS StartUp (OneTime) RAM check, as it is "destructive" (content is overwritten with 0xFF values).<br>**StartUp** **HW Reset** (OneTime) RAM check - **checked**<br>**StartUp** **SW Reset** (OneTime) RAM check - **excluded**<br>**Continuous** (RunTime) RAM check - **checked**<br><span style="color: rgb(255,102,0);">Make sure that the size is a multiple of 0x40.</span>|>= v1.4.1|
|RAMCONT_EXCLUDED_DATA_SIZE| RAMCONT_EXCLUDED_DATA_SIZE = 40|Variable for specifying the size of the excluded data segment from FS Continuous (RunTime) RAM check in RAM (in hexadecimal). This segment is typically used to store the data used for DMA transfers and therefore it is excluded from Continuous RAM check, as it can cause FuncSafe <span style="color: rgb(255,102,0);">RAM check fail. </span><br>**StartUp** (OneTime) RAM check - **checked**<br>**Continuous** (RunTime) RAM check - **excluded**<br><span style="color: rgb(255,102,0);">Make sure that the size is a multiple of 0x40.</span>|>= v1.5.0|

[Back to content](#content)

## **Component management variables** <a name="ComponentManagementVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|common_components| common_components += debug| Adds component from framework. When adding new component, run "make init" to initialize configuration files [1]|>= v1.0.0|
|ped_fw_subcomponent_list| ped_fw_subcomponent_list = basic timer schedule utility| This variable is used to specify which sub-components of ped_fw component will be added for building of variant. By default, all ped_fw sub-components are included for application variants.|>= v1.0.0
|ext_components| ext_components += rtos| Adds component located in ext folder. External components can be GIT submodules or shared components between multiple applications [1]|>= v1.0.0|
|app_components| app_components += \<component\>app_components += led_control| Adds component located in application prog folder. Every application component must have separate build.mak file|>= v1.0.0|

[Back to content](#content)

## **Source code management** <a name="SourceCodeManagement"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|src| `src += PWM/PWM.c`<br>`src += led_control/led_blink.cpp` | Adds source file for translation. Source files has to be with file extensions in contrast with \*obj variables |>= v1.0.0|
|app_libs| `app_libs += NVM/nvm3.a`<br> `app_libs += led_control/led_control_lib.lib`|Variable for adding libraries located in application prog folder.|>= v1.0.0|
|defines| defines += LED_COUNT=10| Content of this variable will be used for creating variant header file|>= v1.0.0|
|search_path| `search_path += $(app_prog_path)/<component>`<br>`search_path += $(app_prog_path)/led_control`|Adds search path for compiler. <span style="color: rgb(255,102,0);">Try to avoid adding search paths. If possible include header files using relative paths like: `#include "led_control/led_control.h` to prevent problems with exceeding maximum command line length.</span>|>= v1.0.0|
|defines_cl| defines_cl += CSD_ENTRY_CALLBACK| Variable for adding definitions which will be used as command line parameters to compiler, bypassing bsh_stdinc.h (former MkDefines.h), This is useful for 3rd party software that can not be changed to include bsh_stdinc.h header file. <span style="color: rgb(255,102,0);">Be careful when adding definitions to this variable, as this might cause problems with exceeding maximum command line length.</span>|>= v1.0.0|
|legacy_search_paths| legacy_search_paths = 1| When defined, adds legacy search paths to all ped_fw components and dbus in common folder|>= v1.0.0|
|obj| `obj += [component]/<source_file>`<br>`obj += led_control/led_blink`|Variable for adding object files for translation. When source file is added it is expected to be .c file. When adding assembly or c++ files, use src variable|>= v1.0.0|

[Back to content](#content)

## **Compilation make variables - compiler** **dependent, see ccTools.mak for selected compiler** <a name="CompilationMakeVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|copt||Compiler options for C and C++ source files|>= v1.0.0|
|ccopt||Compiler options for C source files|>= v1.0.0|
|cppcopt||Compiler options for C++ source files|>= v1.0.0|
|asmopt||Assembler options|>= v1.0.0|
|optim_level| optim_level = 3| Compilation Optimization level (global value for complete build)|>= v1.0.0
|\<module>_optim_level|madc_optim_level = 0|Variable for specifying optimization level for specific module. \<module> needs to be replaced with name of C source file that should be compiled with different optimization level.|>= v1.14.2|
|cc_link_time_opt| cc_link_time_opt = ENABLED|Variable for enabling Link Time Optimization (disabled by default, not recommended for debugging); <span style="color: rgb(165,173,186);">*subtitutes usage of `_link_time_opt` which was valid in v0.4.x and older versions*</span>|>= v1.0.0|
|cc_rtti| cc_rtti = ENABLED| Variable for enabling Real Time Type Identification for C++ code (disabled by default, not recommended); <span style="color: rgb(165,173,186);">*subtitutes usage of `_rtti` which was valid *in v0.4.x and older versions**</span>|>= v1.0.0|
|sw_fpu_enable| sw_fpu_enable = true| Enables software floating point calculations (applicable only for keilArm compiler)|>= v1.0.0|
|ccache_enabled| ccache_enabled = true| Enables compiler cache "ccache". For more information check this page:<br>[How to use ccache][1]|>= v1.4.1|

[Back to content](#content)

## **Static analysis** <a name="StaticAnalysis"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|no_code_analysis| no_code_analysis += <component>/% <br>no_code_analysis += PARAM/parameters.c |Variable used to exclude files or even whole folders from static analysis. This is usually useful for third party SW or automatically generated source files.|>= v1.0.0|

[Back to content](#content)

## **Debug management** <a name="DebugManagement"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|security_disable| security_disable = 1| Variable used to suppress CRC and FS_ROM_Check.<br>If you set "security_disable = 1" then CRC and FS_ROM_Check errors are ignored. Calculation itself is performed though, to preserve time consumption requirements.<br><span style="color: rgb(255,0,0);">"security_disable" is not available from version v1.5.0 - you can use "MAL_FwuOptBytes" in "app/prog/firmware_update/mal/fwu_opt_bytes_cfg.c"</span>|>= v1.4.0   <  v1.5.0|

[Back to content](#content)

## **Library generation** <a name="LibraryGeneration"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|lib_gen| lib_gen += <component>/[library_name]<br>lib_gen += led_control/led_control_lib|Adds new library for library generation. Final library will be placed to $(out_prog_path)/<component_name>/[library_name].lib|>= v1.0.0|
|<library_name>_src| <library_name>_src +=<source_file>.c<br><library_name>_src +=<source_file>.cpp<br><library_name>_src +=<source_file>.asm<br><library_name>_src +=<source_file>.s<br>led_control_lib_src += led_control/led_blink.cpp|Adds source file(s) for translation and then make library from them. Source files can be c, c++ or assembly files, based on used compiler.|>= v1.0.0|
|<library_name>_dep| <library_name>_dep += [library_dependency]|Adds dependency for library. This dependency will be executed prior to generating library, but after compilation of source files |>= v1.0.0|

[Back to content](#content)

## **Binary generation** <a name="BinaryGeneration"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|bin_gen|bin_gen += [target_name]|Creates make target for generation of binary files. For more information check this page: [How to use Binary Generator make targets][2]|>= v1.1.0|

[Back to content](#content)

## **PSoC specific make variables** <a name="PSoCspecificMakeVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|psoc_disable_auto_gen_src| psoc_disable_auto_gen_src = true| This variable is used to disable automatic generation of PSoC source files on PSoC project change|>= v1.0.0|
|psoc_gen_lib| psoc_gen_lib = true | When `true`, PSoC generated sources are added to psoc_lib library. When not defined or `false`, PSoC generated sources are compiled and linked as object files.|>= v1.0.0|

[Back to content](#content)

## **pp_flash target specific make variables** <a name="pp_flashTargetSpecificMakeVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|pp_interface|pp_interface = SWD| When using pp_flash target 'pp_interface' can be overloaded by user's value placing it to application make file or to the command line directly.<br>Possible interface protocols are listed here: JTAG, ISSP, I2C, SWD, SPI, SWD_SWV. Default value is SWD. Recommended value when using MiniProg3 is SWD.|>= v1.4.0<br><span style="color: rgb(165,173,186);">== v1.3.x: interface</span>|
|pp_frequency|pp_frequency = FREQ_03_0| When using pp_flash target 'pp_frequency' can be overloaded by other user's value placing it to application make file or to the command line directly.Possible values for frequency are applicable only for the MiniProg3 device in SWD/JTAG mode: <br>FREQ_03_0, <br>FREQ_01_5, <br>FREQ_01_6, <br>FREQ_08_0, <br>FREQ_12_0, <br>FREQ_06_0, <br>FREQ_03_2, <br>FREQ_16_0, <br>FREQ_24_0, <br>FREQ_48_0. <br> Default value is FREQ_03_0.|>= v1.4.0<br><span style="color: rgb(165,173,186);">== v1.3.x: frequency|
|pp_connector| pp_connector = FIVE_PINS|When using pp_flash target 'pp_connector' can be overloaded by other user's value placing it to application make file or to the command line directly. Possible values for MiniProg3 in SWD mode are: FIVE_PINS, <br>TEN_PINS, <br>FIVE_PINS is for 5-pin connector e.g. KitProg, <br>TEN_PINS is for 10-pins connector e.g. MiniProg3, but MiniProg3 contains 5-pins connector also. Default is FIVE_PINS.|>= v1.4.0<br><span style="color: rgb(165,173,186);">== v1.3.x: connector|

[Back to content](#content)

## **JLink target specific make variables** <a name="JLinkTargetSpecificMakeVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|jlink_flash_exclude_bin| jlink_flash_exclude_bin = true | When true, full chip erase command is skipped in the flashing procedure of `JLink` target in order to preserve areas not being flashed (excluded from binary). Moreover, for PSoC4 platform also RMW procedure is configured to be used for flashing. When not defined or false, flashing with full chip erase will be executed.<br>This may be useful e.g. to preserve Production Data and/or NVMEM area while flashing PSoC4 release binary- see this question in the section Flashing of [FAQ][3].| >= [v1.0.11 LTS](https://github-bshg.boschdevcloud.com/Generic/Framework/releases/tag/v1.0.11)<br> >= [v1.3.3 LTS](https://github-bshg.boschdevcloud.com/Generic/Framework/releases/tag/v1.3.3)<br> >= [v1.4.0](https://github-bshg.boschdevcloud.com/Generic/Framework/releases/tag/v1.4.0)|

[Back to content](#content)

## **doxy target specific make variables** <a name="doxyTargetSpecificMakeVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|doxy_input| doxy_input = $(sdk_prog_path)/firmware_update/BootManager <br> doxy_input = $(call find_files,$(make_search_path),ped_fw/flash/flash.c) | By default, this make variable is automatically set by make process. <br> But it can be overloaded by users to make documentation of one component only. <br> or it can be set for one file only.| >= v1.3.0|
|doxy_quiet | doxy_quiet = false <br> doxy_quiet = true <br> doxy_quiet = filtered | The output from doxygen can be turned on, off or messages could be filtered. <br> If doxy_quite is set to 'false' then **all messages** will be print out to standard output by doxygen. <br> If doxy_quiet is set to 'true' then **no messages** will be print out to standard output by doxygen. This is default value in MCU Framework. <br> If doxy_quite is set to 'filtered', then doxygen is executed in non-quiet mode and additionally, the **output is filtered** by awk. User can use its own awk file by setting make variable `doxy_msg_filter_file`. By default is set to $(local_tool_path)/mixed/doxy_msg_filters.awk.|>= v1.4.2:  true, false, filtered <br> >= v1.3.0: true, false|
|doxy_use_dirs| doxy_use_dirs = true| This make variable is related to **doxy_input** - the variable where is stored list of all source files/directories used as input for Doxygen. <br>Normally **doxy_input** is equal to **doxy_src** variable which stores a list of full-path-named source files from project. In a projects with lot of files in, it might happen that variable **doxy_src** will exceed a boundary 8191 characters - maximum size that can be printed, then **doxy_input** will be automatically switched to variable **doxy_dirs**.<br>The make variable **doxy_dirs** contains a list of directories. This will reduce a size of **doxy_input** variable.<br>Users can explicitly tell Doxygen to use a list of directories by setting make variable **doxy_use_dirs** to `true` <br>By default the variable is set to `false`.|>= v1.3.0|
|doxy_no_sfr| doxy_no_sfr = false | By default  `doxy_no_sfr` is set to `true`. This setting causes excluding header files of MCU from input of Doxygen. It helps to speed up doxy target.<br>Do not change default value if there is no relevant reason to document also structures in MCU header files. Otherwise, an executing the doxy target will take long time.|>= v1.3.0|
|doxy_include_depfiles| doxy_include_depfiles = false | By default is set to `true`. It means all files listed in dependency file (<variant>.dep file) will be included to the input for Doxygen. Explanation: dep file is created at the end of scanning dependency process and it contains all dependent files for every object file. <br>The doxy target uses unique, sorted list of dependency h-files only created from dep file during running doxy target but only if doxy_include_depfiles is `true`.<br>Leave default value for creating documentation of entire project.|>= v1.4.0<br> == v1.3.x: include_depedency_files|
|doxy_project_name| doxy_project_name = First Working Project| This make variable is used for doxygen tag PROJECT_NAME during creation of documentation as main title. By default it is equal to make variable `variant_base` but it can be customized by setting this make variable. It may contain several words. See example.|>= v1.4.0 <br> == v1.3.x: project_doxy_name|
|doxy_project_number| doxy_project_number = 1.8.9.0 | This make variable is used for doxygen tag PROJECT_NUMBER during creation of documentation as version number. User can customize this make variable by setting to defined project version.<br>The default value is set to `12345` as placeholder in documentation. Users can imagine where this value will be placed in document layout.|>= v1.4.0<br> == v1.3.x: project_doxy_number|
|doxy_project_brief| doxy_project_brief = <any short string> | This make variable is used for doxygen tag PROJECT_BRIEF during creation of documentation as optional line description for a project. It appears at the top of each page. Keep this values short (few words.) <br>There is default value set to `My short description about a project $(doxy_project_name).`|>= v1.4.0<br>== v1.3.x: project_doxy_brief|
|doxy_enabled_sections| doxy_enabled_sections = TEST INTERNAL| A list of enabled Doxygen sections (see Doxygen tag ENABLED_SECTIONS in Doxygen manual web-page). More than one section may be enabled.<br>There is no default value for this make variable.|>= v1.3.0|
|doxy_exclude_patterns| doxy_exclude_patterns = \*ior8\*<br>doxy_exclude_patterns = \*/test/\* | Users can use this make variable to specify one or more wildcard patterns to exclude certain files from doxy_input which is defined as list of directories only.<br>Hint: Doxygen matches those wildcards against the files with absolute paths, so use leading asterisk character '\*'.<br>example shows an excluding all test directories.<br>There is no default value for this make variable.|>= v1.3.0|
|doxy_extract_all| doxy_extract_all = YES | Directly set Doxygen tag EXTRACT_ALL therefore possible values are YES and NO.<br>If it is set to YES, then Doxygen will assume all entities in documentation are documented, even if no documentation was available. Private class members and static file members will be hidden unless the EXTRACT_PRIVATE respectively EXTRACT_STATIC tags are set to YES.<br>Note: This will also disable the warnings about undocumented members that are normally produced when WARNINGS is set to YES.<br>There is no default value for this make variable but Doxygen default value is NO.|>= v1.3.0|
|doxy_exclude_src| doxy_exclude_src = true | If user want to do not include source code in generated documentation, then this make variable should be set to `true`.<br>By default there is link to show source code of file in documentation made by Doxygen.<br>It is called 'Go to the source code of this file'.<br>There is no default value for this make variable but source code is included into documentation by default.|>= v1.4.0<br>== v1.3.x: exclude_src|

[Back to content](#content)

## **Others make variables** <a name="OthersMakeVariables"></a>

|Variable name|Example|Description|Version|
|-|-|-|-|
|logging_interrupt_exception|logging_interrupt_exception = true| placement: application make file<br>set to `true` to enable option for logging interrupt excetption handler type by using the custom function `INT_logException`. The function must be defined by users in their module.|v1.9.0|

[Back to content](#content)

## Notes <a name="Notes"></a>

1. When changing this variable, make sure to run `make init` to ensure copying of configuration files needed by newly added component.

[1]: <https://wiki.bsh-sdd.com/display/PFS/How+to+use+ccache> "How to use ccache"
[2]: <https://wiki.bsh-sdd.com/display/PFS/How+to+use+Binary+Generator+make+targets> "How to use Binary Generator make targets"
[3]: <https://wiki.bsh-sdd.com/display/PFS/Frequently+Asked+Questions> "Frequently Asked Questions"

[Home page of help](../../README.md)
