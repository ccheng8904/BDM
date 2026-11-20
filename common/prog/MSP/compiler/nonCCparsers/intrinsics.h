/********************************************************************************
*   PROJECT          PP PED Framework
*******************************************************************************/
#ifndef INTRINSICS_H
#define INTRINSICS_H



typedef unsigned char __istate_t;
typedef unsigned char __ilevel_t;

void __disable_interrupt(void);
void __enable_interrupt(void);
__istate_t __get_interrupt_state(void);
void __set_interrupt_state(__istate_t state);

__ilevel_t __get_interrupt_level(void);
void __set_interrupt_level(__ilevel_t level);

void __no_operation(void);
void __halt(void);
void __stop(void);
void __break(void);
//lint -save -e970 modifier or type '...' outside of a typedef : basic types must be used exactly like in compiler packages
unsigned long __get_ISP_register(void);
unsigned long __get_USP_register(void);
//lint -restore -e970
#endif
