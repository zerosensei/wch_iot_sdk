/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/printk.h>
#include <soc.h>
#include <drivers/uart.h>

#ifdef CONFIG_PRINTK
void vprintk(const char *fmt, va_list ap)
{
    (void)vprintf(fmt, ap);
}

void printk(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt); 
    (void)vprintk(fmt, ap);
    va_end(ap);
}
#endif /* CONFIG_PRINTK */

int snprintk(char *str, size_t size, const char *fmt, ...)
{
    va_list ap;
    int ret;    
    va_start(ap, fmt);
    ret = vsnprintf(str, size, fmt, ap);
    va_end(ap);    
    return ret;
}

int vsnprintk(char *str, size_t size, const char *fmt, va_list ap)
{
    return vsnprintk(str, size, fmt, ap);
}

int _write(int fd, char *buf, int size)
{
    for(int i = 0; i < size; i++) {
#if (defined CONFIG_WCH_UART_0)         \
        && (defined CONFIG_WCH_LOG_UART) && (CONFIG_WCH_LOG_UART == 0)
        const struct device *uart0 = DEVICE_GET(uart0);
        uart_poll_out(uart0, *buf++);
#endif

#if (defined CONFIG_WCH_UART_1)         \
        && (defined CONFIG_WCH_LOG_UART) && (CONFIG_WCH_LOG_UART == 1)
        const struct device *uart1 = DEVICE_GET(uart1);
        uart_poll_out(uart1, *buf++);
#endif

#if (defined CONFIG_WCH_UART_2)         \
        && (defined CONFIG_WCH_LOG_UART) && (CONFIG_WCH_LOG_UART == 2)
        const struct device *uart2 = DEVICE_GET(uart2);
        uart_poll_out(uart2, *buf++);
#endif

#if (defined CONFIG_WCH_UART_3)         \
        && (defined CONFIG_WCH_LOG_UART) && (CONFIG_WCH_LOG_UART == 3)
        const struct device *uart3 = DEVICE_GET(uart3);
        uart_poll_out(uart3, *buf++);
#endif    
    }
    
    return size;
}
