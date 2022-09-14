/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <drivers/adc.h>
#include <drivers/gpio.h>

static enum adc_action repeated_sampings_callback(const struct device *dev,
                            const struct adc_sequence *sequence,
                            uint16_t sampling_index);

const struct device *adc = DEVICE_GET(adc);
const struct device *gpioa = DEVICE_GET(gpioa);

const struct adc_channel_cfg channel_cfg = {
    .channel_id = 1,
    .gain = ADC_GAIN_1_2,
};

const struct adc_channel_cfg channel_cfg_temp = {
    .channel_id = 15,
};

const struct adc_sequence_options options = {
    .callback = repeated_sampings_callback,
    .extra_samplings = 16,
};

uint16_t buf[20];

struct adc_sequence seq = {
    .buffer = buf,
    .buffer_size = sizeof(buf),
    .channels = BIT(1),
    .options = &options,
};

struct adc_sequence seq_temp = {
    .buffer = buf,
    .buffer_size = sizeof(buf),
    .channels = BIT(15),
};

static enum adc_action repeated_sampings_callback(const struct device *dev,
                            const struct adc_sequence *sequence,
                            uint16_t sampling_index)
{
    static uint8_t done_count = 0;

    ++done_count;

    /* Skip the first sample */
    if (done_count == 1U) {
        return ADC_ACTION_REPEAT;
    } else {
        if (done_count > 16) {
            done_count = 0; 
        }

        return ADC_ACTION_CONTINUE;
    }
}

int main(void)
{
    int err;
    printk("This is ADC example\n");
    if (!device_is_ready(adc)) {
        printk("ADC deivice not ready\n");
        
        while(1);
    }

    err = adc_channel_setup(adc, &channel_cfg_temp);
    if (err) {
       printk("Channel #%d setup error (%d)\n", channel_cfg_temp.channel_id, err); 
    }

    printk("Current temperature");

    err = adc_read(adc, &seq_temp);
    while(!adc_read_is_completed(adc)) {
        ;
    }
    int32_t temp = buf[0];
    err = adc_convert(adc, &channel_cfg_temp,
                    &temp);

    if (err < 0) {
        printk(" (value in Celsius not available)\n");
    } else {
        printk(" = %"PRId32" Celsius\n", temp);
    }

    gpio_pin_configure(gpioa, 5, GPIO_INPUT);

    err = adc_channel_setup(adc, &channel_cfg);

    if (err) {
       printk("Channel #%d setup error (%d)\n", channel_cfg.channel_id, err); 
    }

    printk("ADC reading:\n");
    while(1) {
        uint32_t sum = 0;

        printk("- channel %d: ", channel_cfg.channel_id);
    
        err = adc_read(adc, &seq);
        while(!adc_read_is_completed(adc)) {
            ;
        }
        if (err) {
            printk("Could not read (%d)\n", err);
        } 

        for(int i = 0; i < 16; i++) {
            sum += buf[i];
        }

        int32_t val_mv = (sum + 8) / 16;

        err = adc_convert(adc, &channel_cfg,
                        &val_mv);

        if (err < 0) {
            printk(" (value in mV not available)\n");
        } else {
            printk(" = %"PRId32" mV\n", val_mv);
        }

        k_busy_wait(1000*1000);    
    }

}

