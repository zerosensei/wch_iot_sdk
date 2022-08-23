/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "perip_process.h"

static tmosTaskID perip_taskid = TASK_NO_TASK;

tmosEvents perip_process_event(tmosTaskID taskID, tmosEvents event)
{
    if (event & SYS_EVENT_MSG) {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(Peripheral_TaskID)) != NULL)
        {
            Peripheral_ProcessTMOSMsg((tmos_event_hdr_t *)pMsg);
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return event ^ SYS_EVENT_MSG;
    }

    if (event & PERIP_POLL_NOTIFY_EVENT) {

        return event ^ PERIP_POLL_NOTIFY_EVENT;
    }
}

void perip_process_init(void)
{
    perip_taskid = TMOS_ProcessEventRegister(perip_process_event);
}