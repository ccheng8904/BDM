#*******************************************************************************
# BDM - gdb init script
#*******************************************************************************
# Attaches gdb (Tools/8_2019-q3-update/bin/arm-none-eabi-gdb.exe) to the
# J-Link GDB server started by "make.bat debug".
#
# The port has to match GDB_PORT in make.bat (default 2331).
#
# IMPORTANT: J-Link V9.22 has no flash algorithm for CMS32M6736, therefore the
#            generic device "Cortex-M0+" is used and the flash image must NOT be
#            downloaded with "load" here. Program the device with WinFlash (UART),
#            the CMS CMS-Writer or Keil, then attach with this script.
#*******************************************************************************

set pagination off
set confirm off
set print pretty on

target remote localhost:2331

# clear the breakpoints that may be left over from a previous session
monitor clrbp
monitor endian little
monitor reset halt

# Pure RAM debugging (no flash programming needed):
# uncomment the two lines below to download and debug the RAM loader image.
# file out/ramLoader/bin/ramLoader.elf
load

break main
# continue
