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
#define MAX_TICKS ((RTC_ONEDAY_TICKS / 2 - CYC_PER_TICK) / CYC_PER_TICK)
#define MAX_CYC (MAX_TICKS * CYC_PER_TICK)
#define MIN_DELAY		15

#define TICKLESS IS_ENABLED(CONFIG_TICKLESS_KERNEL)

static struct k_spinlock lock;
static uint32_t last_count = 0;

static void set_rtc_alarm(uint32_t time)
{
    hal_rtc_set_alarm(time);
}

static uint32_t get_rtc_cnt(void)
{
    return hal_rtc_get_ticks();
}

__WCH_INT_FAST __HIGHCODE void RTC_IRQHandler(void)
{
	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t now = get_rtc_cnt();
	uint32_t dticks = (now - last_count) / CYC_PER_TICK;

	last_count = now;

	if (!TICKLESS) {
		uint32_t next = last_count + CYC_PER_TICK;

		if ((int32_t)(next - now) < MIN_DELAY) {
			next += CYC_PER_TICK * 33;
		}
		set_rtc_alarm(next);
	}

	k_spin_unlock(&lock, key);
	sys_clock_announce(IS_ENABLED(CONFIG_TICKLESS_KERNEL) ? dticks : 1);

	hal_rtc_flag_clear(RTC_FALG_TRIG);
}

void sys_clock_set_timeout(int32_t ticks, bool idle)
{
    ARG_UNUSED(idle);
#if defined(CONFIG_TICKLESS_KERNEL)
	ticks = ticks == K_TICKS_FOREVER ? MAX_TICKS : ticks;
	ticks = CLAMP(ticks - 1, 0, (int32_t)MAX_TICKS);

	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t now = get_rtc_cnt();
	uint32_t adj, cyc = ticks * CYC_PER_TICK;

	/* Round up to next tick boundary. */
	adj = (uint32_t)(now - last_count) + (CYC_PER_TICK - 1);
	if (cyc <= MAX_CYC - adj) {
		cyc += adj;
	} else {
		cyc = MAX_CYC;
	}
	cyc = (cyc / CYC_PER_TICK) * CYC_PER_TICK;

	if ((int32_t)(cyc + last_count - now) < MIN_DELAY) {
		/**
		 * When set more than one timeout at the same time, 
		 * cyc + last_count should not less then current ticks. 
		 * Otherwise, the alarm will not trig untill a day later.
		 */
		cyc += CYC_PER_TICK * MIN_DELAY;
	}

	set_rtc_alarm(cyc + last_count + 1);
	k_spin_unlock(&lock, key);
#endif
}

uint32_t sys_clock_elapsed(void)
{
	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		return 0;
	}

	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t ret = (get_rtc_cnt() - last_count) / CYC_PER_TICK;

	k_spin_unlock(&lock, key);

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
	last_count = get_rtc_cnt();
	set_rtc_alarm(last_count + CYC_PER_TICK);
    irq_enable(DEV_CFG_GET_IRQ(rtc));
    
	return 0;
}

#define RTC_INIT()		\
	struct pm_device pm_rtc = {		\
		.flags = BIT(PM_DEVICE_FLAG_WS_CAPABLE),			\
	};		\
	const struct device DEVICE_NAME_GET(rtc) = {		\
		COND_CODE_1(CONFIG_PM_DEVICE, (.pm = &pm_rtc,), ())	\
	};		\
	Z_INIT_ENTRY_DEFINE(DEVICE_NAME_GET(rtc), sys_clock_driver_init, DEVICE_GET(rtc), DRIVER, 0)	

RTC_INIT();