/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch58x_common.h>

bool hal_rtc_flag_get(rtc_flag_t flag)
{
    return (R8_RTC_FLAG_CTRL & flag) ? true : false;
}

void hal_rtc_flag_clear(rtc_flag_t flag)
{
    if (flag == RTC_FLAG_TMR) {
        R8_RTC_FLAG_CTRL = RB_RTC_TMR_CLR;
    } else if (flag == RTC_FALG_TRIG) {
        R8_RTC_FLAG_CTRL = RB_RTC_TRIG_CLR;
    }
}

void hal_rtc_mode_trig_config(bool ignore)
{
    if (ignore == true) {
        sys_safe_access_enable();   
        R8_RTC_MODE_CTRL |= RB_RTC_IGNORE_B0;
    } else {
        sys_safe_access_enable();
        R8_RTC_MODE_CTRL &= ~RB_RTC_IGNORE_B0;
    }

    hal_rtc_mode_trig_enable();
}

void hal_rtc_mode_tmr_cfg(rtc_tmr_mode_t perid)
{
    sys_safe_access_enable();   
    R8_RTC_MODE_CTRL &= ~RB_RTC_TMR_MODE;
    sys_safe_access_enable(); 
    R8_RTC_MODE_CTRL |= perid & RB_RTC_TMR_MODE;
    hal_rtc_mode_tmr_enable();
}

void hal_rtc_set_ticks(uint32_t ticks)
{
    if (ticks > RTC_ONEDAY_TICKS) {
        ticks -= RTC_ONEDAY_TICKS;
    }

    sys_safe_access_enable();
    R32_RTC_TRIG = ticks;
    sys_safe_access_disable();
}

uint32_t hal_rtc_get_ticks(void)
{
    volatile uint32_t ticks;

    do {
        ticks = R32_RTC_CNT_32K;
    } while(ticks != R32_RTC_CNT_32K);

    return ticks;
}



void hal_rtc_update_ticks(uint32_t ticks)
{
    //XXX: 需要看同步信号？
    hal_rtc_set_ticks(ticks);
    hal_rtc_load_low();
}

void hal_rtc_update_days(uint16_t days)
{
    hal_rtc_set_ticks(days & RTC_CNT_DAY_MSK);
    hal_rtc_load_high();
}

int hal_rtc_set_timestamp(time_t rawtime)
{
    time_t time_delta;
    uint16_t day = 0, tick = 0;

    time_delta = rawtime - RTC_START_TIMESTAMP;

    if (time_delta < 0 || time_delta > RTC_TIMESTAMP_MAX) {
        return -WCH_ERROR_INVALID_PARAM;
    }

    while (time_delta >= RTC_ONEDDAY_SECONDS) {
        time_delta -= RTC_ONEDDAY_SECONDS;
        day++; 
    }

    tick = time_delta * 32768;

    hal_rtc_update_days(day);
    hal_rtc_update_ticks(tick);

    return 0;
}

time_t hal_rtc_get_timestamp(void)
{
    uint16_t day = R32_RTC_CNT_DAY & RTC_CNT_DAY_MSK;
    uint16_t sec2 = R16_RTC_CNT_2S;
    uint16_t tick = R16_RTC_CNT_32K;

    return (day * RTC_ONEDDAY_SECONDS + sec2 * 2
             + ((tick < 32768) ? 0 : 1));
}

void hal_rtc_init(void)
{
#if (defined CONFIG_SOC_LSE_32768)
    hal_clk_lf_source_select(LF_SOURCE_LSE);
#else
    hal_clk_lf_source_select(LF_SOURCE_LSI);
    hal_clk_lsi_calibrate();

    //TODO: 校准结果怎么用
#endif
    hal_rtc_set_timestamp(RTC_START_TIMESTAMP);
    hal_rtc_mode_trig_enable();
}