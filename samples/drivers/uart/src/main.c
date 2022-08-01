/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <kernel.h>
#include <drivers/uart.h>
#include <logging/log.h>
#include <drivers/pinctrl.h>
#include <drivers/gpio.h>
#include <drivers/timer/system_timer.h>

static const char fifo_data[] = "This is uart0 fifo test.\r\n";
#define DATA_SIZE	(sizeof(fifo_data) - 1)

const struct device *uart0 = DEVICE_GET(uart0);
const struct device *gpioa = DEVICE_GET(gpiob);

extern int sys_clock_driver_init(const struct device *dev);

static void timer_handler1(struct k_timer *timer);
static void timer_handler2(struct k_timer *timer);

K_TIMER_DEFINE(tmr1, timer_handler1, NULL);
K_TIMER_DEFINE(tmr2, timer_handler2, NULL);

static void uart_cb_handler(const struct device *dev, void *data)
{
    static int tx_data_idx = 0;
    uint8_t rcv_data;

    uart_irq_update(dev);
    if (uart_irq_tx_ready(dev) && (tx_data_idx < DATA_SIZE)) {
        if (uart_fifo_fill(dev, 
                (uint8_t *)&fifo_data[tx_data_idx++], 1) > 0) {
            ;
        }

        if (tx_data_idx == DATA_SIZE) {
			uart_irq_tx_disable(dev);
        }
    }

    if (uart_irq_rx_ready(dev)) {
        uart_fifo_read(dev, &rcv_data, 1);
        printk("%c", rcv_data);

        if (rcv_data == '\n') {
			uart_irq_rx_disable(dev);
		}
    }

}

static void timer_handler1(struct k_timer *timer)
{
    printk("tmr(%#lx): %ld ms\n\n", (uint32_t)timer, k_uptime_get_32());
    gpio_port_toggle_bits(gpioa, BIT(15));
}

static void timer_handler2(struct k_timer *timer)
{
    printk("tmr status: %ld\n", k_timer_status_get(&tmr1));
}

int main(void)
{
    wch_sys_init();
    gpio_init();
    uart_init();
    sys_clock_driver_init(NULL);

    printk("wch iot test\n");

    if (!device_is_ready(uart0)) {
        LOG_ERR("uart0 device not ready");
        while(1) {
            ;
        }
    }

    gpio_pin_configure(gpioa, 15, GPIO_OUTPUT_LOW);

    uart_irq_callback_set(uart0, uart_cb_handler);
    uart_irq_rx_enable(uart0);
    uart_irq_tx_enable(uart0);

    while (uart_irq_rx_ready(uart0)) {
        ;
    }

    k_timer_start(&tmr1, K_MSEC(100), K_MSEC(100));
    k_timer_start(&tmr2, K_MSEC(200), K_MSEC(500));

    // __asm__ ("ecall");

    while(1);

    return 0;
}
