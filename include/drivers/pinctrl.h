/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_DRIVERS_PINCTRL_H
#define INCLUDE_DRIVERS_PINCTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <device.h>

typedef uint32_t pinctrl_config;

/** Pin controller configuration for a given device. */
struct pinctrl_dev_config {
	/** List of configurations. */
	const pinctrl_config *cfg;
	/** Number of configurations. */
	uint8_t cnt;
};

/**
 * @brief Pin configration bit filed.
 * 
 * Fields:
 * 
 * - 0..4:   Line number.
 * - 5..8:   Port number.
 * - 9..10:  Pin mode.
 * - 11..12: PUPD config.
 * - 13:     Output data.
 * - 14:     Alternate
 * - 15:     Reserved.
 * - 16..31: Pin alternate/analog function.
 */

#define WCH_PIN_POS             (0U)
#define WCH_PIN_MSK             (BIT_MASK(5) << WCH_PIN_POS)

#define WCH_PORT_POS            (5U)
#define WCH_PORT_MSK            (BIT_MASK(4) << WCH_PORT_POS)

#define WCH_MODE_POS            (9U)
#define WCH_MODE_MSK            (BIT_MASK(2) << WCH_MODE_POS)

#define WCH_PUPD_POS            (11U)
#define WCH_PUPD_MSK            (BIT_MASK(2) << WCH_PUPD_POS)

#define WCH_OUT_DATA_POS        (13U)
#define WCH_OUT_DATA_MSK        (BIT_MASK(1) << WCH_OUT_DATA_POS)

#define WCH_ALT_POS             (14U)
#define WCH_ALT_MSK             (BIT_MASK(1) << WCH_ALT_POS)

#define WCH_FUN_POS             (16U)
#define WCH_FUN_MSK             (BIT_MASK(16) << WCH_FUN_POS)

/* GPIO mode*/
#define WCH_MODE_IN             (0U << WCH_MODE_POS)
#define WCH_MODE_OUT            (1U << WCH_MODE_POS)
#define WCH_MODE_ANALOG         (2U << WCH_MODE_POS)

/* GPIO input type */
#define WCH_PUPD_NO_PULL        (0U << WCH_PUPD_POS)
#define WCH_PUPD_PULL_UP        (1U << WCH_PUPD_POS)
#define WCH_PUPD_PULL_DOWN      (2U << WCH_PUPD_POS)

/* GPIO output type*/
#define WCH_OUTPUT_LOW          (0U << WCH_OUT_DATA_POS)
#define WCH_OUTPUT_HIGH         (1U << WCH_OUT_DATA_POS)

/* GPIO function */
#define WCH_ANALOG_USB_DP_PU    (0U << WCH_FUN_POS)
#define WCH_ANALOG_USB          (1U << WCH_FUN_POS)
#define WCH_ANALOG_ADC_8_9      (2U << WCH_FUN_POS)
#define WCH_ANALOG_ADC_0        (3U << WCH_FUN_POS)
#define WCH_ANALOG_ADC_1        (4U << WCH_FUN_POS)
#define WCH_ANALOG_ADC_12       (5U << WCH_FUN_POS)
#define WCH_ANALOG_ADC_13       (6U << WCH_FUN_POS)
#define WCH_ANALOG_XT32K        (7U << WCH_FUN_POS)
#define WCH_ANALOG_ADC_2_3      (8U << WCH_FUN_POS)
#define WCH_ANALOG_ADC_4_5      (9U << WCH_FUN_POS)

#define WCH_ALT_TMR0            (10U << WCH_FUN_POS)
#define WCH_ALT_TMR1            (11U << WCH_FUN_POS)
#define WCH_ALT_TMR2            (12U << WCH_FUN_POS)
#define WCH_ALT_UART0           (13U << WCH_FUN_POS)
#define WCH_ALT_UART1           (14U << WCH_FUN_POS)
#define WCH_ALT_SPI0            (15U << WCH_FUN_POS)

#define WCH_PINMUX(port, line, mode)                \
    (((line) << WCH_PIN_POS) & WCH_PIN_MSK)         \
    | (((port - 'A') << WCH_PORT_POS) & WCH_PORT_MSK ) \
    | ((mode) & WCH_MODE_MSK)        


int pinctrl_configure_pins(const pinctrl_config *pins, uint8_t pin_cnt);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_PINCTRL_H */