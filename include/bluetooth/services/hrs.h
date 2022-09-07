/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_BLUETOOTH_SERVICES_HRS_H
#define INCLUDE_BLUETOOTH_SERVICES_HRS_H

#include <kernel.h>
#include <bluetooth/bluetooth.h>

#define HRS_CTRL_ENERGY_EXP             0x01
#define HRS_ERR_CTRL_POINT_NOTSUP       0x80

#define HRS_FLAGS_VAL_FORMAT_UINT16     BIT(0)
#define HRS_FLAGS_CONTACT_NOT_DET       BIT(2)
#define HRS_FLAGS_CONTACT_DET           (BIT(1) & BIT(2))
#define HRS_FLAGS_ENERGY_EXP            BIT(3)
#define HRS_FLAGS_RR                    BIT(4)

enum hrs_sens_loc {
    HSR_SENS_LOC_OTHER = 0,
    HSR_SENS_LOC_CHEST,
    HSR_SENS_LOC_WRIST,
    HSR_SENS_LOC_FINGER,
    HSR_SENS_LOC_HAND,
    HSR_SENS_LOC_EARLOBE,
    HSR_SENS_LOC_FOOT,
};


//TODO: descript
typedef struct {
    /** 
     * @brief 
     * 
     */
    void (*hrs_ctrl_energy_exp)(void);

    /**
     * @brief 
     * 
     */
    void (*hrs_ccc_changed)(uint16_t config);
} hrs_cb_t;

/**
 * @brief Initializes the Heart Rate service by registering
 *        GATT attributes with the GATT server.
 * 
 * @param cb Simple servie callback funtions.
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES. 
 */
bStatus_t heartrate_service_register(hrs_cb_t *cb);

/**
 * @brief Set heart rate service sensor location 
 *      characteristic.
 * 
 * @param loc Sensor location. @ref hrs_sens_loc.
 */
void hrs_set_sens_loc(enum hrs_sens_loc loc);

/**
 * @brief Get heart rate service control point 
 *      characteristic.
 * 
 * @return Control point. 
 */
uint8_t hrs_get_ctrl_point(void);

/**
 * @brief Send a notification containing a heart rate
 *        measurement.
 * 
 * @param conn Connection handle.
 * @param value Pointer to the data to be sent.
 * @param len Length of the data.
 * @return SUCCESS or errors @ref BLE_STATUS_VALUES. 
 */
bStatus_t hrs_meas_notify(uint16_t conn, uint8_t *value, size_t len);

#endif /* INCLUDE_BLUETOOTH_SERVICES_HRS_H */