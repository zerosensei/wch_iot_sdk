/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/services/hrs.h>

#define GATT_PREM_READ_MASK     (GATT_PERMIT_READ |     \
                    GATT_PERMIT_AUTHEN_READ |       \
                    GATT_PERMIT_ENCRYPT_READ)

#define GATT_PREM_WRITE_MASK     (GATT_PERMIT_WRITE |     \
                    GATT_PERMIT_AUTHEN_WRITE |       \
                    GATT_PERMIT_ENCRYPT_WRITE)

#ifndef CONFIG_BT_HRS_PERM_RW_AUTHEN
#define CONFIG_BT_HRS_PERM_RW_AUTHEN        0
#endif

#ifndef CONFIG_BT_HRS_PERM_RW_ENCRYPT
#define CONFIG_BT_HRS_PERM_RW_ENCRYPT       0
#endif

#define HRS_GATT_PERM_DEFAULT (     \
    CONFIG_BT_HRS_PERM_RW_AUTHEN ?      \
    (GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE) :      \
    CONFIG_BT_HRS_PERM_RW_ENCRYPT ?         \
    (GATT_PERMIT_ENCRYPT_READ | GATT_PERMIT_ENCRYPT_WRITE) :    \
    GATT_PERMIT_READ | GATT_PERMIT_WRITE)

// Heart rate service
const uint8_t heartRateServUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(HEARTRATE_SERV_UUID), HI_UINT16(HEARTRATE_SERV_UUID)};

// Heart rate measurement characteristic
const uint8_t heartRateMeasUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(HEARTRATE_MEAS_UUID), HI_UINT16(HEARTRATE_MEAS_UUID)};

// Sensor location characteristic
const uint8_t heartRateSensLocUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(BODY_SENSOR_LOC_UUID), HI_UINT16(BODY_SENSOR_LOC_UUID)};

// Command characteristic
const uint8_t hrs_ctrl_point_uuid[ATT_BT_UUID_SIZE] = {
    LO_UINT16(HEARTRATE_CTRL_PT_UUID), HI_UINT16(HEARTRATE_CTRL_PT_UUID)};

// Heart Rate Service attribute
static const gattAttrType_t heartRateService = {ATT_BT_UUID_SIZE, heartRateServUUID};

// Heart Rate Measurement Characteristic
static uint8_t heartRateMeas = 0;
static uint8_t heartRateMeasProps = GATT_PROP_NOTIFY;
static gattCharCfg_t heartRateMeasClientCharCfg[GATT_MAX_NUM_CONN];

// Sensor Location Characteristic
static uint8_t heartRateSensLocProps = GATT_PROP_READ;
static uint8_t heartRateSensLoc = 0;

// Command Characteristic
static uint8_t hrs_ctrl_point_props = GATT_PROP_WRITE;
static uint8_t hrs_ctrl_point = 0;

static gattAttribute_t hrs_srv[] = {
    BLE_GATT_PRIMEARY_SERVICE(&heartRateService),

    BLE_GATT_CHARACTERISTIC(heartRateMeasUUID, 
                        HRS_GATT_PERM_DEFAULT,
                        &heartRateMeasProps,
                        &heartRateMeas),
    BLE_GATT_CCC(HRS_GATT_PERM_DEFAULT,
                heartRateMeasClientCharCfg),

    BLE_GATT_CHARACTERISTIC(heartRateSensLocUUID, 
                        HRS_GATT_PERM_DEFAULT & GATT_PREM_READ_MASK,
                        &heartRateSensLocProps,
                        &heartRateSensLoc),

    BLE_GATT_CHARACTERISTIC(hrs_ctrl_point_uuid, 
                        HRS_GATT_PERM_DEFAULT & GATT_PREM_WRITE_MASK,
                        &hrs_ctrl_point_props,
                        &hrs_ctrl_point),
};

static hrs_cb_t *hrs_cb;

static uint8_t hrs_read(uint16_t connHandle, gattAttribute_t *pAttr,
                        uint8_t *pValue, uint16_t *pLen, uint16_t offset, 
                        uint16_t maxLen, uint8_t method);
static uint8_t hrs_write(uint16_t connHandle, gattAttribute_t *pAttr,
                    uint8_t *pValue, uint16_t len, uint16_t offset,
                    uint8_t method);
static bStatus_t hrs_auth(uint16_t connHandle, gattAttribute_t *pAttr, 
                    uint8_t opcode);


static gattServiceCBs_t hrs_gatt_cb = {
    .pfnReadAttrCB = hrs_read,
    .pfnWriteAttrCB = hrs_write,
    .pfnAuthorizeAttrCB = hrs_auth,
};

static uint8_t hrs_read(uint16_t connHandle, gattAttribute_t *pAttr,
                        uint8_t *pValue, uint16_t *pLen, uint16_t offset, 
                        uint16_t maxLen, uint8_t method)
{
    uint8_t status = SUCCESS;

    uint16_t uuid = sys_get_le16(pAttr->type.uuid);

    // Make sure it's not a blob operation (no attributes in the profile are long)
    if(offset > 0) {
        return (ATT_ERR_ATTR_NOT_LONG);
    }

    if(uuid == BODY_SENSOR_LOC_UUID) {
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
    } else {
        status = ATT_ERR_ATTR_NOT_FOUND;
    }

    return status;
}

static uint8_t hrs_write(uint16_t connHandle, gattAttribute_t *pAttr,
                    uint8_t *pValue, uint16_t len, uint16_t offset,
                    uint8_t method)
{
    uint8_t status = SUCCESS;

    uint16_t uuid = sys_get_le16(pAttr->type.uuid);

    switch(uuid) {
    case HEARTRATE_CTRL_PT_UUID:
        if(offset > 0) {
            status = ATT_ERR_ATTR_NOT_LONG;
        } else if(len != 1) {
            status = ATT_ERR_INVALID_VALUE_SIZE;
        } else if(*pValue != HRS_CTRL_ENERGY_EXP) {
            status = HRS_ERR_CTRL_POINT_NOTSUP;
        } else {
            *(pAttr->pValue) = pValue[0];

            if (hrs_cb && hrs_cb->hrs_ctrl_energy_exp) {
                hrs_cb->hrs_ctrl_energy_exp();
            }
        }
        break;

    case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                        offset, GATT_CLIENT_CFG_NOTIFY);
        if(status == SUCCESS) {
            if (hrs_cb && hrs_cb->hrs_ccc_changed) {
                hrs_cb->hrs_ccc_changed(sys_get_le16(pValue));
            }
        }
        break;

    default:
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }

    return (status);
}

static bStatus_t hrs_auth(uint16_t connHandle, gattAttribute_t *pAttr, uint8_t opcode)
{
    printk("hrs Auth\n");
    printk("uuid: %#x\n", sys_get_le16(pAttr->type.uuid));
    printk("opcode: %#x\n", opcode);

    return 0;
}

void hrs_set_sens_loc(enum hrs_sens_loc loc)
{
    heartRateSensLoc = loc;
}

uint8_t hrs_get_ctrl_point(void)
{
    return hrs_ctrl_point;
}

bStatus_t hrs_meas_notify(uint16_t conn, uint8_t *value, size_t len)
{
    attHandleValueNoti_t hrs_noti;

    if (conn == GAP_CONNHANDLE_INIT) {
        return bleNotConnected;
    }

    hrs_noti.pValue = GATT_bm_alloc(conn, ATT_HANDLE_VALUE_NOTI, len, NULL, 0);

    if (!hrs_noti.pValue) {
        return bleMemAllocError;
    }

    tmos_memcpy(hrs_noti.pValue, value, len);
    hrs_noti.len = len;
    hrs_noti.handle = hrs_srv[2].handle;

    uint16_t ccc_cfg = GATTServApp_ReadCharCfg(conn, heartRateMeasClientCharCfg);

    if (!(ccc_cfg & GATT_CLIENT_CFG_NOTIFY)) {
        return bleIncorrectMode;
    }

    return GATT_Notification(conn, &hrs_noti, FALSE);
}

bStatus_t heartrate_service_register(hrs_cb_t *cb)
{
    bStatus_t ret;

    hrs_cb = cb;

    GATTServApp_InitCharCfg(INVALID_CONNHANDLE, heartRateMeasClientCharCfg);
    ret = GATTServApp_RegisterService(hrs_srv,
                                    GATT_NUM_ATTRS(hrs_srv),
                                    GATT_MAX_ENCRYPT_KEY_SIZE,
                                    &hrs_gatt_cb); 

    return ret;
}

