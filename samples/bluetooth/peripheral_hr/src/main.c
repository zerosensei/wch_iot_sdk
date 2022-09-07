/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>
#include "peripheral_hr.h"

__attribute__((noinline)) __HIGHCODE void main_process(void)
{
    while(1) {
        TMOS_SystemProcess();
    }
}

int main(void)
{
    printk("BLE peripheral sample\n");

    ble_init();
    hal_init();
    GAPRole_PeripheralInit();
    perip_process_init();
    main_process();

    return 0;
}