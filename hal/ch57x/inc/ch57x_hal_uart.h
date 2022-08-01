/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_UART_H
#define HAL_CH57X_INC_CH57X_HAL_UART_H

#include "ch57x_common.h"

typedef struct __attribute__((__packed__))
{
    struct {
        volatile uint8_t MCR;
        volatile uint8_t IER;
        volatile uint8_t FCR;
        volatile uint8_t LCR;
    } CTRL;

    struct {
        volatile uint8_t IIR;
        volatile uint8_t LSR;
        volatile uint8_t MSR;
        uint8_t RESERVED;
    } STAT;

    struct {
        union 
        {
            volatile uint8_t RBR;
            volatile uint8_t THR;
        };
        uint8_t RESERVED;
        volatile uint8_t RFC;
        volatile uint8_t TFC;
    } FIFO;

    struct {
        volatile uint16_t DL;              
        volatile uint8_t DIV;        
        volatile uint8_t ADR;
        uint8_t RESERVED;
    } SETUP;
} WCH_UART_Type;

#define UART0           ((WCH_UART_Type *)BA_UART0)
#define UART1           ((WCH_UART_Type *)BA_UART1)
#define UART2           ((WCH_UART_Type *)BA_UART2)
#define UART3           ((WCH_UART_Type *)BA_UART3)

#define UART_LCR_PAR_MOD_POS        (4U)
#define UART_LCR_PAR_MOD_MSK        (3U << UART_LCR_PAR_MOD_POS)
typedef enum {
    UART_PARITY_ODD,
    UART_PARITY_EVEN,
    UART_PARITY_MARK,
    UART_PARITY_SPACE
} uart_parity_mode_t;

#define UART_LCR_STOP_BIT_POS      (2U)
#define UART_LCR_STOP_BIT_MSK       (1U << UART_LCR_STOP_BIT_POS)
typedef enum {
    UART_STOP_BITS_1,
    UART_STOP_BITS_2
} uart_stop_bits_t;

#define UART_LCR_WORD_SZ_POS        (0U)
#define UART_LCR_WORD_SZ_MSK        (3U << UART_LCR_WORD_SZ_POS)
typedef enum {
    UART_DATA_5_BITS,
    UART_DATA_6_BITS,
    UART_DATA_7_BITS,
    UART_DATA_8_BITS
} uart_word_size_t;

#define UART_FCR_FIFO_TRIG_POS              (6U)
#define UART_FCR_FIFO_TRIG_MSK              (3U << UART_FCR_FIFO_TRIG_POS)
typedef enum {
    UART_FIFO_TRIG_BITS_1,
    UART_FIFO_TRIG_BITS_2,
    UART_FIFO_TRIG_BITS_4,
    UART_FIFO_TRIG_BITS_7
} uart_fifo_trig_t;

typedef enum {
    UART_INT_RCV_RDY = WCH_BIT(0),
    UART_INT_THR_ENMPTY = WCH_BIT(1),
    UART_INT_LINE_STAT = WCH_BIT(2),
    UART_INT_MODEM_CHG = WCH_BIT(3),
} uart_interrupt_t;

typedef enum {
    UART_INT_FLAG_MODEM_CHG = 0,
    UART_INT_FLAG_NO_INTER,
    UART_INT_FLAG_THR_EMPTY,
    UART_INT_FALG_RCV_RDY = 0x04,
    UART_INT_FLAG_LINE_STAT = 0x06,
    UART_INT_FLAG_RCV_TIMEOUT = 0x0c,
    UART_INT_FALG_SLV_ADDR = 0x0e,
} uart_int_flag_t;

typedef enum {
    UART_LINE_DATA_RDY = WCH_BIT(0),
    UART_LINT_FIFO_OVERFLOW = WCH_BIT(1),
    UART_LINE_PAR_ERR = WCH_BIT(2),
    UART_LINE_FRAME_ERR = WCH_BIT(3),
    UART_LINE_BREAK_ERR = WCH_BIT(4),
    UART_LINE_TX_FIFO_EMP = WCH_BIT(5),
    UART_LINE_TX_ALL_EMP = WCH_BIT(6),
    UART_LINE_RX_FIFO_ERR = WCH_BIT(7)
} uart_line_state_t;


static inline void hal_uart_frequency_div(WCH_UART_Type *uart, uint8_t div)
{
    uart->SETUP.DIV = div & WCH_MASK(7);
}

static inline void hal_uart_break_config(WCH_UART_Type *uart, bool cfg)
{
    if(cfg) {
        uart->CTRL.LCR |= RB_LCR_BREAK_EN;
    } else {
        uart->CTRL.LCR &= ~RB_LCR_BREAK_EN;
    }
}

static inline void hal_uart_pairty_set(WCH_UART_Type *uart, uart_parity_mode_t mode)
{
    uart->CTRL.LCR |= RB_LCR_PAR_EN;
    uart->CTRL.LCR &= ~UART_LCR_PAR_MOD_MSK; 
    uart->CTRL.LCR |= ((mode << UART_LCR_PAR_MOD_POS) & UART_LCR_PAR_MOD_MSK);
}

static inline uint8_t hal_uart_pairty_get(WCH_UART_Type *uart)
{
    return (uart->CTRL.LCR & UART_LCR_PAR_MOD_MSK) >> UART_LCR_PAR_MOD_POS;
}

static inline void hal_uart_pairty_disable(WCH_UART_Type *uart)
{
    uart->CTRL.LCR &= ~RB_LCR_PAR_EN;
}

static inline uint8_t hal_uart_pairty_is_enabled(WCH_UART_Type *uart)
{
    return uart->CTRL.LCR & RB_LCR_PAR_EN;
}

static inline void hal_uart_stop_bit_set(WCH_UART_Type *uart, uart_stop_bits_t stop_bits)
{
    uart->CTRL.LCR &= ~UART_LCR_STOP_BIT_MSK;
    uart->CTRL.LCR |= ((stop_bits << UART_LCR_STOP_BIT_POS) & UART_LCR_STOP_BIT_MSK);
}

static inline uint8_t hal_uart_stop_bit_get(WCH_UART_Type *uart)
{
    return (uart->CTRL.LCR & UART_LCR_STOP_BIT_MSK) >> UART_LCR_STOP_BIT_POS;
}

static inline void hal_uart_word_size_set(WCH_UART_Type *uart, uart_word_size_t word_sz)
{
    uart->CTRL.LCR &= ~UART_LCR_WORD_SZ_MSK;
    uart->CTRL.LCR |= ((word_sz << UART_LCR_WORD_SZ_POS) & UART_LCR_WORD_SZ_MSK);
}

static inline uint8_t hal_uart_word_size_get(WCH_UART_Type *uart)
{
    return (uart->CTRL.LCR & UART_LCR_WORD_SZ_MSK) >> UART_LCR_WORD_SZ_POS;
}

static inline void hal_uart_tx_enable(WCH_UART_Type *uart)
{
    uart->CTRL.IER |= RB_IER_TXD_EN;
}

static inline void hal_uart_tx_disable(WCH_UART_Type *uart)
{
    uart->CTRL.IER &= ~RB_IER_TXD_EN;
}

static inline uint8_t hal_uart_is_tx_enabled(WCH_UART_Type *uart)
{
    return uart->CTRL.IER & RB_IER_TXD_EN;
}

static inline void hal_uart_rx_fifo_trig(WCH_UART_Type *uart, uart_fifo_trig_t trig)
{
    uart->CTRL.FCR &= ~UART_FCR_FIFO_TRIG_MSK;
    uart->CTRL.FCR |= ((trig << UART_FCR_FIFO_TRIG_POS) & UART_FCR_FIFO_TRIG_MSK);
}

static inline void hal_uart_fifo_cfg(WCH_UART_Type *uart, bool fifo_en)
{
    if(fifo_en) {
        uart->CTRL.FCR |= RB_FCR_FIFO_EN;
    } else {
        uart->CTRL.FCR &= ~RB_FCR_FIFO_EN;
    }
}

static inline void hal_uart_fifo_tx_clear(WCH_UART_Type *uart)
{
    uart->CTRL.FCR |= RB_FCR_TX_FIFO_CLR;
}

static inline void hal_uart_fifo_rx_clear(WCH_UART_Type *uart)
{
    uart->CTRL.FCR |= RB_FCR_RX_FIFO_CLR;
}

static inline void hal_uart_interrupt_enable(WCH_UART_Type *uart, uint8_t interrupt)
{
    uart->CTRL.IER |= (interrupt) & WCH_MASK(4);
    uart->CTRL.MCR |= RB_MCR_INT_OE;
}

static inline void hal_uart_interrupt_disable(WCH_UART_Type *uart, uint8_t interrupt)
{
    uart->CTRL.IER &= ~((interrupt) & WCH_MASK(4));
}

static inline bool hal_uart_interrupt_is_enabled(WCH_UART_Type *uart, uart_interrupt_t interrupt)
{
    return (uart->CTRL.IER & (interrupt & WCH_MASK(4)) ? true : false);
}

static inline uint8_t hal_uart_get_int_flag(WCH_UART_Type *uart)
{
    return uart->STAT.IIR & RB_IIR_INT_MASK;
}

static inline uint8_t hal_uart_get_line_state(WCH_UART_Type *uart)
{
    return uart->STAT.LSR;
}

static inline void hal_uart_reset(WCH_UART_Type *uart)
{
    uart->CTRL.IER |= RB_IER_RESET;
}

static inline uint8_t hal_uart0_get_modem(void)
{
    return R8_UART0_MSR;
}

static inline void hal_uart0_modem_rts_enable(void)
{
    R8_UART0_IER |= RB_IER_RTS_EN;
}

static inline void hal_uart0_modem_rts_disable(void)
{
    R8_UART0_IER &= ~RB_IER_RTS_EN;
}

static inline void hal_uart0_modem_dtr_enable(void)
{
    R8_UART0_IER |= RB_IER_DTR_EN;
}

static inline void hal_uart0_modem_dtr_disable(void)
{
    R8_UART0_IER &= ~RB_IER_DTR_EN;
}

static inline void hal_uart0_modem_tnow_enable(void)
{
    R8_UART0_IER |= RB_MCR_TNOW;
}

static inline void hal_uart0_modem_tnow_disable(void)
{
    R8_UART0_IER &= ~RB_MCR_TNOW;
}

static inline void hal_uart0_modem_autoflow_enable(void)
{
    R8_UART0_IER |= RB_MCR_AU_FLOW_EN;
}

static inline void hal_uart0_modem_autoflow_disable(void)
{
    R8_UART0_IER &= ~RB_MCR_AU_FLOW_EN;
}

static inline uint8_t hal_uart0_modem_autoflow_is_enabled(void)
{
    return R8_UART0_IER & RB_MCR_AU_FLOW_EN;
}

static inline void hal_uart0_modem_loop_test_enable(void)
{
    R8_UART0_IER |= RB_MCR_LOOP;
}

static inline void hal_uart0_modem_loop_test_disable(void)
{
    R8_UART0_IER &= ~RB_MCR_LOOP;
}

static inline void hal_uart0_modem_out_set(void)
{
    R8_UART0_IER |= RB_MCR_OUT1;
}

static inline void hal_uart0_modem_out_reset(void)
{
    R8_UART0_IER &= ~RB_MCR_OUT1;
}

static inline void hal_uart0_modem_rst_set_low(void)
{
    R8_UART0_IER |= RB_MCR_RTS;
}

static inline void hal_uart0_modem_rst_set_high(void)
{
    R8_UART0_IER &= ~RB_MCR_RTS;
}

static inline void hal_uart0_modem_dtr_set_low(void)
{
    R8_UART0_IER |= RB_MCR_DTR;
}

static inline void hal_uart0_modem_dtr_set_high(void)
{
    R8_UART0_IER &= ~RB_MCR_DTR;
}

static inline void hal_uart0_halfduplex_enable(void)
{
    R8_UART0_MCR |= RB_MCR_HALF;
}

static inline void hal_uart0_halfduplex_disable(void)
{
    R8_UART0_MCR &= ~RB_MCR_HALF;
}

static inline void hal_uart0_set_slv_addr(uint8_t addr)
{
    R8_UART0_ADR = addr;
}

static inline uint8_t hal_uart0_get_slv_addr(void)
{
    return R8_UART0_ADR;
}

static inline void hal_uart_tx(WCH_UART_Type *uart, uint8_t data)
{
    uart->FIFO.THR = data;
}

static inline uint8_t hal_uart_rx(WCH_UART_Type *uart)
{
    return uart->FIFO.RBR;
}

static inline uint8_t hal_uart_get_rx_fifo_len(WCH_UART_Type *uart)
{
    return uart->FIFO.RFC;
}

static inline uint8_t hal_uart_get_tx_fifo_len(WCH_UART_Type *uart)
{
    return uart->FIFO.TFC;
}


void hal_uart_baudrate_set(WCH_UART_Type *uart, uint32_t baudrate);
uint32_t hal_uart_baudrate_get(WCH_UART_Type *uart);
size_t hal_uart_fifo_tx(WCH_UART_Type *uart, const uint8_t *data, size_t len);
uint8_t hal_uart_fifo_rx(WCH_UART_Type *uart, uint8_t *data);
void hal_uart_definit(WCH_UART_Type *uart);


#endif /* HAL_CH57X_INC_CH57X_HAL_UART_H */