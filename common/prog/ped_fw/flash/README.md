# FLASH component


## 1. Introduction
 The Flash component provides user application with the routines to do flash operations like write or erase on the flash memory.
 
## 2. Basic interface of Flash component
 The main interface comprises following functions:
 
- `FLASH_bEnableAccess()`
- `FLASH_bDisableAccess()`
- `FLASH_bIsAccessEnabled()`
- `FLASH_ucGetCellSize()`
- `FLASH_bWrite()`
- `FLASH_bWriteByte() *`
- `FLASH_bRead()`
- `FLASH_bErase()`
- `FLASH_tGetFunctionStatus()`
 
> \* Function not supported for derivatives with flash granularity > 2

 In order to give the user application a possibility to control the flash operations, following
 component interfaces are defined in the user configuration file FlashMcXS.c :
 
- `_FLASH_vResetWatchdogTimer()`

    Here can user place code to reset the watchdog timer.

- `_FLASH_vOperationStart()`

    Called immediately before a flash operation starts.

- `_FLASH_vOperationFinished()`

    Called immediately after a flash operation is finished.
 
>Note: The user should adapt the implementations behind these interfaces if not suitable for his needs.
 
## 3. Extended interface of Flash component

- [NXP mke14z](cfg_templates/mke14z/README.md)
- [NXP mkv5x](cfg_templates/mkv5x/README.md)
- [ST stm32f0](cfg_templates/stm32f0/README.md)
- [ST stm32f1](cfg_templates/stm32f1/README.md)
- [ST stm32f3](cfg_templates/stm32f3/README.md)
- [ST stm32f4](cfg_templates/stm32f4/README.md)
- [ST stm32g0](cfg_templates/stm32g0/README.md)
- [ST stm32g4](cfg_templates/stm32g4/README.md)
- [ST stm32l4](cfg_templates/stm32l4/README.md)

## 4. Usage of Flash component

For effective use of the flash component, the following must be done in the order:

1. Before accessing flash for "erase" or "write", the access must also be enabled using **FLASH_bEnableAccess()**.
2. Once the steps above are completed, **FLASH_bWrite()** or **FLASH_bWriteByte()** can be used for writing,  
   **FLASH_bRead()** for reading and **FLASH_bErase()** for erasing.
3. When flash operation is finished, access must be disabled using **FLASH_bDisableAccess()**.

> - Flash Programming can be done sequentially according to CPU architecture (for example bytewise).
> - Flash Erasing always concerns a whole flash page.
> - It is advisable to check the return values of each function before calling the next function.
> - To set the flash page pointer to a valid address according to page alignment, **FLASH_ucGetCellSize()**
      retrieves the correct page alignment (e.g. bytewise, wordwise etc.). This is important for overlying
      application handling with flash pages in a loop.  
 
## 5. Examples of Flash component routines usage

### 5.1. Lock or unlock flash operation

To lock flash routines and therefore preventing from random flass access (e.g: through **EMC**):

 ```c
   result = FLASH_bDisableAccess();
```
 
> Flash operations are per default locked at system initialization.


To unlock flash routine in order to enable flash operations:

```c
result = FLASH_bEnableAccess();
```

### 5.2. Program Flash

Following procedure can be used to program flash:

```c
result = FLASH_bEnableAccess();
result = result && FLASH_bWrite(address, buffer, length);
result = result && FLASH_bDisableAccess();
```

### 5.3. Erase Flash

Following procedure can be used to erase flash:

```c
result = FLASH_bEnableAccess();
result = result && FLASH_bErase(address);
result = result && FLASH_bDisableAccess();
```
