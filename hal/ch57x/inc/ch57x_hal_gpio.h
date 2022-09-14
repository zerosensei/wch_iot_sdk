/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_GPIO_H
#define HAL_CH57X_INC_CH57X_HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch57x_common.h"

typedef struct 
{
    volatile uint32_t DIR;
    volatile uint32_t PIN;
    volatile uint32_t OUT;
    volatile uint32_t CLR;
    volatile uint32_t PU;
    volatile uint32_t PD_DRV;
} WCH_GPIO_Type;

#define GPIO_A          ((WCH_GPIO_Type *)BA_PA)
#define GPIO_B          ((WCH_GPIO_Type *)BA_PB)

/**
 * @brief	GPIO_pins_define
 */
#define GPIO_Pin_0      (0x00000001) /*!< Pin 0 selected */
#define GPIO_Pin_1      (0x00000002) /*!< Pin 1 selected */
#define GPIO_Pin_2      (0x00000004) /*!< Pin 2 selected */
#define GPIO_Pin_3      (0x00000008) /*!< Pin 3 selected */
#define GPIO_Pin_4      (0x00000010) /*!< Pin 4 selected */
#define GPIO_Pin_5      (0x00000020) /*!< Pin 5 selected */
#define GPIO_Pin_6      (0x00000040) /*!< Pin 6 selected */
#define GPIO_Pin_7      (0x00000080) /*!< Pin 7 selected */
#define GPIO_Pin_8      (0x00000100) /*!< Pin 8 selected */
#define GPIO_Pin_9      (0x00000200) /*!< Pin 9 selected */
#define GPIO_Pin_10     (0x00000400) /*!< Pin 10 selected */
#define GPIO_Pin_11     (0x00000800) /*!< Pin 11 selected */
#define GPIO_Pin_12     (0x00001000) /*!< Pin 12 selected */
#define GPIO_Pin_13     (0x00002000) /*!< Pin 13 selected */
#define GPIO_Pin_14     (0x00004000) /*!< Pin 14 selected */
#define GPIO_Pin_15     (0x00008000) /*!< Pin 15 selected */
#define GPIO_Pin_16     (0x00010000) /*!< Pin 16 selected */
#define GPIO_Pin_17     (0x00020000) /*!< Pin 17 selected */
#define GPIO_Pin_18     (0x00040000) /*!< Pin 18 selected */
#define GPIO_Pin_19     (0x00080000) /*!< Pin 19 selected */
#define GPIO_Pin_20     (0x00100000) /*!< Pin 20 selected */
#define GPIO_Pin_21     (0x00200000) /*!< Pin 21 selected */
#define GPIO_Pin_22     (0x00400000) /*!< Pin 22 selected */
#define GPIO_Pin_23     (0x00800000) /*!< Pin 23 selected */
#define GPIO_Pin_All    (0xFFFFFFFF) /*!< All pins selected */

typedef enum {
    GPIO_MODE_IN_FLOATING,
    GPIO_MODE_IN_PU,	
    GPIO_MODE_IN_PD,	
    GPIO_MODE_OUT_PP_5MA,
    GPIO_MODE_OUT_PP_20MA,
    GPIO_MODE_ANALOG,
} gpio_mode_t;

typedef enum {
    GPIO_IT_MODE_LOWLEVEL,	
    GPIO_IT_MODE_HIGHLEVEL,	
    GPIO_IT_MODE_FALLEDGE,	
    GPIO_IT_MODE_RISEEDGE,	
} gpio_int_mode_t;

typedef enum {
    GPIO_ALT_TMR0 = RB_PIN_TMR0,
    GPIO_ALT_TMR1 = RB_PIN_TMR1,
    GPIO_ALT_TMR2 = RB_PIN_TMR2,
    GPIO_ALT_UART0 = RB_PIN_UART0,
    GPIO_ALT_UART1 = RB_PIN_UART1,
    GPIO_ALT_SPI0 = RB_PIN_SPI0,
} gpio_alernate_perph_t;

static inline void hal_gpio_alernate_enable(uint16_t perph)
{
    R16_PIN_ALTERNATE |= perph;
}

static inline void hal_gpio_alernate_disable(uint16_t perph)
{
    R16_PIN_ALTERNATE &= ~perph;
}

typedef enum {
    GPIO_ANALOG_USB_DP_PU = RB_PIN_USB_DP_PU,
    GPIO_ANALOG_USB = RB_PIN_USB_IE,
    GPIO_ANALOG_ADC_8_9 = RB_PIN_ADC8_9_IE,
    GPIO_ANALOG_ADC_0 = RB_PIN_ADC0_IE,
    GPIO_ANALOG_ADC_1 = RB_PIN_ADC1_IE,
    GPIO_ANALOG_ADC_12 = RB_PIN_ADC12_IE,
    GPIO_ANALOG_ADC_13 = RB_PIN_ADC13_IE,
    GPIO_ANALOG_XT32K = RB_PIN_XT32K_IE,
    GPIO_ANALOG_ADC_2_3 = RB_PIN_ADC2_3_IE,
    GPIO_ANALOG_ADC_4_5 = RB_PIN_ADC4_5_IE,
} gpio_analog_perph_t;

static inline void hal_gpio_analog_enable(uint16_t perph)
{
    R16_PIN_ANALOG_IE |= perph;
}

static inline void hal_gpio_analog_disable(uint16_t perph)
{
    R16_PIN_ANALOG_IE &= ~(perph);
}

static inline void hal_gpio_pins_reset(WCH_GPIO_Type *port, uint32_t pins)
{
    port->CLR |= pins;
}

static inline void hal_gpio_pins_set(WCH_GPIO_Type *port, uint32_t pins)
{
    port->OUT |= pins;
}

static inline void hal_gpio_pins_invert(WCH_GPIO_Type *port, uint32_t pins)
{
    port->OUT ^= pins;
}

static inline uint32_t hal_gpio_port_get(WCH_GPIO_Type *port)
{
    return port->PIN;
}

static inline uint32_t hal_gpio_pins_get(WCH_GPIO_Type *port, uint32_t pins)
{
    return port->PIN & (pins);
}

static inline void hal_gpio_int_pins_enable(WCH_GPIO_Type *port, uint32_t pins)
{
    if (port == GPIO_A) {
        R16_PA_INT_EN |= pins & WCH_MASK(16);
    } else if (port == GPIO_B) {
        R16_PB_INT_EN |= (pins | ((pins & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14)) & WCH_MASK(16);
    }
}

static inline void hal_gpio_int_pins_disable(WCH_GPIO_Type *port, uint32_t pins)
{
    if (port == GPIO_A) {
        R16_PA_INT_EN &= ~(pins);
    } else if (port == GPIO_B) {
        R16_PB_INT_EN &= ~(pins | ((pins & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14));
    }
}

static inline uint32_t hal_gpio_int_flag_port_get(WCH_GPIO_Type *port)
{
    if (port == GPIO_A)  {
        return R16_PA_INT_IF;
    } else if (port == GPIO_B) {
        return ((R16_PB_INT_IF & (~((GPIO_Pin_22 | GPIO_Pin_23) >> 14))) 
            | ((R16_PB_INT_IF << 14) & (GPIO_Pin_22 | GPIO_Pin_23)));
    } else {
        return 0;
    }
}

static inline uint32_t hal_gpio_int_flag_pins_get(WCH_GPIO_Type *port, uint32_t pins)
{
    if (port == GPIO_A)  {
        return R16_PA_INT_IF & (pins);
    } else if (port == GPIO_B) {
        return (R16_PB_INT_IF & ((pins) | (((pins) & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14)));
    } else {
        return 0;
    }
}

static inline void hal_gpio_int_flag_pins_clear(WCH_GPIO_Type *port, uint32_t pins)
{
    if (port == GPIO_A)  {
        R16_PA_INT_IF = pins & WCH_MASK(16);
    } else if (port == GPIO_B) {
        R16_PB_INT_IF = ((pins) | (((pins) & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14)) & WCH_MASK(16);
    }
}

void hal_gpio_mode_config(WCH_GPIO_Type *port, uint32_t pins, gpio_mode_t mode);
void hal_gpio_int_mode_config(WCH_GPIO_Type *port, uint32_t pins, gpio_int_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* HAL_CH57X_INC_CH57X_HAL_GPIO_H */