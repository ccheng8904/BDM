/**
  ******************************************************************************
  * @file    sh30f9xx_sa0_gpio.h
  * @author  sinowealth
  * @version V1.1.0
  * @date    2021-6-10  
  * @brief   This file contains all the functions prototypes for the GPIO firmware
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
  * <h2><center>&copy; COPYRIGHT 2021 Sinowealth</center></h2>
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __SH30F9XX_SA0_GPIO_H
#define  __SH30F9XX_SA0_GPIO_H

#ifdef  __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "sh30f9xx_sa0_libcfg.h"

/** @addtogroup SH30F9xx_sa0_libcfg_StdLib_Driver
  * @{
  */

/** @addtogroup GPIO_MODULE
  * @{
  */
/** @defgroup GPIO_Config_Parameter
  * @{
  */
#define GPIO_MODER_MODER0_Msk          (0x1UL /*<< GPIO_MODER_MODER_Pos*/)        /*!<GPIO MODER: MODER0 Mask     */
#define GPIO_ODRVR_ODRVR_SINK_Pos      (0x1UL /*<< GPIO_ODRVR_ODRVR_Pos*/)        /*!<GPIO_CFG ODRVR: ODRVR0 Mask */
#define GPIO_OTYPER_OT_Msk             (0x1UL /*<< GPIO_OTYPER_OT_Pos*/)          /*!<GPIOA OTYPER: OT Mask       */
#define GPIO_AFRL_AFR0_Msk             (0xFUL /*<< GPIO_AFRL_AFR0_Pos*/)          /*!<GPIO_CFG AFRL: AFR0 Mask    */
#define GPIO_AFRH_AFR8_Msk             (0xFUL /*<< GPIO_AFRH_AFR8_Pos*/)          /*!<GPIO_CFG AFRH: AFR8 Mask    */
#define GPIO_PUPDR_PHDR0_Msk           (0x1UL /*<< GPIOD_PUPDR_PHDR0_Pos*/)       /*!<GPIOD PUPDR: PHDR0 Mask     */

/** 
  * @brief  GPIO Configuration Mode enumeration 
  */
typedef enum
{ 
    GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode */
    GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode */
}GPIOMode_TypeDef;
#define IS_GPIO_MODE(MODE)             (((MODE) == GPIO_Mode_IN) || ((MODE) == GPIO_Mode_OUT))

/** 
  * @brief  GPIO Output type enumeration 
  */
typedef enum
{ 
    GPIO_OType_PP = 0x00,
    GPIO_OType_OD = 0x01
}GPIOOType_TypeDef;
#define IS_GPIO_OTYPE(OTYPE)           (((OTYPE) == GPIO_OType_PP) || ((OTYPE) == GPIO_OType_OD))

/** 
  * @brief  GPIO Output driving ability enumeration 
  */  
typedef enum
{ 
    GPIO_ODrv_NORMAL     = 0x00,   /*!< weak driver */
    GPIO_ODrv_VERYSTRONG = 0x01  /*!< strong driver */
}GPIOODrv_TypeDef;
#define IS_GPIO_ODRV(ODRV)             (((ODRV) == GPIO_ODrv_NORMAL) || ((ODRV) == GPIO_ODrv_VERYSTRONG)) 

/** 
  * @brief  GPIO Configuration PullUp PullDown enumeration 
  */ 
typedef enum
{ 
    GPIO_PuPd_NOPULL = 0x00,
    GPIO_PuPd_UP     = 0x01
}GPIOPuPd_TypeDef;

#define IS_GPIO_PUPD(PUPD)             (((PUPD) == GPIO_PuPd_NOPULL) || ((PUPD) == GPIO_PuPd_UP))

/** 
  * @brief  GPIO Bit SH_SET and Bit SH_RESET enumeration 
  */ 
typedef enum
{ 
    Bit_RESET = 0,
    Bit_SET
}BitAction;  

#define IS_GPIO_BIT_ACTION(BIT_ACTION) (((BIT_ACTION) == Bit_RESET) || ((BIT_ACTION) == Bit_SET))

/** 
  * @brief   GPIO Init structure definition  
  */
typedef struct
{
    uint32_t GPIO_Pin;              /*!< Specifies the GPIO pins to be configured.
                                       This parameter can be any value of @ref GPIO_pins_define */

    GPIOMode_TypeDef GPIO_Mode;     /*!< Specifies the operating mode for the selected pins.
                                       This parameter can be a value of @ref GPIOMode_TypeDef */

    GPIOODrv_TypeDef GPIO_ODrv;   /*!< Specifies the speed for the selected pins.
                                       This parameter can be a value of @ref GPIOODrv_TypeDef */

    GPIOOType_TypeDef GPIO_OType;   /*!< Specifies the operating output type for the selected pins.
                                       This parameter can be a value of @ref GPIOOType_TypeDef */

    GPIOPuPd_TypeDef GPIO_PuPd;     /*!< Specifies the operating Pull-up/Pull down for the selected pins.
                                       This parameter can be a value of @ref GPIOPuPd_TypeDef */
}GPIO_InitTypeDef;  


#define IS_GPIO_ALL_PERIPH(PERIPH)     (((PERIPH) == GPIOA) || ((PERIPH) == GPIOB) \
                                       || ((PERIPH) == GPIOC) || ((PERIPH) == GPIOD))
/**
  * @}
  */

/** @defgroup GPIO_Bit_Lock
  * @{
  */

#define GPIO_BitLock_Disable           (uint8_t)0x00
#define GPIO_BitLock_Enable            (uint8_t)0x01
#define IS_GPIO_BIT_LOCK(LOCK)         ((LOCK == GPIO_BitLock_Disable) || (LOCK == GPIO_BitLock_Enable))

/**
  * @}
  */

/** @defgroup GPIO_Bit_Level
  * @{
  */

#define GPIO_BitLevel_CMOS             (uint8_t)0x00
#define GPIO_BitLevel_TTL              (uint8_t)0x01
#define IS_GPIO_BIT_LEVEL(LEVEL)       ((LEVEL == GPIO_BitLevel_CMOS) || (LEVEL == GPIO_BitLevel_TTL))

/**
  * @}
  */

/** @defgroup GPIO_pins_define
  * @{
  */

#define GPIO_Pin_0                     ((uint16_t)0x0001)   /* Pin 0 selected */
#define GPIO_Pin_1                     ((uint16_t)0x0002)   /* Pin 1 selected */
#define GPIO_Pin_2                     ((uint16_t)0x0004)   /* Pin 2 selected */
#define GPIO_Pin_3                     ((uint16_t)0x0008)   /* Pin 3 selected */
#define GPIO_Pin_4                     ((uint16_t)0x0010)   /* Pin 4 selected */
#define GPIO_Pin_5                     ((uint16_t)0x0020)   /* Pin 5 selected */
#define GPIO_Pin_6                     ((uint16_t)0x0040)   /* Pin 6 selected */
#define GPIO_Pin_7                     ((uint16_t)0x0080)   /* Pin 7 selected */
#define GPIO_Pin_8                     ((uint16_t)0x0100)   /* Pin 8 selected */
#define GPIO_Pin_9                     ((uint16_t)0x0200)   /* Pin 9 selected */
#define GPIO_Pin_10                    ((uint16_t)0x0400)   /* Pin 10 selected */
#define GPIO_Pin_11                    ((uint16_t)0x0800)   /* Pin 11 selected */
#define GPIO_Pin_12                    ((uint16_t)0x1000)   /* Pin 12 selected */
#define GPIO_Pin_13                    ((uint16_t)0x2000)   /* Pin 13 selected */
#define GPIO_Pin_14                    ((uint16_t)0x4000)   /* Pin 14 selected */
#define GPIO_Pin_15                    ((uint16_t)0x8000)   /* Pin 15 selected */
#define GPIO_Pin_All                   ((uint16_t)0xFFFF)   /* All pins selected */

#define IS_GPIO_PIN(PIN)               ((((PIN) & (uint16_t)0x00) == 0x00) && ((PIN) != (uint16_t)0x00))

#define  IS_GET_GPIO_PIN(PIN)        (((PIN) == GPIO_Pin_0)  || ((PIN) == GPIO_Pin_1)    \
                                     || ((PIN) == GPIO_Pin_2)  || ((PIN) == GPIO_Pin_3)  \
                                     || ((PIN) == GPIO_Pin_4)  || ((PIN) == GPIO_Pin_5)  \
                                     || ((PIN) == GPIO_Pin_6)  || ((PIN) == GPIO_Pin_7)  \
                                     || ((PIN) == GPIO_Pin_8)  || ((PIN) == GPIO_Pin_9)  \
                                     || ((PIN) == GPIO_Pin_10) || ((PIN) == GPIO_Pin_11) \
                                     || ((PIN) == GPIO_Pin_12) || ((PIN) == GPIO_Pin_13) \
                                     || ((PIN) == GPIO_Pin_14) || ((PIN) == GPIO_Pin_15))


#define  IS_GPIO_AF(AF)   ((AF) <= 10)

/**
  * @}
  */

/** @defgroup GPIO_Alternat_function_selection_define 
  * @{
  */ 
 


typedef enum
{
    PXX_AF1   = 1, PXX_AF2,        PXX_AF3,     PXX_AF4,     PXX_AF5,    PXX_AF6,       PXX_AF7,     PXX_AF8,     PXX_AF9,  PXX_AF10,
    PA0_GPIO  = 1, PA0_AN0,        PA0_VREF,    PA0_PCA0A,   PA0_T0,                                                                    
    PA1_GPIO  = 1, PA1_AN1,        PA1_PCA0B,   PA1_T1,      PA1_FLT2,                                                                  
    PA2_GPIO  = 1, PA2_AN2,        PA2_PCA3B,   PA2_T2,      PA2_FLT1,                                                                  
    PA3_GPIO  = 1, PA3_T3,                                                                                                              
    PA4_GPIO  = 1, PA4_AN3,        PA4_PCA0ECI, PA4_T0,      PA4_PWM1A,   PA4_SCL0,      PA4_SDA0,                                      
    PA5_GPIO  = 1, PA5_PCA3ECI,    PA5_PWM1B,   PA5_SCL0,    PA5_SDA0,                                                                  
    PA6_GPIO  = 1, PA6_PCA3A,      PA6_FLT0,                                                                                            
    PA7_GPIO  = 1, PA7_PWM0A,      PA7_GSEL,                                                                                            
    PA8_GPIO  = 1, PA8_PWM0B,      PA8_GSDA,                                                                                            
    PA9_GPIO  = 1, PA9_PCA3A,      PA9_PCA3ECI, PA9_TXD1,    PA9_RXD1,    PA9_GSCK,                                                     
    PA10_GPIO = 1, PA10_PCA3B,     PA10_TXD1,   PA10_RXD1,                                                                              
    PA11_GPIO = 1, PA11_AN17,      PA11_T1,                                                                                             
    PA12_GPIO = 1, PA12_AN18,      PA12_T2,                                                                                             
    PA13_GPIO = 1, PA13_AN19,      PA13_COM0,   PA13_PWM3A,  PA13_SCL0,                                                                 
    PA14_GPIO = 1, PA14_AN20,      PA14_COM1,   PA14_PWM3B,  PA14_SDA0,                                                                 
    PA15_GPIO = 1, PA15_COM2,      PA15_PWM2A,  PA15_TXD0,   PA15_RXD0,                                                                 
    PB0_GPIO  = 1, PB0_COM3,       PB0_PWM2B,   PB0_TXD0,    PB0_RXD0,    PB0_GSCK,                                                     
    PB1_GPIO  = 1, PB1_COM4,       PB1_PCA1A,   PB1_T3,      PB1_FLT2,    PB1_GSDA,                                                     
    PB2_GPIO  = 1, PB2_LED_C0,     PB2_LED_S0,  PB2_COM5,    PB2_T0,      PB2_TXD3,      PB2_GSEL,                                      
    PB3_GPIO  = 1, PB3_LED_C1,     PB3_LED_S1,  PB3_COM6,    PB3_T1,      PB3_RXD3,      PB3_SS1,                                       
    PB4_GPIO  = 1, PB4_LED_C2,     PB4_LED_S2,  PB4_COM7,    PB4_PCA1A,   PB4_SCK1,      PB4_SCL0,                                      
    PB5_GPIO  = 1, PB5_LED_C3,     PB5_LED_S3,  PB5_SEG0,    PB5_PCA1B,   PB5_MOSI1,     PB5_SDA0,     PB5_TK0,                         
    PB6_GPIO  = 1, PB6_LED_C4,     PB6_LED_S4,  PB6_SEG1,    PB6_PCA0A,   PB6_PCA1ECI,   PB6_MISO1,    PB6_TK1,                         
    PB7_GPIO  = 1, PB7_AN4,        PB7_LED_C5,  PB7_LED_S5,  PB7_SEG2,    PB7_PCA0B,     PB7_PCA0ECI,  PB7_MISO0,  PB7_SS0,   PB7_TK2,  
    PB8_GPIO  = 1, PB8_AN5,        PB8_LED_C6,  PB8_LED_S6,  PB8_SEG3,    PB8_PCA0B,     PB8_PCA1ECI,  PB8_MOSI0,  PB8_SDA0,  PB8_TK3,  
    PB9_GPIO  = 1, PB9_LED_C7,     PB9_LED_S7,  PB9_SEG4,    PB9_PCA1B,   PB9_FLT0,      PB9_SCK0,     PB9_SCL0,   PB9_TK4,             
    PB10_GPIO = 1, PB10_LED_S8,    PB10_SEG5,   PB10_T2,     PB10_SS0,    PB10_TK5,                                                     
    PB11_GPIO = 1, PB11_LED_S9,    PB11_SEG6,   PB11_T3,     PB11_SS1,    PB11_TK6,                                                     
    PB12_GPIO = 1, PB12_LED_S10,   PB12_SEG7,   PB12_PWM0A,  PB12_TXD2,   PB12_SCK1,     PB12_MISO0,   PB12_SCL0,  PB12_SDA0, PB12_TK7, 
    PB13_GPIO = 1, PB13_LED_S11,   PB13_SEG8,   PB13_PWM0B,  PB13_RXD2,   PB13_MOSI1,    PB13_MOSI0,   PB13_SCL0,  PB13_SDA0, PB13_TK8, 
    PB14_GPIO = 1, PB14_LED_S12,   PB14_SEG9,   PB14_PWM0A,  PB14_TXD2,   PB14_MISO1,    PB14_TK9,                                      
    PB15_GPIO = 1, PB15_LED_S13,   PB15_SEG10,  PB15_T3,     PB15_SS1,    PB15_TK10,                                                    
    PC0_GPIO  = 1, PC0_LED_S14,    PC0_SEG11,   PC0_T2,      PC0_FLT3,    PC0_TK11,                                                     
    PC1_GPIO  = 1, PC1_LED_S15,    PC1_SEG12,   PC1_PWM3A,   PC1_SS0,     PC1_TK12,                                                     
    PC2_GPIO  = 1, PC2_SEG13,      PC2_PWM3B,   PC2_SCK0,    PC2_TK13,                                                                  
    PC3_GPIO  = 1, PC3_SEG14,      PC3_T1,      PC3_PWM1A,   PC3_TXD0,    PC3_RXD0,      PC3_MISO0,    PC3_TK14,                        
    PC4_GPIO  = 1, PC4_SEG15,      PC4_T0,      PC4_PWM2A,   PC4_TXD0,    PC4_RXD0,      PC4_MOSI0,    PC4_TK15,                        
    PC5_GPIO  = 1, PC5_SEG16,      PC5_PCA0A,   PC5_PCA0ECI, PC5_TXD3,    PC5_RXD3,      PC5_SCK1,     PC5_SCL0,   PC5_TK16,            
    PC6_GPIO  = 1, PC6_SEG17,      PC6_PCA2B,   PC6_T0,      PC6_TXD3,    PC6_RXD3,      PC6_MOSI1,    PC6_SCL0,   PC6_SDA0,  PC6_TK17, 
    PC7_GPIO  = 1, PC7_SEG18,      PC7_PCA3ECI, PC7_PWM2B,   PC7_TXD3,    PC7_MISO1,     PC7_SDA0,     PC7_TK18,                        
    PC8_GPIO  = 1, PC8_SEG19,      PC8_PCA3B,   PC8_T1,      PC8_FLT0,    PC8_TXD1,      PC8_RXD1,     PC8_MISO0,  PC8_TK19,            
    PC9_GPIO  = 1, PC9_SEG20,      PC9_PCA3A,   PC9_T2,      PC9_TXD1,    PC9_RXD1,      PC9_MOSI0,    PC9_SDA0,   PC9_TK20,            
    PC10_GPIO = 1, PC10_AN21,      PC10_SEG21,  PC10_PCA2A,  PC10_T3,     PC10_TXD2,     PC10_RXD2,    PC10_SCK0,  PC10_SCL0, PC10_TK21,
    PC11_GPIO = 1, PC11_AN22,      PC11_SEG22,  PC11_PCA2B,  PC11_PCA2ECI,PC11_TXD2,     PC11_RXD2,    PC11_SS0,   PC11_TK22,           
    PC12_GPIO = 1, PC12_AN6,       PC12_SEG23,  PC12_PCA2A,  PC12_PCA2ECI,PC12_SCL0,     PC12_SDA0,    PC12_TK23,                       
    PC13_GPIO = 1, PC13_AN7,       PC13_SEG24,  PC13_PCA1A,  PC13_FLT2,   PC13_SCL0,     PC13_SDA0,    PC13_TK24,                       
    PC14_GPIO = 1, PC14_AN8,       PC14_SEG25,  PC14_PCA1B,  PC14_FLT1,   PC14_SCL0,     PC14_SDA0,    PC14_TK25,                       
    PC15_GPIO = 1, PC15_AN9,       PC15_SEG26,  PC15_PCA1ECI,PC15_PWM1A,  PC15_SCL0,     PC15_SDA0,    PC15_TK26,                       
    PD0_GPIO  = 1, PD0_AN10,       PD0_SEG27,   PD0_PWM1B,   PD0_TXD0,    PD0_RXD0,      PD0_TK27,                                      
    PD1_GPIO  = 1, PD1_AN11,       PD1_SEG28,   PD1_PCA2ECI, PD1_PWM3A,   PD1_TXD0,      PD1_RXD0,     PD1_TK28,                        
    PD2_GPIO  = 1, PD2_AN12,       PD2_SEG29,   PD2_PCA2A,   PD2_FLT1,    PD2_TXD1,      PD2_RXD1,     PD2_TK29,                        
    PD3_GPIO  = 1, PD3_AN13,       PD3_SEG30,   PD3_PCA2B,   PD3_FLT3,    PD3_TXD1,      PD3_RXD1,     PD3_TK30,                        
    PD4_GPIO  = 1, PD4_AN14,       PD4_SEG31,   PD4_PWM0B,   PD4_TK31,                                                                  
    PD5_GPIO  = 1, PD5_T0,         PD5_FLT3,    PD5_SS1,                                                                                
    PD6_GPIO  = 1, PD6_T1,         PD6_SCK1,    PD6_SCL0,                                                                               
    PD7_GPIO  = 1, PD7_AN15,       PD7_T2,      PD7_RXD2,    PD7_MOSI1,   PD7_SDA0,                                                     
    PD8_GPIO  = 1, PD8_AN16,       PD8_T3,      PD8_TXD2,    PD8_MISO1,                                                                 
    PD9_GPIO  = 1, PD9_AN23,       PD9_SEG32,   PD9_PWM1B,   PD9_RXD2,                                                                  
    PD10_GPIO = 1, PD10_AN24,      PD10_SEG33,  PD10_PWM2B,  PD10_TXD2,                                                                 
    PD11_GPIO = 1, PD11_AN25,      PD11_SEG34,  PD11_PWM3B,                                                                             
    PD12_GPIO = 1, PD12_AN26,      PD12_SEG35,  PD12_PWM2A,                                                                             
    
}GPIO_AF_Config;




/**
  * @}
  */


    /* Exported macro ------------------------------------------------------------*/
    /* Exported functions --------------------------------------------------------*/

/** @defgroup GPIO_Exported_Functions
  * @{
  */

/* Reset the GPIOx peripheral registers to their default reset values*/
void GPIO_Reset(GPIO_TypeDef *GPIOx);

/* Initialize the GPIOx peripheral according to the specified parameters in the GPIO_InitStruct*/
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);

/* Fills each GPIO_InitStruct member with its default value*/
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct);

/* Select whether CMOS or TTL level for GPIO Pins*/
void GPIO_PinTTLConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_BitLevel);

/* Lock GPIO pins configuration registers*/
void GPIO_PinLock(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_BitLock);

/* Read the specified input port pin*/
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* Reads the specified GPIO input data port*/
uint16_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx);

/* Read the specified output data port bit*/
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* Read the specified GPIO output data port*/
uint16_t GPIO_ReadOutputData(GPIO_TypeDef *GPIOx);

/* Set the selected data port bits*/
void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* Reset the selected data port bits*/
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* Set or clear the selected data port bit*/
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BitAction BitVal);

/* Write data to selected GPIO data port. This function is used for debugging*/
void GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t PortVal);

/* Toggle the specified GPIO pins*/
void GPIO_ToggleBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* Changes the mapping of the specified pin*/
void GPIO_PinAFConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t GPIO_AF);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SH30F9XX_SA0_GPIO_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Sinowealth *****END OF FILE****/
