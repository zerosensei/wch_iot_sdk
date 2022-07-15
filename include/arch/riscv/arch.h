/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_ARCH_RISCV_ARCH_H
#define INCLUDE_ARCH_RISCV_ARCH_H

#include <stddef.h>
#include <stdint.h>

#include <toolchain.h>
#include <arch/riscv/csr.h>
#include <arch/riscv/exp.h>

/* Common mstatus bits. All supported cores today have the same
 * layouts.
 */

#define MSTATUS_IEN     (1UL << 3)
#define MSTATUS_MPP_M   (3UL << 11)
#define MSTATUS_MPIE_EN (1UL << 7)
#define MSTATUS_FS_INIT (1UL << 13)
#define MSTATUS_FS_MASK ((1UL << 13) | (1UL << 14))

/*
 * use atomic instruction csrrc to lock global irq
 * csrrc: atomic read and clear bits in CSR register
 */
static ALWAYS_INLINE unsigned int arch_irq_lock(void)
{
	unsigned int key;

	__asm__ volatile ("csrrc %0, mstatus, %1"
			  : "=r" (key)
			  : "rK" (MSTATUS_IEN)
			  : "memory");

	return key;
}

/*
 * use atomic instruction csrs to unlock global irq
 * csrs: atomic set bits in CSR register
 */
static ALWAYS_INLINE void arch_irq_unlock(unsigned int key)
{
	__asm__ volatile ("csrs mstatus, %0"
			  :
			  : "r" (key & MSTATUS_IEN)
			  : "memory");
}

static ALWAYS_INLINE bool arch_irq_unlocked(unsigned int key)
{
	return (key & MSTATUS_IEN) != 0;
}

static ALWAYS_INLINE void arch_nop(void)
{
	__asm__ volatile("nop");
}

// extern uint32_t sys_clock_cycle_get_32(void);

// static inline uint32_t arch_k_cycle_get_32(void)
// {
// 	return sys_clock_cycle_get_32();
// }

// extern uint64_t sys_clock_cycle_get_64(void);

// static inline uint64_t arch_k_cycle_get_64(void)
// {
// 	return sys_clock_cycle_get_64();
// }

#endif /* INCLUDE_ARCH_RISCV_ARCH_H */