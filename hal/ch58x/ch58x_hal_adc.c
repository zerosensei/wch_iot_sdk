/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch58x_common.h>


// ADC config (DMA)

// ADC channel

//ADC read

//ADC dma read

//adc convert

int hal_adc_dma_init(adc_dma_t *adc_dma)
{
    hal_adc_dma_reset();
    hal_adc_dma_enable();
    hal_adc_dma_loop(adc_dma->loop);
    hal_adc_dma_period(adc_dma->period);
    hal_adc_dma_int_set((adc_dma->ie_dma_end ? 
                    ADC_INT_FLAG_DMA_END : 0) | 
                    (adc_dma->ie_eoc ? 
                    ADC_INT_FLAG_DMA_COV_END : 0));

    return 0;
}

void hal_adc_init(void)
{
    hal_adc_reset();
    hal_adc_enable();
}



int hal_adc_config(adc_config_t *adc)
{   
    if (!adc) {
        return -WCH_ERROR_INVALID_PARAM;
    }

    hal_adc_temp_enable(false);
    hal_adc_buff(true);

    if (adc->diff) {
        hal_adc_diff(true);
        hal_adc_buff(false);
    } else {
        hal_adc_diff(false);
    }
    
    hal_adc_clock_div(adc->div);
    hal_adc_set_gain(adc->gain);
    hal_adc_set_channel(adc->channel);

    return 0;
}

int hal_adc_self_calibration(void)
{
    uint32_t sum = 0;
    adc_channel_t channel = hal_adc_get_channel();

    hal_adc_test_enable();

    /* Test mode support CH_INT_6 CH_INT_7 CH_INT_10 CH_INT_11 */
    hal_adc_set_channel(CH_INT_7);

    for(int i = 0; i < 16; i++) {
        hal_adc_start();
        sum += (~hal_adc_read_raw()) & WCH_MASK(11);
    }

    hal_adc_test_disbale();

    hal_adc_set_channel(channel);    
    sum = (sum + 8) >> 4; 

    return 2048 - sum;
}


int hal_adc_convert_celsius(uint16_t raw)
{
    uint32_t C25 = 0;
    int temp;

    C25 = (*((PUINT32)ROM_CFG_TMP_25C));

    /* current temperature = standard temperature + (adc deviation * adc linearity coefficient) */ 
    temp = (((C25 >> 16) & 0xFFFF) ? ((C25 >> 16) & 0xFFFF) : 25) + \
        (raw - ((int)(C25 & 0xFFFF))) * 10 / 27; 

    return temp;
}


uint32_t hal_adc_convert_millivolts(adc_pga_gain_t gain, uint16_t val)
{
    switch (gain) {
    case ADC_PGA_1_4:
        return ((val * 1050) >> 9) - 3150;
    case ADC_PGA_1_2:
        return ((val * 1050) >> 10) - 1050;
    case ADC_PGA_1:
        return (val * 1050) >> 11;
    case ADC_PGA_2:
        return ((val * 1050) >> 12) + 525;
    default: 
        return 0;
    }
}

