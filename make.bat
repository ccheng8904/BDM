::*******************************************************************************
:: BDM build and debug launcher
::*******************************************************************************
:: Everything that is not one of the helpers below is forwarded to
:: common\build\makefile (GNU make), e.g. "make.bat link", "make.bat all".
:: The framework targets "flash", "flash_no_link", "verify", ... are forwarded
:: to make as well - the following wrappers only replace them where an extra
:: conversion step is needed on Windows:
::
::   make.bat flash [<make args>]  build (link) and flash via WinFlash/UART
::                                 (= "make link flash_no_link")
::   make.bat flash_no_link [...]  flash without rebuilding
::   make.bat verify [...]         verify a flashed image
::   make.bat debug [batch]        start the J-Link GDB server (SWD, port 2331)
::                                 "batch" = non interactive, stops after 5 s
::   make.bat gdb                  attach arm-none-eabi-gdb to a running server
::                                 (2nd parameter "batch" = non interactive)
::
:: Why the flash wrappers: the framework builds the WinFlash (Flash.exe) option
:: string with a doubled path separator ("\\"), which Flash.exe cannot open -
:: the flash process fails with "Finished not correctly". The wrappers take the
:: framework option string ("make test_flashopt"), reduce the doubled separator
:: to the single backslash Windows expects and hand it over to Flash.exe.
::
:: Note: J-Link (V9.22) has no built-in device support for CMS32M6736, so the
::       generic "Cortex-M0+" device is used: SWD flash programming is NOT
::       possible this way. Program the chip with WinFlash (UART), the CMS
::       CMS-Writer or Keil first and use the J-Link for debugging only.
::
:: Environment variables that may be preset:
::   JLINK_DEVICE  (default Cortex-M0+)
::   JLINK_SPEED   (default 400)
::   GDB_PORT      (default 2331)
::   JLINK_PATH    (folder that contains JLinkGDBServerCL.exe)
::   GDB_ELF       (default out\BDM\bin\BDM.elf)
::*******************************************************************************

@echo off
setlocal enabledelayedexpansion
pushd "%~dp0"

if not defined JLINK_DEVICE set "JLINK_DEVICE=Cortex-M0+"
if not defined JLINK_SPEED  set "JLINK_SPEED=400"
if not defined GDB_PORT     set "GDB_PORT=2331"
if not defined GDB_ELF      set "GDB_ELF=%CD%\out\BDM\bin\BDM.elf"
set "JLINK_FLASH_DEVICE=Cortex-M0+"
set "GDB_EXE=%~dp0Tools\8_2019-q3-update\bin\arm-none-eabi-gdb.exe"
set "GMAKE=%~dp0common\tools\gmake\make.exe"
set "MAKEFILE=common\build\makefile"
set "FLASH_EXE=%~dp0common\tools\Flashprogrammer\4.2.1\Flash.exe"
set "JLINK_FLASH_TEMPLATE=%~dp0flash.jlink"
set "JLINK_FLASH_SCRIPT=%TEMP%\bdm_flash.jlink"
set "DOUBLE=\\"

if /i "%~1"=="flash"         goto flash_prefer_jlink
if /i "%~1"=="flash_no_link" goto flash_prefer_jlink
if /i "%~1"=="verify"        goto verify
if /i "%~1"=="debug"         goto chk_debug
if /i "%~1"=="gdb"           goto chk_gdb
goto make


::-------------------------------------------------------------------------------
:: make.bat flash  - prefer J-Link direct flash, fallback to WinFlash if needed
::-------------------------------------------------------------------------------
:flash_prefer_jlink
if /i "%~1"=="flash" (
    echo [BDM] make link
    "%GMAKE%" -f "%MAKEFILE%" link %*
    if errorlevel 1 goto end_error
)
call :locate_jlink_exe
if errorlevel 1 (
    echo [BDM] J-Link not found, falling back to WinFlash
    goto do_flash
)
call :detect_jlink_flash_device
if errorlevel 1 (
    echo [BDM] J-Link device detection failed, falling back to WinFlash
    goto do_flash
)
call :write_jlink_flash_script
if errorlevel 1 (
    echo [BDM] failed to generate J-Link flash script, falling back to WinFlash
    goto do_flash
)
echo [BDM] trying J-Link Commander flash with device %JLINK_FLASH_DEVICE%
"%JLINK_EXE%" -CommanderScript "%JLINK_FLASH_SCRIPT%"
if not errorlevel 1 (
    echo [BDM] -------- J-Link flashing done -------------
    goto end
)
echo [BDM] J-Link flash failed, falling back to WinFlash
goto do_flash


::-------------------------------------------------------------------------------
:: make.bat flash_no_link  - flash only
::-------------------------------------------------------------------------------
:flash_only
goto do_flash


::-------------------------------------------------------------------------------
:: WinFlash call for the default application variant (Flash.exe, WinFlash)
::-------------------------------------------------------------------------------
:do_flash
call :winflash_cfg
if errorlevel 1 goto end_error
call :winflash_opt
if errorlevel 1 goto end_error
call :check_images
if errorlevel 1 goto end_error
echo [BDM] flashing %FLASH_FILE%
"%FLASH_EXE%" -f "!FLASH_OPT!"
if errorlevel 1 goto end_error
echo [BDM] -------- Flashing done -------------
goto end


::-------------------------------------------------------------------------------
:: make.bat verify  - verify the flashed image
::-------------------------------------------------------------------------------
:verify
call :winflash_cfg
if errorlevel 1 goto end_error
call :winflash_opt
if errorlevel 1 goto end_error
call :check_images
if errorlevel 1 goto end_error
echo [BDM] verifying %FLASH_FILE%
"%FLASH_EXE%" -v "!FLASH_OPT!"
if errorlevel 1 goto end_error
echo [BDM] -------- Verify done ---------------
goto end


::-------------------------------------------------------------------------------
:: helper: regenerate the WinFlash controller description (bp2)
::         GNU make does not see the msp_path/bootloader template in its
::         prerequisites, so an edit of CMS32M67xx.bp2.h may not be picked up
::-------------------------------------------------------------------------------
:winflash_cfg
if not exist "%~dp0out\BDM\bin\winflash_cfg.bp2" (
    echo [BDM] generating WinFlash controller description
    "%GMAKE%" -f "%MAKEFILE%" link >nul 2>&1
)
exit /b 0


::-------------------------------------------------------------------------------
:: helper: build the WinFlash command line of the default application variant
::         (framework target "test_flashopt"), see file header
::-------------------------------------------------------------------------------
:winflash_opt
echo [BDM] collecting WinFlash options
"%GMAKE%" -f "%MAKEFILE%" test_flashopt mcufw_colored_output=false >"%TEMP%\bdm_flashopt.txt" 2>&1
set "FLASH_OPT_FILE=%TEMP%\bdm_flashopt.txt"
set "FLASH_OPT="
set "FLASH_FILE="
set "RAMLOADER="
set "in_opt="
for /f "usebackq delims=" %%l in ("%FLASH_OPT_FILE%") do (
    set "line=%%l"
    if "!line:Winflasher option summary:=!" neq "!line!" (
        set "in_opt=1"
    ) else if defined in_opt (
        if "!line!"=="" (
            set "in_opt="
        ) else (
            set "FLASH_OPT=!FLASH_OPT! !line!"
        )
    )
)
if not defined FLASH_OPT (
    echo [BDM] ERROR: could not read the WinFlash options - output follows:
    type "%FLASH_OPT_FILE%"
    del "%FLASH_OPT_FILE%" >nul 2>&1
    exit /b 1
)
del "%FLASH_OPT_FILE%" >nul 2>&1

:: the framework uses a doubled path separator ("\\"), Windows needs one
set "FLASH_OPT=!FLASH_OPT:%DOUBLE%=\!"

:: pull the image paths out of "-m <ram loader> -a <application>"
set "prev_flag="
for %%p in (!FLASH_OPT!) do (
    if defined prev_flag (
        if /i "!prev_flag!"=="-a" set "FLASH_FILE=%%~p"
        if /i "!prev_flag!"=="-m" set "RAMLOADER=%%~p"
        set "prev_flag="
    ) else (
        if /i "%%~p"=="-a" set "prev_flag=-a"
        if /i "%%~p"=="-m" set "prev_flag=-m"
    )
)
exit /b 0


::-------------------------------------------------------------------------------
:: helper: check that the images referenced in the option string exist
::-------------------------------------------------------------------------------
:check_images
if not defined RAMLOADER goto check_images_app
if not exist "!RAMLOADER!" (
    echo [BDM] WARNING: ram loader image not found: !RAMLOADER!
    echo [BDM]          run "make.bat all" to build all variants
)
:check_images_app
if not defined FLASH_FILE (
    echo [BDM] ERROR: application image not found in the WinFlash options
    exit /b 1
)
if not exist "!FLASH_FILE!" (
    echo [BDM] ERROR: application image not found: !FLASH_FILE!
    echo [BDM] run "make.bat link" first
    exit /b 1
)
exit /b 0


::-------------------------------------------------------------------------------
:: make.bat debug  - start the J-Link GDB server
::-------------------------------------------------------------------------------
:chk_debug
:: "make debug var=<make variable>" is a framework target -> forward it to make
if not "%~2"=="" if /i not "%~2"=="batch" goto make
call :locate_gdb_server
if errorlevel 1 exit /b 1
echo [BDM] J-Link GDB server: device %JLINK_DEVICE%, SWD, port %GDB_PORT%
if /i "%~2"=="batch" (
    echo [BDM] non interactive mode - stopped on purpose after 5 s
    start "J-Link GDB server" /min "%JLINK_GDB_SERVER%" -if SWD -device %JLINK_DEVICE% -speed %JLINK_SPEED% -port %GDB_PORT% -singlerun
    ping -n 6 127.0.0.1 >nul
    taskkill /f /im JLinkGDBServerCL.exe >nul 2>&1
) else (
    echo [BDM] stop with Ctrl+C
    "%JLINK_GDB_SERVER%" -if SWD -device %JLINK_DEVICE% -speed %JLINK_SPEED% -port %GDB_PORT%
)
goto end


::-------------------------------------------------------------------------------
:: make.bat gdb    - attach gdb (2nd parameter "batch" -> non interactive)
::-------------------------------------------------------------------------------
:chk_gdb
if not exist "%GDB_EXE%" (
    echo [BDM] ERROR: gdb not found: %GDB_EXE%
    exit /b 1
)
if not exist "%GDB_ELF%" (
    echo [BDM] ERROR: elf not found: %GDB_ELF%
    echo [BDM] run "make.bat link" first or set GDB_ELF
    exit /b 1
)
if /i "%~2"=="batch" (
    "%GDB_EXE%" -q -batch "%GDB_ELF%" -x "%~dp0gdb_init.gdb"
) else (
    "%GDB_EXE%" -q "%GDB_ELF%" -x "%~dp0gdb_init.gdb"
)
goto end


::-------------------------------------------------------------------------------
:: default - forward everything to GNU make
::-------------------------------------------------------------------------------
:make
"%GMAKE%" -f "%MAKEFILE%" %*
if errorlevel 1 ( set "GMAKE_RC=1" ) else ( set "GMAKE_RC=0" )
goto end


::-------------------------------------------------------------------------------
:: helper: locate JLink.exe for direct flash
::-------------------------------------------------------------------------------
:locate_jlink_exe
set "JLINK_EXE="
if defined JLINK_PATH if exist "%JLINK_PATH%\JLink.exe" set "JLINK_EXE=%JLINK_PATH%\JLink.exe"
if not defined JLINK_EXE for /f "delims=" %%i in ('where JLink.exe 2^>nul') do if not defined JLINK_EXE set "JLINK_EXE=%%~fi"
if not defined JLINK_EXE for /d %%d in ("C:\Program Files\SEGGER\JLink*") do if exist "%%~d\JLink.exe" set "JLINK_EXE=%%~d\JLink.exe"
if not defined JLINK_EXE (
    echo [BDM] ERROR: JLink.exe not found.
    echo [BDM] install SEGGER J-Link or set JLINK_PATH to its installation folder
    exit /b 1
)
echo [BDM] using J-Link Commander: %JLINK_EXE%
exit /b 0


::-------------------------------------------------------------------------------
:: helper: detect if device CMS32M6736 exists in the installed J-Link database
::-------------------------------------------------------------------------------
:detect_jlink_flash_device
set "JLINK_FLASH_DEVICE=Cortex-M0+"
set "JLINK_DEVICE_DB="
for /d %%d in ("C:\Program Files\SEGGER\JLink*") do if exist "%%~d\JLinkDevices.xml" set "JLINK_DEVICE_DB=%%~d\JLinkDevices.xml"
if not defined JLINK_DEVICE_DB for /d %%d in ("C:\Program Files (x86)\SEGGER\JLink*") do if exist "%%~d\JLinkDevices.xml" set "JLINK_DEVICE_DB=%%~d\JLinkDevices.xml"
if defined JLINK_DEVICE_DB (
    findstr /I /C:"CMS32M6736" "%JLINK_DEVICE_DB%" >nul 2>&1
    if not errorlevel 1 set "JLINK_FLASH_DEVICE=CMS32M6736"
)
echo [BDM] using J-Link flash device: %JLINK_FLASH_DEVICE%
exit /b 0


::-------------------------------------------------------------------------------
:: helper: build a temporary J-Link flash script for the selected device
::-------------------------------------------------------------------------------
:write_jlink_flash_script
if exist "%JLINK_FLASH_SCRIPT%" del /f /q "%JLINK_FLASH_SCRIPT%" >nul 2>&1
> "%JLINK_FLASH_SCRIPT%" (
    echo device %JLINK_FLASH_DEVICE%
    echo if SWD
    echo speed 400
    echo erase
    echo loadfile .\out\BDM\bin\BDM.elf
    echo r
    echo g
    echo exit
)
if not exist "%JLINK_FLASH_SCRIPT%" (
    exit /b 1
)
exit /b 0


::-------------------------------------------------------------------------------
:: helper: locate JLinkGDBServerCL.exe
::-------------------------------------------------------------------------------
:locate_gdb_server
set "JLINK_GDB_SERVER="
if defined JLINK_PATH if exist "%JLINK_PATH%\JLinkGDBServerCL.exe" set "JLINK_GDB_SERVER=%JLINK_PATH%\JLinkGDBServerCL.exe"
if not defined JLINK_GDB_SERVER for /f "delims=" %%i in ('where JLinkGDBServerCL.exe 2^>nul') do if not defined JLINK_GDB_SERVER set "JLINK_GDB_SERVER=%%~fi"
if not defined JLINK_GDB_SERVER for /d %%d in ("C:\Program Files\SEGGER\JLink*") do if exist "%%~d\JLinkGDBServerCL.exe" set "JLINK_GDB_SERVER=%%~d\JLinkGDBServerCL.exe"
if not defined JLINK_GDB_SERVER (
    echo [BDM] ERROR: JLinkGDBServerCL.exe not found.
    echo [BDM] install SEGGER J-Link or set JLINK_PATH to its installation folder
    exit /b 1
)
echo [BDM] using J-Link GDB server: %JLINK_GDB_SERVER%
exit /b 0


:end_error
set "GMAKE_RC=1"
:end
popd
endlocal & exit /b %GMAKE_RC%
