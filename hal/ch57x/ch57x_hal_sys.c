/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ch57x_hal_sys.h"

void hal_sys_clock_setup(sys_clk_source_t clk)
{
    SYS_SAFE_ACCESS();
    R8_PLL_CONFIG &= ~(1 << 5); 
    SYS_SAFE_DISABLE();

    switch (clk & SYS_CLK_MODE_MSK) {
    case CLK_MODE_32K:
        /**
         * The clock source selection 32K frequency 
         * division factor is invalid. 
         */
        hal_sys_clk_config(CLK_MODE_32K, 5U);
        break;
    case CLK_MODE_PLL:
        hal_sys_clk_config(CLK_MODE_PLL, clk & SYS_CLK_DIV_MSK);
        //TODO: delay  & flash cfg

        break;
    case CLK_MODE_32M:
    default:
        hal_sys_clk_config(CLK_MODE_32M, clk & SYS_CLK_DIV_MSK);

        for(int i = 0; i < 1200; i++)
        {
            __nop();
            __nop();
        }
        break;
    }




    if (clk & 0x20) {
        // HSE div
        if (!(R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON)) {
            SYS_SAFE_ACCESS();
            R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_PON; // HSE power on
            for (int i = 0; i < 1200; i++) {
                __nop();
                __nop();
            }
        }

        SYS_SAFE_ACCESS();
        R16_CLK_SYS_CFG = (0 << 6) | (clk & 0x1f);
        __nop();
        __nop();
        __nop();
        __nop();

        SYS_SAFE_ACCESS();
        R8_FLASH_CFG = 0x51;
        SYS_SAFE_DISABLE();
    } else if (clk & 0x40) { 
        // PLL div
        if (!(R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON)) {
            SYS_SAFE_ACCESS();
            R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON; // PLL power on
            for (int i = 0; i < 2000; i++) {
                __nop();
                __nop();
            }
        }

        SYS_SAFE_ACCESS();
        R16_CLK_SYS_CFG = (1 << 6) | (clk & 0x1f);
        __nop();
        __nop();
        __nop();
        __nop();

        SYS_SAFE_ACCESS();
        R8_FLASH_CFG = 0x53;
        SYS_SAFE_DISABLE();
    }
}
