/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc.h"
#include <arch/cpu.h>
#include <gen_offset.h>

void sys_arch_reboot(uint8_t type)
{
	ARG_UNUSED(type);

    hal_sys_reset();
}

void wch_sys_init(void)
{
    hal_sys_init();

	//TODO: dcdc deal
}


__HIGHCODE __WCH_INT_FAST void HardFault_Handler(void)
{
	/* esf member offsets */
	GEN_OFFSET_SYM(z_arch_esf_t, ra);
	GEN_OFFSET_SYM(z_arch_esf_t, t0);
	GEN_OFFSET_SYM(z_arch_esf_t, t1);
	GEN_OFFSET_SYM(z_arch_esf_t, t2);
	GEN_OFFSET_SYM(z_arch_esf_t, t3);
	GEN_OFFSET_SYM(z_arch_esf_t, t4);
	GEN_OFFSET_SYM(z_arch_esf_t, t5);
	GEN_OFFSET_SYM(z_arch_esf_t, t6);
	GEN_OFFSET_SYM(z_arch_esf_t, a0);
	GEN_OFFSET_SYM(z_arch_esf_t, a1);
	GEN_OFFSET_SYM(z_arch_esf_t, a2);
	GEN_OFFSET_SYM(z_arch_esf_t, a3);
	GEN_OFFSET_SYM(z_arch_esf_t, a4);
	GEN_OFFSET_SYM(z_arch_esf_t, a5);
	GEN_OFFSET_SYM(z_arch_esf_t, a6);
	GEN_OFFSET_SYM(z_arch_esf_t, a7);

	GEN_OFFSET_SYM(z_arch_esf_t, mepc);
	GEN_OFFSET_SYM(z_arch_esf_t, mstatus);

	GEN_OFFSET_SYM(z_arch_esf_t, s0);

	GEN_ABSOLUTE_SYM(__z_arch_esf_t_SIZEOF, sizeof(z_arch_esf_t));

	__asm__ volatile ("addi sp, sp, -__z_arch_esf_t_SIZEOF");
	__asm__ volatile ("sw t0, __z_arch_esf_t_t0_OFFSET(sp)");	
	__asm__ volatile ("sw t1, __z_arch_esf_t_t1_OFFSET(sp)");
	__asm__ volatile ("sw t2, __z_arch_esf_t_t2_OFFSET(sp)");	
	__asm__ volatile ("sw t3, __z_arch_esf_t_t3_OFFSET(sp)");	
	__asm__ volatile ("sw t4, __z_arch_esf_t_t4_OFFSET(sp)");	
	__asm__ volatile ("sw t5, __z_arch_esf_t_t5_OFFSET(sp)");	
	__asm__ volatile ("sw t6, __z_arch_esf_t_t6_OFFSET(sp)");	
	__asm__ volatile ("sw a0, __z_arch_esf_t_a0_OFFSET(sp)");	
	__asm__ volatile ("sw a1, __z_arch_esf_t_a1_OFFSET(sp)");	
	__asm__ volatile ("sw a2, __z_arch_esf_t_a2_OFFSET(sp)");	
	__asm__ volatile ("sw a3, __z_arch_esf_t_a3_OFFSET(sp)");	
	__asm__ volatile ("sw a4, __z_arch_esf_t_a4_OFFSET(sp)");	
	__asm__ volatile ("sw a5, __z_arch_esf_t_a5_OFFSET(sp)");	
	__asm__ volatile ("sw a6, __z_arch_esf_t_a6_OFFSET(sp)");	
	__asm__ volatile ("sw a7, __z_arch_esf_t_a7_OFFSET(sp)");	
	__asm__ volatile ("sw ra, __z_arch_esf_t_ra_OFFSET(sp)");

	/* Save MEPC register */
	__asm__ volatile ("csrr t0, mepc");
	__asm__ volatile ("sw t0, __z_arch_esf_t_mepc_OFFSET(sp)");

	/* Save MSTATUS register */
	__asm__ volatile ("csrr t4, mstatus");
	__asm__ volatile ("sw t4, __z_arch_esf_t_mstatus_OFFSET(sp)");

	__asm__ volatile ("mv a0, sp");

	__asm__ volatile ("tail _Fault");
}



