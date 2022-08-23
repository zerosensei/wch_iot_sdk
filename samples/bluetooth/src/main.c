/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>

int main(void)
{
    printk("BLE peripheral sample\n");

    bt_init();
    

    while(1);

    return 0;
}