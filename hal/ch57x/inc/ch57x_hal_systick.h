/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_SYSTICK_H
#define HAL_CH57X_INC_CH57X_HAL_SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch57x_common.h"

/* memory mapped structure for SysTick */
typedef struct __attribute__((packed))
{
    volatile uint32_t CTLR;
    volatile uint64_t CNT;
    volatile uint64_t CMP;
    volatile uint32_t CNTFG;
} SysTickType;

#define SYSTICK      ((SysTickType *)0xE000F000)

#define SYSTICK_CTRL_STE_POS                (0U)
#define SYSTICK_CTRL_STE_MSK                (WCH_MASK(1) << SYSTICK_CTRL_STE_POS)

#define SYSTICK_CTRL_STIE_POS               (1U)
#define SYSTICK_CTRL_STIE_MSK               (WCH_MASK(1) << SYSTICK_CTRL_STIE_POS)

#define SYSTICK_CTRL_STCLK_POS              (2U)
#define SYSTICK_CTRL_STCLK_MSK              (WCH_MASK(1) << SYSTICK_CTRL_STCLK_POS)

#define SYSTICK_CTRL_STRELOAD_POS           (8U)
#define SYSTICK_CTRL_STRELOAD_MSK           (WCH_MASK(1) << SYSTICK_CTRL_STRELOAD_POS)

typedef enum {
    SYSTICK_CLK_DIV_8,
    SYSTICK_CLK_DIV_1,
} systick_clk_div_t;

static inline void hal_systick_set_cmp(uint64_t cmp)
{
    SYSTICK->CMP = cmp - 1;
}

static inline uint64_t hal_systick_get_cmp(void)
{
    return SYSTICK->CMP;
}

static inline uint64_t hal_systick_get_cnt(void)
{
    return SYSTICK->CNT;
}

static inline void hal_systick_enable(void)
{
    SYSTICK->CTLR |= SYSTICK_CTRL_STE_MSK;
}

static inline void hal_systick_disable(void)
{
    SYSTICK->CTLR &= ~SYSTICK_CTRL_STE_MSK;
}

static inline void hal_systick_int_enable(void)
{
    SYSTICK->CTLR |= SYSTICK_CTRL_STIE_MSK;
}

static inline void hal_systick_int_disable(void)
{
    SYSTICK->CTLR &= ~SYSTICK_CTRL_STIE_MSK;
}

static inline void hal_systick_clk(systick_clk_div_t clk)
{
    SYSTICK->CTLR |= (clk << SYSTICK_CTRL_STCLK_POS) & SYSTICK_CTRL_STCLK_MSK;
} 

static inline void hal_systick_reload_enable(void)
{
    SYSTICK->CTLR |= SYSTICK_CTRL_STRELOAD_MSK;
}


typedef enum {
    SYSTICK_FLAG_CNT = WCH_BIT(1),
} systick_flag_t;

static inline bool hal_systick_get_flag(systick_flag_t flag)
{
    return (SYSTICK->CNTFG & SYSTICK_FLAG_CNT) ? true : false;
}

static inline void hal_systick_clear_flag(systick_flag_t flag)
{
    SYSTICK->CNTFG &= ~(flag);
}

static inline void hal_systick_sw_enable(void)
{
    SYSTICK->CNTFG |= WCH_BIT(0);
}

static inline void hal_systick_sw_disable(void)
{
    SYSTICK->CNTFG &= ~WCH_BIT(0);
}


void hal_systick_definit(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_CH57X_INC_CH57X_HAL_SYSTICK_H */