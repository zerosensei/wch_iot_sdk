/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CONFIG_DEV_CFG_H
#define CONFIG_DEV_CFG_H
  
#include <sys/util.h>

#define DEV_CFG_GET(dev, prop)            (_dev_cfg_ ## dev ## _ ## prop) 
#define DEV_CFG_GET_IRQ(dev)              DEV_CFG_GET(dev, int_num)

#define _dev_cfg_systick_int_num          SysTick_IRQn
#define _dev_cfg_systick_int_prio         0

#define _dev_cfg_rtc_int_num              RTC_IRQn
#define _dev_cfg_rtc_int_prio             0


#define _dev_cfg_gpioa_reg                GPIO_A
#define _dev_cfg_gpioa_port_pin_mask      (BIT_MASK(16) & (~(BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(6) | BIT(7))))
#define _dev_cfg_gpioa_int_num            GPIO_A_IRQn
#define _dev_cfg_gpioa_int_prio           0

#define _dev_cfg_gpiob_reg                GPIO_B
#define _dev_cfg_gpiob_int_num            GPIO_B_IRQn
#define _dev_cfg_gpiob_int_prio           0
#define _dev_cfg_gpiob_port_pin_mask      BIT_MASK(24) &        \
                                            (~(BIT(1) | BIT(2) | BIT(3) | BIT(8) | BIT(9) | BIT(16)     \
                                            | BIT(17) | BIT(18) | BIT(19) | BIT(20) | BIT(21)))  

#ifdef CONFIG_WCH_UART_0
#define  _dev_cfg_uart0_reg               UART0
#define  _dev_cfg_uart0_baudrate          115200UL
#define  _dev_cfg_uart0_stop_bits         UART_CFG_STOP_BITS_1
#define  _dev_cfg_uart0_data_bits         UART_CFG_DATA_BITS_8
#define  _dev_cfg_uart0_flow_ctrl         UART_CFG_FLOW_CTRL_NONE
#define  _dev_cfg_uart0_parity_ctrl       UART_CFG_PARITY_NONE
#define  _dev_cfg_uart0_int_num           UART0_IRQn
#define  _dev_cfg_uart0_int_prio          0
#endif

#ifdef CONFIG_WCH_UART_1
#define  _dev_cfg_uart1_reg               UART1
#define  _dev_cfg_uart1_baudrate          115200UL
#define  _dev_cfg_uart1_stop_bits         UART_CFG_STOP_BITS_1
#define  _dev_cfg_uart1_data_bits         UART_CFG_DATA_BITS_8
#define  _dev_cfg_uart1_flow_ctrl         UART_CFG_FLOW_CTRL_NONE
#define  _dev_cfg_uart1_parity_ctrl       UART_CFG_PARITY_NONE
#define  _dev_cfg_uart1_int_num           UART1_IRQn
#define  _dev_cfg_uart1_int_prio          0
#endif

#ifdef CONFIG_WCH_UART_2
#define  _dev_cfg_uart2_reg               UART2
#define  _dev_cfg_uart2_baudrate          115200UL
#define  _dev_cfg_uart2_stop_bits         UART_CFG_STOP_BITS_1
#define  _dev_cfg_uart2_data_bits         UART_CFG_DATA_BITS_8
#define  _dev_cfg_uart2_flow_ctrl         UART_CFG_FLOW_CTRL_NONE
#define  _dev_cfg_uart2_parity_ctrl       UART_CFG_PARITY_NONE
#define  _dev_cfg_uart2_int_num           UART2_IRQn
#define  _dev_cfg_uart2_int_prio          0
#endif

#ifdef CONFIG_WCH_UART_3
#define  _dev_cfg_uart3_reg               UART3
#define  _dev_cfg_uart3_baudrate          115200UL
#define  _dev_cfg_uart3_stop_bits         UART_CFG_STOP_BITS_1
#define  _dev_cfg_uart3_data_bits         UART_CFG_DATA_BITS_8
#define  _dev_cfg_uart3_flow_ctrl         UART_CFG_FLOW_CTRL_NONE
#define  _dev_cfg_uart3_parity_ctrl       UART_CFG_PARITY_NONE
#define  _dev_cfg_uart3_int_num           UART3_IRQn
#define  _dev_cfg_uart3_int_prio          0
#endif


#endif /* CONFIG_DEV_CFG_H */
