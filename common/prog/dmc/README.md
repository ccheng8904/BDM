# Data Module Creator

## 1 What is it for?

If you would like to create a module which
- contains pure data (e.g. algorithm parameters, curve table values, appliance variant settings)
- occupies dedicated memory space after application code in ROM
- itself may also have different variants (e.g. different parameter set for CN/EU product or different system settings for BOSCH/SIEMENS)
- may be built as part of the final project binary
- may be built independently creating binary containing only addresses of this data module

Then this Data Module Creator(DMC) helps you to create most of the related files automatically. You will be able to modify those generated files as you want.
However there're still a few steps which are hard to be automated, you'll need to go through them manually. 

## 2 Which files will be generated automatically?

Let's say your data module is called **config**, and it has 2 variants: **bosch** and **siemens**.
Then below files will be generated for you:
- *`app/build/config_bosch.mak`*
- *`app/build/config_siemens.mak`*
- *`app/prog/config_bosch/build.mak`*
- *`app/prog/config_bosch/config_bosch.c`*
- *`app/prog/config_bosch/config_bosch.h`*
- *`app/prog/config_bosch/ModuleHeader_bosch.c`* (only included in build process if it's FWU3 project)
- *`app/prog/config_siemens/build.mak`*
- *`app/prog/config_siemens/config_siemens.c`*
- *`app/prog/config_siemens/config_siemens.h`*
- *`app/prog/config_siemens/ModuleHeader_siemens.`* (only included in build process if it's FWU3 project)
- *`app/prog/dmc/LinkerScriptData.h`*

Here 'config_bosch' and 'config_siemens' are created as two standard app_components (if you don't know what app_component is, please take a look at [How to add project files to Framework](https://wiki.bsh-sdd.com/display/PFS/How+to+add+project+files+to+Framework)). 
In this way you can easily add them to project build process in *`<MyVariant>.mak`*. 
Meanwhile, *`config_bosch.mak`* and *`config_siemens.mak`* are created so that you could build them independently as two of the app_variants, because *`config_bosch.mak`* only contains component config_bosch and nothing else, similar goes for *`config_siemens.mak`*.

## 3 How to create data module?

Still taking data module **config** as an example, here are the steps you need to do:

1. Add the following line in **`app/[<MyProject>/]build/common_config.mak`**:
    ```Makefile
    app_dmodules = config
    ```
    It's also possible to add multiple data modules:
    ```Makefile
    app_dmodules = config CurveTable
    ```
2. Define application end address and the size of config module in **`app/[<MyProject>/]build/common_config.mak`** (application end address should be moved to make space for data module):
    ```Makefile
    FWU_APP1_END_ADDRESS = 0801EFFF
    FWU_CONFIG_SIZE = 800
    ```    
    Multiple data modules:
    ```Makefile
    FWU_APP1_END_ADDRESS = 0801EFFF
    FWU_CONFIG_SIZE = 800
    FWU_CURVETABLE_SIZE = 800
    ```

3. Execute `make init_dmc dmodule=config dvariant=bosch`, now an app_component 'config_bosch' will be created. (*`app/prog/dmc/LinkerScriptData.h`*, *`app/build/config_bosch.mak`*, *`app/prog/config_bosch/...`* as described in the chapter 2 above)
You can repeat this step if you have more data modules or more variants of the same data module e.g. `make init_dmc dmodule=config dvariant=siemens`, `make init_dmc dmodule=CurveTable dvariant=curve1`.

4. Modify generated files according to your need. For example, in *`app/prog/config_bosch/config_bosch.c`* you can edit the data; in *`app/prog/config_bosch/build.mak`* you can set the Firmware ID of this data module.

5. When you want to add config_bosch to your project build process, please add it in **`app/[<MyProject>/]build/<MyVariant>.mak`**:
    ```Makefile
    # application specific components
    app_components = config_bosch CurveTable_curve1
    ```
   When you want to build one data module alone, please add it as an app_variant in **`app/[<MyProject>/]build/common_config.mak`**:
    ```Makefile
    # list of application variants, first is default
    app_variants   = MyVariant config_bosch config_siemens CurveTable_curve1 CurveTable_curve2
    ```
   To build config_bosch binary please execute `make variant=config_bosch`.

6. If you want to add this data module as a partition, you still need to complete [3.2.3 Add a module ID of a new SW module](../firmware_update/README.md#323-add-a-module-id-of-a-new-sw-module), [3.2.4 Add a partition entry of a new SW memory module into the fixed partition table](../firmware_update/README.md#324-add-a-partition-entry-of-a-new-sw-memory-module-into-the-fixed-partition-table) and [3.2.7 Extend ECU Config Read Response DBus message due to a new SW module](../firmware_update/README.md#327-extend-ecu-config-read-response-dbus-message-due-to-a-new-sw-module) from [3.2 How-to add a custom SW memory module](../firmware_update/README.md#32-how-to-add-a-custom-sw-memory-module)