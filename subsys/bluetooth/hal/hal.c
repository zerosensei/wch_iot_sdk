/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <drivers/eeprom.h>
#include <drivers/adc.h>
#include <pm/pm.h>
#include <pm/device.h>
#include "hal.h"
#include <logging/log.h>

LOG_MODULE_REGISTER(ble_init, CONFIG_BT_LOG_LEVEL);

tmosTaskID ble_hal_taskid = INVALID_TASK_ID;

static void ble_hal_process_tmosmsg(tmos_event_hdr_t *pmsg)
{
    switch (pmsg->event) {

        default:
            break;
    }
}

static tmosEvents ble_hal_process_event(tmosTaskID task_id, tmosEvents events)
{
    if (events & SYS_EVENT_MSG) {
        uint8_t *msg = tmos_msg_receive(task_id);

        if (msg) {
            ble_hal_process_tmosmsg((tmos_event_hdr_t *)msg);
            /* De-allocate */
            tmos_msg_deallocate(msg);
        }

        return events ^ SYS_EVENT_MSG;
    }

#if defined CONFIG_BT_CALIBRATION
    if (events & HAL_REG_INIT_EVENT) {
        /* rf calibration */
        BLE_RegInit();  
#if (defined CONFIG_SOC_LSI_32000) || (defined CONFIG_SOC_LSI_32768)
        ble_hal_lsi_calibrate();
#endif
        return events ^ HAL_REG_INIT_EVENT;
    }
#endif /* CONFIG_BT_CALIBRATION */

    return 0;
}

void ble_hal_init(void)
{
    ble_hal_taskid = TMOS_ProcessEventRegister(ble_hal_process_event);

    __ASSERT_NO_MSG(ble_hal_taskid != INVALID_TASK_ID);

    TMOS_TimerInit(0);

#if defined CONFIG_BT_CALIBRATION
    tmos_start_reload_task(ble_hal_taskid, HAL_REG_INIT_EVENT, 
            MS1_TO_SYSTEM_TIME(CONFIG_BT_CALIBRATION_PERIOD));
#endif 

#if defined CONFIG_PM
    pm_device_wakeup_enable(DEVICE_GET(rtc), true);
    __ASSERT(pm_device_wakeup_is_enabled(DEVICE_GET(rtc)), 
            "wakeup source is not enabled");
#endif
}

uint16_t ble_hal_get_inter_temp(void)
{
    uint16_t raw_adc;

    const struct adc_channel_cfg channel_cfg = {
        .channel_id = 15,
    };

    struct adc_channel_cfg cfg_save;
    struct adc_sequence seq_temp = {
        .buffer = &raw_adc,
        .buffer_size = sizeof(raw_adc),
        .channels = BIT(15),
    };

    cfg_save = adc_channel_config_get(DEVICE_GET(adc));
    adc_channel_setup(DEVICE_GET(adc), &channel_cfg);
    adc_read(DEVICE_GET(adc), &seq_temp);

    while(!adc_read_is_completed(DEVICE_GET(adc))) {
        ;
    }

    adc_channel_setup(DEVICE_GET(adc), &cfg_save);

    return raw_adc;
}

void ble_hal_lsi_calibrate(void)
{
#ifdef CONFIG_SOC_SERIES_CH57X
    hal_clk_lsi_calibrate();
#endif
#ifdef CONFIG_SOC_SERIES_CH58X
    hal_clk_lsi_calibrate(LEVEL_64);
#endif
}

uint32_t ble_flash_read(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    const struct device *eeprom = DEVICE_GET(eeprom);

    if (!device_is_ready(eeprom)) {
        LOG_ERR("eeprom is not ready\n");
    }

    return eeprom_read(eeprom, addr, pBuf, num);
}

uint32_t ble_flash_write(uint32_t addr, uint32_t num, uint32_t *pBuf)
{
    const struct device *eeprom = DEVICE_GET(eeprom);

    if (!device_is_ready(eeprom)) {
        LOG_ERR("eeprom is not ready\n");
    }

    return eeprom_write(eeprom, addr, pBuf, num);

    return 0;
}