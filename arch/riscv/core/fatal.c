/*
 * Copyright (c) 2016 Jean-Paul Etienne <fractalclone@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <inttypes.h>
#include <logging/log.h>

#if __riscv_xlen == 32
 #define PR_REG "%08" PRIxPTR
 #define NO_REG "        "
#elif __riscv_xlen == 64
 #define PR_REG "%016" PRIxPTR
 #define NO_REG "                "
#endif

FUNC_NORETURN void z_riscv_fatal_error(unsigned int reason,
				       const z_arch_esf_t *esf)
{
	if (esf != NULL) {
		LOG_ERR("     a0: " PR_REG "    t0: " PR_REG, esf->a0, esf->t0);
		LOG_ERR("     a1: " PR_REG "    t1: " PR_REG, esf->a1, esf->t1);
		LOG_ERR("     a2: " PR_REG "    t2: " PR_REG, esf->a2, esf->t2);
		LOG_ERR("     a3: " PR_REG "    t3: " PR_REG, esf->a3, esf->t3);
		LOG_ERR("     a4: " PR_REG "    t4: " PR_REG, esf->a4, esf->t4);
		LOG_ERR("     a5: " PR_REG "    t5: " PR_REG, esf->a5, esf->t5);
		LOG_ERR("     a6: " PR_REG "    t6: " PR_REG, esf->a6, esf->t6);
		LOG_ERR("     a7: " PR_REG, esf->a7);

		LOG_ERR("     ra: " PR_REG, esf->ra);
		LOG_ERR("   mepc: " PR_REG, esf->mepc);
		LOG_ERR("mstatus: " PR_REG, esf->mstatus);
		LOG_ERR("");
	}

	z_fatal_error(reason, esf);
	CODE_UNREACHABLE;
}

static char *cause_str(unsigned long cause)
{
	switch (cause) {
	case 0:
		return "Instruction address misaligned";
	case 1:
		return "Instruction Access fault";
	case 2:
		return "Illegal instruction";
	case 3:
		return "Breakpoint";
	case 4:
		return "Load address misaligned";
	case 5:
		return "Load access fault";
	case 6:
		return "Store/AMO address misaligned";
	case 7:
		return "Store/AMO access fault";
	case 8:
		return "Environment call from U-mode";
	case 9:
		return "Environment call from S-mode";
	case 11:
		return "Environment call from M-mode";
	case 12:
		return "Instruction page fault";
	case 13:
		return "Load page fault";
	case 15:
		return "Store/AMO page fault";
	default:
		return "unknown";
	}
}

void _Fault(z_arch_esf_t *esf)
{
	unsigned long mcause;
	unsigned long mtval;
	
	__asm__ volatile("csrr %0, mcause" : "=r" (mcause));
	__asm__ volatile("csrr %0, mtval" : "=r" (mtval));

	mcause &= SOC_MCAUSE_EXP_MASK;
	LOG_ERR("");
	LOG_ERR(" mcause: %ld, %s", mcause, cause_str(mcause));
	LOG_ERR("  mtval: %lx", mtval);

	z_riscv_fatal_error(K_ERR_CPU_EXCEPTION, esf);
}

