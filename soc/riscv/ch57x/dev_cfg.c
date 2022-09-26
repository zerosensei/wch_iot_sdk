/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include "dev_cfg.h"
#include <pm/device.h>


/* UART */
// #ifdef CONFIG_WCH_UART_0
// struct device __device_uart0 = {0};
// #endif

// #ifdef CONFIG_WCH_UART_1
// struct device __device_uart1 = {0};
// #endif

// #ifdef CONFIG_WCH_UART_2
// struct device __device_uart2 = {0};
// #endif

// #ifdef CONFIG_WCH_UART_3
// struct device __device_uart3 = {0};
// #endif


/* GPIO */
#ifdef CONFIG_PM
struct pm_device _pm_gpio = {
    .flags = ATOMIC_INIT(PM_DEVICE_FLAG_WS_CAPABLE),
};
#endif
// struct device __device_gpioa = {
// #ifdef CONFIG_PM
//     .pm = &_pm_gpio,
// #endif 
// };
// struct device __device_gpiob = {
// #ifdef CONFIG_PM
//     .pm = &_pm_gpio,
// #endif 
// };

/* RTC */
// #ifdef CONFIG_PM
// struct pm_device _pm_rtc = {
//     .flags = ATOMIC_INIT(PM_DEVICE_FLAG_WS_CAPABLE),
// };
// #endif
// struct device __device_rtc = {
// #ifdef CONFIG_PM
//     .pm = &_pm_rtc,
// #endif
// };

// /* USB */
// #ifdef CONFIG_PM
// struct pm_device _pm_usb = {
//     .flags = ATOMIC_INIT(PM_DEVICE_FLAG_WS_CAPABLE),
// };
// #endif
// struct device __device_usb = {
// #ifdef CONFIG_PM
//     .pm = &_pm_usb,
// #endif
// };

/* BAT */
// #ifdef CONFIG_PM
// struct pm_device _pm_bat = {
//     .flags = ATOMIC_INIT(PM_DEVICE_FLAG_WS_CAPABLE),
// };
// #endif
// struct device __device_bat = {
// #ifdef CONFIG_PM
//     .pm = &_pm_bat,
// #endif
// };