/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <kernel.h>
#include <drivers/gpio.h>
#include "gpio_utils.h"

struct gpio_wch_config {
    /* gpio_driver_config needs to be first */
	struct gpio_driver_config common;
	/* port base address */
	WCH_GPIO_Type *base;
    void (*irq_config_func)(const struct device *dev);
};

struct gpio_wch_data {
	/* gpio_driver_data needs to be first */
	struct gpio_driver_data common;
	/* user ISR cb */
	sys_slist_t cb;
};

#ifdef CONFIG_WCH_GPIO
__WCH_INT_FAST __HIGHCODE void GPIOA_IRQHandler(void)
{
    const struct gpio_wch_config *config = DEVICE_GET(gpioa)->cfg;
    struct gpio_wch_data *data = DEVICE_GET(gpioa)->data;
    uint32_t int_pins;

    int_pins = hal_gpio_int_flag_port_get(config->base);
    hal_gpio_int_flag_pins_clear(config->base, int_pins);

    gpio_fire_callbacks(&data->cb, DEVICE_GET(gpioa), int_pins);
}

__WCH_INT_FAST __HIGHCODE void GPIOB_IRQHandler(void)
{
    const struct gpio_wch_config *config = DEVICE_GET(gpiob)->cfg;
    struct gpio_wch_data *data = DEVICE_GET(gpiob)->data;
    uint32_t int_pins;

    int_pins = hal_gpio_int_flag_port_get(config->base);
    hal_gpio_int_flag_pins_clear(config->base, int_pins);

    gpio_fire_callbacks(&data->cb, DEVICE_GET(gpiob), int_pins);
}
#endif /* CONFIG_WCH_GPIO */

static int gpio_wch_flags_to_cfg(uint32_t flag, int *pin_cfg)
{
    if ((flag & GPIO_OUTPUT) != 0) {
        if ((flag & GPIO_SINGLE_ENDED) != 0) {
            /* open drain or open source */
            return -ENOTSUP;
        } else {
            /* push-pull mode */
            *pin_cfg = GPIO_MODE_OUT_PP_5MA;
        }
    } else if ((flag & GPIO_INPUT) != 0) {
        if ((flag & GPIO_PULL_UP) != 0) {
            *pin_cfg = GPIO_MODE_IN_PU;
        } else if ((flag & GPIO_PULL_DOWN) != 0) {
            *pin_cfg = GPIO_MODE_IN_PD;
        } else {
            *pin_cfg = GPIO_MODE_IN_FLOATING;
        }
    } else {
        //TODO: analog deal
        // *pin_cfg = GPIO_MODE_ANALOG;
    }

    return 0;
}

static int gpio_wch_port_get_raw(const struct device *port, gpio_port_value_t *value)
{
    const struct gpio_wch_config *cfg = port->cfg;
    WCH_GPIO_Type *gpio = (WCH_GPIO_Type *)cfg->base;

    *value = hal_gpio_port_get(gpio);

    return 0;
}

static int gpio_wch_port_set_masked_raw(const struct device *port, 
                                        gpio_port_pins_t mask, 
                                        gpio_port_value_t value)
{
    const struct gpio_wch_config *cfg = port->cfg;
    WCH_GPIO_Type *gpio = (WCH_GPIO_Type *)cfg->base;
    uint32_t get_val;

    get_val = hal_gpio_port_get(gpio);

    hal_gpio_pins_set(gpio, (mask & value) | (get_val & ~mask));

    return 0;
}

static int gpio_wch_port_set_bits_raw(const struct device *port, gpio_port_pins_t pins)
{
    const struct gpio_wch_config *cfg = port->cfg;
    WCH_GPIO_Type *gpio = (WCH_GPIO_Type *)cfg->base;

    hal_gpio_pins_set(gpio, pins);

    return 0;
}

static int gpio_wch_port_clear_bits_raw(const struct device *port, gpio_port_pins_t pins)
{
    const struct gpio_wch_config *cfg = port->cfg;
    WCH_GPIO_Type *gpio = (WCH_GPIO_Type *)cfg->base;

    hal_gpio_pins_reset(gpio, pins);

    return 0;
}

static int gpio_wch_port_toggle_bits(const struct device *port, gpio_port_pins_t pins)
{
    const struct gpio_wch_config *cfg = port->cfg;
    WCH_GPIO_Type *gpio = (WCH_GPIO_Type *)cfg->base;

    hal_gpio_pins_invert(gpio, pins);

    return 0;
}

static int gpio_wch_pin_configure(const struct device *port, gpio_pin_t pin,
			                      gpio_flags_t flags)
{
    const struct gpio_wch_config *cfg = port->cfg;
    WCH_GPIO_Type *gpio = (WCH_GPIO_Type *)cfg->base;
    int err;
    int pin_cfg = 0;

    err = gpio_wch_flags_to_cfg(flags, &pin_cfg);

    if (err != 0) {
        return err;
    }

    if ((flags & GPIO_OUTPUT) != 0) {
        if ((flags & GPIO_OUTPUT_INIT_LOW) != 0) {
            gpio_wch_port_set_bits_raw(port, BIT(pin));
        } else if ((flags & GPIO_OUTPUT_INIT_HIGH) != 0) {
            gpio_wch_port_clear_bits_raw(port, BIT(pin));
        }
    }

    hal_gpio_mode_config(gpio, BIT(pin), pin_cfg);
    
    return 0;
}

static int gpio_wch_pin_interrupt_configure(const struct device *port, gpio_pin_t pin,
                                            enum gpio_int_mode mode, 
                                            enum gpio_int_trig trig)
{
    const struct gpio_wch_config *cfg = port->cfg;
    WCH_GPIO_Type *gpio = (WCH_GPIO_Type *)cfg->base;
    
    if (mode == GPIO_INT_MODE_DISABLED) {
        hal_gpio_int_pins_disable(gpio, BIT(pin));
    } else if (mode == GPIO_INT_MODE_LEVEL) {
        if (trig == GPIO_INT_TRIG_LOW) {
            hal_gpio_int_mode_config(gpio, BIT(pin), GPIO_IT_MODE_LOWLEVEL);
        } else if (trig == GPIO_INT_TRIG_HIGH) {
            hal_gpio_int_mode_config(gpio, BIT(pin), GPIO_IT_MODE_HIGHLEVEL);
        } else {
            return -ENOTSUP;
        }
    } else if (mode == GPIO_INT_MODE_EDGE) {
        if (trig == GPIO_INT_TRIG_LOW) {
            hal_gpio_int_mode_config(gpio, BIT(pin), GPIO_IT_MODE_FALLEDGE);
        } else if (trig == GPIO_INT_TRIG_HIGH) {
            hal_gpio_int_mode_config(gpio, BIT(pin), GPIO_IT_MODE_RISEEDGE);
        } else {
            return -ENOTSUP;
        }
    } else {
        return -ENOTSUP;
    }

    return 0;
}

static int gpio_wch_manage_callback(const struct device *port,
                                    struct gpio_callback *cb, bool set)
{
    struct gpio_wch_data *data = port->data;

	return gpio_manage_callback(&data->cb, cb, set);

}

static uint32_t gpio_wch_get_pending_int(const struct device *port)
{

    return 0;
}

static const struct gpio_driver_api gpio_wch_driver_api = {
    .pin_configure = gpio_wch_pin_configure,
    .port_get_raw = gpio_wch_port_get_raw,
    .port_set_masked_raw = gpio_wch_port_set_masked_raw,
    .port_set_bits_raw = gpio_wch_port_set_bits_raw,
    .port_clear_bits_raw = gpio_wch_port_clear_bits_raw,
    .port_toggle_bits = gpio_wch_port_toggle_bits,
    .pin_interrupt_configure = gpio_wch_pin_interrupt_configure,
    .manage_callback = gpio_wch_manage_callback,
    .get_pending_int = gpio_wch_get_pending_int,
};

static int gpio_wch_init(const struct device *dev)
{
    const struct gpio_wch_config *cfg = dev->cfg;

    cfg->irq_config_func(dev);

    for(int i = 0; i < 32; i++) {
        gpio_wch_pin_configure(dev, i, GPIO_INPUT | GPIO_PULL_UP);
    }

    return 0;
}

#define GPIO_INIT(index)      \
    static void gpio_wch_irq_cfg_func_##index(const struct device *dev)     \
    {       \
        irq_enable(DEV_CFG_GET_IRQ(gpio##index));       \
    }       \
    static struct gpio_wch_config gpio_cfg_##index = {       \
        .common = {     \
            .port_pin_mask = DEV_CFG_GET(gpio##index, port_pin_mask),   \
        },      \
        .base = DEV_CFG_GET(gpio##index, reg),      \
        .irq_config_func = gpio_wch_irq_cfg_func_##index,       \
    };      \
    static struct gpio_wch_data gpio_data_##index;       \
    static struct device_state gpio_sta_##index = {0};          \
    DEVICE_DEFINE(gpio##index, gpio_wch_init, NULL,        \
            &gpio_data_##index,     \
            &gpio_cfg_##index,      \
            &gpio_wch_driver_api,       \
            &gpio_sta_##index,          \
            DRIVER,         \
            0);  


#ifdef CONFIG_WCH_GPIO
GPIO_INIT(a);
GPIO_INIT(b);
#endif