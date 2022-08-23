/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <logging/log.h>
#include <drivers/gpio.h>
#include <pm/pm.h>
#include <pm/device.h>

const struct device *led = DEVICE_GET(gpiob);

static void notify_pm_state_entry(enum pm_state state);
static void notify_pm_state_exit(enum pm_state state);

static struct pm_notifier notifier = {
    .state_entry = notify_pm_state_entry,
    .state_exit = notify_pm_state_exit
};

/**
 * @brief PM state entry noification
 * 
 * @note The execution time cannot be too long. 
 *       If it exceeds the exit PM time point, 
 *       it will not be able to wake up for a long time.
 * 
 * @param state PM state
 */
static void notify_pm_state_entry(enum pm_state state)
{
	switch (state) {
	case PM_STATE_SUSPEND_TO_IDLE:
        gpio_pin_set(led, 15, 0);
		break;
	case PM_STATE_SUSPEND_TO_RAM:
        gpio_pin_set(led, 15, 0);
		break;
	default:
		break;
	}
}

static void notify_pm_state_exit(enum pm_state state)
{
	switch (state) {
	case PM_STATE_SUSPEND_TO_IDLE:
        gpio_pin_set(led, 15, 1);
        printk("Exit sus to idle.\n");
		break;
	case PM_STATE_SUSPEND_TO_RAM:
        gpio_pin_set(led, 15, 1);
        printk("Exit sus to ram.\n");
		break;
	default:
		break;
	}
}

int main(void)
{
    printk("This is pm example\n");

    if (!device_is_ready(led)) {
        printk("device led is not ready\n");
    }

	gpio_pin_configure(led, 15, GPIO_OUTPUT_LOW);
    pm_device_wakeup_enable(DEVICE_GET(rtc), true);

    __ASSERT(pm_device_wakeup_is_enabled(DEVICE_GET(rtc)), 
            "wakeup source is not enabled");

    pm_notifier_register(&notifier);

    while(1) {
        pm_system_suspend(k_ms_to_ticks_ceil32(1000));
        k_busy_wait(1000*1000);
        pm_system_suspend(k_us_to_ticks_ceil32(1000));
        k_busy_wait(1000*1000);
    }

    return 0;
}

