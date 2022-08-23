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

#include <logging/log.h>

#ifdef CONFIG_PM

static ATOMIC_DEFINE(z_post_ops_required, 1);
static sys_slist_t pm_notifiers = SYS_SLIST_STATIC_INIT(&pm_notifiers);

/*
 * Properly initialize cpu power states. Do not make assumptions that
 * ACTIVE_STATE is 0
 */
static struct pm_state_info z_pm_state = {
	.state = PM_STATE_ACTIVE,
};

/* bitmask to check if a power state was forced. */
// static ATOMIC_DEFINE(z_cpus_pm_state_forced, CONFIG_MP_NUM_CPUS);
static struct k_spinlock pm_notifier_lock;

static inline void pm_exit_pos_ops(struct pm_state_info *info)
{
	extern __weak void
		pm_state_exit_post_ops(enum pm_state state, uint8_t substate_id);

	if (pm_state_exit_post_ops != NULL) {
		pm_state_exit_post_ops(info->state, info->substate_id);
	} else {
		/*
		 * This function is supposed to be overridden to do SoC or
		 * architecture specific post ops after sleep state exits.
		 *
		 * The kernel expects that irqs are unlocked after this.
		 */

		irq_unlock(0);
	}
}

static inline void state_set(struct pm_state_info *info)
{
	extern __weak void
		pm_state_set(enum pm_state state, uint8_t substate_id);

	if (pm_state_set != NULL) {
		pm_state_set(info->state, info->substate_id);
	}
}

/*
 * Function called to notify when the system is entering / exiting a
 * power state
 */
static inline void pm_state_notify(bool entering_state)
{
	struct pm_notifier *notifier;
	k_spinlock_key_t pm_notifier_key;
	void (*callback)(enum pm_state state);

	pm_notifier_key = k_spin_lock(&pm_notifier_lock);
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
	k_spin_unlock(&pm_notifier_lock, pm_notifier_key);
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
		pm_exit_pos_ops(&z_pm_state);
		pm_state_notify(false);
		z_pm_state = (struct pm_state_info){PM_STATE_ACTIVE,
			0, 0};
	}
}

bool pm_system_suspend(int32_t ticks)
{
	bool ret = true;
	const struct pm_state_info *info;

	info = pm_policy_next_state(0, ticks);

	if (info != NULL) {
		z_pm_state = *info;
	}

	if (ticks != K_TICKS_FOREVER) {
		/*
		 * We need to set the timer to interrupt a little bit early to
		 * accommodate the time required by the CPU to fully wake up.
		 */
		z_set_timeout_expiry(ticks -
		     k_us_to_ticks_ceil32(
			     z_pm_state.exit_latency_us),
				     true);
	}

	/*
	 * This function runs with interruptions locked but it is
	 * expected the SoC to unlock them in
	 * pm_state_exit_post_ops() when returning to active
	 * state. We don't want to be scheduled out yet, first we need
	 * to send a notification about leaving the idle state. So,
	 * we lock the scheduler here and unlock just after we have
	 * sent the notification in pm_system_resume().
	 */
	/* Enter power state */
	pm_state_notify(true);
	atomic_set_bit(z_post_ops_required, 0);
	state_set(&z_pm_state);

	/* Wake up sequence starts here */
	pm_system_resume();

	return ret;
}

void pm_notifier_register(struct pm_notifier *notifier)
{
	k_spinlock_key_t pm_notifier_key = k_spin_lock(&pm_notifier_lock);

	sys_slist_append(&pm_notifiers, &notifier->_node);
	k_spin_unlock(&pm_notifier_lock, pm_notifier_key);
}

int pm_notifier_unregister(struct pm_notifier *notifier)
{
	int ret = -EINVAL;
	k_spinlock_key_t pm_notifier_key;

	pm_notifier_key = k_spin_lock(&pm_notifier_lock);
	if (sys_slist_find_and_remove(&pm_notifiers, &(notifier->_node))) {
		ret = 0;
	}
	k_spin_unlock(&pm_notifier_lock, pm_notifier_key);

	return ret;
}

const struct pm_state_info *pm_state_next_get(uint8_t cpu)
{
	return &z_pm_state;
}


#endif /* CONFIG_PM */