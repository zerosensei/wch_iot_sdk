/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_KERNEL_ARCH_INTERFACE_H
#define INCLUDE_KERNEL_ARCH_INTERFACE_H

#include <kernel.h>

#ifdef CONFIG_ARCH_HAS_CUSTOM_BUSY_WAIT
/**
 * Architecture-specific implementation of busy-waiting
 *
 * @param usec_to_wait Wait period, in microseconds
 */
void arch_busy_wait(uint32_t usec_to_wait);
#endif

/**
 * @defgroup arch-pm Architecture-specific power management APIs
 * @ingroup arch-interface
 * @{
 */
/** Halt the system, optionally propagating a reason code */
FUNC_NORETURN void arch_system_halt(unsigned int reason);

/**
 * Architecture-specific kernel initialization hook
 *
 * This function is invoked near the top of _Cstart, for additional
 * architecture-specific setup before the rest of the kernel is brought up.
 *
 * TODO: Deprecate, most arches are using a prep_c() function to do the same
 * thing in a simpler way
 */
static inline void arch_kernel_init(void);

/** Do nothing and return. Yawn. */
static inline void arch_nop(void);

/* Include arch-specific inline function implementation */
#include <kernel_arch_func.h>

#endif /* INCLUDE_KERNEL_ARCH_INTERFACE_H */