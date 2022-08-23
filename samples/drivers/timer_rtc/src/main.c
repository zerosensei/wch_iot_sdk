/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>

void tmr_handler(struct k_timer *timer)
{
    printk("current time: %ld ms\n", k_uptime_get_32());
}

K_TIMER_DEFINE(tmr, tmr_handler, NULL);

int main(void)
{
    printk("This is rtc timer example\n");

    k_timer_start(&tmr, K_MSEC(100), K_MSEC(1000));

    while(1);
}