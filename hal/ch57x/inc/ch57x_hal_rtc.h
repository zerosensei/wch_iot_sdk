/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_RTC_H
#define HAL_CH57X_INC_CH57X_HAL_RTC_H

#include "ch57x_common.h"

#define RTC_TIMESTAMP_MAX               1388534400UL      //44 years
#define RTC_ONEDDAY_SECONDS             (60UL * 60UL * 24UL)
#define RTC_ONEDAY_TICKS                (RTC_ONEDDAY_SECONDS * 32768UL)
#define RTC_START_TIMESTAMP             1640995200UL      //2022-01-01 08:00:00

#define RTC_CNT_DAY_MSK                 (WCH_MASK(14))

#define RTC_FLAG_TRIG_POS               (7U)
#define RTC_FLAG_TRIG_MSK               (WCH_MASK(1) << RTC_FLAG_TRIG_POS)

#define RTC_FLAG_TMR_POS                (6U)
#define RTC_FLAG_TMR_MSK                (WCH_MASK(1) << RTC_FLAG_TMR_POS)

typedef enum {
    RTC_FLAG_TMR = WCH_BIT(RTC_FLAG_TMR_POS),
    RTC_FALG_TRIG = WCH_BIT(RTC_FLAG_TRIG_POS),
} rtc_flag_t;

typedef enum {
    PERIOD_0_125_S,			// 0.125s 周期
	PERIOD_0_25_S,				// 0.25s 周期
	PERIOD_0_5_S,				// 0.5s 周期
	PERIOD_1_S,					// 1s 周期
	PERIOD_2_S,					// 2s 周期
	PERIOD_4_S,					// 4s 周期
	PERIOD_8_S,					// 8s 周期
	PERIOD_16_S,				// 16s 周期
} rtc_tmr_mode_t;


static inline void hal_rtc_mode_trig_enable(void)
{
    sys_safe_access_enable();
    R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;
    sys_safe_access_disable();
}

static inline void hal_rtc_mode_trig_disable(void)
{
    sys_safe_access_enable();
    R8_RTC_MODE_CTRL &= ~RB_RTC_TRIG_EN;
    sys_safe_access_disable();
}

static inline void hal_rtc_mode_tmr_enable(void)
{
    sys_safe_access_enable();
    R8_RTC_MODE_CTRL |= RB_RTC_TMR_EN;
    sys_safe_access_disable();
}

static inline void hal_rtc_mode_tmr_disable(void)
{
    sys_safe_access_enable();
    R8_RTC_MODE_CTRL &= ~RB_RTC_TMR_EN;
    sys_safe_access_disable();
}

static inline void hal_rtc_load_high(void)
{
    sys_safe_access_enable();
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
    sys_safe_access_disable();
}

static inline void hal_rtc_load_low(void)
{
    sys_safe_access_enable();
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
    sys_safe_access_disable();
}

static inline uint32_t hal_rtc_get_alarm(void)
{
    return R32_RTC_TRIG;
}

bool hal_rtc_flag_get(rtc_flag_t flag);
void hal_rtc_flag_clear(rtc_flag_t flag);

void hal_rtc_mode_trig_config(bool ignore);
void hal_rtc_mode_tmr_cfg(rtc_tmr_mode_t perid);

void hal_rtc_set_alarm(uint32_t ticks);
void hal_rtc_set_ticks(uint32_t ticks);
uint32_t hal_rtc_get_ticks(void);

void hal_rtc_update_ticks(uint32_t ticks);
void hal_rtc_update_days(uint16_t days);
int hal_rtc_set_timestamp(time_t rawtime);
time_t hal_rtc_get_timestamp(void);

void hal_rtc_init(void);

#endif /* HAL_CH57X_INC_CH57X_HAL_RTC_H */