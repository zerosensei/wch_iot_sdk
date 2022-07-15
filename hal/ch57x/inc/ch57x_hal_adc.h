/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_HAL_ADC_H
#define HAL_CH57X_INC_CH57X_HAL_ADC_H

#include "ch57x_common.h"

#define ROM_CFG_TMP_25C    0x7F014

#define ADC_CHANNEL_POS            (0U)
#define ADC_CHANNEL_MSK            (15U << ADC_DMA_AUTO_POS)
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

#define ADC_CFG_BUF_POS             (1U)
#define ADC_CFG_BUF_MSK             (1U << ADC_CFG_BUF_POS)
typedef enum {
    ADC_BUF_DISABLE,
    ADC_BUF_ENABLE
} adc_cfg_buf_t;

#define ADC_CFG_DIFF_POS            (2U)
#define ADC_CFG_DIFF_MSK            (1U << ADC_CFG_DIFF_POS)
typedef enum {
    CH_SINGLE,
    CH_DIFF,
} adc_cfg_diff_t;

#define ADC_CFG_OFS_TEST_POS         (3U)
#define ADC_CFG_OFS_TEST_MSK         (1U << ADC_CFG_OFS_TEST_POS)
typedef enum {
    TEST_MODE_DISABLE,
    TEST_MODE_ENABLE,
} adc_cfg_test_mode_t;

#define ADC_CFG_PGA_GAIN_POS        (4U)
#define ADC_CFG_PGA_GAIN_MSK        (3U << ADC_CFG_PGA_GAIN_POS)
typedef enum {
    ADC_PGA_1_4 = 0, // -12dB, 1/4倍
    ADC_PGA_1_2,     // -6dB, 1/2倍
    ADC_PGA_0,       // 0dB, 1倍，无增益
    ADC_PGA_2,       // 6dB, 2倍
} adc_cfg_pga_gain_t;

#define ADC_CFG_CLK_DIV_POS         (6U)
#define ADC_CFG_CLK_DIV_MSK         (3U << ADC_CFG_CLK_DIV_POS)
typedef enum {
    SAMPLEFREQ_3_2 = 0, // 3.2M 采样频率
    SAMPLEFREQ_8,       // 8M 采样频率
    SAMPLEFREQ_5_33,    // 5.33M 采样频率
    SAMPLEFREQ_4,       // 4M 采样频率
} adc_cfg_clk_div_t;

#define ADC_DMA_LOOP_POS            (2U)
#define ADC_DMA_LOOP_MSK            (1U << ADC_DMA_LOOP_POS)
typedef enum {
    ADC_DMA_SINGLE,
    ADC_DMA_LOOP
} adc_dma_loop_t;

#define ADC_DMA_END_POS             (3U)
#define ADC_DMA_END_MSK             (1U << ADC_DMA_END_POS)
typedef enum {
    ADC_DMA_INT_END_DISABLE,
    ADC_DMA_INT_END_ENBALE
} adc_dma_int_end_t;

#define ADC_DMA_EOC_POS             (4U)
#define ADC_DMA_EOC_MSK             (1U << ADC_DMA_EOC_POS)
typedef enum {
    ADC_DMA_INT_EOC_DISABLE,
    ADC_DMA_INT_EOC_ENABLE
} adc_dma_int_eoc_t;

#define ADC_DMA_AUTO_POS            (1U)
#define ADC_DAM_AUTO_MSK            (1U << ADC_DMA_AUTO_POS)
typedef enum {
    ADC_DMA_AUTO_DISABLE,
    ADC_DMA_AUTO_ENABLE
} adc_dma_auto_t;

#define ADC_INT_FLAG_POS            (7U)
#define ADC_INT_FLAG_MSK            (1U << ADC_INT_FLAG_POS)

#define ADC_DMA_INT_END_ADC_FLAG_POS            (4U)
#define ADC_DMA_INT_END_ADC_FLAG_MSK            (1U << ADC_DMA_INT_END_ADC_FLAG_POS)

#define ADC_DMA_INT_DMA_END_FLAG_POS            (3U)
#define ADC_DMA_INT_DMA_END_FLAG_MSK            (1U << ADC_DMA_INT_DMA_END_FLAG_POS)

typedef struct {
    adc_cfg_buf_t buf;
    adc_cfg_diff_t diff;
    adc_cfg_test_mode_t test;
    adc_cfg_pga_gain_t pga_gain;
    adc_cfg_clk_div_t clk_div;
} hal_adc_cfg_t;

typedef struct {
    adc_dma_loop_t loop;
    adc_dma_ie_end_t end;
    adc_dma_ie_eoc_t eoc;
    adc_dma_auto_t auto;
    uint8_t auto_cycle;
} hal_adc_ctrl_dma_t;

/**
 * @brief Function for initializing the ADC.
 * 
 * This function write data to ADC config register. The ADC
 * and the touch-key share the same sampling channel and must 
 * disable touch-key power when sampling voltage or temperature.
 * 
 * @param cfg Configuration parameters.
 */
static inline void hal_adc_init(hal_adc_cfg_t *cfg)
{
    R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;

    R8_ADC_CFG = RB_ADC_POWER_ON 
            | ((cfg->buf << ADC_CFG_BUF_POS) & ADC_CFG_BUF_MSK)
            | ((cfg->diff << ADC_CFG_DIFF_POS) & ADC_CFG_DIFF_MSK)
            | ((cfg->test << ADC_CFG_OFS_TEST_POS) & ADC_CFG_OFS_TEST_MSK)
            | ((cfg->pga_gain << ADC_CFG_PGA_GAIN_POS) & ADC_CFG_PGA_GAIN_MSK)
            | ((cfg->clk_div << ADC_CFG_CLK_DIV_POS) & ADC_CFG_CLK_DIV_MSK);
}

static inline void hal_adc_uinit(void)
{
    R8_ADC_CFG = ((ADC_PGA_0 << ADC_CFG_PGA_GAIN_POS) & ADC_CFG_PGA_GAIN_MSK)
            |((SAMPLEFREQ_5_33 << ADC_CFG_CLK_DIV_POS) & ADC_CFG_CLK_DIV_MSK);
}


/**
 * @brief Function for setting ADC clock prescaler.
 * 
 * ATTENTION: The actual sampling frequency is about 1/18 of @param clk
 * 
 * @param clk ADC clock prescaler value.
 */
static inline void hal_adc_set_clock(adc_cfg_clk_div_t clk)
{
    R8_ADC_CFG | = (clk << ADC_CFG_CLK_DIV_POS) & ADC_CFG_CLK_DIV_MSK;
}

static inline void hal_adc_dma_init(hal_adc_ctrl_dma_t *dma_ctrl)
{
    R8_ADC_CTRL_DMA = RB_ADC_DMA_ENABLE
            | ((dma_ctrl->loop << ADC_DMA_LOOP_POS) & ADC_DMA_LOOP_MSK)
            | ((dma_ctrl->end << ADC_DMA_END_POS) & ADC_DMA_END_MSK)
            | ((dma_ctrl->eoc << ADC_DMA_EOC_POS) & ADC_DMA_EOC_MSK)
            | ((dma_ctrl->auto << ADC_DMA_AUTO_POS) & ADC_DAM_AUTO_MSK);

    R8_ADC_AUTO_CYCLE = dma_ctrl->auto_cycle;
}

static inline void hal_adc_dma_uninit(void)
{
    R8_ADC_CTRL_DMA = 0U;
}

static inline void hal_adc_dma_addr(uint8_t *pdata, uint16_t len)
{
    /*Only the lower 15 bits are valid, and the address must be 2-byte aligned.*/
    R16_ADC_DMA_BEG = ((uint16_t)(uint32_t)pdata) & WCH_MASK(15);
    R16_ADC_DMA_END = ((uint16_t)(uint32_t)(pdata + len)) & WCH_MASK(15);
}

static inline uint16_t hal_adc_dma_current_convert_count(void)
{
    return (R16_ADC_DMA_NOW - R16_ADC_DMA_BEG) / 2;
}


static inline uint8_t hal_adc_get_dma_int_flag(void)
{
    return R8_ADC_DMA_IF & (ADC_DMA_INT_DMA_END_FLAG_MSK 
            | ADC_DMA_INT_END_ADC_FLAG_MSK);
}

static inline void hal_adc_clear_dma_int_flag(uint8_t flag)
{
    R8_ADC_DMA_IF |= flag & (ADC_DMA_INT_DMA_END_FLAG_MSK 
            | ADC_DMA_INT_END_ADC_FLAG_MSK);
}

static inline uint8_t hal_adc_get_int_flag(void)
{
    return R8_ADC_INT_FLAG & ADC_INT_FLAG_MSK;
}

static inline void hal_adc_clear_int_flag(uint8_t flag)
{
    (void)flag;

    R8_ADC_CONVERT = 0;
}

static inline void hal_adc_set_channel(adc_channel_t channel) 
{
    R8_ADC_CHANNEL = (channel << ADC_CHANNEL_POS) & ADC_CHANNEL_MSK;
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

static inline uint16_t hal_adc_result_get(void)
{
    while(hal_adc_busy_check()){
        ;
    }

    return R16_ADC_DATA & WCH_MASK(11);
}


#endif /* HAL_CH57X_INC_CH57X_HAL_ADC_H */