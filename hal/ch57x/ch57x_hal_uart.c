/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch57x_common.h>

/* baudrate = Fsys * 2 / (div * 16 * dl)*/
void hal_uart_baudrate_set(WCH_UART_Type *uart, uint32_t baudrate)
{
    uint32_t dl;

    dl = WCH_ROUNDED_DIV(10 * hal_clk_sys_get() * 2 
            /(16 * baudrate * uart->SETUP.DIV), 10);
    uart->SETUP.DL = (uint16_t)dl;
}

uint32_t hal_uart_baudrate_get(WCH_UART_Type *uart)
{
    return (hal_clk_sys_get() * 2 / (16 * uart->SETUP.DL * uart->SETUP.DIV));
}

size_t hal_uart_fifo_tx(WCH_UART_Type *uart, const uint8_t *data, size_t len)
{
    size_t txlen = 0;

    while (len - txlen > 0) {
        if(hal_uart_get_tx_fifo_len(uart) != UART_FIFO_SIZE) {
            hal_uart_tx(uart, *data++);
            txlen++;
        }
    }

    return txlen;
}

uint8_t hal_uart_fifo_rx(WCH_UART_Type *uart, uint8_t *data)
{
    uint8_t len = 0;

    while (hal_uart_get_rx_fifo_len(uart)) {
        *data++ = hal_uart_rx(uart);
        len++;
    }

    return len;
}

void hal_uart_definit(WCH_UART_Type *uart)
{
    hal_uart_reset(uart);
    hal_uart_baudrate_set(uart, 115200);
    hal_uart_pairty_disable(uart);
    hal_uart_stop_bit_set(uart, UART_STOP_BITS_1);
    hal_uart_word_size_set(uart, UART_DATA_8_BITS);
    hal_uart_fifo_cfg(uart, true);
    hal_uart_rx_fifo_trig(uart, UART_FIFO_TRIG_BITS_4);
    hal_uart_tx_enable(uart);

    hal_uart_frequency_div(uart, 1);
}