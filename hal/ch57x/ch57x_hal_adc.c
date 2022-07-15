/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ch57x_hal_adc.h"
// #include "ch57x_hal_gpio.h"

#define ADC_CFG_SINGLE_SAMPE        \
    {                               \
        .buf = ADC_BUF_ENABLE,      \
        .dff = CH_SINGLE,           \
        .pag_gain = ADC_PGA_0,      \
        .clk_div = ADC_PGA_2,       \
    }                               \

#define ADC_CFG_OFFSET_TEST         \
    {                               \
        .test = TEST_MODE_ENABLE,   \
        .pag_gain = ADC_PGA_0,      \
    }                               \

static int16_t adc_offset = 0;

void adc_init(void)
{
    hal_adc_cfg_t adc = ADC_CFG_SINGLE_SAMPE;

    hal_adc_init(&adc);
}

int adc_channel_enable(adc_channel_t ch)
{
    if(ch == CH_INT_6 || ch == CH_INT_7
            || ch == CH_INT_10 || CH_INT_11) 
        return -WCH_ERROR_NOT_SUPPORTED;

    /* cfg gpio */
}

void adc_channel_disable(adc_channel_t ch)
{

}

void adc_single_sample_0_6v_1_5v(void)
{
    hal_adc_cfg_t adc = {
        .buf = ADC_BUF_ENABLE,
        .diff = CH_SINGLE,
        .pag_gain = ADC_PGA_2;
    };

    hal_adc_init(&adc);

    
}

uint16_t adc_result_get(void)
{
    return hal_adc_result_get();
}


void adc_self_calibration(void)
{
    hal_adc_cfg_t adc = ADC_CFG_OFFSET_TEST;
    uint32_t sum;

    /* Test mode support CH_INT_6 CH_INT_7 CH_INT_10 CH_INT_11 */
    hal_adc_set_channel(CH_INT_6);
    hal_adc_init(&adc);

    for(int i = 0; i < 16; i++) {
        hal_adc_start();
        sum += (~hal_adc_result_get()) & WCH_MASK(11);
    }

    sum = (sum + 8) >> 4; 
    hal_adc_uinit();

    adc_offset = 2048 - sum;
}


__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void ADC_IRQHandler(void)
{

}