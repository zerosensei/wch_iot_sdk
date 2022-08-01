/*
 * Copyright (c) 1997-2016 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>


#include <stdbool.h>
#include <spinlock.h>

static struct k_spinlock lock;

/**
 * @brief Handle expiration of a kernel timer object.
 *
 * @param t  Timeout used by the timer.
 */
void z_timer_expiration_handler(struct _timeout *t)
{
	struct k_timer *timer = CONTAINER_OF(t, struct k_timer, timeout);
	k_spinlock_key_t key = k_spin_lock(&lock);

	/*
	 * if the timer is periodic, start it again; don't add _TICK_ALIGN
	 * since we're already aligned to a tick boundary
	 */
	if (!K_TIMEOUT_EQ(timer->period, K_NO_WAIT) &&
	    !K_TIMEOUT_EQ(timer->period, K_FOREVER)) {
		z_add_timeout(&timer->timeout, z_timer_expiration_handler,
			     timer->period);
	}

	/* update timer's status */
	timer->status += 1U;

	/* invoke timer expiry function */
	if (timer->expiry_fn != NULL) {
		/* Unlock for user handler. */
		k_spin_unlock(&lock, key);
		timer->expiry_fn(timer);
	}
}


void k_timer_init(struct k_timer *timer,
			 k_timer_expiry_t expiry_fn,
			 k_timer_stop_t stop_fn)
{
	timer->expiry_fn = expiry_fn;
	timer->stop_fn = stop_fn;
	timer->status = 0U;

	z_init_timeout(&timer->timeout);

	timer->user_data = NULL;
}


void k_timer_start(struct k_timer *timer, k_timeout_t duration,
			  k_timeout_t period)
{

	if (K_TIMEOUT_EQ(duration, K_FOREVER)) {
		return;
	}

	/* z_add_timeout() always adds one to the incoming tick count
	 * to round up to the next tick (by convention it waits for
	 * "at least as long as the specified timeout"), but the
	 * period interval is always guaranteed to be reset from
	 * within the timer ISR, so no round up is desired.  Subtract
	 * one.
	 *
	 * Note that the duration (!) value gets the same treatment
	 * for backwards compatibility.  This is unfortunate
	 * (i.e. k_timer_start() doesn't treat its initial sleep
	 * argument the same way k_sleep() does), but historical.  The
	 * timer_api test relies on this behavior.
	 */
	if (!K_TIMEOUT_EQ(period, K_FOREVER) && period.ticks != 0 &&
	    Z_TICK_ABS(period.ticks) < 0) {
		period.ticks = MAX(period.ticks - 1, 1);
	}
	if (Z_TICK_ABS(duration.ticks) < 0) {
		duration.ticks = MAX(duration.ticks - 1, 0);
	}

	(void)z_abort_timeout(&timer->timeout);
	timer->period = period;
	timer->status = 0U;

	z_add_timeout(&timer->timeout, z_timer_expiration_handler,
		     duration);
}

void k_timer_stop(struct k_timer *timer)
{
	int inactive = z_abort_timeout(&timer->timeout) != 0;

	if (inactive) {
		return;
	}

	if (timer->stop_fn != NULL) {
		timer->stop_fn(timer);
	}
}

uint32_t k_timer_status_get(struct k_timer *timer)
{
	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t result = timer->status;

	timer->status = 0U;
	k_spin_unlock(&lock, key);

	return result;
}


