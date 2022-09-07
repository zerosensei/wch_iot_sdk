/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <timeout_q.h>
#include <drivers/timer/system_timer.h>
#include <sys_clock.h>

void k_busy_wait(uint32_t usec_to_wait)
{
	if (usec_to_wait == 0U) {
		return;
	}

#if !defined(CONFIG_ARCH_HAS_CUSTOM_BUSY_WAIT)
	uint32_t start_cycles = k_cycle_get_32();

	/* use 64-bit math to prevent overflow when multiplying */
	uint32_t cycles_to_wait = (uint32_t)(
		(uint64_t)usec_to_wait *
		(uint64_t)sys_clock_hw_cycles_per_sec() /
		(uint64_t)USEC_PER_SEC
	);

	for (;;) {
		uint32_t current_cycles = k_cycle_get_32();

		/* this handles the rollover on an unsigned 32-bit value */
		if ((current_cycles - start_cycles) >= cycles_to_wait) {
			break;
		}
	}
#else
	arch_busy_wait(usec_to_wait);
#endif /* CONFIG_ARCH_HAS_CUSTOM_BUSY_WAIT */
}

