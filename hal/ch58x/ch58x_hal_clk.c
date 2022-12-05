/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch58x_common.h>

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

static inline bool hal_clk_pwr_pll_is_on(void)
{
    return (R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON) ? true : false;
}

static inline bool hal_clk_pwr_xt32m_is_on(void)
{
    return (R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON) ? true : false;
}

//TODO: usb需要开启pll
static inline void hal_clk_pwr_pll_on(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON;
    sys_safe_access_disable();
    
    for (int i = 0; i < 2000; i++) {
        __asm__ volatile("nop");
        __asm__ volatile("nop");
    }
}

static inline void hal_clk_pwr_pll_off(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL &= ~RB_CLK_PLL_PON;  
    sys_safe_access_disable();
}

static inline void hal_clk_pwr_xt32m_on(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_PON;
    sys_safe_access_disable();

    for (int i = 0; i < 1200; i++) {
        __asm__ volatile("nop");
        __asm__ volatile("nop");
    }
}

static inline void hal_clk_pwr_xt32m_off(void)
{
    sys_safe_access_enable();
    R8_HFCK_PWR_CTRL &= ~RB_CLK_XT32M_PON;
    sys_safe_access_disable();
}

static inline void hal_clk_sys_config(sys_clk_mode_t clk_mode, uint8_t div)
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
        
        if (!hal_clk_pwr_xt32m_is_on()) {
            hal_clk_pwr_xt32m_on();
        }

        if (!hal_clk_pwr_pll_is_on()) {
            hal_clk_pwr_pll_on();
        }

        hal_clk_sys_config(CLK_MODE_PLL, clk & SYS_CLK_DIV_MSK);

        sys_safe_access_enable();
        R8_FLASH_CFG = 0x53;
        sys_safe_access_disable();

        if (clk == CLK_SOURCE_PLL_80MHZ) {
            sys_safe_access_enable();
            R8_FLASH_CFG = 0x02;
            sys_safe_access_disable();
        }

        break;
    case CLK_MODE_32M:
    default:

        if (!hal_clk_pwr_xt32m_is_on()) {
            hal_clk_pwr_xt32m_on();
        }

        hal_clk_sys_config(CLK_MODE_32M, clk & SYS_CLK_DIV_MSK);

        sys_safe_access_enable();
        R8_FLASH_CFG = 0x51;
        sys_safe_access_disable();
        break;
    }

    sys_safe_access_enable();
    R8_PLL_CONFIG |= 1 << 7;
    sys_safe_access_disable();
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

void hal_clk_lsi_calibrate(lsi_cali_level_t cali_Lv)
{
    UINT32 i;
    INT32  cnt_offset;
    UINT8  retry = 0;
    INT32  freq_sys;

    freq_sys = hal_clk_sys_get();

    sys_safe_access_enable();
    R8_CK32K_CONFIG |= RB_CLK_OSC32K_FILT;
    R8_CK32K_CONFIG &= ~RB_CLK_OSC32K_FILT;
    sys_safe_access_enable();
    R8_XT32K_TUNE &= ~3;
    R8_XT32K_TUNE |= 1;

    // 粗调
    sys_safe_access_enable();
    R8_OSC_CAL_CTRL &= ~RB_OSC_CNT_TOTAL;
    R8_OSC_CAL_CTRL |= 1;
    sys_safe_access_enable();
    R8_OSC_CAL_CTRL |= RB_OSC_CNT_EN;
    R16_OSC_CAL_CNT |= RB_OSC_CAL_OV_CLR;
    while( (R8_OSC_CAL_CTRL&RB_OSC_CNT_EN)!= RB_OSC_CNT_EN )
    {
        sys_safe_access_enable();
        R8_OSC_CAL_CTRL |= RB_OSC_CNT_EN;
    }
    while(1)
    {
        while(!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT));
        i = R16_OSC_CAL_CNT; // 用于丢弃
        while(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT);
        R16_OSC_CAL_CNT |= RB_OSC_CAL_OV_CLR;
        while(!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT));
        i = R16_OSC_CAL_CNT; // 实时校准后采样值
        cnt_offset = (i & 0x3FFF) + R8_OSC_CAL_OV_CNT * 0x3FFF - 2000 * (freq_sys / 1000) / CONFIG_SOC_RTC_FREQ;
        if(((cnt_offset > -37 * (freq_sys / 1000) / CONFIG_SOC_RTC_FREQ) && (cnt_offset < 37 * (freq_sys / 1000) / CONFIG_SOC_RTC_FREQ)) || retry > 2)
            break;
        retry++;
        cnt_offset = (cnt_offset > 0) ? (((cnt_offset * 2) / (37 * (freq_sys / 1000) / CONFIG_SOC_RTC_FREQ)) + 1) / 2 : (((cnt_offset * 2) / (37 * (freq_sys / 1000) / CONFIG_SOC_RTC_FREQ)) - 1) / 2;
        sys_safe_access_enable();
        R16_INT32K_TUNE += cnt_offset;
    }

    // 细调
    // 配置细调参数后，丢弃2次捕获值（软件行为）上判断已有一次，这里只留一次
    while(!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT));
    i = R16_OSC_CAL_CNT; // 用于丢弃
    R16_OSC_CAL_CNT |= RB_OSC_CAL_OV_CLR;
    sys_safe_access_enable();
    R8_OSC_CAL_CTRL &= ~RB_OSC_CNT_TOTAL;
    R8_OSC_CAL_CTRL |= cali_Lv;
    while(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT);
    while(!(R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT));
    i = R16_OSC_CAL_CNT; // 实时校准后采样值
    cnt_offset = (i & 0x3FFF) + R8_OSC_CAL_OV_CNT * 0x3FFF - 4000 * (1 << cali_Lv) * (freq_sys / 1000000) / CONFIG_SOC_RTC_FREQ * 1000;
    cnt_offset = (cnt_offset > 0) ? ((((cnt_offset * (3200 / (1 << cali_Lv))) / (1366 * (freq_sys / 1000) / CONFIG_SOC_RTC_FREQ)) + 1) / 2) << 5 : ((((cnt_offset * (3200 / (1 << cali_Lv))) / (1366 * (freq_sys / 1000) / CONFIG_SOC_RTC_FREQ)) - 1) / 2) << 5;
    sys_safe_access_enable();
    R16_INT32K_TUNE += cnt_offset;
    R8_OSC_CAL_CTRL &= ~RB_OSC_CNT_EN;
}