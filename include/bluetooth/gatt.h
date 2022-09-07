/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_BLUETOOTH_GATT_H
#define INCLUDE_BLUETOOTH_GATT_H

#include "bluetooth.h"


#define BLE_GATT_ATTRIBUTE_UUID(_len, _uuid)        \
{                       \
    .len = (_len),          \
    .uuid = (_uuid),        \
}

#define BLE_GATT_ATTRIBUTE(_uuid, _perm, _pvalue)        \
{                               \
    _uuid,          \
    _perm,     \
    0,                \
    _pvalue,          \
}

#define BLE_CHRC_TYPE_UUID_16(_uuid)           \
    BLE_GATT_ATTRIBUTE_UUID(ATT_BT_UUID_SIZE, _uuid)

#define BLE_CHRC_TYPE_UUID_128(_uuid)           \
    BLE_GATT_ATTRIBUTE_UUID(ATT_UUID_SIZE, _uuid)


#define BLE_GATT_PRIMEARY_SERVICE(attr_type)     \
    BLE_GATT_ATTRIBUTE(         \
        BLE_CHRC_TYPE_UUID_16(primaryServiceUUID),  \
        GATT_PERMIT_READ,               \
        (uint8_t *)attr_type)                                   

#define BLE_GATT_SECONDARY_SERVICE(attr_type)     \
    BLE_GATT_ATTRIBUTE(         \
        BLE_CHRC_TYPE_UUID_16(secondaryServiceUUID),  \
        GATT_PERMIT_READ,               \
        (uint8_t *)attr_type)                  

#define BLE_GATT_INCLUDE_SERVICE(attr_type)     \
    BLE_GATT_ATTRIBUTE(         \
        BLE_CHRC_TYPE_UUID_16(includeUUID),  \
        GATT_PERMIT_READ,               \
        (uint8_t *)attr_type)                  

#define BLE_GATT_CHRC_DECLARATION(_props)        \
    BLE_GATT_ATTRIBUTE(BLE_CHRC_TYPE_UUID_16(characterUUID),    \
                    GATT_PERMIT_READ,       \
                    _props)   

#define BLE_GATT_CHRC_VALUE(_uuid, _perm, _value)  \
    BLE_GATT_ATTRIBUTE(BLE_CHRC_TYPE_UUID_16(_uuid),    \
                      _perm,            \
                      (uint8_t *)_value)            

#define BLE_GATT_CHRC_DESCRIPTION(_des) \
    BLE_GATT_ATTRIBUTE(BLE_CHRC_TYPE_UUID_16(charUserDescUUID),         \
                GATT_PERMIT_READ,       \
                _des)

#define BLE_GATT_CHARACTERISTIC(_uuid, _perm, _props, _value)     \
    BLE_GATT_CHRC_DECLARATION(_props),          \
    BLE_GATT_CHRC_VALUE(_uuid, _perm, _value)


#define BLE_GATT_CCC(_perm, _cfg)     \
    BLE_GATT_ATTRIBUTE(         \
        BLE_CHRC_TYPE_UUID_16(clientCharCfgUUID),           \
        _perm,          \
        (uint8_t *)_cfg)



int ble_gatt_service_add(gattAttribute_t *attrs, size_t attrs_size, gattServiceCBs_t *service_cb);

#endif /* INCLUDE_BLUETOOTH_GATT_H */