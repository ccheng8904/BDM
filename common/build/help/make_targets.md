# Targets in MCU Framework make process
---

Common syntax:

`make [project=<project>] [variant=<variant>] [<target>] [<switch(es)>]`

If no parameters are specified, default values and default target are used:
 project=""
 variant=MyVariant
 link


### List of built-in variants


|Variant|Example|Description|
|-------|-------|-----------|
|BootManager|`make variant=BootManager`|make will build BootManager variant (FWU3)|
|lromLoader|`make variant=lromLoader`|make will build Loadable ROM loader variant (FWU3)|
|<variant>|`make variant=MyVariant`|name of your variant|
|programmer|`make variant=programmer`|make will build programmer variant (FWU3)|
|ramLoader|`make variant=ramLoader`|make will build RAM loader (FWU1)|
|romLoader|`make variant=romLoader`|make will build ROM loader (FWU1)|

### List of targets


|Target|Example|Description|Version|
|-------|-------|-----------|-------|
|[clean](./make_clean_example.md)|`make clean`|removes all content in the out folder for current variant|>=v0.1.0|
|[clean_all](./make_clean_all_example.md)|`make clean_all`|removes all content in the out folder for all variants|>=v0.6.0|
|[rmbin](./make_rmbin_example.md)|`make rmbin`|removes binaries only in the out folder for current variant|>=v0.1.0|
||||
|[debug](./make_debug_example.md)|`make debug var=<make-variable>`|shows content of make variable <make-variable> written after var=  |>=v0.1.0|
|ccinfo|`make ccinfo`|shows compiler path, compiler version and  compiler options|>=v1.4.1|
||||
|[link](./make_link_example.md)|`make link`|This is default target. It creates binary by running linkage process for current variant.|>=v0.1.0|
|[relink](./make_relink_example.md)|`make relink`|removes binaries and runs link target again.|>=v0.1.0|
|[sym_info](./make_sym_info_example.md)|`make sym_info`|runs link target and show application symbols.|>=v0.1.0|
|[init](./make_init_example.md)|`make init`|copies configuration files from from templates to the app folder.|>=v0.3.1|
|[all](./make_all_example.md)|`make all`|builds all variants at once (the BootManager, lromLoader, lramLoader, programmer and default application variant).|>=v0.1.2|
|[lint](./make_lint_example.md)|`make lint`|This is the 2nd default target. It starts the lint inspection of source code for current variant.|>=v0.1.0|
|[lint2console](./make_lint2console_example.md)|`make lint2console`|shows lint messages in Problems View window in Eclipse (use in Eclipse IDE only, see [link](https://wiki.bsh-sdd.com/display/PFS/How+to+use+Eclipse+with+Framework+including+GNU+make) how to set).|>=v0.4.0|
|[plib](./make_plib_example.md)|`make plib`|creates project library combining all application source files and framework library.|>=v0.4.1|
|gen_libs|`make gen_libs`|Generates all libraries added to lib_gen variable (done automatically during normal build)|>=v0.3.1|
|test_derivative|`make test_derivative`|shows memory boundaries specific for current MCU derivative|>=v0.1.0|
|mem_map|`make mem_map`|shows basic flash memory layout for current project and its corresponding MCU derivative|>=v1.4.0|
|[release](./make_release_example.md)|`make release`| - make variable *build_type* is automatically set to RELEASE (application variants are always completely rebuilt and macros DEBUG=0 and NDEBUG are defined)<br> - FWU OTP memory area is excluded from release binaries.<br><br>**NOTE:** Unlike in older Framework versions (up to v1.8.x), HWID compatibility check is no more performed after boot in FWU3. Instead, new Firmware Compatibility Check (FCC) is performed. Now, Programmer is no more entered when OTP area is empty.<br>For more details about FCC, please, refer to file [Firmware_Compatibility_Check](./../../prog/firmware_update/doc/Firmware_Compatibility_Check.md).|>=v1.4.0|
|reset_dbuscan|`make reset_dbuscan`| - auxiliary make target for muting node with DBusCAN chip on DBus line (by sending GoOffline message)|>=v1.16.1|
|||**Flashing using FlashProgrammer**|
|flash|`make flash`|starts link target and then flashes a binary (using FlashProgrammer)|>=v0.1.0|
|flash_no_link|`make flash_no_link`|starts flashing a binary only (using FlashProgrammer)|>=v0.1.0|
|flash_only|`make flash_only`|starts flashing a binary only (using FlashProgrammer)|>=v0.1.0|
|verify|`make verify`|starts verifying a flashed binary (using FlashProgrammer)|>=v0.1.0|
|||**Flashing using ST-Link programmer**|
|flashST|`make flashST`|starts link target and then flashes a binary (using ST-Link)|>=v0.1.0|
|flashST_no_link|`make flashST_no_link`|starts flashing a binary only (using ST-Link)|>=v0.1.0|
|flashST_only|`make flashST_only`|starts flashing a binary only (using ST-Link)|>=v0.1.0|
|||**Flashing using GDLink programmer**|
|flashGD|`make flashGD`|starts link target and then flashes a binary (using GDLink)|>=v4.8.4.18197|
|flashGD_no_link|`make flashGD_no_link`|starts flashing a binary only (using GDLink)|>=v4.8.4.18197|
|flashGD_only|`make flashGD_only`|starts flashing a binary only (using GDLink)|>=v4.8.4.18197|
|flashGD_erase|`make flashGD_erase`|starts erasing whole chip only (using GDLink)|>=v4.8.4.18197|
|||**Flashing using JFlash with SEGGER J-Link programmer**|
|JFlash|`make JFlash`|starts link target and then flashes a binary (using J-Link programmer with JFlash)<br>Hint: Legacy, this is supported for PSoC4, NXP-MKE and NXP-MKV only. It is proposed to use JLink target instead|>=v0.5.0|
|JFlash_no_link|`make JFlash_no_link`|starts flashing a binary only (using J-Link programmer with JFlash)<br>see hint for JFlash target|>=v0.5.0|
|JFlash_only|`make JFlash_only`|starts flashing a binary only (using J-Link programmer with JFlash)<br>see hint for JFlash target|>=v0.5.0|
|||**Flashing using JLink Commander with SEGGER J-Link programmer**|
|JLink|`make JLink`|starts link target and then flashes a binary (using J-Link programmer with JLink Commander); updates JLink firmware in the background|>=v0.5.0|
|JLink_no_link|`make JLink_no_link`|starts flashing a binary only (using J-Link programmer with JLink Commander); updates JLink firmware in the background|>=v0.5.0|
|JLink_only|`make JLink_only`|starts flashing a binary only (using J-Link programmer with JLink Commander); updates JLink firmware in the background|>=v0.5.0|
|||**Flashing using Renesas Flash Programmer with E1 Programmer**|
|flashE1|`make flashE1`|starts link target and then flashes a binary (using Renesas E1 programmer)|>=v0.1.0|
|flashE1_no_link|`make flashE1_no_link`|starts flashing a binary only (using Renesas E1 programmer)|>=v0.1.0|
|flashE1_only|`make flashE1_only`|starts flashing a binary only (using Renesas E1 programmer)|>=v0.1.0|
|||**Flashing using PS-Flasher for firmware update**|
|psflash|`make psflash`|starts link target and then flashes a binary (using PS-Flasher and python script)|>=v0.1.0|
|psflash_no_link|`make psflash_no_link`|starts flashing a binary only (using PS-Flasher and python script)|>=v0.1.0|
|psflash_only|`make psflash_only`|starts flashing a binary only (using PS-Flasher and python script)|>=v0.1.0|
|psverify|`make psverify`|starts verifying a flashed binary (using FlashProgrammer)|>=v0.1.0|
|||**Flashing using PSoC Programmer with MiniProg3 (see related List of make variables and definitions for application)**|
|pp_flash|`make pp_flash`|starts link target and then flashes a binary (using Cypress PSoC Programmer, it should be locally installed on PC)|>=v0.4.0|
|pp_flash_no_link|`make pp_flash_no_link`|starts flashing a binary only (using Cypress PSoC Programmer, it should be locally installed on PC)|>=v0.4.0|
|pp_flash_only|`make pp_flash_only`|starts flashing a binary only (using Cypress PSoC Programmer, it should be locally installed on PC)|>=v0.4.0|
|||**Axivion - Static Code Analysis Tool (see related wiki page)**|
|axanalysis|`make axanalysis`|To perform CI based static code analysis using axivion|>=v0.1.0|
|axstyle|`make axstyle`|To detect and report MISRA C Stylecheck violations|>=v0.1.0|
|ncov|`make ncov`|To detect and report BSH Naming Convention violations|>=v0.4.0|
|axstyle_quick|`make axstyle_quick`|To detect and report MISRA C Stylecheck violations (faster mechanism)|>=v0.4.0|
|ncov_quick|`make ncov_quick`|To detect and report BSH Naming Convention violations (faster mechanism)|>=v0.4.0|
|axivion|`make axivion`|To create project level IR (Intermediate Representation) file using axivion|>=v0.1.0|
|axpstd|`make axpstd`|To create unit testing stubs using axivion|>=v0.1.0|
|axconfig|`make axconfig`|To open axivion configuration editor|>=v0.1.0|
|axirviewer|`make axirviewer`|To open axivion IR viewer|>=v0.1.0|
|axgravis|`make axgravis`|To open axivion gravis GUI|>=v0.1.0|
|axstyle_help|`make axstyle_help`|It gives information of all the Stylecheck rules and configuration parameters|>=v0.1.0|
|cure|`make cure misra=2.1`|To display MISRA curing guide example for MISRA C rule.  (misra = rule_number)|>=v0.6.0|
|||**VectorCAST- Unit Testing Tool (see related wiki page)**|
|dyntconfig|`make dyntconfig`|To Prepare Dynamic Testing VectorCast Configuration|>=v0.3.1.r2|
|dyntcreateenv|`make dyntcreateenv`|To create VectorCast Test environments automatically|>=v0.3.1.r2|
|dyntupdate|`make dyntupdate`|To update changes in the Project configuration, defines, VC configuration into VCM (VectorCast Manage) Project file.|>=v0.5.1|
|dyntstart|`make dyntstart`|To start VectorCast GUI|>=v0.3.1.r2|
|dyntreport|`make dyntreport`|To collect Project level and Unit level report in the HTML format after unit tests execution|>=v0.5.2|
|dyntreuse|`make dyntreuse`|To reuse existing test environments and test cases present in one project in to another project|>=v0.6.1|
|||**PSoC Creator**|
|psoc_init|`make psoc_init`|Initializes PSoC Creator projects (done automatically on project setup)|>=v0.4.0|
|psoc_gen_src|`make psoc_gen_src`|Generates PSoC Creator project source files (done automatically during normal build)|>=v0.4.0|
|||**Documentation**|
|doxy|`make doxy`|generates documentation for current variant in chm format|>=v0.1.0|
|pdf|`make pdf`|generates documentation for current variant in pdf format (available in v0.5.3 and newer)|>=v0.1.0|
|help|`make help`|shows the list of make targets and contact information|>=v0.1.0|
|||**Chocolatey- Software Management Tool**|
|choco|`make choco`|installs selected compiler for project and also other tools (Python, Doxygen, Axivion, Curl...)|>=v0.6.0|
|choco_flasher_tool|`make choco_flasher_tool`|installs flashing tools (Renesas Flash Programmer, STM32Cube, PSFlasher, Silabs Commander ...)|>=v0.6.0|
|generic_drivers|`make generic_drivers`|installs generic drivers for UDA 1, UDA2, JLink, ST Link, E1Flasher|>=v0.6.0|
|choco_flasher|`make choco_flasher`|installs both flashing tools and generic drivers|>=v0.6.0|
|choco_setup|`make choco_setup`|installs the version of Python necessary for setup and build of application and also for other functionalities|>=v0.6.0|
|choco_enable|`make choco_enable`|enables using Chocolatey|>=v0.6.0|
|choco_disable|`make choco_disable`|disables using Chocolatey|>=v0.6.0|
|choco_is_present|`make choco_is_present`|checks whether the Chocolatey is installed by trying to launch it|>=v0.6.0|
|||**Artifactory - Universal Repository Manager**|
|artifactory_download|`make artifactory_download af_username=<email> af_api_key=<apikey> af_doc_repo=<Https_address> af_target_download_address=<path> af_target_download_local_path=<path> af_http_proxy=<url> af_bypass_http_proxy=<bool>`|download file from Artifactory<br>- **af_username:** Jfrog artifactory authentication bosch user email<br>- **af_api_key:** Jfrog artifactory authentication token<br>- **af_doc_repo:** Repository address<br>- **af_target_download_address:** Path to file in repository<br>- **af_target_download_local_path:** Local storage address<br>- **af_http_proxy:** a proxy server URL<br>- **af_bypass_http_proxy:** true or false, false id default -> a proxy is used|>=v1.1.0|
|artifactory_upload|`make artifactory_upload af_username=<email> af_api_key=<apikey> af_doc_repo=<Https_address><br>af_target_uploade_path_to_file=<path> af_target_upload_address=<path> af_http_proxy=<url> af_bypass_http_proxy=<bool>`|upload file to Artifactory<br>- **af_username:** Jfrog artifactory authentication bosch user email<br>- **af_api_key:** Jfrog artifactory authentication token<br>- **af_doc_repo:** Repository address<br>- **af_target_uploade_path_to_file:** Local storage address of file to upload<br>- **af_target_upload_address:** Storage path in repository<br>- **af_http_proxy:** a proxy server URL<br>- **af_bypass_http_proxy:** true or false, false id default -> a proxy is used|>=v1.1.0|
|datasheet|`make datasheet`|download and open datasheet|>=v1.4.1|
|refman|`make refman`|download and open reference manual|>=v1.4.1|
|errata|`make errata`|download and open errata sheet|>=v1.14.2|
|||Ccache - Compiler cache (in evaluation, see related wiki page)|
|ccache_stat|`make ccache_stat`|Display statistics including hit/miss rate, number of files in cache, cache size, cache directory etc.|>=v1.4.1|
|ccache_clean|`make ccache_clean`|Clean cache, but keep the statistics|>=v1.4.1|


[Home page of help](../../README.md)
