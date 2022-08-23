/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <logging/log.h>
#include "hal.h"

tmosTaskID hal_taskid = INVALID_TASK_ID;

static void hal_process_tmosmsg(tmos_event_hdr_t *pmsg)
{
    switch (pmsg->event) {

        default:
            break;
    }
}

static tmosEvents hal_process_event(tmosTaskID task_id, tmosEvents events)
{
    if (events & SYS_EVENT_MSG) {
        uint8_t *msg = tmos_msg_receive(task_id);

        if (msg) {
            hal_process_tmosmsg((tmos_event_hdr_t *)msg);
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
        hal_clk_lsi_calibrate();
#endif
        tmos_start_task(hal_taskid, HAL_REG_INIT_EVENT, 
            MS1_TO_SYSTEM_TIME(CONFIG_BT_CALIBRATION_PERIOD));
        return events ^ HAL_REG_INIT_EVENT;
    }
#endif /* CONFIG_BT_CALIBRATION */

    return 0;
}

void hal_init(void)
{
    hal_taskid = TMOS_ProcessEventRegister(hal_process_event);

    __ASSERT_NO_MSG(hal_taskid == INVALID_TASK_ID);

    const struct device *rtc = DEVICE_GET(rtc);

    if (!device_is_ready(rtc)) {
        LOG_DBG("RTC device is not ready\n");

        return ;
    }

    TMOS_TimerInit(k_cycle_get_32);
}