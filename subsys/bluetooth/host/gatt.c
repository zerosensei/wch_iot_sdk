/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>


int ble_gatt_init(void)
{
    int ret = 0;
    
    ret |= GGS_AddService(GATT_ALL_SERVICES);           
    ret |= GATTServApp_AddService(GATT_ALL_SERVICES);

    return ret;
}

int ble_gatt_service_add(gattAttribute_t *attrs, size_t attrs_size, gattServiceCBs_t *service_cb)
{
    int err;

    err = GATTServApp_RegisterService(attrs,
                                attrs_size,
                                GATT_MAX_ENCRYPT_KEY_SIZE,
                                service_cb);

    return err;
} 

//TODO: exchange mtu
