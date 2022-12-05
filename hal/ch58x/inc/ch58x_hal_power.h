/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH58X_INC_CH58X_HAL_POWER_H
#define HAL_CH58X_INC_CH58X_HAL_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch58x_common.h"

/* Config address for hardware config for LDO&OSC and etc. */
#define ROM_CFG_ADR_HW          0x7F00C 

typedef enum {
    PWR_WAKEUP_MODE_LONG,
    PWR_WAKEUP_MODE_SHORT,
} pwr_wakeup_mode_t;

typedef enum {
    PWR_WAKEUP_SOURCE_USB = WCH_BIT(0),
    PWR_WAKEUP_SOURCE_RTC = WCH_BIT(3),
    PWR_WAKEUP_SOURCE_GPIO = WCH_BIT(4),
    PWR_WAKEUP_SOURCE_BAT = WCH_BIT(5)
} pwr_wakeup_source_t;

typedef enum {
    PWR_MONITOR_LEVEL_1V9,
    PWR_MONITOR_LEVEL_2V1,
    PWR_MONITOR_LEVEL_2V3,
    PWR_MONITOR_LEVEL_2V5,

    PWR_MONITOR_LEVEL_LP_1V8 = 0x80,
    PWR_MONITOR_LEVEL_LP_2V0,
    PWR_MONITOR_LEVEL_LP_2V1,
    PWR_MONITOR_LEVEL_LP_2V2,
    PWR_MONITOR_LEVEL_LP_2V3,
    PWR_MONITOR_LEVEL_LP_2V4,
    PWR_MONITOR_LEVEL_LP_2V5,

    PWR_MONITOR_OFF = 0xff,
} pwr_monitor_level_t;

typedef enum {
    PWR_STATE_IDLE,
    PWR_STATE_HALT,
    PWR_STATE_SLEEP_TO_RAM,
    PWR_STATE_SHUTDOWN,
} pwr_state_t;


static inline bool hal_pwr_wakeup_source_is_enabled(pwr_wakeup_source_t src)
{
    return (R8_SLP_WAKE_CTRL & src) ? true : false;
}

void hal_pwr_dcdc_enable(bool enable);
void hal_pwr_wakeup_mode(pwr_wakeup_mode_t mode);
void hal_pwr_wakeup_source(uint8_t source, bool enable);
void hal_pwr_monitor_config(pwr_monitor_level_t level);

void hal_pwr_enter_idle(void);
void hal_pwr_enter_halt(void);
void hal_pwr_enter_sleep(uint8_t peri);
void hal_pwr_enter_shutdown(uint8_t peri);

//TODO: 电源基本操作写成内联函数


#ifdef __cplusplus
}
#endif

#endif /* HAL_CH58X_INC_CH58X_HAL_POWER_H */