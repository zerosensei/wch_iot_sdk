/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/uart.h>
#include <sys/printk.h>
#include <drivers/timer/system_timer.h>

#define BUTTON_PIN			22
#define LED_PIN				15

const struct device *button = DEVICE_GET(gpiob);
const struct device *led = DEVICE_GET(gpiob);

static struct gpio_callback button_cb_data;

extern int sys_clock_driver_init(const struct device *dev);
struct gpio_wch_config {
    /* gpio_driver_config needs to be first */
	struct gpio_driver_config common;
	/* port base address */
	WCH_GPIO_Type *base;
};
void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed\n");
}

int main(void)
{
    int ret;

	printk("This is button example\n");

    if (!device_is_ready(button)) {
        printk("Error: button device is not ready\n");
        while(1);
    }

    ret = gpio_pin_configure(button, BUTTON_PIN, GPIO_INPUT | GPIO_PULL_UP);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d\n",
		       ret, BUTTON_PIN);
		while(1);
	}

	ret = gpio_pin_interrupt_configure(button, BUTTON_PIN,
					      GPIO_INT_EDGE_FALLING);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on pin %d\n",
			ret, BUTTON_PIN);
		while(1);
	}

    gpio_init_callback(&button_cb_data, button_pressed, BIT(BUTTON_PIN));
    gpio_add_callback(button, &button_cb_data);
	printk("Set up button at pin %d\n", BUTTON_PIN);

    if (!device_is_ready(led)) {
        printk("Error: LED device is not ready\n");
        while(1);
    }

	gpio_pin_configure(led, LED_PIN, GPIO_OUTPUT_LOW);

	printk("Press the button\n");

	while(1) {
		int val = gpio_pin_get(button, BUTTON_PIN);

		if (val >= 0) {
			gpio_pin_set(led, LED_PIN, val);
		}

		k_busy_wait(10*1000);
	}

    return 0;
}