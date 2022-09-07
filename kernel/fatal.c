/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <kernel_structs.h>
#include <sys/__assert.h>
#include <arch/cpu.h>
#include <fatal.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(kernel_fatal, CONFIG_KERNEL_LOG_LEVEL);

/* LCOV_EXCL_START */
FUNC_NORETURN __weak void arch_system_halt(unsigned int reason)
{
	ARG_UNUSED(reason);

	(void)arch_irq_lock();
	for (;;) {
		/* Spin endlessly */
	}
}

__weak void k_sys_fatal_error_handler(unsigned int reason,
				      const z_arch_esf_t *esf)
{
	ARG_UNUSED(esf);

	LOG_ERR("Halting system");
	arch_system_halt(reason);
	CODE_UNREACHABLE; 
}

static const char *reason_to_str(unsigned int reason)
{
	switch (reason) {
	case K_ERR_CPU_EXCEPTION:
		return "CPU exception";
	case K_ERR_SPURIOUS_IRQ:
		return "Unhandled interrupt";
	case K_ERR_STACK_CHK_FAIL:
		return "Stack overflow";
	case K_ERR_KERNEL_OOPS:
		return "Kernel oops";
	case K_ERR_KERNEL_PANIC:
		return "Kernel panic";
	default:
		return "Unknown error";
	}
}

FUNC_NORETURN void k_fatal_halt(unsigned int reason)
{
	arch_system_halt(reason);
}

void z_fatal_error(unsigned int reason, const z_arch_esf_t *esf)
{
	unsigned int key = arch_irq_lock();

	LOG_ERR(">>> SYSTEM FATAL ERROR %d: %s", reason,
		reason_to_str(reason));

	k_sys_fatal_error_handler(reason, esf);

    __ASSERT(reason != K_ERR_KERNEL_PANIC,
			 "Attempted to recover from a kernel panic condition");

	arch_irq_unlock(key);
}
