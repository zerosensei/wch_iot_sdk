/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <string.h>

extern char _highcode_lma[];
extern char _highcode_vma_start[];
extern char _highcode_vma_end[];

extern char _data_lma[];
extern char _data_vma[];
extern char _edata[];

extern char _sbss[];
extern char _ebss[];

void bss_zero(void)
{
    (void) memset(_sbss, 0, _ebss - _sbss);
}

void data_copy(void)
{
    (void) memcpy(_data_vma, _data_lma, _edata - _data_vma);
}

void highcode_copy(void)
{
    (void) memcpy(_highcode_vma_start, _highcode_lma, _highcode_vma_end - _highcode_vma_start);
}

void sys_init(void)
{
    bss_zero();
    data_copy();
    highcode_copy();

    arch_kernel_init();
    
    z_sys_init_run_level(_SYS_INIT_LEVEL_KERNEL);
    z_sys_init_run_level(_SYS_INIT_LEVEL_DRIVER);
    z_sys_init_run_level(_SYS_INIT_LEVEL_APPLICATION);

}