/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include "pin_cfg.h"

#ifdef CONFIG_UART_0_ENABLE
const pinctrl_config __cfgs_uart0[] = WCH_UART0_PINS;
const struct pinctrl_dev_config __pinctrl_uart0 = {
    .cfg = __cfgs_uart0,
    .cnt = ARRAY_SIZE(__cfgs_uart0),
};
#endif

#ifdef CONFIG_UART_1_ENABLE
const pinctrl_config __cfgs_uart1[] = WCH_UART1_PINS;
const struct pinctrl_dev_config __pinctrl_uart1 = {
    .cfg = __cfgs_uart1,
    .cnt = ARRAY_SIZE(__cfgs_uart1),
};
#endif

#ifdef CONFIG_UART_2_ENABLE
const pinctrl_config __cfgs_uart2[] = WCH_UART2_PINS;
const struct pinctrl_dev_config __pinctrl_uart2 = {
    .cfg = __cfgs_uart2,
    .cnt = ARRAY_SIZE(__cfgs_uart2),
};
#endif

#ifdef CONFIG_UART_3_ENABLE
const pinctrl_config __cfgs_uart3[] = WCH_UART3_PINS;
const struct pinctrl_dev_config __pinctrl_uart3 = {
    .cfg = __cfgs_uart3,
    .cnt = ARRAY_SIZE(__cfgs_uart3),
};
#endif