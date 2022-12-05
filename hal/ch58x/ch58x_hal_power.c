/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch58x_common.h>

void hal_pwr_dcdc_enable(bool enable)
{
    if (enable) {
        sys_safe_access_enable();
        R16_POWER_PLAN &= ~(RB_PWR_DCDC_EN|RB_PWR_DCDC_PRE);
        sys_safe_access_disable();	
    } else {
        uint32_t hw_info[2];

        FLASH_EEPROM_CMD(CMD_GET_ROM_INFO, ROM_CFG_ADR_HW, hw_info, 0);
        if ((hw_info[0]) & WCH_BIT(13)) {
            return;
        }

        sys_safe_access_enable();
        R16_POWER_PLAN |= RB_PWR_DCDC_EN | RB_PWR_DCDC_PRE;
        sys_safe_access_disable();	
    }
}


void hal_pwr_wakeup_mode(pwr_wakeup_mode_t mode)
{
    uint8_t cfg = 0;

    switch (mode) {
    case PWR_WAKEUP_MODE_LONG:
        cfg = 0;
        break;
    case PWR_WAKEUP_MODE_SHORT:
        cfg = 0x01;
        break;
    default:
        cfg = 0x01;
        break;
    }
    
    sys_safe_access_enable();
    R8_SLP_POWER_CTRL &= ~(RB_WAKE_EV_MODE);
    sys_safe_access_enable();
    R8_SLP_POWER_CTRL |= cfg;
    R8_SLP_WAKE_CTRL |= RB_WAKE_EV_MODE;
    sys_safe_access_disable();
}

void hal_pwr_wakeup_source(uint8_t source, bool enable)
{
    uint8_t temp = R8_SLP_WAKE_CTRL;

    if (enable) {
        temp |= source;
    } else {
        temp &= ~source;
    }

    sys_safe_access_enable();
    R8_SLP_WAKE_CTRL = temp;
    sys_safe_access_disable();
}

void hal_pwr_monitor_config(pwr_monitor_level_t level)
{
    uint8_t ctrl = R8_BAT_DET_CTRL;
    uint8_t cfg = R8_BAT_DET_CFG;

    if (level == PWR_MONITOR_OFF) {
        sys_safe_access_enable();
        R8_BAT_DET_CTRL = 0;
        sys_safe_access_disable();

        return;
    }

    if (level & 0x80) {
        cfg = level & 0x03;
        ctrl = RB_BAT_MON_EN | ((level >> 2) & 1);
    } else {
        cfg = level & 0x03;
        ctrl = RB_BAT_DET_EN;
    }

    sys_safe_access_enable();
    R8_BAT_DET_CTRL = ctrl;
    R8_BAT_DET_CFG = cfg;
    sys_safe_access_disable();

    hal_sys_delay_us(1);
    sys_safe_access_enable();
    R8_BAT_DET_CTRL |= RB_BAT_LOW_IE | RB_BAT_LOWER_IE;
    sys_safe_access_disable();
}

__HIGHCODE void hal_pwr_enter_idle(void)
{
    FLASH_ROM_SW_RESET();
    R8_FLASH_CTRL = 0x04;

    PFIC->SCTLR &= ~(1 << 2); // sleep
    __WFI();
    __asm__ volatile("nop");
    __asm__ volatile("nop");
}

__HIGHCODE void hal_pwr_enter_halt(void)
{
    //FIXME: 需不需要关低压监控

    FLASH_ROM_SW_RESET();
    R8_FLASH_CTRL = 0x04; 
    sys_safe_access_enable();
    R8_PLL_CONFIG |= (1 << 5);
    sys_safe_access_disable();

    PFIC->SCTLR |= (1 << 2); //deep sleep
    __WFI();
    __asm__ volatile("nop");
    __asm__ volatile("nop");

    sys_safe_access_enable();
    R8_PLL_CONFIG &= ~(1 << 5);
    sys_safe_access_disable();

    //FIXME: 需不需要时间等待flash
}

__HIGHCODE void hal_pwr_enter_sleep(uint8_t peri)
{
    //FIXME: 需不需要关低压监控

    FLASH_ROM_SW_RESET();
    R8_FLASH_CTRL = 0x04; 
    sys_safe_access_enable();
    R8_PLL_CONFIG |= (1 << 5);
    sys_safe_access_enable();
    R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
    sys_safe_access_enable();
    R16_POWER_PLAN = RB_PWR_PLAN_EN | RB_PWR_MUST_0010 | RB_PWR_CORE | peri;
    sys_safe_access_disable();

    PFIC->SCTLR |= (1 << 2); //deep sleep
    __WFI();
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    
    sys_safe_access_enable();
    R8_PLL_CONFIG &= ~(1 << 5);
    sys_safe_access_disable();
    hal_sys_delay_us(70);

    //TODO:dcdc
}

__HIGHCODE void hal_pwr_enter_shutdown(uint8_t peri)
{
    //FIXME: 需不需要关低压监控

    FLASH_ROM_SW_RESET();
    R8_FLASH_CTRL = 0x04; 

    hal_clk_sys_setup(CLK_SOURCE_HSE_6_4MHZ);

    sys_safe_access_enable();
    R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
    sys_safe_access_enable();
    R16_POWER_PLAN = RB_PWR_PLAN_EN | RB_PWR_MUST_0010 | peri;
    sys_safe_access_disable();

    PFIC->SCTLR |= (1 << 2); //deep sleep
    __WFI();
    __asm__ volatile("nop");
    __asm__ volatile("nop");
}


