/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ARCH_RISCV_INCLUDE_KERNEL_ARCH_FUNC_H
#define ARCH_RISCV_INCLUDE_KERNEL_ARCH_FUNC_H

#include <toolchain.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_RISCV_PMP
void z_riscv_configure_static_pmp_regions(void);
#endif

static ALWAYS_INLINE void arch_kernel_init(void)
{
#ifdef CONFIG_RISCV_PMP
	z_riscv_configure_static_pmp_regions();
#endif
}


#ifdef __cplusplus
}
#endif

#endif /* ARCH_RISCV_INCLUDE_KERNEL_ARCH_FUNC_H */