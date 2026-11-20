# MCU Framework help main page
---

MCU Framework is a microcontroller development platform from GED. It provides a solution enabling engineers to develop embedded applications from initial evaluation to final production. MCU Framework is a collection of components, tools and make files joined together to one unit. It is using GNU Make to automate operation using specific defined target to do that. GitHub - distributed version control system is used for the MCU Framework. More info about the MCU Framework can be found in the sections below.


### How to use this help system

The command `make help` will show this page. The `<keyword>` may follow after `make help`, then it will show a page related to the topic of the `<keyword>`.
The keyword might be:
- a keyword of make
    - e.g. `make help build` will show help page that describes a build process
- a component name
    - e.g. `make help firmware_update` will show help page related to the Firmware update
    - or   `make help mcal` will show help page related to the MCAL
- a MCAL component
    - e.g. `make help mdio` will show help page related to the specific MCAL component: MDIO
- a HAL component
    - e.g. `make help hdio` will show help page related to the HAL component HDIO
---


## The content of help
<br>

**[What is a make](./build/help/make.md)**

**[Getting started - setup project](./build/help/make_setup.md)**

**[Getting started - task list after project setup](./build/help/make_task_list_after_project_setup.md)**

**[Build project](./build/help/build.md)**

**[Firmware Update](./prog/firmware_update/README.md)**

**[List of targets](./build/help/make_targets.md)**

**[List of variables](./build/help/make_variables.md)**

**[MCAL documentation](./prog/MSP/mcal/README.md)**

**[HAL documentation](./prog/MSP/doc/hal.md)**

**[DBusCAN chip](./prog/devices/dbuscan/README.md)**

**[External Flash - SPI configuration for BTM projects](./prog/MSP/efr32xg21/doc/ext_flash_spi_config_for_btm_proj.md)**

**[Scheduler New](./prog/schedulers_bm/scheduler/doc/README.md)**


<br><br><br>
### Support

- For complete documentation check [main MCU Framework wiki page](https://wiki.bsh-sdd.com/display/PFS)
- To learn basic rules and good practices check [Basic rules and good practices](https://wiki.bsh-sdd.com/display/PFS/Basic+rules+and+good+practices)
- For frequently asked questions refer to [Frequently Asked Questions](https://wiki.bsh-sdd.com/display/PFS/Frequently+Asked+Questions)
- Please write to PED-support@bshg.com if you need further help.
