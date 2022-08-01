/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CONFIG_PIN_CFG_H
#define CONFIG_PIN_CFG_H

#include <drivers/pinctrl.h>

#define DEV_PIN

/* UART pin config */
#define WCH_UART0_TX            		(WCH_PINMUX('B', 7, WCH_MODE_OUT) | WCH_OUTPUT_HIGH)
#define WCH_UART0_RX            		(WCH_PINMUX('B', 4, WCH_MODE_IN) | WCH_PUPD_PULL_UP)
#define WCH_UART0_TX_ALT				(WCH_PINMUX('A', 14, WCH_MODE_OUT) | WCH_OUTPUT_HIGH | WCH_ALT_MSK)
#define WCH_UART0_RX_ALT				(WCH_PINMUX('A', 15, WCH_MODE_IN) | WCH_PUPD_PULL_UP | WCH_ALT_MSK)
#define WCH_UART0_CTS					(WCH_PINMUX('B', 0, WCH_MODE_IN) | WCH_PUPD_PULL_UP)
#define WCH_UART0_RTS					(WCH_PINMUX('B', 6, WCH_MODE_OUT) | WCH_OUTPUT_LOW)

#define WCH_UART1_TX            		(WCH_PINMUX('A', 9, WCH_MODE_OUT) | WCH_OUTPUT_HIGH)
#define WCH_UART1_RX            		(WCH_PINMUX('A', 8, WCH_MODE_IN) | WCH_PUPD_PULL_UP)
#define WCH_UART1_TX_ALT        		(WCH_PINMUX('B', 13, WCH_MODE_OUT) | WCH_OUTPUT_HIGH)
#define WCH_UART1_RX_ALT        		(WCH_PINMUX('B', 12, WCH_MODE_IN) | WCH_PUPD_PULL_UP)

#define WCH_UART2_TX            		(WCH_PINMUX('B', 23, WCH_MODE_OUT) | WCH_OUTPUT_HIGH)
#define WCH_UART2_RX            		(WCH_PINMUX('B', 22, WCH_MODE_IN) | WCH_PUPD_PULL_UP)

#define WCH_UART3_TX            		(WCH_PINMUX('A', 5, WCH_MODE_OUT) | WCH_OUTPUT_HIGH)
#define WCH_UART3_RX            		(WCH_PINMUX('A', 4, WCH_MODE_IN) | WCH_PUPD_PULL_UP)

/* UART pins */
#define WCH_UART0_PINS					{WCH_UART0_TX, WCH_UART0_RX}
#define WCH_UART0_PINS_ALT				{WCH_UART0_TX_ALT, WCH_UART0_RX_ALT}
#define WCH_UART0_PINS_MODEM			{WCH_UART0_CTS, WCH_UART0_RTS}

#define WCH_UART1_PINS					{WCH_UART1_TX, WCH_UART1_RX}
#define WCH_UART1_PINS_ALT				{WCH_UART1_TX_ALT, WCH_UART1_RX_ALT}

#define WCH_UART2_PINS					{WCH_UART2_TX, WCH_UART2_RX}
#define WCH_UART3_PINS					{WCH_UART3_TX, WCH_UART3_RX}



#ifdef CONFIG_WCH_UART_0
extern const struct pinctrl_dev_config __pinctrl_uart0;;
#endif

#ifdef CONFIG_WCH_UART_1
extern const struct pinctrl_dev_config __pinctrl_uart1;
#endif

#ifdef CONFIG_WCH_UART_2
extern const struct pinctrl_dev_config __pinctrl_uart2;;
#endif

#ifdef CONFIG_WCH_UART_3
extern const struct pinctrl_dev_config __pinctrl_uart3;;
#endif

#endif /* CONFIG_PIN_CFG_H */