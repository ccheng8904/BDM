/* arm_compat.h - Compatibility header for ARM Compiler 5 intrinsics */
/* Copyright (C) 2016 ARM Limited. All rights reserved. */

#ifndef __ARM_COMPAT_H
#define __ARM_COMPAT_H

static inline uint32_t __current_pc(void){return 0;}
static inline uint32_t __current_sp(void){return 0;}
static inline void     __disable_irq(void){}
static inline void     __enable_irq(void){}
static inline void     __force_stores(void){}
static inline void     __memory_changed(void){}
static inline void     __schedule_barrier(void){}



#endif /* __ARM_COMPAT_H */

