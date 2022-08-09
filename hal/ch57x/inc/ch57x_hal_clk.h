/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_CLK_H
#define HAL_CH57X_INC_CH57X_HAL_CLK_H

#include "ch57x_common.h"

/* System clock */
#define FHSE                        (32000000)
#define FPLL                        (FHSE * 15)

#define SYS_CLK_MODE_POS            (6U)
#define SYS_CLK_MODE_MSK            (WCH_MASK(2) << SYS_CLK_MODE_POS)

#define SYS_CLK_DIV_POS             (0U)
#define SYS_CLK_DIV_MSK             (WCH_MASK(5) << SYS_CLK_DIV_POS)

typedef enum {
    CLK_TMR0 = WCH_BIT(0),
    CLK_TMR1 = WCH_BIT(1),
    CLK_TMR2 = WCH_BIT(2),
    CLK_TMR3 = WCH_BIT(3),
    CLK_UART0 = WCH_BIT(4),
    CLK_UART1 = WCH_BIT(5),
    CLK_UART2 = WCH_BIT(6),
    CLK_UART3 = WCH_BIT(7),
    CLK_SPI0 = WCH_BIT(8),
    CLK_PWM = WCH_BIT(10),
    CLK_USB = WCH_BIT(12),
    CLK_BLE = WCH_BIT(15),
} periph_clk_t;

typedef enum {
    CLK_MODE_32M,
    CLK_MODE_PLL = 0x01,
    CLK_MODE_32K = 0x11
} sys_clk_mode_t;

typedef enum {
  CLK_SOURCE_LSI = 0x00,
  CLK_SOURCE_LSE,

  CLK_SOURCE_HSE_16MHz = 0x22,
  CLK_SOURCE_HSE_8MHz = 0x24,
  CLK_SOURCE_HSE_6_4MHz = 0x25,
  CLK_SOURCE_HSE_4MHz = 0x28,
  CLK_SOURCE_HSE_2MHz = (0x20|16),
  CLK_SOURCE_HSE_1MHz = (0x20|0),

  CLK_SOURCE_PLL_60MHz = 0x48,
  CLK_SOURCE_PLL_48MHz = (0x40|10),
  CLK_SOURCE_PLL_40MHz = (0x40|12),
  CLK_SOURCE_PLL_36_9MHz = (0x40|13),
  CLK_SOURCE_PLL_32MHz = (0x40|15),
  CLK_SOURCE_PLL_30MHz = (0x40|16),
  CLK_SOURCE_PLL_24MHz = (0x40|20),
  CLK_SOURCE_PLL_20MHz = (0x40|24),
  CLK_SOURCE_PLL_15MHz = (0x40|0),
} sys_clk_type_t;

typedef enum {
    CLK_SOURCE_32KHZ            = (CLK_MODE_32K << SYS_CLK_MODE_POS),
    CLK_SOURCE_HSE_16MHZ        = (CLK_MODE_32M << SYS_CLK_MODE_POS) | (2U),
    CLK_SOURCE_HSE_8MHZ         = (CLK_MODE_32M << SYS_CLK_MODE_POS) | (4U),
    CLK_SOURCE_HSE_6_4MHZ       = (CLK_MODE_32M << SYS_CLK_MODE_POS) | (5U),
    CLK_SOURCE_HSE_4MHZ         = (CLK_MODE_32M << SYS_CLK_MODE_POS) | (8U),
    CLK_SOURCE_HSE_2MHZ         = (CLK_MODE_32M << SYS_CLK_MODE_POS) | (16U),
    CLK_SOURCE_HSE_1MHZ         = (CLK_MODE_32M << SYS_CLK_MODE_POS) | (0U),

    CLK_SOURCE_PLL_60MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (8U),
    CLK_SOURCE_PLL_48MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (10U),
    CLK_SOURCE_PLL_40MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (12U),
    CLK_SOURCE_PLL_36_9MHZ      = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (13U),
    CLK_SOURCE_PLL_32MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (15U),
    CLK_SOURCE_PLL_30MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (16U),
    CLK_SOURCE_PLL_24MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (20U),
    CLK_SOURCE_PLL_20MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (24U),
    CLK_SOURCE_PLL_15MHZ        = (CLK_MODE_PLL << SYS_CLK_MODE_POS) | (0U),
} sys_clk_source_t;


typedef enum {
    LF_SOURCE_NONE,
    LF_SOURCE_LSI,
	LF_SOURCE_LSE,
} low_freq_source_t;

#define HSE_TUNE_IBIAS_POS              (0U)
#define HSE_TUNE_IBIAS_MSK              (WCH_MASK(2) << HSE_TUNE_IBIAS_POS)

#define HSE_TUNE_CLOAD_POS              (4U)
#define HSE_TUNE_CLOAD_MSK              (WCH_MASK(2) << HSE_TUNE_CLOAD_POS)

typedef enum {
    HSE_CURRENT_75,
	HSE_CURRENT_100,
    HSE_CURRENT_125,
    HSE_CURRENT_150
} hse_current_t;

typedef enum {
    HSE_CAP_10P,
	HSE_CAP_12P,  
    HSE_CAP_14P,  
    HSE_CAP_16P, 
    HSE_CAP_18P,  
    HSE_CAP_20P,  
    HSE_CAP_22P,  
    HSE_CAP_24P
} hse_capacitance_t;


#define LSE_TUNE_IBIAS_POS              (0U)
#define LSE_TUNE_IBIAS_MSK              (WCH_MASK(2) << LSE_TUNE_IBIAS_POS)

#define LSE_TUNE_CLOAD_POS              (4U)
#define LSE_TUNE_CLOAD_MSK              (WCH_MASK(4) << LSE_TUNE_CLOAD_POS)

typedef enum {
	LSE_CURRENT_70,
	LSE_CURRENT_100,
    LSE_CURRENT_140,
    LSE_CURRENT_200
} lse_current_t;

typedef enum {
	LSE_CAP_2P,
	LSE_CAP_13P,
    LSE_CAP_14P,
    LSE_CAP_15P,
    LSE_CAP_16P,
    LSE_CAP_17P,
    LSE_CAP_18P,
    LSE_CAP_19P,
    LSE_CAP_20P,
    LSE_CAP_21P,
    LSE_CAP_22P,
    LSE_CAP_23P,
    LSE_CAP_24P,
    LSE_CAP_25P,
    LSE_CAP_26P,
    LSE_CAP_27P
} lse_capacitance_t;


static inline bool hal_clk_pwr_pll_is_on(void)
{
    return (R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON) ? true : false;
}

static inline bool hal_clk_pwr_xt32m_is_on(void)
{
    return (R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON) ? true : false;
}

extern uint16_t int32k_tune_flash;
extern uint16_t int32k_tune_ram;


/**
 * @brief 
 * 
 * @param per 
 */
void hal_clk_periph_clk_enable(uint16_t per);
void hal_clk_periph_clk_disable(uint16_t per);
void hal_clk_sys_pwr_pll_on(void);
void hal_clk_sys_pwr_pll_off(void);
void hal_clk_sys_pwr_xt32m_on(void);
void hal_clk_sys_pwr_xt32m_off(void);
void hal_clk_sys_setup(sys_clk_source_t clk);
uint32_t hal_clk_sys_get(void);
void hal_clk_lf_source_select(low_freq_source_t src);

void hal_clk_lse_cfg_current(lse_current_t cur);
void hal_clk_lse_cfg_capacitance(lse_capacitance_t cap);
void hal_clk_hse_cfg_current(hse_current_t cur);
void hal_clk_hse_cfg_capacitance(hse_capacitance_t cap);

void hal_clk_lsi_calibrate(void);

#endif /* HAL_CH57X_INC_CH57X_HAL_CLK_H */