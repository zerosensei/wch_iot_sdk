/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <drivers/uart.h>
#include <drivers/gpio.h>
#include <drivers/pinctrl.h>

struct uart_wch_config {
    WCH_UART_Type *uart;
    struct uart_config uart_cfg;
    const struct pinctrl_dev_config *pin_cfg;
    uart_irq_config_func_t irq_config_func;
};

struct uart_wch_data {
#ifdef CONFIG_UART_SUPPORT_INTERRUPT
    uart_irq_callback_user_data_t callback;
	void *cb_data;
#endif
};

__HIGHCODE void uart_isr_handler(const struct device *dev)
{
#ifdef CONFIG_UART_SUPPORT_INTERRUPT
    const struct uart_wch_data *data = dev->data;

    if (data->callback) {
        data->callback(dev, data->cb_data);
    }
#endif
}

#ifdef CONFIG_UART_SUPPORT_INTERRUPT
#ifdef CONFIG_WCH_UART_0
__WCH_INT_FAST __HIGHCODE void UART0_IRQHandler(void)
{
    uart_isr_handler(DEVICE_GET(uart0));
}
#endif

#ifdef CONFIG_WCH_UART_1
__WCH_INT_FAST __HIGHCODE void UART1_IRQHandler(void)
{
    uart_isr_handler(DEVICE_GET(uart1));
}
#endif

#ifdef CONFIG_WCH_UART_2
__WCH_INT_FAST __HIGHCODE void UART2_IRQHandler(void)
{
    uart_isr_handler(DEVICE_GET(uart2));
}
#endif

#ifdef CONFIG_WCH_UART_3
__WCH_INT_FAST __HIGHCODE void UART3_IRQHandler(void)
{
    uart_isr_handler(DEVICE_GET(uart3));
}
#endif
#endif /* CONFIG_UART_SUPPORT_INTERRUPT */

static inline int uart_wch_set_pairty(WCH_UART_Type *uart, uint8_t pairty)
{
    switch (pairty) {
    case UART_CFG_PARITY_NONE:
        hal_uart_pairty_disable(uart);
        break;
    case UART_CFG_PARITY_ODD:
        hal_uart_pairty_set(uart, UART_PARITY_ODD);
        break;
    case UART_CFG_PARITY_EVEN:
        hal_uart_pairty_set(uart, UART_PARITY_EVEN);
        break;
    case UART_CFG_PARITY_MARK:
        hal_uart_pairty_set(uart, UART_PARITY_MARK);
        break;
    case UART_CFG_PARITY_SPACE:
        hal_uart_pairty_set(uart, UART_PARITY_SPACE);
        break;
    default:
        return -EINVAL;
        break;
    }

    return 0;
}

static inline int uart_wch_get_pairty(WCH_UART_Type *uart, uint8_t *pairty)
{
    uint8_t hal_pairty;

    if (!hal_uart_pairty_is_enabled(uart)) {
        *pairty = UART_CFG_PARITY_NONE;

        return 0;
    }

    hal_pairty = hal_uart_pairty_get(uart);

    switch (hal_pairty) {
    case UART_PARITY_ODD:
        *pairty = UART_CFG_PARITY_ODD;
        break;
    case UART_PARITY_EVEN:
        *pairty = UART_CFG_PARITY_EVEN;
        break;
    case UART_PARITY_MARK:
        *pairty = UART_CFG_PARITY_MARK;
        break;
    case UART_PARITY_SPACE:
        *pairty = UART_CFG_PARITY_SPACE;
        break;
    default:
        return -EINVAL;
        break;
    }

    return 0;
}

static inline int uart_wch_set_stop_bits(WCH_UART_Type *uart, uint8_t stop_bits)
{
    switch (stop_bits) {
    case UART_CFG_STOP_BITS_1:
        hal_uart_stop_bit_set(uart, UART_STOP_BITS_1);
        break;
    case UART_CFG_STOP_BITS_2:
        hal_uart_stop_bit_set(uart, UART_STOP_BITS_2);
        break;
    case UART_CFG_STOP_BITS_0_5:
    case UART_CFG_STOP_BITS_1_5:
        return -ENOTSUP;
    default:
        return -EINVAL;
        break;
    }

    return 0;
}

static inline int uart_wch_get_stop_bits(WCH_UART_Type *uart, uint8_t *stop_bits)
{
    uint8_t hal_stop_bits = hal_uart_stop_bit_get(uart);

    switch (hal_stop_bits) {
    case UART_STOP_BITS_1:
        *stop_bits = UART_CFG_STOP_BITS_1;
        break;
    case UART_STOP_BITS_2:
        *stop_bits = UART_CFG_STOP_BITS_2;
        break;
    default:
        return -EINVAL;
        break;
    }

    return 0;
}

static inline int uart_wch_set_data_bits(WCH_UART_Type *uart, uint8_t data_bits)
{
    switch (data_bits) {
    case UART_CFG_DATA_BITS_5:
        hal_uart_word_size_set(uart, UART_DATA_5_BITS);
        break;
    case UART_CFG_DATA_BITS_6:
        hal_uart_word_size_set(uart, UART_DATA_6_BITS);
        break;
    case UART_CFG_DATA_BITS_7:
        hal_uart_word_size_set(uart, UART_DATA_7_BITS);
        break;
    case UART_CFG_DATA_BITS_8:
        hal_uart_word_size_set(uart, UART_DATA_8_BITS);
        break;
    case UART_CFG_DATA_BITS_9:
        return -ENOSYS;
        break;
    default:
        return -EINVAL;
        break;
    }

    return 0;
}

static inline int uart_wch_get_data_bits(WCH_UART_Type *uart, uint8_t *data_bits)
{
    uint8_t hal_data_bits = hal_uart_word_size_get(uart);

    switch (hal_data_bits) {
        case UART_DATA_5_BITS:
            *data_bits = UART_CFG_DATA_BITS_5;
            break;
        case UART_DATA_6_BITS:
            *data_bits = UART_CFG_DATA_BITS_6;
            break;
        case UART_DATA_7_BITS:
            *data_bits = UART_CFG_DATA_BITS_6;
            break;
        case UART_DATA_8_BITS:
            *data_bits = UART_CFG_DATA_BITS_6;
            break;
        default:
            return -EINVAL;
            break;
    }

    return 0;
}

static inline int uart_wch_set_flow_ctrl(WCH_UART_Type *uart, uint8_t flow_ctrl)
{
#ifdef CONFIG_WCH_UART_0
    if (uart != UART0) {
        return -ENOTSUP;
    }

    if (flow_ctrl) {
        hal_uart0_modem_autoflow_enable();
        hal_uart0_modem_rts_enable();
        hal_uart0_modem_rst_set_low();
    } else {
        hal_uart0_modem_autoflow_disable();
        hal_uart0_modem_rst_set_high();
        hal_uart0_modem_rts_disable();
    }

    return 0;
#else
    return -ENOTSUP;
#endif
}

static inline int uart_wch_get_flow_ctrl(WCH_UART_Type *uart, uint8_t *flow_ctrl)
{
#ifdef CONFIG_WCH_UART_0
    if (hal_uart0_modem_autoflow_is_enabled()) {
        *flow_ctrl = true;
    } else {
        *flow_ctrl = false;
    }

    return 0;
#else
    return -ENOTSUP;
#endif 
}

static int uart_wch_poll_in(const struct device *dev, unsigned char *c)
{
    const struct uart_wch_config *cfg = dev->cfg;

    if (!(hal_uart_get_line_state(cfg->uart) 
            & UART_LINE_DATA_RDY)) {
        return -1;
    }

    *c = hal_uart_rx(cfg->uart);

    return 0;
}

static void uart_wch_poll_out(const struct device *dev, unsigned char c)
{
    const struct uart_wch_config *cfg = dev->cfg;

    while (!(hal_uart_get_line_state(cfg->uart) 
            & UART_LINE_TX_ALL_EMP)) {
        ;
    }
    hal_uart_tx(cfg->uart, c);
}

static int uart_wch_err_check(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;
    int err = 0;
    uint8_t status = hal_uart_get_line_state(cfg->uart);

    if(status & UART_LINT_FIFO_OVERFLOW) {
        err |= UART_ERROR_OVERRUN;
    }
    
    if (status & UART_LINE_PAR_ERR) {
        err |= UART_ERROR_PARITY;
    }

    if (status & UART_LINE_FRAME_ERR) {
        err |= UART_ERROR_FRAMING;
    }

    if (status & UART_LINE_BREAK_ERR) {
        err |= UART_BREAK;
    }

    return err; 
}

static int uart_wch_configure(const struct device *dev, 
            const struct uart_config *cfg)
{
    const struct uart_wch_config *config = dev->cfg;
    int err = 0;

    if (cfg->baudrate) {
        hal_uart_baudrate_set(config->uart, cfg->baudrate);
    } else {
        return -EINVAL;
    }

    if ((err = uart_wch_set_pairty(config->uart, cfg->parity))) {
        return err;
    }

    if ((err = uart_wch_set_data_bits(config->uart, cfg->data_bits))) {
        return err;
    }

    if ((err = uart_wch_set_stop_bits(config->uart, cfg->stop_bits))) {
        return err;
    }

    if ((err = uart_wch_set_flow_ctrl(config->uart, cfg->data_bits))) {
        return err;
    }

    return 0;
}

static int uart_wch_configure_get(const struct device *dev, 
            struct uart_config *cfg)
{
    const struct uart_wch_config *config = dev->cfg;
    uint8_t data_bits, pairty, stop_bits, flow_ctrl;
    int err = 0;
    
    if ((err = uart_wch_get_data_bits(config->uart, &data_bits))) {
        return err;
    }
    if ((err = uart_wch_get_pairty(config->uart, &pairty))) {
        return err;
    }

    if ((err = uart_wch_get_stop_bits(config->uart, &stop_bits))) {
        return err;
    }

    if ((err = uart_wch_get_flow_ctrl(config->uart, &flow_ctrl))) {
        return err;
    }

    cfg->baudrate = hal_uart_baudrate_get(config->uart);
    cfg->data_bits = data_bits;
    cfg->parity = pairty;
    cfg->stop_bits = stop_bits;
    cfg->flow_ctrl = flow_ctrl;

    return 0;
}


#ifdef CONFIG_UART_SUPPORT_INTERRUPT
static int uart_wch_fifo_fill(const struct device *dev,
            const uint8_t *tx_data, int len)
{
    const struct uart_wch_config *cfg = dev->cfg;

    return hal_uart_fifo_tx(cfg->uart, tx_data, len);
}

static int uart_wch_fifo_read(const struct device *dev,
            uint8_t *rx_data, const int size)
{
    const struct uart_wch_config *cfg = dev->cfg;
    uint8_t num_rx = 0;

    while (size - num_rx > 0) {
        num_rx += hal_uart_fifo_rx(cfg->uart, &rx_data[num_rx]);
    }

    return num_rx;
}

static void uart_wch_irq_tx_enable(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;
    hal_uart_interrupt_enable(cfg->uart, UART_INT_THR_ENMPTY);
}

static void uart_wch_irq_tx_disable(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;

    hal_uart_interrupt_disable(cfg->uart, UART_INT_THR_ENMPTY);
}

static void uart_wch_irq_rx_enable(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;

    hal_uart_interrupt_enable(cfg->uart, UART_INT_RCV_RDY);
}

static void uart_wch_irq_rx_disable(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;

    hal_uart_interrupt_disable(cfg->uart, UART_INT_RCV_RDY);
}

static int uart_wch_irq_tx_complete(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;

    return hal_uart_get_int_flag(cfg->uart) & UART_INT_FLAG_THR_EMPTY;
}

static int uart_wch_irq_tx_ready(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;

    return (hal_uart_get_int_flag(cfg->uart) & UART_INT_FLAG_THR_EMPTY) && 
            hal_uart_interrupt_is_enabled(cfg->uart, UART_INT_THR_ENMPTY);
}

static int uart_wch_irq_rx_ready(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;
    
    return hal_uart_interrupt_is_enabled(cfg->uart, UART_INT_RCV_RDY);
}

static void uart_wch_irq_err_enable(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;

    hal_uart_interrupt_enable(cfg->uart, UART_INT_LINE_STAT);
}

static void uart_wch_irq_err_disable(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;

    hal_uart_interrupt_disable(cfg->uart, UART_INT_LINE_STAT);
}

static int uart_wch_irq_is_pending(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;
    uint8_t flag = hal_uart_get_int_flag(cfg->uart);

    if (hal_uart_interrupt_is_enabled(cfg->uart, UART_INT_RCV_RDY)) {
        if (flag == UART_INT_FALG_RCV_RDY) {
            return true;
        }

        if (flag == UART_INT_FLAG_RCV_TIMEOUT) {
            return true;
        }
    }

    if (hal_uart_interrupt_is_enabled(cfg->uart, UART_INT_THR_ENMPTY)) {
        if (flag == UART_INT_FLAG_LINE_STAT) {
            return true;
        }
    }


#ifdef CONFIG_WCH_UART_0
    if (hal_uart_interrupt_is_enabled(cfg->uart, UART_INT_MODEM_CHG)) {
        if (flag == UART_INT_FLAG_LINE_STAT) {
            return true;
        }
    }
#endif

    return false;

}

static int uart_wch_irq_update(const struct device *dev)
{
    return 1;
}

static void uart_wch_irq_callback_set(const struct device *dev,
            uart_irq_callback_user_data_t cb,
            void *cb_data)
{
    struct uart_wch_data *data = dev->data;

    data->callback = cb;
    data->cb_data = cb_data;
}
#endif /* CONFIG_UART_SUPPORT_INTERRUPT */

static int uart_wch_init(const struct device *dev)
{
    const struct uart_wch_config *cfg = dev->cfg;
    int err = 0;

    pinctrl_configure_pins(cfg->pin_cfg->cfg, cfg->pin_cfg->cnt);

    hal_uart_definit(cfg->uart);

    if (cfg->uart_cfg.baudrate) {
        hal_uart_baudrate_set(cfg->uart, cfg->uart_cfg.baudrate);
    } else {
        return -EINVAL;
    }

    if ((err = uart_wch_set_pairty(cfg->uart, cfg->uart_cfg.parity))) {
        return err;
    }

    if ((err = uart_wch_set_data_bits(cfg->uart, cfg->uart_cfg.data_bits))) {
        return err;
    }

    if ((err = uart_wch_set_stop_bits(cfg->uart, cfg->uart_cfg.stop_bits))) {
        return err;
    }

    if (cfg->uart_cfg.flow_ctrl && 
            (err = uart_wch_set_flow_ctrl(cfg->uart, cfg->uart_cfg.flow_ctrl))) {
        return err;
    }

#ifdef CONFIG_UART_SUPPORT_INTERRUPT
    cfg->irq_config_func(dev);
#endif

    return 0;
}

static const struct uart_driver_api uart_wch_driver_api = {
	.poll_in		    = uart_wch_poll_in,
	.poll_out		    = uart_wch_poll_out,
	.err_check		    = uart_wch_err_check,
    .configure          = uart_wch_configure,
    .config_get         = uart_wch_configure_get,
#ifdef CONFIG_UART_SUPPORT_INTERRUPT
	.fifo_fill          = uart_wch_fifo_fill,
	.fifo_read		    = uart_wch_fifo_read,
	.irq_tx_enable	    = uart_wch_irq_tx_enable,
	.irq_tx_disable	    = uart_wch_irq_tx_disable,
	.irq_tx_ready	    = uart_wch_irq_tx_ready,
	.irq_rx_enable	    = uart_wch_irq_rx_enable,
	.irq_rx_disable	    = uart_wch_irq_rx_disable,
	.irq_tx_complete    = uart_wch_irq_tx_complete,
	.irq_rx_ready	    = uart_wch_irq_rx_ready,
	.irq_err_enable	    = uart_wch_irq_err_enable,
	.irq_err_disable    = uart_wch_irq_err_disable,
	.irq_is_pending	    = uart_wch_irq_is_pending,
    .irq_update         = uart_wch_irq_update,
	.irq_callback_set   = uart_wch_irq_callback_set,
#endif /* CONFIG_UART_SUPPORT_INTERRUPT */
};  

#define UART_INTI(index)   \
    static void uart_wch_irq_cfg_func_##index(const struct device *dev)     \
    {       \
        irq_enable(DEV_CFG_GET_IRQ(uart##index));       \
    }       \
    static const struct uart_wch_config uart_cfg_##index = {    \
        .uart = (WCH_UART_Type *)DEV_CFG_GET(uart##index, reg),   \
        .uart_cfg = {           \
            .baudrate = DEV_CFG_GET(uart##index,  baudrate),      \
            .data_bits = DEV_CFG_GET(uart##index, data_bits),     \
            .stop_bits = DEV_CFG_GET(uart##index, stop_bits),     \
        },      \
        .pin_cfg = &__pinctrl_uart##index,        \
        .irq_config_func = uart_wch_irq_cfg_func_##index,      \
    };     \
    static struct uart_wch_data uart_data_##index;        \
    static struct device_state uart_sta_##index = {0};        \
    DEVICE_DEFINE(uart##index, uart_wch_init, NULL,        \
            &uart_data_##index,     \
            &uart_cfg_##index,      \
            &uart_wch_driver_api,       \
            &uart_sta_##index,          \
            DRIVER,         \
            1);     

#ifdef CONFIG_UART_0
UART_INTI(0);
#endif

#ifdef CONFIG_UART_1
UART_INTI(1);
#endif

#ifdef CONFIG_UART_2
UART_INTI(2);
#endif

#ifdef CONFIG_UART_3
UART_INTI(3);
#endif