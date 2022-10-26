/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch57x_common.h>

__HIGHCODE void hal_sys_reset(void)
{
    //TODO: flash link
    // FLASH_ROM_SW_RESET();
    sys_safe_access_enable();
    R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
    sys_safe_access_disable();
}

__HIGHCODE void hal_sys_init(void)
{
#if (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 60000000)
    hal_clk_sys_setup(CLK_SOURCE_PLL_60MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 48000000)
    hal_clk_sys_setup(CLK_SOURCE_PLL_48MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 40000000)
    hal_clk_sys_setup(CLK_SOURCE_PLL_40MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 32000000)
    hal_clk_sys_setup(CLK_SOURCE_PLL_32MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 24000000)
    hal_clk_sys_setup(CLK_SOURCE_PLL_24MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 16000000)
    hal_clk_sys_setup(CLK_SOURCE_HSE_16MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 8000000)
    hal_clk_sys_setup(CLK_SOURCE_HSE_8MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 4000000)
    hal_clk_sys_setup(CLK_SOURCE_HSE_4MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 2000000)
    hal_clk_sys_setup(CLK_SOURCE_HSE_2MHZ);
#elif (CONFIG_SOC_SYSTEM_OPERATE_FREQ == 1000000)
    hal_clk_sys_setup(CLK_SOURCE_HSE_1MHZ);
#else
#error "CONFIG_SOC_SYSTEM_OPERATE_FREQ is not support"
#endif
}

__HIGHCODE __attribute__((optimize("-Os"))) 
void hal_sys_delay_us(uint16_t t)
{
    uint32_t i = 0;
#if(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 60000000)
    i = t * 15;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 48000000)
    i = t * 12;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 40000000)
    i = t * 10;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 32000000)
    i = t << 3;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 24000000)
    i = t * 6;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 16000000)
    i = t << 2;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 8000000)
    i = t << 1;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 4000000)
    i = t;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 2000000)
    i = t >> 1;
#elif(CONFIG_SOC_SYSTEM_OPERATE_FREQ == 1000000)
    i = t >> 2;
#endif
    do
    {
        __asm__ volatile("nop");
    } while(--i);
}


__HIGHCODE __attribute__((optimize("-Os"))) 
void hal_sys_delay_ms(uint16_t t)
{
    uint16_t i;

    for(i = 0; i < t; i++)
    {
        hal_sys_delay_us(1000);
    }
}