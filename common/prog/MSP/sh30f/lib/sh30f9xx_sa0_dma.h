/**
  ******************************************************************************
  * @file    sh30f9xx_sa0_dma.h
  * @author  sinowealth
  * @version V1.1.0
  * @date    2020-12-18  
  * @brief   This file provide DMA module's APIs
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
#ifndef __SH30F9XX_SA0_DMA_H
#define __SH30F9XX_SA0_DMA_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "sh30f9xx_sa0_libcfg.h"

/** @addtogroup SH30F9xx_sa0_libcfg_StdLib_Driver
  * @{
  */

/** @addtogroup DMA_MODULE
  * @{
  */

/** @defgroup DMA_Group_Constant  Public Constants
  * @{
  */

/** @enum DMA_CHNO_Type
  * @brief DMA ChannelNO number
  */
typedef enum
{     
    DMA_CH_NO_0 = 0, /*!< DMA ChannelNO 0 */
    DMA_CH_NO_1 = 1, /*!< DMA ChannelNO 1 */
    DMA_CH_NO_2 = 2, /*!< DMA ChannelNO 2 */
    DMA_CH_NO_3 = 3, /*!< DMA ChannelNO 3 */
}DMA_CHNO_Type;

/*! check DMA ChannelNO type */
#define IS_DMA_CHNO_Type(ch)         ((ch) <= DMA_CH_NO_3)

/** @enum DMA_TRGMODE_Type
  * @brief DMA trigger mode
  */
typedef enum
{
    DMA_TRGMODE_ONESHOT = 0, /*!< DMA trigger mode : one short */
    DMA_TRGMODE_TOEND   = 1, /*!< DMA trigger mode : to end */
}DMA_TRGMODE_Type;
/*! check Trigger type */
#define IS_DMA_TRGMODE_Type(mode)    ((mode) == DMA_TRGMODE_ONESHOT \
                                     || (mode) <= DMA_TRGMODE_TOEND)

/** @enum DMA_ADDR_Type
  * @brief DMA addressing mode
  */
typedef enum
{
    DMA_ADDR_INC    = 0, /*!< DMA address mode : increase */
    DMA_ADDR_DEC    = 1, /*!< DMA address mode : decrease */
    DMA_ADDR_FIXED  = 2, /*!< DMA address mode : fixed */
    DMA_ADDR_LOOP   = 3, /*!< DMA address mode : loop */
}DMA_ADDR_Type;
/*! check Address type */
#define IS_DMA_ADDR_Type(type)       ((type) == DMA_ADDR_INC  \
                                     || (type) == DMA_ADDR_DEC   \
                                     || (type) == DMA_ADDR_FIXED \
                                     || (type) == DMA_ADDR_LOOP)

/** @enum DMA_UNIT_Type
  * @brief DMA transmit unit size
  */
typedef enum
{
    DMA_UNIT_BYTE      = 0,  /*!< DMA one transmit unit: one time one byte */
    DMA_UNIT_HALFWORD  = 1,  /*!< DMA one transmit unit: one time one halfword */
    DMA_UNIT_WORD      = 2,  /*!< DMA one transmit unit: one time one word */
}DMA_UNIT_Type;
/*! check transmit unit type */
#define IS_DMA_UNIT_Type(type)       ((type) == DMA_UNIT_BYTE    \
                                     || (type) == DMA_UNIT_HALFWORD \
                                     || (type) == DMA_UNIT_WORD )

/** @enum DMA_PRIORITY_Type
  * @brief DMA ChannelNO priority
  */
typedef enum
{
    DMA_PRIOR_LOW      = 0, /*!< DMA ChannelNO priority: low */
    DMA_PRIOR_MIDDLE   = 1, /*!< DMA ChannelNO priority: middle */
    DMA_PRIOR_HIGH     = 2, /*!< DMA ChannelNO priority: high */
    DMA_PRIOR_HIGHEST  = 3, /*!< DMA ChannelNO priority: highest */
}DMA_PRIORITY_Type;
/*! check DMA ChannelNO's priority */
#define IS_DMA_PRIORITY_Type(type)   ((type) == DMA_PRIOR_LOW   \
                                     || (type) == DMA_PRIOR_MIDDLE \
                                     || (type) == DMA_PRIOR_HIGH   \
                                     || (type) == DMA_PRIOR_HIGHEST )

/** @enum DMA_BURSTIDLE_Type
  * @brief DMA burst idle cycles
  */
typedef enum
{
    BURST_IDLE_1CYCLE   = 0, /*!< one burst transmit release 1 cycle  */
    BURST_IDLE_2CYCLE   = 1, /*!< one burst transmit release 2 cycles */
    BURST_IDLE_3CYCLE   = 2, /*!< one burst transmit release 3 cycles */
    BURST_IDLE_4CYCLE   = 3, /*!< one burst transmit release 4 cycles */
    BURST_IDLE_5CYCLE   = 4, /*!< one burst transmit release 5 cycles */
    BURST_IDLE_6CYCLE   = 5, /*!< one burst transmit release 6 cycles */
    BURST_IDLE_7CYCLE   = 6, /*!< one burst transmit release 7 cycles */
    BURST_IDLE_8CYCLE   = 7, /*!< one burst transmit release 8 cycles */
    BURST_IDLE_9CYCLE   = 8, /*!< one burst transmit release 9 cycles */
    BURST_IDLE_10CYCLE  = 9, /*!< one burst transmit release 10 cycles */
    BURST_IDLE_12CYCLE  = 10, /*!< one burst transmit release 12 cycles */
    BURST_IDLE_16CYCLE  = 11, /*!< one burst transmit release 16 cycles */
    BURST_IDLE_20CYCLE  = 12, /*!< one burst transmit release 20 cycles */
    BURST_IDLE_24CYCLE  = 13, /*!< one burst transmit release 24 cycles */
    BURST_IDLE_28CYCLE  = 14, /*!< one burst transmit release 28 cycles */
    BURST_IDLE_32CYCLE  = 15, /*!< one burst transmit release 32 cycles */
}DMA_BURSTIDLE_Type;
/*! check burst idle cycle type */
#define IS_DMA_BURSTIDLE_Type(type)  ((type) <= BURST_IDLE_32CYCLE)

/** @enum DMA_BURSTLEN_Type
  * @brief DMA burst length
  */
typedef enum
{
    BURST_LEN_1UNIT   = 0, /*!< one burst transmit 1 unit */
    BURST_LEN_2UNIT   = 1, /*!< one burst transmit 2 units */
    BURST_LEN_3UNIT   = 2, /*!< one burst transmit 3 units */ 
    BURST_LEN_4UNIT   = 3, /*!< one burst transmit 4 units */
    BURST_LEN_5UNIT   = 4, /*!< one burst transmit 5 units */
    BURST_LEN_6UNIT   = 5, /*!< one burst transmit 6 units */
    BURST_LEN_7UNIT   = 6, /*!< one burst transmit 7 units */
    BURST_LEN_8UNIT   = 7, /*!< one burst transmit 8 units */
    BURST_LEN_9UNIT   = 8, /*!< one burst transmit 9 units */
    BURST_LEN_10UNIT  = 9, /*!< one burst transmit 10 units */
    BURST_LEN_11UNIT  = 10, /*!< one burst transmit 11 units */
    BURST_LEN_12UNIT  = 11, /*!< one burst transmit 12 units */
    BURST_LEN_13UNIT  = 12, /*!< one burst transmit 13 units */
    BURST_LEN_14UNIT  = 13, /*!< one burst transmit 14 units */
    BURST_LEN_15UNIT  = 14, /*!< one burst transmit 15 units */
    BURST_LEN_16UNIT  = 15, /*!< one burst transmit 16 units */
}DMA_BURSTLEN_Type;
/*! check burst idle cycle type */
#define IS_DMA_BURSTLEN_Type(type)  ((uint32_t)(type) <= BURST_LEN_16UNIT)

/**@enum DMA_STRMSEL_Type
   @brief DMA stream selection
  <table>
  <caption id="multi_row">Stream Selection for Channels</caption>
  <tr><th>FUNC <th>CH0        <th>CH1             <th>CH2             <th>CH3         
  <tr><td>0   <td>UART0_TX    <td>UART0_RX        <td>UART1_TX        <td>UART1_RX    
  <tr><td>1   <td>SPI0_TX     <td>SPI0_RX         <td>SPI1_TX         <td>SPI1_RX     
  <tr><td>2   <td>EXTI0/1/8/9 <td>EXTI2/3/10/11   <td>EXTI4/EXTI5     <td>EXTI6/EXTI7 
  <tr><td>3   <td>TIM0        <td>TIM1            <td>TIM2            <td>TIM3        
  <tr><td>4   <td>UART2_RX    <td>UART2_TX        <td>UART3_TX        <td>UART3_RX    
  <tr><td>5   <td>PCA0        <td>PCA1            <td>PCA2            <td>PCA3        
  <tr><td>6   <td>LED/TK      <td>LED/TK          <td>ADC/TK          <td>ADC/TK      
  <tr><td>7   <td>PWM0        <td>PWM1            <td>PWM2            <td>PWM3        
  </table>
  */
typedef enum
{
    DMA_STRMSEL_FUNC0   = 0, /*!< DMA stream function 0 */
    DMA_STRMSEL_FUNC1   = 1, /*!< DMA stream function 1 */
    DMA_STRMSEL_FUNC2   = 2, /*!< DMA stream function 2 */
    DMA_STRMSEL_FUNC3   = 3, /*!< DMA stream function 3 */
    DMA_STRMSEL_FUNC4   = 4, /*!< DMA stream function 4 */
    DMA_STRMSEL_FUNC5   = 5, /*!< DMA stream function 5 */
    DMA_STRMSEL_FUNC6   = 6, /*!< DMA stream function 6 */
    DMA_STRMSEL_FUNC7   = 7, /*!< DMA stream function 7 */
}DMA_STRMSEL_Type;
/*! check DMA stream source select */
#define IS_DMA_STRMSEL_Type(type)    ((type) == DMA_STRMSEL_FUNC0 \
                                     ||(type) == DMA_STRMSEL_FUNC1 \
                                     ||(type) == DMA_STRMSEL_FUNC2 \
                                     ||(type) == DMA_STRMSEL_FUNC3 \
                                     ||(type) == DMA_STRMSEL_FUNC4 \
                                     ||(type) == DMA_STRMSEL_FUNC5 \
                                     ||(type) == DMA_STRMSEL_FUNC6 \
                                     ||(type) == DMA_STRMSEL_FUNC7 )


/** @enum DMA_STREAMSEL_CH0_Type
  * @brief Stream Selection for Channels 0
  */
typedef enum
{
    STRMSEL_CH0_UART0_TX     = DMA_STRMSEL_FUNC0, /*!< Channel0 Function0 */
    STRMSEL_CH0_SPI0_TX      = DMA_STRMSEL_FUNC1, /*!< Channel0 Function1 */
    STRMSEL_CH0_EXTI0        = DMA_STRMSEL_FUNC2, /*!< Channel0 Function2 */
    STRMSEL_CH0_EXTI1        = DMA_STRMSEL_FUNC2, /*!< Channel0 Function2 */
    STRMSEL_CH0_EXTI8        = DMA_STRMSEL_FUNC2, /*!< Channel0 Function2 */
    STRMSEL_CH0_EXTI9        = DMA_STRMSEL_FUNC2, /*!< Channel0 Function2 */
    STRMSEL_CH0_TIM0         = DMA_STRMSEL_FUNC3, /*!< Channel0 Function3 */
    STRMSEL_CH0_UART2_RX     = DMA_STRMSEL_FUNC4, /*!< Channel0 Function4 */
    STRMSEL_CH0_PCA0         = DMA_STRMSEL_FUNC5, /*!< Channel0 Function5 */
    STRMSEL_CH0_LED          = DMA_STRMSEL_FUNC6, /*!< Channel0 Function6 */
    STRMSEL_CH0_TK           = DMA_STRMSEL_FUNC6, /*!< Channel0 Function6 */
    STRMSEL_CH0_PWM0         = DMA_STRMSEL_FUNC7, /*!< Channel0 Function7 */    
}DMA_STREAMSEL_CH0_Type;

/** @enum DMA_STREAMSEL_CH1_Type
  * @brief Stream Selection for Channels 1
  */
typedef enum
{
    STRMSEL_CH1_UART0_RX  = DMA_STRMSEL_FUNC0, /*!< Channel1 Function0 */
    STRMSEL_CH1_SPI0_RX   = DMA_STRMSEL_FUNC1, /*!< Channel1 Function1 */
    STRMSEL_CH1_EXTI2     = DMA_STRMSEL_FUNC2, /*!< Channel1 Function2 */
    STRMSEL_CH1_EXTI3     = DMA_STRMSEL_FUNC2, /*!< Channel1 Function2 */
    STRMSEL_CH1_EXTI10    = DMA_STRMSEL_FUNC2, /*!< Channel1 Function2 */
    STRMSEL_CH1_EXTI11    = DMA_STRMSEL_FUNC2, /*!< Channel1 Function2 */
    STRMSEL_CH1_TIM1      = DMA_STRMSEL_FUNC3, /*!< Channel1 Function3 */
    STRMSEL_CH1_UART2_TX  = DMA_STRMSEL_FUNC4, /*!< Channel1 Function4 */
    STRMSEL_CH1_PCA1      = DMA_STRMSEL_FUNC5, /*!< Channel1 Function5 */
    STRMSEL_CH1_LED       = DMA_STRMSEL_FUNC6, /*!< Channel1 Function6 */
    STRMSEL_CH1_TK        = DMA_STRMSEL_FUNC6, /*!< Channel1 Function6 */
    STRMSEL_CH1_PWM1      = DMA_STRMSEL_FUNC7, /*!< Channel1 Function7 */    
}DMA_STREAMSEL_CH1_Type;

/** @enum DMA_STREAMSEL_CH2_Type
  * @brief Stream Selection for Channels 2
  */
typedef enum
{
    STRMSEL_CH2_UART1_TX  = DMA_STRMSEL_FUNC0, /*!< Channel2 Function0 */
    STRMSEL_CH2_SPI1_TX   = DMA_STRMSEL_FUNC1, /*!< Channel2 Function1 */
    STRMSEL_CH2_EXTI4     = DMA_STRMSEL_FUNC2, /*!< Channel2 Function2 */
    STRMSEL_CH2_EXTI5     = DMA_STRMSEL_FUNC2, /*!< Channel2 Function2 */
    STRMSEL_CH2_EXTI12    = DMA_STRMSEL_FUNC2, /*!< Channel2 Function2 */
    STRMSEL_CH2_EXTI13    = DMA_STRMSEL_FUNC2, /*!< Channel2 Function2 */
 
    STRMSEL_CH2_TIM2      = DMA_STRMSEL_FUNC3, /*!< Channel2 Function3 */  
    STRMSEL_CH2_UART3_RX  = DMA_STRMSEL_FUNC4, /*!< Channel2 Function4 */  
    STRMSEL_CH2_PCA2      = DMA_STRMSEL_FUNC5, /*!< Channel2 Function5 */  
    STRMSEL_CH2_ADC       = DMA_STRMSEL_FUNC6, /*!< Channel2 Function6 */  
    STRMSEL_CH2_TK        = DMA_STRMSEL_FUNC6, /*!< Channel2 Function6 */  
    STRMSEL_CH2_PWM2      = DMA_STRMSEL_FUNC7, /*!< Channel2 Function7 */     
}DMA_STREAMSEL_CH2_Type;



/** @enum DMA_STREAMSEL_CH3_Type
  * @brief Stream Selection for Channels 3
  */
typedef enum
{
    STRMSEL_CH3_UART1_RX  = DMA_STRMSEL_FUNC0, /*!< Channel 3 Function 0 */
    STRMSEL_CH3_SPI1_RX   = DMA_STRMSEL_FUNC1, /*!< Channel 3 Function 1 */
    STRMSEL_CH3_EXTI6     = DMA_STRMSEL_FUNC2, /*!< Channel 3 Function 2 */
    STRMSEL_CH3_EXTI7     = DMA_STRMSEL_FUNC2, /*!< Channel 3 Function 2 */
    STRMSEL_CH3_EXTI14    = DMA_STRMSEL_FUNC2, /*!< Channel 3 Function 2 */
    STRMSEL_CH3_EXTI15    = DMA_STRMSEL_FUNC2, /*!< Channel 3 Function 2 */
 
    STRMSEL_CH3_TIM3      = DMA_STRMSEL_FUNC3, /*!< Channel 3 Function 3 */
    STRMSEL_CH3_UART3_TX  = DMA_STRMSEL_FUNC4, /*!< Channel 3 Function 4 */
    STRMSEL_CH3_PCA3      = DMA_STRMSEL_FUNC5, /*!< Channel 3 Function 5 */
    STRMSEL_CH3_ADC       = DMA_STRMSEL_FUNC6, /*!< Channel 3 Function 6 */
    STRMSEL_CH3_TK        = DMA_STRMSEL_FUNC6, /*!< Channel 3 Function 6 */
    STRMSEL_CH3_PWM3      = DMA_STRMSEL_FUNC7, /*!< Channel 3 Function 7 */
}DMA_STREAMSEL_CH3_Type;

/** @enum DMA_FLAG_Type
  * @brief DMA Transmit flags
  */
typedef enum
{
    DMA_TRANS_BURST          = (1 << DMA_IFSR_BEIF_Pos), /*!< one burst block transmit complete */
    DMA_TRANS_COMPLETE       = (1 << DMA_IFSR_TCIF_Pos), /*!< all transmit complete */
    DMA_TRANS_HALF           = (1 << DMA_IFSR_HTIF_Pos), /*!< half transmit complete */
    DMA_TRANS_ERROR          = (1 << DMA_IFSR_TEIF_Pos), /*!< transmit error */
}DMA_FLAG_Type;

/*! All DMA Flags */
#define DMA_FLAG_ALL                 (DMA_TRANS_BURST      \
                                     | DMA_TRANS_COMPLETE  \
                                     | DMA_TRANS_HALF      \
                                     | DMA_TRANS_ERROR)
/*!check DMA Flag */      
#define IS_DMA_FLAG_Type(f)          (((f) == DMA_TRANS_COMPLETE) \
                                     || ((f) == DMA_TRANS_HALF)   \
                                     || ((f) == DMA_TRANS_ERROR)  \
                                     || ((f) == DMA_TRANS_BURST))
/*!check All DMA Flags */      
#define IS_DMA_FLAG_Types(f)         ((((f)&DMA_FLAG_ALL) != 0) \
                                     && (((f) & (~DMA_FLAG_ALL)) == 0))

/** @enum DMA_INT_Type
  * @brief DMA Interrupt enable type
  */
typedef enum
{
    DMA_INT_COMPLETE = DMA_CCR0_TCIE_Msk, /*!< Transmit complete interrupt enable */
    DMA_INT_HALF     = DMA_CCR0_HTIE_Msk, /*!< Half transmit complete interrupt enable */
    DMA_INT_BURST    = DMA_CCR0_BEIE_Msk, /*!< Burst block transmit complete interrupt enable */
    DMA_INT_ERROR    = DMA_CCR0_TEIE_Msk, /*!< Transmit error interrupt enable */
}DMA_INT_Type;

/*! check DMA Interrupt  Type */
#define IS_DMA_INT_Type(ie)          ((ie) == DMA_INT_COMPLETE \
                                     || (ie) == DMA_INT_HALF   \
                                     || (ie) == DMA_INT_BURST  \
                                     || (ie) == DMA_INT_ERROR) 
/*! All DMA Interrupts */
#define DMA_INT_ALL                  (DMA_INT_COMPLETE \
                                     | DMA_INT_HALF    \
                                     | DMA_INT_BURST   \
                                     | DMA_INT_ERROR)
/*! check All Interrupts */
#define IS_DMA_INT_Types(ie)         ((((ie)&DMA_INT_ALL) != 0) \
                                     && (((ie) & (~DMA_INT_ALL)) == 0))

/**
  * @}
  */

/** @defgroup DMA_Group_Types  Public Types
  * @{
  */

/** @struct  DMA_CH_TypeDef
  * @brief structure for DMA Channel registers
  */
typedef struct
{     
    union 
    {
        __IO  uint32_t  V32;                 /*!< 0010H */
        struct 
        {
            __IO  uint32_t  EN        : 1;  /*!< [b0~b0]*/
            __IO  uint32_t  TCIE      : 1;  /*!< [b1~b1]*/
            __IO  uint32_t  HTIE      : 1;  /*!< [b2~b2]*/
            __IO  uint32_t  BEIE      : 1;  /*!< [b3~b3]*/
            __IO  uint32_t  TEIE      : 1;  /*!< [b4~b4]*/
                  uint32_t  rev0      : 1;  /*!< [b5~b5]*/
            __IO  uint32_t  DPTYP     : 2;  /*!< [b7~b6]*/
            __IO  uint32_t  SPTYP     : 2;  /*!< [b9~b8]*/
            __IO  uint32_t  DSIZE     : 2;  /*!< [b11~b10]*/
            __IO  uint32_t  SSIZE     : 2;  /*!< [b13~b12]*/
            __IO  uint32_t  PL        : 2;  /*!< [b15~b14]*/
            __IO  uint32_t  BURSTLEN  : 4;  /*!< [b19~b16]*/
            __IO  uint32_t  STRMSEL   : 3;  /*!< [b22~b20]*/
            __IO  uint32_t  TRGMODE   : 1;  /*!< [b23~b23]*/
                  uint32_t  rev1      : 8;  /*!< [b31~b24]*/
        }BIT;
    }CCR;                               /*!< 0010H */
    __IO uint16_t  NPKT;                /*!< 0014H */
         uint8_t   Reserved1[2];        /*!< 0016H */
    __I  uint16_t  CPKT;                /*!< 0018H */
         uint8_t   Reserved2[2];        /*!< 001AH */
    __IO uint32_t  SAR;                 /*!< 001CH */
    __IO uint32_t  DAR;                 /*!< 0020H */
         uint32_t  Reserved3[3];        /*!< 0024H */
}DMA_CH_TypeDef;

/*! DMA ChannelNO 0 registers */
#define DMA_CH0                      ((DMA_CH_TypeDef *)&(DMA->CCR0.V32))
/*! DMA ChannelNO 1 registers */
#define DMA_CH1                      ((DMA_CH_TypeDef *)&(DMA->CCR1.V32))
/*! DMA ChannelNO 2 registers */
#define DMA_CH2                      ((DMA_CH_TypeDef *)&(DMA->CCR2.V32))
/*! DMA ChannelNO 3 registers */
#define DMA_CH3                      ((DMA_CH_TypeDef *)&(DMA->CCR3.V32))

/*! DMA ChannelNO x registers
 *@arg ChannelNO = DMA_CH_0 ~ DMA_CH_3
*/
#define DMA_CHx(ChannelNO)           ((DMA_CH_TypeDef *)(DMA_CH0 + (ChannelNO)))

/** @struct  DMA_InitTypeDef
  * @brief structure for DMA initial data
  */
typedef struct
{
    /*!< Channel SH_ENABLE or SH_DISABLE*/
    uint32_t Enable          : 1;
    /*!< transmit complete interrupt SH_ENABLE or SH_DISABLE */
    uint32_t TransCompleteIE : 1;
    /*!< Half transmit complete interrupt SH_ENABLE or SH_DISABLE */
    uint32_t HalfCompleteIE  : 1;
    /*!< One burst transmit complete interrupt SH_ENABLE or SH_DISABLE */
    uint32_t BurstCompleteIE : 1;
    /*!< transmit error interrupt SH_ENABLE or SH_DISABLE*/
    uint32_t TransErrorIE    : 1;
    /*!< revserved */
    uint32_t rev0            : 1;
    /*!< Destination address mode: DMA_ADDR_INC,DMA_ADDR_DEC,DMA_ADDR_FIXED,DMA_ADDR_LOOP */
    uint32_t DestAddrType    : 2;
    /*!< Source address mode: DMA_ADDR_INC,DMA_ADDR_DEC,DMA_ADDR_FIXED,DMA_ADDR_LOOP */
    uint32_t SourceAddrType  : 2;
    /*!< Destination unit size: DMA_UNIT_BYTE,DMA_UNIT_HALFWORD,DMA_UNIT_WORD */
    uint32_t DestUnitSize    : 2;
    /*!< Source unit size: DMA_UNIT_BYTE,DMA_UNIT_HALFWORD,DMA_UNIT_WORD */
    uint32_t SourceUnitSize  : 2;
    /*!< DMA ChannelNO's priority : DMA_PRIOR_LOW, DMA_PRIOR_MIDDLE, DMA_PRIOR_HIGH,DMA_PRIOR_HIGHEST*/
    uint32_t Priority        : 2;
    /*!< One burst transmit length: @ref DMA_BURSTLEN_Type*/
    uint32_t OneBurstLen     : 4;
    /*!< DMA stream contrl source select */
    uint32_t StreamSelect    : 3;
    /*!< DMA tragle mode : DMA_TRGMODE_ONESHOT,DMA_TRGMODE_TOEND*/
    uint32_t TriggerMode     : 1;
    /*!< reserved */
    uint32_t rev1            : 8;

    /*!< autoreload ChannelNO's settings */
    FunctionalState Reload;
    /*!< set burst idle cycles BURST_IDLE_1CYCLE ~ BURST_IDLE_32CYCLE*/
    DMA_BURSTIDLE_Type BurstIdle;
    /*!< Burst Count to transmit value(0~0x1FFF) means (1~0x2000) burst count*/
    uint32_t BurstCount;
    /*!< Destination address */
    uint32_t DestAddr;
    /*!< Source Address */
    uint32_t SourceAddr;
} DMA_InitTypeDef;

/** @struct  DMA_INTStatus_TypeDef
  * @brief structure for DMA Interrupt status
  */
typedef union
{
    struct
    {
        uint32_t BurstComplete : 1;    /*!< One burst block transmit complete flag */
        uint32_t rev0 : 7;             /*!< reserved */
        uint32_t TransComplete : 1;    /*!< All data transmit complete */
        uint32_t rev1 : 7;             /*!< reserved */
        uint32_t HalfComplete  : 1;    /*!< Half data transmit complete */
        uint32_t rev2 : 7;             /*!< reserved */
        uint32_t TransError    : 1;    /*!< Transmit Error */
        uint32_t rev3 : 7;             /*!< reserved */
    } flags;                           /*!< DMA transmit flags struct */
    uint32_t AllFlags;                 /*!< DMA all flags value */
} DMA_INTStatus_TypeDef;

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/ 
/** @addtogroup DMA_Group_Pub_Funcs
  * @{
  */     
/* DMA Channel Init */
void DMA_Init(const DMA_CHNO_Type ChannelNO, const DMA_InitTypeDef *InitCfg);

/* Deinitializes the DMA Channelx registers to their default reset */
void DMA_Reset(const DMA_CHNO_Type ChannelNO);

/*Fills each InitStruct member with its default value*/
void DMA_StructInit(DMA_InitTypeDef *InitStruct);

/*  Enables or disables the specified DMAy Channelx interrupts. */
void DMA_INTConfig(const DMA_CHNO_Type ChannelNO, uint32_t INTType, FunctionalState NewState);

/*  Set the data source address in the current DMA channelx transmission. */
void DMA_SetSourceAddress(const DMA_CHNO_Type ChannelNO, uint32_t SourceAddress);

/* Sets the number of data units in the current DMA Channelx transfer. */
void DMA_SetDataCounter(const DMA_CHNO_Type ChannelNO, uint16_t DataNumber); 

/* Returns the number of remaining data units in the current DMA Channelx transfer. */
uint16_t DMA_GetCurrDataCounter(const DMA_CHNO_Type ChannelNO);

/* DMA Channel Enable or Disable*/
void DMA_OnOff(const DMA_CHNO_Type ChannelNO, CmdState OnOffState);

/* DMA Channel Software Trigger */
void DMA_SWTrigger(const DMA_CHNO_Type ChannelNO);

/* Get interrupt status */
void DMA_GetINTStatus(const DMA_CHNO_Type ChannelNO, DMA_INTStatus_TypeDef *INTStatus);

/* clear interrupt flags */
void DMA_ClearINTStatus(const DMA_CHNO_Type ChannelNO, const DMA_INTStatus_TypeDef *INTStatus);

/* check DMA Channel's busy flag */
bool_t DMA_IsBusy(const DMA_CHNO_Type ChannelNO);

/* DMA Channel transmit complete */
bool_t DMA_IsComplete(const DMA_CHNO_Type ChannelNO);

/* DMA Channel transmit error */
bool_t DMA_IsHalfComplete(const DMA_CHNO_Type ChannelNO);

/* DMA Channel one burst block transmit complete */
bool_t DMA_IsBlockComplete(const DMA_CHNO_Type ChannelNO);

/* DMA Channel transmit error */
bool_t DMA_IsError(const DMA_CHNO_Type ChannelNO);

/* Clear DMA Channel's transmit flags */
void DMA_ClearFlag(const DMA_CHNO_Type ChannelNO, const DMA_FLAG_Type Flags);
    
/* Get DMA Channel's transmit flags */ 
FlagStatus DMA_GetFlagStatus(const DMA_CHNO_Type ChannelNO, const DMA_FLAG_Type Flag);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /*__SH30F9XX_SA0_DMA_H */
/************************ (C) COPYRIGHT Sinowealth *****END OF FILE****/
