# BDM

!Please add you project description here

## Getting Started


Type the following to compile the default variant BDM:
```
make
```


The following variants are currently supported:
* BDM
* --
* --
* --

To lint your default variant you can type:
```
make lint
```

The following targets are currently supported
* link (default)
* lint
* clean
* init  (just to initialize a naked project)


### Prerequisites

You need a valid compiler license


### Multiprocessing

Make supports multiprocessing. To speed-up your build type e.g.:
```
make lint -j
```

To define the number of processes:
```
make lint --jobs=4
```

Usually on a quad core system --jobs=8 is optimum. On a dual core system use --jobs=4.


## Flashing and debugging (J-Link / GDB)

`make.bat` works like before (every target is forwarded to `common\build\makefile`),
but additionally offers the helpers below. They use the framework tools
(WinFlash `Flash.exe`, `arm-none-eabi-gdb.exe`) and the SEGGER J-Link command line
tools.

| Command | Description |
|---------|-------------|
| `make.bat flash` | build (link) and flash via WinFlash over UART/DBus (default application variant) |
| `make.bat flash_no_link` | flash without rebuilding |
| `make.bat verify` | verify the flashed image |
| `make.bat debug` | starts the J-Link GDB server (SWD, port 2331) |
| `make.bat debug batch` | same, non interactive - stops after 5 s (connectivity check) |
| `make.bat gdb` | attaches gdb to the running GDB server (add `batch` as 2nd parameter for a non interactive run) |

All other targets (`link`, `all`, `relink`, `clean`, `release`, `debug var=...`,
`test_flashopt`, ...) are forwarded unchanged to GNU make.

The helpers can be adapted with environment variables: `JLINK_DEVICE` (default
`Cortex-M0+`), `JLINK_SPEED` (default `400`), `GDB_PORT` (default `2331`),
`JLINK_PATH` (J-Link installation folder) and `GDB_ELF` (default
`out\BDM\bin\BDM.elf`). The WinFlash communication settings are the make
variables of the framework: `UDA` (default `UDA-01`), `UDA_TYPE` (UDA1/UDA2),
`UDA_BUS_TYPE` and the DBus node address in `app\build\common_config.mak`.

### Flashing

`make.bat flash` = `make link` + WinFlash (`Flash.exe`). The WinFlash option
string is taken from the framework target `make test_flashopt`; the doubled path
separators (`\\`) that the framework generates are reduced to the single
backslash that `Flash.exe` expects.

Flashing needs:

* the UDA / DBus2 interface connected and powered (`UDA-01` in `common_config.mak`)
* `out\ramLoader\bin\ramLoader.mot` - build the FWU variants once with
  `make.bat all` (the flash target only builds the application variant)
* the controller description `out\BDM\bin\winflash_cfg.bp2` (generated from
  `common\prog\MSP\cms32m67\bootloader\CMS32M67xx.bp2.h`, page size 0x200,
  flash 0x200-0x1FFFF)

> **Known limitation of `CMS32M67xx.bp2.h`:** `AppStartVector` / `Reset` are still
> `0x0000`, while the flash area starts at `0x200`. When the UDA is connected,
> WinFlash therefore reports
> `Reset vector or its part is defined out of flash memory` and aborts. Moving both
> keys to `0x200` and `BlockSize` / `BootloaderBlockSize` to `0x100` (0xF00, the
> BootManager size, is not a power of 2) removes these warnings, but Flash.exe then
> terminates without any report (`Finished not correctly`) - this could not be
> verified here because no UDA / DBus interface is attached. Please confirm the
> values against a working CMS32M67 BP2 description (CMS-Writer / WinFlash manual)
> before the first real flash run.

Alternative flashing tools: CMS CMS-Writer / CMS-ICE with
`Cmsemicon.CMS32M67xx.0.2.1\Flash\CMS32M67xxGH.FLM`, or Keil MDK with the
CMS32M67xx device family pack.

### Debugging - important limitation

J-Link V9.22 has **no** built-in device support for the CMS32M6736 (`Cmsemicon` is
only covered for BAT32G133/BAT32G157), therefore the generic device `Cortex-M0+` is
used. That device allows SWD debugging (halt, single step, breakpoints, RAM access)
but **no flash programming** - there is no flash algorithm. Program the chip with
`make.bat flash` (or one of the tools above) first, then attach the debugger.

### Debugging in VSCode

Three configurations are prepared in `.vscode\launch.json`:

* **J-Link Debug BDM (auto GDB server)** - one click (F5): starts the GDB server,
  loads the symbols of `out\BDM\bin\BDM.elf`, stops at `main`, shows peripherals
  from `common\prog\MSP\cms32m67\drv\CMS32M67xx.svd` and live watch
  (`loadFiles: []` -> the flash is not programmed). Requires the extension
  `marus25.cortex-debug`.
* **J-Link Debug BDM (external GDB server)** - start the VSCode task
  `jlink gdb server` (or `make.bat debug`) first, then launch. Requires the
  extension `ms-vscode.cpptools`.
* **J-Link Attach BDM** - attach to a running target without reset.

The corresponding build/flash tasks are in `.vscode\tasks.json`.

### RAM debugging

`out\ramLoader\bin\ramLoader.elf` is linked to RAM, so it can be downloaded with
gdb (`load`) without a flash algorithm. Uncomment the two lines at the end of
`gdb_init.gdb` to use it.