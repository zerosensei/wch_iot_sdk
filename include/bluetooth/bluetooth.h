/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_BLUETOOTH_BLUETOOTH_H
#define INCLUDE_BLUETOOTH_BLUETOOTH_H

#include <stddef.h>
#include <bluetooth/hal/hal.h>
#include "gatt.h"

typedef struct {
    uint8_t type;
    uint8_t addr[B_ADDR_LEN];
} ble_addr_t;

struct ble_adv_param {
    uint8_t type;
    uint8_t channel;
    uint8_t filter_policy;
    uint32_t interval_min;
    uint32_t interval_max;
    ble_addr_t *dir_addr;
};

struct ble_data {
    uint8_t len;
    uint8_t type;
    const uint8_t *data;
};

#define BT_DATA(_type, _data, _len) \
	{ \
		.type = (_type), \
		.len = (_len), \
		.data = (const uint8_t *)(_data), \
	}

#define BLE_DATA_BYTES(_type, _bytes...)      \
	BT_DATA(_type, ((uint8_t []) { _bytes }), \
		sizeof((uint8_t []) { _bytes }))


#define BLE_ADV_PARAM_INIT(_options, _int_min, _int_max, _addr) \
{ \
	.filter_policy = GAP_FILTER_POLICY_ALL,     \
    .channel = GAP_ADVCHAN_ALL,     \
	.type = (_options), \
	.interval_min = (_int_min), \
	.interval_max = (_int_max), \
	.dir_addr = (_addr), \
}

#define BLE_ADV_PARAM(_options, _int_min, _int_max, _addr) \
	((struct ble_adv_param[]) { \
		BLE_ADV_PARAM_INIT(_options, _int_min, _int_max, _addr) \
	 })

#define BLE_ADV_CONN BLE_ADV_PARAM(GAP_ADTYPE_ADV_IND, 100, 150, NULL)



int ble_adv_enable(const struct ble_adv_param *param,
            const struct ble_data *ad, size_t ad_len,
            const struct ble_data *sd, size_t sd_len);


int ble_adv_disable(void);


void ble_init(void);

#endif /* INCLUDE_BLUETOOTH_BLUETOOTH_H */