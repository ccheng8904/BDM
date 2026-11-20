#           MCU Framework help main page
---


The MCU Framework is a collection of components, tools and make files joined together to one unit. It is using GNU make to automate operation and there are defined many target to do it.
The MCU Framework is versioned by GitHub - a distributed version control system. More info about the MCU Framework can be found [here](./README.md).

## Targets in Framework make process

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


|Target|Example|Description|
|-------|-------|-----------|
|[clean](./build/help/make_clean_example.md)|`make clean`|removes all content in the out folder for current variant|
|[clean_all](./build/help/make_clean_all_example.md)|`make clean_all`|removes all content in the out folder for all variants
|[rmbin](./build/help/make_rmbin_example.md)|`make rmbin`|removes binaries only in the out folder for current variant|
||||
|[debug](./build/help/make_debug_example.md)|`make debug var=<make-variable>`|shows content of make variable <make-variable> written after var=  |
|[ccinfo](./build/help/make_ccinfo_example.md)|`make ccinfo`|shows compiler path, compiler version and  compiler options|
||||
|[link](./build/help/make_link_example.md)|`make link`|This is default target. It creates binary by running linkage process for current variant.|
|[relink](./build/help/make_relink_example.md)|`make relink`|removes binaries and runs link target again.|
|[sym_info](./build/help/make_sym_info_example.md)|`make sym_info`|runs link target and show application symbols.|
|[init](./build/help/make_init_example.md)|`make init`|copies configuration files from from templates to the app folder.|
|[all](./build/help/make_all_example.md)|`make all`|builds all variants at once (the BootManager, lromLoader, lramLoader, programmer and default application variant).|
|[lint](./build/help/make_lint_example.md)|`make lint`|This is the 2nd default target. It starts the lint inspection of source code for current variant.|
|[lint2console](./build/help/make_lint2console_example.md)|`make lint2console`|shows lint messages in Problems View window in Eclipse (use in Eclipse IDE only, see [link](https://wiki.bsh-sdd.com/display/PFS/How+to+use+Eclipse+with+Framework+including+GNU+make) how to set).|
|[plib](./build/help/make_plib_example.md)|`make plib`|creates project library combining all application source files and framework library.|





