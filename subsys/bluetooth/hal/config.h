/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUBSYS_BLUETOOTH_HAL_CONFIG_H
#define SUBSYS_BLUETOOTH_HAL_CONFIG_H

#include <auto_config.h>

#ifdef CONFIG_SOC_SERIES_CH57X
#include <CH57xBLE_LIB.h>
#endif

#ifdef CONFIG_SOC_SERIES_CH58X

#endif

#if defined(CONFIG_BT_CTLR_TX_PWR_PLUS_7)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_7_DBM 
#elif defined(CONFIG_BT_CTLR_TX_PWR_PLUS_6)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_6_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_PLUS_5)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_5_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_PLUS_4)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_4_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_PLUS_3)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_3_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_PLUS_2)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_2_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_PLUS_1)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_1_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_0)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_0_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_MINUS_4)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_MINUS_4_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_MINUS_8)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_MINUS_8_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_MINUS_14)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_MINUS_14_DBM
#elif defined(CONFIG_BT_CTLR_TX_PWR_MINUS_20)
#define BLE_TX_POWER_DEFAULT    LL_TX_POWEER_MINUS_20_DBM
#endif


#endif /* SUBSYS_BLUETOOTH_HAL_CONFIG_H */