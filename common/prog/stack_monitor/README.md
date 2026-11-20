# Stack Monitor component

This component helps to monitor usage of the stack. Note: This feature is not implemated for RL78.

## 1 How to enable/disable stack monitor

Stack monitor component is by default enabled as it's included in **"common_components"** in <MyVariant.mak> :  
> common_components = $(msp) dbus ped_fw firmware_update stack_monitor <other components>

If you want to disable it, please remove "stack_monitor" from this line and rebuild.

## 2 How to check stack usage

The stack usage is calculated by a low priority task in task scheduler. The result(%) is stored in
a global variable **"uint32_t STK_StackConsumption"**, it can be read out by DBUS message via 
e.g. CommunicationMonitor or Edith.

User can also call function **uint32_t STK_getStackRemainingBytes(const bool isUserStack)** to get the 
current number of unused bytes in stack. Input argument isUserStack is only valid for RX platform, 
it is used to distinguish between USTACK and ISTACK.
