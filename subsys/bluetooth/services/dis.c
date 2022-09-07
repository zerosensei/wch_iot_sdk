/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/services/dis.h>

/* UUIDS */
// Device information service
const uint8_t devInfoServUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(DEVINFO_SERV_UUID), HI_UINT16(DEVINFO_SERV_UUID)};

// System ID
const uint8_t devInfoSystemIdUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SYSTEM_ID_UUID), HI_UINT16(SYSTEM_ID_UUID)};

// Model Number String
const uint8_t devInfoModelNumberUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(MODEL_NUMBER_UUID), HI_UINT16(MODEL_NUMBER_UUID)};

// Serial Number String
const uint8_t devInfoSerialNumberUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SERIAL_NUMBER_UUID), HI_UINT16(SERIAL_NUMBER_UUID)};

// Firmware Revision String
const uint8_t devInfoFirmwareRevUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(FIRMWARE_REV_UUID), HI_UINT16(FIRMWARE_REV_UUID)};

// Hardware Revision String
const uint8_t devInfoHardwareRevUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(HARDWARE_REV_UUID), HI_UINT16(HARDWARE_REV_UUID)};

// Software Revision String
const uint8_t devInfoSoftwareRevUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SOFTWARE_REV_UUID), HI_UINT16(SOFTWARE_REV_UUID)};

// Manufacturer Name String
const uint8_t devInfoMfrNameUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(MANUFACTURER_NAME_UUID), HI_UINT16(MANUFACTURER_NAME_UUID)};

// IEEE 11073-20601 Regulatory Certification Data List
const uint8_t devInfo11073CertUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(IEEE_11073_CERT_DATA_UUID), HI_UINT16(IEEE_11073_CERT_DATA_UUID)};

// PnP ID
const uint8_t devInfoPnpIdUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(PNP_ID_UUID), HI_UINT16(PNP_ID_UUID)};

/* Profile Attributes */
// Device Information Service attribute
static const gattAttrType_t devInfoService = {ATT_BT_UUID_SIZE, devInfoServUUID};

// System ID characteristic
static uint8_t devInfoSystemIdProps = GATT_PROP_READ;
static uint8_t devInfoSystemId[DEVINFO_SYSTEM_ID_LEN] = {0, 0, 0, 0, 0, 0, 0, 0};

// Model Number String characteristic
static uint8_t devInfoModelNumberProps = GATT_PROP_READ;
static const uint8_t devInfoModelNumber[] = "Model Number";

// Serial Number String characteristic
static uint8_t devInfoSerialNumberProps = GATT_PROP_READ;
static const uint8_t devInfoSerialNumber[] = "Serial Number";

// Firmware Revision String characteristic
static uint8_t devInfoFirmwareRevProps = GATT_PROP_READ;
static const uint8_t devInfoFirmwareRev[] = "Firmware Revision";

// Hardware Revision String characteristic
static uint8_t devInfoHardwareRevProps = GATT_PROP_READ;
static const uint8_t devInfoHardwareRev[] = "Hardware Revision";

// Software Revision String characteristic
static uint8_t devInfoSoftwareRevProps = GATT_PROP_READ;
static const uint8_t devInfoSoftwareRev[] = "Software Revision";

// Manufacturer Name String characteristic
static uint8_t devInfoMfrNameProps = GATT_PROP_READ;
static const uint8_t devInfoMfrName[] = "Manufacturer Name";

// IEEE 11073-20601 Regulatory Certification Data List characteristic
static uint8_t devInfo11073CertProps = GATT_PROP_READ;
static const uint8_t devInfo11073Cert[] = {
    DEVINFO_11073_BODY_EXP, // authoritative body type
    0x00,                   // authoritative body structure type
                            // authoritative body data follows below:
    'e', 'x', 'p', 'e', 'r', 'i', 'm', 'e', 'n', 't', 'a', 'l'};

// System ID characteristic
static uint8_t devInfoPnpIdProps = GATT_PROP_READ;
static uint8_t devInfoPnpId[DEVINFO_PNP_ID_LEN] = {
    1,                                    // Vendor ID source (1=Bluetooth SIG)
    LO_UINT16(0x07D7), HI_UINT16(0x07D7), // Vendor ID (WCH)
    LO_UINT16(0x0000), HI_UINT16(0x0000), // Product ID (vendor-specific)
    LO_UINT16(0x0110), HI_UINT16(0x0110)  // Product version (JJ.M.N)
};

static gattAttribute_t dev_info_srv[] = {
    BLE_GATT_PRIMEARY_SERVICE(&devInfoService),

    /* System ID */
    BLE_GATT_CHARACTERISTIC(devInfoSystemIdUUID,
                            GATT_PERMIT_READ,
                            &devInfoSystemIdProps,
                            devInfoSystemId),

    /* Model Number String */
    BLE_GATT_CHARACTERISTIC(devInfoModelNumberUUID,
                            GATT_PERMIT_READ,
                            &devInfoModelNumberProps,
                            devInfoModelNumber),

    /* Serial Number String */
    BLE_GATT_CHARACTERISTIC(devInfoSerialNumberUUID,
                            GATT_PERMIT_READ,
                            &devInfoSerialNumberProps,
                            devInfoSerialNumber),

    /* Firmware Revision String */
    BLE_GATT_CHARACTERISTIC(devInfoFirmwareRevUUID,
                            GATT_PERMIT_READ,
                            &devInfoFirmwareRevProps,
                            devInfoFirmwareRev),

    /* Hardware Revision String */
    BLE_GATT_CHARACTERISTIC(devInfoHardwareRevUUID,
                            GATT_PERMIT_READ,
                            &devInfoHardwareRevProps,
                            devInfoHardwareRev),

    /* Software Revision String */
    BLE_GATT_CHARACTERISTIC(devInfoSoftwareRevUUID,
                            GATT_PERMIT_READ,
                            &devInfoSoftwareRevProps,
                            devInfoSoftwareRev),

    /* Manufacturer Name String */
    BLE_GATT_CHARACTERISTIC(devInfoMfrNameUUID,
                            GATT_PERMIT_READ,
                            &devInfoMfrNameProps,
                            devInfoMfrName),

    /* IEEE 11073-20601 Regulatory Certification Data List */
    BLE_GATT_CHARACTERISTIC(devInfo11073CertUUID,
                            GATT_PERMIT_READ,
                            &devInfo11073CertProps,
                            devInfo11073Cert),

    /* PnP ID */
    BLE_GATT_CHARACTERISTIC(devInfoPnpIdUUID,
                            GATT_PERMIT_READ,
                            &devInfoPnpIdProps,
                            devInfoPnpId),
};

static uint8_t dis_read(uint16_t connHandle, gattAttribute_t *pAttr,
                        uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                        uint16_t maxLen, uint8_t method);

static gattServiceCBs_t dis_gatt_cb = {
    .pfnReadAttrCB = dis_read,
};

static bStatus_t dis_read(uint16_t connHandle, gattAttribute_t *pAttr,
                          uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                          uint16_t maxLen, uint8_t method)
{
    bStatus_t status = SUCCESS;
    uint16_t uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

    switch (uuid) {
    case SYSTEM_ID_UUID:
        // verify offset
        if (offset >= sizeof(devInfoSystemId)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length
            *pLen = MIN(maxLen, (sizeof(devInfoSystemId) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoSystemId[offset], *pLen);
        }
        break;

    case MODEL_NUMBER_UUID:
        // verify offset
        if (offset >= (sizeof(devInfoModelNumber) - 1)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, ((sizeof(devInfoModelNumber) - 1) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoModelNumber[offset], *pLen);
        }
        break;

    case SERIAL_NUMBER_UUID:
        // verify offset
        if (offset >= (sizeof(devInfoSerialNumber) - 1)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, ((sizeof(devInfoSerialNumber) - 1) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoSerialNumber[offset], *pLen);
        }
        break;

    case FIRMWARE_REV_UUID:
        // verify offset
        if (offset >= (sizeof(devInfoFirmwareRev) - 1)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, ((sizeof(devInfoFirmwareRev) - 1) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoFirmwareRev[offset], *pLen);
        }
        break;

    case HARDWARE_REV_UUID:
        // verify offset
        if (offset >= (sizeof(devInfoHardwareRev) - 1)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, ((sizeof(devInfoHardwareRev) - 1) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoHardwareRev[offset], *pLen);
        }
        break;

    case SOFTWARE_REV_UUID:
        // verify offset
        if (offset >= (sizeof(devInfoSoftwareRev) - 1)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, ((sizeof(devInfoSoftwareRev) - 1) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoSoftwareRev[offset], *pLen);
        }
        break;

    case MANUFACTURER_NAME_UUID:
        // verify offset
        if (offset >= (sizeof(devInfoMfrName) - 1)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length (exclude null terminating character)
            *pLen = MIN(maxLen, ((sizeof(devInfoMfrName) - 1) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoMfrName[offset], *pLen);
        }
        break;

    case IEEE_11073_CERT_DATA_UUID:
        // verify offset
        if (offset >= sizeof(devInfo11073Cert)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length
            *pLen = MIN(maxLen, (sizeof(devInfo11073Cert) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfo11073Cert[offset], *pLen);
        }
        break;

    case PNP_ID_UUID:
        // verify offset
        if (offset >= sizeof(devInfoPnpId)) {
            status = ATT_ERR_INVALID_OFFSET;
        } else {
            // determine read length
            *pLen = MIN(maxLen, (sizeof(devInfoPnpId) - offset));

            // copy data
            tmos_memcpy(pValue, &devInfoPnpId[offset], *pLen);
        }
        break;

    default:
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }

    return (status);
}

bStatus_t dis_set_param(uint8_t param, uint8_t len, void *value)
{
    bStatus_t ret = SUCCESS;

    switch (param) {
    case DEVINFO_SYSTEM_ID:
        tmos_memcpy(devInfoSystemId, value, len);
        break;

    default:
        ret = INVALIDPARAMETER;
        break;
    }

    return (ret);
}

bStatus_t dis_get_param(uint8_t param, void *value)
{
    bStatus_t ret = SUCCESS;

    switch (param) {
    case DEVINFO_SYSTEM_ID:
        tmos_memcpy(value, devInfoSystemId, sizeof(devInfoSystemId));
        break;
    case DEVINFO_MODEL_NUMBER:
        tmos_memcpy(value, devInfoModelNumber, sizeof(devInfoModelNumber));
        break;
    case DEVINFO_SERIAL_NUMBER:
        tmos_memcpy(value, devInfoSerialNumber, sizeof(devInfoSerialNumber));
        break;
    case DEVINFO_FIRMWARE_REV:
        tmos_memcpy(value, devInfoFirmwareRev, sizeof(devInfoFirmwareRev));
        break;
    case DEVINFO_HARDWARE_REV:
        tmos_memcpy(value, devInfoHardwareRev, sizeof(devInfoHardwareRev));
        break;
    case DEVINFO_SOFTWARE_REV:
        tmos_memcpy(value, devInfoSoftwareRev, sizeof(devInfoSoftwareRev));
        break;
    case DEVINFO_MANUFACTURER_NAME:
        tmos_memcpy(value, devInfoMfrName, sizeof(devInfoMfrName));
        break;
    case DEVINFO_11073_CERT_DATA:
        tmos_memcpy(value, devInfo11073Cert, sizeof(devInfo11073Cert));
        break;
    case DEVINFO_PNP_ID:
        tmos_memcpy(value, devInfoPnpId, sizeof(devInfoPnpId));
        break;
    default:
        ret = INVALIDPARAMETER;
        break;
    }

    return (ret);
}

bStatus_t device_info_service_register(void)
{
    return GATTServApp_RegisterService(dev_info_srv,
                                       GATT_NUM_ATTRS(dev_info_srv),
                                       GATT_MAX_ENCRYPT_KEY_SIZE,
                                       &dis_gatt_cb);
}