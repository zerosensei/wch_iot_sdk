/*
 * Copyright (c) 2022 zerosensei
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>

k_ticks_t k_delay(k_timeout_t timeout)
{
    k_ticks_t ticks;
    k_ticks_t expected_ticks;

	/* in case of K_FOREVER */
	if (K_TIMEOUT_EQ(timeout, K_FOREVER)) {
        while(1);
	}

    ticks = timeout.ticks;

    expected_ticks = ticks + sys_clock_tick_get_32();

    while(sys_clock_tick_get_32() < expected_ticks) {
        arch_nop(); 
    }

	ticks = (k_ticks_t)(expected_ticks - sys_clock_tick_get_32());

    return ticks;
}
