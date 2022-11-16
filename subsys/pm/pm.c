/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <kernel.h>
#include <timeout_q.h>
#include <string.h>
#include <pm/device.h>
#include <pm/pm.h>
#include <pm/state.h>
#include <pm/policy.h>
#include <drivers/timer/system_timer.h>

#include <drivers/gpio.h>


static ATOMIC_DEFINE(z_post_ops_required, 1);
static sys_slist_t pm_notifiers = SYS_SLIST_STATIC_INIT(&pm_notifiers);

/*
 * Properly initialize cpu power states. Do not make assumptions that
 * ACTIVE_STATE is 0
 */
static struct pm_state_info z_pm_state = {
	.state = PM_STATE_ACTIVE,
};

extern void pm_state_set(enum pm_state state, uint8_t substate_id);
extern void pm_state_exit_post_ops(enum pm_state state, uint8_t substate_id);

/*
 * Function called to notify when the system is entering / exiting a
 * power state
 */
static inline void pm_state_notify(bool entering_state)
{
	struct pm_notifier *notifier;
    int key = irq_lock();
	void (*callback)(enum pm_state state);

	SYS_SLIST_FOR_EACH_CONTAINER(&pm_notifiers, notifier, _node) {
		if (entering_state) {
			callback = notifier->state_entry;
		} else {
			callback = notifier->state_exit;
		}

		if (callback) {
			callback(z_pm_state.state);
		}
	}
	irq_unlock(key);
}

void pm_system_resume(void)
{
	/*
	 * This notification is called from the ISR of the event
	 * that caused exit from kernel idling after PM operations.
	 *
	 * Some CPU low power states require enabling of interrupts
	 * atomically when entering those states. The wake up from
	 * such a state first executes code in the ISR of the interrupt
	 * that caused the wake. This hook will be called from the ISR.
	 * For such CPU LPS states, do post operations and restores here.
	 * The kernel scheduler will get control after the ISR finishes
	 * and it may schedule another thread.
	 */
	if (atomic_test_and_clear_bit(z_post_ops_required, 0)) {
		pm_state_exit_post_ops(z_pm_state.state, z_pm_state.substate_id);
		pm_state_notify(false);
		z_pm_state = (struct pm_state_info){PM_STATE_ACTIVE,
			0, 0};
	}
}

__attribute__((section(".highcode")))
uint32_t pm_system_suspend(uint32_t ticks)
{
	uint32_t sleep_ticks;
    int key = irq_lock();
    uint32_t now = k_cycle_get_32();

    irq_unlock(key);

	if (ticks >= now) {
		sleep_ticks = ticks - now;
	} else {
		sleep_ticks = ticks + 
			CONFIG_SOC_RTC_MAX_TICK - now;
	}

	const struct pm_state_info *pm_state = 
		pm_policy_next_state(sleep_ticks);

	if (!pm_state) {
		return 2;
	} 
	z_pm_state = *pm_state;
	sys_clock_set_ticks(ticks - 
			z_pm_state.exit_latency_ticks, 
			true);

	pm_state_notify(true);
	atomic_set_bit(z_post_ops_required, 0);

	pm_state_set(z_pm_state.state, z_pm_state.substate_id);

	if (z_pm_state.exit_latency_ticks) {
		sys_clock_set_ticks(ticks, true);
		pm_state_set(PM_STATE_SUSPEND_TO_IDLE, 0);
	} else {
		pm_system_resume();
		return 1;
	}
	pm_system_resume();

	return 0;
}

void pm_notifier_register(struct pm_notifier *notifier)
{
    int key = irq_lock();

	sys_slist_append(&pm_notifiers, &notifier->_node);
	irq_unlock(key);
}

int pm_notifier_unregister(struct pm_notifier *notifier)
{
	int ret = -EINVAL;
    int key = irq_lock();

	if (sys_slist_find_and_remove(&pm_notifiers, &(notifier->_node))) {
		ret = 0;
	}
	irq_unlock(key);

	return ret;
}

const struct pm_state_info *pm_state_next_get(uint8_t cpu)
{
	return &z_pm_state;
}

