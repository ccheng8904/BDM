# Firmware Compatibility Check (FCC)

# 1 Motivation
The former implemented Boot Manager's (BM's) comparison of **HW IDs** ensures the compatibility of the downloaded software with the electronic hardware.<br>
Nevertheless the compatibility with external components or sensors can not be guaranteed.<br>
For this reason the existing comparison of **HW ID** in BM is replaced with comparison of **FW IDs**.

NOTE: FW ID mentioned in this document is basically SW ID used to identify application software. 

# 2 Concept description

In general, there are two phases / states of user application protection:
- **Phase 1 = Unprotected phase** of the ECU → FW ID is not stored in the production data area(OTP) → as long FW IDs are not written into production data area on microcontroller, the check of the FW IDs will be not performed.
- **Phase 2 = Protected phase** of the ECU → FW IDs is stored in the production data area → BM checks FW ID compatibility during startup.

In protected phase during each startup the BM shall compare the FW IDs stored in the production data area and FW IDs of the application (and other user modules) flashed in microcontroller. (Similar to previously implemented HW ID comparison).
- In case when the FW IDs in the production data area and the FW IDs of applications are **equal** the BM shall **start** the application.
- In case when the FW IDs in the production data area and the FW IDs of applications are **not equal** the BM may **not start** the application.
    + In this case the Programmer will be started as a backup solution. It can be used to re-flash faulty application.

# 3 Implementation notes
- FCC feature is available only for remotely updateable firmware (FWU3)
- The usage of FW ID comparison **feature is optional** for the projects. The project can disable the feature if required.<br>
- Once the protected state is enabled, there is no (remote) way to switch back to unprotected state! Full chip erase is required to erase OTP.<br>
- FW IDs are stored in OTP memory of MCU Framework and is protected by CRC.<br>
- OTP memory is capable to store **up to 10 FW IDs**.
- FCC Feature is completely utilized only in [RELEASE variant] of user application.
    + in DEVELOP variant OTP memory is pre-filled already in binary file, write process is NOT executed during application startup. However, BM still executes FCC check before startup of application.

# 4 Startup sequence
Detailed BM startup sequence is described in [BM documentation].<br>
Below we describe several scenarios for Firmware Compatibility check. We assume that user application [is build as RELEASE variant].
## 4.1 Application in unprotected state (FCC disabled)
As already described above, the BM does not check FW IDs in this case, user application is started. During application startup **no FW_IDs are written** into OTP memory.

## 4.2 Application in protected state (FCC Enabled) - first startup
During first startup (after flashing) of user application the OTP memory is empty. 
- BM starts user application without FW ID compatibility check.
- During user application startup all available FW IDs are copied from user module headers into OTP memory.

## 4.3 Application in protected state (FCC Enabled) - second startup
OTP memory already includes FW IDs of available user modules (applications, configuration files, data modules).
- BM executes CRC check over FW IDs stored in OTP memory.
- BM executes FW ID compatibility check (comparison of FW IDs from OTP memory and from module headers).
- In case of FW ID match the BM starts user application.
- User application does NOT write anything to OTP memory, as OTP memory is already filled with valid FW IDs.

# 5 FCC configuration
## Activating / Deactivating FCC feature
**FCC is enabled by default** in MCU Framework on all application modules.<br>
You can disable this feature selectively on each module by adding module attribute MAL_CTX_MF_NO_COMPATIBILITY_CHECK into module header. (module_administration_cfg.c)
```c
const struct MAL_partition_s MAL_partitionFix[MAL_FIX_PARTITION_ENTRIES] =
{
...
...
    // Application
    {
        .mod_name            = "Application",
        .physical_start      = {.addr = FWU_APP1_START_ADDRESS},
        .physical_end        = {.addr = FWU_APP1_END_ADDRESS},
        .mod_number          = MAL_PRODUCT_APP1,
        .driver_id           = MAL_IntFlashDriver,
        .mod_attributes      = MAL_CTX_MF_RWEX | MAL_CTX_MF_NO_COMPATIBILITY_CHECK,
        .dep_module          = MAL_NO_MODULE
    }
```

## Writing FW_IDs into OTP memory
This process is done automatically during application startup. See basicXS.c:
```c
void BASIC_vUsrInitAppl (void)
{
    /* Do any pre initialization here: ...*/
#if defined(REMOTE_FIRMWARE_UPDATE)
    (void)MAL_writeFirmwareIDs();
#endif
```
<br>

Architecture concept available at [following link](https://wiki.bsh-sdd.com/display/EPACTFS/FW+ID+concept)<br>
[Home page of help](../../../README.md)

   [BM documentation]: <../BootManager/doc>
   [is build as RELEASE variant]: <../../../build/help/make_targets.md>
   [RELEASE variant]: <../../../build/help/make_targets.md>
   