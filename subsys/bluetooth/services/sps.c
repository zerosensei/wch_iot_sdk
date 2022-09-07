/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/services/sps.h>

/* UUIDS */
// Simple GATT Profile Service UUID: 0xFFF0
const uint8_t simpleProfileServUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)};

// Characteristic 1 UUID: 0xFFF1
const uint8_t simpleProfilechar1UUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)};

// Characteristic 2 UUID: 0xFFF2
const uint8_t simpleProfilechar2UUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_CHAR2_UUID), HI_UINT16(SIMPLEPROFILE_CHAR2_UUID)};

// Characteristic 3 UUID: 0xFFF3
const uint8_t simpleProfilechar3UUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_CHAR3_UUID), HI_UINT16(SIMPLEPROFILE_CHAR3_UUID)};

// Characteristic 4 UUID: 0xFFF4
const uint8_t simpleProfilechar4UUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_CHAR4_UUID), HI_UINT16(SIMPLEPROFILE_CHAR4_UUID)};

// Characteristic 5 UUID: 0xFFF5
const uint8_t simpleProfilechar5UUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SIMPLEPROFILE_CHAR5_UUID), HI_UINT16(SIMPLEPROFILE_CHAR5_UUID)};


/* Profile Attributes */
// Simple Profile Service attribute
static const gattAttrType_t simpleProfileService = {ATT_BT_UUID_SIZE, simpleProfileServUUID};

// Simple Profile Characteristic 1 Properties
static uint8_t simpleProfileChar1Props = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
static uint8_t simpleProfileChar1[SIMPLEPROFILE_CHAR1_LEN] = {0};

// Simple Profile Characteristic 1 User Description
static uint8_t simpleProfileChar1UserDesp[] = "Characteristic 1\0";

// Simple Profile Characteristic 2 Properties
static uint8_t simpleProfileChar2Props = GATT_PROP_READ;

// Characteristic 2 Value
static uint8_t simpleProfileChar2[SIMPLEPROFILE_CHAR2_LEN] = {0};

// Simple Profile Characteristic 2 User Description
static uint8_t simpleProfileChar2UserDesp[] = "Characteristic 2\0";

// Simple Profile Characteristic 3 Properties
static uint8_t simpleProfileChar3Props = GATT_PROP_WRITE;

// Characteristic 3 Value
static uint8_t simpleProfileChar3[SIMPLEPROFILE_CHAR3_LEN] = {0};

// Simple Profile Characteristic 3 User Description
static uint8_t simpleProfileChar3UserDesp[] = "Characteristic 3\0";

// Simple Profile Characteristic 4 Properties
static uint8_t simpleProfileChar4Props = GATT_PROP_NOTIFY;

// Characteristic 4 Value
static uint8_t simpleProfileChar4[SIMPLEPROFILE_CHAR4_LEN] = {0};

// Simple Profile Characteristic 4 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t simpleProfileChar4Config[CONFIG_BT_PERIPHERAL_MAX_CONNECTION];

// Simple Profile Characteristic 4 User Description
static uint8_t simpleProfileChar4UserDesp[] = "Characteristic 4\0";

// Simple Profile Characteristic 5 Properties
static uint8_t simpleProfileChar5Props = GATT_PROP_READ;

// Characteristic 5 Value
static uint8_t simpleProfileChar5[SIMPLEPROFILE_CHAR5_LEN] = {0};

// Simple Profile Characteristic 5 User Description
static uint8_t simpleProfileChar5UserDesp[] = "Characteristic 5\0";


static gattAttribute_t simple_srv[] = {
    BLE_GATT_PRIMEARY_SERVICE(&simpleProfileService),

    /* Characteristic 1 */
    BLE_GATT_CHRC_DECLARATION(&simpleProfileChar1Props),
    BLE_GATT_CHRC_VALUE(simpleProfilechar1UUID,
                    GATT_PERMIT_READ | GATT_PERMIT_WRITE,
                    simpleProfileChar1),
    BLE_GATT_CHRC_DESCRIPTION(simpleProfileChar1UserDesp),

    /* Characteristic 2 */
    BLE_GATT_CHRC_DECLARATION(&simpleProfileChar2Props),
    BLE_GATT_CHRC_VALUE(simpleProfilechar2UUID,
                    GATT_PERMIT_READ,
                    simpleProfileChar2),
    BLE_GATT_CHRC_DESCRIPTION(simpleProfileChar2UserDesp),

    /* Characteristic 3 */
    BLE_GATT_CHRC_DECLARATION(&simpleProfileChar3Props),
    BLE_GATT_CHRC_VALUE(simpleProfilechar3UUID,
                    GATT_PERMIT_WRITE,
                    simpleProfileChar3),
    BLE_GATT_CHRC_DESCRIPTION(simpleProfileChar3UserDesp),

    /* Characteristic 4 */
    BLE_GATT_CHRC_DECLARATION(&simpleProfileChar4Props),
    BLE_GATT_CHRC_VALUE(simpleProfilechar4UUID,
                    0,
                    simpleProfileChar4),
    BLE_GATT_CHRC_DESCRIPTION(simpleProfileChar4UserDesp),
    BLE_GATT_CCC(GATT_PERMIT_READ | GATT_PERMIT_WRITE,
                simpleProfileChar4Config),

    /* Characteristic 5 */
    BLE_GATT_CHRC_DECLARATION(&simpleProfileChar5Props),
    BLE_GATT_CHRC_VALUE(simpleProfilechar5UUID,
                    GATT_PERMIT_AUTHEN_READ,
                    simpleProfileChar5),
    BLE_GATT_CHRC_DESCRIPTION(simpleProfileChar5UserDesp),
};

static uint8_t sps_read(uint16_t connHandle, gattAttribute_t *pAttr,
                        uint8_t *pValue, uint16_t *pLen, uint16_t offset, 
                        uint16_t maxLen, uint8_t method);
static uint8_t sps_write(uint16_t connHandle, gattAttribute_t *pAttr,
                    uint8_t *pValue, uint16_t len, uint16_t offset,
                    uint8_t method);

static sps_cb_t *sps_cb;
static gattServiceCBs_t sps_gatt_cb = {
    .pfnReadAttrCB = sps_read,
    .pfnWriteAttrCB = sps_write,
};

static uint8_t sps_read(uint16_t connHandle, gattAttribute_t *pAttr,
                        uint8_t *pValue, uint16_t *pLen, uint16_t offset, 
                        uint16_t maxLen, uint8_t method)
{
    uint8_t status = SUCCESS;

    // If attribute permissions require authorization to read, return error
    if (gattPermitAuthorRead(pAttr->permissions)) {
        // Insufficient authorization
        return (ATT_ERR_INSUFFICIENT_AUTHOR);
    }

    // Make sure it's not a blob operation (no attributes in the profile are long)
    if (offset > 0) {
        return (ATT_ERR_ATTR_NOT_LONG);
    }

    if (pAttr->type.len == ATT_BT_UUID_SIZE) {
        // 16-bit UUID
        uint16_t uuid = sys_get_le16(pAttr->type.uuid);

        switch(uuid) {
        /**
         * No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
         * gattserverapp handles those reads:
         * 
         * characteristics 1 and 2 have read permissions,
         * characteritisc 3 does not have read permissions; therefore it is not
         * included here,
         * characteristic 4 does not have read permissions, but because it
         * can be sent as a notification, it is included here.
         */
        case SIMPLEPROFILE_CHAR1_UUID:
            if(maxLen > SIMPLEPROFILE_CHAR1_LEN) {
                *pLen = SIMPLEPROFILE_CHAR1_LEN;
            } else {
                *pLen = maxLen;
            }

            tmos_memcpy(pValue, pAttr->pValue, *pLen);
            break;

        case SIMPLEPROFILE_CHAR2_UUID:
            if(maxLen > SIMPLEPROFILE_CHAR2_LEN) {
                *pLen = SIMPLEPROFILE_CHAR2_LEN;
            } else {
                *pLen = maxLen;
            }

            tmos_memcpy(pValue, pAttr->pValue, *pLen);
            break;

        case SIMPLEPROFILE_CHAR4_UUID:
            if(maxLen > SIMPLEPROFILE_CHAR4_LEN) {
                *pLen = SIMPLEPROFILE_CHAR4_LEN;
            } else {
                *pLen = maxLen;
            }

            tmos_memcpy(pValue, pAttr->pValue, *pLen);
            break;

        case SIMPLEPROFILE_CHAR5_UUID:
            if(maxLen > SIMPLEPROFILE_CHAR5_LEN) {
                *pLen = SIMPLEPROFILE_CHAR5_LEN;
            }
            else {
                *pLen = maxLen;
            }

            tmos_memcpy(pValue, pAttr->pValue, *pLen);
            break;

        default:
            // Should never get here! (characteristics 3 and 4 do not have read permissions)
            *pLen = 0;
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    } else {
        // 128-bit UUID
        *pLen = 0;
        status = ATT_ERR_INVALID_HANDLE;
    }

    return status;    
}

static uint8_t sps_write(uint16_t connHandle, gattAttribute_t *pAttr,
                    uint8_t *pValue, uint16_t len, uint16_t offset,
                    uint8_t method)
{
    uint8_t status = SUCCESS;
    uint8_t notifyApp = 0xFF;

    // If attribute permissions require authorization to write, return error
    if(gattPermitAuthorWrite(pAttr->permissions)) {
        // Insufficient authorization
        return (ATT_ERR_INSUFFICIENT_AUTHOR);
    }

    if(pAttr->type.len == ATT_BT_UUID_SIZE) {
        // 16-bit UUID
        uint16_t uuid = sys_get_le16(pAttr->type.uuid);

        switch(uuid) {
        case SIMPLEPROFILE_CHAR1_UUID:
            //Validate the value
            // Make sure it's not a blob oper
            if (offset == 0) {
                if (len > SIMPLEPROFILE_CHAR1_LEN) {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            } else {
                status = ATT_ERR_ATTR_NOT_LONG;
            }

            //Write the value
            if (status == SUCCESS) {
                tmos_memcpy(pAttr->pValue, pValue, SIMPLEPROFILE_CHAR1_LEN);
                notifyApp = SIMPLEPROFILE_CHAR1;
            }
            break;

        case SIMPLEPROFILE_CHAR3_UUID:
            // Validate the value
            //  Make sure it's not a blob oper
            if (offset == 0) {
                if (len > SIMPLEPROFILE_CHAR3_LEN) {
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            } else {
                status = ATT_ERR_ATTR_NOT_LONG;
            }

            // Write the value
            if (status == SUCCESS) {
                tmos_memcpy(pAttr->pValue, pValue, SIMPLEPROFILE_CHAR3_LEN);
                notifyApp = SIMPLEPROFILE_CHAR3;
            }
            break;

        case GATT_CLIENT_CHAR_CFG_UUID:
            status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                                    offset, GATT_CLIENT_CFG_NOTIFY);

            if (status == SUCCESS) {
                if (sps_cb && sps_cb->sps_ccc_changed) {
                    sps_cb->sps_ccc_changed(sys_get_le16(pValue));
                }
            }

            break;

        default:
            // Should never get here! (characteristics 2 and 4 do not have write permissions)
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    }
    else {
        // 128-bit UUID
        status = ATT_ERR_INVALID_HANDLE;
    }

    // If a charactersitic value changed then callback function to notify application of change
    if((notifyApp != 0xFF) && sps_cb && sps_cb->sps_value_changed) {
        sps_cb->sps_value_changed(notifyApp, pValue, len);
    }

    return status;
}


bStatus_t sps_set_param(uint8_t charc, void *value, size_t len)
{
    bStatus_t ret = 0;

    switch (charc) {
    case SIMPLEPROFILE_CHAR1:
        if (len == SIMPLEPROFILE_CHAR1_LEN) {
            tmos_memcpy(simpleProfileChar1, value, SIMPLEPROFILE_CHAR1_LEN);
        } else {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR2:
        if (len == SIMPLEPROFILE_CHAR2_LEN) {
            tmos_memcpy(simpleProfileChar2, value, SIMPLEPROFILE_CHAR2_LEN);
        } else {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR3:
        if (len == SIMPLEPROFILE_CHAR3_LEN) {
            tmos_memcpy(simpleProfileChar3, value, SIMPLEPROFILE_CHAR3_LEN);
        } else {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR4:
        if (len == SIMPLEPROFILE_CHAR4_LEN) {
            tmos_memcpy(simpleProfileChar4, value, SIMPLEPROFILE_CHAR4_LEN);
        } else {
            ret = bleInvalidRange;
        }
        break;

    case SIMPLEPROFILE_CHAR5:
        if (len == SIMPLEPROFILE_CHAR5_LEN) {
            tmos_memcpy(simpleProfileChar5, value, SIMPLEPROFILE_CHAR5_LEN);
        } else {
            ret = bleInvalidRange;
        }
        break;
    default:
        ret = INVALIDPARAMETER;
        break;
    }

    return ret;
}

bStatus_t sps_get_param(uint8_t charc, void *value)
{
    bStatus_t ret = 0;

    switch(charc) {
    case SIMPLEPROFILE_CHAR1:
        tmos_memcpy(value, simpleProfileChar1, SIMPLEPROFILE_CHAR1_LEN);
        break;

    case SIMPLEPROFILE_CHAR2:
        tmos_memcpy(value, simpleProfileChar2, SIMPLEPROFILE_CHAR2_LEN);
        break;

    case SIMPLEPROFILE_CHAR3:
        tmos_memcpy(value, simpleProfileChar3, SIMPLEPROFILE_CHAR3_LEN);
        break;

    case SIMPLEPROFILE_CHAR4:
        tmos_memcpy(value, simpleProfileChar4, SIMPLEPROFILE_CHAR4_LEN);
        break;

    case SIMPLEPROFILE_CHAR5:
        tmos_memcpy(value, simpleProfileChar5, SIMPLEPROFILE_CHAR5_LEN);
        break;

    default:
        ret = INVALIDPARAMETER;
        break;
    }

    return ret; 
}

bStatus_t sps_notify(uint16_t conn, uint8_t *value, size_t len)
{
    attHandleValueNoti_t sps_noti;

    if (conn == GAP_CONNHANDLE_INIT) {
        return bleNotConnected;
    }

    sps_noti.pValue = GATT_bm_alloc(conn, ATT_HANDLE_VALUE_NOTI, len, NULL, 0);

    if (!sps_noti.pValue) {
        return bleMemAllocError;
    }

    tmos_memcpy(sps_noti.pValue, value, len);
    sps_noti.len = len;
    sps_noti.handle = simple_srv[11].handle;
    
    uint16_t ccc_cfg = GATTServApp_ReadCharCfg(conn, simpleProfileChar4Config);

    if (!(ccc_cfg & GATT_CLIENT_CFG_NOTIFY)) {
        return bleIncorrectMode;
    }

    return GATT_Notification(conn, &sps_noti, FALSE);
}

bStatus_t simple_service_register(sps_cb_t *cb)
{
    bStatus_t ret;

    sps_cb = cb;

    GATTServApp_InitCharCfg(INVALID_CONNHANDLE, simpleProfileChar4Config);

    ret = GATTServApp_RegisterService(simple_srv,
                                    GATT_NUM_ATTRS(simple_srv),
                                    GATT_MAX_ENCRYPT_KEY_SIZE,
                                    &sps_gatt_cb); 

    return ret;
}
