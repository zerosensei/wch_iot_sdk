/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <common.h>
#include <soc.h>
#include <drivers/uart.h>
#include <logging/log.h>

int main(void)
{       
    wch_sys_init();
    uart_init();

    printf("Hello world\n");

    LOG_ERR("test");

    uint8_t a = 1;
    __ASSERT_NO_MSG(a==0);

    while(1) {
        ;
    }

    return 0;
}