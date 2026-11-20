typedef enum IRQn
{
typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ******************************************************/
  Reset_IRQn                       = -15, /*!< 1 Reset Vector, invoked on Power up and warm reset    */
  NonMaskableInt_IRQn              = -14, /*!< 2 Non Maskable interrupt                              */
  HardFault_IRQn                   = -13, /*!< 3 Cortex-M3 Hard Fault Interrupt                      */
  MemoryManagement_IRQn            = -12, /*!< 4 Cortex-M3 Memory Management interrupt               */
  BusFault_IRQn                    = -11, /*!< 5 Cortex-M3 Bus Fault interrupt                       */
  UsageFault_IRQn                  = -10, /*!< 6 Cortex-M3 Usage Fault interrupt                     */
  SVCall_IRQn                      = -5,  /*!< 11 Cortex-M3 SV Call interrupt                        */
  DebugMonitor_IRQn                = -4,  /*!< 12 Cortex-M3 Debug Monitor interrupt                  */
  PendSV_IRQn                      = -2,  /*!< 14 Cortex-M3 Pend SV interrupt                        */
  SysTick_IRQn                     = -1,  /*!< 15 Cortex-M3 System Tick interrupt                    */
} IRQn_Type;
#include "core_cm3.h"

#if defined(__arm__) && !defined(__NOT_CC__)

#else

volatile DWT_Type __DWT;
volatile SysTick_Type __SysTick;
volatile CoreDebug_Type __CoreDebug;
volatile TPI_Type __TPI;
volatile SCnSCB_Type __SCnSCB;
volatile NVIC_Type __NVIC;
volatile ITM_Type __ITM;
volatile SCB_Type __SCB;
volatile MPU_Type __MPU;

#endif
