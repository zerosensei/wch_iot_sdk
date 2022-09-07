/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch57x_common.h>

uint16_t int32k_tune_flash = 0;
uint16_t int32k_tune_ram = 0;

void hal_clk_periph_clk_enable(uint16_t per)
{
    sys_safe_access_enable();
    R8_SLP_CLK_OFF0 &= ~(per & WCH_MASK(8));
    sys_safe_access_enable();
    R8_SLP_CLK_OFF1 &= ~(per >> 8);
    sys_safe_access_disable();
}

void hal_clk_periph_clk_disable(uint16_t per)
{
    sys_safe_access_enable();
    R8_SLP_CLK_OFF0 |= (per & WCH_MASK(8));
    sys_safe_access_enable();
    R8_SLP_CLK_OFF1 |= (per >> 8);
    sys_safe_access_disable();
}


//TODO: usb需要开启pll
__HIGHCODE void hal_clk_pwr_pll_on(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON;
    sys_safe_access_disable();
    
    for (int i = 0; i < 2000; i++) {
        __asm__ volatile("nop");
        __asm__ volatile("nop");
    }
}

__HIGHCODE void hal_clk_pwr_pll_off(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL &= ~RB_CLK_PLL_PON;  
    sys_safe_access_disable();
}

__HIGHCODE void hal_clk_pwr_xt32m_on(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_PON;
    sys_safe_access_disable();

    for (int i = 0; i < 1200; i++) {
        __asm__ volatile("nop");
        __asm__ volatile("nop");
    }
}

__HIGHCODE void hal_clk_pwr_xt32m_off(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL &= ~RB_CLK_XT32M_PON;
    sys_safe_access_disable();
}

__HIGHCODE static void hal_clk_sys_config(sys_clk_mode_t clk_mode, uint8_t div)
{
    uint8_t cfg = ((clk_mode << SYS_CLK_MODE_POS) & SYS_CLK_MODE_MSK) 
            | ((div << SYS_CLK_DIV_POS) & SYS_CLK_DIV_MSK);

    sys_safe_access_enable();
    R16_CLK_SYS_CFG = cfg;
    sys_safe_access_disable();

    __asm__ volatile("nop");
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    __asm__ volatile("nop");
}

__HIGHCODE void hal_clk_sys_setup(sys_clk_source_t clk)
{
    sys_safe_access_enable();
    R8_PLL_CONFIG &= ~(1 << 5); 
    sys_safe_access_disable();

    switch ((clk & SYS_CLK_MODE_MSK) >> SYS_CLK_MODE_POS) {
    case CLK_MODE_32K:
        /**
         * The clock source selection 32K frequency 
         * division factor is invalid. 
         */
        //XXX: 是否只能在RAM中运行
        hal_clk_sys_config(CLK_MODE_32K, 5U);
        break;
    case CLK_MODE_PLL:
        hal_clk_sys_config(CLK_MODE_PLL, clk & SYS_CLK_DIV_MSK);
        
        if (!hal_clk_pwr_xt32m_is_on()) {
            hal_clk_pwr_xt32m_on();
        }

        if (!hal_clk_pwr_pll_is_on()) {
            hal_clk_pwr_pll_on();
        }

        sys_safe_access_enable();
        R8_FLASH_CFG = 0x53;
        sys_safe_access_disable();
        break;
    case CLK_MODE_32M:
    default:
        hal_clk_sys_config(CLK_MODE_32M, clk & SYS_CLK_DIV_MSK);

        if (!hal_clk_pwr_xt32m_is_on()) {
            hal_clk_pwr_xt32m_on();
        }

        sys_safe_access_enable();
        R8_FLASH_CFG = 0x51;
        sys_safe_access_disable();
        break;
    }
}

uint32_t hal_clk_sys_get(void)
{
    uint8_t reg = R16_CLK_SYS_CFG & WCH_MASK(8);
    uint8_t div = (reg & SYS_CLK_DIV_MSK) >> SYS_CLK_DIV_POS;
    uint8_t mod = (reg & SYS_CLK_MODE_MSK) >> SYS_CLK_MODE_POS;
    uint32_t fsys;

    switch (mod) {
    case CLK_MODE_32K:
        fsys = CONFIG_SOC_RTC_FREQ;
        break;
    case CLK_MODE_PLL:
        fsys = FPLL / div;
        break;
    case CLK_MODE_32M:
    default:
        fsys = FHSE / div;
        break;
    }

    return fsys;
}

void hal_clk_lf_source_select(low_freq_source_t src)
{
    uint8_t cfg = 0;

    //XXX: 先开电源 再选择，否则异步信号不触发
    switch (src) {
    case LF_SOURCE_NONE:
        cfg = 0;
        break;
    case LF_SOURCE_LSE:
        cfg = RB_CLK_XT32K_PON | RB_CLK_OSC32K_XT;
        break;
    case LF_SOURCE_LSI:
    default:
        cfg = RB_CLK_INT32K_PON;
        break;
    }
    sys_safe_access_enable();
    R8_CK32K_CONFIG = cfg;
    sys_safe_access_disable();

    if (src == LF_SOURCE_LSE) {
        hal_clk_lse_cfg_current(LSE_CURRENT_200);
    }

    volatile uint8_t clk_pin;

    //FIXME: 异步信号可不可靠
    do {
        clk_pin = (R8_CK32K_CONFIG & RB_32K_CLK_PIN);
    } while ((clk_pin != (R8_CK32K_CONFIG & RB_32K_CLK_PIN)) || (!clk_pin)); 

    if (src == LF_SOURCE_LSE) {
        hal_clk_lse_cfg_current(LSE_CURRENT_100);
    }
}

void hal_clk_lse_cfg_current(lse_current_t cur)
{
    uint8_t reg_cur = R8_XT32K_TUNE & (~LSE_TUNE_IBIAS_MSK);

    reg_cur |= cur & LSE_TUNE_IBIAS_MSK;
    sys_safe_access_enable();
    R8_XT32K_TUNE = reg_cur;
    sys_safe_access_disable();
}

void hal_clk_lse_cfg_capacitance(lse_capacitance_t cap)
{
    uint8_t reg_cap = R8_XT32K_TUNE & (~LSE_TUNE_CLOAD_MSK);

    reg_cap |= (cap << LSE_TUNE_CLOAD_POS) & LSE_TUNE_CLOAD_MSK;
    sys_safe_access_enable();
    R8_XT32K_TUNE = reg_cap;
    sys_safe_access_disable();
}

void hal_clk_hse_cfg_current(hse_current_t cur)
{
    uint8_t reg_cur = R8_XT32M_TUNE & (~HSE_TUNE_IBIAS_MSK);

    reg_cur |= cur & HSE_TUNE_IBIAS_MSK;
    sys_safe_access_enable();
    R8_XT32M_TUNE = reg_cur;
    sys_safe_access_disable();
}

void hal_clk_hse_cfg_capacitance(hse_capacitance_t cap)
{
    uint8_t reg_cap = R8_XT32M_TUNE & (~HSE_TUNE_CLOAD_MSK);

    reg_cap |= (cap << HSE_TUNE_CLOAD_POS) & HSE_TUNE_CLOAD_MSK;
    sys_safe_access_enable();
    R8_XT32M_TUNE = reg_cap;
    sys_safe_access_disable();
}

static uint16_t hal_clk_lsi_calibrate_flash(void)
{
    uint16_t rev, basev;
    uint32_t calv;
    uint16_t i;
    uint16_t loc, loc_t = 0;
    float CNT_STEP_K;
    signed short diff_1, diff_2, diffc;
    uint8_t k = 0;

    /* 根据当前时钟获取标称值和斜率（T-step） */
    rev = R16_CLK_SYS_CFG & 0xff;

    if ((rev & RB_CLK_SYS_MOD) == (2 << 6)) {
        // 32M做主频
        calv = ((5 * 32000000 + (CONFIG_SOC_RTC_FREQ >> 1)) / CONFIG_SOC_RTC_FREQ);
        CNT_STEP_K = -1.6;
    } else if ((rev & RB_CLK_SYS_MOD) == (1 << 6)) {
        // PLL进行分频
        calv = (((uint32_t)5 * 480000000 / (rev & 0x1f) + (CONFIG_SOC_RTC_FREQ >> 1)) / CONFIG_SOC_RTC_FREQ);
        CNT_STEP_K = -0.1 * (rev & 0x1f);
    }
    else if ((rev & RB_CLK_SYS_MOD) == (0 << 6)) {
        // 32M进行分频
        calv = ((5 * 32000000 / (rev & 0x1f) + (CONFIG_SOC_RTC_FREQ >> 1)) / CONFIG_SOC_RTC_FREQ);
        CNT_STEP_K = -1.6 * (rev & 0x1f);
    } else {
        // 32K做主频
        calv = (5);
        CNT_STEP_K = 0;
    }

    basev = calv; // 获取校准标称值

    if (int32k_tune_flash) {
        loc = int32k_tune_flash;
    } else {
        loc = 2048;
    }
    //  if (loc == 2048)
    //    loc = 1837; //10040.625-1.640625*5000;  //经验曲线

    diff_2 = 0;
    diffc = 0;

    sys_safe_access_enable();
    R8_OSC_CAL_CTRL = RB_OSC_CNT_EN;
    do {
        sys_safe_access_enable();
        R16_INT32K_TUNE = loc;
        sys_safe_access_disable();

        /* 读取当前值 */
        while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
            ;
        i = R16_OSC_CAL_CNT; // 用于丢弃
        while (R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT)
            ;
        while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
            ;
        i = R16_OSC_CAL_CNT; // 实时校准后采样值
        k++;
        diff_1 = i - basev;

        int32k_tune_flash = loc;

        if (diff_1 == 0) {
            return 0; // 校准正好
        } else if ((diff_1 * diff_2) < 0) { 
            // 处于两点之间
            if ((diffc == 1) || (diffc == -1) || (diffc == 0)) {
                // 都变成正数
                if (diff_2 < 0) {
                    diff_2 = ~(diff_2 - 1);
                } else {
                    diff_1 = ~(diff_1 - 1);
                }

                if (diff_1 > diff_2) {
                    sys_safe_access_enable();
                    R16_INT32K_TUNE = loc_t;
                    sys_safe_access_disable();

                    return (diff_2 * 10000 / basev); // 返回误差值，万分之
                } else {
                    return (diff_1 * 10000 / basev);
                }
            }
        }

        if (((i > basev ? (i - basev) : (basev - i)) * 10000 / basev) < 11) {
            return ((i > basev ? (i - basev) : (basev - i)) * 10000 / basev);
        }

        // 保存上一次值
        diff_2 = diff_1;
        loc_t = loc;
        diffc = diff_1 * CNT_STEP_K;
        loc = loc - diffc;

        if (loc == loc_t) {
            if (diff_1 > 0) {
                loc = loc + 1; // 当前频率偏小
            } else {
                loc = loc - 1; // 当前频率偏大
            }
        }
    } while (k < 20);

    int32k_tune_flash = loc;

    return ((i > basev ? (i - basev) : (basev - i)) * 10000 / basev);
}

__HIGHCODE static uint16_t hal_lsi_get_calibrate_cnt_ram(uint16_t loc)
{
    uint16_t i;

    sys_safe_access_enable();
    R16_INT32K_TUNE = loc;
    sys_safe_access_disable();
    /* 读取当前值 */
    while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
        ;
    i = R16_OSC_CAL_CNT; // 用于丢弃

    while (R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT)
        ;
    while (!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT))
        ;
    i = R16_OSC_CAL_CNT;

    return i;
}

static uint16_t hal_clk_lsi_calibrate_ram(void)
{
    uint16_t rev, basev;
    uint32_t calv;
    uint16_t i;
    uint16_t loc, loc_t = 0;
    float CNT_STEP_K;
    signed short diff_1, diff_2, diffc;
    uint8_t k = 0;

    /* 根据当前时钟获取标称值和斜率（T-step） */
    rev = R16_CLK_SYS_CFG & 0xff;
    if ((rev & RB_CLK_SYS_MOD) == (2 << 6)) { 
        // 32M做主频
        calv = ((5 * 32000000 + (CONFIG_SOC_RTC_FREQ >> 1)) / CONFIG_SOC_RTC_FREQ);
        CNT_STEP_K = -1.6;
    } else if ((rev & RB_CLK_SYS_MOD) == (1 << 6)) { 
        // PLL进行分频
        calv = (((uint32_t)5 * 480000000 / (rev & 0x1f) + (CONFIG_SOC_RTC_FREQ >> 1)) / CONFIG_SOC_RTC_FREQ);
        CNT_STEP_K = -0.1 * (rev & 0x1f);
    } else if ((rev & RB_CLK_SYS_MOD) == (0 << 6)) { 
        // 32M进行分频
        calv = ((5 * 32000000 / (rev & 0x1f) + (CONFIG_SOC_RTC_FREQ >> 1)) / CONFIG_SOC_RTC_FREQ);
        CNT_STEP_K = -1.6 * (rev & 0x1f);
    } else { 
        // 32K做主频
        calv = (5);
        CNT_STEP_K = 0;
    }

    basev = calv; // 获取校准标称值

    if (int32k_tune_ram) {
        loc = int32k_tune_ram;
    }
    else {
        loc = 2048;
    }
    //  if (loc == 2048)
    //    loc = 1837; //10040.625-1.640625*5000;  //经验曲线

    diff_2 = 0;
    diffc = 0;

    sys_safe_access_enable();
    R8_OSC_CAL_CTRL = RB_OSC_CNT_EN;

    do {
        i = hal_lsi_get_calibrate_cnt_ram(loc); // 实时校准后采样值
        k++;
        diff_1 = i - basev;

        int32k_tune_ram = loc;

        if (diff_1 == 0) {
            return 0; // 校准正好
        } else if ((diff_1 * diff_2) < 0) { 
            // 处于两点之间
            if ((diffc == 1) || (diffc == -1) || (diffc == 0)) {
                // 都变成正数
                if (diff_2 < 0) {
                    diff_2 = ~(diff_2 - 1);
                } else {
                    diff_1 = ~(diff_1 - 1);
                }

                if (diff_1 > diff_2) {
                    sys_safe_access_enable();
                    R16_INT32K_TUNE = loc_t;
                    sys_safe_access_disable();

                    return (diff_2 * 10000 / basev); // 返回误差值，万分之
                } else {
                    return (diff_1 * 10000 / basev);
                }
            }
        }

        if (((i > basev ? (i - basev) : (basev - i)) * 10000 / basev) < 11) {
            return ((i > basev ? (i - basev) : (basev - i)) * 10000 / basev);
        }

        // 保存上一次值
        diff_2 = diff_1;
        loc_t = loc;
        diffc = diff_1 * CNT_STEP_K;
        loc = loc - diffc;

        if (loc == loc_t) {
            if (diff_1 > 0)
            {
                loc = loc + 1; // 当前频率偏小
            }
            else
            {
                loc = loc - 1; // 当前频率偏大
            }
        }
    } while (k < 20);

    int32k_tune_ram = loc;

    return ((i > basev ? (i - basev) : (basev - i)) * 10000 / basev);
}

void hal_lsi_set_tune_flash(void)
{
    sys_safe_access_enable();
    R16_INT32K_TUNE = int32k_tune_flash;
    sys_safe_access_disable();
}

void hal_lsi_set_tune_ram(void)
{
    sys_safe_access_enable();
    R16_INT32K_TUNE = int32k_tune_ram;
    sys_safe_access_disable();
}

void hal_clk_lsi_calibrate(void)
{
    if (hal_clk_lsi_calibrate_flash() > 15) {
        hal_clk_lsi_calibrate_flash();
    }

    if (hal_clk_lsi_calibrate_ram() > 15) {
        hal_clk_lsi_calibrate_ram();
    }
}