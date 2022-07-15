/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef INCLUDE_ARCH_RISCV_EXP_H
#define INCLUDE_ARCH_RISCV_EXP_H

#include <stddef.h>
#include <stdint.h>

struct __esf {
	uint32_t ra;		/* return address */

	uint32_t t0;		/* Caller-saved temporary register */
	uint32_t t1;		/* Caller-saved temporary register */
	uint32_t t2;		/* Caller-saved temporary register */
	uint32_t t3;		/* Caller-saved temporary register */
	uint32_t t4;		/* Caller-saved temporary register */
	uint32_t t5;		/* Caller-saved temporary register */
	uint32_t t6;		/* Caller-saved temporary register */

	uint32_t a0;		/* function argument/return value */
	uint32_t a1;		/* function argument */
	uint32_t a2;		/* function argument */
	uint32_t a3;		/* function argument */
	uint32_t a4;		/* function argument */
	uint32_t a5;		/* function argument */
	uint32_t a6;		/* function argument */
	uint32_t a7;		/* function argument */

	uint32_t mepc;		/* machine exception program counter */
	uint32_t mstatus;	/* machine status register */

	uint32_t s0;		/* callee-saved s0 */

	uint32_t sp;		/* preserved (user or kernel) stack pointer */

#if defined(CONFIG_FPU)
	uint32_t ft0;		/* Caller-saved temporary floating register */
	uint32_t ft1;		/* Caller-saved temporary floating register */
	uint32_t ft2;		/* Caller-saved temporary floating register */
	uint32_t ft3;		/* Caller-saved temporary floating register */
	uint32_t ft4;		/* Caller-saved temporary floating register */
	uint32_t ft5;		/* Caller-saved temporary floating register */
	uint32_t ft6;		/* Caller-saved temporary floating register */
	uint32_t ft7;		/* Caller-saved temporary floating register */
	uint32_t ft8;		/* Caller-saved temporary floating register */
	uint32_t ft9;		/* Caller-saved temporary floating register */
	uint32_t ft10;	    /* Caller-saved temporary floating register */
	uint32_t ft11;	    /* Caller-saved temporary floating register */
	uint32_t fa0;		/* function argument/return value */
	uint32_t fa1;		/* function argument/return value */
	uint32_t fa2;		/* function argument */
	uint32_t fa3;		/* function argument */
	uint32_t fa4;		/* function argument */
	uint32_t fa5;		/* function argument */
	uint32_t fa6;		/* function argument */
	uint32_t fa7;		/* function argument */
#endif

} __aligned(16);

typedef struct __esf z_arch_esf_t;
#endif /* INCLUDE_ARCH_RISCV_EXP_H */