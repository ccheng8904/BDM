# NVMEM component

This component creates upper layer for access to **N**on **V**olatile **MEM**ory.
Because access from/to such memory type is slower than controler's capability, component offers
cache service in RAM for quick access.  
Two kinds of **n**on **v**olatile **mem**ory type are supported:

- controller's internal flash memory (so called virtual EEPROM)
- external EEPROM memory

## 1 Virtual EEPROM

Virtual EEPROM is memory emulated in controller's flash. NVMEM cooperates with next two
components:  
- VEEPROM
- FLASH

### 1.1 Install

On form **"Framework setup utility"** select **"nvmem(virt)"** from common components list,
then click button **"Add"**.  
Component appears on list **"Added"** below. After click button **"Setup"** all necessary
components and setting will be installed.

![Version with emulated EEPROM](./doc/Images/NVMEM(virt).png "Version with emulated EEPROM")

> *Microcontrollers that don't contain data flash memory (STM32, MKV5X) need to allocate virtual EEPROM  work area in code flash part.*
  *Therefore, after you have successfully installed this component, check if "feature_veeprom = ENABLED" in your common_config.mak.*
  *For derivatives that contain data flash memory (Renesas RA2) let "feature_veeprom = DISABLED". *


### 1.2 Variants

There are two variants of virtual EEPROM :
- normal (short) version
- large version

#### 1.2.1 Normal version
Also called short version. Component uses for its work at least two memory flash pages, which are data exchanged in.  
For current data collection is used 1 memory flash page. When page is full of data and there is no more space to write  
new information, then after new write attempt latest valid data are tranferred to next available page.  
The transferring data to next available page takes some time so this 'new write' will take longer then other 'writes'.  
After data transfer to next page, the previous full page is erased and so stays ready for future data exchange.  
One byte is used for variables numbering, so in this version up to 255 project variables can be saved.

##### 1.2.1.1 Basic setting

To set this component version override next make variables in your custom MyVariant.mak:  
> veeprom_variant = veeprom  
nvmem_variant = nvmemveep

>Note: *This version is set as default after setup.*

##### 1.2.1.2 Other setting

In module *veepromMcXs.c* there is the array, where customer should define positions of pages.  
Two pages(minimum) are selected by default, more pages may be defined if available in controller's memory.

```c
uint8_t *VEEPROM_apucFlashPage[ ] =
{
    PointerToFirstPage,
    PointerToSecondPage
};
```
Be carefull and place pages to free memory, that contains neither customer code nor is CRC checked !  

> Note: Normal version is not available for NXP MKV5X, STM32G0, STM32G4, and STM32L4 controllers !

#### 1.2.2 Large version

Component uses for its work at least two so called memory "units", which are data exchanged in.  
Memory unit consists of 1 - **N** consecutive flash pages. **N** is by default 2.  
Parameter **N** may be redefined in *veepromMcXs.c* according to project resources.
```c
#define FLASH_PAGES_PER_UNIT  2
```
For current data collection is used 1 memory unit.  
When unit is full of data and there is no more space to write new information,
then after new write attempt latest valid data are tranfered to next available unit.  
After data transfer to next unit, the previous full unit is erased and so stays ready for future data exchange.  
Two bytes are used for variables numbering, so in this version up to 65535 project variables can be theoretically saved.  
The more variables plans customer to use, the bigger value should the number **N** have.

##### 1.2.2.1 Basic setting

To set this component version override next make variables in your custom MyVariant.mak:  
> veeprom_variant = veepromlarge  
nvmem_variant = nvmemveeplarge


##### 1.2.2.2 Other setting

In module *veepromMcXs.c* there is the array, where customer should define positions of units.  
Two units(minimum) are selected by default, more units may be defined if available in controller's memory.

```c
uint8_t *VEEPROM_apucUnitStart[ ] =
{
    PointerToFirstUnit,
    PointerToSecondUnit
};
```
Be carefull and place units to free memory, that contains neither customer code nor is CRC checked !  

### 1.3 Preparing custom variables

Customer needs to describe his variables that are to be used. Next array needs to be adapted in application's *cacheXS.c* module:

```c
const tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] =
{

   { &CACHE_aucCache[0], 1},
   { &CACHE_aucCache[1], 2},
   { &CACHE_aucCache[3], 3},
   { &CACHE_aucCache[6], 4}
}
```
First parameter means custom variable cache place definition in RAM, second parameter means custom variable size.  
Array size depends on customer variables count.

> The maximum of the custom variable size is 36 bytes for platforms: NXP MKV5X, STM32G0, STM32G4, STM32L4 and TI MSPM0.  
  The maximum of the custom variable size is 31 bytes for all other platforms.

To have access to array it is useful to define named indexes to it.  
Default named indexes are defined in application's cacheXS.h module.  
You need just rename these indexes that describe your variables :

```c
typedef enum
{
   /* ------------- VEEPROM partition table entries ---------------------*/
   MY_VARIABLE_1BYTE  =  0,
   MY_VARIABLE_2BYTE      ,
   MY_VARIABLE_3BYTE      ,
   MY_VARIABLE_4BYTE      ,
   CACHE_PLACE_COUNT
}tCacheLocationIdentifier;
```
For large component version enumeration item count must > 255.  
In case you have to use large component version, but your variable count is <= 255,  
you may to use next workaround :

```c
typedef enum
{
   /* ------------- VEEPROM partition table entries ---------------------*/
   MY_VARIABLE_1BYTE  =  0,
   MY_VARIABLE_2BYTE      ,
   MY_VARIABLE_3BYTE      ,
   MY_VARIABLE_4BYTE      ,
   CACHE_PLACE_COUNT      ,
   DUMMY_VARIABLE     = 256
}tCacheLocationIdentifier;
```
DUMMY_VARIABLE should not be used.

### 1.4 Work area manipulations

It is helpful to have possibility to either extract data from work area or patch predefined data into work area.  
Way how to do it you can find [**here**](https://wiki.bsh-sdd.com/display/PFS/Binary+output+manipulations "Binary output manipulations").  
There is also wiki page, which describes target defined by users see [**here**](https://wiki.bsh-sdd.com/display/PFS/How+to+use+Binary+Generator+make+targets "Binary generator make targets"). It should supersede the fixed names of target described by first wiki page.

## 2 External EEPROM

External EEPROM is usually connected to controller via I2C bus. NVMEM cooperates with next component:  
- exteeprom  


### 2.1 Install

On form **"Framework setup utility"** select **"nvmem(ext)"** from common components list,
then click button **"Add"**.  
Component appears on list **"Added"** below. After click button **"Setup"** all necessary
components and setting will be installed.

![Version with external EEPROM](./doc/Images/NVMEM(ext).png "Version with external EEPROM")

### 2.2 Variants

There are two variants of external EEPROM :
- normal (short) version
- large version

#### 2.2.1 Normal version

Also called short version. One byte is used for variables numbering, so in this version up to 255 project variables can be saved.

##### 2.2.1.1 Basic setting

To set this component version override next make variables in in  **`<MyVariant>.mak`**:  
> exteeprom_variant = exteeprom  
nvmem_variant = nvmemext

#### 2.2.2 Large version

Two bytes are used for variables numbering, so in this version up to 65535 project variables can be theoretically saved.

##### 2.2.2.1 Basic setting

To set this component version override next make variables in **`<MyVariant>.mak`**:  
> exteeprom_variant = exteepromlarge  
nvmem_variant = nvmemextlarge  

### 2.3 Communication settings

Two kinds of I2C communication drivers are supported:  
- software driven communication via IO pins (HI2C01 package), default setting
- hardware driven communication via internal I2C registers (HI2C02 package)  
  (at the moment available only for STM32F4 and Silicon Labs EFR32BG21)

#### 2.3.1 Software driven communication

To set this communication version override next make variable in **`<MyVariant>.mak`**:  
> hi2c_variant = used_swi2c

>Note: *This version is set as default after setup.*

In your **hi2c01xs.c** module you have to do :  
Set data(SDA), clock(SCL) and write protect(WP) pins according to connection with EEPROM chip.  

In your **ExtEEPlibXS.c** module you have to do :  
Select EEPROM chip size :
  - EEPROM_2_KBIT
  - EEPROM_4_KBIT
  - EEPROM_8_KBIT
  - EEPROM_16_KBIT
  - EEPROM_32_KBIT
  - EEPROM_64_KBIT

>  For other sizes you have to write your own function versions according to EEPROM chip communication protocol.  

Set EEPROM chip count available on i2c bus :
```c
#define CHIPS_COUNT  1
```

Adapt array EXTEEP_tChipInfo[] according to available chip count, connected on i2c bus. Up to 8 chips available for some types.  

#### 2.3.2 Hardware driven communication

To set this communication version override next make variable in your custom MyVariant.mak:  
> hi2c_variant = used_hwi2c

>Note: *After variable change you have to call "make init", as necessary files need to be copied to app folder*

In your **ExtEEPlibMcXS.c** module you have to do:
Select EEPROM chip size :
  - EEPROM_2_KBIT  
  - EEPROM_4_KBIT  
  - EEPROM_8_KBIT  

> At the moment only memories up to 8 kBits are supported

Select I2C hardware unit for communication for next functions:
  - EXTEEP_bRead()  
  - EXTEEP_bWrite()  
  - EXTEEP_tGetStatus()  
  - EXTEEP_ucHandleTask()  

Adapt parameters in EXTEEP_tChipForUnitX*[] according to real parameters of available EEPROM chip.  
Only 1 chip is supported.  
> *X - HW unit number

### 2.4 Preparing custom variables

Customer needs to describe his variables that are to be used.  
Next array needs to be adapted in application's **cacheXS.c** module:

#### 2.4.1 Manually defined EEPROM addresses
```c
const tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] =
{

   { &CACHE_aucCache[0], 1, 0 },
   { &CACHE_aucCache[1], 2, 1 },
   { &CACHE_aucCache[3], 3, 3 },
   { &CACHE_aucCache[6], 4, 6 }
}
```
First parameter means custom variable cache place definition in RAM.  
Second parameter means custom variable size.  
Third parameter means address in external EEPROM.  
Array size depends on customer variables count.  
In case third parameter are absolute numbers or constants, set next variables :
```c
const uint32_t CACHE_ulNegativeOffsetForEepAddress = 0UL;
const uint32_t CACHE_ulOffsetForEepAddress         = 0UL;
```
Third parameter is calculated in function CACHE_vModifyEEPROMaddress().

#### 2.4.2 Automatic calculated EEPROM addresses

There is second useful approach to define your variables :

```c
const tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] =
{

   { &CACHE_aucCache[0], 1, (uint32_t)&CACHE_aucCache[0] },
   { &CACHE_aucCache[1], 2, (uint32_t)&CACHE_aucCache[1] },
   { &CACHE_aucCache[3], 3, (uint32_t)&CACHE_aucCache[3] },
   { &CACHE_aucCache[6], 4, (uint32_t)&CACHE_aucCache[6] }
}
```
Set next variables :

```c
const uint32_t CACHE_ulNegativeOffsetForEepAddress = (uint32_t)CACHE_aucCache;
const uint32_t CACHE_ulOffsetForEepAddress         = 0UL;
```
Third parameter is calculated in function CACHE_vModifyEEPROMaddress().

#### 2.4.3 Next common settings

To have access to array it is useful to define named indexes to it.  
Default named indexes are defined in **cachepriv.h** module.  
To override it first add to your **`<MyVariant>.mak`** :

> defines += USE_CUSTOM_CACHE_LOCATION_IDENTIFIER

then add your specific index names, that describe your variables in application's **cacheXS.h** module:

```c
typedef enum
{
   /* ------------- VEEPROM partition table entries ---------------------*/
   MY_VARIABLE_1BYTE  =  0,
   MY_VARIABLE_2BYTE      ,
   MY_VARIABLE_3BYTE      ,
   MY_VARIABLE_4BYTE      ,
   CACHE_PLACE_COUNT
}tCacheLocationIdentifier;
```
For large component version enumeration item count must > 255.  
In case you have to use large component version, but your variable count is <= 255,  
you may to use next workaround :

```c
typedef enum
{
   /* ------------- VEEPROM partition table entries ---------------------*/
   MY_VARIABLE_1BYTE  =  0,
   MY_VARIABLE_2BYTE      ,
   MY_VARIABLE_3BYTE      ,
   MY_VARIABLE_4BYTE      ,
   CACHE_PLACE_COUNT      ,
   DUMMY_VARIABLE     = 256
}tCacheLocationIdentifier;
```
DUMMY_VARIABLE should not be used.

## 3 Basic component interface
Initialization:  
```c
   result = NVMEM_bInit();
```
or first call of
```c
   result = NVMEM_ucHandleTask();
```
for those, that support extended interface.  

Write:  
```c
   result = NVMEM_bWrite(MY_VARIABLE_1BYTE, &VariableToBeSaved);
```
Read:
```c
   result = NVMEM_bRead(MY_VARIABLE_3BYTE, &VariableToBeRead);  
```
>Note: *All these functions are blocking, after finish all their work is done.*

## 4 Extended component interface
All microcontrollers support NVMEM extended interface, NVMEM variant with external EEPROM.  
But only some of microcontrolers support NVMEM extended interface variant with virual EEPROM.  
Extended interface functions are:  
**Write process start**:  
```c
result = NVMEM_bWriteStart( MY_VARIABLE_1BYTE, &VariableToBeSaved );
```
>Note: *Rest of work must be done in task handler function*

**Task handler**:  
```c
result = NVMEM_ucHandleTask();
```
>Note: *Must be called in case NVMEM_bWriteStart() function is used for writing.*

**Flush** unsaved variables from cache places:
```c
result = NVMEM_bFlushBuffer();
```
>Note: *Function is useful before planned controler's switch off or restart*

**Status functions:**  
Returns number of unsaved cache variables:
```c
unsaved = NVMEM_uiGetUnsavedCount();
```
Returns component current status:
```c
status = NVMEM_tGetStatus();
```
Checks if variable is waiting for save:
```c
saveStatus = NVMEM_bIsSavePending(MY_VARIABLE_1BYTE);
```
Sets data **flush behavior**:
```c
NVMEM_vFlushDefragEnabled(true);
```
