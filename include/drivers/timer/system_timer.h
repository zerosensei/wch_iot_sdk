/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Timer driver API
 *
 * Declare API implemented by system timer driver and used by kernel components.
 */

#ifndef INCLUDE_DRIVERS_TIMER_SYSTEM_TIMER_H
#define INCLUDE_DRIVERS_TIMER_SYSTEM_TIMER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clock APIs
 * @defgroup clock_apis Clock APIs
 * @{
 */

/**
 * @brief Set system clock ticks
 *
 * @param ticks Timeout in tick units
 * @param idle Hint to the driver that the system is about to enter
 *        the idle state immediately after setting the timeout
 */
extern void sys_clock_set_ticks(uint32_t ticks, bool idle);

/**
 * @brief Ticks elapsed since last timeout tiggered
 *
 */
extern uint32_t sys_clock_elapsed(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_TIMER_SYSTEM_TIMER_H */
