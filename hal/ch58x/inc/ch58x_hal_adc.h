/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH58X_INC_CH58X_HAL_ADC_H
#define HAL_CH58X_INC_CH58X_HAL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch58x_common.h"

#define ROM_CFG_TMP_25C    0x7F014

#define ADC_CHANNEL_MSK            WCH_MASK(4)
typedef enum {
    CH_EXT_0,
    CH_EXT_1,
    CH_EXT_2,
    CH_EXT_3,
    CH_EXT_4,
    CH_EXT_5,
    CH_INT_6,
    CH_INT_7,
    CH_EXT_8,
    CH_EXT_9,
    CH_INT_10,
    CH_INT_11,
    CH_EXT_12,
    CH_EXT_13,
    CH_INT_VBAT,
    CH_INT_VTEMP,
} adc_channel_t;



#define ADC_CFG_PGA_GAIN_POS        (4U)
#define ADC_CFG_PGA_GAIN_MSK        (3U << ADC_CFG_PGA_GAIN_POS)
typedef enum {
    ADC_PGA_1_4 = 0, /* -12dB, x 1/4 */
    ADC_PGA_1_2,     /* -6dB, x 1/2 */
    ADC_PGA_1,       /* 0dB, x 1 */
    ADC_PGA_2,       /* 6dB, x 2 */
} adc_pga_gain_t;

#define ADC_CFG_CLK_DIV_POS         (6U)
#define ADC_CFG_CLK_DIV_MSK         (3U << ADC_CFG_CLK_DIV_POS)
typedef enum {
    SAMPLEFREQ_3_2 = 0, /* 3.2MHz */
    SAMPLEFREQ_8,       /* 8MHz */
    SAMPLEFREQ_5_33,    /* 5.33MHz */
    SAMPLEFREQ_4,       /* 4MHz */
} adc_clk_div_t;

#define ADC_DMA_INT_FLAG_POS        (3U)
#define ADC_DMA_INT_FLAG_MSK        (2U << ADC_DMA_INT_FLAG_POS)

typedef enum {
    ADC_INT_FLAG_DMA_END = WCH_BIT(3),
    ADC_INT_FLAG_DMA_COV_END = WCH_BIT(4),
} adc_int_flag_t;

typedef struct {
    bool diff;
    bool test;
    adc_pga_gain_t gain;
    adc_clk_div_t div;
    adc_channel_t channel;
} adc_config_t;

typedef struct {
    bool loop;
    uint16_t period;
    bool ie_dma_end;
    bool ie_eoc;
} adc_dma_t;



/**
 * @brief Function for initializing the ADC.
 * 
 * This function write data to ADC config register. The ADC
 * and the touch-key share the same sampling channel and must 
 * disable touch-key power when sampling voltage or temperature.
 * 
 * @param cfg Configuration parameters.
 */



static inline void hal_adc_enable(void)
{
    R8_ADC_CFG |= RB_ADC_POWER_ON;
}

static inline void hal_adc_disable(void)
{
    R8_ADC_CFG &= ~RB_ADC_POWER_ON;
}

static inline void hal_adc_buff(bool enable)
{
    if (enable) {
        R8_ADC_CFG |= RB_ADC_BUF_EN;
    } else {
        R8_ADC_CFG &= ~RB_ADC_BUF_EN;
    }
}

static inline void hal_adc_diff(bool enable)
{
    if (enable) {
        R8_ADC_CFG |= RB_ADC_DIFF_EN;
    } else {
        R8_ADC_CFG &= ~RB_ADC_DIFF_EN;
    }
}

static inline bool hal_adc_is_diff(void)
{
    return (R8_ADC_CFG & RB_ADC_DIFF_EN) ? true : false;
}

static inline void hal_adc_test_enable(void)
{
    R8_ADC_CFG |= RB_ADC_OFS_TEST;
}

static inline void hal_adc_test_disbale(void)
{
    R8_ADC_CFG &= ~RB_ADC_OFS_TEST;
}

/**
 * @brief Function for setting ADC clock prescaler.
 * 
 * ATTENTION: The actual sampling frequency is about 1/18 of @param clk
 * 
 * @param clk ADC clock prescaler value.
 */
static inline void hal_adc_clock_div(adc_clk_div_t clk)
{
    R8_ADC_CFG &= ~ADC_CFG_CLK_DIV_MSK;
    R8_ADC_CFG |= (clk << ADC_CFG_CLK_DIV_POS) & ADC_CFG_CLK_DIV_MSK;
}

static inline adc_clk_div_t hal_adc_get_clock_div(void)
{
    return (R8_ADC_CFG & ADC_CFG_CLK_DIV_MSK) >> ADC_CFG_CLK_DIV_POS;
}

static inline void hal_adc_set_gain(adc_pga_gain_t gain)
{
    R8_ADC_CFG &= ~ADC_CFG_PGA_GAIN_MSK;
    R8_ADC_CFG |= (gain << ADC_CFG_PGA_GAIN_POS) & ADC_CFG_PGA_GAIN_MSK;
}

static inline adc_pga_gain_t hal_adc_get_gain(void)
{
    return (R8_ADC_CFG & ADC_CFG_PGA_GAIN_MSK) >> ADC_CFG_PGA_GAIN_POS;
}

static inline void hal_adc_set_channel(adc_channel_t channel) 
{
    R8_ADC_CHANNEL = channel & ADC_CHANNEL_MSK;
}

static inline adc_channel_t hal_adc_get_channel(void)
{
    return R8_ADC_CHANNEL & ADC_CHANNEL_MSK;
}

static inline void hal_adc_start(void)
{
    R8_ADC_CONVERT = RB_ADC_START;
}

static inline bool hal_adc_busy_check(void)
{
    //XXX: RB_ADC_EOC_X ? RB_ADC_START
    return R8_ADC_CONVERT & RB_ADC_START;
}

static inline uint16_t hal_adc_read_raw(void)
{
    while (hal_adc_busy_check()) {
        ;
    }

    return R16_ADC_DATA & WCH_MASK(12);
}

static inline uint8_t hal_adc_get_int_flag(void)
{
    return R8_ADC_INT_FLAG & RB_ADC_IF_EOC;
}

static inline void hal_adc_clear_int_flag(void)
{
    R8_ADC_CONVERT = 0;
}


/* DMA */
static inline void hal_adc_dma_enable(void)
{
    R8_ADC_CTRL_DMA |= RB_ADC_DMA_ENABLE;
}

static inline void hal_adc_dma_reset(void)
{
    R8_ADC_CTRL_DMA = 0U;
}

static inline void hal_adc_dma_addr(uint32_t addr, uint16_t len)
{
    /*Only the lower 15 bits are valid, and the address must be 2-byte aligned.*/
    R16_ADC_DMA_BEG = ((uint16_t)addr) & WCH_MASK(15);
    R16_ADC_DMA_END = ((uint16_t)(addr + len)) & WCH_MASK(15);
}

static inline void hal_adc_dma_period(uint16_t period)
{
    /* Sampling Period = (256 - R8_ADC_AUTO_CYCLE) * 16 * Tsys */
    R8_ADC_AUTO_CYCLE = 256 - (min(period + 8, 4096) >> 4);  
}

static inline void hal_adc_dma_loop(bool enable)
{
    if (enable) {
        R8_ADC_CTRL_DMA |= RB_ADC_DMA_LOOP;
    } else {
        R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_LOOP;
    }
}

static inline void hal_adc_dma_start(void)
{
    R8_ADC_CTRL_DMA |= RB_ADC_AUTO_EN;
}

static inline void hal_adc_dma_stop(void)
{
    R8_ADC_CTRL_DMA &= ~RB_ADC_AUTO_EN;
    R16_ADC_DMA_BEG = WCH_MASK(16);
}

static inline uint16_t hal_adc_dma_current_count(void)
{
    return (R16_ADC_DMA_NOW - R16_ADC_DMA_BEG) / 2;
}

static inline void hal_adc_dma_int_set(uint8_t flags)
{
    R8_ADC_CTRL_DMA &= ~ADC_DMA_INT_FLAG_MSK;
    R8_ADC_CTRL_DMA |= flags & ADC_DMA_INT_FLAG_MSK;
}


static inline bool hal_adc_get_dma_int_flag(adc_int_flag_t flag)
{
    return (R8_ADC_DMA_IF & flag) ? true : false; 
}

static inline void hal_adc_clear_dma_int_flag(uint8_t flags)
{
    R8_ADC_DMA_IF |= flags & (RB_ADC_IF_END_ADC |
            RB_ADC_IF_END_ADC);
}


static inline void hal_adc_reset(void)
{
    R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;

    R8_ADC_CFG = ((ADC_PGA_1 << ADC_CFG_PGA_GAIN_POS) & ADC_CFG_PGA_GAIN_MSK)
            |((SAMPLEFREQ_5_33 << ADC_CFG_CLK_DIV_POS) & ADC_CFG_CLK_DIV_MSK);
}


static inline void hal_adc_temp_enable(bool enbale)
{
    if (enbale) {
        R8_TEM_SENSOR = RB_TEM_SEN_PWR_ON;
    } else {
        R8_TEM_SENSOR = 0;
    }
}


static inline void hal_adc_config_temp(void)
{
    hal_adc_temp_enable(true);
    hal_adc_buff(false);
    hal_adc_diff(true);
    hal_adc_set_gain(ADC_PGA_2);
    hal_adc_clock_div(SAMPLEFREQ_3_2);
}

static inline void hal_adc_config_bat(void)
{
    hal_adc_buff(true);
    hal_adc_diff(false);
    hal_adc_set_gain(ADC_PGA_1_4);
    hal_adc_clock_div(SAMPLEFREQ_3_2);
}

void hal_adc_init(void);

int hal_adc_config(adc_config_t *adc);

int hal_adc_dma_init(adc_dma_t *adc_dma);

int hal_adc_self_calibration(void);

int hal_adc_convert_celsius(uint16_t raw);

uint32_t hal_adc_convert_millivolts(adc_pga_gain_t gain, uint16_t val);
 

#ifdef __cplusplus
}
#endif

#endif /* HAL_CH58X_INC_CH58X_HAL_ADC_H */