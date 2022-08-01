/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_KERNEL_H
#define INCLUDE_KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include <toolchain.h>
#include <kernel_structs.h>
#include <sys/__assert.h>
#include <sys/slist.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <sys/time_units.h>
#include <arch/cpu.h>
#include <spinlock.h>
#include <sys_clock.h>
#include <fatal.h>
#include <timeout_q.h>

/**
 * @addtogroup clock_apis
 * @{
 */

/**
 * @brief Generate null timeout delay.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * not to wait if the requested operation cannot be performed immediately.
 *
 * @return Timeout delay value.
 */
#define K_NO_WAIT Z_TIMEOUT_NO_WAIT

/**
 * @brief Generate timeout delay from nanoseconds.
 *
 * This macro generates a timeout delay that instructs a kernel API to
 * wait up to @a t nanoseconds to perform the requested operation.
 * Note that timer precision is limited to the tick rate, not the
 * requested value.
 *
 * @param t Duration in nanoseconds.
 *
 * @return Timeout delay value.
 */
#define K_NSEC(t)     Z_TIMEOUT_NS(t)

/**
 * @brief Generate timeout delay from microseconds.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a t microseconds to perform the requested operation.
 * Note that timer precision is limited to the tick rate, not the
 * requested value.
 *
 * @param t Duration in microseconds.
 *
 * @return Timeout delay value.
 */
#define K_USEC(t)     Z_TIMEOUT_US(t)

/**
 * @brief Generate timeout delay from cycles.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a t cycles to perform the requested operation.
 *
 * @param t Duration in cycles.
 *
 * @return Timeout delay value.
 */
#define K_CYC(t)     Z_TIMEOUT_CYC(t)

/**
 * @brief Generate timeout delay from system ticks.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a t ticks to perform the requested operation.
 *
 * @param t Duration in system ticks.
 *
 * @return Timeout delay value.
 */
#define K_TICKS(t)     Z_TIMEOUT_TICKS(t)

/**
 * @brief Generate timeout delay from milliseconds.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a ms milliseconds to perform the requested operation.
 *
 * @param ms Duration in milliseconds.
 *
 * @return Timeout delay value.
 */
#define K_MSEC(ms)     Z_TIMEOUT_MS(ms)

/**
 * @brief Generate timeout delay from seconds.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a s seconds to perform the requested operation.
 *
 * @param s Duration in seconds.
 *
 * @return Timeout delay value.
 */
#define K_SECONDS(s)   K_MSEC((s) * MSEC_PER_SEC)

/**
 * @brief Generate timeout delay from minutes.

 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a m minutes to perform the requested operation.
 *
 * @param m Duration in minutes.
 *
 * @return Timeout delay value.
 */
#define K_MINUTES(m)   K_SECONDS((m) * 60)

/**
 * @brief Generate timeout delay from hours.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait up to @a h hours to perform the requested operation.
 *
 * @param h Duration in hours.
 *
 * @return Timeout delay value.
 */
#define K_HOURS(h)     K_MINUTES((h) * 60)

/**
 * @brief Generate infinite timeout delay.
 *
 * This macro generates a timeout delay that instructs a kernel API
 * to wait as long as necessary to perform the requested operation.
 *
 * @return Timeout delay value.
 */
#define K_FOREVER Z_FOREVER

#ifdef CONFIG_TIMEOUT_64BIT

/**
 * @brief Generates an absolute/uptime timeout value from system ticks
 *
 * This macro generates a timeout delay that represents an expiration
 * at the absolute uptime value specified, in system ticks.  That is, the
 * timeout will expire immediately after the system uptime reaches the
 * specified tick count.
 *
 * @param t Tick uptime value
 * @return Timeout delay value
 */
#define K_TIMEOUT_ABS_TICKS(t) \
	Z_TIMEOUT_TICKS(Z_TICK_ABS((k_ticks_t)MAX(t, 0)))

/**
 * @brief Generates an absolute/uptime timeout value from milliseconds
 *
 * This macro generates a timeout delay that represents an expiration
 * at the absolute uptime value specified, in milliseconds.  That is,
 * the timeout will expire immediately after the system uptime reaches
 * the specified tick count.
 *
 * @param t Millisecond uptime value
 * @return Timeout delay value
 */
#define K_TIMEOUT_ABS_MS(t) K_TIMEOUT_ABS_TICKS(k_ms_to_ticks_ceil64(t))

/**
 * @brief Generates an absolute/uptime timeout value from microseconds
 *
 * This macro generates a timeout delay that represents an expiration
 * at the absolute uptime value specified, in microseconds.  That is,
 * the timeout will expire immediately after the system uptime reaches
 * the specified time.  Note that timer precision is limited by the
 * system tick rate and not the requested timeout value.
 *
 * @param t Microsecond uptime value
 * @return Timeout delay value
 */
#define K_TIMEOUT_ABS_US(t) K_TIMEOUT_ABS_TICKS(k_us_to_ticks_ceil64(t))

/**
 * @brief Generates an absolute/uptime timeout value from nanoseconds
 *
 * This macro generates a timeout delay that represents an expiration
 * at the absolute uptime value specified, in nanoseconds.  That is,
 * the timeout will expire immediately after the system uptime reaches
 * the specified time.  Note that timer precision is limited by the
 * system tick rate and not the requested timeout value.
 *
 * @param t Nanosecond uptime value
 * @return Timeout delay value
 */
#define K_TIMEOUT_ABS_NS(t) K_TIMEOUT_ABS_TICKS(k_ns_to_ticks_ceil64(t))

/**
 * @brief Generates an absolute/uptime timeout value from system cycles
 *
 * This macro generates a timeout delay that represents an expiration
 * at the absolute uptime value specified, in cycles.  That is, the
 * timeout will expire immediately after the system uptime reaches the
 * specified time.  Note that timer precision is limited by the system
 * tick rate and not the requested timeout value.
 *
 * @param t Cycle uptime value
 * @return Timeout delay value
 */
#define K_TIMEOUT_ABS_CYC(t) K_TIMEOUT_ABS_TICKS(k_cyc_to_ticks_ceil64(t))

#endif


/**
 * @}
 */

/**
 * @cond INTERNAL_HIDDEN
 */

struct k_timer {
	/*
	 * _timeout structure must be first here if we want to use
	 * dynamic timer allocation. timeout.node is used in the double-linked
	 * list of free timers
	 */
	struct _timeout timeout;

	/* runs in ISR context */
	void (*expiry_fn)(struct k_timer *timer);

	/* runs in the context of the thread that calls k_timer_stop() */
	void (*stop_fn)(struct k_timer *timer);

	/* timer period */
	k_timeout_t period;

	/* timer status */
	uint32_t status;

	/* user-specific data, also used to support legacy features */
	void *user_data;
};

#define Z_TIMER_INITIALIZER(obj, expiry, stop) \
	{ \
	.timeout = { \
		.node = {},\
		.fn = z_timer_expiration_handler, \
		.dticks = 0, \
	}, \
	.expiry_fn = expiry, \
	.stop_fn = stop, \
	.status = 0, \
	.user_data = 0, \
	}

/**
 * INTERNAL_HIDDEN @endcond
 */

/**
 * @defgroup timer_apis Timer APIs
 * @ingroup kernel_apis
 * @{
 */

/**
 * @typedef k_timer_expiry_t
 * @brief Timer expiry function type.
 *
 * A timer's expiry function is executed by the system clock interrupt handler
 * each time the timer expires. The expiry function is optional, and is only
 * invoked if the timer has been initialized with one.
 *
 * @param timer     Address of timer.
 */
typedef void (*k_timer_expiry_t)(struct k_timer *timer);

/**
 * @typedef k_timer_stop_t
 * @brief Timer stop function type.
 *
 * A timer's stop function is executed if the timer is stopped prematurely.
 * The function runs in the context of call that stops the timer.  As
 * k_timer_stop() can be invoked from an ISR, the stop function must be
 * callable from interrupt context (isr-ok).
 *
 * The stop function is optional, and is only invoked if the timer has been
 * initialized with one.
 *
 * @param timer     Address of timer.
 */
typedef void (*k_timer_stop_t)(struct k_timer *timer);

/**
 * @brief Statically define and initialize a timer.
 *
 * The timer can be accessed outside the module where it is defined using:
 *
 * @code extern struct k_timer <name>; @endcode
 *
 * @param name Name of the timer variable.
 * @param expiry_fn Function to invoke each time the timer expires.
 * @param stop_fn   Function to invoke if the timer is stopped while running.
 */
#define K_TIMER_DEFINE(name, expiry_fn, stop_fn) \
	Z_DECL_ALIGN(struct k_timer) name = \
		Z_TIMER_INITIALIZER(name, expiry_fn, stop_fn)

/**
 * @brief Initialize a timer.
 *
 * This routine initializes a timer, prior to its first use.
 *
 * @param timer     Address of timer.
 * @param expiry_fn Function to invoke each time the timer expires.
 * @param stop_fn   Function to invoke if the timer is stopped while running.
 */
extern void k_timer_init(struct k_timer *timer,
			 k_timer_expiry_t expiry_fn,
			 k_timer_stop_t stop_fn);

/**
 * @brief Start a timer.
 *
 * This routine starts a timer, and resets its status to zero. The timer
 * begins counting down using the specified duration and period values.
 *
 * Attempting to start a timer that is already running is permitted.
 * The timer's status is reset to zero and the timer begins counting down
 * using the new duration and period values.
 *
 * @param timer     Address of timer.
 * @param duration  Initial timer duration.
 * @param period    Timer period.
 */
void k_timer_start(struct k_timer *timer,
			     k_timeout_t duration, k_timeout_t period);

/**
 * @brief Stop a timer.
 *
 * This routine stops a running timer prematurely. The timer's stop function,
 * if one exists, is invoked by the caller.
 *
 * Attempting to stop a timer that is not running is permitted, but has no
 * effect on the timer.
 *
 * @note The stop handler has to be callable from ISRs if @a k_timer_stop is to
 * be called from ISRs.
 *
 * @funcprops \isr_ok
 *
 * @param timer     Address of timer.
 */
void k_timer_stop(struct k_timer *timer);

/**
 * @brief Read timer status.
 *
 * This routine reads the timer's status, which indicates the number of times
 * it has expired since its status was last read.
 *
 * Calling this routine resets the timer's status to zero.
 *
 * @param timer     Address of timer.
 *
 * @return Timer status.
 */
uint32_t k_timer_status_get(struct k_timer *timer);


/**
 * @brief Get time remaining before a timer next expires, in system ticks
 *
 * This routine computes the time remaining before a running timer
 * next expires, in units of system ticks.  If the timer is not
 * running, it returns zero.
 */
static inline k_ticks_t k_timer_remaining_ticks(const struct k_timer *timer)
{
	return z_timeout_remaining(&timer->timeout);
}

/**
 * @brief Get time remaining before a timer next expires.
 *
 * This routine computes the (approximate) time remaining before a running
 * timer next expires. If the timer is not running, it returns zero.
 *
 * @param timer     Address of timer.
 *
 * @return Remaining time (in milliseconds).
 */
static inline uint32_t k_timer_remaining_get(struct k_timer *timer)
{
	return k_ticks_to_ms_floor32(k_timer_remaining_ticks(timer));
}


/**
 * @brief Associate user-specific data with a timer.
 *
 * This routine records the @a user_data with the @a timer, to be retrieved
 * later.
 *
 * It can be used e.g. in a timer handler shared across multiple subsystems to
 * retrieve data specific to the subsystem this timer is associated with.
 *
 * @param timer     Address of timer.
 * @param user_data User data to associate with the timer.
 */
static inline void k_timer_user_data_set(struct k_timer *timer, void *user_data)
{
	timer->user_data = user_data;
}

/**
 * @brief Retrieve the user-specific data from a timer.
 *
 * @param timer     Address of timer.
 *
 * @return The user data.
 */
static inline void *k_timer_user_data_get(const struct k_timer *timer)
{
	return timer->user_data;
}


/** @} */

/**
 * @addtogroup clock_apis
 * @ingroup kernel_apis
 * @{
 */

/**
 * @brief Get system uptime, in system ticks.
 *
 * This routine returns the elapsed time since the system booted, in
 * ticks (c.f. @kconfig{CONFIG_SYS_CLOCK_TICKS_PER_SEC}), which is the
 * fundamental unit of resolution of kernel timekeeping.
 *
 * @return Current uptime in ticks.
 */
int64_t k_uptime_ticks(void);

/**
 * @brief Get system uptime.
 *
 * This routine returns the elapsed time since the system booted,
 * in milliseconds.
 *
 * @note
 *    While this function returns time in milliseconds, it does
 *    not mean it has millisecond resolution. The actual resolution depends on
 *    @kconfig{CONFIG_SYS_CLOCK_TICKS_PER_SEC} config option.
 *
 * @return Current uptime in milliseconds.
 */
static inline int64_t k_uptime_get(void)
{
	return k_ticks_to_ms_floor64(k_uptime_ticks());
}

/**
 * @brief Get system uptime (32-bit version).
 *
 * This routine returns the lower 32 bits of the system uptime in
 * milliseconds.
 *
 * Because correct conversion requires full precision of the system
 * clock there is no benefit to using this over k_uptime_get() unless
 * you know the application will never run long enough for the system
 * clock to approach 2^32 ticks.  Calls to this function may involve
 * interrupt blocking and 64-bit math.
 *
 * @note
 *    While this function returns time in milliseconds, it does
 *    not mean it has millisecond resolution. The actual resolution depends on
 *    @kconfig{CONFIG_SYS_CLOCK_TICKS_PER_SEC} config option
 *
 * @return The low 32 bits of the current uptime, in milliseconds.
 */
static inline uint32_t k_uptime_get_32(void)
{
	return (uint32_t)k_uptime_get();
}

/**
 * @brief Get elapsed time.
 *
 * This routine computes the elapsed time between the current system uptime
 * and an earlier reference time, in milliseconds.
 *
 * @param reftime Pointer to a reference time, which is updated to the current
 *                uptime upon return.
 *
 * @return Elapsed time.
 */
static inline int64_t k_uptime_delta(int64_t *reftime)
{
	int64_t uptime, delta;

	uptime = k_uptime_get();
	delta = uptime - *reftime;
	*reftime = uptime;

	return delta;
}

/**
 * @brief Read the hardware clock.
 *
 * This routine returns the current time, as measured by the system's hardware
 * clock.
 *
 * @return Current hardware clock up-counter (in cycles).
 */
static inline uint32_t k_cycle_get_32(void)
{
	return arch_k_cycle_get_32();
}

/**
 * @brief Read the 64-bit hardware clock.
 *
 * This routine returns the current time in 64-bits, as measured by the
 * system's hardware clock, if available.
 *
 * @see CONFIG_TIMER_HAS_64BIT_CYCLE_COUNTER
 *
 * @return Current hardware clock up-counter (in cycles).
 */
static inline uint64_t k_cycle_get_64(void)
{
	if (!IS_ENABLED(CONFIG_TIMER_HAS_64BIT_CYCLE_COUNTER)) {
		__ASSERT(0, "64-bit cycle counter not enabled on this platform. "
			    "See CONFIG_TIMER_HAS_64BIT_CYCLE_COUNTER");
		return 0;
	}

	return arch_k_cycle_get_64();
}

/**
 * @internal
 */
extern void z_timer_expiration_handler(struct _timeout *t);

#endif /* INCLUDE_KERNEL_H */
