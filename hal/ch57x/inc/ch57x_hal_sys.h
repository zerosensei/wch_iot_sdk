/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_SYS_H
#define HAL_CH57X_INC_CH57X_HAL_SYS_H

#include "ch57x_common.h"

#define SYS_SAFE_ACCESS()       \
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;  \
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2

#define SYS_SAFE_DISABLE()      R8_SAFE_ACCESS_SIG = 0      



#define SYS_CLK_MODE_POS            (6U)
#define SYS_CLK_MODE_MSK            (WCH_MASK(2) << SYS_CLK_MODE_POS)
typedef enum {
    CLK_MODE_32M,
    CLK_MODE_PLL = 0x01,
    CLK_MODE_32K = 0x11
} sys_clk_mode_t;

#define SYS_CLK_DIV_POS             (0U)
#define SYS_CLK_DIV_MSK             (WCH_MASK(5) << SYS_CLK_DIV_POS)

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


static inline void hal_sys_clk_config(sys_clk_mode_t clk_mode, uint8_t div)
{
    uint8_t cfg = ((clk_mode << SYS_CLK_MODE_POS) & SYS_CLK_MODE_MSK) 
            | ((div << SYS_CLK_DIV_POS) & SYS_CLK_DIV_MSK);

    SYS_SAFE_ACCESS();
    R16_CLK_SYS_CFG = cfg;
    SYS_SAFE_DISABLE();
}

#endif /* HAL_CH57X_INC_CH57X_HAL_SYS_H */