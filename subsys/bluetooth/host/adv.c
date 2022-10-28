/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <bluetooth/bluetooth.h>
#include <buf/buf.h>

BUF_SIMPLE_DEFINE_STATIC(adv_data, B_MAX_ADV_LEN);

int ble_adv_enable(const struct ble_adv_param *param,
            const struct ble_data *ad, size_t ad_len,
            const struct ble_data *sd, size_t sd_len)
{
    int err = 0;
    uint8_t adv_enable = true;
    uint8_t channel = param->channel;
    uint8_t filter = param->filter_policy;
    uint8_t adv_type = param->type;
    ble_addr_t *addr = param->dir_addr; 
    uint32_t adv_interval_min = param->interval_min;
    uint32_t adv_interval_max = param->interval_max;

    if (!param) {
        return -EINVAL;
    }

    err |= GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, 
                        sizeof(uint8_t), &adv_enable);
    err |= GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, 
                        sizeof(uint8_t), &adv_type);
    err |= GAPRole_SetParameter(GAPROLE_ADV_CHANNEL_MAP, 
                        sizeof(uint8_t), &channel);
    err |= GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, 
                        sizeof(uint8_t), &filter);

    err |= GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, adv_interval_min);
    err |= GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, adv_interval_max);

    if (addr) {
        err |= GAPRole_SetParameter(GAPROLE_ADV_DIRECT_TYPE, 
                            sizeof(uint8_t), &addr->type);
        err |= GAPRole_SetParameter(GAPROLE_ADV_DIRECT_ADDR, 
                            B_ADDR_LEN, &addr->addr);
    }

    if (ad) {
        buf_simple_reset(&adv_data);

        for (int i = 0; i < ad_len; i++) {
            (void)buf_simple_add_u8(&adv_data, ad[i].len + sizeof(ad[i].type));
            (void)buf_simple_add_u8(&adv_data, ad[i].type);
            (void)buf_simple_add_mem(&adv_data, ad[i].data, ad[i].len);
        }

        err |= GAPRole_SetParameter(GAPROLE_ADVERT_DATA, 
                            adv_data.len, adv_data.data);
    }

    if (sd) {
        buf_simple_reset(&adv_data);

        for (int i = 0; i < sd_len; i++) {
            (void)buf_simple_add_u8(&adv_data, sd[i].len + sizeof(sd[i].type));
            (void)buf_simple_add_u8(&adv_data, sd[i].type);
            (void)buf_simple_add_mem(&adv_data, sd[i].data, sd[i].len);
        }

        err |= GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, 
                            adv_data.len, adv_data.data);
    }

    if (IS_ENABLED(CONFIG_BT_EXT_ADV)) {

    }

    return err;
}

int ble_adv_disable(void)
{
    int err = 0;
    uint8_t adv_enable = false;

    err = GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, 
                        sizeof(uint8_t), &adv_enable);

    return err;
}

int ble_adv_restart(void)
{
    uint8_t advertising_enable = TRUE;
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advertising_enable);

    return 0;
}