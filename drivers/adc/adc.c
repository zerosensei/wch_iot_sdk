/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <device.h>
#include <drivers/adc.h>
#include <logging/log.h>
#include "adc_context.h"

LOG_MODULE_REGISTER(wch_adc, CONFIG_ADC_LOG_LEVEL);

struct adc_wch_config {
    uint32_t clk_div;
    void (*irq_config_func)(void);
};

struct adc_wch_data {
    signed short offset;
	struct adc_context ctx;
	const struct device *dev;
	uint16_t *buffer;
	uint16_t *repeat_buffer;
};

#ifdef CONFIG_ADC_SUPPORT_INTERRUPT
static inline void adc_wch_handler(const struct device *dev)
{
    struct adc_wch_data *data = dev->data;

    if (hal_adc_get_dma_int_flag(ADC_INT_FLAG_DMA_END)) {
        /* Must stop DMA first */
        hal_adc_dma_stop();
        hal_adc_clear_dma_int_flag(ADC_INT_FLAG_DMA_END);

    }

    if (hal_adc_get_int_flag()) {
        hal_adc_clear_int_flag();
        *data->buffer = R16_ADC_DATA + data->offset;

		adc_context_on_sampling_done(&data->ctx, dev);
    }
}

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void ADC_IRQHandler(void)
{
    adc_wch_handler(DEVICE_GET(adc));
}

#endif /* CONFIG_ADC_SUPPORT_INTERRUPT */

static void adc_context_start_sampling(struct adc_context *ctx)
{
    struct adc_wch_data *data = CONTAINER_OF(ctx, struct adc_wch_data, ctx);

    data->repeat_buffer = data->buffer;

    hal_adc_start();
}


static void adc_context_update_buffer_pointer(struct adc_context *ctx,
					      bool repeat_sampling)
{
    struct adc_wch_data *data = CONTAINER_OF(ctx, struct adc_wch_data, ctx);

    if (repeat_sampling) {
        data->buffer = data->repeat_buffer;
    } else {
        data->buffer++;
    }
}

static int adc_wch_channel_setup(const struct device *dev, 
                const struct adc_channel_cfg *channel_cfg)
{
    const struct adc_wch_config *cfg = dev->cfg;
    struct adc_wch_data *data = dev->data;

    adc_config_t adc = {0};

    if (channel_cfg->channel_id == CH_INT_VTEMP) {
        hal_adc_config_temp();

        return 0;
    }

    if (channel_cfg->channel_id == CH_INT_VBAT) {
        hal_adc_config_bat();

        return 0;
    }

    switch (channel_cfg->gain) {
    case ADC_GAIN_1_4:
        adc.gain = ADC_PGA_1_4;
        break;
    case ADC_GAIN_1_2:
        adc.gain = ADC_PGA_1_2;
        break;
    case ADC_GAIN_1:
        adc.gain = ADC_PGA_1;
        break;
    case ADC_GAIN_2:
        adc.gain = ADC_PGA_2;
        break;
    default:
        LOG_ERR("Selected ADC gain is not valid");
        return -ENOTSUP;
    }

    if (!(BIT(channel_cfg->channel_id) & CONFIG_ADC_CHANNEL_MAP)) {
        LOG_ERR("Invalid channel (%u)\n", channel_cfg->channel_id);
        return -EINVAL;
    }

    if (channel_cfg->acquisition_time != ADC_ACQ_TIME_DEFAULT) {
        LOG_ERR("Acquisition time is not supported");
        return -ENOTSUP;
    }

    if (channel_cfg->differential) {
        adc.diff = true;
    }

    adc.div = cfg->clk_div;

    hal_adc_config(&adc);

    int key = irq_lock();

    if (channel_cfg->calibrate) {
        data->offset = hal_adc_self_calibration();
    }
    
    irq_unlock(key);

    return 0;
}


static int check_buffer_size(const struct adc_sequence *sequences)
{
	size_t needed_buffer_size;

	needed_buffer_size = sizeof(uint16_t);
	if (sequences->options) {
		needed_buffer_size *= (1 + sequences->options->extra_samplings);
	}

	if (sequences->buffer_size < needed_buffer_size) {
		LOG_ERR("Provided buffer is too small (%u/%u)",
				sequences->buffer_size, needed_buffer_size);
		return -ENOMEM;
	}

	return 0;
}

static struct adc_channel_cfg adc_wch_channel_config_get(const struct device *dev)
{
    struct adc_channel_cfg cfg = {0};

    cfg.acquisition_time = ADC_ACQ_TIME_DEFAULT;
    cfg.calibrate = false;
    cfg.channel_id = hal_adc_get_channel();
    cfg.differential = hal_adc_is_diff();

    return cfg;
}

static int adc_wch_read(const struct device *dev,
                const struct adc_sequence *sequence)
{
    int err;
    struct adc_wch_data *data = dev->data;
    uint32_t index = find_lsb_set(sequence->channels) - 1;

    if (sequence->channels > BIT(index)) {
		LOG_ERR("Only single channel supported");
        return -ENOTSUP;
    }

    hal_adc_set_channel(index);

    err = check_buffer_size(sequence);
    if (err) {
        return err;
    }

    data->buffer = sequence->buffer;

    adc_context_start_read(&data->ctx, sequence);

    return 0;
}

static bool adc_wch_is_completed(const struct device *dev)
{
    struct adc_wch_data *data = dev->data;

    return adc_context_status(&data->ctx) == 0 ? true : false; 
}


static int adc_wch_convert(const struct device *dev,
                const struct adc_channel_cfg *channel_cfg, 
                int32_t *pval)
{
    ARG_UNUSED(dev);

    if (channel_cfg->channel_id == CH_INT_VTEMP) {
        *pval = hal_adc_convert_celsius(*pval);

        return 0;
    }

    switch (channel_cfg->gain) {
    case ADC_GAIN_1_4:
        *pval = hal_adc_convert_millivolts(ADC_PGA_1_4, *pval);
        break;
    case ADC_GAIN_1_2:
        *pval = hal_adc_convert_millivolts(ADC_PGA_1_2, *pval);
        break;
    case ADC_GAIN_1:
        *pval = hal_adc_convert_millivolts(ADC_PGA_1, *pval);
        break;
    case ADC_GAIN_2:
        *pval = hal_adc_convert_millivolts(ADC_PGA_2, *pval);
        break;
    default:
        return -ENOTSUP;
    }

    return 0;
}


static struct adc_driver_api adc_wch_driver_api = {
	.channel_setup = adc_wch_channel_setup,
    .get_config = adc_wch_channel_config_get,
	.read = adc_wch_read,
    .convert = adc_wch_convert,
    .is_completed = adc_wch_is_completed,
};


static int adc_wch_init(const struct device *dev)
{
    const struct adc_wch_config *cfg = dev->cfg;

    hal_adc_init();
#ifdef CONFIG_ADC_SUPPORT_INTERRUPT
    cfg->irq_config_func();
#else
    ARG_UNUSED(cfg);
#endif
    return 0;
}


#define ADC_INIT()      \
    static void adc_wch_irq_cfg_func(void)  \
    {       \
        irq_enable(DEV_CFG_GET_IRQ(adc));       \
    }       \
    static const struct adc_wch_config adc_cfg = { \
        .clk_div = SAMPLEFREQ_5_33,    \
        .irq_config_func = adc_wch_irq_cfg_func,    \
    };  \
    static struct adc_wch_data adc_data = {0};       \
    static struct device_state adc_sta = {0};       \
    DEVICE_DEFINE(adc, adc_wch_init, NULL,       \
                &adc_data,      \
                &adc_cfg,       \
                &adc_wch_driver_api,        \
                &adc_sta,       \
                DRIVER,     \
                1)

ADC_INIT();

