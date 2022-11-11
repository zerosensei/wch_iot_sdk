/*
 * Copyright (c) 2018 Intel Corporation.
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <pm/pm.h>
#include <pm/policy.h>
#include <sys_clock.h>
#include <sys/__assert.h>
#include <sys/time_units.h>
#include <sys/atomic.h>
#include <toolchain.h>

/** State lock reference counting */
static atomic_t state_lock_cnt[PM_STATE_COUNT];

#ifdef CONFIG_PM_POLICY_DEFAULT
__attribute__((section(".highcode")))
const struct pm_state_info *pm_policy_next_state(uint32_t ticks)
{
	static struct pm_state_info state = {0};
	uint32_t usecs = k_ticks_to_us_floor64(ticks);

#if (defined SOC_SERIES_CH58X)
	if (usecs > (CONFIG_PM_RESIDENCY_TIME_MAX *
			 1000 * 1000)) {
		return NULL;
	}
#endif

	if (usecs > CONFIG_PM_IDLE_USEC) {
		state.state = PM_STATE_SUSPEND_TO_RAM;
		state.exit_latency_ticks = k_us_to_ticks_ceil32(1400);
		state.min_residency_ticks = k_us_to_ticks_ceil32(100);
	} else if (usecs >= CONFIG_PM_ACTIVE_USEC) {
		state.state = PM_STATE_SUSPEND_TO_IDLE;
		state.exit_latency_ticks = 0;
		state.min_residency_ticks = 0;
	} else {
		state.state = PM_STATE_ACTIVE;
		state.exit_latency_ticks = 0;
		state.min_residency_ticks = 0;
	}

	if (pm_policy_state_lock_is_active(state.state)) {
		return NULL;
	}

	if ((ticks == K_TICKS_FOREVER) ||
		(ticks >= (state.exit_latency_ticks + 
		state.min_residency_ticks))) {
		return &state;
	}
	
	return NULL;
}
#endif

void pm_policy_state_lock_get(enum pm_state state)
{
	atomic_inc(&state_lock_cnt[state]);
}

void pm_policy_state_lock_put(enum pm_state state)
{
	atomic_t cnt = atomic_dec(&state_lock_cnt[state]);

	ARG_UNUSED(cnt);

	__ASSERT(cnt >= 1, "Unbalanced state lock get/put");
}

bool pm_policy_state_lock_is_active(enum pm_state state)
{
	return (atomic_get(&state_lock_cnt[state]) != 0);
}
