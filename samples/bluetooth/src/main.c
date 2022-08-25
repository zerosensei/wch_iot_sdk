/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>

// Minimum connection interval (units of 1.25ms, 6=7.5ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL    6

// Maximum connection interval (units of 1.25ms, 100=125ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL    100

#define SIMPLEPROFILE_SERV_UUID     0xFFE0
#define SIMPLEPROFILE_CHAR1_UUID    0xFFE1

#define DEVICE_NAME     "Simple Peripheral"

static uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = DEVICE_NAME;


const uint8_t simpleProfileServUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)};
const uint8_t simpleProfilechar1UUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)};

static const gattAttrType_t simpleProfileService = {ATT_BT_UUID_SIZE, simpleProfileServUUID};

static uint8_t temp_value[10];
static uint8_t simpleProfileChar1Props = GATT_PROP_NOTIFY;
static uint8_t simpleProfileChar1UserDesp[] = "Characteristic 1\0";
static gattCharCfg_t simpleProfileChar4Config[4];

static gattAttribute_t test_attr[] = {
    BLE_GATT_PRIMEARY_SERVICE(&simpleProfileService),
    BLE_GATT_CHARACTERISTIC(simpleProfilechar1UUID, 
                        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
                        &simpleProfileChar1Props,
                        temp_value,
                        simpleProfileChar1UserDesp),
    BLE_GATT_CCC(GATT_PERMIT_READ | GATT_PERMIT_WRITE,
                simpleProfileChar4Config)
};

__attribute__((noinline)) __HIGHCODE void main_process(void)
{
    while(1) {
        TMOS_SystemProcess();
    }
}

int main(void)
{
    const struct ble_data ad[] = {
        BLE_DATA_BYTES(GAP_ADTYPE_FLAGS, 
            (GAP_ADTYPE_FLAGS_GENERAL | 
            GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED)),
        
        BLE_DATA_BYTES(GAP_ADTYPE_16BIT_MORE,
            LO_UINT16(HEARTRATE_SERV_UUID),
            HI_UINT16(HEARTRATE_SERV_UUID)),
    };

    const struct ble_data sd[] = {
        BLE_DATA_BYTES(GAP_ADTYPE_LOCAL_NAME_COMPLETE, DEVICE_NAME),
        BLE_DATA_BYTES(GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE, 
                LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL), // 100ms
                HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
                LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL), // 1s
                HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL)),
    };

    printk("BLE peripheral sample\n");

    ble_init();
    hal_init();
    GAPRole_PeripheralInit();
    //TODO: GAP GATT init
    GGS_AddService(GATT_ALL_SERVICES);           // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES);
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

    ble_gatt_service_add(test_attr, GATT_NUM_ATTRS(test_attr), NULL);
    ble_adv_enable(BLE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));


    main_process();

    return 0;
}