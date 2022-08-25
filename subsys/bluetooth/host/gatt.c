/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/bluetooth.h>

int ble_gatt_service_add(gattAttribute_t *attrs, size_t attrs_size, gattServiceCBs_t *service_cb)
{
    GATTServApp_RegisterService(attrs,
                                attrs_size,
                                GATT_MAX_ENCRYPT_KEY_SIZE,
                                service_cb);

    return 0;
} 