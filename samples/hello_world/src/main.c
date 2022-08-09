/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <soc.h>
#include <drivers/uart.h>

int main(void)
{       
    wch_sys_init();
    uart_init();

    printk("Hello world\n");

    while(1) {
        ;
    }

    return 0;
}