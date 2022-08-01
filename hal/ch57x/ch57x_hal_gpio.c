/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch57x_common.h>

void hal_gpio_mode_config(WCH_GPIO_Type *port, uint32_t pins, gpio_mode_t mode)
{
    switch (mode) {
    case GPIO_MODE_IN_FLOATING:
        port->PD_DRV &= ~pins;
        port->PU &= ~pins;
        port->DIR &= ~pins;
        break;
    case GPIO_MODE_IN_PU:
        port->PD_DRV &= ~pins;
        port->PU &= ~pins;
        port->DIR &= ~pins;
        break;
    case GPIO_MODE_IN_PD:
        port->PD_DRV |= pins;
        port->PU &= ~pins;
        port->DIR &= ~pins;
        break;
    case GPIO_MODE_OUT_PP_5MA:
        port->PD_DRV &= ~pins;
        port->DIR |= pins;
        break;  
    case GPIO_MODE_OUT_PP_20MA:
        port->PD_DRV |= pins;
        port->DIR |= pins;
        break;
    default:
        break;
    }
}

void hal_gpio_int_mode_config(WCH_GPIO_Type *port, uint32_t pins, gpio_int_mode_t mode)
{
    if (port == GPIO_A) {
        switch (mode) {
        case GPIO_IT_MODE_LOWLEVEL:
            R16_PA_INT_MODE &= ~pins;
            R32_PA_CLR |= pins;
            break;
        case GPIO_IT_MODE_HIGHLEVEL:
            R16_PA_INT_MODE &= ~pins;
            R32_PA_OUT |= pins;
            break;
        case GPIO_IT_MODE_FALLEDGE:
            R16_PA_INT_MODE |= pins;
            R32_PA_CLR |= pins;
            break;
        case GPIO_IT_MODE_RISEEDGE:
            R16_PA_INT_MODE |= pins;
            R32_PA_OUT |= pins;
            break;
        default:
            break;
        }

        R16_PA_INT_IF = pins;
        R16_PA_INT_EN |= pins;
    } else if (port == GPIO_B) {
        uint32_t pins_b = pins | ((pins & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14);

        switch (mode) {
            case GPIO_IT_MODE_LOWLEVEL:
                R16_PB_INT_MODE &= ~pins_b;
                R32_PB_CLR |= pins;
                break;
            case GPIO_IT_MODE_HIGHLEVEL:
                R16_PB_INT_MODE &= ~pins_b;
                R32_PB_OUT |= pins;
                break;
            case GPIO_IT_MODE_FALLEDGE:
                R16_PB_INT_MODE |= pins_b;
                R32_PB_CLR |= pins;
                break;
            case GPIO_IT_MODE_RISEEDGE:
                R16_PB_INT_MODE |= pins_b;
                R32_PB_OUT |= pins;
                break;
            default:
                break;
        }

        R16_PB_INT_IF = pins_b;
        R16_PB_INT_EN |= pins_b;
    }
}
