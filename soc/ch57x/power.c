/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <logging/log.h>
#include <pm/pm.h>
#include "soc.h"
#include <drivers/gpio.h>

#ifdef CONFIG_PM
void pm_device_wakeup_source(const struct device *dev, bool enable)
{
	if (dev == DEVICE_GET(rtc)) {
		hal_pwr_wakeup_source(PWR_WAKEUP_SOURCE_RTC, true);
	} else if (dev == DEVICE_GET(gpioa)) {
		hal_pwr_wakeup_source(PWR_WAKEUP_SOURCE_GPIO, true);
	} else if (dev == DEVICE_GET(gpiob)) {
		hal_pwr_wakeup_source(PWR_WAKEUP_SOURCE_GPIO, true);
	// } else if (dev == DEVICE_GET(usb)) {
	// 	hal_pwr_wakeup_source(PWR_WAKEUP_SOURCE_USB, true);
	// } else if (dev == DEVICE_GET(bat)) {
	// 	hal_pwr_wakeup_source(PWR_WAKEUP_SOURCE_BAT, true);
	}
}

/* Invoke Low Power/System Off specific Tasks */
__weak void pm_state_set(enum pm_state state, uint8_t substate_id)
{
	ARG_UNUSED(substate_id);

	switch (state) {
	case PM_STATE_SUSPEND_TO_IDLE:
		hal_pwr_enter_idle();
		break;
	case PM_STATE_STANDBY:
		hal_clk_hse_cfg_current(HSE_CURRENT_150);
		hal_pwr_enter_halt();
		break;

	case PM_STATE_SUSPEND_TO_RAM:
		hal_clk_hse_cfg_current(HSE_CURRENT_150);
		hal_pwr_enter_sleep(RB_PWR_RAM2K | RB_PWR_RAM16K | RB_PWR_EXTEND);
		break;

	case PM_STATE_SOFT_OFF:
		hal_pwr_enter_shutdown(0);
		break;
	default:
		LOG_DBG("Unsupported power state %u", state);
		break;
	}
}

/* Handle SOC specific activity after Low Power Mode Exit */
__weak void pm_state_exit_post_ops(enum pm_state state, uint8_t substate_id)
{
	ARG_UNUSED(substate_id);

	switch (state) {
	case PM_STATE_SUSPEND_TO_IDLE:
		break;
	case PM_STATE_STANDBY:
		hal_clk_hse_cfg_current(HSE_CURRENT_100);
		break;

	case PM_STATE_SUSPEND_TO_RAM:
		z_set_timeout_expiry(k_us_to_ticks_ceil32(1000), true);
		hal_pwr_enter_idle();
		hal_clk_hse_cfg_current(HSE_CURRENT_100);
		break;

	case PM_STATE_SOFT_OFF:
		break;
	default:
		LOG_DBG("Unsupported power state %u", state);
		break;
	}
}
#endif
