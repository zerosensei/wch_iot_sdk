/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_BLUETOOTH_SERVICES_SPS_H
#define INCLUDE_BLUETOOTH_SERVICES_SPS_H

#include <kernel.h>
#include <bluetooth/bluetooth.h>

// Profile Parameters
#define SIMPLEPROFILE_CHAR1         0           // RW uint8_t - Profile Characteristic 1 value
#define SIMPLEPROFILE_CHAR2         1           // RW uint8_t - Profile Characteristic 2 value
#define SIMPLEPROFILE_CHAR3         2           // RW uint8_t - Profile Characteristic 3 value
#define SIMPLEPROFILE_CHAR4         3           // RW uint8_t - Profile Characteristic 4 value
#define SIMPLEPROFILE_CHAR5         4           // RW uint8_t - Profile Characteristic 4 value

// Simple Profile Service UUID
#define SIMPLEPROFILE_SERV_UUID     0xFFE0

// Key Pressed UUID
#define SIMPLEPROFILE_CHAR1_UUID    0xFFE1
#define SIMPLEPROFILE_CHAR2_UUID    0xFFE2
#define SIMPLEPROFILE_CHAR3_UUID    0xFFE3
#define SIMPLEPROFILE_CHAR4_UUID    0xFFE4
#define SIMPLEPROFILE_CHAR5_UUID    0xFFE5

// Length of characteristic in bytes ( Default MTU is 23 )
#define SIMPLEPROFILE_CHAR1_LEN     1
#define SIMPLEPROFILE_CHAR2_LEN     1
#define SIMPLEPROFILE_CHAR3_LEN     1
#define SIMPLEPROFILE_CHAR4_LEN     1
#define SIMPLEPROFILE_CHAR5_LEN     5


//TODO: descript
typedef struct {
    void (*sps_value_changed)(uint8_t charc, uint8_t *value, uint16_t len);
    void (*sps_ccc_changed)(uint16_t config);
} sps_cb_t;


/**
 * @brief Initializes the Simple Profile service by registering
 *        GATT attributes with the GATT server. 
 * 
 * @param cb Simple servie callback funtions.
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES.
 */
bStatus_t simple_service_register(sps_cb_t *cb);

/**
 * @brief Set a Simple Profile parameter.
 * 
 * @param charc Simple service parameter ID.
 * @param value Pointer to data to write. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 * @param len Length of data to write.
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES. 
 */
bStatus_t sps_set_param(uint8_t charc, void *value, size_t len);

/**
 * @brief Get a Simple Profile parameter.
 * 
 * @param charc Simple service parameter ID.
 * @param value Pointer to data to put. This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES.  
 */
bStatus_t sps_get_param(uint8_t charc, void *value);

/**
 * @brief Send a notification to simple service.
 * 
 * @param conn Connection handle
 * @param value Pointer to the data to be sent. 
 * @param len Length of the data to sent.
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES.  
 */
bStatus_t sps_notify(uint16_t conn, uint8_t *value, size_t len);

#endif /* INCLUDE_BLUETOOTH_SERVICES_SPS_H */