/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include "peripheral.h"
#include <drivers/gpio.h>

const struct device *gpioa = DEVICE_GET(gpioa);

__attribute__((noinline)) __HIGHCODE void main_process(void)
{
    while(1) {
        TMOS_SystemProcess();
    }
}
extern uint32_t llRecvDataDisable;
int main(void)
{
    printk("BLE peripheral sample\n");

    gpio_pin_configure(gpioa, 5, GPIO_OUTPUT_HIGH);

    ble_init();
    hal_init();

    GAPRole_PeripheralInit();
    perip_process_init();

    // llRecvDataDisable |= 2;
    main_process();

    return 0;
}