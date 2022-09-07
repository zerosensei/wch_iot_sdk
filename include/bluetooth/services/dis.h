/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_BLUETOOTH_SERVICES_DIS_H
#define INCLUDE_BLUETOOTH_SERVICES_DIS_H

#include <kernel.h>
#include <bluetooth/bluetooth.h>

// Device Information Service Parameters
#define DEVINFO_SYSTEM_ID              0
#define DEVINFO_MODEL_NUMBER           1
#define DEVINFO_SERIAL_NUMBER          2
#define DEVINFO_FIRMWARE_REV           3
#define DEVINFO_HARDWARE_REV           4
#define DEVINFO_SOFTWARE_REV           5
#define DEVINFO_MANUFACTURER_NAME      6
#define DEVINFO_11073_CERT_DATA        7
#define DEVINFO_PNP_ID                 8

// IEEE 11073 authoritative body values
#define DEVINFO_11073_BODY_EMPTY       0
#define DEVINFO_11073_BODY_IEEE        1
#define DEVINFO_11073_BODY_CONTINUA    2
#define DEVINFO_11073_BODY_EXP         254

// System ID length
#define DEVINFO_SYSTEM_ID_LEN          8

// PnP ID length
#define DEVINFO_PNP_ID_LEN             7


/**
 * @brief Initializes the Device Information service by registering
 *        GATT attributes with the GATT server.
 * 
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES.
 */
bStatus_t device_info_service_register(void);

/**
 * @brief Get a Device Information parameter.
 * 
 * @param param Profile parameter ID.
 * @param value Pointer to data to get.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES. 
 */
bStatus_t dis_get_param(uint8_t param, void *value);

/**
 * @brief Set a Device Information parameter.
 * 
 * @param param Profile parameter ID.
 * @param len Length of data to write.
 * @param value Pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES.
 */
bStatus_t dis_set_param(uint8_t param, uint8_t len, void *value);

#endif /* INCLUDE_BLUETOOTH_SERVICES_DIS_H */