/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <common.h>
#include <device.h>
#include "ch57x_hal_adc.h"

int adc_wch_init(const struct device *dev)
{
    //enable adc
    adc_init();

    //self clibration
    adc_self_calibration();
	return 0;
}


int adc_wch_channel_setup(const struct device *dev,
				  const struct adc_channel_cfg *channel_cfg)
{

    //set channel id
    channel_cfg->channels

    //gain

    //Acquisition

    //channel

    //diff

}

int adc_wch_read(const struct device *dev,
		       const struct adc_sequence *sequence)
{

    //channel id >> channel

    //buffer = data



}

uint16_t adc_wch_ref_internal(const struct device *dev)
{
    return 1050;
}

int adc_wch_gain_invert(enum adc_gain gain,
		    int32_t *value)
{

}

int adc_wch_raw_to_millivolts(int32_t ref_mv,
					enum adc_gain gain,
					uint8_t resolution,
					int32_t *valp)
{
    
}

