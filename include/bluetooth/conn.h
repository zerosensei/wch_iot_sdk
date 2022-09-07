/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_BLUETOOTH_CONN_H
#define INCLUDE_BLUETOOTH_CONN_H

#include <kernel.h>
#include "bluetooth.h"

struct ble_conn_param {
    uint16_t connHandle;
    uint16_t connInterval;
    uint16_t connSlaveLatency;
    uint16_t connTimeout;
};

#define BLE_CONN_PARAM_DEFINE(name)         \
    static struct ble_conn_param name = {       \
        .connHandle = GAP_CONNHANDLE_INIT,      \
        .connInterval = 0,          \
        .connSlaveLatency = 0,          \
        .connTimeout = 0,       \
    }

#define BLE_CONN_PARAM_RESET(name)      \
    name.connHandle = GAP_CONNHANDLE_INIT;      \
    name.connInterval = 0;          \
    name.connSlaveLatency = 0;          \
    name.connTimeout = 0      


struct ble_conn_cb {
    void (*connected)(uint16_t connHandle);
    void (*disconnected)(uint16_t connHandle, uint8_t reason);
	// bool (*le_param_req)(uint16_t conn_handle,
	// 		     struct bt_le_conn_param *param);

    void (*param_updated)(uint16_t connHandle, uint16_t interval,
				 uint16_t latency, uint16_t timeout);

    void (*rssi_read)(uint16_t connHandle, int8_t newRSSI);

#if (defined CONFIG_BT_USER_PHY_UPDATE)
    void (*phy_updated)(uint16_t connHandle, uint8_t tx_phy, uint8_t rx_phy);
#endif

    struct ble_conn_cb *_next;
};



void ble_conn_cb_register(struct ble_conn_cb *cb);

#endif /* INCLUDE_BLUETOOTH_CONN_H */