typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers ******************************************************/
  NonMaskableInt_IRQn           = -14,    /*!<  2 Non Maskable Interrupt                             */
  HardFault_IRQn                = -13,    /*!<  3 Cortex-M0 Hard Fault Interrupt                     */
  SVC_IRQn                      = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                        */
  PendSV_IRQn                   = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                        */
  SysTick_IRQn                  = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                    */  
} IRQn_Type;
#include "core_cm0plus.h"

#if defined(__arm__) && !defined(__NOT_CC__)

#else

volatile NVIC_Type __NVIC;
volatile SysTick_Type __SysTick;
volatile SCB_Type __SCB;
#if (__MPU_PRESENT == 1)
volatile MPU_Type __MPU;
#endif

#endif
