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

static const char fifo_data[] = "This is uart0 fifo test.\r\n";
#define DATA_SIZE	(sizeof(fifo_data) - 1)

const struct device *uart0 = DEVICE_GET(uart0);
const struct device *gpiob = DEVICE_GET(gpiob);

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

int main(void)
{
    printk("This is uart exapmle\n");

    if (!device_is_ready(uart0)) {
        LOG_ERR("uart0 device not ready");
        while(1) {
            ;
        }
    }

    uart_irq_callback_set(uart0, uart_cb_handler);
    uart_irq_rx_enable(uart0);
    uart_irq_tx_enable(uart0);

    while(1);

    return 0;
}
