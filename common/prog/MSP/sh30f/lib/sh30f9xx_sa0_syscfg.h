/**
  ******************************************************************************
  * @file    sh30f9xx_sa0_syscfg.h
  * @author  sinowealth
  * @version V1.1.0
  * @date    2020-12-16  
  * @brief   This file contains all the functions prototypes for the SYSCFG firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * SINOWEALTH IS SUPPLYING THIS SOFTWARE FOR USE EXCLUSIVELY SH_ON SINOWEALTH'S 
  * MICROCONTROLLER PRODUCTS. IT IS PROTECTED UNDER APPLICABLE COPYRIGHT LAWS. 
  * THIS SOFTWARE IS FOR GUIDANCE IN ORDER TO SAVE TIME. AS A RESULT, SINOWEALTH 
  * SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES 
  * WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR
  * THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN 
  * CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2020 Sinowealth</center></h2>
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SH30F9XX_SA0_SYSCFG_H
#define __SH30F9XX_SA0_SYSCFG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "sh30f9xx_sa0_libcfg.h"

/** @addtogroup SH30F9xx_sa0_libcfg_StdLib_Driver
  * @{
  */

/** @addtogroup SYSCFG_MODULE
  * @{
  */

    /* Exported types ------------------------------------------------------------*/
    /* Exported constants --------------------------------------------------------*/

/** @defgroup SYSCFG_Exported_Constants
  * @{
  */

typedef enum
{
    SYSCFG_VLVR_Level_4V1 = 0x00,
    SYSCFG_VLVR_Level_3V7 = 0x01,
    SYSCFG_VLVR_Level_2V8 = 0x02,
    SYSCFG_VLVR_Level_2V3 = 0x03,
}SYSCFG_VLVR_LEVEL_Type;    

#define IS_SYSCFG_VLVR_LEVEL(LEVEL)           ((LEVEL == SYSCFG_VLVR_Level_4V1) \
                                              || (LEVEL == SYSCFG_VLVR_Level_3V7) \
                                              || (LEVEL == SYSCFG_VLVR_Level_2V8) \
                                              || (LEVEL == SYSCFG_VLVR_Level_2V3))

typedef enum
{
    SYSCFG_BOD_Mode_Rising = 0x00,
    SYSCFG_BOD_Mode_Falling = 0x01,
    SYSCFG_BOD_Mode_RisingFalling = 0x02,
}SYSCFGBODMode_Type;    

#define IS_SYSCFG_BOD_MODE(MODE)              ((MODE == SYSCFG_BOD_Mode_Rising) \
                                              || (MODE == SYSCFG_BOD_Mode_Falling) \
                                              || (MODE == SYSCFG_BOD_Mode_RisingFalling))

typedef enum
{
    SYSCFG_BOD_Level_2V40 = 0x00,
    SYSCFG_BOD_Level_2V55 = 0x01,
    SYSCFG_BOD_Level_2V70 = 0x02,
    SYSCFG_BOD_Level_2V85 = 0x03,
    SYSCFG_BOD_Level_3V00 = 0x04,
    SYSCFG_BOD_Level_3V15 = 0x05,
    SYSCFG_BOD_Level_3V30 = 0x06,
    SYSCFG_BOD_Level_3V45 = 0x07,
    SYSCFG_BOD_Level_3V60 = 0x08,
    SYSCFG_BOD_Level_3V75 = 0x09,
    SYSCFG_BOD_Level_3V90 = 0x0A,
    SYSCFG_BOD_Level_4V05 = 0x0B,
    SYSCFG_BOD_Level_4V20 = 0x0C,
    SYSCFG_BOD_Level_4V35 = 0x0D,
    SYSCFG_BOD_Level_4V50 = 0x0E,
    SYSCFG_BOD_Level_4V65 = 0x0F,
}SYSCFGBODLevel_Type;    

#define IS_SYSCFG_BOD_LEVEL(BOD_LEVEL)        ((BOD_LEVEL == SYSCFG_BOD_Level_2V40) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_2V55) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_2V70) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_2V85) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_3V00) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_3V15) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_3V30) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_3V45) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_3V60) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_3V75) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_3V90) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_4V05) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_4V20) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_4V35) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_4V50) \
                                              || (BOD_LEVEL == SYSCFG_BOD_Level_4V65)) 

#define SYSCFG_BOD_FLAG_BODIF                 (uint8_t)0x01
#define SYSCFG_BOD_FLAG_BODF                  (uint8_t)0x02

#define IS_SYSCFG_BOD_FLAG(FLAG)              ((FLAG == SYSCFG_BOD_FLAG_BODIF) \
                                              || (FLAG == SYSCFG_BOD_FLAG_BODF))
                                   
#define SYSCFG_SLEEPEntry_WFI                 (uint8_t)0x00
#define SYSCFG_SLEEPEntry_WFE                 (uint8_t)0x01

#define IS_SYSCFG_SLEEP_ENTRY(ENTRY)          ((ENTRY == SYSCFG_SLEEPEntry_WFI) \
                                              || (ENTRY == SYSCFG_SLEEPEntry_WFE))

#define SYSCFG_STOPEntry_WFI                  (uint8_t)0x00
#define SYSCFG_STOPEntry_WFE                  (uint8_t)0x01

#define IS_SYSCFG_SLOP_ENTRY(ENTRY)           ((ENTRY == SYSCFG_STOPEntry_WFI) \
                                              || (ENTRY == SYSCFG_STOPEntry_WFE))

typedef enum
{
    SYSCFG_SWJ_All_Function = 0x00,
    SYSCFG_SWD_CLOSE_PinRemap = 0x01,
}SYSCFG_SWJCFG_Type;

#define IS_SYSCFG_SWJ_PIN_MODE(MODE)          ((MODE == SYSCFG_SWJ_All_Function) \
                                              || (MODE == SYSCFG_SWD_CLOSE_PinRemap))

typedef enum
{
    SYSCFG_OSC_GPIO = 0x00,
    SYSCFG_OSC_HCrystal = 0x01,
    SYSCFG_OSC_ExtClock = 0x02,
    SYSCFG_OSC_LCrystal = 0x03,   //32.768Khz
}SYSCFG_OSCPin_Type;

#define IS_SYSCFG_OSC_PIN_MODE(MODE)          ((MODE == SYSCFG_OSC_GPIO) \
                                              || (MODE == SYSCFG_OSC_HCrystal) \
                                              || (MODE == SYSCFG_OSC_ExtClock) \
                                              || (MODE == SYSCFG_OSC_LCrystal))
      
#define SYSCFG_NMI_TRIGGER_CSM                (uint16_t)0x0100
#define SYSCFG_NMI_TRIGGER_BOD                (uint16_t)0x0080
#define SYSCFG_NMI_TRIGGER_EXTI0              (uint16_t)0x0040

#define IS_SYSCFG_NMI_TRIGGER_SOURCE(SOURCE)  (((SOURCE & (uint16_t)0xFE3F) == 0x00) \
                                              && (SOURCE != 0x00))

#define SRAM_Sector_0                         (uint16_t)0x0001
#define SRAM_Sector_1                         (uint16_t)0x0002
#define SRAM_Sector_2                         (uint16_t)0x0004
#define SRAM_Sector_3                         (uint16_t)0x0008
#define SRAM_Sector_4                         (uint16_t)0x0010
#define SRAM_Sector_5                         (uint16_t)0x0020
#define SRAM_Sector_6                         (uint16_t)0x0040
#define SRAM_Sector_7                         (uint16_t)0x0080
#define SRAM_Sector_8                         (uint16_t)0x0100
#define SRAM_Sector_9                         (uint16_t)0x0200
#define SRAM_Sector_10                        (uint16_t)0x0400
#define SRAM_Sector_11                        (uint16_t)0x0800
#define SRAM_Sector_12                        (uint16_t)0x1000
#define SRAM_Sector_13                        (uint16_t)0x2000
#define SRAM_Sector_14                        (uint16_t)0x4000
#define SRAM_Sector_15                        (uint16_t)0x8000

#define IS_SYSCFG_SRAM_SECTOR(LOCK_SECTOR)    (LOCK_SECTOR != 0x0000)

#define DBG_Periph_HLV                        (uint16_t)0x0004
#define DBG_Periph_LCD                        (uint16_t)0x0008
#define DBG_Periph_LED                        (uint16_t)0x0010
#define DBG_Periph_DMA                        (uint16_t)0x0020
#define DBG_Periph_IWDT                       (uint16_t)0x0040
#define DBG_Periph_WWDT                       (uint16_t)0x0080
#define DBG_Periph_PCA                        (uint16_t)0x0100
#define DBG_Periph_TIM                        (uint16_t)0x0200
#define DBG_Periph_PWM                        (uint16_t)0x0400
#define DBG_Periph_UART                       (uint16_t)0x0800
#define DBG_Periph_SPI                        (uint16_t)0x1000
#define DBG_Periph_TWI                        (uint16_t)0x2000

#define IS_DBG_PERIPH(PERIPH)                 (((PERIPH & (uint16_t)0xC003) == 0x00) \
                                              && (PERIPH != 0x00))

#define DBG_LOWPOWER_SLEEP                    (uint16_t)0x0001
#define DBG_LOWPOWER_STOP                     (uint16_t)0x0002
#define IS_DBG_LOWPOWER_MODE(MODE)            (((MODE & (uint16_t)0xFFFC) == 0x00) \
                                              && (MODE != 0x00))

#define GPIOB_MODULE_CLOCK_IN_SCIOB           (uint8_t)0x00
#define GPIOB_MODULE_CLOCK_IN_AHB             (uint8_t)0x01
#define IS_GPIOB_CLOCK_SOURCE(SRC)            ((SRC == 0x00) || (SRC == 0x01))

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/* Initialize BOD mode and voltage threshold*/
void SYSCFG_BODInit(SYSCFGBODMode_Type SYSCFG_BOD_Mode, SYSCFGBODLevel_Type SYSCFG_BOD_Level);  

/* Enable or disable the specified BOD interrupt*/
void SYSCFG_BODINTConfig(FunctionalState NewState);

/* Initialize LVR voltage threshold*/
void SYSCFG_VLVRInit(SYSCFG_VLVR_LEVEL_Type SYSCFG_VLVR_Level);

/* Enable or disable the specified LVR function*/
void SYSCFG_VLVRConfig(FunctionalState NewState);

/* Open or close BOD module*/
void SYSCFG_BODOnOff(CmdState OnOffState);

/* Check whether the specified BOD flag is set or not*/
FlagStatus SYSCFG_BODGetFlagStatus(uint8_t BOD_Flag);

/* Clear BOD pending bit*/
void SYSCFG_BODIntClearFlag(void);

/* Enters SLEEP mode*/
void SYSCFG_EnterSLEEPMode(uint8_t SYSCFG_SLEEPEntry);

/* Enters STOP mode*/
void SYSCFG_EnterSTOPMode(uint8_t SYSCFG_STOPEntry);

/* Configure NMI interrupt trigger source*/
void SYSCFG_NMIInterruptTriggerConfig(uint16_t TriggerSource, FunctionalState NewState);

/* Specifie SWJ pin alternate function*/
void SYSCFG_SWJPinConfig(SYSCFG_SWJCFG_Type SWJPin_Mode);

/* Specifie XTAL pin And XTALX alternate*/
void SYSCFG_OSCPinConfig(SYSCFG_OSCPin_Type OSCPin_Mode);

/* Lock or unlock SRAM sectors*/
void SYSCFG_SRAMLockConfig(uint8_t SectorNum, FunctionalState NewState);

/* Enable or disable peripherals clock in debug mode*/
void SYSCFG_DBGPeriphConfig(uint16_t DBG_Periph, FunctionalState NewState);

/* Enable or disable low power clock in debug mode*/
void SYSCFG_DBGLowPowerConfig(uint16_t LowPowerMode, FunctionalState NewState);

/* GPIO module mount bus configuration register*/
void SYSCFG_GPIOBCRConfig(uint8_t GPIOB_Clock_Source);
#ifdef __cplusplus
}
#endif

#endif /* __SH30F9XX_SA0_SYSCFG_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Sinowealth *****END OF FILE****/
