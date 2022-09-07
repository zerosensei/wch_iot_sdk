/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bluetooth/bluetooth.h>


static tmosTaskID ble_taskid = TASK_NO_TASK;
static bool is_device_started = false;



void ble_conn_cb_register(struct ble_conn_cb *cb)
{
    // cb->_next = conn_cb_list;
    // conn_cb_list = cb;
}


static gapRolesBroadcasterCBs_t *gap_broadcaster_cb = NULL;
static gapRoleObserverCB_t *gap_observer_cb = NULL;
static gapRolesCBs_t *gap_peripheral_cb = NULL;
static gapCentralRoleCB_t *gap_centerl_cb = NULL;

static gapBondCBs_t *gap_bond_cb = NULL;


int ble_gap_role_cb_register(gapRolesCBs_t *cb)
{
    if (is_device_started) {
        return -EACCES;
    }

    gap_peripheral_cb = cb;

    return 0;
}

int ble_gap_bond_cb_register(gapBondCBs_t *cb)
{
    if (is_device_started) {
        return -EACCES;
    }

    gap_bond_cb = cb;

    return 0;
}

static void ble_process_msg(tmos_event_hdr_t *msg)
{
    switch(msg->event) {
    case GAP_MSG_EVENT:
        // Peripheral_ProcessGAPMsg((gapRoleEvent_t *)msg);
        break;

    default:
        break;

    }
}


static tmosEvents ble_process_event(tmosTaskID taskID, tmosEvents event)
{
    if(event & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(ble_taskid)) != NULL)
        {
            ble_process_msg((tmos_event_hdr_t *)pMsg);
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (event ^ SYS_EVENT_MSG);
    }


    return 0;
}

int ble_process_init(uint8_t role)
{
    int ret = 0;

    ble_taskid = TMOS_ProcessEventRegister(ble_process_event);
    __ASSERT_NO_MSG(ble_taskid != TASK_NO_TASK);

    if (role & GAP_PROFILE_BROADCASTER) {
        ret |= GAPRole_BroadcasterInit();
        ret |= GAPRole_BroadcasterStartDevice(gap_broadcaster_cb);
        __ASSERT_NO_MSG(ret == 0);
    }

    if (role & GAP_PROFILE_OBSERVER) {
        ret |= GAPRole_ObserverInit();
        ret |= GAPRole_ObserverStartDevice(gap_observer_cb);
        __ASSERT_NO_MSG(ret == 0);
    }

    if (role & GAP_PROFILE_PERIPHERAL) {
        ret |= GAPRole_PeripheralInit();
        ret |= GAPRole_PeripheralStartDevice(ble_taskid, gap_bond_cb, gap_peripheral_cb);
        __ASSERT_NO_MSG(ret == 0);
    }

    if (role & GAP_PROFILE_CENTRAL) {
        ret |= GAPRole_CentralInit();
        ret |= GAPRole_CentralStartDevice(ble_taskid, gap_bond_cb, gap_centerl_cb);
        __ASSERT_NO_MSG(ret == 0);
    }


    if (ret == 0) {
        is_device_started = true;
    }

    return ret;
}
