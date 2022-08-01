/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <kernel.h>
#include <drivers/pinctrl.h>

static WCH_GPIO_Type * const wch_port[] = {
    GPIO_A,
    GPIO_B,
};

int pinctrl_configure_pins(const pinctrl_config *pins, uint8_t pin_cnt)
{
    uint32_t pinmux;
    WCH_GPIO_Type *port;
    uint32_t pin;
    gpio_mode_t mode;

    for (int i = 0; i < pin_cnt; i++) {
        pinmux = pins[i];
        port = wch_port[(pinmux & WCH_PORT_MSK) >> WCH_PORT_POS];
        pin = BIT((pinmux & WCH_PIN_MSK) >> WCH_PIN_POS);

        if (pinmux & WCH_ALT_MSK) {
            switch (pinmux & WCH_FUN_MSK) {
            case WCH_ALT_TMR0:
                hal_gpio_alernate_enable(GPIO_ALT_TMR0);
                break;
            case WCH_ALT_TMR1:
                hal_gpio_alernate_enable(GPIO_ALT_TMR1);
                break;
            case WCH_ALT_TMR2:
                hal_gpio_alernate_enable(GPIO_ALT_TMR2);
                break;
            case WCH_ALT_UART0:
                hal_gpio_alernate_enable(GPIO_ALT_UART0);
                break;
            case WCH_ALT_UART1:
                hal_gpio_alernate_enable(GPIO_ALT_UART1);
                break;
            case WCH_ALT_SPI0:
                hal_gpio_alernate_enable(GPIO_ALT_SPI0);
                break;
            default:
                return -ENOTSUP;
            }
        }
        
        switch (pinmux & WCH_MODE_MSK) {
        case WCH_MODE_IN:
            if ((pinmux & WCH_PUPD_MSK) == WCH_PUPD_NO_PULL) {
                mode = GPIO_MODE_IN_FLOATING;
            } else if ((pinmux & WCH_PUPD_MSK) == WCH_PUPD_PULL_UP) {
                mode = GPIO_MODE_IN_PU;
            } else if ((pinmux & WCH_PUPD_MSK) == WCH_PUPD_PULL_DOWN) {
                mode = GPIO_MODE_IN_PD;
            } else {
                return -ENOTSUP;
            }
            hal_gpio_mode_config(port, pin, mode);
            break;
        case WCH_MODE_OUT:
            mode = GPIO_MODE_OUT_PP_5MA;
            if ((pinmux & WCH_OUT_DATA_MSK) == WCH_OUTPUT_LOW) {
                hal_gpio_pins_reset(port, pin);
            } else if ((pinmux & WCH_OUT_DATA_MSK) == WCH_OUTPUT_HIGH) {
                hal_gpio_pins_set(port, pin);
            } else {
                return -ENOTSUP;
            }
            hal_gpio_mode_config(port, pin, mode);
            break;
        case WCH_MODE_ANALOG:
            switch (pinmux & WCH_FUN_MSK) {
            case WCH_ANALOG_USB_DP_PU:
                hal_gpio_analog_enable(GPIO_ANALOG_USB_DP_PU);
                break;
            case WCH_ANALOG_USB:
                hal_gpio_analog_enable(GPIO_ANALOG_USB);
                break;
            case WCH_ANALOG_ADC_8_9:
                hal_gpio_analog_enable(GPIO_ANALOG_ADC_8_9);
                break;
            case WCH_ANALOG_ADC_0:
                hal_gpio_analog_enable(GPIO_ANALOG_ADC_0);
                break;
            case WCH_ANALOG_ADC_1:
                hal_gpio_analog_enable(GPIO_ANALOG_ADC_1);
                break;
            case WCH_ANALOG_ADC_12:
                hal_gpio_analog_enable(GPIO_ANALOG_ADC_12);
                break;
            case WCH_ANALOG_ADC_13:
                hal_gpio_analog_enable(GPIO_ANALOG_ADC_13);
                break;
            case WCH_ANALOG_XT32K:
                hal_gpio_analog_enable(GPIO_ANALOG_XT32K);
                break;
            case WCH_ANALOG_ADC_2_3:
                hal_gpio_analog_enable(GPIO_ANALOG_ADC_2_3);
                break;
            case WCH_ANALOG_ADC_4_5:
                hal_gpio_analog_enable(GPIO_ANALOG_ADC_4_5);
                break;
            default:
                return -ENOTSUP;
                break;
            }
        default:
            return -ENOTSUP;
            break;
        }

    }

    return 0;
}