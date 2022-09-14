/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUBSYS_BLUETOOTH_HAL_HAL_H
#define SUBSYS_BLUETOOTH_HAL_HAL_H

#include "config.h"

#define HAL_REG_INIT_EVENT          (1<<0)

uint32_t ble_flash_write(uint32_t addr, uint32_t num, uint32_t *pBuf);
uint32_t ble_flash_read(uint32_t addr, uint32_t num, uint32_t *pBuf);
uint16_t ble_hal_get_inter_temp(void);
void ble_hal_lsi_calibrate(void);


void ble_hal_init(void);


#endif /* SUBSYS_BLUETOOTH_HAL_HAL_H */