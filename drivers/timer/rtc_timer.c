/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <kernel.h>
#include <pm/device.h>
#include <drivers/timer/system_timer.h>

#define CYC_PER_TICK (sys_clock_hw_cycles_per_sec()	\
		      / CONFIG_SYS_CLOCK_TICKS_PER_SEC)
#define MAX_TICKS ((RTC_MAX_VALUE / 2 - CYC_PER_TICK) / CYC_PER_TICK)
#define MAX_CYC (MAX_TICKS * CYC_PER_TICK)
#define MIN_DELAY		10

static uint32_t last_count = 0;


//TODO: 专门写个rtc 和systic 的驱动 与sys无关 CH573可以用timer代替systick
static inline void set_rtc_alarm(uint32_t time)
{
    hal_rtc_set_alarm(time);
}

static inline uint32_t get_rtc_cnt(void)
{
    return hal_rtc_get_ticks();
}

__WCH_INT_FAST __HIGHCODE void RTC_IRQHandler(void)
{
    int key = irq_lock();
	uint32_t now = get_rtc_cnt();

	last_count = now;

	irq_unlock(key);

	hal_rtc_flag_clear(RTC_FALG_TRIG);
}

void sys_clock_set_ticks(uint32_t ticks, bool idle)
{
    ARG_UNUSED(idle);
    int key = irq_lock();

	set_rtc_alarm(ticks);
	irq_unlock(key);
}

uint32_t sys_clock_elapsed(void)
{
	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		return 0;
	}

    int key = irq_lock();
	uint32_t ret = (get_rtc_cnt() - last_count) / CYC_PER_TICK;

	irq_unlock(key);

	return ret;
}

uint32_t sys_clock_cycle_get_32(void)
{
	return get_rtc_cnt();
}

int sys_clock_driver_init(const struct device *dev)
{
    ARG_UNUSED(dev);

    hal_rtc_init();
	last_count = 0;
    irq_enable(DEV_CFG_GET_IRQ(rtc));
    
	return 0;
}

#define RTC_INIT()		\
	struct pm_device pm_rtc = {		\
		.flags = BIT(PM_DEVICE_FLAG_WS_CAPABLE),			\
	};		\
	const struct device DEVICE_NAME_GET(rtc) = {		\
		COND_CODE_1(CONFIG_PM, (.pm = &pm_rtc,), ())	\
	};		\
	Z_INIT_ENTRY_DEFINE(DEVICE_NAME_GET(rtc), sys_clock_driver_init, DEVICE_GET(rtc), DRIVER, 0)	

RTC_INIT();